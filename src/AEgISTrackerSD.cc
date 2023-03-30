//
// ********************************************************************
// * License and Disclaimer                                           *
// *                                                                  *
// * The  Geant4 software  is  copyright of the Copyright Holders  of *
// * the Geant4 Collaboration.  It is provided  under  the terms  and *
// * conditions of the Geant4 Software License,  included in the file *
// * LICENSE and available at  http://cern.ch/geant4/license .  These *
// * include a list of copyright holders.                             *
// *                                                                  *
// * Neither the authors of this software system, nor their employing *
// * institutes,nor the agencies providing financial support for this *
// * work  make  any representation or  warranty, express or implied, *
// * regarding  this  software system or assume any liability for its *
// * use.  Please see the license in the file  LICENSE  and URL above *
// * for the full disclaimer and the limitation of liability.         *
// *                                                                  *
// * This  code  implementation is the result of  the  scientific and *
// * technical work of the GEANT4 collaboration.                      *
// * By using,  copying,  modifying or  distributing the software (or *
// * any work based  on the software)  you  agree  to acknowledge its *
// * use  in  resulting  scientific  publications,  and indicate your *
// * acceptance of all terms of the Geant4 Software license.          *
// ********************************************************************
//
//
/// \file AEgISTrackerSD.cc
/// \brief Implementation of the AEgISTrackerSD class

#include "AEgISTrackerSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4AnalysisManager.hh"
#include "G4ios.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

AEgISTrackerSD::AEgISTrackerSD(const G4String& name,
				 const G4String& hitsCollectionName,
				 G4bool is_forward,
				 G4int ntuple_id,
				 G4bool kill_particle) 
  : G4VSensitiveDetector(name),
    fHitsCollection(NULL),
    fForward(is_forward),
    fNtupleId(ntuple_id),
    fKillParticle(kill_particle)
{
  collectionName.insert(hitsCollectionName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

AEgISTrackerSD::~AEgISTrackerSD() 
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void AEgISTrackerSD::Initialize(G4HCofThisEvent* hce)
{
  // Create hits collection

  fHitsCollection = new AEgISTrackerHitsCollection(SensitiveDetectorName, collectionName[0]);
  // Add this collection in hce

  G4int hcID 
    = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  hce->AddHitsCollection( hcID, fHitsCollection ); 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool AEgISTrackerSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
// check if it is moving backwards
  G4ThreeVector momentumDirection = aStep->GetTrack()->GetMomentumDirection();
  G4bool test = fForward ^ (momentumDirection[2] > 0);
  if(fForward ^ (momentumDirection[2] > 0) ) return false;
  // if fForward is true, we want to count particles with pZ > 0 -> true xor true = false
  // if fForward is false, we want to count particles with pZ < 0 -> false xor false = false
  // in other situations, we have one true and one false -> false xor true = true -> return
  
  // // is it comming from a sensitive detector?  
  if(aStep->GetPostStepPoint()->GetSensitiveDetector() != NULL) return false;
  // analyse particles only that exit the sensitive detector (just in case there are more than one hit in the detector)
  
  G4double particle_mass = aStep->GetPostStepPoint()->GetMass();
  G4double particle_charge = aStep->GetPostStepPoint()->GetCharge();
  G4ParticleDefinition* particle = aStep->GetTrack()->GetDefinition();
  G4String partName = particle->GetParticleName();

  // select antiprotons
  if (abs(particle_mass - 938.) > 1.) return false;
  if (particle_charge > 0) return false;
  if (partName != "anti_proton") return false;
// get position and momentum of the antiproton
   G4ThreeVector position = aStep->GetPostStepPoint()->GetPosition();

// how much energy is left
   G4double eResidual = aStep->GetPostStepPoint()->GetKineticEnergy();
   // what time did the particle arrive
   G4double globalTime = aStep->GetPreStepPoint()->GetGlobalTime();

//   G4cout << "++ Energy after aStep :" << eResidual << G4endl;
   auto man = G4AnalysisManager::Instance();
   //fill in position
   man->FillNtupleDColumn(fNtupleId,0,position[0]/CLHEP::mm);
   man->FillNtupleDColumn(fNtupleId,1,position[1]/CLHEP::mm);
   man->FillNtupleDColumn(fNtupleId,2,position[2]/CLHEP::mm);
   //fill in momentum
   man->FillNtupleDColumn(fNtupleId,3,momentumDirection[0]*eResidual/CLHEP::keV);
   man->FillNtupleDColumn(fNtupleId,4,momentumDirection[1]*eResidual/CLHEP::keV);
   man->FillNtupleDColumn(fNtupleId,5,momentumDirection[2]*eResidual/CLHEP::keV);
   //fill in energy
   man->FillNtupleDColumn(fNtupleId,6,eResidual/CLHEP::keV);
   //fill in time
   man->FillNtupleDColumn(fNtupleId,7,globalTime/CLHEP::ns);
   //add entry to the ntuple
   man->AddNtupleRow(fNtupleId);
   
   // if the flag is true, kill the particle
   if(fKillParticle){
     aStep->GetTrack()->SetTrackStatus(fStopAndKill);
   }
  
  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void AEgISTrackerSD::EndOfEvent(G4HCofThisEvent*)
{
  if ( verboseLevel>1 ) { 
     G4int nofHits = fHitsCollection->entries();
     G4cout
       << G4endl 
       << "-------->Hits Collection: in this event there are " << nofHits 
       << " hits in the tracker chambers: " << G4endl;
     for ( G4int i=0; i<nofHits; i++ ) (*fHitsCollection)[i]->Print();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

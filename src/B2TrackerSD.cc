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
/// \file B2TrackerSD.cc
/// \brief Implementation of the B2TrackerSD class

#include "B2TrackerSD.hh"
#include "G4HCofThisEvent.hh"
#include "G4Step.hh"
#include "G4ThreeVector.hh"
#include "G4SDManager.hh"
#include "G4AnalysisManager.hh"
#include "G4ios.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B2TrackerSD::B2TrackerSD(const G4String& name,
                         const G4String& hitsCollectionName) 
 : G4VSensitiveDetector(name),
   fHitsCollection(NULL)
{
  collectionName.insert(hitsCollectionName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B2TrackerSD::~B2TrackerSD() 
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2TrackerSD::Initialize(G4HCofThisEvent* hce)
{
  // Create hits collection

  fHitsCollection = new B2TrackerHitsCollection(SensitiveDetectorName, collectionName[0]); 

  // Add this collection in hce

  G4int hcID 
    = G4SDManager::GetSDMpointer()->GetCollectionID(collectionName[0]);
  hce->AddHitsCollection( hcID, fHitsCollection ); 
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool B2TrackerSD::ProcessHits(G4Step* aStep, G4TouchableHistory*)
{
  // // where is it comming from?
  G4VPhysicalVolume* pv_after = aStep->GetPostStepPoint()->GetPhysicalVolume();
  if(pv_after->GetName() == "Detector") return true; // particle already analysed
  
  G4double particle_mass = aStep->GetPostStepPoint()->GetMass();
  G4double particle_charge = aStep->GetPostStepPoint()->GetCharge();
  G4ParticleDefinition* particle = aStep->GetTrack()->GetDefinition();
  G4String partName = particle->GetParticleName();

  // select antiprotons
  if (abs(particle_mass - 938.) > 1.) return true;
  if (particle_charge > 0) return true;
  if (partName != "anti_proton") return true;
// get position and momentum of the antiproton
   G4ThreeVector position = aStep->GetPostStepPoint()->GetPosition();
   G4ThreeVector momentumDirection = aStep->GetPostStepPoint()->GetMomentumDirection();

// how much energy is left
   G4double eResidual = aStep->GetPostStepPoint()->GetKineticEnergy();

//   G4cout << "++ Energy after aStep :" << eResidual << G4endl;
   auto man = G4AnalysisManager::Instance();
   //fill in position
   man->FillNtupleDColumn(1,0,position[0]/CLHEP::mm);
   man->FillNtupleDColumn(1,1,position[1]/CLHEP::mm);
   man->FillNtupleDColumn(1,2,position[2]/CLHEP::mm);
   man->AddNtupleRow(1);

   //fill in momentum
   man->FillNtupleDColumn(2,0,momentumDirection[0]*eResidual/CLHEP::keV);
   man->FillNtupleDColumn(2,1,momentumDirection[1]*eResidual/CLHEP::keV);
   man->FillNtupleDColumn(2,2,momentumDirection[2]*eResidual/CLHEP::keV);
   man->FillNtupleDColumn(2,3,eResidual/CLHEP::keV);
   man->AddNtupleRow(2);
  
  return true;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2TrackerSD::EndOfEvent(G4HCofThisEvent*)
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

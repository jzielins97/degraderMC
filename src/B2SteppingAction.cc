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
/// copy of \file B1SteppingAction.cc
/// \brief Implementation of the B2SteppingAction class

#include "B2SteppingAction.hh"
#include "B2EventAction.hh"
#include "B2bDetectorConstruction.hh"
#include "B2MagneticField.hh"

#include "G4Tubs.hh"
#include "G4Step.hh"    //  from track/src
#include "G4SteppingVerbose.hh"
#include "G4SteppingManager.hh"
#include "G4ParticleChange.hh"
#include "G4VParticleChange.hh"
#include "G4StepStatus.hh"    // Include from 'tracking'
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "G4LogicalVolume.hh"
#include "G4Material.hh"
#include "G4UnitsTable.hh"
class G4VProcess;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B2SteppingAction::B2SteppingAction(B2EventAction* eventAction)
: G4UserSteppingAction(),
  fEventAction(eventAction),
  fScoringVolume(0),
  fStuckSteps(0)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B2SteppingAction::~B2SteppingAction() {}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2SteppingAction::UserSteppingAction(const G4Step* step)
{
  
  G4Track* track = step->GetTrack();
  G4ParticleDefinition* particleType = track->GetDefinition();
  // G4cout << "++++++++++++ Step output ++++++++++"<< G4endl;
  // 	 << "+ Particle:"<<particleType->GetParticleName()<<G4endl
  // 	 << "+ mass:"<<step->GetPostStepPoint()->GetMass()<<G4endl
  // 	 << "+ charge:"<<step->GetPostStepPoint()->GetCharge()<<G4endl
  //     << "+ position:"<< track->GetPosition()<<G4endl
  //     << "+ direction:"<< track->GetMomentumDirection()<<G4endl
  // 	 << "+ energy:"<<step->GetPostStepPoint()->GetKineticEnergy()<<G4endl
  //     << "+ Moving from "<<step->GetPreStepPoint()->GetPhysicalVolume()->GetName()
  //     <<" to "<<step->GetPostStepPoint()->GetPhysicalVolume()->GetName()<<G4endl
  //     << "Magnetic moment ="<<step->GetPostStepPoint()->GetMagneticMoment()<<G4endl
  // 	 << "deposited energy:"<<step->GetTotalEnergyDeposit()<<G4endl
  //     << "+++++++++++++++++++++++++++++++++++"<<G4endl<<G4endl;
  
  // check if it is antiproton
  if( particleType->GetParticleName() != "anti_proton"){
    // it isn't an antiproton, kill it
    track->SetTrackStatus(fStopAndKill);
    return;
  }
  
  // check if the momentum direction is not perpendicular to the BField
  G4ThreeVector direction = track->GetMomentumDirection();
  G4ThreeVector position = track->GetPosition();
  G4ThreeVector postDirection = step->GetPostStepPoint()->GetMomentumDirection();
  G4ThreeVector postPosition = step->GetPostStepPoint()->GetPosition();
  // G4cout << preDirection << " " << step->GetPreStepPoint()->GetPosition()<<G4endl;
  if(direction[2] < 1e-6 && abs(postPosition[2] - position[2]) < 0.1 * CLHEP::nm){
    fStuckSteps += 1;
    // Antiproton is moving perpendicular to the BField.
    // This creates circular motion that is not moving forward in the Z direction.
    // Antiproton will never hit the dump. Therefore, kill it now
    if( fStuckSteps > 50){
      // G4cerr << "Antiproton moving in circles (for "<< fStuckSteps << " steps)-> stop it" <<G4endl;
      track->SetTrackStatus(fStopAndKill);
      fEventAction->IsKilledEvent();
      return;
    }
  }else{
    fStuckSteps=0;
  }
  
  
  
  G4VPhysicalVolume* ph_volume = step->GetPostStepPoint()->GetPhysicalVolume();
  // Check if antiproton is moving in oposite direction
  if(direction[2] < 0 && ( ph_volume->GetName() == "MagneticField" || ph_volume->GetName() == "World" ) ){
    // G4cerr << "Antiproton moving backwards at ("<<position[0] <<","<<position[1]<<","<<position[2]<<") -> stop it" <<G4endl;
    track->SetTrackStatus(fStopAndKill);
    fEventAction->IsKilledEvent();
    return;
  }

  const G4VProcess* G4ProcessAfter = step->GetPostStepPoint()->GetProcessDefinedStep();
  if ( G4ProcessAfter->GetProcessName().find("CaptureAtRest") != std::string::npos ){
    // G4cout <<  G4ProcessAfter->GetProcessName() << " process taken as annihilation" << G4endl;
    fEventAction->IsAnnihilationEvent();    
  }
    
  if(ph_volume->GetName() != "Dump") return;
  // particle hit the Dump volume, kill its track
  track->SetTrackStatus(fStopAndKill);
  return;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


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
  fScoringVolume(0)
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B2SteppingAction::~B2SteppingAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2SteppingAction::UserSteppingAction(const G4Step* step)
{
  G4ParticleDefinition* particleType = step->GetTrack()->GetDefinition();
  // check if it is antiproton
  if( particleType->GetParticleName() != "anti_proton"){
    // it isn't an antiproton, kill it
    step->GetTrack()->SetTrackStatus(fStopAndKill);
    return;
  }
  
  G4VPhysicalVolume* ph_volume = step->GetPostStepPoint()->GetPhysicalVolume();
  if(ph_volume->GetName() != "Dump") return;
  // particle hit the Dump volume, kill its track
  step->GetTrack()->SetTrackStatus(fStopAndKill);
  return;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


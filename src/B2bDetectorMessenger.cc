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
/// \file B2bDetectorMessenger.cc
/// \brief Implementation of the B2bDetectorMessenger class

#include "B2bDetectorMessenger.hh"
#include "B2bDetectorConstruction.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithADoubleAndUnit.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B2bDetectorMessenger::B2bDetectorMessenger(B2bDetectorConstruction* Det)
 : G4UImessenger(),
   fDetectorConstruction(Det)
{
  fB2Directory = new G4UIdirectory("/AEgIS/");
  fB2Directory->SetGuidance("UI commands specific to this project.");

  fDetDirectory = new G4UIdirectory("/AEgIS/degrader");
  fDetDirectory->SetGuidance("Degraders construction control");

  fStepMaxCmd = new G4UIcmdWithADoubleAndUnit("/AEgIS/degrader/stepMax",this);
  fStepMaxCmd->SetGuidance("Define a step max");
  fStepMaxCmd->SetParameterName("stepMax",false);
  fStepMaxCmd->SetUnitCategory("Length");
  fStepMaxCmd->AvailableForStates(G4State_Idle);

  fFirstDegraderThicknessCmd = new G4UIcmdWithADoubleAndUnit("/AEgIS/degrader/setFirstThickness",this);
  fFirstDegraderThicknessCmd->SetGuidance("Define thickness of the first foil");
  fFirstDegraderThicknessCmd->SetParameterName("firstDegraderThickness",false);
  fFirstDegraderThicknessCmd->SetUnitCategory("Length");
  fFirstDegraderThicknessCmd->SetDefaultUnit("nm");
  fFirstDegraderThicknessCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSecondDegraderThicknessCmd = new G4UIcmdWithADoubleAndUnit("/AEgIS/degrader/setSecondThickness",this);
  fSecondDegraderThicknessCmd->SetGuidance("Define thickness of the second foil");
  fSecondDegraderThicknessCmd->SetParameterName("secondDegraderThickness",false);
  fSecondDegraderThicknessCmd->SetUnitCategory("Length");
  fSecondDegraderThicknessCmd->SetDefaultUnit("nm");
  fSecondDegraderThicknessCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fFirstMetalizationThicknessCmd = new G4UIcmdWithADoubleAndUnit("/AEgIS/degrader/metalization/setFirstThickness",this);
  fFirstMetalizationThicknessCmd->SetGuidance("Define thickness of the metalization for the first foil (up beam)");
  fFirstMetalizationThicknessCmd->SetParameterName("firstMetalizationThickness",false);
  fFirstMetalizationThicknessCmd->SetUnitCategory("Length");
  fFirstMetalizationThicknessCmd->SetDefaultUnit("nm");
  fFirstMetalizationThicknessCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSecondMetalizationThicknessCmd = new G4UIcmdWithADoubleAndUnit("/AEgIS/degrader/metalization/setSecondThickness",this);
  fSecondMetalizationThicknessCmd->SetGuidance("Define thickness of the metalization for the second foil (down beam)");
  fSecondMetalizationThicknessCmd->SetParameterName("secondMetalizationThickness",false);
  fSecondMetalizationThicknessCmd->SetUnitCategory("Length");
  fSecondMetalizationThicknessCmd->SetDefaultUnit("nm");
  fSecondMetalizationThicknessCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fFirstDegraderMaterialCmd = new G4UIcmdWithAString("/AEgIS/degrader/setFirstMaterial",this);
  fFirstDegraderMaterialCmd->SetGuidance("Select Material of the First Degrader.");
  fFirstDegraderMaterialCmd->SetParameterName("firstDegraderMaterial",false);
  fFirstDegraderMaterialCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSecondDegraderMaterialCmd = new G4UIcmdWithAString("/AEgIS/degrader/setSecondMaterial",this);
  fSecondDegraderMaterialCmd->SetGuidance("Select Material of the Second Degrader.");
  fSecondDegraderMaterialCmd->SetParameterName("secondDegraderMaterial",false);
  fSecondDegraderMaterialCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fFirstMetalizationMaterialCmd = new G4UIcmdWithAString("/AEgIS/degrader/metalization/setFirstMaterial",this);
  fFirstMetalizationMaterialCmd->SetGuidance("Select Material of the First Metalization.");
  fFirstMetalizationMaterialCmd->SetParameterName("firstMetalizationMaterial",false);
  fFirstMetalizationMaterialCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

  fSecondMetalizationMaterialCmd = new G4UIcmdWithAString("/AEgIS/degrader/metalization/setSecondMaterial",this);
  fSecondMetalizationMaterialCmd->SetGuidance("Select Material of the Second Metalization.");
  fSecondMetalizationMaterialCmd->SetParameterName("secondMetalizationMaterial",false);
  fSecondMetalizationMaterialCmd->AvailableForStates(G4State_PreInit,G4State_Idle);
  
  fMagneticFieldOnCmd = new G4UIcmdWithAString("/AEgIS/BField",this);
  fSecondDegraderMaterialCmd->SetGuidance("Switch on/off magnetic field");
  fSecondDegraderMaterialCmd->SetParameterName("magneticFieldOn",false);
  fSecondDegraderMaterialCmd->AvailableForStates(G4State_PreInit,G4State_Idle);

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B2bDetectorMessenger::~B2bDetectorMessenger()
{
  delete fFirstDegraderThicknessCmd;
  delete fSecondDegraderThicknessCmd;
  delete fStepMaxCmd;
  delete fB2Directory;
  delete fDetDirectory;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2bDetectorMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{
  if( command == fFirstDegraderThicknessCmd )
   { fDetectorConstruction->SetFirstDegraderThickness(fFirstDegraderThicknessCmd->GetNewDoubleValue(newValue));}

  if( command == fSecondDegraderThicknessCmd )
   { fDetectorConstruction->SetSecondDegraderThickness(fSecondDegraderThicknessCmd->GetNewDoubleValue(newValue));}

  if( command == fFirstMetalizationThicknessCmd )
   { fDetectorConstruction->SetFirstMetalizationThickness(fFirstDegraderThicknessCmd->GetNewDoubleValue(newValue));}

  if( command == fSecondMetalizationThicknessCmd )
   { fDetectorConstruction->SetSecondMetalizationThickness(fSecondDegraderThicknessCmd->GetNewDoubleValue(newValue));}

  if( command == fFirstDegraderMaterialCmd )
    { fDetectorConstruction->SetFirstDegraderMaterial(newValue); }

  if( command == fSecondDegraderMaterialCmd )
   { fDetectorConstruction->SetSecondDegraderMaterial(newValue);}

  if( command == fFirstMetalizationMaterialCmd )
   { fDetectorConstruction->SetFirstMetalizationMaterial(newValue);}

  if( command == fSecondMetalizationMaterialCmd )
   { fDetectorConstruction->SetSecondMetalizationMaterial(newValue);}

  if( command == fStepMaxCmd ) {
    fDetectorConstruction->SetMaxStep(fStepMaxCmd->GetNewDoubleValue(newValue));
  }

  if( command == fMagneticFieldOnCmd ) {
    if(newValue == "on" || newValue == "On" || newValue =="ON") fDetectorConstruction->SetMagneticField(true);
    else if(newValue == "off" || newValue == "Off" || newValue =="OFF") fDetectorConstruction->SetMagneticField(false);    
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

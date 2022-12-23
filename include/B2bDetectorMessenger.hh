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
/// \file B2bDetectorMessenger.hh
/// \brief Definition of the B2bDetectorMessenger class

#ifndef B2bDetectorMessenger_h
#define B2bDetectorMessenger_h 1

#include "globals.hh"
#include "G4UImessenger.hh"
#include "G4FieldManager.hh"

class B2MagneticField;

class B2bDetectorConstruction;
class G4UIdirectory;
class G4UIcmdWithAString;
class G4UIcmdWithADoubleAndUnit;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

/// Messenger class that defines commands for B2bDetectorConstruction.
///
/// It implements commands:
/// - /AEgIS/degrader/setFirstDegraderThickness value unit
/// - /AEgIS/degrader/setSecondDegraderThickness value unit 
/// - /AEgIS/degrader/setFirstDegraderMaterial name
/// - /AEgIS/degrader/setSecondDegraderMaterial name
/// - /AEgIS/degrader/setFirstMetalizationMaterial name 
/// - /AEgIS/degrader/setSecondMetalizationMaterial name
/// - /AEgIS/degrader/stepMax value unit

class B2bDetectorMessenger: public G4UImessenger
{
  public:
    B2bDetectorMessenger(B2bDetectorConstruction* );
    virtual ~B2bDetectorMessenger();
    
    virtual void SetNewValue(G4UIcommand*, G4String);
    
  private:
    B2bDetectorConstruction*  fDetectorConstruction;

    static G4ThreadLocal B2MagneticField* fMagneticField;
    static G4ThreadLocal G4FieldManager* fFieldMgr;

    G4UIdirectory*           fB2Directory;
    G4UIdirectory*           fDetDirectory;

    G4UIcmdWithADoubleAndUnit* fStepMaxCmd;
    G4UIcmdWithADoubleAndUnit* fFirstDegraderThicknessCmd;
    G4UIcmdWithADoubleAndUnit* fSecondDegraderThicknessCmd;
    G4UIcmdWithADoubleAndUnit* fFirstMetalizationThicknessCmd;
    G4UIcmdWithADoubleAndUnit* fSecondMetalizationThicknessCmd;
    G4UIcmdWithAString* fFirstDegraderMaterialCmd;
    G4UIcmdWithAString* fSecondDegraderMaterialCmd;
    G4UIcmdWithAString* fFirstMetalizationMaterialCmd;
    G4UIcmdWithAString* fSecondMetalizationMaterialCmd;
    G4UIcmdWithAString* fMagneticFieldOnCmd;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

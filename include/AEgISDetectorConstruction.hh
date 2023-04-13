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
/// \file AEgISDetectorConstruction.hh
/// \brief Definition of the AEgISDetectorConstruction class

#ifndef AEgISDetectorConstruction_h
#define AEgISDetectorConstruction_h 1

#include "globals.hh"
#include "G4Cache.hh"
#include "G4VUserDetectorConstruction.hh"
#include "tls.hh"
#include "G4FieldManager.hh"
#include "G4ThreeVector.hh"

class AEgISMagneticField;
class AEgISGlobalField;
class G4VPhysicalVolume;
class G4LogicalVolume;
class G4Tubs;
class G4VSolid;
class G4Material;
class G4UserLimits;
class G4GlobalMagFieldMessenger;

class AEgISDetectorMessenger;

/// Detector construction class to define materials, geometry
/// and global uniform magnetic field.

class AEgISDetectorConstruction : public G4VUserDetectorConstruction
{
public:
  AEgISDetectorConstruction();
  virtual ~AEgISDetectorConstruction();

public:
  virtual G4VPhysicalVolume* Construct();
  virtual void ConstructSDandField();

  // Get methods
  G4bool GetB5TFieldFlag(){return f5TFieldOn;}
  G4LogicalVolume* GetMgnt5T(){return fMgnt5TLV;}
  G4double GetMgnt5Tlength(){return fMgnt5Tlength;}
  G4double GetMgnt5Tradius(){return fMgnt5Tradius;}
  G4ThreeVector GetMgnt5TCenter(){return fMgnt5TCenter;}
  
  // Set methods
  void SetFirstDegraderThickness(G4double);
  void SetSecondDegraderThickness(G4double);
  void SetFirstMetalizationThickness(G4double);
  void SetSecondMetalizationThickness(G4double);    
  void SetFirstDegraderMaterial(G4String);
  void SetSecondDegraderMaterial(G4String);
  void SetFirstMetalizationMaterial(G4String);
  void SetSecondMetalizationMaterial(G4String);
  void SetMaxStep (G4double );
  void SetCheckOverlaps(G4bool );
  void SetMagneticField(G4bool );

private:
  // methods
  void AddDetector(G4ThreeVector position, G4LogicalVolume* motherLV, G4int detectorID);
  void DefineMaterials();
  G4VPhysicalVolume* DefineVolumes();
  G4Material* SetMaterial(G4LogicalVolume*, G4String);
    
  // data members
  G4Cache<AEgISGlobalField*> fFieldSetUp;
  
  G4Material*        fVacuumMaterial;  // pointer to vacuum material
  G4Material*        fFirstDegraderMaterial;
  G4Material*        fSecondDegraderMaterial;
  G4Material*        fFirstMetalizationMaterial; // pointer to the chamber material
  G4Material*        fSecondMetalizationMaterial; // pointer to the chamber material
  G4Material*        fTrapMaterial;// pointer to material for traps (Al)

  
  // solids
  G4Tubs* fWorldS;
  G4Tubs* fFirstDegraderS;
  G4Tubs* fFirstMetalizationS;
  G4Tubs* fSecondDegraderS;
  G4Tubs* fSecondMetalizationS;
  G4Tubs* fDumpTubeS;
  G4Tubs* fDumpCapS;
  G4VSolid* fDumpS;
  G4Tubs* fDetectorS[7];
  G4Tubs* fMgnt5TS;
  G4Tubs* fTrap5TS;
  G4Tubs* fHVElectrodeS;
  G4Tubs* fTrap1TS;

  
  // logical volumes
  G4LogicalVolume*   fWorldLV;         // pointer to the logical World
  G4LogicalVolume*   fFirstDegraderLV;  // pointer to the logical First Degrader
  G4LogicalVolume*   fFirstMetalizationLV;  // pointer to the logical metalization of the First Degrader
  G4LogicalVolume*   fSecondDegraderLV;   // pointer to the logical Second Degrader (in magnetic field)
  G4LogicalVolume*   fSecondMetalizationLV;  // pointer to the logical metalization of the Second Degrader
  G4LogicalVolume*   fDumpLV;   // pointer to the logical (Au) "Detector" layer
  G4LogicalVolume*   fDetectorLV[7];   // pointer to the logical detector
  G4LogicalVolume*   fMgnt5TLV; // pointer to the logical magnetic volume
  G4LogicalVolume*   fTrap5TLV; // pointer to the logical volume for 5T trap
  G4LogicalVolume*   fHVElectrodeLV;
  G4LogicalVolume*   fTrap1TLV; // pointer to the logical volume for 1T trap

  
  // Physical Volume
  G4VPhysicalVolume* fWorldPV;
  G4VPhysicalVolume* fFirstDegraderPV;
  G4VPhysicalVolume* fFirstMetalizationPV;
  G4VPhysicalVolume* fSecondDegraderPV;
  G4VPhysicalVolume* fSecondMetalizationPV;
  G4VPhysicalVolume* fDumpPV;
  G4VPhysicalVolume* fDetectorPV[7];
  G4VPhysicalVolume* fMgnt5TPV;
  G4VPhysicalVolume* fTrap5TPV;
  G4VPhysicalVolume* fHVElectrodePV;
  G4VPhysicalVolume* fTrap1TPV;

  G4double           fFirstDegraderThickness;
  G4double           fSecondDegraderThickness;
  G4double           fFirstMetalizationThickness;
  G4double           fSecondMetalizationThickness;

  
  G4bool             f5TFieldOn = true;
  // G4double           fMagneticFieldStart;
  G4ThreeVector fMgnt5TCenter;
  G4double fMgnt5Tlength;
  G4double fMgnt5Tradius;
  

  G4UserLimits*      fStepLimit;       // pointer to user step limits

  AEgISDetectorMessenger*  fMessenger;   // detector messenger
    
  // static...

  static G4ThreadLocal AEgISMagneticField* fMagneticField;
  static G4ThreadLocal G4FieldManager* fFieldMgr;
    
  G4bool  fCheckOverlaps; // option to activate checking of volumes overlaps 
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

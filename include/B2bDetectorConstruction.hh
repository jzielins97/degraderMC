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
/// \file B2bDetectorConstruction.hh
/// \brief Definition of the B2bDetectorConstruction class

#ifndef B2bDetectorConstruction_h
#define B2bDetectorConstruction_h 1

#include "globals.hh"
#include "G4VUserDetectorConstruction.hh"
#include "tls.hh"
#include "G4FieldManager.hh"

class B2MagneticField;
class G4VPhysicalVolume;
class G4LogicalVolume;
class G4Tubs;
class G4VSolid;
class G4Material;
class G4UserLimits;
class G4GlobalMagFieldMessenger;

class B2bDetectorMessenger;

/// Detector construction class to define materials, geometry
/// and global uniform magnetic field.

class B2bDetectorConstruction : public G4VUserDetectorConstruction
{
  public:
    B2bDetectorConstruction();
    virtual ~B2bDetectorConstruction();

  public:
    virtual G4VPhysicalVolume* Construct();
    virtual void ConstructSDandField();

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
    void DefineMaterials();
    G4VPhysicalVolume* DefineVolumes();
    G4Material* SetMaterial(G4LogicalVolume*, G4String);
    
    // data members
    G4LogicalVolume*   fWorldLV;         // pointer to the logical World
    G4LogicalVolume*   fFirstDegraderLV;  // pointer to the logical First Degrader
    G4LogicalVolume*   fFirstMetalizationLV;  // pointer to the logical metalization of the First Degrader
    G4LogicalVolume*   fSecondDegraderLV;   // pointer to the logical Second Degrader (in magnetic field)
    G4LogicalVolume*   fSecondMetalizationLV;  // pointer to the logical metalization of the Second Degrader
    G4LogicalVolume*   fDumpLV;   // pointer to the logical (Au) "Detector" layer
    G4LogicalVolume*   fDetectorLV;   // pointer to the logical detector
    G4LogicalVolume*   fMagneticLV; // pointer to the logical magnetic volume (=beamtube)

    G4Material*        fWorldMaterial;   // pointer to the world material
    G4Material*        fVacuumMaterial;  // pointer to vacuum material
    G4Material*        fFirstDegraderMaterial;
    G4Material*        fSecondDegraderMaterial;
    G4Material*        fFirstMetalizationMaterial; // pointer to the chamber material
    G4Material*        fSecondMetalizationMaterial; // pointer to the chamber material
    G4Material*        fDumpMaterial;// pointer to detector material (10 nm Au)

    // solids
    G4Tubs* fWorldS;
    G4Tubs* fFirstDegraderS;
    G4Tubs* fFirstMetalizationS;
    G4Tubs* fSecondDegraderS;
    G4Tubs* fSecondMetalizationS;
    G4Tubs* fDumpTubeS;
    G4Tubs* fDumpCapS;
    G4VSolid* fDumpS;
    G4Tubs* fDetectorS;
    G4Tubs* fMagneticS;
   

    // Physical Volume
    G4VPhysicalVolume* fWorldPV;
    G4VPhysicalVolume* fFirstDegraderPV;
    G4VPhysicalVolume* fFirstMetalizationPV;
    G4VPhysicalVolume* fSecondDegraderPV;
    G4VPhysicalVolume* fSecondMetalizationPV;
    G4VPhysicalVolume* fDumpPV;
    G4VPhysicalVolume* fDetectorPV;
    G4VPhysicalVolume* fMagneticPV;

    G4bool             fBFieldOn = true;

    G4double           fFirstDegraderThickness;
    G4double           fSecondDegraderThickness;
    G4double           fFirstMetalizationThickness;
    G4double           fSecondMetalizationThickness;

    G4double           fMagneticFieldStart;

    G4UserLimits*      fStepLimit;       // pointer to user step limits

    B2bDetectorMessenger*  fMessenger;   // detector messenger
    
// static...

    static G4ThreadLocal B2MagneticField* fMagneticField;
    static G4ThreadLocal G4FieldManager* fFieldMgr;
//*    static G4ThreadLocal G4GlobalMagFieldMessenger*  fMagFieldMessenger; 
                                         // magnetic field messenger
    
    G4bool  fCheckOverlaps; // option to activate checking of volumes overlaps 
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

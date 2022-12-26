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
/// \file B2bDetectorConstruction.cc
/// \brief Implementation of the B2bDetectorConstruction class
 
#include "B2bDetectorConstruction.hh"
#include "B2MagneticField.hh"
#include "B2bDetectorMessenger.hh"
#include "B2bChamberParameterisation.hh"
#include "B2TrackerSD.hh"

// BBBBBBBBBBBBBBBBBBb
#include "G4FieldManager.hh"
#include "G4TransportationManager.hh"
#include "G4Mag_UsualEqRhs.hh"
// BBBBBBBBBBBBBBBBBBb

#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4SDManager.hh"

#include "G4VSolid.hh"
#include "G4UnionSolid.hh"
#include "G4Tubs.hh"
#include "G4LogicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4PVParameterised.hh"
#include "G4GlobalMagFieldMessenger.hh"
#include "G4AutoDelete.hh"

#include "G4GeometryTolerance.hh"
#include "G4GeometryManager.hh"

#include "G4UserLimits.hh"

// BBBBBBBBBBBBBBBBBBBBBB
#include "G4SDManager.hh"
#include "G4VSensitiveDetector.hh"
#include "G4RunManager.hh"
#include "G4GenericMessenger.hh"
// BBBBBBBBBBBBBBBBBBBBBB

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// BBBBBBBBBBBBBBBBBBBBBB
//*G4ThreadLocal G4GlobalMagFieldMessenger* B2bDetectorConstruction::fMagFieldMessenger = 0;
G4ThreadLocal B2MagneticField* B2bDetectorConstruction::fMagneticField = 0;
G4ThreadLocal G4FieldManager* B2bDetectorConstruction::fFieldMgr = 0;
// BBBBBBBBBBBBBBBBBBBBBB


B2bDetectorConstruction::B2bDetectorConstruction():
  G4VUserDetectorConstruction(),
  fWorldLV(NULL),
  fFirstDegraderLV(NULL),fFirstMetalizationLV(NULL),
  fSecondDegraderLV(NULL), fSecondMetalizationLV(NULL),
  fDumpLV(NULL),fDetectorLV(NULL),fMagneticLV(NULL),
  fFirstDegraderMaterial(NULL),fFirstMetalizationMaterial(NULL),
  fSecondDegraderMaterial(NULL),fSecondMetalizationMaterial(NULL),
  fCheckOverlaps(true)
{
  fMessenger = new B2bDetectorMessenger(this);
  fFirstDegraderThickness = 100 * nm;
  fSecondDegraderThickness = 100 * nm;

  fFirstMetalizationThickness = 10 * nm;
  fSecondMetalizationThickness = 10 * nm;

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
B2bDetectorConstruction::~B2bDetectorConstruction()
{
  delete fStepLimit;
  delete fMessenger;

  // DefineMaterials();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
G4VPhysicalVolume* B2bDetectorConstruction::Construct()
{
  DefineMaterials();
  return DefineVolumes();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2bDetectorConstruction::DefineMaterials()
{
  // Material definition 

  G4NistManager* nistManager = G4NistManager::Instance();
  
  // Vacum (for the beam tube) defined using NIST Manager
  fVacuumMaterial = nistManager->FindOrBuildMaterial("G4_Galactic");
  
  // Air defined using NIST Manager
  fWorldMaterial  = nistManager->FindOrBuildMaterial("G4_AIR");
  
  //---------------------  Target and Degrader material -------------------
  
  if(!fFirstDegraderMaterial) fFirstDegraderMaterial  = nistManager->FindOrBuildMaterial("G4_AIR");
  if(!fSecondDegraderMaterial) fSecondDegraderMaterial = nistManager->FindOrBuildMaterial("G4_MYLAR");
  
  // Vapor deposited material
  if(!fFirstMetalizationMaterial) fFirstMetalizationMaterial = nistManager->FindOrBuildMaterial("G4_Al");
  if(!fSecondMetalizationMaterial) fSecondMetalizationMaterial = nistManager->FindOrBuildMaterial("G4_Al");

  fDumpMaterial = nistManager->FindOrBuildMaterial("G4_Galactic");

  // Print materials
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// define B-field volumes as part of general definition of volumes in DefineVolumes()

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* B2bDetectorConstruction::DefineVolumes()
{
  G4cout<<"++++++ definition of the geometry ++++++"<<G4endl;
  // Sizes of the principal geometrical components (solids)
// ==========================================================
  // beam tube for antiproton transport
  G4double beamtubeRadius = 10 * cm;
  G4double beamtubeLength = 216*cm;
  
  // radius for the foils
  G4double foilRadius = 10 * cm;

  // antiproton dump (Au)
  G4double dumpRadius = 1.2*beamtubeRadius;

  // distance between foils
  G4double foilGap = 10*cm;

  // steps
  G4double maxFoilStep = 50*nm;
  G4double maxFieldStep = beamtubeLength/5;
  G4double maxMetalizationStep = 5 * nm;

// ==========================================================

  // G4double worldLength = foilGap/2 + foilMetalizationLength + fFirstDegraderThickness + foilGap + fSecondDegraderThickness + foilMetalizationLength + foilGap + dumpLength;
  G4double worldLength = 1.1 * (foilGap/2 + fFirstMetalizationThickness + fFirstDegraderThickness + 2*foilGap + beamtubeLength  + foilGap/2);
  G4double dumpLength = 0.01*worldLength;
  
  // Definitions of Solids, Logical Volumes, Physical Volumes

  // =================== World =========================

  // G4GeometryManager::GetInstance()->SetWorldMaximumExtent(worldLength);

  G4cout << "Computed tolerance = "
         << G4GeometryTolerance::GetInstance()->GetSurfaceTolerance()/mm
         << " mm" << G4endl;

  fWorldS = new G4Tubs("world",                                    //its name
		       0,
		       1.01*dumpRadius,
		       worldLength/2,                              //its size
		       0.*deg,
		       360.*deg);
  
  G4cout << "World extends from " << -worldLength/2 << " to " << worldLength/2 << G4endl;

//  G4LogicalVolume* worldLV
  fWorldLV = new G4LogicalVolume(
				 fWorldS,              //its solid
				 fVacuumMaterial,      //its material = vacuum
				 "WorldLV");            //its name
  
  //  Must place the World Physical volume unrotated at (0,0,0).
  // 
  fWorldPV = new G4PVPlacement(
			       0,               // no rotation
			       G4ThreeVector(), // at (0,0,0)
			       //                 worldLV,         // its logical volume
			       fWorldLV,         // its logical volume
			       "World",         // its name
			       0,               // its mother  volume
			       false,           // no boolean operations
			       0,               // copy number
			       fCheckOverlaps); // checking overlaps
  
  // G4ThreeVector positionFirstDegrader = G4ThreeVector(0,0,-worldLength/2 + dumpLength + foilGap/4 + fFirstMetalizationThickness + fFirstDegraderThickness/2);
  G4ThreeVector positionFirstDegrader = G4ThreeVector(0,0,-worldLength/2/1.1 + foilGap/2 + fFirstMetalizationThickness + fFirstDegraderThickness/2);
  if(fFirstDegraderThickness > 0){ // only place thin degrader if its thickness is more than 0
    // this degrader has a metalization layer:
    // *********************************************************
    // ===== First degrader metalization layer (downstream) ====
    // *********************************************************
  
    G4ThreeVector positionFirstMetalization = positionFirstDegrader - G4ThreeVector(0,0,fFirstDegraderThickness/2 + fFirstMetalizationThickness/2);
    G4cout << "Metalization of the first foils is placed at " << positionFirstMetalization << G4endl;

    fFirstMetalizationS = new G4Tubs("firstMetalization",
				     0.,
				     foilRadius,
				     fFirstMetalizationThickness/2,
				     0.*deg,
				     360.*deg);
  
    fFirstMetalizationLV = new G4LogicalVolume(fFirstMetalizationS,
					       fFirstMetalizationMaterial,
					       "FirstMetalizationLV",
					       0,
					       0,
					       0);
  
    fFirstMetalizationPV = new G4PVPlacement(0,                // no rotation
					     positionFirstMetalization, // at (x,y,z)
					     fFirstMetalizationLV,   // its logical volume
					     "FirstMetalization",       // its name
					     fWorldLV,   // its mother volume
					     false,            // no boolean operations
					     0,                // copy number
					     fCheckOverlaps);  // checking overlaps 

    // --------------------------------------------------------
    // tiny steps in the trace lines (made of Au) ...
    // ---------------------------------------------------------
    fFirstMetalizationLV->SetUserLimits(new G4UserLimits(maxMetalizationStep));

    G4cout << "    Metalization of the first foil is " << fFirstMetalizationThickness/nm << " nm of " << fFirstMetalizationMaterial->GetName() << G4endl;

    // *********************************************************
    // ============== First degrader foil    ===================
    // *********************************************************


    
    G4cout << "First degrader foil is placed at " << positionFirstDegrader << G4endl;

    fFirstDegraderS = new G4Tubs("firstDegrader",
				 0.,
				 foilRadius,
				 fFirstDegraderThickness/2,
				 0.*deg,
				 360.*deg);
  
    fFirstDegraderLV = new G4LogicalVolume(fFirstDegraderS,
					   fFirstDegraderMaterial,
					   "FirstDegraderLV",
					   0,
					   0,
					   0);
  
    fFirstDegraderPV = new G4PVPlacement(0,              // no rotation
					 positionFirstDegrader,// at (x,y,z)
					 fFirstDegraderLV,   // its logical volume
					 "FirstDegrader",       // its name
					 fWorldLV, // its mother volume
					 false,          // no boolean operations
					 0,              // copy number
					 fCheckOverlaps); // checking overlaps 

    // --------------------------------------------------
    // tiny steps in the degrader foil ...
    // Sets a max step length in the "target" region, with G4StepLimiter, of 50 nm
    // --------------------------------------------------    
    fStepLimit = new G4UserLimits(maxFoilStep);
    fFirstDegraderLV->SetUserLimits(fStepLimit);

    G4cout << "    First degrader foil is " << fFirstDegraderThickness/nm << " nm of " << fFirstDegraderMaterial->GetName() << G4endl;

  }else{ // only place thin degrader if its thickness is more than 0
    fFirstDegraderLV = NULL;
    fFirstMetalizationLV = NULL;
  }
  
  // *********************************************************
  // =================== Beam tube =========================
  // *********************************************************

  
   G4ThreeVector positionBeamTube = positionFirstDegrader + G4ThreeVector(0,0,fFirstDegraderThickness/2 + 2*foilGap + beamtubeLength/2);
   // G4ThreeVector positionBeamTube = G4ThreeVector(0,0,-beamtubeLength);
   G4cout << "Magnetic tube is placed at " << positionBeamTube << G4endl;
  
   // BBBBBBBBBBBBBBBB overlapping magnetic field region BBBBBBBBBBBBBB
  
   // Tube with Local Magnetic field
  
   fMagneticS = new G4Tubs("magneticTubs",
			   0.,
			   beamtubeRadius,
			   beamtubeLength/2,
			   0.,
			   360.*deg);
  
   fMagneticLV = new G4LogicalVolume(fMagneticS,
				     fVacuumMaterial,
				     "MagneticLV");
   
   // placement of Tube

   fMagneticPV = new G4PVPlacement(0,              // no rotation
				   positionBeamTube, // at (x,y,z)
				   fMagneticLV,   // its logical volume
				   "MagneticField",       // its name
				   //                    worldLV,        // its mother volume
				   fWorldLV,        // its mother volume
				   false,          // no boolean operations
				   0,              // copy number
				   fCheckOverlaps); // checking overlaps

   // set step limit in tube with magnetic field
   fMagneticLV->SetUserLimits(new G4UserLimits(maxFieldStep));

  // BBBBBBBBBBBBBBBB

  // *********************************************************
  // Second Degrader foil (in magnetic field)
  // *********************************************************
  G4ThreeVector positionSecondDegrader = G4ThreeVector(0,0,(111.4*cm - beamtubeLength/2));
  // G4ThreeVector positionSecondDegrader = positionFirstDegrader + G4ThreeVector(0,0,fFirstDegraderThickness/2 + foilGap + fSecondDegraderThickness/2);
  G4cout << "Second degrader foil is placed at " << positionBeamTube + positionSecondDegrader << G4endl;
  
  fSecondDegraderS = new G4Tubs("secondDegrader",
				0.,
				foilRadius,
				fSecondDegraderThickness/2,
				0.*deg,
				360.*deg);
  
 fSecondDegraderLV = new G4LogicalVolume(fSecondDegraderS,
					 fSecondDegraderMaterial,
					 "SecondDegraderLV",
					 0,
					 0,
					 0);
 
 fSecondDegraderPV = new G4PVPlacement(0,              // no rotation
				       positionSecondDegrader, // at (x,y,z)
				       fSecondDegraderLV,   // its logical volume
				       "SecondDegrader",       // its name
				       fMagneticLV, // its mother volume
				       false,          // no boolean operations
				       0,              // copy number
				       fCheckOverlaps); // checking overlaps 

// --------------------------------------------------
// tiny steps in the degrader foil ...
// Sets a max step length in the "target" region, with G4StepLimiter, of 50 nm
// --------------------------------------------------

 fStepLimit = new G4UserLimits(maxFoilStep);
 fSecondDegraderLV->SetUserLimits(fStepLimit);

 G4cout << "    Second degrader foil is " << fSecondDegraderThickness/nm << " nm of "
	<< fSecondDegraderMaterial->GetName() << G4endl;

// *********************************************************
// ======= second degrader metalization layer    ===========
// *********************************************************
  
  G4ThreeVector positionSecondMetalization = positionSecondDegrader + G4ThreeVector(0,0, fSecondDegraderThickness/2 + fSecondMetalizationThickness/2);
  G4cout << "Metalization of the second foil is placed at " << positionBeamTube+positionSecondMetalization << G4endl;


  fSecondMetalizationS = new G4Tubs("secondMetalization",
				    0.,
				    foilRadius,
				    fSecondMetalizationThickness/2,
				    0.*deg,
				    360.*deg);
  
  fSecondMetalizationLV = new G4LogicalVolume(fSecondMetalizationS,
					      fSecondMetalizationMaterial,
					      "SecondMetalizationLV",
					      0,
					      0,
					      0);
  
  fSecondMetalizationPV = new G4PVPlacement(0,                // no rotation
					    positionSecondMetalization, // at (x,y,z)
					    fSecondMetalizationLV,   // its logical volume
					    "SecondMetalization",       // its name
					    fMagneticLV,   // its mother volume
					    false,            // no boolean operations
					    0,                // copy number
					    fCheckOverlaps);  // checking overlaps 

// --------------------------------------------------------
// tiny steps in the trace lines (made of Au) ...
// Sets a max step length in the "detector" region, with G4StepLimiter, of 5 nm
// ---------------------------------------------------------

  fStepLimit = new G4UserLimits(maxMetalizationStep);
  fSecondMetalizationLV->SetUserLimits(fStepLimit);

  G4cout << "    Metalization of the second foil is " << fSecondMetalizationThickness/nm << " nm of " << fSecondMetalizationMaterial->GetName() << G4endl;
  

  // *********************************************************
  // ======= DETECTOR   ===========
  // *********************************************************
  
  G4ThreeVector positionDetector = positionSecondDegrader + G4ThreeVector(0,0, fSecondDegraderThickness/2 + 3*fSecondMetalizationThickness/2);
  G4cout << " Detector is placed at " << positionDetector << G4endl;


  fDetectorS = new G4Tubs("detectorS",
			  0.,
			  foilRadius,
			  5*nm,
			  0.*deg,
			  360.*deg);
  
  fDetectorLV = new G4LogicalVolume(fDetectorS,
				    fVacuumMaterial,
				    "DetectorLV",
				    0,
				    0,
				    0);
  
  fDetectorPV = new G4PVPlacement(0,                // no rotation
				  positionDetector, // at (x,y,z)
				  fDetectorLV,   // its logical volume
				  "Detector",       // its name
				  fMagneticLV,   // its mother volume
				  false,            // no boolean operations
				  0,                // copy number
				  fCheckOverlaps);  // checking overlaps 

// --------------------------------------------------------
// tiny steps in the trace lines (made of Au) ...
// Sets a max step length in the "detector" region, with G4StepLimiter, of 5 nm
// ---------------------------------------------------------

  fDetectorLV->SetUserLimits(new G4UserLimits(5*nm));
  
// *********************************************************
// =======   particles dump  =========================
// *********************************************************
  
  // G4ThreeVector positionDump = positionSecondMetalization + G4ThreeVector(0,0,foilMetalizationLength/2 + foilGap + dumpLength/2);
  G4ThreeVector positionDump = G4ThreeVector(0,0,worldLength/2 - dumpLength/2);
  
  G4cout << " Dump is placed at " << positionDump << G4endl;

  fDumpCapS = new G4Tubs("dumpCap",
			 0.,
			 dumpRadius,
			 dumpLength/2,
			 0.*deg,
			 360.*deg);
  
  fDumpTubeS = new G4Tubs("dumpTube",
			  dumpRadius,
			  1.01*dumpRadius,
			  worldLength/2,
			  0.*deg,
			  360.*deg);

  fDumpS = new G4UnionSolid("dump", fDumpTubeS, fDumpCapS, 0,positionDump);
  fDumpS = new G4UnionSolid("dump", fDumpS, fDumpCapS, 0,-positionDump);
  
  fDumpLV = new G4LogicalVolume(fDumpS,
				fDumpMaterial,
				"DumpLV",
				0,
				0,
				0);
  
  fDumpPV = new G4PVPlacement(0,                // no rotation
			      G4ThreeVector(0,0,0), // at (x,y,z)
			      fDumpLV,   // its logical volume
			      "Dump",       // its name
			      fWorldLV,   // its mother volume
			      false,            // no boolean operations
			      0,                // copy number
			      fCheckOverlaps);  // checking overlaps 

// --------------------------------------------------------
// tiny steps in the dump (made of Au) ...
// Sets a max step length in the "dump" region, with G4StepLimiter, of 5 nm
// ---------------------------------------------------------

  fDumpLV->SetUserLimits(new G4UserLimits(worldLength));

  G4cout << "Dump is " << dumpLength/nm << " nm of " << fDumpMaterial->GetName() << G4endl;

  // Visualization attributes

  G4VisAttributes* worldVisAtt= new G4VisAttributes(G4Colour(1.0,1.0,1.0));
  fWorldLV->SetVisAttributes(worldVisAtt);  

  G4VisAttributes* firstDegraderVis = new G4VisAttributes(G4Colour(0.0,1.0,0.0)); // green
  firstDegraderVis->SetForceSolid(true);
  if(fFirstDegraderThickness > 0) fFirstDegraderLV->SetVisAttributes(firstDegraderVis);
  
  G4VisAttributes* secondDegraderVis = new G4VisAttributes(G4Colour(0.0,0.0,1.0)); // blue
  secondDegraderVis->SetForceSolid(true);
  fSecondDegraderLV->SetVisAttributes(secondDegraderVis);

  G4VisAttributes* metalizationVis = new G4VisAttributes(G4Colour::Gray());
  if(fFirstDegraderThickness > 0) fSecondMetalizationLV->SetVisAttributes(metalizationVis);
  fSecondMetalizationLV->SetVisAttributes(metalizationVis);
  
  // G4VisAttributes* redDump= new G4VisAttributes(G4Colour(1.0,0.0,0.0));
  // fDumpLV ->SetVisAttributes(redDump);

  G4VisAttributes* magneticVisAtt= new G4VisAttributes(G4Colour(0.9,0.9,0.9,0.2));
  magneticVisAtt->SetForceAuxEdgeVisible(true);
  fMagneticLV->SetVisAttributes(magneticVisAtt);  


  // Example of User Limits
  //
  // Below is an example of how to set tracking constraints in a given
  // logical volume
  //
  // Sets a max step length in the tracker region, with G4StepLimiter

  //  G4double maxStep = 0.5*chamberWidth;
  //  fStepLimit = new G4UserLimits(maxStep);
  //  trackerLV->SetUserLimits(fStepLimit);
  
  /// Set additional contraints on the track, with G4UserSpecialCuts
  ///
  /// G4double maxLength = 2*trackerLength, maxTime = 0.1*ns, minEkin = 10*MeV;
  /// trackerLV->SetUserLimits(new G4UserLimits(maxStep,
  ///                                           maxLength,
  ///                                           maxTime,
  ///                                           minEkin));

  // Always return the physical world

  return fWorldPV;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
void B2bDetectorConstruction::ConstructSDandField()
{
  // Sensitive detectors

  // G4String trackerChamberSDname = "B2/TrackerChamberSD";
  B2TrackerSD* dumpDetector = new B2TrackerSD("AntiprotonDetector", "AntiprotonHitsCollection");
  fDetectorLV->SetSensitiveDetector(dumpDetector);
  
  // Create global magnetic field messenger.
  // Uniform magnetic field is then created automatically if
  // the field value is not zero.
  
  // magnetic field ----------------------------------------------------------
  fMagneticField = new B2MagneticField();
  fFieldMgr = new G4FieldManager();
  fFieldMgr->SetDetectorField(fMagneticField);
  fFieldMgr->CreateChordFinder(fMagneticField);
  //
  // associate this field manager with the logical volume (magnetic volume)
  G4bool forceToAllDaughters = true;
  if(fBFieldOn) fMagneticLV->SetFieldManager(fFieldMgr, forceToAllDaughters);
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2bDetectorConstruction::SetFirstDegraderThickness(G4double size)
{
  fFirstDegraderThickness = size;
  // Construct();
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2bDetectorConstruction::SetSecondDegraderThickness(G4double size)
{
  fSecondDegraderThickness = size;
  // Construct();
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2bDetectorConstruction::SetFirstMetalizationThickness(G4double size)
{
  fFirstMetalizationThickness = size;
  // Construct();
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2bDetectorConstruction::SetSecondMetalizationThickness(G4double size)
{
  fSecondMetalizationThickness = size;
  // Construct();
  G4RunManager::GetRunManager()->GeometryHasBeenModified();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
void B2bDetectorConstruction::SetMaxStep(G4double maxStep)
{
  if ((fStepLimit)&&(maxStep>0.)) fStepLimit->SetMaxAllowedStep(maxStep);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// General function for changing material of specified Logical Volume
G4Material* B2bDetectorConstruction::SetMaterial(G4LogicalVolume* fGeneralLV, G4String materialName)
{
  G4NistManager* nistManager = G4NistManager::Instance();
  if(G4StrUtil::contains(materialName, "PARYLENE")) materialName = "G4_NAPHTHALENE";
  G4Material* pttoMaterial = nistManager->FindOrBuildMaterial(materialName);
  if (fGeneralLV) {
     if ( pttoMaterial ) {
       if (fGeneralLV->GetMaterial() != pttoMaterial){
	 fGeneralLV->SetMaterial(pttoMaterial);
	 G4cout << G4endl << "----> The " << fGeneralLV->GetName() <<  " is made of " << materialName << G4endl;
       }
     } else {
       G4cout
	 << G4endl 
	 << "-->  WARNING from SetMaterial : "
	 << materialName << " not found" << G4endl;
     }
  }
  G4cout<<pttoMaterial<<G4endl;
  return pttoMaterial;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2bDetectorConstruction::SetFirstDegraderMaterial(G4String materialName)
{
  fFirstDegraderMaterial = SetMaterial(fFirstDegraderLV, materialName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2bDetectorConstruction::SetSecondDegraderMaterial(G4String materialName)
{
  fSecondDegraderMaterial = SetMaterial(fSecondDegraderLV, materialName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2bDetectorConstruction::SetFirstMetalizationMaterial(G4String materialName)
{
  fFirstMetalizationMaterial = SetMaterial(fFirstMetalizationLV, materialName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2bDetectorConstruction::SetSecondMetalizationMaterial(G4String materialName)
{
  fSecondMetalizationMaterial = SetMaterial(fSecondMetalizationLV,  materialName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2bDetectorConstruction::SetMagneticField(G4bool state)
{
  fBFieldOn = state;
  if(fMagneticLV){
    if(fBFieldOn)fMagneticLV->SetFieldManager(fFieldMgr, true);
    else fMagneticLV->SetFieldManager(NULL, true);
  }

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

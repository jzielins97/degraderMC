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
/// \file AEgISDetectorConstruction.cc
/// \brief Implementation of the AEgISDetectorConstruction class
 
#include "AEgISDetectorConstruction.hh"
#include "AEgISMagneticField.hh"
#include "AEgISGlobalField.hh"
#include "AEgISDetectorMessenger.hh"
#include "AEgISTrackerSD.hh"

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

#include "G4GeometryManager.hh"
#include "G4PhysicalVolumeStore.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4SolidStore.hh"

// BBBBBBBBBBBBBBBBBBBBBB
#include "G4SDManager.hh"
#include "G4VSensitiveDetector.hh"
#include "G4RunManager.hh"
#include "G4StateManager.hh"
#include "G4GenericMessenger.hh"
// BBBBBBBBBBBBBBBBBBBBBB

#include "G4VisAttributes.hh"
#include "G4Colour.hh"

#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// BBBBBBBBBBBBBBBBBBBBBB
//*G4ThreadLocal G4GlobalMagFieldMessenger* AEgISDetectorConstruction::fMagFieldMessenger = 0;
G4ThreadLocal AEgISMagneticField* AEgISDetectorConstruction::fMagneticField = 0;
G4ThreadLocal G4FieldManager* AEgISDetectorConstruction::fFieldMgr = 0;
// BBBBBBBBBBBBBBBBBBBBBB


AEgISDetectorConstruction::AEgISDetectorConstruction():
  G4VUserDetectorConstruction(),
  fWorldLV(NULL),
  fFirstDegraderLV(NULL),fFirstMetalizationLV(NULL),
  fSecondDegraderLV(NULL), fSecondMetalizationLV(NULL),
  fDumpLV(NULL),fMgnt5TLV(NULL),
  fFirstDegraderMaterial(NULL),fFirstMetalizationMaterial(NULL),
  fSecondDegraderMaterial(NULL),fSecondMetalizationMaterial(NULL),
  fCheckOverlaps(true)
{
  fMessenger = new AEgISDetectorMessenger(this);
  fFirstDegraderThickness = 100 * nm;
  fSecondDegraderThickness = 100 * nm;

  fFirstMetalizationThickness = 10 * nm;
  fSecondMetalizationThickness = 10 * nm;

  for(int i=0;i<7;i++){
    fDetectorS[i] = NULL;
    fDetectorLV[i] = NULL;
    fDetectorPV[i] = NULL;
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
AEgISDetectorConstruction::~AEgISDetectorConstruction()
{
  delete fStepLimit;
  delete fMessenger;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
G4VPhysicalVolume* AEgISDetectorConstruction::Construct()
{
  if (fWorldPV) {
     G4GeometryManager::GetInstance()->OpenGeometry();
     G4PhysicalVolumeStore::GetInstance()->Clean();
     G4LogicalVolumeStore::GetInstance()->Clean();
     G4SolidStore::GetInstance()->Clean();
  }

  DefineMaterials();
  return DefineVolumes();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void AEgISDetectorConstruction::DefineMaterials()
{
  // Material definition 

  G4NistManager* nistManager = G4NistManager::Instance();
  
  // Vacum (for the beam tube) defined using NIST Manager
  fVacuumMaterial = nistManager->FindOrBuildMaterial("G4_Galactic");
  
  // Air defined using NIST Manager
  // fWorldMaterial  = nistManager->FindOrBuildMaterial("G4_AIR");
  
  //---------------------  Target and Degrader material -------------------
  
  if(!fFirstDegraderMaterial) fFirstDegraderMaterial  = nistManager->FindOrBuildMaterial("G4_AIR");
  if(!fSecondDegraderMaterial) fSecondDegraderMaterial = nistManager->FindOrBuildMaterial("G4_MYLAR");
  
  // Vapor deposited material
  if(!fFirstMetalizationMaterial) fFirstMetalizationMaterial = nistManager->FindOrBuildMaterial("G4_Al");
  if(!fSecondMetalizationMaterial) fSecondMetalizationMaterial = nistManager->FindOrBuildMaterial("G4_Al");

  fTrapMaterial = nistManager->FindOrBuildMaterial("G4_Al");

  // Print materials
  G4cout << *(G4Material::GetMaterialTable()) << G4endl;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// define B-field volumes as part of general definition of volumes in DefineVolumes()

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void AEgISDetectorConstruction::AddDetector(G4ThreeVector position, G4LogicalVolume* motherLV, G4int detectorID)
{
  fDetectorS[detectorID] = new G4Tubs("detectorS_"+detectorID,
			  0.,
			  17 * mm,
			  2.5*nm,
			  0.*deg,
			  360.*deg);
  
  fDetectorLV[detectorID] = new G4LogicalVolume(fDetectorS[detectorID],
				    fVacuumMaterial,
				    "DetectorLV_"+detectorID,
				    0,
				    0,
				    0);
  
  fDetectorPV[detectorID] = new G4PVPlacement(0,                // no rotation
				  position, // at (x,y,z)
				  fDetectorLV[detectorID],   // its logical volume
				  "Detector_"+detectorID,       // its name
				  motherLV,   // its mother volume
				  false,            // no boolean operations
				  0,                // copy number
				  false);  // checking overlaps

// --------------------------------------------------------
// tiny steps in the trace lines (made of Au) ...
// Sets a max step length in the "detector" region, with G4StepLimiter, of 5 nm
// ---------------------------------------------------------

  fDetectorLV[detectorID]->SetUserLimits(new G4UserLimits(5*nm));

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VPhysicalVolume* AEgISDetectorConstruction::DefineVolumes()
{
  G4cout<<"++++++ definition of the geometry ++++++"<<G4endl;
  // Sizes of the principal geometrical components (solids)
// ==========================================================
  // beam tube for antiproton transport
  fMgnt5Tradius = 300 * mm;
  // G4double magneticLength = 3500*mm; 
  fMgnt5Tlength = 3570*mm; // full 5T
  
  // radius for the foils
  G4double foilRadius = 10.5 * mm;

  //traps
  G4double trap5Tlength = 1070 * mm;
  G4double trap1Tlength = 480 * mm;
  G4double trapSpacing = 66 * mm;
  G4double trapInnerD = 30 * mm;
  G4double trapThickness = 3 * mm;

  // 1T MCP
  

  // antiproton dump
  G4double dumpRadius = 1.2*fMgnt5Tradius;

  // spacer
  G4double spacerLength = 10*cm;

  // steps
  G4double maxFoilStep = 50*nm;
  // G4double maxFieldStep = fMgnt5Tlength/10;
  G4double maxMetalizationStep = 5 * nm;

// ==========================================================
  G4double worldLength = 1.1 * (spacerLength/2 + fMgnt5Tlength  + spacerLength/2);
  G4double dumpLength = 0.01*worldLength;
  
  // Definitions of Solids, Logical Volumes, Physical Volumes
  // =================== World =========================

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
  

  // *********************************************************
  // ================= Magnetic field ========================
  // *********************************************************

  
   fMgnt5TCenter = G4ThreeVector(0,0,0);
   // G4ThreeVector positionBeamTube = G4ThreeVector(0,0,-fMgnt5Tlength);
   G4cout << "Magnetic tube 5T is placed at " << fMgnt5TCenter << G4endl;
   G4cout << "  From "<< fMgnt5TCenter[2] - fMgnt5Tlength/2<<" to "<< fMgnt5TCenter[2] + fMgnt5Tlength/2<<G4endl;
   // fMagneticFieldStart = positionBField[2] - fMgnt5Tlength/2;
  
   // BBBBBBBBBBBBBBBB overlapping magnetic field region BBBBBBBBBBBBBB
   // Tube with Local Magnetic field
  
   fMgnt5TS = new G4Tubs("magneticTubs",
			   0.,
			   fMgnt5Tradius,
			   fMgnt5Tlength/2,
			   0.,
			   360.*deg);
  
   fMgnt5TLV = new G4LogicalVolume(fMgnt5TS,
				   fVacuumMaterial,
				   "MagneticLV");
   
   // placement of Tube

   fMgnt5TPV = new G4PVPlacement(0,              // no rotation
				   fMgnt5TCenter, // at (x,y,z)
				   fMgnt5TLV,   // its logical volume
				   "MagneticField",       // its name
				   //                    worldLV,        // its mother volume
				   fWorldLV,        // its mother volume
				   false,          // no boolean operations
				   0,              // copy number
				   fCheckOverlaps); // checking overlaps

   // BBBBBBBBBBBBBBBB

   G4bool firstFoil = (fFirstDegraderThickness > 0);
   // magnetic field starts 250 cm from the center of the experiment
   // first foil is positioned at 170 cm from the center
   // so it is 250 - 170 = 80 cm from the beginning of the magnetic field
   G4ThreeVector positionFirstMetalization = G4ThreeVector(0,0, -fMgnt5Tlength/2 + 80*cm + fFirstMetalizationThickness/2);
   
  if(firstFoil){ // only place thin degrader if its thickness is more than 0
    // this degrader has a metalization layer:
    // *********************************************************
    // ===== First degrader metalization layer (downstream) ====
    // *********************************************************
    G4cout << "Metalization of the first foils is placed at " << fMgnt5TCenter + positionFirstMetalization << G4endl;
    
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
					     fMgnt5TLV,   // its mother volume
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


    G4ThreeVector positionFirstDegrader = positionFirstMetalization + G4ThreeVector(0,0, fFirstMetalizationThickness/2 + fFirstDegraderThickness/2);
    G4cout << "First degrader foil is placed at " << fMgnt5TCenter + positionFirstDegrader << G4endl;

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
					 fMgnt5TLV, // its mother volume
					 false,          // no boolean operations
					 0,              // copy number
					 fCheckOverlaps); // checking overlaps 

    // --------------------------------------------------
    // tiny steps in the trace lines
    // ---------------------------------------------------------
    fStepLimit = new G4UserLimits(maxFoilStep);
    fFirstDegraderLV->SetUserLimits(fStepLimit);
    
    G4cout << "    First degrader foil is " << fFirstDegraderThickness/nm << " nm of " << fFirstDegraderMaterial->GetName() << G4endl;
    
  }else{ // only place thin degrader if its thickness is more than 0
    fFirstDegraderLV = NULL;
    fFirstMetalizationLV = NULL;
  }
  
  
  // *********************************************************
  // Second Degrader foil (in magnetic field)
  // *********************************************************
  // main degrader is placed 111.9 cm from the center of the experiment
  // so it is 250 - 111.9 = 138.1 cm from beginning of the magnetic field
  G4ThreeVector positionSecondDegrader(0,0,-fMgnt5Tlength/2 + 138.1 * cm + fSecondDegraderThickness/2);
  G4cout << "Second degrader foil is placed at " << fMgnt5TCenter + positionSecondDegrader << G4endl;
  
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
				       fMgnt5TLV, // its mother volume
				       false,          // no boolean operations
				       0,              // copy number
				       fCheckOverlaps); // checking overlaps 

// --------------------------------------------------
 fStepLimit = new G4UserLimits(maxFoilStep);
 fSecondDegraderLV->SetUserLimits(fStepLimit);

 G4cout << "    Second degrader foil is " << fSecondDegraderThickness/nm << " nm of "
	<< fSecondDegraderMaterial->GetName() << G4endl;

// *********************************************************
// ======= second degrader metalization layer    ===========
// *********************************************************
  
  G4ThreeVector positionSecondMetalization = positionSecondDegrader + G4ThreeVector(0,0, fSecondDegraderThickness/2 + fSecondMetalizationThickness/2);
  G4cout << "Metalization of the second foil is placed at " << fMgnt5TCenter + positionSecondMetalization << G4endl;


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
					    fMgnt5TLV,   // its mother volume
					    false,            // no boolean operations
					    0,                // copy number
					    fCheckOverlaps);  // checking overlaps 

// --------------------------------------------------------
// tiny steps in the trace lines (made of Au) ...
// ---------------------------------------------------------
  fStepLimit = new G4UserLimits(maxMetalizationStep);
  fSecondMetalizationLV->SetUserLimits(fStepLimit);
  
  G4cout << "    Metalization of the second foil is " << fSecondMetalizationThickness/nm << " nm of " << fSecondMetalizationMaterial->GetName() << G4endl;
  
  // *********************************************************
  // ======= 5T trap ===========
  // *********************************************************
  G4ThreeVector positionTrap5T = positionSecondMetalization + G4ThreeVector(0,0,fSecondMetalizationThickness/2 + 5 * mm + trap5Tlength/2);
  G4cout << "5T Trap is placed at " << positionTrap5T << G4endl;
  G4cout << "  From "<< positionTrap5T[2] - trap5Tlength/2<<" to "<< positionTrap5T[2] + trap5Tlength/2<<G4endl;
  
  fTrap5TS = new G4Tubs("trap5T",
			trapInnerD/2,
			trapInnerD/2+trapThickness,
			trap5Tlength/2,
			0.*deg,
			360.*deg);
  
  fTrap5TLV = new G4LogicalVolume(fTrap5TS,
				  fTrapMaterial,
				  "Trap5TLV",
				  0,
				  0,
				  0);
  
  fSecondDegraderPV = new G4PVPlacement(0,              // no rotation
					positionTrap5T, // at (x,y,z)
					fTrap5TLV,   // its logical volume
					"Trap5T",       // its name
					fMgnt5TLV, // its mother volume
					false,          // no boolean operations
					0,              // copy number
					fCheckOverlaps); // checking overlaps 
  
  
  fSecondMetalizationLV->SetUserLimits(new G4UserLimits(maxMetalizationStep));

  // *********************************************************
  // ======= electrode in the 5T trap    ===========
  // *********************************************************
  // let's assume that the electrode is 3 mm thick
  G4ThreeVector positionElectrode = G4ThreeVector(0,0, trap5Tlength/2 - trapThickness/2);
  G4cout << "HV Electrode is placed at " << fMgnt5TCenter + positionTrap5T + positionElectrode << G4endl;
  
  fHVElectrodeS = new G4Tubs("HVElectrodeS",
			     0.,
			     trapInnerD/2,
			     trapThickness/2,
			     0.*deg,
			     360.*deg);
  
  fHVElectrodeLV = new G4LogicalVolume(fHVElectrodeS,
				       fVacuumMaterial,
				       "HVElectrodeLV",
				       0,
				       0,
				       0);
  
  fHVElectrodePV = new G4PVPlacement(0,                // no rotation
				     positionElectrode, // at (x,y,z)
				     fHVElectrodeLV,   // its logical volume
				     "HVElectrode",       // its name
				     fMgnt5TLV,   // its mother volume
				     false,            // no boolean operations
				     0,                // copy number
				     fCheckOverlaps);  // checking overlaps 
 
  
  // *********************************************************
  // ======= DETECTOR   ===========
  // *********************************************************
  G4ThreeVector positionDetector;
  // each detector is 5 nm thick
  if(firstFoil){
    // first foil backward detector
    positionDetector  = positionFirstMetalization - G4ThreeVector(0,0, fFirstMetalizationThickness/2 + 2.5 * nm);
    G4cout << " First foil backward detector is placed at " << fMgnt5TCenter + positionDetector << G4endl;
    AddDetector(positionDetector, fMgnt5TLV, 0);

    // first foil backward detector
    positionDetector  = positionFirstMetalization + G4ThreeVector(0,0, fFirstMetalizationThickness/2 + fFirstDegraderThickness + 2.5 * nm);
    G4cout << " First foil forward detector is placed at " << fMgnt5TCenter + positionDetector << G4endl;
    AddDetector(positionDetector, fMgnt5TLV, 1);
  }
  
  // second foil backward detector
  positionDetector  = positionSecondDegrader - G4ThreeVector(0,0, fSecondDegraderThickness/2 + 2.5 * nm);
  G4cout << " Second foil backward detector is placed at " << fMgnt5TCenter + positionDetector << G4endl;
  AddDetector(positionDetector, fMgnt5TLV, 2);

  // second foil forward detector
  positionDetector  = positionSecondDegrader + G4ThreeVector(0,0, fSecondDegraderThickness/2 + fSecondMetalizationThickness + 2.5 * nm);
  G4cout << " Second foil forward detector is placed at " << fMgnt5TCenter + positionDetector << G4endl;
  AddDetector(positionDetector, fMgnt5TLV, 3);

  //=========== 5T trap detectors for trapping=========
  // 5T trap - inside
  // position it at the end of the 5T trap - inside
  positionDetector  = positionElectrode - G4ThreeVector(0,0, trapThickness/2 + 2.5 * nm);
  G4cout << " 5T inside detector is placed at " << fMgnt5TCenter + positionDetector << G4endl;
  AddDetector(positionDetector, fMgnt5TLV, 6);

  // 5T trap - exit forward
  // position it just outside 5T at the end
  positionDetector  = positionTrap5T + G4ThreeVector(0,0, trap5Tlength/2 + 2.5 * nm);
  G4cout << " 5T forward exist detector is placed at " << fMgnt5TCenter + positionDetector << G4endl;
  AddDetector(positionDetector, fMgnt5TLV, 5);

  // 5T trap - exit backward
  // position it at the front of 5T trap - inside
  positionDetector  = -G4ThreeVector(0,0, trap5Tlength/2 - 2.5 * nm);
  G4cout << " 5T backward exit detector is placed at " << fMgnt5TCenter + positionTrap5T + positionDetector << G4endl;
  AddDetector(positionDetector, fMgnt5TLV, 6);

  
// *********************************************************
// =======   particles dump  =========================
// *********************************************************
  
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
				fVacuumMaterial,
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
// tiny steps in the dump (made of nothing) ...
// Sets a max step length in the "dump" region, with G4StepLimiter, of 5 nm
// ---------------------------------------------------------

  fDumpLV->SetUserLimits(new G4UserLimits(worldLength));

  G4cout << "Dump is " << dumpLength/nm << " nm of " << fVacuumMaterial->GetName() << G4endl;

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
  if(fFirstDegraderThickness > 0) fFirstMetalizationLV->SetVisAttributes(metalizationVis);
  fSecondMetalizationLV->SetVisAttributes(metalizationVis);
  
 
  G4VisAttributes* magneticVisAtt= new G4VisAttributes(G4Colour(0.9,0.9,0.9,0.2));
  magneticVisAtt->SetForceAuxEdgeVisible(true);
  fMgnt5TLV->SetVisAttributes(magneticVisAtt);

  G4VisAttributes* trapVisAtt= new G4VisAttributes(G4Colour(1.,1.0,0.0));
  trapVisAtt->SetForceAuxEdgeVisible(true);
  fTrap5TLV->SetVisAttributes(trapVisAtt);  



  // Always return the physical world
  return fWorldPV;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
void AEgISDetectorConstruction::ConstructSDandField()
{
  // Sensitive detectors
  if(fDetectorLV[0]){
    G4cout << "Set First Foil Backward SD" << G4endl;
    AEgISTrackerSD* foil1BackwardDetector = new AEgISTrackerSD("Foil1BackwardDetector",
							       "Foil1BackwardCollection",
							       false, // is forward?
							       0, // ntuple id
							       true); // kill particle?
    fDetectorLV[0]->SetSensitiveDetector(foil1BackwardDetector);
  }

  if(fDetectorLV[1]){
    G4cout << "Set First Foil Forward SD" << G4endl;
    AEgISTrackerSD* foil1ForwardDetector = new AEgISTrackerSD("Foil1ForwardDetector",
							      "Foil1ForwardCollection",
							      true, // is forward?
							      1, // ntuple id
							      false); // kill particle?
    fDetectorLV[1]->SetSensitiveDetector(foil1ForwardDetector);
  }

  if(fDetectorLV[2]){
    G4cout << "Set Second Foil Backward SD" << G4endl;
    AEgISTrackerSD* foil2BackwardDetector = new AEgISTrackerSD("Foil2BackwardDetector",
							       "Foil2BackwardCollection",
							       false, // is forward?
							       2, // ntuple id
							       true); // kill particle?
    fDetectorLV[2]->SetSensitiveDetector(foil2BackwardDetector);
  }

  if(fDetectorLV[3]){
    G4cout << "Set Second Foil Forward SD" << G4endl;
    AEgISTrackerSD* foil2ForwardDetector = new AEgISTrackerSD("Foil2ForwardDetector",
							      "Foil2ForwardCollection",
							      true, // is forward?
							      3, // ntuple id
							      false); // kill particle?
    fDetectorLV[3]->SetSensitiveDetector(foil2ForwardDetector);
  }

  if(fDetectorLV[4]){
    G4cout << "Set Trap Inside SD" << G4endl;
    AEgISTrackerSD* trapInsideDetector = new AEgISTrackerSD("TrapInsideDetector",
							    "TrapInsideCollection",
							    false, // is forward?
							    4, // ntuple id
							    false); // kill particle?
    fDetectorLV[4]->SetSensitiveDetector(trapInsideDetector);
  }

  if(fDetectorLV[5]){
    G4cout << "Set Trap forward exit SD" << G4endl;
    AEgISTrackerSD* trapForawrdDetector = new AEgISTrackerSD("TrapForawrdDetector",
							    "TrapForawrdCollection",
							    true, // is forward?
							    5, // ntuple id
							    true); // kill particle?
    fDetectorLV[5]->SetSensitiveDetector(trapForawrdDetector);
  }

  if(fDetectorLV[6]){
    G4cout << "Set Trap backward exit SD" << G4endl;
    AEgISTrackerSD* trapBackwardDetector = new AEgISTrackerSD("TrapBackwardDetector",
							    "trapBackwardCollection",
							    false, // is forward?
							    6, // ntuple id
							    true); // kill particle?
    fDetectorLV[6]->SetSensitiveDetector(trapBackwardDetector);
  }


  if (!fFieldSetUp.Get()) {
     AEgISGlobalField* field = AEgISGlobalField::GetObject(this);
     G4AutoDelete::Register(field);  // Kernel will delete the AEgISGlobalField
     fFieldSetUp.Put(field);
  }
  // Create global magnetic field messenger.
  // Uniform magnetic field is then created automatically if
  // the field value is not zero.
  
  // magnetic field ----------------------------------------------------------
  // fMagneticField = new AEgISMagneticField(fMagneticFieldStart);
  // fFieldMgr = new G4FieldManager();
  // fFieldMgr->SetDetectorField(fMagneticField);
  // fFieldMgr->CreateChordFinder(fMagneticField);
  // //
  // // associate this field manager with the logical volume (magnetic volume)
  // G4bool forceToAllDaughters = true;
  // if(fBFieldOn) fMgnt5TLV->SetFieldManager(fFieldMgr, forceToAllDaughters);
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void AEgISDetectorConstruction::SetFirstDegraderThickness(G4double size)
{
  fFirstDegraderThickness = size;
  if ( G4StateManager::GetStateManager()->GetCurrentState() != G4State_PreInit ) {
    G4RunManager::GetRunManager()->ReinitializeGeometry();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void AEgISDetectorConstruction::SetSecondDegraderThickness(G4double size)
{
  fSecondDegraderThickness = size;
  if ( G4StateManager::GetStateManager()->GetCurrentState() != G4State_PreInit ) {
    G4RunManager::GetRunManager()->ReinitializeGeometry();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void AEgISDetectorConstruction::SetFirstMetalizationThickness(G4double size)
{
  fFirstMetalizationThickness = size;
  if ( G4StateManager::GetStateManager()->GetCurrentState() != G4State_PreInit ) {
    G4RunManager::GetRunManager()->ReinitializeGeometry();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void AEgISDetectorConstruction::SetSecondMetalizationThickness(G4double size)
{
  fSecondMetalizationThickness = size;
  if ( G4StateManager::GetStateManager()->GetCurrentState() != G4State_PreInit ) {
    G4RunManager::GetRunManager()->ReinitializeGeometry();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
 
void AEgISDetectorConstruction::SetMaxStep(G4double maxStep)
{
  if ((fStepLimit)&&(maxStep>0.)) fStepLimit->SetMaxAllowedStep(maxStep);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

// General function for changing material of specified Logical Volume
G4Material* AEgISDetectorConstruction::SetMaterial(G4LogicalVolume* fGeneralLV, G4String materialName)
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

void AEgISDetectorConstruction::SetFirstDegraderMaterial(G4String materialName)
{
  fFirstDegraderMaterial = SetMaterial(fFirstDegraderLV, materialName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void AEgISDetectorConstruction::SetSecondDegraderMaterial(G4String materialName)
{
  fSecondDegraderMaterial = SetMaterial(fSecondDegraderLV, materialName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void AEgISDetectorConstruction::SetFirstMetalizationMaterial(G4String materialName)
{
  fFirstMetalizationMaterial = SetMaterial(fFirstMetalizationLV, materialName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void AEgISDetectorConstruction::SetSecondMetalizationMaterial(G4String materialName)
{
  fSecondMetalizationMaterial = SetMaterial(fSecondMetalizationLV,  materialName);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void AEgISDetectorConstruction::SetMagneticField(G4bool state)
{
  f5TFieldOn = state;
  if ( G4StateManager::GetStateManager()->GetCurrentState() != G4State_PreInit ) {
    G4RunManager::GetRunManager()->ReinitializeGeometry();
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

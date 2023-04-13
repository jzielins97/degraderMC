/*****************************************
* Written based on the field04 from      *
* Geant4 extended examples.              *
*                                        *
* Author:                                *
* Jakub Zieliński, AEgIS Collaboration   *
* jakub.stanislaw.zielinski@cern.ch      *
*****************************************/ 
//
//
/// \file src/AEgISMagneticField5T.cc
/// \brief Implementation of the AEgISMagneticField5T class
//

#include "globals.hh"

#include "G4GeometryManager.hh"
#include "G4Tubs.hh"

#include "AEgISGlobalField.hh"

#include "AEgISMagneticField5T.hh"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

AEgISMagneticField5T::AEgISMagneticField5T(G4String filename,
					   G4LogicalVolume* lv,
					   G4ThreeVector center) : ElementField(center,lv)

{
  fFieldLength = 2.*((G4Tubs*)fVolume->GetSolid())->GetZHalfLength();
  fFieldRadius = ((G4Tubs*)fVolume->GetSolid())->GetOuterRadius();

  //  read the full B-field map (array as CSV giving r,z, Br and Bz)
  //  this array will be stored as aegisrB,aegisrZ,aegisbR,aegisbZ 
  //  and set aegisbGranularity (expect 65592 entries in the field map)

  G4double A[4];
  std::ifstream ifs;
  G4String s1;
  char c;

  aegisbGranularity=0;
  aegisrGranularity=0;

  ifs.open(filename);
  if(!ifs.is_open()){
    G4cerr<<"Error in opening file("<<filename<<")."<<G4endl;
    return;
  }

  int k=0;
  bool isNewZ = false;
  G4double prev_z = 0;

  while(getline(ifs,s1)){
    // if (ifs.eof()) {break;}
    std::stringstream stream(s1);
    int j=0;
    while(stream >> A[j]){
      j++;
      if (!stream) {break;}
    }
    
    if(k==0){
      fRmin=A[0];
      fRmax=A[0];
      fZmin=A[1];
      fZmax=A[1];
      prev_z = A[1];
    }else{
      // update position range
      if(fRmin > A[0]) fRmin = A[0];
      if(fRmax < A[0]) fRmax = A[0];
      if(fZmin > A[1]) fZmin = A[1];
      if(fZmax < A[1]) fZmax = A[1];
      if(prev_z != A[1]) isNewZ = true;
    }

    aegisrB[k]=A[0]*10;
    aegiszB[k]=A[1]*10;
    //save the B-field in T
    aegisbR[k]=A[2]/10000.;
    aegisbZ[k]=A[3]/10000.;
    
    k++;
    if(!isNewZ) aegisrGranularity++;
  }
  ifs.close();

  aegisbGranularity = k;
  aegiszGranularity = aegisbGranularity/aegisrGranularity;
  // file was in cm, change to mm
  fRmin *= 10.;
  fRmax *= 10.;
  fZmin *= 10.;
  fZmax *= 10.;
  
  return;
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void AEgISMagneticField5T::AddFieldValue(const G4double point[4],
                                           G4double field[6]) const
{
   G4ThreeVector global(point[0],point[1],point[2]);

   G4ThreeVector local = fGlobal2local.TransformPoint(global);
   if (IsOutside(local)) return;

   G4double length = ((AEgISMagneticField5T*)this)->GetLength();
   // caclulate the index (assume AEgIS Z axis is global Z axis -> this field is only translated along Z)
   G4double radius = sqrt(local[0]*local[0]+local[1]*local[1]);
   G4int rIndex = radius / ((fRmax-fRmin)/aegisrGranularity);
   G4int zIndex = (local[2] + length/2) / ((fZmax-fZmin)/aegiszGranularity);
   G4int index = zIndex*aegisrGranularity + rIndex;
   // G4cout<<"*****Checking the BField ******"<<G4endl;
   // G4cout<<"   we are at: "<<radius<<","<<fZmin + (local[2] + length/2)<<G4endl;
   // G4cout<<"   index : "<<index<<"("<<zIndex<<"*"<<aegisrGranularity<<"+"<<rIndex<<")"<<G4endl;
   // G4cout<<"   we read  : "<<aegisrB[index]<<","<<aegiszB[index]<<G4endl;
   

   if(index < 0 || index > aegisbGranularity) return;   
   G4ThreeVector B(0.0,0.0,0.0);
   if(radius!=0){
     B[0] = aegisbR[index]*local[0]/radius;
     B[1] = aegisbR[index]*local[1]/radius;
   }else{
     B[0] = 0.;
     B[1] = 0.;
   }
   B[2] = aegisbZ[index];

   B = fGlobal2local.Inverse().TransformAxis(B);

   field[0] += B[0];
   field[1] += B[1];
   field[2] += B[2];
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool AEgISMagneticField5T::IsOutside(G4ThreeVector& local) const
{
//  EInside inside = tubs->Inside(local);
//  return (inside == kOutside);
  G4double r = std::sqrt(local.x()*local.x()+local.y()*local.y());
  return (r > fFieldRadius || std::fabs(local.z()) > fFieldLength/2.0);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4bool AEgISMagneticField5T::IsWithin(G4ThreeVector& local) const
{
//  EInside inside = tubs->Inside(local);
//  return (inside == kInside);
  G4double r = std::sqrt(local.x()*local.x()+local.y()*local.y());
  return (r < fFieldRadius && std::fabs(local.z()) < fFieldLength/2.0);
}

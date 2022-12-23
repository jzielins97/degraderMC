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
/// \file B5MagneticField.cc
/// \brief Implementation of the B5MagneticField class

#include "B2MagneticField.hh"
#include "G4FileUtilities.hh"
#include "G4GenericMessenger.hh"
#include "G4SystemOfUnits.hh"
#include "globals.hh"
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
using namespace std;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B2MagneticField::B2MagneticField()
: G4MagneticField(), 
  fMessenger(nullptr), fBz(2.5*tesla)
{
  // define commands for this class
  DefineCommands();

//  read the full B-field map (array as CSV giving r,z, Br and Bz)
//  this array will be stored as aegisrB,aegisrZ,aegisbR,aegisbZ 
//  and set aegisbGranularity (expect 65592 entries in the field map)

    FILE *fp;
    G4double A[4];
    ifstream ifs;
    string s1;
    char c;

    aegisbGranularity=0;

    fp = fopen("bfield.csv", "r");
    if(NULL == fp)
    {
       printf("\nError in opening file.");
       return;
    }
    fclose(fp);

    ifs.open("bfield.csv");
    for (int k=0; k<65592; k++)
    {
      getline(ifs,s1);
      if (ifs.eof()) {break;}
      stringstream stream(s1);
      int j=0;
      while(1)
      {
        stream >> A[j];
        stream >> c;
        j++;
        if (!stream) {break;}
      }
      aegisrB[k]=A[0];
      aegiszB[k]=A[1];
      aegisbR[k]=A[2];
      aegisbZ[k]=A[3];
      aegisbGranularity = aegisbGranularity+1;
//      G4cout << " r, z, Br, Bz "  
//             << aegisrB[k] << " ; " << aegiszB[k] << " ; " << aegisbR[k] << " ; " << aegisbZ[k] 
//             << " ; " << aegisbGranularity << G4endl;

    }
    ifs.close();
    return;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B2MagneticField::~B2MagneticField()
{ 
  delete fMessenger; 
//  delete aegisrB;
//  delete aegiszB;
//  delete aegisbR;
//  delete aegisbZ;
}

void B2MagneticField::GetFieldValue(const G4double position[4],G4double *bField) const
{
// =============================================================
// Return as Bfield[0], [1], [2] the magnetic field x, y & z components
// and   as Bfield[3], [4], [5] the electric field x, y & z components
//
//   the coordinates are x=position[0], y=position[1], z=position[2]
//   the index n = (z[cm]+250)*301 + (r[cm]/30.)*300
//   the field map gives B_r and B_z, but we need to return B_x, B_y and B_z
//   so the relevant values are:
//
//   B_z = aegisbZ[n]
//   B_x = aegisbR[n]*x/sqrt(x^2+y^2)
//   B_y = aegisbR[n]*y/sqrt(x^2+y^2)
//

// cordinates are in mm, map is in cm

      G4double radius = sqrt(position[0]*position[0] + position[1]*position[1])/10.;
      G4int index = position[2]/10;
            index = (index+250)*301 + (radius*10);

//      G4cout << " vvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvvv" << G4endl;      
//      G4cout << " number of points in field map: " << aegisbGranularity << G4endl;
//      G4cout << " GetFieldValue: r,z,index: " << radius << " , " << position[2] << " , " << index << G4endl;

// B-field in Tesla (map is in Gauss)

     if ((position[2]>-3.5*cm)||(radius>10.*cm)||(index>=aegisbGranularity)||(index<0)) {
       bField[0] = 0.;
       bField[1] = 0.;
       bField[2] = 0.1;
     } else {
       if (radius!=0.) {
          bField[0] = aegisbR[index]*position[0]/radius/10000.;
          bField[1] = aegisbR[index]*position[1]/radius/10000.;
       } else {
          bField[0] = 0.;
          bField[1] = 0.;
       }
       bField[2] = aegisbZ[index]/10000.;
     }

     if ((index>=aegisbGranularity)||(index<0)) {
//     G4cout << " index out of bounds i,z,r !!! " << index << " " << position[2] << " " << radius << G4endl;
     } else {
//     G4cout << " GetFieldValue: x,y,z,r,index: " << position[0] << " " << position[1] << " " 
//            << position[2] << " " << radius  << " , " << index << G4endl;
//     G4cout << " GetValue: Bx,Bz,Bz,Br " << bField[0] << " " << bField[1] << " " << bField[2] << 
//               " ; " << aegisbR[index] << " " << aegisbZ[index] << G4endl;
     }
//      G4cout << " ^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^^" << G4endl;      

//     bField[0] = 0.;
//     bField[1] = 0.;
//     bField[2] = 0.1;
     
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2MagneticField::DefineCommands()
{
  // Define /B2/field command directory using generic messenger class
  fMessenger = new G4GenericMessenger(this,
				      "/AEgIS/field/",
				      "Field control");

  // fieldValue command 
  auto& valueCmd = fMessenger->DeclareMethodWithUnit("value",
						     "tesla",
						     &B2MagneticField::SetField, 
						     "Set field strength.");
  
  valueCmd.SetParameterName("field", true);
  valueCmd.SetDefaultValue("1.");
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

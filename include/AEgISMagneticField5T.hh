/*****************************************
* Coppied from the field04 from Geant4   *
* extended examples.                     *
*                                        *
* Author:                                *
* Jakub Zieliński, AEgIS Collaboration   *
* jakub.stanislaw.zielinski@cern.ch      *
*****************************************/ 

/// \file include/AEgISMagneticField5T.hh
/// \brief Definition of the AEgISMagneticField5T class
//

#ifndef AEgISMagneticField5T_h
#define AEgISMagneticField5T_h 1

#include "G4LogicalVolume.hh"

#include "ElementField.hh"

//  AEgISMagneticField5T implements the magnetic field from 5T magnet
//  It uses a file with values of the magnetic field in AEgIS coordinate system
//  It means the distance is measured from the center of the experiment
//  file has r [cm], z [cm], Br [gaus], Bz [gaus]


class AEgISMagneticField5T : public ElementField
{
public:

  ///  Default constructor.
  AEgISMagneticField5T(G4String, G4LogicalVolume*, G4ThreeVector);

  ///  Destructor.
  virtual ~AEgISMagneticField5T() {}

  ///  GetLength() returns the length of the solenoid
  virtual G4double GetLength() { return fFieldLength; }

  ///  GetWidth() returns the solenoid diameter
  virtual G4double GetWidth() { return fFieldRadius*2.0; }

  ///  GetHeight() returns the solenoid diameter
  virtual G4double GetHeight() { return fFieldRadius*2.0; }

  ///  IsOutside() returns true when outside the solenoid
  G4bool IsOutside(G4ThreeVector& local) const;

  /// IsWithin() returns true when inside the solenoid
  G4bool IsWithin(G4ThreeVector& local) const;

  ///  AddFieldValue() adds the field for this solenoid into field[].
  ///  point[] is in global coordinates.
  virtual void AddFieldValue(const G4double point[4], G4double field[6]) const;

private:
  // maximum and minimum position in the AEgIS coordinate system
  G4double fZmin;
  G4double fRmin;
  G4double fZmax;
  G4double fRmax;

  //array to store the B-field values from the file
  G4double aegisrB[107067];
  G4double aegiszB[107067];
  G4double aegisbR[107067];
  G4double aegisbZ[107067];
  G4int    aegisbGranularity; // how many total points are storred in the file
  G4int    aegisrGranularity; // how many points along R
  G4int    aegiszGranularity; // how many points along Z axis

  G4double fFieldLength;
  G4double fFieldRadius;

};

#endif

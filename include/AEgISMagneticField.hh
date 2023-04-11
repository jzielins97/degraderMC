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
/// \file B5MagneticField.hh
/// \brief Definition of the B5MagneticField class

#ifndef AEgISMagneticField_H
#define AEgISMagneticField_H 1

#include "globals.hh"
#include "G4MagneticField.hh"

class G4GenericMessenger;

/// Magnetic field

class AEgISMagneticField : public G4MagneticField
{
  public:
    AEgISMagneticField(G4double);
    virtual ~AEgISMagneticField();
    
    virtual void GetFieldValue(const G4double point[4], G4double* bField ) const;
        
  private:
    G4GenericMessenger* fMessenger;
    G4double fMagneticFieldStart; // first Z value with the magnetic field

//    G4double rB[6], zB[6], bR[6], bZ[6];
    // G4double aegisrB[107067];
    // G4double aegiszB[107067];
    G4double aegisbR[107067];
    G4double aegisbZ[107067];
    G4int    aegisbGranularity;

};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

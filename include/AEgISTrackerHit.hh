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
/// \file AEgISTrackerHit.hh
/// \brief Definition of the AEgISTrackerHit class

#ifndef AEgISTrackerHit_h
#define AEgISTrackerHit_h 1

#include "G4VHit.hh"
#include "G4THitsCollection.hh"
#include "G4Allocator.hh"
#include "G4ThreeVector.hh"
#include "tls.hh"

/// Tracker hit class
///
/// It defines data members to store the trackID, chamberNb, energy deposit,
/// and position of charged particles in a selected volume:
/// - fTrackID, fChamberNB, fEdep, fPos

class AEgISTrackerHit : public G4VHit
{
public:
  AEgISTrackerHit();
  AEgISTrackerHit(const AEgISTrackerHit&);
  virtual ~AEgISTrackerHit();

  // operators
  const AEgISTrackerHit& operator=(const AEgISTrackerHit&);
  G4bool operator==(const AEgISTrackerHit&) const;

  inline void* operator new(size_t);
  inline void  operator delete(void*);

  // methods from base class
  virtual void Draw();
  virtual void Print();

  // Set methods
  void SetTrackID  (G4int track)      { fTrackID = track; };
  void SetPos      (G4ThreeVector xyz){ fPos = xyz; };
  void SetMomentum (G4ThreeVector xyz){ fMomentum = xyz; };
  void SetMomentumDirection (G4ThreeVector xyz){ fMomentumDirection = xyz; };

  // Get methods
  G4int GetTrackID() const     { return fTrackID; };
  G4ThreeVector GetPos() const { return fPos; };
  G4ThreeVector GetMomentum() const { return fMomentum; };
  G4ThreeVector GetMomentumDirection () const { return fMomentumDirection; };

private:

  G4int         fTrackID;
  G4ThreeVector fPos;
  G4ThreeVector fMomentum;
  G4ThreeVector fMomentumDirection;
  
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

typedef G4THitsCollection<AEgISTrackerHit> AEgISTrackerHitsCollection;

extern G4ThreadLocal G4Allocator<AEgISTrackerHit>* AEgISTrackerHitAllocator;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void* AEgISTrackerHit::operator new(size_t)
{
  if(!AEgISTrackerHitAllocator) AEgISTrackerHitAllocator = new G4Allocator<AEgISTrackerHit>;
  return (void *) AEgISTrackerHitAllocator->MallocSingle();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

inline void AEgISTrackerHit::operator delete(void *hit)
{
  AEgISTrackerHitAllocator->FreeSingle((AEgISTrackerHit*) hit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

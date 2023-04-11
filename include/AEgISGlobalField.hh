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
/// \file include/AEgISGlobalField.hh
/// \brief Definition of the AEgISGlobalField class
//

#ifndef AEgISGlobalField_h
#define AEgISGlobalField_h 1

#include <vector>

#include "G4FieldManager.hh"
#include "G4PropagatorInField.hh"
#include "G4MagIntegratorStepper.hh"
#include "G4ChordFinder.hh"

#include "G4MagneticField.hh"
#include "G4ElectroMagneticField.hh"

#include "G4Mag_EqRhs.hh"
#include "G4Mag_SpinEqRhs.hh"

#include "G4EqMagElectricField.hh"
#include "G4EqEMFieldWithSpin.hh"

// #include "AEgISFieldMessenger.hh"
#include "ElementField.hh"

#include "AEgISDetectorConstruction.hh"

//  AEgISGlobalField - handles the global ElectroMagnetic field
//
//  There is a single AEgISGlobalField object.
//
//  The field from each individual beamline element is given by a
//  ElementField object. Any number of overlapping ElementField
//  objects can be added to the global field. Any element that
//  represents an element with an EM field must add the appropriate
//  ElementField to the global GlobalField object.

typedef std::vector<ElementField*> FieldList;

class AEgISGlobalField : public G4ElectroMagneticField {
//class AEgISGlobalField : public G4MagneticField {

private:

  AEgISGlobalField(AEgISDetectorConstruction* const);
  AEgISGlobalField(const AEgISGlobalField&);

  AEgISGlobalField& operator=(const AEgISGlobalField&);

  void SetupArray();

public:

  virtual ~AEgISGlobalField();

  /// GetObject() returns the single AEgISGlobalField object.
  /// It is constructed, if necessary.
  static AEgISGlobalField* GetObject(AEgISDetectorConstruction* const);
  static AEgISGlobalField* GetObject();

  /// GetFieldValue() returns the field value at a given point[].
  /// field is really field[6]: Bx,By,Bz,Ex,Ey,Ez.
  /// point[] is in global coordinates: x,y,z,t.
  virtual void GetFieldValue(const G4double* point, G4double* field) const;

  /// DoesFieldChangeEnergy() returns true.
  virtual G4bool DoesFieldChangeEnergy() const { return true; }

  /// AddElementField() adds the ElementField object for a single
  /// element to the global field.
  void AddElementField(ElementField* f)
  {
    if (fFields) fFields->push_back(f);
  }

  /// Clear() removes all ElementField-s from the global object,
  /// and destroys them. Used before the geometry is completely
  /// re-created.
  void Clear();

  /// constructs all field tracking objects
  void ConstructField();

  /// Set the Stepper types
  void SetStepperType( G4int i ) { fStepperType = i; }

  /// Set the Stepper
  void SetStepper();

  /// Set the minimum step length
  void SetMinStep(G4double stp) { fMinStep = stp; }

  /// Set the delta chord length
  void SetDeltaChord(G4double dcr) { fDeltaChord = dcr; }

  /// Set the delta one step length
  void SetDeltaOneStep(G4double stp) { fDeltaOneStep = stp; }

  /// Set the delta intersection length
  void SetDeltaIntersection(G4double its) { fDeltaIntersection = its; }

  /// Set the minimum eps length
  void SetEpsMin(G4double eps) { fEpsMin = eps; }

  /// Set the maximum eps length
  void SetEpsMax(G4double eps) { fEpsMax = eps; }

  /// Return the list of Element Fields
  FieldList* GetFields() { return fFields; }

protected:

  /// Get the global field manager
  G4FieldManager* GetGlobalFieldManager();

private:

  static G4ThreadLocal AEgISGlobalField* fObject;

  G4int fNfp;
  G4bool fFirst;

  FieldList* fFields;

  const ElementField **fFp;

private:

  // A. INVARIANTS: 
  // --------------
  // INVARIANT: an integer to indicate the type of RK integration method ('stepper') used
  G4int fStepperType;

  // INVARIANTS: Accuracy parameters of field propagation (and the integration it uses.)
  // 1. These values are lengths - initialised in src
  G4double fMinStep;
  G4double fDeltaChord;
  G4double fDeltaOneStep;
  G4double fDeltaIntersection;
  // 2. Dimensionless numbers - can initialise here
  G4double fEpsMin = 2.5e-7; // Relative accuracy of integration (minimum)
  G4double fEpsMax = 0.001;  // Relative accuracy of integration (maximum)

  // B. STATE: objects which carry out the propagation and are modified during tracking 
  // --------
//  G4Mag_EqRhs*            fEquation;
//  G4Mag_SpinEqRhs*        fEquation;

//  G4EqMagElectricField*   fEquation;
  G4EqEMFieldWithSpin*     fEquation        = nullptr;

  G4FieldManager*          fFieldManager    = nullptr;
  G4PropagatorInField*     fFieldPropagator = nullptr;
  G4MagIntegratorStepper*  fStepper         = nullptr;
  G4ChordFinder*           fChordFinder     = nullptr;

  // INVARIANTS during tracking: Auxiliary class & information - used for setup
  // AEgISFieldMessenger*       fFieldMessenger;
  AEgISDetectorConstruction* fDetectorConstruction= nullptr;
};

#endif

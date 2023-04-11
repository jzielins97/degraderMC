/*****************************************
* Coppied from the field04 from Geant4   *
* extended examples.                     *
*                                        *
* Author:                                *
* Jakub Zieliński, AEgIS Collaboration   *
* jakub.stanislaw.zielinski@cern.ch      *
*****************************************/ 

/// \file include/ElementField.hh
/// \brief Implementation of the ElementField class
//

#ifndef ElementField_h
#define ElementField_h 1

#include "globals.hh"

#include "G4Navigator.hh"
#include "G4TransportationManager.hh"

#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"

//  class F04ElementField - interface for the EM field of one element

//  This is the interface class used by GlobalField to compute the field
//  value at a given point[].

//  An element that represents an element with an EM field will
//  derive a class from this one and implement the computation for the
//  element. The Construct() function will add the derived object into
//  GlobalField.

class ElementField 
{

  private:

    ElementField& operator=(const ElementField&);

  public:

    ///  Constructor.
    ElementField(const G4ThreeVector, G4LogicalVolume*);

    /// the actual implementation constructs the F04ElementField
    void Construct();

    ///  Destructor.
    virtual ~ElementField() {}

    /// SetMaxStep(G4double) sets the max. step size
    void SetMaxStep(G4double stp)
    {
      fMaxStep = stp;
      fUserLimits->SetMaxAllowedStep(fMaxStep);
      fVolume->SetUserLimits(fUserLimits);
    }

    /// GetMaxStep() returns the max. step size
    G4double GetMaxStep() { return fMaxStep; }

    /// SetColor(G4String) sets the color
    void SetColor(G4String c)
    {
      fColor = c;
      fVolume->SetVisAttributes(GetVisAttribute(fColor));
    }

    /// GetColor() returns the color
    G4String GetColor() { return fColor; }

    ///  GetVisAttribute() returns the appropriate G4VisAttributes.
    static G4VisAttributes* GetVisAttribute(G4String color);

    ///  SetGlobalPoint() ensures that the point is within the global
    ///  bounding box of this ElementField's global coordinates.
    ///  Normally called 8 times for the corners of the local bounding
    ///  box, after a local->global coordinate transform.
    ///  If never called, the global bounding box is infinite.
    ///  BEWARE: if called only once, the bounding box is just a point.
    void SetGlobalPoint(const G4double point[4])
    {
      if(fMinX == -DBL_MAX || fMinX > point[0]) fMinX = point[0];
      if(fMinY == -DBL_MAX || fMinY > point[1]) fMinY = point[1];
      if(fMinZ == -DBL_MAX || fMinZ > point[2]) fMinZ = point[2];
      if(fMaxX ==  DBL_MAX || fMaxX < point[0]) fMaxX = point[0];
      if(fMaxY ==  DBL_MAX || fMaxY < point[1]) fMaxY = point[1];
      if(fMaxZ ==  DBL_MAX || fMaxZ < point[2]) fMaxZ = point[2];
    }

    ///  IsInBoundingBox() returns true if the point is within the
    ///  global bounding box - global coordinates.
    bool IsInBoundingBox(const G4double point[4]) const
    {
      if(point[2] < fMinZ || point[2] > fMaxZ) return false;
      if(point[0] < fMinX || point[0] > fMaxX) return false;
      if(point[1] < fMinY || point[1] > fMaxY) return false;
      return true;
    }

    ///  AddFieldValue() will add the field value for this element to field[].
    ///  Implementations must be sure to verify that point[] is within
    ///  the field region, and do nothing if not.
    ///  point[] is in global coordinates and geant4 units; x,y,z,t.
    ///  field[] is in geant4 units; Bx,By,Bz,Ex,Ey,Ez.
    ///  For efficiency, the caller may (but need not) call
    ///  IsInBoundingBox(point), and only call this function if that
    ///  returns true.
    virtual void
        AddFieldValue(const G4double point[4], G4double field[6]) const = 0;

    virtual G4double GetLength() = 0;
    virtual G4double GetWidth()  = 0;
    virtual G4double GetHeight() = 0;

  protected:

    G4LogicalVolume* fVolume;

    G4AffineTransform fGlobal2local;

//    F04ElementField(const F04ElementField&);

  private:

    static G4ThreadLocal G4Navigator* fNavigator;

    G4String fColor;

    G4ThreeVector fCenter;
    G4double fMinX, fMinY, fMinZ, fMaxX, fMaxY, fMaxZ;

    G4double fMaxStep;
    G4UserLimits* fUserLimits;

};

#endif

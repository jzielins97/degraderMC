/*****************************************
* Coppied from the field04 from Geant4   *
* extended examples.                     *
*                                        *
* Author:                                *
* Jakub Zieliński, AEgIS Collaboration   *
* jakub.stanislaw.zielinski@cern.ch      *
*****************************************/ 

/// \file src/ElementField.cc
/// \brief Implementation of the ElementField class
//

#include "ElementField.hh"

#include "AEgISGlobalField.hh"

#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4ThreadLocal G4Navigator* ElementField::fNavigator = 0;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

ElementField::ElementField(G4ThreeVector c, G4LogicalVolume* lv)
{
  fCenter = c;

  fMinX = fMinY = fMinZ = -DBL_MAX;
  fMaxX = fMaxY = fMaxZ =  DBL_MAX;

  AEgISGlobalField::GetObject()->AddElementField(this);

  fColor = "1,1,1";

  fVolume = lv;
  //fVolume->SetVisAttributes(GetVisAttribute(fColor));

  fMaxStep = 1*mm;
  fUserLimits = new G4UserLimits(fMaxStep);

  // fUserLimits->SetMaxAllowedStep(fMaxStep);

  // fUserLimits->SetUserMaxTrackLength(500.*m);
  // fUserLimits->SetUserMaxTime(10*ms);
  // fUserLimits->SetUserMinEkine(0.1*MeV);
  // fUserLimits->SetUserMinRange(1*mm);

  fVolume->SetUserLimits(fUserLimits);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void ElementField::Construct()
{
  G4Navigator* theNavigator =
                    G4TransportationManager::GetTransportationManager()->
                                                 GetNavigatorForTracking();

  if (!fNavigator) {
     fNavigator = new G4Navigator();
     if ( theNavigator->GetWorldVolume() )
               fNavigator->SetWorldVolume(theNavigator->GetWorldVolume());
   }

  fNavigator->LocateGlobalPointAndSetup(fCenter,0,false);

  G4TouchableHistoryHandle touchable = fNavigator->
                                         CreateTouchableHistoryHandle();

  G4int depth = touchable->GetHistoryDepth();
  for (G4int i = 0; i<depth; ++i) {
      if(touchable->GetVolume()->GetLogicalVolume() == fVolume)break;
      touchable->MoveUpHistory();
  }

  // set fGlobal2local transform
  fGlobal2local = touchable->GetHistory()->GetTopTransform();

  // set global bounding box
  G4double local[4], global[4];

  G4ThreeVector globalPosition;
  local[3] = 0.0;
  for (int i=0; i<2; ++i) {
      local[0] = (i==0 ? -1.0 : 1.0) * GetWidth()/2.;
      for (int j=0; j<2; ++j) {
          local[1] = (j==0 ? -1.0 : 1.0) * GetHeight()/2.;
          for (int k=0; k<2; ++k) {
              local[2] = (k==0 ? -1.0 : 1.0) * GetLength()/2.;
              G4ThreeVector localPosition(local[0],local[1],local[2]);
              globalPosition =
                         fGlobal2local.Inverse().TransformPoint(localPosition);
              global[0] = globalPosition.x();
              global[1] = globalPosition.y();
              global[2] = globalPosition.z();
              SetGlobalPoint(global);
           }
      }
  }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

G4VisAttributes* ElementField::GetVisAttribute(G4String color)
{
   G4VisAttributes* p = NULL;
   if(color.size() > 0 &&
     (isdigit(color.c_str()[0]) || color.c_str()[0] == '.')) {
        G4double red=0.0, green=0.0, blue=0.0;
        if (sscanf(color.c_str(),"%lf,%lf,%lf",&red,&green,&blue) == 3) {
           p = new G4VisAttributes(true,G4Color(red,green,blue));
        } else {
           G4cout << " Invalid color " << color << G4endl;
        }
   }

   if (!p) p = new G4VisAttributes(G4VisAttributes::GetInvisible());
   p->SetDaughtersInvisible(false);

   return p;
}

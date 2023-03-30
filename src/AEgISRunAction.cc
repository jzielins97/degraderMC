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
/// \file AEgISRunAction.cc
/// \brief Implementation of the AEgISRunAction class

#include "AEgISRunAction.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

AEgISRunAction::AEgISRunAction()
 : G4UserRunAction()
{ 
  // set printing event number per each 100 events
  G4RunManager::GetRunManager()->SetPrintProgress(1000);
  auto man = G4AnalysisManager::Instance();
  man->SetNtupleMerging(true);

  man->SetFirstNtupleId(0);

  man->CreateNtuple("fFoil1Backward","Antiprotons Reflected from Foil 1");
  man->CreateNtupleDColumn("x_mm");
  man->CreateNtupleDColumn("y_mm");
  man->CreateNtupleDColumn("z_mm");
  man->CreateNtupleDColumn("px_keV");
  man->CreateNtupleDColumn("py_keV");
  man->CreateNtupleDColumn("pz_keV");
  man->CreateNtupleDColumn("kineticEnergy_keV");
  man->CreateNtupleDColumn("time_ns");
  man->FinishNtuple();

  man->CreateNtuple("fFoil1Forward","Antiprotons Transmitted through Foil 1");
  man->CreateNtupleDColumn("x_mm");
  man->CreateNtupleDColumn("y_mm");
  man->CreateNtupleDColumn("z_mm");
  man->CreateNtupleDColumn("px_keV");
  man->CreateNtupleDColumn("py_keV");
  man->CreateNtupleDColumn("pz_keV");
  man->CreateNtupleDColumn("kineticEnergy_keV");
  man->CreateNtupleDColumn("time_ns");
  man->FinishNtuple();

  man->CreateNtuple("fFoil2Backward","Antiprotons Reflected from Foil 2");
  man->CreateNtupleDColumn("x_mm");
  man->CreateNtupleDColumn("y_mm");
  man->CreateNtupleDColumn("z_mm");
  man->CreateNtupleDColumn("px_keV");
  man->CreateNtupleDColumn("py_keV");
  man->CreateNtupleDColumn("pz_keV");
  man->CreateNtupleDColumn("kineticEnergy_keV");
  man->CreateNtupleDColumn("time_ns");
  man->FinishNtuple();

  man->CreateNtuple("fFoil2Forward","Antiprotons Transmitted through Foil 2");
  man->CreateNtupleDColumn("x_mm");
  man->CreateNtupleDColumn("y_mm");
  man->CreateNtupleDColumn("z_mm");
  man->CreateNtupleDColumn("px_keV");
  man->CreateNtupleDColumn("py_keV");
  man->CreateNtupleDColumn("pz_keV");
  man->CreateNtupleDColumn("kineticEnergy_keV");
  man->CreateNtupleDColumn("time_ns");
  man->FinishNtuple();

  man->CreateNtuple("fHVReflected","Antiprotons Reflected from HV Barrier at the end of the trap");
  man->CreateNtupleDColumn("x_mm");
  man->CreateNtupleDColumn("y_mm");
  man->CreateNtupleDColumn("z_mm");
  man->CreateNtupleDColumn("px_keV");
  man->CreateNtupleDColumn("py_keV");
  man->CreateNtupleDColumn("pz_keV");
  man->CreateNtupleDColumn("kineticEnergy_keV");
  man->CreateNtupleDColumn("time_ns");
  man->FinishNtuple();

  man->CreateNtuple("fHVTransmitted","Antiprotons Transmitted through HV Barrier at the end of the trap");
  man->CreateNtupleDColumn("x_mm");
  man->CreateNtupleDColumn("y_mm");
  man->CreateNtupleDColumn("z_mm");
  man->CreateNtupleDColumn("px_keV");
  man->CreateNtupleDColumn("py_keV");
  man->CreateNtupleDColumn("pz_keV");
  man->CreateNtupleDColumn("kineticEnergy_keV");
  man->CreateNtupleDColumn("time_ns");
  man->FinishNtuple();

  man->CreateNtuple("fTrapped","Antiprotons at the position of the First HV Barrier");
  man->CreateNtupleDColumn("x_mm");
  man->CreateNtupleDColumn("y_mm");
  man->CreateNtupleDColumn("z_mm");
  man->CreateNtupleDColumn("px_keV");
  man->CreateNtupleDColumn("py_keV");
  man->CreateNtupleDColumn("pz_keV");
  man->CreateNtupleDColumn("kineticEnergy_keV");
  man->CreateNtupleDColumn("time_ns");
  man->FinishNtuple();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

AEgISRunAction::~AEgISRunAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void AEgISRunAction::BeginOfRunAction(const G4Run*)
{ 
  //inform the runManager to save random number seed
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);
  auto man = G4AnalysisManager::Instance();

  G4String filename = "man_output.root";
  man->OpenFile(filename);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void AEgISRunAction::EndOfRunAction(const G4Run *)
{
  auto man = G4AnalysisManager::Instance();

  man->Write();
  man->CloseFile();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

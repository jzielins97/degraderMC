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
/// \file B2RunAction.cc
/// \brief Implementation of the B2RunAction class

#include "B2RunAction.hh"

#include "G4Run.hh"
#include "G4RunManager.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B2RunAction::B2RunAction()
 : G4UserRunAction()
{
  fAnnihilationEvents = 0;
  fKilledEvents = 0;
  fNormalEvents = 0;
  // set printing event number per each 100 events
  G4RunManager::GetRunManager()->SetPrintProgress(1000);
  auto man = G4AnalysisManager::Instance();
  man->SetNtupleMerging(true);

  man->SetFirstNtupleId(1);

  man->CreateNtuple("fPosition","Antiproton Position in mm");
  man->CreateNtupleDColumn("x_mm");
  man->CreateNtupleDColumn("y_mm");
  man->CreateNtupleDColumn("z_mm");
  man->FinishNtuple();

  man->CreateNtuple("fMomentum","Antiproton Momentum in keV");
  man->CreateNtupleDColumn("px_keV");
  man->CreateNtupleDColumn("py_keV");
  man->CreateNtupleDColumn("pz_keV");
  man->CreateNtupleDColumn("kineticEnergy_keV");
  man->FinishNtuple();

  man->CreateNtuple("fRunSummary","Summary of the events in the run");
  man->CreateNtupleIColumn("annihilations");
  man->CreateNtupleIColumn("normal");
  man->CreateNtupleIColumn("killed");
  man->FinishNtuple();
  
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

B2RunAction::~B2RunAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2RunAction::BeginOfRunAction(const G4Run*)
{ 
  //inform the runManager to save random number seed
  G4RunManager::GetRunManager()->SetRandomNumberStore(false);
  auto man = G4AnalysisManager::Instance();

  G4String filename = "man_output.root";
  man->OpenFile(filename);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2RunAction::EndOfRunAction(const G4Run *)
{
  G4cout << "Run finished with:"<<G4endl;
  G4cout << "Normal events:"<<fNormalEvents<<G4endl;
  G4cout << "Killed events:"<<fKilledEvents<<G4endl;
  G4cout << "Annihilation events:"<<fAnnihilationEvents<<G4endl;
  auto man = G4AnalysisManager::Instance();

  man->FillNtupleIColumn(3,0,fAnnihilationEvents);
  man->FillNtupleIColumn(3,1,fNormalEvents);
  man->FillNtupleIColumn(3,2,fKilledEvents);
  man->AddNtupleRow(3);

  man->Write();
  man->CloseFile();

}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2RunAction::AbortEvent(){
  G4RunManager::GetRunManager()->AbortEvent();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2RunAction::AnnihilationEvent(){
  fAnnihilationEvents+=1;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2RunAction::KilledEvent(){
  fKilledEvents+=1;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void B2RunAction::NormalEvent(){
  fNormalEvents+=1;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

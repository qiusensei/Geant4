#include <iostream>

#include "G4RunManager.hh"
#include "G4UImanager.hh"
#include "G4VisManager.hh"
#include "G4VisExecutive.hh"
#include "G4UIExecutive.hh"

#include "construction.hh"
#include "physics.hh"
#include "action.hh"
#include "generator.hh"

int main(int argc, char** argv)
{
    G4RunManager *runmanager = new G4RunManager();
    
    runmanager->SetUserInitialization(new MyDetectorConstruction());
    runmanager->SetUserInitialization(new MyPhysicsList());
    runmanager->SetUserInitialization(new MyActionInitialization());


    runmanager->Initialize();

    G4UIExecutive *ui = new G4UIExecutive(argc,argv);
    
    G4VisManager *visManager = new G4VisExecutive();
    visManager->Initialize();

    G4UImanager *UImanager = G4UImanager::GetUIpointer();

    UImanager->ApplyCommand("/vis/open OGL");
    UImanager->ApplyCommand("/vis/viewer/set/viewpointVector 1 1 1");
    UImanager->ApplyCommand("/vis/drawVolume");
    UImanager->ApplyCommand("/vis/viewer/set/autoRefresh true");
    UImanager->ApplyCommand("/vis/scene/add/trajectories smooth");

    ui->SessionStart();

    return 0;
}
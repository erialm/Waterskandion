
#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#include "G4Threading.hh"
#else
#include "G4RunManager.hh"
#endif

#include "globals.hh"
#include "G4UImanager.hh"
#include "Randomize.hh"

#include "DetectorConstruction.hh"
#include "G4GenericPhysicsList.hh"
#include "ActionInitialization.hh"
#include "PhysicsList.hh"
#include "G4tgrMessenger.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif
#include "Shielding.hh"

//=================================================================================

int main(int argc,char** argv)
{
  
  G4Random::setTheEngine(new CLHEP::RanecuEngine);
  
  // Construct the default run manager
#ifdef G4MULTITHREADED
  
  G4MTRunManager* runManager = new G4MTRunManager;
  G4int nthreads=G4Threading::G4GetNumberOfCores();
  runManager->SetNumberOfThreads(nthreads);
  G4cout << "\n\n\tRunning in multithreaded mode with " << nthreads 
         << " threads\n\n" << G4endl;
#else
  G4RunManager* runManager = new G4RunManager;
  G4cout << "\n\n\tRunning in serial mode\n\n" << G4endl; 
#endif
  
    
 // Set user action classes
 runManager->SetUserInitialization(new DetectorConstruction()); 
 runManager->SetUserInitialization(new PhysicsList());
 runManager->SetUserInitialization(new ActionInitialization("../../INPUTDATA/Plan.txt"));
   
  
#ifdef G4VIS_USE
  // visualisation manager
  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();
#endif
  
  
  G4UImanager* UImanager = G4UImanager::GetUIpointer();
  
  
  if (argc==1)
    {
      runManager->Initialize();
#ifdef G4UI_USE
      G4UIExecutive* ui = new G4UIExecutive(argc, argv);
#ifdef G4VIS_USE
      UImanager->ApplyCommand("/control/execute vis.mac");
#endif
      ui->SessionStart();
      delete ui;
#endif
    }
  else
    {
      G4String command = "/control/execute ";
      G4String fileName = argv[1];
      G4int NoProtons=runManager->GetNumberOfEventsToBeProcessed();
      G4cout << "Running a total number of : " << NoProtons << " protons" << G4endl;
      UImanager->ApplyCommand(command+fileName);
      runManager->Initialize();
      runManager->BeamOn(NoProtons);
    }
  
  delete runManager;
  
#ifdef G4VIS_USE
  delete visManager;
#endif
  
  return 0;
}

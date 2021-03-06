
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
//#include "PhysicsList.hh"
#include "G4tgrMessenger.hh"

#ifdef G4VIS_USE
#include "G4VisExecutive.hh"
#endif

#ifdef G4UI_USE
#include "G4UIExecutive.hh"
#endif
#include "Shielding.hh"
#include "G4EmParameters.hh"
#include "G4PhysListFactory.hh"
#include "G4VModularPhysicsList.hh"
#include <limits>
#include <csignal>

//=================================================================================
#ifdef G4MULTITHREADED
  void SignalHandling(G4int Signal)
  {
          G4cout << "Interrupt signal " << Signal << " recieved.\n";
          G4MTRunManager::GetMasterRunManager()->AbortRun(true);
  }
#else
  void SignalHandling(G4int Signal)
  {
          G4cout << "Interrupt signal " << Signal << " recieved.\n";
          G4RunManager::GetRunManager()->AbortRun(true);
          delete RunManager;
  }
#endif

int main(int argc,char** argv)
{
  
  G4Random::setTheEngine(new CLHEP::RanecuEngine);
  
  // Construct the default run manager
#ifdef G4MULTITHREADED
  
  G4MTRunManager* runManager = new G4MTRunManager;
  G4int NoThreads=G4Threading::G4GetNumberOfCores();
  runManager->SetNumberOfThreads(NoThreads);
  G4cout << "\n\n\tRunning in multithreaded mode with " << NoThreads
         << " threads\n\n" << G4endl;

#else
  G4RunManager* runManager = new G4RunManager;
  G4cout << "\n\n\tRunning in serial mode\n\n" << G4endl; 
#endif
  
    
 // Set user action classes
// runManager->SetUserInitialization(new PhysicsList());
 G4PhysListFactory factory;
 G4VModularPhysicsList* physicsList = factory.GetReferencePhysList("QGSP_INCLXX_EMZ");
 runManager->SetUserInitialization(physicsList);
 runManager->SetUserInitialization(new DetectorConstruction()); 
 runManager->SetUserInitialization(new ActionInitialization("../../INPUTDATA/Plan.txt"));
   
 std::signal(SIGINT, SignalHandling);  
#ifdef G4VIS_USE
  // visualisation manager
  G4VisManager* visManager = new G4VisExecutive;
  visManager->Initialize();
#endif
  
  
  G4UImanager* UImanager = G4UImanager::GetUIpointer();
  
  
  if (argc==1)
    {
      //runManager->Initialize();
#ifdef G4UI_USE
       G4UIExecutive* ui = new G4UIExecutive(argc, argv);
#ifdef G4VIS_USE
      UImanager->ApplyCommand("/control/execute setupvis.mac");
#endif
      ui->SessionStart();
      delete ui;
#endif
    }
  else
    {
      G4String command = "/control/execute ";
      G4String fileName = argv[1];
      G4EmParameters* EM = G4EmParameters::Instance();
      EM->SetNumberOfBinsPerDecade(100);
      UImanager->ApplyCommand(command+fileName);
      runManager->Initialize();
      physicsList->SetCutValue(10.*m,"e+");
      physicsList->SetCutValue(10.*m,"e-");
      runManager->BeamOn(std::numeric_limits<G4int>::max());
    }
  
  delete runManager;
  
#ifdef G4VIS_USE
  delete visManager;
#endif
  
  return 0;
}

#include "RunActionMessenger.hh"
#include "RunAction.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWithADouble.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunActionMessenger::RunActionMessenger(
                           RunAction* TheRun)
 : G4UImessenger(),
   fRunAction{TheRun},
   fTopDirectory{nullptr},
   fSetDepthCmd{nullptr},
   fSetToleranceCmd{nullptr}
{ 
    fTopDirectory = new G4UIdirectory("/Uncertainty/");
    fTopDirectory->SetGuidance("UI for RunAction messenger.");
  
  
    fSetDepthCmd = new G4UIcmdWithADouble("/Uncertainty/SetDepth",this);
    fSetDepthCmd->SetGuidance("Set depth for uncertainty calculation (cm).");
    fSetDepthCmd->SetParameterName("Depth",false);
    fSetDepthCmd->AvailableForStates(G4State_PreInit,G4State_Init);
  
    fSetToleranceCmd = new G4UIcmdWithADouble("/Uncertainty/SetTolerance",this);
    fSetToleranceCmd->SetGuidance("Set tolerance for uncertainty caluclation (%).");
    fSetToleranceCmd->SetParameterName("Tolerance",false);
    fSetToleranceCmd->AvailableForStates(G4State_PreInit,G4State_Init);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

RunActionMessenger::~RunActionMessenger()
{
    delete fTopDirectory;
    delete fSetDepthCmd;
    delete fSetToleranceCmd;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void RunActionMessenger::SetNewValue(G4UIcommand* command, 
                                                     G4String newValue)
{ 
    if (command == fSetDepthCmd) 
    { 
        G4double Depth = fSetDepthCmd->GetNewDoubleValue(newValue);
        fRunAction->SetDepth(Depth);
    }
    else if (command == fSetToleranceCmd) 
    {	
        G4double Tolerance = fSetToleranceCmd->GetNewDoubleValue(newValue);
        fRunAction->SetTolerance(Tolerance);
    }
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

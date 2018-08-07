#ifndef RunActionMessenger_h
#define RunActionMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class RunAction;
class G4UIdirectory;
class G4UIcmdWithADouble;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


class RunActionMessenger: public G4UImessenger
{
  public:
    RunActionMessenger(RunAction* );
    virtual ~RunActionMessenger();
    
    virtual void SetNewValue(G4UIcommand* command, G4String newValue);
    
  private:
    RunAction*                 fRunAction;
    G4UIdirectory*             fTopDirectory;
    G4UIcmdWithADouble*	       fSetDepthCmd;
    G4UIcmdWithADouble*	       fSetToleranceCmd;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif



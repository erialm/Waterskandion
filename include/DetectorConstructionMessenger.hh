
#ifndef DetectorConstructionMessenger_h
#define DetectorConstructionMessenger_h 1

#include "G4UImessenger.hh"
#include "globals.hh"

class DetectorConstruction;
class G4UIdirectory;
class G4UIcmdWith3Vector;
class G4UIcmdWithADouble;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


class DetectorConstructionMessenger: public G4UImessenger
{
  public:
    DetectorConstructionMessenger(DetectorConstruction* );
    virtual ~DetectorConstructionMessenger();
    
    virtual void SetNewValue(G4UIcommand* command, G4String newValue);
    
  private:
    DetectorConstruction*      fDetectorConstruction;
    G4UIdirectory*             fTopPhantomDir;
    G4UIcmdWith3Vector*        fSetDimensionsCmd;
    G4UIcmdWithADouble*	       fSetIsoDepthCmd;
    G4UIcmdWith3Vector*	       fSetVoxelSizeCmd;
    G4UIdirectory*             fTopSnoutDir;
    G4UIcmdWithADouble*	       fSetSnoutPositionCmd;
    
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif


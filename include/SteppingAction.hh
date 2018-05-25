#ifndef SteppingAction_h
#define SteppingAction_h 1

#include "G4UserSteppingAction.hh"
#include "globals.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolume.hh"
#include "RunAction.hh"
class DetectorConstruction;

class TrackingAction;

class G4VPhysicalVolume;

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

class SteppingAction : public G4UserSteppingAction
{
  public:
    SteppingAction(RunAction*);
   ~SteppingAction();

    virtual void UserSteppingAction(const G4Step*);
  private:
    G4LogicalVolume* Voxel;
    RunAction* TheRun;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif


#ifndef EventAction_h
#define EventAction_h 1

#include "G4UserEventAction.hh"
#include "G4ThreeVector.hh"
#include <vector>
#include "globals.hh"

class RunAction;
class EventAction : public G4UserEventAction
{
  public:
    EventAction(RunAction* runAction);
    virtual ~EventAction();

    virtual void BeginOfEventAction(const G4Event* event);
    virtual void EndOfEventAction(const G4Event* event);
    void AddStepDose(G4int, G4int, G4int, G4double);
    G4int GetZNum() const {return ZNum;}
  private:
    RunAction* TheRun;
    G4double*** EventArray;
    G4int XNum,YNum,ZNum;
    std::vector<G4ThreeVector> DosePositions;
};

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#endif

    

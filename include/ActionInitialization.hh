#ifndef ActionInitialization_h
#define ActionInitialization_h 1

#include "G4VUserActionInitialization.hh"
#include "RunAction.hh"
#include "globals.hh"


class ActionInitialization : public G4VUserActionInitialization
{
  public:
    ActionInitialization(G4String);
    virtual ~ActionInitialization();
    
    virtual void BuildForMaster() const;
    virtual void Build() const;
  private:
    RunAction* TheRun;
};


#endif


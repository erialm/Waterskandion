#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include <vector>
#include "ProtonPlan.hh"
#include "BeamModel.hh"
#include "G4Run.hh"
#include "G4ThreeVector.hh"
class RunAction : public G4UserRunAction
{
  public:
    RunAction(G4String);
    virtual ~RunAction();

    virtual void BeginOfRunAction(const G4Run*);
    virtual void   EndOfRunAction(const G4Run*);

    ProtonPlan* GetThePlan() const {return ThePlan;}
    BeamModel* GetTheModel() const {return TheModel;}
    void AddDose(G4int, G4int, G4int, G4double) noexcept;
 private:
    static constexpr G4double MeV2J=1.60217662e-13;
    G4double TotalNoProtons;
    G4double SimFraction;
    ProtonPlan* ThePlan;
    BeamModel* TheModel;
    G4double NormFactor;
    G4double**** DoseSpectrum; 	//4D array; (x,y,z,D)
};

#endif

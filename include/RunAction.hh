#ifndef RunAction_h
#define RunAction_h 1

#include "G4UserRunAction.hh"
#include "globals.hh"
#include <vector>
#include "ProtonPlan.hh"
#include "BeamModel.hh"
#include "G4Run.hh"
#include "G4ThreeVector.hh"
#include "EventAction.hh"
#include "RunActionMessenger.hh"
class RunAction : public G4UserRunAction
{
  public:
    RunAction(G4String);
    virtual ~RunAction();

    virtual void BeginOfRunAction(const G4Run*);
    virtual void   EndOfRunAction(const G4Run*);

    ProtonPlan* GetThePlan() const {return ThePlan;}
    BeamModel* GetTheModel() const {return TheModel;}
    void AddEventDose(const G4int, const G4int, const G4int, const G4double, const G4bool);
    G4int GetXNum() const {return XNum;}
    G4int GetYNum() const {return YNum;}
    G4int GetZNum() const {return ZNum;}
    void SetTolerance(G4double Tolerance) {UncTol=Tolerance;}
    void SetDepth(G4double Depth) {UncDepth=Depth;}
    void ComputeCurrentUncertainty();
 private:
    static constexpr G4double MeV2J=1.60217662e-13;
    ProtonPlan* ThePlan;
    BeamModel* TheModel;
    G4double TotalNoProtons;
    G4double SimFraction;
    G4double NormFactor;
    G4double**** DoseSpectrum; 	//4D array; (x,y,z,D)
    G4int XNum,YNum,ZNum;
    G4int UncX, UncY, UncZ;
    G4double UncDepth;
    G4double UncTol;
    G4int N;
    RunActionMessenger Messenger;
};

#endif

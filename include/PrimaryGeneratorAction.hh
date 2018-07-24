#ifndef PrimaryGeneratorAction_h
#define PrimaryGeneratorAction_h 1

#include "G4VUserPrimaryGeneratorAction.hh"
#include "G4ParticleGun.hh"
#include "globals.hh"
#include "ProtonPlan.hh"
#include "BeamModel.hh"
#include <vector>
#include "RunAction.hh"
#include <array>
class G4Event;

class PrimaryGeneratorAction : public G4VUserPrimaryGeneratorAction
{
  public:
    PrimaryGeneratorAction();
    virtual ~PrimaryGeneratorAction();
    virtual void GeneratePrimaries(G4Event*);
    struct StartParameters {
	G4double X;
	G4double Theta;

	G4double Y;
	G4double Phi;

	G4double Z;
	G4double Psi;

	G4double Energy;
    };
    std::array<G4int,2> SampleSpot();
    void SampleSpotParameters(G4int, G4int);
  private:
    static constexpr G4double Pi=3.14159265359;
    G4ParticleGun* TheParticleGun;
    ProtonPlan* ThePlan;
    BeamModel* TheModel;
    StartParameters SampledParameters;
};
#endif

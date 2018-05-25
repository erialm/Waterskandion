#ifndef PhysicsList_h
#define PhysicsList_h 1

#include "G4VModularPhysicsList.hh"
#include "globals.hh"
#include "G4SystemOfUnits.hh"
class G4VPhysicsConstructor;



class PhysicsList: public G4VModularPhysicsList
{
public:
  PhysicsList();
 ~PhysicsList();

  //virtual void ConstructParticle();

  //virtual void ConstructProcess();

  virtual void SetCuts();

};


#endif


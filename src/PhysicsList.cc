#include "PhysicsList.hh"
#include "G4HadronPhysicsINCLXX.hh"
#include "G4EmStandardPhysics_option4.hh"

PhysicsList::PhysicsList()
{
  RegisterPhysics(new G4HadronPhysicsINCLXX());		//Add INCLXX model for hadronic interactions
  RegisterPhysics(new G4EmStandardPhysics_option4());	//add EMZ/EM option 4 model for EM interactions
}
PhysicsList::~PhysicsList()
{
}
void PhysicsList::SetCuts()
{
        SetCutValue(10*m,"e-");
        SetCutValue(10*m,"e+");
}


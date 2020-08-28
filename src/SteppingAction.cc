
#include "SteppingAction.hh"
#include "DetectorConstruction.hh"

#include "G4Step.hh"
#include "G4Track.hh"
#include "G4TrackStatus.hh"
#include "G4Event.hh"
#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif
#include "G4LogicalVolume.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4TouchableHandle.hh"
#include "G4Material.hh"
#include "RunAction.hh"
#include "G4ParticleDefinition.hh"
#include "G4Electron.hh"
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::SteppingAction(EventAction* TheEvent)
: G4UserSteppingAction(), Voxel{nullptr}, Event{TheEvent}
{
    G4LogicalVolumeStore* LogicStore=G4LogicalVolumeStore::GetInstance();
	Voxel=LogicStore->GetVolume("Voxel");
}

//....oooOO0OOo§oo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

SteppingAction::~SteppingAction()
{}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void SteppingAction::UserSteppingAction(const G4Step* step)
{
	G4LogicalVolume* CurrentVolume=step->GetTrack()->GetVolume()->GetLogicalVolume();
	if (CurrentVolume!=Voxel) return;	//Don't do anything if particle is not in target volume
	G4TouchableHandle Touchable = step->GetPreStepPoint()->GetTouchableHandle();
    G4int x=Touchable->GetReplicaNumber(2);
	G4int y=Touchable->GetReplicaNumber(1);
	G4int z=Touchable->GetReplicaNumber(0);
    G4int ZNum=Event->GetZNum();
    z=(ZNum-1)-z;
	G4double EnergyDeposition=step->GetTotalEnergyDeposit();
	Event->AddStepDose(x,y,z,EnergyDeposition);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


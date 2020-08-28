
#include "DetectorConstructionMessenger.hh"
#include "DetectorConstruction.hh"

#include "G4UIdirectory.hh"
#include "G4UIcmdWith3Vector.hh"
#include "G4UIcmdWithADouble.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstructionMessenger::DetectorConstructionMessenger(
                           DetectorConstruction* detectorConstruction)
 : G4UImessenger(),
   fDetectorConstruction{detectorConstruction},
   fTopPhantomDir{nullptr},
   fSetDimensionsCmd{nullptr},
   fSetIsoDepthCmd{nullptr},
   fSetVoxelSizeCmd{nullptr},
   fTopSnoutDir{nullptr},
   fSetSnoutPositionCmd{nullptr}
   
   
{ 
    fTopPhantomDir = new G4UIdirectory("/TargetPhantom/");
    fTopPhantomDir->SetGuidance("UI for Detectorconstruction messenger");
  

    fSetDimensionsCmd = new G4UIcmdWith3Vector("/TargetPhantom/SetTargetDimensions",this);
    fSetDimensionsCmd->SetGuidance("Set dimensions (x,y,z) of the target phantom (cm).");
    fSetDimensionsCmd->SetParameterName("TargetX", "TargetY","TargetZ",false);
    fSetDimensionsCmd->AvailableForStates(G4State_PreInit);
  
    fSetIsoDepthCmd = new G4UIcmdWithADouble("/TargetPhantom/SetIsoDepth",this);
    fSetIsoDepthCmd->SetGuidance("Set isodepth for the target phantom (cm).");
    fSetIsoDepthCmd->SetParameterName("IsoDepth",false);
    fSetIsoDepthCmd->AvailableForStates(G4State_PreInit);
  
    fSetVoxelSizeCmd = new G4UIcmdWith3Vector("/TargetPhantom/SetVoxelSize",this);
    fSetVoxelSizeCmd->SetGuidance("Set voxel size (mm).");
    fSetVoxelSizeCmd->SetParameterName("VoxelSizeX","VoxelSizeY","VoxelSizeZ",false);
    fSetVoxelSizeCmd->AvailableForStates(G4State_PreInit);
  
    fTopSnoutDir = new G4UIdirectory("/Snout/");
    fTopSnoutDir->SetGuidance("UI for Detectorconstruction messenger");
  
    fSetSnoutPositionCmd = new G4UIcmdWithADouble("/Snout/SetSnoutPosition",this);
    fSetSnoutPositionCmd->SetGuidance("Set snout position (cm).");
    fSetSnoutPositionCmd->SetParameterName("SnoutPosition",false);
    fSetSnoutPositionCmd->AvailableForStates(G4State_PreInit);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
DetectorConstructionMessenger::~DetectorConstructionMessenger()
{
    delete fTopPhantomDir;
    delete fSetDimensionsCmd;
    delete fSetIsoDepthCmd;
    delete fSetVoxelSizeCmd;
    delete fTopSnoutDir;
    delete fSetSnoutPositionCmd;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void DetectorConstructionMessenger::SetNewValue(G4UIcommand* command, G4String newValue)
{ 
    if (command == fSetDimensionsCmd) 
    { 
        G4ThreeVector Dimensions = fSetDimensionsCmd->GetNew3VectorValue(newValue);
        fDetectorConstruction->SetTargetDimensions(Dimensions.x(), Dimensions.y(), Dimensions.z());
    }
    else if (command == fSetIsoDepthCmd) 
    { 	
        G4double IsoDepth = fSetIsoDepthCmd->GetNewDoubleValue(newValue);
        fDetectorConstruction->SetIsoDepth(IsoDepth);
    }
    else if (command == fSetVoxelSizeCmd) 
    {	
        G4ThreeVector VoxelSize = fSetVoxelSizeCmd->GetNew3VectorValue(newValue);
        fDetectorConstruction->SetVoxelSize(VoxelSize.x(),VoxelSize.y(),VoxelSize.z());
    }
    else if (command == fSetSnoutPositionCmd) 
    {	
        G4double SnoutPosition = fSetSnoutPositionCmd->GetNewDoubleValue(newValue);
        fDetectorConstruction->SetSnoutPosition(SnoutPosition*cm);
    }
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

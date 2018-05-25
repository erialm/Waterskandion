
#include "DetectorConstruction.hh"
#include "G4SystemOfUnits.hh"
#include "G4Region.hh"
#include "G4ProductionCuts.hh"
#include "G4UserLimits.hh"
#include "G4PVReplica.hh"
#include "G4PVParameterised.hh"
#include "NestedParameterisation.hh"
#include <stdexcept>
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::DetectorConstruction(G4double TX, G4double TY, G4double TZ, G4double PZ, G4double VX, G4double VY, G4double VZ)
:G4VUserDetectorConstruction(), TargetDimensions{TX,TY,TZ}, Messenger{this}, IsoDepth{PZ}, 
VoxelSize{VX,VY,VZ},NoVoxelsX{0},NoVoxelsY{0},NoVoxelsZ{0}
{}  

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

DetectorConstruction::~DetectorConstruction()
{}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
G4VPhysicalVolume* DetectorConstruction::Construct()
{
  G4NistManager * man = G4NistManager::Instance();
  G4Material * H2O = man->FindOrBuildMaterial("G4_WATER");
  G4Material * Air = man->FindOrBuildMaterial("G4_AIR");


  //ISOCENTER AT WORLD ORIGIN

  // WORLD VOLUME
  G4double worldSizeX  = TargetDimensions.x()*2*cm;
  G4double worldSizeY  = TargetDimensions.y()*2*cm;
  G4double worldSizeZ  = 150*cm;	//fixed depth; should be enough for proton transport from nozzle exit	

  G4VSolid* solidWorld = new G4Box("World",         //its name
                          worldSizeX/2,
                          worldSizeY/2,
                          worldSizeZ/2);  //its size

  G4LogicalVolume* logicWorld = new G4LogicalVolume(solidWorld,  //its solid
                                    Air,  //its material
                                    "World");    //its name

  G4VPhysicalVolume* physiWorld = new G4PVPlacement(0,      //no rotation
                                  G4ThreeVector(),  //at (0,0,0)
                                  "World",    //its name
                                  logicWorld,    //its logical volume
                                  nullptr,      //its mother  volume
                                  false,      //no boolean operation
                                  0);      //copy number

  G4double ChargedCutWorld=10*m;
  G4UserLimits* WorldLimiter=new G4UserLimits;
  WorldLimiter->SetUserMinRange(ChargedCutWorld);
  logicWorld->SetUserLimits(WorldLimiter);

  G4Box* targetSolid = new G4Box("Target",              
                                 TargetDimensions.x()/2*cm,
                                 TargetDimensions.y()/2*cm,
                                 TargetDimensions.z()/2*cm);   

  G4LogicalVolume* logicTarget =
      new G4LogicalVolume(targetSolid,  
                          H2O,  
                          "Target");    


  G4double TargetZPos=TargetDimensions.z()/2*cm-IsoDepth*cm;	//position Target so as to extend IsoDepth cm beyond world Z=0
  new G4PVPlacement(0,  
			    G4ThreeVector(0,0,TargetZPos),  
                    "Target", 
                    logicTarget,  
                    physiWorld,  
                    false,        
                    0);           


  using std::runtime_error;
  if (std::trunc((TargetDimensions.x()*cm)/(VoxelSize.x()*mm))==(TargetDimensions.x()*cm)/(VoxelSize.x()*mm)) 
	NoVoxelsX=(TargetDimensions.x()*cm)/(VoxelSize.x()*mm);
  else
	throw runtime_error("Error: non-integer number of X voxels! Exiting...");
  if (std::trunc((TargetDimensions.y()*cm)/(VoxelSize.y()*mm))==(TargetDimensions.y()*cm)/(VoxelSize.y()*mm))
        NoVoxelsY=(TargetDimensions.y()*cm)/(VoxelSize.y()*mm);
  else
	throw runtime_error("Error: non-integer number of Y voxels! Exiting...");
  if (std::trunc((TargetDimensions.z()*cm)/(VoxelSize.z()*mm))==(TargetDimensions.z()*cm)/(VoxelSize.z()*mm))
	NoVoxelsZ=(TargetDimensions.z()*cm)/(VoxelSize.z()*mm);
  else
	throw runtime_error("Error: non-integer number of Z voxels! Exiting...");

  size_t NoVoxels=NoVoxelsX*NoVoxelsY*NoVoxelsZ;
  if (!NoVoxels) throw runtime_error("Error: non-positive number of voxels! Exiting...");

  
  G4VSolid* solidXReplicas = 
  new G4Box("XReplicas",VoxelSize.x()/2.,TargetDimensions.y()/2.*cm,TargetDimensions.z()/2.*cm);
  G4LogicalVolume* logicXReplicas = new G4LogicalVolume(solidXReplicas,H2O,"XReplicas");
  new G4PVReplica("XReplicas",logicXReplicas,logicTarget,kXAxis,NoVoxelsX,VoxelSize.x());

  G4VSolid* solidYReplicas = new G4Box("YReplicas",VoxelSize.x()/2.,VoxelSize.y()/2.,TargetDimensions.z()/2.*cm);
  G4LogicalVolume* logicYReplicas = new G4LogicalVolume(solidYReplicas,H2O,"YReplicas");
  new G4PVReplica("YReplicas",logicYReplicas,logicXReplicas,kYAxis,NoVoxelsY,VoxelSize.y());


  G4VSolid* solidVoxel = new G4Box("Voxel",VoxelSize.x()/2.,VoxelSize.y()/2.,VoxelSize.z()/2.);
  G4LogicalVolume* logicVoxel = new G4LogicalVolume(solidVoxel,H2O,"Voxel");
  VoxelMass=logicVoxel->GetMass()/kg; //convert to kg immediately
  

  NestedParameterisation* Parameterisation = new NestedParameterisation(VoxelSize,NoVoxelsZ,H2O,TargetDimensions.z());

  new G4PVParameterised("PhantomVoxel",      // their name
                          logicVoxel,        // their logical volume
                          logicYReplicas,    // Mother logical volume
                          kUndefined,        // Are placed along this axis 
                          NoVoxelsZ,         // Number of cells
                          Parameterisation);  // Parameterisation.

  return physiWorld;
}

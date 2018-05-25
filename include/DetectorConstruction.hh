
#ifndef DetectorConstruction_h
#define DetectorConstruction_h 1

#include "G4VUserDetectorConstruction.hh"
#include "G4VPhysicalVolume.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4Material.hh"
#include "G4NistManager.hh"
#include "G4PVPlacement.hh"
#include "G4UserLimits.hh"
#include "G4VisAttributes.hh"
#include "DetectorConstructionMessenger.hh"
#include "G4SystemOfUnits.hh"

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo....

class DetectorMessenger;

class DetectorConstruction : public G4VUserDetectorConstruction
{
public:

  DetectorConstruction(G4double TX=10*cm,G4double TY=10*cm, G4double TZ=10*cm, G4double IsoDepth=0*cm,
		       G4double VX=2*mm,G4double VY=2*mm,G4double VZ=2*mm);

  virtual ~DetectorConstruction();

  virtual G4VPhysicalVolume* Construct();

  void SetTargetDimensions(G4double Tx, G4double Ty, G4double Tz) {TargetDimensions=G4ThreeVector{Tx,Ty,Tz};}
  void SetIsoDepth(G4double Z){IsoDepth=Z;}
  void SetVoxelSize(G4double Vx, G4double Vy, G4double Vz) {VoxelSize=G4ThreeVector{Vx,Vy,Vz};}
  G4int GetNoVoxelsX() const {return NoVoxelsX;}
  G4int GetNoVoxelsY() const {return NoVoxelsY;}
  G4int GetNoVoxelsZ() const {return NoVoxelsZ;}
  G4double GetVoxelMass() const {return VoxelMass;} 
                         
private:
  DetectorConstructionMessenger Messenger;
  G4ThreeVector      TargetDimensions;
  G4ThreeVector	     VoxelSize;
  G4double 	     IsoDepth;
  G4double	     VoxelMass;
  G4int 	     NoVoxelsX,NoVoxelsY,NoVoxelsZ;
  
};
#endif

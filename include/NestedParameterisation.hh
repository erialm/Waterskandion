#ifndef NESTEDPARAMETERISATION_HH
#define NESTEDPARAMETERISATION_HH

#include "G4Types.hh"
#include "G4VNestedParameterisation.hh"
#include "G4ThreeVector.hh"
#include <vector>
#include "G4Material.hh"
#include "G4VPhysicalVolume.hh"
#include "G4VTouchable.hh"
#include "globals.hh"
#include "G4Box.hh"
class NestedParameterisation: public G4VNestedParameterisation
{
  public:  
    NestedParameterisation(G4ThreeVector,G4int,G4Material*,G4double DimZ);
   ~NestedParameterisation();

    G4Material* ComputeMaterial(G4VPhysicalVolume *currentVol,
                                const G4int repNo,
                                const G4VTouchable *parentTouch=0
                                        );
    G4int GetNumberOfMaterials() const;
    G4Material* GetMaterial(G4int idx) const;
    void ComputeTransformation(const G4int no,G4VPhysicalVolume *currentPV) const;
    void ComputeDimensions(G4Box &,const G4int,const G4VPhysicalVolume *) const;

  private:
    G4ThreeVector VoxelSize;
    G4int NoZVoxels;
    std::vector<G4double> ZPositions;
    G4Material* Material;
    G4double ZLength;
  void ComputeDimensions (G4Trd&,const G4int,const G4VPhysicalVolume*)
    const {}
  void ComputeDimensions (G4Trap&,const G4int,const G4VPhysicalVolume*)
    const {}
  void ComputeDimensions (G4Cons&,const G4int,const G4VPhysicalVolume*)
    const {}
  void ComputeDimensions (G4Sphere&,const G4int,const G4VPhysicalVolume*)
    const {}
  void ComputeDimensions (G4Orb&,const G4int,const G4VPhysicalVolume*)
    const {}
  void ComputeDimensions (G4Ellipsoid&,const G4int,const G4VPhysicalVolume*)
    const {}
  void ComputeDimensions (G4Torus&,const G4int,const G4VPhysicalVolume*)
    const {}
  void ComputeDimensions (G4Para&,const G4int,const G4VPhysicalVolume*)
    const {}
  void ComputeDimensions (G4Hype&,const G4int,const G4VPhysicalVolume*)
    const {}
  void ComputeDimensions (G4Tubs&,const G4int,const G4VPhysicalVolume*)
    const {}
  void ComputeDimensions (G4Polycone&,const G4int,const G4VPhysicalVolume*)
    const {}
  void ComputeDimensions (G4Polyhedra&,const G4int,const G4VPhysicalVolume*)
    const {}


};
#endif

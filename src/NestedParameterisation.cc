#include "NestedParameterisation.hh"
#include "G4SystemOfUnits.hh"
NestedParameterisation::NestedParameterisation(G4ThreeVector VoxS,G4int ZNum,G4Material* Mat, G4double DimZ):G4VNestedParameterisation(),Material{Mat},NoZVoxels{ZNum},VoxelSize{VoxS}, ZLength{DimZ}
{
	ZPositions.clear();
	ZPositions.resize(NoZVoxels);
	for (size_t i=0;i<NoZVoxels;++i)
	{
		 ZPositions[i]=(-ZLength*5+VoxelSize.z()/2+VoxelSize.z()*i);
	}
}
NestedParameterisation::~NestedParameterisation()
{
}

G4Material* NestedParameterisation::ComputeMaterial(G4VPhysicalVolume*, const G4int,const G4VTouchable*)
{
	return Material;
}

G4int NestedParameterisation::GetNumberOfMaterials() const
{
  	return 1;
}

G4Material* NestedParameterisation::GetMaterial(G4int) const
{
  	return Material;
}
void NestedParameterisation::ComputeTransformation(const G4int copyNo, G4VPhysicalVolume* physVol) const
{
  	G4ThreeVector position(0.,0.,ZPositions[copyNo]);
  	physVol->SetTranslation(position);
}
void NestedParameterisation::ComputeDimensions(G4Box& box, const G4int, const G4VPhysicalVolume* ) const{

  	box.SetXHalfLength(VoxelSize.x()/2.);
  	box.SetYHalfLength(VoxelSize.y()/2.);
  	box.SetZHalfLength(VoxelSize.z()/2.);
}

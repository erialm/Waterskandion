#ifndef ProtonPlan_h
#define ProtonPlan_h 1

#include "globals.hh"
#include <vector>
#include <array>
class ProtonPlan
{
public:
	ProtonPlan(G4String);
	struct Spot
	{
		G4double X;
		G4double Y;	
		G4double Weight;
		G4double GantryAngle;
	};
	struct CDF
	{
		G4int LayerNumber;
		G4int SpotNumber;
		G4double Prob;
	};
	void PrintEnergies() const {for (size_t i=0;i<EnergyVector.size();++i) G4cout << EnergyVector[i] << G4endl;}
	G4int GetNoLayers() const {return SpotVector.size();}
	G4int GetNoSpots(G4int i) const {return SpotVector[i].size();}
	G4double GetSpotX(G4int LayerNum, G4int SpotNum) const {return SpotVector[LayerNum][SpotNum].X;}
	G4double GetSpotY(G4int LayerNum, G4int SpotNum) const {return SpotVector[LayerNum][SpotNum].Y;}
	G4double GetSpotWeight(G4int LayerNum, G4int SpotNum) const {return SpotVector[LayerNum][SpotNum].Weight;}
	G4double GetLayerEnergy(G4int LayerNum) const {return EnergyVector[LayerNum];} 
	G4double GetGantryAngle(G4int LayerNum) const {return GantryVector[LayerNum];}
	G4int GetCDFLayer(G4int i) const {return PlanCDF[i].LayerNumber;}
	G4int GetCDFSpot(G4int i) const {return PlanCDF[i].SpotNumber;}
	G4double GetCDFProb(G4int i) const {return PlanCDF[i].Prob;}
	G4int GetCDFSize() const {return PlanCDF.size();}
	G4double GetNoProtons() const {return TotalNoProtons;}
	void SetUpCDF();
private:
	G4int GTR;
	std::vector< std::vector<Spot>> SpotVector;		
	std::vector<G4double> EnergyVector;
	std::vector<G4double> GantryVector;
	std::vector<CDF> PlanCDF;
	static constexpr char LayerMarker='L';
	G4double TotalNoProtons;
};
#endif

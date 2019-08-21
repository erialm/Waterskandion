#ifndef BeamModel_h
#define BeamModel_h 1

#include "globals.hh"
#include "ProtonPlan.hh"
#include <cmath>
#include <vector>
#include "G4ThreeVector.hh"
class BeamModel 
{
public:
    BeamModel(ProtonPlan*);
    struct OpticalParameters {
	G4double A0X;
	G4double A1X;
	G4double A2X;

	G4double A0Y;
	G4double A1Y;
	G4double A2Y;
    };
    struct BaseSpotParameters {
	G4ThreeVector Position;
	G4ThreeVector Direction;
    };
    void ComputeStartingEnergies();	
    void ComputePrimaryOpticalParameters();	
    void ComputeSecondaryOpticalParameters();	
    void ComputeBaseSpotParameters();
    void ComputeEnergySpread();
    
    G4double GetBaseStartingEnergy(G4int LayerNum) const {return SimStartEnergies[LayerNum];}
    G4double GetA0X(G4int LayerNum) const {return Optical[LayerNum].A0X;}
    G4double GetA1X(G4int LayerNum) const {return Optical[LayerNum].A1X;}
    G4double GetA2X(G4int LayerNum) const {return Optical[LayerNum].A2X;}
    
    G4double GetSecA0X(G4int LayerNum) const {return OpticalSec[LayerNum].A0X;}
    G4double GetSecA1X(G4int LayerNum) const {return OpticalSec[LayerNum].A1X;}
    G4double GetSecA2X(G4int LayerNum) const {return OpticalSec[LayerNum].A2X;}

    G4double GetA0Y(G4int LayerNum) const {return Optical[LayerNum].A0Y;}
    G4double GetA1Y(G4int LayerNum) const {return Optical[LayerNum].A1Y;}
    G4double GetA2Y(G4int LayerNum) const {return Optical[LayerNum].A2Y;}
    
    G4double GetSecA0Y(G4int LayerNum) const {return OpticalSec[LayerNum].A0Y;}
    G4double GetSecA1Y(G4int LayerNum) const {return OpticalSec[LayerNum].A1Y;}
    G4double GetSecA2Y(G4int LayerNum) const {return OpticalSec[LayerNum].A2Y;}

    G4double GetBaseX(G4int LayerNum, G4int SpotNum) const {return BaseSpot[LayerNum][SpotNum].Position.getX();}
    G4double GetBaseTheta(G4int LayerNum, G4int SpotNum) const {return BaseSpot[LayerNum][SpotNum].Direction.getX();}
    G4double GetBaseY(G4int LayerNum, G4int SpotNum) const {return BaseSpot[LayerNum][SpotNum].Position.getY();}
    G4double GetBasePhi(G4int LayerNum, G4int SpotNum) const {return BaseSpot[LayerNum][SpotNum].Direction.getY();}
    G4double GetBaseZ(G4int LayerNum, G4int SpotNum) const {return BaseSpot[LayerNum][SpotNum].Position.getZ();}
    G4double GetBasePsi(G4int LayerNum, G4int SpotNum) const {return BaseSpot[LayerNum][SpotNum].Direction.getZ();}
    G4double GetNozzleExit() const {return NozzleExit;}
    G4double GetEnergySpread(G4int LayerNum) const {return EnergySpreads[LayerNum];}
    G4double GetSecondaryWeight(G4int LayerNum) const {return Weights[LayerNum];}

private:
    static constexpr G4double NozzleExit=538.91;
    static constexpr G4double SADX=2203;
    static constexpr G4double SADY=1830;

    std::vector<G4double> SimStartEnergies;
    std::vector<OpticalParameters> Optical;
    std::vector<OpticalParameters> OpticalSec;
    std::vector<std::vector<BaseSpotParameters>> BaseSpot;
    std::vector<G4double> EnergySpreads;
    std::vector<G4double> Weights;

    ProtonPlan* ThePlan;
};
#endif

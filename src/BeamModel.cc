#include "BeamModel.hh"
#include <fstream>
#include "c2_factory.hh"
#define PI 3.14159265359
static c2_factory<G4double> c2; 
typedef c2_ptr<G4double> c2p;

BeamModel::BeamModel(ProtonPlan* Plan)
: ThePlan{Plan}
{
	G4int NoLayers=ThePlan->GetNoLayers();
	BaseSpot.resize(NoLayers);
	for (G4int i=0;i<NoLayers;++i)
	{
		BaseSpot[i].resize(ThePlan->GetNoSpots(i));	
	}
	ComputeStartingEnergies();	
	ComputeEnergySpread();
	ComputeOpticalParameters();	
	ComputeBaseSpotParameters();
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void BeamModel::ComputeStartingEnergies()
{
	std::ifstream EnergyFile{"../../INPUTDATA/RoundedBestEnergies.txt"};
	if (!EnergyFile) throw std::runtime_error("Could not open energies data file!");
	
	std::vector<G4double> NozzleEnergy;
	std::vector<G4double> IsoEnergy;
	G4double EnergyEntry;
	while (EnergyFile>>EnergyEntry)
	{
		NozzleEnergy.push_back(EnergyEntry);
		IsoEnergy.push_back(std::floor(EnergyEntry));
	}

	c2p Iso2NozzleExit=c2.interpolating_function().load(IsoEnergy,NozzleEnergy,true,0,true,0,true);
	for (G4int i=0;i<ThePlan->GetNoLayers();++i) SimStartEnergies.push_back(Iso2NozzleExit(ThePlan->GetLayerEnergy(i)));

}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void BeamModel::ComputeEnergySpread()
{
	using std::pow;
	G4double E,EnergySpread;
	for (G4int i=0;i<ThePlan->GetNoLayers();++i)
	{
		E=SimStartEnergies[i];
		EnergySpread=-3.0147e-07*pow(E,3)+7.5117e-05*pow(E,2)+1.2467e-03*E+2.0982e-01;
		EnergySpreads.push_back(EnergySpread);
	}
	
		
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void BeamModel::ComputeOpticalParameters()
{

	//THIS USES INTERPOLATION OF THE PARAMETERS, RATHER THAN FITTING; CHANGE AT SOME POINT OR KEEP?
	using std::vector;
	std::ifstream ParametersFile{"../../INPUTDATA/MergedPara.txt"};
	if (!ParametersFile) throw std::runtime_error("Could not open optical parameters data file!");
	G4double EEnt,A0XEnt,A1XEnt,A2XEnt,A0YEnt,A1YEnt,A2YEnt;
	vector<G4double> Energy;
	vector<G4double> A0X;	//A0 = variance of angular distribution
	vector<G4double> A1X;	//A1 = correlation of angular and spatial distributions
	vector<G4double> A2X;	//A2 = variance of spatial distribution
	vector<G4double> A0Y;
	vector<G4double> A1Y;
	vector<G4double> A2Y;

	while(ParametersFile>>EEnt>>A0XEnt>>A1XEnt>>A2XEnt>>A0YEnt>>A1YEnt>>A2YEnt)
	{
		Energy.push_back(EEnt);		//MeV at nozzle exit
		A0X.push_back(A0XEnt);	//rad
		A1X.push_back(A1XEnt);	//mm*rad
		A2X.push_back(A2XEnt);	//mm	
		A0Y.push_back(A0YEnt);
		A1Y.push_back(A1YEnt);
		A2Y.push_back(A2YEnt);
		
	}

	c2p A0XInt=c2.interpolating_function().load(Energy, A0X,true,0,true,0,false);//false=linear interpolation
	c2p A1XInt=c2.interpolating_function().load(Energy, A1X,true,0,true,0,false);
	c2p A2XInt=c2.interpolating_function().load(Energy, A2X,true,0,true,0,false);
	c2p A0YInt=c2.interpolating_function().load(Energy, A0Y,true,0,true,0,false);
	c2p A1YInt=c2.interpolating_function().load(Energy, A1Y,true,0,true,0,false);
	c2p A2YInt=c2.interpolating_function().load(Energy, A2Y,true,0,true,0,false);


	OpticalParameters Entry;
	G4double LayerEnergy;
	for (G4int i=0;i<ThePlan->GetNoLayers();++i)
	{
		LayerEnergy=SimStartEnergies[i];	
		Entry.A0X=A0XInt(LayerEnergy);
		Entry.A1X=A1XInt(LayerEnergy);
		Entry.A2X=A2XInt(LayerEnergy);
		Entry.A0Y=A0YInt(LayerEnergy);
		Entry.A1Y=A1YInt(LayerEnergy);
		Entry.A2Y=A2YInt(LayerEnergy);
		Optical.push_back(Entry);	
	}
	
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void BeamModel::ComputeBaseSpotParameters()
{
	//FIXED COORDINATE SYSTEM: LOOKING TOWARD THE GANTRY, AT GANTRY ANGLE 0, X AXIS POINTS TO THE RIGHT, Y TOWARD THE GANTRY AND Z UPWARDS IN THE OPPOSITE DIRECTION OF THE BEAM	
	//GANTRY ROTATES CLOCKWISE
	using std::cos;
	using std::sin;
	using std::abs;
	G4double GantryAngle, KY, Y;
	G4ThreeVector SourceX, IsoX, DiffX;
	for (G4int i=0;i<ThePlan->GetNoLayers();++i)
	{
		GantryAngle=ThePlan->GetGantryAngle(i)*PI/180; //convert to radians
		SourceX=G4ThreeVector{SADX*sin(GantryAngle),0,SADX*cos(GantryAngle)}; //Position at virtual source position X in fixed coordinates
		for (G4int j=0;j<ThePlan->GetNoSpots(i);++j)
		{					
			IsoX=G4ThreeVector{ThePlan->GetSpotX(i,j)*cos(GantryAngle),0,ThePlan->GetSpotX(i,j)*sin(-GantryAngle)}; //Fixed X coordinate at isoplane
			DiffX=G4ThreeVector{(IsoX.x()-SourceX.x())/SADX,(IsoX.y()-SourceX.y())/SADX,(IsoX.z()-SourceX.z())/SADX};
			BaseSpot[i][j].Position.setX(SourceX.x()-DiffX.x()*(NozzleExit-SADX)); //X-position at nozzle exit
			BaseSpot[i][j].Position.setZ(SourceX.z()-DiffX.z()*(NozzleExit-SADX)); //Z-position at nozzle exit
			Y=ThePlan->GetSpotY(i,j); //Fixed Y coordinate at isoplane
			KY=Y/SADY;
			BaseSpot[i][j].Position.setY(KY*(SADY-NozzleExit)); //Y-position at nozzle exit

			BaseSpot[i][j].Direction.setX(DiffX.x()); //X-direction
			BaseSpot[i][j].Direction.setY(KY); //Y-direction
			BaseSpot[i][j].Direction.setZ(DiffX.z()); //Z-direction		
		}
	}
}

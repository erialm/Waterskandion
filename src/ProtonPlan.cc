#include "ProtonPlan.hh"
#include <stdexcept>
#include <fstream>
#include <array>
#include "c2_factory.hh"
static c2_factory<G4double> c2;
typedef c2_ptr<G4double> c2p;
ProtonPlan::ProtonPlan(G4String FilePath) : TotalNoProtons{0.}
{
	std::ifstream PlanFile{FilePath};
	if (!PlanFile) throw std::runtime_error("Could not open proton plan file!");
	char CharEntry;
	double Energy, GantryAngle;
	double XPos, YPos, SpotWeight; //spot entry
	
	Spot aSpot;
	G4int LayerCounter=-1;
	G4String GantryString;
	G4int GantryNumber;
	PlanFile>>GantryString>>GantryNumber;
	GTR=GantryNumber;
	while (PlanFile>>CharEntry)
	{	
		if (CharEntry==LayerMarker) 
		{
			++LayerCounter;
			PlanFile>>Energy>>GantryAngle;
			SpotVector.push_back(std::vector<Spot> (0));
			EnergyVector.push_back(Energy);
			GantryVector.push_back(GantryAngle);
			continue;
		}
		else PlanFile.putback(CharEntry);
		
		if (PlanFile>>XPos>>YPos>>SpotWeight)
		{
			aSpot.X=XPos;
			aSpot.Y=YPos;
			aSpot.Weight=SpotWeight;	
			SpotVector[LayerCounter].push_back(aSpot);
		}
		
	}
	SetUpCDF();
}
void ProtonPlan::SetUpCDF()
{
	using std::array;
	using std::vector;
	using std::ifstream;

	G4String ProtonPerMUFile;
	if (GTR==1) ProtonPerMUFile="../../INPUTDATA/ProtonsPerMUGTR1.txt";
	else if (GTR==2) ProtonPerMUFile="../../INPUTDATA/ProtonsPerMUGTR2.txt";
	//else Throw exception!!
	ifstream ProtonsPerMU{ProtonPerMUFile};	
	if (!ProtonsPerMU) throw std::runtime_error("Could not open Protons per MU data file!");


	vector<G4double> Energies;
	vector<G4double> NumProtons;
	G4double Energy, NoProton;
	while (ProtonsPerMU>>Energy>>NoProton)
	{
		Energies.push_back(Energy);
		NumProtons.push_back(NoProton);
	}
	c2p NoProtonsInt=c2.interpolating_function().load(Energies,NumProtons,true,0,true,0,false);


	G4double Weight=0, NoProtons=0;
	CDF InputCDF;
	for (G4int i=0;i<GetNoLayers();++i)
	{
		Energy=GetLayerEnergy(i);
		for (G4int j=0;j<GetNoSpots(i);++j)
		{
			Weight=GetSpotWeight(i,j);
			NoProtons=Weight*NoProtonsInt(Energy);
			TotalNoProtons+=NoProtons;
			InputCDF.LayerNumber=i;
			InputCDF.SpotNumber=j;
			InputCDF.Prob=TotalNoProtons;
			PlanCDF.push_back(InputCDF);	
		}	
	}
	for (size_t i=0;i<PlanCDF.size();++i) PlanCDF[i].Prob/=TotalNoProtons;		
}


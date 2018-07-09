#include "RunAction.hh"
#include "PrimaryGeneratorAction.hh"
#include "DetectorConstruction.hh"

#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif
#include "G4Run.hh"
#include "G4AccumulableManager.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4HEPEvtInterface.hh"
#include "G4AutoLock.hh"
#include "G4UnitsTable.hh"
#include "G4SystemOfUnits.hh"
#include "globals.hh"
#include <fstream>
#include <cmath>
#include "G4NistManager.hh"
#include "G4Proton.hh"
#include "G4ParticleDefinition.hh"
#include <stdexcept>
#include <limits>
#define BINS 2
#define RATIO 1000

namespace { G4Mutex SteppingMutexLock = G4MUTEX_INITIALIZER; }
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
RunAction::RunAction(G4String PlanPath)
: G4UserRunAction(), ThePlan{new ProtonPlan(PlanPath)}, TheModel{new BeamModel(ThePlan)}, TotalNoProtons{0.}, SimFraction{0.}
{
	#ifdef G4MULTITHREADED	
	G4MTRunManager* RunManager=G4MTRunManager::GetMasterRunManager();
	#else
	G4RunManager* RunManager=G4RunManager::GetRunManager();
	#endif
	G4double NoPlanProtons=ThePlan->GetNoProtons(); 
	G4int SimProtons=NoPlanProtons/RATIO; //simulate approximately 1/RATIO of the real number of protons; narrowing to G4int from G4double because SetNumberOfEventsToBeProcessed method requires it
	NormFactor=NoPlanProtons/SimProtons;
	RunManager->SetNumberOfEventsToBeProcessed(SimProtons);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void RunAction::AddDose(G4int x, G4int y, G4int z,G4double D) noexcept
{
	G4AutoLock lock(&SteppingMutexLock);
	DoseSpectrum[x][y][z][0]+=D;	//Dose
	DoseSpectrum[x][y][z][1]+=D*D; //Dose squared
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
RunAction::~RunAction()
{
	delete ThePlan;
	delete TheModel;
	delete[] DoseSpectrum;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void RunAction::BeginOfRunAction(const G4Run*)
{
	

	#ifdef G4MULTITHREADED	
	G4MTRunManager* RunManager=G4MTRunManager::GetMasterRunManager();
	#else
	G4RunManager* RunManager=G4RunManager::GetRunManager();
	#endif
	const DetectorConstruction* Detector = static_cast<const DetectorConstruction*> (RunManager->GetUserDetectorConstruction()); //get DetectorConstruction; need to recast since RunManager will return G4VUserDetectorConstruction pointer only		
	size_t XVoxNum=Detector->GetNoVoxelsX();
	size_t YVoxNum=Detector->GetNoVoxelsY();
	size_t ZVoxNum=Detector->GetNoVoxelsZ();
	DoseSpectrum = new G4double***[XVoxNum];
	for (size_t i=0;i<XVoxNum;++i)
	{
		DoseSpectrum[i]= new G4double**[YVoxNum];
		for (size_t j=0;j<YVoxNum;++j)
		{
			DoseSpectrum[i][j]=new G4double*[ZVoxNum];
			for (size_t k=0;k<ZVoxNum;++k) 
			{
				DoseSpectrum[i][j][k]=new G4double[BINS]; //TotalEnergyDep,TotalEnergyDep^2
				for (size_t l=0;l<BINS;++l) DoseSpectrum[i][j][k][l]=0.;
			}
		}
	}
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void RunAction::EndOfRunAction(const G4Run*)
{
	using std::ofstream;
	using std::pow;
	using std::sqrt;
	G4RunManager* RunManager=G4RunManager::GetRunManager();
	const DetectorConstruction* Detector = static_cast<const DetectorConstruction*> (RunManager->GetUserDetectorConstruction());
	G4int XVoxNum=Detector->GetNoVoxelsX();
	G4int YVoxNum=Detector->GetNoVoxelsY();
	G4int ZVoxNum=Detector->GetNoVoxelsZ();

	ofstream DoseMatrix{"../../OUTPUTDATA/DoseMatrix.dat",ofstream::binary};
	ofstream DoseUncertainty{"../../OUTPUTDATA/DoseUncertainty.dat",ofstream::binary};
	DoseMatrix.write(reinterpret_cast<char*>(&XVoxNum),sizeof(G4int));
	DoseMatrix.write(reinterpret_cast<char*>(&YVoxNum),sizeof(G4int));
	DoseMatrix.write(reinterpret_cast<char*>(&ZVoxNum),sizeof(G4int));
	
	DoseUncertainty.write(reinterpret_cast<char*>(&XVoxNum),sizeof(G4int));
	DoseUncertainty.write(reinterpret_cast<char*>(&YVoxNum),sizeof(G4int));
	DoseUncertainty.write(reinterpret_cast<char*>(&ZVoxNum),sizeof(G4int));
        
	G4double S, DoseSquare, VoxelDose=-1, VoxelMass=0;
        G4int N=ThePlan->GetNoProtons()/RATIO;
	VoxelMass=Detector->GetVoxelMass();
	for (G4int i=(ZVoxNum-1);i>=0;--i) //start from negative since IEC is negative in the beam direction
	{
		for (G4int j=0;j<YVoxNum;++j)
		{
			for (G4int k=0;k<XVoxNum;++k)
			{	
				VoxelDose=DoseSpectrum[k][j][i][0];
				DoseSquare=DoseSpectrum[k][j][i][1];
                                S=(DoseSquare-pow(VoxelDose,2)/N)/(N-1);
                                S=sqrt(S)/sqrt(N);	//standard deviation of the mean energy deposition;
				S=N*S*MeV2J/VoxelMass*NormFactor; //Standard deviation of the total voxel dose
				
	
				VoxelDose*=MeV2J/VoxelMass*NormFactor;
				DoseMatrix.write(reinterpret_cast<char*>(&VoxelDose),sizeof(G4double));
				if (VoxelDose>0) S=S/VoxelDose*100;
                                else S=std::numeric_limits<G4double>::quiet_NaN();
                                DoseUncertainty.write(reinterpret_cast<char*>(&S),sizeof(G4double));
			}
		}
	}
}




//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

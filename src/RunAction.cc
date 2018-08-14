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
#define FREQUENCY 10000
#define NOSIGMA 2
namespace { G4Mutex SteppingMutexLock = G4MUTEX_INITIALIZER; }
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
RunAction::RunAction(G4String PlanPath)
: G4UserRunAction(), ThePlan{new ProtonPlan(PlanPath)}, TheModel{new BeamModel(ThePlan)}, TotalNoProtons{0.}, SimFraction{0.}, NormFactor(0.), DoseSpectrum{nullptr},
UncX{0}, UncY{0}, UncZ{0},UncDepth{0}, UncTol{0.}, N{0}, Messenger{this}
{
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
        G4cout << "Running until uncertainty is below " << UncTol << "% at approximately " << UncDepth << " cm depth." << G4endl;  
	#ifdef G4MULTITHREADED	
	G4MTRunManager* RunManager=G4MTRunManager::GetMasterRunManager();
	#else
	G4RunManager* RunManager=G4RunManager::GetRunManager();
	#endif
	const DetectorConstruction* Detector = static_cast<const DetectorConstruction*> (RunManager->GetUserDetectorConstruction()); //get DetectorConstruction; need to recast since RunManager will return G4VUserDetectorConstruction pointer only		
	XNum=Detector->GetNoVoxelsX();
	YNum=Detector->GetNoVoxelsY();
	ZNum=Detector->GetNoVoxelsZ();
	DoseSpectrum = new G4double***[XNum];
	for (G4int i=0;i<XNum;++i)
	{
		DoseSpectrum[i]= new G4double**[YNum];
		for (G4int j=0;j<YNum;++j)
		{
			DoseSpectrum[i][j]=new G4double*[ZNum];
			for (G4int k=0;k<ZNum;++k) 
			{
				DoseSpectrum[i][j][k]=new G4double[BINS]; //TotalEnergyDep,TotalEnergyDep^2
				for (G4int l=0;l<BINS;++l) DoseSpectrum[i][j][k][l]=0.;
			}
		}
	}
        UncX=XNum/2; //integer division
        UncY=YNum/2; 
        G4double Distance, MinDistance=std::numeric_limits<G4double>::max();
        using std::abs;
        for (G4int i=0;i<ZNum;++i) 
        {
              Distance=abs(UncDepth*10-Detector->GetDepth(i));
              if (Distance<MinDistance)
              {
                      MinDistance=Distance;
                      UncZ=i;
              }
        }
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void RunAction::AddEventDose(const G4int x, const G4int y, const G4int z, const G4double D, bool LastElement)
{
	G4AutoLock lock(&SteppingMutexLock);
        DoseSpectrum[x][y][z][0]+=D;	//Dose
	DoseSpectrum[x][y][z][1]+=D*D; //Dose squared
        if (LastElement)
        {       ++N;
                if (N%FREQUENCY==0) ComputeCurrentUncertainty(); //when this is called, DoseSpectrum may include data partially transferred by the other NoThreads-1 EventAction classes. Does not matter since the intention is to provide a decent estimate for the number of primaries transported to obtain desired uncertainty.
        }
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void RunAction::ComputeCurrentUncertainty()
{
        #ifdef G4MULTITHREADED	
	G4MTRunManager* RunManager=G4MTRunManager::GetMasterRunManager();
	#else
        G4RunManager* RunManager=G4RunManager::GetRunManager();
	#endif
	const DetectorConstruction* Detector = static_cast<const DetectorConstruction*> (RunManager->GetUserDetectorConstruction());
	G4double VoxelMass=Detector->GetVoxelMass();
	G4double VoxelDose=DoseSpectrum[UncX][UncY][UncZ][0];
	G4double DoseSquare=DoseSpectrum[UncX][UncY][UncZ][1];
        G4double S=(DoseSquare-pow(VoxelDose,2)/N)/(N-1);
        S=sqrt(S)/sqrt(N);	
        NormFactor=ThePlan->GetNoProtons()/N;
        S=N*S*MeV2J/VoxelMass*NormFactor; 
	VoxelDose*=MeV2J/VoxelMass*NormFactor;
        if (VoxelDose>0) S=S*NOSIGMA/VoxelDose*100;
        else S=std::numeric_limits<G4double>::quiet_NaN();
        G4cout << UncX << ' ' << UncY << ' ' << UncZ << ' ' << VoxelDose << '\n';
        G4cout << "Current uncertainty " << S << "% with tolerance " << UncTol << "% during event " << N << G4endl;
        if (S<UncTol) 
        {
                RunManager->AbortRun(true);
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

	ofstream DoseMatrix{"../../OUTPUTDATA/DoseMatrix.dat",ofstream::binary};
	ofstream DoseUncertainty{"../../OUTPUTDATA/DoseUncertainty.dat",ofstream::binary};
	DoseMatrix.write(reinterpret_cast<char*>(&XNum),sizeof(G4int));
	DoseMatrix.write(reinterpret_cast<char*>(&YNum),sizeof(G4int));
	DoseMatrix.write(reinterpret_cast<char*>(&ZNum),sizeof(G4int));
	
	DoseUncertainty.write(reinterpret_cast<char*>(&XNum),sizeof(G4int));
	DoseUncertainty.write(reinterpret_cast<char*>(&YNum),sizeof(G4int));
	DoseUncertainty.write(reinterpret_cast<char*>(&ZNum),sizeof(G4int));
        
	G4double S, DoseSquare, VoxelDose=-1, VoxelMass=0;
        G4cout << "Simulated " << N << " primaries.\n";

        NormFactor=ThePlan->GetNoProtons()/N; //Number of threads -1 additional primaries will be transported after current uncertainty calculation 
	VoxelMass=Detector->GetVoxelMass();
	for (G4int i=0;i<ZNum;++i) //start from negative since IEC is negative in the beam direction
	{
		for (G4int j=0;j<YNum;++j)
		{
			for (G4int k=0;k<XNum;++k)
			{	
				VoxelDose=DoseSpectrum[k][j][i][0];
				DoseSquare=DoseSpectrum[k][j][i][1];
                                S=(DoseSquare-pow(VoxelDose,2)/N)/(N-1);
                                S=sqrt(S)/sqrt(N);	//standard deviation of the mean energy deposition;
				S=N*S*MeV2J/VoxelMass*NormFactor; //Standard deviation of the total voxel dose
				
				VoxelDose*=MeV2J/VoxelMass*NormFactor;
				DoseMatrix.write(reinterpret_cast<char*>(&VoxelDose),sizeof(G4double));
				if (VoxelDose>0) S=S*NOSIGMA/VoxelDose*100;
                                else S=std::numeric_limits<G4double>::quiet_NaN();
                                DoseUncertainty.write(reinterpret_cast<char*>(&S),sizeof(G4double));
			}
		}
	}
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

#include "PrimaryGeneratorAction.hh"
#include "G4ThreeVector.hh"
#include "G4LogicalVolumeStore.hh"
#include "G4LogicalVolume.hh"
#include "G4Box.hh"
#include "G4ParticleGun.hh"
#ifdef G4MULTITHREADED
#include "G4MTRunManager.hh"
#else
#include "G4RunManager.hh"
#endif
#include "G4ParticleTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4SystemOfUnits.hh"
#include "Randomize.hh"
#include "G4Proton.hh"
#include "RunAction.hh"
#include "G4AutoLock.hh"
namespace { G4Mutex GeneratorMutexLock = G4MUTEX_INITIALIZER; }
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::PrimaryGeneratorAction()
: G4VUserPrimaryGeneratorAction(), TheParticleGun{new G4ParticleGun}, ThePlan{nullptr}, TheModel{nullptr}
{
	#ifdef G4MULTITHREADED
	const RunAction* TheRun=static_cast<const RunAction*> (G4MTRunManager::GetMasterRunManager()->GetUserRunAction());
	#else
	const RunAction* TheRun=static_cast<const RunAction*> (G4RunManager::GetRunManager()->GetUserRunAction());
	#endif
	ThePlan=TheRun->GetThePlan();
	TheModel=TheRun->GetTheModel();
  	TheParticleGun->SetParticleDefinition(G4Proton::ProtonDefinition());
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

PrimaryGeneratorAction::~PrimaryGeneratorAction()
{
  	delete TheParticleGun;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void PrimaryGeneratorAction::SampleSpotParameters(G4int LayerNumber, G4int SpotNumber)
{
	using std::exp;
	using std::sqrt;
	using std::pow;
	using std::tan;
	using std::cos;
	using std::tan;
	
	G4double A0X, A1X, A2X, X, Angle, AngleTheta, NTheta, BaseTheta, AnglePsi, BasePsi, NPsi;
	G4double GantryAngle=ThePlan->GetGantryAngle(LayerNumber);
        G4double Weight=TheModel->GetSecondaryWeight(LayerNumber);
        G4double RandNumber=G4RandFlat::shoot(0.,1.);
        if (RandNumber>Weight)
        {       
                A0X=TheModel->GetA0X(LayerNumber);
	        A1X=TheModel->GetA1X(LayerNumber);
	        A2X=TheModel->GetA2X(LayerNumber);
        }
        else
        {
                A0X=TheModel->GetSecA0X(LayerNumber);
                A1X=TheModel->GetSecA1X(LayerNumber);
                A2X=TheModel->GetSecA2X(LayerNumber);
        }
	X=G4RandGauss::shoot(0.,1.);
	Angle=G4RandGauss::shoot(0.,1.);
	Angle=(A1X*X+sqrt(A2X*A0X-pow(A1X,2))*Angle)/sqrt(A2X); //sample from bivariate Gaussian using Cholesky decomposition
	X=sqrt(A2X)*X;

	SampledParameters.X=TheModel->GetBaseX(LayerNumber,SpotNumber)*mm+X*mm*cos(GantryAngle);
	SampledParameters.Z=TheModel->GetBaseZ(LayerNumber,SpotNumber)*mm+X*mm*sin(GantryAngle);
	BaseTheta=TheModel->GetBaseTheta(LayerNumber,SpotNumber);
	BasePsi=TheModel->GetBasePsi(LayerNumber,SpotNumber);	
	AngleTheta=Angle*cos(GantryAngle);
	AnglePsi=Angle*sin(GantryAngle);
	NTheta=(BaseTheta+tan(AngleTheta))/(tan(AngleTheta)*BaseTheta+1);
	NPsi=(BasePsi+tan(AnglePsi))/(tan(AnglePsi)*BasePsi+1);
	SampledParameters.Theta=NTheta;
	SampledParameters.Psi=NPsi;

	G4double A0Y, A1Y, A2Y, Y, Phi, NPhi, BasePhi;
        if (RandNumber>Weight)
	
        {
                A0Y=TheModel->GetA0Y(LayerNumber);
	        A1Y=TheModel->GetA1Y(LayerNumber);
	        A2Y=TheModel->GetA2Y(LayerNumber);
        }
        else
        {
                A0Y=TheModel->GetSecA0Y(LayerNumber);
	        A1Y=TheModel->GetSecA1Y(LayerNumber);
	        A2Y=TheModel->GetSecA2Y(LayerNumber);
        }
	Y=G4RandGauss::shoot(0.,1.);
	Phi=G4RandGauss::shoot(0.,1.);
	Phi=(A1Y*Y+sqrt(A2Y*A0Y-pow(A1Y,2))*Phi)/sqrt(A2Y);
	Y=sqrt(A2Y)*Y;
		
	SampledParameters.Y=TheModel->GetBaseY(LayerNumber,SpotNumber)*mm+Y*mm;
	BasePhi=TheModel->GetBasePhi(LayerNumber,SpotNumber);
	NPhi=(BasePhi+tan(Phi))/(tan(Phi)*BasePhi+1);
	SampledParameters.Phi=NPhi;
	
	G4double EnergySpread=TheModel->GetEnergySpread(LayerNumber);
	G4double NozzleEnergy=TheModel->GetBaseStartingEnergy(LayerNumber);
	G4double E=G4RandGauss::shoot(NozzleEnergy,EnergySpread);
	SampledParameters.Energy=E*MeV;
	
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
std::array<G4int,2> PrimaryGeneratorAction::SampleSpot()
{
	G4int Size=ThePlan->GetCDFSize();
	G4double P;
	G4double R=G4RandFlat::shoot(0.,1.);
	std::array<G4int,2> LayerSpot;
	for (G4int i=0;i<Size;++i)
	{	
		P=ThePlan->GetCDFProb(i);
		if (R<=P)
		{
			LayerSpot[0]=ThePlan->GetCDFLayer(i);
			LayerSpot[1]=ThePlan->GetCDFSpot(i);
			break;
		}
	}
	return LayerSpot;
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void PrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
	G4AutoLock lock(&GeneratorMutexLock);
	std::array<G4int,2> LayerSpot=SampleSpot();
	SampleSpotParameters(LayerSpot[0], LayerSpot[1]);	
	TheParticleGun->SetParticlePosition(G4ThreeVector(SampledParameters.X,SampledParameters.Y,SampledParameters.Z));
	TheParticleGun->SetParticleMomentumDirection((G4ThreeVector(SampledParameters.Theta,SampledParameters.Phi,SampledParameters.Psi)));
	TheParticleGun->SetParticleEnergy(SampledParameters.Energy);
	TheParticleGun->GeneratePrimaryVertex(anEvent);
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......


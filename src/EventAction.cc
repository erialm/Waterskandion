#include "EventAction.hh"
#include "RunAction.hh"
#include "G4Event.hh"
#include "G4RunManager.hh"
#include "DetectorConstruction.hh"
#include "G4MTRunManager.hh"
//#include <algorithm>
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

EventAction::EventAction(RunAction* runAction)
: G4UserEventAction(), TheRun{runAction}, XNum{0},YNum{0},ZNum{0}
{      
	#ifdef G4MULTITHREADED	
	G4MTRunManager* RunManager=G4MTRunManager::GetMasterRunManager();
	#else
	G4RunManager* RunManager=G4RunManager::GetRunManager();
	#endif
        const DetectorConstruction* Detector = static_cast<const DetectorConstruction*> (RunManager->GetUserDetectorConstruction());
	XNum=Detector->GetNoVoxelsX();
	YNum=Detector->GetNoVoxelsY();
	ZNum=Detector->GetNoVoxelsZ();
        EventArray = new G4double**[XNum];
	for (G4int i=0;i<XNum;++i)
	{
		EventArray[i]= new G4double*[YNum];
		for (G4int j=0;j<YNum;++j)
		{
			EventArray[i][j]=new G4double[ZNum];
                        for (G4int k=0;k<ZNum;++k) EventArray[i][j][k]=0.;
		}
	}
        DosePositions.reserve(ZNum*5); //protons from a single event will travel mostly along single Z direction, so thus number of voxels to which dose is deposited should be proportional to number of Z voxels
} 

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void EventAction::AddStepDose(G4int x, G4int y, G4int z, G4double E)
{
        EventArray[x][y][z]+=E;
        G4ThreeVector NewPos=G4ThreeVector(x,y,z); 
        DosePositions.push_back(NewPos);
        //if (!std::find(DosePositions.begin(), DosePositions.end(),NewPos)!=v.end()) DosePositions.push_back(G4ThreeVector(x,y,z)); //probably too slow to go through entire vector for each step
        //if (!DosePositions[DoseDepositions.size()-1]==NewPos) DosePositions.push_back(NewPos);  //check if current step is depositing energy in the last registered position in vector; will reduce number of redundant elements in DosePositions 
        //Probably faster to go through a vector with some redudant elements at the end of an event than go fiddle with vector for each step
        //This might become a problem if secondary electron transport is enabled, since size of DosePosition vector will increase
}
//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
EventAction::~EventAction()
{       
        delete[] EventArray;
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

void EventAction::BeginOfEventAction(const G4Event*)
{    
        DosePositions.clear();
}

//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......
void EventAction::EndOfEventAction(const G4Event*)
{   
        G4int X, Y, Z;
        G4bool LastElement;
        for (size_t i=0;i<DosePositions.size();++i)
        {
                if (i==(DosePositions.size()-1)) 
                {
                        LastElement=true;
                }
                else LastElement=false;
                X=DosePositions[i].x();
                Y=DosePositions[i].y();
                Z=DosePositions[i].z();
                TheRun->AddEventDose(X,Y,Z,EventArray[X][Y][Z],LastElement);
                EventArray[X][Y][Z]=0.;
        }
}


//....oooOO0OOooo........oooOO0OOooo........oooOO0OOooo........oooOO0OOooo......

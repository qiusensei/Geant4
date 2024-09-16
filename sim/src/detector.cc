#include "detector.hh"

MySensitiveDetector::MySensitiveDetector(G4String name) : G4VSensitiveDetector(name)
{}

MySensitiveDetector::~MySensitiveDetector()
{}

G4bool MySensitiveDetector::ProcessHits(G4Step *aStep,G4TouchableHistory *ROhist)
{
    G4Track *track = aStep->GetTrack();

    track->SetTrackStatus(fStopAndKill);

    G4StepPoint *preStepPoint = aStep->GetPreStepPoint();
    G4StepPoint *postStepPoint = aStep->GetPostStepPoint();

    G4ThreeVector posPhoton = preStepPoint->GetPosition();

    //G4cout <<"Photon position:" << posPhoton << G4endl;

    const G4VTouchable *touchable = aStep->GetPreStepPoint()->GetTouchable();

    G4int copyNo = touchable->GetCopyNumber();

    //G4cout << "Copy Number:" << copyNo << G4endl;

    G4VPhysicalVolume *physVol = touchable->GetVolume();
    G4ThreeVector posDetector = physVol->GetTranslation();

    G4cout << "Detector Position:" << posDetector << G4endl;

    G4int evt = G4RunManager::GetRunManager()->GetCurrentEvent()->GetEventID();
        
    G4double photonEnergy;
    G4double photonWavelength;
    G4ThreeVector position;

    // 获取光子能量
    photonEnergy = aStep->GetTrack()->GetKineticEnergy();

    // 计算光子波长 (假设真空)
    const G4double CLIGHT = 2.99792458e+8;
    photonWavelength = CLIGHT / photonEnergy;
    
    G4AnalysisManager *man = G4AnalysisManager::Instance();
    man->FillNtupleIColumn(0,evt);
    man->FillNtupleDColumn(1,photonEnergy);
    man->FillNtupleDColumn(2,photonWavelength);
    man->AddNtupleRow(0);
}
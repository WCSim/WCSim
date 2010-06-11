#include "WCSimRunActionMessenger.hh"

#include "WCSimRunAction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4UIcmdWithAString.hh"

WCSimRunActionMessenger::WCSimRunActionMessenger(WCSimRunAction* WCSimRA)
:WCSimRun(WCSimRA)
{ 
  WCSimIODir = new G4UIdirectory("/WCSimIO/");
  WCSimIODir->SetGuidance("Commands to select I/O options");

  RootFile = new G4UIcmdWithAString("/WCSimIO/RootFile",this);
  RootFile->SetGuidance("Set the root file name");
  RootFile->SetGuidance("Enter the name of the output ROOT file");
  RootFile->SetParameterName("RootFileName",true);
  RootFile->SetDefaultValue("wcsim.root");

}

WCSimRunActionMessenger::~WCSimRunActionMessenger()
{
  delete RootFile;
  delete WCSimIODir;
}

void WCSimRunActionMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{

  if ( command == RootFile)
    {
      WCSimRun->SetRootFileName(newValue);
      G4cout << "Output ROOT file set to " << newValue << G4endl;
    }

}

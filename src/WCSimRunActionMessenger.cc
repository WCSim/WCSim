#include "WCSimRunActionMessenger.hh"

#include "WCSimRunAction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcommand.hh"
#include "G4UIparameter.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"

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

  RooTracker = new G4UIcmdWithABool("/WCSimIO/SaveRooTracker",this);
  RooTracker->SetGuidance("Save the input NEUT Rootracker objects to the output file");
  RooTracker->SetGuidance("Enter a boolean to save or drop the NEUT RooTracker information");
  RooTracker->SetParameterName("SaveRooTracker",false);
  RooTracker->SetDefaultValue(false);

}

WCSimRunActionMessenger::~WCSimRunActionMessenger()
{
  delete RootFile;
  delete RooTracker;
  delete WCSimIODir;
}

void WCSimRunActionMessenger::SetNewValue(G4UIcommand* command,G4String newValue)
{

  if ( command == RootFile)
    {
      WCSimRun->SetRootFileName(newValue);
      G4cout << "Output ROOT file set to " << newValue << G4endl;
    }

  if ( command == RooTracker)
    {
      WCSimRun->SetSaveRooTracker(RooTracker->GetNewBoolValue(newValue));
      if(newValue) G4cout << "Saving NEUT RooTracker information to output file"  << G4endl;
    }

}


#include "WCSimOpticalPhotonMessenger.hh"
#include "WCSimOpticalPhotonTrackInfo.hh"


WCSimOpticalPhotonMessenger::WCSimOpticalPhotonMessenger(WCSimOpticalPhotonTrackInfo* info)
:opInfo(info)
{
  opInfoDir = new G4UIdirectory("/opInfo/");
  
  opEnabled = new G4UIcmdWithABool("/opInfo/Enabled", this);
  opEnabled->SetGuidance("Enables Optical Photon track info tree");
  opEnabled->SetParameterName("opEnabled", true);
  opEnabled->SetDefaultValue(false);
  opEnabled->AvailableForStates(G4State_PreInit, G4State_Idle);
}

WCSimOpticalPhotonMessenger::~WCSimOpticalPhotonMessenger(){
  delete opInfoDir;
  delete opEnabled;
}

void WCSimOpticalPhotonMessenger::SetNewValue(G4UIcommand* command,G4String newValue){

  if(command == opEnabled){
    bool value = opEnabled->GetNewBoolValue(newValue);
    std::cout << "Value is " << value << ". New value is " << newValue << std::endl; 
    if(value){
      opInfo->enable();
    } else {
      opInfo->disable();
    }
  } 
}

#include "WCSimPrimaryGeneratorMessenger.hh"
#include "WCSimPrimaryGeneratorAction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4ios.hh"

WCSimPrimaryGeneratorMessenger::WCSimPrimaryGeneratorMessenger(WCSimPrimaryGeneratorAction* pointerToAction)
:myAction(pointerToAction)
{
  mydetDirectory = new G4UIdirectory("/mygen/");
  mydetDirectory->SetGuidance("WCSim detector control commands.");

  genCmd = new G4UIcmdWithAString("/mygen/generator",this);
  genCmd->SetGuidance("Select primary generator.");
  //T. Akiri: Addition of laser
  genCmd->SetGuidance(" Available generators : muline, normal, laser, radioactive");
  genCmd->SetParameterName("generator",true);
  genCmd->SetDefaultValue("muline");
  //T. Akiri: Addition of laser
  genCmd->SetCandidates("muline normal laser radioactive");

  fileNameCmd = new G4UIcmdWithAString("/mygen/vecfile",this);
  fileNameCmd->SetGuidance("Select the file of vectors.");
  fileNameCmd->SetGuidance(" Enter the file name of the vector file");
  fileNameCmd->SetParameterName("fileName",true);
  fileNameCmd->SetDefaultValue("inputvectorfile");

  isotopeCmd = new G4UIcmdWithAString("/mygen/isotope",this);
  isotopeCmd->SetGuidance("Select radioactive isotpe.");
  isotopeCmd->SetGuidance(" Available isotopes : Tl208, Bi214, K40");
  isotopeCmd->SetParameterName("isotope",true);
  isotopeCmd->SetDefaultValue("Tl208");
  isotopeCmd->SetCandidates("Tl208 Bi214 K40");


}

WCSimPrimaryGeneratorMessenger::~WCSimPrimaryGeneratorMessenger()
{
  delete genCmd;
  delete mydetDirectory;
  delete isotopeCmd;
}

void WCSimPrimaryGeneratorMessenger::SetNewValue(G4UIcommand * command,G4String newValue)
{
  if( command==genCmd )
  {
    if (newValue == "muline")
    {
      myAction->SetMulineEvtGenerator(true);
      myAction->SetNormalEvtGenerator(false);
      myAction->SetLaserEvtGenerator(false);
      myAction->SetRadioactiveEvtGenerator(false);
    }
    else if ( newValue == "normal")
    {
      myAction->SetMulineEvtGenerator(false);
      myAction->SetNormalEvtGenerator(true);
      myAction->SetLaserEvtGenerator(false);
      myAction->SetRadioactiveEvtGenerator(false);
    }
    else if ( newValue == "laser")   //T. Akiri: Addition of laser
    {
      myAction->SetMulineEvtGenerator(false);
      myAction->SetNormalEvtGenerator(false);
      myAction->SetLaserEvtGenerator(true);
      myAction->SetRadioactiveEvtGenerator(false);
    }
    else if ( newValue == "radioactive")
    {
      myAction->SetMulineEvtGenerator(false);
      myAction->SetNormalEvtGenerator(false);
      myAction->SetLaserEvtGenerator(false);
      myAction->SetRadioactiveEvtGenerator(true);
    }
  }

  if( command == fileNameCmd )
  {
    myAction->OpenVectorFile(newValue);
    G4cout << "Input vector file set to " << newValue << G4endl;
  }

  if( command==isotopeCmd )
  {
    if (newValue == "Tl208")
    {
      myAction->SetTl208EvtGenerator(true);
      myAction->SetBi214EvtGenerator(false);
      myAction->SetK40EvtGenerator(false);
    }
    else if ( newValue == "Bi214")
    {
      myAction->SetTl208EvtGenerator(false);
      myAction->SetBi214EvtGenerator(true);
      myAction->SetK40EvtGenerator(false);
    }
    else if ( newValue == "K40")
    {
      myAction->SetTl208EvtGenerator(false);
      myAction->SetBi214EvtGenerator(false);
      myAction->SetK40EvtGenerator(true);
    }
  }

}

G4String WCSimPrimaryGeneratorMessenger::GetCurrentValue(G4UIcommand* command)
{
  G4String cv;
  
  if( command==genCmd )
  {
    if(myAction->IsUsingMulineEvtGenerator())
      { cv = "muline"; }
    else if(myAction->IsUsingNormalEvtGenerator())
      { cv = "normal"; }
    else if(myAction->IsUsingLaserEvtGenerator())
      { cv = "laser"; }   //T. Akiri: Addition of laser
    else if(myAction->IsUsingRadioactiveEvtGenerator())
      { cv = "radioactive"; }
  }


  if( command==isotopeCmd )
  {
    if(myAction->IsUsingTl208EvtGenerator())
      { cv = "Tl208"; }
    else if(myAction->IsUsingBi214EvtGenerator())
      { cv = "Bi214"; }
    else if(myAction->IsUsingK40EvtGenerator())
      { cv = "K40"; }
  }
  
  return cv;
}


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

  radioactive_time_window_Cmd = new G4UIcmdWithADouble("/mygen/radioactive_time_window",this);
  radioactive_time_window_Cmd->SetGuidance("Select time window for radioactivity");
  radioactive_time_window_Cmd->SetParameterName("radioactive_time_window",true);
  radioactive_time_window_Cmd->SetDefaultValue(0.);

  fileNameCmd = new G4UIcmdWithAString("/mygen/vecfile",this);
  fileNameCmd->SetGuidance("Select the file of vectors.");
  fileNameCmd->SetGuidance(" Enter the file name of the vector file");
  fileNameCmd->SetParameterName("fileName",true);
  fileNameCmd->SetDefaultValue("inputvectorfile");

  isotopeCmd = new G4UIcmdWithAString("/mygen/isotope",this);
  isotopeCmd->SetGuidance("Select properties of radioactive isotope");
  isotopeCmd->SetGuidance("[usage] /mygen/isotope ISOTOPE LOCATION ACTIVITY");
  isotopeCmd->SetGuidance("     ISOTOPE : Tl208, Bi214, K40");
  isotopeCmd->SetGuidance("     LOCATION : water PMT");
  isotopeCmd->SetGuidance("     ACTIVITY : (int) activity of isotope (Bq) ");
  G4UIparameter* param;
  param = new G4UIparameter("ISOTOPE",'s',true);
  param->SetDefaultValue("Tl208");
  isotopeCmd->SetParameter(param);
  param = new G4UIparameter("LOCATION",'s',true);
  param->SetDefaultValue("water");
  isotopeCmd->SetParameter(param);
  param = new G4UIparameter("ACTIVITY",'d',true);
  param->SetDefaultValue("0");
  isotopeCmd->SetParameter(param);


}

WCSimPrimaryGeneratorMessenger::~WCSimPrimaryGeneratorMessenger()
{
  delete genCmd;
  delete radioactive_time_window_Cmd;
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
    IsotopeCommand(newValue);
  }

  if( command==radioactive_time_window_Cmd )
    {
      myAction->SetRadioactiveTimeWindow(StoD(newValue));
    }

}

 void  WCSimPrimaryGeneratorMessenger::IsotopeCommand(G4String newValue)
 {

   G4Tokenizer next( newValue );

   G4String isotope = next();
   if (isotope == "Tl208")
     {
       myAction->SetTl208EvtGenerator(true);
       myAction->SetBi214EvtGenerator(false);
       myAction->SetK40EvtGenerator(false);
     }
   else if ( isotope == "Bi214")
     {
       myAction->SetTl208EvtGenerator(false);
       myAction->SetBi214EvtGenerator(true);
       myAction->SetK40EvtGenerator(false);
     }
   else if ( isotope == "K40")
     {
      myAction->SetTl208EvtGenerator(false);
      myAction->SetBi214EvtGenerator(false);
      myAction->SetK40EvtGenerator(true);
     }

   G4String location = next();
   if (location == "water")
     {
       myAction->SetWaterEvtGenerator(true);
       myAction->SetPMTEvtGenerator(false);
     }
   else if (location == "PMT")
     {
       myAction->SetWaterEvtGenerator(false);
       myAction->SetPMTEvtGenerator(true);
     }

   myAction->SetIsotopeActivity(StoD(next()));

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


#include "WCSimPrimaryGeneratorMessenger.hh"
#include "WCSimPrimaryGeneratorAction.hh"
#include "G4UIdirectory.hh"
#include "G4UIcmdWithAString.hh"
#include "G4UIcmdWithABool.hh"
#include "G4UIcmdWithADouble.hh"
#include "G4ios.hh"

WCSimPrimaryGeneratorMessenger::WCSimPrimaryGeneratorMessenger(WCSimPrimaryGeneratorAction* pointerToAction)
:myAction(pointerToAction)
{
  mydetDirectory = new G4UIdirectory("/mygen/");
  mydetDirectory->SetGuidance("WCSim detector control commands.");

  genCmd = new G4UIcmdWithAString("/mygen/generator",this);
  genCmd->SetGuidance("Select primary generator.");
  genCmd->SetGuidance(" Available generators : muline, gun, laser, gps, cosmics, radioactive, rootracker, radon, injector");
  genCmd->SetParameterName("generator",true);
  genCmd->SetDefaultValue("muline");
  genCmd->SetCandidates("muline gun laser gps cosmics radioactive rootracker radon injector");

  fileNameCmd = new G4UIcmdWithAString("/mygen/vecfile",this);
  fileNameCmd->SetGuidance("Select the file of vectors.");
  fileNameCmd->SetGuidance(" Enter the file name of the vector file");
  fileNameCmd->SetParameterName("fileName",true);
  fileNameCmd->SetDefaultValue("inputvectorfile");

  genSet = false;

  //C. Vilela: Adding PMTPoisson for generating photoelectrons directly on PMTs according to a Poisson distribution.
  poisCmd = new G4UIcmdWithABool("/mygen/pmtPoisson",this);
  poisCmd->SetGuidance("Flag for generating photoelectrons directly on PMTs according to a Poisson distribution. These PE's will be generated in addition to light produce by any particles generated. Set dark rate to 0 and do not generate any particles for events with only Poisson PE's.");
  poisCmd->SetGuidance("Set poisson mean with /mygen/poissonMean");
  poisCmd->SetParameterName("pmtPoisson", true);

  poisMeanCmd = new G4UIcmdWithADouble("/mygen/poissonMean",this);
  poisMeanCmd->SetGuidance("Set Poisson mean to be used with /mygen/pmtPoisson. Defaults to 1.");
  poisMeanCmd->SetParameterName("poissonMean", true);
  poisMeanCmd->SetDefaultValue(1);

  fileNameCmdCosmics = new G4UIcmdWithAString("/mygen/cosmicsfile",this);
  fileNameCmdCosmics->SetGuidance("Select the file of cosmics.");
  fileNameCmdCosmics->SetGuidance(" Enter the file name of the cosmics file");
  fileNameCmdCosmics->SetParameterName("fileName",true);
  fileNameCmdCosmics->SetDefaultValue("data/MuonFlux-HyperK-ThetaPhi.dat");

  timeUnitCmd = new G4UIcmdWithAString("/mygen/time_unit",this);
  timeUnitCmd->SetGuidance("Define the units used for time in the input file.");
  timeUnitCmd->SetGuidance("Can be picosecond, ps, ns, nanosecond, ms, millisecond, s, sec or second");
  timeUnitCmd->SetGuidance("Default if not set is nanosecond");
  timeUnitCmd->SetParameterName("unit",true);
  timeUnitCmd->SetDefaultValue("ns");

  radioactive_time_window_Cmd = new G4UIcmdWithADouble("/mygen/radioactive_time_window",this);
  radioactive_time_window_Cmd->SetGuidance("Select time window for radioactivity");
  radioactive_time_window_Cmd->SetParameterName("radioactive_time_window",true);
  radioactive_time_window_Cmd->SetDefaultValue(0.);

  // K.M.Tsui: options for injector events
  nPhotonsCmd = new G4UIcmdWithAnInteger("/mygen/injector_nPhotons",this);
  nPhotonsCmd->SetGuidance("Number of photons emitted for each injector event");
  nPhotonsCmd->SetParameterName("injector_nPhotons",true);
  nPhotonsCmd->SetDefaultValue(1);

  injectorOnCmd = new G4UIcmdWithAnInteger("/mygen/injector_on_index",this);
  injectorOnCmd->SetGuidance("Index of the injector to be turned on");
  injectorOnCmd->SetParameterName("injector_on_index",true);
  injectorOnCmd->SetDefaultValue(0.);

  // not really implemented yet, just a placeholder
  injectorTimeCmd = new G4UIcmdWithADouble("/mygen/injector_time_width",this);
  injectorTimeCmd->SetGuidance("Injector time width");
  injectorTimeCmd->SetParameterName("injector_time_width",true);
  injectorTimeCmd->SetDefaultValue(0.);

  openingAngleCmd = new G4UIcmdWithADouble("/mygen/injector_opening_angle",this);
  openingAngleCmd->SetGuidance("Opening angle of light injector in deg");
  openingAngleCmd->SetParameterName("injector_opening_angle",true);
  openingAngleCmd->SetDefaultValue(0.);

  injectorWavelengthCmd = new G4UIcmdWithADouble("/mygen/injector_wavelength",this);
  injectorWavelengthCmd->SetGuidance("Wavelength of the injector laser in nm");
  injectorWavelengthCmd->SetParameterName("injector_wavelength",true);
  injectorWavelengthCmd->SetDefaultValue(435.);
    
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
  
  radonScalingCmd = new G4UIcmdWithAString("/mygen/radon_scaling",this);
  radonScalingCmd->SetGuidance("Select scalling scenario, if scenario 0 is selected, Bi214 are generated uniformly");
  radonScalingCmd->SetGuidance("[usage] /mygen/radon SCENARIO ");
  radonScalingCmd->SetGuidance("     SCENARIO : 0, A, B");
  radonScalingCmd->SetCandidates("0 A B");
  param = new G4UIparameter("SCENARIO",'s',true);
  param->SetDefaultValue("A");
  radonScalingCmd->SetParameter(param);
  
  radonGeoSymCmd = new G4UIcmdWithAnInteger("/mygen/radon_symmetry",this);
  radonGeoSymCmd->SetGuidance("Select scalling scenario");
  radonGeoSymCmd->SetGuidance("[usage] /mygen/radon SCENARIO ");
  radonGeoSymCmd->SetGuidance("     SYMMETRY : 1 ... ");
  param = new G4UIparameter("SYMMETRY",'d',true);
  param->SetDefaultValue("1");
  radonGeoSymCmd->SetParameter(param);
}

WCSimPrimaryGeneratorMessenger::~WCSimPrimaryGeneratorMessenger()
{
  delete genCmd;
  delete mydetDirectory;
  delete radonScalingCmd;
  delete radonGeoSymCmd;
  delete radioactive_time_window_Cmd;
  delete nPhotonsCmd;
  delete injectorOnCmd;
  delete injectorTimeCmd;
  delete openingAngleCmd;
  delete injectorWavelengthCmd;
}

void WCSimPrimaryGeneratorMessenger::SetNewValue(G4UIcommand * command,G4String newValue)
{
  if( command==genCmd )
  {
    genSet = true;
    if (newValue == "muline")
    {
      myAction->SetMulineEvtGenerator(true);
      myAction->SetGunEvtGenerator(false);
      myAction->SetRootrackerEvtGenerator(false);
      myAction->SetLaserEvtGenerator(false);
      myAction->SetInjectorEvtGenerator(false);
      myAction->SetGPSEvtGenerator(false);
      myAction->SetCosmicsGenerator(false);
      myAction->SetRadioactiveEvtGenerator(false);
      myAction->SetRadonEvtGenerator(false);
    }
    else if ( newValue == "gun")
    {
      myAction->SetMulineEvtGenerator(false);
      myAction->SetGunEvtGenerator(true);
      myAction->SetRootrackerEvtGenerator(false);
      myAction->SetLaserEvtGenerator(false);
      myAction->SetInjectorEvtGenerator(false);
      myAction->SetGPSEvtGenerator(false);
      myAction->SetCosmicsGenerator(false);
      myAction->SetRadioactiveEvtGenerator(false);
      myAction->SetRadonEvtGenerator(false);
    }
    else if ( newValue == "rootracker")
    {
      myAction->SetMulineEvtGenerator(false);
      myAction->SetGunEvtGenerator(false);
      myAction->SetRootrackerEvtGenerator(true);
      myAction->SetLaserEvtGenerator(false);
      myAction->SetInjectorEvtGenerator(false);
      myAction->SetGPSEvtGenerator(false);
      myAction->SetCosmicsGenerator(false);
      myAction->SetRadioactiveEvtGenerator(false);
      myAction->SetRadonEvtGenerator(false);
    }
    else if ( newValue == "laser")   //T. Akiri: Addition of laser
    {
      myAction->SetMulineEvtGenerator(false);
      myAction->SetGunEvtGenerator(false);
      myAction->SetRootrackerEvtGenerator(false);
      myAction->SetLaserEvtGenerator(true);
      myAction->SetInjectorEvtGenerator(false);
      myAction->SetGPSEvtGenerator(false);
      myAction->SetCosmicsGenerator(false);
      myAction->SetRadioactiveEvtGenerator(false);
      myAction->SetRadonEvtGenerator(false);
    }
    else if ( newValue == "injector")   // addition of injector events
    {
      myAction->SetMulineEvtGenerator(false);
      myAction->SetGunEvtGenerator(false);
      myAction->SetRootrackerEvtGenerator(false);
      myAction->SetLaserEvtGenerator(false);
      myAction->SetInjectorEvtGenerator(true);
      myAction->SetGPSEvtGenerator(false);
      myAction->SetCosmicsGenerator(false);
      myAction->SetRadioactiveEvtGenerator(false);
      myAction->SetRadonEvtGenerator(false);
    }
    else if ( newValue == "gps")
    {
      myAction->SetMulineEvtGenerator(false);
      myAction->SetGunEvtGenerator(false);
      myAction->SetRootrackerEvtGenerator(false);
      myAction->SetLaserEvtGenerator(false);
      myAction->SetInjectorEvtGenerator(false);
      myAction->SetGPSEvtGenerator(true);
      myAction->SetCosmicsGenerator(false);
      myAction->SetRadioactiveEvtGenerator(false);
      myAction->SetRadonEvtGenerator(false);
    }
    else if ( newValue == "cosmics")
    {
      myAction->SetMulineEvtGenerator(false);
      myAction->SetGunEvtGenerator(false);
      myAction->SetRootrackerEvtGenerator(false);
      myAction->SetLaserEvtGenerator(false);
      myAction->SetInjectorEvtGenerator(false);
      myAction->SetGPSEvtGenerator(false);
      myAction->SetCosmicsGenerator(true);
      myAction->SetRadioactiveEvtGenerator(false);
      myAction->SetRadonEvtGenerator(false);
    }
    else if ( newValue == "radioactive") //G. Pronost: Addition of Radioactivity (from F. Nova code)
    {
      myAction->SetMulineEvtGenerator(false);
      myAction->SetGunEvtGenerator(false);
      myAction->SetRootrackerEvtGenerator(false);
      myAction->SetLaserEvtGenerator(false);
      myAction->SetInjectorEvtGenerator(false);
      myAction->SetGPSEvtGenerator(false);
      myAction->SetCosmicsGenerator(false);
      myAction->SetRadioactiveEvtGenerator(true);
      myAction->SetRadonEvtGenerator(false);
    }
    else if ( newValue == "radon" ) //G. Pronost: Addition of Radon generator (based on F. Nova's radioactive generator but dedicated to radioactive events in water)
    {
      myAction->SetMulineEvtGenerator(false);
      myAction->SetGunEvtGenerator(false);
      myAction->SetRootrackerEvtGenerator(false);
      myAction->SetLaserEvtGenerator(false);
      myAction->SetInjectorEvtGenerator(false);
      myAction->SetGPSEvtGenerator(false);
      myAction->SetCosmicsGenerator(false);
      myAction->SetRadioactiveEvtGenerator(false);
      myAction->SetRadonEvtGenerator(true);
    }
  }

  if( command == fileNameCmd)
  {
    if(genSet){
        if(myAction->IsUsingRootrackerEvtGenerator()){
            myAction->OpenRootrackerFile(newValue);
        }
        else{
            myAction->OpenVectorFile(newValue);
        }
        G4cout << "Input vector file set to " << newValue << G4endl;
    }
    else{
        G4cout << "Generator has not been set, guessing input vector file is NOT in the Rootracker format - this will crash if you are using a Rootracker input file" << G4endl;
        G4cout << "Please put the '/mygen/generator' command above the '/mygen/vecfile' command in the mac file." << G4endl;
    }
  }
  if(command == fileNameCmdCosmics )
  {
    myAction->OpenCosmicsFile(newValue);
    G4cout << "Input cosmics data file set to " << newValue << G4endl;
  }
  if( command==isotopeCmd )
  {
    IsotopeCommand(newValue);
  }

  if( command==radioactive_time_window_Cmd )
  {
    myAction->SetRadioactiveTimeWindow(StoD(newValue));
  }

  if ( command==radonScalingCmd )
  {
    RadonScalingCommand(newValue);
  }

  if ( command==radonGeoSymCmd )
  {
    myAction->SetRadonSymmetry(radonGeoSymCmd->GetNewIntValue(newValue));
  }

  if ( command==timeUnitCmd)
  {
    myAction->SetTimeUnit(newValue);
    G4cout << "Time unit set to " << newValue << G4endl;
  }

  if( command == poisCmd )
    {
      if ( poisCmd->GetNewBoolValue(newValue) ){
	myAction->SetPoissonPMT(true);
	G4cout << "Running with PoissonPMT flag. Photoelectrons will be generated directly on the PMTs according to a Poisson distribuition. Any hits resulting from physics generated elsewhere will be discarded !!!" << G4endl;
      }
      else myAction->SetPoissonPMT(false);
    }
  
  if( command == poisMeanCmd )
    {
      myAction->SetPoissonPMTMean(poisMeanCmd->GetNewDoubleValue(newValue));
      G4cout << "PoissonPMT mean set to: " << poisMeanCmd->GetNewDoubleValue(newValue) << G4endl;
    }
    
  if( command==radioactive_time_window_Cmd )
    {
      myAction->SetRadioactiveTimeWindow(StoD(newValue));
    }
  
  if ( command==radonScalingCmd ) 
    {
      RadonScalingCommand(newValue);
    }
  
  if ( command==radonGeoSymCmd ) 
    {
      myAction->SetRadonSymmetry(radonGeoSymCmd->GetNewIntValue(newValue));
    }
  
  if ( command==nPhotonsCmd ) 
    {
      myAction->SetInjectorBeamPhotons(nPhotonsCmd->GetNewIntValue(newValue));
    }

  if ( command==injectorOnCmd ) 
    {
      myAction->SetInjectorOnIdx(injectorOnCmd->GetNewIntValue(newValue));
    }
  
  if ( command==injectorTimeCmd ) 
    {
      myAction->SetInjectorTimeWindow(injectorTimeCmd->GetNewDoubleValue(newValue));
    }

  if ( command==openingAngleCmd ) 
    {
      myAction->SetInjectorOpeningAngle(openingAngleCmd->GetNewDoubleValue(newValue));
    }

  if ( command== injectorWavelengthCmd )
    {
      myAction->SetInjectorWavelength(injectorWavelengthCmd->GetNewDoubleValue(newValue));
    }

}

G4String WCSimPrimaryGeneratorMessenger::GetCurrentValue(G4UIcommand* command)
{
  G4String cv;
  
  if( command==genCmd )
  {
    if(myAction->IsUsingMulineEvtGenerator())
      { cv = "muline"; }
    else if(myAction->IsUsingGunEvtGenerator())
      { cv = "gun"; }
    else if(myAction->IsUsingLaserEvtGenerator())
      { cv = "laser"; }   //T. Akiri: Addition of laser
    else if(myAction->IsUsingInjectorEvtGenerator())
      { cv = "injector"; }   
    else if(myAction->IsUsingGPSEvtGenerator())
      { cv = "gps"; }
    else if(myAction->IsUsingRootrackerEvtGenerator())
      { cv = "rootracker"; }   //M. Scott: Addition of Rootracker events
    else if(myAction->IsUsingCosmicsGenerator())
      { cv = "cosmics"; }
    else if(myAction->IsUsingRadioactiveEvtGenerator())
      { cv = "radioactive"; }
    else if(myAction->IsUsingRadonEvtGenerator())
      { cv = "radon"; }
  }
  
  return cv;
}

void  WCSimPrimaryGeneratorMessenger::IsotopeCommand(G4String newValue)
{
  G4Tokenizer next( newValue );

  G4String isotope = next();
  G4String location = next();
  G4double activity = StoD(next());

  myAction->AddRadioactiveSource(isotope, location, activity);
}

void WCSimPrimaryGeneratorMessenger::RadonScalingCommand(G4String newValue)
{
  G4Tokenizer next( newValue );

  G4String scenario = next();
  G4int iScenario = 0;
   
  if ( scenario == "A" ) iScenario = 1; // Relative scaling with respect to full ID volume (Pessimistic)
  if ( scenario == "B" ) iScenario = 2; // Relative scaling with respect to fiducial volume
   
  myAction->SetRadonScenario(iScenario);
}

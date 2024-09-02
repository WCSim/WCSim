#include <string>
#include <iostream>

#include "WCSimEnumerations.hh"

std::string WCSimEnumerations::EnumAsString(DigitizerType_t d)
{
  switch(d) {
  case (kDigitizerSKI) :
    return "SKI";
    break;
  default:
    return "";
    break;
  }
  return "";
}

std::string WCSimEnumerations::EnumAsString(TriggerType_t t)
{
  switch(t) {
  case (kTriggerNDigits) :
    return "NDigits";
    break;
  case (kTriggerNDigitsTest) :
    return "NDigits_TEST";
    break;
  case (kTriggerTestVertices) :
    return "TestVertices";
    break;
  case (kTriggerFailure) :
    return "No_trigger_passed";
    break;
  case (kTriggerNoTrig) :
    return "NoTrigger";
    break;
  default:
    return "";
    break;
    }
  return "";
}

std::string WCSimEnumerations::EnumAsString(WCSimRandomGenerator_t r)
{
  switch(r) {
  case (RANDOM_E_RANLUX) :
    return "RANLUX";
    break;
  case (RANDOM_E_RANECU) :
    return "RANECU";
    break;
  case (RANDOM_E_HEPJAMES) :
    return "HEPJAMES";
    break;
  default:
    return "";
    break;
  }
  return "";
}

std::string WCSimEnumerations::EnumAsString(BoundaryType_t b)
{
  switch(b) {
  case (kBlackSheet) :
    return "Blacksheet";
    break;
  case (kMPMT) :
    return "mPMT";
    break;
  case (kTyvek) :
    return "Tyvek";
    break;
  case (kCave) :
    return "Cave";
    break;
  default:
    return "";
    break;
  }
  return "";
}

std::string WCSimEnumerations::EnumAsString(ReflectionSurface_t r)
{
  switch(r) {
  case (kOtherS) :
    return "Others";
    break;
  case (kBlackSheetS) :
    return "Blacksheet";
    break;
  case (kReflectorS) :
    return "Reflector";
    break;
  case (kPhotocathodeS) :
    return "Photocathode";
    break;
  default:
    return "";
    break;
  }
  return "";
}
 
std::string WCSimEnumerations::EnumAsString(ProcessType_t p)
{
  switch(p) {
  case (kDarkNoise) :
    return "darkNoise";
    break;
  case (kHadElastic) :
    return "hadElastic";
    break;
  case (kNCapture) :
    return "nCapture";
    break;
  case (kCompt) :
    return "compt";
    break;
  case (kPhot) :
    return "phot";
    break;
  case (kCCerenkov) :
    return "Cerenkov";
    break;
  case (kSScintillation) :
    return "Scintillation";
    break;
  case (kEBrem) :
    return "eBrem";
    break;
  case (kAnnihil) :
    return "annihil";
    break;
  case (kConv) :
    return "conv";
    break;
  case (kEIoni) :
    return "eIoni";
    break;
  case (kNeutronInelastic) :
    return "neutronInelastic";
    break;
  case (kMuMinusCaptureAtRest) :
    return "muMinusCaptureAtRest";
    break;
  case (kMuIoni) :
    return "muIoni";
    break;
  case (kHIoni) :
    return "hIoni";
    break;
  case (kDecay) :
    return "Decay";
    break;
  case (kRadioactiveDecay) :
    return "RadioactiveDecay";
    break;
  case (kPhotonNuclear) :
    return "photonNuclear";
    break;
  case (kElectronNuclear) :
    return "electronNuclear";
    break;
  case (kOpticalPhotonWavelengthShifting) :
    return "OpWLS";
    break;
  case (kHBertiniCaptureAtRest) :
    return "hBertiniCaptureAtRest";
    break;
  case (kProtonInelastic) :
    return "protonInelastic";
    break;
  case (kMuonMinusAtomicCapture) :
    return "muMinusAtomicCaptureAtRest";
    break;
  case (kHadronStoppingProcess) :
    return "hadronCaptureAtRest";
    break;
  case (kEplusPolarizedAnnihilation) :
    return "pol-annihil";
    break;
  case (kEPolarizedIonisation) :
    return "pol-eIoni";
    break;
  case (kEPolarizedBremsstrahlung) :
    return "pol-eBrem";
    break;
  case (kEeToHadrons) :
    return "ee2hadr";
    break;
  case (kHhIonisation) :
    return "hhIoni";
    break;
  case (kIonIonisation) :
    return "ionIoni";
    break;
  case (kAlphaIonisation) :
    return "alphaIoni";
    break;
  case (kCoulombScattering) :
    return "CoulombScat";
    break;
  case (kHadronicProcess) :
    return "Hadronic";
    break;
  case (kHMultipleScattering) :
    return "msc";
    break;
  case (kNuclearStopping) :
    return "nuclearStopping";
    break;
  case (kAdjointhMultipleScattering) :
    return "msc";
    break;
  case (kForwardXrayTR) :
    return "XrayTR";
    break;
  case (kMuMultipleScattering) :
    return "muMsc";
    break;
  case (kDecayWithSpin) :
    return "DecayWithSpin";
    break;
  case (kUnknownDecay) :
    return "UnknownDecay";
    break;
  case (kMuonicAtomDecay) :
    return "MuonicAtomDecay";
    break;
  case (kStepLimiter) :
    return "StepLimiter";
    break;
  case (kNeutronKiller) :
    return "nKiller";
    break;
  case (kUserSpecialCuts) :
    return "UserSpecialCut";
    break;
  case (kChargeExchangeProcess) :
    return "chargeExchange";
    break;
  case (kAntiProtonInelasticProcess) :
    return "anti_protonInelastic";
    break;
  case (kHe3InelasticProcess) :
    return "he3Inelastic";
    break;
  case (kIonInelasticProcess) :
    return "ionInelastic";
    break;
  case (kSigmaMinusInelasticProcess) :
    return "sigma-Inelastic";
    break;
  case (kLambdaInelasticProcess) :
    return "lambdaInelastic";
    break;
  case (kAntiAlphaInelasticProcess) :
    return "anti_alphaInelastic";
    break;
  case (kSigmaPlusInelasticProcess) :
    return "sigma+Inelastic";
    break;
  case (kUCNLoss) :
    return "UCNLoss";
    break;
  case (kHadronFissionProcess) :
    return "nFission";
    break;
  case (kUCNBoundaryProcess) :
    return "UCNBoundaryProcess";
    break;
  case (kAntiOmegaMinusInelasticProcess) :
    return "anti_omega-Inelastic";
    break;
  case (kKaonZeroSInelasticProcess) :
    return "kaon0SInelastic";
    break;
  case (kTritonInelasticProcess) :
    return "tInelastic";
    break;
  case (kKaonMinusInelasticProcess) :
    return "kaon-Inelastic";
    break;
  case (kMuonNuclearProcess) :
    return "muonNuclear";
    break;
  case (kAntiLambdaInelasticProcess) :
    return "anti-lambdaInelastic";
    break;
  case (kPhotoCaptureProcess) :
    return "photonCapture";
    break;
  case (kUCNMultiScattering) :
    return "UCNMultiScattering";
    break;
  case (kXiZeroInelasticProcess) :
    return "xi0Inelastic";
    break;
  case (kPionMinusInelasticProcess) :
    return "pi-Inelastic";
    break;
  case (kKaonPlusInelasticProcess) :
    return "kaon+Inelastic";
    break;
  case (kPionPlusInelasticProcess) :
    return "pi+Inelastic";
    break;
  case (kPhotoFissionProcess) :
    return "photonFission";
    break;
  case (kUCNAbsorption) :
    return "UCNAbsorption";
    break;
  case (kPositronNuclearProcess) :
    return "positronNuclear";
    break;
  case (kAntiSigmaMinusInelasticProcess) :
    return "anti_sigma-Inelastic";
    break;
  case (kAntiTritonInelasticProcess) :
    return "anti_tritonInelastic";
    break;
  case (kAntiHe3InelasticProcess) :
    return "anti_He3Inelastic";
    break;
  case (kAntiXiZeroInelasticProcess) :
    return "anti_xi0Inelastic";
    break;
  case (kAntiNeutronInelasticProcess) :
    return "anti_neutronInelastic";
    break;
  case (kAntiDeuteronInelasticProcess) :
    return "anti_deuteronInelastic";
    break;
  case (kXiMinusInelasticProcess) :
    return "xi-Inelastic";
    break;
  case (kKaonZeroLInelasticProcess) :
    return "kaon0LInelastic";
    break;
  case (kDeuteronInelasticProcess) :
    return "dInelastic";
    break;
  case (kAntiXiMinusInelasticProcess) :
    return "anti_xi-Inelastic";
    break;
  case (kAntiSigmaPlusInelasticProcess) :
    return "anti_sigma+Inelastic";
    break;
  case (kOmegaMinusInelasticProcess) :
    return "omega-Inelastic";
    break;
  case (kAlphaInelasticProcess) :
    return "alphaInelastic";
    break;
  case (kAntiNeutronAnnihilationAtRest) :
    return "AntiNeutronAnnihilationAtRest";
    break;
  case (kOpMieHG) :
    return "OpMieHG";
    break;
  case (kOpAbsorption) :
    return "OpAbsorption";
    break;
  case (kOpRayleigh) :
    return "OpRayleigh";
    break;
  case (kOpBoundaryProcess) :
    return "OpBoundary";
    break;
  case (kHImpactIonisation) :
    return "hImpactIoni";
    break;
  case (kPolarizedGammaConversion) :
    return "pol-conv";
    break;
  case (kPolarizedPhotoElectricEffect) :
    return "pol-phot";
    break;
  case (kPolarizedCompton) :
    return "pol-compt";
    break;
  case (kMicroElecInelastic) :
    return "MicroElecIonisation";
    break;
  case (kMuElecInelastic) :
    return "MuElecIonisation";
    break;
  case (kMuElecElastic) :
    return "MuElecElastic";
    break;
  case (kRayleighScattering) :
    return "Rayl";
    break;
  case (kMicroElecElastic) :
    return "MicroElecElastic";
    break;
  case (kAnnihiToMuPair) :
    return "AnnihiToMuPair";
    break;
  case (kHPairProduction) :
    return "hPairProd";
    break;
  case (kHBremsstrahlung) :
    return "hBrems";
    break;
  case (kEMultipleScattering) :
    return "msc";
    break;
  case (kEAdjointMultipleScattering) :
    return "msc";
    break;
  case (kVTransitionRadiation) :
    return "TR";
    break;
  case (kTransitionRadiation) :
    return "TR";
    break;
  case (kLEPTSAttachmentModel) :
    return "G4LEPTSAttachmentModel";
    break;
  case (kErrorEnergyLoss) :
    return "G4ErrorEnergyLoss";
    break;
  case (kMuBremsstrahlung) :
    return "muBrems";
    break;
  case (kMuPairProduction) :
    return "muPairProd";
    break;
  case (kEPairProduction) :
    return "ePairProd";
    break;
  case (kPhononDownconversion) :
    return "phononDownconversion";
    break;
  case (kPhononScattering) :
    return "phononScattering";
    break;
  case (kPhononReflection) :
    return "phononReflection";
    break;
  case (kFastSimulationManagerProcess) :
    return "G4FastSimulationManagerProcess";
    break;
  case (kMuonMinusCapture) :
    return "muMinusCaptureAtRest";
    break;
  case (kHadronicAbsorptionFritiof) :
    return "hFritiofCaptureAtRest";
    break;
  case (kInitialParticle) :
    return "initial";
    break;
  default: 
    return "";
    break;
  }
  return "";
}

ProcessType_t WCSimEnumerations::ProcessTypeStringToEnum(const std::string& str) {
  if (str == "darkNoise") {
    return kDarkNoise;
  } else if (str == "hadElastic") {
    return kHadElastic;
  } else if (str == "nCapture") {
    return kNCapture;
  } else if (str == "compt") {
    return kCompt;
  } else if (str == "phot") {
    return kPhot;
  } else if (str == "Cerenkov") {
    return kCCerenkov;
  } else if (str == "Scintillation") {
    return kSScintillation;
  } else if (str == "eBrem") {
    return kEBrem;
  } else if (str == "annihil") {
    return kAnnihil;
  } else if (str == "conv") {
    return kConv;
  } else if (str == "eIoni") {
    return kEIoni;
  } else if (str == "neutronInelastic") {
    return kNeutronInelastic;
  } else if (str == "muMinusCaptureAtRest") {
    return kMuMinusCaptureAtRest;
  } else if (str == "muIoni") {
    return kMuIoni;
  } else if (str == "hIoni") {
    return kHIoni;
  } else if (str == "Decay") {
    return kDecay;
  } else if (str == "RadioactiveDecay") {
    return kRadioactiveDecay;
  } else if (str == "photonNuclear") {
    return kPhotonNuclear;
  } else if (str == "electronNuclear") {
    return kElectronNuclear;
  } else if (str == "OpWLS") {
    return kOpticalPhotonWavelengthShifting;
  } else if (str == "hBertiniCaptureAtRest") {
    return kHBertiniCaptureAtRest;
  } else if (str == "protonInelastic") {
    return kProtonInelastic;
  } else if (str == "muMinusAtomicCaptureAtRest") {
    return kMuonMinusAtomicCapture;
  } else if (str == "hadronCaptureAtRest") {
    return kHadronStoppingProcess;
  } else if (str == "pol-annihil") {
    return kEplusPolarizedAnnihilation;
  } else if (str == "pol-eIoni") {
    return kEPolarizedIonisation;
  } else if (str == "pol-eBrem") {
    return kEPolarizedBremsstrahlung;
  } else if (str == "ee2hadr") {
    return kEeToHadrons;
  } else if (str == "hhIoni") {
    return kHhIonisation;
  } else if (str == "ionIoni") {
    return kIonIonisation;
  } else if (str == "alphaIoni") {
    return kAlphaIonisation;
  } else if (str == "CoulombScat") {
    return kCoulombScattering;
  } else if (str == "Hadronic") {
    return kHadronicProcess;
  } else if (str == "msc") {
    return kHMultipleScattering;
  } else if (str == "nuclearStopping") {
    return kNuclearStopping;
  } else if (str == "msc") {
    return kAdjointhMultipleScattering;
  } else if (str == "XrayTR") {
    return kForwardXrayTR;
  } else if (str == "muMsc") {
    return kMuMultipleScattering;
  } else if (str == "DecayWithSpin") {
    return kDecayWithSpin;
  } else if (str == "UnknownDecay") {
    return kUnknownDecay;
  } else if (str == "MuonicAtomDecay") {
    return kMuonicAtomDecay;
  } else if (str == "StepLimiter") {
    return kStepLimiter;
  } else if (str == "nKiller") {
    return kNeutronKiller;
  } else if (str == "UserSpecialCut") {
    return kUserSpecialCuts;
  } else if (str == "chargeExchange") {
    return kChargeExchangeProcess;
  } else if (str == "anti_protonInelastic") {
    return kAntiProtonInelasticProcess;
  } else if (str == "he3Inelastic" || str == "He3Inelastic") {
    return kHe3InelasticProcess;
  } else if (str == "ionInelastic") {
    return kIonInelasticProcess;
  } else if (str == "sigma-Inelastic") {
    return kSigmaMinusInelasticProcess;
  } else if (str == "lambdaInelastic") {
    return kLambdaInelasticProcess;
  } else if (str == "anti_alphaInelastic") {
    return kAntiAlphaInelasticProcess;
  } else if (str == "sigma+Inelastic") {
    return kSigmaPlusInelasticProcess;
  } else if (str == "UCNLoss") {
    return kUCNLoss;
  } else if (str == "nFission") {
    return kHadronFissionProcess;
  } else if (str == "UCNBoundaryProcess") {
    return kUCNBoundaryProcess;
  } else if (str == "anti_omega-Inelastic") {
    return kAntiOmegaMinusInelasticProcess;
  } else if (str == "kaon0SInelastic") {
    return kKaonZeroSInelasticProcess;
  } else if (str == "tInelastic") {
    return kTritonInelasticProcess;
  } else if (str == "kaon-Inelastic") {
    return kKaonMinusInelasticProcess;
  } else if (str == "muonNuclear") {
    return kMuonNuclearProcess;
  } else if (str == "anti-lambdaInelastic") {
    return kAntiLambdaInelasticProcess;
  } else if (str == "photonCapture") {
    return kPhotoCaptureProcess;
  } else if (str == "UCNMultiScattering") {
    return kUCNMultiScattering;
  } else if (str == "xi0Inelastic") {
    return kXiZeroInelasticProcess;
  } else if (str == "pi-Inelastic") {
    return kPionMinusInelasticProcess;
  } else if (str == "kaon+Inelastic") {
    return kKaonPlusInelasticProcess;
  } else if (str == "pi+Inelastic") {
    return kPionPlusInelasticProcess;
  } else if (str == "photonFission") {
    return kPhotoFissionProcess;
  } else if (str == "UCNAbsorption") {
    return kUCNAbsorption;
  } else if (str == "positronNuclear") {
    return kPositronNuclearProcess;
  } else if (str == "anti_sigma-Inelastic") {
    return kAntiSigmaMinusInelasticProcess;
  } else if (str == "anti_tritonInelastic") {
    return kAntiTritonInelasticProcess;
  } else if (str == "anti_He3Inelastic") {
    return kAntiHe3InelasticProcess;
  } else if (str == "anti_xi0Inelastic") {
    return kAntiXiZeroInelasticProcess;
  } else if (str == "anti_neutronInelastic") {
    return kAntiNeutronInelasticProcess;
  } else if (str == "anti_deuteronInelastic") {
    return kAntiDeuteronInelasticProcess;
  } else if (str == "xi-Inelastic") {
    return kXiMinusInelasticProcess;
  } else if (str == "kaon0LInelastic") {
    return kKaonZeroLInelasticProcess;
  } else if (str == "dInelastic") {
    return kDeuteronInelasticProcess;
  } else if (str == "anti_xi-Inelastic") {
    return kAntiXiMinusInelasticProcess;
  } else if (str == "anti_sigma+Inelastic") {
    return kAntiSigmaPlusInelasticProcess;
  } else if (str == "omega-Inelastic") {
    return kOmegaMinusInelasticProcess;
  } else if (str == "alphaInelastic") {
    return kAlphaInelasticProcess;
  } else if (str == "AntiNeutronAnnihilationAtRest") {
    return kAntiNeutronAnnihilationAtRest;
  } else if (str == "OpMieHG") {
    return kOpMieHG;
  } else if (str == "OpAbsorption") {
    return kOpAbsorption;
  } else if (str == "OpRayleigh") {
    return kOpRayleigh;
  } else if (str == "OpBoundary") {
    return kOpBoundaryProcess;
  } else if (str == "hImpactIoni") {
    return kHImpactIonisation;
  } else if (str == "pol-conv") {
    return kPolarizedGammaConversion;
  } else if (str == "pol-phot") {
    return kPolarizedPhotoElectricEffect;
  } else if (str == "pol-compt") {
    return kPolarizedCompton;
  } else if (str == "MicroElecIonisation") {
    return kMicroElecInelastic;
  } else if (str == "MuElecIonisation") {
    return kMuElecInelastic;
  } else if (str == "MuElecElastic") {
    return kMuElecElastic;
  } else if (str == "Rayl") {
    return kRayleighScattering;
  } else if (str == "MicroElecElastic") {
    return kMicroElecElastic;
  } else if (str == "AnnihiToMuPair") {
    return kAnnihiToMuPair;
  } else if (str == "hPairProd") {
    return kHPairProduction;
  } else if (str == "hBrems") {
    return kHBremsstrahlung;
  } else if (str == "msc") {
    return kEMultipleScattering;
  } else if (str == "msc") {
    return kEAdjointMultipleScattering;
  } else if (str == "TR") {
    return kVTransitionRadiation;
  } else if (str == "TR") {
    return kTransitionRadiation;
  } else if (str == "G4LEPTSAttachmentModel") {
    return kLEPTSAttachmentModel;
  } else if (str == "G4ErrorEnergyLoss") {
    return kErrorEnergyLoss;
  } else if (str == "muBrems") {
    return kMuBremsstrahlung;
  } else if (str == "muPairProd") {
    return kMuPairProduction;
  } else if (str == "ePairProd") {
    return kEPairProduction;
  } else if (str == "phononDownconversion") {
    return kPhononDownconversion;
  } else if (str == "phononScattering") {
    return kPhononScattering;
  } else if (str == "phononReflection") {
    return kPhononReflection;
  } else if (str == "G4FastSimulationManagerProcess") {
    return kFastSimulationManagerProcess;
  } else if (str == "hFritiofCaptureAtRest") {
    return kHadronicAbsorptionFritiof;
  } else if (str == "initial") {
    return kInitialParticle;
  } else {
    std::cerr << "UNKNOWN PROCESS TYPE: " << str << std::endl
	      << "You've encountered a new ProcessType_t! "
	      << "Other people will also need to use this, so please put in a pull request, "
	      << "where you add this to WCSimEnumerations (both in the .cc & .hh files) "
	      << "Thank you in advance" << std::endl;
    //exit(-1);
    // Default value in case of unknown string
    return kUnknownProcess;
  }
}

TriggerType_t WCSimEnumerations::TriggerTypeFromString(std::string s)
{
  for(int i = int(kTriggerUndefined)+1; i <= kTriggerFailure; i++) {
    if(s.compare(WCSimEnumerations::EnumAsString((TriggerType_t)i)) == 0) {
      return (TriggerType_t)i;
    }
  }
  std::cerr << "WCSimEnumerations::TriggerTypeFromString() Unknown string value " << s << std::endl;
  return kTriggerUndefined;
}

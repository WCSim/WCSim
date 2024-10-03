#ifndef WCSimEnumerations_h
#define WCSimEnumerations_h 1

#include <string>

typedef enum ETriggerType {
  kTriggerUndefined = -1,
  kTriggerNDigits,
  kTriggerNDigitsTest,
  kTriggerTestVertices,
  kTriggerNoTrig,
  kTriggerFailure // this should always be the last entry (for looping)
} TriggerType_t;

typedef enum EDigitizerType {
  kDigitizerUndefined = -1,
  kDigitizerSKI
} DigitizerType_t;

typedef enum EInteractionMode {
  UNKNOWN = -1,
  BEAM = 0,
  COSMICS = 10,
  PARTICLEGUN = 20,
  LASER = 30
} InteractionType_t;

typedef enum ERandomGeneratorType {
  RANDOM_E_RANLUX=1,
  RANDOM_E_RANECU=2,
  RANDOM_E_HEPJAMES=3
} WCSimRandomGenerator_t;

typedef enum EBoundaryType {
  kNull=0,
  kBlackSheet,
  kMPMT,
  kTyvek,
  kCave
} BoundaryType_t;

typedef enum EReflectionSurface {
  kOtherS=0,
  kBlackSheetS,
  kReflectorS,
  kPhotocathodeS
} ReflectionSurface_t;

/*!
 * The process that directly created an object.
 * Used in the output files for true tracks and true hits.
 * This is a rather catch all `enum`, e.g. `kTarget` is not truly a process
 */
typedef enum EProcessType { // Diego Costas 27/02/2024
  kUnknownProcess,
  kDarkNoise,
  kHadElastic,
  kNCapture,
  kCompt,
  kPhot,
  kCCerenkov,
  kSScintillation,
  kEBrem,
  kAnnihil,
  kConv,
  kEIoni,
  kNeutronInelastic,
  kMuMinusCaptureAtRest,
  kMuIoni,
  kHIoni,
  kDecay,
  kRadioactiveDecay,
  kPhotonNuclear,
  kElectronNuclear,
  kOpticalPhotonWavelengthShifting,
  kHBertiniCaptureAtRest,
  kProtonInelastic,
  kMuonMinusAtomicCapture,
  kHadronStoppingProcess,
  kEplusPolarizedAnnihilation,
  kEPolarizedIonisation,
  kEPolarizedBremsstrahlung,
  kEeToHadrons,
  kHhIonisation,
  kIonIonisation,
  kAlphaIonisation,
  kCoulombScattering,
  kHadronicProcess,
  kHMultipleScattering,
  kNuclearStopping,
  kAdjointhMultipleScattering,
  kForwardXrayTR,
  kMuMultipleScattering,
  kDecayWithSpin,
  kUnknownDecay,
  kMuonicAtomDecay,
  kStepLimiter,
  kNeutronKiller,
  kUserSpecialCuts,
  kChargeExchangeProcess,
  kAntiProtonInelasticProcess,
  kHe3InelasticProcess,
  kIonInelasticProcess,
  kSigmaMinusInelasticProcess,
  kLambdaInelasticProcess,
  kAntiAlphaInelasticProcess,
  kSigmaPlusInelasticProcess,
  kUCNLoss,
  kHadronFissionProcess,
  kUCNBoundaryProcess,
  kAntiOmegaMinusInelasticProcess,
  kKaonZeroSInelasticProcess,
  kTritonInelasticProcess,
  kKaonMinusInelasticProcess,
  kMuonNuclearProcess,
  kAntiLambdaInelasticProcess,
  kPhotoCaptureProcess,
  kUCNMultiScattering,
  kXiZeroInelasticProcess,
  kPionMinusInelasticProcess,
  kKaonPlusInelasticProcess,
  kPionPlusInelasticProcess,
  kPhotoFissionProcess,
  kUCNAbsorption,
  kPositronNuclearProcess,
  kAntiSigmaMinusInelasticProcess,
  kAntiTritonInelasticProcess,
  kAntiHe3InelasticProcess,
  kAntiXiZeroInelasticProcess,
  kAntiNeutronInelasticProcess,
  kAntiDeuteronInelasticProcess,
  kXiMinusInelasticProcess,
  kKaonZeroLInelasticProcess,
  kDeuteronInelasticProcess,
  kAntiXiMinusInelasticProcess,
  kAntiSigmaPlusInelasticProcess,
  kOmegaMinusInelasticProcess,
  kAlphaInelasticProcess,
  kAntiNeutronAnnihilationAtRest,
  kOpMieHG,
  kOpAbsorption,
  kOpRayleigh,
  kOpBoundaryProcess,
  kHImpactIonisation,
  kPolarizedGammaConversion,
  kPolarizedPhotoElectricEffect,
  kPolarizedCompton,
  kMicroElecInelastic,
  kMuElecInelastic,
  kMuElecElastic,
  kRayleighScattering,
  kMicroElecElastic,
  kAnnihiToMuPair,
  kHPairProduction,
  kHBremsstrahlung,
  kEMultipleScattering,
  kEAdjointMultipleScattering,
  kVTransitionRadiation,
  kTransitionRadiation,
  kLEPTSAttachmentModel,
  kErrorEnergyLoss,
  kMuBremsstrahlung,
  kMuPairProduction,
  kEPairProduction,
  kPhononDownconversion,
  kPhononScattering,
  kPhononReflection,
  kFastSimulationManagerProcess,
  kMuonMinusCapture,
  kHadronicAbsorptionFritiof,
  kInitialParticle
} ProcessType_t;

class WCSimEnumerations
{
public:

  static std::string EnumAsString(DigitizerType_t d);
  static std::string EnumAsString(TriggerType_t t);
  static std::string EnumAsString(WCSimRandomGenerator_t r);
  static std::string EnumAsString(BoundaryType_t b);
  static std::string EnumAsString(ReflectionSurface_t r);
  //! These ProcessType_t strings must use the same naming convention as Geant4
  static std::string EnumAsString(ProcessType_t p);
  //! These ProcessType_t strings must use the same naming convention as Geant4
  static ProcessType_t ProcessTypeStringToEnum(const std::string& str);
  static TriggerType_t TriggerTypeFromString(std::string s);

};

#endif

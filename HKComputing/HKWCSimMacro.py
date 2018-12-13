class HKWCSimMacro(object):

    def __init__(self,
                 outputFileName='HKWCSim',
                 macroFileName='HKWCSim'):
        self.macro = ''
        self.outputFileName = outputFileName
        self.macroFileName = macroFileName

    def createRandomParameter(self,
                              ranGen='RANLUX',
                              ranSeed='0'):
        self.macro += '/WCSim/random/generator {0}\n'.format(ranGen)
        self.macro += '/WCSim/random/seed {0}\n'.format(ranSeed)

    def createHeader(self):
        # Sample setup macro with no visualization
        self.macro += '/run/verbose 0\n'
        self.macro += '/tracking/verbose 0\n'
        self.macro += '/hits/verbose 0\n'

    def createGeometry(self,
                       det='HyperK'):
        # HyperK
        self.macro += '/WCSim/WCgeom {0}\n'.format(det)
        # Update geom
        self.macro += '/WCSim/Construct\n'

    def createPMTsOptions(self,
                          PMTQE='Stacking_Only',
                          PMTColl='on',
                          SavePi0='false'):
        # PMT QE
        self.macro += '/WCSim/PMTQEMethod {0}\n'.format(PMTQE)
        # PMT Collection efficiency
        self.macro += '/WCSim/PMTCollEff {0}\n'.format(PMTColl)
        # Save Pi0 info
        self.macro += '/WCSim/SavePi0 {0}\n'.format(SavePi0)

    def createDAQOptions(self,
                         digi='SKI',
                         trig='NoTrigger',
                         darkMode='1',
                         darkLow='0',
                         darkHigh='100000',
                         darkWindow='4000',
                         darkRate='0',
                         darkRateUnit='kHz'):
        # Choose trigger and digitizer
        self.macro += '/DAQ/Digitizer {0}\n'.format(digi)
        self.macro += '/DAQ/Trigger {0}\n'.format(trig)

        # Grab DAQ options
        self.macro += '/control/execute macros/daq.mac\n'

        # Dark Rate Tank
        self.macro += '/DarkRate/SetDetectorElement tank\n'
        self.macro += '/DarkRate/SetDarkMode {0}\n'.format(darkMode)
        self.macro += '/DarkRate/SetDarkLow {0}\n'.format(darkLow)
        self.macro += '/DarkRate/SetDarkHigh {0}\n'.format(darkHigh)
        self.macro += '/DarkRate/SetDarkWindow {0}\n'.format(darkWindow)
        self.macro += '/DarkRate/SetDarkRate {0} {1}\n'.format(darkRate, darkRateUnit)

    def createGammaBckgGenerator(self):
        pass

    def createSandMuGenerator(self):
        ### GPS generator
        Gen = '/mygen/generator gps\n'
        Gen += '/gps/particle mu-\n'
        Gen += '/gps/pos/type Volume\n'
        Gen += '/gps/pos/shape Para\n'
        Gen += '/gps/pos/centre -37 0 0 m\n'
        Gen += '/gps/pos/halfx 1 m\n'
        Gen += '/gps/pos/halfy 10 m\n'
        Gen += '/gps/pos/halfz 10 m\n'

        Gen += '/gps/ene/type Gauss\n'
        Gen += '/gps/ene/mono {0} GeV\n'.format(self.energy)
        Gen += '/gps/ene/sigma {0} MeV\n'.format(self.energy * 100)

        Gen += '/gps/direction 1 0 0\n'

        self.macro += Gen

    def createOutGoingMuGenerator(self):
        ### GPS generator
        Gen = '/mygen/generator gps\n'
        Gen += '/gps/particle mu-\n'
        Gen += '/gps/pos/type Volume\n'
        Gen += '/gps/pos/shape Para\n'
        Gen += '/gps/pos/centre 34 0 0 m\n'
        Gen += '/gps/pos/halfx 1 m\n'
        Gen += '/gps/pos/halfy 10 m\n'
        Gen += '/gps/pos/halfz 10 m\n'

        Gen += '/gps/ene/type Gauss\n'
        Gen += '/gps/ene/mono {0} GeV\n'.format(self.energy)
        Gen += '/gps/ene/sigma {0} MeV\n'.format(self.energy * 100)

        Gen += '/gps/direction 1 0 0\n'

        self.macro += Gen

    def createTopCornersMuGenerator(self):
        pass

    def createUpGoingMuGenerator(self,
                                 Energy=1,
                                 EnergyUnit='GeV',
                                 EnergySigma=1,
                                 EnergySigmaUnit='GeV'):
        ### GPS generator
        Gen = '/mygen/generator gps\n'
        Gen += '/gps/particle mu-\n'
        Gen += '/gps/pos/type Volume\n'
        Gen += '/gps/pos/shape Para\n'
        Gen += '/gps/pos/centre 0 0 0 m\n'
        Gen += '/gps/pos/halfx 30 m\n'
        Gen += '/gps/pos/halfy 30 m\n'
        Gen += '/gps/pos/halfz 25 m\n'

        Gen += '/gps/ene/type Gauss\n'
        Gen += '/gps/ene/mono {0} {1}\n'.format(Energy,EnergyUnit)
        Gen += '/gps/ene/sigma {0} {1}\n'.format(EnergySigma,EnergySigmaUnit)

        Gen += '/gps/direction 0 0 1\n'

        self.macro += Gen

    def createDownGoingMuGenerator(self):
        ### GPS generator
        Gen = '/mygen/generator gps\n'
        Gen += '/gps/particle mu-\n'
        Gen += '/gps/pos/type Volume\n'
        Gen += '/gps/pos/shape Para\n'
        Gen += '/gps/pos/centre 0 0 0 m\n'
        Gen += '/gps/pos/halfx 30 m\n'
        Gen += '/gps/pos/halfy 30 m\n'
        Gen += '/gps/pos/halfz 25 m\n'

        Gen += '/gps/ene/type Gauss\n'
        Gen += '/gps/ene/mono {0} GeV\n'.format(self.energy)
        Gen += '/gps/ene/sigma {0} MeV\n'.format(self.energy * 100)

        Gen += '/gps/direction 0 0 -1\n'

        self.macro += Gen

    def createCosmicsGenerator(self):
        Gen = '/mygen/generator cosmics\n'
        self.macro += Gen

    def createParticleGun(self,
                          Particle='mu-',
                          Energy=1,
                          EnergyUnit='GeV',
                          Dir='0 0 -1',
                          Pos='0 0 50',
                          PosUnit='m'):
        Generator = '/mygen/generator gun\n'

        self.macro += Generator
        self.macro += '/gun/particle {0}\n'.format(Particle)
        self.macro += '/gun/energy {0} {1}\n'.format(Energy, EnergyUnit)
        self.macro += '/gun/direction {0}\n'.format(Dir)
        self.macro += '/gun/position {0} {1}\n'.format(Pos, PosUnit)

    def createFooter(self,
                     NbOfEvts=1):
        ### Name output file
        output = '/WCSimIO/RootFile {0}.root\n'.format(self.outputFileName)
        self.macro += output

        ### run
        run = '/run/beamOn {0}\n'.format(NbOfEvts)
        exit = 'exit\n'
        self.macro += run + exit

        file = open('{0}.mac'.format(self.macroFileName), 'w')
        file.write(self.macro)
        file.close()

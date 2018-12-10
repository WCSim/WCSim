class HKWCSimMacro(object):

    def __init__(self,
                 outputFileName='HKWCSim',
                 macroFileName='HKWCSim'):
        self.macro = ''
        self.outputFileName = outputFileName
        self.macroFileName = macroFileName

    def createHeader(self):
        # Sample setup macro with no visualization
        self.macro += '/run/verbose 0\n'
        self.macro += '/tracking/verbose 0\n'
        self.macro += '/hits/verbose 0\n'

    def createGeometry(self):
        # HyperK
        self.macro += '/WCSim/WCgeom HyperK\n'
        # Update geom
        self.macro += '/WCSim/Construct\n'

    def createPMTsOptions(self):
        # PMT QE
        self.macro += '/WCSim/PMTQEMethod Stacking_Only\n'
        # PMT Collection efficiency
        self.macro += '/WCSim/PMTCollEff on\n'
        # Save Pi0 info
        self.macro += '/WCSim/SavePi0 false\n'

    def createDAQOptions(self):
        # Choose trigger and digitizer
        Digi = '/DAQ/Digitizer SKI\n'
        self.macro += Digi
        Trig = '/DAQ/Trigger NoTrigger\n'
        self.macro += Trig

        # Grab DAQ options
        DAQ = '/control/execute macros/daq.mac\n'
        self.macro += DAQ

        # Dark Rate Tank
        DarkRateDetEl = '/DarkRate/SetDetectorElement tank\n'
        DarkRateMode = '/DarkRate/SetDarkMode 1\n'
        DarkRateLow = '/DarkRate/SetDarkLow 0\n'
        DarkRateHigh = '/DarkRate/SetDarkHigh 100000\n'
        DarkRateWindow = '/DarkRate/SetDarkWindow 4000\n'
        DarkRate = '/DarkRate/SetDarkRate 0 kHz\n'
        self.macro += DarkRateDetEl
        self.macro += DarkRateMode
        self.macro += DarkRateLow
        self.macro += DarkRateHigh
        self.macro += DarkRateWindow
        self.macro += DarkRate

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

    def createUpGoingMuGenerator(self):
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

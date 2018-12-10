from HKODWCSimMacro import HKODWCSimMacro

WaterThickness = {100, 150, 200}
WaterThicknessUnit = 'cm'
E = 1
EUnit = 'GeV'

Dir = {'In': '-1 0 0', 'Out': '1 0 0'}
Vertex = {'3650cm': '36.5 0 0', '4000cm': '40 0 0'}

for dirName, dirValue in Dir.items():
    for w in WaterThickness:
        for vtxName, vtxValue in Vertex.items():
            mac = HKODWCSimMacro(outputFileName='Mu{0}{1}{2}WaterThick{3}{4}Vtx{5}'.format(dirName,
                                                                                           E,
                                                                                           EUnit,
                                                                                           w,
                                                                                           WaterThicknessUnit,
                                                                                           vtxName),
                                 macroFileName='Mu{0}{1}{2}WaterThick{3}{4}Vtx{5}'.format(dirName,
                                                                                          E,
                                                                                          EUnit,
                                                                                          w,
                                                                                          WaterThicknessUnit,
                                                                                          vtxName))
            mac.createHeader()
            mac.createGeometry(PMTODRadius=3,
                               ODLateralWaterDepth=w,
                               ODLateralWaterDepthUnit=WaterThicknessUnit,
                               ODHeightWaterDepth=2,
                               ODHeightWaterDepthUnit='m',
                               ODDeadSpace=600,
                               ODDeadSpaceUnit='mm',
                               ODTyvekSheetThickness=1,
                               ODTyvekSheetThicknessUnit='mm',
                               ODWLSPlatesThickness=1,
                               ODWLSPlatesThicknessUnit='cm',
                               ODWLSPlatesLength=48,
                               ODWLSPlatesLengthUnit='cm',
                               PMTODperCellHorizontal=3,
                               PMTODperCellVertical=2,
                               PMTODPercentCoverage=0.42,
                               ODPMTShift=0)
            mac.createPMTsOptions()
            mac.createDAQOptions()
            mac.createODDAQOptions()

            mac.createParticleGun(Particle='mu-',
                                  Energy=E,
                                  EnergyUnit=EUnit,
                                  Dir=dirValue,
                                  Pos=vtxValue,
                                  PosUnit='m')
            if dirName == 'Out' and vtxName == '4000cm':
                pass
            else:
                mac.createFooter(NbOfEvts=1000)


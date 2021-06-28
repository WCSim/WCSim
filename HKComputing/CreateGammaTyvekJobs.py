from HKODWCSimMacro import HKODWCSimMacro

WaterThicknessUnit = 'cm'
E = 8
EUnit = 'MeV'

Dir = {'In': '-1 0 0'}
Vertex = {100: '37 0 0', 150: '37.5 0 0', 200: '38 0 0'}

for dirName, dirValue in Dir.items():
    for vtxName, vtxValue in Vertex.items():
        mac = HKODWCSimMacro(outputFileName='Gamma{0}{1}{2}WaterThick{3}{4}Vtx{5}_8inch'.format(dirName,
                                                                                                    E,
                                                                                                    EUnit,
                                                                                                    vtxName,
                                                                                                    WaterThicknessUnit,
                                                                                                    vtxName),
                             macroFileName='Gamma{0}{1}{2}WaterThick{3}{4}Vtx{5}_8inch'.format(dirName,
                                                                                         E,
                                                                                         EUnit,
                                                                                         vtxName,
                                                                                         WaterThicknessUnit,
                                                                                         vtxName))
        mac.createHeader()
        mac.createGeometry(PMTODRadius=8,
                           ODLateralWaterDepth=vtxName,
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
                           PMTODperCellHorizontal=1,
                           PMTODperCellVertical=2,
                           PMTODPercentCoverage=1,
                           ODPMTShift=0)
        mac.createPMTsOptions()
        mac.createDAQOptions()
        mac.createODDAQOptions()

        mac.createParticleGun(Particle='gamma',
                              Energy=E,
                              EnergyUnit=EUnit,
                              Dir=dirValue,
                              Pos=vtxValue,
                              PosUnit='m')
        if dirName == 'Out' and vtxName == '4000cm':
            pass
        else:
            mac.createFooter(NbOfEvts=1000)


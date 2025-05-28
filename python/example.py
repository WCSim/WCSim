import pyWCSim as wc
instance = wc.build_wcsim_instance()
instance.execute("macros/tuning_parameters.mac")

G4ThreeVector = wc.g4.G4ThreeVector
cm = wc.g4.cm

wc.g4.GeometryScan.SetScanGeometryBoundaryCenter(G4ThreeVector( 1590.750000000*cm,
                                                                 318.150000000*cm,
                                                               -1818.921111111*cm))

wc.g4.GeometryScan.SetScanGeometryBoundaryWidth(G4ThreeVector(2*cm,
                                                              2*cm,
                                                              2*cm))

wc.g4.GeometryScan.SetScanGeometryStepSize(G4ThreeVector(1*cm,
                                                              1*cm,
                                                              1*cm))

wc.g4.GeometryScan.ScanGeometry("testfile.txt")

instance.execute("WCSim.mac")

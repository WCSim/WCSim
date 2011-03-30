# Dump the geometry into a file
/vis/ASCIITree/set/outFile wlsp.dat
/vis/ASCIITree/verbose 1
/vis/drawTree

# Draw Something
/vis/geometry/list WCBarrelCell #(list its properties)
/vis/geometry/set/visibility WCPMT -1 1 #(make it visible)
/vis/geometry/set/colour WCPMT -1 1 0 0 0 #(set color)
/vis/geometry/set/forceSolid WCPMT -1 1 #(force solid)

/vis/viewer/reset #(reset the scene)

/vis/viewer/select viewer-0 #(select the scene) 

/vis/viewer/set/viewpointThetaPhi 45 45 deg #(select angle)

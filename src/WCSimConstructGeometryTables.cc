#include "WCSimDetectorConstruction.hh"
#include "WCSimPmtInfo.hh"

#include "G4Material.hh"
#include "G4Element.hh"
#include "G4Box.hh"
#include "G4LogicalVolume.hh"
#include "G4VPhysicalVolume.hh"
#include "G4PVPlacement.hh"
#include "G4ThreeVector.hh"
#include "G4Vector3D.hh"
#include "globals.hh"
#include "G4VisAttributes.hh"
#include "G4Tubs.hh"
#include "G4Sphere.hh"

#include <sstream>
#include <iomanip>

using std::setw;
// These routines are object registration routines that you can pass
// to the traversal code.

void WCSimDetectorConstruction::PrintGeometryTree
(G4VPhysicalVolume* aPV ,int aDepth, int /*replicaNo*/, 
 const G4Transform3D& aTransform) 
{
  for (int levels = 0; levels < aDepth; levels++) G4cout << " ";
  G4cout << aPV->GetName() << " Level:" << aDepth 
	 << " Pos:" << aTransform.getTranslation() 
	 << " Rot:" << aTransform.getRotation().getTheta()/deg 
	 << "," << aTransform.getRotation().getPhi()/deg 
	 << "," << aTransform.getRotation().getPsi()/deg
	 << G4endl;
}

void WCSimDetectorConstruction::GetWCGeom
(G4VPhysicalVolume* aPV ,int aDepth, int /*replicaNo*/, 
 const G4Transform3D& aTransform) 
{

    // Grab mishmash of useful information from the tree while traversing it
    // This information will later be written to the geometry file
    // (Alternatively one might define accessible constants)
  
    if ((aPV->GetName() == "WCBarrel") ||
        (aPV->GetName() == "WorldBox")) {    // last condition is the HyperK Envelope name.
    // Stash info in data member
    WCOffset = G4ThreeVector(aTransform.getTranslation().getX()/cm,
			     aTransform.getTranslation().getY()/cm,
			     aTransform.getTranslation().getZ()/cm);
    }

	

    // Stash info in data member
    // AH Need to store this in CM for it to be understood by SK code
    WCPMTSize = WCPMTRadius/cm;// I think this is just a variable no if needed


    // Note WC can be off-center... get both extremities
    static G4float zmin=100000,zmax=-100000.;
    static G4float xmin=100000,xmax=-100000.;
    static G4float ymin=100000,ymax=-100000.;
    if (aDepth == 0) { // Reset for this traversal
        xmin=100000,xmax=-100000.; 
        ymin=100000,ymax=-100000.; 
        zmin=100000,zmax=-100000.; 
    }

    if ((aPV->GetName() == "WCCapBlackSheet") || (aPV->GetName().find("glassFaceWCPMT") != std::string::npos)){ 
      G4float x =  aTransform.getTranslation().getX()/cm;
      G4float y =  aTransform.getTranslation().getY()/cm;
      G4float z =  aTransform.getTranslation().getZ()/cm;
      
      if (x<xmin){xmin=x;}
      if (x>xmax){xmax=x;}
      
      if (y<ymin){ymin=y;}
      if (y>ymax){ymax=y;}

      if (z<zmin){zmin=z;}
      if (z>zmax){zmax=z;}
      

 
      WCCylInfo[0] = xmax-xmin;
      WCCylInfo[1] = ymax-ymin;
      WCCylInfo[2] = zmax-zmin;
      //      G4cout << "determin hight: " << zmin << "  " << zmax << " " << aPV->GetName()<<" " << z  << G4endl;
  } 
}

void WCSimDetectorConstruction::DescribeAndRegisterPMT(G4VPhysicalVolume* aPV ,int aDepth, int replicaNo,
                                                       const G4Transform3D& aTransform) 
{
  static std::string replicaNoString[20];

  std::stringstream depth;
  std::stringstream pvname;

  depth << replicaNo;
  pvname << aPV->GetName();

  replicaNoString[aDepth] = pvname.str() + "-" + depth.str();

  if (aPV->GetName()== WCIDCollectionName ||aPV->GetName()== WCODCollectionName ) 
    {

    // First increment the number of PMTs in the tank.
    totalNumPMTs++;  
    
    // Put the location of this tube into the location map so we can find
    // its ID later.  It is coded by its tubeTag string.
    // This scheme must match that used in WCSimWCSD::ProcessHits()

    std::string tubeTag;
    for (int i=0; i <= aDepth; i++)
      tubeTag += ":" + replicaNoString[i];
    // G4cout << tubeTag << G4endl;
    
    if ( tubeLocationMap.find(tubeTag) != tubeLocationMap.end() ) {
        G4cerr << "Repeated tube tag: " << tubeTag << G4endl;
        G4cerr << "Assigned to both tube #" << tubeLocationMap[tubeTag] << " and #" << totalNumPMTs << G4endl;
        G4cerr << "Cannot continue -- hits will not be recorded correctly."  << G4endl;
        G4cerr << "Please make sure that logical volumes with multiple placements are each given a unique copy number" << G4endl;
        assert(false);
    }
    tubeLocationMap[tubeTag] = totalNumPMTs;
    
    // Put the transform for this tube into the map keyed by its ID
    tubeIDMap[totalNumPMTs] = aTransform;
   
    
    // G4cout <<  "depth " << depth.str() << G4endl;
    // G4cout << "tubeLocationmap[" << tubeTag  << "]= " << tubeLocationMap[tubeTag] << "\n";
      
    // Print
    //     G4cout << "Tube: "<<std::setw(4) << totalNumPMTs << " " << tubeTag
    //     	   << " Pos:" << aTransform.getTranslation()/cm 
    //     	   << " Rot:" << aTransform.getRotation().getTheta()/deg 
    //     	   << "," << aTransform.getRotation().getPhi()/deg 
    //     	   << "," << aTransform.getRotation().getPsi()/deg
    //     	   << G4endl; 
  }
}

// Utilities to do stuff with the info we have found.

// Output to WC geometry text file
void WCSimDetectorConstruction::DumpGeometryTableToFile()
{
  // Open a file
  geoFile.open("geofile.txt", std::ios::out);

  geoFile.precision(2);
  geoFile.setf(std::ios::fixed);

  // (JF) Get first tube transform for filling in detector radius
  // the height is still done with WCCylInfo above
  G4Transform3D firstTransform = tubeIDMap[2];
  innerradius = sqrt(pow(firstTransform.getTranslation().getX()/cm,2)
                            + pow(firstTransform.getTranslation().getY()/cm,2));

  if (isEggShapedHyperK){
    geoFile << setw(8)<< 0;
    geoFile << setw(8)<< 0;
  }else{
    geoFile << setw(8)<< innerradius;
    geoFile << setw(8)<<WCCylInfo[2];
  }
  geoFile << setw(10)<<totalNumPMTs;
  geoFile << setw(8)<<WCPMTSize << setw(4)  <<G4endl;

  geoFile << setw(8)<< WCOffset(0)<< setw(8)<<WCOffset(1)<<
    setw(8) << WCOffset(2)<<G4endl;

  //G4double maxZ=0.0;// used to tell if pmt is on the top/bottom cap
  //G4double minZ=0.0;// or the barrel
  G4int cylLocation;


  // clear before add new stuff in
  for (unsigned int i=0;i<fpmts.size();i++){
    delete fpmts.at(i);
  }
  fpmts.clear();

  // Grab the tube information from the tubeID Map and dump to file.
  for ( int tubeID = 1; tubeID <= totalNumPMTs; tubeID++){
    G4Transform3D newTransform = tubeIDMap[tubeID];

    // Get tube orientation vector
    G4Vector3D nullOrient = G4Vector3D(0,0,1);
    G4Vector3D pmtOrientation = newTransform * nullOrient;
    //cyl_location cylLocation = tubeCylLocation[tubeID];

    // Figure out if pmt is on top/bottom or barrel
    // print key: 0-top, 1-barrel, 2-bottom
    if (pmtOrientation*newTransform.getTranslation() > 0)//veto pmt
    {cylLocation=3;}
    else if (pmtOrientation.z()==1.0)//bottom
    {cylLocation=2;}
    else if (pmtOrientation.z()==-1.0)//top
    {cylLocation=0;}
    else // barrel
    {cylLocation=1;}
    
    geoFile.precision(9);
     geoFile << setw(4) << tubeID 
 	    << " " << setw(8) << newTransform.getTranslation().getX()/cm
 	    << " " << setw(8) << newTransform.getTranslation().getY()/cm
 	    << " " << setw(8) << newTransform.getTranslation().getZ()/cm
	    << " " << setw(7) << pmtOrientation.x()
	    << " " << setw(7) << pmtOrientation.y()
	    << " " << setw(7) << pmtOrientation.z()
 	    << " " << setw(3) << cylLocation
 	    << G4endl;
     
     WCSimPmtInfo *new_pmt = new WCSimPmtInfo(cylLocation,
					      newTransform.getTranslation().getX()/cm,
					      newTransform.getTranslation().getY()/cm,
					      newTransform.getTranslation().getZ()/cm,
					      pmtOrientation.x(),
					      pmtOrientation.y(),
					      pmtOrientation.z(),
					      tubeID);
     
     fpmts.push_back(new_pmt);

  }
  geoFile.close();


} 


// Code for traversing the geometry tree.  This code is very general you pass
// it a function and it will call the function with the information on each
// object it finds.
//
// The traversal code comes from a combination of me/G4Lab project &
// from source/visualization/modeling/src/G4PhysicalVolumeModel.cc
//
// If you are trying to understand how passing the function works you need
// to understand pointers to member functions...
//
// Also notice that DescriptionFcnPtr is a (complicated) typedef.
//

void WCSimDetectorConstruction::TraverseReplicas
(G4VPhysicalVolume* aPV, int aDepth, const G4Transform3D& aTransform,
 DescriptionFcnPtr registrationRoutine)
{
  // Recursively visit all of the geometry below the physical volume
  // pointed to by aPV including replicas.
  
  G4ThreeVector     originalTranslation = aPV->GetTranslation();
  G4RotationMatrix* pOriginalRotation   = aPV->GetRotation();
  
  if (aPV->IsReplicated() ) 
  {
    EAxis    axis;
    G4int    nReplicas;
    G4double width, offset;
    G4bool   consuming;

    aPV->GetReplicationData(axis,nReplicas,width,offset,consuming);
    
    for (int n = 0; n < nReplicas; n++) 
    {
      switch(axis) {
      default:
      case kXAxis:
	aPV->SetTranslation(G4ThreeVector
			    (-width*(nReplicas-1)*0.5+n*width,0,0));
	aPV->SetRotation(0);
	break;
      case kYAxis:
	aPV->SetTranslation(G4ThreeVector
			    (0,-width*(nReplicas-1)*0.5+n*width,0));
	aPV->SetRotation(0);
	break;
      case kZAxis:
	aPV->SetTranslation(G4ThreeVector
			    (0,0,-width*(nReplicas-1)*0.5+n*width));
	aPV->SetRotation(0);
	break;
      case kRho:
	//Lib::Out::putL("GeometryVisitor::visit: WARNING:");
	//Lib::Out::putL(" built-in replicated volumes replicated");
	//Lib::Out::putL(" in radius are not yet properly visualizable.");
	aPV->SetTranslation(G4ThreeVector(0,0,0));
	aPV->SetRotation(0);
	break;
      case kPhi:
	{
	  G4RotationMatrix rotation;
          rotation.rotateZ(-(offset+(n+0.5)*width));
          // Minus Sign because for the physical volume we need the
          // coordinate system rotation.
          aPV->SetTranslation(G4ThreeVector(0,0,0));
          aPV->SetRotation(&rotation);
	}
	break;

      } // axis switch

      DescribeAndDescendGeometry(aPV, aDepth, n, aTransform, 
				 registrationRoutine);

    }   // num replicas for loop
  }     // if replicated
  else 
    DescribeAndDescendGeometry(aPV, aDepth, aPV->GetCopyNo(), aTransform, 
			       registrationRoutine);
  
  // Restore original transformation...
  aPV->SetTranslation(originalTranslation);
  aPV->SetRotation(pOriginalRotation);
}

void WCSimDetectorConstruction::DescribeAndDescendGeometry
(G4VPhysicalVolume* aPV ,int aDepth, int replicaNo, 
 const G4Transform3D& aTransform,  DescriptionFcnPtr registrationRoutine)
{
  // Calculate the new transform relative to the old transform

  G4Transform3D* transform = 
    new G4Transform3D(*(aPV->GetObjectRotation()), aPV->GetTranslation());

  G4Transform3D newTransform = aTransform * (*transform);
  delete transform; 

  // Call the routine we use to print out geometry descriptions, make
  // tables, etc.  The routine was passed here as a paramater.  It needs to
  // be a memeber function of the class

  (this->*registrationRoutine)(aPV, aDepth, replicaNo, newTransform);

  int nDaughters = aPV->GetLogicalVolume()->GetNoDaughters();
  
  for (int iDaughter = 0; iDaughter < nDaughters; iDaughter++) 
    TraverseReplicas(aPV->GetLogicalVolume()->GetDaughter(iDaughter),
		     aDepth+1, newTransform, registrationRoutine);
}



G4double WCSimDetectorConstruction::GetGeo_Dm(G4int i){
  if (i>=0&&i<=2){
    return WCCylInfo[i];
  }else if(i==3){
    return innerradius;
  }else{
    return 0;
  }
}

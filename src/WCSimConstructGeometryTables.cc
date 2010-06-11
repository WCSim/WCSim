#include "WCSimDetectorConstruction.hh"

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
(G4VPhysicalVolume* aPV ,int aDepth, int replicaNo, 
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
(G4VPhysicalVolume* aPV ,int aDepth, int replicaNo, 
 const G4Transform3D& aTransform) 
{

  // Grab mishmash of useful information from the tree while traversing it
  // This information will later be written to the geometry file
  // (Alternatively one might define accessible constants)
  
 //aah original line-> if ((aPV->GetName() == "WCBarrel")){
	if ((aPV->GetName() == "WCBarrel")||(aPV->GetName() == "physiMBTank")){//last condition is the Mailbox Physical Tank name.
    // Stash info in data member
    WCOffset = G4ThreeVector(aTransform.getTranslation().getX()/cm,
			     aTransform.getTranslation().getY()/cm,
			     aTransform.getTranslation().getZ()/cm);
  }

	
	//aah---original line->  if ((aPV->GetName() == "WCCapPMTGlass") || (aPV->GetName() == "WCPMTGlass")){
	//if ((aPV->GetName().contains("PMTGlass"))){    //aah--new version

    //(JF) all volumes have the same name now
//    if ((aPV->GetName() == "WCPMTGlass"){
    // Assume all the PMTs are the same size
   // G4Sphere* solidWCPMT = (G4Sphere *)aPV->GetLogicalVolume()->GetSolid();
    //G4double outerRadius = solidWCPMT->GetOuterRadius()/cm;

    // Stash info in data member
    WCPMTSize = WCPMTRadius;// I think this is just a variable no if needed
//  }
// (JF) None of this is needed anymore.  Values are calculated when writing file
/*   
	WCCylInfo[0] = 0;//aah  in case not a cylinder at all--eg Mailbox
	
  if ((aPV->GetName() == "WCPMTGlass")){

    //    G4Tubs* CylInfo[0] 
    //    G4double innerRadius = solidWCBSCell->GetInnerRadius()/cm;
    // M Fechner : it is no longer a cylinder
    
    G4double innerRadius = sqrt(pow(aTransform.getTranslation().getX()/cm,2) + pow(aTransform.getTranslation().getY()/cm,2));
    //    G4double innerRadius = aTransform.getTranslation().getZ()/cm;
  // Stash info in data member
    WCCylInfo[0] = innerRadius;
  } 
*/

  // Note WC can be off-center... get both extremities
  static G4float zmin=100000,zmax=-100000.;
  if (aDepth == 0) { // Reset for this traversal
    zmin=100000,zmax=-100000.; 
  }

 // aah ->old version
    if ((aPV->GetName() == "WCCapBlackSheet") || (aPV->GetName() == "WCPMTGlass")){
//	if ((aPV->GetName().contains("BlackSheet")) || (aPV->GetName()=="WCCapPMT") || (aPV->GetName().contains("PMTGlass_"))){//aah new version--does effectively the same thing, finds max and min z maybe less efficiently since more volumes may be found, but it is done only once per run.	
    G4float z =  aTransform.getTranslation().getZ()/cm;
    if (z<zmin){zmin=z;}
    if (z>zmin){zmax=z;}

    WCCylInfo[1] = zmax-zmin;
    //G4cout << "determin hight: " << zmin << "  " << zmax << " " << aPV->GetName()<<" " << z  << G4endl;
  } 
}

void WCSimDetectorConstruction::DescribeAndRegisterPMT
(G4VPhysicalVolume* aPV ,int aDepth, int replicaNo,
 const G4Transform3D& aTransform) 
{
  static std::string replicaNoString[10];

#ifndef USE_STRSTREAM
  std::stringstream depth;
#else
  char buffer[100];
  std::strstream depth(buffer,100);
#endif

  depth << replicaNo;
#ifdef USE_STRSTREAM
  depth << std::ends;
#endif
  
  replicaNoString[aDepth] = depth.str();

//aah original line->
if ((aPV->GetName() == "WCPMTGlass"))
//	if ((aPV->GetName().contains("PMTGlass")))//aah I believe this works for all
  {
    // First increment the number of PMTs in the tank.
    totalNumPMTs++;  
    
    // Put the location of this tube into the location map so we can find
    // its ID later.  It is coded by its tubeTag string.
    
    std::string tubeTag(aPV->GetName());
    for (int i=0; i <= aDepth; i++)
      tubeTag += ":" + replicaNoString[i];
    tubeLocationMap[tubeTag] = totalNumPMTs;
    
    // Record where tube is in the cylinder
    // (JF) This distinction was useful for 2km detector
    // not so much for DUSEL
/*    
//aah original
if (aPV->GetName() == "WCPMTGlass"){
//if ((aPV->GetName()=="WCPMTGlass") || (aPV->GetName().contains("D_PMTGlass")))
//aah  also find the Mailbox pmt's on side wall (LxD_PMTGlass or WxD_PMTGlass!)
   
      tubeCylLocation[totalNumPMTs] = wall;
    }
    else 
    {   // It's a cap pmt
      // Get which endcap 
		if ((aPV->GetName() == "WCCapPMTGlass")) //aah
		{									//aah      
      //// Cap tubes could be at either of two possible depths..
      //// Get the depth of the cap, up either 3 or 4
      ////      G4int capdepth = (aDepth==6 ? aDepth-4 : aDepth-3);
      //// M F : NOT TRUE ANY MORE : WE REMOVED THE CAP CELLS , THE
      //// DEPTHS ARE DIFFERENT !!!
      
      G4int capdepth = 2;
      G4int endcap = std::atoi(replicaNoString[capdepth].c_str());
      //      G4cout << "endcap " << endcap << "\n";
      //// If the cap is replica 1, it's endcap1.  If replica 0, it's endcap 2
      tubeCylLocation[totalNumPMTs] = (endcap ? endcap1: endcap2);
		}  //aah
		else	//aah
		{		//aah
			if ((aPV->GetName() == "LXW_PMT_plus"))tubeCylLocation[totalNumPMTs]=endcap1;	//top plane  aah
			if ((aPV->GetName() == "LXW_PMT_minus"))tubeCylLocation[totalNumPMTs]=endcap2;   //bottom plane  aah
		}				//aah
    }
*/    
    
    // Put the transform for this tube into the map keyed by its ID
    tubeIDMap[totalNumPMTs] = aTransform;
    /*
      G4cout <<  "depth " << depth.str() << G4endl;
      G4cout << "tubeLocationmap[" << tubeTag  << "]= " << tubeLocationMap[tubeTag] << "\n";
      G4cout << "tubeCylLocation[" << totalNumPMTs  << "]= " << tubeCylLocation[totalNumPMTs] << "\n";
    */
    // Print
    //     G4cout << "Tube: "<<std::setw(4) << totalNumPMTs << " " << tubeTag
    //     	   << " Pos:" << aTransform.getTranslation()/cm 
    //     	   << " Rot:" << aTransform.getRotation().getTheta()/deg 
    //     	   << "," << aTransform.getRotation().getPhi()/deg 
    //     	   << "," << aTransform.getRotation().getPsi()/deg
    //     	   << G4endl; 
  }
}


// (JF) This is not needed for DUSEL Detectors
/*
// Maximilien Fechner : needed because the 1KT geometry is treated 
// differently
// WARNING : this routine is "hard-coded", and relies on the way the
// Construction routine works, on the K2K-1KT geometry, and
// on the original geofile.txt output (from spring 2004)

void WCSimDetectorConstruction::DescribeAndRegisterPMT_1KT
(G4VPhysicalVolume* aPV ,int aDepth, int replicaNo,
 const G4Transform3D& aTransform) 
{
  static std::string replicaNoString[10];
  static int traversedBarrelTubes = 0;
  static int traversedCapTubes = 0;
  int NumPMT = 0; // M F 
  

#ifndef USE_STRSTREAM
  std::stringstream depth;
#else
  char buffer[100];
  std::strstream depth(buffer,100);
#endif

  depth << replicaNo;
#ifdef USE_STRSTREAM
  depth << std::ends;
#endif
  
  replicaNoString[aDepth] = depth.str();

  if ((aPV->GetName() == "WCCapPMTGlass") || (aPV->GetName() == "WCPMTGlass")){
    // increment the total number of tubes
    totalNumPMTs++;

	// Put the location of this tube into the location map so we can find
	// its ID later.  It is coded by its tubeTag string.

	std::string tubeTag(aPV->GetName());
    for (int i=0; i <= aDepth; i++)
      tubeTag += ":" + replicaNoString[i];
    //MF : not any more, doesn't work with 1KT software
    //    tubeLocationMap[tubeTag] = totalNumPMTs;
    
    // Record where tube is in the cylinder
    
    if (aPV->GetName() == "WCPMTGlass"){    
      // same as always : first the caps, then the barrel
      if ( aDepth == 3 ) {
	G4int num  = std::atoi(replicaNoString[aDepth].c_str()) + 1;
	if ( num >= 225) {  // in the outermost rings
	  tubeCylLocation[num] = wall;
	  tubeLocationMap[tubeTag] = num ;
	  NumPMT = num;      
	}
      } else { //replicated rings in the middle
	traversedBarrelTubes++;
	tubeCylLocation[traversedBarrelTubes+262] = wall;
	tubeLocationMap[tubeTag] = traversedBarrelTubes+262;
	NumPMT = traversedBarrelTubes  + 262;
      }
    } else {   // It's a cap pmt
      // Get which endcap 
      G4int capdepth = 2;
      G4int endcap = std::atoi(replicaNoString[capdepth].c_str());
      //      G4cout << "endcap " << endcap << "\n";
      //// If the cap is replica 1, it's endcap1.  If replica 0, it's endcap 2
      traversedCapTubes++;
      tubeLocationMap[tubeTag] = traversedCapTubes;
      tubeCylLocation[traversedCapTubes] = ( (traversedCapTubes <= 112) ? endcap2: endcap1); 
      NumPMT = traversedCapTubes;
    }
    
    // Put the transform for this tube into the map keyed by its ID
    tubeIDMap[NumPMT] = aTransform;
    
    //    G4cout <<  "depth " << depth.str() << G4endl;
    //G4cout << "tubeLocationmap[" << tubeTag  << "]= " << tubeLocationMap[tubeTag] << "\n";
    //G4cout << "tubeCylLocation[" << NumPMT  << "]= " << tubeCylLocation[NumPMT] << "\n";
    
    // Print
//     G4cout << "Tube: "<<std::setw(4) << totalNumPMTs << " " << tubeTag
//     	   << " Pos:" << aTransform.getTranslation()/cm 
//     	   << " Rot:" << aTransform.getRotation().getTheta()/deg 
//     	   << "," << aTransform.getRotation().getPhi()/deg 
//     	   << "," << aTransform.getRotation().getPsi()/deg
//     	   << G4endl; 
  }

}
*/

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
  G4double innerradius = sqrt(pow(firstTransform.getTranslation().getX()/cm,2)
                            + pow(firstTransform.getTranslation().getY()/cm,2));

  geoFile << setw(8)<< innerradius;
  geoFile << setw(8)<<WCCylInfo[1];
  geoFile << setw(10)<<totalNumPMTs;
  geoFile << setw(8)<<WCPMTSize << setw(4)  <<G4endl;

  geoFile << setw(8)<< WCOffset(0)<< setw(8)<<WCOffset(1)<<
    setw(8) << WCOffset(2)<<G4endl;

  G4double maxZ=0.0;// used to tell if pmt is on the top/bottom cap
  G4double minZ=0.0;// or the barrel
  G4int cylLocation;

  // Grab the tube information from the tubeID Map and dump to file.
  for ( int tubeID = 1; tubeID <= totalNumPMTs; tubeID++){
    G4Transform3D newTransform = tubeIDMap[tubeID];

    // Get tube orientation vector
    G4Vector3D nullOrient = G4Vector3D(0,0,1);
    G4Vector3D pmtOrientation = newTransform * nullOrient;
    //cyl_location cylLocation = tubeCylLocation[tubeID];

    // Figure out if pmt is on top/bottom or barrel
    // print key: 0-top, 1-barrel, 2-bottom
    if (pmtOrientation.z()!=1.0)//barrel pmt
    {cylLocation=1;}
    else if (newTransform.getTranslation().getZ() > 0.0)//top
    {cylLocation=0;}
    else // bottom
    {cylLocation=2;}
    
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


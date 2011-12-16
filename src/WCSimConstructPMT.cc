#include "WCSimDetectorConstruction.hh"

#include "G4Box.hh"
#include "G4Sphere.hh"
#include "G4SubtractionSolid.hh"
#include "G4LogicalVolume.hh"
#include "G4VisAttributes.hh"
#include "G4Material.hh"
#include "G4Polycone.hh"
#include "G4PVPlacement.hh"
#include "G4LogicalBorderSurface.hh"

using namespace std;


//PMT logical volume construction.

void WCSimDetectorConstruction::ConstructPMT()
{

  G4bool addWinstonCones = true; //test, need to add messenger

  sphereRadius = (WCPMTExposeHeight*WCPMTExposeHeight+ WCPMTRadius*WCPMTRadius)
  			/(2*WCPMTExposeHeight);
  PMTOffset =  sphereRadius - WCPMTExposeHeight;

  //Need a volume to cut away excess behind blacksheet
  G4Box* solidCutOffTubs =
  	new G4Box(	"cutOffTubs",
			sphereRadius+1.*cm,
			sphereRadius+1.*cm,
			PMTOffset);


  //Create PMT Interior
  G4Sphere* tmpSolidInteriorWCPMT =
  	new G4Sphere(	"tmpInteriorWCPMT",
				0.0*m,(sphereRadius-WCPMTGlassThickness),
				0.0*deg,360.0*deg,
				0.0*deg,90.0*deg);

  G4SubtractionSolid* solidInteriorWCPMT =
  	new G4SubtractionSolid(	"InteriorWCPMT",
					tmpSolidInteriorWCPMT,
					solidCutOffTubs);

  G4LogicalVolume* logicInteriorWCPMT =
	new G4LogicalVolume(	solidInteriorWCPMT,
					G4Material::GetMaterial("Vacuum"),
					"InteriorWCPMT",
					0,0,0);


  //Create PMT Glass Face
  G4Sphere* tmpGlassFaceWCPMT =
  	new G4Sphere(	"tmpGlassFaceWCPMT",
				(sphereRadius-WCPMTGlassThickness),
				sphereRadius,
				0.0*deg,360.0*deg,
				0.0*deg,90.0*deg);
  
  G4SubtractionSolid* solidGlassFaceWCPMT =
  	new G4SubtractionSolid(	"GlassFaceWCPMT",
					tmpGlassFaceWCPMT,
					solidCutOffTubs); 

  logicGlassFaceWCPMT =
	new G4LogicalVolume(	solidGlassFaceWCPMT,
					G4Material::GetMaterial("Glass"),
					"GlassFaceWCPMT",
					0,0,0);

  logicGlassFaceWCPMT->SetVisAttributes(G4VisAttributes::Invisible);

  G4double holderHeight = WCPMTExposeHeight;
  G4double holderRadius = WCPMTRadius;


  //Add Winston Cones
  G4LogicalVolume* logicWinstonConeWCPMT;

  if (addWinstonCones) {

    const G4int coneSegments = 20; //made up

    G4double coneRadiusMin = 152.4 * mm;
    G4double coneRadiusMax = 217.382 * mm;
    G4double coneVertSMA = 584.525 * mm; //semi-major axis, perp. to blacksheet
    G4double coneHorizSMA = 165.97 * mm; //semi-major axis, parallel to blacksheet
    G4double coneHorizOffset = 95.48 * mm; //distance from ellipse vertical axis to rotation axis
    
    G4double coneThickness = 1 * mm; //made up
        
    G4double coneThetaMin = asin( (coneRadiusMin-coneHorizOffset) / coneHorizSMA);
    G4double coneThetaMax = asin( (coneRadiusMax-coneHorizOffset) / coneHorizSMA);

    //set distance between blacksheet and ellipse horiz axis
    //so radiusmin is slightly above blacksheet.
    G4double coneVertOffset = coneVertSMA * cos(coneThetaMin) + .1*mm;

    G4double coneInnerRadius [coneSegments+1];
    G4double coneOuterRadius [coneSegments+1];
    G4double coneHeight [coneSegments+1];
    
    G4double coneTheta;

    //fill in the shape vectors
    for (int i=0; i <= coneSegments; i++){
      coneTheta = coneThetaMin + i*(coneThetaMax-coneThetaMin)/float(coneSegments);

      coneInnerRadius[i] = coneHorizOffset + coneHorizSMA*sin(coneTheta);
      coneHeight[i] = coneVertOffset - coneVertSMA*cos(coneTheta);

      coneOuterRadius[i] = coneInnerRadius[i] + coneThickness * sqrt(1+coneHorizSMA/coneVertSMA/tan(coneTheta));

    //G4cout << coneTheta<<G4endl;
    //G4cout << coneInnerRadius[i]<<G4endl;
    //G4cout << coneOuterRadius[i]<<G4endl;
    //G4cout << coneHeight[i]<<G4endl<<G4endl;

    }

    G4Polycone* solidWinstonConeWCPMT =
    	new G4Polycone(	"WinstonConeWCPMT",                    
				0.0*deg,
				360.0*deg,
    				coneSegments+1,
    				coneHeight,
    				coneInnerRadius,
    				coneOuterRadius);

    logicWinstonConeWCPMT =
    	new G4LogicalVolume(	solidWinstonConeWCPMT,
					G4Material::GetMaterial("Glass"),
					"WinstonConeWCPMT",
					0,0,0);

    //make space for cone
    holderHeight = max( holderHeight, coneHeight[coneSegments] );
    holderRadius = max( holderRadius, coneRadiusMax );

  }


  //All components of the PMT are now contained in a single logical volume logicWCPMT.
  //Origin is on the blacksheet, faces positive z-direction.

  G4double PMTHolderZ[2] = {0, holderHeight};
  G4double PMTHolderR[2] = {holderRadius, holderRadius};
  G4double PMTHolderr[2] = {0,0};

  G4Polycone* solidWCPMT =
   new G4Polycone("WCPMT",                    
			0.0*deg,
			360.0*deg,
    			2,
    			PMTHolderZ,
    			PMTHolderr,
    			PMTHolderR);


  logicWCPMT =
	new G4LogicalVolume(	solidWCPMT,
					G4Material::GetMaterial("Water"),
					"WCPMT",
					0,0,0);

  G4VisAttributes* WCPMTVisAtt = new G4VisAttributes(G4Colour(0.2,0.2,0.2));
  WCPMTVisAtt->SetForceWireframe(true);
  logicWCPMT->SetVisAttributes(WCPMTVisAtt);

  //put components in holder volume
  G4VPhysicalVolume* physiInteriorWCPMT =
  	new G4PVPlacement(0,
  				G4ThreeVector(0, 0, -1.0*PMTOffset),
  				logicInteriorWCPMT,
  				"InteriorWCPMT",
  				logicWCPMT,
  				false,
  				0);

  G4VPhysicalVolume* physiGlassFaceWCPMT =
  	new G4PVPlacement(0,
  				G4ThreeVector(0, 0, -1.0*PMTOffset),
  				logicGlassFaceWCPMT,
  				"GlassFaceWCPMT",
  				logicWCPMT,
  				false,
  				0);

  if (addWinstonCones) {
    G4VPhysicalVolume* physiWinstonConeWCPMT =
  	  new G4PVPlacement(	0,
  					G4ThreeVector(0, 0, 0),
  					logicWinstonConeWCPMT,
  					"WinstonConeWCPMT",
  					logicWCPMT,
  					false,
  					0);
  }


  //Add Logical Border Surface
  G4LogicalBorderSurface*  GlassCathodeSurface =
  	new G4LogicalBorderSurface(	"GlassCathodeSurface",
  						physiGlassFaceWCPMT,
  						physiInteriorWCPMT,
  						OpGlassCathodeSurface);


}

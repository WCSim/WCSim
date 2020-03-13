#include "WCSimPrimaryGeneratorAction.hh"
#include "WCSimDetectorConstruction.hh"
#include "WCSimPrimaryGeneratorMessenger.hh"

#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleTable.hh"
#include "G4IonTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4ThreeVector.hh"
#include "globals.hh"
#include "Randomize.hh"
#include <fstream>
#include <vector>
#include <string>

#include "G4Navigator.hh"
#include "G4TransportationManager.hh"

#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"

using std::vector;
using std::string;
using std::fstream;

vector<string> tokenize( string separators, string input );

inline vector<string> readInLine(fstream& inFile, int lineSize, char* inBuf)
{
  // Read in line break it up into tokens
  inFile.getline(inBuf,lineSize);
  return tokenize(" $\r", inBuf);
}

inline float atof( const string& s ) {return std::atof( s.c_str() );}
inline int   atoi( const string& s ) {return std::atoi( s.c_str() );}

WCSimPrimaryGeneratorAction::WCSimPrimaryGeneratorAction(
					  WCSimDetectorConstruction* myDC)
  :myDetector(myDC), vectorFileName("")
{
  //T. Akiri: Initialize GPS to allow for the laser use 
  MyGPS = new G4GeneralParticleSource();

  // Initialize to zero
  mode = 0;
  nvtxs = 0;
  for( Int_t u=0; u<50; u++){
    vtxsvol[u] = 0;
    vtxs[u] = G4ThreeVector(0.,0.,0.);
  }
  nuEnergy = 0.;
  _counterRock=0; // counter for generated in Rock
  _counterCublic=0; // counter generated
  
  //---Set defaults. Do once at beginning of session.
  
  G4int n_particle = 1;
  particleGun = new G4ParticleGun(n_particle);
  particleGun->SetParticleEnergy(1.0*GeV);
  particleGun->SetParticleMomentumDirection(G4ThreeVector(0.,0.,1.0));

  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4IonTable* ionTable = G4IonTable::GetIonTable();
  G4String particleName;
  particleGun->
    SetParticleDefinition(particleTable->FindParticle(particleName="mu+"));

  particleGun->
    SetParticlePosition(G4ThreeVector(0.*m,0.*m,0.*m));
    
  messenger = new WCSimPrimaryGeneratorMessenger(this);
  useMulineEvt 		= true;
  useGunEvt    		= false;
  useLaserEvt  		= false;
  useGPSEvt    		= false;
  useRadioactiveEvt  	= false;
  useRadonEvt        	= false;
  
  // Radioactive and Radon generator variables:
  radioactive_sources.clear();
  myRn222Generator	= 0;
  fRnScenario		= 0;
  fRnSymmetry		= 1;
}

WCSimPrimaryGeneratorAction::~WCSimPrimaryGeneratorAction()
{
  if (IsGeneratingVertexInRock()){
    G4cout << "Fraction of Rock volume is : " << G4endl;
      G4cout << " Random number generated in Rock / in Cublic = " 
             << _counterRock << "/" << _counterCublic 
             << " = " << _counterRock/(G4double)_counterCublic << G4endl;
  }
  inputFile.close();
  delete particleGun;
  delete MyGPS;   //T. Akiri: Delete the GPS variable
  delete messenger;
}

void WCSimPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{

  // We will need a particle table
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4IonTable* ionTable = G4IonTable::GetIonTable();

  // Temporary kludge to turn on/off vector text format 

  G4bool useNuanceTextFormat = true;


  // Do for every event

  if (useMulineEvt)
  { 

    if ( !inputFile.is_open() )
    {
      G4cout << "Set a vector file using the command /mygen/vecfile name"
	     << G4endl;
      exit(-1);
    }

    //
    // Documentation describing the nuance text format can be found here: 
    // http://neutrino.phy.duke.edu/nuance-format/
    //
    // The format must be strictly adhered to for it to be processed correctly.
    // The lines and their meanings from begin through info are fixed, and then
    // a variable number of tracks may follow.
    //
    if (useNuanceTextFormat)
      {
	const int lineSize=100;
	char      inBuf[lineSize];
	vector<string> token(1);
	
	token = readInLine(inputFile, lineSize, inBuf);
	  
        if (token.size() == 0) 
	  {
	    G4cout << "end of nuance vector file!" << G4endl;
	  }
	else if (token[0] != "begin")
	  {
	    G4cout << "unexpected line begins with " << token[0] << G4endl;
	  }
	else   // normal parsing begins here
	  {
	    // Read the nuance line (ignore value now)

	    token = readInLine(inputFile, lineSize, inBuf);
	    mode = atoi(token[1]);

	    // Read the Vertex line
	    
	    int iVertex=0;
	    
	    token = readInLine(inputFile, lineSize, inBuf);
	    
	    while(token[0]=="vertex")
	    {
                 
	    vtxs[iVertex] = G4ThreeVector(atof(token[1])*cm,
				    atof(token[2])*cm,
				    atof(token[3])*cm);
	    G4double VertexTime=atof(token[4])*ms;
	    
            // true : Generate vertex in Rock , false : Generate vertex in WC tank
            SetGenerateVertexInRock(false);

	    // Next we read the incoming neutrino and target
	    
	    // First, the neutrino line

	    token=readInLine(inputFile, lineSize, inBuf);
	    beampdgs[iVertex] = atoi(token[1]);
	    beamenergies[iVertex] = atof(token[2])*MeV;
	    beamdirs[iVertex] = G4ThreeVector(atof(token[3]),
					atof(token[4]),
					atof(token[5]));
 
	    SetBeamEnergy(beamenergies[iVertex]);
                    
	    SetBeamDir(beamdirs[iVertex]);
	    // Now read the target line

	    token=readInLine(inputFile, lineSize, inBuf);
	    targetpdgs[iVertex] = atoi(token[1]);
	    targetenergies[iVertex] = atof(token[2])*MeV;
	    targetdirs[iVertex] = G4ThreeVector(atof(token[3]),
					  atof(token[4]),
					  atof(token[5]));

	    // Read the info line, basically a dummy
	    token=readInLine(inputFile, lineSize, inBuf);
	    G4cout << "Vector File Record Number " << token[2] << G4endl;
            vecRecNumber = atoi(token[2]);
	    
	    // Now read the outgoing particles
	    // These we will simulate.


	    while ( token=readInLine(inputFile, lineSize, inBuf),
		    token[0] == "track" )
	      {
		// We are only interested in the particles
		// that leave the nucleus, tagged by "0"


		if ( token[6] == "0")
		  {
		    G4int pdgid = atoi(token[1]);
		    G4double energy = atof(token[2])*MeV;
		    G4ThreeVector dir = G4ThreeVector(atof(token[3]),
						      atof(token[4]),
						      atof(token[5]));
		    std::cout<<"PDGcode "<<pdgid<<"\n";
		    //must handle the case of an ion speratly from other particles
		    //check PDG code if we have an ion.
		    //PDG code format for ions ±10LZZZAAAI
		    char strPDG[11];
		    char strA[10]={0};
		    char strZ[10]={0};
		    

		    long int A=0,Z=0;
		    //		    A=strotl(strPDG,&str);
		    if(abs(pdgid) >= 1000000000)
		      {
			//ion
			sprintf(strPDG,"%i",abs(pdgid));
			strncpy(strZ, &strPDG[3], 3);
			strncpy(strA, &strPDG[6], 3);
			strA[3]='\0';
			strZ[3]='\0';
			A=atoi(strA);
			Z=atoi(strZ);
			G4ParticleDefinition* ion;
			ion =  ionTable->GetIon(Z, A, 0.);
			particleGun->SetParticleDefinition(ion);
			particleGun->SetParticleCharge(0);
		      }
		    else {
		      //not ion
		      particleGun->
			SetParticleDefinition(particleTable->
		      FindParticle(pdgid));
		    }
		    G4double mass = 
		      particleGun->GetParticleDefinition()->GetPDGMass();

		    G4double ekin = energy - mass;

		    particleGun->SetParticleEnergy(ekin);
		    //G4cout << "Particle: " << pdgid << " KE: " << ekin << G4endl;
		    particleGun->SetParticlePosition(vtxs[0]);
		    particleGun->SetParticleMomentumDirection(dir);
		    particleGun->SetParticleTime(VertexTime);
		    particleGun->GeneratePrimaryVertex(anEvent);
		    //SetVtx(vtxs[0]);
		    //SetBeamEnergy(ekin);
		   // SetBeamDir(dir);
		  }
	      }
	       iVertex++;
	    }
	    nvtxs=iVertex;
	  }
      }
    else 
      {    // old muline format  
	inputFile >> nuEnergy >> energy >> xPos >> yPos >> zPos 
		  >> xDir >> yDir >> zDir;
	
	G4double random_z = ((myDetector->GetWaterTubePosition())
			     - .5*(myDetector->GetWaterTubeLength()) 
			     + 1.*m + 15.0*m*G4UniformRand())/m;
	zPos = random_z;
	G4ThreeVector vtx = G4ThreeVector(xPos, yPos, random_z);
	G4ThreeVector dir = G4ThreeVector(xDir,yDir,zDir);

	particleGun->SetParticleEnergy(energy*MeV);
	particleGun->SetParticlePosition(vtx);
	particleGun->SetParticleMomentumDirection(dir);
	particleGun->GeneratePrimaryVertex(anEvent);
      }
  }

  else if (useGunEvt)
  {      // manual gun operation
    particleGun->GeneratePrimaryVertex(anEvent);

    //To prevent occasional seg fault from an un assigned targetpdg 
    targetpdgs[0] = 2212; //ie. proton

    G4ThreeVector P  =anEvent->GetPrimaryVertex()->GetPrimary()->GetMomentum();
    G4ThreeVector vtx=anEvent->GetPrimaryVertex()->GetPosition();
    G4double m       =anEvent->GetPrimaryVertex()->GetPrimary()->GetMass();
    G4int pdg        =anEvent->GetPrimaryVertex()->GetPrimary()->GetPDGcode();

    char strPDG[11];
    char strA[10]={0};
    char strZ[10]={0};
    
    
    long int A=0,Z=0;
    //		    A=strotl(strPDG,&str);
    if(abs(pdg) >= 1000000000)
      {
	//ion
	sprintf(strPDG,"%i",abs(pdg));
	strncpy(strZ, &strPDG[3], 3);
	strncpy(strA, &strPDG[6], 3);
	strA[3]='\0';
	strZ[3]='\0';
	A=atoi(strA);
	Z=atoi(strZ);

	G4ParticleDefinition* ion   = G4IonTable::GetIonTable()->GetIon(Z, A, 0);
	ion->SetPDGStable(false);
	ion->SetPDGLifeTime(0.);
	
	G4ParticleDefinition* ion2   = G4IonTable::GetIonTable()->GetIon(Z, A, 0);
	std::cout<<"ion2 "<<ion2->GetPDGLifeTime()<<"\n";
      }
    
    
    G4ThreeVector dir  = P.unit();
    G4double E         = std::sqrt((P.dot(P))+(m*m));

//     particleGun->SetParticleEnergy(E);
//     particleGun->SetParticlePosition(vtx);
//     particleGun->SetParticleMomentumDirection(dir);

    SetVtx(vtx);
    SetBeamEnergy(E);
    SetBeamDir(dir);
    SetBeamPDG(pdg);
  }
  else if (useLaserEvt)
    {
      targetpdgs[0] = 2212; //ie. proton 
      //T. Akiri: Create the GPS LASER event
      MyGPS->GeneratePrimaryVertex(anEvent);
      
      G4ThreeVector P   =anEvent->GetPrimaryVertex()->GetPrimary()->GetMomentum();
      G4ThreeVector vtx =anEvent->GetPrimaryVertex()->GetPosition();
      G4int pdg         =anEvent->GetPrimaryVertex()->GetPrimary()->GetPDGcode();
      
      //     G4ThreeVector dir  = P.unit();
      G4double E         = std::sqrt((P.dot(P)));
      
      //SetVtx(vtx);
      SetBeamEnergy(E);
      //SetBeamDir(dir);
      SetBeamPDG(pdg);
    }
  else if (useGPSEvt)
    {
      MyGPS->GeneratePrimaryVertex(anEvent);
      
      G4ThreeVector P   =anEvent->GetPrimaryVertex()->GetPrimary()->GetMomentum();
      G4ThreeVector vtx =anEvent->GetPrimaryVertex()->GetPosition();
      G4double m        =anEvent->GetPrimaryVertex()->GetPrimary()->GetMass();
      G4int pdg         =anEvent->GetPrimaryVertex()->GetPrimary()->GetPDGcode();
      
      G4ThreeVector dir  = P.unit();
      G4double E         = std::sqrt((P.dot(P))+(m*m));
      
      SetVtx(vtx);
      SetBeamEnergy(E);
      SetBeamDir(dir);
      SetBeamPDG(pdg);
    }
  else if (useRadioactiveEvt)
    {
      
      // initialize GPS properties
      MyGPS->ClearAll();
      
      MyGPS->SetMultipleVertex(true);
      
      std::vector<WCSimPmtInfo*> *pmts;
      
      std::vector<struct radioactive_source>::iterator it;
      
      for ( it = radioactive_sources.begin(); it != radioactive_sources.end(); it++ ){
	G4String IsotopeName = it->IsotopeName;
	G4String IsotopeLocation = it->IsotopeLocation;
	G4double IsotopeActivity = it->IsotopeActivity;

	double average= IsotopeActivity * GetRadioactiveTimeWindow();
	if (IsotopeLocation.compareTo("PMT") == 0){
	  pmts = myDetector->Get_Pmts();
	  average *= pmts->size();
	}
	  
	// random poisson number of vertices based on average
	int n_vertices = CLHEP::RandPoisson::shoot(average);

	//	n_vertices = 1; // qqq

	for(int u=0; u<n_vertices; u++){
	    
	  MyGPS->AddaSource(1.);
	    
	  MyGPS->SetCurrentSourceto(MyGPS->GetNumberofSource() - 1);

	  if (IsotopeName.compareTo("Tl208") == 0){
	    MyGPS->SetParticleDefinition(G4IonTable::GetIonTable()->GetIon( 81, 208, 0));
	  }else if (IsotopeName.compareTo("Bi214") == 0){
	    MyGPS->SetParticleDefinition(G4IonTable::GetIonTable()->GetIon( 83, 214, 0));
	  }else if (IsotopeName.compareTo("K40") == 0){
	    MyGPS->SetParticleDefinition(G4IonTable::GetIonTable()->GetIon( 19, 40, 0));
	  }else if (IsotopeName.compareTo("Rn220") == 0){
	    MyGPS->SetParticleDefinition(G4IonTable::GetIonTable()->GetIon( 86, 220, 0));
	  }else if (IsotopeName.compareTo("Po216") == 0){
	    MyGPS->SetParticleDefinition(G4IonTable::GetIonTable()->GetIon( 84, 216, 0));
	  }else if (IsotopeName.compareTo("Pb212") == 0){
	    MyGPS->SetParticleDefinition(G4IonTable::GetIonTable()->GetIon( 82, 212, 0));
	  }else if (IsotopeName.compareTo("Bi212") == 0){
	    MyGPS->SetParticleDefinition(G4IonTable::GetIonTable()->GetIon( 83, 212, 0));
	  }else if (IsotopeName.compareTo("Po212") == 0){
	    MyGPS->SetParticleDefinition(G4IonTable::GetIonTable()->GetIon( 84, 212, 0));
	  }else if (IsotopeName.compareTo("Rn222") == 0){
	    MyGPS->SetParticleDefinition(G4IonTable::GetIonTable()->GetIon( 86, 222, 0));
	  }else if (IsotopeName.compareTo("Po218") == 0){
	    MyGPS->SetParticleDefinition(G4IonTable::GetIonTable()->GetIon( 84, 218, 0));
	  }else if (IsotopeName.compareTo("At218") == 0){
	    MyGPS->SetParticleDefinition(G4IonTable::GetIonTable()->GetIon( 85, 218, 0));
	  }else if (IsotopeName.compareTo("Pb214") == 0){
	    MyGPS->SetParticleDefinition(G4IonTable::GetIonTable()->GetIon( 82, 214, 0));
	  }else if (IsotopeName.compareTo("Po214") == 0){
	    MyGPS->SetParticleDefinition(G4IonTable::GetIonTable()->GetIon( 84, 214, 0));
	  }else if (IsotopeName.compareTo("Tl210") == 0){
	    MyGPS->SetParticleDefinition(G4IonTable::GetIonTable()->GetIon( 81, 210, 0));
	  }else if (IsotopeName.compareTo("Pb210") == 0){
	    MyGPS->SetParticleDefinition(G4IonTable::GetIonTable()->GetIon( 82, 210, 0));
	  }else if (IsotopeName.compareTo("Bi210") == 0){
	    MyGPS->SetParticleDefinition(G4IonTable::GetIonTable()->GetIon( 83, 210, 0));
	  }else if (IsotopeName.compareTo("Po210") == 0){
	    MyGPS->SetParticleDefinition(G4IonTable::GetIonTable()->GetIon( 84, 210, 0));
	  }else if (IsotopeName.compareTo("Hg206") == 0){
	    MyGPS->SetParticleDefinition(G4IonTable::GetIonTable()->GetIon( 80, 206, 0));
	  }else if (IsotopeName.compareTo("Tl206") == 0){
	    MyGPS->SetParticleDefinition(G4IonTable::GetIonTable()->GetIon( 81, 206, 0));
	  }else if (IsotopeName.compareTo("Rn219") == 0){
	    MyGPS->SetParticleDefinition(G4IonTable::GetIonTable()->GetIon( 86, 219, 0));
	  }else if (IsotopeName.compareTo("Po215") == 0){
	    MyGPS->SetParticleDefinition(G4IonTable::GetIonTable()->GetIon( 84, 215, 0));
	  }else if (IsotopeName.compareTo("At215") == 0){
	    MyGPS->SetParticleDefinition(G4IonTable::GetIonTable()->GetIon( 85, 215, 0));
	  }else if (IsotopeName.compareTo("Pb211") == 0){
	    MyGPS->SetParticleDefinition(G4IonTable::GetIonTable()->GetIon( 82, 211, 0));
	  }else if (IsotopeName.compareTo("Bi211") == 0){
	    MyGPS->SetParticleDefinition(G4IonTable::GetIonTable()->GetIon( 83, 211, 0));
	  }else if (IsotopeName.compareTo("Po211") == 0){
	    MyGPS->SetParticleDefinition(G4IonTable::GetIonTable()->GetIon( 84, 211, 0));
	  }else if (IsotopeName.compareTo("Tl207") == 0){
	    MyGPS->SetParticleDefinition(G4IonTable::GetIonTable()->GetIon( 81, 207, 0));
	  }else if (IsotopeName.compareTo("Th232") == 0){
	    MyGPS->SetParticleDefinition(G4IonTable::GetIonTable()->GetIon( 90, 232, 0));
	  }else if (IsotopeName.compareTo("Ra228") == 0){
	    MyGPS->SetParticleDefinition(G4IonTable::GetIonTable()->GetIon( 88, 228, 0));
	  }else if (IsotopeName.compareTo("Ac228") == 0){
	    MyGPS->SetParticleDefinition(G4IonTable::GetIonTable()->GetIon( 89, 228, 0));
	  }else if (IsotopeName.compareTo("Th228") == 0){
	    MyGPS->SetParticleDefinition(G4IonTable::GetIonTable()->GetIon( 90, 228, 0));
	  }else if (IsotopeName.compareTo("Ra224") == 0){
	    MyGPS->SetParticleDefinition(G4IonTable::GetIonTable()->GetIon( 88, 224, 0));
	  }else if (IsotopeName.compareTo("U238") == 0){
	    MyGPS->SetParticleDefinition(G4IonTable::GetIonTable()->GetIon( 92, 238, 0));
	  }else if (IsotopeName.compareTo("Th234") == 0){
	    MyGPS->SetParticleDefinition(G4IonTable::GetIonTable()->GetIon( 90, 234, 0));
	  }else if (IsotopeName.compareTo("Pa234") == 0){
	    MyGPS->SetParticleDefinition(G4IonTable::GetIonTable()->GetIon( 91, 234, 0));
	  }else if (IsotopeName.compareTo("U234") == 0){
	    MyGPS->SetParticleDefinition(G4IonTable::GetIonTable()->GetIon( 92, 234, 0));
	  }else if (IsotopeName.compareTo("Th230") == 0){
	    MyGPS->SetParticleDefinition(G4IonTable::GetIonTable()->GetIon( 90, 230, 0));
	  }else if (IsotopeName.compareTo("Ra226") == 0){
	    MyGPS->SetParticleDefinition(G4IonTable::GetIonTable()->GetIon( 88, 226, 0));
	  }else if (IsotopeName.compareTo("U235") == 0){
	    MyGPS->SetParticleDefinition(G4IonTable::GetIonTable()->GetIon( 92, 235, 0));
	  }else if (IsotopeName.compareTo("Th231") == 0){
	    MyGPS->SetParticleDefinition(G4IonTable::GetIonTable()->GetIon( 90, 231, 0));
	  }else if (IsotopeName.compareTo("Pa231") == 0){
	    MyGPS->SetParticleDefinition(G4IonTable::GetIonTable()->GetIon( 91, 231, 0));
	  }else if (IsotopeName.compareTo("Ac227") == 0){
	    MyGPS->SetParticleDefinition(G4IonTable::GetIonTable()->GetIon( 89, 227, 0));
	  }else if (IsotopeName.compareTo("Th227") == 0){
	    MyGPS->SetParticleDefinition(G4IonTable::GetIonTable()->GetIon( 90, 227, 0));
	  }else if (IsotopeName.compareTo("Fr223") == 0){
	    MyGPS->SetParticleDefinition(G4IonTable::GetIonTable()->GetIon( 87, 223, 0));
	  }else if (IsotopeName.compareTo("Ra223") == 0){
	    MyGPS->SetParticleDefinition(G4IonTable::GetIonTable()->GetIon( 88, 223, 0));
	  }else if (IsotopeName.compareTo("At219") == 0){
	    MyGPS->SetParticleDefinition(G4IonTable::GetIonTable()->GetIon( 85, 219, 0));
	  }else if (IsotopeName.compareTo("Bi215") == 0){
	    MyGPS->SetParticleDefinition(G4IonTable::GetIonTable()->GetIon( 83, 215, 0));
	  }

	  if (IsotopeLocation.compareTo("water") == 0){
	    MyGPS->GetCurrentSource()->GetEneDist()->SetEnergyDisType("Mono");
	    MyGPS->GetCurrentSource()->GetEneDist()->SetMonoEnergy(0.);
	    MyGPS->GetCurrentSource()->GetPosDist()->SetPosDisType("Point");
	    MyGPS->GetCurrentSource()->GetPosDist()->SetCentreCoords(G4ThreeVector(0, 0, 0));
	    MyGPS->GetCurrentSource()->GetPosDist()->SetPosDisType("Volume");
	    MyGPS->GetCurrentSource()->GetPosDist()->SetPosDisShape("Cylinder");
	    G4String WCIDCollectionName = myDetector->GetIDCollectionName();
	    WCSimPMTObject *PMT = myDetector->GetPMTPointer(WCIDCollectionName);
	    MyGPS->GetCurrentSource()->GetPosDist()->SetRadius(myDetector->GetGeo_Dm(3)*CLHEP::cm - 2.*PMT->GetRadius());
	    MyGPS->GetCurrentSource()->GetPosDist()->SetHalfZ(myDetector->GetGeo_Dm(2)*CLHEP::cm/2. - 2.*PMT->GetRadius());
	    MyGPS->GetCurrentSource()->GetPosDist()->SetPosRot1(G4ThreeVector(1, 0, 0));
	    MyGPS->GetCurrentSource()->GetPosDist()->SetPosRot2(G4ThreeVector(0, 1, 0));

	  }
	  else if (IsotopeLocation.compareTo("PMT") == 0){
	    int npmts = pmts->size();
	    int random_pmt_id = CLHEP::RandFlat::shootInt(1,npmts);
	    WCSimPmtInfo* pmtinfo = (WCSimPmtInfo*)pmts->at( random_pmt_id - 1 );
	    G4ThreeVector random_pmt_center(pmtinfo->Get_transx()*CLHEP::cm, pmtinfo->Get_transy()*CLHEP::cm, pmtinfo->Get_transz()*CLHEP::cm);
	    double random_cos_theta = CLHEP::RandFlat::shoot(0., 1.);
	    double random_sin_theta = sqrt(1. - pow(random_cos_theta,2));
	    random_sin_theta *= (CLHEP::RandFlat::shootBit() == 0 ? -1 : 1);
	    double random_phi = CLHEP::RandFlat::shoot(0., 2.*CLHEP::pi*CLHEP::rad);
	    G4String WCIDCollectionName = myDetector->GetIDCollectionName();
	    WCSimPMTObject *PMT = myDetector->GetPMTPointer(WCIDCollectionName);
	    double PMT_radius = PMT->GetRadius();
	    double glassThickness = PMT->GetPMTGlassThickness();
	    double expose = PMT->GetExposeHeight();
	    double sphereRadius = (expose*expose+ PMT_radius*PMT_radius)/(2*expose);
	    double Rmin = sphereRadius-glassThickness;
	    double Rmax = sphereRadius;
	    double random_R = CLHEP::RandFlat::shoot(Rmin, Rmax);
	    G4ThreeVector orientation(pmtinfo->Get_orienx(), pmtinfo->Get_orieny(), pmtinfo->Get_orienz());
	    G4ThreeVector axis_1 = orientation.orthogonal();
	    G4ThreeVector axis_2 = orientation.cross(axis_1);
	    G4ThreeVector position = random_pmt_center + random_R*(orientation*random_cos_theta + axis_1*random_sin_theta*cos(random_phi) + axis_2*random_sin_theta*sin(random_phi));
	      
	    //G4cout << " random id " << random_pmt_id << " of " << npmts << " costheta " << random_cos_theta << " sintheta " << random_sin_theta << " phi " << random_phi << " WCIDCollectionName " << WCIDCollectionName << " PMT_radius " << PMT_radius << " expose " << expose << " sphereRadius " << sphereRadius << " Rmin " << Rmin << " Rmax " << Rmax << " random_R " << random_R << " orientation (" << orientation.x() << ", " << orientation.y() << ", " << orientation.z() << ") center (" << random_pmt_center.x() << ", " << random_pmt_center.y() << ", " << random_pmt_center.z() << ") position (" << position.x() << ", " << position.y() << ", " << position.z() << ") " << G4endl;
	      
	    MyGPS->GetCurrentSource()->GetEneDist()->SetEnergyDisType("Mono");
	    MyGPS->GetCurrentSource()->GetEneDist()->SetMonoEnergy(0.);
	    MyGPS->GetCurrentSource()->GetPosDist()->SetPosDisType("Point");
	    MyGPS->GetCurrentSource()->GetPosDist()->SetCentreCoords(position);
	  }
	    
	}

//	G4cout << " is " << IsotopeName << " of " << radioactive_sources.size() << " loc " << IsotopeLocation << " a " << IsotopeActivity << " nv " << n_vertices << G4endl;

      }

      G4int number_of_sources = MyGPS->GetNumberofSource();

      // this will generate several primary vertices
      MyGPS->GeneratePrimaryVertex(anEvent);

      SetNvtxs(number_of_sources);
      for( G4int u=0; u<number_of_sources; u++){
	targetpdgs[u] = 2212; //ie. proton 

      	G4ThreeVector P   =anEvent->GetPrimaryVertex(u)->GetPrimary()->GetMomentum();
      	G4ThreeVector vtx =anEvent->GetPrimaryVertex(u)->GetPosition();
      	G4int pdg         =anEvent->GetPrimaryVertex(u)->GetPrimary()->GetPDGcode();
      
      	//       G4ThreeVector dir  = P.unit();
      	G4double E         = std::sqrt((P.dot(P)));

//	G4cout << " vertex " << u << " of " << number_of_sources << " (" << vtx.x() << ", " << vtx.y() << ", " << vtx.z() << ")" << G4endl;

      	SetVtxs(u,vtx);
      	SetBeamEnergy(E,u);
      	//       SetBeamDir(dir);
      	SetBeamPDG(pdg,u);
      }

    }
  else if (useRadonEvt)
    { //G. Pronost: Add Radon (adaptation of Radioactive event)
    
      // Currently only one generator is possible
      // In order to have several, we need to find a solution for the fitting graphes (which are static currently)
      // Idea: array of fitting graphes? (each new generators having a specific ID)
      if ( !myRn222Generator ) {
      	myRn222Generator = new WCSimGenerator_Radioactivity(myDetector);
      	myRn222Generator->Configuration(fRnScenario);
      }
      
      //G4cout << " Generate radon events " << G4endl;
      // initialize GPS properties
      MyGPS->ClearAll();
      
      MyGPS->SetMultipleVertex(true);
      
      std::vector<WCSimPmtInfo*> *pmts;
      
      std::vector<struct radioactive_source>::iterator it;
      
      G4String IsotopeName = "Rn222";
      G4double IsotopeActivity = myRn222Generator->GetMeanActivity() * 1e-3; // mBq to Bq
      G4double iEventAvg = IsotopeActivity * GetRadioactiveTimeWindow();

      //G4cout << " Average " << iEventAvg << G4endl;
      // random poisson number of vertices based on average
      int n_vertices = CLHEP::RandPoisson::shoot(iEventAvg);
      //G4cout << " Vtx " << n_vertices << G4endl;

      if ( n_vertices < 1 ) {
      	 n_vertices = 1;
      }
      
      for(int u=0; u<n_vertices; u++){
	
	MyGPS->AddaSource(1.);	
	MyGPS->SetCurrentSourceto(MyGPS->GetNumberofSource() - 1);
	
	// Bi214 (source of electron in Rn222 decay chain, assumed to be in equilibrium)
	MyGPS->SetParticleDefinition(G4IonTable::GetIonTable()->GetIon( 83, 214, 0));
	
	// Get position (first position take few seconds to be produced, there after there is no trouble)
	//G4cout << "GetRandomVertex" << G4endl;
	G4ThreeVector position = myRn222Generator->GetRandomVertex(fRnSymmetry);
	//G4cout << "Done: " << position << G4endl;
	// energy 
	MyGPS->GetCurrentSource()->GetEneDist()->SetEnergyDisType("Mono");
	MyGPS->GetCurrentSource()->GetEneDist()->SetMonoEnergy(0.);
	    
	// position 
	MyGPS->GetCurrentSource()->GetPosDist()->SetPosDisType("Point");
	MyGPS->GetCurrentSource()->GetPosDist()->SetCentreCoords(position);

	//G4cout << u << " is " << IsotopeName << " loc " << position  << G4endl;

      }
      G4int number_of_sources = MyGPS->GetNumberofSource();

      // this will generate several primary vertices
      MyGPS->GeneratePrimaryVertex(anEvent);

      SetNvtxs(number_of_sources);
      for( G4int u=0; u<number_of_sources; u++){
	targetpdgs[u] = 2212; //ie. proton 

      	G4ThreeVector P   =anEvent->GetPrimaryVertex(u)->GetPrimary()->GetMomentum();
      	G4ThreeVector vtx =anEvent->GetPrimaryVertex(u)->GetPosition();
      	G4int pdg         =anEvent->GetPrimaryVertex(u)->GetPrimary()->GetPDGcode();
      
      	//       G4ThreeVector dir  = P.unit();
      	G4double E         = std::sqrt((P.dot(P)));
	
	//G4cout << " vertex " << u << " of " << number_of_sources << " (" << vtx.x() << ", " << vtx.y() << ", " << vtx.z() << ") with pdg: " << pdg << G4endl;

      	SetVtxs(u,vtx);
      	SetBeamEnergy(E,u);
      	//       SetBeamDir(dir);
      	SetBeamPDG(pdg,u);
      }

    }
}

void WCSimPrimaryGeneratorAction::SaveOptionsToOutput(WCSimRootOptions * wcopt)
{
  if(useMulineEvt)
    wcopt->SetVectorFileName(vectorFileName);
  else
    wcopt->SetVectorFileName("");
  wcopt->SetGeneratorType(GetGeneratorTypeString());
}

G4String WCSimPrimaryGeneratorAction::GetGeneratorTypeString()
{
  if(useMulineEvt)
    return "muline";
  else if(useGunEvt)
    return "gun";
  else if(useGPSEvt)
    return "gps";
  else if(useLaserEvt)
    return "laser";
  return "";
}

// Returns a vector with the tokens
vector<string> tokenize( string separators, string input ) 
{
  std::size_t startToken = 0, endToken; // Pointers to the token pos
  vector<string> tokens;  // Vector to keep the tokens
  
  if( separators.size() > 0 && input.size() > 0 ) 
    {
    
      while( startToken < input.size() )
	{
	  // Find the start of token
	  startToken = input.find_first_not_of( separators, startToken );
      
	  // If found...
	  if( startToken != input.npos ) 
	    {
	      // Find end of token
	      endToken = input.find_first_of( separators, startToken );
	      if( endToken == input.npos )
		// If there was no end of token, assign it to the end of string
		endToken = input.size();
        
	      // Extract token
	      tokens.push_back( input.substr( startToken, endToken - startToken ) );
        
	      // Update startToken
	      startToken = endToken;
	    }
	}
    }
  
  return tokens;
}


#include "WCSimPrimaryGeneratorAction.hh"
#include "HepMC3/FourVector.h"
#include "HepMC3/GenParticle.h"
#include "HepMC3/GenParticle_fwd.h"
#include "HepMC3/GenVertex.h"
#include "HepMC3/GenVertex_fwd.h"
#include "WCSimDetectorConstruction.hh"
#include "WCSimPrimaryGeneratorMessenger.hh"
#include "G4RunManager.hh"
#include "G4Event.hh"
#include "G4ParticleGun.hh"
#include "G4GeneralParticleSource.hh"
#include "G4ParticleTable.hh"
#include "G4IonTable.hh"
#include "G4ParticleDefinition.hh"
#include "G4IonTable.hh"
#include "G4ThreeVector.hh"
#include "G4Vector3D.hh"
#include "G4EventManager.hh"
#include "globals.hh"
#include <G4LorentzVector.hh>
#include <G4Types.hh>
#include <G4ios.hh>
#include "Randomize.hh"
#include <fstream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <TFile.h>

//#include "WCSimEnumerations.hh"

#include "G4Navigator.hh"
#include "G4TransportationManager.hh"

#include "TRandom3.h"
#include "G4PhysicalConstants.hh"
#include "G4SystemOfUnits.hh"
//#define PAIR//B.Q
using std::vector;
using std::string;
using std::fstream;

vector<string> tokenize( string separators, string input );

inline vector<string> readInLine(fstream& inFile, int lineSize, char* inBuf)
{
  // Read in line break it up into tokens
  // Any line starting with # is ignored
	while(true)
	{
		if (inFile.getline(inBuf,lineSize))
		{
			if(inBuf[0]!='#')
				return tokenize(" $\r", inBuf);
		}
		else
		{
		  if(inFile.fail())
		    G4cerr << "Failed to read line. Is the buffer size large enough?" << G4endl;
			vector<string> nullLine;
			return nullLine;
		}
	}
}

inline double atof( const string& str ) {return std::atof( str.c_str() );}
inline int    atoi( const string& str ) {return std::atoi( str.c_str() );}

WCSimPrimaryGeneratorAction::WCSimPrimaryGeneratorAction(
							 WCSimDetectorConstruction* myDC)
  :myDetector(myDC), vectorFileName("")
{
  //T. Akiri: Initialize GPS to allow for the laser use
  MyGPS = new G4GeneralParticleSource();

  // Initialize to zero
  mode[0] = 0;
  nvtxs = 0;
  for( Int_t u=0; u<MAX_N_VERTICES; u++){
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
//  G4IonTable* ionTable = G4IonTable::GetIonTable();
  G4String particleName;
  particleGun->
    SetParticleDefinition(particleTable->FindParticle(particleName="mu+"));

  particleGun->
    SetParticlePosition(G4ThreeVector(0.*m,0.*m,0.*m));

  messenger = new WCSimPrimaryGeneratorMessenger(this);

  useMulineEvt 		    = true;
  useAmBeEvt          = false;
  useRootrackerEvt   	= false;
  useGunEvt    		    = false;
  useLaserEvt  		    = false;
  useInjectorEvt    	= false;
  useGPSEvt      		  = false;
  useDataTableEvt     = false;
  useIBDEvt           = false;
  useHepMC3Evt        = false;
  useCosmics          = false;
  useRadioactiveEvt   = false;
  useRadonEvt         = false;
  useLightInjectorEvt = false;
  useMPMTledEvt       = false;

  //rootracker related variables
  fEvNum = 0;
  fInputRootrackerFile = NULL;
  fNEntries = 1;
	  
  needConversion = false;
  foundConversion = true;	  

  // Radioactive and Radon generator variables:
  radioactive_sources.clear();
  myRn222Generator	= 0;
  fRnScenario		= 0;
  fRnSymmetry		= 1;

  //injector related variables
  nPhotons = 1;
  injectorOnIdx = 0;
  twindow = 0.;
  openangle = 0.;
  wavelength = 435.;

  //Light injector generator variables (LIGen)
  LIGen = 0;
  injectorType = "";
  injectorIdx = "";
  injectorFilename = "";
  photonMode = 0;

  mPMTLEDId1 = 1;
  mPMTLEDId2 = 0;
  mPMTLED_dTheta = 0.;
  mPMTLED_dPhi = 0.;
  opticalphoton_pd = particleTable->FindParticle("opticalphoton");

  // Time units for vertices
  fTimeUnit=CLHEP::nanosecond;
}


void WCSimPrimaryGeneratorAction::Create_cosmics_histogram(){
  // Create the relevant histograms to generate muons
  // according to SuperK flux extrapolated at HyperK site

  altCosmics = myDetector->GetWCIDHeight();
  G4cout << "altCosmics : " << altCosmics << G4endl;
  if (inputCosmicsFile.is_open()) inputCosmicsFile.close();

  inputCosmicsFile.open(cosmicsFileName, std::fstream::in);

  if (!inputCosmicsFile.is_open()) {
    G4cout << "Cosmics data file " << cosmicsFileName << " not found" << G4endl;
    exit(-1);
  } 
  else {
    G4cout << "Cosmics data file " << cosmicsFileName << " found" << G4endl;
    string line;
    vector<string> token(1);

    double binCos, binPhi;
    //double cosThetaMean, cosThetaMin, cosThetaMax;
    //double phiMean, phiMin, phiMax;
    double flux;
    double Emean;

    hFluxCosmics = new TH2D("hFluxCosmics","HK Flux", 180,0,360,100,0,1);
    hFluxCosmics->GetXaxis()->SetTitle("#phi (deg)");
    hFluxCosmics->GetYaxis()->SetTitle("cos #theta");
    hFluxCosmics->SetDirectory(0);
    hEmeanCosmics = new TH2D("hEmeanCosmics","HK Flux", 180,0,360,100,0,1);
    hEmeanCosmics->GetXaxis()->SetTitle("#phi (deg)");
    hEmeanCosmics->GetYaxis()->SetTitle("cos #theta");
    hEmeanCosmics->SetDirectory(0);

    while ( getline(inputCosmicsFile,line) ){
      token = tokenize(" $", line);

      binCos=(atof(token[0]));
      binPhi=(atof(token[1]));
      /*
      cosThetaMean=(atof(token[2]));
      cosThetaMin=(atof(token[3]));
      cosThetaMax=(atof(token[4]));
      phiMean=(atof(token[5]));
      phiMin=(atof(token[6]));
      phiMax=(atof(token[7]));
      */
      flux=(atof(token[8]));
      Emean=(atof(token[9]));

      hFluxCosmics->SetBinContent(binPhi+1,binCos+1,flux);
      hEmeanCosmics->SetBinContent(binPhi+1,binCos+1,Emean);
    }

    TFile *file = new TFile("cosmicflux.root","RECREATE");
    hFluxCosmics->Write();
    hEmeanCosmics->Write();
    file->Close();
  }
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
  inputCosmicsFile.close();

  if(useRootrackerEvt) delete fRooTrackerTree;
  if (myRn222Generator) delete myRn222Generator;

  delete particleGun;
  delete MyGPS;   //T. Akiri: Delete the GPS variable
  delete messenger;
  if (useCosmics){
    delete hFluxCosmics;
    delete hEmeanCosmics;
  }
  if(LIGen) delete LIGen;
  if(IBDGen) delete IBDGen;
}

void WCSimPrimaryGeneratorAction::GeneratePrimaries(G4Event* anEvent)
{
  // We will need a particle table
  G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
  G4IonTable* ionTable = G4IonTable::GetIonTable();

  // Temporary kludge to turn on/off vector text format
  G4bool useNuanceTextFormat = true;

  // Do for every event
  if (useMulineEvt) {
    if ( !inputFile.is_open() ) {
      G4cout << "Set a vector file using the command /mygen/vecfile name"
	     << G4endl;
      exit(-1);
    }

    // The original documentation describing the nuance text format can be found here:
    // http://neutrino.phy.duke.edu/nuance-format/
    //
    // Information specific to WCSim can be found in the file Nuance_MC_Format.txt in
    // the doc/ directory.
    // The format must be strictly adhered to for it to be processed correctly.
    // The lines and their meanings from begin through info are fixed, and then
    // a variable number of tracks may follow.
    //
    if (useNuanceTextFormat) {
      const int lineSize=1000;
      char      inBuf[lineSize];
      vector<string> token(1);

      token = readInLine(inputFile, lineSize, inBuf);

      if (token.size() == 0 || token[0] == "stop") {
	G4cout << "End of nuance vector file - run terminated..."<< G4endl;
	G4RunManager::GetRunManager()-> AbortRun();
      }
      else if (token[0] != "begin") {
	G4cout << "unexpected line begins with \"" << token[0]
	       << "\"we were expecting \" begin \"" << G4endl;
      }
      else {   // normal parsing begins here
	// Read the nuance line
	// should be nuance <value>
	// but could be just
	// nuance
	// if value is given set mode to equal it.

	token = readInLine(inputFile, lineSize, inBuf);
	int iVertex=0;
	while(token[0]=="nuance" && iVertex < MAX_N_VERTICES) {
	  if(token.size()>1)
	    mode[iVertex] = atoi(token[1]);

	  // Read the Vertex line
	  // - this contains position and time
	  token = readInLine(inputFile, lineSize, inBuf);
	  vtxs[iVertex] = G4ThreeVector(atof(token[1])*cm,
					atof(token[2])*cm,
					atof(token[3])*cm);
	  G4double VertexTime=atof(token[4])*fTimeUnit;
	  vertexTimes[iVertex]=VertexTime;

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
	  G4cout << "Neutrino generated is = "<< beampdgs[iVertex]<<", Enu = " << beamenergies[iVertex] << " and interacts through mode = " << mode[iVertex] << G4endl;

	  // Now read the target line

	  //B.Q: there can be some cases (2p2h i.e. neut mode = 2) where there are 2 targets. The while loop is added for this purpose.
	  // Note that the information of the 1st target is lost
	  while ( token=readInLine(inputFile, lineSize, inBuf),
		  token[0] == "track" ) {
	    targetpdgs[iVertex] = atoi(token[1]);
	    targetenergies[iVertex] = atof(token[2])*MeV;
	    targetdirs[iVertex] = G4ThreeVector(atof(token[3]),
						atof(token[4]),
						atof(token[5]));
	    G4cout << "Target hit is = "<< targetpdgs[iVertex] <<", E = " << targetenergies[iVertex] << G4endl;
	  }//loop over target lines

	  // The info line is read in the exiting step of the while loop aboe
	  // The info line is (almost) a dummy
	  G4cout << "Vector File Record Number " << token[2] << G4endl;
	  vecRecNumber = atoi(token[2]);

	  // Now read the outgoing particles
	  // These we will simulate.
	  while ( token=readInLine(inputFile, lineSize, inBuf),
		  token[0] == "track" ) {
	    // We are only interested in the particles
	    // that leave the nucleus, tagged by "0"
	    if ( token[6] == "0") {
	      G4int pdgid = atoi(token[1]);
	      G4double energy_total = atof(token[2])*MeV;
	      G4ThreeVector dir = G4ThreeVector(atof(token[3]),
						atof(token[4]),
						atof(token[5]));

	      //must handle the case of an ion seperatly from other particles
	      //check PDG code if we have an ion.
	      //PDG code format for ions ±10LZZZAAAI
	      if(abs(pdgid) >= 1000000000){
		//ion
		char strPDG[11];
		char strA[10]={0};
		char strZ[10]={0};
		long int A=0,Z=0;
		sprintf(strPDG,"%i",abs(pdgid));
		//stop GCC complaining about string truncation
		// - we're copying from the middle of a long string
		// - we do terminate the string correctly below
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
		strncpy(strZ, &strPDG[3], 3);
#pragma GCC diagnostic pop
		strncpy(strA, &strPDG[6], 3);
		strA[3]='\0';
		strZ[3]='\0';
		A=atoi(strA);
		Z=atoi(strZ);
		G4ParticleDefinition* ion;
		ion =  ionTable->GetIon(Z, A, 0.);
		particleGun->SetParticleDefinition(ion);
		particleGun->SetParticleCharge(0);
	      }//ion
	      else {
		//not ion
		particleGun->
		  SetParticleDefinition(particleTable->
					FindParticle(pdgid));
	      }//not ion

	      G4double mass =
		particleGun->GetParticleDefinition()->GetPDGMass();

	      G4double ekin = energy_total - mass;
	      G4cout << "Generating particle = "<< pdgid << ", E = " << energy_total << " MeV, Ec = " << ekin <<  " MeV, and dir = " << dir[0] << ", " << dir[1] << ", " << dir[2] << G4endl;
	      particleGun->SetParticleEnergy(ekin);
	      particleGun->SetParticlePosition(vtxs[iVertex]);
	      particleGun->SetParticleMomentumDirection(dir);
	      particleGun->SetParticleTime(VertexTime);
	      particleGun->GeneratePrimaryVertex(anEvent);
	    }//token[6] == "0" (particle exiting nucleus)
	  }//loop over "track" lines
	  iVertex++;
	  if(iVertex > MAX_N_VERTICES)
	    G4cout<<" CAN NOT DEAL WITH MORE THAN "<<MAX_N_VERTICES
		  <<" VERTICES - TRUNCATING EVENT HERE "<<G4endl;
	}//loop over vertex blocks
	nvtxs=iVertex;
	SetNvtxs(nvtxs);
      }//valid file format
    }//useNuanceTextFormat
    else {
      // old muline format
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
    }//old muline format
  }//useMuLineEvt
  
  else if(useAmBeEvt){ // Diego Costas (diego.costas.rodriguez@usc.es) 2023
    // Initialise the ambe generator once per sim
    // This will get AmBe settings (position, direction, etc)
    if ( !AmBeGen ){
      AmBeGen = new WCSimAmBeGen(myDetector);
    }

    if (!myDetector || !myDetector->IsBGOGeometrySet()) {
        G4Exception("WCSimPrimaryGeneratorActino::GeneratePrimaries", "WCSimError", FatalException, 
            "You are trying to run AmBeGen without having set the BGO geometry. Please configure it in your .mac file using /WCSim/BGOPlacement true");
    }
    else{
      AmBeGen->GenerateNG(anEvent);
    }
  } 
  
  else if (useRootrackerEvt)
    {
      if ( !fInputRootrackerFile->IsOpen() )
        {
	  G4cout << "Set a Rootracker vector file using the command /mygen/vecfile name"
		 << G4endl;
	  return;
        }

      //Generate 1 event

      //Load event from file
      if(fEvNum == 0){
	fSettingsTree->SetBranchAddress("NuBeamAng",&fNuBeamAng);
	fSettingsTree->SetBranchAddress("DetRadius",&fNuPrismRadius);
	fSettingsTree->SetBranchAddress("NuIdfdPos",fNuPlanePos);
	fSettingsTree->GetEntry(0);
      }
      if (fEvNum<fNEntries){
	fRooTrackerTree->GetEntry(fEvNum);
	fEvNum++;
      }
      else{
	G4cout << "End of rootracker file - run terminated..."<< G4endl;
	G4RunManager::GetRunManager()-> AbortRun();
	return;
      }

      // Calculate offset from neutrino generation plane to centre of nuPRISM detector
      float z_offset = fNuPlanePos[2]*cm;
      float y_offset = 0;//(fNuPrismRadius/zDir)*yDir;
      float x_offset = fNuPlanePos[0]*cm;

      //Subtract offset to get interaction position in WCSim coordinates
        xPos = fTmpRootrackerVtx->EvtVtx[0]*cm - x_offset;
        yPos = fTmpRootrackerVtx->EvtVtx[1]*cm - y_offset;
        zPos = fTmpRootrackerVtx->EvtVtx[2]*cm - z_offset;

        //Check if event is outside detector; skip to next event if so; keep
        //loading events until one is found within the detector or there are
        //no more interaction to simulate for this event.
        //The current neut vector files do not correspond directly to the detector dimensions, so only keep those events within the detector
        while (sqrt(pow(xPos,2)+pow(zPos,2)) > (myDetector->GetWCIDDiameter()/2.) ||
	       (abs(yPos - myDetector->GetWCIDVerticalPosition()) > (myDetector->GetWCIDHeight()/2.))){
            //Load another event
            if (fEvNum<fNEntries){
                fRooTrackerTree->GetEntry(fEvNum);
                G4cout << "Skipped event# " << fEvNum - 1 << " (event vertex outside detector)" << G4endl;
                fEvNum++;
            }
            else{
		G4cout << "End of rootracker file - run terminated..."<< G4endl;
		G4RunManager::GetRunManager()-> AbortRun();
                return;
            }
	    // Calculate offset from neutrino generation plane to centre of nuPRISM detector
	    z_offset = fNuPlanePos[2]*cm;
	    y_offset = 0;//(fNuPrismRadius/zDir)*yDir;
	    x_offset = fNuPlanePos[0]*cm;

            //Convert coordinates
	    //Subtract offset to get interaction position in WCSim coordinates
            xPos = fTmpRootrackerVtx->EvtVtx[0]*cm - x_offset;
            yPos = fTmpRootrackerVtx->EvtVtx[1]*cm - y_offset;
            zPos = fTmpRootrackerVtx->EvtVtx[2]*cm - z_offset;
        }

	//Generate particles
	//i = 0 is the neutrino
	//i = 1 is the target nucleus
	//i = 2 is the target nucleon
	//i > 2 are the outgoing particles

	// First simulate the incoming neutrino
	// Get the neutrino direction
      xDir=fTmpRootrackerVtx->StdHepP4[0][0]*GeV;
      yDir=fTmpRootrackerVtx->StdHepP4[0][1]*GeV;
      zDir=fTmpRootrackerVtx->StdHepP4[0][2]*GeV;

      double momentum=sqrt((xDir*xDir)+(yDir*yDir)+(zDir*zDir));

      G4ThreeVector vtx = G4ThreeVector(xPos, yPos, zPos);
      G4ThreeVector dir = G4ThreeVector(-xDir, -yDir, -zDir);

      dir = dir*(1./momentum);

      particleGun->SetParticleDefinition(particleTable->FindParticle(fTmpRootrackerVtx->StdHepPdgTemp[0]));
      double kin_energy = momentum;//fabs(fTmpRootrackerVtx->StdHepP4[i][3])*GeV - particleGun->GetParticleDefinition()->GetPDGMass();
      particleGun->SetParticleEnergy(kin_energy);
      particleGun->SetParticlePosition(vtx);
      particleGun->SetParticleMomentumDirection(dir);
      // Will want to include some beam time structure at some point, but not needed at the moment since we only simulate 1 interaction per events
      // particleGun->SetParticleTime(time);
      particleGun->GeneratePrimaryVertex(anEvent);  //Place vertex in stack

        // Now simulate the outgoing particles
        for (int i = 0; i < fTmpRootrackerVtx->StdHepN; i++){

            // Skip the initial neutrino, target nucleus and target nucleon
            if( i < 3){
                int pdg = abs(fTmpRootrackerVtx->StdHepPdgTemp[i]);
                if(pdg > 100000 || pdg == 12 || pdg == 14 || pdg == 2112 || pdg == 2212){
                    continue;
                }
            }

            xDir=fTmpRootrackerVtx->StdHepP4[i][0]*GeV;
            yDir=fTmpRootrackerVtx->StdHepP4[i][1]*GeV;
            zDir=fTmpRootrackerVtx->StdHepP4[i][2]*GeV;

            momentum=sqrt((xDir*xDir)+(yDir*yDir)+(zDir*zDir));

            vtx.setX(xPos);
            vtx.setY(yPos);
            vtx.setZ(zPos);

            dir.setX(xDir);
            dir.setY(yDir);
            dir.setZ(zDir);

            dir = dir*(1./momentum);

            particleGun->SetParticleDefinition(particleTable->FindParticle(fTmpRootrackerVtx->StdHepPdgTemp[i]));

            kin_energy = fabs(fTmpRootrackerVtx->StdHepP4[i][3])*GeV - particleGun->GetParticleDefinition()->GetPDGMass();

            particleGun->SetParticleEnergy(kin_energy);
            particleGun->SetParticlePosition(vtx);
            particleGun->SetParticleMomentumDirection(dir);
            // Will want to include some beam time structure at some point, but not needed at the moment since we only simulate 1 interaction per events
            // particleGun->SetParticleTime(time);
            particleGun->GeneratePrimaryVertex(anEvent);  //Place vertex in stack
        }
    }

  else if (useGunEvt)
    {      // manual gun operation
      G4cout << "Use GUN event" << G4endl;
      G4cout << "Name: " << particleGun->GetParticleDefinition()->GetParticleName() << ", E = " << particleGun->GetParticleEnergy() << G4endl;
      particleGun->GeneratePrimaryVertex(anEvent);

      //To prevent occasional seg fault from an un-assigned targetpdg
      targetpdgs[0] = 2212; //ie. proton

      G4ThreeVector P  =anEvent->GetPrimaryVertex()->GetPrimary()->GetMomentum();
      G4ThreeVector vtx=anEvent->GetPrimaryVertex()->GetPosition();
      G4double mass    =anEvent->GetPrimaryVertex()->GetPrimary()->GetMass();
      G4int pdg        =anEvent->GetPrimaryVertex()->GetPrimary()->GetPDGcode();

      G4ThreeVector dir  = P.unit();
      G4double E         = std::sqrt((P.dot(P))+(mass*mass));

      //mode            = PARTICLEGUN;

    long int A=0,Z=0;
    //		    A=strotl(strPDG,&str);
    if(abs(pdg) >= 1000000000) {
      //ion
      char strPDG[11];
      char strA[10]={0};
      char strZ[10]={0};
      sprintf(strPDG,"%i",abs(pdg));
      //stop GCC complaining about string truncation
      // - we're copying from the middle of a long string
      // - we do terminate the string correctly below
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-truncation"
      strncpy(strZ, &strPDG[3], 3);
#pragma GCC diagnostic pop
      strncpy(strA, &strPDG[6], 3);
      strA[3]='\0';
      strZ[3]='\0';
      A=atoi(strA);
      Z=atoi(strZ);

      G4ParticleDefinition* ion   = G4IonTable::GetIonTable()->GetIon(Z, A, 0);
      ion->SetPDGStable(false);
      ion->SetPDGLifeTime(0.);

      G4ParticleDefinition* ion2   = G4IonTable::GetIonTable()->GetIon(Z, A, 0);
      G4cout<<"ion2 "<<ion2->GetPDGLifeTime()<<G4endl;
    }

      //     particleGun->SetParticleEnergy(E);
      //     particleGun->SetParticlePosition(vtx);
      //     particleGun->SetParticleMomentumDirection(dir);
      SetVtx(vtx);
      SetBeamEnergy(E);
      SetBeamDir(dir);
      SetBeamPDG(pdg);

#ifdef PAIR//B.Q
      G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
      G4String particleName;
      particleGun->SetParticleDefinition(particleTable->FindParticle(particleName="e+"));
      particleGun->SetParticlePosition(vtx);
      particleGun->SetParticleMomentumDirection(P.unit());
      particleGun->SetParticleMomentum(P);
      //particleGun->SetParticleMomentum(P);
      particleGun->GeneratePrimaryVertex(anEvent);
      G4cout << "Use PAIR GUN event" << G4endl;
      G4cout << "Name: " << particleGun->GetParticleDefinition()->GetParticleName() << ", E = " << particleGun->GetParticleEnergy() << G4endl;

      particleGun->SetParticleDefinition(particleTable->FindParticle(particleName="e-"));
      //particleGun->SetParticleEnergy();
#endif

    }
  else if (useLaserEvt)
    {
      //T. Akiri: Create the GPS LASER event
      MyGPS->GeneratePrimaryVertex(anEvent);

	  //To prevent occasional seg fault from an un assigned targetpdg
      targetpdgs[0] = 2212; //ie. proton

      G4ThreeVector P   =anEvent->GetPrimaryVertex()->GetPrimary()->GetMomentum();
      G4ThreeVector vtx =anEvent->GetPrimaryVertex()->GetPosition();
      G4double mass     =anEvent->GetPrimaryVertex()->GetPrimary()->GetMass(); // will be 0 for photon anyway, but for other gps particles not
      G4int pdg         =anEvent->GetPrimaryVertex()->GetPrimary()->GetPDGcode();

      G4ThreeVector dir  = P.unit();
      //G4double E         = std::sqrt((P.dot(P)));
      G4double E         = std::sqrt((P.dot(P))+(mass*mass));
      //G4cout << "Energy " << E << " eV " << G4endl;


      //mode            = LASER; //actually could also be particle gun here. Gps and laser will be separate soon!!

      SetVtx(vtx);
      SetBeamEnergy(E);
      SetBeamDir(dir);
      SetBeamPDG(pdg);
    }
  else if (useInjectorEvt) // K.M.Tsui: addition of injector events
    {

      MyGPS->ClearAll();
      MyGPS->SetMultipleVertex(true); // possibility of using multiple source, but not implemented yet

      int nLayerInjectors = 7; // 7 layers of injectors evenly spaced in z
      int nInjectorsPerLayer = 4; // 4 injectors per layer
      int nCapInjectors = 4; // 4 injectors each the top and bottom endcap

      int nPhotonsPerInjectors = nPhotons; // number of photons per event
      double photoEnergy = 1239.84193/wavelength*eV; //wavelength in nm, Planck constant in (eV)(nm)
      int injectorOn = injectorOnIdx; // index of the injector to be turned on
      int nInjector = -1;
      double injectorangle = openangle; // injector opening angle

      for (int i=0;i<nLayerInjectors;i++) {
        double zpos = myDetector->GetWCIDHeight()/(nLayerInjectors+1.)*(i+1.)-myDetector->GetWCIDHeight()/2.; // evenly spaced in z
        for (int j=0;j<nInjectorsPerLayer;j++) {
          double dist_to_center = myDetector->GetWCIDDiameter()/2. - 20*cm; // not sure the exact value so just a guess
          double theta_start = 0; // may want some offset to avoid overlap with other components
          double theta_current = theta_start+j*CLHEP::pi/2.;
          double xpos = dist_to_center*cos(theta_current);
          double ypos = dist_to_center*sin(theta_current);

          nInjector++;
          if (injectorOn!=nInjector) continue; // only add the injector as specified

          MyGPS->AddaSource(1.);
          G4ParticleDefinition* pd = particleTable->FindParticle("opticalphoton");
          MyGPS->SetParticleDefinition(pd);
          MyGPS->GetCurrentSource()->GetEneDist()->SetEnergyDisType("Mono");
	        MyGPS->GetCurrentSource()->GetEneDist()->SetMonoEnergy(photoEnergy);

          G4ThreeVector position(xpos,ypos,zpos);
          MyGPS->GetCurrentSource()->GetPosDist()->SetPosDisType("Point"); // may need a more realistic shape
	        MyGPS->GetCurrentSource()->GetPosDist()->SetCentreCoords(position);

          // Point the source to tank center
          G4ThreeVector dirz(xpos,ypos,0); // local z axis in source frame
          G4ThreeVector dirx(0,0,1);
          G4ThreeVector diry = dirz.cross(dirx);
          MyGPS->GetCurrentSource()->GetAngDist()->DefineAngRefAxes("angref1",dirx);
          MyGPS->GetCurrentSource()->GetAngDist()->DefineAngRefAxes("angref2",diry);
          MyGPS->GetCurrentSource()->GetAngDist()->SetAngDistType("iso"); // isotropic emission for now
          MyGPS->GetCurrentSource()->GetAngDist()->SetMinTheta(0.);
          MyGPS->GetCurrentSource()->GetAngDist()->SetMaxTheta(injectorangle*deg);
          MyGPS->GetCurrentSource()->GetAngDist()->SetMinPhi(0.);
          MyGPS->GetCurrentSource()->GetAngDist()->SetMaxPhi(360*deg);
          MyGPS->GetCurrentSource()->SetNumberOfParticles(nPhotonsPerInjectors);

        }
      }

      for (int i=0;i<2;i++) { // top and bottom endcap
        double zpos = myDetector->GetWCIDHeight()/2.-20*cm; // again the exact value is unknown
        if (i==1) zpos *= -1;
        for (int j=0;j<nCapInjectors;j++) {
          double dist_to_center = 0.12*myDetector->GetWCIDDiameter()/2.; // again the exact value is unknown
          double theta_start = 0; // may want some offset to avoid overlap with other components
          double theta_current = theta_start+j*CLHEP::pi/2.;
          double xpos = dist_to_center*cos(theta_current);
          double ypos = dist_to_center*sin(theta_current);

          nInjector++;
          if (injectorOn!=nInjector) continue;

          MyGPS->AddaSource(1.);
          G4ParticleDefinition* pd = particleTable->FindParticle("opticalphoton");
          MyGPS->SetParticleDefinition(pd);
          MyGPS->GetCurrentSource()->GetEneDist()->SetEnergyDisType("Mono");
	        MyGPS->GetCurrentSource()->GetEneDist()->SetMonoEnergy(photoEnergy);

          G4ThreeVector position(xpos,ypos,zpos);
          MyGPS->GetCurrentSource()->GetPosDist()->SetPosDisType("Point");
	        MyGPS->GetCurrentSource()->GetPosDist()->SetCentreCoords(position);

          // Point the source to tank center
          G4ThreeVector dirx(1,0,0);
          G4ThreeVector diry(0,1,0);
          if (i==1) diry*=-1; // reverse direction for bottom injector
          MyGPS->GetCurrentSource()->GetAngDist()->DefineAngRefAxes("angref1",dirx);
          MyGPS->GetCurrentSource()->GetAngDist()->DefineAngRefAxes("angref2",diry);
          MyGPS->GetCurrentSource()->GetAngDist()->SetAngDistType("iso");
          MyGPS->GetCurrentSource()->GetAngDist()->SetMinTheta(0.);
          MyGPS->GetCurrentSource()->GetAngDist()->SetMaxTheta(injectorangle*deg);
          MyGPS->GetCurrentSource()->GetAngDist()->SetMinPhi(0.);
          MyGPS->GetCurrentSource()->GetAngDist()->SetMaxPhi(360*deg);
          MyGPS->GetCurrentSource()->SetNumberOfParticles(nPhotonsPerInjectors);
        }
      }

      MyGPS->GeneratePrimaryVertex(anEvent);

      G4ThreeVector P   =anEvent->GetPrimaryVertex()->GetPrimary()->GetMomentum();
      G4ThreeVector vtx =anEvent->GetPrimaryVertex()->GetPosition();
      G4double mass     =anEvent->GetPrimaryVertex()->GetPrimary()->GetMass(); // will be 0 for photon anyway, but for other gps particles not
      G4int pdg         =anEvent->GetPrimaryVertex()->GetPrimary()->GetPDGcode();

      G4ThreeVector dir  = P.unit();
      //G4double E         = std::sqrt((P.dot(P)));
      G4double E         = std::sqrt((P.dot(P))+(mass*mass));
      //G4cout << "Energy " << E << " eV " << G4endl;


      //mode            = LASER; //actually could also be particle gun here. Gps and laser will be separate soon!!

      SetVtx(vtx);
      SetBeamEnergy(E);
      SetBeamDir(dir);
      SetBeamPDG(pdg);
    }

  else if (useLightInjectorEvt)
    {

        // Initialise the light injector once per sim. 
        // This will get LI settings (position, direction, etc) from the db
        // and produce a 3D histogram of the LI profile.
        if ( !LIGen ) {
            LIGen = new WCSimLIGen();
            LIGen->SetPhotonMode(photonMode);
            LIGen->ReadFromDatabase(injectorType,injectorIdx,injectorFilename);
        }

        // Generate the required number of photons with
        // directions distributed as per the LI profile        
        // and populate the G4Event

        G4ThreeVector vtx = LIGen->GetInjectorPosition();
        G4ThreeVector dir = LIGen->GetInjectorDirection();
        G4int pdg = 0;
        G4double E = LIGen->GetPhotonEnergy();
        LIGen->GeneratePhotons(anEvent,nphotons);

        // save injector properties
        G4cout << " Saving injector properties: " << vtx << ", " << E << ", " << dir << ", " << pdg << G4endl;
        SetVtx(vtx);
        SetBeamDir(dir);
        SetBeamEnergy(energy);
        SetBeamPDG(pdg);

    }


  else if (useGPSEvt)
    {
      MyGPS->GeneratePrimaryVertex(anEvent);

      G4ThreeVector P   =anEvent->GetPrimaryVertex()->GetPrimary()->GetMomentum();
      G4ThreeVector vtx =anEvent->GetPrimaryVertex()->GetPosition();
      G4double mass     =anEvent->GetPrimaryVertex()->GetPrimary()->GetMass();
      G4int pdg         =anEvent->GetPrimaryVertex()->GetPrimary()->GetPDGcode();

      G4ThreeVector dir  = P.unit();
      G4double E         = std::sqrt((P.dot(P))+(mass*mass));
      G4cout << " GPS primary vertex (" << vtx.x() << ", " << vtx.y() << ", " << vtx.z() << "), dir ("
	     << dir.x() << ", " << dir.y() << ", " << dir.z() << ") m " << mass << " E "<< E << " pdg " << pdg << G4endl;

      SetVtx(vtx);
      SetBeamEnergy(E);
      SetBeamDir(dir);
      SetBeamPDG(pdg);
#ifdef PAIR//B.Q
      G4ParticleTable* particleTable = G4ParticleTable::GetParticleTable();
      G4String particleName;
      particleGun->SetParticleDefinition(particleTable->FindParticle(particleName="e+"));
      particleGun->SetParticlePosition(vtx);
      particleGun->SetParticleMomentumDirection(P.unit());
      particleGun->SetParticleMomentum(P);
      //particleGun->SetParticleMomentum(P);
      particleGun->GeneratePrimaryVertex(anEvent);
      G4cout << "Use PAIR GUN event" << G4endl;
      G4cout << "Name: " << particleGun->GetParticleDefinition()->GetParticleName() << ", E = " << particleGun->GetParticleEnergy() << G4endl;

      particleGun->SetParticleDefinition(particleTable->FindParticle(particleName="e-"));
      //particleGun->SetParticleEnergy();
#endif
      
      if(needConversion) {
          G4PrimaryParticle *primaryParticle = anEvent->GetPrimaryVertex(0)->GetPrimary(0);
          G4ThreeVector tmpDir(1, 0, 0);
          G4ThreeVector tmpPos(0, 0, 0);
          particleGun->SetParticleDefinition(primaryParticle->GetG4code());
          particleGun->SetParticleEnergy(primaryParticle->GetKineticEnergy());
          particleGun->SetParticlePosition(tmpPos);
          particleGun->SetParticleMomentumDirection(tmpDir);
          foundConversion = false;
          while (!foundConversion) {
              G4Event *tmpEvent = new G4Event(-1);
              particleGun->GeneratePrimaryVertex(tmpEvent);
              G4EventManager::GetEventManager()->ProcessOneEvent(tmpEvent);
              delete tmpEvent;
          }
          G4ThreeVector newDir = primaryParticle->GetMomentumDirection();
          if (!newDir.isParallel(tmpDir, 1e-5)) {
              G4ThreeVector rotationAxis = tmpDir.cross(newDir);
              rotationAxis = rotationAxis / rotationAxis.mag();
              double rotationAngle = acos(newDir.dot(tmpDir));
              for (int i = 0; i < 2; i++) {
                  conversionProductMomentum[i].rotate(rotationAngle, rotationAxis);
              }
          }
          primaryParticle->SetParticleDefinition(conversionProductParticle[0]);
          primaryParticle->SetMomentum(conversionProductMomentum[0].getX(),
                                       conversionProductMomentum[0].getY(),
                                       conversionProductMomentum[0].getZ());
          G4PrimaryParticle *secondProduct = new G4PrimaryParticle(conversionProductParticle[1],
                                                                   conversionProductMomentum[1].getX(),
                                                                   conversionProductMomentum[1].getY(),
                                                                   conversionProductMomentum[1].getZ());
          anEvent->GetPrimaryVertex(0)->SetPrimary(secondProduct);
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


      std::vector<struct radioactive_source>::iterator it;

      G4String IsotopeName = "Rn222";
      G4double IsotopeActivity = myRn222Generator->GetMeanActivity() * 1e-3; // mBq to Bq
      G4double iEventAvg = IsotopeActivity * GetRadioactiveTimeWindow();

      //G4cout << " Average " << iEventAvg << G4endl;
      // random poisson number of vertices based on average
      int n_vertices = CLHEP::RandPoisson::shoot(iEventAvg);

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

    } else if (useIBDEvt) {
        // Generate IBD events from an input spectrum

        // Initialise IBD generator once per sim
        // This reads the spectrum once and populates vector attributes of the
        //   WCSimIBDGen object with the energy and flux
        if(!IBDGen){
            IBDGen = new WCSimIBDGen(ibd_database, ibd_model, myDetector);
        }

        // Event variables
        G4ThreeVector nu_dir;
        G4LorentzVector neutrino;
        G4LorentzVector positron;
        G4LorentzVector neutron;

        // Generate event. GenEvent fills the Lorentz vectors with the direction and energy of the particles
        IBDGen->GenEvent(nu_dir, neutrino, positron, neutron);

        // Generate random isotopic position inside detector
        G4ThreeVector vtx = IBDGen->GenRandomPosition();

        // Populate the event variables to be written out
        SetVtx(vtx);
        SetBeamDir(positron.getV(), 0);
        SetBeamDir(neutron.getV(), 1);
        SetBeamEnergy(positron.getT(), 0);
        SetBeamEnergy(neutron.getT(), 1);
        SetBeamPDG(particleTable->FindParticle("e+")->GetPDGEncoding(), 0);
        SetBeamPDG(particleTable->FindParticle("neutron")->GetPDGEncoding(), 1);

        // Generate neutron
        particleGun->SetParticlePosition(vtx);
        particleGun->SetParticleDefinition(particleTable->FindParticle("neutron"));
        particleGun->SetParticleEnergy(neutron.getT());
        particleGun->SetParticleMomentumDirection(neutron.getV());
        particleGun->SetParticleTime(0.);
        particleGun->GeneratePrimaryVertex(anEvent);
        
        // Generate positron
        particleGun->SetParticlePosition(vtx);
        particleGun->SetParticleDefinition(particleTable->FindParticle("e+"));
        particleGun->SetParticleEnergy(positron.getT());
        particleGun->SetParticleMomentumDirection(positron.getV());
        particleGun->SetParticleTime(0.);
        particleGun->GeneratePrimaryVertex(anEvent);

    } else if (useDataTableEvt) {
    // Setup local variables to store the data table values
    G4int index;
    G4int pdgid;
    G4double ene;
    G4ThreeVector pos;
    G4ThreeVector dir;
    G4double t;

    // Check if the input file is open
    if (!inputFile.is_open()) {
      G4cout << "Error: Input file is not open" << G4endl;
      G4cout << "Set a vector file using the command /mygen/vecfile <FILENAME>"
             << G4endl;
      exit(-1);
    }

    // Flag for first event
    G4bool firstParticle = true;

    // Line position before the next particle
    std::streampos lastLinePos;

    // Counter for the number of particles per event
    G4int nParticles = 0;

    // Read the data table
    std::string line;

    // Skip any lines that start with a #
    while (std::getline(inputFile, line)) {
      if (line.empty() || line[0] == '#') {
        continue;
      }

      // If we've reached the end of the file before beamOn events have been reached then we need to stop
      if (inputFile.eof()) {
        G4cout << "End of datatable file - run terminated..." << G4endl;
        G4RunManager::GetRunManager()->AbortRun();
      }

      if(nParticles > MAX_N_VERTICES){
        G4cout << "CANNOT DEAL WITH MORE THAN " << MAX_N_VERTICES << " VERTICES" << G4endl;
        exit(-1);
      }

      // Buffer to convert between string and other variables
      std::istringstream buffer(line);

      // Load information into local variables
      buffer >> index >> pdgid >> ene >> pos[0] >> pos[1] >> pos[2] >> dir[0] >>
          dir[1] >> dir[2] >> t;

      // We've reached the end of the event N that we're generating and have also read the first line of event
      // N + 1. We need to rewind to the start of event N + 1 so that the first particle of N + 1 is not missed when
      // this function is called again for the next event.
      if (index == 0 && firstParticle == false) {
        // Go back a line
        inputFile.seekg(lastLinePos);
        break;
      }

      // Buffer the position of the current line in the file
      lastLinePos = inputFile.tellg();

      // Set values to save to output
      SetBeamEnergy(ene, nParticles);
      SetBeamDir(dir, nParticles);
      SetBeamPDG(pdgid, nParticles);
      SetVtxs(nParticles, pos);

      nParticles++;

      // Print out the first three particles in the event to be generated
      if (index < 3) {
        // Use the kinetic energy, not total
        G4cout << G4endl
               << "=====================================================\n"
               << "Generating particle " << index << " with id = " << pdgid
               << "\n    with kinetic energy = " << ene
               << "\n    MeV, position = " << pos[0] << ", " << pos[1] << ", "
               << pos[2] << ",\n    and direction = " << dir[0] << ", "
               << dir[1] << ", " << dir[2]
               << "\n====================================================="
               << G4endl;
      }

      // No longer on the first particle
      firstParticle = false;

      // Set the particle gun
      // Particle type
      particleGun->SetParticleDefinition(particleTable->FindParticle(pdgid));
      // Position
      particleGun->SetParticlePosition(pos);
      // Direction
      particleGun->SetParticleMomentumDirection(dir);
      // Energy
      particleGun->SetParticleEnergy(ene);
      // Time
      particleGun->SetParticleTime(t);
      // Set the event
      particleGun->GeneratePrimaryVertex(anEvent);
    }

    G4cout << "Number of particles generated for this event: " << nParticles << G4endl;

    // Set the number of particles in this event
    SetNvtxs(nParticles);

  } else if (useCosmics) {

    if (hFluxCosmics == nullptr) 
      Create_cosmics_histogram();

    //////////////////
    // DEBUG PRINTS
    G4cout << G4endl;
    G4cout << "COSMYMYMATICS" << G4endl;
    G4cout << "#############" << G4endl;
    //////////////////

    // get muon direction
    double phiMuon, cosThetaMuon;
    energy = 0;
    while((int)(energy) == 0){
      hFluxCosmics->GetRandom2(phiMuon,cosThetaMuon);
      int phiMuonBin = hFluxCosmics->GetXaxis()->FindFixBin(phiMuon);
      int cosThetaMuonBin = hFluxCosmics->GetYaxis()->FindFixBin(cosThetaMuon);
      energy = hEmeanCosmics->GetBinContent(phiMuonBin,cosThetaMuonBin)*GeV;
    }

    G4ThreeVector dir(0,0,0);
    dir.setRThetaPhi(-1,acos(cosThetaMuon),phiMuon);

    // generate point uniformly distributed inside the ID cylinder
    double detHalfHeight = 0.5*myDetector->GetWCIDHeight();
    double detRadius     = 0.5*myDetector->GetWCIDDiameter();
    double posInCylR     = sqrt(gRandom->Uniform())*detRadius;
    double posInCylPhi   = gRandom->Uniform(TMath::TwoPi());
    double posInCylZ     = gRandom->Uniform(-1.*detHalfHeight,detHalfHeight);

    G4ThreeVector posInCyl(0,0,0);
    posInCyl.setX(posInCylR*cos(posInCylPhi));
    posInCyl.setY(posInCylR*sin(posInCylPhi));
    posInCyl.setZ(posInCylZ);

    if (myDetector->GetIsNuPrism())
    {
      dir.rotateX(-90.*deg);
      posInCyl.rotateX(-90.*deg);
    }

    // generate muon at the intersection
    // between an sphere with radius = altComics
    // and a line made with the muon direction
    // and the generated point inside the ID cylinder
    double a = dir.mag2();
    double b = -2.*posInCyl.dot(dir);
    double c = posInCyl.mag2()-altCosmics*altCosmics;
    double t = (sqrt(b*b-4.*c*a)-b)/(2.*a);

    G4ThreeVector vtx(0,0,0);
    vtx.setX(posInCyl.x()-t*dir.x());
    vtx.setY(posInCyl.y()-t*dir.y());
    vtx.setZ(posInCyl.z()-t*dir.z());

    int pdgid = 13; // MUON
    particleGun->SetParticleDefinition(particleTable->FindParticle(pdgid));
    G4double mass =particleGun->GetParticleDefinition()->GetPDGMass();
    G4double ekin = energy - mass;

    //////////////////
    // DEBUG PRINTS
    G4cout << G4endl;
    G4cout << "Generated at position : " << vtx.getX()/m << "m "
           << vtx.getY()/m << "m "
           << vtx.getZ()/m << "m " << G4endl;
    G4cout << "phi : " << phiMuon << " cosTheta : " << cosThetaMuon << G4endl;
    G4cout << "E : " << energy/GeV << " GeV" << G4endl;
    G4cout << G4endl;
    //////////////////

    SetVtx(vtx);
    SetBeamEnergy(energy);
    SetBeamDir(dir);
    SetBeamPDG(pdgid);

    particleGun->SetParticleEnergy(ekin);
    particleGun->SetParticlePosition(vtx);
    particleGun->SetParticleMomentumDirection(dir);
    particleGun->GeneratePrimaryVertex(anEvent);

  }
  else if (useRadioactiveEvt)
    {

      // initialize GPS properties
      MyGPS->ClearAll();

      MyGPS->SetMultipleVertex(true);

      std::vector<WCSimPmtInfo*> *pmts=NULL;

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
  else if (useMPMTledEvt)
    {
      // Check configuration for mPMT
      bool usePMT1 = true;
      G4int nID_mPMTs = 0;
      const int LEDID_max = 12;
      if (!myDetector->GetHybridPMT())
      {
        if (myDetector->GetmPMT_nID()==1)
        {
          G4cout<<" Use mPMT-LED generator for non-mPMT >> Exit !!"<<G4endl;
          exit(-1);
        }
        else
        {
          G4cout<<" Use mPMT-LED generator for PMT type 1"<<G4endl;
          nID_mPMTs = myDetector->GetTotalNumPmts();
        }
      }
      else
      {
        if (myDetector->GetmPMT_nID()>1)
        {
          G4cout<<" Use mPMT-LED generator for PMT type 1"<<G4endl;
          nID_mPMTs = myDetector->GetTotalNumPmts();
        }
        else if (myDetector->GetmPMT_nID2()>1)
        {
          G4cout<<" Use mPMT-LED generator for PMT type 2"<<G4endl;
          usePMT1 = false;
          nID_mPMTs = myDetector->GetTotalNumPmts2();
        }
        else
        {
          G4cout<<" Use mPMT-LED generator for non-mPMT >> Exit !!"<<G4endl;
          exit(-1);
        }
      }

      if (mPMTLEDId1 > myDetector->GetTotalNum_mPmts())
      {
        G4cout<<" mPMT id > TotalNum_mPmts >>  Exit !! "<<G4endl;
        exit(-1);
      }

      if (mPMTLEDId2 >= LEDID_max)
      {
        G4cout<<" LED id >= LEDID_max >>  Exit !! "<<G4endl;
        exit(-1);
      }

      // Get the center PMT and 1st PMT to define local axes
      G4int centerPMTId = 0;
      G4int firstPMTId  = 0;
      static std::map<int, std::pair< int, int > > mpmt_id_map = usePMT1 ? myDetector->GetTube_mPMTIDMap() :
                                                                           myDetector->GetTube_mPMTIDMap2();
      for (G4int i=1;i<=nID_mPMTs;i++)
      {
        if (mpmt_id_map[i].first==mPMTLEDId1)
        {
          if (mpmt_id_map[i].second==1) centerPMTId = i;
          else if (mpmt_id_map[i].second==2) firstPMTId = i;
        }
      }
      if (centerPMTId==0 || firstPMTId==0)
      {
        G4cout<<" Cannot locate mPMT id = "<< mPMTLEDId1 << " --> Exit !! "<<G4endl;
        exit(-1);
      }
      G4Transform3D tubeTransformCenter = usePMT1 ? myDetector->GetTubeTransform(centerPMTId) :
                                                    myDetector->GetTubeTransform2(centerPMTId);
      G4Transform3D tubeTransformFirst  = usePMT1 ? myDetector->GetTubeTransform(firstPMTId) :
                                                    myDetector->GetTubeTransform2(firstPMTId);
      G4Vector3D nullOrient = G4Vector3D(0,0,1);
      G4Vector3D pmtOrientationCenter = tubeTransformCenter * nullOrient;
      G4Vector3D pmtOrientationFirst = tubeTransformFirst * nullOrient;
      G4Vector3D z_axis = pmtOrientationCenter.unit();;
      G4Vector3D y_axis = z_axis.cross(pmtOrientationFirst).unit();
      G4Vector3D x_axis = y_axis.cross(z_axis).unit();
      G4Vector3D posCenter(tubeTransformCenter.getTranslation().getX(),
                           tubeTransformCenter.getTranslation().getY(),
                           tubeTransformCenter.getTranslation().getZ());
      G4Vector3D posFirst(tubeTransformFirst.getTranslation().getX(),
                          tubeTransformFirst.getTranslation().getY(),
                          tubeTransformFirst.getTranslation().getZ());
      G4double distFromOriginToPMT = (posCenter-posFirst).mag()/(pmtOrientationCenter-pmtOrientationFirst).mag();
      G4Vector3D pmtOrigin = posCenter - distFromOriginToPMT*pmtOrientationCenter;

      // Predefined LED positions on the mPMT matrix
      G4double LEDth, LEDphi;
      if (mPMTLEDId2<3)
      {
        LEDth = 0.17; 
        LEDphi = CLHEP::pi/6 + CLHEP::pi*2/3*mPMTLEDId2; 
      }
      else if (mPMTLEDId2<6)
      {
        LEDth = 0.388;
        LEDphi = CLHEP::pi/2 + (mPMTLEDId2-3)*CLHEP::pi*2/3;
      }
      else
      {
        LEDth = 0.707;
        LEDphi = CLHEP::pi/12 + (mPMTLEDId2-6)*CLHEP::pi/3;
      }

      G4double distToPMT = 5.8*cm; // ad-hoc value to start photons in acrylic
      G4Vector3D LEDdir = sin(LEDth)*(cos(LEDphi)*x_axis+sin(LEDphi)*y_axis)+cos(LEDth)*z_axis;
      G4double xpos = pmtOrigin.x() + LEDdir.x()*(distFromOriginToPMT+distToPMT);
      G4double ypos = pmtOrigin.y() + LEDdir.y()*(distFromOriginToPMT+distToPMT);
      G4double zpos = pmtOrigin.z() + LEDdir.z()*(distFromOriginToPMT+distToPMT);
      // Change LEDdir with specified angle
      LEDdir = sin(LEDth+mPMTLED_dTheta)*(cos(LEDphi+mPMTLED_dPhi)*x_axis+sin(LEDphi+mPMTLED_dPhi)*y_axis)+cos(LEDth+mPMTLED_dTheta)*z_axis;

      MyGPS->SetParticleDefinition(opticalphoton_pd);
      MyGPS->GetCurrentSource()->GetEneDist()->SetEnergyDisType("Mono");

      G4ThreeVector position(xpos,ypos,zpos);
      MyGPS->GetCurrentSource()->GetPosDist()->SetPosDisType("Point"); // may need a more realistic shape
      MyGPS->GetCurrentSource()->GetPosDist()->SetCentreCoords(position);

      // Point the source in the PMT direction
      G4ThreeVector dirz(-LEDdir.x(),-LEDdir.y(),-LEDdir.z()); // local z axis in source frame
      G4ThreeVector dirx = dirz.orthogonal();
      G4ThreeVector diry = dirz.cross(dirx);
      MyGPS->GetCurrentSource()->GetAngDist()->DefineAngRefAxes("angref1",dirx);
      MyGPS->GetCurrentSource()->GetAngDist()->DefineAngRefAxes("angref2",diry);

      MyGPS->GeneratePrimaryVertex(anEvent);

      // Save LED direction to vtx
      G4ThreeVector P   =-dirz*anEvent->GetPrimaryVertex()->GetPrimary()->GetTotalEnergy();
      G4ThreeVector vtx =anEvent->GetPrimaryVertex()->GetPosition();
      G4double mass     =anEvent->GetPrimaryVertex()->GetPrimary()->GetMass(); // will be 0 for photon anyway, but for other gps particles not
      G4int pdg         =anEvent->GetPrimaryVertex()->GetPrimary()->GetPDGcode();

      G4ThreeVector dir  = P.unit();
      G4double E         = std::sqrt((P.dot(P))+(mass*mass));

      SetVtx(vtx);
      SetBeamEnergy(E);
      SetBeamDir(dir);
      SetBeamPDG(pdg);

    } else if (useHepMC3Evt) {

      G4cout << "Using HepMC3 event" << G4endl;
      // Check if the WCSimNuHepMC3Reader object has been initiaited yet
      if (!hepmc3_reader) {
        hepmc3_reader = new WCSimNuHepMC3Reader(hepmc3_filename, myDetector);
      }

      // Get the next event from the reader
      if (!hepmc3_reader->ReadEvent(hepmc3_positionGen)) {
        G4cout << "NuHepMC3Reader: \033[1m[INFO]\033[0m end of file reached. Run terminated." << G4endl;
        G4RunManager::GetRunManager()->AbortRun();
      }

      // Loop over the particles
      for (HepMC3::GenParticlePtr part : hepmc3_reader->event.particles()) {

        // Skip nuclear remnants
        if (part->pid() == 2009900000) {
          continue;
        }

        // If particle has status 4 then it is a beam particle. This needs writing out, but not simulating
        if (part->status() == 4) {
          // Get direction (momentum) and normalise
          G4ThreeVector dir(part->momentum().px(), part->momentum().py(), part->momentum().pz());

          // Set write outs
          SetBeamPDG(part->pdg_id(), 0);
          SetBeamEnergy(part->momentum().e(), 0);
          SetBeamDir(dir, 0);

          // For a beam particle we want the end vertex
          G4ThreeVector vtx(part->end_vertex()->position().x(), part->end_vertex()->position().y(),
                            part->end_vertex()->position().z());

          SetVtx(vtx);

          continue;
        }

        // If the particle status is 20 then we have a target particle. This needs writing out, but not simulating
        if (part->status() == 20) {
          targetpdgs[0] = part->pdg_id();
          targetenergies[0] = part->momentum().e();
          targetdirs[0] = G4ThreeVector(part->momentum().px(), part->momentum().py(), part->momentum().pz());
          continue;
        }

        // If the particle status is 1 then the particle needs simulating and writing out.
        if (part->status() == 1) {

          // Print in green
          std::cout << "\033[32m";

          // Print out info line with particle information
          std::cout << "\
NuHepMC3Reader: [INFO] Particle ID: "
                    << part->pdg_id() << "\
\n                       Status: "
                    << part->status() << "\
\n                       Momentum: "
                    << part->momentum().px() << " " << part->momentum().py() << " " << part->momentum().pz() << "\
\n                       Energy: "
                    << part->momentum().e() << "\
\n                       Position: "
                    << part->production_vertex()->position().x() << " "
                    << part->production_vertex()->position().y() << " "
                    << part->production_vertex()->position().z() << "\
\n                       Time: "
                    << part->production_vertex()->position().t() << "\
\n                       Direction: "
                    << part->momentum().px() << " " << part->momentum().py() << " " << part->momentum().pz() << "\
\n                       Momentum mag: "
                    << part->momentum().p3mod() << std::endl;

          // Print in default colour
          std::cout << "\033[0m";

          // Get direction (momentum) and normalise
          G4ThreeVector dir(part->momentum().px(), part->momentum().py(), part->momentum().pz());

          // Get particle position
          G4ThreeVector vtx(part->production_vertex()->position().x(), part->production_vertex()->position().y(),
                            part->production_vertex()->position().z());
          // Set the vertex
          SetVtx(vtx);

          // Set the number of vertices
          SetNvtxs(1);

          // Generate the final state particles with the particle gun
          particleGun->SetParticlePosition(vtx);
          particleGun->SetParticleDefinition(G4ParticleTable::GetParticleTable()->FindParticle(part->pdg_id()));
          particleGun->SetParticleEnergy(part->momentum().e());
          particleGun->SetParticleMomentum(part->momentum().p3mod());
          particleGun->SetParticleMomentumDirection(dir);
          particleGun->SetParticleTime(part->production_vertex()->position().t());
          particleGun->GeneratePrimaryVertex(anEvent);
          continue;
        }
      }
    }
}

void WCSimPrimaryGeneratorAction::SaveOptionsToOutput(WCSimRootOptions * wcopt)
{
  if(useMulineEvt)
    wcopt->SetVectorFileName(vectorFileName);
  else if (useHepMC3Evt)
    wcopt->SetVectorFileName(hepmc3_filename);
  else
    wcopt->SetVectorFileName("");
  wcopt->SetGeneratorType(GetGeneratorTypeString());
}

G4String WCSimPrimaryGeneratorAction::GetGeneratorTypeString()
{
  if(useMulineEvt)
    return "muline";
  else if(useAmBeEvt)
    return "ambeevt";
  else if(useGunEvt)
    return "gun";
  else if(useGPSEvt)
    return "gps";
  else if(useLaserEvt)
    return "laser";
  else if(useInjectorEvt)
    return "injector";
  else if(useRootrackerEvt)
    return "rooTrackerEvt";
  else if(useCosmics)
    return "cosmics";
  else if (useMPMTledEvt)
    return "mPMT-LED";
  else if(useIBDEvt)
    return "IBD";
  else if(useHepMC3Evt)
    return "hepmc3";
  else if(useDataTableEvt)
    return "data-table";
  else if(useCosmics)
    return "cosmics";
  else if(useRadioactiveEvt)
    return "radioactivity";
  else if(useRadonEvt)
    return "radon";
  else if(useLightInjectorEvt)
    return "light-injector";
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

void WCSimPrimaryGeneratorAction::OpenRootrackerFile(G4String fileName)
{
  if (fInputRootrackerFile) fInputRootrackerFile->Delete();

  fInputRootrackerFile = TFile::Open(fileName.data());
  if (!fInputRootrackerFile){
    G4cout << "Cannot open: " << fileName << G4endl;
    exit(1);
  }

  fRooTrackerTree = (TTree*) fInputRootrackerFile->Get("nRooTracker");
  fSettingsTree = (TTree*) fInputRootrackerFile->Get("Settings");
  if (!fRooTrackerTree){
    G4cout << "File: " << fileName << " does not contain a Rootracker nRooTracker tree - please check you intend to process Rootracker events" << G4endl;
    exit(1);
  }
  fNEntries=fRooTrackerTree->GetEntries();

  fTmpRootrackerVtx = new NRooTrackerVtx();
  SetupBranchAddresses(fTmpRootrackerVtx); //link fTmpRootrackerVtx and current input file

  fSettingsTree->SetBranchAddress("NuBeamAng",&fNuBeamAng);
  fSettingsTree->SetBranchAddress("DetRadius",&fNuPrismRadius);
  fSettingsTree->SetBranchAddress("NuIdfdPos",fNuPlanePos);

}

void WCSimPrimaryGeneratorAction::SetupBranchAddresses(NRooTrackerVtx* nrootrackervtx){

  // Set up branch address for rooTrackerVertex tree in nuPRISM files
  fRooTrackerTree->SetBranchAddress("EvtCode",        &(nrootrackervtx->EvtCode) );
  fRooTrackerTree->SetBranchAddress("EvtNum",         &(nrootrackervtx->EvtNum) );
  fRooTrackerTree->SetBranchAddress("EvtXSec",        &(nrootrackervtx->EvtXSec) );
  fRooTrackerTree->SetBranchAddress("EvtDXSec",       &(nrootrackervtx->EvtDXSec) );
  fRooTrackerTree->SetBranchAddress("EvtWght",        &(nrootrackervtx->EvtWght) );
  fRooTrackerTree->SetBranchAddress("EvtProb",        &(nrootrackervtx->EvtProb) );
  fRooTrackerTree->SetBranchAddress("EvtVtx",          (nrootrackervtx->EvtVtx) );
  fRooTrackerTree->SetBranchAddress("StdHepN",        &(nrootrackervtx->StdHepN) );
  fRooTrackerTree->SetBranchAddress("StdHepPdg",       (nrootrackervtx->StdHepPdgTemp) );
  fRooTrackerTree->SetBranchAddress("StdHepStatus",    (nrootrackervtx->StdHepStatusTemp) );
  fRooTrackerTree->SetBranchAddress("StdHepX4",        (nrootrackervtx->StdHepX4) );
  fRooTrackerTree->SetBranchAddress("StdHepP4",        (nrootrackervtx->StdHepP4) );
  fRooTrackerTree->SetBranchAddress("StdHepPolz",      (nrootrackervtx->StdHepPolz) );
  fRooTrackerTree->SetBranchAddress("StdHepFd",        (nrootrackervtx->StdHepFdTemp) );
  fRooTrackerTree->SetBranchAddress("StdHepLd",        (nrootrackervtx->StdHepLdTemp) );
  fRooTrackerTree->SetBranchAddress("StdHepFm",        (nrootrackervtx->StdHepFmTemp) );
  fRooTrackerTree->SetBranchAddress("StdHepLm",        (nrootrackervtx->StdHepLmTemp) );

  // NEUT > v5.0.7 && MCP > 1 (>10a)
  fRooTrackerTree->SetBranchAddress("NEnvc",          &(nrootrackervtx->NEnvc)    );
  fRooTrackerTree->SetBranchAddress("NEipvc",          (nrootrackervtx->NEipvcTemp)   );
  fRooTrackerTree->SetBranchAddress("NEpvc",           (nrootrackervtx->NEpvc)    );
  fRooTrackerTree->SetBranchAddress("NEiorgvc",        (nrootrackervtx->NEiorgvcTemp) );
  fRooTrackerTree->SetBranchAddress("NEiflgvc",        (nrootrackervtx->NEiflgvcTemp) );
  fRooTrackerTree->SetBranchAddress("NEicrnvc",        (nrootrackervtx->NEicrnvcTemp) );

  fRooTrackerTree->SetBranchAddress("NEnvert",        &(nrootrackervtx->NEnvert)  );
  fRooTrackerTree->SetBranchAddress("NEposvert",       (nrootrackervtx->NEposvert) );
  fRooTrackerTree->SetBranchAddress("NEiflgvert",      (nrootrackervtx->NEiflgvertTemp) );
  fRooTrackerTree->SetBranchAddress("NEnvcvert",      &(nrootrackervtx->NEnvcvert) );
  fRooTrackerTree->SetBranchAddress("NEdirvert",       (nrootrackervtx->NEdirvert) );
  fRooTrackerTree->SetBranchAddress("NEabspvert",      (nrootrackervtx->NEabspvertTemp) );
  fRooTrackerTree->SetBranchAddress("NEabstpvert",     (nrootrackervtx->NEabstpvertTemp) );
  fRooTrackerTree->SetBranchAddress("NEipvert",        (nrootrackervtx->NEipvertTemp)  );
  fRooTrackerTree->SetBranchAddress("NEiverti",        (nrootrackervtx->NEivertiTemp)  );
  fRooTrackerTree->SetBranchAddress("NEivertf",        (nrootrackervtx->NEivertfTemp)  );
  // end Rootracker > v5.0.7 && MCP > 1 (>10a)

  // NEUT > v5.1.2 && MCP >= 5
  fRooTrackerTree->SetBranchAddress("NEcrsx",          &(nrootrackervtx->NEcrsx)    );
  fRooTrackerTree->SetBranchAddress("NEcrsy",          &(nrootrackervtx->NEcrsy)    );
  fRooTrackerTree->SetBranchAddress("NEcrsz",          &(nrootrackervtx->NEcrsz)    );
  fRooTrackerTree->SetBranchAddress("NEcrsphi",        &(nrootrackervtx->NEcrsphi)    );

  fRooTrackerTree->SetBranchAddress("NuParentDecMode", &(nrootrackervtx->NuParentDecMode));
  fRooTrackerTree->SetBranchAddress("NuParentPdg",     &(nrootrackervtx->NuParentPdg));
  fRooTrackerTree->SetBranchAddress("NuNg",            &(nrootrackervtx->NuNg));
  fRooTrackerTree->SetBranchAddress("NuGp",             (nrootrackervtx->NuGp));
  fRooTrackerTree->SetBranchAddress("NuGv",             (nrootrackervtx->NuGv));
  fRooTrackerTree->SetBranchAddress("NuGpid",           (nrootrackervtx->NuGpid));
  fRooTrackerTree->SetBranchAddress("NuGmat",           (nrootrackervtx->NuGmat));
  fRooTrackerTree->SetBranchAddress("NuGmec",           (nrootrackervtx->NuGmec));
  fRooTrackerTree->SetBranchAddress("NuGdistc",         (nrootrackervtx->NuGdistc));
  fRooTrackerTree->SetBranchAddress("NuGdistal",        (nrootrackervtx->NuGdistal));

}

void WCSimPrimaryGeneratorAction::CopyRootrackerVertex(NRooTrackerVtx* nrootrackervtx){
  nrootrackervtx->Copy(fTmpRootrackerVtx);
  nrootrackervtx->TruthVertexID = -999;
}

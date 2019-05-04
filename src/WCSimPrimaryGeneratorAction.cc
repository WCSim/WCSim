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
#include <ios>

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
  useMulineEvt = true;
  useGunEvt    = false;
  useLaserEvt  = false;
  useGPSEvt    = false;
  useMultiVertEvt = false;
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
	    token = readInLine(inputFile, lineSize, inBuf);
	    vtxs[0] = G4ThreeVector(atof(token[1])*cm,
				    atof(token[2])*cm,
				    atof(token[3])*cm);
	    
            // true : Generate vertex in Rock , false : Generate vertex in WC tank
            SetGenerateVertexInRock(false);

	    // Next we read the incoming neutrino and target
	    
	    // First, the neutrino line

	    token=readInLine(inputFile, lineSize, inBuf);
	    beampdgs[0] = atoi(token[1]);
	    beamenergies[0] = atof(token[2])*MeV;
	    beamdirs[0] = G4ThreeVector(atof(token[3]),
					atof(token[4]),
					atof(token[5]));

	    // Now read the target line

	    token=readInLine(inputFile, lineSize, inBuf);
	    targetpdgs[0] = atoi(token[1]);
	    targetenergies[0] = atof(token[2])*MeV;
	    targetdirs[0] = G4ThreeVector(atof(token[3]),
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
		    particleGun->GeneratePrimaryVertex(anEvent);
		    SetVtx(vtxs[0]);
		    SetBeamEnergy(ekin);
		    SetBeamDir(dir);
		  }
	      }
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
  else if (useMultiVertEvt)
    {
      if ( !inputFile.is_open() )
	{
	  G4cout << "Set a nuance-formatted file using the command /mygen/vecfile name"
		 << G4endl;
	  exit(-1);
	}

      //
      // Documentation describing the nuance text format can be found here: 
      // http://neutrino.phy.duke.edu/nuance-format/
      //
      // The nuance input file specification above is vague enough that we can extend
      // the scope of event generation beyond that of the nuance code itself.
      // In particular, this code allows the definition of multiple vertices in each
      // simulated event. They can have different locations and/or times of generation.
      // Each vertex can have an arbitrary number of tracks (no change). 
      //
      // In some simulations, the user may not wish to declare an initial state particle (-1)
      // or final state particle before interactions (-2). If these are omitted, then 
      // ensure you know how the rest of the code and the output ntuple will be affected.
      // Then the "info" line is also not required and can be omitted.
      //
      // We also include the functionality to include comments (definitely beyond the original
      // nuance format scope). Any line that begins with "# " is ignored.
      // 
      // Blank lines are still a no-no. At least one "vertex" must come before a "track".
      //
      // Original functionality is retained in reading standard nuance input files.
      //
      // To simulate optical photons (rather than G4Gamma), use code 30. It is an unused 
      // PDG code from the boson table.
      //

      const int lineSize=100;
      char      inBuf[lineSize];
      vector<string> token(1);

      // Look for "begin", but skip comments
      while (token = readInLine(inputFile, lineSize, inBuf), token[0] == "#") {for (int t = 0; t < token.size(); ++t) std::cout << token[t] << " "; std::cout << std::endl; }
      	  
      if (token.size() == 0) 
	{
	  G4cout << "end of nuance vector file!" << G4endl;
	}
      else if (token[0] != "begin")
	{
	  G4Exception("WCSimPrimaryGeneratorAction::GeneratePrimaries","Event Must Be Aborted",EventMustBeAborted,"Error reading nuance-formatted event input, skipping event. Unexpected token (expected 'begin').");
	}
      else   // normal parsing begins here
	{
	  // Read the nuance mode line (ignore value now)
	  while (token = readInLine(inputFile, lineSize, inBuf), token[0] == "#") { for (int t = 0; t < token.size(); ++t) std::cout << token[t] << " "; std::cout << std::endl;}
	  if (token[0] != "nuance")
	    {
	      G4Exception("WCSimPrimaryGeneratorAction::GeneratePrimaries","Event Must Be Aborted",EventMustBeAborted,"Error reading nuance-formatted event input, skipping event. Unexpected token (expected 'nuance').");
	    }
	  mode = atoi(token[1]);

	  // loop over arbitrary number of vertices
	  do 
	    {
	      token = readInLine(inputFile, lineSize, inBuf);
	      if ( token.size() == 0 ) 
		{
		  G4Exception("WCSimPrimaryGeneratorAction::GeneratePrimaries","Event Must Be Aborted",EventMustBeAborted,"Error reading nuance-formatted event input, skipping event. Unexpected blank line.");
		}
	      if ( token[0] == "#" )
		{
		  for (int t = 0; t < token.size(); ++t) std::cout << token[t] << " "; std::cout << std::endl;
		  continue;
		}
	      else if ( token[0] == "vertex" )
		{
		  vtxs[0] = G4ThreeVector(atof(token[1]),
					  atof(token[2]),
					  atof(token[3]));//cm
		  G4double time = atof(token[4]);//ns

		  // true : Generate vertex in Rock , false : Generate vertex in WC tank
		  SetGenerateVertexInRock(false);

		  particleGun->SetParticleTime(time);
		  particleGun->SetParticlePosition(vtxs[0]);
		}
	      else if ( token[0] == "info" )
		{
		  vecRecNumber = atoi(token[2]);
		}
	      else if ( token[0] == "track" )
		{
		  if ( atoi(token[6]) == -1 )
		    {
		      beampdgs[0] = atoi(token[1]);
		      beamenergies[0] = atof(token[2]);//MeV;
		      beamdirs[0] = G4ThreeVector(atof(token[3]),
						  atof(token[4]),
						  atof(token[5]));
		    }
		  else if ( atoi(token[6]) == -2 )
		    {
		      targetpdgs[0] = atoi(token[1]);
		      targetenergies[0] = atof(token[2]);//MeV;
		      targetdirs[0] = G4ThreeVector(atof(token[3]),
						    atof(token[4]),
						    atof(token[5]));
		    }
		  else if ( atoi(token[6]) == 0 )
		    {
		      // daughter particle
		      G4int pdgid = atoi(token[1]);
		      G4double energy = atof(token[2]);//MeV;
		      G4ThreeVector dir = G4ThreeVector(atof(token[3]),
							atof(token[4]),
							atof(token[5]));

		      if (pdgid == 30)
			{
			  // Special case for optical photons which do not have a unique PDG code.
			  // Use an unused PDG from the boson table.
			  particleGun->SetParticleDefinition(particleTable->FindParticle("opticalphoton"));
			}
		      else
			{
			  particleGun->SetParticleDefinition(particleTable->FindParticle(pdgid));
			}
		      
		      G4double mass = particleGun->GetParticleDefinition()->GetPDGMass();
		      G4double ekin = energy - mass;
			  
		      particleGun->SetParticleEnergy(ekin);
		      particleGun->SetParticleMomentumDirection(dir);
		      particleGun->GeneratePrimaryVertex(anEvent);

		      /*
		      G4ThreeVector vertex = particleGun->GetParticlePosition();
		      G4double time = particleGun->GetParticleTime();
		      G4cout << "Event " << anEvent->GetEventID() << ": PDG=" << pdgid << "  Ekin=" << ekin << "  dir=(" << dir.x() << "," << dir.y() << "," << dir.z() << ")  time=" << time << "  vtx=(" << vertex.x() << "," << vertex.y() << "," << vertex.z() << ")" << G4endl;
		      */
		    }
		}
	      else 
		{
		  G4String msg = "Error reading nuance-formatted event input, skipping event. Unexpected line starting with ";
		  msg += token[0];
		  msg += ".";
		  G4Exception("WCSimPrimaryGeneratorAction::GeneratePrimaries","Event Must Be Aborted",EventMustBeAborted,msg.data());
		}
	    } while (token[0] != "end");
	}
      // temporary kludge to read the same event repeatedly
      inputFile.clear();
      inputFile.seekg(0,inputFile.beg);
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
  else if(useMultiVertEvt)
    return "multivert";
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


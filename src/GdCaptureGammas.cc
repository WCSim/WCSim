
///////////////////////////////////////////////////////////////////////////////
//                 Spectrum of radiative neutron capture by Gadolinium            
//                                    version 1.0.0                                
//                                    (Sep.09.2005)                               
                            
//                Author : karim.zbiri@subatech.in2p3.fr                  

//This file contains the gammas spectrum produced in radiative capture of 
//neutrons by gadolinium.
//This work is adapted from earlier work in geant3 for chooz 1.

//First version by Karim Zbiri, April, 2005
///////////////////////////////////////////////////////////////////////////////


#include "GdCaptureGammas.hh"
#include "Randomize.hh"
#include <vector>
#include "G4Gamma.hh"
using namespace std;

GdCaptureGammas::GdCaptureGammas () {}

GdCaptureGammas::~GdCaptureGammas () {}

G4ReactionProductVector *  GdCaptureGammas::GetGammas(G4int isotopeA) 
{
  G4ReactionProductVector * theGammas = new G4ReactionProductVector;
  vector<double> nrj = Initialize(isotopeA);
  for(unsigned int i=0; i<nrj.size(); i++)
    {
      G4ReactionProduct * theOne = new G4ReactionProduct;
      theOne->SetDefinition( G4Gamma::Gamma() );

      G4double costheta = 2.*G4UniformRand()-1;
      G4double theta = acos(costheta);
      G4double phi = CLHEP::twopi*G4UniformRand();
      G4double sinth = sin(theta);

      theOne->SetTotalEnergy( nrj[i] );
      G4ThreeVector temp(nrj[i]*sinth*cos(phi), nrj[i]*sinth*sin(phi),nrj[i]*cos(theta) );
      theOne->SetMomentum( temp ) ;
      theGammas->push_back(theOne);  
    }
   return theGammas;
}

vector<double> GdCaptureGammas::Initialize(G4int isotopeA)
{
  vector<double> Eg; 
  //double DefElem = G4UniformRand()*48.80;
  //if(DefElem < 39.83)
  if(isotopeA == 157)
    Eg = CapGad157();
  else if(isotopeA == 155)
    Eg = CapGad155();
  else {
      G4cerr << "Unexpected Gd isotope A=" << isotopeA << ". Using 157 instead." << G4endl;
      Eg = CapGad157();
  }

  return Eg;
}

vector<double>  GdCaptureGammas::CapGad155()
{
  //  gammas from GAD155
  //                   either 2 gammas
  //                   either a continuum
  //                   total energy = 8.46 MeV

   Elevel = 8.46;
  double Nlevel = G4UniformRand();
  vector<double> Egammas;

  if(Nlevel > 0.023) Egammas=continuum();
  else if (Nlevel >= 0.013) Egammas=casca1Gd155();
  else  Egammas=casca2Gd155();

  return Egammas;
}

vector<double>  GdCaptureGammas::CapGad157()
{
  //  gammas from GAD155
  //                   either 2 gammas
  //                   either a continuum
  //                   total energy = 7.87 MeV

  Elevel = 7.87;
  double Nlevel = G4UniformRand();
  vector<double> Egammas;

  if(Nlevel > 0.068) Egammas=continuum();
  else if(Nlevel < 0.037) Egammas=casca1Gd157(); 
  else if(Nlevel < 0.05)  Egammas=casca2Gd157(); 
  else if(Nlevel >= 0.05)  Egammas=casca3Gd157(); 

  return Egammas;
}

vector<double>  GdCaptureGammas::casca1Gd155()
{
  vector<double> energy;
  energy.push_back(7.33);
  energy.push_back(1.17);
  return energy;
}

vector<double>  GdCaptureGammas::casca2Gd155()
{
  vector<double> energy;
  energy.push_back(6.44);
  energy.push_back(2.18);
  return energy;
}

vector<double>  GdCaptureGammas::casca1Gd157()
{
  vector<double> energy;
  energy.push_back(6.74);
  energy.push_back(1.11);
  return energy;
}

vector<double>  GdCaptureGammas::casca2Gd157()
{
  vector<double> energy;
  energy.push_back(5.62);
  energy.push_back(2.25);
  return energy;
}

vector<double>  GdCaptureGammas::casca3Gd157()
{
  vector<double> energy;
  energy.push_back(5.88);
  energy.push_back(1.99);
  return energy;
}


vector<double>  GdCaptureGammas::continuum()
{
  // continuum part of gadolinium
  //cross sections
  vector<double> energy;
  const double eps=0.0001,bint=0.01;
  const int ngadbin=750;
  float sigfond,sigex1,sigftot,sigcont,sigtot;
  double dexlevel,auxe,hasard,cint,Eneuve;
  int isigma;
  do
    {      
  //first excited level and ground level
      sigfond=pow(Elevel,3); 
      sigex1=pow(Elevel-1,3);
      sigftot=sigfond+sigex1; 
      
  //continuum
      auxe=(Elevel-2.)/bint;
      isigma=int(auxe+eps);
      sigcont=cumule(isigma);
      sigtot=sigfond+sigex1+sigcont;      
      dexlevel=G4UniformRand()*sigtot; 
      if(dexlevel < sigfond) 
	{ 
	  energy.push_back(Elevel); Elevel =0.;
	  return energy;}
     if((dexlevel>= sigfond) &&  (dexlevel <= sigftot)) {
	energy.push_back(Elevel-1);
	energy.push_back(1);
	Elevel=0.;
	return energy;}
      if(dexlevel > sigftot) 
	{
	  hasard=G4UniformRand()*sigcont;
	  for(int i=0;i<ngadbin;i++)
	    {
	      Eneuve=bint*(i+1.)+2.; 
	      cint=cumule(i+1); 	      
	      if(cint>hasard) 
		{
		  if(Elevel>Eneuve)
		    {
		      energy.push_back(Elevel-Eneuve);
		      Elevel=Eneuve;
		    }		
		  if(Elevel<2.) exit(0);
		  if(Elevel <= 2.01) {energy.push_back(Elevel);Elevel =0.;
		    return energy;}
		  break;
		}
	      if(i==ngadbin-1) exit(0);
	    }
	}     
    } while(Elevel > 2.01);
  //energy.push_back(Elevel);
   return energy;
}

double  GdCaptureGammas::cumule(int ienerg)
{  
//   computes the integrated probability from the threshold
//                                       to the energy of indice ienerg

//   input  :    IENERG  indice for the energy with NGADBIN intervalles
//                                             of width BINT

//   output : integrated probabilities cumule

  double norm,A,A2,A4,A6,T1,T2,T3,T4,xi;
  // const double eps=0.0001;
  const int ngadbin=750;
  if(ienerg>ngadbin-1) exit(0);
  intc();
  norm=2./(pow(sqrt(37.),8));
  A=sqrt(37.)*sqrt(Elevel-2.);
  A2=pow(A,2);
  A4=pow(A,4);
  A6=pow(A,6);
  T1=xint[0][ienerg]*A6; 
  T2=-3.*xint[1][ienerg]*A4;
  T3=3.*xint[2][ienerg]*A2;
  T4=-1.0*xint[3][ienerg]; 
  xi=T1+T2+T3+T4;
  return norm*xi;
} 

void  GdCaptureGammas::intc()
{
//   computes the table xint(4,750)
//                          integration 
//                          of the function  (X**N) * exp(X)


//   output : N=1 comes in XINT(1,75)
//                 2               2
//                 3               3
//                 4               4

  double aux[7],ener,xk;
  const double ngadbin=750,bint=0.01;
  for(int i=0;i<ngadbin;i++)
    {
      ener=bint*(i+1.)+2.;
      xk=sqrt(37.)*sqrt(ener-2.);
      aux[0]=exp(xk)*(xk-1.)+1.;
      for(int j=1;j<7;j++)
	{	 
	  aux[j]=pow(xk,j+1)*exp(xk)-(j+1)*aux[j-1];
	}
      xint[0][i]=aux[0];
      xint[1][i]=aux[2];
      xint[2][i]=aux[4];
      xint[3][i]=aux[6];
    } 
}






///////////////////////////////////////////////////////////////////////////////
//                   Spectrum of radiative neutron capture by Gadolinium           
//                                    version 1.0.0                                
//                                    (Sep.09.2005)                               
                            
//                Author : karim.zbiri@subatech.in2p3.fr                  

//This file contains the gammas spectrum produced in radiative capture of 
//neutrons by gadolinium.
//This work is adapted from earlier work in geant3 for chooz 1.

//First version by Karim Zbiri, April, 2005
///////////////////////////////////////////////////////////////////////////////



#ifndef GdCaptureGammas_hh
#define GdCaptureGammas_hh

#include "G4ReactionProductVector.hh"
#include <vector>

using namespace std;

class GdCaptureGammas
{
 public:
  
  GdCaptureGammas();
  ~GdCaptureGammas();
  G4ReactionProductVector *GetGammas(G4int isotopeA);
  vector<double> Initialize(G4int isotopeA);
  vector<double> CapGad155();
  vector<double> CapGad157();
  vector<double> casca1Gd155();
  vector<double> casca2Gd155();
  vector<double> casca1Gd157();
  vector<double> casca2Gd157();
  vector<double> casca3Gd157();
  vector<double> continuum();
  double cumule(int );
  void intc();
  
  
 public:
  double Elevel;
  double  xint[4][750];
 
};
#endif

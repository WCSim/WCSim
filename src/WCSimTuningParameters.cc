// Tuning parameters

#include "WCSimTuningParameters.hh"
#include "WCSimTuningMessenger.hh"

#include <fstream>


WCSimTuningParameters::WCSimTuningParameters()
{

 TuningMessenger = new WCSimTuningMessenger(this);
 // Default values
 rayff=0.75;
 bsrff=2.50;
 abwff=1.30;
 rgcff=0.32;
 qeff=1.0;
 mieff=0.0;
 ttsff=1.0;
 //nltinfo=0.0; //meaning time resolution should not be linear w/ Q by default
 //qoiff=0.0; //TD 2019.6.26
 pmtsatur=-1;

 pmtsurftype=0;
 cathodeThickness = 0.;
 nCathodePara = 0;
 cathodeparaTable.clear();

 //jl145 - For Top Veto
 tvspacing = 100.0;
 topveto = false;

 WCODWLSCladdingReflectivity   = 0.90; //
 WCODTyvekReflectivity   = 0.90; //

}

WCSimTuningParameters::~WCSimTuningParameters()
{
  delete TuningMessenger;
  TuningMessenger = 0;
}

void WCSimTuningParameters::SaveOptionsToOutput(WCSimRootOptions * wcopt)
{
  wcopt->SetRayff(rayff);
  wcopt->SetBsrff(bsrff);
  wcopt->SetAbwff(abwff);
  wcopt->SetRgcff(rgcff);
  wcopt->SetQeff(qeff);
  wcopt->SetMieff(mieff);
  wcopt->SetTtsff(ttsff);
  //wcopt->SetQoiff(qoiff);//TD 2019.6.26
  //wcopt->SetNLTinfo(nltinfo);
  wcopt->SetPMTSatur(pmtsatur);
  wcopt->SetTvspacing(tvspacing);
  wcopt->SetTopveto(topveto);
}

void WCSimTuningParameters::ReadCathodeParaTable(std::string fname)
{
  cathodeThickness = 0.;
  nCathodePara = 0.;
  cathodeparaTable.clear();

  std::ifstream Data(fname.c_str(),std::ios_base::in);
  if (!Data)
  {
    G4cout<<"Cathode parameter file "<<fname<<" could not be opened --> Exiting..."<<G4endl;
    exit(-1);
  }
  else
    G4cout<<"Cathode parameter file "<<fname<<" is opened to read"<<G4endl;

  std::string str, tmp;
	G4int Column=0;
	while (std::getline(Data, str)) {
		if (str=="#DATASTART") break;
	}
	std::ifstream::pos_type SavePoint = Data.tellg();
	std::getline(Data, str);
	std::istringstream stream(str);
	while (std::getline(stream,tmp,' ')) Column++;
	if (Column!=3)
  {
    G4cerr<<"Number of column = "<<Column<<" which is not equal to 3. "<<G4endl;
    G4cerr<<"Inappropriate input --> Exiting..."<<G4endl;
    exit(-1);
  }
  Data.seekg(SavePoint);

  Data>>pmtsurftype>>cathodeThickness>>nCathodePara;

  for (int i=0;i<nCathodePara;i++)
  {
    if (Data.eof())
    {
      G4cerr<<"Number of row is less than "<<nCathodePara<<G4endl;
      G4cerr<<"Inappropriate input --> Exiting..."<<G4endl;
      exit(-1);
    }
    std::vector<G4double> cathodePara(3);
    Data>>cathodePara[0]>>cathodePara[1]>>cathodePara[2];
    cathodeparaTable.emplace_back(cathodePara);
  }
}
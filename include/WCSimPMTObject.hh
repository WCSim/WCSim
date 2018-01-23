#ifndef WCSimWCPMTObject_h
#define WCSimWCPMTObject_h 1

#include "WCSimDetectorConstruction.hh"
#include "globals.hh"
#include "Randomize.hh"
#include <map>
#include <vector>
#include <TH1F.h>
#include <TGraph.h>

class WCSimPMTObject
{

public:
  virtual G4String GetPMTName()=0;
  virtual G4double GetExposeHeight()=0;
  virtual G4double GetRadius()=0;
  virtual G4float* Getqpe()=0;
  virtual G4float* GetQE()=0;
  virtual G4float* GetQEWavelength()=0;
  virtual G4float  GetmaxQE()=0;
  virtual G4float  GetCollectionEfficiency(float);
  virtual float    HitTimeSmearing(float)=0;
  virtual G4double GetPMTGlassThickness()=0;
  virtual G4float  GetDarkRate()=0;
  virtual G4float  GetDarkRateConversionFactor()=0;
  virtual G4int    GetNbOfQEDefined()=0;
protected:
  virtual G4float* GetCollectionEfficiencyArray();
  virtual G4float* GetCollectionEfficiencyAngle();
  G4float Interpolate_func(G4float, G4int, G4float*, G4float*);
};

class PMT20inch : public WCSimPMTObject
{

public:
  
  PMT20inch();
  ~PMT20inch();
 
public:
  G4String GetPMTName() ;
  G4double GetExposeHeight();
  G4double GetRadius();
  G4float* Getqpe();
  G4float* GetQE();
  G4float* GetQEWavelength();
  G4float  GetmaxQE();
  float    HitTimeSmearing(float);
  G4double GetPMTGlassThickness();
  G4float  GetDarkRate();
  G4float  GetDarkRateConversionFactor();
  G4int    GetNbOfQEDefined();

};

class PMT8inch : public WCSimPMTObject
{

public:
  
  PMT8inch();
  ~PMT8inch();
 
public:
  G4String GetPMTName(); 
  G4double GetExposeHeight(); 
  G4double GetRadius();
  G4float* Getqpe();
  G4float* GetQE();
  G4float* GetQEWavelength();
  G4float  GetmaxQE();
  float    HitTimeSmearing(float);
  G4double GetPMTGlassThickness();
  G4float  GetDarkRate();
  G4float  GetDarkRateConversionFactor();
  G4int    GetNbOfQEDefined();
};

 class PMT10inch : public WCSimPMTObject
{

public: 
  PMT10inch();
  ~PMT10inch();
 
public:
  G4String GetPMTName(); 
  G4double GetExposeHeight(); 
  G4double GetRadius(); 
  G4float* Getqpe();
  G4float* GetQE();
  G4float* GetQEWavelength();
  G4float  GetmaxQE();
  float    HitTimeSmearing(float);
  G4double GetPMTGlassThickness();
  G4float  GetDarkRate();
  G4float  GetDarkRateConversionFactor();
  G4int    GetNbOfQEDefined();
 };

 class PMT10inchHQE : public WCSimPMTObject
{

public: 
  PMT10inchHQE();
  ~PMT10inchHQE();
 
public:
  G4String GetPMTName(); 
  G4double GetExposeHeight(); 
  G4double GetRadius(); 
  G4float* Getqpe();
  G4float* GetQE();
  G4float* GetQEWavelength();
  G4float  GetmaxQE();
  float    HitTimeSmearing(float);
  G4double GetPMTGlassThickness();
  G4float  GetDarkRate();
  G4float  GetDarkRateConversionFactor();
  G4int    GetNbOfQEDefined();
 };

 class PMT12inchHQE : public WCSimPMTObject
{

public: 
  PMT12inchHQE();
  ~PMT12inchHQE();
 
public:
  G4String GetPMTName(); 
  G4double GetExposeHeight(); 
  G4double GetRadius(); 
  G4float* Getqpe();
  G4float* GetQE();
  G4float* GetQEWavelength();
  G4float  GetmaxQE();
  float    HitTimeSmearing(float);
  G4double GetPMTGlassThickness();
  G4float  GetDarkRate();
  G4float  GetDarkRateConversionFactor();
  G4int    GetNbOfQEDefined();
 };

class HPD20inchHQE : public WCSimPMTObject
{

public:
  
  HPD20inchHQE();
  ~HPD20inchHQE();
 
public:
  G4String GetPMTName() ;
  G4double GetExposeHeight();
  G4double GetRadius();
  G4float* Getqpe();
  G4float* GetQE();
  G4float* GetQEWavelength();
  G4float  GetmaxQE();
  float    HitTimeSmearing(float);
  G4double GetPMTGlassThickness();
  G4float  GetDarkRate();
  G4float  GetDarkRateConversionFactor();
  G4int    GetNbOfQEDefined();
protected:
  G4float* GetCollectionEfficiencyArray();
};

class HPD12inchHQE : public WCSimPMTObject
{

public:
  
  HPD12inchHQE();
  ~HPD12inchHQE();
 
public:
  G4String GetPMTName() ;
  G4double GetExposeHeight();
  G4double GetRadius();
  G4float* Getqpe();
  G4float* GetQE();
  G4float* GetQEWavelength();
  G4float  GetmaxQE();
  float    HitTimeSmearing(float);
  G4double GetPMTGlassThickness();
  G4float  GetDarkRate();
  G4float  GetDarkRateConversionFactor();
  G4int    GetNbOfQEDefined();
protected:
  G4float* GetCollectionEfficiencyArray();
};

class BoxandLine20inchHQE : public WCSimPMTObject
{

public:
  
  BoxandLine20inchHQE();
  ~BoxandLine20inchHQE();
 
public:
  G4String GetPMTName() ;
  G4double GetExposeHeight();
  G4double GetRadius();
  G4float* Getqpe();
  G4float* GetQE();
  G4float* GetQEWavelength();
  G4float  GetmaxQE();
  float    HitTimeSmearing(float);
  G4double GetPMTGlassThickness();
  G4float  GetDarkRate();
  G4float  GetDarkRateConversionFactor();
  G4int    GetNbOfQEDefined();
 protected:
  G4float* GetCollectionEfficiencyArray();
};

class BoxandLine12inchHQE : public WCSimPMTObject
{

public:
  
  BoxandLine12inchHQE();
  ~BoxandLine12inchHQE();
 
public:
  G4String GetPMTName() ;
  G4double GetExposeHeight();
  G4double GetRadius();
  G4float* Getqpe();
  G4float* GetQE();
  G4float* GetQEWavelength();
  G4float  GetmaxQE();
  float    HitTimeSmearing(float);
  G4double GetPMTGlassThickness();
  G4float  GetDarkRate();
  G4float  GetDarkRateConversionFactor();
  G4int    GetNbOfQEDefined();
 protected:
  G4float* GetCollectionEfficiencyArray();
};

class WCSimBasicPMTObject
{

 public:
  WCSimBasicPMTObject();
  WCSimBasicPMTObject(std::map<G4float,G4float>);
  WCSimBasicPMTObject(std::vector<G4float>,std::vector<G4float>,G4float);
  ~WCSimBasicPMTObject();

 private:
  std::vector<G4float> QE;
  std::vector<G4float> wavelength;
  std::map<G4float,G4float> mapQE;
  G4float  maxQE;
  TGraph   *gQE;

 public:
  std::vector<G4float> GetQE(){ return QE;};
  void SetQE(std::vector<G4float> qe){ QE=qe;};

  std::vector<G4float> GetWavelength(){ return wavelength;};
  void SetWavelength(std::vector<G4float> qe){ wavelength=qe;};

  std::map<G4float,G4float> GetMapQE(){ return mapQE;};
  void SetMapQE(std::map<G4float,G4float> qe){ mapQE=qe;};

  G4float GetmaxQE(){ return maxQE;};
  void SetmaxQE(G4float qe){ maxQE=qe;};

  TGraph* GetgQE(){ return gQE;};
  void SetgQE(TGraph *g){ gQE=g;};

  void DefineQEHist(std::map<G4float,G4float>);
};

#endif

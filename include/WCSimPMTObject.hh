#ifndef WCSimWCPMTObject_h
#define WCSimWCPMTObject_h 1

#include "WCSimDetectorConstruction.hh"
#include "globals.hh"
#include "Randomize.hh"
#include <map>
#include <vector>


class WCSimPMTObject
{

public:
  virtual G4String GetPMTName()=0;
  virtual G4double GetExposeHeight()=0;
  virtual G4double GetRadius()=0;
  virtual G4float* Getqpe()=0;
  virtual G4double* GetQE()=0;
  virtual G4float* GetQEWavelength()=0;
  virtual G4float  GetmaxQE()=0;
  virtual G4float  GetCollectionEfficiency(float);
  virtual float    HitTimeSmearing(float)=0;
  virtual G4double GetPMTGlassThickness()=0;
  virtual G4float  GetDarkRate()=0;
  virtual G4float  GetDarkRateConversionFactor()=0;
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
  G4double* GetQE();
  G4float* GetQEWavelength();
  G4float  GetmaxQE();
  float    HitTimeSmearing(float);
  G4double GetPMTGlassThickness();
  G4float  GetDarkRate();
  G4float  GetDarkRateConversionFactor();


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
  G4double* GetQE();
  G4float* GetQEWavelength();
  G4float  GetmaxQE();
  float    HitTimeSmearing(float);
  G4double GetPMTGlassThickness();
  G4float  GetDarkRate();
  G4float  GetDarkRateConversionFactor();
};

class PMT5inch : public WCSimPMTObject
{

public:
  
  PMT5inch();
  ~PMT5inch();
 
public:
  G4String GetPMTName(); 
  G4double GetExposeHeight(); 
  G4double GetRadius();
  G4float* Getqpe();
  G4double* GetQE();
  G4float* GetQEWavelength();
  G4float  GetmaxQE();
  float    HitTimeSmearing(float);
  G4double GetPMTGlassThickness();
  G4float  GetDarkRate();
  G4float  GetDarkRateConversionFactor();
};

class PMT3inch : public WCSimPMTObject
{

public:

  PMT3inch();
  ~PMT3inch();

public:
  G4String GetPMTName();
  G4double GetExposeHeight();
  G4double GetRadius();
  G4float* Getqpe();
  G4double* GetQE();
  G4float* GetQEWavelength();
  G4float  GetmaxQE();
  float    HitTimeSmearing(float);
  G4double GetPMTGlassThickness();
  G4float  GetDarkRate();
  G4float  GetDarkRateConversionFactor();
};

class PMT3inchGT : public WCSimPMTObject
{

public:

  PMT3inchGT();
  ~PMT3inchGT();

public:
  G4String GetPMTName();
  G4double GetExposeHeight();
  G4double GetRadius();
  G4float* Getqpe();
  G4double* GetQE();
  G4float* GetQEWavelength();
  G4float  GetmaxQE();
  float    HitTimeSmearing(float);
  G4double GetPMTGlassThickness();
  G4float  GetDarkRate();
  G4float  GetDarkRateConversionFactor();
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
  G4double* GetQE();
  G4float* GetQEWavelength();
  G4float  GetmaxQE();
  float    HitTimeSmearing(float);
  G4double GetPMTGlassThickness();
  G4float  GetDarkRate();
  G4float  GetDarkRateConversionFactor();
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
  G4double* GetQE();
  G4float* GetQEWavelength();
  G4float  GetmaxQE();
  float    HitTimeSmearing(float);
  G4double GetPMTGlassThickness();
  G4float  GetDarkRate();
  G4float  GetDarkRateConversionFactor();
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
  G4double* GetQE();
  G4float* GetQEWavelength();
  G4float  GetmaxQE();
  float    HitTimeSmearing(float);
  G4double GetPMTGlassThickness();
  G4float  GetDarkRate();
  G4float  GetDarkRateConversionFactor();
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
  G4double* GetQE();
  G4float* GetQEWavelength();
  G4float  GetmaxQE();
  float    HitTimeSmearing(float);
  G4double GetPMTGlassThickness();
  G4float  GetDarkRate();
  G4float  GetDarkRateConversionFactor();
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
  G4double* GetQE();
  G4float* GetQEWavelength();
  G4float  GetmaxQE();
  float    HitTimeSmearing(float);
  G4double GetPMTGlassThickness();
  G4float  GetDarkRate();
  G4float  GetDarkRateConversionFactor();
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
  G4double* GetQE();                 //has to be double due to double math inside array ("narrowing conversion" error in C++11)
  G4float* GetQEWavelength();
  G4float  GetmaxQE();
  float    HitTimeSmearing(float);
  G4double GetPMTGlassThickness();
  G4float  GetDarkRate();
  G4float  GetDarkRateConversionFactor();
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
  G4double* GetQE();
  G4float* GetQEWavelength();
  G4float  GetmaxQE();
  float    HitTimeSmearing(float);
  G4double GetPMTGlassThickness();
  G4float  GetDarkRate();
  G4float  GetDarkRateConversionFactor();

protected:
  G4float* GetCollectionEfficiencyArray();
};


// ToDo: make object of classes instead of infinite cp-pastes
class PMT3inchR12199_02 : public WCSimPMTObject
{

public:
  
  PMT3inchR12199_02();
  ~PMT3inchR12199_02();
 
public:
  G4String GetPMTName() ;
  G4double GetExposeHeight();
  G4double GetRadius();
  G4float* Getqpe();
  G4double* GetQE();
  G4float* GetQEWavelength();
  G4float  GetmaxQE();
  float    HitTimeSmearing(float);
  G4double GetPMTGlassThickness();
  G4float  GetDarkRate();
  G4float  GetDarkRateConversionFactor();
protected:
  G4float* GetCollectionEfficiencyArray();
};

// ToDo: make object of classes instead of infinite cp-pastes
class PMT4inchR12199_02 : public WCSimPMTObject
{

public:
  
  PMT4inchR12199_02();
  ~PMT4inchR12199_02();
 
public:
  G4String GetPMTName() ;
  G4double GetExposeHeight();
  G4double GetRadius();
  G4float* Getqpe();
  G4double* GetQE();
  G4float* GetQEWavelength();
  G4float  GetmaxQE();
  float    HitTimeSmearing(float);
  G4double GetPMTGlassThickness();
  G4float  GetDarkRate();
  G4float  GetDarkRateConversionFactor();
protected:
  G4float* GetCollectionEfficiencyArray();
};

// ToDo: make object of classes instead of infinite cp-pastes
class PMT5inchR12199_02 : public WCSimPMTObject
{

public:
  
  PMT5inchR12199_02();
  ~PMT5inchR12199_02();
 
public:
  G4String GetPMTName() ;
  G4double GetExposeHeight();
  G4double GetRadius();
  G4float* Getqpe();
  G4double* GetQE();
  G4float* GetQEWavelength();
  G4float  GetmaxQE();
  float    HitTimeSmearing(float);
  G4double GetPMTGlassThickness();
  G4float  GetDarkRate();
  G4float  GetDarkRateConversionFactor();
protected:
  G4float* GetCollectionEfficiencyArray();
};


#endif

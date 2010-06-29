#ifndef _Xin_Draw_H
#define _Xin_Draw_H
#include "TObject.h"
#include <TApplication.h>
#include <TGClient.h>
#include <TGButton.h>
#include "TGLayout.h"
#include <TGFrame.h>
#include <TGListBox.h>
#include <TList.h>
#include <TFrame.h>
#include <TRootEmbeddedCanvas.h>
#include <TGStatusBar.h>
#include <TGButtonGroup.h>
#include <TGTextEntry.h>
#include <TGTextBuffer.h>
#include <TCanvas.h>
#include <TF1.h>
#include <TH2F.h>
#include <TRandom.h>
#include <TGraph.h>
#include <TAxis.h>
#include <TFile.h>
#include <TTree.h>
#include <TString.h>
#include "TBox.h"
#include "TEllipse.h"
#include "TColor.h"
#include "TMarker.h"
#include "TPaveText.h"
#include "TText.h"
#include "WCSimRootEvent.hh"
#include "WCSimRootGeom.hh"
#include "MCEvent.h"
#include "iostream.h"

/* ************************************************************* */
/* Event Display for WCSIM */
/* Xin Qian mailto: xqian@caltech.edu */

/* Modified from run_wcmap.cc from C. Walter of Duke */
/* ************************************************************* */
/* ************************************************************* */

/* Main features:  */
/* 1. Add GUI */
/* 2. Give the status of current event */
/* 3. Can Select to only display one or multiple particles out */
/*    of all the particles. */
/* 4. Change the color scale (log vs linear) */
/* 5. Three mode display, timing, energy, and special display  */
/*    for pi0 */
/* 6. With vertex/ring information or not */
/* 7. Can place a cut of NPE per pmt to clear plot */
/* 8. Can select set number for saving the plot */
/* 9  Can change index of refraction if needed (can used to  */
/*    project the ring) */
/* 10. Can select event no (other than next and prev) */
/* 11. Support both Mailbox and Cylinder shape.  */
//   12. Can do both batch mode or interactive mode
//    see example in event_display.C
/* ************************************************************** */


enum ETestCommandIdentifiers {
  HId1,
  HId2,
  HId3,
  HId4
};

class Xin_Draw : public TGMainFrame {

private:
   TRootEmbeddedCanvas  *fEcan;
   TGStatusBar          *fStatusBar;
   TFile *file;
   TTree *tree;
   TTree *gtree;
   WCSimRootEvent* wcsimrootevent ;
   WCSimRootGeom* wcsimrootgeom ;
   WCSimRootTrigger* wcsimroottrigger;
   
   TBranch *branch;
   TBranch *gbranch;
   
   Int_t eventno;
   Int_t ncolors;        // number of colors used for map
   Int_t npe_cut;
   Int_t log_scale;
   Int_t flag_vr;
   Int_t setno;
   
   //get the mother particle type etc
  /*  Int_t particle_type; */
/*    Int_t particle_momentum; */
/*    Int_t particle_dir[3]; */
/*    Int_t particle_vertex[3]; */

   TGListBox           *fListBox;
   TGTextEntry         *fTeh1,*fTeh2,*fTeh3,*fTeh4;
   TGTextBuffer        *fTbh1,*fTbh2,*fTbh3,*fTbh4;

   TClonesArray *temp;

  

   TBox *WCMain;
   TEllipse *WCTop;
   TEllipse *WCBottom;
   TBox *WCTop1;
   TBox *WCBottom1;
   Int_t palette_Choice; // 0 = multicolor, 1 = 2 tone
   
   Int_t DIM_X;          // x-dimension of canvas
   Int_t DIM_Y;          // y-dimension of canvas
   Int_t NBINS_X;        // number of bins in x-dimension
   Int_t NBINS_Y;        // number of bins in y-dimension
   Int_t Draw_Mode;

   Int_t padNum;
   TPad* subPad;         // for drawing to canvas corners
   TH2F* empty_hist[5];  // blank histogram for title, axis labels, etc

   MCEvent* mcevent;
   bool isPi0;

   Float_t ref_index;
   
   TGCheckButton       *fCheckMulti;
   TList               *fSelected;   
   
  

 public:
   
   Int_t tracks[100];
   Int_t trackid[100];
   Int_t ntracks;

   Xin_Draw(TString filename, const TGWindow *p, UInt_t w, UInt_t h);
   
    
   virtual ~Xin_Draw();
   void DoExit();
   void DoDraw();
   void DoNext();
   void DoPrev();
   void DoSave();
   void SetSetNo(Int_t a){setno = a;};
   void SetEventNo(Int_t a);
   void SetPalette(Int_t a);
   void SetRef_Index(Float_t a){ref_index = a;};
   void SetFlagVR(Int_t a){flag_vr = a;}
   void SetDrawMode(Int_t a){Draw_Mode = a;};
   void SetLogType(Int_t a){log_scale = a;};
   void SetNPE_cut(Int_t a){npe_cut = a;};
   void PrintEventNo();
   void DoText(const char *text);
   void GetDimensions();  // retrieves dimensions from geometry tree
   void DrawBackground(const char *title = "Water Cherenkov Detector");
   void DrawBackground(Int_t event); // same as above, with event # in name
   void DrawSignal(Int_t mode);
   void Create2TonePalette(Int_t nclrs=100);
   void CreateMultiColorPalette(Int_t nclrs=100);
   void DrawColorLegend(const char *min="min", const char *max="max");
   void DrawColorLegend(Float_t min, Float_t max, Int_t mode);
   void PrintGammaInfo(Float_t gamma1E, Float_t gamma2E); 
   void DrawSizeLegend(const char *min="min", const char *max="max");
   void DrawSizeLegend(Float_t min, Float_t max, Int_t mode);
   void DrawPMT(Float_t x,Float_t y,Float_t z,Float_t color,Float_t size=0.6875,Bool_t isReduced=false);
   void SwitchToSubcanvas(Int_t i);
   void HandleButtons();
   void SaveAs(TString filename);
   void SaveAs(Int_t set,Int_t evt);
   void DrawVertexLegend(Int_t real=1,Int_t rec=1,Int_t gamma1=1,Int_t gamma2=1);
   void DrawVertex(Float_t x, Float_t y, Float_t z, Int_t option=2);
   
   std::vector<Float_t>  ProjectToTankSurface_C(Float_t vtx[3],
					      Float_t dir[3]);
   
   std::vector<std::vector<Float_t> > FindRing(Float_t vertex[3],
					       Float_t dir[3],
					       Int_t resolution,
					       Float_t theta);

   std::vector<Float_t>  ProjectToTankSurface_M(Float_t vtx[3],
					      Float_t dir[3]);
   
  

   
   void DrawRing(Float_t vertex[3],
		 Float_t dir[3],
		 Int_t resolution=200,
		 Float_t theta=42,
		 Int_t color=3);
   
   /* void SetStatusText(const char *txt, Int_t pi); */
/*    void EventInfo(Int_t event, Int_t px, Int_t py, TObject *selected); */
   
   ClassDef(Xin_Draw, 0)
};

#endif

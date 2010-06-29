#include "Xin_Draw.h"
#include "TMath.h"
#include "TROOT.h"

// *************************************************************
// Event Display for WCSIM
// Xin Qian mailto: xqian@caltech.edu

// Modified from run_wcmap.cc from C. Walter of Duke
// *************************************************************
// *************************************************************

// Main features: 
// 1. Add GUI
// 2. Give the status of current event
// 3. Can Select to only display one or multiple particles out
//    of all the particles.
// 4. Change the color scale (log vs linear)
// 5. Three mode display, timing, energy, and special display 
//    for pi0
// 6. With vertex/ring information or not
// 7. Can place a cut of NPE per pmt to clear plot
// 8. Can select set number for saving the plot
// 9  Can change index of refraction if needed (can used to 
//    project the ring)
// 10. Can select event no (other than next and prev)
// 11. Support both Mailbox and Cylinder shape. 
// 12. Can do both batch mode or interactive mode
//    see example in event_display.C
// **************************************************************


void Xin_Draw::DrawPMT(Float_t x,Float_t y,Float_t z,Float_t color,Float_t size,Bool_t isReduced)
{
  // draws a PMT according to its 3-dimensional coordinates and according to a
  // user-defined color and size.  The isReduced variable is only true when the
  // PMT is being drawn into a corner image.
  Float_t x_, y_;
  
  if (wcsimrootgeom->GetGeo_Type()==0){
    if(wcsimrootgeom->GetOrientation() == 1) {
      x_ = -atan2(x,z)*wcsimrootgeom->GetWCCylRadius();
      y_ = y;
      
      if (y_>((wcsimrootgeom->GetWCCylLength())/2-.01))
	{
	  //std::cout<< 'except 1'<<std::endl;
	  x_ = -x;
	  y_ = -z +
	    wcsimrootgeom->GetWCCylLength()/2 +
	    wcsimrootgeom->GetWCCylRadius();
	}
      
      if (y_<-((wcsimrootgeom->GetWCCylLength())/2-.01))
	{
	  //std::cout<< 'except 2'<<std::endl;
	  x_ = -x;
	  y_ = z -
	    wcsimrootgeom->GetWCCylLength()/2 -
	    wcsimrootgeom->GetWCCylRadius();
	}
    }else{
      x_ = -atan2(y,x)*wcsimrootgeom->GetWCCylRadius();
      y_ = z;
      if (y_>((wcsimrootgeom->GetWCCylLength())/2-.01))
	{
	  x_ = -y;
	  y_ = -x +
	    wcsimrootgeom->GetWCCylLength()/2 +
	    wcsimrootgeom->GetWCCylRadius();
	}
      
      if (y_<-((wcsimrootgeom->GetWCCylLength())/2-.01))
	{
	  x_ = -y;
	  y_ = x -
	    wcsimrootgeom->GetWCCylLength()/2 -
	    wcsimrootgeom->GetWCCylRadius();
	}
    }
  }else{
   
    x_ = 0; y_ = 0;
    //x-z front
    if (y<-(wcsimrootgeom->GetMailBox_y()/2.-0.01)){
      x_ = x;
      y_ = z;
    }

    //y-z right
    if (x>(wcsimrootgeom->GetMailBox_x()/2.-0.01)){
      x_ = x + (y + wcsimrootgeom->GetMailBox_y()/2.);
      y_ = z;
    }

    //y-z right
    if (x<-(wcsimrootgeom->GetMailBox_x()/2.-0.01)){
      
      x_ = x - (y + wcsimrootgeom->GetMailBox_y()/2.);
      y_ = z;
      //cout << x_ << "\t" << y_ << endl;
    }

    //x-z back
    if (y>(wcsimrootgeom->GetMailBox_y()/2.-0.01)){
      if (x>0){
	x_ = wcsimrootgeom->GetMailBox_x()/2.-x + wcsimrootgeom->GetMailBox_y() + wcsimrootgeom->GetMailBox_x()/2.;
      }else{
	x_ = -(x+wcsimrootgeom->GetMailBox_x()/2.) - (wcsimrootgeom->GetMailBox_y() + wcsimrootgeom->GetMailBox_x()/2.);
      }
      
      y_ = z;
    }
        
    //x-y top
    if (z>((wcsimrootgeom->GetMailBox_z())/2.-.01)){
      x_ = x;
      y_ = y + wcsimrootgeom->GetMailBox_y()/2. + wcsimrootgeom->GetMailBox_z()/2;
    }
    //x-y botom
    if (z<-((wcsimrootgeom->GetMailBox_z())/2.-.01)){
      x_ = x;
      y_ = -(y+wcsimrootgeom->GetMailBox_y()/2.) - wcsimrootgeom->GetMailBox_z()/2;
     }
    
    
  }

  
  Float_t maxMarkerSize = 0.6875;
  Float_t minMarkerSize = .2;

  if (isReduced)
  {
    maxMarkerSize=.15;
    minMarkerSize=.15;
  }

  TMarker * mrkr = new TMarker(x_,y_,20);

  Int_t markerColor;
  if (palette_Choice==0)
    markerColor=(int)((300+ncolors-1)-((ncolors-1)*color));
  if (palette_Choice==1)
    markerColor=(int)((400+ncolors-1)-((ncolors-1)*color));

  mrkr->SetMarkerColor(markerColor);

  mrkr->SetMarkerSize((maxMarkerSize-minMarkerSize)*size+minMarkerSize);
  if (size>0){
    mrkr->Draw();
  }
}


void Xin_Draw::PrintGammaInfo(Float_t gamma1E, Float_t gamma2E)
{
  // draws a TPaveText in the top right corner describing the indvidual gamma
  // energies of a Pizero

  TCanvas *c1 = fEcan->GetCanvas();
  TPaveText* pavetext = new TPaveText((DIM_X)*.45,
				      (DIM_Y)*.75,
				      (DIM_X)*.9,
				      (DIM_Y)*.9);
  char label1[30];
  sprintf(label1, "gamma 1: %4.2f MeV",gamma1E);
  TText *t1=pavetext->AddText(label1);

  char label2[30];
  sprintf(label2, "gamma 2: %4.2f MeV",gamma2E);
  TText *t2=pavetext->AddText(label2);

  pavetext->SetFillColor(kWhite);
  pavetext->Draw();
  c1->Update();
}


//_____________________________________________________________________________

void Xin_Draw::DrawSizeLegend(Float_t min, Float_t max, Int_t mode)
{
  // mode: 0 = time   (ns)
  //       1 = energy (pE)

  char label1[10];
  char label2[10];

  if (mode==0)
  {
    sprintf(label1,"%d ns",(int)max);
    sprintf(label2,"%d ns",(int)min);
  }
  
  if (mode==1)
    {
      sprintf(label1,"%d pE",(int)min);
      sprintf(label2,"%d pE",(int)max);
    }

  DrawSizeLegend(label1,label2);
}

//_____________________________________________________________________________

void Xin_Draw::DrawSizeLegend(const char *min, const char *max)
{
  // draws a size legend in the bottom right corner of the canvas

  TCanvas *c1 = fEcan->GetCanvas();
  Float_t std_dim_x = 1338.0;
  Float_t std_dim_y = 1467.0;
  
  Float_t interval = 35 * DIM_X / std_dim_x;
  Float_t length = 600 * DIM_X / std_dim_x;
  Int_t   nPMTs = (int)(length/interval);
  Float_t textlength = 100*DIM_X / std_dim_x;
  Float_t textheight = 50 * DIM_Y / std_dim_y;
  Float_t textsize = 0.018;

  Float_t x_offset = 750 * DIM_X / std_dim_x;
  Float_t y_offset = 530 * DIM_Y / std_dim_y;

  Float_t maxMarkerSize = 0.6875;
  Float_t minMarkerSize = .2;

  TMarker * mrkr[nPMTs];

  for (Int_t i=0;i<nPMTs;i++)
  {

    if (wcsimrootgeom->GetGeo_Type()==0){
      mrkr[i] = new TMarker(wcsimrootgeom->GetWCCylRadius()*
			    6.28318531/2-(725*DIM_X / std_dim_x)+(interval*i),
			    -(wcsimrootgeom->GetWCCylLength())/2-(475*DIM_Y/std_dim_y),
			    20);
    }else{
      mrkr[i] = new TMarker(DIM_X-(725*DIM_X / std_dim_x)+(interval*i),
			    -(wcsimrootgeom->GetMailBox_z())/2-(475*DIM_Y/std_dim_y),
			    20);
    }

    
    mrkr[i]->SetMarkerColor(kBlack);
    Float_t ratio = (float)i/nPMTs;
    mrkr[i]->SetMarkerSize((maxMarkerSize-minMarkerSize)*(ratio)+minMarkerSize);
    mrkr[i]->Draw();
  }

  TPaveText* pavetext1;
  TPaveText* pavetext2;
  
  if (wcsimrootgeom->GetGeo_Type()==0){
    pavetext1= 
      new TPaveText((wcsimrootgeom->GetWCCylRadius())*6.28318531/2-x_offset-textlength/2,
		    -(wcsimrootgeom->GetWCCylLength())/2-y_offset,
		    (wcsimrootgeom->GetWCCylRadius())*6.28318531/2-x_offset+textlength/2,
		    -(wcsimrootgeom->GetWCCylLength())/2-y_offset-textheight);
    pavetext2 = 
      new TPaveText((wcsimrootgeom->GetWCCylRadius())*6.28318531/2-x_offset-textlength/2+length,
		    -(wcsimrootgeom->GetWCCylLength())/2-y_offset,
		    (wcsimrootgeom->GetWCCylRadius())*6.28318531/2-x_offset+textlength/2+length,
		    -(wcsimrootgeom->GetWCCylLength())/2-y_offset-textheight);
  }else{
    pavetext1= 
      new TPaveText(DIM_X-x_offset-textlength/2,
		    -(wcsimrootgeom->GetMailBox_z())/2-y_offset,
		    DIM_X-x_offset+textlength/2,
		    -(wcsimrootgeom->GetMailBox_z())/2-y_offset-textheight);
    pavetext2 = 
      new TPaveText(DIM_X-x_offset-textlength/2+length,
		    -(wcsimrootgeom->GetMailBox_z())/2-y_offset,
		    DIM_X-x_offset+textlength/2+length,
		    -(wcsimrootgeom->GetMailBox_z())/2-y_offset-textheight);
  }

  char label1[30];
  TText *t1=pavetext1->AddText(min);
  t1->SetTextSize(textsize);

  pavetext1->SetFillColor(kWhite);
  pavetext1->SetBorderSize(0);
  pavetext1->Draw();

  
 
  char label2[30];
  TText *t2=pavetext2->AddText(max);
  t2->SetTextSize(textsize);

  pavetext2->SetFillColor(kWhite);
  pavetext2->SetBorderSize(0);
  pavetext2->Draw();
  
  c1->Update();
}


void Xin_Draw::DrawColorLegend(Float_t min, Float_t max, Int_t mode)
{
  // mode: 0 = energy (pE)
  //       1 = time   (ns)

  char label1[10];
  char label2[10];

  if (mode==0)
    {
      sprintf(label1,"%d pE",(int)min);
      sprintf(label2,"%d pE",(int)max);
    }

  if (mode==1)
  {
    sprintf(label1,"%d ns",(int)max);
    sprintf(label2,"%d ns",(int)min);
  }

  if (mode==2)
  {  
    sprintf(label1,"Gamma %d",2);
    sprintf(label2,"Gamma %d",1);
  }

  DrawColorLegend(label1,label2);
}

void Xin_Draw::DrawColorLegend(const char *min, const char *max)
{
  // draws a color legend in the bottom right corner of the canvas
  TCanvas *c1 = fEcan->GetCanvas();
  TBox *legend[ncolors];

  Float_t length = DIM_X / (2.27);
  Float_t height = DIM_Y / 48.9;
  Float_t textlength = DIM_X / 13.38;
  Float_t textheight = DIM_Y / 29.34;
  Float_t textsize = 0.018;
  Float_t x_offset = DIM_X / 1.784; // was 750
  Float_t y_offset = DIM_Y / 3.6675; // was 400

  Int_t firstcolor;
  if (palette_Choice==0)
    firstcolor=300;
  if (palette_Choice==1)
    firstcolor=400;

  for (Int_t i=0;i<ncolors;i++)
  {
    if (wcsimrootgeom->GetGeo_Type()==0){
      legend[i] = new TBox(DIM_X-x_offset+(length*i)/ncolors,
			   -(wcsimrootgeom->GetWCCylLength())/2-y_offset,
			   DIM_X-x_offset+(length*(i+1))/ncolors,
			   -(wcsimrootgeom->GetWCCylLength())/2-(y_offset+height));
    }else{
      legend[i] = new TBox( DIM_X-x_offset+(length*i)/ncolors,
			    -(wcsimrootgeom->GetMailBox_z())/2-y_offset,
			    DIM_X-x_offset+(length*(i+1))/ncolors,
			    -(wcsimrootgeom->GetMailBox_z())/2-(y_offset+height));
	
    }
    
    legend[i]->SetFillColor((firstcolor-1)+ncolors-i);
    legend[i]->SetLineWidth(0);
    legend[i]->Draw();
  }

  TPaveText* pavetext1;
  TPaveText* pavetext2;
 
  if (wcsimrootgeom->GetGeo_Type()==0){
    pavetext1= 
      new TPaveText((wcsimrootgeom->GetWCCylRadius())*6.28318531/2-x_offset-textlength/2,
		    -(wcsimrootgeom->GetWCCylLength())/2-(y_offset-5)+textheight,
		    (wcsimrootgeom->GetWCCylRadius())*6.28318531/2-x_offset+textlength/2,
		    -(wcsimrootgeom->GetWCCylLength())/2-(y_offset-5));
    pavetext2= 
      new TPaveText((wcsimrootgeom->GetWCCylRadius())*6.28318531/2-x_offset-textlength/2+length,
		    -(wcsimrootgeom->GetWCCylLength())/2-(y_offset-5)+textheight,
		    (wcsimrootgeom->GetWCCylRadius())*6.28318531/2-x_offset+textlength/2+length,
		    -(wcsimrootgeom->GetWCCylLength())/2-(y_offset - 5));
  }else{
    pavetext1= 
      new TPaveText(DIM_X-x_offset-textlength/2,
		    -(wcsimrootgeom->GetMailBox_z())/2-(y_offset-5)+textheight,
		    DIM_X-x_offset+textlength/2,
		    -(wcsimrootgeom->GetMailBox_z())/2-(y_offset-5));
    pavetext2= 
      new TPaveText(DIM_X-x_offset-textlength/2+length,
		    -(wcsimrootgeom->GetMailBox_z())/2-(y_offset-5)+textheight,
		    DIM_X-x_offset+textlength/2+length,
		    -(wcsimrootgeom->GetMailBox_z())/2-(y_offset - 5));
  }
  
  char label1[30];
  TText *t1=pavetext1->AddText(min);
  t1->SetTextSize(textsize);

  pavetext1->SetFillColor(kWhite);
  pavetext1->SetBorderSize(0);
  pavetext1->Draw();

  char label2[30];
  TText *t2=pavetext2->AddText(max);
  t2->SetTextSize(textsize);

  pavetext2->SetFillColor(kWhite);
  pavetext2->SetBorderSize(0);
  pavetext2->Draw();

  c1->Update();
}



void Xin_Draw::Create2TonePalette(Int_t nclrs)
{
  // creates a color palette that starts at red and mixes into blue.  The
  // total number of colors in the palette is user-definable.

  ncolors = nclrs;
  TColor *color[ncolors];
  for (int p=0;p<ncolors;p++)
  {
    Float_t red   = 1-(float)(p+.5)/(float)ncolors;
    Float_t green = 0;
    Float_t blue  = (float)(p+.5)/(float)ncolors;
    
    if (p==0){red=1;green=0;blue=0;}
    if (p==ncolors-1){red=0;green=0;blue=1;}
    
    if (!gROOT->GetListOfColors()->At(p+400))
    color[p]=new TColor(400+p,red,green,blue);
  }
}


void Xin_Draw::CreateMultiColorPalette(Int_t nclrs)
{
  // creates a color palette that incorporates all the colors.  The total
  // number of colors in the palette is user-definable.

  ncolors = nclrs;
  TColor *color[ncolors];
  for (int p=0;p<ncolors;p++)
  {
    Float_t red;
    Float_t green;
    Float_t blue;

    if (p==0){red=1;green=0;blue=0;}

    if (p<ncolors/4 && p!=0)
    {
      red   = 1;
      green = (float)(p+.5)/((float)ncolors/4);
      blue  = 0;
    }

    if (p==ncolors/4){red=1;green=1;blue=0;}

    if (p>ncolors/4 && p<ncolors/2)
    {
      red   = 1-(float)((p-ncolors/4)+.5)/((float)ncolors/4);
      green = 1;
      blue  = 0;
    }

    if (p==ncolors/2){red=0;green=1;blue=0;}

    if (p>ncolors/2 && p<3*ncolors/4)
    {
      red   = 0;
      green = 1;
      blue  = (float)((p-ncolors/2)+.5)/((float)ncolors/4);
    }

    if (p==3*ncolors/4){red=0;green=1;blue=1;}

    if (p>3*ncolors/4 && p<ncolors)
    {
      red   = 0;
      green = 1-(float)((p-3*ncolors/4)+.5)/((float)ncolors/4);
      blue  = 1;
    }

    if (p==ncolors-1){red=0;green=0;blue=1;}
    
    if (!gROOT->GetListOfColors()->At(p+300))
      color[p]=new TColor(300+p,red,green,blue);
  }
}


void Xin_Draw::DrawBackground(Int_t event)
{
  char label[50];

  sprintf(label, "Water Cherenkov Detector: Event # %d",event);
  DrawBackground(label);
}

void Xin_Draw::DrawBackground(const char *title){
  TCanvas *c1 = fEcan->GetCanvas();
  c1->SetFillColor(10);
  empty_hist[0]->Draw();
  empty_hist[0]->SetTitle(title);
  
  empty_hist[0]->GetXaxis()->SetNdivisions(1007);
  empty_hist[0]->GetYaxis()->SetNdivisions(1007);
  if (wcsimrootgeom->GetGeo_Type()==0){
    empty_hist[0]->GetXaxis()->SetTitle("r * theta (m)");
    empty_hist[0]->GetYaxis()->SetTitle("z (m)");
    WCMain->Draw();
    WCTop->Draw();
    WCBottom->Draw();
  }else{
    empty_hist[0]->GetXaxis()->SetTitle("x - y (m)");
    empty_hist[0]->GetYaxis()->SetTitle("z (m)");
    WCMain->Draw();
    WCTop1->Draw();
    WCBottom1->Draw();
  }

  


  c1->Update();
}

void Xin_Draw::GetDimensions()
{
  // retrieves detector dimensions from the geometry tree
  //cylinder 

  if (wcsimrootgeom->GetGeo_Type()==0){
    DIM_X   = (Int_t)(wcsimrootgeom->GetWCCylRadius()*6.28318531/2);
    DIM_Y   = (Int_t)(wcsimrootgeom->GetWCCylLength()/2 +
		    (2*wcsimrootgeom->GetWCCylRadius()));
    NBINS_X = (Int_t)(5000*DIM_X/(DIM_X+DIM_Y));
    NBINS_Y = (Int_t)(5000*DIM_Y/(DIM_X+DIM_Y));
    
    WCMain = new TBox(-(wcsimrootgeom->GetWCCylRadius())*6.28318531/2,
			    -(wcsimrootgeom->GetWCCylLength())/2,
			    (wcsimrootgeom->GetWCCylRadius())*6.28318531/2,
			    (wcsimrootgeom->GetWCCylLength())/2);
    WCMain->SetFillStyle(0);
    WCMain->SetLineColor(1);
    WCMain->SetLineWidth(2);

    WCTop = new TEllipse(0,(wcsimrootgeom->GetWCCylLength()/2+wcsimrootgeom->GetWCCylRadius()),wcsimrootgeom->GetWCCylRadius());
    WCTop->SetFillStyle(0);
    WCTop->SetLineColor(1);
    WCTop->SetLineWidth(2);

    WCBottom = new TEllipse(0,-(wcsimrootgeom->GetWCCylLength()/2+wcsimrootgeom->GetWCCylRadius()),wcsimrootgeom->GetWCCylRadius());
    WCBottom->SetFillStyle(0);
    WCBottom->SetLineColor(1);
    WCBottom->SetLineWidth(2);
  }else{
    DIM_X = (Int_t)((wcsimrootgeom->GetMailBox_x() + wcsimrootgeom->GetMailBox_y()));
    DIM_Y = (Int_t)((wcsimrootgeom->GetMailBox_z() + 2.*wcsimrootgeom->GetMailBox_y())/2.);

    NBINS_X = (Int_t)(5000*DIM_X/(DIM_X+DIM_Y));
    NBINS_Y = (Int_t)(5000*DIM_Y/(DIM_X+DIM_Y));
    // cout << wcsimrootgeom->GetMailBox_x() << "\t" << wcsimrootgeom->GetMailBox_y() << "\t" << wcsimrootgeom->GetMailBox_z() << endl;

    WCMain = new TBox(-(wcsimrootgeom->GetMailBox_x() + wcsimrootgeom->GetMailBox_y()),
      -(wcsimrootgeom->GetMailBox_z())/2,
      (wcsimrootgeom->GetMailBox_x() + wcsimrootgeom->GetMailBox_y()),
      (wcsimrootgeom->GetMailBox_z())/2);
    WCMain->SetFillStyle(0);
    WCMain->SetLineColor(1);
    WCMain->SetLineWidth(2);

    WCTop1 = new TBox(-wcsimrootgeom->GetMailBox_x()/2.,(wcsimrootgeom->GetMailBox_z())/2,wcsimrootgeom->GetMailBox_x()/2.,(wcsimrootgeom->GetMailBox_z())/2.+(wcsimrootgeom->GetMailBox_y()));
    WCTop1->SetFillStyle(0);
    WCTop1->SetLineColor(1);
    WCTop1->SetLineWidth(2);

    WCBottom1 = new TBox(-wcsimrootgeom->GetMailBox_x()/2.,-(wcsimrootgeom->GetMailBox_z())/2,wcsimrootgeom->GetMailBox_x()/2.,-(wcsimrootgeom->GetMailBox_z())/2.-(wcsimrootgeom->GetMailBox_y()));
    WCBottom1->SetFillStyle(0);
    WCBottom1->SetLineColor(1);
    WCBottom1->SetLineWidth(2);
  }

  

  padNum = 0;
  TString histoname;
  histoname.Form("wc%d",padNum);
  empty_hist[padNum] = new TH2F(histoname,
				histoname,
				NBINS_X,
				-DIM_X,
				DIM_X,
				NBINS_Y,
				-DIM_Y,
				DIM_Y);
  empty_hist[padNum]->SetStats(false);
}


void Xin_Draw::DrawVertexLegend(Int_t real,
			    Int_t rec,
			    Int_t gamma1,
			    Int_t gamma2)
{
  // draws vertex legend in the bottom right corner of the canvas

  if (gamma1>1||gamma2>1||real>1||rec>1||gamma1<0||gamma2<0||real<0||rec<0)
  {
    std::cout<<"VertexLegend: incorrect parameters."<<std::endl;
    return;
  }

  Float_t std_dim_x = 1338.0;
  Float_t std_dim_y = 1467.0;

  Bool_t gamma1_isDrawn=false;
  Bool_t gamma2_isDrawn=false;
  Bool_t real_isDrawn=false;
  Bool_t rec_isDrawn=false;

  if (!gamma1)           // if gamma1=0, we say it is already drawn so the loop
    gamma1_isDrawn=true; // will skip it
  if (!gamma2)
    gamma2_isDrawn=true;
  if (!real)
    real_isDrawn=true;
  if (!rec)
    rec_isDrawn=true;
  

  Float_t length = 800 * DIM_X / std_dim_x;
  Float_t textlength = 150 * DIM_X / std_dim_x;
  Float_t textheight = 50 * DIM_Y / std_dim_y;
  Float_t textsize = 0.01;

  Float_t x_offset = 850 * DIM_X / std_dim_x;

  
  TPaveText* title;
  
  if (wcsimrootgeom->GetGeo_Type()==0){
    title = 
      new TPaveText((wcsimrootgeom->GetWCCylRadius())*6.28318531/2-(750*DIM_X/std_dim_x)+((length-(200*DIM_X/std_dim_x))/2)-(400*DIM_X/std_dim_x),
		    -(wcsimrootgeom->GetWCCylLength())/2-(50*DIM_Y/std_dim_y),
		    (wcsimrootgeom->GetWCCylRadius())*6.28318531/2-(750*DIM_X/std_dim_x)+((length-(200*DIM_X/std_dim_x))/2)+(400*DIM_X/std_dim_x),
		    -(wcsimrootgeom->GetWCCylLength())/2-(150*DIM_Y/std_dim_y));
  }else{
    title = 
      new TPaveText(wcsimrootgeom->GetMailBox_x() + wcsimrootgeom->GetMailBox_y() -(750*DIM_X/std_dim_x)+((length-(200*DIM_X/std_dim_x))/2)-(400*DIM_X/std_dim_x),
		    -(wcsimrootgeom->GetMailBox_z())/2-(50*DIM_Y/std_dim_y),
		    wcsimrootgeom->GetMailBox_x() + wcsimrootgeom->GetMailBox_y()-(750*DIM_X/std_dim_x)+((length-(200*DIM_X/std_dim_x))/2)+(400*DIM_X/std_dim_x),
		    -(wcsimrootgeom->GetMailBox_z())/2-(150*DIM_Y/std_dim_y));
  }


  title->AddText("Vertices");
  title->SetFillColor(kWhite);
  title->SetBorderSize(0);
  title->Draw();

  Int_t nVertices=(gamma1+gamma2+real+rec);
  TMarker* marker[nVertices];
  TMarker* bgmarker[nVertices];
  TPaveText* pavetext[nVertices];
  TText* text[nVertices];
  for (Int_t i=0;i<nVertices;i++)
  {
    if (wcsimrootgeom->GetGeo_Type()==0){
      marker[i] = new TMarker((wcsimrootgeom->GetWCCylRadius())*6.28318531/2-
			      x_offset+(length*(i+1)/(nVertices+1)),
			      -(wcsimrootgeom->GetWCCylLength())/2-(200*DIM_Y/std_dim_y),
			      27);
      bgmarker[i] = new TMarker((wcsimrootgeom->GetWCCylRadius())*6.28318531/2-
				x_offset+(length*(i+1)/(nVertices+1)),
				-(wcsimrootgeom->GetWCCylLength())/2-(200*DIM_Y/std_dim_y),
				27);
      pavetext[i]= new TPaveText((wcsimrootgeom->GetWCCylRadius())*6.28318531/2-
				 x_offset+(length*(i+1)/(nVertices+1))-textlength/2,
				 -(wcsimrootgeom->GetWCCylLength())/2-(240*DIM_Y/std_dim_y),
				 (wcsimrootgeom->GetWCCylRadius())*6.28318531/2-
				 x_offset+(length*(i+1)/(nVertices+1))+textlength/2,
				 -(wcsimrootgeom->GetWCCylLength())/2-(240*DIM_Y/std_dim_y)-textheight);
    }else{
      marker[i] = new TMarker(wcsimrootgeom->GetMailBox_x() + wcsimrootgeom->GetMailBox_y()-
			      x_offset+(length*(i+1)/(nVertices+1)),
			      -(wcsimrootgeom->GetMailBox_z())/2-(200*DIM_Y/std_dim_y),
			      27);
      bgmarker[i] = new TMarker(wcsimrootgeom->GetMailBox_x() + wcsimrootgeom->GetMailBox_y()-
				x_offset+(length*(i+1)/(nVertices+1)),
				-(wcsimrootgeom->GetMailBox_z())/2-(200*DIM_Y/std_dim_y),
				27);
      pavetext[i]= new TPaveText(wcsimrootgeom->GetMailBox_x() + wcsimrootgeom->GetMailBox_y()-
				 x_offset+(length*(i+1)/(nVertices+1))-textlength/2,
				 -(wcsimrootgeom->GetMailBox_z())/2-(240*DIM_Y/std_dim_y),
				 wcsimrootgeom->GetMailBox_x() + wcsimrootgeom->GetMailBox_y()-
				 x_offset+(length*(i+1)/(nVertices+1))+textlength/2,
				 -(wcsimrootgeom->GetMailBox_z())/2-(240*DIM_Y/std_dim_y)-textheight);
    }

    bgmarker[i]->SetMarkerSize(1.6);
    bgmarker[i]->SetMarkerColor(1);
    bgmarker[i]->Draw();

    marker[i]->SetMarkerSize(1);

  pavetext[i]->SetFillColor(kWhite);
  pavetext[i]->SetBorderSize(0);

    if (!real_isDrawn)
    {
      marker[i]->SetMarkerColor(5);
      real_isDrawn=true;
      text[i] = pavetext[i]->AddText(" Real  ");
      text[i]->SetTextSize(textsize);
      continue;
    }
    if (!rec_isDrawn)
    {
      marker[i]->SetMarkerColor(3);
      rec_isDrawn=true;
      text[i] = pavetext[i]->AddText(" Recon ");
      text[i]->SetTextSize(textsize);
      continue;
    }
    if (!gamma1_isDrawn)
    {
      marker[i]->SetMarkerColor(2);
      gamma1_isDrawn=true;
      text[i] = pavetext[i]->AddText("Gamma 1");
      text[i]->SetTextSize(textsize);
      continue;
    }
    if (!gamma2_isDrawn)
    {
      marker[i]->SetMarkerColor(4);
      gamma2_isDrawn=true;
      text[i] = pavetext[i]->AddText("Gamma 2");
      text[i]->SetTextSize(textsize);
      continue;
    }
  }

  for (Int_t j=0;j<nVertices;j++)
  {
    marker[j]->Draw();
    pavetext[j]->Draw();
  }


}




void Xin_Draw::DoDraw()
{
  fSelected->Clear();
  fListBox->GetSelectedEntries(fSelected);
  
  ntracks = 0;
  for (Int_t i=0;i<fSelected->GetEntries();i++){
    if (ntracks<100) 
      {
	tracks[ntracks]  = trackid[Int_t(((TGLBEntry*)(fSelected->At(i)))->EntryId()-1)];
	ntracks++; 
      }
  }
  

  tree->GetEntry(eventno);
  wcsimroottrigger = wcsimrootevent->GetTrigger(0);
  mcevent = new MCEvent(wcsimroottrigger,wcsimrootgeom,tracks,ntracks);
  
  isPi0 = (mcevent->GetGammaRatio(0) != -1);
  TCanvas *c1 = fEcan->GetCanvas();
  
  
  //draw message
  temp = (TClonesArray*)wcsimroottrigger->GetTracks();  
  fListBox->RemoveAll();
  TString message;
  
  // message = "All particles";
//   fListBox->NewEntry(message);
  for (int i = 0; i < wcsimroottrigger->GetNtrack(); i++) {
    TString tmp;
    if (((WCSimRootTrack*)temp->At(i))->GetIpnu()==111){
      tmp = "Type:pi0 ";
    }else if (((WCSimRootTrack*)temp->At(i))->GetIpnu()==11){
      tmp = "Type:e-  ";
    }else if (((WCSimRootTrack*)temp->At(i))->GetIpnu()==-11){
      tmp = "Type:e+  ";
    }else if (((WCSimRootTrack*)temp->At(i))->GetIpnu()==22){
      tmp = "Type:gamm";
    }else if (((WCSimRootTrack*)temp->At(i))->GetIpnu()==13){
      tmp = "Type:mu- ";
    }else if (((WCSimRootTrack*)temp->At(i))->GetIpnu()==-13){
      tmp = "Type:mu+ ";
    }else if (((WCSimRootTrack*)temp->At(i))->GetIpnu()==2112){
      tmp = "Type:neut";
    }else if (((WCSimRootTrack*)temp->At(i))->GetIpnu()==2212){
      tmp = "Type:prot";
    }else if (((WCSimRootTrack*)temp->At(i))->GetIpnu()==12){
      tmp = "Type:nue ";
    }else if (((WCSimRootTrack*)temp->At(i))->GetIpnu()==14){
      tmp = "Type:num ";
    }else if (((WCSimRootTrack*)temp->At(i))->GetIpnu()==-12){
      tmp = "Type:anue";
    }else if (((WCSimRootTrack*)temp->At(i))->GetIpnu()==-14){
      tmp = "Type:anum";
    }else{
      tmp.Form("Type:%d",((WCSimRootTrack*)temp->At(i))->GetIpnu());
    }
    trackid[i] = ((WCSimRootTrack*)temp->At(i))->GetId();
    
    message.Form("    P: %4.2f GeV    Dir: %4.2f %4.2f %4.2f    Vertex: %4.2f %4.2f %4.2f    Flag: %d   Particle ID: %d",((WCSimRootTrack*)temp->At(i))->GetP()/1000.,((WCSimRootTrack*)temp->At(i))->GetDir(0),((WCSimRootTrack*)temp->At(i))->GetDir(1),((WCSimRootTrack*)temp->At(i))->GetDir(2),((WCSimRootTrack*)temp->At(i))->GetStart(0)/1000.,((WCSimRootTrack*)temp->At(i))->GetStart(1)/1000.,((WCSimRootTrack*)temp->At(i))->GetStart(2)/1000.,((WCSimRootTrack*)temp->At(i))->GetFlag(),((WCSimRootTrack*)temp->At(i))->GetId());
    message = tmp + message;
    fListBox->NewEntry(message);
  }
  
  SwitchToSubcanvas(-1);
  if (Draw_Mode==1){
    DrawSignal(1);
  }else if (Draw_Mode==2){
    DrawSignal(2);
  }else{
    DrawSignal(3);
  }
  c1->Update();

  SwitchToSubcanvas(0);
  if (Draw_Mode==1){
    DrawSignal(2);
  }else if (Draw_Mode==2){
    DrawSignal(3);
  }else{
    DrawSignal(1);
  }
  c1->Update();


  SwitchToSubcanvas(2);
  if (Draw_Mode==1){
    DrawSignal(3);
  }else if (Draw_Mode==2){
    DrawSignal(1);
  }else{
    DrawSignal(2);
  }
  c1->Update();

  delete mcevent;
}

void Xin_Draw::DrawVertex(Float_t x, Float_t y, Float_t z, Int_t option)
{
  // draws the vertex indicators using the vertex's 3-dimensional coordinates.

  // option: 0=real, 1=reconstructed, 2=gamma1, 3=gamma2

  Float_t x1,y1,x2,y2;

  if (wcsimrootgeom->GetGeo_Type()==0){
    x1 = wcsimrootgeom->GetWCCylRadius()*3.14159265;
    y1 = z;
    
    x2 = -y;
    y2 = -x+wcsimrootgeom->GetWCCylLength()/2+wcsimrootgeom->GetWCCylRadius();
  }else{
    x1 = (wcsimrootgeom->GetMailBox_x() + wcsimrootgeom->GetMailBox_y()) ;
    y1 = z;
    
    x2 = x;
    y2 = y + wcsimrootgeom->GetMailBox_z()/2.+wcsimrootgeom->GetMailBox_y()/2.;
  }

  TMarker * mrkr1 = new TMarker(x1,y1,27);
  TMarker * mrkr2 = new TMarker(x2,y2,27);
  TMarker * mrkr3 = new TMarker(x1,y1,27);
  TMarker * mrkr4 = new TMarker(x2,y2,27);

  Int_t markerColor;

  if (option==0)
    markerColor=2;
  if (option==1)
    markerColor=4;
  if (option==2)
    markerColor=5;
  if (option>=3 || option<0)
    markerColor=3;

  mrkr1->SetMarkerColor(markerColor);
  mrkr2->SetMarkerColor(markerColor);
  mrkr3->SetMarkerColor(1);
  mrkr4->SetMarkerColor(1);

  mrkr1->SetMarkerSize(1);
  mrkr2->SetMarkerSize(1);
  mrkr3->SetMarkerSize(1.6);
  mrkr4->SetMarkerSize(1.6);

  mrkr3->Draw();
  mrkr4->Draw();
  mrkr1->Draw();
  mrkr2->Draw();
}


void Xin_Draw::DrawSignal(Int_t a){
  if (a==1){
    DrawBackground(eventno);
    if(isPi0) {
      SetPalette(0);
      PrintGammaInfo(mcevent->GetGammaEnergy(0), mcevent->GetGammaEnergy(1));
      DrawSizeLegend(mcevent->GetMinEnergy(),mcevent->GetMaxEnergy(),1);
      DrawColorLegend(0,0,2);
      
    }else{
      SetPalette(0);
      DrawColorLegend(mcevent->GetMinEnergy(),mcevent->GetMaxEnergy(),0);
      
    }
    for (Int_t j=0;j<wcsimroottrigger->GetNcherenkovhits();j++)
      {
	Float_t x = mcevent->GetPosition(j,0);
	Float_t y = mcevent->GetPosition(j,1);
	Float_t z = mcevent->GetPosition(j,2);
	
	Float_t size = mcevent->GetEnergyRatio_s(j);
	Float_t color;
	if(isPi0) {
	  color = mcevent->GetGammaRatio(j);
	} else {
	  color = mcevent->GetEnergyRatio_s(j);
	}
	if (mcevent->GetEnergy(j)>npe_cut){
	  if (log_scale==0){
	    DrawPMT(x,y,z,color,size);
	  }else{
	    color = fabs(1./(log(color)/log(10.)-1));
	    size = fabs(1./(log(size)/log(10.)-1));
	    DrawPMT(x,y,z,color,size);
	  }
	}
      }
  }else if(a==2){
    SetPalette(0);
    DrawBackground("Energy");
    DrawColorLegend(mcevent->GetMinEnergy(),mcevent->GetMaxEnergy(),0);
    for (Int_t j=0;j<wcsimroottrigger->GetNcherenkovhits();j++)
      {
	Float_t x = mcevent->GetPosition(j,0);
	Float_t y = mcevent->GetPosition(j,1);
	Float_t z = mcevent->GetPosition(j,2);
	Float_t color = mcevent->GetEnergyRatio_s(j);
	
	if (mcevent->GetEnergy(j)>npe_cut){
	  if (log_scale == 0){
	    DrawPMT(x,y,z,color,1,true);
	  }else{
	    color = fabs(1./(log(color)/log(10.)-1));
	    DrawPMT(x,y,z,color,1,true);
	  }
	}
      }
  }else{
    SetPalette(0);
    DrawBackground("Time (No Log Scale)");
    DrawColorLegend(mcevent->GetMinTime(),mcevent->GetMaxTime(),1);
    
    for (Int_t j=0;j<wcsimroottrigger->GetNcherenkovhits();j++)
      {
	Float_t x = mcevent->GetPosition(j,0);
	Float_t y = mcevent->GetPosition(j,1);
	Float_t z = mcevent->GetPosition(j,2);
	Float_t size = mcevent->GetEnergyRatio_s(j);
	Float_t color = mcevent->GetTimeRatio(j);
	if (mcevent->GetEnergy(j)>npe_cut){
	  DrawPMT(x,y,z,color,size,true);
	}
      }
  }

  if (flag_vr==1){
    if (isPi0){
      DrawVertexLegend(0,0,1,1);
      DrawVertex(mcevent->GetGammaVertex(0,0),
		 mcevent->GetGammaVertex(0,1),
		 mcevent->GetGammaVertex(0,2),
		 0);
      DrawVertex(mcevent->GetGammaVertex(1,0),
		 mcevent->GetGammaVertex(1,1),
		 mcevent->GetGammaVertex(1,2),
		 1);
    }else{
      DrawVertexLegend(1,0,0,0);
      DrawVertex(wcsimroottrigger->GetVtx(0),
		 wcsimroottrigger->GetVtx(1),
		 wcsimroottrigger->GetVtx(2),
		 2);
    }
    Float_t tmp_vertex[3];
    Float_t tmp_dir[3];
    Int_t tmp_par_type;
    Float_t tmp_energy;
    Int_t flag_draw = 0;
    Float_t theta_angle =42;
    if (ntracks==0){
      for (int i = 0; i < wcsimroottrigger->GetNtrack(); i++){
	tmp_vertex[0] = ((WCSimRootTrack*)temp->At(i))->GetStart(0);
	tmp_vertex[1] = ((WCSimRootTrack*)temp->At(i))->GetStart(1);
	tmp_vertex[2] = ((WCSimRootTrack*)temp->At(i))->GetStart(2);
	tmp_dir[0] = ((WCSimRootTrack*)temp->At(i))->GetDir(0);
	tmp_dir[1] = ((WCSimRootTrack*)temp->At(i))->GetDir(1);
	tmp_dir[2] = ((WCSimRootTrack*)temp->At(i))->GetDir(2);
	tmp_par_type = ((WCSimRootTrack*)temp->At(i))->GetIpnu();
	tmp_energy = ((WCSimRootTrack*)temp->At(i))->GetE()/1000.; //GeV
	if (fabs(tmp_par_type)==13){
	  flag_draw = 1;
	  theta_angle = acos(1./ref_index/sqrt(1.-105.658*105.658/1000./1000./tmp_energy/tmp_energy))*180./3.1415926;
	}else if (fabs(tmp_par_type)==11 || fabs(tmp_par_type)==22){
	  flag_draw = 1;
	  theta_angle = acos(1./ref_index/sqrt(1.-0.511*0.511/1000./1000./tmp_energy/tmp_energy))*180./3.1415926;
	}
	if (flag_draw ==1){
	  DrawRing(tmp_vertex,tmp_dir,500,theta_angle,6);
	}
      }
    }else{
      for (int i = 0; i < wcsimroottrigger->GetNtrack(); i++){
	tmp_vertex[0] = ((WCSimRootTrack*)temp->At(i))->GetStart(0);
	tmp_vertex[1] = ((WCSimRootTrack*)temp->At(i))->GetStart(1);
	tmp_vertex[2] = ((WCSimRootTrack*)temp->At(i))->GetStart(2);
	tmp_dir[0] = ((WCSimRootTrack*)temp->At(i))->GetDir(0);
	tmp_dir[1] = ((WCSimRootTrack*)temp->At(i))->GetDir(1);
	tmp_dir[2] = ((WCSimRootTrack*)temp->At(i))->GetDir(2);
	tmp_par_type = ((WCSimRootTrack*)temp->At(i))->GetIpnu();
	tmp_energy = ((WCSimRootTrack*)temp->At(i))->GetE()/1000.; //GeV
	if (fabs(tmp_par_type)==13){
	  flag_draw = 1;
	  theta_angle = acos(1./ref_index/sqrt(1.-105.658*105.658/1000./1000./tmp_energy/tmp_energy))*180./3.1415926;
	}else if (fabs(tmp_par_type)==11 || fabs(tmp_par_type)==22){
	  flag_draw = 1;
	  theta_angle = acos(1./ref_index/sqrt(1.-0.511*0.511/1000./1000./tmp_energy/tmp_energy))*180./3.1415926;
	}
	for (int j =0;j<ntracks;j++){
	  if (((WCSimRootTrack*)temp->At(i))->GetId() == tracks[j] && flag_draw == 1)
	    DrawRing(tmp_vertex,tmp_dir,500,theta_angle,6);
	}
	
      }
    }
  }
}

void Xin_Draw::SwitchToSubcanvas(Int_t i)
{
  TCanvas *c1 = fEcan->GetCanvas();
  c1->cd();

  padNum = i+1;

  if (i==0)
   subPad = new TPad("subPad","",.12,.68,.37,.88);

  if (i==1)
    subPad = new TPad("subPad","",.63,.68,.88,.88);

  if (i==2)
    subPad = new TPad("subPad","",.12,.12,.37,.32);

  if (i==3)
    subPad = new TPad("subPad","",.63,.12,.88,.32);

  if (i>=0 && i<4)
  {
    subPad->SetBorderMode(0);
    subPad->SetFillColor(kWhite);
    subPad->Range(5,5,DIM_X*1/2,DIM_Y*1/2);

    subPad->Draw();
    subPad->cd();
  }
  else
    c1->cd();
}



void Xin_Draw::DoText(const char *)
{
  TGTextEntry *te = (TGTextEntry *) gTQSender;
  Int_t id = te->WidgetId();
  switch (id) {
  case HId1:
    SetEventNo(atoi(fTbh1->GetString()));
    break;
  case HId2:
    SetNPE_cut(atoi(fTbh2->GetString()));
    break;
  case HId3:
    SetSetNo(atoi(fTbh3->GetString()));
    break;
  case HId4:
    SetRef_Index(atof(fTbh4->GetString()));
    break;
  default:
    break;
  }
}

void Xin_Draw::PrintEventNo(){
  cout << eventno << endl;
}

void Xin_Draw::SetEventNo(Int_t a){
  eventno = a;
  if (a >= tree->GetEntries()) eventno = tree->GetEntries()-1;
  if (a < 0 ) eventno = 0;
  fTeh1->SetText(Form("%d",eventno));
}

void Xin_Draw::DoNext()
{
  eventno++;
  SetEventNo(eventno);
  DoDraw();
}

void Xin_Draw::DoPrev()
{
  eventno--;
  SetEventNo(eventno);
  DoDraw();
}

void Xin_Draw::DoExit()
{
   printf("Exit application...");
   gApplication->Terminate(0);
}

// void Xin_Draw::SetStatusText(const char *txt, Int_t pi)
// {
//    // Set text in status bar.
//    // fStatusBar->SetText(txt,pi);
// }

// void Xin_Draw::EventInfo(Int_t event, Int_t px, Int_t py, TObject *selected)
// {
// //  Writes the event status in the status bar parts

//    // const char *text0, *text1, *text3;
// //    char text2[50];
// //    text0 = selected->GetTitle();
// //    SetStatusText(text0,0);
// //    text1 = selected->GetName();
// //    SetStatusText(text1,1);
// //    if (event == kKeyPress)
// //       sprintf(text2, "%c", (char) px);
// //    else
// //       sprintf(text2, "%d,%d", px, py);
// //    SetStatusText(text2,2);
// //    text3 = selected->GetObjectInfo(px,py);
// //    SetStatusText(text3,3);
// }

Xin_Draw::Xin_Draw(TString filename, const TGWindow *p, UInt_t w, UInt_t h) :
   TGMainFrame(p, w, h)
{
  
  eventno = 0;
  
  //open file 
  file = new TFile(filename);
  tree = (TTree*)file->Get("wcsimT");
  gtree = (TTree*)file->Get("wcsimGeoT");
  
  //get the event tree
  wcsimrootevent = new WCSimRootEvent();
  branch = tree->GetBranch("wcsimrootevent");
  branch->SetAddress(&wcsimrootevent);
  
  //get the geometry tree
  wcsimrootgeom = new WCSimRootGeom();
  gbranch = gtree->GetBranch("wcsimrootgeom");
  gbranch->SetAddress(&wcsimrootgeom);
  
  //get the trigger
  WCSimRootTrigger* wcsimroottrigger;
  
  // get the geometry
  gtree->GetEntry(0);
  
  
  // Create the embedded canvas
  fEcan = new TRootEmbeddedCanvas(0,this,w,h);
  Int_t wid = fEcan->GetCanvasWindowId();
  TCanvas *myc = new TCanvas("MyCanvas", 10,10,wid);
  fEcan->AdoptCanvas(myc);
//   myc->Connect("ProcessedEvent(Int_t,Int_t,Int_t,TObject*)","Xin_Draw",this, 
//                "EventInfo(Int_t,Int_t,Int_t,TObject*)");
  
  AddFrame(fEcan, new TGLayoutHints(kLHintsTop | kLHintsLeft | 
				    kLHintsExpandX  | kLHintsExpandY,0,0,1,1));
  
  // status bar
  //Int_t parts[] = {45, 15, 10, 30};
  //   fStatusBar = new TGStatusBar(this, 50, 10, kVerticalFrame);
  //   fStatusBar->SetParts(parts, 4);
  //   fStatusBar->Draw3DCorner(kFALSE);
  //   AddFrame(fStatusBar, new TGLayoutHints(kLHintsExpandX, 0, 0, 10, 0));
  
  fListBox = new TGListBox(this, 89);
  fListBox->Resize(800,100);
  AddFrame(fListBox, new TGLayoutHints(kLHintsCenterX, 
				       5, 5, 3, 4));
                                                        
  fSelected = new TList;
  

  // Create a horizontal frame containing two buttons
  TGHorizontalFrame *hframe1 = new TGHorizontalFrame(this, 200, 40);
  fCheckMulti = new TGCheckButton(hframe1, "&Mutl. sel.", 10);
  hframe1->AddFrame(fCheckMulti, new TGLayoutHints(kLHintsTop | kLHintsLeft,
					  5, 5, 5, 5));
  fCheckMulti->Connect("Clicked()", "Xin_Draw", this, "HandleButtons()"); 

  TGButtonGroup *horizontal = new TGButtonGroup(hframe1, "Color");
  horizontal->SetTitlePos(TGGroupFrame::kCenter);
  new TGRadioButton(horizontal, "Log", 1);
  new TGRadioButton(horizontal, "Linear", 0);
  horizontal->SetButton(kTextCenterX);
  horizontal->Connect("Pressed(Int_t)", "Xin_Draw", this, "SetLogType(Int_t)");
  hframe1->AddFrame(horizontal, new TGLayoutHints(kLHintsExpandX));


  TGButtonGroup *horizontal1 = new TGButtonGroup(hframe1, "Mode");
  horizontal1->SetTitlePos(TGGroupFrame::kCenter);
  new TGRadioButton(horizontal1, "Pi0", 1);
  new TGRadioButton(horizontal1, "Energy", 2);
  new TGRadioButton(horizontal1, "Time", 3);
  horizontal1->SetButton(kTextCenterX);
  horizontal1->Connect("Pressed(Int_t)", "Xin_Draw", this, "SetDrawMode(Int_t)");
  hframe1->AddFrame(horizontal1, new TGLayoutHints(kLHintsCenterX,5,5,3,4));

  TGButtonGroup *horizontal2 = new TGButtonGroup(hframe1, "V&R");
  horizontal2->SetTitlePos(TGGroupFrame::kCenter);
  new TGRadioButton(horizontal2, "Wo. VR", 0);
  new TGRadioButton(horizontal2, "W. VR", 1);
  horizontal2->SetButton(kTextCenterX);
  horizontal2->Connect("Pressed(Int_t)", "Xin_Draw", this, "SetFlagVR(Int_t)");
  hframe1->AddFrame(horizontal2, new TGLayoutHints(kLHintsCenterX,5,5,3,4));
  

  fTeh2 = new TGTextEntry(hframe1, fTbh2 = new TGTextBuffer(2), HId2);
  fTeh2->SetToolTipText("NPE_cut");
  fTbh2->AddText(0, "0");
  fTeh2->Connect("TextChanged(char*)", "Xin_Draw", this, "DoText(char*)");
  hframe1->AddFrame(fTeh2,new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));

  fTeh1 = new TGTextEntry(hframe1, fTbh1 = new TGTextBuffer(3), HId1);
  fTeh1->SetToolTipText("EventNo");
  fTbh1->AddText(0, "0");
  fTeh1->Connect("TextChanged(char*)", "Xin_Draw", this, "DoText(char*)");
  hframe1->AddFrame(fTeh1,new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));
  
  TGTextButton *draw = new TGTextButton(hframe1, "&Draw");
  draw->Connect("Clicked()", "Xin_Draw", this, "DoDraw()");
  hframe1->AddFrame(draw, new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));
  
  TGTextButton *prev = new TGTextButton(hframe1, "&Prev ");
  prev->Connect("Clicked()", "Xin_Draw", this, "DoPrev()");
  hframe1->AddFrame(prev, new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));

  TGTextButton *next = new TGTextButton(hframe1, "&Next ");
  next->Connect("Clicked()", "Xin_Draw", this, "DoNext()");
  hframe1->AddFrame(next, new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));


  fTeh3 = new TGTextEntry(hframe1, fTbh3 = new TGTextBuffer(2), HId3);
  fTeh3->SetToolTipText("Set No");
  fTbh3->AddText(0, "0");
  fTeh3->Connect("TextChanged(char*)", "Xin_Draw", this, "DoText(char*)");
  hframe1->AddFrame(fTeh3,new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));

  fTeh4 = new TGTextEntry(hframe1, fTbh4 = new TGTextBuffer(5), HId4);
  fTeh4->SetToolTipText("Refraction of Index");
  fTbh4->AddText(0, "1.333");
  fTeh4->Connect("TextChanged(char*)", "Xin_Draw", this, "DoText(char*)");
  hframe1->AddFrame(fTeh4,new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));

  TGTextButton *save = new TGTextButton(hframe1, "&Save ");
  save->Connect("Pressed()", "Xin_Draw", this, "DoSave()");
  hframe1->AddFrame(save, new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));

  TGTextButton *exit = new TGTextButton(hframe1, "&Exit ");
  exit->Connect("Pressed()", "Xin_Draw", this, "DoExit()");
  hframe1->AddFrame(exit, new TGLayoutHints(kLHintsCenterX, 5, 5, 3, 4));
  

  AddFrame(hframe1, new TGLayoutHints(kLHintsCenterX, 2, 2, 2, 2));
  
  
  GetDimensions();
  DrawBackground("DUSEL Water Cerenkov");

  Create2TonePalette(100);
  CreateMultiColorPalette(50);

  SetSetNo(1);
  SetEventNo(0);
  SetPalette(0);
  SetDrawMode(1);
  SetLogType(0);
  SetNPE_cut(0);
  SetFlagVR(1);
  SetRef_Index(1.333);
    
  // Set a name to the main frame   
  SetWindowName("Xin's WCSim Event Display");
  MapSubwindows();
  
  // Initialize the layout algorithm via Resize()
  Resize(GetDefaultSize());
  
  // Map main frame
  MapWindow();
}

void Xin_Draw::DoSave(){
  SaveAs(setno,eventno);
}

void Xin_Draw::SaveAs(TString filename)
{
  
  TCanvas *c1 = fEcan->GetCanvas();
  c1->SaveAs(filename);
}


//_____________________________________________________________________________

void Xin_Draw::SaveAs(Int_t set,Int_t evt)
{
  TString filename;
  
  if (set<100 && set>=9)
  {
    if (evt<10){
      filename.Form("./images/0%d-00%d.eps",set,evt);
    }else if (evt<100){
      filename.Form("./images/0%d-0%d.eps",set,evt);
    }else{
      filename.Form("./images/0%d-%d.eps",set,evt);
    }
  }

  if (set>=100)
  {
    if (evt<10){
      filename.Form("./images/%d-00%d.eps",set,evt);
    }else if (evt<100){
      filename.Form("./images/%d-0%d.eps",set,evt);
    }else{
      filename.Form("./images/%d-%d.eps",set,evt);
    } 
  }

  if (set<10&&set>=0)
  {
    if (evt<10){
      filename.Form("./images/00%d-00%d.eps",set,evt);
    }else if (evt<100){
      filename.Form("./images/00%d-0%d.eps",set,evt);
    }else{
      filename.Form("./images/00%d-%d.eps",set,evt);
    } 
  }

  //cout << "haha" << set << "\t" << evt << "\t" << filename << endl;

  SaveAs(filename);
}


void Xin_Draw::HandleButtons()
{
   // Handle check button.
   Int_t id;
   TGButton *btn = (TGButton *) gTQSender;
   id = btn->WidgetId();
   // printf("HandleButton: id = %d\n", id);

   if (id == 10)  
      fListBox->SetMultipleSelections(fCheckMulti->GetState());
}


Xin_Draw::~Xin_Draw()
{
   // Clean up main frame...
   Cleanup();
   if (fSelected) {
      fSelected->Delete();
      delete fSelected;
   }
   delete temp;
   delete fEcan;
   delete fListBox;
   delete fTeh1;
   delete fTbh1;
   delete wcsimrootevent;
   delete branch;
   delete wcsimrootgeom;
   delete gbranch;
   delete wcsimroottrigger;
   delete tree;
   delete gtree;
   delete subPad;
   for (Int_t i=0;i<5;i++)
     delete empty_hist[i];
   file->Close();
   delete file;
}


void Xin_Draw::SetPalette(Int_t a){
  palette_Choice = a;
  //cout << palette_Choice << endl;
}





std::vector<Float_t> Xin_Draw::ProjectToTankSurface_C(Float_t vtx[3],Float_t dir[3])
{
  // projects a vector to the tank surface (taken from SuperScan)
  float a,b,c,d=0;
  Float_t det_r = wcsimrootgeom->GetWCCylRadius();
  Float_t det_z = wcsimrootgeom->GetWCCylLength()/2;
  Float_t pos[3];
  /* Find intercept of this vector with walls of detector */
  a = (dir[0]*dir[0]) + (dir[1]*dir[1]);
  b = 2.0 * ( dir[0] * vtx[0] + dir[1] * vtx[1] );
  c = (vtx[0]*vtx[0]) + (vtx[1]*vtx[1]) - (det_r*det_r);
  
  if (a != 0.0)
  {
    /* Find intersection with walls */

    c = (b*b - 4.0*a*c);
    if (c>0.0)
    {
      c = sqrt(c);
      d = (-b+c)/(2.0*a);
      if (d<0.0) d = (-b-c)/(2.0*a);
	  
      pos[2] = vtx[2] + d*dir[2];
	  
      /* Check that the z coordinate is inside the detector */

      if (pos[2]>det_z) /* Intersects top */
      {
	d = (det_z-vtx[2])/dir[2];
	pos[2] = det_z;
      }
      else
	if (pos[2]<-det_z) /* Intersects bottom */
	{
	  d = (-det_z-vtx[2])/dir[2];
	  pos[2] = -det_z;
	}
    }
    pos[0] = vtx[0] + d*dir[0];
    pos[1] = vtx[1] + d*dir[1];
  }
  else
  {
    /* Goes out through the top */

    pos[0] = vtx[0];
    pos[1] = vtx[1];
    if (dir[2]>0.0)
    {
      pos[2] = det_z;
    }
    else
    {
      pos[2] = -det_z;
    }
  }

  std::vector<Float_t> tmp;
  for (Int_t i=0;i<3;i++)
    tmp.push_back(pos[i]);
  return tmp;
}

std::vector<std::vector<Float_t> > Xin_Draw::FindRing(Float_t vertex[3],
						  Float_t dir[3],
						  Int_t resolution=200,
						  Float_t theta=42)
{
  // this code is borrowed from superscan
 

  std::vector<std::vector<Float_t> > ring;

  Int_t i;
  Float_t pos[3], e1[3], e2[3], e3[3];
  Float_t magnitude = 0.0, phi = 0.0, dphi, st, ct;
  Float_t a, b = 0.0;
  Float_t vcone[3];
  Float_t pi=3.14159265;

  pos[0] = vertex[0];
  pos[1] = vertex[1];
  pos[2] = vertex[2];

  /* get coordinate system with e3 along dir.
     This used to be done by vbasis (random direction and all), but
     for now, make this ultra-portable, by doing by hand */

  e3[0] = dir[0]; 
  e3[1] = dir[1]; 
  e3[2] = dir[2];

  /* use either x-axis or y-axis to define a plane, and find
     transformed y-axis as perpendicular to this plane */

  if (fabs(dir[1]) < fabs(dir[0])) {

    /* if dir[1] is more nearly perpendicular to the y-axis,
       then take e2 as (y x e3) */

    e2[0] = e3[2];
    e2[1] = 0;
    e2[2] = -e3[0];
  }
  else {

    /* dir[0] is more nearly perpendicular to x-axis,
       so take e2 as (x x e3) */

    e2[0] = 0;
    e2[1] = -e3[2];
    e2[2] = e3[1];
  }

  /* now find last axis (transformed x-axis) by e2 x e3 */

  e1[0] = e2[1]*e3[2] - e2[2]*e3[1];
  e1[1] = e2[2]*e3[0] - e2[0]*e3[2];
  e1[2] = e2[0]*e3[1] - e2[1]*e3[0];

  /* normalize directions */

  magnitude = sqrt(e1[0]*e1[0] + e1[1]*e1[1] + e1[2]*e1[2]);
  e1[0] = e1[0]/magnitude;
  e1[1] = e1[1]/magnitude;
  e1[2] = e1[2]/magnitude;

  magnitude = sqrt(e2[0]*e2[0] + e2[1]*e2[1] + e2[2]*e2[2]);
  e2[0] = e2[0]/magnitude;
  e2[1] = e2[1]/magnitude;
  e2[2] = e2[2]/magnitude;

  magnitude = sqrt(e3[0]*e3[0] + e3[1]*e3[1] + e3[2]*e3[2]);
  e3[0] = e3[0]/magnitude;
  e3[1] = e3[1]/magnitude;
  e3[2] = e3[2]/magnitude;

  /* Get ready to loop over cone */

  theta *= (pi / 180.0);
  dphi = (2.0 * pi) / (float)resolution;
  st = sin(theta);
  ct = cos(theta);

  for (i = 0; i < resolution; i++) {

    /* Get a vector in the cone */
    
    a = cos(phi)*st;
    b = sin(phi)*st;
    
    vcone[0] = a * e1[0] + b * e2[0] + ct * e3[0];
    vcone[1] = a * e1[1] + b * e2[1] + ct * e3[1];
    vcone[2] = a * e1[2] + b * e2[2] + ct * e3[2];
    
    /* Find intersection of this vector with detector */
     if (wcsimrootgeom->GetGeo_Type()==0){
       ring.push_back(ProjectToTankSurface_C(pos,vcone));
     }else{
       ring.push_back(ProjectToTankSurface_M(pos,vcone));
     }
    phi += dphi;
  }
  return ring;
}

//_____________________________________________________________________________

void Xin_Draw::DrawRing(Float_t vertex[3],Float_t dir[3],Int_t resolution,Float_t theta,Int_t color)
{
  
  std::vector<std::vector<Float_t> > ring = FindRing(vertex,
						     dir,
						     resolution,
						     theta);
  
  for (Int_t i=0;i<ring.size();i++)
    {
      Float_t pos[3];
      Float_t x,y,z;
      for (Int_t j=0;j<3;j++)
	pos[j] = ring.at(i).at(j);
      x = pos[0];
      y = pos[1];
      z = pos[2];
      
      Float_t x_, y_;

      if (wcsimrootgeom->GetGeo_Type()==0){
	x_ = -atan2(pos[1],pos[0])*wcsimrootgeom->GetWCCylRadius();
	y_ = pos[2];
	if (y_>((wcsimrootgeom->GetWCCylLength())/2-.01))
	  {
	    x_ = -pos[1];
	    y_ = -pos[0] +
	      wcsimrootgeom->GetWCCylLength()/2 +
	      wcsimrootgeom->GetWCCylRadius();
	  }
	
	if (y_<-((wcsimrootgeom->GetWCCylLength())/2-.01))
	  {
	    x_ = -pos[1];
	    y_ = pos[0] -
	      wcsimrootgeom->GetWCCylLength()/2 -
	      wcsimrootgeom->GetWCCylRadius();
	  }
      }else{
	//x-z front
	if (y<-(wcsimrootgeom->GetMailBox_y()/2.-0.01)){
	  x_ = x;
	  y_ = z;
	}
	
	//y-z right
	if (x>(wcsimrootgeom->GetMailBox_x()/2.-0.01)){
	  x_ = x + (y + wcsimrootgeom->GetMailBox_y()/2.);
	  y_ = z;
	}
	
	//y-z right
	if (x<-(wcsimrootgeom->GetMailBox_x()/2.-0.01)){
	  
	  x_ = x - (y + wcsimrootgeom->GetMailBox_y()/2.);
	  y_ = z;
	  //cout << x_ << "\t" << y_ << endl;
	}
	
	//x-z back
	if (y>(wcsimrootgeom->GetMailBox_y()/2.-0.01)){
	  if (x>0){
	    x_ = wcsimrootgeom->GetMailBox_x()/2.-x + wcsimrootgeom->GetMailBox_y() + wcsimrootgeom->GetMailBox_x()/2.;
	  }else{
	    x_ = -(x+wcsimrootgeom->GetMailBox_x()/2.) - (wcsimrootgeom->GetMailBox_y() + wcsimrootgeom->GetMailBox_x()/2.);
	  }
	  
	  y_ = z;
	}
        
	//x-y top
	if (z>((wcsimrootgeom->GetMailBox_z())/2.-.01)){
	  x_ = x;
	  y_ = y + wcsimrootgeom->GetMailBox_y()/2. + wcsimrootgeom->GetMailBox_z()/2;
	}
	//x-y botom
	if (z<-((wcsimrootgeom->GetMailBox_z())/2.-.01)){
	  x_ = x;
	  y_ = -(y+wcsimrootgeom->GetMailBox_y()/2.) - wcsimrootgeom->GetMailBox_z()/2;
	}
      }

      TMarker * mrkr = new TMarker(x_,y_,20);
      mrkr->SetMarkerColor(color);
      mrkr->SetMarkerSize(.5);
      
      mrkr->Draw();
    }
  
}


std::vector<Float_t> Xin_Draw::ProjectToTankSurface_M(Float_t vtx[3],Float_t dir[3])
{
  // projects a vector to the tank surface 
  
  Float_t det_x = wcsimrootgeom->GetMailBox_x()/2.;
  Float_t det_y = wcsimrootgeom->GetMailBox_y()/2.;
  Float_t det_z = wcsimrootgeom->GetMailBox_z()/2;
  Float_t pos[6][3];

  
  // algorithim, find all six intersections, 
  // find the right direction (up to 3)
  // find the cloest one
  
  Float_t a,tmp_pos[3]={0,0,0};
  Int_t cur_d,oth_d1,oth_d2;
  Int_t b;


  // front face  y = -det_y
  cur_d = 1;
  oth_d1 = 0;
  oth_d2 = 2;
  a = -det_y;
  b = 0;
  
  pos[b][cur_d] = a;
  if (dir[cur_d]!=0){
    pos[b][oth_d1] = dir[oth_d1]/dir[cur_d]*(a - vtx[cur_d]) + vtx[oth_d1];
    pos[b][oth_d2] = dir[oth_d2]/dir[cur_d]*(a - vtx[cur_d]) + vtx[oth_d2];
  }else if (dir[cur_d]==0){
    pos[b][oth_d1] = 1e9;
    pos[b][oth_d2] = 1e9;
  }
  
  // back face y = det_y
  a = det_y;
  b = 1;
  pos[b][cur_d] = a;
  if (dir[cur_d]!=0){
    pos[b][oth_d1] = dir[oth_d1]/dir[cur_d]*(a - vtx[cur_d]) + vtx[oth_d1];
    pos[b][oth_d2] = dir[oth_d2]/dir[cur_d]*(a - vtx[cur_d]) + vtx[oth_d2];
  }else if (dir[cur_d]==0){
    pos[b][oth_d1] = 1e9;
    pos[b][oth_d2] = 1e9;
  }

  // left face  x = -det_x
  a = -det_x;
  b = 2;
  cur_d = 0;
  oth_d1 = 1;
  oth_d2 = 2;
  pos[b][cur_d] = a;
  if (dir[cur_d]!=0){
    pos[b][oth_d1] = dir[oth_d1]/dir[cur_d]*(a - vtx[cur_d]) + vtx[oth_d1];
    pos[b][oth_d2] = dir[oth_d2]/dir[cur_d]*(a - vtx[cur_d]) + vtx[oth_d2];
  }else if (dir[cur_d]==0){
    pos[b][oth_d1] = 1e9;
    pos[b][oth_d2] = 1e9;
  }

  // right face x = det_x
  a = det_x;
  b = 3;
  pos[b][cur_d] = a;
  if (dir[cur_d]!=0){
    pos[b][oth_d1] = dir[oth_d1]/dir[cur_d]*(a - vtx[cur_d]) + vtx[oth_d1];
    pos[b][oth_d2] = dir[oth_d2]/dir[cur_d]*(a - vtx[cur_d]) + vtx[oth_d2];
  }else if (dir[cur_d]==0){
    pos[b][oth_d1] = 1e9;
    pos[b][oth_d2] = 1e9;
  }


  // top face z = det_z
  a = det_z ;
  b =4;
  cur_d = 2;
  oth_d1 = 1;
  oth_d2 = 0;
  pos[b][cur_d] = a;
  if (dir[cur_d]!=0){
    pos[b][oth_d1] = dir[oth_d1]/dir[cur_d]*(a - vtx[cur_d]) + vtx[oth_d1];
    pos[b][oth_d2] = dir[oth_d2]/dir[cur_d]*(a - vtx[cur_d]) + vtx[oth_d2];
  }else if (dir[cur_d]==0){
    pos[b][oth_d1] = 1e9;
    pos[b][oth_d2] = 1e9;
  }
  
  // bottom face z = -det_z
  a = -det_z;
  b = 5;
  
  pos[b][cur_d] = a;
  if (dir[cur_d]!=0){
    pos[b][oth_d1] = dir[oth_d1]/dir[cur_d]*(a - vtx[cur_d]) + vtx[oth_d1];
    pos[b][oth_d2] = dir[oth_d2]/dir[cur_d]*(a - vtx[cur_d]) + vtx[oth_d2];
  }else if (dir[cur_d]==0){
    pos[b][oth_d1] = 1e9;
    pos[b][oth_d2] = 1e9;
  }

  Float_t min_dis = 1e9;

  for (Int_t i=0;i!=6;i++){
    if (dir[0]*(pos[i][0]-vtx[0]) + dir[1]*(pos[i][1]-vtx[1]) + dir[2]*(pos[i][2]-vtx[2]) >0){
      if (pow(pos[i][0]-vtx[0],2) +pow(pos[i][1]-vtx[1],2) + pow(pos[i][2]-vtx[2],2) < min_dis*min_dis){
	min_dis = sqrt(pow(pos[i][0]-vtx[0],2) +pow(pos[i][1]-vtx[1],2) + pow(pos[i][2]-vtx[2],2));
	tmp_pos[0] = pos[i][0];
	tmp_pos[1] = pos[i][1];
	tmp_pos[2] = pos[i][2];
	
      }
    }
  }


  std::vector<Float_t> tmp;
  for (Int_t i=0;i<3;i++)
    tmp.push_back(tmp_pos[i]);
  return tmp;
}

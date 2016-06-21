
#include "TROOT.h"
#include "TFile.h"
#include "TTree.h"
#include "WCSimRootGeom.hh"
#include "WCSimRootEvent.hh"
#include <vector>
#include <iostream>

// Input arguments:   infile -- name of the WCSim rootfile from which PMT's are to be removed
//                      should have geometry and rootevent trees.  infile is read-only
//                    outfile -- the same, but with a fraction of PMT's removed
//                    removefrac -- fraction of PMT's to remove.  For example, 0.1 will remove 10% of the
//                    existing PMT's and adjust the geometry and hits and write the result to the output rootfile

// Example of its use:  It has to be compiled because of the use of stl vectors in 
// one of the set routines for WCSimRootEvent

//{
// gSystem->Load("libWCSimRoot.so");
// gSystem->CompileMacro("pmtremove.C","k");
// pmtremove("inputsample.root","outputsample.root",0.1);
//}


void pmtremove(TString infile, TString outfile, double removefrac)
{
  // read old geometry from the input rootfile

  TFile *f2 = new TFile(infile);  
  TTree *oldtree = (TTree*)f2->Get("wcsimGeoT");
  WCSimRootGeom* oldgeom = new WCSimRootGeom();
  TBranch *gb = oldtree->GetBranch("wcsimrootgeom");
  gb->SetAddress(&oldgeom);
  oldtree->GetEntry(0);
  int oldpmtnum = oldgeom->GetWCNumPMT();
  Printf("Initial Number of PMTs: %d", oldpmtnum);

  // set up the new output file
  TFile *f3 = new TFile(outfile,"RECREATE");
  TTree *newtree = oldtree->CloneTree(0);
  WCSimRootGeom* newgeom = new WCSimRootGeom();  
  newtree->SetBranchAddress("wcsimrootgeom",&newgeom);

  newgeom->SetWCCylRadius(oldgeom->GetWCCylRadius());
  newgeom->SetWCCylLength(oldgeom->GetWCCylLength());
  newgeom->SetMailBox_x(oldgeom->GetMailBox_x());
  newgeom->SetMailBox_y(oldgeom->GetMailBox_y());
  newgeom->SetMailBox_z(oldgeom->GetMailBox_z());
  newgeom->SetGeo_Type(oldgeom->GetGeo_Type());
  newgeom->SetWCOffset(oldgeom->GetWCOffset(0),
		       oldgeom->GetWCOffset(1),
		       oldgeom->GetWCOffset(2));
  newgeom->SetOrientation(oldgeom->GetOrientation());
  newgeom->SetWCPMTRadius(oldgeom->GetWCPMTRadius());

  // associates old PMTs with new PMTs  Index into it is the old pmt number,
  // gives back new PMT number.  If PMT is removed, this array has -1 in it.

  const int asize = oldpmtnum + 1;
  int pmtassoc[asize];

  int newpmtnum = 0;
  int ict = 0;  // old floor count
  for (int i=0;i<oldpmtnum;i++)
    {
      WCSimRootPMT pmt = oldgeom->GetPMT(i);
      int oldtubeno = pmt.GetTubeNo();
      double ft = removefrac*( (double) i);
      int icc = (int) floor(ft);  // check to see when this rolls around another integer

      if ( ict != icc)
	{
	  ict = icc;  
	  pmtassoc[oldtubeno] = -1;  // and remove this tube
	}
      else
	{
          pmtassoc[oldtubeno] = newpmtnum+1;
	  float rot[3];
	  float pos[3];
	  for (int j=0;j<3;j++)
	    { 
	      rot[j] = pmt.GetOrientation(j);
	      pos[j] = pmt.GetPosition(j);
	    }
          newgeom->SetPMT(newpmtnum,newpmtnum+1,pmt.GetCylLoc(),rot,pos,true);
          newpmtnum++;
	}
      //if (i<100)  std::cout << "input pmt: " << i << " " << oldtubeno << " assoc: " << pmtassoc[oldtubeno] <<  std::endl;

    }
  newgeom->SetWCNumPMT(newpmtnum);
  std::cout << "New PMT count: " << newpmtnum << std::endl;
  newtree->Fill();
  newtree->Write();

  //-------------------------------------------------------------------------------------------
  //----------  Go through events -- drop PMT hits and renumber PMTs with the ones we have.
  //-------------------------------------------------------------------------------------------

  TTree *oldevtree = (TTree*) f2->Get("wcsimT");
  int nevent = oldevtree->GetEntries();
  std::cout << "Number of events: " << nevent << std::endl;

  // Create a WCSimRootEvent to put stuff from the old tree in
  WCSimRootEvent* oldsuperevent = new WCSimRootEvent();
  TBranch *branch = oldevtree->GetBranch("wcsimrootevent");
  branch->SetAddress(&oldsuperevent);
  // Force deletion to prevent memory leak 
  oldevtree->GetBranch("wcsimrootevent")->SetAutoDelete(kTRUE);

  // Initialize the output tree

  TTree *newevtree = oldevtree->CloneTree(0);
  WCSimRootEvent *newsuperevent = new WCSimRootEvent();
  newsuperevent->Initialize();
  newevtree->SetBranchAddress("wcsimrootevent",&newsuperevent);

  // Now loop over events
  for (int ev=0; ev<nevent; ev++)
    {
      // Read the event from the tree into the WCSimRootEvent instance
      oldevtree->GetEntry(ev);      

      // new output event -- allocate memory here and let it go out of scope at the end of the loop.
      // performance isn't so much an issue here.

      for (int itrigger = 0; itrigger < oldsuperevent->GetNumberOfEvents(); itrigger++)
	{
	  //std::cout << "about to get a trigger:  Event: " << ev << " Trigger: " << itrigger << std::endl;
	  WCSimRootTrigger *oldtrigger;
	  oldtrigger = oldsuperevent->GetTrigger(itrigger);
	  if (itrigger > 0) newsuperevent->AddSubEvent();  // don't have to add the first trigger, done by initialize
	  WCSimRootTrigger *newtrigger = newsuperevent->GetTrigger(itrigger);
	  
	  WCSimRootEventHeader *oldheader = oldtrigger->GetHeader();
	  newtrigger->SetHeader(oldheader->GetEvtNum(),oldheader->GetRun(),oldheader->GetDate(),oldheader->GetSubEvtNumber());
	  newtrigger->SetMode(oldtrigger->GetMode());
	  newtrigger->SetVtxvol(oldtrigger->GetVtxvol());
	  for (int i=0;i<3;i++) newtrigger->SetVtx(i,oldtrigger->GetVtx(i));
	  newtrigger->SetJmu(oldtrigger->GetJmu());
	  newtrigger->SetJp(oldtrigger->GetJp());
	  newtrigger->SetNpar(oldtrigger->GetNpar());
	
	  WCSimRootPi0 *pi0info = oldtrigger->GetPi0Info();
	  float pi0vtx[3],gammae[2],gammavtx[2][3];
	  int gammaid[2];
	  for (int i=0;i<3;i++)
	    { 
	      pi0vtx[i] = pi0info->GetPi0Vtx(i);
	      gammavtx[0][i] = pi0info->GetGammaVtx(0,i);
	      gammavtx[1][i] = pi0info->GetGammaVtx(1,i);
	    }
	  for (int i=0;i<2;i++)
	    {
	      gammaid[i] = pi0info->GetGammaID(i);
	      gammae[i] = pi0info->GetGammaE(i);
	    }
	  newtrigger->SetPi0Info(pi0vtx,gammaid,gammae,gammavtx);

	  // Get the number of tracks
	  int ntrack = oldtrigger->GetNtrack();
	  //printf("ntracks=%d\n",ntrack);
    
	  // Loop through elements in the TClonesArray of WCSimTracks
	  for (int i=0; i<ntrack; i++)
	    {
	      TObject *element = (oldtrigger->GetTracks())->At(i);
	      WCSimRootTrack *track = dynamic_cast<WCSimRootTrack*>(element);
	      float dir[3],pdir[3],stop[3],start[3];
	      for (int j=0;j<3;j++)
		{
		  dir[j] = track->GetDir(j);
		  pdir[j] = track->GetPdir(j);
		  stop[j] = track->GetStop(j);
		  start[j] = track->GetStart(j);
		}
	      newtrigger->AddTrack(track->GetIpnu(),
				   track->GetFlag(),
				   track->GetM(),
				   track->GetP(),
				   track->GetE(),
				   track->GetStartvol(),
				   track->GetStopvol(),
				   dir,
				   pdir,
				   stop,
				   start,
				   track->GetParenttype(),
				   track->GetTime(),
				   track->GetId());

	    }  // End of loop over tracks

	  // stopped here March 22 -- keep copying info from the old trigger to the new trigger
    
	  // Now look at the Cherenkov hits

	  // Get the number of Cherenkov hits.
	  // Note... this is *NOT* the number of photons that hit tubes.
	  // It is the number of tubes hit with Cherenkov photons.
	  // The number of digitized tubes will be smaller because of the threshold.
	  // Each hit "raw" tube has several photon hits.  The times are recorded.
	  // See chapter 5 of ../doc/DetectorDocumentation.pdf
	  // for more information on the structure of the root file.
	  //  
	  // For digitized info (one time/charge tube after a trigger) use
	  // the digitized information.
	  //
    
	  int ncherenkovhits     = oldtrigger->GetNcherenkovhits();
	  int ncherenkovdigihits = oldtrigger->GetNcherenkovdigihits(); 
    
	  // Grab the big arrays of times and parent IDs
	  TClonesArray *timeArray = oldtrigger->GetCherenkovHitTimes();
    
	  // Loop through elements in the TClonesArray of WCSimRootCherenkovHits and copy info for non-removed PMT's

	  for (int i=0; i< ncherenkovhits; i++)
	    {
	      TObject *Hit = (oldtrigger->GetCherenkovHits())->At(i);
	      WCSimRootCherenkovHit *wcsimrootcherenkovhit = dynamic_cast<WCSimRootCherenkovHit*>(Hit);

	      int tubeNumber     = wcsimrootcherenkovhit->GetTubeID();
	      int timeArrayIndex = wcsimrootcherenkovhit->GetTotalPe(0);
	      int peForTube      = wcsimrootcherenkovhit->GetTotalPe(1);

	      if (tubeNumber < 1 || tubeNumber > oldpmtnum)
		{
		  std::cout << "Error in pmtremove: tube number out of range: " << tubeNumber << " max: " << oldpmtnum << std::endl;
		  exit(0); // die if we get a bad tube number
		}
	      int newtubenumber = pmtassoc[tubeNumber];
	      if (newtubenumber > 0)   // keep the tube's hits
		{
		  vector<float> truetime;
		  vector<int> ppid;
		  for (int j=0; j<peForTube; j++)
		    {
		      WCSimRootCherenkovHitTime *HitTime = dynamic_cast<WCSimRootCherenkovHitTime*>(timeArray->At(timeArrayIndex+j));
		      truetime.push_back(HitTime->GetTruetime());
		      ppid.push_back(HitTime->GetParentID());
		    }
		  newtrigger->AddCherenkovHit(newtubenumber,truetime,ppid);
		}
      	    } // End of loop over Cherenkov hits

	  // Copy digitized hits for non-removed PMT's, and recompute sumq

	  float sumq = 0;

	  for (int i=0;i<ncherenkovdigihits;i++)
	    {
	      // Loop through elements in the TClonesArray of WCSimRootCherenkovDigiHits
	      TObject *element = (oldtrigger->GetCherenkovDigiHits())->At(i);
	      WCSimRootCherenkovDigiHit *wcsimrootcherenkovdigihit = 
		dynamic_cast<WCSimRootCherenkovDigiHit*>(element);
	      int tubeNumber = wcsimrootcherenkovdigihit->GetTubeId();
	      if (tubeNumber < 1 || tubeNumber > oldpmtnum)
		{
		  std::cout << "Error in pmtremove: tube number out of range: " << tubeNumber << " max: " << oldpmtnum << std::endl;
		  exit(0); // die if we get a bad tube number
		}
	      int newtubenumber = pmtassoc[tubeNumber];
	      if (newtubenumber > 0)   // keep the tube's hits
		{
	          newtrigger->AddCherenkovDigiHit(wcsimrootcherenkovdigihit->GetQ(),
						  wcsimrootcherenkovdigihit->GetT(),
						  newtubenumber);
	          sumq += wcsimrootcherenkovdigihit->GetQ();
		}
	      //if ( i < 10 ) // Only print first XX=10 tubes
	      //printf("q, t, tubeid: %f %f %d \n",wcsimrootcherenkovdigihit->GetQ(),
	      //       wcsimrootcherenkovdigihit->GetT(),wcsimrootcherenkovdigihit->GetTubeId());
	
	    } // End of loop over Cherenkov digihits
	  newtrigger->SetSumQ(sumq);
	  //if (ev<100) { std::cout << "Sum charge: " << ev << " " << itrigger << " " <<  oldtrigger->GetSumQ() << " " << sumq <<  std::endl; }
	} // End of loop over triggers in the event

      newevtree->Fill();
      newevtree->Write();

      // reinitialize super events between loops.

      oldsuperevent->ReInitialize();
      newsuperevent->ReInitialize();
    
    } // End of loop over events
 


  delete f2;
  delete f3;
}

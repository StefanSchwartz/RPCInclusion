///////////////////////
// twoH_RPC_pt.C
//
// Author: David Curry
//
// Studies the effects of inserting one rpc hit into a three hit csc track and finding new pT.
//
//////////////////////

#include<exception>
#include<vector>
#include<iostream>
#include<cmath>

#include<TClass.h>
#include<TSystem.h>
#include<TFile.h>
#include<TTree.h>
#include<TCanvas.h>
#include<TH1.h>
#include<TH2.h>
#include<TLegend.h>
#include<TMath.h>
#include<TROOT.h>
#include<TStyle.h>
#include<TString.h>
#include<TMultiGraph.h>
#include<TEfficiency.h>
#include<TString.h>

using namespace std;

#include "include/MapCounter.h"
#include "include/EffiEvent.h"
#include "src/EffiEvent.cc"

void twoHit_RPC_pt(int printLevel = 0){
  
  // class that wraps TTree variables and methods
  EffiEvent evt;
  
  TFile* file = TFile::Open("/exports/uftrig01a/dcurry/data/rpc/2012D_raw_reco_6_19_merge_small.root");
  
  evt.AttachToFile(file);
  
  // Output File
  TFile *newfile = new TFile("twoHit_RPC_pt.root","recreate");
  
  bool isMC = false;
  
  double Pi  = acos(-1.);
  
  //========= define Hists ===================================
  

  const float rate[] = {0., 3., 5., 7., 10., 12., 16., 20., 30.};
  const int N_rate = (sizeof(rate)/sizeof(float) -1);
  
  TH1F * hrate_t = new TH1F("hrate_t", "CSCTF Rate Track", N_rate,  rate);
  TH1F * hrate_r = new TH1F("hrate_r", "CSCTF Rate RPC", N_rate,  rate);
  
  TH1F* hres_t = new TH1F("hres_t", "", 100, -10, 10);
  TH1F* hres_r = new TH1F("hres_r", "", 100, -10, 10);

  TH1F* hdeta_csc_13 = new TH1F("hdeta_csc_13" ,"", 50, -0.1, 0.1);
  
  TH2F* hdeta_csc_13_invpt = new TH2F("hdeta_csc_13_invpt", "", 50, 0, 0.5, 50, -0.1, 0.1); 
  
  // ========================================================
  
  //Start loop over events
  //for (int iEvt = 0; iEvt < evt.GetEntries(); iEvt++) {

  // for testing
  for (int iEvt = 0; iEvt < 500000; iEvt++) {

    evt.GetEntry(iEvt);

    if ( (iEvt % 100000) == 0 ) printf(" --- Event # %6d \n", iEvt+1);

    if (printLevel > 0) {
      cout << "\n=======================  Starting new event loop! ===============================" << endl;
      cout << "Event = " << evt.Event << endl;
      cout << " # csctf tracks in event = " << evt.SizeTrk       << endl;
      cout << " # RPC Hits in event     = " << evt.rpc_NumLctsTrk << endl;
      cout << " # Muons in event        = " << evt.muonSize << endl;
    }


    for (int iReco = 0; iReco < evt.muonSize; iReco++) {
      
      if (evt.muon_pt[iReco] < 2) continue;
      if ( abs(evt.muon_eta[iReco]) < 1.2 || abs(evt.muon_eta[iReco]) > 2.4 )  continue;
      
      if (printLevel > 0) cout << "Looping over muon # " << iReco << endl;

      vector<double> best_dr; // fill with all dr values to find best track-muon match
      
      float rpc_pt = -999; // new pt value when rpc is inserted into track
      bool is_rpc_inserted = false;

      bool isMatched = false;
      // Loop over csc tracks
      for (int iCSCTrk = 0; iCSCTrk < evt.SizeTrk; iCSCTrk++) {
	
	best_dr.clear();
	
	// find which track, if any, is matched to muon.
	double deta = evt.EtaTrk[iCSCTrk] - evt.muon_eta[iReco];
	double dr = sqrt( deta*deta );
	
	if (dr < 0.2) {

	  if (printLevel > 0) cout << "Muon # " << iReco << " is matched to Track # " << iCSCTrk << endl;
	  isMatched = true;
	  best_dr.push_back(dr);
	}

      } // end loop over tracks

      int iCSCTrk = -999; // set to be the best macthed track #
      
      // skip muons who are not matched to a track
      if (!isMatched) continue;

      
      // returns which track # is matched to current muon
      auto const it = lower_bound(best_dr.begin(), best_dr.end(), 0);
      
      if (printLevel > 0) cout << "Best delta R match is Track # " << it - best_dr.begin() << endl;

      iCSCTrk = it - best_dr.begin();

      // two hit tracks only
      if (evt.NumLctsTrk[iCSCTrk] != 2) continue;


      // =============== What mode is track? ================
      // Possible modes are station combinations:
      // 1-2  Mode 1 sum 3
      // 1-3  Mode 2 sum 4
      // 1-4  Mode 3 sum 5
      // 2-3  Mode 4 sum 5
      // 2-4  Mode 5 sum 6
      // 3-4  Mode 6 sum 7
      
      int temp_mode  = 0;
      int track_mode = 0;
      bool isStation_1 = false;  bool isStation_2 = false;
      
      // loop over tracks hits to find mode
      for (int iCsc=0; iCsc < evt.NumLctsTrk[iCSCTrk]; iCsc++) {
	
	temp_mode += evt.trLctStation[iCSCTrk][iCsc];
	if (evt.trLctStation[iCSCTrk][iCsc] == 1) isStation_1 = true;
	if (evt.trLctStation[iCSCTrk][iCsc] == 2) isStation_2 = true;
      }

      // Which mode is it?
      switch (temp_mode) {
      case 3: track_mode = 1;
	break;
      case 4: track_mode = 2;
	break;
      case 6: track_mode = 5;
	break;
      case 7: track_mode = 6;
	break;
      }
      
      // In the event of mode 5 we need to find which configuration
      if ( (track_mode == 0) && (isStation_1) ) track_mode = 3;
      else if (track_mode == 0) track_mode = 4;
      
      if (printLevel>1) cout << " This track is mode  = " << track_mode << endl << endl;
      
      // only look at two hit tracks(stations 1 and 3)
      if (track_mode != 2) continue;

      if (printLevel>0) cout << " ----> Track is mode 2\n";

            // What is deta of csc1-3 for the best track?
      float deta13 = evt.trLctglobalPhi[iCSCTrk][0] - evt.trLctglobalPhi[iCSCTrk][1];
      
      if (printLevel > 0) cout << "Deta CSC1-3 = " << deta13 << endl;

      // fill plots
      hdeta_csc_13 -> Fill(deta13);
      
      hdeta_csc_13_invpt -> Fill(1/evt.muon_pt[iReco], deta13);


      //vector to store rpc pt values.  After loop selct highest(?) one
      vector<float> pt_vec;
      vector<float> dphi12; vector<float> dphi23;
      
      dphi12.clear();
      dphi23.clear();
      
      // loop over csc track hits to look for rpc insertion candidate
      for (int iCsc=0; iCsc < evt.NumLctsTrk[iCSCTrk]; iCsc++) {   
      
	if (printLevel>0) cout << "Looping over CSC Lct # " << iCsc << " in station # " << evt.trLctStation[iCSCTrk][iCsc] << endl;
	
	//if (evt.trLctStation[iCSCTrk][iCsc] != 1 || evt.trLctStation[iCSCTrk][iCsc] != 3) continue;

	pt_vec.clear();

	// loop over rpc hits
	for (int iRpc = 0; iRpc < evt.rpc_NumLctsTrk; iRpc++) {
	  
	  if (printLevel>0) cout << "   Looping over RPC hit # " << iRpc << endl;
	  
	  if (evt.rpc_Station[iRpc] != 2) continue;
	  
	  // dont look at rpc hits that are not in 0.9-1.6 eta
	  if ( (abs(evt.rpc_gblEta[iRpc]) < 0.9) || (abs(evt.rpc_gblEta[iRpc]) > 1.6) ) continue;
	  
	  // if csc hit is in station 1, see if an rpc hit is in station two.
 	  float dphi = 999; float dphi_23 = 999;
	  if (evt.trLctStation[iCSCTrk][iCsc] == 1 && evt.rpc_Station[iRpc] == 2) {

	    dphi = evt.rpc_gblPhi[iRpc] - evt.trLctglobalPhi[iCSCTrk][iCsc];
	    
	    dphi12.push_back( abs(dphi) );
	    
	    if (printLevel > 0) cout << "----> CSC1 and RPC 2.  Dphi = " << dphi << endl; 
	  }
	  
	  // if csc hit is in station 3, see if an rpc hit is in station two.
	  if (evt.trLctStation[iCSCTrk][iCsc] == 3 && evt.rpc_Station[iRpc] == 2) {

	    dphi_23 = evt.rpc_gblPhi[iRpc] - evt.trLctglobalPhi[iCSCTrk][iCsc];

	    dphi23.push_back( abs(dphi_23) );

	    if (printLevel > 0) cout << "----> CSC3 and RPC 2.  Dphi = " << dphi_23 << endl;
	  }
	  
	  // ===============================================
	  // For rpc hits in stations 2 find pt when dphi < 0.1.  The InvPt profile has been fitted for predictive behavior.
	  // Fit = [0]*x + [1], where [0] = 1.44178 and [1] = 1.20259e-01, and x = dphi
	  if (dphi == 999) continue;
	  
	  if ( abs(dphi) < 0.5) {
	    
	    is_rpc_inserted = true;
	    
	    /*	    
	    if (evt.muon_pt[iReco] > 10) {
	      cout << "track pt  = " << evt.PtTrk[iCSCTrk] << endl;
	      cout << "rpc pt    = " << rpc_pt << endl;
	      cout << "muon pt   = " <<  evt.muon_pt[iReco] << endl;
	      cout << "dphi      = " << abs(dphi) << endl << endl; 
	    }
	    */
	    
	    rpc_pt = 1/(1.44178 * abs(dphi) + 1.20259e-01);
	    
	    if ( abs(dphi) < 0.05 &&
		 evt.PtTrk_reco_best_two[iCSCTrk][iCsc] != -999) rpc_pt = evt.PtTrk_reco_best_two[iCSCTrk][iCsc];
	    
	    //	    if ( abs(deta13) < 0.01 && abs(dphi) < 0.05) rpc_pt = 11; 
	    
	    if ( abs(deta13) < 0.01 && abs(dphi) <= 0.018) rpc_pt = 15;

	    if ( abs(deta13) < 0.01 && abs(dphi) <= 0.015) rpc_pt = 21;


	    //if ( abs(dphi) <= 0.05 ) rpc_pt = 12; 
	    
	    //if ( abs(dphi) <= 0.01 ) rpc_pt = 25;
	    
	    pt_vec.push_back(rpc_pt);
	    
	  }  
	  
	} // end loop over rpc hits
	
      } // end loop over csc track lcts

      if (!is_rpc_inserted) continue;

      /*
      // ======================================================================
      // find largest total phi bend from vectors.
      vector<float> dphi_total; // stores sum of dphi12 and dphi23
      vector<float> temp_vec;   // used to find largest dphi
      float biggest_dphi = 999;
      
      for (int i=0; i < dphi12.size(); i++) dphi_total.push_back( dphi12[i] + dphi23[i] );
      
      temp_vec = dphi_total;
      
      sort(temp_vec.begin(), temp_vec.end());
      
      biggest_dphi = temp_vec.front();
      
      if (printLevel>0) cout << "Biggest Dphi total = " << biggest_dphi << endl; 
      
      // which rpc does this biggest dphi belong to?
      auto best_pt_pos = find(dphi_total.begin(), dphi_total.end(), biggest_dphi) - dphi_total.begin();
      
      if (printLevel>0) cout << " best pt pos = " << best_pt_pos << endl;

      float best_rpc = pt_vec[best_pt_pos]; 
      
      if (printLevel>0) cout << "best pt  = " << best_rpc << endl;
      
      // =========================================================================
      */

      // find highest rpc pt value from vector
      float best_rpc = -999;
      
      sort(pt_vec.begin(), pt_vec.end());
      
      auto const iter = lower_bound(pt_vec.begin(), pt_vec.end(), 100);
      
      best_rpc = *iter;
      
      if (printLevel>0) cout << "Highest rpc pt value =  " << best_rpc << endl;
      

      // Calculate rate between track pt and new rpc pt
      for (int bin = N_rate -1 ; bin >=0 ; bin-- ) {
	
	if (evt.PtTrk[iCSCTrk] > rate[bin] ) hrate_t -> Fill(rate[bin]);

	if (best_rpc > rate[bin] ) hrate_r -> Fill(rate[bin]);
	
      }

      // Fill resolution histograms
      hres_t -> Fill( (evt.PtTrk[iCSCTrk] - evt.muon_pt[iReco])/evt.muon_pt[iReco] );
      
      hres_r ->Fill( (best_rpc - evt.muon_pt[iReco])/evt.muon_pt[iReco] );

    }// endl loop over muons
    
  } //end loop over events

  
  //  ============== Write Hists to file =====================
  
  hrate_t -> Write();
  hrate_r -> Write();

  hres_t -> Write();
  hres_r -> Write();

  hdeta_csc_13 -> Write();
  
  hdeta_csc_13_invpt -> Write();

  delete newfile;    
  
} // end main

///////////////////////
// RPCeffcompare_two.C
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

#include <TClass.h>
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

using namespace std;

#include "include/MapCounter.h"
#include "include/EffiEvent.h"
#include "src/EffiEvent.cc"

void RPCeffcompare_two(int printLevel = 0){

  // class that wraps TTree variables and methods
  EffiEvent evt;

  //TFile* file = TFile::Open("../test/rpc_tuple_data_test.root");
  TFile* file = TFile::Open("../test/rpc_tuple_allevtTest_MC.root");
  //TFile* file = TFile::Open("/cms/data/store/user/dcurry/rpc/2012C_best4/merged.root");

  evt.AttachToFile(file);



  //========= define Hists ===================================
  
  TH1F* hdeta12 = new TH1F("hdeta12", "Delta Eta 1-2", 25, -0.2, 0.2);
  TH1F* hdphi12 = new TH1F("hdphi12", "Delta Phi 1-2", 25, -0.2, 0.2);
  TH1F* hdphi23 = new TH1F("hdphi23", "Delta Phi 2-3", 25, -0.2, 0.2);
  TH1F* hdeta23 = new TH1F("hdeta23", "Delta Eta 2-3", 25, -0.2, 0.2);

  TH2F* hdphi12_all = new TH2F("hdphi12_all", "Delta Phi 1-2", 25, -0.2, 0.2, 25, 0, 40);
  TH2F* hdphi23_all = new TH2F("hdphi23_all", "Delta Phi 2-3", 25, -0.2, 0.2, 25, 0, 40);
    
  TH1F* hdeta12_r = new TH1F("hdeta12_r", "RPC Delta Eta 1-2", 25, -0.2, 0.2);
  TH1F* hdphi12_r = new TH1F("hdphi12_r", "RPC Delta Phi 1-2", 25, -0.2, 0.2);
  TH1F* hdphi23_r = new TH1F("hdphi23_r", "RPC Delta Phi 2-3", 25, -0.2, 0.2);
  TH1F* hdeta23_r = new TH1F("hdeta23_r", "RPC Delta Eta 2-3", 25, -0.2, 0.2);
  
  TH2F* hdeta12_pt = new TH2F("hdeta12_pt", "Delta Eta 1-2", 25, -0.2, 0.2, 25, 0, 40);
  TH2F* hdphi12_pt = new TH2F("hdphi12_pt", "Delta Phi 1-2", 25, -0.2, 0.2, 25, 0, 40);
  TH2F* hdphi23_pt = new TH2F("hdphi23_pt", "Delta Phi 2-3", 25, -0.2, 0.2, 25, 0, 40);
  TH2F* hdeta23_pt = new TH2F("hdeta23_pt", "Delta Eta 2-3", 25, -0.2, 0.2, 25, 0, 40);

  TH2F* hdeta12_r_pt = new TH2F("hdeta12_r_pt", "RPC Delta Eta 1-2", 25, -0.2, 0.2, 25, 0, 40);
  TH2F* hdphi12_r_pt = new TH2F("hdphi12_r_pt", "RPC Delta Phi 1-2", 25, -0.2, 0.2, 25, 0, 40);
  TH2F* hdphi23_r_pt = new TH2F("hdphi23_r_pt", "RPC Delta Phi 2-3", 25, -0.2, 0.2, 25, 0, 40);
  TH2F* hdeta23_r_pt = new TH2F("hdeta23_r_pt", "RPC Delta Eta 2-3", 25, -0.2, 0.2, 25, 0, 40);
  
  TH2F* hdphi1_pt = new TH2F("hdphi1_pt", "Delta Phi Station 1 vs Muon Pt", 25, -0.1, 0.1, 25, 0, 60);
  TH2F* hdphi2_pt = new TH2F("hdphi2_pt", "Delta Phi Station 2 vs Muon Pt", 25, -0.1, 0.1, 25, 0, 60);  
  TH2F* hdphi3_pt = new TH2F("hdphi3_pt", "Delta Phi Station 3 vs Muon Pt", 25, -0.1, 0.1, 25, 0, 60);
  
  TH2F* hdphi1_pt_inv = new TH2F("hdphi1_pt_inv", "Delta Phi Station 1 vs Muon Inv Pt", 25, -0.1, 0.1, 25, 0, 60);
  TH2F* hdphi2_pt_inv = new TH2F("hdphi2_pt_inv", "Delta Phi Station 2 vs Muon Inv Pt", 25, -0.1, 0.1, 25, 0, 60);
  TH2F* hdphi3_pt_inv = new TH2F("hdphi3_pt_inv", "Delta Phi Station 3 vs Muon Inv Pt", 25, -0.1, 0.1, 25, 0, 60);

  TH2F* hdphi11_csc_phi = new TH2F("hdphi11_csc_phi", "Delta Phi Station 1 vs CSC Phi", 25, -0.1, 0.1, 25, -3.14, 3.14);
  TH2F* hdphi22_csc_phi = new TH2F("hdphi22_csc_phi", "Delta Phi Station 2 vs CSC Phi", 25, -0.1, 0.1, 25, -3.14, 3.14);
  TH2F* hdphi33_csc_phi = new TH2F("hdphi33_csc_phi", "Delta Phi Station 3 vs CSC Phi", 25, -0.1, 0.1, 25, -3.14, 3.14);
  
  TH1F* hdphi1 = new TH1F("hdphi1", "Delta Phi Station 1", 25, -0.1, 0.1);
  TH1F* hdphi2 = new TH1F("hdphi2", "Delta Phi Station 2", 25, -0.1, 0.1);
  TH1F* hdphi3 = new TH1F("hdphi3", "Delta Phi Station 3", 25, -0.1, 0.1);

  TH1F* hpt     = new TH1F("hpt", "Muon Pt", 50, 0, 50);
  TH1F* hpt_inv = new TH1F("hpt_inv", "Inv Muon Pt", 50, 0, 1);

  //==========================================================
  

  // Global Counters =========================================

  // =========================================================

  //Start loop over events
  for (int iEvt = 0; iEvt < evt.GetEntries(); iEvt++) {
    
    // for testing
  // for (int iEvt = 0; iEvt < 1000000; iEvt++) {

    evt.GetEntry(iEvt);
    if ( ( iEvt % 100000) == 0 ) printf(" --- Event # %6d \n", iEvt+1);

    if (printLevel > 1) {
      cout << "\n=======================  Starting new event loop! ===============================" << endl;
      cout << "Event = " << iEvt << endl;
      cout << " # csctf tracks in event = " << evt.SizeTrk       << endl;
      cout << " # RPC Hits in event = " << evt.rpc_NumLctsTrk << endl;
    }
    
    // Loop over csctf tracks
    for (int iCSCTrk = 0; iCSCTrk < evt.SizeTrk; iCSCTrk++) {
      
      if (printLevel > 1) {
	cout << "\n============ Looping over CSC track # " << iCSCTrk << endl;
	cout << "   # CSC Hits in track = " << evt.NumLctsTrk[iCSCTrk] << endl;
      }
      
      if (evt.NumLctsTrk[iCSCTrk] != 3) continue;
 	    
      if (printLevel > 1) cout << "Track has 3 LCTS. Continue On. \n";

      
      // ======= Muon Info ==========
      // Loop over muons and match to this track. Get pt of muon

      double pt_muon = -999;
      
      for (int iReco = 0; iReco < evt.muonSize; iReco++) {
	
	if (printLevel > 1) cout << "Looping over muon # " << iReco << endl;

	// use dr to match track to muon
	double dphi = abs( abs( abs(evt.PhiTrk[iCSCTrk]-evt.muon_phi[iReco]) - 3.14) -3.14);
	double deta = abs( evt.EtaTrk[iCSCTrk] - evt.muon_eta[iReco] );
	
	double dr = sqrt( dphi*dphi + deta*deta );
	
	if (printLevel > 1) cout << " Delta R Track-muon = " << dr << endl;
	
	if (dr < 0.2) {
	  
	  if (printLevel > 1) cout << "Muon # " << iReco << " is matched to Track # " << iCSCTrk << endl;
	  
	  
	  pt_muon = evt.muon_pt[iReco];
	  break;
	}

      } // end loop over muons
            

      // First loop to determine how many rpc matches are in the track/ what mode track is ===============
      // For three hit tracks can only be modes: 1,2,3 / 1,2,4 / 2,3,4 / 1,3,4 
      
      int temp_csc_mode   = 0;
      int csc_mode        = 0;
      float track_eta     = 0;
      int track_sector    = 0;

      // Loop over csc rpc hits to get basic track info: mode, is it matched, etc.
      for (int iCsc=0; iCsc < evt.NumLctsTrk[iCSCTrk]; iCsc++) {
        temp_csc_mode += evt.trLctStation[iCSCTrk][iCsc];	
	track_eta = evt.trLctglobalEta[iCSCTrk][iCsc];
	track_sector = evt.trLctSector[iCSCTrk][iCsc];
      }

      switch(temp_csc_mode) {
      case 6: csc_mode = 1;
	break;
      case 7: csc_mode = 2;
	break;
      case 9: csc_mode = 4;
	break;
      case 8: csc_mode = 3;
	break;
      }

      if (printLevel>0) cout << "Track is mode = " << csc_mode << endl;
	      
      // =============================================================================
      // Track mode Filter
      if (csc_mode != 1) continue;

      //if ( abs(track_eta) > 1.479 ) continue;
     
      //if ( track_sector != 6 ) continue;

      // =============================================================================
      
      // Fill pt hist
      hpt -> Fill(pt_muon);
      hpt_inv -> Fill(1/pt_muon);
      
      // Loop over csc hits and look at bending between station 1-2 ans 2-3
      float dphi12 = 0; float dphi23 = 0;
      float deta12 = 0; float deta23 = 0;
      float phi1; float phi2; float phi3;
      float eta1; float eta2; float eta3;
      int which_lct_station2 = -999;


      for (int iCsc=0; iCsc < 3; iCsc++) {
	
	if (printLevel > 1) {
	  cout << "\nLooping over CSC LCT # " << iCsc << endl; 
	  cout << "  csc_gblEta = " << evt.trLctglobalEta[iCSCTrk][iCsc] << endl;
	  cout << "  csc_gblPhi = " << evt.trLctglobalPhi[iCSCTrk][iCsc] << endl;
	}
	
	if (evt.trLctStation[iCSCTrk][iCsc] == 1) {
	  eta1 = evt.trLctglobalEta[iCSCTrk][iCsc];
	  phi1 = evt.trLctglobalPhi[iCSCTrk][iCsc];
	}
	
	if (evt.trLctStation[iCSCTrk][iCsc] == 2) {
          eta2 = evt.trLctglobalEta[iCSCTrk][iCsc];
          phi2 = evt.trLctglobalPhi[iCSCTrk][iCsc];
	  which_lct_station2 = iCsc;
        }
		
	if (evt.trLctStation[iCSCTrk][iCsc] == 3) {
          eta3 = evt.trLctglobalEta[iCSCTrk][iCsc];
          phi3 = evt.trLctglobalPhi[iCSCTrk][iCsc];
        }
	
      } // end loop over csc lcts

      // set bend angles
      dphi12 = phi1 - phi2;
      dphi23 = phi2 - phi3;
      deta12 = eta1 - eta2;
      deta23 = eta2 - eta3;

      if (printLevel>1) {
	cout << "==== CSC Bend Angles ====\n";
	cout << "dphi12 = " << dphi12 << endl;
	cout << "dphi23 = " << dphi23 << endl;
	cout << "deta12 = " << deta12 << endl;
	cout << "deta23 = " << deta23 << endl;
      }

      // Fill hists for all csc bend angles. Used to examine structure
      hdphi12_all -> Fill(dphi12, evt.PtTrk[iCSCTrk]);
      hdphi23_all -> Fill(dphi23, evt.PtTrk[iCSCTrk]);


      // Loop over rpc hits and plot internal station bends
      // Now does an rpc hit exist in station two within same bend angles?
      bool isMatch = false;
      float dphi12_rpc, dphi23_rpc;
      float deta12_rpc, deta23_rpc;
      
      for (int iRpc=0; iRpc < evt.rpc_NumLctsTrk; iRpc++) {
	
	if (printLevel > 1) cout << "\nLooping over RPC LCT # " << iRpc << endl;
	
	// look at one station at a time
	if (evt.rpc_Station[iRpc] == 1) {
	  
	  // find bend between csc and rpc hit.  Fill hist
	  hdphi1 -> Fill(evt.rpc_gblPhi[iRpc] - phi1);
	  
	  hdphi1_pt -> Fill(evt.rpc_gblPhi[iRpc] - phi1, pt_muon);
	
	  hdphi11_csc_phi -> Fill(evt.rpc_gblPhi[iRpc] - phi1, phi1);	  

	} // end station 1
	
	
	// select station two rpc hits
	if (evt.rpc_Station[iRpc] == 2) {
	  
	  // find bend between csc and rpc hit.  Fill hist
	  hdphi2 -> Fill(evt.rpc_gblPhi[iRpc] - phi2);
	  
	  hdphi22_csc_phi -> Fill(evt.rpc_gblPhi[iRpc] - phi2, phi2);
	  
	  // hdphi2_pt -> Fill(evt.rpc_gblPhi[iRpc] - phi2, evt.gen_pt);
	  
	  // is a pt value stored in tuple?
	  if (evt.csc_rpc_match[iCSCTrk][which_lct_station2][iRpc] != 1) continue;
	    
	  // find bend angles
	  dphi12_rpc = evt.rpc_gblPhi[iRpc] - phi1;
	  dphi23_rpc = evt.rpc_gblPhi[iRpc] - phi3;
	  
	  deta12_rpc = evt.rpc_gblEta[iRpc] - eta1;
	  deta23_rpc = evt.rpc_gblEta[iRpc] - eta3;
	  
	  if (printLevel>1){
	    cout <<"==== RPC Bend Angles ====\n";
	    cout << "dphi12_rpc = " << dphi12_rpc << endl;
	    cout << "dphi23_rpc = " << dphi23_rpc << endl;
	    cout << "deta12_rpc = " << deta12_rpc << endl;
	    cout << "deta23_rpc = " << deta23_rpc << endl;
	  }
	  
	  
	  // are they within csc windows?
	  if ( (abs(deta12_rpc) <= abs(deta12)) && 
	       (abs(dphi12_rpc) <= abs(dphi12)) &&    
	       (abs(deta23_rpc) <= abs(deta23)) &&
	       (abs(dphi23_rpc) <= abs(dphi23)) ) {
	    
	    isMatch = true;
	    
	    // fill plots
	    hdeta12   -> Fill(deta12);
	    hdeta12_r -> Fill(deta12_rpc);
	    
	    hdphi12   -> Fill(dphi12);
	    hdphi12_r -> Fill(dphi12_rpc);

	    hdeta23   -> Fill(deta23);
            hdeta23_r -> Fill(deta23_rpc);

            hdphi23   -> Fill(dphi23);
	    hdphi23_r -> Fill(dphi23_rpc);

	    hdeta12_pt -> Fill(deta12, evt.PtTrk[iCSCTrk]); 	    
	    hdeta23_pt -> Fill(deta23, evt.PtTrk[iCSCTrk]);
	    hdphi12_pt -> Fill(dphi12, evt.PtTrk[iCSCTrk]);
	    hdphi23_pt -> Fill(dphi23, evt.PtTrk[iCSCTrk]);

	    hdeta12_r_pt -> Fill(deta12_rpc, evt.PtTrk_reco_rpc_csc[iCSCTrk][which_lct_station2][iRpc]);
	    hdeta23_r_pt -> Fill(deta23_rpc, evt.PtTrk_reco_rpc_csc[iCSCTrk][which_lct_station2][iRpc]);
	    hdphi12_r_pt -> Fill(dphi12_rpc, evt.PtTrk_reco_rpc_csc[iCSCTrk][which_lct_station2][iRpc]);
	    hdphi23_r_pt -> Fill(dphi23_rpc, evt.PtTrk_reco_rpc_csc[iCSCTrk][which_lct_station2][iRpc]);
	    
	  }

	  } // end station 2
	  
	  
	  // select station three rpc hits
          if (evt.rpc_Station[iRpc] == 3) {

            // find bend between csc and rpc hit.  Fill hist
	    hdphi3 -> Fill(evt.rpc_gblPhi[iRpc] - phi3);

	    hdphi33_csc_phi -> Fill(evt.rpc_gblPhi[iRpc] - phi3, phi3);
	    
	    //	    hdphi3_pt -> Fill(evt.rpc_gblPhi[iRpc] - phi3, evt.gen_pt);

	  } // end station 3


      } //End loop over rpc hits

    } //End loop over csc tracks
    
  } //End loop through events



  // ============== Report Results ================================
  
  cout << "\n============ Analysis Results =============\n" ; 




  // ============== Write Hists to File -==========================

  TFile *newfile = new TFile("RPCeffcompare_two.root","recreate");

  hdeta12   -> Write();
  hdeta12_r -> Write();
  hdeta23   -> Write();
  hdeta23_r -> Write();
  hdphi23_r -> Write();
  hdphi12   -> Write();
  hdphi23   -> Write();
  hdphi12_r -> Write();
  
  hdeta12_pt -> Write();
  hdeta23_pt -> Write();  
  hdphi12_pt -> Write();
  hdphi23_pt -> Write();
  
  hdeta12_r_pt -> Write();
  hdeta23_r_pt-> Write();
  hdphi12_r_pt-> Write();
  hdphi23_r_pt-> Write();
  
  hdphi12_all -> Write();
  hdphi23_all -> Write();

  hdphi1 -> Write();
  hdphi2 -> Write();
  hdphi3 -> Write();
  
  hdphi1_pt -> Write();
  hdphi2_pt -> Write();
  hdphi3_pt -> Write();

  hpt -> Write();
  hpt_inv -> Write();
  
  hdphi22_csc_phi -> Write();
  hdphi33_csc_phi -> Write();
  hdphi11_csc_phi -> Write();

  delete newfile;

} // end main function


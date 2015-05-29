///////////////////////
// RPCeffcompare.C
//
// Author: David Curry
//
// Studies the effects of inserting one rpc hit into a two hit csc track and finding new pT.
//
//////////////////////

#include<exception>
#include<vector>
#include<iostream>
#include<cmath>

#include<TSystem.h>
#include<TFile.h>
#include<TTree.h>
#include<TCanvas.h>
#include<TH1.h>
#include<TH2F.h>
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


void RPCeffcompare_three(int printLevel = 0){
  
  // class that wraps TTree variables and methods
  EffiEvent evt;
  
  //TFile* file = TFile::Open("data/rpc_tuple_2012C_best.root");
  //TFile* file = TFile::Open("../test/rpc_tuple_allevtTest_MC.root");
  //TFile* file = TFile::Open("../test/rpc_tuple_data_test.root");
  TFile* file = TFile::Open("/cms/data/store/user/dcurry/rpc/2012C_best3/merged_2012C.root");

  evt.AttachToFile(file);

  // ====== Define Hists ================================
  TH1F* track_pt    = new TH1F("track_pt", "Track Pt", 100, 0, 100);
  TH1F* rpc_pt      = new TH1F("rpc_pt", "Rpc Pt", 100, 0, 100);
  TH1F* rate_pt     = new TH1F("rate_pt", "Track pt Rate", 100, 0, 100);
  TH1F* rate_pt_rpc = new TH1F("rate_pt_rpc", "Rpc pt Rate", 100, 0, 100);
  TH1F* rate_reduc  = new TH1F("rate_reduc", "Rate Reduction", 100, 0, 100);
  TH1F* deltaR      = new TH1F("deltaR", "", 50, -0.3, 0.3);
  TH1F* track_modes = new TH1F("track_modes", "Track Mode Distribution", 6, 1, 7);
  TH1F* mode2_cand  = new TH1F("mode2_cand", "RPC candidates: Mode 2 Tracks", 10, 1, 11);
  TH1F* dpt         = new TH1F("dpt", "", 50, -50, 50);
  
  TH1F* phi_prec    = new TH1F("phi_prec", "CSCTF Integer Phi Conversion Precision", 100, -8100, 8100);
  TH2F* phi_bit     = new TH2F("phi_bit", "Phi Bit", 1000, 0, 3, 128, 0, 128);

  TH2F* twod_mode_pt   = new TH2F("twod_mode_pt", "Track Mode vs. Pt Difference", 6, 1, 7, 50, -100, 100);
  TH2F* twod_mode_rpc  = new TH2F("twod_mode_rpc", "RPC Mode vs. Pt Difference", 3, 1, 4, 50, -100, 100);
  TH2F* twod_mode_phi  = new TH2F("twod_mode_phi", "RPC Phi Diff vs. Pt Difference", 50, -200, 200, 50, -100, 100);
  TH2F* twod_mode_cand = new TH2F("twod_mode_cand", "Track Mode vs. RPC Candidates", 6, 1, 7, 10, 0, 9);


  // =====================================================

  // Global counters
  int num_rpc_matched    = 0;
  int num_tracks         = 0;
  int num_tracks_matched = 0;

  // mode 2 track counters
  int num_mode2_rpc2     = 0;
  int num_mode2_match13  = 0;
  int num_mode2_match1  = 0;
  int num_mode2_match3  = 0;
  
  int num_mode5_rpc3     = 0;
  int num_mode5_match24  = 0;
  int num_mode5_match2  = 0;
  int num_mode5_match4  = 0;

  // =====================================================

  //Start loop over events
  //for (int iEvt = 0; iEvt < evt.GetEntries(); iEvt++) {
  
  // for testing
  for (int iEvt = 0; iEvt < 1000000; iEvt++) {

    evt.GetEntry(iEvt);
    if ( ( iEvt % 100000) == 0 ) printf(" --- Event # %6d \n", iEvt+1);


    if (printLevel > 1) {
      cout << "\n=======================  Starting new event loop! ===============================" << endl;
      cout << "Event = " << evt.Event << endl;
      cout << "Lumi  = " << evt.Lumi  << endl; 
      cout << " # csctf tracks in event = " << evt.SizeTrk        << endl;
      cout << " # rpc lcts in event     = " << evt.rpc_NumLctsTrk << endl;
    }


    vector<int> pt_temp;
    int pt_reco_best = 0;
    
    // Loop over csctf tracks
    for (int iCSCTrk = 0; iCSCTrk < evt.SizeTrk; iCSCTrk++) {

      if (printLevel > 1) cout << "\n============ Looping over CSC track # " << iCSCTrk << endl;

      
      // Loop over csc rpc hits to get basic track info: mode, is it matched, etc.
      for (int iCsc=0; iCsc < evt.NumLctsTrk[iCSCTrk]; iCsc++) {

	//	if (evt.trLctSector[iCSCTrk][iCsc] == 3) continue;
	//if (evt.trLctSector[iCSCTrk][iCsc] == 6) continue;
	
	//convert gbl phi to phi bit
	
	
	// create eat bit plots
        phi_bit -> Fill(evt.trLctglobalEta[iCSCTrk][iCsc], evt.trLctEtaBit[iCSCTrk][iCsc]);
	
	// test to see if phi bit conversion works
	//	int phi_diff = evt.trLctPhiBit[iCSCTrk][iCsc] - phi_Bit;
	//phi_prec -> Fill(phi_diff);

      } // end loop over csc lcts
      

      num_tracks++;
      pt_temp.clear();


      
      // only analyze tracks with quality = 3
      bool quality_3 = false;
      if (evt.ModeTrk[iCSCTrk] == 2 && evt.EtaTrk[iCSCTrk] > 1.2 && evt.EtaTrk[iCSCTrk] < 2.1)     quality_3 = true;
      if (evt.ModeTrk[iCSCTrk] == 4 && evt.EtaTrk[iCSCTrk] < 2.1)                                  quality_3 = true;
      if (evt.ModeTrk[iCSCTrk] == 5)                                                               quality_3 = true;
      if (evt.ModeTrk[iCSCTrk] == 11 || evt.ModeTrk[iCSCTrk] == 12 || evt.ModeTrk[iCSCTrk] == 14 ) quality_3 = true;

      if (!quality_3) continue;

      // Is this a two hit or three hit track?
      if (evt.NumLctsTrk[iCSCTrk] != 2) continue;

      if (printLevel>1) cout << " Track has two hits\n"; 

      // =============== What mode is track? ================
      // Possible modes are station combinations:
      // 1-2  Mode 1 sum 3  
      // 1-3  Mode 2 sum 4
      // 1-4  Mode 3 sum 5
      // 2-3  Mode 4 sum 5
      // 2-4  Mode 5 sum 6
      // 3-4  Mode 6 sum 7
      
      // Set track variables
      int track_mode = 0;
      int temp_mode  = 0;
      bool isStation_1 = false;
      bool isTrackMatch = false;
      int num_rpc_cand = 0;
      
      // Loop over csc rpc hits to get basic track info: mode, is it matched, etc.
      for (int iCsc=0; iCsc < evt.NumLctsTrk[iCSCTrk]; iCsc++) {
	
	temp_mode += evt.trLctStation[iCSCTrk][iCsc];
	
	if (evt.trLctStation[iCSCTrk][iCsc] == 1) isStation_1 = true;
	
      } // end loop over csc lcts
      
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
      
      // =====================================================
      
      if (printLevel>1) cout << " Track Mode = " << track_mode << endl;
      
      // =======  Filter only certain modes, etc. =========== 
      
      if (track_mode != 2) continue;
      
      //if (num_rpc_cand != 1) continue;
      
      // =====================================================
	   

      num_tracks_matched ++;
	    
      track_modes -> Fill(track_mode);
      
      // Counters for eta/phi windows
      int num_rpc_mode2 = 0;
      int num_matched_1 = 0;
      int num_matched_2 = 0;
      int num_matched_12 = 0;
      int num_matched_24 = 0;
      int pt_rpc = 0;

      // Loop over RPC hits in event record.  Look for insertion rpc candidate
      for (int iRpc = 0; iRpc < evt.rpc_NumLctsTrk; iRpc++) {
	
	// Set rpc eta to be csctf eta bit
	int temp_eta_bit = 85.3*abs(evt.rpc_gblEta[iRpc]) - 81;
	bool isMatched_13    = false;
	
	if (printLevel>1) {
	  cout << "\nLooping over RPC # " << iRpc            << endl;
	  cout << " RPC station = " << evt.rpc_Station[iRpc] << endl;
	  cout << " RPC Gbl Eta = " << evt.rpc_gblEta[iRpc]  << endl;
	  cout << " RPC Gbl Phi = " << evt.rpc_gblPhi[iRpc]  << endl;
	  cout << " RPC Eta Bit = " << temp_eta_bit          << endl;
	  cout << " RPC Phi Bit = " << evt.rpc_Phibit[iRpc]  << endl;
	}
	
	// is rpc inserted into track?
	if (evt.rpcIsmatched[iCSCTrk][iRpc] != 1) continue;
	
	// double check dr to track, just to make sure
		float dr_temp = sqrt(abs(abs(abs(evt.rpc_gblPhi[iRpc]-evt.PhiTrk[iCSCTrk])-3.14)-3.14)* \
				     abs(abs(abs(evt.rpc_gblPhi[iRpc]-evt.PhiTrk[iCSCTrk])-3.14)-3.14)+ \
				     ((evt.rpc_gblEta[iRpc]-evt.EtaTrk[iCSCTrk])*(evt.rpc_gblEta[iRpc]-evt.EtaTrk[iCSCTrk])));
		

	// ========================= Does the rpc hit fall within eta/phi windows with the csc hit? ===============================

	// Tracks with hits in stations 1 and 3 
	if ( (track_mode == 2) && (evt.rpc_Station[iRpc] == 2) ) {

	  if (printLevel>1) cout << " Track is mode 2 and rpc is in station 2" << endl;
	  
	  num_rpc_mode2++;
	  num_mode2_rpc2++;

	  bool isMatched_eta_1 = false;
	  bool isMatched_eta_3 = false; 
	  bool isMatched_phi_1 = false;
	  bool isMatched_phi_3 = false;

	  // loop over csc hits
	  for (int iCsc=0; iCsc < evt.NumLctsTrk[iCSCTrk]; iCsc++) {
	    
	    // check csc station 1 with rpc station 2
	    if (evt.trLctStation[iCSCTrk][iCsc] == 1) {
	      
	      if ( abs(evt.trLctEtaBit[iCSCTrk][iCsc] - temp_eta_bit) <= 4 ) isMatched_eta_1 = true;    
	     
	      int eta_bit = temp_eta_bit >> 1;
	      int dphi_window = eta_dphi_ME1toME2[eta_bit];
	      if ( abs(evt.trLctPhiBit[iCSCTrk][iCsc] - evt.rpc_Phibit[iRpc]) <= dphi_window ) isMatched_phi_1 = true;
	      
	      if (printLevel>2) {
		cout << "   Checking csc station 1 with rpc" << endl;
		cout << "   CSC Station = " << evt.trLctStation[iCSCTrk][iCsc] << endl;
		cout << "   CSC Gbl Phi = " << evt.trLctglobalPhi[iCSCTrk][iCsc] << endl;
		cout << "   CSC Gbl Eta = " << evt.trLctglobalEta[iCSCTrk][iCsc] << endl;
		cout << "   CSC Phi Bit = " << evt.trLctPhiBit[iCSCTrk][iCsc] << endl;
		cout << "   CSC Eta Bit = " << evt.trLctEtaBit[iCSCTrk][iCsc] << endl;
		cout << "   dEta        = " << abs(evt.trLctEtaBit[iCSCTrk][iCsc] - temp_eta_bit) << endl;
		cout << "   is deta?    = " << isMatched_eta_1 <<endl;
		cout << "   dPhi        = " << abs(evt.trLctPhiBit[iCSCTrk][iCsc] - evt.rpc_Phibit[iRpc]) << endl;
		cout << "   is dphi?    = " << isMatched_phi_1 << endl << endl;
	      }
	      
	    }

	    // check csc station 3 with rpc station 2
            if (evt.trLctStation[iCSCTrk][iCsc] == 3) {

              if ( abs(evt.trLctEtaBit[iCSCTrk][iCsc] - temp_eta_bit) <= 6 ) isMatched_eta_3 = true;

	      if ( abs(evt.trLctPhiBit[iCSCTrk][iCsc]/4 - evt.rpc_Phibit[iRpc]/4) <= 127 ) isMatched_phi_3 = true;
	     
	      if (printLevel>2) {
		cout << "   Checking csc station 3 with rpc" << endl;
		cout << "   dEta     = " << abs(evt.trLctEtaBit[iCSCTrk][iCsc] - temp_eta_bit) << endl;
		cout << "   is deta? = " << isMatched_eta_3 << endl;
                cout << "   dPhi     = " << abs(evt.trLctPhiBit[iCSCTrk][iCsc]/4 - evt.rpc_Phibit[iRpc]/4) << endl;
		cout << "   is dphi? = " << isMatched_phi_3 << endl << endl;
	      }
            }

	  } // end loop over csc lcts
	 
	  if ( !isMatched_eta_3 || !isMatched_phi_3 ) 
	    if ( isMatched_eta_1 && isMatched_phi_1 ) {
	      num_matched_1++;
	      num_mode2_match1++; 
	    }

	  if (!isMatched_eta_1 || !isMatched_phi_1) 
	    if (isMatched_eta_3 && isMatched_phi_3) {
	      num_matched_2++;
	      num_mode2_match3++;
	    }

	  if ( isMatched_eta_1 && isMatched_phi_1 && isMatched_eta_3 && isMatched_phi_3) {

	    num_matched_12++, num_mode2_match13++, isMatched_13=true; 
	    
	    pt_rpc = evt.PtTrk_reco_best_two[iCSCTrk][iRpc];

	    dpt -> Fill(evt.PtTrk_reco_best_two[iCSCTrk][iRpc] - evt.PtTrk[iCSCTrk]);
	    //rpc_pt   -> Fill(evt.PtTrk_reco_best_two[iCSCTrk][iRpc]);
	    //track_pt -> Fill(evt.PtTrk[iCSCTrk]);
	    
	    if (printLevel>1) cout << "RPC is matched to stations 1 and 3. Pt = " << evt.PtTrk_reco_best_two[iCSCTrk][iRpc] << endl << endl; 

	  }

	  //=================================================================================================

	} // end if mode 2 

      
	
	// Now check mode 5(2-4) Tracks =====================================================================
	if ( (track_mode == 5) && (evt.rpc_Station[iRpc] == 3) ) {

          if (printLevel>1) cout << " Track is mode 5 and rpc is in station 3" << endl;

          num_mode5_rpc3++;

          bool isMatched_eta_2 = false;
          bool isMatched_eta_4 = false;
          bool isMatched_phi_2 = false;
	  bool isMatched_phi_4 = false;

	  // loop over csc hits
          for (int iCsc=0; iCsc < evt.NumLctsTrk[iCSCTrk]; iCsc++) {

	    // check csc station 2 with rpc station 3
            if (evt.trLctStation[iCSCTrk][iCsc] == 2) {

              if ( abs(evt.trLctEtaBit[iCSCTrk][iCsc] - temp_eta_bit) <= 6 ) isMatched_eta_2 = true;

	      if ( abs(evt.trLctPhiBit[iCSCTrk][iCsc]/4 - evt.rpc_Phibit[iRpc]/4) <= 127 ) isMatched_phi_2 = true;
	      
	      if (printLevel>2) {
		cout << "   Checking csc station 2 with rpc" << endl;
                cout << "   CSC Station = " << evt.trLctStation[iCSCTrk][iCsc] << endl;
                cout << "   CSC Gbl Phi = " << evt.trLctglobalPhi[iCSCTrk][iCsc] << endl;
                cout << "   CSC Gbl Eta = " << evt.trLctglobalEta[iCSCTrk][iCsc] << endl;
                cout << "   CSC Phi Bit = " << evt.trLctPhiBit[iCSCTrk][iCsc] << endl;
                cout << "   CSC Eta Bit = " << evt.trLctEtaBit[iCSCTrk][iCsc] << endl;
                cout << "   dEta        = " << abs(evt.trLctEtaBit[iCSCTrk][iCsc] - temp_eta_bit) << endl;
                cout << "   is deta?    = " << isMatched_eta_2 <<endl;
                cout << "   dPhi        = " << abs(evt.trLctPhiBit[iCSCTrk][iCsc] - evt.rpc_Phibit[iRpc]) << endl;
                cout << "   is dphi?    = " << isMatched_phi_2 << endl << endl;
              }

            }

            // check csc station 4 with rpc station 3
            if (evt.trLctStation[iCSCTrk][iCsc] == 4) {

              if ( abs(evt.trLctEtaBit[iCSCTrk][iCsc] - temp_eta_bit) <= 6 ) isMatched_eta_4 = true;

              if ( abs(evt.trLctPhiBit[iCSCTrk][iCsc]/4 - evt.rpc_Phibit[iRpc]/4) <= 127 ) isMatched_phi_4 = true;

	      if (printLevel>2) {
                cout << "   Checking csc station 4 with rpc" << endl;
                cout << "   dEta     = " << abs(evt.trLctEtaBit[iCSCTrk][iCsc] - temp_eta_bit) << endl;
                cout << "   is deta? = " << isMatched_eta_4 << endl;
                cout << "   dPhi     = " << abs(evt.trLctPhiBit[iCSCTrk][iCsc]/4 - evt.rpc_Phibit[iRpc]/4) << endl;
                cout << "   is dphi? = " << isMatched_phi_4 << endl << endl;
              }
            }

          } // end loop over csc lcts

	  if ( !isMatched_eta_4 || !isMatched_phi_4 )
            if ( isMatched_eta_2 && isMatched_phi_2 ) num_mode5_match2++;
            

          if (!isMatched_eta_2 || !isMatched_phi_2)
	    if (isMatched_eta_4 && isMatched_phi_4) num_mode5_match4++;
            

          if ( isMatched_eta_2 && isMatched_phi_2 && isMatched_eta_4 && isMatched_phi_4) {
	    
	    num_mode5_match24++; 
	    num_matched_24++;
	    
	    pt_rpc = evt.PtTrk_reco_best_two[iCSCTrk][iRpc];
	    
	    dpt -> Fill(evt.PtTrk_reco_best_two[iCSCTrk][iRpc] - evt.PtTrk[iCSCTrk]);
	  
	  }
	  
          //=================================================================================================
        } // end if mode 5

       
	
	if (printLevel>1) cout << " --->RPC inserted into Track";
	
	num_rpc_matched ++;
	
 	// Where is the rpc inserted?  Assign an rpc_mode based on station configuration.
	// =============================================================================
	int rpc_mode = 0;
	switch(evt.rpc_Station[iRpc]) {
	case 1: rpc_mode = 1;
	  break;
	case 2: rpc_mode = 2;
          break;
	case 3: rpc_mode = 3;
          break;
	case 4: rpc_mode = 4;
          break;
	}
	
	
	// =============================================================================
	
	
	// find pt difference as function of track mode
	double pt_diff = evt.PtTrk_reco_best_two[iCSCTrk][iRpc] - evt.PtTrk[iCSCTrk];
	
	// find dr between rpc hit and track
	float dr = sqrt(abs(abs(abs(evt.rpc_gblPhi[iRpc]-evt.PhiTrk[iCSCTrk])-3.14)-3.14)* \
                        abs(abs(abs(evt.rpc_gblPhi[iRpc]-evt.PhiTrk[iCSCTrk])-3.14)-3.14)+ \
			((evt.rpc_gblEta[iRpc]-evt.EtaTrk[iCSCTrk])*(evt.rpc_gblEta[iRpc]-evt.EtaTrk[iCSCTrk])));
	
	
	deltaR -> Fill(dr);

	// Put rpc track pt in temp vector
	//pt_temp.push_back(evt.PtTrk_reco_best_two[iCSCTrk][iRpc]);

  	//===============================================================================
	
	
	twod_mode_pt  -> Fill(track_mode, pt_diff);
	
	twod_mode_rpc -> Fill(rpc_mode, pt_diff);
	
	twod_mode_cand -> Fill(track_mode, num_rpc_cand);
	
	
      } // End loop over rpc hits

      // How many rpc hits were available for insertion?
      if (printLevel>1) {
	cout << "This Mode 2 track has " << num_rpc_mode2  << " rpc hits in station 2" << endl;
	cout << "This Mode 2 track has " << num_matched_12 << " rpc matched to hits in stations 1 and 3" << endl;
	cout << "                      " << num_matched_1  << " rpc matched to hits in stations 1" << endl;
	cout << "                      " << num_matched_2  << " rpc matched to hits in stations 3" << endl;
      }

      
      // If at least one rpc hit was matched to both csc hits(modes 2 and 5), fill plots
      if ( (num_matched_12 > 0) || (num_matched_24 > 0)) {
	
	 // Fill histogram with largest rpc pt
	 rpc_pt   -> Fill(pt_rpc);
	 track_pt -> Fill(evt.PtTrk[iCSCTrk]);
      }      

      // if no rpc insertion then fill with old track pt values
      else {
	track_pt -> Fill(evt.PtTrk[iCSCTrk]);
        rpc_pt   -> Fill(evt.PtTrk[iCSCTrk]);
      }

    } //End loop over csc tracks
        
  } //End loop through events

  
  // Report Results ========================
  cout << "===============  Analysis Results  ==============\n ";
  cout << " Number of two hit tracks = " << num_tracks << endl;
  //cout << " Number of times a two hit track with rpc hit is able to be inserted = " << num_tracks_matched << endl;
 
  cout << " Mode 2 Tracks:" << num_tracks_matched << endl;
  cout << "  Number of station 2 RPC hits = " << num_mode2_rpc2 << endl;
  cout << "  Number of station 2 RPC hits matched to CSC station 1 only = " << num_mode2_match1  << endl;
  cout << "  Number of station 2 RPC hits matched to CSC station 3 only = " << num_mode2_match3  << endl;
  cout << "  Number of station 2 RPC hits matched to CSC station 1 & 3  = " << num_mode2_match13 << endl;

  cout << " Mode 5 Tracks:" << endl;
  cout << "  Number of station 3 RPC hits = " << num_mode5_rpc3 << endl;
  cout << "  Number of station 3 RPC hits matched to CSC station 2 only = " << num_mode5_match2  << endl;
  cout << "  Number of station 3 RPC hits matched to CSC station 4 only = " << num_mode5_match4  << endl;
  cout << "  Number of station 3 RPC hits matched to CSC station 2 & 4  = " << num_mode5_match24 << endl;


    
  // ========== Make Rate Reduction Plots ============================================
  // Loop over bins in pt histogram.  For each bin value get integral from bin upwards.
  
  // Set integral norms 
  double norm_track = track_pt -> Integral(-1, 100); 
  double norm_rpc   = rpc_pt   -> Integral(-1, 100);
  int num_entries_track = track_pt -> GetEntries ();
  
    cout << "track entries = " << num_entries_track << endl;
    cout << "track integral = " << norm_track << endl;
    cout << "rpc integral = " << norm_rpc << endl;
  
  // loop over pt histogram bins
  for (int i=0; i<100; i++) {
    
    double integral     = track_pt -> Integral(i, 100);
    double integral_rpc = rpc_pt   -> Integral(i, 100);
    
    rate_pt     -> SetBinContent(i, integral/norm_track);
    rate_pt_rpc -> SetBinContent(i, integral_rpc/norm_rpc);
    
    // calcualte rate reduction bin contents
    double rate_reduction = integral/integral_rpc;
    
    if ( rate_reduction > 0 && rate_reduction < 100) 
      rate_reduc -> SetBinContent(i, rate_reduction);
    
  }// end bin loop


      
  // Draw the plots
  TCanvas* pt_rate = new TCanvas("pt_rate", "Rate", 420, 500, 600, 600);
  rate_pt -> GetXaxis() -> SetTitle(" pT Threshold(GeV) ");
  rate_pt -> GetYaxis() -> SetTitle(" normalized # above Threshold ");
  rate_pt -> GetYaxis() -> SetTitleOffset(1.5);
  rate_pt -> Draw();
  rate_pt -> SetStats(0);
  
  //TCanvas* pt_rate_rpc = new TCanvas("pt_rate_rpc", "Rate", 420, 500, 600, 600);
  rate_pt_rpc -> GetXaxis() -> SetTitle(" pT Threshold(GeV) ");
  rate_pt_rpc -> GetYaxis() -> SetTitle(" normalized # above Threshold ");
  rate_pt_rpc -> GetYaxis() -> SetTitleOffset(1.5);
  rate_pt_rpc -> SetLineColor(kRed);
  rate_pt_rpc -> Draw("same");
  rate_pt_rpc -> SetStats(0);

  
  TCanvas* pt_track = new TCanvas("pt_track", "", 420, 500, 600, 600);
  track_pt -> GetXaxis() -> SetTitle("Track Pt");
  track_pt -> GetYaxis() -> SetTitle("Count");
  track_pt -> Draw();
  //track_pt -> SetStats(0);
  
  TCanvas* pt_rpc = new TCanvas("pt_rpc", "", 420, 500, 600, 600);
  rpc_pt -> GetXaxis() -> SetTitle("RPC Pt");
  rpc_pt -> GetYaxis() -> SetTitle("Count");
  rpc_pt -> Draw();
  //rpc_pt -> SetStats(0);

  
  TCanvas* reduc = new TCanvas("reduc", "", 420, 500, 600, 600);
  rate_reduc -> GetXaxis() -> SetTitle("pT Threshold(GeV)");
  rate_reduc -> GetYaxis() -> SetTitle("Fraction");
  rate_reduc -> GetYaxis() -> SetTitleOffset(1.5);
  rate_reduc -> Draw();
  rate_reduc -> SetStats(0);

    
  TCanvas* delta = new TCanvas("delta", "", 420, 500, 600, 600);
  deltaR -> GetXaxis() -> SetTitle("Delta R");
  deltaR -> GetYaxis() -> SetTitle("count");
  deltaR -> Draw();
  //track_modes -> SetStats(0);

  
  TCanvas* etabit = new TCanvas("etabit", "", 420, 500, 600, 600);
  phi_bit -> GetXaxis() -> SetTitle("GBL Phi");
  phi_bit -> GetXaxis() -> SetTitleOffset(1.5);
  phi_bit -> GetYaxis() -> SetTitle("Bit Phi");
  phi_bit -> GetYaxis() -> SetTitleOffset(1.5);
  phi_bit -> Draw();
  phi_bit -> SetStats(0);
  

  /*    
  TCanvas* modes = new TCanvas("modes", "modes", 420, 500, 600, 600);
  track_modes -> GetXaxis() -> SetTitle("track mode");
  track_modes -> GetYaxis() -> SetTitle("");
  track_modes -> Draw();
  //track_modes -> SetStats(0);
  
  
  TCanvas* twod_modes = new TCanvas("twod_modes", "twod_modes", 420, 500, 600, 600);
  twod_mode_pt -> GetXaxis() -> SetTitle("track mode");
  twod_mode_pt -> GetXaxis() -> SetTitleOffset(1.5);
  twod_mode_pt -> GetYaxis() -> SetTitle("Pt Difference");
  twod_mode_pt -> GetYaxis() -> SetTitleOffset(1.5);
  twod_mode_pt -> Draw("LEGOFB");
  //twod_mode_pt -> SetStats(0);

  TCanvas* twod_modes2 = new TCanvas("twod_modes2", "", 420, 500, 600, 600);
  twod_mode_rpc -> GetXaxis() -> SetTitle("rpc mode");
  twod_mode_rpc -> GetXaxis() -> SetTitleOffset(1.5);
  twod_mode_rpc -> GetYaxis() -> SetTitle("Pt Difference");
  twod_mode_rpc -> GetYaxis() -> SetTitleOffset(1.5);
  twod_mode_rpc -> Draw("LEGOFB");
  twod_mode_rpc -> SetStats(0);

  TCanvas* twod_modes3 = new TCanvas("twod_modes3", "", 420, 500, 600, 600);
  twod_mode_phi -> GetXaxis() -> SetTitle("rpc Phi Diff");
  twod_mode_phi -> GetXaxis() -> SetTitleOffset(1.5);
  twod_mode_phi -> GetYaxis() -> SetTitle("Pt Difference");
  twod_mode_phi -> GetYaxis() -> SetTitleOffset(1.5);
  twod_mode_phi -> Draw("LEGOFB");
  twod_mode_phi -> SetStats(0);

  TCanvas* twod_modes4 = new TCanvas("twod_modes4", "", 420, 500, 600, 600);
  twod_mode_cand -> GetXaxis() -> SetTitle("track mode");
  twod_mode_cand -> GetXaxis() -> SetTitleOffset(1.5);
  twod_mode_cand -> GetYaxis() -> SetTitle("# RPC candidates");
  twod_mode_cand -> GetYaxis() -> SetTitleOffset(1.5);
  twod_mode_cand -> Draw("LEGOFB");
  twod_mode_cand -> SetStats(0);
  

  
  TCanvas* etabit = new TCanvas("etabit", "", 420, 500, 600, 600);
  phi_bit -> GetXaxis() -> SetTitle("GBL Phi");
  phi_bit -> GetXaxis() -> SetTitleOffset(1.5);
  phi_bit -> GetYaxis() -> SetTitle("Bit Phi");
  phi_bit -> GetYaxis() -> SetTitleOffset(1.5);
  phi_bit -> Draw();
  phi_bit -> SetStats(0);
  

  
  TCanvas* phi1 = new TCanvas("phi1", "", 420, 500, 600, 600);
  phi_prec -> GetXaxis() -> SetTitle("Int Phi Difference(Known - Reconstructed)");
  phi_prec -> GetXaxis() -> SetTitleOffset(1.5);
  phi_prec -> GetYaxis() -> SetTitle("Count");
  phi_prec -> GetYaxis() -> SetTitleOffset(1.5);
  phi_prec -> Draw();
  //  phi_prec -> SetStats(0);
  
  */
  TCanvas* c1 = new TCanvas("c1", "", 420, 500, 600, 600);
  dpt -> GetXaxis() -> SetTitle("RPC Pt - CSCTF Pt");
  dpt -> GetXaxis() -> SetTitleOffset(1.5);
  dpt -> GetYaxis() -> SetTitle("Count");
  dpt -> GetYaxis() -> SetTitleOffset(1.5);
  dpt -> Draw();
  dpt -> SetStats(0);

  // =========== End Plots ============================================================

} //End program

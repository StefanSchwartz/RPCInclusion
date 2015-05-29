///////////////////////
// twoH_RPC_pt_all.C
//
// Author: David Curry
//
// Studies the effects of inserting one rpc hit into a three hit csc track and finding new pT.
//
// Can handle all two hit csctf tracks
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
#include<TGraphAsymmErrors.h>

using namespace std;

#include "include/MapCounter.h"
#include "include/EffiEvent.h"
#include "src/EffiEvent.cc"
//#inlcude "../plugins/PtAddress.h"



void all_twoHit_RPC_pt(int printLevel = 0){


  EffiEvent evt;

  //  TFile* file = TFile::Open("/exports/uftrig01a/dcurry/data/rpc/2012D_raw_reco_6_19_merge_small.root");

  TFile* file = TFile::Open("/exports/uftrig01a/dcurry/data/rpc/2012D_raw_reco_7_5_merge_charge.root");

  evt.AttachToFile(file);

  // Output File
  TFile *newfile = new TFile("all_twoHit_RPC_pt.root","recreate");

  bool isMC = false;

  double Pi  = acos(-1.);

  //========= define Hists ===================================

  const float rate[] = {0., 3., 5., 7., 10., 12., 16., 20., 30.};
  const int N_rate = (sizeof(rate)/sizeof(float) -1);

  TH1F * hrate_t = new TH1F("hrate_t", "CSCTF Rate Track", N_rate,  rate);
  TH1F * hrate_r = new TH1F("hrate_r", "CSCTF Rate RPC", N_rate,  rate);
  
  TH1F * hrate_t_all = new TH1F("hrate_t_all", "", N_rate,  rate);
  TH1F * hrate_r_all = new TH1F("hrate_r_all", "", N_rate,  rate);

  TH1F * hrate_t_mode2 = new TH1F("hrate_t_mode2", "", N_rate,  rate);
  TH1F * hrate_t_mode4 = new TH1F("hrate_t_mode4", "", N_rate,  rate);
  TH1F * hrate_t_mode1 = new TH1F("hrate_t_mode1", "", N_rate,  rate);
  TH1F * hrate_t_mode5 = new TH1F("hrate_t_mode5", "", N_rate,  rate);

  TH1F * hrate_r_mode1 = new TH1F("hrate_r_mode1", "", N_rate,  rate);
  TH1F * hrate_r_mode2 = new TH1F("hrate_r_mode2", "", N_rate,  rate);
  TH1F * hrate_r_mode4 = new TH1F("hrate_r_mode4", "", N_rate,  rate);
  TH1F * hrate_r_mode5 = new TH1F("hrate_r_mode5", "", N_rate,  rate);


  TH1F* hres_t = new TH1F("hres_t", "", 100, -10, 10);
  TH1F* hres_r = new TH1F("hres_r", "", 100, -10, 10);

  TH1F* hres_t_all = new TH1F("hres_t_all", "", 100, -10, 10);
  TH1F* hres_r_all = new TH1F("hres_r_all", "", 100, -10, 10);


  // Turn On Efficiencies
  const float pt_thresh[] = {7., 10., 12., 16.};
  const int N_thresh = sizeof(pt_thresh)/sizeof(float);
  
  const float PTbin[] = {2.0, 2.5, 3.0, 3.5, 4.0, 4.5, 5.0, 6.0, 7.0, 8.0,
			 9.0, 10.0, 14.0, 20.0, 50., 140.0};
  const int NPTbin = (sizeof(PTbin)/sizeof(float)-1);
  
  TH1F* csctfPt[N_thresh];
  TH1F* csctfPt_rpc[N_thresh];
  
  TH1F* csctfPt_all = new TH1F("csctfPt_all", "", NPTbin,  PTbin);

  for(int ihist = 0; ihist < N_thresh; ihist++) {
    csctfPt[ihist]     = new TH1F(Form("csctfPt_%f", pt_thresh[ihist]), "", NPTbin, PTbin);
    csctfPt_rpc[ihist] = new TH1F(Form("csctfPt_rpc_%f", pt_thresh[ihist]), "", NPTbin, PTbin);
  }

  // ========================================================
  // Global Counters
  int num_trk;
  int num_trk_two;
  int num_trk_two_rpc;
  int num_chg_not_match;
  int num_chg_match;
  
  // ========================================================

  //Start loop over events
  //for (int iEvt = 0; iEvt < evt.GetEntries(); iEvt++) {

  // for testing
  for (int iEvt = 0; iEvt < 200000; iEvt++) {

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
      if ( abs(evt.muon_eta[iReco]) < 0.9 || abs(evt.muon_eta[iReco]) > 2.4 )  continue;

      if (printLevel > 0) cout << "Looping over muon # " << iReco << endl;
      
      
      // Objects needed for study
      vector<double> best_dr; // fill with all dr values to find best track-muon match
      vector<float> pt_vec;
      float best_rpc = 999;
      bool is_rpc_inserted = false;
      int track_mode = 0;
      bool isMatched = false;

      // empty the vector for each muon
      best_dr.clear();

      // Loop over csc tracks
      for (int iCSCTrk = 0; iCSCTrk < evt.SizeTrk; iCSCTrk++) {

	pt_vec.clear();
	
	// find which track, if any, is matched to muon.
        double deta = evt.EtaTrk[iCSCTrk] - evt.muon_eta[iReco];
	double dr = sqrt( deta*deta );
	
        if (dr < 0.2) {
          
	  if (printLevel > 0) {
	    cout << "Muon # " << iReco << " is matched to Track # " << iCSCTrk << endl;
	    cout << "Delta R  = " << dr << endl;
	  }
	  
	  isMatched = true;
          best_dr.push_back(dr);
        }

      } // end loop over tracks

      int iCSCTrk = -999; // set to be the best macthed track #
      
      // skip muons who are not matched to a track
      if (!isMatched) continue;
      
      num_trk += evt.SizeTrk;
      
      // returns which track # is matched to current muon
      auto const it = lower_bound(best_dr.begin(), best_dr.end(), 0);

      if (printLevel > 1) cout << "Best delta R match is Track # " << it - best_dr.begin() << endl;
      
      iCSCTrk = it - best_dr.begin();


      //      if (evt.muon_pt[iReco] < 10) continue;
      // Debug Charge
      //      cout << "Track Chg = " << evt.ChgTrk[iCSCTrk] << endl;      
      //cout << "Muon Chg  = " << evt.muon_chg[iReco]
      if (evt.muon_chg[iReco] != evt.ChgTrk[iCSCTrk]) num_chg_not_match++;
      else num_chg_match++;
      

      // =====================================================
      // Quality 3 Filter
      
      bool quality_3 = false;
      if (evt.ModeTrk[iCSCTrk] == 2 && evt.EtaTrk[iCSCTrk] > 1.2 && evt.EtaTrk[iCSCTrk] < 2.1)     quality_3 = true;
      if (evt.ModeTrk[iCSCTrk] == 4 && evt.EtaTrk[iCSCTrk] < 2.1)                                  quality_3 = true;
      if (evt.ModeTrk[iCSCTrk] == 5)                                                               quality_3 = true;
      if (evt.ModeTrk[iCSCTrk] == 11 || evt.ModeTrk[iCSCTrk] == 12 || evt.ModeTrk[iCSCTrk] == 14 ) quality_3 = true;
      
      if (printLevel>0) cout << "Matched Track is quality 3?  = " << quality_3 << endl; 

      // ==================================================== 

      // if ( abs(evt.muon_eta[iReco]) < 1.0) cout << " High Eta Pt = " << evt.muon_pt[iReco] << endl;
      
      
      // two hit tracks only
      //if (evt.NumLctsTrk[iCSCTrk] != 2) continue;
      if (evt.NumLctsTrk[iCSCTrk] == 2) {
	
	num_trk_two++;

	// =============== What mode is track? ================
	// Possible modes are station combinations:
	// 1-2  Mode 1 sum 3
	// 1-3  Mode 2 sum 4
	// 1-4  Mode 3 sum 5
	// 2-3  Mode 4 sum 5
	// 2-4  Mode 5 sum 6
	// 3-4  Mode 6 sum 7
	
	
	int temp_mode  = 0;
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

	if (track_mode != 2) continue;
	
	if (printLevel>0) cout << " This track is mode  = " << track_mode << endl << endl;

	
	// ===========================================================================================
	// Different methods to handle the different csctf track modes.
	float phi1; float phi2;
	float eta1; float eta2; float eta3; float eta4;
	float dphi12   = 999;
	float deta13   = 999;
	
	// Fit parameters
	float deta_cut_1 = 0.1;
	float deta_cut_2 = 0.1;
	float dphi_cut_1  = 0.018;
	float dphi_cut_2  = 0.015;
	// ================


	// ===========================================================================================
	// Mode 2 Tracks(1-3)
	
	if (track_mode == 2) {
	  
	  // loop over csc Lcts
	  for (int iCsc=0; iCsc < evt.NumLctsTrk[iCSCTrk]; iCsc++) {
	    
	    if (printLevel>0) {
	      cout << "Looping over CSC Lct # " << iCsc << " in station # " << evt.trLctStation[iCSCTrk][iCsc] << endl;
	      cout << "Gbl Phi = " << evt.trLctglobalPhi[iCSCTrk][iCsc] << endl;
	      cout << "Gbl Eta = " << evt.trLctglobalEta[iCSCTrk][iCsc] << endl;
	    }
	    
	    if (evt.trLctStation[iCSCTrk][iCsc] == 1) {
	      phi1 = evt.trLctglobalPhi[iCSCTrk][iCsc];
	      eta1 = evt.trLctglobalEta[iCSCTrk][iCsc];
	    }
	    
	    if (evt.trLctStation[iCSCTrk][iCsc] == 3) eta3 = evt.trLctglobalEta[iCSCTrk][iCsc];
	    
	  } // end loop over CSC Lcts
	  
	  deta13 = abs(eta1 - eta3);
	  
	  // Now loop over rpc hits.  For mode 2 look for station 2 rpcs
	  for (int iRpc = 0; iRpc < evt.rpc_NumLctsTrk; iRpc++) {
	    
	    if (printLevel>0) cout << "   Looping over RPC hit # " << iRpc << endl;
	    
	    if (evt.rpc_Station[iRpc] != 2) continue;
	    
	    if (printLevel>0) cout << "   RPC is in Station 2\n";
	    
	    // dont look at rpc hits that are not in 0.9-1.6 eta
	    if ( (abs(evt.rpc_gblEta[iRpc]) < 0.9) || (abs(evt.rpc_gblEta[iRpc]) > 1.6) ) continue;
	    
	    dphi12 = abs( abs( abs(phi1 - evt.rpc_gblPhi[iRpc]) -Pi) -Pi);
	    
	    if (printLevel>0) {
	      cout << "   dPhi12  = " << dphi12 << endl;
	      cout << "   CSC Phi1 = " << phi1 << endl;
	      cout << "   RPC Phi2 = " << evt.rpc_gblPhi[iRpc] << endl; 
	    }
	    
	    // ===============================================
	    // For rpc hits in stations 2 find pt when dphi < 0.1.  The InvPt profile has been fitted for predictive behavior.
	    // Fit = [0]*x + [1], where [0] = 1.44178 and [1] = 1.20259e-01, and x = dphi
	    float rpc_pt = 999;
	    
	    if (dphi12 == 999) continue;
	    
	    if ( abs(dphi12) < 0.5) {
	      
	      //rpc_pt = 1/(1.44178 * dphi12 + 1.20259e-01);
	      rpc_pt = evt.PtTrk_reco_best_two[iCSCTrk][iRpc];

	      is_rpc_inserted = true;
	      
	      //if ( deta13 < 0.01 && abs(dphi12) <= 0.018) rpc_pt = 15.;
	      
	      //	      if ( deta13 < 0.01 && abs(dphi12) <= 0.015) rpc_pt = 21.;
	      
	      pt_vec.push_back(rpc_pt);
	      
	      if (printLevel>0) cout << "   Dphi12 is < 0.5.  RPC Pt = " << rpc_pt << endl;   
	    }
	    
	  } // end loop over rpcs
	  
	}
	// End mode 2
	// ===========================================================================================
	
	
	// ===========================================================================================
	// Start Mode 4(2-3)
	
	
	if (track_mode == 4) {
	  
	  // loop over csc Lcts
	  for (int iCsc=0; iCsc < evt.NumLctsTrk[iCSCTrk]; iCsc++) {
	    
	    if (printLevel>0) {
	      cout << "Looping over CSC Lct # " << iCsc << " in station # " << evt.trLctStation[iCSCTrk][iCsc] << endl;
	      cout << "Gbl Phi = " << evt.trLctglobalPhi[iCSCTrk][iCsc] << endl;
	      cout << "Gbl Eta = " << evt.trLctglobalEta[iCSCTrk][iCsc] << endl;
	    }
	    
	    if (evt.trLctStation[iCSCTrk][iCsc] == 2) phi2 = evt.trLctglobalPhi[iCSCTrk][iCsc];
	    
	    if (evt.trLctStation[iCSCTrk][iCsc] == 3) eta3 = evt.trLctglobalEta[iCSCTrk][iCsc];
	    
	  } // end loop over CSC Lcts
	  
	  deta13 = abs(eta1 - eta3);
	  
	  // Now loop over rpc hits.  For mode 4 look for station 1 rpcs
	  for (int iRpc = 0; iRpc < evt.rpc_NumLctsTrk; iRpc++) {
	    
	    if (printLevel>0) cout << "   Looping over RPC hit # " << iRpc << endl;
	    
	    if (evt.rpc_Station[iRpc] != 1) continue;
	    
	    if (printLevel>0) cout << "   RPC is in Station 1\n";
	    
	    // dont look at rpc hits that are not in 0.9-1.6 eta
	    if ( (abs(evt.rpc_gblEta[iRpc]) < 0.9) || (abs(evt.rpc_gblEta[iRpc]) > 1.6) ) continue;
	    
	    dphi12 = abs( abs( abs(phi2 - evt.rpc_gblPhi[iRpc]) -Pi) -Pi);
	    
	    deta13 = abs( eta3 - evt.rpc_gblEta[iRpc]);
	    
	    if (printLevel>0) {
	      cout << "   dPhi12  = " << dphi12 << endl;
	      cout << "   CSC Phi2 = " << phi1 << endl;
	      cout << "   RPC Phi1 = " << evt.rpc_gblPhi[iRpc] << endl; 
	    }
	    
	    // ===============================================
	    // For rpc hits in stations 2 find pt when dphi < 0.1.  The InvPt profile has been fitted for predictive behavior.
	    // Fit = [0]*x + [1], where [0] = 1.44178 and [1] = 1.20259e-01, and x = dphi
	    float rpc_pt = 999;
	    
	    if (dphi12 == 999) continue;
	    
	    if ( dphi12 < 0.5) {
	      
	      rpc_pt = 1/(1.44178 * dphi12 + 1.20259e-01);
	      
	      is_rpc_inserted = true;
	      
	      if ( deta13 < 0.1 && abs(dphi12) <= 0.020) rpc_pt = 15.;
	      
	      if ( deta13 < 0.05 && abs(dphi12) <= 0.020) rpc_pt = 21.;
	      
	      pt_vec.push_back(rpc_pt);
	      
	      if (printLevel>0) cout << "   Dphi12 is < 0.5.  RPC Pt = " << rpc_pt << endl;   
	    }
	    
	  } // end loop over rpcs
	  
	}
	
	// End mode 4
	// ===========================================================================================
	
	
	
	// ===========================================================================================
	// Start Mode 1(1-2)
	
	if (track_mode == 1) {
	  
	  // loop over csc Lcts
	  for (int iCsc=0; iCsc < evt.NumLctsTrk[iCSCTrk]; iCsc++) {
	    
	    if (printLevel>0) {
	      cout << "Looping over CSC Lct # " << iCsc << " in station # " << evt.trLctStation[iCSCTrk][iCsc] << endl;
	      cout << "Gbl Phi = " << evt.trLctglobalPhi[iCSCTrk][iCsc] << endl;
	      cout << "Gbl Eta = " << evt.trLctglobalEta[iCSCTrk][iCsc] << endl;
	    }
	    
	    if (evt.trLctStation[iCSCTrk][iCsc] == 1) {
	      phi1 = evt.trLctglobalPhi[iCSCTrk][iCsc];
	      eta1 = evt.trLctglobalEta[iCSCTrk][iCsc];
	    }
	    
	    if (evt.trLctStation[iCSCTrk][iCsc] == 2) phi2 = evt.trLctglobalEta[iCSCTrk][iCsc];
	    
	  } // end loop over CSC Lcts
	  
	  dphi12 = abs( abs( abs(phi2 - phi1) -Pi) -Pi);	
	  
	  // Now loop over rpc hits.  For mode 1 look for station 3 rpcs
	  for (int iRpc = 0; iRpc < evt.rpc_NumLctsTrk; iRpc++) {
	    
	    if (printLevel>0) cout << "   Looping over RPC hit # " << iRpc << endl;
	    
	    if (evt.rpc_Station[iRpc] != 3) continue;
	    
	    if (printLevel>0) cout << "   RPC is in Station 3\n";
	    
	    // dont look at rpc hits that are not in 0.9-1.6 eta
	    if ( (abs(evt.rpc_gblEta[iRpc]) < 0.9) || (abs(evt.rpc_gblEta[iRpc]) > 1.6) ) continue;
	    
	    deta13 = abs( eta1 - evt.rpc_gblEta[iRpc]);
	    
	    // ===============================================
	    // For rpc hits in stations 2 find pt when dphi < 0.1.  The InvPt profile has been fitted for predictive behavior.
	    // Fit = [0]*x + [1], where [0] = 1.44178 and [1] = 1.20259e-01, and x = dphi
	    float rpc_pt = 999;
	    
	    if (dphi12 == 999) continue;
	    
	    if ( dphi12 < 0.5) {
	      
	      rpc_pt = 1/(1.44178 * dphi12 + 1.20259e-01);
	      
	      is_rpc_inserted = true;
	      
	      if ( deta13 < 2.01 && abs(dphi12) <= 0.020) rpc_pt = 15.;
	      
	      if ( deta13 < 1.01 && abs(dphi12) <= 0.018) rpc_pt = 21.;
	      
	      pt_vec.push_back(rpc_pt);
	      
	      if (printLevel>0) cout << " Deta13 = " << deta13 << ". RPC Pt = " << rpc_pt << endl;   
	    }
	    
	  } // end loop over rpcs
	  
	}
	
	// End mode 1
	// ===========================================================================================
	
	
        // ===========================================================================================
	// Mode 5 Tracks(2-4)

        if (track_mode == 5) {

          // loop over csc Lcts
          for (int iCsc=0; iCsc < evt.NumLctsTrk[iCSCTrk]; iCsc++) {

            if (printLevel>0) {
              cout << "Looping over CSC Lct # " << iCsc << " in station # " << evt.trLctStation[iCSCTrk][iCsc] << endl;
              cout << "Gbl Phi = " << evt.trLctglobalPhi[iCSCTrk][iCsc] << endl;
              cout << "Gbl Eta = " << evt.trLctglobalEta[iCSCTrk][iCsc] << endl;
            }

            if (evt.trLctStation[iCSCTrk][iCsc] == 2) {
              phi2 = evt.trLctglobalPhi[iCSCTrk][iCsc];
              eta2 = evt.trLctglobalEta[iCSCTrk][iCsc];
            }

            if (evt.trLctStation[iCSCTrk][iCsc] == 4) eta4 = evt.trLctglobalEta[iCSCTrk][iCsc];

          } // end loop over CSC Lcts
	  
	  deta13 = abs(eta2 - eta4);
	  
	  // Now loop over rpc hits.  For mode 2 look for station 2 rpcs
          for (int iRpc = 0; iRpc < evt.rpc_NumLctsTrk; iRpc++) {
	    
            if (printLevel>0) cout << "   Looping over RPC hit # " << iRpc << endl;
	    
            if (evt.rpc_Station[iRpc] != 3) continue;
	    
            if (printLevel>0) cout << "   RPC is in Station 3\n";
	    
            // dont look at rpc hits that are not in 0.9-1.6 eta
            if ( (abs(evt.rpc_gblEta[iRpc]) < 0.9) || (abs(evt.rpc_gblEta[iRpc]) > 1.6) ) continue;

            dphi12 = abs( abs( abs(phi2 - evt.rpc_gblPhi[iRpc]) -Pi) -Pi);

	    if (printLevel>0) {
              cout << "   dPhi23  = " << dphi12 << endl;
              cout << "   CSC Phi2 = " << phi2 << endl;
              cout << "   RPC Phi3 = " << evt.rpc_gblPhi[iRpc] << endl;
            }

	    
            // ===============================================
            // For rpc hits in stations 2 find pt when dphi < 0.1.  The InvPt profile has been fitted for predictive behavior.
            // Fit = [0]*x + [1], where [0] = 1.44178 and [1] = 1.20259e-01, and x = dphi
            float rpc_pt = 999;

            if (dphi12 == 999) continue;

            if ( abs(dphi12) < 0.5) {

              rpc_pt = 1/(1.44178 * dphi12 + 1.20259e-01);

              is_rpc_inserted = true;

              if ( deta13 < 0.01 && abs(dphi12) <= 0.018) rpc_pt = 15.;

              if ( deta13 < 0.01 && abs(dphi12) <= 0.015) rpc_pt = 21.;

              pt_vec.push_back(rpc_pt);

              if (printLevel>0) cout << "   Dphi12 is < 0.5.  RPC Pt = " << rpc_pt << endl;
            }

          } // end loop over rpcs

        }
        // End mode 5
        // ===========================================================================================


      } // end if 2 Lcts


      
      
      // ============================================================================================
      // Now find highest/lowest pt from pt_vec
      if (is_rpc_inserted) {
	
	num_trk_two_rpc++;

	quality_3 = true;

	sort(pt_vec.begin(), pt_vec.end());
	
	best_rpc = pt_vec.back();
	
	if (printLevel>0) cout << "Highest rpc pt value =  " << best_rpc << endl;
      }
	
      if (printLevel>0) {
	cout << "Track pt = " << evt.PtTrk[iCSCTrk] << endl; 
	cout << "RPC Pt   = " << best_rpc << endl;
      }
      
      

      if (!quality_3) continue;


      // ============================================================================================
      // Fill Hists for all tracks to check overall rate
      for (int bin = N_rate -1 ; bin >=0 ; bin-- ) {
	
	if (evt.PtTrk[iCSCTrk] > rate[bin] ) hrate_t_all -> Fill(rate[bin]);
	
	if (is_rpc_inserted  && best_rpc > rate[bin])            hrate_r_all -> Fill(rate[bin]);
	if (!is_rpc_inserted && evt.PtTrk[iCSCTrk] > rate[bin] ) hrate_r_all -> Fill(rate[bin]);
      }
      
      // Fill resolution histograms
      hres_t_all -> Fill( (evt.PtTrk[iCSCTrk] - evt.muon_pt[iReco])/evt.muon_pt[iReco] );

      if (is_rpc_inserted) hres_r_all -> Fill( (best_rpc - evt.muon_pt[iReco])/evt.muon_pt[iReco] );      
      else hres_r_all                 -> Fill( (evt.PtTrk[iCSCTrk] - evt.muon_pt[iReco])/evt.muon_pt[iReco] );
     
      // Fill Cut Off Histograms
      csctfPt_all -> Fill(evt.muon_pt[iReco]);
      
      for (int ihist = 0; ihist < N_thresh; ihist++) {
	
	if (evt.PtTrk[iCSCTrk] >= pt_thresh[ihist])
	  csctfPt[ihist] -> Fill(evt.muon_pt[iReco]);
	
	if (is_rpc_inserted && best_rpc >= pt_thresh[ihist])
	  csctfPt_rpc[ihist] -> Fill(evt.muon_pt[iReco]);
	
	if (!is_rpc_inserted && evt.PtTrk[iCSCTrk] >= pt_thresh[ihist])
	  csctfPt_rpc[ihist] -> Fill(evt.muon_pt[iReco]);
      } 
      
      // ===========================================================================================
      // Now fill hists for isolated track modes
      if (!is_rpc_inserted) continue;
      
      // Calculate rate between track pt and new rpc pt
      for (int bin = N_rate-1; bin >=0; bin--) {

        if (evt.PtTrk[iCSCTrk] > rate[bin] ) hrate_t -> Fill(rate[bin]);
	
        if (best_rpc > rate[bin] ) hrate_r -> Fill(rate[bin]);
	

	if (track_mode == 1) {
          if (best_rpc > rate[bin]) hrate_r_mode1 -> Fill(rate[bin]);
          if (evt.PtTrk[iCSCTrk] > rate[bin] ) hrate_t_mode1 -> Fill(rate[bin]);
	}

	if (track_mode == 2) { 
	  if (best_rpc > rate[bin]) hrate_r_mode2 -> Fill(rate[bin]);
	  if (evt.PtTrk[iCSCTrk] > rate[bin] ) hrate_t_mode2 -> Fill(rate[bin]);
	}

	if (track_mode == 4) {
	  if (best_rpc > rate[bin]) hrate_r_mode4 -> Fill(rate[bin]);
	  if (evt.PtTrk[iCSCTrk] > rate[bin] ) hrate_t_mode4 -> Fill(rate[bin]);
	}

	if (track_mode == 5) {
          if (best_rpc > rate[bin]) hrate_r_mode5 -> Fill(rate[bin]);
          if (evt.PtTrk[iCSCTrk] > rate[bin] ) hrate_t_mode5 -> Fill(rate[bin]);
        }

	
      }

      // Fill resolution histograms
      hres_t -> Fill( (evt.PtTrk[iCSCTrk] - evt.muon_pt[iReco])/evt.muon_pt[iReco] );

      hres_r ->Fill( (best_rpc - evt.muon_pt[iReco])/evt.muon_pt[iReco] );
      
      
    
    } // end loop over muons

  } // endl loop over events


  
  //  ============== Write Hists to file =====================
  
  // Little more work for turn on curves
  TGraphAsymmErrors* Pt_turn[N_thresh];
  TGraphAsymmErrors* Pt_turn_rpc[N_thresh];
  for (int ihist = 0; ihist < N_thresh; ihist++) {
    
    Pt_turn[ihist]     = new TGraphAsymmErrors(csctfPt[ihist], csctfPt_all, "");
    Pt_turn_rpc[ihist] = new TGraphAsymmErrors(csctfPt_rpc[ihist], csctfPt_all, "");
    
    Pt_turn_rpc[ihist] -> Write();
    Pt_turn[ihist]     -> Write();
  }
  
  
  hrate_t -> Write();
  hrate_r -> Write();

  hrate_t_all -> Write();
  hrate_r_all -> Write();

  hrate_t_mode1 -> Write();
  hrate_t_mode2 -> Write();
  hrate_t_mode4 -> Write();
  hrate_t_mode5 -> Write();

  hrate_r_mode1 -> Write();
  hrate_r_mode2 -> Write();
  hrate_r_mode4 -> Write();
  hrate_r_mode5 -> Write();

  hres_t -> Write();
  hres_r -> Write();

  hres_t_all -> Write();
  hres_r_all -> Write();

  delete newfile;


  // ===============================================
  // Analysis Results
  cout << "====== Analysis Results =======\n";
  cout << " Number of CSCTF Tracks               = " << num_trk << endl;
  cout << " Number of CSCTF two Hit Tracks       = " << num_trk_two << endl; 
  cout << " Number of CSCTF two Hit Tracks w/RPC = " << num_trk_two_rpc << endl;
  cout << " Number of muons-Tracks with Different Charge = " << num_chg_not_match << endl;
  cout << " Number of muons-Tracks with Same Charge = " << num_chg_match << endl;

}  // end main

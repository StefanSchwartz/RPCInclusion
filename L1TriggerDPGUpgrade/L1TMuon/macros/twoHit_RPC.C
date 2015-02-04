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

void twoHit_RPC(int printLevel = 0){
  
  // class that wraps TTree variables and methods
  EffiEvent evt;
  
  //TFile* file = TFile::Open("../test/rpc_tuple_data_test.root");
  //TFile* file = TFile::Open("../test/rpc_tuple_lowPt_MC.root");
  //TFile* file = TFile::Open("../test/rpc_tuple_allevtTest_MC.root");
  //TFile* file = TFile::Open("root://eoscms//eos/cms/store/user/dcurry/rpc/2012D_reco_5_26/rpc_tuple_2012D_15_1_iiS.root");
  //TFile* file = TFile::Open("root://eoscms//eos/cms/store/user/dcurry/rpc/SingleMu_6_2/rpc_tuple_2012D_34_1_ezD.root");
  //TFile* file = TFile::Open("/data/uftrig01b/dcurry/trig_eff/CMSSW_6_1_2/src/L1TriggerDPG/L1Ntuples/data/rpc/merge2.root");  
  
  
  
  // new storage space on uftrig01a
  //TFile* file = TFile::Open("/exports/uftrig01a/dcurry/data/rpc/2012D_raw_reco_6_5_merge.root");
  TFile* file = TFile::Open("/exports/uftrig01a/dcurry/data/rpc/2012D_raw_reco_6_19_merge_small.root");
  
  evt.AttachToFile(file);
  
  // Output File
  TFile *newfile = new TFile("twoHit_RPC_100.root","recreate");
  //TFile *newfile = new TFile("twoHit_RPC_mc.root","recreate");
  
  bool isMC = false;  
  
  double Pi  = acos(-1.);
  
  //========= define Hists ===================================
  int nbins = 100;
  
  TH2F* hdeta12_pt = new TH2F("hdeta12_pt", "Delta Eta 1-2", 25, -0.2, 0.2, 25, 0, 40);
  TH2F* hdeta23_pt = new TH2F("hdeta23_pt", "Delta Eta 2-3", 25, -0.2, 0.2, 25, 0, 40);
  
  TH2F* hdphi12_pt = new TH2F("hdphi12_pt", "Delta Phi CSC 1-2", nbins, -0.2, 0.2, nbins, 0, 60);
  
  TH1F* hdphi12_csc       = new TH1F("hdphi12_csc", "Delta Phi: CSC1 - CSC2", nbins, -0.1, 0.1);
  TH1F* hdphi12_csc_m     = new TH1F("hdphi12_csc_m", "Delta Phi: CSC1 - CSC2", nbins, -0.1, 0.1);
  
  TH1F* hdphi23_csc       = new TH1F("hdphi23_csc", "Delta Phi: CSC1 - CSC2", nbins, -0.03, 0.03);
  TH1F* hdphi23_csc_m     = new TH1F("hdphi23_csc_m", "Delta Phi: CSC1 - CSC2", nbins, -0.03, 0.03);
  

  TString tempString;
  TH1F* hdphi12_csc_plus[5];
  int pt[] = {3, 5, 7, 10, 15};

  for (int i = 0; i < 5; i++) {
    tempString = TString::Format("hdphi12_csc_plus%d", i);
    hdphi12_csc_plus[i] = new TH1F(tempString.Data(),"", nbins, -0.1, 0.1);
  }

  TH1F* hdphi12_csc_minus[5];
  for (int i = 0; i < 5; i++) {
    tempString = TString::Format("hdphi12_csc_minus%d", i);
    hdphi12_csc_minus[i] = new TH1F(tempString.Data(),"", nbins, -0.1, 0.1);
  }

  TH1F* hdphi12_csc_bit = new TH1F("hdphi12_csc_bit", "Delta Phi: CSC1 - CSC2", nbins, -200, 200);
  TH2F* hdphi12_csc_invpt = new TH2F("hdphi12_csc_invpt", "", nbins, -0.3, 0.3, nbins, -0.05, 0.05);
  TH2F* hdphi12_csc_invpt_abs = new TH2F("hdphi12_csc_invpt_abs", "", nbins, 0, 0.1, nbins, 0, 0.1);
  
  TH2F* hdphi12_csc_pt = new TH2F("hdphi12_csc_pt", "", nbins, -0.2, 0.2, nbins, -0.3, 0.3);
  TH2F* hdphi23_csc_invpt = new TH2F("hdphi23_csc_invpt", "", nbins, -0.2, 0.2, nbins, -0.03, 0.03);
  
  
  TH2F* hdphi12_invpt = new TH2F("hdphi12_invpt", "Delta Phi 1-2, Inv Pt", nbins, -0.2, 0.2, nbins, -0.2, 0.2);
  TH2F* hdphi13_invpt = new TH2F("hdphi13_invpt", "Delta Phi 1-2, Inv Pt", nbins, -0.2, 0.2, nbins, -0.2, 0.2);
  TH2F* hdphi11_invpt = new TH2F("hdphi11_invpt", "Delta Phi 1-2, Inv Pt", nbins, -0.2, 0.2, nbins, -0.2, 0.2);
  TH2F* hdphi23_invpt = new TH2F("hdphi23_invpt", "Delta Phi 1-2, Inv Pt", nbins, -0.2, 0.2, nbins, -0.2, 0.2);

  TH2F* hdphi12_invpt_m = new TH2F("hdphi12_invpt_m", "Delta Phi 1-2, Inv Pt", nbins, 0, 0.3, nbins, -0.2, 0.2);
  TH2F* hdphi13_invpt_m = new TH2F("hdphi13_invpt_m", "Delta Phi 1-2, Inv Pt", nbins, 0, 0.3, nbins, -0.2, 0.2);
  TH2F* hdphi11_invpt_m = new TH2F("hdphi11_invpt_m", "Delta Phi 1-2, Inv Pt", nbins, 0, 0.3, nbins, -0.2, 0.2);
  TH2F* hdphi23_invpt_m = new TH2F("hdphi23_invpt_m", "Delta Phi 1-2, Inv Pt", nbins, 0, 0.3, nbins, -0.2, 0.2);

  TH2F* hdphi12_invpt_abs = new TH2F("hdphi12_invpt_abs", "Delta Phi 1-2, Inv Pt", nbins, -0.3, 0.3, nbins, 0, 0.1);

  TH2F* hdeta13_pt_csc = new TH2F("hdeta13_pt_csc", "Delta Eta 1-3", 25, -0.2, 0.2, 25, 0, 50);
  TH2F* hdphi13_pt_csc = new TH2F("hdphi13_pt_csc", "Delta Phi 1-3", 25, -0.2, 0.2, 25, 0, 50);
  
  TH1F* hdpt = new TH1F("hdpt", "RPC - CSC Pt", 50, -30, 30);
    
  const float rate[] = {0., 3., 5., 7., 10., 12., 16., 20., 30.};
  const int N_rate = (sizeof(rate)/sizeof(float) -1);
  
  TH1F * hrate_f = new TH1F("hrate_f", "CSCTF Rate Front", N_rate,  rate);
  TH1F * hrate_r = new TH1F("hrate_r", "CSCTF Rate Rear", N_rate,  rate);
  TH1F * hrate_t = new TH1F("hrate_t", "CSCTF Rate Track", N_rate,  rate);
  
  TH1F* hdphi12 = new TH1F("hdphi12", "Delta Phi: RPC2 - CSC1", 100, -0.1, 0.1);
  TH1F* hdphi23 = new TH1F("hdphi23", "Delta Phi: RPC2 - CSC3", 100, -0.1, 0.1);
  TH1F* hdphi11 = new TH1F("hdphi11", "Delta Phi: RPC1 - CSC1", 100, -0.1, 0.1);
  TH1F* hdphi13 = new TH1F("hdphi13", "Delta Phi: RPC1 - CSC3", 100, -0.1, 0.1);
  
  TH1F* hdphi12_m = new TH1F("hdphi12_m", "Delta Phi: RPC2 - CSC1", 100, -0.1, 0.1);
  TH1F* hdphi23_m = new TH1F("hdphi23_m", "Delta Phi: RPC2 - CSC3", 100, -0.1, 0.1);
  TH1F* hdphi11_m = new TH1F("hdphi11_m", "Delta Phi: RPC1 - CSC1", 100, -0.1, 0.1);
  TH1F* hdphi13_m = new TH1F("hdphi13_m", "Delta Phi: RPC1 - CSC3", 100, -0.1, 0.1);
  
  
  TH2F* hdphi12_front = new TH2F("hdphi12_front", "Delta Phi 1-2 vs Pt Front", 25, -0.2, 0.2, 25, -30, 30);
  TH2F* hdphi12_rear  = new TH2F("hdphi12_rear", "Delta Phi 1-2 vs Pt Rear", 25, -0.2, 0.2, 25, -30, 30);
  TH2F* hdphi23_rear  = new TH2F("hdphi23_rear", "Delta Phi 2-3 vs Pt Rear", 25, -0.2, 0.2, 25, -30, 30);

  /*
  TString tempString;
  TH1F* hmode[6];

  for (int i = 0; i < 6; i++) {
    tempString = TString::Format("hmode%d", i+1);
    hmode[i] = new TH1F(tempString.Data(),"", 10, 0, 10);
  }
  */

  
  TH1F* hmode1 = new TH1F("hmode1", "Mode 1", 10, 0, 10);
  TH1F* hmode2 = new TH1F("hmode2", "Mode 2", 10, 0, 10);
  TH1F* hmode3 = new TH1F("hmode3", "Mode 3", 10, 0, 10);
  TH1F* hmode4 = new TH1F("hmode4", "Mode 4", 10, 0, 10);
  TH1F* hmode5 = new TH1F("hmode5", "Mode 5", 10, 0, 10);
  TH1F* hmode6 = new TH1F("hmode6", "Mode 6", 10, 0, 10);
  
  TH1F* hmode  = new TH1F("hmode", "", 8, 0, 7);
  

  TH1F* hpt       = new TH1F("hpt","", 100, 0, 100);
  TH1F* hpt12_inv = new TH1F("hpt12_inv","", 50, 0, 1);
  TH1F* hpt12   = new TH1F("hpt12","", 50, 0, 30);
  
  TH1F* heta = new TH1F("heta","", 50, -5, 5);
  TH1F* hphi = new TH1F("hphi","", 50, -3.14, 3.14);
  TH1F* hphi_trk = new TH1F("hphi_trk","", 50, -7.14, 7.14);
  TH1F* heta_trk = new TH1F("heta_trk","", 50, -5, 5);
  
  TH1F* hdeta = new TH1F("hdeta","", 50, 0, 2);
  TH1F* hdr   = new TH1F("hdr","", 50, 0, 1);

  TH1F* hnum_tracks = new TH1F("hnum_tracks" , "", 10, 0, 10);
  
  //==========================================================
  // Counters
  
  int num_trks = 0;
  int num_gbl  = 0;
  int num_matched = 0;
  int num_gbl_match = 0;
  
  //==========================================================
  
  //Start loop over events
  //for (int iEvt = 0; iEvt < evt.GetEntries(); iEvt++) {
  
  // for testing
  for (int iEvt = 0; iEvt < 1000000; iEvt++) {
    
    evt.GetEntry(iEvt);

    if ( (iEvt % 100000) == 0 ) printf(" --- Event # %6d \n", iEvt+1);

    if (printLevel > 1) {
      cout << "\n=======================  Starting new event loop! ===============================" << endl;
      cout << "Event = " << evt.Event << endl;
      cout << " # csctf tracks in event = " << evt.SizeTrk       << endl;
      cout << " # RPC Hits in event     = " << evt.rpc_NumLctsTrk << endl;
      cout << " # Muons in event        = " << evt.muonSize << endl;    
    }
    
    // for data, juts look at single muon events
    if (evt.muonSize > 1) continue;
    
    // ============================================================
    
    // Make initial plots of muon variables
    if (isMC) {
      num_gbl++;
      hpt  -> Fill(evt.gen_pt);
      hphi -> Fill(evt.gen_phi);
      heta -> Fill(evt.gen_eta);

    }
    
    if (!isMC) {
      for (int iReco = 0; iReco < evt.muonSize; iReco++) {
	
	// Muon Info
	double pt_muon = 999;
	int chg_muon   = 999;
	bool isMuon    = false;
	int iCSCTrk    = 999;
	
	if (evt.muon_pt[iReco] < 2) continue;
	if ( abs(evt.muon_eta[iReco]) < 1.2 || abs(evt.muon_eta[iReco]) > 2.1 )  continue;
	
	if (printLevel >1) cout << "Looping over muon # " << iReco << endl;
	
	num_gbl++;
	
	//if (evt.muon_match[iReco] < 900) num_gbl_match++;
	//cout << "muon_match = " << evt.muon_match[iReco] << endl;
		
	hpt  -> Fill(evt.muon_pt[iReco]);
	hphi -> Fill(evt.muon_phi[iReco]);
	heta -> Fill(evt.muon_eta[iReco]);
	
	vector<double> best_dr;
	
	hnum_tracks -> Fill(evt.SizeTrk);

	// Loop over csctf tracks
	for (int iCSCTrk = 0; iCSCTrk < evt.SizeTrk; iCSCTrk++) {
	  
	  if (printLevel >1) cout << "Looping over track # " << iCSCTrk << endl;

	  num_trks++;

	  best_dr.clear();
	  
	  // find which track, if any, is matched to muon.
	  double deta = evt.EtaTrk[iCSCTrk] - evt.muon_eta[iReco];
	  double dr = sqrt( deta*deta );
	  
	  hdeta -> Fill(deta);
	  hdr   -> Fill(dr);
	  
	  if (printLevel > 1) cout << " Delta R Track-muon = " << dr << endl;
	  
	  if (dr < 0.2) {
	    
	    if (printLevel > 1) cout << "Muon # " << iReco << " is matched to Track # " << iCSCTrk << endl;
	    
	    pt_muon  = evt.muon_pt[iReco];
	    chg_muon = evt.muon_chg[iReco];

	    best_dr.push_back(dr);

	    isMuon = true;
	   
	  }

	} // end csctf loop
	
	// Which track is the best match?

	if (isMuon) {
	  
	  num_matched++;

	  auto const it = lower_bound(best_dr.begin(), best_dr.end(), 0);
	  
	  if (printLevel>1) cout << "Best delta R match is Track # " << it - best_dr.begin() << endl;
	
	  iCSCTrk = it - best_dr.begin();
	}
	

    // Filter out non muon events
    if (!isMuon) continue;
    
    // only look at quality three 
    /*    if (evt.ModeTrk[iCSCTrk] != 2 &&
	evt.ModeTrk[iCSCTrk] != 4 && 
	evt.ModeTrk[iCSCTrk] != 5 &&
	evt.ModeTrk[iCSCTrk] != 11 &&
	evt.ModeTrk[iCSCTrk] != 12 &&
	evt.ModeTrk[iCSCTrk] != 14 ) continue;
    */
    //if (evt.ModeTrk[iCSCTrk] !=3) continue;
      
    if ( abs(evt.EtaTrk[iCSCTrk] > 2.1) ) continue;
    
    heta_trk -> Fill(evt.EtaTrk[iCSCTrk]);
    hphi_trk -> Fill(evt.PhiTrk[iCSCTrk]);
    
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
      bool isStation_1 = false;  bool isStation_2 = false; bool isStation_3 = false;
      float phi1 = -999;
      float phi2 = -999;
      float phi3 = -999;
      int phi1_bit = -999;
      int phi2_bit = -999;
      
      // Loop over csc rpc hits to get basic track info: mode
      for (int iCsc=0; iCsc < evt.NumLctsTrk[iCSCTrk]; iCsc++) {
	
        temp_mode += evt.trLctStation[iCSCTrk][iCsc];
	
	if (evt.trLctStation[iCSCTrk][iCsc] == 1) {
	  
	  if (isStation_1) continue;
	  
	  phi1 = evt.trLctglobalPhi[iCSCTrk][iCsc];
	  phi1_bit = evt.trLctPhiBit[iCSCTrk][iCsc];
	  isStation_1 = true;
	}
	
	if (evt.trLctStation[iCSCTrk][iCsc] == 2) {

	  if (isStation_2) continue;
	  
	  phi2 = evt.trLctglobalPhi[iCSCTrk][iCsc];
	  phi2_bit = evt.trLctPhiBit[iCSCTrk][iCsc];
	  isStation_2 = true;
	}

	
        if (evt.trLctStation[iCSCTrk][iCsc] == 3) {

          if (isStation_3) continue;

          phi3 = evt.trLctglobalPhi[iCSCTrk][iCsc];
          phi2_bit = evt.trLctPhiBit[iCSCTrk][iCsc];
          isStation_3 = true;
        }

	
      } // end loop over csc lcts
      
      // fill csc debug hist
      if (isStation_1 && isStation_2) {
	
	float dphi = phi1 - phi2;
	if( dphi > Pi)  dphi = 2*Pi - dphi;
	if( dphi < -Pi) dphi = -(2*Pi - abs(dphi));
	
	hdphi12_csc_invpt -> Fill(chg_muon/pt_muon, dphi);
	hdphi12_csc_bit -> Fill(phi1_bit - phi2_bit);
	hdphi12_csc_invpt_abs -> Fill( abs(dphi), chg_muon/pt_muon);
	
	hdphi12_csc_pt -> Fill(dphi, 1/pt_muon);

	if (chg_muon > 0) hdphi12_csc   -> Fill(dphi);
	else              hdphi12_csc_m -> Fill(dphi);
	
	//cout << "\n\n Phi1 = " << phi1 << ", Phi2 = " << phi2 << endl;
	//cout << "Muon charge = " << chg_muon << endl;


	if (pt_muon > 3 && pt_muon < 5)
	  if (chg_muon > 0) hdphi12_csc_plus[0]  -> Fill(dphi);
	  else              hdphi12_csc_minus[0] -> Fill(dphi);
	
	if (pt_muon > 5 && pt_muon < 7)
          if (chg_muon > 0) hdphi12_csc_plus[1]  -> Fill(dphi);
	  else              hdphi12_csc_minus[1] -> Fill(dphi);

	if (pt_muon > 7 && pt_muon < 10)
          if (chg_muon > 0) hdphi12_csc_plus[2]  -> Fill(dphi);
	  else              hdphi12_csc_minus[2] -> Fill(dphi);
	
	if (pt_muon > 10 && pt_muon < 15)
          if (chg_muon > 0) hdphi12_csc_plus[3]  -> Fill(dphi);
	  else              hdphi12_csc_minus[3] -> Fill(dphi);

	if (pt_muon > 15)
          if (chg_muon > 0) hdphi12_csc_plus[4]  -> Fill(dphi);
	  else              hdphi12_csc_minus[4] -> Fill(dphi);
	
      }
      
      if (isStation_2 && isStation_3) {

	float dphi23 = phi2 - phi3;
	
	if (chg_muon > 0) hdphi23_csc   -> Fill(dphi23);
        else              hdphi23_csc_m -> Fill(dphi23);	
	
	hdphi23_csc_invpt -> Fill(chg_muon/pt_muon, dphi23);	
	
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
      
      // =====================================================
      
      if (printLevel>1) cout << " Track Mode = " << track_mode << endl;

      
      // Fill track mode hists
      
      switch (track_mode) {
      case 1: hmode -> Fill(1);
	break;
      case 2: hmode -> Fill(2);
	break;
      case 3: hmode -> Fill(3);
        break;
      case 4: hmode -> Fill(4);
	break;
      case 5: hmode -> Fill(5);
        break;
      case 6: hmode -> Fill(6);
        break;
      }
      

      // =======  Filter only certain modes, etc. ===========
      
      //      if (track_mode != 2) continue;
      
      // =====================================================
      
      int num_rpc_matched = 0;

      // Loop over RPC hits in event record.  Look for insertion rpc candidate
      for (int iRpc = 0; iRpc < evt.rpc_NumLctsTrk; iRpc++) {
	
	// dont look at rpc hits that are not in 0.9-1.6 eta
	if ( (abs(evt.rpc_gblEta[iRpc]) < 0.9) || (abs(evt.rpc_gblEta[iRpc]) > 1.6) ) continue;
	
	//if (evt.PtTrk_reco_front[iCSCTrk][iRpc] == -999 || evt.PtTrk_reco_front[iCSCTrk][iRpc] == 999) continue;
	
	num_rpc_matched++;
	
        // ========================= Does the rpc hit fall within eta/phi windows with the csc hit? ===============================
        
        if ( evt.rpc_Station[iRpc] == 2 || evt.rpc_Station[iRpc] == 1 ) {

          if (printLevel>1) cout << " Track is mode 2 and rpc is in station 2 or 1" << endl;
	  
	  // bools for rpc-csc windows
	  bool isMatched_eta_1 = false;
          bool isMatched_eta_3 = false;
          bool isMatched_phi_1 = false;
          bool isMatched_phi_3 = false;
	  bool isMatched_13    = false;

	  float dphi12 = -999; float dphi23 = -999;
	  float deta12 = -999; float deta23 = -999;
	  float dphi11 = -999; float dphi13 = -999;
	  float eta1; float eta3; 
	  float phi3;

	  // loop over csc hits
          for (int iCsc=0; iCsc < evt.NumLctsTrk[iCSCTrk]; iCsc++) {
	    
	    if (printLevel>1) {
	      cout << "LCT gbl eta = " << evt.trLctglobalEta[iCSCTrk][iCsc] << endl;
	      cout << "LCT gbl phi = " << evt.trLctglobalPhi[iCSCTrk][iCsc] << endl;
	    }
	    
	    //if (evt.trLctglobalEta[iCSCTrk][iCsc] < 0 ) continue;

	    // Set rpc eta to be csctf eta bit
	    int temp_eta_bit = 90*abs(evt.rpc_gblEta[iRpc]) - 81;
	    
	    // check csc station 1 with rpc station 2
            if (evt.trLctStation[iCSCTrk][iCsc] == 1 && evt.rpc_Station[iRpc] == 2) {
	      
	      eta1 = evt.trLctglobalEta[iCSCTrk][iCsc];
	      phi1 = evt.trLctglobalPhi[iCSCTrk][iCsc];
	      
	      dphi12 = evt.rpc_gblPhi[iRpc] - evt.trLctglobalPhi[iCSCTrk][iCsc]; 
	      deta12 = evt.rpc_gblEta[iRpc] - evt.trLctglobalEta[iCSCTrk][iCsc];
	      
	      if(dphi12 > Pi) dphi12 = 2*Pi - dphi12;
	      
	      // Fill dphi hists
	      //	      hdphi12       -> Fill(dphi12);
	      hdphi12_front -> Fill(dphi12, evt.PtTrk_reco_front[iCSCTrk][iRpc]-evt.PtTrk[iCSCTrk]);
	      hdphi12_rear  -> Fill(dphi12, evt.PtTrk_reco_rear[iCSCTrk][iRpc]-evt.PtTrk[iCSCTrk]);
	      
	      hpt12     -> Fill(pt_muon);
	      hpt12_inv -> Fill(chg_muon/pt_muon);
	      hdphi12_invpt -> Fill(chg_muon/pt_muon, dphi12);      
	      hdphi12_pt    -> Fill(dphi12, pt_muon);

	      if (chg_muon > 0) {
		hdphi12       -> Fill(dphi12);
	      }
	      else {
		hdphi12_invpt_m -> Fill(1/pt_muon, dphi12);
		hdphi12_m       -> Fill(dphi12);
	      }

	      hdphi12_invpt_abs -> Fill(chg_muon/pt_muon, abs(dphi12) );
	      

	      // ===============================================
	      // For rpc hits in stations find pt when dphi < 0.1.  The InvPt profile has been fitted for predictive behavior.
	      // Fit = [0]*x + [1], where [0] = 1.44178 and [1] = 1.20259e-01, and x = dphi 

	      if ( abs(dphi12) < 0.01) {
	      
		float rpc_pt = 1/(1.44178 * abs(dphi12) + 1.20259e-01);
		     
		     if (printLevel > 0) {
		       cout << " Track Pt  = " << evt.PtTrk[iCSCTrk] << endl;
		       cout << " Rpc Pt    = " << rpc_pt << endl;
		     }
		   }
		
	      
              if ( abs(evt.trLctEtaBit[iCSCTrk][iCsc] - temp_eta_bit) <= 4 ) isMatched_eta_1 = true;
	      
	      int eta_bit = temp_eta_bit >> 1;
              int dphi_window = eta_dphi_ME1toME2[eta_bit];
	      if ( abs(evt.trLctPhiBit[iCSCTrk][iCsc] - evt.rpc_Phibit[iRpc]) <= dphi_window ) isMatched_phi_1 = true;
	      
	    }
	    
	    
	    // Also look at CSC station 1 with RPC station 1.  Both rpc station 1 and 2 are sandwhiched between ME 1 and 3.
	    if (evt.trLctStation[iCSCTrk][iCsc] == 1 && evt.rpc_Station[iRpc] == 1) {

	      dphi11 = evt.rpc_gblPhi[iRpc] - evt.trLctglobalPhi[iCSCTrk][iCsc];
	      
	      if(dphi11 > Pi) dphi11 = 2*Pi - dphi11;
	      
	      hdphi11 -> Fill(dphi11);
	      hdphi11_invpt -> Fill(chg_muon/pt_muon, dphi11);

     
	      if (chg_muon > 0) {
		hdphi11       -> Fill(dphi11);
              }
              else {
		hdphi11_invpt_m -> Fill(1/pt_muon, dphi11);
		hdphi11_m       -> Fill(dphi11);
              }
	    }
	    
	    
            if (evt.trLctStation[iCSCTrk][iCsc] == 3 && evt.rpc_Station[iRpc] == 1) {
	      
	      dphi13 = evt.rpc_gblPhi[iRpc] - evt.trLctglobalPhi[iCSCTrk][iCsc];

	      if(dphi13 > Pi) dphi13 = 2*Pi - dphi13;

	      hdphi13_invpt -> Fill(chg_muon/pt_muon, dphi13);
              hdphi13 -> Fill(dphi13);
	      
	      if (chg_muon > 0) {
		
                hdphi13       -> Fill(dphi13);
              }
              else {
		hdphi13_invpt_m -> Fill(1/pt_muon, dphi13);
		hdphi13_m       -> Fill(dphi13);
              }
            }
	    
	    // check csc station 3 with rpc station 2
	    if (evt.trLctStation[iCSCTrk][iCsc] == 3 && evt.rpc_Station[iRpc] == 2) {
	      
	      eta3 = evt.trLctglobalEta[iCSCTrk][iCsc];
	      phi3 = evt.trLctglobalPhi[iCSCTrk][iCsc];
	      
	      dphi23 = evt.rpc_gblPhi[iRpc] - evt.trLctglobalPhi[iCSCTrk][iCsc];
	      deta23 = evt.rpc_gblEta[iRpc] - evt.trLctglobalEta[iCSCTrk][iCsc];

	      if(dphi23 > Pi) dphi23 = 2*Pi - dphi23;
	      
	      hdphi23_invpt -> Fill(chg_muon/pt_muon, dphi23);
	      hdphi23 -> Fill(dphi23);

	      if (chg_muon > 0) {
                hdphi23       -> Fill(dphi23);
              }
              else {
		hdphi23_invpt_m -> Fill(1/pt_muon, dphi23);
		hdphi23_m       -> Fill(dphi23);
              }

	      if ( abs(evt.trLctEtaBit[iCSCTrk][iCsc] - temp_eta_bit) <= 6 ) isMatched_eta_3 = true;
	      
	      if ( abs(evt.trLctPhiBit[iCSCTrk][iCsc]/4 - evt.rpc_Phibit[iRpc]/4) <= 127 ) isMatched_phi_3 = true;
	      
	    } 
	    
	  } // end CSC lct loop
	  
	  //	  if ( isMatched_eta_1 && isMatched_phi_1 && isMatched_eta_3 && isMatched_phi_3) {
	    
	  if ( abs(dphi12) < 0.2 && abs(dphi23) < 0.2 ) {
	    
            isMatched_13=true;

	    if (printLevel>1) {
	      cout << "Front RPC pt = " << evt.PtTrk_reco_front[iCSCTrk][iRpc] << endl; 
	      cout << "Rear RPC pt  = " << evt.PtTrk_reco_rear[iCSCTrk][iRpc] << endl;
	      cout << "Track pt     = " << evt.PtTrk[iCSCTrk] << endl; 
	      cout << "Best pt      = " << evt.PtTrk_reco_best_two[iCSCTrk][iRpc] << endl;
	    }
	    
	    /*
	    // Fill Hists
	    hdeta12_pt -> Fill(deta12, evt.gen_pt);
	    hdeta23_pt -> Fill(deta23, evt.gen_pt);
	    hdphi12_pt -> Fill(dphi12, evt.gen_pt);
	    hdphi23_pt -> Fill(dphi23, evt.gen_pt);
	    
	    hdeta13_pt_csc -> Fill(eta1-eta3, evt.gen_pt);
	    hdphi13_pt_csc -> Fill(phi1-phi3, evt.gen_pt);
	    
	    hdpt -> Fill(evt.PtTrk_reco_best_two[iCSCTrk][iRpc] - evt.PtTrk[iCSCTrk]);
	    */
	    
	    // fill rate plots
	    
	    if (evt.PtTrk_reco_front[iCSCTrk][iRpc] == -999 || evt.PtTrk_reco_rear[iCSCTrk][iRpc] == -999) continue;

	    for (int bin = N_rate -1 ; bin >=0 ; bin-- ) {
	      
	      if (evt.PtTrk[iCSCTrk] > rate[bin] ) hrate_t -> Fill(rate[bin]); 
	      
	      if (evt.PtTrk_reco_front[iCSCTrk][iRpc] > rate[bin] ) hrate_f -> Fill(rate[bin]); 
	      
	      if (evt.PtTrk_reco_rear[iCSCTrk][iRpc] > rate[bin] ) hrate_r -> Fill(rate[bin]); 
	      
	    }

	  } // is matched
	  
	}

      } // end RPC loop
      
    
      } // end reco loop
    } // end !isMc
  } // end event loop
  
  // ========================================================================
  
  
  hdeta12_pt -> Write();
  hdeta23_pt -> Write();
  hdphi12_pt -> Write();


  hdphi12_invpt -> Write();
  hdphi11_invpt -> Write();
  hdphi13_invpt -> Write();
  hdphi23_invpt -> Write();

  hdphi12_invpt_m -> Write();
  hdphi11_invpt_m -> Write();
  hdphi13_invpt_m -> Write();
  hdphi23_invpt_m -> Write();

  hdeta13_pt_csc -> Write();
  hdphi13_pt_csc -> Write();

  hdpt    -> Write();
  hrate_f -> Write();
  hrate_r -> Write();
  hrate_t -> Write();
  
  hdphi12 -> Write();
  hdphi23 -> Write();
  hdphi11 -> Write();
  hdphi13 -> Write();

  hdphi12_m -> Write();
  hdphi23_m -> Write();
  hdphi11_m -> Write();
  hdphi13_m -> Write();

  hmode -> Write();
  hmode2 ->Write();
  hmode3 ->Write();
  hmode4 ->Write();
  hmode5 ->Write();
  hmode6 ->Write();

  hdphi12_front ->Write();
  hdphi12_rear  ->Write();

  hpt  -> Write();
  hphi -> Write();
  heta -> Write();
    
  hpt12     -> Write();
  hpt12_inv -> Write();

  hdphi12_csc_invpt -> Write();
  hdphi12_csc -> Write();
  hdphi12_csc_bit -> Write();
  hdphi12_csc_invpt_abs -> Write();

  hdphi12_csc_pt    -> Write();
  hdphi23_csc_invpt -> Write();

  heta_trk -> Write();
  hphi_trk -> Write();
  hdeta    -> Write();
  hdr      -> Write();

  hnum_tracks -> Write();

  hdphi12_invpt_abs -> Write();

  hdphi12_csc_m -> Write();

  hdphi23_csc_m -> Write();
  hdphi23_csc   -> Write();


  for (int i=0; i < 5; i++) {
    hdphi12_csc_plus[i]  -> Write();
    hdphi12_csc_minus[i] -> Write();
  }


  delete newfile;

  // ==========================================================
  // Analysis Results
  
  cout << "============= Results ============\n";
  cout << " # Global Muons   = " << num_gbl     << endl;
  cout << " # CSCTF Tracks   = " << num_trks    << endl;
  cout << " # Matched Tracks = " << num_matched << endl;
  cout << " # Matched Tracks with new algorithm = " << num_gbl_match << endl;

} // end main

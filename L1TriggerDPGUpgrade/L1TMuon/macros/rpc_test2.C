#include<exception>
#include<vector>
#include<iostream>
#include<cmath>

#include<TSystem.h>
#include<TFile.h>
#include<TTree.h>
#include<TCanvas.h>
#include<TLine.h>
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


void rpc_test2(int printLevel = 0){

  // class that wraps TTree variables and methods
  EffiEvent evt;

  //TFile* file = TFile::Open("data/rpc_tuple_2012C_best.root");
  TFile* file = TFile::Open("../test/rpc_tuple_allevtTest_MC.root");
  //TFile* file = TFile::Open("../test/rpc_tuple_data_test.root");
  //TFile* file = TFile::Open("/cms/data/store/user/dcurry/rpc/2012C_best3/merged_2012C.root");

  evt.AttachToFile(file);

  // ========= Histograms ======================================
  TH2F* dphi_12_pt = new TH2F("dphi_12_pt", "Track Pt vs Delta Phi: CSC1 - CSC2", 50, -0.2, 0.2, 60, 0, 60);
  TH2F* dphi_23_pt = new TH2F("dphi_23_pt", "Track Pt vs Delta Phi: CSC2 - CSC3", 50, -0.2, 0.2, 60, 0, 60);

  TH2F* dphi_12_pt_rpc = new TH2F("dphi_12_pt_rpc", "Above Track Pt vs Delta Phi: CSC1 - RPC2", 40, -0.2, 0.2, 40, 0, 60);
  TH2F* dphi_23_pt_rpc = new TH2F("dphi_23_pt_rpc", "Above Track Pt vs Delta Phi: CSC3 - RPC2", 40, -0.2, 0.2, 40, 0, 60);

  TH2F* dphi_12_pt_rpc_below = new TH2F("dphi_12_pt_rpc_below", "Below Track Pt vs Delta Phi: CSC1 - RPC2", 40, -0.2, 0.2, 40, 0, 60);
  TH2F* dphi_23_pt_rpc_below = new TH2F("dphi_23_pt_rpc_below", "Below Track Pt vs Delta Phi: CSC3 - RPC2", 40, -0.2, 0.2, 40, 0, 60);

  TH2F* pt_promote  = new TH2F("pt_promote", "", 40, -30, 30, 40, -30, 30); 

  // ==========================================================
  
  // Counters =================================================




  //Start loop over events
  for (int iEvt = 0; iEvt < evt.GetEntries(); iEvt++) {

  // for testing
  //for (int iEvt = 0; iEvt < 10000000; iEvt++) {

    evt.GetEntry(iEvt);
    if ( ( iEvt % 100000) == 0 ) printf(" --- Event # %6d \n", iEvt+1);

    float dphi12 = -999;
    float dphi23 = -999;
    int pt       = -999; 

    int pt_rpc       = -999;
    float dphi12_rpc = -999;
    float dphi23_rpc = -999;    

    // Loop over csctf tracks
    for (int iCSCTrk = 0; iCSCTrk < evt.SizeTrk; iCSCTrk++) {

      if (evt.NumLctsTrk[iCSCTrk] != 3) continue;

      // first find a three hit track(station 1-2-3) and record its deflection angles and pt
      int temp_mode = 0;

      // Loop over csc rpc hits to get basic track info: mode, is it matched, etc.
      for (int iCsc=0; iCsc < evt.NumLctsTrk[iCSCTrk]; iCsc++) {

	temp_mode += evt.trLctStation[iCSCTrk][iCsc];

      } // end loop over csc lcts
      
      if (temp_mode != 6) continue;

      // loop over csc hits and record defelction angles between station 1-2 and 2-3 hits
      //cout << "tracks lct stations  = " << evt.trLctStation[iCSCTrk][0] << "," << evt.trLctStation[iCSCTrk][1] << "," << evt.trLctStation[iCSCTrk][2] << endl;
      
      dphi12 = evt.trLctglobalPhi[iCSCTrk][0] - evt.trLctglobalPhi[iCSCTrk][1];
      dphi23 = evt.trLctglobalPhi[iCSCTrk][1] - evt.trLctglobalPhi[iCSCTrk][2];
      pt = evt.PtTrk[iCSCTrk];
      
      dphi_12_pt -> Fill(dphi12, pt);
      dphi_23_pt -> Fill(dphi23, pt);
      
     
      // cout << "This 3 hit track(1-2-3) has pt = " << pt << endl;
      // cout << " dphi12 = " << dphi12 << endl;
      // cout << " dphi23 = " << dphi23 << endl << endl;
      
      break;

    } // end first track loop      


    // ======================================================================================================================

      // loop over tracks again, this time looking at two hit tracks
    for (int iCSCTrk = 0; iCSCTrk < evt.SizeTrk; iCSCTrk++) {

      if (evt.NumLctsTrk[iCSCTrk] != 2) continue;
      
      // first find a two hit track(station 1-3)
      int temp_mode = 0;

      // Loop over csc rpc hits to get basic track info: mode, is it matched, etc.
      for (int iCsc=0; iCsc < evt.NumLctsTrk[iCSCTrk]; iCsc++) {

        temp_mode += evt.trLctStation[iCSCTrk][iCsc];

      } // end loop over csc lcts

      if (temp_mode != 4) continue;

      // look for rpc hit in station 2, record its delfection angles and pt when inserted.
      for (int iRpc = 0; iRpc < evt.rpc_NumLctsTrk; iRpc++) {
      
	// check to see if rpc is in station 2, is matched, and has small dr
	if (evt.rpcIsmatched[iCSCTrk][iRpc] != 1) continue;
	
	if (evt.rpc_Station[iRpc] != 2) continue;

	pt_rpc = evt.PtTrk_reco_best_two[iCSCTrk][iRpc];

	// loop over csc lcts to find deflection angles
	for (int iCsc=0; iCsc < evt.NumLctsTrk[iCSCTrk]; iCsc++) {

	  if (evt.trLctStation[iCSCTrk][iCsc] == 1) dphi12_rpc = evt.trLctglobalPhi[iCSCTrk][iCsc] - evt.rpc_gblPhi[iRpc];
	  
	  if (evt.trLctStation[iCSCTrk][iCsc] == 3) dphi23_rpc = evt.rpc_gblPhi[iRpc] - evt.trLctglobalPhi[iCSCTrk][iCsc];


	  if (pt_rpc >= evt.PtTrk[iCSCTrk]) {
	    dphi_12_pt_rpc -> Fill (dphi12_rpc,  pt_rpc);
	    dphi_23_pt_rpc -> Fill (dphi23_rpc,  pt_rpc);
	  }

	  if (pt_rpc < evt.PtTrk[iCSCTrk]) {
            dphi_12_pt_rpc_below -> Fill (dphi12_rpc,  pt_rpc);
            dphi_23_pt_rpc_below -> Fill (dphi23_rpc,  pt_rpc);
          }
	  
	  // find if track was promoted or not
	  float dpt_track = pt_rpc - evt.PtTrk[iCSCTrk]; 
	  
	  float track_muon  = abs(evt.PtTrk[iCSCTrk] - evt.gen_pt);
	  float rpc_muon    = abs(pt_rpc - evt.gen_pt);
	  
	  float pt_close    = rpc_muon - track_muon;
	  
	  pt_promote -> Fill(dpt_track, pt_close);

	  


	} // end loop over csc lcts

      } // end loop over rpc hits
      
      //  cout << "This 2 hit track(1-3) has pt = " << pt_rpc << endl;
      //  cout << " dphi12 = " << dphi12_rpc << endl; 
      //  cout << " dphi23 = " << dphi23_rpc << endl << endl;


    } // endl loop over csc tracks

  } // end loop over events


  // Make Plots ==============================
  
  TCanvas* c7 = new TCanvas("c7", "", 420, 500, 600, 600);
  c7 -> Divide(1,2);
  c7 -> cd(1);
  dphi_12_pt -> GetXaxis() -> SetTitle("CSC Phi - CSC Phi");
  dphi_12_pt -> GetXaxis() -> SetTitleOffset(1.5);
  dphi_12_pt -> GetYaxis() -> SetTitle("CSCTF Pt GeV");
  dphi_12_pt -> GetYaxis() -> SetTitleOffset(1.5);
  dphi_12_pt -> Draw("CONT");
  dphi_12_pt -> SetStats(0);
  c7 -> cd(2);
  //  TCanvas* c8 = new TCanvas("c8", "", 420, 500, 600, 600);
  dphi_23_pt -> GetXaxis() -> SetTitle("CSC Phi - CSC Phi");
  dphi_23_pt -> GetXaxis() -> SetTitleOffset(1.5);
  dphi_23_pt -> GetYaxis() -> SetTitle("CSCTF Pt GeV");
  dphi_23_pt -> GetYaxis() -> SetTitleOffset(1.5);
  dphi_23_pt -> SetFillColor(kRed);
  dphi_23_pt -> Draw("CONT");
  dphi_23_pt -> SetStats(0);
  

  

  TCanvas* c8 = new TCanvas("c8", "", 420, 500, 600, 600);
  c8 -> Divide(1,2);
  c8 -> cd(1);
  dphi_12_pt_rpc -> GetXaxis() -> SetTitle("RPC Phi - CSC Phi");
  dphi_12_pt_rpc -> GetXaxis() -> SetTitleOffset(1.5);
  dphi_12_pt_rpc -> GetYaxis() -> SetTitle("CSCTF Pt GeV");
  dphi_12_pt_rpc -> GetYaxis() -> SetTitleOffset(1.5);
  dphi_12_pt_rpc -> Draw("CONT");
  dphi_12_pt_rpc -> SetStats(0);
 
  c8 -> cd(2);
  dphi_23_pt_rpc -> GetXaxis() -> SetTitle("RPC Phi - CSC Phi");
  dphi_23_pt_rpc -> GetXaxis() -> SetTitleOffset(1.5);
  dphi_23_pt_rpc -> GetYaxis() -> SetTitle("CSCTF Pt GeV");
  dphi_23_pt_rpc -> GetYaxis() -> SetTitleOffset(1.5);
  dphi_23_pt_rpc -> Draw("CONT");
  dphi_23_pt_rpc -> SetStats(0);


  TCanvas* c9 = new TCanvas("c9", "", 420, 500, 600, 600);
  c9 -> Divide(1,2);
  c9 -> cd(1);
  dphi_12_pt_rpc_below -> GetXaxis() -> SetTitle("RPC Phi - CSC Phi");
  dphi_12_pt_rpc_below -> GetXaxis() -> SetTitleOffset(1.5);
  dphi_12_pt_rpc_below -> GetYaxis() -> SetTitle("CSCTF Pt GeV");
  dphi_12_pt_rpc_below -> GetYaxis() -> SetTitleOffset(1.5);
  dphi_12_pt_rpc_below -> Draw("CONT");
  dphi_12_pt_rpc_below -> SetStats(0);

  c9 -> cd(2);
  dphi_23_pt_rpc_below -> GetXaxis() -> SetTitle("RPC Phi - CSC Phi");
  dphi_23_pt_rpc_below -> GetXaxis() -> SetTitleOffset(1.5);
  dphi_23_pt_rpc_below -> GetYaxis() -> SetTitle("CSCTF Pt GeV");
  dphi_23_pt_rpc_below -> GetYaxis() -> SetTitleOffset(1.5);
  dphi_23_pt_rpc_below -> Draw("CONT");
  dphi_23_pt_rpc_below -> SetStats(0);

  TCanvas* c1 = new TCanvas("c1", "", 420, 500, 600, 600);
  dphi_12_pt_rpc -> SetFillColor(kRed);
  dphi_12_pt_rpc -> GetXaxis() -> SetTitle("RPC Phi - CSC Phi");
  dphi_12_pt_rpc -> GetXaxis() -> SetTitleOffset(1.5);
  dphi_12_pt_rpc -> GetYaxis() -> SetTitle("CSCTF Pt GeV");
  dphi_12_pt_rpc -> GetYaxis() -> SetTitleOffset(1.5);
  dphi_12_pt_rpc -> Draw("box");
  dphi_12_pt_rpc -> SetStats(0);

  dphi_12_pt_rpc_below -> SetFillColor(kBlue);
  dphi_12_pt_rpc_below -> Draw("box SAME");
  dphi_12_pt_rpc_below -> SetStats(0);  

  TCanvas* c2 = new TCanvas("c2", "", 420, 500, 600, 600);
  dphi_23_pt_rpc -> SetFillColor(kRed);
  dphi_23_pt_rpc -> GetXaxis() -> SetTitle("RPC Phi - CSC Phi");
  dphi_23_pt_rpc -> GetXaxis() -> SetTitleOffset(1.5);
  dphi_23_pt_rpc -> GetYaxis() -> SetTitle("CSCTF Pt GeV");
  dphi_23_pt_rpc -> GetYaxis() -> SetTitleOffset(1.5);
  dphi_23_pt_rpc -> Draw("box");
  dphi_23_pt_rpc -> SetStats(0);

  dphi_23_pt_rpc_below -> SetFillColor(kBlue);
  dphi_23_pt_rpc_below -> Draw("box SAME");
  dphi_23_pt_rpc_below -> SetStats(0);
  

  TCanvas* c3 = new TCanvas("c2", "", 420, 500, 600, 600);
  //dphi_23_pt_rpc -> SetFillColor(kRed);
  pt_promote -> GetXaxis() -> SetTitle("RPC Pt - Track Pt");
  pt_promote -> GetXaxis() -> SetTitleOffset(1.5);
  pt_promote -> GetYaxis() -> SetTitle("Comparison of rpc-track & rpc-muon");
  pt_promote -> GetYaxis() -> SetTitleOffset(1.5);
  pt_promote -> Draw("CONT");
  pt_promote -> SetStats(0);
  
  TLine *line_1 = new TLine(0,-30,0,30);
  line_1 -> SetLineWidth(2);
  line_1 -> SetLineStyle(kDashed);
  line_1 -> SetLineColor(kRed);
  line_1 -> Draw("same");

  TLine *line_2 = new TLine(-30,0,30,0);
  line_2 -> SetHorizontal();
  line_2 -> SetLineWidth(2);
  line_2 -> SetLineStyle(kDashed);
  line_2 -> SetLineColor(kRed);
  line_2 -> Draw("same");


} // end main

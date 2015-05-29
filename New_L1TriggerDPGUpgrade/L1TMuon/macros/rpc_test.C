
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


void rpc_test(int printLevel = 0){

  
  // class that wraps TTree variables and methods
  EffiEvent evt;

  //TFile* file = TFile::Open("data/rpc_tuple_2012C_best.root");
  //TFile* file = TFile::Open("../test/rpc_tuple_allevtTest_MC.root");
  //TFile* file = TFile::Open("../test/rpc_tuple_data_test.root");
  TFile* file = TFile::Open("/cms/data/store/user/dcurry/rpc/2012C_best3/merged_2012C.root");

  evt.AttachToFile(file);

  // ====== Histograms ======
  TH1F* dr_12  = new TH1F("dr_12", "Delta Phi: CSC1 - RPC2", 50, -0.2, 0.2);
  TH1F* eta_12 = new TH1F("eta_12", "Delta Eta: CSC1 - RPC2", 50, -0.2, 0.2);
  
  TH1F* dr_23  = new TH1F("dr_23", "Delta Phi: CSC3 - RPC2", 50, -0.2, 0.2);
  TH1F* eta_23 = new TH1F("eta_23", "Delta Eta: CSC3 - RPC2", 50, -0.2, 0.2);
  
  TH1F* dr_11  = new TH1F("dr_11", "Delta Phi: CSC1 - RPC1", 50, -0.2, 0.2);
  TH1F* eta_11 = new TH1F("eta_11", "Delta Eta: CSC1 - RPC1", 50, -0.2, 0.2);
  
  TH1F* dr_22  = new TH1F("dr_22", "Delta Phi: CSC2 - RPC2", 50, -0.2, 0.2);
  TH1F* eta_22 = new TH1F("eta_22", "Delta Eta: CSC2 - RPC2", 50, -0.2, 0.2);

  TH1F* dr_33  = new TH1F("dr_33", "Delta R: CSC3 - RPC3", 50, -0.2, 0.2);
  TH1F* eta_33 = new TH1F("eta_33", "Delta Eta: CSC3 - RPC3", 50, -0.2, 0.2);

  TH2F* dphi_123   = new TH2F("dphi_123", "Delta Phi: CSC1&3 - RPC2", 50, -0.2, 0.2, 50, -0.2, 0.2);
  TH2F* dphi_12_pt = new TH2F("dphi_12_pt", "Track Pt vs Delta Phi", 50, -0.2, 0.2, 60, 0, 60);
  TH2F* dphi_23_pt = new TH2F("dphi_23_pt", "Track Pt vs Delta Phi", 50, -0.2, 0.2, 60, 0, 60);
  // ========================



  //Start loop over events
  //for (int iEvt = 0; iEvt < evt.GetEntries(); iEvt++) {

  // for testing
  for (int iEvt = 0; iEvt < 10000000; iEvt++) {

    evt.GetEntry(iEvt);
    if ( ( iEvt % 100000) == 0 ) printf(" --- Event # %6d \n", iEvt+1);

    // Loop over csctf tracks
    for (int iCSCTrk = 0; iCSCTrk < evt.SizeTrk; iCSCTrk++) {

      if (evt.SizeTrk !=2) continue;

      bool isStation1 = false;
      bool isStation3 =false;

      // loop over csc lcts
      for (int iCsc=0; iCsc < evt.NumLctsTrk[iCSCTrk]; iCsc++) {

 	if (evt.trLctStation[iCSCTrk][iCsc] == 1) isStation1 = true;
	if (evt.trLctStation[iCSCTrk][iCsc] == 3) isStation3 = true;
      
      } // end loop over csc lcts
      
      // look at tracks which have hit in at least stations 1 and 3
      if ( !isStation1 || !isStation3 ) continue;	

      // loop over rpc hits
      for (int iRpc=0; iRpc<evt.rpc_NumLctsTrk; iRpc++) {

	// skip rpc hits that arent matched to track
	if (evt.rpcIsmatched[iCSCTrk][iRpc] != 1) continue;

	// used for scatter plot
	float dphi12 = 0;
	float dphi23 = 0;
	bool is12 = false;
	bool is23 = false;


	// loop over csc lcts
	for (int iCsc=0; iCsc < evt.NumLctsTrk[iCSCTrk]; iCsc++) {
	
	  // check to make sure lct and rpc are in same endcap
	  if ( (evt.rpc_gblEta[iRpc] > 0 && evt.trLctglobalEta[iCSCTrk][iCsc] < 0) ||
	       (evt.rpc_gblEta[iRpc] < 0 && evt.trLctglobalEta[iCSCTrk][iCsc] > 0) ) continue;

	  float dr = sqrt(abs(abs(abs(evt.rpc_gblPhi[iRpc]-evt.trLctglobalPhi[iCSCTrk][iCsc])-3.14)-3.14)* \
			  abs(abs(abs(evt.rpc_gblPhi[iRpc]-evt.trLctglobalPhi[iCSCTrk][iCsc])-3.14)-3.14)+ \
			  ((evt.rpc_gblEta[iRpc]-evt.trLctglobalEta[iCSCTrk][iCsc])*(evt.rpc_gblEta[iRpc]-evt.trLctglobalEta[iCSCTrk][iCsc])));
	  
	  if (dr > 0.1) continue;

	  // fill plots for different station combos
	  if (evt.trLctStation[iCSCTrk][iCsc] == 1 && evt.rpc_Station[iRpc] == 2) {
	    
	    is12 = true;
	    float dphi = evt.rpc_gblPhi[iRpc]-evt.trLctglobalPhi[iCSCTrk][iCsc];
	    
	    dphi12     = evt.rpc_gblPhi[iRpc]-evt.trLctglobalPhi[iCSCTrk][iCsc];

	    float deta = evt.rpc_gblEta[iRpc]-evt.trLctglobalEta[iCSCTrk][iCsc];
	    
	    dr_12 -> Fill(dphi);
	    eta_12 -> Fill(deta);
	    
	    if (deta > 1) {
	      cout << "Dr      = " << dr << endl;
	      cout << "Lct eta = " << evt.trLctglobalEta[iCSCTrk][iCsc] << endl;
	      cout << "Rpc eta = " << evt.rpc_gblEta[iRpc] << endl << endl;
	    }
	    
	  }

	  
	  if (evt.trLctStation[iCSCTrk][iCsc] == 3 && evt.rpc_Station[iRpc] == 2) {

	    is23 = true;
	    float dphi = evt.rpc_gblPhi[iRpc]-evt.trLctglobalPhi[iCSCTrk][iCsc];
            
	    dphi23     = evt.rpc_gblPhi[iRpc]-evt.trLctglobalPhi[iCSCTrk][iCsc];
	    
	    float deta = evt.rpc_gblEta[iRpc]-evt.trLctglobalEta[iCSCTrk][iCsc];
            dr_23 -> Fill(dphi);
	    eta_23 -> Fill(deta);
          }

	  if (evt.trLctStation[iCSCTrk][iCsc] == 1 && evt.rpc_Station[iRpc] == 1) {

	    float dphi = evt.rpc_gblPhi[iRpc]-evt.trLctglobalPhi[iCSCTrk][iCsc];
            float deta = evt.rpc_gblEta[iRpc]-evt.trLctglobalEta[iCSCTrk][iCsc];

	    dr_11 -> Fill(dphi);
	    eta_11 -> Fill(deta);
          }

	  if (evt.trLctStation[iCSCTrk][iCsc] == 2 && evt.rpc_Station[iRpc] == 2) {

	    float dphi = evt.rpc_gblPhi[iRpc]-evt.trLctglobalPhi[iCSCTrk][iCsc];
            float deta = evt.rpc_gblEta[iRpc]-evt.trLctglobalEta[iCSCTrk][iCsc];

            dr_22 -> Fill(dphi);
	    eta_22 -> Fill(deta);
          }

	  if (evt.trLctStation[iCSCTrk][iCsc] == 3 && evt.rpc_Station[iRpc] == 3) {

	    float dphi = evt.rpc_gblPhi[iRpc]-evt.trLctglobalPhi[iCSCTrk][iCsc];
            float deta = evt.rpc_gblEta[iRpc]-evt.trLctglobalEta[iCSCTrk][iCsc];

            dr_33 -> Fill(dphi);
	    eta_33 -> Fill(deta);
          }

	} // end loop over csc hits

	// fill scatter plot
	if (is12 && is23) {
	  dphi_123   -> Fill(dphi12, dphi23);
	  dphi_12_pt -> Fill(dphi12, evt.PtTrk[iCSCTrk]);
	  dphi_23_pt -> Fill(dphi23, evt.PtTrk[iCSCTrk]);
	}

      } // end loop pver rpc hits

    } // end loop over tracks

  } // end loop over events



  // ====== Make PLots =======

  TCanvas* c1 = new TCanvas("c1", "", 420, 500, 600, 600);
  c1 -> Divide(1,2);
  c1 -> cd(1);
  dr_12 -> GetXaxis() -> SetTitle("RPC Phi - CSC Phi");
  dr_12 -> GetXaxis() -> SetTitleOffset(1.5);
  dr_12 -> GetYaxis() -> SetTitle("Count");
  dr_12 -> GetYaxis() -> SetTitleOffset(1.5);
  dr_12 -> Draw();
  dr_12 -> SetStats(0);

  c1 ->cd(2);
  eta_12 -> GetXaxis() -> SetTitle("RPC Eta  - CSC Eta");
  eta_12 -> GetXaxis() -> SetTitleOffset(1.5);
  eta_12 -> GetYaxis() -> SetTitle("Count");
  eta_12 -> GetYaxis() -> SetTitleOffset(1.5);
  eta_12 -> Draw();
  eta_12 -> SetStats(0);
  
  TCanvas* c2 = new TCanvas("c2", "", 420, 500, 600, 600);
  c2 -> Divide(1,2);
  c2 -> cd(1);
  dr_23 -> GetXaxis() -> SetTitle("RPC Phi - CSC Phi");
  dr_23 -> GetXaxis() -> SetTitleOffset(1.5);
  dr_23 -> GetYaxis() -> SetTitle("Count");
  dr_23 -> GetYaxis() -> SetTitleOffset(1.5);
  dr_23 -> Draw();
  dr_23 -> SetStats(0);
 
  c2 -> cd(2);
  eta_23 -> GetXaxis() -> SetTitle("RPC Eta  - CSC Eta");
  eta_23 -> GetXaxis() -> SetTitleOffset(1.5);
  eta_23 -> GetYaxis() -> SetTitle("Count");
  eta_23 -> GetYaxis() -> SetTitleOffset(1.5);
  eta_23 -> Draw();
  eta_23 -> SetStats(0);
  c2->Update();

  TCanvas* c3 = new TCanvas("c3", "", 420, 500, 600, 600);
  c3 -> Divide(1,2);
  c3 -> cd(1);
  dr_11 -> GetXaxis() -> SetTitle("RPC Phi - CSC Phi");
  dr_11 -> GetXaxis() -> SetTitleOffset(1.5);
  dr_11 -> GetYaxis() -> SetTitle("Count");
  dr_11 -> GetYaxis() -> SetTitleOffset(1.5);
  dr_11 -> Draw();
  dr_11 -> SetStats(0);

  c3 ->cd(2);
  eta_11 -> GetXaxis() -> SetTitle("RPC Eta  - CSC Eta");
  eta_11 -> GetXaxis() -> SetTitleOffset(1.5);
  eta_11 -> GetYaxis() -> SetTitle("Count");
  eta_11 -> GetYaxis() -> SetTitleOffset(1.5);
  eta_11 -> Draw();
  eta_11 -> SetStats(0);

  TCanvas* c4 = new TCanvas("c4", "", 420, 500, 600, 600);
  c4 -> Divide(1,2);
  c4 -> cd(1);
  dr_22 -> GetXaxis() -> SetTitle("RPC Phi - CSC Phi");
  dr_22 -> GetXaxis() -> SetTitleOffset(1.5);
  dr_22 -> GetYaxis() -> SetTitle("Count");
  dr_22 -> GetYaxis() -> SetTitleOffset(1.5);
  dr_22 -> Draw();
  dr_22 -> SetStats(0);

  c4 ->cd(2);
  eta_22 -> GetXaxis() -> SetTitle("RPC Eta  - CSC Eta");
  eta_22 -> GetXaxis() -> SetTitleOffset(1.5);
  eta_22 -> GetYaxis() -> SetTitle("Count");
  eta_22 -> GetYaxis() -> SetTitleOffset(1.5);
  eta_22 -> Draw();
  eta_22 -> SetStats(0);

  TCanvas* c5 = new TCanvas("c5", "", 420, 500, 600, 600);
  c5 -> Divide(1,2);
  c5 -> cd(1);
  dr_33 -> GetXaxis() -> SetTitle("RPC Phi  - CSC Phi");
  dr_33 -> GetXaxis() -> SetTitleOffset(1.5);
  dr_33 -> GetYaxis() -> SetTitle("Count");
  dr_33 -> GetYaxis() -> SetTitleOffset(1.5);
  dr_33 -> Draw();
  dr_33 -> SetStats(0);

  c5 ->cd(2);
  eta_33 -> GetXaxis() -> SetTitle("RPC Eta  - CSC Eta");
  eta_33 -> GetXaxis() -> SetTitleOffset(1.5);
  eta_33 -> GetYaxis() -> SetTitle("Count");
  eta_33 -> GetYaxis() -> SetTitleOffset(1.5);
  eta_33 -> Draw();
  eta_33 -> SetStats(0);

  
  TCanvas* c6 = new TCanvas("c6", "", 420, 500, 600, 600);
  dphi_123 -> GetXaxis() -> SetTitle("RPC2 Phi - CSC1 Phi");
  dphi_123 -> GetXaxis() -> SetTitleOffset(1.5);
  dphi_123 -> GetYaxis() -> SetTitle("RPC2 Phi - CSC3 Phi");
  dphi_123 -> GetYaxis() -> SetTitleOffset(1.5);
  dphi_123 -> Draw("CONT");
  dphi_123 -> SetStats(0);
  
  TCanvas* c7 = new TCanvas("c7", "", 420, 500, 600, 600);
  c7 -> Divide(1,2);
  c7 -> cd(1);
  dphi_12_pt -> GetXaxis() -> SetTitle("RPC2 Phi - CSC1 Phi");
  dphi_12_pt -> GetXaxis() -> SetTitleOffset(1.5);
  dphi_12_pt -> GetYaxis() -> SetTitle("CSCTF Pt GeV");
  dphi_12_pt -> GetYaxis() -> SetTitleOffset(1.5);
  dphi_12_pt -> Draw("CONT");
  dphi_12_pt -> SetStats(0);
  c7 -> cd(2);
  //  TCanvas* c8 = new TCanvas("c8", "", 420, 500, 600, 600);
  dphi_23_pt -> GetXaxis() -> SetTitle("RPC2 Phi - CSC3 Phi");
  dphi_23_pt -> GetXaxis() -> SetTitleOffset(1.5);
  dphi_23_pt -> GetYaxis() -> SetTitle("CSCTF Pt GeV");
  dphi_23_pt -> GetYaxis() -> SetTitleOffset(1.5);
  dphi_23_pt -> Draw("CONT");
  dphi_23_pt -> SetStats(0);

} // end main

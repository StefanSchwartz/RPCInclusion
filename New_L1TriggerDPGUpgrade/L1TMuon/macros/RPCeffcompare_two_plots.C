

// =======================================================================
// plots.C   Takes the saved root file from csctf_rate.C and makes plots
//
// by David Curry
//
// 30.04.2014
// =======================================================================

#include "TFile.h"
#include "TTree.h"
#include <vector>
#include <string>
#include <iostream>
#include <fstream>

#include <TROOT.h>
#include <TChain.h>
#include <TFile.h>
#include <TTree.h>
#include <TFriendElement.h>
#include <TList.h>
#include <TMatrix.h>
#include <TH1D.h>
#include <TGraphAsymmErrors.h>
#include <TLine.h>
#include "TMath.h"
#include <TH1F.h>
#include <TH2D.h>
#include <TH2F.h>
#include <TCanvas.h>
#include <TLegend.h>
#include <TLatex.h>


void RPCeffcompare_two_plots (){
  
  TFile *file = TFile::Open("RPCeffcompare_two.root");
  
  /*  
  TH2F* hdeta12_pt = (TH2F*) file->Get("hdeta12_pt");
  TH2F* hdeta23_pt = (TH2F*) file->Get("hdeta23_pt");
  
  TCanvas* c1 = new TCanvas("c1", "", 420, 500, 600, 600);
  c1 -> Divide(1,2);
  c1 -> cd(1);
  hdeta12_pt -> SetMarkerStyle(kFullDotSmall);
  hdeta12_pt -> SetMarkerColor(kBlue);
  hdeta12_pt -> GetXaxis() -> SetTitle("dEta: CSC1-CSC2");
  hdeta12_pt -> GetXaxis() -> SetTitleOffset(1.5);
  hdeta12_pt -> GetYaxis() -> SetTitle("Pt");
  hdeta12_pt -> GetYaxis() -> SetRange(0,30);
  hdeta12_pt -> Draw();
  hdeta12_pt -> SetStats(0);
  c1->Update();
  
  c1 ->cd(2);
  hdeta23_pt -> SetMarkerStyle(kFullDotSmall);
  hdeta23_pt -> SetMarkerColor(kBlue);
  hdeta23_pt -> GetXaxis() -> SetTitle("dEta: CSC2-CSC3");
  hdeta23_pt -> GetXaxis() -> SetTitleOffset(1.5);
  hdeta23_pt -> GetYaxis() -> SetTitle("Pt");
  hdeta23_pt -> Draw();
  hdeta23_pt -> SetStats(0);
  
  c1->SaveAs("deta_pt_csc.png");


  
  TH2F* hdphi12_pt = (TH2F*) file->Get("hdphi12_pt");
  TH2F* hdphi23_pt = (TH2F*) file->Get("hdphi23_pt");

  TCanvas* c3 = new TCanvas("c3", "", 420, 500, 600, 600);
  c3 -> Divide(1,2);
  c3 -> cd(1);
  hdphi12_pt -> SetMarkerStyle(kFullDotSmall);
  hdphi12_pt -> SetMarkerColor(kBlue);
  hdphi12_pt -> GetXaxis() -> SetTitle("dPhi: CSC1-CSC2");
  hdphi12_pt -> GetXaxis() -> SetTitleOffset(1.5);
  hdphi12_pt -> GetYaxis() -> SetTitle("Pt");
  hdphi12_pt -> Draw();
  hdphi12_pt -> SetStats(0);

  c3 ->cd(2);
  hdphi23_pt -> SetMarkerStyle(kFullDotSmall);
  hdphi23_pt -> SetMarkerColor(kBlue);
  hdphi23_pt -> GetXaxis() -> SetTitle("dPhi: CSC2-CSC3");
  hdphi23_pt -> GetXaxis() -> SetTitleOffset(1.5);
  hdphi23_pt -> GetYaxis() -> SetTitle("Pt");
  hdphi23_pt -> Draw();
  hdphi23_pt -> SetStats(0);
  
  c3->SaveAs("dphi_pt_csc.png");

  
  TH2F* hdphi12_r_pt = (TH2F*) file->Get("hdphi12_r_pt");
  TH2F* hdphi23_r_pt = (TH2F*) file->Get("hdphi23_r_pt");

  TCanvas* c2 = new TCanvas("c2", "", 420, 500, 600, 600);
  c2 -> Divide(1,2);
  c2 -> cd(1);
  hdphi12_r_pt -> SetMarkerStyle(kFullDotSmall);
  hdphi12_r_pt -> SetMarkerColor(kBlue);
  hdphi12_r_pt -> GetXaxis() -> SetTitle("dPhi: CSC1-RPC2");
  hdphi12_r_pt -> GetXaxis() -> SetTitleOffset(1.5);
  hdphi12_r_pt -> GetYaxis() -> SetTitle("Pt");
  hdphi12_r_pt -> Draw();
  hdphi12_r_pt -> SetStats(0);

  c2 ->cd(2);
  hdphi23_r_pt -> SetMarkerStyle(kFullDotSmall);
  hdphi23_r_pt -> SetMarkerColor(kBlue);
  hdphi23_r_pt -> GetXaxis() -> SetTitle("dPhi: RPC2-CSC3");
  hdphi23_r_pt -> GetXaxis() -> SetTitleOffset(1.5);
  hdphi23_r_pt -> GetYaxis() -> SetTitle("Pt");
  hdphi23_r_pt -> Draw();
  hdphi23_r_pt -> SetStats(0);

  c2->SaveAs("dphi_pt_rpc.png");



  TH2F* hdeta12_r_pt = (TH2F*) file->Get("hdeta12_r_pt");
  TH2F* hdeta23_r_pt = (TH2F*) file->Get("hdeta23_r_pt");

  TCanvas* c4 = new TCanvas("c4", "rpc eta", 420, 500, 600, 600);
  c4 -> Divide(1,2);
  c4 -> cd(1);
  hdeta12_r_pt -> SetMarkerStyle(kFullDotSmall);
  hdeta12_r_pt -> SetMarkerColor(kBlue);
  hdeta12_r_pt -> GetXaxis() -> SetTitle("dEta: CSC1-RPC2");
  hdeta12_r_pt -> GetXaxis() -> SetTitleOffset(1.5);
  hdeta12_r_pt -> GetYaxis() -> SetTitle("Pt");
  hdeta12_r_pt -> Draw();
  hdeta12_r_pt -> SetStats(0);

  c4 -> cd(2);
  hdeta23_r_pt -> SetMarkerStyle(kFullDotSmall);
  hdeta23_r_pt -> SetMarkerColor(kBlue);
  hdeta23_r_pt -> GetXaxis() -> SetTitle("dEta: RPC2-CSC3");
  hdeta23_r_pt -> GetXaxis() -> SetTitleOffset(1.5);
  hdeta23_r_pt -> GetYaxis() -> SetTitle("Pt");
  hdeta23_r_pt -> Draw();
  hdeta23_r_pt -> SetStats(0);

  c4->SaveAs("deta_pt_rpc.png");

  TH2F* hdphi12_all = (TH2F*) file->Get("hdphi12_all");
  TH2F* hdphi23_all = (TH2F*) file->Get("hdphi23_all");  
  
  TCanvas* c5 = new TCanvas("c5", "rpc eta", 420, 500, 600, 600);
  
  c5 -> Divide(1,2);
  c5 -> cd(1);
  hdphi12_all -> SetMarkerStyle(kFullDotSmall);
  hdphi12_all -> SetMarkerColor(kBlue);
  hdphi12_all -> GetXaxis() -> SetTitle("dEta: CSC1-RPC2");
  hdphi12_all -> GetXaxis() -> SetTitleOffset(1.5);
  hdphi12_all -> GetYaxis() -> SetTitle("Pt");
  hdphi12_all -> Draw();
  hdphi12_all -> SetStats(0);

  c5 -> cd(2);
  hdphi23_all -> SetMarkerStyle(kFullDotSmall);
  hdphi23_all -> SetMarkerColor(kBlue);
  hdphi23_all -> GetXaxis() -> SetTitle("dEta: RPC2-CSC3");
  hdphi23_all -> GetXaxis() -> SetTitleOffset(1.5);
  hdphi23_all -> GetYaxis() -> SetTitle("Pt");
  hdphi23_all -> Draw();
  hdphi23_all -> SetStats(0);

  c5->SaveAs("dphi12_csc_all.png");
  */

  TH1F* hdphi1 = (TH1F*) file->Get("hdphi1");
  TH1F* hdphi2 = (TH1F*) file->Get("hdphi2");
  TH1F* hdphi3 = (TH1F*) file->Get("hdphi3");
  
  TCanvas* c6 = new TCanvas("c6", "", 420, 500, 600, 600);
  hdphi1-> GetXaxis() -> SetTitle("Delta Phi(radians)");
  hdphi1-> GetYaxis() -> SetTitle("count");
  hdphi1 -> Draw();
  hdphi1 -> SetStats(0);
  
  c6->SaveAs("dphi1_data.png");
  
  TCanvas* c7 = new TCanvas("c7", "", 420, 500, 600, 600);
  hdphi2-> GetXaxis() -> SetTitle("Delta Phi(radians)");
  hdphi2-> GetYaxis() -> SetTitle("count");
  hdphi2 -> Draw();
  hdphi2 -> SetStats(0);

  c7->SaveAs("dphi2_data.png");

  TCanvas* c8 = new TCanvas("c8", "", 420, 500, 600, 600);
  hdphi3-> GetXaxis() -> SetTitle("Delta Phi(radians)");
  hdphi3-> GetYaxis() -> SetTitle("count");
  hdphi3 -> Draw();
  hdphi3 -> SetStats(0);

  c8->SaveAs("dphi3_data.png");

  /*
  TH1F* hdphi1_pt = (TH1F*) file->Get("hdphi1_pt");
  TH1F* hdphi2_pt = (TH1F*) file->Get("hdphi2_pt");
  TH1F* hdphi3_pt = (TH1F*) file->Get("hdphi3_pt");

  TCanvas* c9 = new TCanvas("c9", "", 420, 500, 600, 600);
  hdphi1_pt -> SetMarkerStyle(kFullDotSmall);
  hdphi1_pt -> SetMarkerColor(kBlue);
  hdphi1_pt-> GetXaxis() -> SetTitle("Delta Phi(radians)");
  hdphi1_pt-> GetYaxis() -> SetTitle("count");
  hdphi1_pt -> Draw();
  hdphi1_pt -> SetStats(0);

  c9->SaveAs("dphi1_pt.png");

  TCanvas* c10 = new TCanvas("c10", "", 420, 500, 600, 600);
  hdphi2_pt -> SetMarkerStyle(kFullDotSmall);
  hdphi2_pt -> SetMarkerColor(kBlue);
  hdphi2_pt-> GetXaxis() -> SetTitle("Delta Phi(radians)");
  hdphi2_pt-> GetYaxis() -> SetTitle("count");
  hdphi2_pt -> Draw();
  hdphi2_pt -> SetStats(0);

  c10->SaveAs("dphi2_pt.png");

  TCanvas* c11 = new TCanvas("c11", "", 420, 500, 600, 600);
  hdphi3_pt -> SetMarkerStyle(kFullDotSmall);
  hdphi3_pt -> SetMarkerColor(kBlue);
  hdphi3_pt-> GetXaxis() -> SetTitle("Delta Phi(radians)");
  hdphi3_pt-> GetYaxis() -> SetTitle("count");
  hdphi3_pt -> Draw();
  hdphi3_pt -> SetStats(0);

  c11->SaveAs("dphi3_pt.png");  
  */

}





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
#include <TF1.h>
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
#include <TProfile.h>

void twoHit_RPC_plots (){

  TFile *file     = TFile::Open("twoHit_RPC_100.root");
  //TFile *file_mc  = TFile::Open("twoHit_RPC_mc.root");
  TFile *file_rpc = TFile::Open("twoHit_RPC_pt.root");
  
  /*  
  TH1F* hpt12_inv = (TH1F*) file->Get("hpt12_inv");
  
  TCanvas* c1 = new TCanvas("c1", "", 420, 500, 600, 600);
  hpt12_inv -> Smooth();
  hpt12_inv -> SetFillColor(kBlue);
  hpt12_inv -> Draw();
  hpt12_inv -> SetStats(0);
  hpt12_inv -> SetTitle("Inv Pt: Stations CSC1-RPC2;Pt [GeV]");
    
  c1 -> SaveAs("pt.png");
  
  TH1F* hpt12 = (TH1F*) file->Get("hpt12");
  
  TCanvas* c2 = new TCanvas("c2", "", 420, 500, 600, 600);
  hpt12 -> Smooth();
  hpt12 -> SetFillColor(kBlue);
  hpt12 -> Draw();
  hpt12 -> SetTitle("Pt: Stations CSC1-RPC2;Pt [GeV]");
  hpt12 -> SetStats(0);
  */
  // ===================================================================

  TH1F* hdphi12 = (TH1F*) file->Get("hdphi12");
  TH1F* hdphi23 = (TH1F*) file->Get("hdphi23");
  TH1F* hdphi11 = (TH1F*) file->Get("hdphi11");
  TH1F* hdphi13 = (TH1F*) file->Get("hdphi13");
  
    
  TH1F* hdphi12_m = (TH1F*) file->Get("hdphi12_m");
  TH1F* hdphi23_m = (TH1F*) file->Get("hdphi23_m");
  TH1F* hdphi11_m = (TH1F*) file->Get("hdphi11_m");
  TH1F* hdphi13_m = (TH1F*) file->Get("hdphi13_m");
  
  TCanvas* c6 = new TCanvas("c6", "", 420, 500, 600, 600);

  c6 -> Divide(2,2);
  c6 -> cd(1);

  double norm    = 1./ hdphi12    -> Integral();
  double norm_m = 1./ hdphi12_m -> Integral();
  hdphi12    -> Scale(norm);
  hdphi12_m -> Scale(norm_m);
  
  hdphi12_m -> Smooth();
  //hdphi12 -> Smooth();
  hdphi12_m -> SetStats(0);
  hdphi12_m -> SetLineColor(kRed);
  //hdphi12 -> SetFillColor(kYellow);
  hdphi12_m -> Draw();
  hdphi12_m -> SetTitle("Delta Phi: RPC2-CSC1;Delta Phi[rad]");
  hdphi12 -> Draw("same");


  c6 -> cd(2);

  double norm2    = 1./ hdphi23    -> Integral();
  double norm2_m = 1./ hdphi23_m -> Integral();
  hdphi23    -> Scale(norm2);
  hdphi23_m -> Scale(norm2_m);
  
  hdphi23_m -> Draw();
  //hdphi23 -> Smooth();
  hdphi23_m -> SetStats(0);
  hdphi23_m ->SetLineColor(kRed);
  //hdphi23 -> SetFillColor(kYellow);
  hdphi23_m -> SetTitle("Delta Phi: RPC2-CSC3;Delta Phi[rad]");
  hdphi23 -> Draw("same");

  c6 -> cd(3);

  double norm3    = 1./ hdphi11    -> Integral();
  double norm3_m = 1./ hdphi11_m -> Integral();
  hdphi11    -> Scale(norm3);
  hdphi11_m -> Scale(norm3_m);
  
  
  hdphi11_m -> Draw();
  //hdphi11 -> Smooth();
  hdphi11_m -> SetStats(0);
  hdphi11_m ->SetLineColor(kRed);
  //hdphi11 -> SetFillColor(kYellow);
  hdphi11_m -> SetTitle("Delta Phi: RPC1-CSC1;Delta Phi[rad]");
  hdphi11 -> Draw("same");

  /*
  c6 -> cd(4);

  
  double norm4    = 1./ hdphi13    -> Integral();
  double norm4_m = 1./ hdphi13_m -> Integral();
  hdphi13    -> Scale(norm4);
  hdphi13_m -> Scale(norm4_m);
  

  hdphi13_m -> Draw();
  //hdphi13 -> Smooth();
  hdphi13_m -> SetStats(0);
  hdphi13_m ->SetLineColor(kRed);
  //hdphi13 -> SetFillColor(kYellow);
  hdphi13_m -> SetTitle("Delta Phi: RPC1-CSC3;Delta Phi[rad]");
  hdphi13    -> Draw("same");
  */

  c6 -> SaveAs("dphi_plus_minus.png");
      
  // =============================================================
    
  
  TH2F* hdphi12_pt    = (TH2F*) file->Get("hdphi12_pt");

  TH2F* hdphi12_invpt = (TH2F*) file->Get("hdphi12_invpt");
  TH2F* hdphi23_invpt = (TH2F*) file->Get("hdphi23_invpt");
  TH2F* hdphi11_invpt = (TH2F*) file->Get("hdphi11_invpt");
  TH2F* hdphi13_invpt = (TH2F*) file->Get("hdphi13_invpt");
  
  /*  
  TCanvas* c7 = new TCanvas("c7", "", 420, 500, 600, 600);  
  c7 -> Divide(2,2);
  
  c7 -> cd(1);
  hdphi12_invpt -> SetMarkerStyle(kFullDotSmall);
  hdphi12_invpt -> SetMarkerColor(kBlue);
  hdphi12_invpt -> SetStats(0);
  hdphi12_invpt -> SetTitle("Delta Phi: RPC2 - CSC1 vs Inv Pt;ad];Inv Pt [GeV]");
  //hdphi12_invpt -> Draw("col");

  TProfile *prof1 = hdphi12_invpt -> ProfileX();
  prof1 -> SetTitle("Delta Phi: RPC2 - CSC1 vs Inv Pt, Profile; Inv Pt[q/GeV]; Delta Phi[rad]");
  prof1 -> SetStats(0);
  prof1 -> Draw();

  c7 -> cd(2);
  hdphi23_invpt -> SetMarkerStyle(kFullDotSmall);
  hdphi23_invpt -> SetMarkerColor(kBlue);
  hdphi23_invpt -> SetStats(0);
  hdphi23_invpt -> SetTitle("Delta Phi: RPC2- CSC3 vs Inv Pt;Delta Phi[rad];Inv Pt [GeV]");
  //hdphi23_invpt -> Draw("col");

  TProfile *prof2 = hdphi23_invpt -> ProfileX();
  prof2 -> SetTitle("Delta Phi: RPC2 - CSC3 vs Inv Pt, Profile; Inv Pt[q/GeV]; Delta Phi[rad]");
  prof2 -> SetStats(0);
  prof2 -> Draw();

  
  c7 -> cd(3);
  hdphi11_invpt -> SetMarkerStyle(kFullDotSmall);
  hdphi11_invpt -> SetMarkerColor(kBlue);
  hdphi11_invpt -> SetStats(0);
  hdphi11_invpt -> SetTitle("Delta Phi: RPC1 - CSC1 vs Inv Pt;Delta Phi[rad];Inv Pt [GeV]");
  //hdphi11_invpt -> Draw("col");

  TProfile *prof3 = hdphi11_invpt -> ProfileX();
  prof3 -> SetTitle("Delta Phi: RPC1 - CSC1 vs Inv Pt, Profile; Inv Pt[q/GeV]; Delta Phi[rad]");
  prof3 -> SetStats(0);
  prof3 -> Draw();

  c7 -> cd(4);
  hdphi13_invpt -> SetMarkerStyle(kFullDotSmall);
  hdphi13_invpt -> SetMarkerColor(kBlue);
  hdphi13_invpt -> SetStats(0);
  hdphi13_invpt -> SetTitle("Delta Phi: RPC1 - CSC3 vs Inv Pt;Delta Phi[rad];Inv Pt [GeV]");
  //hdphi13_invpt -> Draw("col");

  TProfile *prof4 = hdphi13_invpt -> ProfileX();
  prof4 -> SetTitle("Delta Phi: RPC1 - CSC3 vs Inv Pt, Profile;Inv Pt[q/GeV]; Delta Phi[rad]");
  prof4 -> SetStats(0);
  prof4 -> Draw();


  c7 -> SaveAs("dphi12_pt.png");
  */  
  // =============================================================
  
  TCanvas* c9 = new TCanvas("c9", "", 420, 500, 600, 600);
  c9 -> Divide(2,2);

  c9 -> cd(1);
  hdphi12_invpt -> SetMarkerStyle(kFullDotSmall);
  hdphi12_invpt -> SetMarkerColor(kBlue);
  hdphi12_invpt -> SetStats(0);
  hdphi12_invpt -> SetTitle("RPC2-CSC1;Inv Pt [q/GeV];Delta Phi[rad]");
  hdphi12_invpt -> GetYaxis() -> SetTitleOffset(1.5);
  hdphi12_invpt -> Draw("col");
    
  c9 -> cd(2);
  hdphi23_invpt -> SetStats(0);
  hdphi23_invpt -> SetTitle("RPC2-CSC3;Inv Pt [q/GeV];Delta Phi[rad]");
  hdphi23_invpt -> GetYaxis() -> SetTitleOffset(1.5);
  hdphi23_invpt -> SetMarkerStyle(kFullDotSmall);
  hdphi23_invpt -> SetMarkerColor(kBlue);
  hdphi23_invpt -> Draw("col");

  c9 -> cd(3);
  hdphi11_invpt -> SetStats(0);
  hdphi11_invpt -> SetTitle("RPC1-CSC1;Inv Pt [q/GeV];Delta Phi[rad]");
  hdphi11_invpt -> GetYaxis() -> SetTitleOffset(1.5);
  hdphi11_invpt -> SetMarkerStyle(kFullDotSmall);
  hdphi11_invpt -> SetMarkerColor(kBlue);
  hdphi11_invpt -> Draw("col");
  
  /*
  c9 -> cd(4);
  hdphi13_invpt -> SetStats(0);
  hdphi13_invpt -> SetTitle("Delta Phi: RPC1 - CSC3 vs Inv Pt;Inv Pt [q/GeV];Delta Phi[rad]");
  hdphi13_invpt -> SetMarkerStyle(kFullDotSmall);
  hdphi13_invpt -> SetMarkerColor(kBlue);
  hdphi13_invpt -> Draw("col");
  */

  c9 -> SaveAs("dphi_invpt_data_temp_all.png");  
  
  
  // =============================================================

  // =============================================================
  /*  TH2F* hdphi12_invpt_m = (TH2F*) file->Get("hdphi12_invpt_m");
  TH2F* hdphi23_invpt_m = (TH2F*) file->Get("hdphi23_invpt_m");
  TH2F* hdphi11_invpt_m = (TH2F*) file->Get("hdphi11_invpt_m");
  TH2F* hdphi13_invpt_m = (TH2F*) file->Get("hdphi13_invpt_m");

  TCanvas* c12 = new TCanvas("c12", "", 420, 500, 600, 600);
  c12 -> Divide(2,2);

  c12 -> cd(1);
  hdphi12_invpt_m -> SetMarkerStyle(kFullDotSmall);
  hdphi12_invpt_m -> SetMarkerColor(kBlue);
  hdphi12_invpt_m -> SetStats(0);
  hdphi12_invpt_m -> SetTitle("Delta Phi: RPC2 - CSC1 vs Inv Pt;Inv Pt [-q/GeV];Delta Phi[rad]");
  hdphi12_invpt_m -> Draw("col");
    
  c12 -> cd(2);
  hdphi23_invpt_m -> SetStats(0);
  hdphi23_invpt_m -> SetTitle("Delta Phi: RPC2 - CSC3 vs Inv Pt;Inv Pt [-q/GeV];Delta Phi[rad]");
  hdphi23_invpt_m -> SetMarkerStyle(kFullDotSmall);
  hdphi23_invpt_m -> SetMarkerColor(kBlue);
  hdphi23_invpt -> Draw("col");

  c12 -> cd(3);
  hdphi11_invpt_m -> SetStats(0);
  hdphi11_invpt_m -> SetTitle("Delta Phi: RPC1 - CSC1 vs Inv Pt;Inv Pt [-q/GeV];Delta Phi[rad]");
  hdphi11_invpt_m -> SetMarkerStyle(kFullDotSmall);
  hdphi11_invpt_m -> SetMarkerColor(kBlue);
  hdphi11_invpt_m -> Draw("col");
  
  c12 -> cd(4);
  hdphi13_invpt_m -> SetStats(0);
  hdphi13_invpt_m -> SetTitle("Delta Phi: RPC1 - CSC3 vs Inv Pt;Inv Pt [-q/GeV];Delta Phi[rad]");
  hdphi13_invpt_m-> SetMarkerStyle(kFullDotSmall);
  hdphi13_invpt_m -> SetMarkerColor(kBlue);
  hdphi13_invpt_m -> Draw("col");

  c12 -> SaveAs("dphi_invpt_data_temp_all_minus.png");  

  */
  // ==================================================================================================

  
  TH2F* hdphi12_csc_invpt = (TH2F*) file -> Get("hdphi12_csc_invpt");

  TH1F* hdphi12_csc   = (TH1F*) file    -> Get("hdphi12_csc");
  TH1F* hdphi12_csc_m = (TH1F*) file -> Get("hdphi12_csc_m");
  
  TCanvas* c8 = new TCanvas("c8", "", 420, 500, 600, 600);
  
  c8 -> Divide(1,3);
  
  c8 -> cd(1);
  hdphi12_csc -> SetTitle("Delta Phi: CSC1 - CSC2;Delta Phi[rad]");
  //hdphi12_csc -> Smooth();
  hdphi12_csc -> SetLineColor(kRed);
  hdphi12_csc -> Draw();
  hdphi12_csc -> SetStats(0);
  hdphi12_csc_m -> Draw("same");



  c8 -> cd(2);
  hdphi12_csc_invpt -> SetTitle("Inv Pt vs Delta Phi: CSC1 - CSC2;Inv Pt[q/GeV];Delta Phi[rad]");
  hdphi12_csc_invpt -> Draw("col");
  hdphi12_csc_invpt -> SetStats(0);

  c8 -> cd(3);
  TProfile *prof = hdphi12_csc_invpt -> ProfileX();
  prof -> SetTitle("Inv Pt vs Delta Phi: CSC1 - CSC2, Profile;Inv Pt[q/GeV];Delta Phi[rad]");
  prof -> SetStats(0);
  prof -> Draw();
  
  // =============================================================
  /*
  TH1F* hdphi23_csc       = (TH1F*) file -> Get("hdphi23_csc");
  TH2F* hdphi23_csc_invpt = (TH2F*) file -> Get("hdphi23_csc_invpt");
  TH1F* hdphi23_csc_m     = (TH1F*) file -> Get("hdphi23_csc_m");
  
  TCanvas* c8 = new TCanvas("c8", "", 420, 500, 600, 600);
  
  c8 -> Divide(1,3);

  c8 -> cd(1);
  hdphi23_csc -> SetTitle("Delta Phi: CSC2 - CSC3;Delta Phi[rad]");
  //hdphi12_csc -> Smooth();
  hdphi23_csc -> SetLineColor(kRed);
  hdphi23_csc_m -> Draw();
  hdphi23_csc -> SetStats(0);
  hdphi23_csc -> Draw("same");
  
  c8 -> cd(2);
  hdphi23_csc_invpt -> SetTitle("Delta Phi: CSC2 - CSC3 vs Inv Pt; Inv Pt[q/GeV]; Delta Phi[rad]");
  hdphi23_csc_invpt -> SetStats(0);
  hdphi23_csc_invpt -> Draw("col");
  
   
  c8 -> cd(3);
  TProfile *prof1 = hdphi23_csc_invpt -> ProfileX();
  prof1 -> SetTitle("Delta Phi: CSC2 - CSC3 vs Inv Pt, Profile;Inv Pt[q/GeV];Delta Phi[rad]");
  prof1 -> SetStats(0);
  prof1 -> Draw();
  */  
  
  // =============================================================

  /*
  TH1F* hdeta_csc_13 = (TH1F*) file_rpc -> Get("hdeta_csc_13");
  TH2F* hdeta_csc_13_invpt = (TH2F*) file_rpc -> Get("hdeta_csc_13_invpt");

  TCanvas* c20 = new TCanvas("c20", "", 420, 500, 600, 600);
  
  c20 -> Divide(1,2);  
  
  c20 -> cd(1);
  hdeta_csc_13 -> Draw();
  hdeta_csc_13 -> SetStats(0);
  hdeta_csc_13 -> SetTitle(" Delta Eta: CSC1 - CSC3; Delta Eta");
  
  c20 -> cd(2);
  hdeta_csc_13_invpt -> SetStats(0);
  hdeta_csc_13_invpt -> SetTitle(" Delta Eta: CSC1 - CSC3 vs InvPt; InvPt[Gev; Delta Eta");
  hdeta_csc_13_invpt -> Draw("col");
  
  c20 -> SaveAs("dphi_csc13_invpt.png");
  */
  // =============================================================
  /*
  TH2F* hdphi12_csc_pt = (TH2F*) file -> Get("hdphi12_csc_pt");
  
  TCanvas* c20 = new TCanvas("c20", "", 420, 500, 600, 600);

  TProfile *prof2 = hdphi12_csc_pt -> ProfileX();
  prof2 -> SetTitle("Delta Phi: CSC1 - CSC2 vs Pt, Profile;Delta Phi[rad]");
  prof2 -> SetStats(0);
  prof2 -> Draw();
  
  TF1 *fit = new TF1("fit", "[0]*x + [1]", 0, 0.2);

  fit -> SetParameter(0, 0.0);
  fit -> SetParameter(1, 0.0);

  //fit -> SetParLimits(0,1,10);
  //fit -> SetParLimits(1,1,10);

  prof2 -> Fit("fit", "r");
  
  prof2 -> GetFunction("fit") -> SetLineColor(kOrange+3);

  //fit -> Draw("Psame");
  
  c20 -> SaveAs("dphi12_csc_pt.png");
  */
  // =============================================================

  /*

  TH1F* hpt = (TH1F*) file->Get("hpt");
  
  TCanvas* c10 = new TCanvas("c10", "", 420, 500, 600, 600);

  hpt -> SetFillColor(kYellow);
  hpt -> SetStats(0);
  hpt -> SetTitle("Pt Distribution: Data;Pt[GeV]");
  hpt -> Draw();
  
  c10 -> SaveAs("pt_single_mu.png");
  */

  // =============================================================
  /*
  TH2F* hdphi12_invpt_abs = (TH2F*) file->Get("hdphi12_invpt_abs");

  TCanvas* c11 = new TCanvas("c11", "", 420, 500, 600, 600);

  c11 -> Divide(1,2);
  
  c11 ->cd(1);
  hdphi12_invpt_abs -> SetMarkerStyle(kFullDotSmall);
  hdphi12_invpt_abs -> SetMarkerColor(kBlue);
  hdphi12_invpt_abs -> SetStats(0);
  hdphi12_invpt_abs -> SetTitle("|Delta Phi|: RPC2 - CSC1 vs Inv Pt;Inv Pt [q/GeV];Delta Phi[rad]");
  hdphi12_invpt_abs -> Draw("col");

  c11 ->cd(2);
  TProfile *prof5 = hdphi12_invpt_abs -> ProfileX();
  prof5 -> SetTitle("|Delta Phi|: RPC2 - CSC1 vs Inv Pt, Profile;Inv Pt[q/GeV]; Delta Phi[rad]");
  prof5 -> SetStats(0);
  prof5 -> Draw();  
  
  c11 -> SaveAs("abs_dphi_pt_12.png");

  */

  // =============================================================
  /*
  TCanvas* c13 = new TCanvas("c13", "", 420, 500, 600, 600);
  c13 -> Divide(2,2);
  
  c13 -> cd(1);
  hdphi12_invpt_m -> SetMarkerStyle(kFullDotSmall);
  hdphi12_invpt_m -> SetMarkerColor(kRed);
  hdphi12_invpt_m -> SetStats(0);
  hdphi12_invpt_m -> SetTitle("Delta Phi: RPC2 - CSC1 vs Inv Pt;Inv Pt [-q/GeV];Delta Phi[rad]");
  hdphi12_invpt_m -> Draw();
  hdphi12_invpt   -> Draw("same");

  c13 -> cd(2);
  hdphi23_invpt_m -> SetStats(0);
  hdphi23_invpt_m -> SetTitle("Delta Phi: RPC2 - CSC3 vs Inv Pt;Inv Pt [-q/GeV];Delta Phi[rad]");
  hdphi23_invpt_m -> SetMarkerStyle(kFullDotSmall);
  hdphi23_invpt_m -> SetMarkerColor(kRed);
  hdphi23_invpt_m -> Draw();
  hdphi23_invpt   -> Draw("same");

  c13 -> cd(3);
  hdphi11_invpt_m -> SetStats(0);
  hdphi11_invpt_m -> SetTitle("Delta Phi: RPC1 - CSC1 vs Inv Pt;Inv Pt [-q/GeV];Delta Phi[rad]");
  hdphi11_invpt_m -> SetMarkerStyle(kFullDotSmall);
  hdphi11_invpt_m -> SetMarkerColor(kRed);
  hdphi11_invpt_m -> Draw();
  hdphi11_invpt   -> Draw("same");
  
  c13 -> cd(4);
  hdphi13_invpt_m -> SetStats(0);
  hdphi13_invpt_m -> SetTitle("Delta Phi: RPC1 - CSC3 vs Inv Pt;Inv Pt [-q/GeV];Delta Phi[rad]");
  hdphi13_invpt_m-> SetMarkerStyle(kFullDotSmall);
  hdphi13_invpt_m -> SetMarkerColor(kRed);
  hdphi13_invpt_m -> Draw();
  hdphi13_invpt   -> Draw("same");

  c13 -> SaveAs("dphi_invpt_data_temp_all_overlay.png");  
  */  
  // ====================================================================================

  /*
  TH1F* h1 = (TH1F*) file->Get("hdphi12_csc_bit");

  TCanvas* c14 = new TCanvas("c14", "", 420, 500, 600, 600);

  h1 -> SetFillColor(kYellow);
  h1 -> SetStats(0);
  h1 -> SetTitle("Dphi CSC bit; DPhi");
  h1 -> Draw();

  c14 -> SaveAs("dphi_bit_csc.png");  
  */

  /*  
  TH1F* hdphi12_m = (TH1F*) file->Get("hdphi12_m");
  TH1F* hdphi23_m = (TH1F*) file->Get("hdphi23_m");
  TH1F* hdphi11_m = (TH1F*) file->Get("hdphi11_m");
  TH1F* hdphi13_m = (TH1F*) file->Get("hdphi13_m");
  
  TCanvas* c14 = new TCanvas("c14", "", 420, 500, 600, 600);
  c14 -> Divide(2,2);
  
  c14 -> cd(1); 
  hdphi12_m -> SetStats(0);
  hdphi12_m -> SetLineColor(kRed);
  hdphi12_m -> Draw();
  hdphi12   -> Draw("same");

  hdphi23_m -> Draw();
  hdphi23_m -> SetStats(0);
  hdphi23_m -> SetLineColor(kRed);
  hdphi23   -> Draw("same");

  c14 -> cd(3);

  hdphi11_m -> Draw();
  hdphi11_m -> SetStats(0);
  hdphi11_m -> SetLineColor(kRed);
  hdphi11   -> Draw("same");

  c14 -> cd(4);

  hdphi13_m -> Draw();
  hdphi13_m -> SetStats(0);
  hdphi13_m -> SetLineColor(kRed);
  hdphi13   -> Draw("same");

  c14 -> SaveAs("dphi_plus_minus.png");
  */
  // =============================================================


  /*  
  TH2F* hdeta12_pt = (TH2F*) file->Get("hdeta12_pt");
  TH2F* hdeta23_pt = (TH2F*) file->Get("hdeta23_pt");
  
  TCanvas* c1 = new TCanvas("c1", "", 420, 500, 600, 600);
  c1 -> Divide(1,2);
  c1 -> cd(1);
  hdeta12_pt -> SetMarkerStyle(kFullDotSmall);
  hdeta12_pt -> SetMarkerColor(kBlue);
  hdeta12_pt -> GetXaxis() -> SetTitle("dEta: RPC2-CSC1");
  hdeta12_pt -> GetXaxis() -> SetTitleOffset(1.5);
  hdeta12_pt -> GetYaxis() -> SetTitle("Pt");
  hdeta12_pt -> GetYaxis() -> SetRange(0,30);
  hdeta12_pt -> Draw();
  hdeta12_pt -> SetStats(0);
  c1->Update();

  c1 ->cd(2);
  hdeta23_pt -> SetMarkerStyle(kFullDotSmall);
  hdeta23_pt -> SetMarkerColor(kBlue);
  hdeta23_pt -> GetXaxis() -> SetTitle("dEta: RPC2-CSC3");
  hdeta23_pt -> GetXaxis() -> SetTitleOffset(1.5);
  hdeta23_pt -> GetYaxis() -> SetTitle("Pt");
  hdeta23_pt -> Draw();
  hdeta23_pt -> SetStats(0);

  c1->SaveAs("two_hit_deta_pt.png");

  
  TH2F* hdphi12_pt = (TH2F*) file->Get("hdphi12_pt");
  TH2F* hdphi23_pt = (TH2F*) file->Get("hdphi23_pt");

  TCanvas* c3 = new TCanvas("c3", "", 420, 500, 600, 600);
  c3 -> Divide(1,2);
  c3 -> cd(1);
  hdphi12_pt -> SetMarkerStyle(kFullDotSmall);
  hdphi12_pt -> SetMarkerColor(kBlue);
  hdphi12_pt -> GetXaxis() -> SetTitle("dPhi: RPC2-CSC1");
  hdphi12_pt -> GetXaxis() -> SetTitleOffset(1.5);
  hdphi12_pt -> GetYaxis() -> SetTitle("Pt");
  hdphi12_pt -> Draw();
  hdphi12_pt -> SetStats(0);

  c3 ->cd(2);
  hdphi23_pt -> SetMarkerStyle(kFullDotSmall);
  hdphi23_pt -> SetMarkerColor(kBlue);
  hdphi23_pt -> GetXaxis() -> SetTitle("dPhi: CSC2-RPC3");
  hdphi23_pt -> GetXaxis() -> SetTitleOffset(1.5);
  hdphi23_pt -> GetYaxis() -> SetTitle("Pt");
  hdphi23_pt -> Draw();
  hdphi23_pt -> SetStats(0);

  c3->SaveAs("two_hit_dphi_pt.png");

  
  TH2F* hdeta13_pt_csc = (TH2F*) file->Get("hdeta13_pt_csc");
  TH2F* hdphi13_pt_csc = (TH2F*) file->Get("hdphi13_pt_csc");
  
  TCanvas* c2 = new TCanvas("c2", "", 420, 500, 600, 600);
  c2 -> Divide(1,2);
  c2 -> cd(1);
  hdeta13_pt_csc -> SetMarkerStyle(kFullDotSmall); 
  hdeta13_pt_csc -> SetMarkerColor(kBlue);
  hdeta13_pt_csc -> GetXaxis() -> SetTitle("dEta: CSC1-CSC3");
  hdeta13_pt_csc -> GetXaxis() -> SetTitleOffset(1.5);
  hdeta13_pt_csc -> GetYaxis() -> SetTitle("Pt");
  hdeta13_pt_csc -> GetYaxis() -> SetRange(0,30);
  hdeta13_pt_csc -> Draw();
  hdeta13_pt_csc -> SetStats(0);
  c2->Update();

  c2 ->cd(2);
  hdphi13_pt_csc -> SetMarkerStyle(kFullDotSmall);
  hdphi13_pt_csc -> SetMarkerColor(kBlue);
  hdphi13_pt_csc -> GetXaxis() -> SetTitle("dPhi: CSC1-CSC3");
  hdphi13_pt_csc -> GetXaxis() -> SetTitleOffset(1.5);
  hdphi13_pt_csc -> GetYaxis() -> SetTitle("Pt");
  hdphi13_pt_csc -> Draw();
  hdphi13_pt_csc -> SetStats(0);

  c2->SaveAs("two_hit_dcsc.png");

  */
  // =====================================================================
  
  /*
  TH1F* hdpt = (TH1F*) file->Get("hdpt");
  
  TCanvas* c4 = new TCanvas("c4", "", 420, 500, 600, 600);
    
  hdpt -> GetXaxis() -> SetTitle("RPC - CSC Pt");
  hdpt -> GetXaxis() -> SetTitleOffset(1.5);
  hdpt -> GetYaxis() -> SetTitle("count");
  hdpt -> Draw();
  hdpt -> SetStats(0);
  
  c4->SaveAs("two_hit_dpt.png");
  */
  // ========================================================================

  /*
  TH1F* hrate_r = (TH1F*) file_rpc->Get("hrate_r");
  TH1F* hrate_t = (TH1F*) file_rpc->Get("hrate_t");

  TCanvas* c5 = new TCanvas("c5", "", 420, 500, 600, 600);

  //c5 -> SetLogy();
  hrate_t -> SetLineColor(kRed);
  hrate_t -> SetStats(0);
  hrate_t -> SetTitle("CSCTF Rate; Pt[GeV]");  
  hrate_t -> Draw();

  hrate_r -> Draw("same");
  
  c5 -> BuildLegend();

  c5 -> SaveAs("csctf_rate.png");
  
  // ======================================================================
  
  
  TH1F* hres_r = (TH1F*) file_rpc->Get("hres_r");
  TH1F* hres_t = (TH1F*) file_rpc->Get("hres_t");

  TCanvas* c6 = new TCanvas("c6", "", 420, 500, 600, 600);
  
  hres_t -> SetLineColor(kRed);
  hres_t -> SetStats(0);
  hres_t -> SetTitle("CSCTF Resoltuion; (CSCTF Pt - Gbl Muon Pt)/ Gbl Muon Pt");
  hres_t -> Draw();

  hres_r -> Draw("same");

  //c6 -> BuildLegend();
  TLegend *leg = new TLegend(0.1,0.7,0.48,0.9);
  leg -> SetFillStyle(0);
  leg -> AddEntry(hres_t, "CSCTF",  "l");
  leg -> AddEntry(hres_r, "CSCTF + RPC",  "l");
  leg -> Draw("same");


  c6 -> SaveAs("csctf_res.png");
  */
  // ===================================================================


  /*
  TH1F* hdphi12 = (TH1F*) file->Get("hdphi12");
  TH1F* hdphi23 = (TH1F*) file->Get("hdphi23");
  TH1F* hdphi11 = (TH1F*) file->Get("hdphi11");

  TCanvas* c6 = new TCanvas("c6", "", 420, 500, 600, 600);
  
  c6 -> Divide(1,3);
  c6 -> cd(1);
  hdphi12 -> SetStats(0);
  hdphi12 -> Draw();
  
  c6 -> cd(2);
  hdphi23 -> SetStats(0);
  hdphi23 -> Draw();
  
  c6 -> cd(3);
  hdphi11 -> SetStats(0);
  hdphi11 -> Draw();
  
  c6 -> SaveAs("dphi_rpc_csc.png");
  */  
  

  
  TH1F* hmode = (TH1F*) file->Get("hmode");
    
  TCanvas* c10 = new TCanvas("c7", "", 420, 500, 600, 600);

  hmode -> SetTitle("Two Hit CSCTF Modes; Mode");
  hmode -> SetStats(0);
  hmode -> SetBarOffset(-0.7);
  hmode -> Draw("bar2");


  c10 -> SaveAs("twohit_mode.png");
  
}

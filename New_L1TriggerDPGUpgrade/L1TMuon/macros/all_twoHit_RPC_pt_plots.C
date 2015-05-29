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
#include "TStyle.h"

void all_twoHit_RPC_pt_plots (){

  TFile *file = TFile::Open("all_twoHit_RPC_pt.root");

  //gStyle->SetLabelSize(0.030, "xy");
  //gStyle->SetTitleSize(0.04, "xy");
  //gStyle->SetTitleOffset(1.4, "xy");
  
  // ===== Start PLots ======

  // ===========================================================================

  TH1F* hrate_r = (TH1F*) file->Get("hrate_r");
  TH1F* hrate_t = (TH1F*) file->Get("hrate_t");
  TH1F* hrate_r_mode2 = (TH1F*) file->Get("hrate_r_mode2");
  TH1F* hrate_r_mode4 = (TH1F*) file->Get("hrate_r_mode4");
  TH1F* hrate_t_mode2 = (TH1F*) file->Get("hrate_t_mode2");
  TH1F* hrate_t_mode4 = (TH1F*) file->Get("hrate_t_mode4");
  TH1F* hrate_t_mode1 = (TH1F*) file->Get("hrate_t_mode1");
  TH1F* hrate_r_mode1 = (TH1F*) file->Get("hrate_r_mode1");
  TH1F* hrate_t_mode5 = (TH1F*) file->Get("hrate_t_mode5");
  TH1F* hrate_r_mode5 = (TH1F*) file->Get("hrate_r_mode5");

  TCanvas* c5 = new TCanvas("c5", "", 420, 500, 600, 600);
  
  c5 ->Divide(2,2);

  /*  c5 ->cd(1);
  c5 -> SetLogy();  
  hrate_t -> SetLineColor(kRed);
  hrate_t -> SetStats(0);
  hrate_t -> SetTitle("CSCTF Rate; Pt[GeV]");
  hrate_t -> Draw();
  hrate_r -> Draw("same");
  */


  c5 ->cd(1);
  hrate_r_mode2 -> SetStats(0);
  hrate_r_mode2 -> SetTitle("CSCTF Rate Mode 2; Pt[GeV]");
  hrate_r_mode2 -> Draw();
  hrate_t_mode2 -> SetLineColor(kRed);
  hrate_t_mode2 -> Draw("same");

  TLegend *leg1 = new TLegend(0.1,0.7,0.48,0.9);
  leg1 -> SetFillStyle(0);
  leg1 -> AddEntry(hrate_t_mode2, "CSCTF",  "l");
  leg1 -> AddEntry(hrate_r, "CSCTF + RPC",  "l");
  leg1 -> Draw("same");

  c5 -> cd(2);
  hrate_r_mode5 -> SetStats(0);
  hrate_r_mode5 -> SetTitle("CSCTF Rate Mode 5; Pt[GeV]");
  hrate_r_mode5 -> Draw();
  hrate_t_mode5 -> SetLineColor(kRed);
  hrate_t_mode5 -> Draw("same");



  c5 -> cd(3);
  hrate_r_mode4 -> SetStats(0);
  hrate_r_mode4 -> SetTitle("CSCTF Rate Mode 4; Pt[GeV]");
  hrate_r_mode4 -> Draw();
  hrate_t_mode4 -> SetLineColor(kRed);
  hrate_t_mode4 -> Draw("same");

  c5 ->cd(4);
  hrate_r_mode1 -> SetStats(0);
  hrate_r_mode1 -> SetTitle("CSCTF Rate Mode 1; Pt[GeV]");
  hrate_r_mode1 -> Draw();
  hrate_t_mode1 -> SetLineColor(kRed);
  hrate_t_mode1 -> Draw("same");

  c5 -> SaveAs("all_csctf_rate.png");

  // ===========================================================================
  TH1F* hrate_t_all = (TH1F*) file->Get("hrate_t_all");
  TH1F* hrate_r_all = (TH1F*) file->Get("hrate_r_all");

  TCanvas* c2 = new TCanvas("c2", "", 420, 500, 600, 600);
  
  c2 -> Divide(1,2);

  c2 -> cd(1);
  hrate_t_all -> SetLineColor(kRed);
  hrate_t_all -> SetStats(0);
  hrate_t_all -> SetTitle("CSCTF Rate: All Modes; Pt[GeV]");
  hrate_t_all -> Draw();
  hrate_r_all -> Draw("same");

  TLegend *leg3 = new TLegend(0.1,0.7,0.48,0.9);
  leg3 -> SetFillStyle(0);
  leg3 -> AddEntry(hrate_t_all, "CSCTF",  "l");
  leg3 -> AddEntry(hrate_r_all, "CSCTF + RPC",  "l");
  leg3 -> Draw("same");  

  c2 -> cd(2);
  // Divide the rate plots
  const float rate[] = {0., 3., 5., 7., 10., 12., 16., 20., 30.};
  const int N_rate = (sizeof(rate)/sizeof(float) -1);
  TH1F* hrate_ratio = new TH1F("hrate_ratio", "", N_rate, rate);

  hrate_ratio -> Divide(hrate_t_all, hrate_r_all);
  hrate_ratio -> SetStats(0);
  hrate_ratio -> SetTitle("CSCTF Rate: Ratio; Pt[GeV]; CSCTF/(CSCTF+RPC)");
  hrate_ratio -> Draw();

  // ===========================================================================  
  TH1F* hres_r_all = (TH1F*) file->Get("hres_r_all");
  TH1F* hres_t_all = (TH1F*) file->Get("hres_t_all");
  
  TCanvas* c6 = new TCanvas("c6", "", 420, 500, 600, 600);
  
  hres_t_all -> SetLineColor(kRed);
  hres_t_all -> SetStats(0);
  hres_t_all -> SetTitle("CSCTF Resoltuion; (CSCTF Pt - Gbl Muon Pt)/ Gbl Muon Pt");
  hres_t_all -> Draw();
  c6 -> SetLogy();
  hres_r_all -> Draw("same");

  
  //c6 -> BuildLegend();
  TLegend *leg = new TLegend(0.1,0.7,0.48,0.9);
  leg -> SetFillStyle(0);
  leg -> AddEntry(hres_t_all, "CSCTF",  "l");
  leg -> AddEntry(hres_r_all, "CSCTF + RPC",  "l");
  leg -> Draw("same");

  c6 -> SaveAs("csctf_res.png");

  // ====================================================================
  
  TGraphAsymmErrors* hpt_7 = (TGraphAsymmErrors*) file->Get("divide_csctfPt_7.000000_by_csctfPt_all");
  TGraphAsymmErrors* hpt_r7 = (TGraphAsymmErrors*) file->Get("divide_csctfPt_rpc_7.000000_by_csctfPt_all");
  TGraphAsymmErrors* hpt_10 = (TGraphAsymmErrors*) file->Get("divide_csctfPt_10.000000_by_csctfPt_all");
  TGraphAsymmErrors* hpt_r10 = (TGraphAsymmErrors*) file->Get("divide_csctfPt_rpc_10.000000_by_csctfPt_all");
  TGraphAsymmErrors* hpt_12 = (TGraphAsymmErrors*) file->Get("divide_csctfPt_12.000000_by_csctfPt_all");
  TGraphAsymmErrors* hpt_r12 = (TGraphAsymmErrors*) file->Get("divide_csctfPt_rpc_12.000000_by_csctfPt_all");

  TCanvas* c7 = new TCanvas("c7", "", 420, 500, 600, 600);

  hpt_7 -> GetXaxis() -> SetTitle("Global #mu P_{t} [GeV]");
  hpt_7 -> GetXaxis() -> SetTitleOffset(1.4);
  hpt_7 -> SetLineColor(kRed);
  hpt_7 -> Draw("APE");
  hpt_r7 -> Draw("PEsame");
  
  TLatex l_3;
  l_3.SetNDC();
  l_3.SetTextAlign(31); //align right
  l_3.SetTextSize(0.03);
  l_3.SetTextAlign(11); //align left
  l_3.DrawLatex(0.50,0.93,"All Q, CSCTF P_{t} > 7 GeV");

  TLine * linePt_3 = new TLine(7, 0.33, 7, 0.97);
  linePt_3->SetLineColor(kBlue);
  linePt_3->SetLineWidth(3);
  linePt_3->Draw();
  l_3.Draw("same");

  c7 -> SetLogx(1);

  TCanvas* c8 = new TCanvas("c8", "", 420, 500, 600, 600);
  
  hpt_10 -> GetXaxis() -> SetTitle("Global #mu P_{t} [GeV]");
  hpt_10 -> GetXaxis() -> SetTitleOffset(1.4);
  hpt_10 -> SetLineColor(kRed);
  hpt_10 -> Draw("APE");
  hpt_r10 -> Draw("PEsame");

  TLatex l_10;
  l_10.SetNDC();
  l_10.SetTextAlign(31); //align right
  l_10.SetTextSize(0.03);
  l_10.SetTextAlign(11); //align left
  l_10.DrawLatex(0.50,0.93,"All Q, CSCTF P_{t} > 10 GeV");

  TLine * linePt_10 = new TLine(10, 0.29, 10, 0.95);
  linePt_10->SetLineColor(kBlue);
  linePt_10->SetLineWidth(3);
  linePt_10->Draw();
  l_10.Draw("same");

  c8 -> SetLogx(1);

  TCanvas* c9 = new TCanvas("c9", "", 420, 500, 600, 600);

  hpt_12 -> GetXaxis() -> SetTitle("Global #mu P_{t} [GeV]");
  hpt_12 -> GetXaxis() -> SetTitleOffset(1.4);
  hpt_12 -> SetLineColor(kRed);
  hpt_12 -> Draw("APE");
  hpt_r12 -> Draw("PEsame");

  TLatex l_12;
  l_12.SetNDC();
  l_12.SetTextAlign(31); //align right
  l_12.SetTextSize(0.03);
  l_12.SetTextAlign(11); //align left
  l_12.DrawLatex(0.50,0.93,"All Q, CSCTF P_{t} > 12 GeV");

  TLine * linePt_12 = new TLine(12, 0.27, 12, 0.94);
  linePt_12->SetLineColor(kBlue);
  linePt_12->SetLineWidth(3);
  linePt_12->Draw();
  l_12.Draw("same");

  TLegend * leg_12 = new TLegend(0.6,0.20,0.95,0.50);
  leg_12 -> AddEntry(hpt_12,"CSCTF");
  leg_12 -> AddEntry(hpt_r12,"CSCTF+RPC");
  leg_12 -> SetFillColor(0);
  leg_12 -> Draw("same");

  c9 -> SetLogx(1);  
  
  

} // end main

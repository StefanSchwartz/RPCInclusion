#ifndef __EFFIEVENT_H__
#define __EFFIEVENT_H__

#include <vector>
#include "TFile.h"
#include "TTree.h"

using namespace std;

class EffiEvent{

 public:
  
  EffiEvent();

  //----------------------------------------------------------------------------
  // Useful Methods
  //----------------------------------------------------------------------------

  
  void AttachToFile(TFile *file);
  
  void CloseFile(TFile *file);

  long int GetEntries() { return CSCtree ->GetEntries(); }

  void GetEntry(long int iEvt);

  TTree *CSCtree;
  
  // Pt assignment methods

  float pt_13(TTree CSCtree, int iCSCTrk, int printLevel);
  
  
  //----------------------------------------------------------------------------
  // Access the needed variables
  //----------------------------------------------------------------------------
  
#define MAX_CSCTF_TRK 36    // max # of CSCTF tracks per BX
#define MAX_LCTS_PER_TRK 4  // max # of LCTS which form a CSCTF track
#define MAX_RPC_LCTS_EVT 36
#define MAX_MUONS 100  

 
 // Put generated muons in CSC tree
  float gen_pt;
  float gen_eta;
  float gen_phi;
  unsigned int gen_chg;
  
  int Event;
  int Lumi;
  int Run;
  int Run_Test;
  // Reco Objects -----------------------------------------------------------
  int muonSize;
  
  double muon_pt[MAX_MUONS];
  double muon_eta[MAX_MUONS];
  double muon_phi[MAX_MUONS];
  float muon_chg[MAX_MUONS];
  int muon_match[MAX_MUONS];

  // CSC objects ------------------------------------------------------------
  int SizeTrk;

  float PtTrk   [MAX_CSCTF_TRK];
  double EtaTrk [MAX_CSCTF_TRK];
  double PhiTrk [MAX_CSCTF_TRK];
  int EtaBitTrk [MAX_CSCTF_TRK];
  int PhiBitTrk [MAX_CSCTF_TRK];
  int PtBitTrk  [MAX_CSCTF_TRK];
  int ModeTrk   [MAX_CSCTF_TRK];
  int ChgTrk    [MAX_CSCTF_TRK];

  
  // These store track pT info from PtAddtess.h
  int PtTrk_reco_front     [MAX_CSCTF_TRK][MAX_RPC_LCTS_EVT];
  int PtTrk_reco_rear      [MAX_CSCTF_TRK][MAX_RPC_LCTS_EVT];
  int PtTrk_reco_best_two  [MAX_CSCTF_TRK][MAX_RPC_LCTS_EVT];
  int rpcIsmatched         [MAX_CSCTF_TRK][MAX_RPC_LCTS_EVT];
  int trkIsMatched         [MAX_CSCTF_TRK];
  int RpcMatchedID         [MAX_CSCTF_TRK];

  //Added by Scott for looking at efficiency
  int PtTrk_reco_front_three [MAX_CSCTF_TRK];
  int PtTrk_reco_rear_three  [MAX_CSCTF_TRK]; 
  int PtTrk_reco_best        [MAX_CSCTF_TRK];
  int isGoodTrk              [MAX_CSCTF_TRK];

  // Info about LCTs forming the track
  // Stored in 2D array [track x] [LCT y] - each event has x tracks containing y Lcts
  int NumLctsTrk[MAX_CSCTF_TRK];  // How many Lcts made up track

  double trLctglobalPhi [MAX_CSCTF_TRK][MAX_LCTS_PER_TRK];
  double trLctglobalEta [MAX_CSCTF_TRK][MAX_LCTS_PER_TRK];
  int trLctPhiBit       [MAX_CSCTF_TRK][MAX_LCTS_PER_TRK];
  int trLctEtaBit       [MAX_CSCTF_TRK][MAX_LCTS_PER_TRK];
  //int trLctEtaBitMatt   [MAX_CSCTF_TRK][MAX_LCTS_PER_TRK];
  int trLctSector       [MAX_CSCTF_TRK][MAX_LCTS_PER_TRK];
  int trLctSubSector    [MAX_CSCTF_TRK][MAX_LCTS_PER_TRK];
  int trLctStation      [MAX_CSCTF_TRK][MAX_LCTS_PER_TRK];
  int trLctChamber      [MAX_CSCTF_TRK][MAX_LCTS_PER_TRK];
  int trLctEndcap       [MAX_CSCTF_TRK][MAX_LCTS_PER_TRK];
  int trLctBx           [MAX_CSCTF_TRK][MAX_LCTS_PER_TRK];
  int trLctCSCId        [MAX_CSCTF_TRK][MAX_LCTS_PER_TRK];
  int trLctRing         [MAX_CSCTF_TRK][MAX_LCTS_PER_TRK];


  // RPC objects ------------------------------------------------------------

  int PtTrk_reco_rpc_csc [MAX_CSCTF_TRK][3][MAX_RPC_LCTS_EVT];
  int csc_rpc_match      [MAX_CSCTF_TRK][3][MAX_RPC_LCTS_EVT];

  // Trigger Primitve variables
  int rpc_NumLctsTrk;
  
  double rpc_gblEta  [MAX_RPC_LCTS_EVT];
  double rpc_gblPhi  [MAX_RPC_LCTS_EVT];
  // unsigned rpc_strip [MAX_RPC_LCTS_EVT];
  //  unsigned rpc_Layer [MAX_RPC_LCTS_EVT];
  int rpc_bx         [MAX_RPC_LCTS_EVT];
  int rpc_Station    [MAX_RPC_LCTS_EVT];
  int rpc_Sector     [MAX_RPC_LCTS_EVT];
  int rpc_Phibit     [MAX_RPC_LCTS_EVT];
  //int rpc_Etabit     [MAX_RPC_LCTS_EVT];

};

  int eta_dphi_ME1toME2[64] = {127, 127, 127, 127, 127, 127, 40, 80, 80, 64,
                             127, 127,  62,  41,  41,  45, 47, 48, 47, 46,
			     47,  50,  52,  51,  53,  54, 55, 73, 82, 91,
			     91,  94, 100,  99,  95,  94, 95, 91, 96, 96,
			     94,  94,  88,  88,  80,  80, 84, 84, 79, 78,
			     80,  78,  75,  72,  70,  71, 69, 71, 71, 66,
			     61,  60,  43, 127};
  

// RPC phi: calculating rpc global phi in CSC bit form
int CalcIntegerPhi(float GblPhi,int sector,int station,int ring){

  if (sector > 6) sector -= 6;

  float slope[3][4][6] = {{{3783.79,3784.91,3784.06,3784.56,3784.05,3784.86},{3783.79,3784.91,3784.06,3784.56,3784.05,3784.86},\
			   {3784.54,3784.61,3784.54,3785.22,3784.56,3784.51},{3783.79,3784.91,3784.06,3784.56,3784.05,3784.86}},
			  {{0,0,0,0,0,0},{3784.07,3784.65,3784.17,3784.59,3784.17,3785.08},{0,0,0,0,0,0},{0,0,0,0,0,0}},
			  {{0,0,0,0,0,0},{3784.36,3784.68,3784.09,3784.61,3784.14,3785.02},{0,0,0,0,0,0},{0,0,0,0,0,0}}};

  float offset[3][4][6] = {{{-924.362,-4888.94,-8849.91,10964.5,7000.71,3038.25},{-924.362,-4888.94,-8849.91,10964.5,7000.71,3038.25},
			    {-925.02,-4888.23,-8851.1,10966.2,7001.31,3038.31}, {-925.02,-4888.23,-8851.1,10966.2,7001.31,3038.31}},
			   {{0,0,0,0,0,0},{-924.479,-4888.28,-8850.11,10964.6,7001.03,3038.3},{0,0,0,0,0,0},{0,0,0,0,0,0}},
			   {{0,0,0,0,0,0},{-924.573,-4888.41,-8849.93,10964.6,7000.99,3038.31},{0,0,0,0,0,0},{0,0,0,0,0,0}}};

  int phi_out = -1;

  if (sector == 3 and GblPhi < 0) GblPhi += 2*3.14159;

  phi_out = (GblPhi*slope[ring-1][station-1][sector-1]) + offset[ring-1][station-1][sector-1];

  if (phi_out < 0) phi_out += 4096;
    
  if (phi_out > 4096) phi_out -= 4096;
    
  return phi_out;
}




#endif

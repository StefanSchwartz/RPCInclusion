////////////////////////////////////////////////
// rpc_tuplizer.cc
//
//  - Creates ntuple of RPC, CSC hits and tracks(framework in place for DT).
//  - Works in L1Tmuon Framework
//
//  - Imports rpc_tuplizer.h for class declarations
// 
//  - Each subsystem(DT, CSC, RPC) has its own Ttree.
//    Tracks and its' primitives are filled, then all
//    primitives in event.
//
//  - Sample Python module to run this ntuplizer is at:
//    /home/dcurry/public/rpc/CMSSW_6_2_2/src/L1TriggerDPGUpgrade/L1TMuon/test/L1TMuon_ntuple.py
//
//  - Sample code to access the created root file is at:
//    /home/dcurry/public/trig_eff/CMSSW_6_2_0_pre5/src/L1CSCTFAnalysis/TrigEff/macros/rpc/LCTfilter.C
//
// Written by David Curry  Jan, 2014
//
///////////////////////////////////////////////


#include "L1TriggerDPGUpgrade/L1TMuon/plugins/rpc_tuplizer.h"
#include "track_pT.h"
#include "muon_track_matching.h"
#include <math.h>


rpc_tuplizer::rpc_tuplizer(const PSet& p) {

  outputFile    = p.getParameter<string>("outputFile");
  csctfTag      = p.getParameter<InputTag>("csctfTag");
  rpcTPTag      = p.getParameter<InputTag>("rpcTPTag");
  LUTparam      = p.getParameter<ParameterSet>("lutParam");
  muonsTag      = p.getParameter<InputTag>("muonsTag");
  cscSegTag     = p.getParameter<InputTag>("cscSegTag"); 
  
  // printLevel
  //  0 - No printout
  //  1 - Basic info printout
  //  2 - Verbose
  printLevel = p.getUntrackedParameter<int>("printLevel",2);
  
  // Is Monte Carlo or not?
  isMC = p.getUntrackedParameter<int>("isMC",0);

  if (isMC == 1) genSrc = p.getUntrackedParameter<InputTag>("genSrc");
 

  file  = new TFile(outputFile.c_str(),"RECREATE");

  
   // Set Tree branches/variables
  if (printLevel > 1) cout << "--->Initializing TTree" << endl << endl;
  rpc_tuplizer::set_tree();
  
  // Set eta/phi CSCTF LUTs
  if (printLevel > 1) cout << "--->Initializing CSCTF LUTs" << endl << endl;
  rpc_tuplizer::set_etaLUT();

} 


// destructor
rpc_tuplizer::~rpc_tuplizer(void){
  file->Write();
  file->Close();
}


// analyze - the main method called by cmssw.  Events are passed to it by a python module
void rpc_tuplizer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup){
  
  // Initialize the TTrees
  rpc_tuplizer::init_tree();

  // Defines the csc tracks collection coming form CSCTrackCoverter
  edm::Handle< vector<L1TMuon::InternalTrack> > CSCTFtracks;
  if ( csctfTag.label() != "null" ) iEvent.getByLabel(csctfTag, CSCTFtracks);
  
  // Defines the rpc trigger primitive collection coming from TriggerPrimitive Producer
  edm::Handle< vector<L1TMuon::TriggerPrimitive> > RPCTP;
  if ( rpcTPTag.label() != "null" ) iEvent.getByLabel(rpcTPTag, RPCTP);
  
  // Defines the generated muons from monte carlo
  edm::Handle<reco::GenParticleCollection> genps;
  if (isMC == 1) iEvent.getByLabel(genSrc, genps);
  
  // Defines Reco Muon Collection  
  edm::Handle<MuonCollection> muons;
  if( muonsTag.label() != "null" ) iEvent.getByLabel(muonsTag, muons);

  // Defines muon segment Collection
  edm::Handle<CSCSegmentCollection> cscSegments;
  if( cscSegTag.label() != "null" ) iEvent.getByLabel(cscSegTag, cscSegments);

  // General event info
  Run   = iEvent.id().run();
  Event = iEvent.id().event();
  Lumi  = iEvent.luminosityBlock();


  if (printLevel > 1 )
    cout << "\n======================================= New Event =========================================\n"
         <<   "===========================================================================================\n" 
	 <<   "Event # = " << Event 
	 <<   ", Run # = " << Run << endl;
  
  
  // ==================================================================================
  // =========== Start Filling of Track Finders  ======================================  
  
  if (printLevel > 1 ) 
    cout << "\n===================== FILLING CSCTF ========================\n"
         <<   "============================================================\n" << endl;
    
  
  // Make sure CSC track collection is valid type
  if( CSCTFtracks.isValid() ){
    
    // Initialize CSCTF pT LUTs
    rpc_tuplizer::pt_lut_init();
    ESHandle< L1MuTriggerScales > scales;
    iSetup.get< L1MuTriggerScalesRcd >().get(scales);
    scale = scales.product();
    ESHandle< L1MuTriggerPtScale > ptscales;
    iSetup.get< L1MuTriggerPtScaleRcd >().get(ptscales);
    ptScale = ptscales.product();
 
    // Call method to fill CSCTF variables(defined below).
    // Takes in CSC track collection, rpc hits collection, pt scales and LUTs
    rpc_tuplizer::fillCSCTF(CSCTFtracks, RPCTP, scale, ptScale, srLUTs_);

  }
  
  else cout << "Invalid L1CSCInternalTrack collection... skipping it\n";
  

  // ------ end CSCTF ------------------------------------------------------

  
  if (printLevel > 1 )
    cout<< "\n===================== FILLING Generated Particles ========================\n"
	<<   "============================================================\n" << endl;

  if (isMC == 1) rpc_tuplizer::fillGenParticles(genps, cscSegments, CSCTFtracks);

  if( muons.isValid() ){

    //fill the muons
    if (printLevel >= 1)
      cout<< "\n===================== FILLING MUONS ========================\n"
          <<   "============================================================\n"
          << endl;

    fillMuons(muons, cscSegments, CSCTFtracks);
  
  }


  // fill the ttree
  if (printLevel >1) cout << "---->Final Tree Filling:CSCTF" << endl << endl;
  rpc_tuplizer::fill_CSCtree();



  // --------- end Tree Filling Methods -----------------------------------------------------




  if (printLevel >1) cout << "---->Deleting Current Tree Pointers" << endl << endl;;
  //clean-up the pointers
  rpc_tuplizer::del_tree();

} // end analyze






//---------- Begin Tree Filling Methods ------------------------------------------------------
//--------------------------------------------------------------------------------------------


// This method fills csctf tracks and all LCTs that make up the tracks.  ALso brings in RPC hits.
void rpc_tuplizer::fillCSCTF(const edm::Handle< vector<L1TMuon::InternalTrack> > tracks,
			     const edm::Handle< vector<L1TMuon::TriggerPrimitive> > rpc_hits,
			     const L1MuTriggerScales  *scale,
			     const L1MuTriggerPtScale *ptScale,
			     CSCSectorReceiverLUT* srLUTs_[5][2]) {

  // ===================================================================================================
  // Fill RPC hits

  // For RPC Declustering
  vector<double> stat1_phi_vec;
  vector<double> stat2_phi_vec;
  vector<double> stat3_phi_vec;
  vector<double> stat4_phi_vec;

  vector<double> stat1_eta_vec;
  vector<double> stat2_eta_vec;
  vector<double> stat3_eta_vec;
  vector<double> stat4_eta_vec;

  vector<double> phi_average;
  vector<double> eta_average;

  rpc_stat1_cluster_phi = -999;
  rpc_stat2_cluster_phi = -999;
  rpc_stat3_cluster_phi = -999;
  rpc_stat4_cluster_phi = -999;
  
  rpc_stat1_cluster_eta = -999;
  rpc_stat2_cluster_eta = -999;
  rpc_stat3_cluster_eta = -999;
  rpc_stat4_cluster_eta = -999;

  cluster_size = 0;
  cluster_size_outliers = 0;

  double aphi = -999;
  double aeta = -999;

  int rpc_stat1_cluster_ring = -999;
  int rpc_stat2_cluster_ring = -999;
  int rpc_stat3_cluster_ring = -999;
  int rpc_stat4_cluster_ring = -999;

  int rpc_stat1_cluster_sector = -999;
  int rpc_stat2_cluster_sector = -999;
  int rpc_stat3_cluster_sector = -999;
  int rpc_stat4_cluster_sector = -999;

  int rpc_stat1_cluster_subsector = -999;
  int rpc_stat2_cluster_subsector = -999;
  int rpc_stat3_cluster_subsector = -999;
  int rpc_stat4_cluster_subsector = -999;


  int RpcTrkId_2 = 0;
  for ( auto rpc = rpc_hits->cbegin(); rpc < rpc_hits->cend(); rpc++, RpcTrkId_2++) {

    if (RpcTrkId_2 == MAX_RPC_LCTS_EVT-1) break;

    double rpc_gblphi  = rpc->getCMSGlobalPhi();
    double rpc_gbleta  = rpc->getCMSGlobalEta();
    
    if (rpc_gblphi < 0){
	rpc_gblphi = rpc_gblphi + (2 * M_PI);
    }

    // NEW:  RPC Declustering.  Per station, reduces # of RPC hits to one phi coordinate.
    //  Four different vectors of RPC phi for each station

    if (rpc_gbleta > 0.9 && rpc_gbleta < 2.1){
      	phi_average.push_back(rpc_gblphi);
      	eta_average.push_back(rpc_gbleta);
    }
  } // end RPC hit loop

  sort(phi_average.begin(), phi_average.end());
  sort(eta_average.begin(), eta_average.end());

  if (phi_average.size() > 0) {
    aphi = std::accumulate(phi_average.begin(), phi_average.end(), 0.0) / phi_average.size();
    aeta = std::accumulate(eta_average.begin(), eta_average.end(), 0.0) / eta_average.size();
  }


  int RpcTrkId_ = 0;
  for ( auto rpc = rpc_hits->cbegin(); rpc < rpc_hits->cend(); rpc++, RpcTrkId_++) {

    if (RpcTrkId_ == MAX_RPC_LCTS_EVT-1) break;

    // ------------- Fill all RPC variables ------------------
    RPCDetId rpc_id = rpc -> detId<RPCDetId>();

    double rpc_gblphi  = rpc->getCMSGlobalPhi();
    double rpc_gbleta  = rpc->getCMSGlobalEta();
    int rpc_station    = rpc_id.station();
    int rpc_sector     = rpc_id.sector();
    int rpc_subsector  = rpc_id.subsector();
    int rpc_ring       = rpc_id.ring();    
    unsigned rpc_layer = rpc->getRPCData().layer;

    // Send look up table adjusted sector(1-6 only)
    int temp_sector = -999;
    if (rpc_sector>6) temp_sector = rpc_sector - 6;
    else temp_sector = rpc_sector;
    float rpc_phiBit   = CalcIntegerPhi(rpc_gblphi, temp_sector);
    // =========================================

    if (printLevel > 1) {
      cout << "RPC Station: " << rpc_station << endl;  
      cout << "RPC Sector: " << rpc_sector << endl;
      cout << "RPC Ring: " << rpc_ring << endl;
      cout << "RPC PhiBit: " << rpc_phiBit << endl << endl;
    }
    
    if (rpc_gblphi < 0){
	rpc_gblphi = rpc_gblphi + (2 * M_PI);
    }

   // cout << "gblphi: " << rpc_gblphi << endl << "station: " << rpc_station << endl;
    
    // Fill Branches
    rpc_gblEta[RpcTrkId_]  = rpc_gbleta;
    rpc_gblPhi[RpcTrkId_]  = rpc_gblphi;
    rpc_Station[RpcTrkId_] = rpc_station;
    rpc_Sector[RpcTrkId_]  = rpc_sector;
    rpc_Layer[RpcTrkId_]   = rpc_layer;
    rpc_Phibit[RpcTrkId_]  = rpc_phiBit;
    //rpc_Etabit[RpcTrkId_]  = rpc_trLctEtaBit_Matt;

    // NEW:  RPC Declustering.  Per station, reduces # of RPC hits to one phi coordinate.
    //  Four different vectors of RPC phi for each station
    if (rpc_station == 1) {
      stat1_phi_vec.push_back(rpc_gblphi);
      stat1_eta_vec.push_back(rpc_gbleta);
      rpc_stat1_cluster_ring   = rpc_ring;
      rpc_stat1_cluster_sector = rpc_sector;
      rpc_stat1_cluster_subsector = rpc_subsector;
    }

    if (rpc_station == 2) {
      if ((rpc_gbleta > 0.9 && rpc_gbleta < 2.1) && ((rpc_gbleta/aeta > .75) && (rpc_gbleta/aeta < 1.25)) && ((rpc_gblphi/aphi > .75) && (rpc_gblphi/aphi < 1.25))){
      	stat2_phi_vec.push_back(rpc_gblphi);
      	stat2_eta_vec.push_back(rpc_gbleta);
      }
      rpc_stat2_cluster_ring   = rpc_ring;
      rpc_stat2_cluster_sector = rpc_sector;
      rpc_stat2_cluster_subsector = rpc_subsector;
      cluster_size_outliers += 1;
    }

    if (rpc_station == 3) {
      stat3_phi_vec.push_back(rpc_gblphi);
      stat3_eta_vec.push_back(rpc_gbleta);
      rpc_stat3_cluster_ring   = rpc_ring;
      rpc_stat3_cluster_sector = rpc_sector;
      rpc_stat3_cluster_subsector = rpc_subsector;
    }

    if (rpc_station == 4) {
      stat4_phi_vec.push_back(rpc_gblphi);
      stat4_eta_vec.push_back(rpc_gbleta);
      rpc_stat4_cluster_ring   = rpc_ring;
      rpc_stat4_cluster_sector = rpc_sector;
      rpc_stat4_cluster_subsector = rpc_subsector;
    }
  } // end RPC hit loop

  // For clustering, take average of each vector for decluster RPC phi
  sort(stat1_phi_vec.begin(), stat1_phi_vec.end());
  sort(stat2_phi_vec.begin(), stat2_phi_vec.end());
  sort(stat3_phi_vec.begin(), stat3_phi_vec.end());
  sort(stat4_phi_vec.begin(), stat4_phi_vec.end());

  sort(stat1_eta_vec.begin(), stat1_eta_vec.end());
  sort(stat2_eta_vec.begin(), stat2_eta_vec.end());
  sort(stat3_eta_vec.begin(), stat3_eta_vec.end());
  sort(stat4_eta_vec.begin(), stat4_eta_vec.end());

  if (stat1_phi_vec.size() > 0) {
    //rpc_stat1_cluster_phi = (stat1_phi_vec.front() + stat1_phi_vec.back()) / 2.0;
    //rpc_stat1_cluster_eta = (stat1_eta_vec.front() + stat1_eta_vec.back()) / 2.0;
    rpc_stat1_cluster_phi = std::accumulate(stat1_phi_vec.begin(), stat1_phi_vec.end(), 0.0) / stat1_phi_vec.size();
   // cout << "Cluster Phi 1: " << rpc_stat1_cluster_phi << endl;
    rpc_stat1_cluster_eta = std::accumulate(stat1_eta_vec.begin(), stat1_eta_vec.end(), 0.0) / stat1_eta_vec.size();
  }

  if (stat2_phi_vec.size() > 0) {
    //rpc_stat2_cluster_phi = (stat2_phi_vec.front() + stat2_phi_vec.back()) / 2.0;
    //rpc_stat2_cluster_eta = (stat2_eta_vec.front() + stat2_eta_vec.back()) / 2.0;
    rpc_stat2_cluster_phi = (std::accumulate(stat2_phi_vec.begin(), stat2_phi_vec.end(),0.0) / stat2_phi_vec.size());
    //cout << "Cluster Phi 2: " << rpc_stat2_cluster_phi << endl;
    cluster_size = stat2_phi_vec.size();
    rpc_stat2_cluster_eta = std::accumulate(stat2_eta_vec.begin(), stat2_eta_vec.end(), 0.0) / stat2_eta_vec.size();
  }

  if (stat3_phi_vec.size() > 0) {
    //rpc_stat3_cluster_phi = (stat3_phi_vec.front() + stat3_phi_vec.back()) / 2.0;
    //rpc_stat3_cluster_eta = (stat3_eta_vec.front() + stat3_eta_vec.back()) / 2.0;
    rpc_stat3_cluster_phi = std::accumulate(stat3_phi_vec.begin(), stat3_phi_vec.end(),0.0) / stat3_phi_vec.size();
    //cout << "Cluster Phi 3: " << rpc_stat3_cluster_phi << endl;
    rpc_stat3_cluster_eta = std::accumulate(stat3_eta_vec.begin(), stat3_eta_vec.end(), 0.0) / stat3_eta_vec.size();
  }

  if (stat4_phi_vec.size() > 0) {
    // rpc_stat4_cluster_phi = (stat4_phi_vec.front() + stat4_phi_vec.back()) / 2.0;
    //rpc_stat4_cluster_eta = (stat4_eta_vec.front() + stat4_eta_vec.back()) / 2.0;
    rpc_stat4_cluster_phi = std::accumulate(stat4_phi_vec.begin(), stat4_phi_vec.end(),0.0) / stat4_phi_vec.size();
  //  cout << "Cluster Phi 4: " << rpc_stat4_cluster_phi << endl;
    rpc_stat4_cluster_eta = std::accumulate(stat4_eta_vec.begin(), stat4_eta_vec.end(), 0.0) / stat4_eta_vec.size();
  }

  // Cluster sector must be determined manually.  Sometimes the rpc sector and the cluster sector are not the same
  // station1
  if (rpc_stat1_cluster_phi > 0.243 && rpc_stat1_cluster_phi < 1.2914)
    rpc_stat1_cluster_sector = 1;
  if (rpc_stat1_cluster_phi > 1.2914 && rpc_stat1_cluster_phi < 2.338)
    rpc_stat1_cluster_sector = 2;
  if (rpc_stat1_cluster_phi > 2.388 && rpc_stat1_cluster_phi < -2.898)
    rpc_stat1_cluster_sector = 3;
  if (rpc_stat1_cluster_phi > -2.898 && rpc_stat1_cluster_phi < -1.8507)
    rpc_stat1_cluster_sector = 4;
  if (rpc_stat1_cluster_phi > -1.8507 && rpc_stat1_cluster_phi < -0.803)
    rpc_stat1_cluster_sector = 5;
  if (rpc_stat1_cluster_phi > -0.803 && rpc_stat1_cluster_phi < 0.243)
    rpc_stat1_cluster_sector = 6;

  // station 2
  if (rpc_stat2_cluster_phi > 0.243 && rpc_stat2_cluster_phi < 1.2914)
    rpc_stat2_cluster_sector = 1;
  if (rpc_stat2_cluster_phi > 1.2914 && rpc_stat2_cluster_phi < 2.338)
    rpc_stat2_cluster_sector = 2;
  if (rpc_stat2_cluster_phi > 2.388 && rpc_stat2_cluster_phi < -2.898)
    rpc_stat2_cluster_sector = 3;
  if (rpc_stat2_cluster_phi > -2.898 && rpc_stat2_cluster_phi < -1.8507)
    rpc_stat2_cluster_sector = 4;
  if (rpc_stat2_cluster_phi > -1.8507 && rpc_stat2_cluster_phi < -0.803)
    rpc_stat2_cluster_sector = 5;
  if (rpc_stat2_cluster_phi > -0.803 && rpc_stat2_cluster_phi < 0.243)
    rpc_stat2_cluster_sector = 6;

  // station 3
  if (rpc_stat3_cluster_phi > 0.243 && rpc_stat3_cluster_phi < 1.2914)
    rpc_stat3_cluster_sector = 1;
  if (rpc_stat3_cluster_phi > 1.2914 && rpc_stat3_cluster_phi < 2.338)
    rpc_stat3_cluster_sector = 2;
  if (rpc_stat3_cluster_phi > 2.388 && rpc_stat3_cluster_phi < -2.898)
    rpc_stat3_cluster_sector = 3;
  if (rpc_stat3_cluster_phi > -2.898 && rpc_stat3_cluster_phi < -1.8507)
    rpc_stat3_cluster_sector = 4;
  if (rpc_stat3_cluster_phi > -1.8507 && rpc_stat3_cluster_phi < -0.803)
    rpc_stat3_cluster_sector = 5;
  if (rpc_stat3_cluster_phi > -0.803 && rpc_stat3_cluster_phi < 0.243)
    rpc_stat3_cluster_sector = 6;

  // station 4
  if (rpc_stat4_cluster_phi > 0.243 && rpc_stat4_cluster_phi < 1.2914)
    rpc_stat4_cluster_sector = 1;
  if (rpc_stat4_cluster_phi > 1.2914 && rpc_stat4_cluster_phi < 2.338)
    rpc_stat4_cluster_sector = 2;
  if (rpc_stat4_cluster_phi > 2.388 && rpc_stat4_cluster_phi < -2.898)
    rpc_stat4_cluster_sector = 3;
  if (rpc_stat4_cluster_phi > -2.898 && rpc_stat4_cluster_phi < -1.8507)
    rpc_stat4_cluster_sector = 4;
  if (rpc_stat4_cluster_phi > -1.8507 && rpc_stat4_cluster_phi < -0.803)
    rpc_stat4_cluster_sector = 5;
  if (rpc_stat4_cluster_phi > -0.803 && rpc_stat4_cluster_phi < 0.243)
    rpc_stat4_cluster_sector = 6;


  if (printLevel > 2) {
    cout << "rpc cluster 1 phi = " << rpc_stat1_cluster_phi << endl;
    cout << "rpc cluster 2 phi = " << rpc_stat2_cluster_phi << endl;
    cout << "rpc cluster 3 phi = " << rpc_stat3_cluster_phi << endl;
    cout << "rpc cluster 4 phi = " << rpc_stat4_cluster_phi << endl;

    cout << "rpc cluster 1 eta = " << rpc_stat1_cluster_eta << endl;
    cout << "rpc cluster 2 eta = " << rpc_stat2_cluster_eta << endl;
    cout << "rpc cluster 3 eta = " << rpc_stat3_cluster_eta << endl;
    cout << "rpc cluster 4 eta = " << rpc_stat4_cluster_eta << endl;
    
    cout << "rpc cluster 1 ring = " << rpc_stat1_cluster_ring << endl;
    cout << "rpc cluster 2 ring = " << rpc_stat2_cluster_ring << endl;
    cout << "rpc cluster 3 ring = " << rpc_stat3_cluster_ring << endl;
    cout << "rpc cluster 4 ring = " << rpc_stat4_cluster_ring << endl;
    
    cout << "rpc cluster 1 sector = " << rpc_stat1_cluster_sector << endl;
    cout << "rpc cluster 2 sector = " << rpc_stat2_cluster_sector << endl;
    cout << "rpc cluster 3 sector = " << rpc_stat3_cluster_sector << endl;
    cout << "rpc cluster 4 sector = " << rpc_stat4_cluster_sector << endl;
    
    cout << "rpc cluster 1 subsector = " << rpc_stat1_cluster_subsector << endl;
    cout << "rpc cluster 2 subsector = " << rpc_stat2_cluster_subsector << endl;
    cout << "rpc cluster 3 subsector = " << rpc_stat3_cluster_subsector << endl;
    cout << "rpc cluster 4 subsector = " << rpc_stat4_cluster_subsector << endl;
  }


  // Now the phiBit conversions for clusters(stat2 only right now)
  // Send look up table adjusted sector(1-6 only)
  if (rpc_stat2_cluster_phi != -999) {
    rpc_stat2_cluster_phiBit = CalcIntegerPhi(rpc_stat2_cluster_phi, rpc_stat2_cluster_sector);
    if (printLevel>2) cout << " RPC cluster stat2 phiBit = " <<  rpc_stat2_cluster_phiBit << endl;
  }

  double phi1 = -999;
  double phi2 = rpc_stat2_cluster_phi;

  rpc_NumLctsTrk = RpcTrkId_;
  //cout << " rpc_NumLctsTrk = " << rpc_NumLctsTrk;
  
  // ===================================================================================================
  
  // loop over CSCTF tracks
  int nTrk=0;
  for( auto trk = tracks->cbegin(); trk < tracks->cend(); trk++){
    
    if (printLevel > 1) cout << "Looping over track # " << nTrk << endl;

    if (nTrk >= MAX_CSCTF_TRK-1) break; // dont exceed array size

    // Create a vector to be filled with LCT variables.  Used by PtAddress.h
    std::vector<std::vector<int>> CSChits;
    CSChits.clear();

    // Access the track variables in bit form
    int trkPt_bit         = trk -> pt_packed();
    float trkEta_bit      = trk -> eta_packed();
    float trkPhi_bit      = trk -> phi_packed();
    unsigned long trkMode = trk -> cscMode();
    int trk_charge        = trk -> chargeValue();                
    //    int trSector          = 6*(trk->endcap()-1)+trk->sector();
    
    
    // Access pt/eta in human readable form ----------------------------------
    float trkPt  = ptscale[trkPt_bit];
    double trEta = scale -> getRegionalEtaScale(2) -> getCenter(trkEta_bit);    
    double trPhi = scale -> getPhiScale() -> getLowEdge(trkPhi_bit);
    
    //Phi in one sector varies from [0,62] degrees -> Rescale manually to global coords.
    //    double trPhi02PI = fmod(trPhi +
    //                      ((trSector-1)*TMath::Pi()/3) + //sector 1 starts at 15 degrees
      //                      (TMath::Pi()/12) , 2*TMath::Pi());
    
    
    if (printLevel > 1) {
      cout << "============================"  << endl;
      cout << " CSC Track Pt             = " << trkPt   << endl;
      cout << " CSC Track Eta(bit)       = " << trkEta_bit  << endl;
      cout << " CSC Track Eta(Gbl)       = " << trEta << endl;
      cout << " CSC Track mode           = " << trkMode << endl;
      cout << " CSC Track Phi(bit)       = " << trkPhi_bit << endl;
      cout << " CSC Track Phi(Gbl)       = " << trPhi << endl;
    } 
   
    // Fill the branches
    PtBitTrk  [nTrk] = trkPt_bit;
    EtaBitTrk [nTrk] = trkEta_bit;
    ModeTrk   [nTrk] = trkMode;
    PtTrk     [nTrk] = trkPt;
    EtaTrk    [nTrk] = trEta;
    PhiTrk    [nTrk] = trPhi;
    ChgTrk    [nTrk] = trk_charge;


    // For each trk, get the list of its LCTs.  
    if (printLevel > 1) cout << "---------- Loop over LCTs matched to Track -----------\n" ;
    
    auto lct_map = trk -> getStubs();  // Get map for LCTs to stations
    
    int LctTrkId_ = 0;
    std::vector<int> hits; // to be filled with all lCT variables.  Used in PtAdress.h

    // Loop over stations and get all stubs in each one
    for( unsigned station = 1; station <= 4; ++station ) {
      if (printLevel > 1) cout << " \n Looping over station " << station << " to get LCTs" << endl;

      std::vector<int> hits; // to be filled with all lCT variables.  Used in PtAdress.h
    
      const unsigned id    = 4*L1TMuon::InternalTrack::kCSC+station-1; // unique identifier for each csc station
      
      if( !lct_map.count(id) && printLevel > 1) cout << "  CSC Station map empty " << endl;
      
      auto x_LCTs = lct_map[id];  // access the matched lcts for a given station id
      
      // Loop over lcts in each station
      for ( auto t_lcts = x_LCTs.cbegin(); t_lcts != x_LCTs.cend(); t_lcts++ ) {

        auto lcts = *t_lcts; // dereference the edm:Ref object
        CSCDetId id = lcts->detId<CSCDetId>();
	
	// choose the objects you'd like to access
        auto trlct_station       = id.station();
        auto trlct_endcap        = id.endcap();
	auto trlct_chamber       = id.chamber();
	auto trlct_ring          = id.ring();
	double trlct_phi         = lcts->getCMSGlobalPhi();
        double trlct_eta         = lcts->getCMSGlobalEta();
        uint16_t trlct_bx        = lcts->getCSCData().bx;
        int trlct_sector         = CSCTriggerNumbering::triggerSectorFromLabels(id);
        int trlct_subsector      = CSCTriggerNumbering::triggerSubSectorFromLabels(id);
        //uint16_t trlct_bx0       = lcts->getCSCData().bx0;
        uint16_t trlct_cscID     = lcts->getCSCData().cscID;
        uint16_t trlct_strip     = lcts->getCSCData().strip;
        uint16_t trlct_pattern   = lcts->getCSCData().pattern;
        uint16_t trlct_bend      = lcts->getCSCData().bend;
        uint16_t trlct_quality   = lcts->getCSCData().quality;
        uint16_t trlct_keywire   = lcts->getCSCData().keywire;

	if (trlct_station == 1){	
		phi1 = trlct_phi;
	}	
	
	//unsigned trlct_Sector    = id.sector();
	//unsigned trlct_Subsector = id.subsector();
		
	
	// Now access eta/phi in bit form used by CSCTF
	int FPGALct = ( trlct_subsector ? trlct_subsector-1 : trlct_station );
	if (trlct_endcap == 2) trlct_endcap = 0;
	
	lclphidat lclPhi = srLUTs_[FPGALct][trlct_endcap] -> localPhi(trlct_strip,
								      trlct_pattern,
								      trlct_quality,
								      trlct_bend);
	
	gblphidat gblPhi = srLUTs_[FPGALct][trlct_endcap] -> globalPhiME(lclPhi.phi_local,
									 trlct_keywire,
									 trlct_cscID);
	
	gbletadat gblEta = srLUTs_[FPGALct][trlct_endcap] -> globalEtaME(lclPhi.phi_bend_local,
									 lclPhi.phi_local,
									 trlct_keywire,
									 trlct_cscID);
	
	
	// This is used for PtAddress.h module
	float trLctEtaBit_Matt = 0;
	for(int iETA = 0; iETA < 15; iETA++)
	  if(fabs(trlct_eta) >= etabins[iETA] && fabs(trlct_eta) < etabins[iETA+1] ) {
	    trLctEtaBit_Matt = iETA;
	    break;
	  }  
	
	// These are called global but are really CSCTF bits
	auto trlct_phi_bit = gblPhi.global_phi;
	auto trlct_eta_bit = gblEta.global_eta;
	
	if (printLevel > 1) {
          cout << "  CSC LCT # = " << LctTrkId_       << endl;
          cout << "  Phi       = " << trlct_phi       << endl;
          cout << "  Eta       = " << trlct_eta       << endl;
          cout << "  Phi(bit)  = " << trlct_phi_bit   << endl;
	  cout << "  Eta(bit)  = " << trlct_eta_bit   << endl;
	  cout << "  Bunch X   = " << trlct_bx        << endl;
          cout << "  Sector    = " << trlct_sector    << endl;
	  //cout << "  New Sector= " << trlct_Sector    << endl;
          cout << "  SubSector = " << trlct_subsector << endl;
	  //cout << "  New SubSector = " << trlct_Subsector << endl;
          cout << "  Endcap    = " << trlct_endcap    << endl;
          cout << "  Station   = " << trlct_station   << endl;
          cout << "  cscID     = " << trlct_cscID     << endl << endl;
        }
        //------------- Fill ntuple with LCT values ------------
	
        if ( (nTrk-1) > (MAX_CSCTF_TRK-1) || LctTrkId_ > (MAX_LCTS_PER_TRK-1) ) {
	  
	  if (printLevel>1) {
	  cout << "the track has " << nTrk-1 << " tracks, and " << LctTrkId_+1 << "Lcts, but the MAX allowed tracks is "
               << MAX_CSCTF_TRK << " and the max LCTs is " << MAX_LCTS_PER_TRK << " , -> Skipping the rest of tracks... " << endl;
          }
	  continue;
        }
	
	// Check if DetId is within range
        if(   trlct_sector < 1  || trlct_sector > 12 ||
              trlct_station < 1 || trlct_station > 4 ||
	      trlct_cscID < 1   || trlct_cscID >  9 ||
              trlct_endcap > 1  || trlct_endcap < 0  ) {
          if (printLevel > 1) cout << "  TRACK ERROR: CSC digi are out of range! " << endl;
          continue;
        }
	
        trLctglobalPhi[nTrk][LctTrkId_]   = trlct_phi;
	trLctglobalEta[nTrk][LctTrkId_]   = trlct_eta;
        trLctPhiBit[nTrk][LctTrkId_]      = trlct_phi_bit;
        trLctEtaBit[nTrk][LctTrkId_]      = trlct_eta_bit;
        trLctEtaBitMatt[nTrk][LctTrkId_]  = trLctEtaBit_Matt;
        trLctEndcap[nTrk][LctTrkId_]      = trlct_endcap;
	
	// if ( trlct_endcap == 0)
	//   trLctSector[nTrk][LctTrkId_]    = trlct_sector;
        //else
	//trLctSector[nTrk][LctTrkId_]    = 6+trlct_sector;
        trLctSector[nTrk][LctTrkId_]      = trlct_sector;
	trLctSubSector[nTrk][LctTrkId_]   = trlct_subsector;
        trLctBx[nTrk][LctTrkId_]          = trlct_bx;
        trLctStation[nTrk][LctTrkId_]     = trlct_station;
        trLctCSCId[nTrk][LctTrkId_]       = trlct_cscID;
	trLctChamber[nTrk][LctTrkId_]     = trlct_chamber;
	trLctRing[nTrk][LctTrkId_]        = trlct_ring;
	
	// fill hits vector with LCT variables
	hits.push_back(trlct_station);
	hits.push_back(trlct_phi_bit);
	hits.push_back( trLctEtaBit_Matt);
	hits.push_back(trLctSector[nTrk][LctTrkId_]);
	hits.push_back(trlct_subsector);
	hits.push_back(trlct_cscID);
	hits.push_back(trlct_chamber);
	
	// Fill vector to be used in PtAddress.h
	CSChits.push_back(hits);
	
	LctTrkId_ += 1;
	
      } // end loop over lcts matched to station
    } // end loop over stations
    
    NumLctsTrk[nTrk] = LctTrkId_;
    
    if (printLevel >1) {
      cout << "End loop over Lcts" << endl << endl;
      cout << "Track contains " << LctTrkId_ << " Lcts." << endl;
    }
    
    // Now check if PtAddress.h computes correct track pt for 3 hit tracks only. 
    int isGoodTrack = rpc_tuplizer::check_track_pt(CSChits, trkPt);
    
    if (isGoodTrack == 1 || isGoodTrack == 2) {

      if (printLevel>2) cout << " --->Track has good pT assignment. Continue On" << endl;
      

      if (rpc_stat2_cluster_phi != -999) {
	
	// This module takes CSC tracks with 3 LCTs and replaces one with RPC LCT, then computes pT and stores in TTree.
	rpc_tuplizer::three_track_pt(CSChits, nTrk, trkPt, rpc_stat2_cluster_phiBit, rpc_stat2_cluster_phi, rpc_stat2_cluster_eta, rpc_hits);
	
	isGoodTrk[nTrk] = 1;
      }
    }
    
    else {
      if (printLevel>2) cout << " --->If 3 hit Track: has bad pT assignment " << endl;
      isGoodTrk[nTrk] = 0;
    }

    double dphi12 = phi2 - phi1;
    
    
    // ------------ For each track use its LCT info to find track pt if there are 2 Lcts from different stations
    
    if ( LctTrkId_ == 2  &&
	 trLctStation[nTrk][0] != trLctStation[nTrk][1] ) {
      
      // ====================================================================================
      // Now we try and use an rpc hit for pt calculation. Still only looking at 2 Lct tracks
      // For each CSC Lct loop over RPC Lcts and see if there is a match(dr < 0.2).
      // UPDATE: Look at all RPC hits regardless of dr. Also look at decluster phi

      int RpcTrkId_ = 0 ;
      vector<int> rpchits;
      vector<int> rpchits_adjusted;
      vector<vector<int>> temp_cschits;
      vector<vector<int>> temp_cschits_adjusted;
      vector<int> pt_temp;
      
      
      if (printLevel > 2) cout << "\n  Track has two LCTs. Use rpc hit to find pT " << endl;

      // ==================================================================
      // NEW.  Use declustered RPC phi to get track pt.
      if ( (trLctStation[nTrk][0] == 1 || trLctStation[nTrk][0] == 3) &&
	   (trLctStation[nTrk][1] == 1 || trLctStation[nTrk][1] == 3) &&
	   rpc_stat2_cluster_phi != -999 ) {
	
	if (printLevel > 2) cout << "RPC cluster is in station 2, and csc hits are in station 1-3\n";  
	
	// This is used for PtAddress.h module
	float rpc_trLctEtaBit_Matt = 0;
        for (int iETA = 0; iETA < 15; iETA++) {
          if(fabs(rpc_stat2_cluster_eta) >= etabins[iETA] && fabs(rpc_stat2_cluster_eta) < etabins[iETA+1] ) {
            rpc_trLctEtaBit_Matt = iETA;
            break;
	  }
	}

	rpc_stat2_cluster_phi_adjusted = -999;

	if (dphi12 > 0){
		rpc_stat2_cluster_phi_adjusted = rpc_stat2_cluster_phi + (0.1 * dphi12);
	}	

	// Now the phiBit conversions for clusters(stat2 only right now)
	// Send look up table adjusted sector(1-6 only)
	if (rpc_stat2_cluster_phi != -999) {
	        rpc_stat2_cluster_phiBit = CalcIntegerPhi(rpc_stat2_cluster_phi, rpc_stat2_cluster_sector);
		rpc_stat2_cluster_phiBit_adjusted = CalcIntegerPhi(rpc_stat2_cluster_phi_adjusted, rpc_stat2_cluster_sector);
	        if (printLevel>2) cout << " RPC cluster stat2 phiBit = " <<  rpc_stat2_cluster_phiBit << endl;
	}
	
	// Fill CSChits vector with both csc lcts and the matched rpc lct.  Use info of first csc lct as stand in for rpc info.
	rpchits.push_back(2);
	rpchits.push_back(rpc_stat2_cluster_phiBit);
	rpchits.push_back(rpc_trLctEtaBit_Matt);
 	rpchits.push_back(trLctSector[nTrk][0]);
	rpchits.push_back(trLctSubSector[nTrk][0]);
	rpchits.push_back(trLctCSCId[nTrk][0]);
	rpchits.push_back(trLctChamber[nTrk][0]);

	rpchits_adjusted.push_back(2);
        rpchits_adjusted.push_back(rpc_stat2_cluster_phiBit_adjusted);
        rpchits_adjusted.push_back(rpc_trLctEtaBit_Matt);
        rpchits_adjusted.push_back(trLctSector[nTrk][0]);
        rpchits_adjusted.push_back(trLctSubSector[nTrk][0]);
        rpchits_adjusted.push_back(trLctCSCId[nTrk][0]);
        rpchits_adjusted.push_back(trLctChamber[nTrk][0]);

	// Add rpc variables to csc hit variables vector
	temp_cschits = CSChits;
	temp_cschits_adjusted = CSChits;

	temp_cschits.push_back(rpchits);
	temp_cschits_adjusted.push_back(rpchits_adjusted);

	// Get pt with Matts module
	ptadd address1 = getAddress1(temp_cschits);
	ptadd address0 = getAddress0(temp_cschits);
	CSCTFPtLUT lut = CSCTFPtLUT(LUTparam, scale, ptScale);

	ptadd address1_adjusted = getAddress1(temp_cschits_adjusted);
	ptadd address0_adjusted = getAddress0(temp_cschits_adjusted);

	if (printLevel > 2) {
	  cout << " ----> Calculating pT with PtAddress.h" << endl;
	  cout << "        Cluster Front scaled   = " << scaling(lut.PtReal(address1)) << endl;
	  cout << "        Cluster Rear  scaled   = " << scaling(lut.PtReal(address0)) << endl;
	}
	
	// fill track branches with reco pT
	PtTrk_cluster_front[nTrk] = scaling(lut.PtReal(address1));
	PtTrk_cluster_rear[nTrk]  = scaling(lut.PtReal(address0));
      }

      else {
	PtTrk_cluster_front[nTrk] = -999;
	PtTrk_cluster_rear[nTrk]  = -999;
      }
      // end declustering
      // ========================================================


      
      // Begin pt assignment on all RPC hits 
      for ( auto rpc = rpc_hits->cbegin(); rpc != rpc_hits->cend(); rpc++, RpcTrkId_++) {
	
	if (RpcTrkId_ >= MAX_RPC_LCTS_EVT-1) break;
	
	if (printLevel > 2) cout << "\n   Looping over RPC hit # " << RpcTrkId_ << endl;
	
	// ------------- Fill all RPC variables ------------------
	RPCDetId rpc_id = rpc -> detId<RPCDetId>();
	
	double rpc_gblphi  = rpc->getCMSGlobalPhi();
	double rpc_gbleta  = rpc->getCMSGlobalEta();
	int rpc_station    = rpc_id.station();
	int rpc_sector     = rpc_id.sector();
	int rpc_ring       = rpc_id.ring();
	int rpc_subsector  = rpc_id.subsector();
	unsigned int rpc_layer = rpc->getRPCData().layer;
 	
	// access rpc phi in csc bit form
	int temp_sector = -999;
	if (rpc_sector>6) temp_sector = rpc_sector - 6;
	else temp_sector = rpc_sector;
	float rpc_phiBit = CalcIntegerPhi(rpc_gblphi, temp_sector);
	
	// This is used for PtAddress.h module
        float rpc_trLctEtaBit_Matt = 0;
        for (int iETA = 0; iETA < 15; iETA++) {
          if(fabs(rpc_gbleta) >= etabins[iETA] && fabs(rpc_gbleta) < etabins[iETA+1] ) {
            rpc_trLctEtaBit_Matt = iETA;
            break;
          }
	}

	pt_temp.clear();
	rpchits.clear();
	temp_cschits.clear();
	
	// Check delta r between rpc hit and csc hit # 1
	float dr = sqrt(abs(abs(abs(rpc_gblphi-trLctglobalPhi[nTrk][0])-3.14)-3.14)* \
			abs(abs(abs(rpc_gblphi-trLctglobalPhi[nTrk][0])-3.14)-3.14)+ \
			((rpc_gbleta-trLctglobalEta[nTrk][0])*(rpc_gbleta-trLctglobalEta[nTrk][0])));
 	
	//	double dphi = abs(rpc_gblphi - trLctglobalPhi[nTrk][0]);
	
	if (printLevel > 4) {
	  cout << "  RPC LCT # = " << RpcTrkId_     << endl;
	  cout << "  Phi       = " << rpc_gblphi    << endl;
	  cout << "  Eta       = " << rpc_gbleta    << endl;
	  cout << "  Phi Bit   = " << rpc_phiBit    << endl;
	  cout << "  Eta Bit   = " << rpc_trLctEtaBit_Matt << endl;
	  cout << "  Station   = " << rpc_station   << endl;
	  cout << "  Sector    = " << rpc_sector    << endl;
	  cout << "  SubSector = " << rpc_subsector << endl;
	  cout << "  Layer     = " << rpc_layer     << endl;
	  cout << "  Delta R   = " << dr            << endl;
	  cout << "  Ring      = " << rpc_ring      << endl;
	}
	
	// if the rpc hit is within dr cone, lets see if it is in a different station than the other 2 CSC Lcts
	if ( (rpc_station != trLctStation[nTrk][0]) &&
	     (rpc_station != trLctStation[nTrk][1])     
	     ) {
	  
	  if (printLevel > 2) {
	    cout << "---->Match between RPC and CSC hits. RPC is in different station than CSC hits. " << endl;
	    cout << "  rpc station   = " << rpc_station << endl;
	    cout << "  csc station 1 = " << trLctStation[nTrk][0] << endl;
	    cout << "  csc station 2 = " << trLctStation[nTrk][1] << endl; 
	  }	  
	  
	  rpcIsmatched[nTrk][RpcTrkId_] = 1;	 
	  
	  // Fill CSChits vector with both csc lcts and the matched rpc lct.  Use info of first csc lct as stand in for rpc info.
	  rpchits.push_back(rpc_station);
	  rpchits.push_back(rpc_phiBit);
	  rpchits.push_back(rpc_trLctEtaBit_Matt);
	  rpchits.push_back(rpc_sector);
	  rpchits.push_back(rpc_subsector);
	  rpchits.push_back(trLctCSCId[nTrk][0]);
	  rpchits.push_back(trLctChamber[nTrk][0]);
	  
	  // Add rpc variables to csc hit variables vector
	  temp_cschits = CSChits;
	  
	  temp_cschits.push_back(rpchits);
	  
	  // Get pt with Matts module
	  ptadd address1 = getAddress1(temp_cschits);
	  ptadd address0 = getAddress0(temp_cschits);
	  CSCTFPtLUT lut = CSCTFPtLUT(LUTparam, scale, ptScale);
	  
	  if (printLevel > 3) {
	    cout << " ----> Calculating pT with PtAddress.h" << endl;
	    cout << "        Front scaled   = " << scaling(lut.PtReal(address1)) << endl;
	    cout << "        Rear  scaled   = " << scaling(lut.PtReal(address0)) << endl;
	  }
	  
	  // store pt values in vector
	  pt_temp.push_back(scaling(lut.PtReal(address1)));
	  pt_temp.push_back(scaling(lut.PtReal(address0)));
	  
	  // fill track branches with reco pT
	  PtTrk_reco_front [nTrk][RpcTrkId_] = scaling(lut.PtReal(address1));
	  PtTrk_reco_rear  [nTrk][RpcTrkId_] = scaling(lut.PtReal(address0));
	  
	  sort(pt_temp.begin(), pt_temp.end());
	  int best_pt  = closest(pt_temp, 500);
	  
	  PtTrk_reco_best_two[nTrk][RpcTrkId_] = best_pt;

	} // end if dr < 0.2

	else {
	  PtTrk_reco_front [nTrk][RpcTrkId_]   = -999;
          PtTrk_reco_rear  [nTrk][RpcTrkId_]   = -999;
	  PtTrk_reco_best_two[nTrk][RpcTrkId_] = -999;
	  rpcIsmatched[nTrk][RpcTrkId_]        = -999;
	}
	
	
      } // end loop over rpc lcts
      
    } // end if Lcts == 2
    
    else { 
      
      // fill with dummy values for non matched track
      PtTrk_reco_front [nTrk][RpcTrkId_]   = -999;
      PtTrk_reco_rear  [nTrk][RpcTrkId_]   = -999;
      PtTrk_reco_best_two[nTrk][RpcTrkId_] = -999;
      trkIsMatched     [nTrk] = 0;      
      RpcMatchedID     [nTrk]   = -999;
      
    }


    // ----------------------------------------------------------------------------------------------------------
    // ----------------------------------------------------------------------------------------------------------
    
    
    nTrk += 1;

  } // end loop over csctf tracks

  SizeTrk = nTrk;
  if (printLevel>1) cout << "# of Tracks in event = " <<  SizeTrk << endl;


} // end fillCSCTF method


// ========================   methods for the reco muons collection ======================================
void rpc_tuplizer::fillMuons(const edm::Handle<reco::MuonCollection> muons,
			     edm::Handle<CSCSegmentCollection> cscSegments,
			     const edm::Handle< vector<L1TMuon::InternalTrack> > tracks) {


  int iReco = 0; // keep track of muons in event
  
  for( auto muon_itr = muons->cbegin(); muon_itr < muons->cend(); muon_itr++) {

    if (iReco > 10) break;
   
    if (printLevel>1) cout << "Looping Over Muon # " << iReco << endl;
    
    // look at global or stand alone muons only
    if ( !muon_itr->isGlobalMuon() ) continue;
    
    if (printLevel>1) cout << "----> Muon is GBL\n"; 
    
    //auto muon = muon_itr;

    // Is Gbl matched to csctf track?
    // Returns an int of track matched to muon.  If no match, returns 999.
    int isMatched = rpc_tuplizer::isMuonMatched(muon_itr, cscSegments, tracks, 0);
    
    if (printLevel > 1) cout << " Muon is matched to track = " << isMatched << endl;
    
    muon_match[iReco] = isMatched;
       
    // fill muon variables
    // global muon
    if (muon_itr -> combinedMuon().isNonnull()) {
      
      TrackRef trackRef = muon_itr -> combinedMuon();
      
      muon_pt[iReco]  = trackRef->pt();
      muon_eta[iReco] = trackRef->eta();
      muon_phi[iReco] = trackRef->phi();
      muon_chg[iReco] = trackRef->charge();

      cout << muon_pt[iReco]; 

      if (printLevel>1) {
	cout << "Muon pt = " << muon_pt[iReco] << endl; 
	cout <<"Muon eta = " << muon_eta[iReco] << endl;
	cout <<"Muon phi = " << muon_phi[iReco] << endl;
	cout <<"Muon chg = " << muon_chg[iReco] << endl;
      }
     
      
      iReco++;

    }
    
  } // end reco loop
  
  muonSize = iReco;
  if (printLevel>1) cout << "Muon Size = " << muonSize << endl << endl;
   
} //end fillMuons





// ================== Fill MC Gen SRC ====================================================================
void rpc_tuplizer::fillGenParticles(const edm::Handle<reco::GenParticleCollection> genps,
				    edm::Handle<CSCSegmentCollection> cscSegments,
  				    const edm::Handle< vector<L1TMuon::InternalTrack> > tracks) {
  
  for( auto bgen = genps->cbegin(); bgen < genps->cend(); bgen++ ) {
    
    if( abs(bgen->pdgId()) == 13) {
      
      // fill muon variables
      gen_pt  = bgen->pt();
      gen_eta = bgen->eta();
      gen_phi = bgen->phi();
      gen_chg = bgen->charge();

    
    }
  }
}


#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(rpc_tuplizer);

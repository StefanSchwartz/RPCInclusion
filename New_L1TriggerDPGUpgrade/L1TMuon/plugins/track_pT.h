////////////////////////////////////////////////
// track_pT.h
//
// Imported by rpc_tuplizer.cc
// Works in L1Tmuon Framework
//
// Finds CSC track pT using RPC hits
//
// Written by David Curry
//
///////////////////////////////////////////////

//#include "PtAddress.h"


// function to find closest value to x in a vector
int rpc_tuplizer::closest( vector<int> const& vec, int value) {
  
  if (printLevel > 3) cout << "   Looking for closest value to track pt " << endl;
  
  auto const it = lower_bound(vec.begin(), vec.end(), value);
  if (it == vec.end()) { return vec.back(); }

  return *it;
}


// Takes in csc hits and track pt and checks if PtAddress.h computes correct pT.
// Returns 0 if neither front or rear match
//         1 if front matches
//         2 if rear matches
int rpc_tuplizer::check_track_pt(vector<vector<int>> cschits, 
				 float track_pt) {
  
  int  isMatch  = 999;
  bool isRear   = false;
  bool isFront  = false;
  
  // only look at 3 hit tracks
  if (cschits.size() == 3) {
    
    if (printLevel > 3) cout << "  ---->Looking at 3 hit CSC tracks for check_track_pt() " << endl;
     
    // find pT from PtAddress.h and compare to true track pT
    ptadd address_front = getAddress1(cschits);
    ptadd address_rear = getAddress0(cschits);
    CSCTFPtLUT lut = CSCTFPtLUT(LUTparam, scale, ptScale);
    
    if ( track_pt == scaling(lut.PtReal(address_front)) ) isFront = true;
    
    if ( track_pt == scaling(lut.PtReal(address_rear)) )  isRear = true;
    
    if (!isFront && !isRear) isMatch = 0;
    
    if (isFront) isMatch = 1;
    
    if (isRear) isMatch = 2;
    
    if (printLevel > 3) {
      cout << "          ----> Calculating pT with PtAddress.h" << endl;
      cout << "         front scaled  = " << scaling(lut.PtReal(address_front)) << endl;
      cout << "         rear  scaled  = " << scaling(lut.PtReal(address_rear)) << endl;
      cout << "             Actual pT = " << track_pt << endl;
      cout << "             isMatch   = " << isMatch << endl << endl;
    }
    
  }

  return isMatch;

} // end check_track_pt


// method that takes in 3 hit csc tracks and returns a pT estimate when using one rpc hit in place of a csc hit
// Now takes in rpc_cluster phi from station 2 for more checks
void rpc_tuplizer::three_track_pt(vector<vector<int>> csc_hits, 
				  int nTrk,
				  int trkPt,
				  int rpc_cluster_phiBit,
				  double rpc_cluster_phi,
				  double rpc_cluster_eta,
				  const edm::Handle< vector<L1TMuon::TriggerPrimitive> > rpc_hits ) {
    
    if (printLevel>3) cout << "   ---->Looking at 3 hit tracks for pT assignment with three_track_pt() " << endl;
    
    // loop over rpc hits and look for a match with csc track LCT
    int RpcTrkId_ = 0 ;
    vector<int> rpchits;
    vector<vector<int>> temp_cschits;
    vector<int> pt_temp;
    

    for ( auto rpc = rpc_hits->cbegin(); rpc < rpc_hits->cend(); rpc++, RpcTrkId_++) {

      if (RpcTrkId_ >= MAX_RPC_LCTS_EVT-1) continue;
     
      // ------------- Fill all RPC variables ------------------
      RPCDetId rpc_id = rpc -> detId<RPCDetId>();

      double rpc_gblphi  = rpc->getCMSGlobalPhi();
      double rpc_gbleta  = rpc->getCMSGlobalEta();
      int rpc_station    = rpc_id.station();
      int rpc_sector     = rpc_id.sector();
      int rpc_ring       = rpc_id.ring();


      // Find rpc phi in csc bit form
      // Send look up table adjusted sector(1-6 only)
      int temp_sector = -999;
      if (rpc_sector>6) temp_sector = rpc_sector - 6;
      else temp_sector = rpc_sector;
      float rpc_phiBit = CalcIntegerPhi(rpc_gblphi, temp_sector);

      rpc_gblEta[RpcTrkId_]  = rpc_gbleta;
      rpc_gblPhi[RpcTrkId_]  = rpc_gblphi;
      rpc_Station[RpcTrkId_] = rpc_station;
      rpc_Sector[RpcTrkId_]  = rpc_sector;
      rpc_Phibit[RpcTrkId_]  = rpc_phiBit;
      
      
      if (printLevel > 3) {
	cout << "\n   Looping over RPC hit # " << RpcTrkId_ << endl;
	cout << "   Rpc Station = " << rpc_station << endl;
	cout << "   Rpc Sector  = " << rpc_sector  << endl;
	cout << "   Rpc gbl Phi  = " << rpc_gblphi  << endl;
	cout << "   Rpc gbl Eta  = " << rpc_gbleta  << endl;
	cout << "   Rpc ring     = " << rpc_ring << endl;
      }
      
      
      // Check delta r between rpc hit and csc hits
      for (int iLct = 0; iLct < 3; iLct++) {
	
	if (printLevel > 3) cout << "\n   Looping over CSC Lct # " << iLct << endl;
	
	rpchits.clear();
	temp_cschits.clear();
	pt_temp.clear();
	
	float dr = sqrt(abs(abs(abs(rpc_gblphi-trLctglobalPhi[nTrk][iLct])-3.14)-3.14)*	\
			abs(abs(abs(rpc_gblphi-trLctglobalPhi[nTrk][iLct])-3.14)-3.14)+	\
			((rpc_gbleta-trLctglobalEta[nTrk][iLct])*(rpc_gbleta-trLctglobalEta[nTrk][iLct])));

	if (printLevel > 3) {
	  cout << "    Delta r = " << dr << endl; 
	  cout << "    Lct Staion  = " << trLctStation[nTrk][iLct] << endl;
	  cout << "    Lct Sector  = " << trLctSector[nTrk][iLct]  << endl;
	}
	
	if ( dr > 0.1 ) {
	  if (printLevel > 3) cout << "   Delta r is too large. Go to next Lct" << endl << endl;
	  csc_rpc_match[nTrk][iLct]      = -999;
          PtTrk_reco_rpc_csc[nTrk][iLct] = -999;
	  continue;
	}
	
	if (rpc_station != trLctStation[nTrk][iLct]) {
	  if (printLevel > 3) cout << "   Stations are different. Go to next Lct" << endl << endl;
	  csc_rpc_match[nTrk][iLct]      = -999;
          PtTrk_reco_rpc_csc[nTrk][iLct] = -999;
	  continue;
	}
	
	if ( (rpc_sector != trLctSector[nTrk][iLct]) && (rpc_sector != trLctSector[nTrk][iLct]-6) ) {
          if (printLevel > 3) cout << "   Sectors are different. Go to next Lct" << endl << endl;
	  csc_rpc_match[nTrk][iLct]      = -999;
          PtTrk_reco_rpc_csc[nTrk][iLct] = -999;
          continue;
	}
	
	if (printLevel > 3) cout << "     RPC is matched to LCT # " << iLct << endl;
	csc_rpc_match[nTrk][iLct] = 1;
	csc_rpc_id[nTrk][iLct] = RpcTrkId_;
	

	// Fill vector to use in PtAddress.h
	rpchits.push_back(trLctStation[nTrk][iLct]);
	rpchits.push_back(rpc_phiBit);
	//rpchits.push_back(trLctPhiBit[nTrk][iLct]);
	rpchits.push_back(trLctEtaBitMatt[nTrk][iLct]);
	rpchits.push_back(trLctSector[nTrk][iLct]);
	rpchits.push_back(trLctSubSector[nTrk][iLct]);
	rpchits.push_back(trLctCSCId[nTrk][iLct]);
	rpchits.push_back(trLctChamber[nTrk][iLct]);
	
	// Now swap out lct with matched rpc hit
	temp_cschits = csc_hits;
	
	temp_cschits.at(iLct) = rpchits;
	
	ptadd address1 = getAddress1(temp_cschits);
	ptadd address0 = getAddress0(temp_cschits);
	CSCTFPtLUT lut = CSCTFPtLUT(LUTparam, scale, ptScale);
	
	if (printLevel > 3) {
	  cout << "      ----> Calculating pT with PtAddress.h" << endl;
	  cout << "       front scaled  = " << scaling(lut.PtReal(address1)) << endl;
	  cout << "       rear  scaled  = " << scaling(lut.PtReal(address0)) << endl;
	  cout << "       Actual pT = " << trkPt << endl;
	}
	
	// store pt values in vector
	pt_temp.push_back(scaling(lut.PtReal(address1)));
	pt_temp.push_back(scaling(lut.PtReal(address0)));
	
	// Which is better, front or rear?
	sort(pt_temp.begin(), pt_temp.end());
	int best_pt  = closest(pt_temp, trkPt);
	
	if (printLevel > 3) {
	  cout << "    Best Pt match = " << best_pt << endl;
	  cout << "    Pt Diff = " << best_pt - trkPt << endl;
	}

	
	// store in branch
	//if (abs(best_pt - trkPt) < 10) 
	PtTrk_reco_rpc_csc[nTrk][iLct] = best_pt;
	//else PtTrk_reco_rpc_csc[nTrk][iLct] = -999;
	

      } // end loop over Lcts
      
    } // end loop over rpc hits


    // Now do the same process for rpc cluster phi
    // Check delta r between rpc hit and csc hits
    for (int iLct = 0; iLct < 3; iLct++) {

      if (printLevel > 3) cout << "\n   Looping over CSC Lct # " << iLct << endl;

      rpchits.clear();
      temp_cschits.clear();
      pt_temp.clear();
      
      if (trLctStation[nTrk][iLct] != 2) continue;

      float dr = sqrt(abs(abs(abs(rpc_cluster_phi-trLctglobalPhi[nTrk][iLct])-3.14)-3.14)* \
		      abs(abs(abs(rpc_cluster_phi-trLctglobalPhi[nTrk][iLct])-3.14)-3.14)+ \
		      ((rpc_cluster_eta-trLctglobalEta[nTrk][iLct])*(rpc_cluster_eta-trLctglobalEta[nTrk][iLct])));

      if (printLevel > 3) {
	cout << "    Delta r = " << dr << endl;
	cout << "    Lct Station  = " << trLctStation[nTrk][iLct] << endl;
	cout << "    Lct Sector  = " << trLctSector[nTrk][iLct]  << endl;
      }

      if ( dr > 0.01 ) {
	if (printLevel > 3) cout << "   Delta r is too large. Go to next Lct" << endl << endl;
	PtTrk_threeHit_rpc_cluster[nTrk][iLct] = -999;
	continue;
      }
      
      // Fill vector to use in PtAddress.h
      rpchits.push_back(trLctStation[nTrk][iLct]);
      rpchits.push_back(rpc_cluster_phiBit);
      //rpchits.push_back(trLctPhiBit[nTrk][iLct]);
      rpchits.push_back(trLctEtaBitMatt[nTrk][iLct]);
      rpchits.push_back(trLctSector[nTrk][iLct]);
      rpchits.push_back(trLctSubSector[nTrk][iLct]);
      rpchits.push_back(trLctCSCId[nTrk][iLct]);
      rpchits.push_back(trLctChamber[nTrk][iLct]);

      // Now swap out lct with matched rpc hit
      temp_cschits = csc_hits;

      temp_cschits.at(iLct) = rpchits;

      ptadd address1 = getAddress1(temp_cschits);
      ptadd address0 = getAddress0(temp_cschits);
      CSCTFPtLUT lut = CSCTFPtLUT(LUTparam, scale, ptScale);

      if (printLevel > 3) {
	cout << "      ----> Calculating pT with PtAddress.h" << endl;
	cout << "       front scaled  = " << scaling(lut.PtReal(address1)) << endl;
	cout << "       rear  scaled  = " << scaling(lut.PtReal(address0)) << endl;
	cout << "       Actual pT = " << trkPt << endl;
      }

      // store pt values in vector
      pt_temp.push_back(scaling(lut.PtReal(address1)));
      pt_temp.push_back(scaling(lut.PtReal(address0)));
      
      // Which is better, front or rear?
      sort(pt_temp.begin(), pt_temp.end());
      int best_pt  = closest(pt_temp, trkPt);

      if (printLevel > 3) {
	cout << "    Best Pt match = " << best_pt << endl;
	cout << "    Pt Diff = " << best_pt - trkPt << endl;
      }
      
      // store in branch
      PtTrk_threeHit_rpc_cluster[nTrk][iLct] = best_pt;
      
    } // end loop over Lcts
      
    return;
    
} // end three_track_pt


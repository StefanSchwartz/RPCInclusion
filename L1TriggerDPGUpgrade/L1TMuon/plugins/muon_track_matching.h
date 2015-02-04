////////////////////////////////////////////////
// muon_track_matching.h
//
//
// Imported by rpc_tuplizer.cc
// Works in L1Tmuon Framework
//
// Matched a global muon to a track.  Returns pos or neg matching
//
// Written by David Curry
//
///////////////////////////////////////////////

#include "SegmentLCTMatchBox.h"
#include "rpc_tuplizer.h"

// =================================================================================================================
// ===== Classes/objects used in segment matching ======

// class container for the segment and number of hits matching the global muon
class Segment {
 public:
  const CSCSegment cscsegcand;
  int nMatchedHits;
  Segment(const CSCSegment &seg, int nMHits):cscsegcand(seg), nMatchedHits(nMHits){};
};

typedef std::vector<Segment*> SegmentVector;

SegmentVector* SegmentsInMuon(const reco::Muon* muon, const CSCSegmentCollection* segments, int printLevel);

// importing directly Ivan's object...
SegmentLCTMatchBox _matchBox;

// =================================================================================================================

// Method to take in a muon, loop over its segments, and match it to csctrf track lcts
// Returns an int: 999 is muon is not matched to any track, or the track number if matched to a track.


int rpc_tuplizer::isMuonMatched(__gnu_cxx::__normal_iterator<const reco::Muon*, std::vector<reco::Muon> >& muon,
				 edm::Handle<CSCSegmentCollection> cscSegments,
				 const edm::Handle< vector<L1TMuon::InternalTrack> > CSCTFtracks,
				 int printLevel) {
  
      
  // get the segments which match the muon candidate.  Comes from class defined below
  SegmentVector *segVect = SegmentsInMuon( &(*muon), &(*cscSegments), printLevel );
  
  if (printLevel > 1) {
    cout << " ----> Matching GBL muon to CSCTF muon(track)\n";
    cout << "The muon has " << segVect -> size() << " csc segments" << endl;
  }  
  
    int iSegment=-1;
    int which_track = 999;
    // loop over the CSC segments
    if (printLevel > 1) cout << "Looping over the CSC segments\n\n";
    
    for (auto segmentCSC = segVect -> begin(); segmentCSC != segVect->end(); iSegment++, segmentCSC++){
      
    if (printLevel > 1) {
        printf("#############\n");
        printf("Segment  #%d\n", iSegment);
        printf("#############\n\n");
	
        if ( iSegment > (MAX_SEGS_STD-1) ) {
          cout << "the muon has " << iSegment+1 << ", but the MAX allowed is "
               << MAX_SEGS_STD << " -> Skipping the segment... " << endl;
          continue;
        }
      }

      int nTrk=0;

      // Loop over csctf tracks and access all lcts
      for( auto trk = CSCTFtracks->cbegin(); trk < CSCTFtracks->cend(); trk++, nTrk++){

	if (printLevel > 1) cout << "Looping over track # " << nTrk << endl;

	int num_segs_matched = 0; // # of segments matched to this track

	auto lct_map = trk -> getStubs();  // Get map for LCTs to stations
	
	// Loop over stations and get all lcts in each one
	for( unsigned station = 1; station <= 4; ++station ) {
	  
	  const unsigned id    = 4*L1TMuon::InternalTrack::kCSC+station-1; // unique identifier for each csc station

	  auto x_LCTs = lct_map[id];  // access the a reference to vector<lcts> for a given station id
	  
	  // is the segment matched to an LCT?
	  bool isLCTMatched  = _matchBox.isMatched( (*segmentCSC)->cscsegcand, x_LCTs, 0);

	  if (isLCTMatched) num_segs_matched++;

	  if (printLevel > 1) {
	    cout << "isLCTMatched = " << isLCTMatched << endl;
	  }
	
	} // end loop over csc stations
	
	if (printLevel>1) cout << " Number of matched segments in track # " << nTrk << " = " << num_segs_matched << endl;

	if (num_segs_matched > 1) {
	  
	  if (printLevel>1) cout << " ----> This muon is matched to this track!\n";
	  
	  which_track = nTrk;
	}

      } // loop over tracks
      
    } // end loop over segments
    
    if (printLevel>1) cout << "  All loops have ended.  This muon is matched to track = " << which_track << endl;
        
    return which_track;
    
}



SegmentVector* SegmentsInMuon(const reco::Muon* muon,
			      const CSCSegmentCollection* segments,
			      int printLevel) {

  SegmentVector *retVal = new SegmentVector();

  if (printLevel > 1) cout << "\nSegmentsInMuon Method\n";

  bool isMuonStd=false; // has the muon a standalone component
  if (muon->combinedMuon().isNonnull() || muon->standAloneMuon().isNonnull())
    isMuonStd=true;

  // return empty vector if the muon is not interesting
  if (!isMuonStd) return retVal;
  int nMuonMatchedHits=0;
  int iSegment=0;
  // --------- Loop over the CSC segments -------------
  for (auto segIter = segments->begin(); segIter != segments->end(); segIter++){

    int nHits=segIter -> nRecHits();

    if (printLevel > 2) {
      cout << " ======================================== " << endl;
      cout << "Segment in CSC:" << iSegment++ << endl;
      cout << "# segs hits:"    << nHits      << endl;
    }

    const std::vector<CSCRecHit2D>& theHits = segIter -> specificRecHits();
    std::vector<CSCRecHit2D>::const_iterator hitIter;

    int iHit=0;
    // loop over the segments hits
    for (hitIter = theHits.begin(); hitIter!= theHits.end(); hitIter++){

      if (printLevel>2) std::cout << "iHit:" << iHit++ << ", ";

      // check if the hit will match the standalone muon component
      bool isHitMatched=false;

      LocalPoint seghitlocal = hitIter -> localPosition();

      double segHitX = seghitlocal.x();
      double segHitY = seghitlocal.y();

      if (printLevel > 2)
	std::cout << "segHitX="<<segHitX <<  ", "
                  << "segHitY="<<segHitY;


      // The muon now returns segments (2012), while in 2010 it was returning hits...
      for(trackingRecHit_iterator segm  = muon->outerTrack()->recHitsBegin();
          segm != muon->outerTrack()->recHitsEnd();
          segm++){

	// some basic protection
        if ( !((*segm)->isValid()) ) continue;

        // Hardware ID of the RecHit (in terms of wire/strips/chambers)
        DetId detid = (*segm)->geographicalId();

        // Interested in muon systems only
        if( detid.det() != DetId::Muon ) continue;

        //Look only at CSC Hits (CSC id is 2)
        if (detid.subdetId() != MuonSubdetId::CSC) continue;

        CSCDetId id(detid.rawId());

        // another sanity check
	if  (id.station() < 1) continue;

	// get the CSCSegment
        const CSCSegment* cscSegment = dynamic_cast<const CSCSegment*>(&**segm);
        // check the segment is not NULL
	if (cscSegment == NULL) continue;

	// try to get the CSC recHits that contribute to this segment.
	std::vector<CSCRecHit2D> theseRecHits = (*cscSegment).specificRecHits();

	// loop over the rechits
        for ( std::vector<CSCRecHit2D>::const_iterator iRH = theseRecHits.begin();
              iRH != theseRecHits.end(); iRH++) {

          // get the rechit ID
          //CSCDetId idRH = (CSCDetId)(*iRH).cscDetId();

          // CSC chamber
	  //  const CSCChamber* cscchamber = cscGeom->chamber(idRH);
          //if (!cscchamber) continue;
	  // local position
          LocalPoint rhitlocal = iRH->localPosition();

          if (segHitX==rhitlocal.x() &&
              segHitY==rhitlocal.y()  )
            isHitMatched=true;
        } // end loop over hits of a segment

      } // end loop trackingRecHit_iterator (segments of a muon)

      if (printLevel > 2 ){
        if (isHitMatched) cout<< " -> Matched" << endl;
        else              cout<< " -> NOT Matched" << endl;
      }

      if (isHitMatched) nMuonMatchedHits++;

    }

    if (printLevel > 2) cout<< "segment has "  << nMuonMatchedHits
			    << " hits out of " << nHits
			    << " matched"      << endl;


    // fill the the vector with the matching segments
    if (nMuonMatchedHits!=0) {
      Segment* segment = new Segment(*segIter, nMuonMatchedHits);
      retVal->push_back(segment);
    }

  }  // end loop on the CSC segments
  return retVal;

} // end function

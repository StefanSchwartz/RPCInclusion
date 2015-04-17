#Same as twoHit_RPC but for three hit tracks

import sys
from ROOT import *
from array import *
import numpy as np
from collections import Counter
from matplotlib import interactive



print '\n\n ======== Good Luck! =======\n\n'

# Set the print level. Default = 0
if len(sys.argv) is 1: printLevel = 0
else: printLevel = sys.argv[1]

# Import Root file
file = TFile('singleMu_lowPt_clustering_4_9_15.root')


# Set the branch address of TTree in Tfile
evt = file.Get("CSCtree")

# bind methods to improve speed
getEntry = evt.GetEntry

# Counters
rpc_count = Counter()

#Function to make Phi positive
def positivePhi(phi):

    while phi < 0:
	phi += 2 * np.pi

    return phi


# ================ Histograms =============================================
# =========================================================================
newfile = TFile('threeHit_RPC_pt_plots.root','recreate')

nbins = 100

# dphi vs inv muon pt and dphi vs pt
hdphi12_Pt = TH2F('hdphi12_Pt', '', nbins/2, 0, 120, nbins/2, -0.1, 0.1)
hdphi12_invPt = TH2F('hdphi12_invPt', '', nbins, -0.3, 0, nbins/2, -0.1, 0.1)
hdphi12_Pt_Prof = TProfile('hdphi12_Pt_Prof', '', nbins/2, 0, 120, -0.1, 0.1)
hdphi12_invPt_Prof = TProfile('hdphi12_invPt_Prof', '', nbins, -0.3, 0, -0.1, 0.1)

# rate
rate = [0, 3, 5, 7, 10, 12, 16, 20, 30]
rate_array = array('d', rate)
n_rate = len(rate)

hrate_mode2 = TH1F('hrate_mode2', '', n_rate-1, rate_array)

# efficiency(turn on)
pt_thresh = [7., 10., 12., 16.]
n_thresh = len(pt_thresh)

ptbin = [2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 16, 20, 50, 140]
ptbin_array = array('d', ptbin)
n_ptbin = len(ptbin)

csctfPt = [0]*n_thresh

csctfPt_all = TH1F('csctfPt_all', '', n_ptbin-1, ptbin_array)

for ihist, thresh in enumerate(pt_thresh):
    csctfPt[ihist]     = TH1F('csctfPt_'+str(thresh), '', n_ptbin-1, ptbin_array)

# final turn on curves    
Pt_turn     = [0]*n_thresh

hist_list = [hdphi12_Pt, hdphi12_invPt, hdphi12_Pt_Prof, hdphi12_invPt_Prof, hrate_mode2, csctfPt_all]

# =====================================================================================



# ===============================================
# Loop over over events in TFile
for iEvt in range(evt.GetEntries()):
    
    getEntry(iEvt)
    
    if iEvt % 10000 is 0: print 'Event #', iEvt

    if printLevel > 0: print '\n============== New Event # ', evt.Event, ' ================='
    
    if evt.gen_pt < 3: continue
    if abs(evt.gen_eta) < 0.9 or abs(evt.gen_eta) > 2.1: continue
        
    rpc_count['gbl_mouns'] += 1
        
    # Each muon starts as not matched to a track
    isMatched = False
    best_dr = []
        
    # Loop over csc tracks
    for iCSCTrk in range(evt.SizeTrk):
        
        if printLevel > 0: print '\n====== Looping over Track # ', iCSCTrk, ' ======='
        
        # find which track, if any, is matched to muon.
        dr = abs( evt.gen_eta - evt.EtaTrk[iCSCTrk]) 
                    
        if dr < 0.2:
            
            if printLevel > 0:
                print 'Muon # ', 1, ' is matched to Track # ', iCSCTrk
                print 'Delta R  = ', dr
                
            isMatched = True
            best_dr.append(dr)
            chg_muon = evt.gen_chg
            pt_muon  = evt.gen_pt
                
        else: best_dr.append(999)
    
    #end track loop
        
    # skip muons who are not matched to a track
    if not isMatched: continue
      
    # returns which track # is matched to current muon
    iCSCTrk = best_dr.index(min(best_dr))
      
    if printLevel > 1: print 'Best delta R match is Track # ', iCSCTrk
      
    rpc_count['all_matched_tracks'] += 1
      
    # Look only at 3 hit tracks
    if evt.NumLctsTrk[iCSCTrk] is not 3: continue
      
    rpc_count['3hit_matched_tracks'] += 1
      
    # =============== What mode is track? ================
    # Possible modes are station combinations:
    # 1-2-3  Mode 2 sum 6
    # 1-2-4  Mode 3 sum 7
    # 1-3-4  Mode 4 sum 8
    # 2-3-4  Mode 5 sum 9
      
    temp_mode = 0; track_mode = 0
      
    # loop over tracks hits to find mode
    for iCsc in range(evt.NumLctsTrk[iCSCTrk]):       
        temp_mode += evt.trLctStation[iCSCTrk*4 + iCsc]
    
    # Which mode is track
    if temp_mode is 6: track_mode = 2
    if temp_mode is 7: track_mode = 3
    if temp_mode is 8: track_mode = 4
    if temp_mode is 9: track_mode = 5
            
    if printLevel > 0: print ' This track is mode =', track_mode, '\n'
       
    if track_mode != 2: continue
       
    rpc_count['mode2_trks'] += 1

    # loop over csc Lcts
    for iCsc in range(evt.NumLctsTrk[iCSCTrk]):
            
        if printLevel>0:
            print 'Looping over CSC Lct # ', iCsc,' in station # ', evt.trLctStation[iCSCTrk*4 + iCsc]
            print 'Gbl Phi = ', evt.trLctglobalPhi[iCSCTrk*4 +iCsc]
            print 'Gbl Eta = ', evt.trLctglobalEta[iCSCTrk*4 +iCsc]

        if evt.trLctStation[iCSCTrk*4 + iCsc] is 1:
            csc_phi1 = evt.trLctglobalPhi[iCSCTrk*4 +iCsc]
            csc_eta1 = evt.trLctglobalEta[iCSCTrk*4 +iCsc]

        if evt.trLctStation[iCSCTrk*4 + iCsc] is 2:
            csc_phi2 = evt.trLctglobalPhi[iCSCTrk*4 +iCsc]
            csc_eta2 = evt.trLctglobalEta[iCSCTrk*4 +iCsc]
                
        if evt.trLctStation[iCSCTrk*4 + iCsc] is 3:
            csc_eta3 = evt.trLctglobalEta[iCSCTrk*4 +iCsc]
            csc_phi3 = evt.trLctglobalPhi[iCSCTrk*4 +iCsc]
        
    # end loop over CSC Lcts

    printCondition = False

    #Converts all phi values to positive
    csc_phi1 = positivePhi(csc_phi1)
    csc_phi2 = positivePhi(csc_phi2)	
    csc_phi3 = positivePhi(csc_phi3)
        
    #Looking at the change in phi without the rpc information
    dphi12 = abs(csc_phi2 - csc_phi1)    
    Trk = evt.PtTrk[iCSCTrk]

    if (dphi12 > np.pi): 
	dphi12 = 2*np.pi - dphi12

    #Forcing positive for right now, so this will never be true
    #if (dphi12 < (-1*np.pi)):
    #	dphi12 = dphi12 + 2*np.pi
                
    # invPt plots
    hdphi12_Pt.Fill(Trk, dphi12)
    hdphi12_invPt.Fill(chg_muon/Trk, dphi12)
    hdphi12_Pt_Prof.Fill(Trk, dphi12)
    hdphi12_invPt_Prof.Fill(chg_muon/Trk, dphi12)

    if (dphi12 < 0):
	printCondition = True

    if (printCondition is True):
    	print '\n dphi: ', dphi12
	print 'PtTrk: ', Trk
	print 'Reco Pt: ', pt_muon
	print 'CSC 1: (', csc_eta1, ',', csc_phi1, ')'
	print 'CSC 2: (', csc_eta2, ',', csc_phi2, ')'
	print 'CSC 3: (', csc_eta3, ',', csc_phi3, ')'

        
    # rate
    for bin in reversed(rate):            
    	if Trk > bin: hrate_mode2.Fill(bin)

    # turn on curves        
    csctfPt_all.Fill(pt_muon)
        
    for ihist, thresh in enumerate(pt_thresh):
            
    	if Trk >= thresh:
        	csctfPt[ihist].Fill(pt_muon)                
                
    # end turn on curves
# end event loop

#  ======== Write Hists ==========

# Little more work for turn on curves
for ihist, thresh in enumerate(pt_thresh): 
    
    csctfPt[ihist].Write()
    
    Pt_turn[ihist] = TGraphAsymmErrors(csctfPt[ihist], csctfPt_all)    
    
for hist in hist_list:
    if isinstance(hist, list):
        newfile.mkdir('%s' % hist[0].GetName()).cd()
        for i in hist: i.Write()
    else: hist.Write()

del newfile

# ================================



# ========= Results ===========
print '\n\n=========== Analysis Results ============'

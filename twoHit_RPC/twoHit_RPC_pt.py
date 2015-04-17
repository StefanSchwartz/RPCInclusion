########################################################
## A script to Import ML coefficients and predict RPC pt
##
## By David Curry
##
########################################################

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

#Fucntion to compute delta r
def deltaR(phi1, phi2, eta1, eta2):
    
    deta = eta1 - eta2
    dphi = phi1 - phi2
    if dphi > np.pi: dphi - np.pi
    
    return sqrt(dphi*dphi + deta*deta)

#Function to make Phi positive
def positivePhi(phi):

    while phi < 0:
	phi += 2 * np.pi

    return phi


# ================ Histograms =============================================
# =========================================================================
newfile = TFile('twoHit_RPC_pt_plots.root','recreate')

hrpc_hits = TH1F('hrpc_hits', '', 20, 0, 20)

nbins = 100

# delta phi
hdphi_cluster_csc1_plus  = TH1F('hdphi_cluster_csc1_plus', '', nbins, -0.1, 0.1)
hdphi_cluster_csc1_minus = TH1F('hdphi_cluster_csc1_minus','', nbins, -0.1, 0.1)

hdphi_cluster_csc3_plus  = TH1F('hdphi_cluster_csc3_plus','', nbins, -0.1, 0.1)
hdphi_cluster_csc3_minus = TH1F('hdphi_cluster_csc3_minus','', nbins, -0.1, 0.1)

# delta phi vs inv muon pt
hdphi_cluster2_csc1_invPt = TH2F('hdphi_cluster2_csc1_invPt', '', nbins, -0.3, 0, nbins, -0.1, 0.1)
hdphi_cluster2_csc3_invPt = TH2F('hdphi_cluster2_csc3_invPt', '', nbins, -0.3, 0.3, nbins, -0.3, 0.3)
hdphi_cluster2_csc1_invPt_Prof = TProfile('hdphi_cluster2_csc1_invPt_Prof', '', nbins, -0.3, 0, -0.1, 0.1)

# delta phi vs muon pt
hdphi_cluster2_csc1_Pt = TH2F('hdphi_cluster2_csc1_Pt', '', nbins/2, 0, 120, nbins/2, -0.1, 0.1)
hdphi_cluster2_csc1_Pt_Prof = TProfile('hdphi_cluster2_csc1_Pt_Prof', '', nbins/2, 0, 120, -0.1, 0.1)

# resolution
hres     = TH1F('hres', '', 50, -2, 2)
hres_rpc_cluster = TH1F('hres_rpc_cluster', '', 50, -2, 2)
hres_rpc_cluster_rear = TH1F('hres_rpc_cluster_rear', '', 50, -2, 2)

# rate
rate = [0, 3, 5, 7, 10, 12, 16, 20, 30]
rate_array = array('d', rate)
n_rate = len(rate)

hrate_mode2     = TH1F('hrate_mode2', '', n_rate-1, rate_array)
hrate_rpc_mode2 = TH1F('hrate_rpc_mode2', '', n_rate-1, rate_array)
hrate_rpc_mode2_rear = TH1F('hrate_rpc_mode2_rear', '', n_rate-1, rate_array)


# efficiency(turn on)
pt_thresh = [7., 10., 12., 16.]
n_thresh = len(pt_thresh)

ptbin = [2, 3, 4, 5, 6, 7, 8, 9, 10, 12, 16, 20, 50, 140]
ptbin_array = array('d', ptbin)
n_ptbin = len(ptbin)

csctfPt = [0]*n_thresh
csctfPt_rpc = [0]*n_thresh

csctfPt_all = TH1F('csctfPt_all', '', n_ptbin-1, ptbin_array)

for ihist, thresh in enumerate(pt_thresh):
    csctfPt[ihist]     = TH1F('csctfPt_'+str(thresh), '', n_ptbin-1, ptbin_array)
    csctfPt_rpc[ihist] = TH1F('csctfPt_rpc_'+str(thresh), '', n_ptbin-1, ptbin_array)

# final turn on curves    
Pt_turn     = [0]*n_thresh
Pt_turn_rpc = [0]*n_thresh

# cluster size plots
hcluster_size = TH1F('hcluster_size', '', 10, 0, 10)
#hcluster_size_allstations = TH1F('hcluster_size_allstations', '', 10, 0, 10)
hcluster_size_outliers = TH1F('hcluster_size_outliers', '', 10, 0, 10)
#hcluster_size_outliers_allstations = TH1F('hcluster_size_outliers_allstations', '', 10, 0, 10)


hist_list = [hres, hres_rpc_cluster, hres_rpc_cluster_rear, hrate_mode2, hrate_rpc_mode2, hrpc_hits, \
             hrate_rpc_mode2_rear, hdphi_cluster_csc1_plus, hdphi_cluster_csc1_minus, hdphi_cluster_csc3_plus, hdphi_cluster_csc3_minus,\
             hdphi_cluster2_csc1_invPt, hdphi_cluster2_csc3_invPt, hdphi_cluster2_csc1_invPt_Prof, hdphi_cluster2_csc1_Pt, hdphi_cluster2_csc1_Pt_Prof, csctfPt_all, \
             hcluster_size, hcluster_size_outliers]

# =====================================================================================



# ===============================================
# Loop over over events in TFile
for iEvt in range(evt.GetEntries()):
    
    getEntry(iEvt)
    
    if iEvt % 10000 is 0: print 'Event #', iEvt

    if printLevel > 0: print '\n============== New Event # ', evt.Event, ' ================='


    # ===== for testing RPCs ======
    
    #print '\n rpc cluster 1 phi: ', evt.rpc_stat1_cluster_phi  
    #print 'rpc cluster 2 phi: ', evt.rpc_stat2_cluster_phi
    #print 'rpc cluster 3 phi: ', evt.rpc_stat3_cluster_phi
    #print 'rpc cluster 4 phi: ', evt.rpc_stat4_cluster_phi,
    
    # =============================
    
    if evt.gen_pt < 3: continue
    if abs(evt.gen_eta) < 0.9 or abs(evt.gen_eta) > 2.1: continue

        
    rpc_count['gbl_mouns'] += 1
        
    # Each muon starts as not matched to a track
    isMatched = False
    best_dr = []


    #hcluster_size_allstations.Fill(evt.cluster_size)
    #hcluster_size_outliers_allstations.Fill(evt.cluster_size)
        
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
      
    # Look only at 2 hit tracks
    if evt.NumLctsTrk[iCSCTrk] is not 2: continue
      
    rpc_count['2hit_matched_tracks'] += 1
      
    # =============== What mode is track? ================
    # Possible modes are station combinations:
    # 1-2  Mode 1 sum 3
    # 1-3  Mode 2 sum 4
    # 1-4  Mode 3 sum 5
    # 2-3  Mode 4 sum 5
    # 2-4  Mode 5 sum 6
    # 3-4  Mode 6 sum 7
      
    temp_mode = 0; track_mode = 0
    isStation_1 = False; isStation_2 = False
      
    # loop over tracks hits to find mode
    for iCsc in range(evt.NumLctsTrk[iCSCTrk]):
          
        temp_mode += evt.trLctStation[iCSCTrk*4 + iCsc]
        if evt.trLctStation[iCSCTrk*4 + iCsc] == 1: isStation_1 = True
        if evt.trLctStation[iCSCTrk*4 + iCsc] == 2: isStation_2 = True
    
    # Which mode is track
    if temp_mode is 3: track_mode = 1
    if temp_mode is 4: track_mode = 2
    if temp_mode is 6: track_mode = 5
    if temp_mode is 7: track_mode = 6
        
    # In the event of mode 5 we need to find which configuration
    if track_mode is 0 and isStation_1: track_mode = 3
    elif track_mode is 0: track_mode = 4
            
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
                
        if evt.trLctStation[iCSCTrk*4 +iCsc] is 3:
            csc_eta3 = evt.trLctglobalEta[iCSCTrk*4 +iCsc]
            csc_phi3 = evt.trLctglobalPhi[iCSCTrk*4 +iCsc]
        
    # end loop over CSC Lcts
            
    rpc_pt = -99
    rpc_count['temp'] = 0



    # ================= NEW ====================
    # ==========================================
    # RPC Declustering:  all rpc hits in a station have been reduced to one phi/eta value.  track pt is stored in tuple.
        
    if evt.rpc_stat2_cluster_phi != -999:
	printCondition = False
            
        rpc_count['mode2_trks_w/rpc2_cluster'] += 1

	Trk = evt.PtTrk[iCSCTrk]
            
        rpc_cluster_pt      = evt.PtTrk_cluster_front[iCSCTrk];
        rpc_cluster_pt_rear = evt.PtTrk_cluster_rear[iCSCTrk];

	rpc_phi = evt.rpc_stat2_cluster_phi
	rpc_eta = evt.rpc_stat2_cluster_eta

	#Converts all phi values to positive
	csc_phi1 = positivePhi(csc_phi1)	
	csc_phi3 = positivePhi(csc_phi3)
	rpc_phi = positivePhi(rpc_phi)
            
        dphi12_cluster = abs(rpc_phi - csc_phi1)
        dphi23_cluster = abs(rpc_phi - csc_phi3)
            
        if (dphi12_cluster > np.pi): 
		dphi12_cluster = 2*np.pi - dphi12_cluster
        if (dphi23_cluster > np.pi): 
		dphi23_cluster = 2*np.pi - dphi23_cluster

	if (dphi12_cluster < 0):
		printCondition = True

	if (printCondition is True):
		print '\n dphi: ', dphi12_cluster
		print 'cluster pt: ', rpc_cluster_pt
		print 'reco pt: ', pt_muon

        if printLevel > 0: print '\n ====== RPC Cluster Values ======'
        if printLevel > 0: print 'RPC Station 2 Cluster Phi = ', rpc_phi
        if printLevel > 0: print 'RPC Track Pt = ', rpc_cluster_pt
        if printLevel > 0: print 'dphi12_cluster = ', dphi12_cluster
        if printLevel > 0: print 'dphi23_cluster = ', dphi23_cluster
        if printLevel > 0: print 'chg_muon = ', chg_muon
        if printLevel > 0: print 'Inv pt_muon  = ', 1/pt_muon
            
        # fill cluster plots
        if chg_muon > 0:
            hdphi_cluster_csc1_plus.Fill(dphi12_cluster)
            hdphi_cluster_csc3_plus.Fill(dphi23_cluster)
        
        else:
            hdphi_cluster_csc1_minus.Fill(dphi12_cluster)
            hdphi_cluster_csc3_minus.Fill(dphi23_cluster)

	if (chg_muon is -1):
		chg_muon = -1.0
	elif (chg_muon is 1):
		chg_muon = 1.0

        # invPt plots    
        hdphi_cluster2_csc1_invPt.Fill(chg_muon/rpc_cluster_pt, dphi12_cluster)
        hdphi_cluster2_csc3_invPt.Fill(chg_muon/rpc_cluster_pt, dphi23_cluster)
	hdphi_cluster2_csc1_invPt_Prof.Fill(chg_muon/rpc_cluster_pt, dphi12_cluster)

	# Pt Prof plots
	hdphi_cluster2_csc1_Pt_Prof.Fill(rpc_cluster_pt, dphi12_cluster)
	hdphi_cluster2_csc1_Pt.Fill(rpc_cluster_pt, dphi12_cluster)
            
        # Resolution
        #hres.                 Fill( (evt.PtTrk[iCSCTrk] - pt_muon)/pt_muon )
        #hres_rpc_cluster.     Fill( (evt.PtTrk_RPC_regr - pt_muon)/pt_muon )
        
        
        # rate
        for bin in reversed(rate):
            
            if Trk > bin: hrate_mode2.Fill(bin)
            
            if rpc_cluster_pt > bin:
                hrate_rpc_mode2.Fill(bin)

	#print '---------------------------------------------------------------'

        # turn on curves        
        csctfPt_all.Fill(pt_muon)
        
        for ihist, thresh in enumerate(pt_thresh):
            
            if Trk >= thresh:
                csctfPt[ihist].Fill(pt_muon)
                
            if rpc_cluster_pt >= thresh:
                csctfPt_rpc[ihist].Fill(pt_muon)

		#if pt_muon < thresh:
		#	print '\n\n Cluster pT: ', evt.PtTrk_cluster_front[iCSCTrk]
		#	print 'Reco pT: ', pt_muon
		#	print 'dphi12: ', dphi12_cluster
        # end turn on curves     


	# cluster size
	hcluster_size.Fill(evt.cluster_size)
	hcluster_size_outliers.Fill(evt.cluster_size_outliers)   
        
# end event loop

#  ======== Write Hists ==========

# Little more work for turn on curves
for ihist, thresh in enumerate(pt_thresh): 
    
    csctfPt[ihist].Write()
    csctfPt_rpc[ihist].Write()
    
    Pt_turn[ihist]     = TGraphAsymmErrors(csctfPt[ihist], csctfPt_all)
    Pt_turn_rpc[ihist] = TGraphAsymmErrors(csctfPt_rpc[ihist], csctfPt_all)
    
    
for hist in hist_list:
    if isinstance(hist, list):
        newfile.mkdir('%s' % hist[0].GetName()).cd()
        for i in hist: i.Write()
    else: hist.Write()

del newfile

# ================================



# ========= Results ===========
print '\n\n=========== Analysis Results ============'
print rpc_count
        


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
file = TFile('/exports/uftrig01a/dcurry/data/rpc/singleMu_lowPt_clustering_2_3_15.root')

# Set the branch address of TTree in Tfile
evt = file.Get("CSCtree")

# bind methods to improve speed
getEntry = evt.GetEntry

# Counters
rpc_count = Counter()

def deltaR(phi1, phi2, eta1, eta2):

    deta = eta1 - eta2
    dphi = phi1 - phi2
    if dphi > np.pi: dphi - np.pi
    
    return sqrt(dphi*dphi + deta*deta)


# ================ Histograms =============================================
# =========================================================================
newfile = TFile('twoHit_RPC_pt_plots.root','recreate')

h_phibit_diff = TH1F('h_phibit_diff', '', 50, -20, 20)

h_pt_diff = TH1F('h_pt_diff', '', 50, -100, 100)

h_csc2_cluster2_phidiff = TH1F('h_csc2_cluster2_phidiff', '', 50, -0.02, 0.02)

hist_list = []

# =========================================================================




#  ========= Pre_loop Plot Section ===========

c1 = TCanvas('c1')
#h1 = TH1F('h1', '', 100, -3.14, 3.14)
h2 = TH2F('h2', '', 50, -3.14, 3.14, 100, 0, 4096) 

#evt.Project('h1', 'PtTrk_reco_rpc_csc[0][0]-PtTrk[0]', 'isGoodTrk[0]==1 && rpc_Station==2')

evt.Project('h1', 'trLctglobalPhi[0][0]', 'trLctSector[0][0] == 5')

#evt.Project('h2', 'rpc_Phibit[0]:rpc_gblPhi[0]', 'rpc_Station[0]==2')

#h1.Draw()



# ============================================



# ========= event loop =============
for iEvt in range(evt.GetEntries()):

    # for testing
    #if iEvt > 1000000: break

    getEntry(iEvt)
    
    if iEvt % 10000 is 0: print 'Event #', iEvt
    
    if printLevel > 0: print '\n============== New Event # ', evt.Event, ' ================='


    

    # Loop over csc tracks
    for iCSCTrk in range(evt.SizeTrk):
        
        if printLevel > 0: print '\n====== Looping over Track # ', iCSCTrk, ' ======='
        
        #if evt.isGoodTrk[iCSCTrk] != 1: continue         
        
        # loop over tracks hits to find mode
        for iCsc in range(evt.NumLctsTrk[iCSCTrk]):
            
            if printLevel > 0: print '\n====== Looping over CSC Lct # ', iCsc, ' ======='
            
            if evt.trLctStation[iCSCTrk*4 + iCsc] != 2: continue
            
            # fill hists
            if abs(evt.trLctglobalPhi[iCSCTrk*4 + iCsc] - evt.rpc_stat2_cluster_phi) < 0.001:
                h_phibit_diff.Fill(evt.trLctPhiBit[iCSCTrk*4 + iCsc] -  evt.rpc_stat2_cluster_phiBit)
                
            h_csc2_cluster2_phidiff.Fill(evt.trLctglobalPhi[iCSCTrk*4 + iCsc] - evt.rpc_stat2_cluster_phi)    


            if evt.PtTrk_threeHit_rpc_cluster[iCSCTrk*4 + iCsc] == -999: continue


            rpc_count['num_tracks_corrected'] += 1

            
            # Now compare track pt with and without rpc cluster
            if printLevel > 0:
                print '\n ===== CSC Lct replaced with RPC Cluster ====='
                print '   Track Pt: ', evt.PtTrk[iCSCTrk]
                print '   Track Pt w/cluster: ', evt.PtTrk_threeHit_rpc_cluster[iCSCTrk*4 + iCsc]
                print '   rpc_cluster phiBit: ', evt.rpc_stat2_cluster_phiBit
                print '   rpc_cluster gblPhi: ', evt.rpc_stat2_cluster_phi
                print '   trkLct 2 phiBit: ', evt.trLctPhiBit[iCSCTrk*4 + iCsc]
                print '   trkLct 2 gblPhi: ', evt.trLctglobalPhi[iCSCTrk*4 + iCsc]
                print '   trkLct 1 phiBit: ', evt.trLctPhiBit[iCSCTrk*4 + 0]
                print '   trkLct 1 gblPhi: ', evt.trLctglobalPhi[iCSCTrk*4 + 0]
                print '   trkLct 2 station: ', evt.trLctStation[iCSCTrk*4 + iCsc]
                
                
            if evt.PtTrk_threeHit_rpc_cluster[iCSCTrk*4 + iCsc] > evt.PtTrk[iCSCTrk]:
                if printLevel > 0: print '----> Pt has gone up with rpc cluster...'
                rpc_count['num_tracks_incerase_Pt'] += 1

            if evt.PtTrk_threeHit_rpc_cluster[iCSCTrk*4 + iCsc] < evt.PtTrk[iCSCTrk]:
                 if printLevel > 0: print '----> Pt has gone down with rpc cluster...'
                 rpc_count['num_tracks_decrease_Pt'] += 1
                 
                 



# Final Hist drawing

#h_phibit_diff.Draw()

#h_pt_diff.Draw()

h_csc2_cluster2_phidiff.Draw()



print rpc_count

raw_input('---> press enter to continue...')






                      

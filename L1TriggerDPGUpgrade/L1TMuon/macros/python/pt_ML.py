########################################################
## A script to Import ML coefficients and predict RPC pt
##
## By David Curry
##
########################################################

import sys
from ROOT import *
import numpy as np
from collections import Counter
from matplotlib import interactive



print '\n\n ======== You Are Now Running Your Code. Good Luck! =======\n\n'

# Set the print level. Default = 0
if len(sys.argv) is 1: printLevel = 0
else: printLevel = sys.argv[1]

# Import numpy file
coef = np.load('coef.npy')
#print 'Data contents = ', data

# Import Root file
file = TFile("/exports/uftrig01a/dcurry/data/rpc/2012D_raw_reco_7_5_merge_charge.root");

# Set the branch address of TTree in Tfile
evt = file.Get("CSCtree")

# bind methods to improve speed
getEntry = evt.GetEntry

# Counters
rpc_count = Counter()

# Hists
hmode1 = TH1F('hmode1', '', 10 , 0, 10)



# ===============================================
# Loop over over events in TFile
for iEvt in range(evt.GetEntries()):
    
    # for testing
    if iEvt > 40000: break
    
    getEntry(iEvt)
    
    if iEvt % 10000 is 0: print 'Event #', iEvt
    
    if printLevel > 0: print '\n============== New Event # ', iEvt, ' ================='

    # Loop over muons
    for iReco in range(evt.muonSize):
        
        if evt.muon_pt[iReco] < 2: continue
        if abs(evt.muon_eta[iReco]) < 0.9 or abs(evt.muon_eta[iReco]) > 2.4: continue
        
        if printLevel > 0: print '\n====== Looping over Muon # ', iReco, ' ======='

        # Each muon starts as not matched to a track
        isMatched = False
        best_dr = []

        # Loop over csc tracks
        for iCSCTrk in range(evt.SizeTrk):
            
            if printLevel > 0: print '\n====== Looping over Track # ', iCSCTrk, ' ======='
            
            # find which track, if any, is matched to muon.
            deta = evt.EtaTrk[iCSCTrk] - evt.muon_eta[iReco];
            dr = sqrt( deta*deta );
            
            if dr < 0.2:
                
                if printLevel > 0:
                    print 'Muon # ', iReco, ' is matched to Track # ', iCSCTrk
                    print 'Delta R  = ', dr
                    
                isMatched = True
                best_dr.append(dr)
                    
            else: best_dr.append(999)
                
        # end track loop
                
        # skip muons who are not matched to a track
        if not isMatched: continue
        
        # returns which track # is matched to current muon
        iCSCTrk = best_dr.index(min(best_dr))
        
        if printLevel > 1: print 'Best delta R match is Track # ', iCSCTrk
        
        # Look only at 2 hit tracks
        if evt.NumLctsTrk[iCSCTrk] is 2: 
            
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

                #print 'Lcts Station = ', evt.trLctStation[iCSCTrk*4 + iCsc]
                
                temp_mode += evt.trLctStation[iCSCTrk*4 + iCsc]
                if evt.trLctStation[iCSCTrk*4 + iCsc] == 1: isStation_1 = True
                if evt.trLctStation[iCSCTrk*4 + iCsc] == 2: isStation_2 = True
                

            # Which mode is track?
            if temp_mode is 3: track_mode = 1
            if temp_mode is 4: track_mode = 2
            if temp_mode is 6: track_mode = 5
            if temp_mode is 7: track_mode = 6

            # In the event of mode 5 we need to find which configuration
            if track_mode is 0 and isStation_1: track_mode = 3
            elif track_mode is 0: track_mode = 4
            
            if printLevel > 0: print ' This track is mode =', track_mode, '\n' 


            # ===========================================================================================
            # Mode 2 Tracks(1-3)

            if track_mode is 2: 

                # loop over csc Lcts
                for iCsc in range(evt.NumLctsTrk[iCSCTrk]):

                    if printLevel>0:
                        print 'Looping over CSC Lct # ', iCsc,' in station # ', evt.trLctStation[iCSCTrk*4 + iCsc] 
                        print 'Gbl Phi = ', evt.trLctglobalPhi[iCSCTrk*4 +iCsc] 
                        print 'Gbl Eta = ', evt.trLctglobalEta[iCSCTrk*4 +iCsc] 
                        
                    if evt.trLctStation[iCSCTrk*4 + iCsc] is 1:
                        phi1 = evt.trLctglobalPhi[iCSCTrk*4 +iCsc]
                        eta1 = evt.trLctglobalEta[iCSCTrk*4 +iCsc]
                        
                    if evt.trLctStation[iCSCTrk*4 +iCsc] is 3:
                        eta3 = evt.trLctglobalEta[iCSCTrk*4 +iCsc]
                        phi3 = evt.trLctglobalPhi[iCSCTrk*4 +iCsc] 
                    
                # end loop over CSC Lcts


                # Now loop over rpc hits.  For mode 2 look for station 2 rpcs
                for iRpc in range(evt.rpc_NumLctsTrk):

                    # dont look at rpc hits that are not in 0.9-1.6 eta
                    if abs(evt.rpc_gblEta[iRpc]) < 0.9 or abs(evt.rpc_gblEta[iRpc]) > 1.6: continue
                    
                    if printLevel>0: print '   Looping over RPC hit # ', iRpc 
                    
                    if evt.rpc_Station[iRpc] is not 2: continue;
                    
                    if printLevel>0: print '   RPC is in Station 2\n'
                    
                    phi2 = evt.rpc_gblPhi[iRpc]
                    eta2 = evt.rpc_gblEta[iRpc]
                    
                # end loop over rpcs

            # end mode 2
            # ===========================================================================================
            

            hmode1.Fill(evt.rpc_NumLctsTrk)
                
                


            if track_mode is not 2: continue
            
            # Store all features in a numpy array. Order of features is:
            # bias unit, phi12 , phi13 , phi23 , eta12 , eta13 , eta23
            dphi12 = abs( abs( abs(phi1 - phi2) - np.pi) - np.pi)
            dphi13 = abs( abs( abs(phi1 - phi3) -np.pi) -np.pi)
            dphi23 = abs( abs( abs(phi2 - phi3) -np.pi) -np.pi)
            deta12  = abs(eta1 - eta2)
            deta13  = abs(eta1 - eta3)
            deta23  = abs(eta2 - eta3)
                        
            features_data = np.array([1, dphi12, dphi13 , dphi23 , deta12 , deta13 , deta23]) 
            
            ans = np.dot(features_data, coef)
            
            #print 'Pt Prediction = ' , ans
            #print 'Pt True       = ' , evt.muon_pt[iReco], '\n\n'
            
        # end if 2 lcts
        
    # end loop over muon

# end loop over events

c1 = TCanvas('c1')
hmode1.SetStats(0)
hmode1.Draw()

c1.SaveAs('test.png')

raw_input('press return to continue')

print '\n\n ======== Your Code Has Finished. Congradulations! =======\n\n'

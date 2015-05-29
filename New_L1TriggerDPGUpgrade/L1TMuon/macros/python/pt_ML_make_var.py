#  A script to store csc pt variables in data file.  Later use in machine learning algo
#
# By David Curry
#
#

import sys
from ROOT import *
import numpy as np

print '\n\n ======== You Are Now Running Your Code. Good Luck! =======\n\n'

# Set the print level. Default = 0
if len(sys.argv) is 1: printLevel = 0
else: printLevel = sys.argv[1]
print '-----> printLevel =', printLevel

# open the tfile
file = TFile("/exports/uftrig01a/dcurry/data/rpc/2012D_raw_reco_7_5_merge_charge.root");
print '-----> Opening TFile = ', file

# Set the branch address of TTree in Tfile
evt = file.Get("CSCtree")

# bind methods to improve speed
getEntry = evt.GetEntry

# Variables, arrays to be used
dphi12_  = []
dphi13_  = []
dphi23_  = []
deta12_  = []
deta13_  = []
deta23_  = []
trkEta_  = []
muon_pt_ = []

# ===============================================
# Loop over over events in TFile
for iEvt in range(evt.GetEntries()):

    # for testing
    if iEvt > 1000000: break
    
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
        
        # Now calculate all variables for ML algos
        # DPhi13, DPhi12, Deta13, Deta12, DPhiRPC11, DphiRPC22, DPhiRPC33
        
        # Look only at 3 hit tracks
        if evt.SizeTrk is not 3: continue


        # variables, arrays 
        
        isStation1 = False
        isStation2 = False
        isStation3 = False
        
        # Loop over Lcts in track
        for iLct in range(evt.SizeTrk):
            
            if evt.trLctStation[iCSCTrk*4 + iLct] is 1:
                eta1 = evt.trLctglobalEta[iCSCTrk*4+ iLct]
                phi1 = evt.trLctglobalPhi[iCSCTrk*4+ iLct]
                isStation1 = True
                
            if evt.trLctStation[iCSCTrk*4+ iLct] is 2:
                eta2 = evt.trLctglobalEta[iCSCTrk*4+ iLct]
                phi2 = evt.trLctglobalPhi[iCSCTrk*4+ iLct]
                isStation2 = True
                
            if evt.trLctStation[iCSCTrk*4+ iLct] is 3:
                eta3 = evt.trLctglobalEta[iCSCTrk*4+ iLct]
                phi3 = evt.trLctglobalPhi[iCSCTrk*4+ iLct]
                isStation3 = True
                

        # End loop over Lcts       

        # Sanity Check
        if not isStation1 or not isStation2 or not isStation3: continue

        # Calculate variables
        dphi12 = abs( abs( abs(phi1 - phi2) - np.pi) - np.pi)
        dphi13 = abs( abs( abs(phi1 - phi3) -np.pi) -np.pi)
        dphi23 = abs( abs( abs(phi2 - phi3) -np.pi) -np.pi)
        deta12  = abs(eta1 - eta2)
        deta13  = abs(eta1 - eta3)
        deta23  = abs(eta2 - eta3)
        
        if printLevel > 0:
            print '--->Track is Station 1,2,3'
            print 'Dphi12 = ', dphi12
            

        # Store the variables in numpy arrays.  I'd like them to be columns of a file(dat, txt, etc) 
        dphi12_.append(dphi12)
        dphi13_.append(dphi13)
        dphi23_.append(dphi23)
        deta12_.append(deta12)
        deta13_.append(deta13)
        deta23_.append(deta23)
        muon_pt_.append(evt.muon_pt[iReco])
        trkEta_.append(evt.EtaTrk[iCSCTrk])

    # end muon loop

# end event loop


# Write varibales to file.  Each variable should be a column. Dimensions are [num entries, num variables+pt]
dphi12_ = np.array(dphi12_)
dphi13_ = np.array(dphi13_)
dphi23_ = np.array(dphi23_)
deta12_ = np.array(deta12_)
deta13_ = np.array(deta13_)
deta23_ = np.array(deta23_)
trkEta_ = np.array(trkEta_)
muon_pt_ = np.array(muon_pt_) 


file = np.column_stack( (dphi12_, dphi13_, dphi23_, deta12_, deta13_, deta23_, trkEta_, muon_pt_) )

np.savetxt('test.txt', file, delimiter=" ") 
                    
print '\n\n ======== Your Piece of Code Actually Finished. Congradulations! =======\n\n'



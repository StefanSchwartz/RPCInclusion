########################################################
## me11.py   A script to analyze me11 lcts information
##
## By David Curry
##
########################################################

import sys
from ROOT import *
import numpy as np



# Set the print level. Default = 0
if len(sys.argv) is 1: printLevel = 0
else: printLevel = sys.argv[1]

# Import Root file
#file = TFile("GlobalRun_CSCTF.root")
file = TFile("MinBias_2012C_test_trkPt.root")

# Set the branch address of TTree in Tfile
evt = file.Get("csctfTTree")

# bind methods to improve speed
getEntry = evt.GetEntry

# ===============================================
# Loop over over events in TFile
for iEvt in range(evt.GetEntries()):

    # for testing
    if iEvt > 40000: break

    getEntry(iEvt)

    if iEvt % 10000 is 0: print 'Event #', iEvt

#    if printLevel > 0: print '\n============== New Event # ', evt.Event, ' ================='
        
    # Loop over tracks
    for iCSCTrk in range(evt.SizeTrk):

        if printLevel > 0: print '==== Looping over Track #', iCSCTrk, '====' 

        # Loop over lcts in track
        for iLct in range(evt.NumLCTsTrk[iCSCTrk]):

            if printLevel > 0:
                print ' == Looping over Lct #', iLct
                print ' Lct Station = ', evt.trLctStation[iCSCTrk*4 + iLct]

            
                


                
            
        # end loop over Lcts

    # end loop over tracks
    
# end loop over events
    



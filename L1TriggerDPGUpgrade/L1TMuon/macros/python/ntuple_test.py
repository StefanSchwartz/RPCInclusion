#  A script to test the ntuple creation of rpc_tuplizer.cc
#
# By David Curry
#
#

import sys
from ROOT import *
import numpy as np

print ' ======== You Are Now Testing Your Ntuple.  Good Luck! =======' 

# Set the print level. Default = 0
if len(sys.argv) is 1: printLevel = 0
else: printLevel = sys.argv[1]
print '-----> printLevel =', printLevel

# open the tfile
file = TFile("../test/rpc_tuple_allevtTest_MC.root");
print '-----> Opening TFile = ', file

# Set the branch address of TTree in Tfile
tr = file.Get("CSCtree");

# bind methods to improve speed
getEntry = CSCtree.GetEntry

# ===============================================
# Loop over over events in TFile
for iEvt in range(tr.GetEntries()):

    getEntry(iEvt)
    
    if iEvt%10000 is 0: print 'Event #', iEvt

    if printLevel > 0:
        print '================ Event =', iEvt, '==================' 
        print '                 Lumi  =' 
        print '                 Run   ='
        
    # Loop over CSCTF tracks
    for iCSCTrk in range(tr.SizeTrk):

        if printLevel > 0:
            print '  === Looping over track #', iCSCTrk
            print '      Track eta ='              
            print '      Track Phi ='
            print '      Track Pt  =' 


            # Loop over track lcts
            for iLct in range(tr.NumLctsTrk[iCSCTrk]):

                if printLevel > 0:
                    print '      === Looping over CSC Track LCT #', iLct
                    print '          Lct eta ='
                    print '          Lct Phi ='
                    print '          Lct Pt  ='




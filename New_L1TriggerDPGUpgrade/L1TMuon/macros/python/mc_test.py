########################################################
## ntuple_test.py   A script to analyze me11 lcts information
##
## By David Curry
##
########################################################

print '------> Setting Environment'

import sys
from ROOT import *
import numpy as np
from collections import Counter

# Set the print level. Default = 0
if len(sys.argv) is 1: printLevel = 0
else: printLevel = sys.argv[1]

print '------> Importing Root File'

file = TFile("/exports/uftrig01a/dcurry/data/rpc/rpc_tuple_charge_MC.root")

# Define Hist file to be saved
newfile = TFile("mc_charge_test.root", "recreate")

# Set the branch address of TTree in Tfile
evt = file.Get("CSCtree")

# A more efficient way to count
count = Counter()

# ====== Histograms =========

hdphi_plus  = TH1F('hdphi_plus' , '', 50, -0.2, 0.2)
hdphi_minus = TH1F('hdphi_minus', '', 50, -0.2, 0.2)


hlist = [hdphi_plus, hdphi_minus]
# ===========================




# Loop over over events in TFile
for iEvt in range(evt.GetEntries()):

        # for testing
        #if iEvt > 20: break

        evt.GetEntry(iEvt)
        
        if iEvt % 10000 is 0: print 'Event #', iEvt
	
	print ' Event  =', evt.Event 
	print ' Run    =', evt.Run   
	print ' Lumi   =', evt.Lumi  
	print ' Muon charge = ', evt.gen_chg
	
        # single muon MC file(no muon loop needed)
        # match muon to track
        # loop over tracks
	isMatch = False
        for iTrk in range(evt.SizeTrk):
		
		print 'Track charge = ', evt.ChgTrk[iTrk]
		
		if isMatch: continue
		
		dphi = abs( abs( abs(evt.gen_phi - evt.PhiTrk[iTrk]) -np.pi) -np.pi)   
		deta = abs(evt.gen_eta - evt.EtaTrk[iTrk])
		
		dr = sqrt(deta*deta + dphi*dphi)

		if dr > 0.2: continue

		isMatch = True

		phi1= 999; phi2 = 999
		# Loop over track lcts
		for iLct in range(evt.NumLctsTrk[iTrk]):
			
			if evt.trLctStation[iTrk*4 + iLct] is 1:
				phi1 = evt.trLctglobalPhi[iTrk*4 + iLct] 
				phi1_bit = evt.trLctPhiBit[iTrk*4 + iLct]

			if evt.trLctStation[iTrk*4 + iLct] is 2:
				phi2 = evt.trLctglobalPhi[iTrk*4 + iLct]
				phi2_bit = evt.trLctPhiBit[iTrk*4 + iLct]
			
		# end lct loop

		if phi1 is 999: continue
		if phi2 is 999: continue

		print 'Phi1 = ', phi1, 'Phi bit = ', phi1_bit
		print 'Phi2 = ', phi2, 'Phi bit = ', phi2_bit
		
		print 'wire1 = ', 

		dphi = phi1 - phi2 

		# Fill Hists
		if evt.gen_chg is 1: hdphi_plus.  Fill(dphi)
		else:            hdphi_minus. Fill(dphi)

		            
        # end track loop


# end event loop

# Write Hists to File

for hist in hlist: hist.Write()


del newfile





UpgradedTrackFinder
===================
Checkout Instructions:

1.clone into your working src directory $CMMSW_BASE/src

2.change top directory name from UpgradedTrackFinder/ to L1TriggerDPGUpgrade
     (Either this title should be changed on github or I should update all 
      of the headers but it hasn't been done as of yet.)

3. scram b and everything should work

4. the main code is in L1TriggerDPGUpgrade/L1TMuon/plugins/L1TMuonTextDumper.cc which 
   which pulls in other modules. Has extra many extra cout statements for debugging 
   currently and working code for efficiency studies.

5. to get a sample to run on you need to use a ptgun(provided in L1TMuon/test/) or 
  similar sample(data included) then run the run_L1TMuon_cfg.py with this sample
  as it it's input to obtain the L1TMuon objects.

# RPCInclusion
===============
setup:

	cmsrel CMSSW_6_2_3
	cd CMSSW_6_2_3/src
	cmsenv
	git clone git@github.com:StefanSchwartz/RPCInclusion.git ./
	scramv1 b -j 20

To produce new Monte Carlo Data Samples:

	cd twoHit_RPC;
	cmsRun particle_gun_gen_sim.py

Need file L1TMuon_tuple_MC.py to create NTuple, which calls rpc_tuplizer.cc, so the entire codebase is needed

To Analyze NTuples:

	Run twoHit_RPC_pt.py and threeHit_RPC_pt.py first (requires numpy and pyROOT, etc)
	Run comparisonPlots.cxx and invPtLUT.cxx to generate rate, efficiency, and pT comparison plots

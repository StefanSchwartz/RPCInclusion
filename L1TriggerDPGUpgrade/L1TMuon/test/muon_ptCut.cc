//This function plots the histogram of muon_pt, with cuts s.t. ModeTrk > 10 and muon_eta < 2.4
void muon_ptCut(){

	TTree* mytree = (TTree*)gDirectory->Get("CSCtree");

	TCut *cut1 = new TCut("ModeTrk>10");//defines the ModeTrk cut
	TCut *cut2 = new TCut("muon_eta<2.4");//defines the muon_eta cut

	mytree->Draw("muon_pt",*cut1&&*cut2);//Draws the actual histogram with the cuts
} 

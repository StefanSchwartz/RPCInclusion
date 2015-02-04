//This is a ROOT macro similar to muon_ptCut.cxx, but it reads the information in a different way

void muon_ptCutv2(){
	
	//Getting the tree from the file
	TFile *InFile = TFile::Open("csc_raw_test.root", "READ");
	TTree* mytree = (TTree*)InFile->Get("CSCtree");

	//Defines the cuts a.k.a. the restrictions for the second histogram
//	TCut *cut1 = new TCut("muon_eta<-2.1");
//	TCut *cut2 = new TCut("muon_eta>2.1");
	
	//Initializing variables to hold the data from the tree
	Double_t muon_pt;//, muon_eta;
	
	//Creating pointers to the appropriate variables
	mytree->SetBranchAddress("muon_pt",&muon_pt);
//	mytree->SetBranchAddress("muon_eta",&muon_eta);

	//Creating histograms of the data
	TH1F *hpt = new TH1F("hpt","muon_pt distribution",100,0,200);

	Int_t nentries = (Int_t)mytree->GetEntries();

	//looping through the branches to fill the histogram
	for (Int_t i = 0; i < nentries; i++){
		mytree->GetEntry(i);
		hpt->Fill(muon_pt);
	}

//	hpt->Draw();	

	TFile *OutFile = TFile::Open("output.root", "RECREATE");
	hpt->Write();
	
	OutFile->Close();
	InFile->Close();
}	

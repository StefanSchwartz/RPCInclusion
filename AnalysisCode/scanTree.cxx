//This function displays the contents of several variable located in csc_raw_test.toot

void scanTree(){

	TFile *InFile = TFile::Open("csc_raw_test.root", "READ");
	TTree* mytree = (TTree*)InFile->Get("CSCtree");

	mytree->Scan("PtTrk:NumHits");

	InFile->Close();
}

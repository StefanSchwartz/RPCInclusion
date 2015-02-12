//This program creates a 2-dimensional histogram comparing the difference between track and calculated pT to the track pT

void differencePt(){

	//Getting the tree from the file
	TFile *InFile = TFile::Open("csc_raw_test.root", "READ");
	TTree *mytree = (TTree*)InFile->Get("CSCtree");
	mytree->SetMakeClass(1);

	//Initalizing variables to hold data from the tree
	Int_t Three_Hit_pt;
	Int_t Three_Calc_pt;
	
	//Initialize the histogram, to be filled later
	TH2F *h1 = new TH2F("h1","Three Hit p_{T} Difference;Track p_{T} (GeV);LUT-Track",100,0,150,100,-150,150);
	mytree->SetBranchAddress("Three_Hit_pt", &Three_Hit_pt);
	mytree->SetBranchAddress("Three_Calc_pt", &Three_Calc_pt);

	Int_t n = (Int_t)mytree->GetEntries();

	//Filling the histogram
	for (Int_t i = 0; i < n; i++){
		mytree->GetEntry(i);
		if (Three_Calc_pt != -999){
        		h1->Fill(Three_Hit_pt,Three_Hit_pt-Three_Calc_pt);
		}
	}

	//Drawing the canvas and making it look pretty
	TCanvas *c1 = new TCanvas("c1");
	h1->Draw("COLz");
	gPad->Update();
	TPaveStats *st = (TPaveStats*)h1->FindObject("stats");
	st->SetOptStat(111);
	st->SetX1NDC(.6);
	st->SetX2NDC(.8);
	st->SetY1NDC(.15);
	st->SetY2NDC(.3);

	//Output to file
	TFile *OutFile = TFile::Open("Three Hit pT Difference.root", "RECREATE");
	h1->Write();
	c1->Print("Three Hit pT Difference.pdf");
	OutFile->Close();
	InFile->Close();
}

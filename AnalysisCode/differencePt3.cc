//The third RPC hit type, clustering and single hits

void differencePt3(){

	//Getting the tree from the file
	TFile *InFile = TFile::Open("csc_raw_test.root", "READ");
	TTree *mytree = (TTree*)InFile->Get("CSCtree");
	mytree->SetMakeClass(1);

	//Initalizing variables to hold data from the tree
	Int_t Two_Hit_Pt3;
	Int_t Two_Calc_Pt3;
	
	//Initialize the histogram, to be filled later
	TH2F *h1 = new TH2F("h1","Two Hit p_{T} Difference;Track p_{T} (GeV);LUT-Track",100,0,150,100,-150,150);
	mytree->SetBranchAddress("Two_Hit_Pt3", &Two_Hit_Pt3);
	mytree->SetBranchAddress("Two_Calc_Pt3", &Two_Calc_Pt3);

	Int_t n = (Int_t)mytree->GetEntries();

	//Fill histogram
	for (Int_t i = 0; i < n; i++){
		mytree->GetEntry(i);
		if (Two_Calc_Pt3 != -999){
        		h1->Fill(Two_Hit_Pt3,Two_Hit_Pt3-Two_Calc_Pt3);
		}
	}

	//Draw graph and add maquillage
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
	TFile *OutFile = TFile::Open("Two Hit pT Difference 3.root", "RECREATE");
	h1->Write();
	c1->Print("Two Hit pT Difference 3.pdf");
	OutFile->Close();
	InFile->Close();
}

//I hate root so much right now. It's the worst.

void getAllHit(){
	
	//Getting the tree from the file
	TFile *InFile = TFile::Open("csc_raw_test.root", "READ");
	TTree* mytree = (TTree*)InFile->Get("CSCtree");
	mytree->SetMakeClass(1);

	//Initializing variables to hold the data from the tree
	Float_t PtTrk;
	Int_t All_Hit_pt;
	Int_t All_Hit_Match;
		
	//Creating pointers to the appropriate variables
	mytree->SetBranchAddress("PtTrk",&PtTrk);
	mytree->SetBranchAddress("All_Hit_pt",&All_Hit_pt);
	mytree->SetBranchAddress("All_Hit_Match",&All_Hit_Match);

	Double_t xbins[5] = {10,20,40,60,200};

	//Creating histograms of the data
	TH1F *hpttrk = new TH1F("hpttrk","PtTrk distribution",4,xbins);
	TH1F *hahpt = new TH1F("hahpt","All_Hit_pt distribution",4,xbins);
	TH1F *hahm = new TH1F("hahm","All_Hit_Match distribution",4,xbins);

	Int_t nentries = (Int_t)mytree->GetEntries();

	//looping through the branches to fill the histogram
	for (Int_t i = 0; i < nentries; i++){
		mytree->GetEntry(i);
		hpttrk->Fill(PtTrk);
		hahpt->Fill(All_Hit_pt);
		hahm->Fill(All_Hit_Match);
	}

	hpttrk->SetDirectory(0);
	hthpt->SetDirectory(0);
	hthm->SetDirectory(0);
	InFile->Close();

	Double_t xbins[5] = {10,20,40,60,200};

	//Creating a histogram of the efficiency, Matches/Total
	TH1F *eff2 = new TH1F("eff2","Efficiency, All Hit Tracks",4,xbins);

	eff2->Divide(hahm,hpttrk,1.,1.,"B");

	//Initializing variables to calculate errors
	Double_t temp_n;
	Double_t temp_n2;
	Double_t temp_error;
	Double_t temp_error2;
	Double_t temp_eff;
	Double_t temp_eff_error;

	//Looping through all the bins and calculating error for the efficiency plot
	for (Int_t j = 0; j < 5; j++){
		temp_n = hpttrk->GetBinContent(j);
		temp_n2 = hahm->GetBinContent(j);

		cout << "Bin content of bin " << j << ":" << endl;
		cout << "hpttrk = " << temp_n << endl;
		cout << "hahm = " << temp_n2 << endl;

		temp_error = sqrt(temp_n)/temp_n;
		temp_error2 = sqrt(temp_n2)/temp_n2;

		temp_eff = eff2->GetBinContent(j);
		temp_eff_error = temp_eff * sqrt(pow(temp_error,2)+pow(temp_error2,2));

		eff2->SetBinError(j,temp_eff_error);
		cout << "Efficiency is " << temp_eff << endl;
		cout << "Final error is " << temp_eff_error << endl << endl;
	}

	//Writing all the results to a file
	TFile *OutFile = TFile::Open("getAllOut.root", "RECREATE");
	hpttrk->Write();
	hahpt->Write();
	hahm->Write();
	eff2->Write();
	
	OutFile->Close();
}	


































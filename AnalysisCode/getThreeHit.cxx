//This function get the data of PtTrk, Three_Hit_pt, and Three_Hit_Match from csc_raw_test.rootand stores it in separate histogram files

void getThreeHit(){
	
	//Getting the tree from the file
	TFile *InFile = TFile::Open("csc_raw_test.root", "READ");
	TTree* mytree = (TTree*)InFile->Get("CSCtree");
	mytree->SetMakeClass(1);

	//Initializing variables to hold the data from the tree
	Float_t PtTrk;
	Int_t Three_Hit_pt;
	Int_t Three_Hit_Match;
	Int_t All_Hit_pt;
	Int_t All_Hit_Match;
		
	//Creating pointers to the appropriate variables
	mytree->SetBranchAddress("PtTrk",&PtTrk);
	mytree->SetBranchAddress("Three_Hit_pt",&Three_Hit_pt);
	mytree->SetBranchAddress("Three_Hit_Match",&Three_Hit_Match);

	Double_t xbins[5] = {10,20,40,60,200};

	//Creating histograms of the data
	TH1F *hpttrk = new TH1F("hpttrk","PtTrk distribution",4,xbins);
	TH1F *hthpt = new TH1F("hthpt","Three_Hit_pt distribution",4,xbins);
	TH1F *hthm = new TH1F("hthm","Three_Hit_Match distribution",4,xbins);

	Int_t nentries = (Int_t)mytree->GetEntries();

	//looping through the branches to fill the histogram
	for (Int_t i = 0; i < nentries; i++){
		mytree->GetEntry(i);
		hpttrk->Fill(PtTrk);
		hthpt->Fill(Three_Hit_pt);
		hthm->Fill(Three_Hit_Match);
	}
		
	hpttrk->SetDirectory(0);
	hthpt->SetDirectory(0);
	hthm->SetDirectory(0);
	InFile->Close();

	//Creating a histogram of the efficiency, Matches/Total
	TH1F *eff = new TH1F("eff","Efficiency, CSC Hits vs. CSC/RPC Combined Hits",4,xbins);

	eff->Divide(hthm,hpttrk,1.,1.,"B");

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
		temp_n2 = hthm->GetBinContent(j);

		cout << "Bin content of bin " << j << ":" << endl;
		cout << "hpttrk = " << temp_n << endl;
		cout << "hthm = " << temp_n2 << endl;

		temp_error = sqrt(temp_n)/temp_n;
		temp_error2 = sqrt(temp_n2)/temp_n2;

		temp_eff = eff->GetBinContent(j);
		temp_eff_error = temp_eff * sqrt(pow(temp_error,2)+pow(temp_error2,2));

		eff->SetBinError(j,temp_eff_error);
		cout << "Efficiency is " << temp_eff << endl;
		cout << "Final error is " << temp_eff_error << endl << endl;
	}

	//Writing all the results to a file
	TFile *OutFile = TFile::Open("getThreeOut.root", "RECREATE");
	hpttrk->Write();
	hthpt->Write();
	hthm->Write();
	eff->Write();
	
	OutFile->Close();
}	

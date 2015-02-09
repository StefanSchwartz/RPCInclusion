//This functions makes a scatter plot of calculated pT against track pT

void plotPt(){

	//Initializing variables to hold data from tree
	int n = 10000;
	int count = 0;
	int All_Calc_pt;
	int All_Hit_pt;
	int *x = new int[n];
	int *y = new int[n];

	//Getting the tree from the file
	TFile *InFile = TFile::Open("csc_raw_test.root", "READ");
	TTree* mytree = (TTree*)InFile->Get("CSCtree");
	mytree->SetMakeClass(1);

	mytree->SetBranchAddress("All_Calc_pt",&All_Calc_pt);
	mytree->SetBranchAddress("All_Hit_pt",&All_Hit_pt);

	for (Int_t i = 0; i < n; i++){
		mytree->GetEntry(i);

		if (All_Hit_pt != -999 && All_Calc_pt != -999){
			x[i-count] = All_Calc_pt;
			y[i-count] = All_Hit_pt;	
		}
		else {
			count++;
		}
	}

	cout << x << endl;

	cout << "Past the loop" << endl;

	InFile->Close();
	
	int length = n - count;
	int matchCount = 0;

	cout << x << endl;

	for (int j = 0; j < length; j++){
		//cout << x[j] << endl;
		//cout << y[j] << endl;

		if (x[j] == y[j]){
			matchCount++;
		}
	}

	cout << "There are " << matchCount << " matches and " << length - matchCount << " non-matches" << endl;

	TGraph *gr1 = new TGraph(length,y,x);
	TCanvas *c1 = new TCanvas("c1");
	gr1->Draw("A*");
	gr1->GetXaxis()->SetTitle("Track pT (GeV)");
	gr1->GetYaxis()->SetTitle("Calculated pT (GeV)");
	gr1->SetTitle("Track pT vs. Calculated pT");

	//Writing all the results to a file
	TFile *OutFile = TFile::Open("pT Scatter Plot.root", "RECREATE");
	gr1->Write();
	c1->Print("pT Scatter Plot.pdf");
	OutFile->Close();

	delete[] x;
	x = 0;
	delete[] y;
	y = 0;
}

//Compares the efficiency of the three hit emulator

void twoHit(){

	//Getting the tree from the file
	TFile *InFile = TFile::Open("csc_raw_test.root", "READ");
	TTree* mytree = (TTree*)InFile->Get("CSCtree");
	mytree->SetMakeClass(1);

	//Initializing variables for data from tree
	Int_t Two_Hit_Pt;
	Int_t Two_Hit_Match;
	Int_t Two_Hit_Match_Front;
	Int_t Two_Hit_Match_Rear;
	Double_t xbins[5] = {10,20,40,60,200};

	//Creating histograms of the data
	TH1F *h1 = new TH1F("h1","Two Hit trkPt distribution",4,xbins);
	TH1F *h2 = new TH1F("h2","Two_Hit_Match distribution",4,xbins);
	TH1F *h3 = new TH1F("h3","Two_Hit_Match_Front distribution",4,xbins);
	TH1F *h4 = new TH1F("h4","Two_Hit_Match_Rear distribution",4,xbins);

	//Creating pointers to the appropriate variables
	mytree->SetBranchAddress("Two_Hit_Pt",&Two_Hit_Pt);
	mytree->SetBranchAddress("Two_Hit_Match",&Two_Hit_Match);
	mytree->SetBranchAddress("Two_Hit_Match_Front",&Two_Hit_Match_Front);
	mytree->SetBranchAddress("Two_Hit_Match_Rear",&Two_Hit_Match_Rear);	

	Int_t nentries = (Int_t)mytree->GetEntries();

	cout << "Entering the loop" << endl;

	//looping through the branches to fill the histogram
	for (Int_t i = 0; i < nentries; i++){
		mytree->GetEntry(i);
		h1->Fill(Two_Hit_Pt);
		h2->Fill(Two_Hit_Match);
		h3->Fill(Two_Hit_Match_Front);
		h4->Fill(Two_Hit_Match_Rear);
	}

	cout << "Through the loop" << endl;

	//Defining histograms for the efficiencies
	TH1F *eff_both = (TH1F*)h2->Clone("eff_both");
	eff_both->Divide(h1);
	TH1F *eff_front = (TH1F*)h3->Clone("eff_both");
	eff_front->Divide(h1);
	TH1F *eff_rear = (TH1F*)h4->Clone("eff_both");
	eff_rear->Divide(h1);

	//initializing variables for error calculation
	Double_t n1, n2, n3, n4, e1, e2, e3, e4, nfront, efront, nboth, eboth, nrear, erear;

	cout << "Entering errfor loop" << endl;

	//Calculating the error in the efficiencies for each bin
	for (Int_t j = 0; j < 5; j++){
		n1 = h1->GetBinContent(j);
		n2 = h2->GetBinContent(j);
		n3 = h3->GetBinContent(j);
		n4 = h4->GetBinContent(j);

		e1 = sqrt(n1)/n1;
		e2 = sqrt(n2)/n2;
		e3 = sqrt(n3)/n3;
		e4 = sqrt(n4)/n4;

		nboth = eff_both->GetBinContent(j);
		eboth = nboth * sqrt(pow(e1,2)+pow(e2,2));
		eff_both->SetBinError(j,eboth);

		nfront = eff_front->GetBinContent(j);
		efront = nfront * sqrt(pow(e1,2)+pow(e3,2));
		cout << nfront << endl << efront << endl << endl;
		eff_front->SetBinError(j,efront);
		
		nrear = eff_rear->GetBinContent(j);
		erear = nrear * sqrt(pow(e1,2)+pow(e4,2));
		eff_rear->SetBinError(j,erear);
	}

	cout << "Out of error loop" << endl;

	//Set up the graph
	TCanvas *c1 = new TCanvas();
	eff_both->Draw("ep");
	eff_both->SetLineColor(kRed);
	eff_both->SetMarkerColor(kRed);
	eff_both->SetAxisRange(0,1,"Y");
	eff_both->SetXTitle("pT (GeV)");
	eff_both->SetYTitle("Efficiency");

	//Draw the second histogram on the same graph
	eff_front->Draw("ep same");
	eff_front->SetLineColor(kBlue);
	eff_front->SetMarkerColor(kBlue);

	//And draw the third
	eff_rear->Draw("ep same");
	eff_rear->SetLineColor(kGreen+3);
	eff_rear->SetMarkerColor(kGreen+3);

	//Create a legend for the graph
	leg = new TLegend(.5,.7,.79,.89);
	leg->AddEntry(eff_both,"Front or Rear Matches","l");
	leg->AddEntry(eff_front,"Front Matches Only","l");
	leg->AddEntry(eff_rear,"Rear Matches Only","l");
	leg->SetHeader("Legend");
	leg->Draw();

	//Get rid of the stats box
	gStyle->SetOptStat(0);

	//Write the graph to output files	
	TFile *OutFile = TFile::Open("Two Hit Comparison 1.root","RECREATE");
	c1->Write();
	c1->Print("Two Hit Comparison 1.pdf");
	OutFile->Close();
	InFile->Close();
}

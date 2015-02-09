//Function to display multiple histograms on separate canvases. Built for the output of getThreeHit.cxx

void dispHist(){

	TFile *InFile = TFile::Open("getThreeOut.root","READ");

/*	TCanvas *c1 = new TCanvas();
	TH1F *hpttrk = (TH1F*)InFile->Get("hpttrk");
	hpttrk->SetDirectory(0);
	hpttrk->Draw();

	TCanvas *c2 = new TCanvas();
	TH1F *hthpt = (TH1F*)InFile->Get("hthpt");
	hthpt->SetDirectory(0);
	hthpt->Draw();

	TCanvas *c3 = new TCanvas();
	TH1F *hthm = (TH1F*)InFile->Get("hthm");
	hthm->SetDirectory(0);
	hthm->Draw();
*/
	TCanvas *c4 = new TCanvas("c4","Efficiency, CSC Hits vs. CSC and RPC Hits");
	TH1F *eff = (TH1F*)InFile->Get("eff");
	eff->SetDirectory(0);
	eff->Draw("ep");
	eff->SetLineColor(kRed);
	eff->SetMarkerColor(kRed);

	eff->SetAxisRange(0,.6,"Y");
	eff->SetXTitle("pT (GeV)");
	eff->SetYTitle("Efficiency");

	InFile->Close();

	TFile *InFile2 = TFile::Open("getAllOut.root","READ");

/*	TCanvas *c5 = new TCanvas();
	TH1F *hpttrk = (TH1F*)InFile2->Get("hpttrk");
	hpttrk->SetDirectory(0);
	hpttrk->Draw();

	TCanvas *c6 = new TCanvas();
	TH1F *hahpt = (TH1F*)InFile2->Get("hahpt");
	hahpt->SetDirectory(0);
	hahpt->Draw();

	TCanvas *c7 = new TCanvas();
	TH1F *hahm = (TH1F*)InFile2->Get("hahm");
	hahm->SetDirectory(0);
	hahm->Draw();
*/
//	TCanvas *c8 = new TCanvas();
	TH1F *eff2 = (TH1F*)InFile2->Get("eff2");
	eff2->SetDirectory(0);
	eff2->Draw("ep same");
	eff2->SetLineColor(kBlue);
	eff2->SetMarkerColor(kBlue);

	//Computing the ratio of the efficiencies
	Double_t xbins[5] = {10,20,40,60,200};
	TH1F *ratio = new TH1F("ratio","Ratio of the Two Efficiencies",4,xbins);
	ratio->Divide(eff,eff2,1.,1.,"B");
	ratio->SetDirectory(0);

	Double_t temp_e, temp_e2, temp_rerror, temp_rerror2, temp_ratio, temp_ratio_error;

	//Looping through all the bins and calculating error for the ratio plot
	for (Int_t j = 0; j < 5; j++){
		temp_e = eff->GetBinContent(j);
		temp_e2 = eff2->GetBinContent(j);

		cout << "Bin content of bin " << j << ":" << endl;
		cout << "eff = " << temp_e << endl;
		cout << "eff2 = " << temp_e2 << endl;

		temp_rerror = (eff->GetBinError(j))/temp_e;
		temp_rerror2 = (eff2->GetBinError(j))/temp_e2;

		temp_ratio = ratio->GetBinContent(j);
		temp_ratio_error = temp_ratio * sqrt(pow(temp_rerror,2)+pow(temp_rerror2,2));

		ratio->SetBinError(j,temp_ratio_error);
		cout << "Ratio is " << temp_ratio << endl;
		cout << "Final error is " << temp_ratio_error << endl << endl;
	}

	gStyle->SetOptStat(0);

	leg = new TLegend(.5,.3,.79,.49);
	leg->AddEntry(eff,"CSC Hits Only","l");
	leg->AddEntry(eff2,"CSC and RPC Hits","l");
	leg->SetHeader("Legend");
	leg->Draw();

	InFile2->Close();

	gPad->Modified();

	TCanvas *c9 = new TCanvas();
	ratio->Draw();
	ratio->SetAxisRange(0,1,"Y");
	gPad->Modified();
	
	TFile *OutFile = TFile::Open("efficiency.root","RECREATE");
	c4->Write();
	c4->Print("Efficiency.pdf");
	c9->Write();
	c9->Print("Efficiency Ratio.pdf");
	OutFile->Close();
}

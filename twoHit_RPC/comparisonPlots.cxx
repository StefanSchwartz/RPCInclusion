//This ROOT macro combines the function of dispHist and dispThreeHit for simplicity and better comparison

void comparisonPlots(){

	//Opening the input files
	TFile *InFile = TFile::Open("twoHit_RPC_pt_plots.root","READ");
	TFile *InFile2 = TFile::Open("threeHit_RPC_pt_plots.root","READ");

	/////////////////////////////////////////////////////////////////////////////////////////////
	//RATE COMPARISON PLOT
	/////////////////////////////////////////////////////////////////////////////////////////////
	TCanvas *c1 = new TCanvas();

	double content;
	bool RPC = true;

	//Get all the rate histograms	
	TH1F *hrate_twohit = (TH1F*)InFile->Get("hrate_mode2");
	TH1F *hrate_twohit_rpc = (TH1F*)InFile->Get("hrate_rpc_mode2");
	TH1F *hrate_threehit = (TH1F*)InFile2->Get("hrate_mode2");

	double twohit_entries = hrate_twohit->GetEntries();
	double twohit_rpc_entries = hrate_twohit_rpc->GetEntries();
	double threehit_entries = hrate_threehit->GetEntries();

	for (int i = 1; i < 9; i++){
		content = hrate_twohit->GetBinContent(i);
		content = content/twohit_entries;
		hrate_twohit->SetBinContent(i, content);

		content = hrate_twohit_rpc->GetBinContent(i);
		content = content/twohit_rpc_entries;
		hrate_twohit_rpc->SetBinContent(i, content);

		content = hrate_threehit->GetBinContent(i);
		content = content/threehit_entries;
		hrate_threehit->SetBinContent(i, content);	
	}

	//Draw and fix graph settings
	hrate_twohit->Draw();
	hrate_twohit->SetLineColor(kRed);
	hrate_twohit->SetMarkerColor(kRed);
	hrate_twohit->SetXTitle("pT (GeV)");
	hrate_twohit->SetYTitle("Percent of Tracks");
	hrate_twohit->SetTitle("Rate Comparison");
	hrate_twohit->SetAxisRange(0,.16,"Y");
	
	//Add other histograms to the plot
	if (RPC){	
		hrate_twohit_rpc->Draw("same");
		hrate_twohit_rpc->SetLineColor(kGreen+3);
		hrate_twohit_rpc->SetMarkerColor(kGreen+3);
	}
	hrate_threehit->Draw("same");

	gStyle->SetOptStat(0);

	//Add a legend
	leg1 = new TLegend(.5,.7,.79,.89);
	leg1->AddEntry(hrate_twohit,"2 Hit CSC Track","l");
	if (RPC){
		leg1->AddEntry(hrate_twohit_rpc,"2 Hit Track w/ 3rd RPC Hit","l");
	}
	leg1->AddEntry(hrate_threehit,"3 Hit CSC Track","l");
	leg1->SetHeader("Legend");
	leg1->Draw();

	c1->Print("Rate Comparison.pdf");


	/////////////////////////////////////////////////////////////////////////////////////////////
	//EFFICIENCY COMPARISON PLOTS
	/////////////////////////////////////////////////////////////////////////////////////////////

	//------------------------------------------------------------------------------------------
	//7 GeV Cutoff

	//Creating a canvas for the efficiency plot
	TCanvas *c3 = new TCanvas();

	c3->SetLogx();

	//Get turn on curves for efficiency computation	
	TH1F *hall_twohit = (TH1F*)InFile->Get("csctfPt_all");
	TH1F *hcsc_twohit = (TH1F*)InFile->Get("csctfPt_7.0");
	TH1F *hrpc = (TH1F*)InFile->Get("csctfPt_rpc_7.0");
	TH1F *hall_threehit = (TH1F*)InFile2->Get("csctfPt_all");
	TH1F *hcsc_threehit = (TH1F*)InFile2->Get("csctfPt_7.0");

	//Get efficiency curves
	heff = new TGraphAsymmErrors(hcsc_twohit, hall_twohit);	
	heff_rpc = new TGraphAsymmErrors(hrpc, hall_twohit);
	heff_threehit = new TGraphAsymmErrors(hcsc_threehit, hall_threehit);

	//Set up plot
	heff->Draw("AP");
	heff->GetYaxis()->SetTitle("Efficiency");
	heff->GetXaxis()->SetTitle("pT (GeV), Log Scale");
	heff->SetTitle("Efficiency Comparison, 7 GeV Cutoff");
	heff->SetLineColor(kRed);
	heff->SetMarkerColor(kRed);
	heff->GetYaxis()->SetRangeUser(0,1.1);

	//Add other efficiency curves
	if (RPC){	
		heff_rpc->Draw("P");
		heff_rpc->SetLineColor(kGreen+3);
		heff_rpc->SetMarkerColor(kGreen+3);
	}
	heff_threehit->Draw("P");

	gStyle->SetOptStat(0);

	//Add legend
	leg3 = new TLegend(.6,.2,.89,.39);
	leg3->AddEntry(heff,"2 Hit CSC Track","l");
	if (RPC){
		leg3->AddEntry(heff_rpc,"2 Hit Track w/ 3rd RPC Hit","l");
	}
	leg3->AddEntry(heff_threehit,"3 Hit CSC Track","l");
	leg3->SetHeader("Legend");
	leg3->Draw();

	c3->Print("Efficiency Comparison 7 GeV.pdf");

	//----------------------------------------------------------------------------------
	//10 GeV Cutoff

	//Creating a canvas for the efficiency plot
	TCanvas *c3 = new TCanvas();

	c3->SetLogx();

	//Get turn on curves for efficiency computation	
	TH1F *hall_twohit = (TH1F*)InFile->Get("csctfPt_all");
	TH1F *hcsc_twohit = (TH1F*)InFile->Get("csctfPt_10.0");
	TH1F *hrpc = (TH1F*)InFile->Get("csctfPt_rpc_10.0");
	TH1F *hall_threehit = (TH1F*)InFile2->Get("csctfPt_all");
	TH1F *hcsc_threehit = (TH1F*)InFile2->Get("csctfPt_10.0");

	//Get efficiency curves
	heff = new TGraphAsymmErrors(hcsc_twohit, hall_twohit);	
	heff_rpc = new TGraphAsymmErrors(hrpc, hall_twohit);
	heff_threehit = new TGraphAsymmErrors(hcsc_threehit, hall_threehit);

	//Set up plot
	heff->Draw("AP");
	heff->GetYaxis()->SetTitle("Efficiency");
	heff->GetXaxis()->SetTitle("pT (GeV), Log Scale");
	heff->SetTitle("Efficiency Comparison, 10 GeV Cutoff");
	heff->SetLineColor(kRed);
	heff->SetMarkerColor(kRed);
	heff->GetYaxis()->SetRangeUser(0,1.1);

	//Add other efficiency curves
	if (RPC){	
		heff_rpc->Draw("P");
		heff_rpc->SetLineColor(kGreen+3);
		heff_rpc->SetMarkerColor(kGreen+3);
	}
	heff_threehit->Draw("P");

	gStyle->SetOptStat(0);

	//Add legend
	leg3 = new TLegend(.6,.2,.89,.39);
	leg3->AddEntry(heff,"2 Hit CSC Track","l");
	if (RPC){
		leg3->AddEntry(heff_rpc,"2 Hit Track w/ 3rd RPC Hit","l");
	}
	leg3->AddEntry(heff_threehit,"3 Hit CSC Track","l");
	leg3->SetHeader("Legend");
	leg3->Draw();	

	c3->Print("Efficiency Comparison 10 GeV.pdf");

	//--------------------------------------------------------------------------
	//12 GeV Cutoff

	//Creating a canvas for the efficiency plot
	TCanvas *c3 = new TCanvas();

	c3->SetLogx();

	//Get turn on curves for efficiency computation	
	TH1F *hall_twohit = (TH1F*)InFile->Get("csctfPt_all");
	TH1F *hcsc_twohit = (TH1F*)InFile->Get("csctfPt_12.0");
	TH1F *hrpc = (TH1F*)InFile->Get("csctfPt_rpc_12.0");
	TH1F *hall_threehit = (TH1F*)InFile2->Get("csctfPt_all");
	TH1F *hcsc_threehit = (TH1F*)InFile2->Get("csctfPt_12.0");

	//Get efficiency curves
	heff = new TGraphAsymmErrors(hcsc_twohit, hall_twohit);	
	heff_rpc = new TGraphAsymmErrors(hrpc, hall_twohit);
	heff_threehit = new TGraphAsymmErrors(hcsc_threehit, hall_threehit);

	//Set up plot
	heff->Draw("AP");
	heff->GetYaxis()->SetTitle("Efficiency");
	heff->GetXaxis()->SetTitle("pT (GeV), Log Scale");
	heff->SetTitle("Efficiency Comparison, 12 GeV Cutoff");
	heff->SetLineColor(kRed);
	heff->SetMarkerColor(kRed);
	heff->GetYaxis()->SetRangeUser(0,1.1);

	//Add other efficiency curves
	if (RPC){	
		heff_rpc->Draw("P");
		heff_rpc->SetLineColor(kGreen+3);
		heff_rpc->SetMarkerColor(kGreen+3);
	}
	heff_threehit->Draw("P");

	gStyle->SetOptStat(0);

	//Add legend
	leg3 = new TLegend(.6,.2,.89,.39);
	leg3->AddEntry(heff,"2 Hit CSC Track","l");
	if (RPC){
		leg3->AddEntry(heff_rpc,"2 Hit Track w/ 3rd RPC Hit","l");
	}
	leg3->AddEntry(heff_threehit,"3 Hit CSC Track","l");
	leg3->SetHeader("Legend");
	leg3->Draw();	

	c3->Print("Efficiency Comparison 12 GeV.pdf");

	//----------------------------------------------------------------------------------
	//16 GeV Cutoff
	
	//Creating a canvas for the efficiency plot
	TCanvas *c3 = new TCanvas();

	c3->SetLogx();

	//Get turn on curves for efficiency computation	
	TH1F *hall_twohit = (TH1F*)InFile->Get("csctfPt_all");
	TH1F *hcsc_twohit = (TH1F*)InFile->Get("csctfPt_16.0");
	TH1F *hrpc = (TH1F*)InFile->Get("csctfPt_rpc_16.0");
	TH1F *hall_threehit = (TH1F*)InFile2->Get("csctfPt_all");
	TH1F *hcsc_threehit = (TH1F*)InFile2->Get("csctfPt_16.0");

	//Get efficiency curves
	heff = new TGraphAsymmErrors(hcsc_twohit, hall_twohit);	
	heff_rpc = new TGraphAsymmErrors(hrpc, hall_twohit);
	heff_threehit = new TGraphAsymmErrors(hcsc_threehit, hall_threehit);

	//Set up plot
	heff->Draw("AP");
	heff->GetYaxis()->SetTitle("Efficiency");
	heff->GetXaxis()->SetTitle("pT (GeV), Log Scale");
	heff->SetTitle("Efficiency Comparison, 16 GeV Cutoff");
	heff->SetLineColor(kRed);
	heff->SetMarkerColor(kRed);
	heff->GetYaxis()->SetRangeUser(0,1.1);

	//Add other efficiency curves
	if (RPC){	
		heff_rpc->Draw("P");
		heff_rpc->SetLineColor(kGreen+3);
		heff_rpc->SetMarkerColor(kGreen+3);
	}
	heff_threehit->Draw("P");

	gStyle->SetOptStat(0);

	//Add legend
	leg3 = new TLegend(.6,.2,.89,.39);
	leg3->AddEntry(heff,"2 Hit CSC Track","l");
	if (RPC){
		leg3->AddEntry(heff_rpc,"2 Hit Track w/ 3rd RPC Hit","l");
	}
	leg3->AddEntry(heff_threehit,"3 Hit CSC Track","l");
	leg3->SetHeader("Legend");
	leg3->Draw();

	c3->Print("Efficiency Comparison 16 GeV.pdf");

	////////////////////////////////////////////////////////////////////////////////////////
	//CLOSING FILES
	///////////////////////////////////////////////////////////////////////////////////////

	InFile->Close();
	InFile2->Close();
}

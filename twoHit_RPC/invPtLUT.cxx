//This ROOT macro creates and saves plots for dphi vs pt and invpt

void invPtLUT(){

	//Open input files
	TFile *InFile = TFile::Open("threeHit_RPC_pt_plots.root","READ");
	TFile *InFile2 = TFile::Open("twoHit_RPC_pt_plots.root","READ");

	////////////////////////////////////////////////////////////////////////////////////////
	//INVERSE PT PLOTS
	////////////////////////////////////////////////////////////////////////////////////////

	TCanvas *c1 = new TCanvas();

	TH2F *hinvpt = (TH2F*)InFile->Get("hdphi12_invPt");
	hinvpt->Draw("COLz");	
	hinvpt->SetYTitle("dPhi");
	hinvpt->SetXTitle("chg/Reco pT (1/GeV)");
	gStyle->SetOptStat(0);

	c1->Print("invPt Three Hit Plot.pdf");

	//Profile plot of the same thing
	TCanvas *c3 = new TCanvas();

	TH2F *hinvpt_prof = (TH2F*)InFile->Get("hdphi12_invPt_Prof");
	hinvpt_prof->Draw();	
	hinvpt_prof->SetYTitle("dPhi");
	hinvpt_prof->SetXTitle("chg/Reco pT (1/GeV)");
	gStyle->SetOptStat(0);

	c3->Print("invPt Three Hit Prof Plot.pdf");

	TCanvas *c1 = new TCanvas();

	TH2F *hinvpt_rpc = (TH2F*)InFile2->Get("hdphi_cluster2_csc1_invPt");
	hinvpt_rpc->Draw("COLz");	
	hinvpt_rpc->SetYTitle("dPhi");
	hinvpt_rpc->SetXTitle("chg/Reco pT (1/GeV)");
	gStyle->SetOptStat(0);

	c1->Print("invPt Two Hit RPC Plot.pdf");

	//Profile plot of the same thing
	TCanvas *c3 = new TCanvas();

	TH2F *hinvpt_rpc_prof = (TH2F*)InFile2->Get("hdphi_cluster2_csc1_invPt_Prof");
	hinvpt_rpc_prof->Draw();	
	hinvpt_rpc_prof->SetYTitle("dPhi");
	hinvpt_rpc_prof->SetXTitle("chg/Reco pT (1/GeV)");
	gStyle->SetOptStat(0);

	c3->Print("invPt Two Hit RPC Prof Plot.pdf");


	////////////////////////////////////////////////////////////////////////////////////////
	//DPHI VS PT PLOTS
	///////////////////////////////////////////////////////////////////////////////////////

	TCanvas *c2 = new TCanvas();

	TH2F *hpt = (TH2F*)InFile->Get("hdphi12_Pt");
	hpt->Draw("COLz");	
	hpt->SetYTitle("dPhi");
	hpt->SetXTitle("Reco pT (GeV)");
	hpt->GetYaxis()->SetRangeUser(-.03,.05);
	gStyle->SetOptStat(0);

	c2->Print("Pt Three Hit Plot.pdf");

	//Pt Scatter Plot for RPC data
	TCanvas *c5 = new TCanvas();

	TH2F *hpt2 = (TH2F*)InFile2->Get("hdphi_cluster2_csc1_Pt");
	hpt2->Draw("COLz");	
	hpt2->SetYTitle("dPhi");
	hpt2->SetXTitle("Reco pT (GeV)");
	hpt2->GetYaxis()->SetRangeUser(-.03,.05);
	gStyle->SetOptStat(0);

	c5->Print("Pt Two Hit RPC Plot.pdf");

	//Profile version of the same thing, with rpc/three hit comparison	
	TCanvas *c4 = new TCanvas();

	TH2F *hpt_prof = (TH2F*)InFile->Get("hdphi12_Pt_Prof");
	hpt_prof->Draw();	
	hpt_prof->SetYTitle("dPhi");
	hpt_prof->SetXTitle("Reco pT (GeV)");
	hpt_prof->GetYaxis()->SetRangeUser(0,0.03);
	gStyle->SetOptStat(0);

	//Gets values for a pt lookup table
	double content;
	double low_edge;
	double high_edge;

	for (int i = 1; i < 50; i++){
		content = hpt_prof->GetBinContent(i);
		low_edge = (120.0 / 50.0) * double(i);
		high_edge = (120.0 / 50.0) * double(i+1);
		cout << low_edge << " to " << high_edge << ": " << content << endl;
	}

	//Adds the rpc curve to the plot
	TH2F *hpt_prof2 = (TH2F*)InFile2->Get("hdphi_cluster2_csc1_Pt_Prof");
	hpt_prof2->Draw("same");
	hpt_prof2->SetMarkerColor(kRed);
	hpt_prof2->SetLineColor(kRed);

	//Creating a legend
	leg1 = new TLegend(.5,.7,.79,.89);
	leg1->AddEntry(hpt_prof,"CSC 1-2 dphi","l");
	leg1->AddEntry(hpt_prof2,"RPC Cluster dphi","l");
	leg1->SetHeader("Legend");
	leg1->Draw();

	c4->Print("Pt Prof Comparison Plot.pdf");

	////////////////////////////////////////////////////////////////////////////////////////
	//CLOSING FILES
	////////////////////////////////////////////////////////////////////////////////////////

	InFile->Close();
	InFile2->Close();
}

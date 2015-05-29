###########################################
# Plot Maker for quick plots
#
# by David Curry
#
# 7.22.1014
###########################################


from ROOT import *
from matplotlib import interactive


# Open root file of saved Hists
file  = TFile('twoHit_RPC_pt_plots.root')

# common strings
header = 'MinBias 2012D, All Q, CSCTF P_{t} > 3 , Two Hit CSCTF Tracks(1-3)'


#  ==== what to plot =====

rate = False
rate = True

eff = False
eff = True

dphi = False
#dphi = True

# ======================


if dphi:
    # ===== Delta Phi PLots ========

    hdphi_cluster_csc1_plus  = file.Get('hdphi_cluster_csc1_plus')
    hdphi_cluster_csc1_minus = file.Get('hdphi_cluster_csc1_minus')
    
    hdphi_cluster_csc3_plus  = file.Get('hdphi_cluster_csc3_plus')
    hdphi_cluster_csc3_minus = file.Get('hdphi_cluster_csc3_minus')

    cdPhi1 = TCanvas('cdPhi1')
    sdPhi1 = THStack('sdPhi1', '')
    
    sdPhi1.Add(hdphi_cluster_csc1_plus)
    sdPhi1.Add(hdphi_cluster_csc1_minus)
    
    hdphi_cluster_csc1_plus.SetLineColor(kRed)
    hdphi_cluster_csc1_plus.SetStats(0)
    
    
    sdPhi1.Draw('nostack')
    sdPhi1.SetTitle('')
    sdPhi1.GetYaxis().SetTitle('Entries')
    sdPhi1.GetXaxis().SetTitle('RPC2 Cluster - CSC1: delta phi[rad]')
    
    l_1 = TLatex()
    l_1.SetNDC()
    l_1.SetTextSize(0.03)
    l_1.DrawLatex(0.1, 0.93, header)
    l_1.Draw('same')
    
    
    
    cdPhi3 = TCanvas('cdPhi3')
    sdPhi3 = THStack('sdPhi3', '')
    
    sdPhi3.Add(hdphi_cluster_csc3_plus)
    sdPhi3.Add(hdphi_cluster_csc3_minus)
    
    hdphi_cluster_csc3_plus.SetLineColor(kRed)
    hdphi_cluster_csc3_plus.SetStats(0)
    
    sdPhi3.Draw('nostack')
    sdPhi3.SetTitle('')
    sdPhi3.GetYaxis().SetTitle('Entries')
    sdPhi3.GetXaxis().SetTitle('RPC2 Cluster - CSC3: delta phi[rad]')
    
    l_1 = TLatex()
    l_1.SetNDC()
    l_1.SetTextSize(0.03)
    l_1.DrawLatex(0.1, 0.93, header)
    l_1.Draw('same')
    
    
    '''
    leg = TLegend(0.62,0.6,0.9,0.9)
    leg.SetFillStyle(0)
    leg.SetBorderSize(0)
    leg.AddEntry(hres, 'Legacy', 'l')
    leg.AddEntry(hres_rpc, 'Legacy(w/RPC)', 'l')
    leg.Draw('same')
    '''
    
    
    cdPhi1.SaveAs('plots/dphi12.pdf')
    cdPhi3.SaveAs('plots/dPhi23.pdf')
    
    # ==================================================
    
    
    # ======== Delta Phi vs Pt Plots =================
    
    hdphi_cluster2_csc1_invPt = file.Get('hdphi_cluster2_csc1_invPt')
    hdphi_cluster2_csc3_invPt = file.Get('hdphi_cluster2_csc3_invPt')
    
    cdPhiPt1 = TCanvas('cdPhiPt1')
    
    hdphi_cluster2_csc1_invPt.SetStats(0)
    hdphi_cluster2_csc1_invPt.Draw('COL')
    
    hdphi_cluster2_csc1_invPt.SetTitle('')
    hdphi_cluster2_csc1_invPt.GetXaxis().SetTitle('Gbl Muon Charge / Gbl Muon Pt[GeV]')
    hdphi_cluster2_csc1_invPt.GetYaxis().SetTitle('RPC2 Cluster - CSC1: delta phi[rad]')
    
    l_1 = TLatex()
    l_1.SetNDC()
    l_1.SetTextSize(0.03)
    l_1.DrawLatex(0.1, 0.93, header)
    l_1.Draw('same')
    
    cdPhiPt1.SaveAs('plots/dphi12_invpt.pdf')
    
    
    cdPhiPt2 = TCanvas('cdPhiPt2')
    
    hdphi_cluster2_csc3_invPt.SetStats(0)
    hdphi_cluster2_csc3_invPt.Draw('COL')
    
    hdphi_cluster2_csc3_invPt.SetTitle('')
    hdphi_cluster2_csc3_invPt.GetXaxis().SetTitle('Gbl Muon Charge / Gbl Muon Pt[GeV]')
    hdphi_cluster2_csc3_invPt.GetYaxis().SetTitle('RPC2 Cluster - CSC3: delta phi[rad]')
    
    l_1 = TLatex()
    l_1.SetNDC()
    l_1.SetTextSize(0.03)
    l_1.DrawLatex(0.1, 0.93, "MinBias 2012D, All Q, CSCTF P_{t} > 5 , Two Hit CSCTF Tracks(1-3) ")
    l_1.Draw('same')
    
    cdPhiPt2.SaveAs('plots/dphi23_invpt.pdf')
    
    

# ==================================================


'''
# ======== Resolution plots =============

hres     = file.Get('hres')
hres_rpc_cluster = file.Get('hres_rpc_cluster')

norm     = 1/ hres.Integral() 
norm_rpc = 1/ hres_rpc_cluster.Integral()

hres.Scale(norm)
hres_rpc_cluster.Scale(norm_rpc)

cRes = TCanvas('cRes')
sRes = THStack('sRes', '')

sRes.Add(hres)
sRes.Add(hres_rpc_cluster)

hres.SetLineColor(kRed)
hres.SetStats(0)

sRes.Draw('nostack')
sRes.SetTitle('')
sRes.GetYaxis().SetTitle('Entries')
sRes.GetXaxis().SetTitle(' (CSCTF(front bit) Pt - Gbl Muon Pt) / Gbl Muon Pt')

l_1 = TLatex()
l_1.SetNDC()
l_1.SetTextSize(0.03)
l_1.DrawLatex(0.1, 0.93, header)
l_1.Draw('same')

leg = TLegend(0.62,0.6,0.9,0.9)
leg.SetFillStyle(0)
leg.SetBorderSize(0)
leg.AddEntry(hres, 'Legacy', 'l')
leg.AddEntry(hres_rpc_cluster, 'Legacy(w/RPC)', 'l')
leg.Draw('same')

cRes.SaveAs('plots/resolution.pdf')


hres_rpc_cluster_rear = file.Get('hres_rpc_cluster_rear')

norm_rpc = 1/ hres_rpc_cluster_rear.Integral()
hres_rpc_cluster_rear.Scale(norm_rpc)

cResR = TCanvas('cResR')
sResR = THStack('sResR', '')

sResR.Add(hres)
sResR.Add(hres_rpc_cluster_rear)

hres.SetLineColor(kRed)
hres.SetStats(0)

sResR.Draw('nostack')
sResR.SetTitle('')
sResR.GetYaxis().SetTitle('Entries')
sResR.GetXaxis().SetTitle(' (CSCTF(rear bit) Pt - Gbl Muon Pt) / Gbl Muon Pt')

l_1 = TLatex()
l_1.SetNDC()
l_1.SetTextSize(0.03)
l_1.DrawLatex(0.1, 0.93, header)
l_1.Draw('same')

leg1 = TLegend(0.62,0.6,0.9,0.9)
leg1.SetFillStyle(0)
leg1.SetBorderSize(0)
leg1.AddEntry(hres, 'Legacy', 'l')
leg1.AddEntry(hres_rpc_cluster_rear, 'Legacy(w/RPC)', 'l')
leg1.Draw('same')

cResR.SaveAs('plots/resolution_rear.pdf')
'''

# ==============================================


# Rate Plot

if rate:
    
    hrate_mode2 = file.Get('hrate_mode2')
    hrate_rpc_mode2 = file.Get('hrate_rpc_mode2')
    
    cRate = TCanvas('cRate')
    sRate = THStack('sRate', '')
    
    sRate.Add(hrate_mode2)
    sRate.Add(hrate_rpc_mode2)
    
    hrate_rpc_mode2.SetLineColor(kRed)
    hrate_rpc_mode2.SetStats(0)
    
    sRate.Draw('nostack')
    sRate.SetTitle('')
    sRate.GetYaxis().SetTitle('Entries')
    sRate.GetXaxis().SetTitle(' pT [GeV]')
    
    l_1 = TLatex()
    l_1.SetNDC()
    l_1.SetTextSize(0.03)
    l_1.DrawLatex(0.1, 0.93, header)
    l_1.Draw('same')
    
    leg2 = TLegend(0.62,0.6,0.9,0.9)
    leg2.SetFillStyle(0)
    leg2.SetBorderSize(0)
    leg2.AddEntry(hrate_mode2, 'Legacy', 'l')
    leg2.AddEntry(hrate_rpc_mode2, 'Legacy(w/RPC)', 'l')
    leg2.Draw('same')
    
    cRate.SaveAs('plots/rate.pdf')


# ================================================================

if eff:

    pt_all = file.Get('csctfPt_all')
    
    hpt7     = file.Get('csctfPt_16.0')
    hpt7_rpc = file.Get('csctfPt_rpc_16.0')

    tg7     = TGraphAsymmErrors(hpt7, pt_all, '')
    tg7_rpc = TGraphAsymmErrors(hpt7_rpc, pt_all, '')
    
    c7 = TCanvas('c7')
    c7.SetLogx(1)

    tg7.SetTitle('Turn On Efficiency: 16 GeV')
    tg7.GetXaxis().SetTitle('Global #mu P_{t} [GeV]')
    tg7.GetXaxis().SetTitleOffset(1.4)
    tg7_rpc.SetLineColor(kRed)
    tg7.SetLineColor(kBlue)
    tg7.Draw('APE')
    tg7_rpc.Draw('PEsame')

    c7.SaveAs('plots/efficiency_7GeV.pdf')
    

raw_input('Press return to continue...')

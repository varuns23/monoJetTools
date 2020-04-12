from ROOT import *
import os
gROOT.SetBatch(1)
regions = { region:TFile("%s/PSW_SF.root"%region) for region in os.listdir(".") if os.path.isdir(region) }
temp = regions["SignalRegion"]
store = []
colors = [kRed,kBlue,kViolet,kGreen,kOrange,kBlack]

procs = {
    "ZJets":("SignalRegion"),
    "WJets":("SignalRegion","SingleEleCR","SingleMuCR"),
    "GJets":("GammaCR"),
    "DYJets":("DoubleEleCR","DoubleMuCR")
}
def plotdiff(tdirs,process,psw):
    cname = "%s_%s_diff"%(process,psw)
    print cname
    c=TCanvas(cname,cname)
    gStyle.SetOptStat(0)
    c.SetGrid()
    hsup = [ tdir.Get(psw+"Up") for tdir in tdirs ]
    hsdn = [ tdir.Get(psw+"Down") for tdir in tdirs ]
    coliter=iter(colors)
    bins = []
    for up,dn,tdir in zip(hsup,hsdn,tdirs):
        up.Smooth(1,"r"); dn.Smooth(1,"r")
        for i,bin in enumerate(up): up.SetBinContent(i,abs(1-bin))
        for i,bin in enumerate(dn): dn.SetBinContent(i,abs(1-bin))
        color = next(coliter)
        up.SetName(tdir.region)
        up.SetLineColor(color)
        up.SetLineWidth(2)
        up.SetMarkerColor(color)
        up.SetMarkerStyle(22)
        up.SetMarkerSize(2)
        dn.SetLineColor(color)
        dn.SetLineWidth(2)
        dn.SetMarkerColor(color)
        dn.SetMarkerStyle(23)
        dn.SetMarkerSize(2)
        bins += list(up)[1:-1] + list(dn)[1:-1]
    ymin = min(bins)*0.8
    ymax = max(bins)*1.2
    for up,dn in zip(hsup,hsdn):
        up.GetYaxis().SetRangeUser(ymin,ymax)
        up.Draw("HIST PL SAME")
        dn.Draw("HIST PL SAME")
    leg = c.BuildLegend()
    leg.Clear()
    leg.SetFillColor(kWhite);
    leg.SetFillStyle(0);
    for hs in hsup: leg.AddEntry(hs,hs.GetName(),"l")
    leg.Draw()
    store.append(c)
    c.SaveAs("/afs/hep.wisc.edu/home/ekoenig4/public_html/MonoZprimeJet/test/PSWeights/%s.png"%c.GetName())
def plotvar(tdirs,process,psw):
    cname = "%s_%s_var"%(process,psw)
    print cname
    c=TCanvas(cname,cname)
    gStyle.SetOptStat(0)
    c.SetGrid()
    hsup = [ tdir.Get(psw+"Up") for tdir in tdirs ]
    hsdn = [ tdir.Get(psw+"Down") for tdir in tdirs ]
    coliter=iter(colors)
    bins = []
    for up,dn,tdir in zip(hsup,hsdn,tdirs):
        up.Smooth(1,"r"); dn.Smooth(1,"r")
        color = next(coliter)
        up.SetName(tdir.region)
        up.SetLineColor(color)
        up.SetLineWidth(2)
        dn.SetLineColor(color)
        dn.SetLineWidth(2)
        bins += list(up)[1:-1] + list(dn)[1:-1]
    ymin = min(bins)*0.8
    ymax = max(bins)*1.2
    for up,dn in zip(hsup,hsdn):
        up.GetYaxis().SetRangeUser(ymin,ymax)
        up.Draw("HIST L SAME")
        dn.Draw("HIST L SAME")
    leg = c.BuildLegend()
    leg.Clear()
    leg.SetFillColor(kWhite);
    leg.SetFillStyle(0);
    for hs in hsup: leg.AddEntry(hs,hs.GetName(),"l")
    leg.Draw()
    store.append(c)
    c.SaveAs("/afs/hep.wisc.edu/home/ekoenig4/public_html/MonoZprimeJet/test/PSWeights/%s.png"%c.GetName())
for process in iter( key.GetName() for key in temp.GetListOfKeys() ):
    tdirs = [ tfile.GetDirectory(process) for tfile in regions.values() ]
    for region,tdir in zip(regions,tdirs): tdir.region=region
    tdirs = [ tdir for tdir in tdirs if tdir.region in procs[process] ]
    for psw in iter( key.GetName().replace("Up","") for key in tdirs[0].GetListOfKeys() if "Up" in key.GetName() and "Con" in key.GetName() ):
        plotvar(tdirs,process,psw)
        plotdiff(tdirs,process,psw)


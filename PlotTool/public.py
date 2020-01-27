from ROOT import *
from sys import argv, path
from PlotTool import *
import os

gROOT.SetBatch(1)

samples = Region()
    
for variable in samples.args:          
    print "Plotting",variable
    samples.initiate(variable)
    c = TCanvas("c","canvas",800,800)
    gStyle.SetOptStat(0);
    gStyle.SetLegendBorderSize(0);
    c.SetLeftMargin(0.12);
    c.SetBottomMargin(0.12)
    c.SetLogy();
    c.cd();

    for mc in samples.MC_Info:
        samples.histo[mc].SetFillColor(samples.MC_Info[mc][TColor]);

    hs_datamc = THStack("hs_datamc","Data/MC comparison");

    hs_order = {}
    if (samples.name == "Cutflow"):
        if samples.region == "SignalRegion":lastBin = 9
        else:lastBin = 11
        for key in samples.SampleList:
            if not (key == "Data" or key == "Signal"):hs_order[str(samples.histo[key].GetBinContent(lastBin))] = key
    else:
        for key in samples.MC_Info:hs_order[str(samples.MC_Info[key][int])] = key
    keylist = hs_order.keys()
    keylist.sort(key=float)
    for order in keylist:hs_datamc.Add(samples.histo[hs_order[order]])
    hs_datamc.SetTitle("");
    min=pow(10,-6);max=pow(10,2.5);
    hs_datamc.SetMinimum(0.1 if not samples.args.normalize else hs_datamc.GetMaximum()*min);
    hs_datamc.SetMaximum(hs_datamc.GetMaximum()*max);

    hs_datamc.Draw("hist")
    hs_datamc.GetYaxis().SetTitle("Events")
    hs_datamc.GetYaxis().SetTitleOffset(1.5)
    hs_datamc.GetXaxis().SetTitle(samples.name)
    hs_datamc.GetXaxis().SetTitleOffset(1.5)
    
    if hasattr(samples,'signal'):
        samples.histo[samples.signal[0]].SetLineWidth(2)
        samples.histo[samples.signal[0]].Draw("HIST SAME")

    leg = TLegend(0.62,0.60,0.86,0.887173,"");
    if (hasattr(samples,'signal')):
        mx = samples.signal[0].split("_")[0].replace("Mx","")
        mv = samples.signal[0].split("_")[1].replace("Mv","")
        leg.AddEntry(samples.histo[samples.signal[0]],"M_{\chi}="+mx+", M_{v}="+mv)   
    leg.AddEntry(samples.histo['WJets'],"W#rightarrowl#nu","f");
    leg.AddEntry(samples.histo['DYJets'],"Z#rightarrow ll","F"); 
    leg.AddEntry(samples.histo['DiBoson'],"WW/WZ/ZZ","F");
    leg.AddEntry(samples.histo['QCD'], "QCD","F");
    leg.AddEntry(samples.histo['TTJets'], "Top Quark", "F"); 
    leg.AddEntry(samples.histo['GJets'],"#gamma+jets", "F"); 
    leg.AddEntry(samples.histo['ZJets'],"Z#rightarrow#nu#nu","F");
    leg.SetFillColor(kWhite);
    leg.SetFillStyle(0);
    leg.SetTextSize(0.025);
    leg.Draw();

    lumi_label = '%s' % float('%.3g' % (samples.lumi/1000.)) + " fb^{-1}"
    if (samples.args.normalize): lumi_label="Normalized"
    texS = TLatex(0.20,0.837173,("#sqrt{s} = 13 TeV, "+lumi_label));
    texS.SetNDC();
    texS.SetTextFont(42);
    texS.SetTextSize(0.040);
    texS.Draw();
    texS1 = TLatex(0.12092,0.907173,"#bf{CMS} : #it{Preliminary} (2018)");
    texS1.SetNDC();
    texS1.SetTextFont(42);
    texS1.SetTextSize(0.040);
    texS1.Draw();

    dir = os.getcwd().split("/")[-1]
    file_path="/afs/hep.wisc.edu/home/ekoenig4/public_html/MonoJet/APS/2018/"+dir+"Plots_EWK/"
    #print file_path
    sub = ""
    if (samples.args.allHisto):sub = "all"
    directory=os.path.join(os.path.dirname(file_path),sub)
    if not os.path.exists(directory):
        os.mkdir(directory,0755)
        print directory
    c.SaveAs(directory+"/mc_"+variable+".pdf")
    c.SaveAs(directory+"/mc_"+variable+".png")

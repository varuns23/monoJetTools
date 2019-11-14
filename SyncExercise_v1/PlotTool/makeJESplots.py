#!/cvmfs/cms.cern.ch/slc6_amd64_gcc630/cms/cmssw/CMSSW_10_1_6/external/slc6_amd64_gcc630/bin/python

from ROOT import *
from sys import argv, path
import Plot as plot
from cfg_saveplot import config
import os

gROOT.SetBatch(1)

samples = plot.datamc()
jes = config['Uncertainty']['jes']['unc']

for variable in samples.args:
    for region in config['regions']:
        if samples.region in region:
            break
    nhisto = config['regions'][region]
    print variable
    samples.initiate(variable+'_'+nhisto)
    samples.setUnc( [variable+'_'+jes['jesUp'][id]],[variable+'_'+jes['jesDown'][id]] )
    up,dn = samples.getUnc()
    band = plot.GetUncBand(up,dn)

    band.SetFillColor(40)
    band.SetLineColor(40)
    band.SetFillStyle(3005)

    c = TCanvas("c", "canvas",800,800);
    gStyle.SetOptStat(0);
    gStyle.SetLegendBorderSize(0);
    #c.SetLeftMargin(0.15);
    #c.SetLogy();
    #c.cd();
    
    pad1 = TPad("pad1","pad1",0.01,0.25,0.99,0.99);
    pad1.Draw(); pad1.cd();
    pad1.SetLogy();
    pad1.SetFillColor(0); pad1.SetFrameBorderMode(0); pad1.SetBorderMode(0);
    pad1.SetBottomMargin(0.);

    samples.histo['Data'].SetLineWidth(2)
    samples.histo['Data'].SetLineColor(kWhite);
    samples.histo['Data'].SetTitle("");
    samples.histo['Data'].GetXaxis().SetTitle("");
    samples.histo['Data'].GetXaxis().SetTickLength(0);
    samples.histo['Data'].GetXaxis().SetLabelOffset(999);
    samples.histo['Data'].GetYaxis().SetTitle("");
    samples.histo['Data'].GetYaxis().SetTickLength(0);
    samples.histo['Data'].GetYaxis().SetLabelOffset(999);
    samples.histo['Data'].SetLineColor(kBlack);
    samples.histo['Data'].SetMarkerStyle(20);
    samples.histo['Data'].SetMarkerSize(0.9);
    if (samples.options.normalize):samples.histo['Data'].Scale(1/samples.histo['Data'].Integral())

    for mc in samples.MC_Info:
        samples.histo[mc].SetTitle("");
        samples.histo[mc].GetXaxis().SetTitle("");
        samples.histo[mc].GetXaxis().SetTickLength(0);
        samples.histo[mc].GetXaxis().SetLabelOffset(999);
        samples.histo[mc].GetYaxis().SetTitle("");
        samples.histo[mc].GetYaxis().SetTickLength(0);
        samples.histo[mc].GetYaxis().SetLabelOffset(999);
        samples.histo[mc].SetFillColor(samples.MC_Info[mc][TColor]);
        if (samples.options.normalize):samples.histo[mc].Scale(1/samples.BkgIntegral)
        

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
    hs_datamc.SetMinimum(0.1 if not samples.options.normalize else hs_datamc.GetMaximum()*min);
    hs_datamc.SetMaximum(hs_datamc.GetMaximum()*max);

    hs_datamc.Draw("HIST")

    samples.histo['Data'].Draw('pex0same')

    if samples.signal != None:
        samples.histo[samples.signal[0]].SetLineWidth(2)
        samples.histo[samples.signal[0]].Draw("HIST SAME")

    band.Draw("5 same")
    #################################################

    leg = TLegend(0.62,0.60,0.86,0.887173,"");
    leg.AddEntry(samples.histo['Data'],"Data","lp");
    if (samples.signal != None): leg.AddEntry(samples.histo[samples.signal[0]], samples.signal[0])   
    leg.AddEntry(samples.histo['WJets'],"W#rightarrowl#nu","f");
    leg.AddEntry(samples.histo['DYJets'],"Z#rightarrow ll","F"); 
    leg.AddEntry(samples.histo['DiBoson'],"WW/WZ/ZZ","F");
    leg.AddEntry(samples.histo['QCD'], "QCD","F");
    leg.AddEntry(samples.histo['TTJets'], "Top Quark", "F"); 
    leg.AddEntry(samples.histo['GJets'],"#gamma+jets", "F"); 
    leg.AddEntry(samples.histo['ZJets'],"Z#rightarrow#nu#nu","F");
    leg.AddEntry(band,"JES","F")
    leg.SetFillColor(kWhite);
    leg.SetFillStyle(0);
    leg.SetTextSize(0.025);
    leg.Draw();

    lumi_label = '%s' % float('%.3g' % (samples.lumi/1000.)) + " fb^{-1}"
    if (samples.options.normalize): lumi_label="Normalized"
    texS = TLatex(0.20,0.837173,("#sqrt{s} = 13 TeV, "+lumi_label));
    texS.SetNDC();
    texS.SetTextFont(42);
    texS.SetTextSize(0.040);
    texS.Draw();
    texS1 = TLatex(0.12092,0.907173,"#bf{CMS} : #it{Preliminary} ("+samples.version+")");
    texS1.SetNDC();
    texS1.SetTextFont(42);
    texS1.SetTextSize(0.040);
    texS1.Draw();

    c.cd();
    pad2 = TPad("pad2","pad2",0.01,0.01,0.99,0.25);
    pad2.Draw(); pad2.cd();
    pad2.SetFillColor(0); pad2.SetFrameBorderMode(0); pad2.SetBorderMode(0);
    pad2.SetTopMargin(0);
    pad2.SetBottomMargin(0.35);

    ######################################

    Ratio = plot.GetRatio(samples.histo['Data'],hs_datamc.GetStack().Last())
    ratioUp = plot.GetRatio(up,hs_datamc.GetStack().Last())
    ratioDn = plot.GetRatio(dn,hs_datamc.GetStack().Last())
    ratioBa = plot.GetUncBand(ratioUp,ratioDn)

    rymin = 0.3; rymax = 1.7
    Ratio.GetYaxis().SetRangeUser(rymin,rymax);
    Ratio.SetStats(0);
    Ratio.GetYaxis().CenterTitle();
    Ratio.SetMarkerStyle(20);
    Ratio.SetMarkerSize(0.7);
    
    line = TLine(hs_datamc.GetXaxis().GetXmin(), 1.,hs_datamc.GetXaxis().GetXmax(), 1.);
    line.SetLineStyle(8);
    Ratio.Draw("A")
    ratioBa.SetFillColor(40)
    ratioBa.SetLineColor(40)
    ratioBa.SetFillStyle(3005)
    ratioBa.GetYaxis().SetLabelSize(0.14);
    ratioBa.GetYaxis().SetTitleSize(0.12);
    ratioBa.GetYaxis().SetLabelFont(42);
    ratioBa.GetYaxis().SetTitleFont(42);
    ratioBa.GetYaxis().SetTitleOffset(0.25);
    ratioBa.GetYaxis().SetNdivisions(100);
    ratioBa.GetYaxis().SetTickLength(0.05);
    
    ratioBa.GetXaxis().SetLabelSize(0);
    ratioBa.GetXaxis().SetTitleSize(0);
    ratioBa.GetXaxis().SetLabelFont(42);
    ratioBa.GetXaxis().SetTitleFont(42);
    ratioBa.GetXaxis().SetTitleOffset(999);
    ratioBa.GetXaxis().SetTickLength(0.05);
    ratioBa.SetTitle("")
    ratioBa.Draw("5 same")
    
    Ratio.Draw("pex0 same");
    Ratio.GetYaxis().SetLabelSize(0.14);
    Ratio.GetYaxis().SetTitleSize(0.12);
    Ratio.GetYaxis().SetLabelFont(42);
    Ratio.GetYaxis().SetTitleFont(42);
    Ratio.GetYaxis().SetTitleOffset(0.25);
    Ratio.GetYaxis().SetNdivisions(100);
    Ratio.GetYaxis().SetTickLength(0.05);
    
    Ratio.GetXaxis().SetLabelSize(0.15);
    Ratio.GetXaxis().SetTitleSize(0.12);
    Ratio.GetXaxis().SetLabelFont(42);
    Ratio.GetXaxis().SetTitleFont(42);
    Ratio.GetXaxis().SetTitleOffset(0.9);
    Ratio.GetXaxis().SetTickLength(0.05);
    line.SetLineColor(kBlack);
    line.Draw("same");
    
    c.Update();
    hs_datamc.GetYaxis().SetTitle("Events");
    hs_datamc.GetYaxis().SetTitleOffset(1.5);

    ###########################################

    nbins = samples.histo['Data'].GetNbinsX();
    xmin = hs_datamc.GetXaxis().GetXmin();
    xmax = hs_datamc.GetXaxis().GetXmax();
    xwmin = xmin;
    xwmax = xmax;
    
    xaxis = TGaxis(xmin,rymin,xmax,rymin,xwmin,xwmax,510);
    if type(samples.name) == str: xaxis.SetTitle(samples.name);
    xaxis.SetLabelFont(42);
    xaxis.SetLabelSize(0.10);
    xaxis.SetTitleFont(42);
    xaxis.SetTitleSize(0.12);
    xaxis.SetTitleOffset(1.2);
    xaxis.Draw("SAME");

    if (samples.name == "Cutflow"):
        xaxis.SetLabelOffset(-999)
        xaxis.SetTitle("");
        label = []
        for i in range(1,nbins+1):
            label.append(TLatex(i-0.5,rymin-0.2,hs_datamc.GetXaxis().GetBinLabel(i)));
	    label[i-1].SetTextSize(0.065);
	    label[i-1].SetTextAngle(-30.);
	    label[i-1].Draw("SAME");
      

    yaxis = TGaxis(xmin,rymin,xmin,rymax,rymin,rymax,6,"");
    yaxis.SetTitle("Data/MC");
    yaxis.SetLabelFont(42);
    yaxis.SetLabelSize(0.10);
    yaxis.SetTitleFont(42);
    yaxis.SetTitleSize(0.12);
    yaxis.SetTitleOffset(0.35);
    yaxis.Draw("SAME");

    dir = os.getcwd().split("/")[-1]
    file_path="/afs/hep.wisc.edu/home/ekoenig4/public_html/MonoZprimeJet/Plots"+samples.version+"/JetEnergyScalePlots_EWK/"+dir+"Plots_EWK/"
    #print file_path
    sub = ""
    if (samples.options.allHisto):sub = "all"
    if (samples.options.sub != None): sub = samples.options.sub
    directory=os.path.join(os.path.dirname(file_path),sub)
    if not os.path.exists(directory):
        os.mkdir(directory,0755)
        print directory
    c.SaveAs(directory+"/datamc_"+variable+".pdf")
    c.SaveAs(directory+"/datamc_"+variable+".png")
    
    
    

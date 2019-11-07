#!/usr/bin/env python

from ROOT import *
from sys import argv,path
import Plot as plot
from mcinfo import lumi_by_era
from collections import OrderedDict
import os

gROOT.SetBatch(1)

def compareEra(variable,samples,eraLumi):
    c = TCanvas("c", "canvas",800,800);
    gStyle.SetOptStat(0);
    gStyle.SetLegendBorderSize(0);
    c.SetLeftMargin(0.15);
    c.SetLogy();
    c.cd();

    colors = [kRed,kBlue,kGreen,kOrange+7,kMagenta,kTeal,kYellow-3]
    eraHisto = []
    for i,era in enumerate(eraLumi.keys()):
        eraFile = TFile.Open("DataEra/"+samples.Data_FileNames[samples.region]+"_"+era+".root")
        if not any(eraFile.GetListOfKeys()): continue # If this particular era is missing in this region, skip it
        era_hs = eraFile.Get(variable).Clone("Era "+era);era_hs.SetDirectory(0);eraFile.Close()
        era_hs.SetTitle("");
        era_hs.GetXaxis().SetTitle(samples.name)
        era_hs.GetYaxis().SetTitle("Events")
        era_hs.SetLineColor(colors[i]);
        era_hs.SetLineWidth(2)
        eraHisto.append(era_hs)

    eraHisto[0].Draw("HIST")
    max = eraHisto[0].GetMaximum()
    for era in eraHisto[1:]:
        era.Draw("HIST SAME")
        if (era.GetYaxis().GetXmax() > max): max = era.GetMaximum()
    print max
    eraHisto[0].GetYaxis().SetRangeUser(0.1,max*pow(10,2.5))
    
    #################################################
    
    leg = TLegend(0.62,0.60,0.86,0.887173,"");
    for era in eraHisto:
        leg.AddEntry(era,era.GetName(),"l")
    leg.SetFillColor(kWhite);
    leg.SetFillStyle(0);
    leg.SetTextSize(0.025);
    leg.Draw();
    lumi_label = '%s' % float('%.3g' % (samples.lumi/1000.))
    texS = TLatex(0.20,0.837173,("#sqrt{s} = 13 TeV, "+lumi_label+" fb^{-1}"));
    texS.SetNDC();
    texS.SetTextFont(42);
    texS.SetTextSize(0.040);
    texS.Draw();
    texS1 = TLatex(0.12092,0.907173,"#bf{CMS} : #it{Preliminary} ("+samples.version+ ")");
    texS1.SetNDC();
    texS1.SetTextFont(42);
    texS1.SetTextSize(0.040);
    texS1.Draw();

    ###############################################
    
    dir = os.getcwd().split("/")[-1]
    file_path="/afs/hep.wisc.edu/home/ekoenig4/public_html/MonoZprimeJet/Plots"+samples.version+"/"+dir+"Plots_EWK/DataEra/"
    #print file_path
    directory=os.path.join(os.path.dirname(file_path),"")
    if not os.path.exists(directory):
        os.mkdir(directory,0755)
        print directory
    c.SaveAs(directory+"/"+variable+".pdf")
    c.SaveAs(directory+"/"+variable+".png")
#################################################
        
samples = plot.datamc(argv)
eraLumi = OrderedDict(sorted(lumi_by_era[samples.region].items(),key=lambda t:t[0]))
for variable in argv[1:]:
    samples.initiate(variable)
    compareEra(variable,samples,eraLumi)   
    for era,lumi in eraLumi.items():
        eraFile = TFile.Open("DataEra/"+samples.Data_FileNames[samples.region]+"_"+era+".root")
        if not any(eraFile.GetListOfKeys()): continue
        samples = plot.datamc(argv,lumi=(lumi))
        samples.initiate(variable)
        print "Era",era,"lumi:",samples.lumi
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

        eraVariable = eraFile.Get(variable)
        eraVariable.SetLineWidth(2)
        eraVariable.SetLineColor(kWhite);
        eraVariable.SetTitle("");
        eraVariable.GetXaxis().SetTitle("");
        eraVariable.GetXaxis().SetTickLength(0);
        eraVariable.GetXaxis().SetLabelOffset(999);
        eraVariable.GetYaxis().SetTitle("");
        eraVariable.GetYaxis().SetTickLength(0);
        eraVariable.GetYaxis().SetLabelOffset(999);
        eraVariable.SetLineColor(kBlack);
        eraVariable.SetMarkerStyle(20);
        eraVariable.SetMarkerSize(0.9);
        
        for mc in samples.MC_Info:
            samples.histo[mc].SetTitle("");
            samples.histo[mc].GetXaxis().SetTitle("");
            samples.histo[mc].GetXaxis().SetTickLength(0);
            samples.histo[mc].GetXaxis().SetLabelOffset(999);
            samples.histo[mc].GetYaxis().SetTitle("");
            samples.histo[mc].GetYaxis().SetTickLength(0);
            samples.histo[mc].GetYaxis().SetLabelOffset(999);
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
        min=0.1;max=pow(10,2.5);
        hs_datamc.SetMinimum(min);
        hs_datamc.SetMaximum(hs_datamc.GetMaximum()*max);
        
        hs_datamc.Draw("HIST")
        
        eraVariable.Draw('pex0same')
        
        if samples.signal != None:samples.histo[samples.signal[0]].Draw("HIST SAME")
        
        #################################################
        
        leg = TLegend(0.62,0.60,0.86,0.887173,"");
        leg.AddEntry(eraVariable,"Data "+era+" Era","lp");
        if (samples.signal != None): leg.AddEntry(samples.histo[samples.signal[0]], samples.signal[0])   
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
        lumi_label = '%s' % float('%.3g' % (samples.lumi/1000.))
        texS = TLatex(0.20,0.837173,("#sqrt{s} = 13 TeV, "+lumi_label+" fb^{-1}"));
        texS.SetNDC();
        texS.SetTextFont(42);
        texS.SetTextSize(0.040);
        texS.Draw();
        texS1 = TLatex(0.12092,0.907173,"#bf{CMS} : #it{Preliminary} (2017)");
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
        
        nbins = eraVariable.GetNbinsX();  
        Ratio = eraVariable.Clone("Ratio");
        last_hist = hs_datamc.GetStack().Last();
        last = last_hist.Clone("last");
        for ibin in range(1,nbins+1):
            stackcontent = last.GetBinContent(ibin);
            stackerror = last.GetBinError(ibin);
            datacontent = eraVariable.GetBinContent(ibin);
            dataerror = eraVariable.GetBinError(ibin);
            # print "bin: "+str(ibin)+"stackcontent: "+str(stackcontent)+" and data content: "+str(datacontent)
            ratiocontent=0;
            if(datacontent!=0 and stackcontent != 0):ratiocontent = ( datacontent) / stackcontent
            error=0;
            if(datacontent!=0 and stackcontent != 0): error = ratiocontent*((dataerror/datacontent)**2 + (stackerror/stackcontent)**2)**(0.5)
            else: error = 2.07
            # print "bin: "+str(ibin)+" ratio content: "+str(ratiocontent)+" and error: "+str(error);
            Ratio.SetBinContent(ibin,ratiocontent);
            Ratio.SetBinError(ibin,error);
            
        Ratio.GetYaxis().SetRangeUser(0.0,2.2);
        Ratio.SetStats(0);
        
        Ratio.GetYaxis().CenterTitle();
        Ratio.SetMarkerStyle(20);
        Ratio.SetMarkerSize(0.7);
        
        line = TLine(hs_datamc.GetXaxis().GetXmin(), 1.,hs_datamc.GetXaxis().GetXmax(), 1.);
        line.SetLineStyle(8);
        
        
        Ratio.Draw("pex0");
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
        
        nbins = eraVariable.GetNbinsX();
        xmin = hs_datamc.GetXaxis().GetXmin();
        xmax = hs_datamc.GetXaxis().GetXmax();
        xwmin = xmin;
        xwmax = xmax;
        
        xaxis = TGaxis(xmin,0,xmax,0,xwmin,xwmax,510);
        xaxis.SetTitle(samples.name);
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
                label.append(TLatex(i-0.5,-0.3,hs_datamc.GetXaxis().GetBinLabel(i)));
	        label[i-1].SetTextSize(0.065);
	        label[i-1].SetTextAngle(-30.);
	        label[i-1].Draw("SAME");
                
                
        yaxis = TGaxis(xmin,0,xmin,2.2,0,2.2,6,"");
        yaxis.SetTitle("Data/MC");
        yaxis.SetLabelFont(42);
        yaxis.SetLabelSize(0.10);
        yaxis.SetTitleFont(42);
        yaxis.SetTitleSize(0.12);
        yaxis.SetTitleOffset(0.35);
        yaxis.Draw("SAME");
        
        dir = os.getcwd().split("/")[-1]
        file_path="/afs/hep.wisc.edu/home/ekoenig4/public_html/MonoZprimeJet/Plots"+samples.version+"/"+dir+"Plots_EWK/DataEra/"
        #print file_path
        directory=os.path.join(os.path.dirname(file_path),era)
        if not os.path.exists(directory):
            os.mkdir(directory,0755)
            print directory
        c.SaveAs(directory+"/"+variable+".pdf")
        c.SaveAs(directory+"/"+variable+".png")

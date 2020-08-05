"""
Compare uncorrected MET distributions
Usage: python PlotTool/compareCorrected.py variable1 variable2
"""

from array import array
from PlotTool import *
from ROOT import *
import config
from sys import argv

group = parser.add_group(__file__,__doc__,"Script")

gROOT.SetBatch(1)

uncorrected_path = '/hdfs/store/user/ekoenig/MonoJet/PostFiles/%s/%s/20200212_uncorrectedMET/'

def compare():
    corrected = Region(autovar=True,show=False)
    uncorrected = Region(autovar=True,show=False,path=uncorrected_path%(corrected.year,corrected.region))
    def plot(variable,weight=None):
        for samples in (corrected,uncorrected):
            samples.initiate(variable,weight)
            samples.setSumOfBkg()

        def plot_type(corr,uncorr,name):
            table = Table(["%s Yields"%variable,"Corrected","Uncorrected","Ratio"])
            table.addRow(["Raw",corr.raw_total,uncorr.raw_total,corr.raw_total/uncorr.raw_total])
            table.addRow(["Scaled",corr.scaled_total,uncorr.scaled_total,corr.scaled_total/uncorr.scaled_total])
            print table
            if weight == '1': return
            c = TCanvas('c','c',800,800)
            gStyle.SetOptStat(0)
            gStyle.SetLegendBorderSize(0)
            c.SetLogy()
            
            pad1 = TPad("pad1","pad1",0.01,0.25,0.99,0.99);
            pad1.SetBottomMargin(0.);
            pad1.Draw(); pad1.cd();
            pad1.SetLogy();
            pad1.SetFillColor(0); pad1.SetFrameBorderMode(0); pad1.SetBorderMode(0);
            
            corr = corr.histo
            uncorr = uncorr.histo
            
            corr.SetTitle(""); uncorr.SetTitle("")
            
            DataStyle(corr)
            uncorr.SetLineColor(kRed)
            uncorr.SetLineWidth(2)
            
            corr.GetYaxis().SetTitle('Events / GeV')
            ymax = max( max(hs[ibin] for hs in (corr,uncorr)) for ibin in range(1,corr.GetNbinsX()+1) )
            uncorr.SetMaximum(ymax*pow(10,1.2))
            uncorr.Draw('hist')
            corr.Draw('pex0same')
            
            leg = getLegend(xmin=0.5,xmax=0.7,ymin=0.7,ymax=0.9)
            leg.AddEntry(corr,'Corrected %s'%name,'pl')
            leg.AddEntry(uncorr,'Uncorrected %s'%name,'l')
            leg.Draw()
            
            lumi_label = '%s' % float('%.3g' % (samples.lumi/1000.)) + " fb^{-1}"
            if (parser.args.normalize): lumi_label="Normalized"
            texLumi,texCMS = getCMSText(lumi_label,samples.year,scale=0.8)
            texLumi.Draw();
            texCMS.Draw();
            
            
            c.cd();
            pad2 = TPad("pad2","pad2",0.01,0.01,0.99,0.25);
            pad2.SetGridy()
            pad2.Draw(); pad2.cd();
            pad2.SetFillColor(0); pad2.SetFrameBorderMode(0); pad2.SetBorderMode(0);
            pad2.SetTopMargin(0);
            pad2.SetBottomMargin(0.35);
            
            ratio = GetRatio(corr,uncorr)
            RatioStyle(ratio,xname=samples.name,yname='Corr/Uncorr')
            ratio.Draw('pex0')
            
            line = getRatioLine(ratio.GetXaxis().GetXmin(),ratio.GetXaxis().GetXmax())
            line.Draw("same");
            
            c.Update()
            SaveAs(c,"%s_%s"%(name.lower(),variable),year=samples.year,region=samples.region,sub="Compare_Corrected")
        plot_type(corrected['SumOfBkg'],uncorrected['SumOfBkg'],'MC')
        plot_type(corrected['Data'],uncorrected['Data'],'Data')
    for variable in parser.args.argv:
        plot(variable)
compare()

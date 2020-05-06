#!/usr/bin/env python
import os
import sys
sys.path.append("PlotTool")
from PlotTool import *
import config
config.mclist = ["GJets"]

parser.add_argument("--plot",action="store_true")
parser.add_argument("--clean",action="store_true",default=True)
parser.add_argument("--save",action="store_true")

from ROOT import TCanvas,gStyle,kRed,kGreen

sig_path = "SigTemplate"
bkg_path = "BkgTemplate"

sig_template = Region(path=sig_path,autovar=True,show=0)
bkg_template = Region(path=bkg_path,autovar=True,show=0)

def save_template(template,output):
    output.cd()
    template.Write()
def temp_style(hs,color):
    hs.SetLineColor(color)
    hs.SetLineWidth(2)
    hs.SetFillColor(0)
    hs.SetFillStyle(0)

def PlotBkg(templates):
    data = templates["Data"].histo.Clone()
    gjets = templates["GJets"].histo.Clone()
    fake = templates["Fake"].histo.Clone()

    c = TCanvas("c", "canvas",800,800);
    gStyle.SetOptStat(0);
    gStyle.SetLegendBorderSize(0);
    c.SetLeftMargin(0.15);
    c.SetLogy();
    #c.cd();

    temp_style(fake,kRed)
    temp_style(gjets,kGreen)
    DataStyle(data)

    leg = getLegend()

    fake.Draw("hist")
    hslist = [fake]

    fake.GetYaxis().SetTitle("Events")
    fake.GetXaxis().SetTitle(templates.variable.xaxis_title)

    if parser.args.clean:
        gjets.Draw("hist same")
        data.Draw("pex0 same")
        hslist = hslist + [gjets,data]

        leg.AddEntry(data,"Fake Data","lp")
        leg.AddEntry(gjets,"Fake GJets","l")
    leg.AddEntry(fake,"Fake QCD","l")
    leg.Draw()

    SetBounds(hslist,scale=1,log=1)
    
    SaveAs(c,"fake_template_%s"%templates.varname,year=templates.year,sub="GammaPurity")
def BkgTemplates(variable,output):
    bkg_template.initiate(variable)

    fakeproc = Process("Fake",[],{},"fake",year=bkg_template.year,region=bkg_template.year)
    fakeproc.add( bkg_template["Data"] )

    bkg_template.processes["Fake"] = fakeproc

    if parser.args.save: save_template(bkg_template["Fake"].histo.Clone("fake_qcd"),output)
    if parser.args.clean: bkg_template["Fake"].histo.Add( bkg_template["GJets"].histo,-1 )
    if parser.args.plot: PlotBkg(bkg_template)

    return bkg_template
def PlotSig(templates,fake_templates):
    data = templates["Data"].histo.Clone()
    gjets = templates["GJets"].histo.Clone()

    c = TCanvas("c", "canvas",800,800);
    gStyle.SetOptStat(0);
    gStyle.SetLegendBorderSize(0);
    c.SetLeftMargin(0.15);
    c.SetLogy();
    #c.cd();

    temp_style(gjets,kGreen)
    DataStyle(data)

    leg = getLegend()

    gjets.Draw("hist")
    
    gjets.GetYaxis().SetTitle("Events")
    gjets.GetXaxis().SetTitle(templates.variable.xaxis_title)
    
    data.Draw("pex0 same")
    hslist = [data,gjets]
    
    leg.AddEntry(data,"Data","lp")
    leg.AddEntry(gjets,"Real GJets","l")

    if fake_templates:
        fake = fake_templates["Fake"].histo.Clone()
        temp_style(fake,kRed)
        fake.Draw("hist same")
        hslist = hslist + [fake]
        leg.AddEntry(fake,"Fake QCD","l")

    SetBounds(hslist,scale=1,log=1)
    leg.Draw()
    
    SaveAs(c,"real_template_%s"%templates.varname,year=templates.year,sub="GammaPurity")
def SigTemplates(variable,output,fake_templates=None):
    sig_template.initiate(variable)

    if parser.args.save:
        save_template(sig_template["Data"].histo.Clone("data"),output)
        save_template(sig_template["GJets"].histo.Clone("signal_gjets"),output)
    if parser.args.plot and fake_templates: PlotSig(sig_template,fake_templates)

    return sig_template

if __name__ == "__main__":
    parser.parse_args()

    output = None
    for variable in parser.args.argv:
        if parser.args.save:
            output = TFile("templates/template_%s.root"%variable,"recreate")
            print "Writing templates to",output.GetName()
        fake = BkgTemplates(variable,output)
        real = SigTemplates(variable,output,fake)

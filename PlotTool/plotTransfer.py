from ROOT import *
import os
from sys import argv
from PlotTool import *
import config
import re

gROOT.SetBatch(1)
# gROOT.SetBatch(0)

processMap = {
    "SignalRegion": {
        "Z":{"proc":"ZJets","label":"Z(#nu#nu)",'text':'nn'},
        "W":{"proc":"WJets","label":"W(l#nu)",'text':'ln'},
    },
    "SingleEleCR": {
        "W":{"proc":"WJets","label":"W(e#nu)",'text':'en'}
    }, 
    "SingleMuCR": {
        "W":{"proc":"WJets","label":"W(#mu#nu)",'text':'mn'}
    },
    "SingleLepCR": {
        "W":{"proc":"SumOfBkg","label":"W(l#nu)",'text':'ln'},
    },
    "DoubleEleCR": {
        "Z":{"proc":"DYJets","label":"Z(ee)",'text':'ee'}
    },
    "DoubleMuCR": {
        "Z":{"proc":"DYJets","label":"Z(#mu#mu)",'text':'mm'}
    },
    "DoubleLepCR": {
        "Z":{"proc":"SumOfBkg","label":"Z(ll)",'text':'ll'},
    },
    "GammaCR": {
        "G":{"proc":"GJets","label":"#gamma","text":""}
    }
}

rangemap = {
    "recoil" : {
        "SignalRegion" : {
            "SignalRegion" : (0,13),
            "SingleEleCR" :  (0,2),
            "SingleMuCR" : (0,1.5),
            "DoubleEleCR" : (0,20),
            "DoubleMuCR" : (0,20),
        },
        "DoubleMuCR" : {
            "SingleMuCR" : (0,0.25)
        },
        "DoubleEleCR" : {
            "SingleEleCR" : (0,0.3)
        },
        "DoubleLepCR" : {
            "SingleLepCR" : (0,0.3)
        }
    },
    "ChNemPtFrac" : { 
        "SignalRegion" : {
            "SignalRegion" : (0,5),
            "SingleEleCR" :  (0,6),
            "SingleMuCR" : (0,6),
            "DoubleEleCR" : (0,25),
            "DoubleMuCR" : (0,15),
        },
        "DoubleMuCR" : {
            "SingleMuCR" : (0,0.25)
        },
        "DoubleEleCR" : {
            "SingleEleCR" : (0,0.45)
        },
        "DoubleLepCR" : {
            "SingleLepCR" : (0,0.25)
        }
    }
}

varmap = {}

def tfmc_style(tf_proc,color=kBlue,xname=None):
    tf_proc.histo.SetLineWidth(2)
    tf_proc.histo.SetLineColor(color);
    tf_proc.histo.SetTitle("")
    tf_proc.histo.GetYaxis().SetTitle(tf_proc.name)
    tf_proc.histo.GetYaxis().SetTitleOffset(1.05)
    tf_proc.histo.GetYaxis().SetTitleSize(0.049)
    tf_proc.histo.GetYaxis().SetLabelSize(0.04)
    if xname is not None:
        tf_proc.histo.GetXaxis().SetTitle(xname)
    tf_proc.histo.GetXaxis().SetTitleOffset(1.2)
def tf_style(tf,color=kBlack,xname=None):
    tf.histo.SetLineWidth(2)
    tf.histo.SetLineColor(kBlack);
    tf.histo.SetMarkerStyle(20);
    tf.histo.SetMarkerSize(1.5);
    tf.histo.SetTitle("")
    tf.histo.GetYaxis().SetTitle(tf.name)
    tf.histo.GetYaxis().SetTitleSize(0.049)
    tf.histo.GetYaxis().SetLabelSize(0.04)
    tf.histo.GetYaxis().SetTitleOffset(1.2)
    if xname is not None:
        tf.histo.GetXaxis().SetTitle(xname)
    tf.histo.GetXaxis().SetTitleOffset(1.2)
    tf.histo.GetXaxis().SetLabelSize(0.045);
    tf.histo.GetXaxis().SetTitleSize(0.045);
    tf.histo.GetXaxis().SetLabelFont(42);
    tf.histo.GetXaxis().SetTitleFont(42);
    tf.histo.GetXaxis().SetTitleOffset(1.1);
    tf.histo.GetXaxis().SetTickLength(0.05);
def SetBounds(tf,num_sample,den_sample):
    bins = list(tf.histo)[1:-1]
    avg = sum( ibin for ibin in bins ) / len(bins)
    maxdiff = max( abs(ibin - avg) for ibin in bins )
    tf.histo.SetMinimum( avg - 5*maxdiff )
    tf.histo.SetMaximum( avg + 5*maxdiff )
    
    return
    if not any(varmap): return
    if num_sample.region not in varmap: return
    if den_sample.region not in varmap[num_sample.region]: return
    yrange = varmap[num_sample.region][den_sample.region]
    tf.histo.SetMinimum(yrange[0]); tf.histo.SetMaximum(yrange[1])
def plotTF(num_sample,den_sample):
    lumi_label = '%s' % float('%.3g' % (num_sample.lumi/1000.)) + " fb^{-1}"
    year = num_sample.year
    varname = num_sample.varname
    
    num_info = processMap[num_sample.region][num_sample.num_boson]
    den_info = processMap[den_sample.region][den_sample.den_boson]
    num_proc = num_sample[num_info["proc"]]
    den_proc = den_sample[den_info["proc"]]

    tfname = "%s+jets / %s+jets" % (num_info['label'],den_info['label'])
    tf = Transfer(tfname,num_proc,den_proc)
    tf.printUnc()

    c = TCanvas("c", "canvas",800,800);
    gStyle.SetOptStat(0);
    gStyle.SetLegendBorderSize(0);
    #c.SetLogy();
    #c.cd();
    
    pad1 = TPad("pad1","pad1",0.01,0.01,0.99,0.99);
    pad1.Draw(); pad1.cd();
    # pad1.SetLogy();
    pad1.SetFillColor(0);
    pad1.SetFrameBorderMode(0);
    pad1.SetBorderMode(0);
    pad1.SetLeftMargin(0.15)
    # pad1.SetBottomMargin(0.);
    
    SetBounds(tf,num_sample,den_sample)
    tf.histo.Draw("axis")

    if (num_sample.region is 'SignalRegion' and (den_sample.region is 'SignalRegion' or den_sample.region is "GammaCR")):
        fullband = tf.getUncBand()
        tf.fullUnc(['Stat','QCD_Shape','QCD_Scale','QCD_Proc','QCD_EWK_Mix'])
        midband = tf.getUncBand()
        UncBandStyle(fullband,color=kGreen)
        fullband.Draw("2same")
        UncBandStyle(midband)
        midband.Draw("2same")
    else:
        uncband = tf.getUncBand()
        UncBandStyle(uncband)
        uncband.Draw("2same")
    tf.histo.Draw("pex0same")
    tf_style(tf,xname=num_sample.name)
    
    texCMS,texLumi = getCMSText(lumi_label,year,scale=0.8)
    leg = getLegend(xmin=0.45,xmax=0.85,ymin=0.6,scale=0.6)
    leg.AddEntry(tf.histo,"Transfer Factor (Stat Uncert)","p")
    leg.Draw()

    if len(varname.split("_")) == 2:
        variable = varname.split("_")[0]
        binning = '_'+varname.split("_")[1]
    else:
        variable = varname
        binning = ''

    tfproc = "%s%s%s%s" % (num_sample.num_boson,num_info['text'],den_sample.den_boson,den_info['text'])
    outname = "%s_%s" % (tfproc,num_sample.variable.base)
    SaveAs(c,outname,year=year,sub="TransferFactors/%s"%num_sample.variable.base)
def plotTF_datamc(num_sample,den_sample):
    lumi_label = '%s' % float('%.3g' % (num_sample.lumi/1000.)) + " fb^{-1}"
    year = num_sample.year
    varname = num_sample.varname
    
    num_info = processMap[num_sample.region][num_sample.num_boson]
    den_info = processMap[den_sample.region][den_sample.den_boson]
    num_proc = num_sample[num_info["proc"]]
    den_proc = den_sample[den_info["proc"]]
    num_data = num_sample["Data"]
    den_data = den_sample["Data"]

    tfname = "%s+jets / %s+jets" % (num_info['label'],den_info['label'])
    tf_data = Transfer(tfname,num_data,den_data)
    tf_proc = Transfer(tfname,num_proc,den_proc)

    c = TCanvas("c", "canvas",800,800);
    gStyle.SetOptStat(0);
    gStyle.SetLegendBorderSize(0);
    c.SetLeftMargin(0.5);
    #c.SetLogy();
    #c.cd();
    
    pad1 = TPad("pad1","pad1",0.01,0.25,0.99,0.99);
    pad1.Draw(); pad1.cd();
    pad1.SetFillColor(0);
    pad1.SetFrameBorderMode(0);
    pad1.SetBorderMode(0);
    pad1.SetBottomMargin(0.);
    
    SetBounds(tf_proc,num_sample,den_sample)
    tf_proc.histo.Draw("axis")
    uncband = tf_proc.getUncBand()
    UncBandStyle(uncband)
    uncband.Draw("2same")
    tf_proc.histo.Draw("histsame")
    tf_data.histo.Draw("pe0same")

    tfmc_style(tf_proc,xname=num_sample.name)

    tf_data.histo.SetLineWidth(2)
    tf_data.histo.SetLineColor(kBlack)
    tf_data.histo.SetMarkerStyle(20);
    tf_data.histo.SetMarkerSize(1.5);
    tf_data.histo.SetMarkerColor(kBlack)
    
    texCMS,texLumi = getCMSText(lumi_label,year)
    leg = getLegend(ymin=0.7,scale=0.8)
    leg.AddEntry(tf_data.histo,"Data","lp")
    leg.AddEntry(tf_proc.histo,"Background","l")
    leg.Draw()

    #######################################
    c.cd()
    pad2 = TPad("pad2","pad2",0.01,0.01,0.99,0.25);
    pad2.Draw(); pad2.cd();
    pad2.SetFillColor(0); pad2.SetFrameBorderMode(0); pad2.SetBorderMode(0);
    pad2.SetTopMargin(0); pad2.SetGridy()
    pad2.SetBottomMargin(0.35);
        
    datamc = GetRatio(tf_data.histo,tf_proc.histo)
    # rymin = 0.65; rymax = 1.35
    rymin = 0.5; rymax = 1.5
    RatioStyle(datamc,rymin,rymax,color=38,xname=num_sample.name)
    datamc.Draw("pe0")
    line = getRatioLine(datamc.GetXaxis().GetXmin(),datamc.GetXaxis().GetXmax())
    line.Draw("same");
    c.Update()

    if len(varname.split("_")) == 2:
        variable = varname.split("_")[0]
        binning = '_'+varname.split("_")[1]
    else:
        variable = varname
        binning = ''

    tfproc = "%s%s%s%s" % (num_sample.num_boson,num_info['text'],den_sample.den_boson,den_info['text'])
    outname = "%s_%s" % (tfproc,num_sample.variable.base)
    SaveAs(c,outname,year=year,sub="TransferFactors/%s"%num_sample.variable.base)
def plotTransfer(variable,samplemap):
    global varmap
    if variable in rangemap: varmap = rangemap[variable]
    for region in samplemap:
        print region
        samplemap[region].initiate(variable)
        samplemap[region].fullUnc(['Stat'])

    for region in samplemap:
        if 'SignalRegion' in region: continue
        if 'Single' in region:
            samplemap[region].num_boson = 'W'
            samplemap[region].den_boson = 'W'
        if 'Double' in region:
            samplemap[region].num_boson = 'Z'
            samplemap[region].den_boson = 'Z'
        if 'Gamma' in region:
            samplemap[region].num_boson = 'G'
            samplemap[region].den_boson = 'G'
        

    print "Z/W Linking"
    samplemap["SignalRegion"].num_boson = "Z"
    samplemap["SignalRegion"].den_boson = "W"
    plotTF(samplemap["SignalRegion"],samplemap["SignalRegion"])

    print "Z/G Linking"
    plotTF(samplemap["SignalRegion"],samplemap["GammaCR"])
    
    print "DoubleEleCR Transfer"
    samplemap["SignalRegion"].num_boson = "Z"
    plotTF(samplemap["SignalRegion"],samplemap["DoubleEleCR"])
    print "DoubleMuCR Transfer"
    samplemap["SignalRegion"].num_boson = "Z"
    plotTF(samplemap["SignalRegion"],samplemap["DoubleMuCR"])
    
    print "SingleEleCR Transfer"
    samplemap["SignalRegion"].num_boson = "W"
    plotTF(samplemap["SignalRegion"],samplemap["SingleEleCR"])
    print "SingleMuCR Transfer"
    samplemap["SignalRegion"].num_boson = "W"
    plotTF(samplemap["SignalRegion"],samplemap["SingleMuCR"])

    print "Electron CR W/G Linking"
    plotTF_datamc(samplemap['SingleEleCR'],samplemap['GammaCR'])
    print "Electron CR Z/G Linking"
    plotTF_datamc(samplemap['DoubleEleCR'],samplemap['GammaCR'])
    print "Electron CR Z/W Linking"
    plotTF_datamc(samplemap["DoubleEleCR"],samplemap["SingleEleCR"])
    
    print "Muon CR W/G Linking"
    plotTF_datamc(samplemap['SingleMuCR'],samplemap['GammaCR'])
    print "Muon CR Z/G Linking"
    plotTF_datamc(samplemap['DoubleMuCR'],samplemap['GammaCR'])
    print "Muon CR Z/W Linking"
    plotTF_datamc(samplemap["DoubleMuCR"],samplemap["SingleMuCR"])

    for region in samplemap:
        if 'CR' not in region: continue
        samplemap[region].setSumOfBkg()
    doublelep = samplemap["DoubleEleCR"]; doublelep.add(samplemap["DoubleMuCR"])
    doublelep.region = "DoubleLepCR"; doublelep.num_boson = "Z"
    singlelep = samplemap["SingleEleCR"]; singlelep.add(samplemap["SingleMuCR"])
    singlelep.region = "SingleLepCR"; singlelep.num_boson = "W"; singlelep.den_boson = "W"
    print "CR Z/G Linking"
    plotTF_datamc(doublelep,samplemap["GammaCR"])
    print "CR W/G Linking"
    plotTF_datamc(singlelep,samplemap["GammaCR"])
    print "CR Z/W Linking"
    plotTF_datamc(doublelep,singlelep)

def runAll(args):
    scale_lumi = max(config.lumi.values())
    samplemap = { region:Region(path=region,show=False,lumi=scale_lumi,autovar=True) for region in config.regions }
    for variable in args.argv: plotTransfer(variable,samplemap)

if __name__ == "__main__":
    from PlotTool import parser
    parser.parse_args()
    if not any(parser.args.argv): parser.args.argv.append('recoil')
    runAll(parser.args)

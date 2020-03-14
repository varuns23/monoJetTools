from ROOT import *
import os
from sys import argv
from PlotTool import *
import config
import re

gROOT.SetBatch(1)
# gROOT.SetBatch(0)
gStyle.SetOptStat(0)
gStyle.SetLegendBorderSize(0)

def unc_style(up,dn,color):
    for hs in (up,dn):
        hs.SetTitle("")
        hs.SetLineColor(color)
        hs.SetLineWidth(2)
def set_bounds(up,dn,ymin,ymax):
    for hs in (up,dn):
        hs.SetMaximum(ymax*1.2)
        hs.SetMinimum(ymin*1.2)

nuisances = {}
store = []
def plotUnc(tf,sample):
    print "Plotting %s " % tf.name

    tfmap = {
        "ZnnWln":"Z(#nu#nu)+jets / W(l#nu)+jets",
        "ZnnG":"Z(#nu#nu)+jets / #gamma+jets"
    }
    def plotTFUnc(tf,sample,subset):
        print "Plotting %s Variations" % subset
        colorlist = [1,2,4,8]
        coliter = iter(colorlist)
        tf.addUnc(subset)
        variations = {
            tf.numname:next( nuisance for nuisance in tf.nuisances if subset in nuisance and tf.numname in nuisance ),
            tf.denname:next( nuisance for nuisance in tf.nuisances if subset in nuisance and tf.denname in nuisance ),
            # 'combined':nuisance+'_Comb'
        }
        varlist = (tf.numname,tf.denname)
        
        c = TCanvas("%s_%s" % (tf.name,subset),"",800,800)
        
        pad1 = TPad("pad1","pad1",0.,0.25,1.0,1.0);
        
        pad1.Draw(); pad1.cd();
        pad1.SetFillColor(0);
        pad1.SetFrameBorderMode(0);
        pad1.SetBorderMode(0);
        pad1.SetBottomMargin(0.);
        pad1.SetGridy()
        pad1.SetLeftMargin(0.15)

        hsmap = {}
        for name in varlist:
            variation = variations[name]
            up,dn = tf.nuisances[variation].GetDiff()
            unc_style(up,dn,next(coliter))

            up.GetYaxis().SetTitle("%s %s Uncertainty" % (tfmap[tf.name],nuisance))
            
            up.Draw("hist same")
            dn.Draw("hist same")
            hsmap[name] = (up,dn)

        binlist = []
        for hslist in hsmap.values():
            for hs in hslist: binlist += list(hs)[1:-1]

        ymax = max(binlist)
        ymin = min(binlist)
        print ymin,'-',ymax
        for up,dn in hsmap.values(): set_bounds(up,dn,ymin,ymax)

        ratio_up = GetRatio(hsmap[tf.numname][0],hsmap[tf.denname][0])
        ratio_dn = GetRatio(hsmap[tf.numname][1],hsmap[tf.denname][1])

        RatioStyle(ratio_up,color=kRed,xname=sample.name,yname='%s/%s'%(tf.numname,tf.denname))
        RatioStyle(ratio_dn,color=kBlue,xname=sample.name,yname='%s/%s'%(tf.numname,tf.denname))
        ratio_up.GetYaxis().SetRangeUser(0.5,1.5)

        leg = getLegend(xmin=0.5,xmax=0.7,ymin=0.7,ymax=0.9)
        for name in varlist: leg.AddEntry(hsmap[name][0],variations[name],"l")
        leg.Draw()

        #######################################
        c.cd()
        pad2 = TPad("pad2","pad2",0.,0.,1.0,0.25);
        pad2.Draw(); pad2.cd();
        pad2.SetFillColor(0); pad2.SetFrameBorderMode(0); pad2.SetBorderMode(0);
        pad2.SetTopMargin(0); pad2.SetGridy()
        pad2.SetBottomMargin(0.35);
        pad2.SetLeftMargin(0.15)

        ratio_up.Draw("hist")
        ratio_dn.Draw("hist same")

        store.append(vars())
        outname = "%s_%s" % (tf.name,subset)
        SaveAs(c,outname,year=sample.year,sub="TransferFactors/%s/Uncertainty/"%sample.variable.base)
    for nuisance in nuisances: plotTFUnc(tf,sample,nuisance)
if __name__ == "__main__":
    from PlotTool import parser
    sr = Region(path="SignalRegion",autovar=True,show=False)
    ga = Region(path="GammaCR",autovar=True,show=False)
    parser.parse_args()
    for variable in parser.args.argv:
        print "Fetching",variable
        sr.initiate(variable)
        ga.initiate(variable)
        zw_tf = Transfer("ZnnWln",sr["ZJets"],sr["WJets"])
        ga_tf = Transfer("ZnnG",sr["ZJets"],ga["GJets"])
        nuisances.update(sr.variable.nuisances)
        plotUnc(zw_tf,sr)
        plotUnc(ga_tf,sr)
    

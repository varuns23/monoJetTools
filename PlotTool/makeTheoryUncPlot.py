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
def plotUnc(tf,sample):
    print "Plotting %s " % tf.name
    def plotTFUnc(tf,sample,subset):
        print "Plotting %s Variations" % subset
        colorlist = [1,2,4,8]
        coliter = iter(colorlist)
        variations = [ unc for unc in Transfer.zwunc if subset in unc ]
        c = TCanvas("%s_%s" % (tf.name,subset),"",800,800)

        hsmap = {}
        for var in variations:
            print tf.nuisances[var]
            up,dn = tf.nuisances[var].GetDiff()
            unc_style(up,dn,next(coliter))
            up.Draw("hist same")
            dn.Draw("hist same")
            hsmap[var] = (up,dn)

        ymax = max( max( hs.GetMaximum() for hs in updn ) for updn in hsmap.values() )
        ymin = max( max( hs.GetMinimum() for hs in updn ) for updn in hsmap.values() )

        for up,dn in hsmap.values():
            set_bounds(up,dn,ymax,ymin)

        leg = getLegend(xmin=0.5,xmax=0.7)
        for var in variations: leg.AddEntry(hsmap[var][0],var,"l")
        leg.Draw()

        outname = "%s_%s" % (tf.name,subset)
        SaveAs(c,outname,year=sample.year,sub="TransferFactors/%s/Uncertainty/"%sample.variable.base)
    plotTFUnc(tf,sample,'QCD')
    plotTFUnc(tf,sample,'NNLO')
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
        plotUnc(zw_tf,sr)
        ga_tf = Transfer("ZnnG",sr["ZJets"],ga["GJets"])
        plotUnc(ga_tf,sr)
    

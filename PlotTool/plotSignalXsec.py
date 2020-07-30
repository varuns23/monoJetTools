"""
Plot Signal Xsecs
Usage: python PlotTool/plotSignalXsec.py
"""

from PlotTool import monoJet_XS as sx
from PlotTool import *
from ROOT import *

from array import array
import re

group = parser.add_group(__file__,__doc__,"Script")

gROOT.SetBatch(1)

colormap = {
    '1':kRed,
    '10':kAzure+10,
    '50':kViolet,
    '100':kBlack,
    '150':kOrange-2,
    '500':kGreen,
    '1000':kBlue
}

mx_pat = re.compile('Mx\d+'); mv_pat = re.compile('Mv\d+')

def plotSignalXsec(signalxsec,label):
    dataset = {}
    for signal,xsec in signalxsec.iteritems():
        mx = mx_pat.findall(signal)[0].replace('Mx','')
        mv = mv_pat.findall(signal)[0].replace('Mv','')
        
        if mx not in dataset: dataset[mx] = {}
        dataset[mx][mv] = xsec

    c = TCanvas("c","c",800,800)
    gStyle.SetOptStat(0);
    gStyle.SetLegendBorderSize(0);
    c.SetLeftMargin(0.15);
    c.SetLogy();
    c.cd();
    leg = getLegend(xmin=0.6,xmax=0.8)
    multigraph = TMultiGraph()
    graphmap = {}
    
    mxlist = sorted(dataset.keys(),key=float)
    for mx in mxlist:
        mvmap = dataset[mx]
        mvlist = sorted(mvmap.keys(),key=float)
        xlist = [float(mv) for mv in mvlist]
        ylist = [mvmap[mv] for mv in mvlist]
        
        graph = TGraph(len(xlist),array('d',xlist),array('d',ylist))
        graph.SetLineColor(colormap[mx])
        graph.SetLineWidth(3)
        leg.AddEntry(graph,'m_{#chi} = %s' % mx,'l')
        multigraph.Add(graph)

    multigraph.Draw('Apl')
    multigraph.GetXaxis().SetTitle("m_{med} (GeV)")
    multigraph.GetYaxis().SetTitle("cross-section ( pb^{-1} )")
    leg.Draw('same')
    
    SaveAs(c,"%s_signal_xsec"%label,sub="SignalPlots")
#######################################################
if __name__ == "__main__":
    plotSignalXsec(sx.centralxsec,'central')
    plotSignalXsec(sx.privatexsec,'private')
    plotSignalXsec(sx.signalxsec,'combined')

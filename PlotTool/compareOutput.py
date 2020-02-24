import sys
sys.path.append("PlotTool")
from ROOT import *
from PlotTool import *
import config

parser.add_argument("--sub",type=str,default=None)
args = parser.parse_args()

gROOT.SetBatch(1)

def compareOutput(fname1,fname2):
    tfile1 = TFile.Open(fname1)
    tfile2 = TFile.Open(fname2)

    def compare(hsname):
        hs1 = tfile1.Get(hsname)
        hs2 = tfile2.Get(hsname)

        DataStyle(hs1)
        hs2.SetLineColor(kRed)
        hs2.SetLineWidth(2)

        c = TCanvas(hsname,hsname,800,800)
        c.SetLogy()
        gStyle.SetOptStat(0)
        gStyle.SetLegendBorderSize(0)
        c.SetTicks(0,1)

        
        ratio = TRatioPlot(hs1,hs2)
        ratio.SetH1DrawOpt("pex0")
        ratio.SetH2DrawOpt("histsame")
        ratio.SetGraphDrawOpt("p")
        ratio.Draw()
        hi = ratio.GetUpperRefObject()
        hi.GetYaxis().SetTitle("Events")

        upad = ratio.GetUpperPad()
        upad.cd()
        leg = getLegend(xmin=0.5,xmax=0.7)
        leg.AddEntry(hs1,"%s BU" % hsname,"pl")
        leg.AddEntry(hs2,"%s UW" % hsname,"fl")
        leg.Draw()

        lo = ratio.GetLowerRefGraph()
        lo.GetYaxis().SetTitle("BU/UW")
        lo.GetYaxis().SetRangeUser(0.75,1.35)

        lo.SetMarkerStyle(20)
        lo.SetMarkerSize(1)
        
        SaveAs(c,hsname,year=config.version,sub="BU_Comparison",exts=".png")
        return 
    keylist1 =[ key.GetName() for key in  tfile1.GetListOfKeys()]
    keylist2 =[ key.GetName() for key in  tfile2.GetListOfKeys()]
    for key in keylist1:
        if key in keylist2:
            compare(key)


compareOutput(args.argv[0],args.argv[1])
    

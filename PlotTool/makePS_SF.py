import os
from ROOT import *
import sys
sys.path.append('PlotTool')
from PlotTool import *
import config

gROOT.SetBatch(1)

config.mclist = ["ZJets","WJets","GJets","DYJets"]

output = TFile("PSW_SF.root","RECREATE")
pswlist = [ "isrRed","fsrRed","isrDef","fsrDef","isrCon","fsrCon",      
	    "fsr_G2GG_muR","fsr_G2QQ_muR","fsr_Q2QG_muR","fsr_X2XG_muR",
	    "fsr_G2GG_cNS","fsr_G2QQ_cNS","fsr_Q2QG_cNS","fsr_X2XG_cNS",
	    "isr_G2GG_muR","isr_G2QQ_muR","isr_Q2QG_muR","isr_X2XG_muR",
	    "isr_G2GG_cNS","isr_G2QQ_cNS","isr_Q2QG_cNS","isr_X2XG_cNS"]
# pswlist = [ "fsr%s" % psw for psw in ("Red","Def","Con") ]

template = TH1F("template","template",50,0,1.05)
line = TLine(0,1,1.05,1)
def DebugDraw(process):
    process.up.SetMarkerStyle(22); process.up.SetLineStyle(1)
    process.dn.SetMarkerStyle(23); process.dn.SetLineStyle(1)
    process.up.Draw("p hist"); process.dn.Draw("p hist same"); line.Draw("same")
    process.up.GetYaxis().SetRangeUser(0.5,1.5)
    raw_input()
def MakeSF(process,psw):

    process.up.Divide(process.nominal); process.dn.Divide(process.nominal)
    
    process.outputdir.cd()
    process.up.SetName("PSW_%sUp" % psw)
    process.dn.SetName("PSW_%sDown" % psw)
    process.up.Write()
    process.dn.Write()
def GenerateSF():
    sample = Region(autovar=True)
    sample.SampleList = list(config.mclist)

    sample.initiate("ChNemPtFrac",weight="1")
    for process in sample:
        process.nominal = process.histo.Clone()
        process.outputdir = output.mkdir(process.process)
    for psw in pswlist:
        sample.initiate("ChNemPtFrac",weight="PSW_%sUp"%psw)
        for process in sample: process.up = process.histo.Clone()
        sample.initiate("ChNemPtFrac",weight="PSW_%sDown"%psw)
        for process in sample: process.dn = process.histo.Clone()
        for process in sample: MakeSF(process,psw)
 
if __name__ == "__main__": GenerateSF()

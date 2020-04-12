from ROOT import *
import os

regions = ["SignalRegion","SingleEleCR","SingleMuCR","DoubleEleCR","DoubleMuCR","GammaCR"]
procs = {
    "ZJets":("SignalRegion",),
    "WJets":("SignalRegion","SingleEleCR","SingleMuCR"),
    "GJets":("GammaCR",),
    "DYJets":("DoubleEleCR","DoubleMuCR")
}

pswlist = ["PSW_isrCon","PSW_fsrCon"]

def ProducePSW(process,tfiles,psw):
    print process,psw
    tdirs = [ tfile.GetDirectory(process) for tfile in tfiles ]
    def ProduceVariation(tdirs,psw):
        hslist = [ tdir.Get(psw) for tdir in tdirs ]
        for hs in hslist: hs.Smooth(1,"r")
        histo = hslist[0].Clone()
        for ibin in range(1,histo.GetNbinsX()+1):
            bin = 0
            for hs in hslist: bin += hs[ibin]
            bin /= len(hslist)
            histo.SetBinContent(ibin,bin)
        return histo
    up = ProduceVariation(tdirs,psw+'Up')
    dn = ProduceVariation(tdirs,psw+'Down')
    return up,dn

filemap = { region:TFile("%s/PSW_SF.root"%region) for region in regions }
output = TFile("PSW_SF.root","RECREATE")
for proc,filelist in procs.items():
    tfiles = [ filemap[region] for region in filelist ]
    output.cd()
    output.mkdir(proc).cd()
    for psw in pswlist:
        up,dn = ProducePSW(proc,tfiles,psw)
        up.Write()
        dn.Write()

from ROOT import *
import os
from sys import argv
from PlotTool import *
import config
import re

# gROOT.SetBatch(1)
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

tfmap = {
        "zwinfo":"Z(#nu#nu)/W(l#nu)",
        "gzinfo":"#gamma +jets/Z(#nu#nu)",
        "zminfo":"Z(#nu#nu)/Z(#mu#mu)",
        "zeinfo":"Z(#nu#nu)/Z(ee)",
        "wginfo":"W(l#nu)/#gamma +jets",
        "wminfo":"W(l#nu)/W(l#mu)",
        "weinfo":"W(l#nu)/W(le)",
}

def GetHistoRatio(num,den):
    n = num.Integral()
    d = den.Integral()
    r = n/d
    # print "%s: %f %s: %f r: %f"%(num.GetName(),n,den.GetName(),d,r)
    return n/d

def GetProcessRatio(num,den):
    class data: pass
    data.nominal = GetHistoRatio(num.histo,den.histo)
    unc = "JER"
    num.addUnc(unc)
    numup,numdn = num.nuisances[unc].GetHistos()
    den.addUnc(unc)
    denup,dendn = den.nuisances[unc].GetHistos()
    data.jerup = GetHistoRatio(numup,denup)
    data.jerdn = GetHistoRatio(numdn,dendn)
    data.jerperup = (data.jerup - data.nominal)/data.nominal
    data.jerperdn = (data.jerdn - data.nominal)/data.nominal

    unc = "JES"
    num.addUnc(unc)
    numup,numdn = num.nuisances[unc].GetHistos()
    den.addUnc(unc)
    denup,dendn = den.nuisances[unc].GetHistos()
    data.jesup = GetHistoRatio(numup,denup)
    data.jesdn = GetHistoRatio(numdn,dendn)
    data.jesperup = (data.jesup - data.nominal)/data.nominal
    data.jesperdn = (data.jesdn - data.nominal)/data.nominal
    return data
varmap = {}
def plotTransfer(variable,samplemap):
    for region in samplemap:
        proclist = [ proc['proc'] for proc in processMap[region].values() ]
        samplemap[region].SampleList = proclist
        samplemap[region].initiate(variable)

    tfinfo = {
        "zwinfo":GetProcessRatio(samplemap["SignalRegion"]["ZJets"],samplemap["SignalRegion"]["WJets"]),
        "gzinfo":GetProcessRatio(samplemap["GammaCR"]["GJets"],samplemap["SignalRegion"]["ZJets"]),
        "zminfo":GetProcessRatio(samplemap["SignalRegion"]["ZJets"],samplemap["DoubleMuCR"]["DYJets"]),
        "zeinfo":GetProcessRatio(samplemap["SignalRegion"]["ZJets"],samplemap["DoubleEleCR"]["DYJets"]),
        "wginfo":GetProcessRatio(samplemap["SignalRegion"]["WJets"],samplemap["GammaCR"]["GJets"]),
        "wminfo":GetProcessRatio(samplemap["SignalRegion"]["WJets"],samplemap["SingleMuCR"]["WJets"]),
        "weinfo":GetProcessRatio(samplemap["SignalRegion"]["WJets"],samplemap["SingleEleCR"]["WJets"]),
    }

    col = ['nominal','jerup','jerdn','jesup','jesdn','jerperup','jerperdn','jesperup','jesperdn']
    print '|'.join(['tf']+col)
    for tf in ("zwinfo","zminfo","zeinfo","gzinfo","wginfo","wminfo","weinfo"):
        data = tfinfo[tf]
        row = [ "%s %s"%(tfmap[tf],samplemap["SignalRegion"].year) ]
        row += [ str(getattr(data,v)) for v in col ]
        print '|'.join(row)

    
def runAll(args):
    scale_lumi = max(config.lumi.values())
    samplemap = { region:Region(path=region,show=False,lumi=scale_lumi,autovar=True) for region in config.regions }
    for variable in args.argv: plotTransfer(variable,samplemap)

if __name__ == "__main__":
    from PlotTool import parser
    parser.parse_args()
    if not any(parser.args.argv): parser.args.argv.append('recoil')
    runAll(parser.args)

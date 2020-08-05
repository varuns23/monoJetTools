"""
Build transfer systematic files for use in limit code
Usage: python PlotTool/makeTransferSys.root variable
"""

gROOT.SetBatch(1)

import os
import sys
from PlotTool import *
from ROOT import *

group = parser.add_group(__file__,__doc__,"Script")

nuisancemap = {
    "QCD_Scale"  :"QCDScale",
    "QCD_Proc"   :"QCDProcess",
    "QCD_Shape"  :"QCDShape",
    "NNLO_EWK"   :"NNLOEWK",
    "NNLO_Sud"   :"Sudakov",
    "NNLO_Miss"  :"NNLOMiss",
    "QCD_EWK_Mix":"MIX",
    # "PDF"      :"PDF"
}

theorylist = [
    "QCD_Scale",
    "QCD_Proc",     
    "QCD_Shape",
    "NNLO_EWK",
    "NNLO_Sud",
    "NNLO_Miss",
    "QCD_EWK_Mix",
    "PSW_isrCon",
    "PSW_fsrCon"
]

explist = [
    "JES",
    "JER"
]

correlation = {
    "QCD_Scale":True,
    "QCD_Shape":True,
    "QCD_Proc":True,
    "NNLO_Sud":False,
    "NNLO_Miss":False,
    "NNLO_EWK":True,
    "QCD_EWK_Mix":True,
    "PDF":True,
    "JES":True,
    "JER":True,
    "PSW_isrCon":True,
    "PSW_fsrCon":True
}

def AddNuisance(tf,nuisance,output):
    print "Getting",nuisance
    syst = tf.addUnc(nuisance,correlation[nuisance])
    name = nuisancemap[nuisance] if nuisance in nuisancemap else nuisance
    output.cd()
    if not correlation[nuisance]:
        num,den = syst
        numup,numdn = num.GetScale()
        numup.Write("%s_%s1_met"%(tf.name,name))
        numdn.Write("%s_%s1_met_Down"%(tf.name,name))

        denup,dendn = den.GetScale()
        denup.Write("%s_%s2_met"%(tf.name,name))
        dendn.Write("%s_%s2_met_Down"%(tf.name,name))
    else:
        up,dn = syst.GetScale()
        up.Write("%s_%s_met"%(tf.name,name))
        dn.Write("%s_%s_met_Down"%(tf.name,name))
    # exit()
def MakeSys(num,den,tfname):
    tf = Transfer(tfname,num,den)

    if tfname == "ZW": output = TFile("theory_sys/wz_unc.root","recreate")
    if tfname == "ZG": output = TFile("theory_sys/gz_unc.root","recreate")
    for nuisance in theorylist: AddNuisance(tf,nuisance,output)
    
    # if tfname == "ZW": output = TFile("theory_sys/wz_exp_unc.root","recreate")
    # if tfname == "ZG": output = TFile("theory_sys/gz_exp_unc.root","recreate")
    # for nuisance in explist: AddNuisance(tf,nuisance,output)

if __name__ == "__main__":
    
    sr = Region(path="SignalRegion",autovar=True,show=False)
    sr.SampleList = ["ZJets","WJets"]
    sr.initiate(parser.args.argv[0])
    MakeSys(sr["ZJets"],sr["WJets"],"ZW")
    
    ga = Region(path="GammaCR",autovar=True,show=False)
    ga.SampleList = ["GJets"]
    ga.initiate(parser.args.argv[0])
    MakeSys(sr["ZJets"],ga["GJets"],"ZG")

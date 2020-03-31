from ROOT import *
gROOT.SetBatch(1)

import os
import sys
from PlotTool import *

nuisancemap = {
    "QCD_Scale":"QCDScale",
    "QCD_Proc":"QCDProcess",
    "QCD_Shape":"QCDShape",
    "NNLO_EWK":"NNLOEWK",
    "NNLO_Sud":"Sudakov",
    "NNLO_Miss":"NNLOMiss",
    "QCD_EWK_Mix":"MIX",
    # "PDF":"PDF"
}
correlation = {
    "QCD_Scale":True,
    "QCD_Shape":True,
    "QCD_Proc":True,
    "NNLO_Sud":False,
    "NNLO_Miss":False,
    "NNLO_EWK":True,
    "QCD_EWK_Mix":True,
    "PDF":True
}

def AddNuisance(tf,nuisance,output):
    print "Getting",nuisance
    syst = tf.addUnc(nuisance,correlation[nuisance])
    output.cd()
    if not correlation[nuisance]:
        num,den = syst
        numup,numdn = num.GetScaleDiff()
        numup.Write("%s_%s1_met"%(tf.name,nuisancemap[nuisance]))
        numdn.Write("%s_%s1_met_Down"%(tf.name,nuisancemap[nuisance]))

        denup,dendn = den.GetScaleDiff()
        denup.Write("%s_%s2_met"%(tf.name,nuisancemap[nuisance]))
        dendn.Write("%s_%s2_met_Down"%(tf.name,nuisancemap[nuisance]))
    else:
        up,dn = syst.GetScaleDiff()

        # up.Draw("hist")
        # dn.Draw("histsame")
        
        up.Write("%s_%s_met"%(tf.name,nuisancemap[nuisance]))
        dn.Write("%s_%s_met_Down"%(tf.name,nuisancemap[nuisance]))
    # exit()
def MakeSys(num,den,tfname):
    tf = Transfer(tfname,num,den)

    if tfname == "ZW": output = TFile("wz_unc.root","recreate")
    if tfname == "ZG": output = TFile("gz_unc.root","recreate")
    for nuisance in nuisancemap: AddNuisance(tf,nuisance,output)

if __name__ == "__main__":
    sr = Region(path="SignalRegion",autovar=True,show=False)
    sr.SampleList = ["ZJets","WJets"]
    sr.initiate("recoil")
    # MakeSys(sr["ZJets"],sr["WJets"],"ZW")
    
    ga = Region(path="GammaCR",autovar=True,show=False)
    ga.SampleList = ["GJets"]
    ga.initiate("recoil")
    MakeSys(sr["ZJets"],ga["GJets"],"ZG")

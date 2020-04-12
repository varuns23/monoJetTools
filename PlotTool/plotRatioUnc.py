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
    "Stat":False,
    "PSW_isrCon":True,
    "PSW_fsrCon":True
}

showerror = ["JES","JER"]

def unc_style(up,dn,color):
    for hs in (up,dn):
        hs.SetTitle("")
        hs.SetLineColor(color)
        hs.SetLineWidth(2)
def set_bounds(up,dn,ymin,ymax):
    for hs in (up,dn):
        hs.SetMaximum(ymax*1.5)
        hs.SetMinimum(ymin*1.5)
def GetStat(nuisance,stat):
    from PlotTool import AddDiffNuisances
    up = stat.norm.Clone()
    dn = stat.norm.Clone()
    AddDiffNuisances([nuisance,stat],up,dn,stat.norm)
    return Nuisance(stat.process,nuisance.name+'_stat',up,dn,stat.norm)
def SetStat(var,stat):
    var.SetFillColor(33)
    for ibin in range(1,var.GetNbinsX()+1): var.SetBinError(ibin,abs(var[ibin]-stat[ibin]))
nuisances = {"PSW_fsrCon":True,"PSW_isrCon":True}
store = []
def plotUnc(name,num,den,sample):
    tf = Transfer(name,num,den)
    print "Plotting %s " % tf.name
    tf.addUnc("Stat",False)
    tfmap = {
        "ZnnWln":"Z(#nu#nu)+jets / W(l#nu)+jets",
        "ZnnG":"Z(#nu#nu)+jets / #gamma+jets",
        "WlnWmn":"W(l#nu)+jets / W(#mu#nu)+jets",
        "WlnWen":"W(l#nu)+jets / W(e#nu)+jets",
        "ZnnZee":"Z(#nu#nu)+jets / Z(ee)+jets",
        "ZnnZmm":"Z(#nu#nu)+jets / Z(#mu#mu)+jets"
    }
    def plotTFUnc(tf,sample,subset):
        print "Plotting %s Variations" % subset
        colorlist = [1,2,4,8]
        coliter = iter(colorlist)
        correlated = correlation[subset] if subset in correlation else False
        tf.addUnc(subset,correlated)
        variations = {'combined':subset}
        varlist = variations.keys()
        # if correlated:
        #     variations = {
        #         'combined':subset
        #     }
        #     varlist = ('combined',)
        # else:
        #     variations = {
        #         tf.numname:next( nuisance for nuisance in tf.nuisances if subset in nuisance and tf.numname in nuisance ),
        #         tf.denname:next( nuisance for nuisance in tf.nuisances if subset in nuisance and tf.denname in nuisance )
        #     }
        #     varlist = (tf.numname,tf.denname)
        
        c = TCanvas("%s_%s" % (tf.name,subset),"",800,800)
        
        pad1 = TPad("pad1","pad1",0.,0.,1.0,1.0);
        
        pad1.Draw(); pad1.cd();
        pad1.SetFillColor(0);
        pad1.SetFrameBorderMode(0);
        pad1.SetBorderMode(0);
        # pad1.SetBottomMargin(0.);
        pad1.SetGrid()
        pad1.SetLeftMargin(0.15)

        hsmap = {}
        for name in varlist:
            variation = variations[name]
            nuis = tf.nuisances[variation]
            print nuis
            up,dn = tf.nuisances[variation].GetDiff()
            unc_style(up,dn,next(coliter))



            up.GetYaxis().SetTitle("%s %s Uncertainty" % (tfmap[tf.name],nuisance))

            up.Draw("axis same")

            if subset in showerror:
                stat = GetStat(nuis,tf.nuisances['Stat'])
                stup,stdn = stat.GetDiff()
                erup = up.Clone()
                erdn = dn.Clone()
                SetStat(erup,stup)
                SetStat(erdn,stdn)
                erup.Draw("E2 same")
                erdn.Draw("E2 same")
                hsmap[name+'_stat'] = (erup,erdn)
            
            up.Draw("hist same")
            dn.Draw("hist same")
            hsmap[name] = (up,dn)
        pad1.RedrawAxis()
        binlist = []
        for hslist in hsmap.values():
            for hs in hslist: binlist += list(hs)[1:-1]

        ymax = max(binlist)
        ymin = min(binlist)
        for up,dn in hsmap.values(): set_bounds(up,dn,ymin,ymax)

        leg = getLegend(xmin=0.2,xmax=0.4,ymin=0.7,ymax=0.9)
        corr_label = " Correlated" if correlated else " Uncorrelated"
        leg.AddEntry(hsmap['combined'][0],variations['combined']+corr_label,"l")
        if subset in showerror: leg.AddEntry(hsmap['combined_stat'][0],"Stat","f")
        leg.Draw()

        store.append(vars())

        varname = sample.varname
        if len(varname.split("_")) == 2:
            variable = varname.split("_")[0]
            binning = '_'+varname.split("_")[1]
        else:
            variable = varname
            binning = ''
        outname = "%s_%s" % (tf.name,subset)
        SaveAs(c,outname,year=sample.year,sub="TransferFactors/%s/Uncertainty/%s"%(sample.variable.base,tf.name))
    for nuisance in nuisances: plotTFUnc(tf,sample,nuisance)
if __name__ == "__main__":
    from PlotTool import parser
    sr = Region(path="SignalRegion",autovar=True,show=False)
    sr.SampleList = ["ZJets","WJets"]
    ga = Region(path="GammaCR",autovar=True,show=False)
    ga.SampleList = ["GJets"]
    we = Region(path="SingleEleCR",autovar=True,show=False)
    we.SampleList = ["WJets"]
    wm = Region(path="SingleMuCR",autovar=True,show=False)
    wm.SampleList = ["WJets"]
    ze = Region(path="DoubleEleCR",autovar=True,show=False)
    ze.SampleList = ["DYJets"]
    zm = Region(path="DoubleMuCR",autovar=True,show=False)
    zm.SampleList = ["DYJets"]
    parser.parse_args()
    for variable in parser.args.argv:
        print "Fetching",variable
        for r in (sr,ga,we,wm,ze,zm): r.initiate(variable)
        nuisances.update(sr.variable.nuisances)
        plotUnc("ZnnWln",sr["ZJets"],sr["WJets"],sr)
        plotUnc("ZnnG",sr["ZJets"],ga["GJets"],sr)
        plotUnc("WlnWen",sr["WJets"],we["WJets"],sr)
        plotUnc("WlnWmn",sr["WJets"],wm["WJets"],sr)
        plotUnc("ZnnZee",sr["ZJets"],ze["DYJets"],sr)
        plotUnc("ZnnZmm",sr["ZJets"],zm["DYJets"],sr)
    

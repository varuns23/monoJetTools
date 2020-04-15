from ROOT import *
from utilities import *
from Nuisance import *
import copy

class Transfer:
    zwunc = [
            "QCD_Scale",
            "QCD_Shape",
            "QCD_Proc",
            "NNLO_EWK",
            "NNLO_Miss",
            "NNLO_Sud",
            "QCD_EWK_Mix"]
    def __init__(self,name,num,den,namelist=None):
        self.name = name; self.num = num; self.den = den;
        self.histo = GetRatio(num.histo,den.histo)
        self.nuisances = {}
        if namelist is None:
            self.numname = self.num.process
            self.denname = self.den.process
        else:
            self.numname = namelist[0]
            self.denname = namelist[1]
    def addUnc(self,nuisance,correlated=False):
        self.num.addUnc(nuisance)
        self.den.addUnc(nuisance)

        numnuis = self.num.nuisances[nuisance]
        dennuis = self.den.nuisances[nuisance]

        numup,numdn = numnuis.GetHistos()
        denup,dendn = dennuis.GetHistos()
        if not correlated:
            tfnumup = GetRatio(numup,self.den.histo)
            tfnumdn = GetRatio(numdn,self.den.histo)
            numunc = "%s_%s" % (nuisance,self.numname)
            self.nuisances[numunc] = Nuisance(self.name,numunc,tfnumup,tfnumdn,self.histo,type='abs')
            
            tfdenup = GetRatio(self.num.histo,denup)
            tfdendn = GetRatio(self.num.histo,dendn)
            denunc = "%s_%s" % (nuisance,self.denname)
            self.nuisances[denunc] = Nuisance(self.name,denunc,tfdenup,tfdendn,self.histo,type='abs')

            tfup = self.histo.Clone(); tfup.Reset()
            tfdn = self.histo.Clone(); tfdn.Reset()
            AddDiffNuisances([self.nuisances[numunc],self.nuisances[denunc]],tfup,tfdn,self.histo)
            self.nuisances[nuisance] = Nuisance(self.name,nuisance,tfup,tfdn,self.histo)
            return self.nuisances[numunc],self.nuisances[denunc]
        else:
            tfup = GetRatio(numup,denup)
            tfdn = GetRatio(numdn,dendn)
            
            self.nuisances[nuisance] = Nuisance(self.name,nuisance,tfup,tfdn,self.histo,type='abs')

            # numerr = self.num.histo.Clone()
            # for ibin in range(1,numerr.GetNbinsX()+1): numerr.SetBinError(ibin,abs(numerr[ibin] - numup[ibin]))
            # denerr = self.den.histo.Clone()
            # for ibin in range(1,denerr.GetNbinsX()+1): denerr.SetBinError(ibin,abs(denerr[ibin] - denup[ibin]))

            # error = GetRatio(numerr,denerr)

            # erup = error.Clone()
            # erdn = error.Clone()
            # for ibin in range(1,error.GetNbinsX()+1):
            #     erup.SetBinContent(ibin,error.GetBinError(ibin))
            #     erdn.SetBinContent(ibin,error.GetBinError(ibin))
            # error = Nuisance(self.name,nuisance,erup,erdn,self.histo)
            # self.nuisances[nuisance] = error
            
            return self.nuisances[nuisance]
    def fullUnc(self,unclist):
        nuislist = []
        for unc in unclist:
            for nuisance in self.nuisances.values():
                if unc in nuisance.name: nuislist.append(nuisance)
        up,dn = self.histo.Clone(),self.histo.Clone()
        AddDiffNuisances(nuislist,up,dn,self.histo)
        self.nuisances['Total'] = Nuisance(self.name,'Total',up,dn,self.histo)
    def getUncBand(self,nuisance='Total'):
        if nuisance not in self.nuisances:
            if nuisance is 'Total': self.fullUnc(self.nuisances.keys())
            else: self.addUnc(nuisance)
        up,dn = self.nuisances[nuisance].GetHistos()
        return GetUncBand(up,dn)
    def printUnc(self):
        def sort_key(name):
            varup,vardn = self.nuisances[name].VarDiff()
            return varup - vardn
        for nuisance in sorted(self.nuisances.keys(),key=sort_key,reverse=True): print self.nuisances[nuisance]
if __name__ == "__main__":
    FindConfig()
    from Region import Region
    from style import UncBandStyle
    
    sample = Region(autovar=True)
    sample.initiate("recoil")

    tf_zw = Transfer("Z/W",sample["ZJets"],sample["WJets"])
    uncband = tf_zw.getUncBand()
    UncBandStyle(uncband)
    c=TCanvas()
    tf_zw.histo.Draw("axis")
    uncband.Draw("2same")
    tf_zw.histo.Draw("hist same")
    raw_input()

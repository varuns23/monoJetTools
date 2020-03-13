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
        # stat_hs = self.histo.Clone()
        # for ibin in range(1,self.histo.GetNbinsX()+1):
        #     stat = self.histo[ibin] * TMath.Sqrt(sum( (proc.histo.GetBinError(ibin)/proc.histo[ibin])**2 for proc in (num,den) if proc.histo[ibin] != 0))
        #     stat_hs[ibin] = stat
        #     self.histo.SetBinError(ibin,stat)
        # self.nuisances['Stat'] = Nuisance(self.name,'Stat',stat_hs,stat_hs,self.histo)
    def addUnc(self,nuisance,correlated=False):
        self.num.addUnc(nuisance)
        self.den.addUnc(nuisance)

        numup,numdn = self.num.nuisances[nuisance].GetHistos()
        denup,dendn = self.den.nuisances[nuisance].GetHistos()
        if not correlated:
            tfnumup = GetRatio(numup,self.den.histo)
            tfnumdn = GetRatio(numdn,self.den.histo)
            numunc = "%s_%s" % (nuisance,self.numname)
            self.nuisances[numunc] = Nuisance(self.name,numunc,tfnumup,tfnumdn,self.histo,type='abs')
            
            tfdenup = GetRatio(self.num.histo,denup)
            tfdendn = GetRatio(self.num.histo,dendn)
            denunc = "%s_%s" % (nuisance,self.denname)
            self.nuisances[denunc] = Nuisance(self.name,denunc,tfdenup,tfdendn,self.histo,type='abs')
        else:
            tfup = GetRatio(numup,denup)
            tfdn = GetRatio(numdn,dendn)
            self.nuisances[nuisance] = Nuisance(self.name,nuisance,tfup,tfdn,self.histo,type='abs')
    def fullUnc(self,unclist):
        nuislist = []
        for unc in unclist:
            for nuisance in self.nuisances.values():
                if unc in nuisance.name: nuislist.append(nuisance)
        up,dn = self.histo.Clone(),self.histo.Clone()
        AddDiffNuisances(nuislist,up,dn,self.histo)
        self.nuisances['Total'] = Nuisance(self.name,'Total',up,dn,self.histo)
    def getUncBand(self,nuisance='Total'):
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

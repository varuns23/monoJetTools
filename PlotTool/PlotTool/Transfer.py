from ROOT import *
from utilities import *
from Nuisance import *

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
        self.name = name; self.num = num; self.den = den
        self.histo = GetRatio(num.histo,den.histo)
        self.nuisances = {}
        stat_hs = self.histo.Clone()
        for ibin in range(1,self.histo.GetNbinsX()+1):
            stat = self.histo[ibin] * TMath.Sqrt(sum( (proc.histo.GetBinError(ibin)/proc.histo[ibin])**2 for proc in (num,den) if proc.histo[ibin] != 0))
            stat_hs[ibin] = stat
            self.histo.SetBinError(ibin,stat)
        self.nuisances['Stat'] = Nuisance(self.name,'Stat',stat_hs,stat_hs,self.histo)

        self.getZWUnc()

        up,dn = self.histo.Clone(),self.histo.Clone()
        for ibin in range(1,self.histo.GetNbinsX()+1):
            up[ibin] = TMath.Sqrt( sum( nuisance.up[ibin]**2 for nuisance in self.nuisances.values() ) )
            dn[ibin] = TMath.Sqrt( sum( nuisance.dn[ibin]**2 for nuisance in self.nuisances.values() ) )
        self.nuisances['Total'] = Nuisance(self.name,'Total',up,dn,self.histo)
        # self.nuisances['Total'].printByBin()
    def getZWUnc(self):
        isZ = (self.num.process == "ZJets" or self.num.process == "DYJets")
        isW = self.den.process == "WJets"
        if not (isZ and isW): return
        for theory in self.zwunc:
            if theory in self.num.variable.nuisances:
                self.num.addUnc(theory)
                self.den.addUnc(theory)
            else:
                self.getTheoryUnc(theory)
    def getTheoryUnc(self,theory): pass
        
    def getUncBand(self,nuisance='Total'):
        up,dn = self.nuisances[nuisance].GetHistos()
        return GetUncBand(up,dn)
    def printUnc(self):
        def sort_key(name):
            varup,vardn = self.nuisances[name].VarDiff()
            return varup - vardn
        for nuisance in sorted(self.nuisances.keys(),key=sort_key,reverse=True): print self.nuisances[nuisance]

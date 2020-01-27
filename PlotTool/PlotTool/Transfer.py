from ROOT import *
from utilities import *
from Nuisance import Nuisance

class Transfer:
    tranunc = [
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
            
        if not any(num.nuisances): return
        if namelist is None:
            numname = num.process
            denname = den.process
        else:
            numname = namelist[0]
            denname = namelist[1]
        for nuisance in Transfer.tranunc:
            #Numerator Variation
            numup,numdn = num.nuisances[nuisance].GetHistos()
            tfnumup = GetRatio(numup,den.histo)
            tfnumdn = GetRatio(numdn,den.histo)
            numunc = '%s_%s' % (nuisance,numname)
            self.nuisances[numunc] = Nuisance(self.name,numunc,tfnumup,tfnumdn,self.histo,type='abs')
            # self.nuisances[numunc].printByBin()

            #Denomenator Variation
            denup,dendn = den.nuisances[nuisance].GetHistos()
            tfdenup = GetRatio(num.histo,denup)
            tfdendn = GetRatio(num.histo,dendn)
            denunc = '%s_%s' % (nuisance,denname)
            self.nuisances[denunc] = Nuisance(self.name,denunc,tfdenup,tfdendn,self.histo,type='abs')
            # self.nuisances[denunc].printByBin()

        up,dn = self.histo.Clone(),self.histo.Clone()
        for ibin in range(1,self.histo.GetNbinsX()+1):
            up[ibin] = TMath.Sqrt( sum( nuisance.up[ibin]**2 for nuisance in self.nuisances.values() ) )
            dn[ibin] = TMath.Sqrt( sum( nuisance.dn[ibin]**2 for nuisance in self.nuisances.values() ) )
        self.nuisances['Total'] = Nuisance(self.name,'Total',up,dn,self.histo)
        # self.nuisances['Total'].printByBin()
    def getUncBand(self,nuisance='Total'):
        up,dn = self.nuisances[nuisance].GetHistos()
        return GetUncBand(up,dn)
    def printUnc(self):
        def sort_key(name):
            varup,vardn = self.nuisances[name].VarDiff()
            return varup - vardn
        for nuisance in sorted(self.nuisances.keys(),key=sort_key,reverse=True): print self.nuisances[nuisance]

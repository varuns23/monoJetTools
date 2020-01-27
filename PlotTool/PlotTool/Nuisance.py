from ROOT import TMath

def MakeDiff(self):
    nbins = self.norm.GetNbinsX()
    for ibin in range(1,nbins+1):
        diffUp = self.norm[ibin] - self.up[ibin]; diffDn = self.norm[ibin] - self.dn[ibin]
        self.up[ibin] = abs(max(diffUp,diffDn))
        self.dn[ibin] = abs(min(diffUp,diffDn))

class Nuisance(object):
    unclist = []
    def __init__(self,process,name,up,dn,norm,type="diff"):
        self.process = process
        self.name = name
        self.norm = norm
        self.up,self.dn = up,dn
        if type == "abs": MakeDiff(self)
    def Integral(self):
        nbins = self.norm.GetNbinsX()
        intUp = sum( self.norm[ibin] + self.up[ibin] for ibin in range(1,nbins+1) )
        intDn = sum( self.norm[ibin] - self.dn[ibin] for ibin in range(1,nbins+1) )
        return intUp,intDn
    def VarDiff(self):
        norm = self.norm.Integral()
        up,dn = self.Integral()
	varup = (up-norm)/norm
        vardn = (dn-norm)/norm
        return varup,vardn
    def GetHistos(self):
        up = self.up.Clone(); dn = self.dn.Clone()
        nbins = self.norm.GetNbinsX()
        for ibin in range(1,nbins+1):
            up[ibin] = self.norm[ibin] + self.up[ibin]
            dn[ibin] = self.norm[ibin] - self.dn[ibin]
        return up,dn
    def __str__(self):
        varup,vardn = self.VarDiff()
        return '{0:<20}'.format('%s %s' % (self.name,self.process))+'%+.1e/%+.1e' % (varup,vardn)
    def printByBin(self):
        string = '{0:<20}'.format('%s %s' % (self.name,self.process))
        for ibin in range(1,self.norm.GetNbinsX()+1):
            varup = self.up[ibin]/self.norm[ibin]
            vardn = -self.dn[ibin]/self.norm[ibin]
            string += '{0:<10}'.format('\n%i bin:' % ibin) + '%+.1e/%+.1e' % (varup,vardn)
        print string
    def copy(self,process=None):
        if process is None: process = self.process
        name = self.name
        norm = self.norm.Clone()
        up = self.up.Clone()
        dn = self.dn.Clone()
        return Nuisance(process,name,up,dn,norm)
    def add(self,other):
        if self.name != other.name: raise ValueError("%s is not %s" % (self.nuisance,other.nuisance))
        self.norm.Add(other.norm)
        nbins = self.norm.GetNbinsX()
        for ibin in range(1,nbins+1):
            self.up[ibin] = TMath.Sqrt( sum( nuis.up[ibin]**2 for nuis in (self,other) ) )
            self.dn[ibin] = TMath.Sqrt( sum( nuis.dn[ibin]**2 for nuis in (self,other) ) )

from ROOT import TMath,gDirectory,TFile,gROOT,TCanvas,gPad
from utilities import GetRootFiles,debug_hslist

nuisfiles = {}

def GetPSWFile():
    if "psw" in nuisfiles: return
    rootdir = GetRootFiles()
    nuisfiles["psw"] = TFile("%s/psw/PSW_SF.root"%rootdir)
def GetProcessPSW(self,nuisance):
    GetPSWFile()
    def getPSW():
        tfile = nuisfiles["psw"]
        name = "%s_%s_%s"%(self.name,self.variable.base,nuisance)
        up = self.histo.Clone("%sUp"%name)
        dn = self.histo.Clone("%sDown"%name)
        if not any(self.process == procs.GetName() for procs in tfile.GetListOfKeys()):
            return up,dn
        tdir = tfile.GetDirectory(self.process)
        if not any(nuisance in nuis.GetName() for nuis in tdir.GetListOfKeys()):
            return up,dn
        upsf = tdir.Get(nuisance+"Up")
        dnsf = tdir.Get(nuisance+"Down")
        up.Multiply(upsf)
        dn.Multiply(dnsf)
        return up,dn
    up,dn = getPSW()
    self.nuisances[nuisance] = Nuisance(self.process,nuisance,up,dn,self.histo,type="abs")
    return self.nuisances[nuisance]
def MakeDiff(self):
    nbins = self.norm.GetNbinsX()
    for ibin in range(1,nbins+1):
        diffUp = self.up[ibin] - self.norm[ibin]; diffDn = self.dn[ibin] - self.norm[ibin]
        self.up[ibin] = abs(max(diffUp,diffDn))
        self.dn[ibin] = abs(min(diffUp,diffDn))
def MakeSym(self,select=max):
    nbins = self.norm.GetNbinsX()
    for ibin in range(1,nbins+1):
        self.up[ibin] = select(self.up[ibin],self.dn[ibin])
        self.dn[ibin] = select(self.up[ibin],self.dn[ibin])
def AddLikeNuisances(nuisances,up,dn):
    nbins = up.GetNbinsX()
    for ibin in range(1,nbins+1):
        up[ibin] = TMath.Sqrt( sum( nuisance.up[ibin]**2 for nuisance in nuisances ) )
        dn[ibin] = TMath.Sqrt( sum( nuisance.dn[ibin]**2 for nuisance in nuisances ) )
def AddDiffNuisances(nuisances,up,dn,norm):
    nbins = up.GetNbinsX()
    for ibin in range(1,nbins+1):
        if norm[ibin] == 0: continue
        up[ibin] = norm[ibin] * TMath.Sqrt( sum( (nuisance.up[ibin]/norm[ibin])**2 for nuisance in nuisances) )
        dn[ibin] = norm[ibin] * TMath.Sqrt( sum( (nuisance.dn[ibin]/norm[ibin])**2 for nuisance in nuisances) )
def GetNuisanceList(tfile,dirname):
    tfile.cd(dirname)
    shapelist = [ key.GetName().replace('Up','') for key in gDirectory.GetListOfKeys() if 'Up' in key.GetName() ]
    tree = gDirectory.Get('norm')
    scalelist = [ key.GetName().replace('Up','') for key in tree.GetListOfBranches() if 'Up' in key.GetName() if ('PSW' in key.GetName() and 'Con' in key.GetName()) or 'PSW' not in key.GetName() ]
    nuisances = {}
    for shape in shapelist:
        nuisances[shape] = 'shape'
    for scale in scalelist:
        nuisances[scale] = 'scale'
    nuisances['Stat'] = True
    return nuisances
def GetScaleWeight(nuisance):
    zwunc = [
            "QCD_Scale",
            "QCD_Shape",
            "QCD_Proc",
            "NNLO_EWK",
            "NNLO_Miss",
            "NNLO_Sud",
            "QCD_EWK_Mix"]
    if nuisance in zwunc: return nuisance+"%s/kfactor"
    if nuisance == 'prefiring': return nuisance+"%s/prefiring"
    return nuisance+'%s'
        
class Nuisance(object):
    def __init__(self,process,name,up,dn,norm,type="diff",sym=True):
        self.process = process
        self.name = name
        self.norm = norm
        self.up,self.dn = up,dn
        self.up.SetTitle("%s_%sUp"%(process,name))
        self.dn.SetTitle("%s_%sDown"%(process,name))
        if type == "abs": MakeDiff(self)
        if sym: MakeSym(self)
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
    def GetDiff(self):
        up = self.up.Clone(); dn = self.dn.Clone()
        dn.Scale(-1)
        return up,dn
    def GetScale(self):
        up = self.up.Clone(); dn = self.dn.Clone()
        nbins = self.norm.GetNbinsX()
        for ibin in range(1,nbins+1):
            up[ibin] =  (self.up[ibin])/self.norm[ibin]
            dn[ibin] = -(self.dn[ibin])/self.norm[ibin]
        return up,dn
    def GetScaleDiff(self):
        up = self.up.Clone(); dn = self.dn.Clone()
        nbins = self.norm.GetNbinsX()
        for ibin in range(1,nbins+1):
            up[ibin] =  (self.norm[ibin] + self.up[ibin])/self.norm[ibin]
            dn[ibin] =  (self.norm[ibin] - self.dn[ibin])/self.norm[ibin]
        return up,dn
    def __str__(self):
        varup,vardn = self.VarDiff()
        return '{0:<20}'.format('%s %s' % (self.name,self.process))+'%+.1e/%+.1e' % (varup,vardn)
    def printByBin(self):
        string = str(self)
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
    def addLike(self,nuisance):
        AddLikeNuisances([self,nuisance],self.up,self.dn)

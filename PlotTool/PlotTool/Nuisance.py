from ROOT import TMath,gDirectory,TFile,gROOT,TCanvas,gPad
from utilities import GetRootFiles,debug_hslist
from Parser import parser

nuisfiles = {}

def GetProcessPSW(self,nuisance):
    if "recoil" in self.variable.variable:
        var = "recoil"
        psw_file = self.variable.cutfix
    if "ChNemPtFrac" in self.variable.variable:
        var = "chnemptfrac"
        psw_file = "_res"
    
    if "psw_"+var not in nuisfiles:
        rootdir = GetRootFiles()
        nuisfiles["psw_"+var] = TFile("%s/psw/%s/PSW_SF%s.root"%(rootdir,var,psw_file))
    def getPSW():
        tfile = nuisfiles["psw_"+var]
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
def GetlnNShape(self,nuisance):
    if "recoil" in self.variable.variable:
        var = "recoil"
    if "ChNemPtFrac" in self.variable.variable:
        var = "chnemptfrac"
        
    if "lnn" not in nuisfiles:
        rootdir = GetRootFiles()
        nuisfiles["lnn"] = TFile("%s/sys_shape/%s/shape_lnn_sys.root"%(rootdir,var))
    dirmap = {"SignalRegion":"sr","DoubleEleCR":"ze","DoubleMuCR":"zm","SingleEleCR":"we","SingleMuCR":"wm","GammaCR":"ga"}
    def getlnN():
        tfile = nuisfiles["lnn"]
        tdir = tfile.GetDirectory(dirmap[self.region])
        name = "%s_%s_sys"%(self.name,self.variable.base)
        up = self.histo.Clone("%sUp"%name)
        dn = self.histo.Clone("%sDown"%name)
        if not any(self.process in procs.GetName() for procs in tdir.GetListOfKeys() ):
            return up,dn
        upsf = tdir.Get("%s_sysUp"%self.process)
        dnsf = tdir.Get("%s_sysDown"%self.process)
        up.Multiply(upsf)
        dn.Multiply(dnsf)
        return up,dn
    up,dn = getlnN()
    self.nuisances[nuisance] = Nuisance(self.process,nuisance,up,dn,self.histo,type="abs")
    return self.nuisances[nuisance]
def GetTheoryShape(self,nuisance):
    if "recoil" in self.variable.variable:
        var = "recoil"
    if "ChNemPtFrac" in self.variable.variable:
        var = "chnemptfrac"
        
    if "theory" not in nuisfiles:
        rootdir = GetRootFiles()
        nuisfiles["theory"] = TFile("%s/sys_shape/%s/shape_theory_sys.root"%(rootdir,var))
    dirmap = {"SignalRegion":"sr","DoubleEleCR":"ze","DoubleMuCR":"zm","SingleEleCR":"we","SingleMuCR":"wm","GammaCR":"ga"}
    def getTheory():
        tfile = nuisfiles["theory"]
        tdir = tfile.GetDirectory(dirmap[self.region])
        name = "%s_%s_%s"%(self.name,self.variable.base,nuisance)
        up = self.histo.Clone("%sUp"%name)
        dn = self.histo.Clone("%sDown"%name)
        if not any(self.process in procs.GetName() for procs in tdir.GetListOfKeys() ):
            return up,dn
        upsf = tdir.Get("%s_%sUp"%(self.process,nuisance.replace("THEORY_","")))
        dnsf = tdir.Get("%s_%sDown"%(self.process,nuisance.replace("THEORY_","")))
        up.Multiply(upsf)
        dn.Multiply(dnsf)
        return up,dn
    up,dn = getTheory()
    self.nuisances[nuisance] = Nuisance(self.process,nuisance,up,dn,self.histo,type="abs")
    return self.nuisances[nuisance]
def MakeDiff(self):
    nbins = self.norm.GetNbinsX()
    for ibin in range(1,nbins+1):
        diffUp = self.up[ibin] - self.norm[ibin]; diffDn = self.dn[ibin] - self.norm[ibin]
        self.up[ibin] = diffUp
        self.dn[ibin] = -diffDn
def MakeScale(self):
    self.up.Divide(self.norm)
    self.dn.Divide(self.norm)
def MakeSym(self,select=max):
    nbins = self.norm.GetNbinsX()
    sign = lambda x : (1,-1)[x < 0]
    for ibin in range(1,nbins+1):
        val = select(self.up[ibin]-1,self.dn[ibin]-1)
        signup = sign(self.up[ibin]-1)
        signdn = sign(self.dn[ibin]-1)

        if signdn == signup:
            signdn = -signup
        self.up[ibin] = signup*val+1
        self.dn[ibin] = signdn*val+1
def ConstantNuisance(self,nuisance,constant):
    nbins = self.histo.GetNbinsX()
    up = self.histo.Clone("%s_%s_%sUp" % (self.name,self.variable.base,nuisance)); up.Reset()
    dn = self.histo.Clone("%s_%s_%sDown" % (self.name,self.variable.base,nuisance)); dn.Reset()
    for ibin in range(1,nbins+1):
        up[ibin] = (1 + constant)*self.histo[ibin]
        dn[ibin] = (1 - constant)*self.histo[ibin]
    self.nuisances[nuisance] = Nuisance(self.process,nuisance,up,dn,self.histo,type="abs")
def AddLikeNuisances(nuisances,up,dn,norm=None):
    # nbins = up.GetNbinsX()
    # for ibin in range(1,nbins+1):
    #     up[ibin] = TMath.Sqrt( sum( nuisance.up[ibin]**2 for nuisance in nuisances ) )
    #     dn[ibin] = TMath.Sqrt( sum( nuisance.dn[ibin]**2 for nuisance in nuisances ) )
    hslist = []
    for nuisance in nuisances:
        u,d = nuisance.GetHistos()
        n = nuisance.norm.Clone()
        hslist.append( (u,d,n) )
    up.Reset(); dn.Reset()
    for hs in hslist:
        u,d,n = hs
        if u.Integral() == 0: u = n
        if d.Integral() == 0: d = n 
        up.Add(u); dn.Add(d)
    if norm is not None:
        up.Divide(norm)
        dn.Divide(norm)
def AddDiffNuisances(nuisances,up,dn,norm):
    nbins = up.GetNbinsX()
    for ibin in range(1,nbins+1):
        if norm[ibin] == 0:
            up[ibin],dn[ibin] = 1,1
            continue
        up[ibin] = 1 + TMath.Sqrt( sum( (nuisance.up[ibin]-1 )**2 for nuisance in nuisances) )
        dn[ibin] = 1 - TMath.Sqrt( sum( (nuisance.dn[ibin]-1 )**2 for nuisance in nuisances) )
def GetNuisanceList(tfile,dirname):
    tfile.cd(dirname)
    shapelist = [ key.GetName().replace('Up','') for key in gDirectory.GetListOfKeys() if 'Up' in key.GetName() ]
    tree = gDirectory.Get('tree')
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
    def __init__(self,process,name,up,dn,norm,type="scale",sym=True):
        self.process = process
        self.name = name
        self.norm = norm.Clone()
        if up.Integral() == 0 and dn.Integral() == 0:
            type = "abs"
            self.up = norm.Clone("%s_%sUp"%(process,name))
            self.dn = norm.Clone("%s_%sDown"%(process,name))
        else:
            self.up = up.Clone("%s_%sUp"%(process,name))
            self.dn = dn.Clone("%s_%sDown"%(process,name))
        
        if type == "abs": MakeScale(self)
        if sym: MakeSym(self)
    def Integral(self):
        nbins = self.norm.GetNbinsX()
        hsup,hsdn = self.GetHistos()
        intUp = hsup.Integral()
        intDn = hsdn.Integral()
        return intUp,intDn
    def VarDiff(self):
        norm = self.norm.Integral()
        up,dn = self.Integral()
	varup = (up-norm)/norm if norm > 0 else 0
        vardn = (dn-norm)/norm if norm > 0 else 0
        return varup,vardn
    def GetHistos(self):
        # up = self.up.Clone(); dn = self.dn.Clone()
        # nbins = self.norm.GetNbinsX()
        # for ibin in range(1,nbins+1):
        #     up[ibin] = self.norm[ibin] + self.up[ibin]
        #     dn[ibin] = self.norm[ibin] - self.dn[ibin]
        # return up,dn
        up = self.norm.Clone()
        dn = self.norm.Clone()
        up.Multiply(self.up)
        dn.Multiply(self.dn)
        return up,dn
    def GetDiff(self):
        up = self.up.Clone(); dn = self.dn.Clone()
        nbins = self.norm.GetNbinsX()
        for ibin in range(1,nbins+1):
            up[ibin] = self.norm[ibin]*(self.up[ibin] - 1)
            dn[ibin] = self.norm[ibin]*(self.dn[ibin] - 1)
        return up,dn
    def GetScale(self):
        up = self.up.Clone(); dn = self.dn.Clone()
        # nbins = self.norm.GetNbinsX()
        # for ibin in range(1,nbins+1):
        #     up[ibin] =  (self.up[ibin])/self.norm[ibin]
        #     dn[ibin] = -(self.dn[ibin])/self.norm[ibin]
        return up,dn
    def GetScaleDiff(self):
        up = self.up.Clone(); dn = self.dn.Clone()
        nbins = self.norm.GetNbinsX()
        for ibin in range(1,nbins+1):
            # up[ibin] =  (self.norm[ibin] + self.up[ibin])/self.norm[ibin]
            # dn[ibin] =  (self.norm[ibin] - self.dn[ibin])/self.norm[ibin]
            up[ibin] =  self.up[ibin] - 1
            dn[ibin] =  self.dn[ibin] - 1
        return up,dn
    def GetBand(self):
        band = self.norm.Clone()
        for ibin in range(1,band.GetNbinsX()+1):
            valup = band[ibin]*self.up[ibin] - band[ibin]
            valdn = band[ibin]*self.dn[ibin] - band[ibin]
            val = max( abs(valup),abs(valdn) )
            band.SetBinError(ibin,val)
        return band
    def __str__(self):
        if self.norm:
            varup,vardn = self.VarDiff()
            end = '%+.1e/%+.1e' % (varup,vardn)
        else: end = "None"
        return '{0:<20}'.format('%s %s' % (self.name,self.process))+end
    def printByBin(self):
        string = str(self)
        for ibin in range(1,self.norm.GetNbinsX()+1):
            varup = self.up[ibin] - 1
            vardn = self.dn[ibin] - 1
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

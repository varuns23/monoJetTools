from ROOT import *
import os
from utilities import *
from VariableInfo import *
from Nuisance import *
from Parser import parser

gROOT.SetBatch(1)
        
class SubProcess(object):
    def __init__(self,process,name,fname,xsec=None,year=None,region=None):
        self.process = process; self.subprocess = name; self.fname = fname; self.xsec = xsec; self.year = year; self.region = region
        self.name = GetProcessName(name,year,region)
        self.treemap = {}
        self.initVariable()
    def initVariable(self):
        self.variable = None
        self.nuisances = {}
        self.histo = None
        self.raw_total = 0
        self.scaled_total = 0
    def open(self,config=None,verbose=0):
        if hasattr(self,'isOpen'): return
        self.isOpen = True
        if not os.path.isfile(self.fname+'.root'):
            if verbose: print 'No file %s.root' % self.fname,
            if config is not None and self.fname in config.filevariants:
                alt_fname = next( (fname for fname in config.filevariants[self.fname] if os.path.isfile(fname+'.root')),None )
                if alt_fname is None: print 'skipping'; return False
                self.fname = alt_fname
                if verbose: print 'using %s.root instead' % self.fname
                if self.fname in config.xsec: self.xsec = config.xsec[self.fname]
            else:
                if verbose: print 'skippping';
                return False
        self.tfile = TFile.Open(self.fname+'.root')
        cutflow = GetTObject('h_cutflow',self.tfile)
        self.cutflow = cutflow.GetBinContent(1)
        return self.cutflow > 0
    def output(self,prompt="-- integral of %s: %s",ntemp="{0:<15}",itemp="{0:<8}",total_bkg=0):
        if total_bkg > 0:
            percent = ("%.4g%%" % (100*self.scaled_total/total_bkg))
            print prompt % ( ntemp.format(self.subprocess),itemp.format( '%.6g' % self.scaled_total ) ),'| %s' % (percent)
        else:
            print prompt % ( ntemp.format(self.subprocess),itemp.format( '%.6g' % self.scaled_total ) )
    def setTree(self,dirname,treename):
        tree = None
        if treename not in self.treemap:
            tree = GetTObject('%s/%s' % (dirname,treename),self.tfile)
        if tree is not None: self.treemap[treename] = tree
        return tree is None
    def setVariable(self,variable,weight="weight",cut=None):
        self.tfile.cd()
        self.initVariable()
        if type(variable) == str:
            variable = VariableInfo(self.tfile)
            variable.setVariable(variable,weight,cut)
        self.variable = variable
        self.scaleWidth = variable.scaleWidth
        if variable.isGlobal: self.histo = GetTObject(variable.variable,self.tfile)
        elif variable.isBranch:
            self.setTree(variable.dirname,'tree')
            self.histo = GetBranch('%s_%s' % (self.name,variable.base),variable,self.treemap['tree'])
        elif variable.isNhisto: self.histo = GetTObject("%s/%s"%(variable.dirname,variable.variable),self.tfile)
        
        if self.variable.rebin is not None:
            self.histo = self.histo.Rebin(self.variable.rebin)
        if self.variable.overflow:
            AddOverflow(self.histo)
            
        self.raw_total = self.histo.Integral()
    def scale(self,lumi=None,histo=None):
        if histo is None and lumi is not None:
            histo = self.histo
            if self.xsec is None: self.scaling = 1
            else:                 self.scaling = lumi * self.xsec / self.cutflow
        if self.scaleWidth: histo.Scale(self.scaling,"width")
        elif self.scaling != 1: histo.Scale(self.scaling)
        if histo == self.histo: self.scaled_total = histo.Integral()
    def hasUnc(self,nuisance):
        if nuisance not in self.variable.nuisances: return False
        if not self.variable.isBranch: return False
        isScale = self.variable.nuisances[nuisance] == 'scale'
        if isScale: return hasattr(self.treemap['tree'],nuisance+'Up')
        try:
            self.setTree(self.variable.dirname,nuisance+'Up')
            self.setTree(self.variable.dirname,nuisance+'Down')
        except ValueError: return False
        return True
    def addStat(self):
        self.tfile.cd()
        up = self.histo.Clone('%s_%s_StatUp' % (self.name,self.variable.base)); up.Reset()
        dn = self.histo.Clone('%s_%s_StatDown' % (self.name,self.variable.base)); up.Reset()
        nbins = self.histo.GetNbinsX()
        for ibin in range(1,nbins+1):
            up[ibin] = self.histo[ibin] + self.histo.GetBinError(ibin)
            dn[ibin] = self.histo[ibin] - self.histo.GetBinError(ibin)
        self.nuisances["Stat"] = Nuisance(self.subprocess,"Stat",up,dn,self.histo,type="abs")
    def addUnc(self,nuisance):
        self.tfile.cd()
        if nuisance == 'Stat': self.addStat()
        if nuisance in self.nuisances: return
        if not self.hasUnc(nuisance): return
        isScale = self.variable.nuisances[nuisance] == 'scale'
        name = '%s_%s_%s' % (self.name,self.variable.base,nuisance)
        def getScale():
            scale_weight = GetScaleWeight(nuisance)
            up = GetBranch('%sUp' % name,self.variable,self.treemap['tree'],"%s*%s" % (self.variable.weight,scale_weight%"Up"))
            dn = GetBranch('%sDown' % name,self.variable,self.treemap['tree'],"%s*%s" % (self.variable.weight,scale_weight%"Down"))
            return up,dn
        def getShape():
            treeup = '%sUp' % nuisance;   
            treedn = '%sDown' % nuisance; 
            up = GetBranch('%sUp' % name,self.variable,self.treemap[treeup])
            dn = GetBranch('%sDown' % name,self.variable,self.treemap[treedn])
            return up,dn
        if isScale: up,dn = getScale()
        else:       up,dn = getShape()

        if self.variable.rebin is not None:
            up = up.Rebin(self.variable.rebin)
            dn = dn.Rebin(self.variable.rebin)
        if self.variable.overflow:
            AddOverflow(up)
            AddOverflow(dn)

        self.scale(histo=up); self.scale(histo=dn)
        self.nuisances[nuisance] = Nuisance(self.subprocess,nuisance,up,dn,self.histo,type="abs")
class Process:
    def __init__(self,name=None,filenames=None,xsecs=None,proctype=None,year=None,region=None,leg=None,color=kGray+1):
        if name is None and filenames is None: return
        
        self.process = name; self.filenames = list(filenames); self.xsecs = xsecs; self.proctype = proctype
        self.year = year; self.region = region; self.leg = leg; self.color = color
        self.name = GetProcessName(name,year,region)
        self.sublist = [ '%s_%s' % (self.name,filename.replace("post","")) for filename in self.filenames ]
        self.xsecs = { sub:xsecs[filename] for sub,filename in zip(self.sublist,self.filenames) } if xsecs is not None else None

        self.subprocesses = {}
        for sub,filename in zip(self.sublist,self.filenames):
            name = filename.replace('post','')
            if self.proctype is 'data' or xsecs is None: xsec = None
            else: xsec = self.xsecs[sub]
            self.subprocesses[sub] = SubProcess(self.process,name,filename,xsec,self.year,self.region)
        self.initVariable()
    def __len__(self): return len(self.sublist)
    def __getitem__(self,i):
        if type(i) == int: key = self.sublist[i]
        if type(i) == str: key = i
        return self.subprocesses[key];
    def __iter__(self):
        for i in range(len(self)): yield self[i]
    def output(self,prompt="integral of %s: %s",ntemp="{0:<15}",itemp="{0:<8}",total_bkg=0,verbose=False):
        if total_bkg > 0:
            if self.proctype == "signal":
                percent = ("%.4g" % (self.scaled_total/TMath.Sqrt(total_bkg)))
            else:
                percent = ("%.4g%%" % (100*self.scaled_total/total_bkg))
            print prompt % ( ntemp.format(self.process),itemp.format( '%.6g' % self.scaled_total ) ),'| %s' % (percent)
        else:
            print prompt % ( ntemp.format(self.process),itemp.format( '%.6g' % self.scaled_total ) )
        if verbose and len(self) > 1:
            for subprocess in self: subprocess.output(total_bkg=self.scaled_total)
    def initVariable(self):
        for subprocess in self: subprocess.initVariable()
        self.histo = None
        self.raw_total = 0
        self.scaled_total = 0
        self.nuisances = {}
    def open(self,config=None):
        if hasattr(self,'isOpen'): return
        self.isOpen = True
        filelist = list(self.filenames)
        sublist = list(self.sublist)
        for subkey,filekey in zip(sublist,filelist):
            if not self[subkey].open(config):
                self.filenames.remove(filekey)
                self.subprocesses.pop(subkey)
                self.sublist.remove(subkey)
        return any(self)
    def setVariable(self,variable,lumi,weight="weight",cut=None):
        self.initVariable()
        if type(variable) == str:
            variable = VariableInfo(self[0].tfile)
            variable.setVariable(variable,weight,cut)
        self.variable = variable
        for subprocess in self:
            subprocess.setVariable(variable)
            self.raw_total += subprocess.raw_total
            subprocess.scale(lumi)
            self.scaled_total += subprocess.scaled_total

            if self.histo is None: self.histo = subprocess.histo.Clone( "%s_%s" % (self.name,variable.base))
            else:                  self.histo.Add(subprocess.histo.Clone())
    def addUnc(self,nuisance,show=False):
        if self.proctype == 'data': return
        if "PSW" in nuisance:
            GetProcessPSW(self,nuisance)
            return
        if "lnn_sys" in nuisance:
            GetlnNShape(self,nuisance)
            return
        if "THEORY" in nuisance:
            GetTheoryShape(self,nuisance)
            return
        if nuisance in self.nuisances: return
        for subprocess in self:
            subprocess.addUnc(nuisance)
        nbins = self.histo.GetNbinsX()
        up = self.histo.Clone("%s_%s_%sUp" % (self.name,self.variable.base,nuisance)); up.Reset()
        dn = self.histo.Clone("%s_%s_%sDown" % (self.name,self.variable.base,nuisance)); dn.Reset()
        AddLikeNuisances([subprocess.nuisances[nuisance] for subprocess in self if nuisance in subprocess.nuisances],up,dn,self.histo)
        self.nuisances[nuisance] = Nuisance(self.process,nuisance,up,dn,self.histo)
        if show: print self.nuisances[nuisance]
    def fullUnc(self,unclist,show=True):
        if self.proctype == 'data': return
        for nuisance in unclist: self.addUnc(nuisance)
        up = self.histo.Clone('%s_%s_TotalUp' % (self.name,self.variable.base));  up.Reset()
        dn = self.histo.Clone('%s_%s_TotalDown' % (self.name,self.variable.base)); dn.Reset()
        AddDiffNuisances([self.nuisances[nuisance] for nuisance in unclist if nuisance in self.nuisances],up,dn,self.histo)
        self.nuisances['Total'] = Nuisance(self.process,'Total',up,dn,self.histo)

        if not show: return
        print self.nuisances["Total"]
    def add(self,other):
        if self.histo is not None: self.histo.Add(other.histo)
        else: self.histo = other.histo.Clone(self.name)
        self.raw_total += other.raw_total
        self.scaled_total += other.scaled_total

        self.sublist += other.sublist
        self.subprocesses.update(other.subprocesses)

        if any(self.nuisances): self.nuisances = {}

            # for nuisance in other.nuisances:
                # up = self.histo.Clone()
                # dn = self.histo.Clone()
                # if nuisance in self.nuisances:
                    # AddLikeNuisances([self.nuisances[nuisance],other.nuisances[nuisance]],up,dn,self.histo)
                    # self.nuisances[nuisance] = Nuisance(self.process,nuisance,up,dn,self.histo)
                # else:
                #     AddLikeNuisances([other.nuisances[nuisace]],up,dn,self.histo)

from ROOT import *
import os
from utilities import GetDirname
from changeBinning import b_info
from Nuisance import Nuisance

def IsGlobal(variable,tfile):
    return tfile.GetListOfKeys().Contains(variable)
def IsNhisto(variable,tfile):
    dirname,ndir = GetDirname(variable)
    tdir = tfile.GetDirectory(dirname)
    if tdir == None: return False
    isNhisto = tdir.GetListOfKeys().Contains(variable)
    # tdir.Close()
    return isNhisto
def IsBranch(variable,tfile):
    dirname,ndir = GetDirname(variable,sub='trees')
    tdir = tfile.GetDirectory(dirname)
    if tdir == None: return False
    b_variable = variable.replace('_%s' % ndir,'')
    tree = tdir.Get('norm')
    isBranch = tree.GetListOfBranches().Contains(b_variable)
    # tdir.Close()
    return isBranch
def CheckHisto(histo):
    pass
def GetTObject(name,tfile):
    tobject = tfile.Get(name)
    if tobject == None: raise ValueError("Unable to find %s in %s" % (name,tfile.GetName()))
    return tobject
def GetBranch(name,b_template,b_variable,tree,weight,cut):
    histo = b_template.Clone(name); histo.Reset()
    if cut == None: tree.Draw("%s>>%s" % (b_variable,name),weight,'goff')
    else:         tree.Draw("%s>>%s" % (b_variable,name),'%s*(%s)' % (weight,cut),'goff')
    return histo
def HistoEqual(hs1,hs2):
    if hs1 == hs2: return True
    if hs1 is None: return True
    if hs2 is None: return False
    xarray1 = list(hs1.GetXaxis().GetXbins())
    xmin1,xmax1 = hs1.GetXaxis().GetXmin(),hs1.GetXaxis().GetXmax()
    xarray2 = list(hs2.GetXaxis().GetXbins())
    xmin2,xmax2 = hs2.GetXaxis().GetXmin(),hs2.GetXaxis().GetXmax()
    if len(xarray1) != len(xarray2): return False
    if xmin1 != xmin2 or xmax1 != xmax2: return False
    return not any( width1 != width2 for width1,width2 in zip(xarray1,xarray2) )
def GetFileTemplate(tfile,variable):
    dirname,ndir = GetDirname(variable)
    b_variable = variable.replace('_%s' % ndir,'')
    template = tfile.Get('%s/%s' % (dirname,variable)).Clone(b_variable)
    template.Reset()
    return template
def GetNuisanceList(tfile,dirname):
    tfile.cd()
    tdir = tfile.GetDirectory(dirname)
    shapelist = [ key.GetName().replace('Up','') for key in tdir.GetListOfKeys() if 'Up' in key.GetName() ]
    tree = tdir.Get('norm')
    scalelist = [ key.GetName().replace('Up','') for key in tree.GetListOfBranches() if 'Up' in key.GetName() ]
    nuisances = {}
    for shape in shapelist:
        nuisances[shape] = 'shape'
        if shape not in Nuisance.unclist: Nuisance.unclist.append(shape)
    for scale in scalelist:
        nuisances[scale] = 'scale'
        if scale not in Nuisance.unclist: Nuisance.unclist.append(scale)
    nuisances['Stat'] = True
    return nuisances

class SubProcess(object):
    def __init__(self,name=None,fname=None,xsec=None,lumi=None,year=None,region=None,copy=None,args=None,config=None):
        if copy is not None: self.copy(copy); return
        self.process = name; self.name = name; self.year = year; self.region = region; self.lumi = lumi; self.args = args; self.config = config
        if region is not None: self.name = "%s_%s" % (region,self.name)
        if year is not None:   self.name = "%s_%s" % (year,self.name)
        self.fname = fname; self.xsec = xsec; self.treemap = {}
        self.initVariable()
    def open(self):
        if not os.path.isfile(self.fname+'.root'):
            print 'No file %s.root,' % self.fname,
            if self.config is not None and self.fname in self.config.filevariants:
                tmp_fname = next( (fname for fname in self.config.filevariants[self.fname] if os.path.isfile(fname+'.root')),None )
                if tmp_fname == None: print 'skipping'; return False
                self.fname = tmp_fname
                print 'using %s.root instead' % self.fname
                self.xsec = self.config.xsec[self.fname]
            else: print 'skipping'; return False
        self.tfile = TFile.Open(self.fname+'.root')
        cutflow = GetTObject("h_cutflow",self.tfile)
        self.cutflow = cutflow.GetBinContent(1)
        return True
    def initVariable(self):
        self.nuisances = {}
        self.histo = None
        self.raw_total = 0
        self.scaled_total = 0
    def setGlobal(self,variable):
        self.tfile.cd()
        self.histo = GetTObject(variable,self.tfile)
    def setNhisto(self,variable):
        self.tfile.cd()
        dirname,ndir = GetDirname(variable)
        self.histo = GetTObject( '%s/%s' % (dirname,variable),self.tfile).Clone('%s_%s' % (self.name,variable))
    def setBranchInfo(self,b_template,cut,variable):
        if cut != b_info.cut: b_info.cut = cut
        if b_template is None and b_info.template is None: b_info.template = GetFileTemplate(self.tfile,variable)
        elif not HistoEqual(b_template,b_info.template): b_info.template = b_template
    def setTree(self,treename,dirname=None):
        self.tfile.cd()
        if dirname is None: dirname = self.dirname
        else: self.dirname = dirname
        if treename not in self.treemap:
            self.treemap[treename] = GetTObject('%s/%s' % (dirname,treename),self.tfile)
        elif self.treemap[treename] == None:
            self.treemap[treename] = GetTObject('%s/%s' % (dirname,treename),self.tfile)
    def setBranch(self,b_template,variable,treename,weight,cut):
        self.tfile.cd()
        self.setBranchInfo(b_template,cut,variable)
        dirname,ndir = GetDirname(variable)
        b_template = b_info.template
        b_variable = b_template.GetName()
        self.setTree(treename,dirname)
        self.nuisances = GetNuisanceList(self.tfile,'%s/trees' % dirname)
        self.histo = GetBranch('%s_%s' % (self.name,b_variable),b_template,b_variable,self.treemap[treename],weight,cut)
        if hasattr(b_info,'post'): b_info.post(self.histo)
    def fillRaw(self):
        self.raw_total = self.histo.Integral()
        return self.raw_total != 0
    def scale(self,histo=None,lumi=None):
        if lumi is None: lumi = self.lumi
        #scaling = Luminosity * NNLO-cross-section / Total-Events
        if histo is None:
            histo = self.histo
            if not self.args.no_width:
                self.scaleWidth = any( "%.3f" % histo.GetBinWidth(ibin) != "%.3f" % histo.GetBinWidth(ibin+1) for ibin in range(1,histo.GetNbinsX()) )
            else: self.scaleWidth = False
            if self.xsec is None: self.scaling = 1
            else:                 self.scaling = lumi * self.xsec / self.cutflow
        if self.scaleWidth:     histo.Scale(self.scaling,"width")
        elif self.scaling != 1: histo.Scale(self.scaling)
        if histo == self.histo: self.scaled_total = histo.Integral()
    def addUnc(self,nuisance):
        self.tfile.cd()
        if nuisance not in self.nuisances: return
        if type(self.nuisances[nuisance]) == Nuisance: return
        if nuisance == 'Stat': self.addStat(); return
        b_template = b_info.template
        b_variable = b_template.GetName()
        cut = b_info.cut
        isScale = self.nuisances[nuisance] == 'scale'
        name = '%s_%s_%s' % (self.name,b_variable,nuisance)
        if isScale:
            treename = 'trees/norm'
            self.setTree(treename)
            up = GetBranch('%sUp' % name,b_template,b_variable,self.treemap[treename],'%sUp' % nuisance,cut)
            dn = GetBranch('%sDown' % name,b_template,b_variable,self.treemap[treename],'%sDown' % nuisance,cut)
        else:
            treeup = 'trees/%sUp' % nuisance; self.setTree(treeup)
            treedn = 'trees/%sDown' % nuisance; self.setTree(treedn)
            up = GetBranch('%sUp' % name,b_template,b_variable,self.treemap[treeup],'weight',cut)
            dn = GetBranch('%sDown' % name,b_template,b_variable,self.treemap[treeup],'weight',cut)
        self.scale(up); self.scale(dn)
        self.nuisances[nuisance] = Nuisance(self.process,nuisance,up,dn,self.histo,type="abs")
    def addStat(self):
        self.tfile.cd()
        b_variable = b_info.template.GetName()
        up = self.histo.Clone('%s_%s_StatUp' % (self.name,b_variable)); up.Reset()
        dn = self.histo.Clone('%s_%s_StatDown' % (self.name,b_variable)); up.Reset()
        nbins = self.histo.GetNbinsX()
        for ibin in range(1,nbins+1):
            up[ibin] = self.histo.GetBinError(ibin)
            dn[ibin] = self.histo.GetBinError(ibin)
        self.nuisances["Stat"] = Nuisance(self.process,"Stat",up,dn,self.histo)
    def copy(self,other):
        self.process = other.name; self.name = other.name; self.year = other.year;
        self.region = other.region; self.lumi = other.lumi; self.fname = other.fname; self.xsec = other.xsec;
        self.args = other.args
        self.tfile = other.tfile
        self.histo = other.histo.Clone()
        self.raw_total = other.raw_total
        self.scaled_total = other.scaled_total
        self.treemap = other.treemap
class Process(object):
    isGlobal = False
    isNhisto = False
    isBranch = False
    def __init__(self,name=None,filenames=None,xsecs=None,proctype=None,year=None,region=None,leg=None,lumi=1,color=kGray+1,copy=None,args=None,config=None):
        if copy is not None: self.copy(copy); return
        self.process = name; self.name = name; self.year = year; self.region = region
        if region is not None: self.name = "%s_%s" % (region,self.name)
        if year is not None:   self.name = "%s_%s" % (year,self.name)
        self.sublist = [ '%s_%s' % (self.name,filename) for filename in filenames ]
        self.xsecs = { '%s_%s' % (self.name,filename):xsecs[filename] for filename in filenames } if xsecs is not None else xsecs
        self.filenames = list(filenames);
        self.proctype = proctype
        self.leg = leg; self.lumi = lumi; self.color = color; self.args = args; self.config = config
        self.isOpen = False
        self.subprocesses = {}; self.nuisances = {}
        filelist = list(self.filenames)
        for sub,filename in zip(self.sublist,filelist):
            name = filename.replace('post','')
            if self.proctype == 'data': xsec = None
            else: xsec = self.xsecs[sub]
            self.subprocesses[sub] = SubProcess(name,filename,xsec,self.lumi,self.year,self.region,args=self.args,config=self.config)
        self.initVariable()
    def __len__(self): return len(self.sublist)
    def __getitem__(self,i):
        if type(i) == int: key = self.sublist[i]
        if type(i) == str: key = i
        return self.subprocesses[key];
    def __iter__(self):
        for i in range(len(self)): yield self[i]
    def open(self):
        if self.isOpen: return
        self.isOpen = False
        filelist = list(self.filenames)
        sublist = list(self.sublist)
        for subkey,filekey in zip(sublist,filelist):
            if not self[subkey].open():
                self.filenames.remove(filekey)
                self.subprocesses.pop(subkey)
                self.sublist.remove(subkey)
        return any(self)
    def initVariable(self):
        for subprocess in self: subprocess.initVariable()
        self.histo = None
        self.raw_total = 0
        self.scaled_total = 0
        self.nuisances = {}

        Process.isGlobal = False
        Process.isNhisto = False
        Process.isBranch = False
    def setGlobal(self,variable):
        Process.isGlobal = True
        for subprocess in self: subprocess.setGlobal(variable)
    def setNhisto(self,variable):
        Process.isNhisto = True
        for subprocess in self: subprocess.setNhisto(variable)
    def setBranchInfo(self,b_template,weight,cut):
        if cut != b_info.cut: b_info.cut = cut
        if weight != b_info.weight: b_info.weight = weight
        if not HistoEqual(b_template,b_info.template): b_info.template = b_template
    def setBranch(self,variable,b_template,weight,cut):
        Process.isBranch = True
        self.setBranchInfo(b_template,weight,cut)
        b_template = b_info.template
        cut = b_info.cut
        weight = b_info.weight
        for subprocess in self: subprocess.setBranch(b_template,variable,'trees/norm',weight,cut)
    def fillRaw(self):
        self.raw_total = 0
        for subprocess in self:
            subprocess.fillRaw()
            self.raw_total += subprocess.raw_total
    def scale(self):
        self.scaled_total = 0
        for subprocess in self:
            subprocess.scale(lumi=self.lumi)
            self.scaleWidth = subprocess.scaleWidth
            self.scaled_total += subprocess.scaled_total
    def setVariable(self,variable,b_template=None,weight=None,cut=None):
        self.initVariable()
        tfile = iter(self).next().tfile
        if IsGlobal(variable,tfile): self.setGlobal(variable)
        elif IsBranch(variable,tfile): self.setBranch(variable,b_template,weight,cut)
        elif IsNhisto(variable,tfile): self.setNhisto(variable)
        self.fillRaw()
        self.scale()
        self.histo = None
        for subprocess in self:
            if self.histo is None: self.histo = subprocess.histo.Clone('%s_%s' % (self.name,variable))
            else:                  self.histo.Add(subprocess.histo)
    def addUnc(self,nuisance,show=True):
        if nuisance in self.nuisances: return
        if self.proctype == 'data': return
        for subprocess in self: subprocess.addUnc(nuisance)
        b_template = b_info.template
        b_variable = b_template.GetName()
        cut = b_info.cut
        nbins = self.histo.GetNbinsX()
        up = self.histo.Clone("%s_%s_%sUp" % (self.name,b_variable,nuisance))
        dn = self.histo.Clone("%s_%s_%sDown" % (self.name,b_variable,nuisance))
        for ibin in range(1,nbins+1):
            up[ibin] = TMath.Sqrt( sum( subprocess.nuisances[nuisance].up[ibin]**2 for subprocess in self
                                        if nuisance in subprocess.nuisances and type(subprocess.nuisances[nuisance]) == Nuisance) )
            dn[ibin] = TMath.Sqrt( sum( subprocess.nuisances[nuisance].dn[ibin]**2 for subprocess in self
                                        if nuisance in subprocess.nuisances and type(subprocess.nuisances[nuisance]) == Nuisance) )
        self.nuisances[nuisance] = Nuisance(self.process,nuisance,up,dn,self.histo)
        if not show or self.scaled_total == 0: return
        print self.nuisances[nuisance]
    def addStat(self):
        b_template = b_info.b_tempalte
        b_variable = b_template.GetName()
        up = self.histo.Clone('%s_%s_StatUp' % (self.name,b_variable)); up.Reset()
        dn = self.histo.Clone('%s_%s_StatDown' % (self.name,b_variable)); dn.Reset()
        nbins = self.histo.GetNbinsX()
        for ibin in range(1,nbins+1):
            up[ibin] = self.histo.GetBinError(ibin)
            dn[ibin] = self.histo.GetBinError(ibin)
        self.nuisances["Stat"] = Nuisance(self.process,"Stat",up,dn,self.histo)
    def fullUnc(self,unclist=None,stat=False,show=True):
        if self.proctype != 'bkg': return
        
        if unclist is None: unclist = self.nuisances.keys()
        unclist = list(unclist)
        if not stat and 'Stat' in unclist: unclist.remove('Stat')
        elif stat and 'Stat' not in unclist: unclist.append('Stat')
        
        for nuisance in unclist: self.addUnc(nuisance,show=show)
        b_template = b_info.template
        b_variable = b_template.GetName()
        up = self.histo.Clone('%s_%s_TotalUp' % (self.name,b_variable));  up.Reset()
        dn = self.histo.Clone('%s_%s_TotalDown' % (self.name,b_variable)); dn.Reset()
        nbins = up.GetNbinsX()
        for ibin in range(1,nbins+1):
            if self.histo[ibin] == 0: continue
            up[ibin] = self.histo[ibin] * TMath.Sqrt( sum( (self.nuisances[nuisance].up[ibin]/self.histo[ibin])**2 for nuisance in unclist) )
            dn[ibin] = self.histo[ibin] * TMath.Sqrt( sum( (self.nuisances[nuisance].dn[ibin]/self.histo[ibin])**2 for nuisance in unclist) )
        self.nuisances['Total'] = Nuisance(self.process,'Total',up,dn,self.histo)
    def copy(self,other):
        self.process = other.process; self.name = other.name; self.year = other.year; self.region = other.region
        self.proctype = other.proctype
        self.leg = other.leg; self.lumi = other.lumi; self.color = other.color
        self.histo = other.histo
        self.raw_total = other.raw_total
        self.scaled_total = other.scaled_total
        self.nuisances = other.nuisances
        self.args = other.args
        # self.isOpen = other.isOpen
        # self.filenames = list(other.filenames)
        # self.xsecs = other.xsecs
        # self.subprocesses = { filename:SubProcess(copy=other.subprocesses[filename]) for filename in self.filenames }
    def add(self,other):
        # if self.process != other.process:
        #     if not (self.proctype == 'sumofbkg' and other.proctype == 'bkg'):raise ValueError("%s is not %s" % (self.process,other.process))
        if self.histo is not None: self.histo.Add(other.histo)
        else: self.histo = other.histo.Clone(self.name)
        self.raw_total += other.raw_total
        self.scaled_total += other.scaled_total
        # self.subprocesses.update(other.subprocesses)
        for nuisance in other.nuisances:
            if nuisance not in self.nuisances: self.nuisances[nuisance] = other.nuisances[nuisance].copy(self.process)
            else: self.nuisances[nuisance].add(other.nuisances[nuisance])

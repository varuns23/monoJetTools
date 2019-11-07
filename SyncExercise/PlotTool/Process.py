from ROOT import *

def ValidProctype(proctype):
    if not (proctype == 'data' or proctype == 'bkg' or proctype == 'signal'):
        raise ValueError('Invalid proctype: %s' % proctype)
def GetDirname(variable,sub=None):
    ndir = variable.split('_')[-1]
    dirname = 'ZprimeJet_%s' % ndir
    if sub != None: dirname += '/%s' % sub
    return dirname,ndir
def IsGlobal(variable,tfile):
    return tfile.GetListOfKeys().Contains(variable)
def IsNhisto(variable,tfile):
    dirname,ndir = GetDirname(variable)
    tdir = tfile.GetDirectory(dirname)
    if tdir == None: return False
    return tdir.GetListOfKeys().Contains(variable)
def IsBranch(variable,tfile):
    dirname,ndir = GetDirname(variable,sub='trees')
    tdir = tfile.GetDirectory(dirname)
    if tdir == None: return False
    b_variable = variable.replace('_%s' % ndir,'')
    tree = tdir.Get('norm')
    return tree.GetListOfBranches().Contains(b_variable)
def GetBranch(b_template,b_variable,tree,weight,cut):
    b_template.Reset()
    b_name = 'template_%s_%s_%s' % (b_variable,tree.GetName(),weight)
    b_template.SetName(b_name)
    b_template.SetDirectory(gDirectory)
    if not tree.GetListOfBranches().Contains(weight): weight = 'weight'
    if cut == None: tree.Draw('%s>>%s' % (b_variable,b_name),weight,'goff')
    else:           tree.Draw('%s>>%s' % (b_variable,b_name),'%s*(%s%s)' % (weight,b_variable,cut),'goff')
    b_template.SetDirectory(0)
    return b_template
class SubProcess(object):
    def __init__(self,name,filename,tfile,xsec):
        self.name = name
        self.filename = filename
        self.tfile = tfile
        self.xsec = xsec
        self.tdir = None
        self.trees = {}
        self.cutflow = 0
        self.init()
    def init(self):
        self.histo = None
        self.raw_total = 0
        self.scaled_total = 0
        self.nuisances = {}
        self.b_variable = ''
        self.b_template = None
    def getCutflow(self):
        cutflow = self.tfile.Get('h_cutflow')
        cutflow = cutflow.GetBinContent(1)
        self.cutflow = cutflow
    def getGlobal(self):
        self.histo = self.tfile.Get(variable)
    def setDir(self,dirname):
        self.tdir = self.tfile.Get(dirname)
    def getNhisto(self,variable):
        self.tdir.cd()
        hs = self.tdir.Get(variable).Clone('%s_%s' % (variable,self.filename))
        hs.SetDirectory(0)
        self.histo = hs
    def getNuisanceList(self):
        if any(self.nuisances): return
        shapelist = [ key.GetName().replace('Up','') for key in self.tdir.GetListOfKeys() if 'Up' in key.GetName() ]
        tree = self.tdir.Get('norm')
        scalelist = [ key.GetName().replace('Up','') for key in tree.GetListOfBranches() if 'Up' in key.GetName() ]
        for shape in shapelist: self.nuisances[shape] = {type:'shape'}
        for scale in scalelist: self.nuisances[scale] = {type:'scale'}
    def getBranch(self,b_template,b_variable,treename,weight,cut):
        self.b_variable = b_variable
        self.b_template = b_template.Clone()
        self.tdir.cd()
        self.getNuisanceList()
        if treename not in self.trees: self.trees[treename] = self.tdir.Get(treename)
        self.histo = GetBranch(b_template,b_variable,self.trees[treename],weight,cut).Clone('%s_%s' % (b_variable,self.filename))
        if 'post' in dir(b_template): b_template.post(self.histo)
    def fillRaw(self):
        self.raw_total = self.histo.Integral()
        return self.raw_total != 0
    def scale(self,lumi):
        #Scaling = (1/TotalEvents)*Luminosity*NNLO-cross-section
        self.scaling = (1./self.cutflow) * self.xsec * lumi
        self.histo.Scale(self.scaling)
        self.scaled_total = self.histo.Integral()
    def addUnc(self,nuisance,cut):
        if nuisance not in self.nuisances:
            self.nuisances[nuisance] = {'Up':self.histo,'Down':self.histo}
            return
        info = self.nuisances[nuisance]
        isScale = info[type] == 'scale'
        b_variable = self.b_variable
        b_template = self.b_template.Clone()
        self.tdir.cd()
        for variation in ('Up','Down'):
            if isScale:
                treename = 'norm'
                if not treename in self.trees: self.trees[treename] = self.tdir.Get(treename)
                tree = self.trees[treename]
                hs_unc = GetBranch(b_template,b_variable,tree,nuisance+variation,cut).Clone('%s_%s_%s%s' % (b_variable,self.filename,nuisance,variation))
            else:
                treename = nuisance+variation
                if not treename in self.trees: self.trees[treename] = self.tdir.Get(treename)
                tree = self.trees[treename]
                hs_unc = GetBranch(b_template,b_variable,tree,'weight',cut).Clone('%s_%s_%s%s' % (b_variable,self.filename,nuisance,variation))
            hs_unc.Scale(self.scaling)
            info[variation] = hs_unc
    
class Process(object):
    def __init__(self,name,filenames,xsecs,proctype,lumi=1,color = None):
        ValidProctype(proctype)
        self.name = name
        self.filenames = [ filename for filename in filenames ]
        self.xsecs = xsecs
        self.proctype = proctype
        
        self.lumi = lumi

        self.color = color

        self.variable = None
        self.dirname = None
        self.ndir = None
        self.b_template = None
        
        self.subprocesses = {}
        self.nuisances = {}
        
        self.histo = None
        self.raw_total = 0
        self.scaled_total = 0

        self.isGlobal = False
        self.isNhisto = False
        self.isBranch = False
        self.isOpen = False
    def __len__(self): return len(self.filenames)
    def __getitem__(self,i):
        f = self.filenames[i]
        return self.subprocesses[f]
    def __iter__(self):
        for i in range( len(self) ):
            yield self[i]
    def open(self):
        self.isOpen = True
        for filename in self.filenames:
            fname = filename + '.root'
            name = filename.replace('post','')
            tfile = TFile.Open(fname)
            if self.proctype != 'data': xsec = self.xsecs[filename]
            else:                  xsec = None
            subprocess = SubProcess(name,filename,tfile,xsec)
            self.subprocesses[filename] = subprocess
            
    def getCutflow(self):
        for subprocess in self: subprocess.getCutflow()
    def getGlobal(self,variable):
        self.isGlobal = True
        self.variable = variable
        for subprocess in self: subprocess.getGlobal()
    def setDir(self,dirname,ndir):
        self.dirname = dirname
        self.ndir = ndir
        for subprocess in self: subprocess.setDir(dirname)
    def getNhisto(self,variable):
        self.isNhisto = True
        dirname,ndir = GetDirname(variable)
        if self.dirname != dirname:
            self.setDir(dirname,ndir)
            self.variable = variable
        for subprocess in self: subprocess.getNhisto(variable)
    def getFileTemplate(self,variable):
        tfile = iter(self).next().tfile
        dirname,_ = GetDirname(variable)
        tdir = tfile.Get(dirname)
        if 'h_recoil' in variable: variable = variable.replace('h_recoil','pfMET')
        self.b_template = tdir.Get(variable)
    def getBranch(self,variable,b_template,cut):
        self.isBranch = True
        if variable == None: variable = self.variable
        if b_template == None and self.b_template == None: self.getFileTemplate(variable)
        if b_template == None: b_template = self.b_template
        else:             self.b_template = b_template
        self.cut = cut
        
        dirname,ndir = GetDirname(variable,sub='trees')
        if self.dirname != dirname:
            self.setDir(dirname,ndir)
            self.variable = variable
        b_variable = variable.replace('_%s' % ndir,'')
        for subprocess in self:
            subprocess.getBranch(b_template,b_variable,'norm','weight',cut)
            for nuisance,info in subprocess.nuisances.iteritems(): self.nuisances[nuisance] = info
    def fillRaw(self):
        self.raw_total = 0
        for subprocess in self:
            subprocess.fillRaw()
            self.raw_total += subprocess.raw_total
    def scale(self):
        if self.proctype == 'data': return
        self.scaled_total = 0
        for subprocess in self:
            subprocess.scale(self.lumi)
            self.scaled_total += subprocess.scaled_total
    def combine(self):
        self.histo = iter(self).next().histo.Clone('%s_%s' % (self.variable,self.name))
        self.histo.Reset()
        for subprocess in self: self.histo.Add(subprocess.histo)
    def initVariable(self):
        if not self.isOpen:
            self.open()
            self.getCutflow()

        for subprocess in self: subprocess.init()

        self.histo = None
        self.raw_total = 0
        self.scaled_total = 0
        
        self.isGlobal = False
        self.isNhisto = False
        self.isBranch = False
    def getVariable(self,variable,b_template,cut):
        self.initVariable()
        tfile = iter(self).next().tfile
        if IsGlobal(variable,tfile):   self.getGlobal(variable)
        elif IsBranch(variable,tfile): self.getBranch(variable,b_template,cut)
        elif IsNhisto(variable,tfile): self.getNhisto(variable)
        self.fillRaw()
        self.scale()
        if self.proctype != 'signal': self.combine()
    def addUnc(self,nuisance):
        if not self.isBranch: return
        if nuisance not in self.nuisances:
            self.nuisances[nuisance] = {}
        for subprocess in self:
            subprocess.addUnc(nuisance,self.cut)
        for variation in ('Up','Down'):
            histo = self.b_template
            histo.Reset()
            for subprocess in self:
                histo.Add(subprocess.nuisances[nuisance][variation].Clone())
                subprocess.nuisances[nuisance].pop(variation)
            if self.proctype != 'signal':
                self.nuisances[nuisance][variation] = histo.Clone( '%s_%s%s' % (self.name,nuisance,variation) )
            

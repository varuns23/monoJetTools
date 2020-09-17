
from ROOT import TH1F
from array import array
import numpy as np
from utilities import *
from Nuisance import *
from Parser import parser
from samplenames import samplenames

group = parser.add_group(__file__,__doc__,"Class")
group.add_argument("-b","--binning",help="specify function for rebinning histogram",action="store",type=str,default=None)
group.add_argument("--branch",help="Use TTree if available",action="store_true",default=False)
group.add_argument("--rebin",help="Specify number of bins to merge using TH1::Rebin()",type=int)
group.add_argument("-w","--weight",help="Specify the weight to use for branch variables",type=str,default="weight")
group.add_argument("-c","--cut",help="Specify cut on branch variable using TTree string",type=lambda arg:str(arg).replace('"','').replace("'",""),default=None)
group.add_argument("--no-width",help="Disable bin width scaling",action="store_true",default=False)
group.add_argument("--add-overflow",help="Add overflow bin to last bin",action="store_true",default=False)

extraction_variables = ("recoil","ChNemPtFrac")

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
    # if not any( extraction in variable for extraction in extraction_variables ): return False
    dirname,ndir = GetDirname(variable,sub='trees')
    tdir = tfile.GetDirectory(dirname)
    if tdir == None: return False
    b_variable = variable.replace('_%s' % ndir,'')
    tree = tdir.Get('tree' if "monoJet" in dirname else "norm")
    isBranch = tree.GetListOfBranches().Contains(b_variable)
    # tdir.Close()
    return isBranch
def FileTemplate(self,tfile,variable):
    dirname,ndir = GetDirname(variable)
    return tfile.Get("%s/%s"%(dirname,variable)).Clone('template_%s'%self.base)

def linspace(xmin,xmax,nx): return list(np.linspace(xmin,xmax,nx+1))

def AddOverflow(hs):
    nbins = hs.GetNbinsX()
    overflow = hs.GetBinContent(nbins) + hs.GetBinContent(nbins+1)
    sqr_error = hs.GetBinError(nbins)**2 + hs.GetBinError(nbins+1)**2
    hs.SetBinContent(nbins,overflow)
    hs.SetBinError(nbins,TMath.Sqrt(sqr_error))

    hs.SetBinContent(nbins+1,0)
    hs.SetBinError(nbins+1,0)
    
    return
def PtFractionBinning(self,arg):
    arg = arg.replace('res','')
    # bins = array('d',[0,0.3,0.5,0.7,0.8,0.9,1.0])
    bins = array('d',[0,0.15,0.3,0.4,0.5,0.6,0.7,0.75,0.8,0.85,0.9,0.95,1.0])
    if arg == "2": bins = array('d',[0,0.25,0.4,0.55,0.7,0.85,1.0])
    self.overflow = True
    nbins= len(bins)-1
    template = TH1F(self.base,'{title}:{xaxis_title}:{yaxis_title}'.format(**vars(self)),nbins,bins)
    return template
def inclusiveBinning(self,arg):
    nbins = int(arg.replace('incl',''))
    template = TH1F(self.base,'{title}:{xaxis_title}:{yaxis_title}'.format(**vars(self)),nbins,0,1)
    self.overflow = True
    return template

def inclusiveCutBinning(self,arg):
    nbins = int(arg.replace('incu',''))
    cut = self.cut
    if '>' in cut:
        lim = float(cut.split('>')[-1])
        bmin = lim; bmax = 1
    template = TH1F(self.base,'{title}:{xaxis_title}:{yaxis_title}'.format(**vars(self)),nbins,bmin,bmax)
    self.overflow = True
    return template
    
class VariableInfo:
    binningMap = {
        'incl':inclusiveBinning,
        'incu':inclusiveCutBinning,
        'res':PtFractionBinning
    }
    def __init__(self,tfile=None):
        self.initVariable()
        self.initFile(tfile)
    def initFile(self,tfile):
        if type(tfile) == str: tfile = TFile(tfile)
        finaldir = None
        finalnhs = None
        self.dirlist = []
        for key in tfile.GetListOfKeys():
            if tfile.GetDirectory(key.GetName()):
                dirname = key.GetName()
                ndir = dirname.split('_')[1]
                self.dirlist.append(ndir)
                if tfile.GetDirectory("%s/trees"%dirname):
                    finaldir = dirname
                    finalnhs = ndir
        self.nuisances = {'Stat':True}
        if tfile.GetDirectory('%s/trees'%finaldir ): self.nuisances = GetNuisanceList(tfile,'%s/trees'%finaldir)
        self.finaldir = finaldir
        self.finalnhs = finalnhs
        self.tfile = tfile
    def initVariable(self):
        self.variable = None
        self.base = None
        self.template = None
        self.cut = None
        self.weight = None

        self.isGlobal = False
        self.isNhisto = False
        self.isBranch = False
    def setVariable(self,variable,weight="weight",cut=None,autovar=None,tfile=None):
        if tfile is not None: self.initFile(tfile)
        tfile = self.tfile
        self.initVariable()
        if autovar is not None:
            nh = str(int(self.finalnhs)+autovar)
            if nh not in self.dirlist: nh = self.finalnhs
            variable += '_'+nh
        self.variable = variable

        self.weight = weight
        if self.weight is "weight": self.weight = parser.args.weight
        
        self.cut = cut
        if self.cut is None: self.cut = parser.args.cut
        if self.cut is not None:
            cutvar = self.cut.replace('>','?').replace('<','?').split('?')[0]
            if cutvar in variable: self.cutfix = self.cut.replace(cutvar,'').replace('<','-').replace('>','+')
            else: self.cutfix = "_"+self.cut.replace('<','-').replace('>','+')
        
        if IsGlobal(variable,tfile): self.initGlobal(tfile,variable)
        elif parser.args.branch and IsBranch(variable,tfile): self.initBranch(tfile,variable)
        elif IsNhisto(variable,tfile): self.initNhisto(tfile,variable)

        if self.template == None:
            raise ValueError("Unidentified variable %s"%self.variable)
        self.title = self.template.GetTitle()
        self.xaxis_title = self.template.GetXaxis().GetTitle()
        self.yaxis_title = self.template.GetYaxis().GetTitle()

        self.rebin = parser.args.rebin
        self.overflow = parser.args.add_overflow

        use_overflow = ("recoil","Frac","Perc")
        if any( var in variable for var in use_overflow ): self.overflow = True

        self.scaleWidth = True
        if parser.args.no_width: self.scaleWidth = False
        # if not parser.args.no_width and self.template.ClassName() == "TH1":
            # self.scaleWidth = any( "%.3f" % self.template.GetBinWidth(ibin) != "%.3f" % self.template.GetBinWidth(ibin+1) for ibin in range(1,self.template.GetNbinsX()) )
        
    def initGlobal(self,tfile,variable):
        self.isGlobal = True
        self.base = variable.replace('h_','')
        self.template = tfile.Get(variable).Clone('template_%s'%self.base)
        self.template.SetDirectory(0)
        self.template.Reset()
        self.title = self.template.GetTitle()
        self.xaxis_title = self.template.GetXaxis().GetTitle()
        self.yaxis_title = self.template.GetYaxis().GetTitle()
    def initNhisto(self,tfile,variable):
        self.isNhisto = True
        self.dirname,ndir = GetDirname(variable)
        self.base = variable.replace('_%s'%ndir,"")
        self.template = tfile.Get("%s/%s"%(self.dirname,self.variable)).Clone('template_%s'%self.base)
        self.template.SetDirectory(0)
        self.template.Reset()
        self.title = self.template.GetTitle()
        self.xaxis_title = self.template.GetXaxis().GetTitle()
        self.yaxis_title = self.template.GetYaxis().GetTitle()
    def initBranch(self,tfile,variable):
        self.isBranch = True
        dirname,ndir = GetDirname(variable)
        self.base = variable.replace('_%s'%ndir,"")
        self.template = self.getBinning(tfile,variable)
        self.template.SetDirectory(0)
        self.template.Reset()
        self.dirname,ndir = GetDirname(variable,'trees')
    def getBinning(self,tfile,variable):
        self.file_template = FileTemplate(self,tfile,variable)
        self.title = self.file_template.GetTitle()
        self.xaxis_title = self.file_template.GetXaxis().GetTitle()
        self.yaxis_title = self.file_template.GetYaxis().GetTitle()
        if "ChNemPtFrac" in variable: parser.args.binning = "res"
        if parser.args.binning is None: return self.file_template
        for label,binning in self.binningMap.iteritems():
            if label in parser.args.binning:
                if label is not 'fix' and label is not 'res': self.binfix = parser.args.binning
                return binning(self,parser.args.binning)
    def setXaxisTitle(self):
        self.name = None
        for title in samplenames:
            if title in self.variable:
                self.name = samplenames[title];
            key = self.variable.split("_")[-2]
            if key == title:
                self.name = samplenames[title];
                break
        if self.name == None: self.name = self.xaxis_title

 

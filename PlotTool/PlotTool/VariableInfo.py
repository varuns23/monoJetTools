
from ROOT import TH1F
from array import array
import numpy as np
from utilities import *
from Nuisance import *
from Parser import parser

parser.add_argument("-b","--binning",help="specify function for rebinning histogram",action="store",type=str,default=None)
parser.add_argument("-w","--weight",help="Specify the weight to use for branch variables",type=str,default="weight")
parser.add_argument("-c","--cut",help="Specify cut on branch variable using TTree string",type=lambda arg:str(arg).replace('"','').replace("'",""),default=None)
parser.add_argument("--no-width",help="Disable bin width scaling",action="store_true",default=False)

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
def FileTemplate(self,tfile,variable):
    dirname,ndir = GetDirname(variable)
    return tfile.Get("%s/%s"%(dirname,variable)).Clone('template_%s'%self.base)

def linspace(xmin,xmax,nx): return list(np.linspace(xmin,xmax,nx+1))

def rebin(self,arg):
    # bins = array('d',[250.,280.,310.,340.,370.,400.,430.,470.,510.,550.,590.,640.,690.,740.,790.,840.,900.,960.,1020.,1090.,1160.,1250.,1400.])
    nbins = int(arg.replace('rebin',''))
    histo = self.file_template
    histo.Rebin(nbins)
    return histo
    
class VariableInfo:
    binningMap = {
        "rebin":rebin
    }
    def __init__(self,tfile=None):
        self.initVariable()
        self.initFile(tfile)
    def initFile(self,tfile):
        if type(tfile) == str: tfile = TFile(tfile)
        finaldir = None
        finalnhs = None
        for key in tfile.GetListOfKeys():
            if tfile.GetDirectory(key.GetName()):
                finaldir = key.GetName()
                finalnhs = finaldir.split('_')[1]
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
    def setVariable(self,variable,weight="weight",cut=None,autovar=False,tfile=None):
        if tfile is not None: self.initFile(tfile)
        tfile = self.tfile
        self.initVariable()
        if autovar: variable += '_'+self.finalnhs
        self.variable = variable

        self.weight = weight
        if self.weight is "weight": self.weight = parser.args.weight
        
        self.cut = cut
        if self.cut is None: self.cut = parser.args.cut
        if self.cut is not None:
            cutvar = self.cut.replace('>','?').replace('<','?').split('?')[0]
            if cutvar in variable: self.cutfix = self.cut.replace(cutvar,'').replace('<','-').replace('>','+')
            else: self.cutfix = self.cut.replace('<','-').replace('>','+')
        
        if IsGlobal(variable,tfile): self.initGlobal(tfile,variable)
        elif IsBranch(variable,tfile): self.initBranch(tfile,variable)
        elif IsNhisto(variable,tfile): self.initNhisto(tfile,variable)

        if parser.args.no_width: self.scaleWidth = False
        else:
            self.scaleWidth = any( "%.3f" % self.template.GetBinWidth(ibin) != "%.3f" % self.template.GetBinWidth(ibin+1) for ibin in range(1,self.template.GetNbinsX()) )
        
    def initGlobal(self,tfile,variable):
        self.isGlobal = True
        self.base = variable.replace('h_','')
        self.template = tfile.Get(variable).Clone('template_%s'%self.base)
        self.template.SetDirectory(0)
        self.template.Reset()
    def initNhisto(self,tfile,variable):
        self.isNhisto = True
        self.dirname,ndir = GetDirname(variable)
        self.base = variable.replace('_%s'%ndir,"")
        self.template = tfile.Get("%s/%s"%(self.dirname,self.variable)).Clone('template_%s'%self.base)
        self.template.SetDirectory(0)
        self.template.Reset()
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
        if parser.args.binning is None: return self.file_template
        for label,binning in self.binningMap.iteritems():
            if label in parser.args.binning:
                if label is not 'fix': self.binfix = parser.args.binning
                return binning(self,parser.args.binning)
        

 

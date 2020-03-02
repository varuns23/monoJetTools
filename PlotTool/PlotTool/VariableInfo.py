from ROOT import TH1F
from array import array
import numpy as np
from utilities import *
from Nuisance import *
from Parser import parser

parser.add_argument("-b","--binning",help="specify function for rebinning histogram",action="store",type=str,default=None)
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
def linspace(xmin,xmax,nx): return list(np.linspace(xmin,xmax,nx+1))

def rebin(arg,sample,name):
    bins = array('d',[250.,280.,310.,340.,370.,400.,430.,470.,510.,550.,590.,640.,690.,740.,790.,840.,900.,960.,1020.,1090.,1160.,1250.,1400.])
    histo = TH1F(name,'',len(bins)-1,bins)
    histo.Rebin(2)
    b_info.template = histo
    
class VariableInfo:
    binningMap = {
        'rebin':rebin
    }
    def __init__(self,tfile=None,variable=None,weight=None,cut=None):
        self.initVariable()
        if variable is not None and tfile is not None:
            self.setVariable(tfile,variable,weight,cut)
    def initVariable(self):
        self.variable = None
        self.base = None
        self.template = None
        self.cut = None
        self.weight = None

        self.isGlobal = False
        self.isNhisto = False
        self.isBranch = False

        self.nuisances = {}
    def setVariable(self,tfile,variable,weight="weight",cut=None):
        self.initVariable()
        self.variable = variable
        self.weight = weight
        
        self.cut = cut
        if cut is not None:
            cutvar = cut.replace('>','?').replace('<','?').split('?')[0]
            if cutvar in variable: self.cutfix = cut.replace(cutvar,'').replace('<','-').replace('>','+')
            else: self.cutfix = cut.replace('<','-').replace('>','+')
        
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
        self.nuisances = GetNuisanceList(tfile,self.dirname)
    def getBinning(self,tfile,variable):
        if parser.args.binning is None:
            dirname,ndir = GetDirname(variable)
            return tfile.Get("%s/%s"%(dirname,variable)).Clone('template_%s'%self.base)
        for label,binning in self.binningMap.iteritems():
            if label in parser.args.binning:
                if label is not 'fix': self.binfix = self.args.binning
                return binning(parser.args.binning,self.base)
        

 

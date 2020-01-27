from ROOT import *
import sys
import os
import re
import mergeFiles as merge
from Parser import parser
from Process import Process
from Nuisance import Nuisance
from utilities import *
from samplenames import samplenames
from changeBinning import b_info

DataFileMap = {
    "SignalRegion":"postMETdata",
    "SingleEleCR":"postSingleEle",
    "SingleMuCR":"postSingleMu",
    "DoubleEleCR":"postDoubleEle",
    "DoubleMuCR":"postDoubleMu",
    "GammaCR":"postGamma"
}

parser.add_argument("-r","--reset",help="removes all post files from currently directory and rehadds them from the .output directory",action="store_true", default=False)
parser.add_argument("-l","--lumi",help="set the luminosity for scaling",action="store",type=float,dest="lumi")
parser.add_argument("-s","--signal",help="specify the signal file to use",action="store",type=str,default=None,dest="signal")
parser.add_argument("-b","--binning",help="specify function for rebinning histogram",action="store",type=str,default=None)
parser.add_argument("--nhists",help="Plot all 1D plots at nhists level",type=int)
parser.add_argument("--mc-solid",help="Make MC solid color",action="store_true",default=False)
parser.add_argument("-d","--directory",help="Specify directory to get post files from",type=valid_directory)
parser.add_argument("-c","--cut",help="Specify cut on branch variable using TTree string",type=lambda arg:str(arg).replace('"','').replace("'",""),default=None)
parser.add_argument("-e","--era",help="Specify the eras to use",type=lambda arg:sorted(arg.upper()),default=None)
parser.add_argument("-a","--autovar",help="Specify to use the automatic basic nhist",action="store_true",default=False)
parser.add_argument("--normalize",help="Specify to normalize plots to unity",action="store_true",default=False)
parser.add_argument("-w","--weight",help="Specify the weight to use for branch variables",type=str,default="weight")
parser.add_argument("--no-width",help="Disable bin width scaling",action="store_true",default=False)
parser.add_argument("--nlo",help="Use all available NLO samples",action="store_true",default=False)

class Region(object):
    def __init__(self,year=None,region=None,lumi=None,path=None,config=None,autovar=False,useMaxLumi=False,copy=None,show=True):
        if copy is not None: self.copy(copy); return
        self.isOpen = False
        self.args = parser.parse_args()
        self.year = year; self.region = region; self.lumi = lumi; self.path = path; self.show = show
        self.cwd = os.getcwd()

        self.setPath()

        if config is None: import config
        self.config = config
        if self.year is None: self.year = config.version
        self.xsec = config.xsec

        if self.region is None: self.region = GetRegion()
        self.lumimap = config.lumi_by_era[self.region]
        if self.lumi is None: self.lumi = config.lumi[self.region]
        if self.args.lumi is not None: self.lumi = self.args.lumi
        if self.args.era is not None:
            self.lumimap = { era:self.lumimap[era] for era in self.args.era }
            self.lumi = sum(self.lumimap.values())
        if useMaxLumi: self.lumi = max( config.lumi.values() )
            
        self.lumi_label = '%s' % float('%.3g' % (self.lumi/1000.)) + " fb^{-1}"
        if (self.args.normalize): self.lumi_label="Normalized"

        if autovar: self.nhist = config.regions[self.region]
        if self.args.autovar: self.nhist = config.regions[self.region]

        self.MCList = []
        for mc in config.mclist:
            if self.args.nlo and mc in config.nlomap:
                self.MCList.append(config.nlomap[mc])
            else: self.MCList.append(mc)
        self.SampleList = ["Data"] + self.MCList

        self.processes = {}
        datafile = DataFileMap[self.region]
        datalist = [ '%s_%s' % (datafile,era) for era in sorted(self.lumimap.keys()) ]
        self.processes["Data"] =    Process("Data",datalist,None,'data',year=self.year,region=self.region,args=self.args)
        for mc in self.MCList: self.processes[mc] = Process(mc,config.filemap[mc],GetMCxsec(config.filemap[mc],config.xsec),'bkg',
                                                              lumi=self.lumi,leg=config.legmap[mc],color=config.colmap[mc],
                                                              year=self.year,region=self.region,args=self.args,config=self.config)
        if self.region == "SignalRegion" and self.args.signal != None:
            self.setSignalInfo()
        if self.show:
            print "Running in "+self.region+":"
            print "Plotting at",self.lumi,"pb^{-1}"
        self.haddFiles()
        if os.getcwd() != self.cwd: os.chdir(self.cwd)
    def __len__(self): return len(self.SampleList)
    def __getitem__(self,i):
        if type(i) == str: key = i
        if type(i) == int: key = self.SampleList[i];
        return self.processes[key]
    def __iter__(self):
        for i in range(len(self)): yield self[i]
    def __contains__(self,procname): return procname in self.processes
    def haddFiles(self):
        if os.getcwd() != self.path: os.chdir(self.path)
        if not os.path.isdir('.output/'): return
        def validfile(fname): return os.path.isfile(fname)
        filelist = []
        for process in self: filelist += [ filename for filename in process.filenames if not validfile(filename+'.root') ]
        filelist += [ filename for filename in self.xsec if filename not in filelist and not validfile(filename+'.root') ]
        merge.HaddFiles(filelist)
        if os.getcwd() != self.cwd: os.chdir(self.cwd)
    def setPath(self):
        if self.path is None: self.path = self.cwd
        if self.args.directory is not None: self.path = self.args.directory
        os.chdir(self.path); self.path = os.getcwd()
        hasLocalFiles = any( re.search('post.*\.root',fname) for fname in os.listdir('.') )
        hasOutputFiles = os.path.isdir('.output') and any( re.search('post.*\.root',fname) for fname in os.listdir('.output') )
        if os.path.isfile('postpath.txt') and not (hasLocalFiles or hasOutputFiles):
            with open('postpath.txt') as f: self.path = f.read().strip()
            os.chdir(self.path); self.path = os.getcwd()
        print 'Using %s' % self.path
    def setSignalInfo(self,scale=1):
        from monoZprime_XS import centralxsec as signalxsec
        self.SignalList = [ ]
        if self.args.signal == '-1':
            self.signal = 'Signal'
            for signal in signalxsec:
                self.SignalList.append(signal)
                fname = 'post'+signal
                xsecmap = { fname:signalxsec[signal]}
                self.processes[signal] = Process(signal,[fname],xsecmap,'signal',lumi=self.lumi,year=self.year,region=self.region,args=self.args)
                self.SampleList.insert(1,signal)
        elif IsSignal(self.args.signal):
            self.signal = self.args.signal
            self.SignalList.append(self.signal)
            fname = 'post'+self.signal
            xsecmap = {fname:signalxsec[self.signal]}
            self.processes[self.signal] = Process(self.signal,[fname],xsecmap,'signal',lumi=self.lumi,year=self.year,region=self.region,args=self.args)
            self.SampleList.insert(1,self.signal)
    def initVariable(self,variable=None,weight=None):
        b_info.initVariable()
        Nuisance.unclist = []
        self.bkgIntegral = 0
        if 'SumOfBkg' in self.processes:
            tmp = self.processes.pop('SumOfBkg')
            del tmp
        if variable is not None:
            self.varname = variable
            self.cut = self.args.cut
            self.weight = weight
            if self.weight is None: self.weight = self.args.weight
            self.setBinning(variable,self.weight,self.cut)
    def open(self):
        if self.isOpen: return
        self.isOpen = True
        proclist = self.processes.keys()
        for process in proclist:
            if not self[process].open():
                self.processes.pop(process)
                if process in self.SampleList: self.SampleList.remove(process)
                if process in self.MCList: self.MCList.remove(process)
                if hasattr(self,'SignalList') and process in self.SignalList: self.SignalList.remove(process)
    def initiate(self,variable,weight=None):
        if os.getcwd() != self.path: os.chdir(self.path)
        self.initVariable(variable,weight)
        self.open()
        if hasattr(self,'nhist'): variable = '%s_%s' % (variable,self.nhist)
        for process in self:
            process.setVariable(variable,b_info.template,b_info.weight,b_info.cut)
            self.scaleWidth = process.scaleWidth
        self.setXaxisTitle(variable)
        if self.show: self.output()
        if os.getcwd() != self.cwd: os.chdir(self.cwd)
    def output(self):
        if self.scaleWidth: print "Bin Width Normalization"
        prompt = 'integral of %s: %s'
        ntemp = '{0:<15}'; itemp = '{0:<8}'
        print prompt % ( ntemp.format('Data'),itemp.format( '%.6g' % self.processes['Data'].scaled_total ) )
        if hasattr(self,'signal'):
            for signal in self.SignalList:
                signal = self[signal]
                print prompt % ( ntemp.format(signal.process),itemp.format( '%.6g' % signal.scaled_total ) )
        BkgIntegral = sum( process.scaled_total for name,process in self.processes.iteritems() if process.proctype == 'bkg' )
        print prompt % ( ntemp.format('SumOfBkg'),itemp.format( '%.6g' % BkgIntegral ) )
        for sample in sorted(self.MCList,key=lambda sample: self.processes[sample].scaled_total,reverse=True):
            process = self.processes[sample]
            percent = ("%.4g%%" % (100*process.scaled_total/BkgIntegral)) if BkgIntegral != 0 else 'Nan'
            print prompt % ( ntemp.format(sample),itemp.format( '%.6g' % process.scaled_total ) ),'| %s' % (percent)
        ratio = ('%.6g' % (self.processes['Data'].scaled_total/BkgIntegral)) if BkgIntegral != 0 else 'Nan'
        print '            %s: %s' % (ntemp.format('data/mc'),itemp.format(ratio))
    def setSumOfBkg(self):
        sumofbkg = Process('SumOfBkg',[],{},'sumofbkg',year=self.year,region=self.region)
        for process in self:
            if process.proctype == 'bkg':
                sumofbkg.add(process)
        self.processes['SumOfBkg'] = sumofbkg
    def setXaxisTitle(self,variable):
        self.name = 'Xaxis Title'
        for title in samplenames:
            if title in variable:
                self.name = samplenames[title];
            key = variable.split("_")[-2]
            if key == title:
                self.name = samplenames[title];
                break
        if self.name == 'Xaxis Title':
            for process in self:
                for subprocess in process:
                    self.name = subprocess.histo.GetXaxis().GetTitle()
                    break
    def setBinning(self,variable,weight,cut):
        _,ndir = GetDirname(variable)
        if ndir is None: ndir = ''
        else: ndir = '_%s' % ndir
        b_variable = variable.replace(ndir,'')
        b_info.cut = cut
        b_info.weight = weight
        self.varname = b_variable
        if cut is not None:
            cutvar = cut.replace('>','?').replace('<','?').split('?')[0]
            if cutvar in variable: self.varname += cut.replace(cutvar,'').replace('<','-').replace('>','+')
            else: self.varname  += cut.replace('<','-').replace('>','+')
        self.varname += ndir
        if self.args.binning == None: return
        for label,binning in b_info.binninglist.iteritems():
            if label in self.args.binning:
                self.varname += '_'+self.args.binning
                binning(self.args.binning,self,b_variable) 
    def addUnc(self,nuisance,show=True):
        for process in self: process.addUnc(nuisance,show=show)
    def fullUnc(self,unclist=None,stat=False,show=None):
        if show is None: show = self.show
        if unclist is None: unclist = Nuisance.unclist
        unclist = list(unclist)
        if not stat and 'Stat' in unclist: unclist.remove('Stat')
        elif stat and 'Stat' not in unclist: unclist.append('Stat')
        self.unclist = unclist
        for process in self: process.fullUnc(unclist,stat,show=show)
        self.setSumOfBkg()
        print self["SumOfBkg"].nuisances["Total"]
    def getUncBand(self,unclist=None,stat=False):
        if not hasattr(self,'unclist'): self.fullUnc(unclist,stat)
        elif sorted(unclist) != sorted(self.unclist): self.fullUnc(unclist,stat)
        data = self['Data'].histo
        up,dn = self['SumOfBkg'].nuisances['Total'].GetHistos()
        rup = GetRatio(data,up); rdn = GetRatio(data,dn)
        uncband = GetUncBand(rup,rdn,norm=1)
        return uncband
    def copy(self,other):
        self.args = other.args; self.year = other.year; self.region = other.region; self.lumi = other.lumi; self.path = other.path; self.show = other.show
        self.MCList = list(other.MCList); self.SampleList = list(other.SampleList);
        if hasattr(other,'signal'):
            self.signal = other.signal
            self.SignalList = list(other.SignalList)
        self.processes = { name:Process(copy=process) for name,process in other.processes.iteritems() }
        self.scaleWidth = other.scaleWidth
        self.name = other.name
        self.varname = other.varname
        self.cut = other.cut
    def add(self,other):
        # if self.variable != other.variable: raise ValueError("%s is not %s" % (self.variable,other.variable))
        samplelist = self.processes.keys()
        self.lumi += other.lumi
        for sample in other.processes.keys():
            if sample not in samplelist: samplelist.append(sample)
        for sample in samplelist:
            if sample in self.processes and sample in other.processes:
                self.processes[sample].add(other.processes[sample])
            if sample not in self.processes and sample in other.processes:
                self.processes[sample] = Process(copy=other.processes[sample])

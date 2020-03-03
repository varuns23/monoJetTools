# from ROOT import *
import mergeFiles as merge
import re
import os
from Parser import parser
from Process import Process
from Nuisance import Nuisance
from utilities import *
from samplenames import samplenames
from VariableInfo import *
import copy

DataFileMap = {
    "SignalRegion":"postMETdata",
    "SingleEleCR":"postSingleEle",
    "SingleMuCR":"postSingleMu",
    "DoubleEleCR":"postDoubleEle",
    "DoubleMuCR":"postDoubleMu",
    "GammaCR":"postGamma"
}

MCOrderMap = {
    "SignalRegion":[
        "ZJets","WJets","GJets","DiBoson","TTJets","QCD","DYJets"
    ],
    "SingleEleCR":[
        "WJets","TTJets","DiBoson","GJets","QCD","DYJets","ZJets"
    ],
    "SingleMuCR":[
        "WJets","TTJets","QCD","DYJets","DiBoson","GJets","ZJets"
    ],
    "DoubleEleCR":[
        "DYJets","DiBoson","TTJets","WJets","QCD","GJets","ZJets"
    ],
    "DoubleMuCR":[
        "DYJets","DiBoson","TTJets","WJets","QCD","GJets","ZJets"
    ],
    "GammaCR":[
        "GJets","QCD","WJets","DiBoson","TTJets","DYJets","ZJets"
    ]
}

parser.add_argument("-r","--reset",help="removes all post files from currently directory and rehadds them from the .output directory",action="store_true", default=False)
parser.add_argument("-l","--lumi",help="set the luminosity for scaling",action="store",type=float,dest="lumi")
parser.add_argument("-s","--signal",help="specify the signal file to use",action="store",type=str,default=None,dest="signal")
parser.add_argument("--nhists",help="Plot all 1D plots at nhists level",type=int,nargs='?',const=-1)
parser.add_argument("--mc-solid",help="Make MC solid color",action="store_true",default=False)
parser.add_argument("-d","--directory",help="Specify directory to get post files from",type=valid_directory)
parser.add_argument("-c","--cut",help="Specify cut on branch variable using TTree string",type=lambda arg:str(arg).replace('"','').replace("'",""),default=None)
parser.add_argument("-e","--era",help="Specify the eras to use",type=lambda arg:sorted(arg.upper()),default=None)
parser.add_argument("-a","--autovar",help="Specify to use the automatic basic nhist",action="store_true",default=False)
parser.add_argument("--normalize",help="Specify to normalize plots to unity",action="store_true",default=False)
parser.add_argument("-w","--weight",help="Specify the weight to use for branch variables",type=str,default="weight")
parser.add_argument("--nlo",help="Use all available NLO samples",action="store_true",default=False)
parser.add_argument("--postpath",help="Force path to come from postpath.txt",action="store_true",default=False)

class Region(object):
    def __init__(self,year=None,region=None,lumi=None,path=None,config=None,autovar=False,useMaxLumi=False,show=True):
        parser.parse_args()
        self.year = year; self.region = region; self.show = show
        self.setPath(path)
        self.setConfig(config)
        if self.region is None: self.region = GetRegion()
        self.setLumi(lumi,useMaxLumi)
        
        self.autovar = autovar
        if parser.args.autovar: self.autovar = True

        self.isBlinded = False

        self.MCList = []
        for mc in self.config.mclist:
            if parser.args.nlo and mc in self.config.nlomap:
                self.MCList.append(self.config.nlomap[mc])
            else: self.MCList.append(mc)
        self.SampleList = ["Data"] + self.MCList
        self.processes = {}
        datafile = DataFileMap[self.region]
        if 'Ele' in self.region and self.year == '2017':
            datalist = []
            for type in ("SE","SP"):
                for era in sorted(self.lumimap.keys()):
                    datalist.append( '%s_%s_%s' % (datafile,type,era))
        else:
            datalist = [ '%s_%s' % (datafile,era) for era in sorted(self.lumimap.keys()) ]
        self.processes["Data"] =    Process("Data",datalist,None,'data',year=self.year,region=self.region)
        for mc in self.MCList: self.processes[mc] = Process(mc,self.config.filemap[mc],GetMCxsec(self.config.filemap[mc],self.config.xsec),'bkg',
                                                            leg=self.config.legmap[mc],color=self.config.colmap[mc],year=self.year,region=self.region)
        if self.region == "SignalRegion" and parser.args.signal != None:
            self.setSignalInfo()
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
    def setPath(self,path=None):
        self.cwd = os.getcwd()
        self.path = path
        if self.path is None: self.path = self.cwd
        if parser.args.directory is not None: self.path = parser.args.directory
        os.chdir(self.path)
        hasLocalFiles = any( re.search('post.*\.root',fname) for fname in os.listdir('.') )
        hasOutputFiles = os.path.isdir('.output') and any( re.search('post.*\.root',fname) for fname in os.listdir('.output') )
        hasLocal = hasLocalFiles and hasOutputFiles
        if os.path.isfile('postpath.txt') and not hasLocal:
            with open('postpath.txt') as f: self.path = f.read().strip()
            os.chdir(self.path);
        self.path = os.getcwd()
        print 'Using %s' % self.path
    def setConfig(self,config=None):
        if config is None: import config
        self.config = config
        if self.year is None: self.year = config.version
        self.xsec = config.xsec
    def setLumi(self,lumi=None,useMaxLumi=False):
        self.lumi = lumi
        self.lumimap = self.config.lumi_by_era[self.region]
        if self.lumi is None: self.lumi = self.config.lumi[self.region]
        if parser.args.lumi is not None: self.lumi = parser.args.lumi
        if parser.args.era is not None:
            self.lumimap = { era:self.lumimap[era] for era in parser.args.era }
            self.lumi = sum(self.lumimap.values())
        self.max_lumi = max( self.config.lumi.values() )
        if useMaxLumi: self.lumi = self.max_lumi
        
        self.lumi_label = '%s' % float('%.3g' % (self.lumi/1000.)) + " fb^{-1}"
        if (parser.args.normalize): self.lumi_label="Normalized"
    def setSignalInfo(self,scale=1):
        from monoJet_XS import centralxsec as signalxsec
        self.SignalList = [ ]
        if parser.args.signal == '-1':
            self.signal = 'Signal'
            for signal in signalxsec:
                self.SignalList.append(signal)
                fname = 'post'+signal
                xsecmap = { fname:signalxsec[signal]}
                self.processes[signal] = Process(signal,[fname],xsecmap,'signal',lumi=self.lumi,year=self.year,region=self.region,args=parser.args)
                self.SampleList.insert(1,signal)
        elif IsSignal(parser.args.signal):
            self.signal = parser.args.signal
            self.SignalList.append(self.signal)
            fname = 'post'+self.signal
            xsecmap = {fname:signalxsec[self.signal]}
            self.processes[self.signal] = Process(self.signal,[fname],xsecmap,'signal',lumi=self.lumi,year=self.year,region=self.region,args=parser.args)
            self.SampleList.insert(1,self.signal)
    def open(self):
        if hasattr(self,'isOpen'): return
        self.isOpen = True

        proclist = self.processes.keys()
        for process in proclist:
            if not self[process].open(self.config):
                if self[process].proctype == 'data':
                    print 'Blinded: Setting data as SumOfBkg'
                    self.isBlinded = True
                self.processes.pop(process)
                if process in self.SampleList: self.SampleList.remove(process)
                if process in self.MCList: self.MCList.remove(process)
                if hasattr(self,'SignalList') and process in self.SignalList: self.SignalList.remove(process)
        if self.isBlinded: self.setLumi(self.max_lumi)
    def initVariable(self,variable,weight,cut):
        if not hasattr(self,'first_init'):
            self.first_init = True
            tfile = next( process[0].tfile for process in self if process[0].tfile is not None )
            self.variable = VariableInfo(tfile)
            
            if self.show:
                print "Running in "+self.region+":"
                print "Plotting at",self.lumi,"pb^{-1}"

            
        self.total_bkg = 0
        if 'SumOfBkg' in self.processes:
            tmp = self.processes.pop('SumOfBkg')
            del tmp
        self.variable.setVariable(variable,weight,cut,autovar=self.autovar)
        self.scaleWidth = self.variable
        self.varname = self.variable.variable
        if self.autovar: self.varname = self.variable.base
        if hasattr(self.variable,'cutfix'): self.varname += self.variable.cutfix
        if hasattr(self.variable,'binfix'): self.varname += '_'+self.variable.binfix
    def initiate(self,variable,weight='weight',cut=None):
        if os.getcwd() != self.path: os.chdir(self.path)
        self.open()
        self.initVariable(variable,weight,cut)
        variable = self.variable
        for process in self:
            if self.isBlinded and process.proctype == 'data': continue
            process.setVariable(variable,self.lumi)
            if process.proctype == 'bkg':
                self.total_bkg += process.scaled_total
        self.setMCOrder()
        self.setXaxisTitle(variable)
        if self.isBlinded:
            self.setSumOfBkg()
            self.processes['Data'] = copy.deepcopy(self['SumOfBkg'])
            self['Data'].proctype = 'bkg'
        if self.show: self.output()
        if os.getcwd() != self.cwd: os.chdir(self.cwd)
    def setMCOrder(self):
        def mcsort(process):
            if 'cutflow' in variable.variable:
                return self[process].histo.GetBinContent(self[process].histo.GetNbinsX())
            return self[process].scaled_total
        if self.region not in MCOrderMap:
            self.MCOrder = [ process.process for process in self if process.proctype == 'bkg']
            self.MCOrder.sort(key=mcsort,reverse=True)
        else: self.MCOrder = MCOrderMap[self.region]
    def output(self):
        if self.scaleWidth: print "Bin Width Normalization"
        prompt = 'integral of %s: %s'
        ntemp = '{0:<15}'; itemp = '{0:<8}'
        print prompt % ( ntemp.format('Data'),itemp.format( '%.6g' % self.processes['Data'].scaled_total ) )
        if hasattr(self,'signal'):
            for signal in self.SignalList:
                signal = self[signal]
                print prompt % ( ntemp.format(signal.process),itemp.format( '%.6g' % signal.scaled_total ) )
        print prompt % ( ntemp.format('SumOfBkg'),itemp.format( '%.6g' % self.total_bkg ) )
        for sample in self.MCOrder:
            process = self.processes[sample]
            percent = ("%.4g%%" % (100*process.scaled_total/self.total_bkg)) if self.total_bkg != 0 else 'Nan'
            print prompt % ( ntemp.format(sample),itemp.format( '%.6g' % process.scaled_total ) ),'| %s' % (percent)
        ratio = ('%.6g' % (self.processes['Data'].scaled_total/self.total_bkg)) if self.total_bkg != 0 else 'Nan'
        print '            %s: %s' % (ntemp.format('data/mc'),itemp.format(ratio))
    def setSumOfBkg(self):
        sumofbkg = Process('SumOfBkg',[],{},'sumofbkg',year=self.year,region=self.region)
        for process in self:
            if process.proctype == 'bkg':
                sumofbkg.add(process)
        self.processes['SumOfBkg'] = sumofbkg
    def setXaxisTitle(self,variable):
        self.name = None
        for title in samplenames:
            if title in variable.variable:
                self.name = samplenames[title];
            key = variable.variable.split("_")[-2]
            if key == title:
                self.name = samplenames[title];
                break
        if self.name == None:
            for process in self:
                for subprocess in process:
                    self.name = subprocess.histo.GetXaxis().GetTitle()
                    break
    def addUnc(self,nuisance):
        for process in self: process.addUnc(nuisance)
    def fullUnc(self,unclist,show=False):
        for process in self: process.fullUnc(unclist,show)
        self.setSumOfBkg()
        for nuisance in ['Total']:
            up = self['SumOfBkg'].histo.Clone('%s_%s_TotalUp' % (self['SumOfBkg'].name,self.variable.base));  up.Reset()
            dn = self['SumOfBkg'].histo.Clone('%s_%s_TotalDown' % (self['SumOfBkg'].name,self.variable.base)); dn.Reset()
            AddDiffNuisances([process.nuisances[nuisance] for process in self if process.proctype == 'bkg'],up,dn,self['SumOfBkg'].histo)
            self['SumOfBkg'].nuisances[nuisance] = Nuisance('SumOfBkg',nuisance,up,dn,self['SumOfBkg'].histo)
        if show: print self['SumOfBkg'].nuisances['Total']
    def getUncBand(self,unclist):
        self.fullUnc(unclist)
        data = self['Data'].histo
        up,dn = self['SumOfBkg'].nuisances['Total'].GetHistos()
        rup = GetRatio(data,up); rdn = GetRatio(data,dn)
        uncband = GetUncBand(rup,rdn,norm=1)
        return uncband
    def add(self,other,addlumi=False):
        samplelist = self.processes.keys()
        if addlumi: self.lumi += other.lumi
        for sample in other.processes.keys():
            if sample not in samplelist: samplelist.append(sample)
        for sample in samplelist:
            if sample in self.processes and sample in other.processes:
                self.processes[sample].add(other.processes[sample])
            if sample not in self.processes and sample in other.processes:
                self.processes[sample] = copy.deepcopy(other.processes[sample])

if __name__ == "__main__":
    FindConfig()
    sample = Region(autovar=True)
    sample.initiate('recoil')

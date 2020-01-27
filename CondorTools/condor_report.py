#!/usr/bin/env python
import sys
sys.path.append("/cms/sw/farmout")
import CondorUserLog as cul
import os
from argparse import ArgumentParser

cmssw = os.getenv("CMSSW_BASE")
cwd = os.getcwd()
years = ["2016","2017","2018"]
regions = ["SignalRegion","SingleEleCR","SingleMuCR","DoubleEleCR","DoubleMuCR"]

def getStatusDirs(cwd):
    def helper(cwd,statusmap):
        for dir in os.listdir(cwd):
            if dir in years:
                statusmap[dir] = {}
                helper( os.path.join(cwd,dir),statusmap[dir] )
            if dir in regions:
                statusmap[dir] = {}
                helper( os.path.join(cwd,dir),statusmap[dir] )
            if dir == '.status':
                statusmap[dir] = os.path.join(cwd,'.status')
    statusmap = {}
    helper(cwd,statusmap)
    return statusmap
def getLogfiles(statusdirs):
    def helper(statusdirs,logmap):
        for dir,path in statusdirs.iteritems():
            if dir != '.status': logmap[dir] = {}; helper(path,logmap[dir]); continue
            for statusdir in os.listdir(path):
                cwd = os.path.join(path,statusdir)
                logmap[statusdir] = [ os.path.join(cwd,logfile) for logfile in os.listdir(cwd) if logfile.endswith('.log') ]
    logmap = {}
    helper(statusdirs,logmap)
    return logmap

def section(string): return string.center(100,'-')

class Stats(cul.CondorUserLogStats):
    pending_jobs = 0
    def printOut(self,label,verbose=0):
        self.total_jobs = self.pending_jobs + self.good_jobs + self.bad_jobs
        verbose_map = {
            "-2":self.total_jobs > 0 and self.pending_jobs == 0,
            "-1":self.total_jobs > 0 and self.pending_jobs < self.total_jobs,
            "0" :self.total_jobs > 0 and self.pending_jobs <= self.total_jobs,
            "1" :True
        }
        if not verbose_map[str(verbose)]: return
        if label is not None: print section(label)
        print self
        print
        print "          Pending Jobs: %s" % self.pending_jobs
        print "          Total Jobs:   %s" % self.total_jobs
        if self.good_jobs == self.total_jobs:
            print "          Finished"

def GetStats(logmap):
    stats = Stats()
    def helper(logmap):
        for dir,path in logmap.iteritems():
            if type(path) != list: helper(path); continue
            for i,ulog in enumerate(path):
                if type(ulog) == str: path[i] = cul.ReadCondorUserLog(ulog); ulog = path[i]
                stats.add(ulog)
                if ulog.events[-1].event_type != cul.TERMINATED_EVENT:
                    stats.pending_jobs += 1
    helper(logmap)
    return stats

def GetAll(logmap,verbose):
    GetStats(logmap).printOut("All",verbose)

def GetByRegion(logmap,verbose):
    def helper(logmap,year=''):
        keylist = logmap.keys()
        if any(key in years for key in keylist):
            keylist.sort(key=lambda v: years.index(v))
        if any(key in regions for key in keylist):
            keylist.sort(key=lambda v: regions.index(v))
        for dir in keylist:
            path = logmap[dir]
            if type(path) == list: GetStats(logmap).printOut(None,verbose); return
            if dir in years: helper(path,year=dir); continue
            label = dir if year == '' else ' '.join( [year,dir] )
            GetStats(path).printOut(label,verbose)
    helper(logmap)

def GetByYear(logmap,verbose):
    keylist = logmap.keys()
    keylist.sort(key=lambda v: years.index(v))
    for dir in keylist:
        path = logmap[dir]
        if dir not in years: GetStats(logmap).printOut(None,verbose); return
        GetStats(path).printOut(dir)

def getargs():
    parser = ArgumentParser()
    parser.add_argument("-r","--region",help="Group results by region",action="store_true",default=False)
    parser.add_argument("-y","--year",help="Group results by year",action="store_true",default=False)
    parser.add_argument("-a","--all",help="Group all results together",action="store_true",default=False)
    parser.add_argument("-v","--verbose",help="Verbosity level (-2 = finished jobs; -1 = finished, and running jobs;" +
                        " 0 = finished, running, and pending jobs; 1 = finished, running, pending, and empty jobs)",action='store',choices=range(-2,2),type=int,default=0)
    return parser.parse_args()

if __name__ == "__main__":
    args = getargs()
    statusdirs = getStatusDirs(cwd)
    logmap = getLogfiles(statusdirs)

    if args.region: GetByRegion(logmap,verbose=args.verbose)
    if args.year:   GetByYear(logmap,verbose=args.verbose)
    if args.all:    GetAll(logmap,verbose=args.verbose)
    if not args.region and not args.year and not args.all:
        GetByRegion(logmap,verbose=args.verbose)
    

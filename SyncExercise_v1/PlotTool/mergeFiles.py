from os import listdir,path,mkdir
import sys
from subprocess import Popen,PIPE,STDOUT
from time import sleep

def singleThread(AllFiles,show=False):
    #Hadd files together
    for fn in AllFiles:
        nfile = [f for f in listdir(".output/") if fn+"_" in f]
        if len(nfile) != 0:
            arg = ["hadd","-f",fn+".root"]
            for f in nfile:arg.append(".output/"+f)
            proc = Popen(arg,stdout=PIPE,stderr=STDOUT)
            print proc.stderr
###################################

def multiThread(AllFiles,show=False):
    threads = {}
    nthreads = 0
    for fn in AllFiles:
        nfile = [f for f in listdir(".output/") if fn+"_" in f]
        if len(nfile) != 0:
            arg = ["hadd","-f",fn+".root"]
            for f in nfile:arg.append(".output/"+f)
            ID = str(len(threads))
            threads[ID]=Popen(arg,stdout=PIPE,stderr=STDOUT)
            nthreads += 1
            sys.stdout.write("\r"+str(nthreads)+" Started Threads")
            sys.stdout.flush()
    if len(threads) != 0: print
    merging = nthreads != 0 
    out = "\r"+str(nthreads)+" Threads Remaining"
    dthreads = []
    while any(threads):
        IDlist = threads.keys(); IDlist.sort(key=int)
        for ID in IDlist:
            if threads[ID].poll() != None:
                threads.pop(ID)
        if len(threads) != nthreads:
            nthreads = len(threads)
            out = "\r"+str(nthreads)+" Threads Remaining"
            if out != None and len(threads) != 0:
                sys.stdout.write(out)
                sys.stdout.flush()
                out = None
    if merging: print "\nFiles Merged"
###################################
def mergeData(dataFiles,eramap,show=False):
    if not any(dataFiles): return
    if type(dataFiles) == str: final = dataFiles
    else:                      final = dataFiles[0].split('_')[0]
    arg = ["hadd",final+".root"]
    if not path.isdir("DataEra/"): mkdir("DataEra/")
    def mvEraFiles():
        for i,e in enumerate(sorted(eramap.keys())):
            i_fname = final+"_"+str(i)+".root"
            o_fname = "DataEra/"+final+"_"+e+".root"
            if not path.isfile(i_fname): continue
            proc = Popen(["mv",i_fname,o_fname])
            proc.wait()
    ################################################
    if eramap != None and type(dataFiles) == list: mvEraFiles()
    arg += [ 'DataEra/'+fname for fname in listdir('DataEra') ]
    if show: print "Merging Data"
    proc = Popen(arg)
    proc.wait()
###################################

def HaddFiles(datafiles,mcfiles,eramap=None,single=False,show=False):
    if type(datafiles) == str: haddfiles = mcfiles
    else: haddfiles = datafiles + mcfiles
    if (single): singleThread(haddfiles,show=show)
    else:multiThread(haddfiles,show=show)
    mergeData(datafiles,eramap,show=show)
###############################################################################################################
    

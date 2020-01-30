import os
import sys
from subprocess import Popen,PIPE,STDOUT
from time import sleep
from Parser import parser
import re

parser.add_argument("--single",help="hadd files using a single thread, instead of multiple",action="store_true",default=False)
parser.add_argument("--nohadd",help="does not try to hadd files together",action="store_true",default=False)

def progress(current,total,prompt):
    percent = 100 * (total - current)/float(total)
    out = '\r%s : %.3f%%' % (prompt,percent)
    sys.stdout.write(out)
    sys.stdout.flush()

def singleThread(AllFiles,show=False):
    #Hadd files together
    for fn in AllFiles:
        nfile = [f for f in os.listdir(".output/") if fn+"_" in f]
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
        nfile = [f for f in os.listdir(".output/") if fn+"_" in f]
        if len(nfile) != 0:
            arg = ["hadd","-f",fn+".root"]
            for f in nfile:arg.append(".output/"+f)
            ID = str(len(threads))
            threads[ID]=Popen(arg,stdout=PIPE,stderr=STDOUT)
            nthreads += 1
            sys.stdout.write("\r"+str(nthreads)+" Started Threads")
            sys.stdout.flush()
    merging = False
    total_threads = len(threads)
    if total_threads != 0:
        print
        merging = True
        progress(total_threads,total_threads,'Threads Complete')
    while any(threads):
        IDlist = threads.keys(); IDlist.sort(key=int)
        for ID in IDlist:
            if threads[ID].poll() != None:
                threads.pop(ID)
        if len(threads) != nthreads:
            nthreads = len(threads)
            progress(nthreads,total_threads,'Threads Complete')
    if merging:
        progress(0,total_threads,'Threads Complete')
        print "\nFiles Merged"
###################################
def mergeData(datafiles,eralist,show=False):
    if not any(datafiles): return
    def checkfile(fname):
        if os.path.isfile(fname+'.root'):
            marked = re.findall(r'_\d*$',fname)
            if any(marked):
                nera = int(marked[0].split('_')[1])
                new_fname = fname.replace(marked[0],'_%s' % eralist[nera])
                os.rename(fname+'.root',new_fname+'.root')
                fname = new_fname
            return fname
        return None
    filelist = []
    for fname in datafiles:
        fname = checkfile(fname)
        if fname is not None and fname not in filelist: filelist.append(fname+'.root')
    if any(filelist):
        output = filelist[0].split('_')[0]
        command = ['hadd','-f','%s.root' % output ] + filelist
        os.system(' '.join(command))
###################################

def HaddFiles(haddfiles,single=False,show=False):
    if (single): singleThread(haddfiles,show=show)
    else:multiThread(haddfiles,show=show)
###############################################################################################################
    

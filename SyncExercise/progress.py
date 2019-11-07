import os
import subprocess
from sys import argv
from optparse import OptionParser

#user = 'ekoenig4'
user = 'varuns'
parser = OptionParser()
parser.add_option("-r","--resubmit",help="resubmit condor jobs that didn't produce output files",action="store_true",default=False)
parser.add_option("-v","--verbose",help="print all files being checked",action="store_true",default=False)

(options,args) = parser.parse_args()
cwd = os.getcwd()

state = 0

def printOutput(condor,status,incomplete,nfiles,space=3):
    global state
    if state == 0:
        print "NAME"+" "*(space),
        print "STATUS"+"   ",
        print "RUN"+"    ",
        print "IDLE"+"    ",
        print "HELD"+"    ",
        print "TOTAL"+"    ",
        print "FAIL"+"    ",
        print "FILES"
        state += 1
        printOutput(condor,status,incomplete,nfiles,space=space)
    elif state == 1:
        print condor+" "*(space+4-len(condor)),
        #STATUS
        if incomplete == 0: print "DONE  "+"   ",
        elif status['jobs'] == 0: print "FAIL  "+"   ",
        else: print "RUN   "+"   ",
        #RUN
        if incomplete == 0: print "-  "+"    ",
        elif status['jobs'] == 0: print "-  "+"    ",
        else: print str(status['running'])+" "*(7 - len(str(status['running']))),
        #IDLE
        if incomplete == 0: print "-  "+"     ",
        elif status['jobs'] == 0: print "-  "+"     ",
        else: print str(status['idle'])+" "*(8 - len(str(status['idle']))),
        #HELD
        if incomplete == 0: print "-  "+"     ",
        elif status['jobs'] == 0: print "-  "+"     ",
        else: print str(status['held'])+" "*(8 - len(str(status['held']))),
        #TOTAL
        if incomplete == 0: print "-  "+"      ",
        elif status['jobs'] == 0: print "-  "+"      ",
        else: print str(status['jobs'])+" "*(9 - len(str(status['jobs']))),
        #FAIL
        if incomplete == 0: print str(incomplete)+" "*(8 - len(str(incomplete))),
        elif status['jobs'] == 0: print str(incomplete)+" "*(8 - len(str(incomplete))),
        else: print "-  "+"     ",
        #FILES
        if incomplete == 0: print str(nfiles)+" "*(8 - len(str(nfiles)))
        elif status['jobs'] == 0: print str(nfiles)+" "*(8 - len(str(nfiles)))
        else: print "-  "+"     "
    elif state == 2:
        print "Total:",status['jobs'],"jobs;",status['completed'],"completed,",status['removed'],"removed,",
        print status['idle'],"idle,",status['running'],"running,",status['held'],"held,",status['suspended'],"suspended"

def check(dir):
    global state
    print dir
    os.chdir(dir+'/.output/')
    condor = [file for file in os.listdir('.') if 'condor_' in file]
    space = len(max(condor,key=len))
    condor.sort()
    finished = True
    total = {'jobs':0,'completed':0,'removed':0,'idle':0,'running':0,'held':0,'suspended':0}
    for i,file in enumerate(condor):
        with open(file,'r') as f:
            arglist = []
            for line in f.readlines():
                if 'Argument' in line: arglist.append(line)
                if 'Log'      in line: logdir = line.split()[2].split("$")[0]
        for f in os.listdir(logdir):
            if '.log' in f:
                logfile = logdir+'/'+f
                break
     varuns####################
        with open(logfile,'r') as f:
            try: cluster = f.readlines()[0].split()[1].split(".")[0].replace("(","")
            except IndexError: cluster = "-1"
        complete = True
        incomplete = 0
        for arg in arglist:
            output = arg.split()[3]
            if not os.path.isfile(output):
                # print "\t",output,"Not Found"
                incomplete += 1
                complete = False
                finished = False
        status = {'jobs':0,'completed':0,'removed':0,'idle':0,'running':0,'held':0,'suspended':0}
        if incomplete != 0:
            condor_q = subprocess.Popen(['condor_q','-s',user,cluster],stdout=subprocess.PIPE,stderr=subprocess.STDOUT)
            stdout, stderr = condor_q.communicate()
            info = [ line for line in stdout.split('\n') if "jobs;" in line ]
            if cluster != "-1":
                for server in info:
                    tmp = server.split()
                    status['jobs']      += int(tmp[0])
                    status['completed'] += int(tmp[2])
                    status['removed']   += int(tmp[4])
                    status['idle']      += int(tmp[6])
                    status['running']   += int(tmp[8])
                    status['held']      += int(tmp[10])
                    status['suspended'] += int(tmp[12])
            ##############################################
            for type in status: total[type] += status[type]
            printOutput(file,status,incomplete,len(arglist),space=space)
        elif options.verbose:
            printOutput(file,status,incomplete,len(arglist),space=space)
        if not complete and options.resubmit:
            for f in os.listdir(logdir): os.remove(logdir+'/'+f)
            os.system('condor_submit '+file)
    if finished: print "Finished."
    else:
        state += 1
        printOutput(None,total,None,None)
    os.chdir(cwd)
    print
###################################################################################################################

for dir in args: check(dir)

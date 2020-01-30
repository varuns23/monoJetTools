#!/usr/bin/env python
import os
import sys
cmssw = os.getenv("CMSSW_BASE")
repo = '%s/src/monoJetTools/' % cmssw
sys.path.append(repo)

from CondorTools.SubmitDataset import submit,options,mclist
options['year'] = '2017'
options['region'] = 'WM'
options['parallel'] = True
options['batchsize'] = 10
# options['submit'] = False
#----Submit---#
submit('met',sub='B',label='SingleMu_',filelist=True)
# submit('met',label='DoubleMu_',filelist=True)
# for mc in mclist: submit(mc)


#!/usr/bin/env python
import os
import sys
cmssw = os.getenv("CMSSW_BASE")
repo = '%s/src/monoJetTools/' % cmssw
sys.path.append(repo)

from CondorTools.SubmitDataset import submit,options,mclist,signalist
options['year'] = '2018'
options['region'] = 'SR'
options['parallel'] = False
options['batchsize'] = 100
# options['submit'] = False
#----Submit---#
submit('met',sub='A',filelist=True)
for signal in signalist: submit(signal,split=1)
for mc in mclist: submit(mc)


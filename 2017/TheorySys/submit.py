#!/usr/bin/env python
import os
import sys
cmssw = os.getenv("CMSSW_BASE")
repo = '%s/src/monoJetTools/' % cmssw
sys.path.append(repo)

from CondorTools.SubmitDataset import submit,options,mclist,signalist
options['year'] = '2017'
options['region'] = 'TS'
options['parallel'] = False
options['batchsize'] = 50
# options['submit'] = False
#----Submit---#
# submit('met',sub='B',filelist=True)
# for signal in signalist: submit(signal,split=1)
# for mc in mclist: submit(mc)
for mc in ("zjets","wjets","gjets"): submit(mc)

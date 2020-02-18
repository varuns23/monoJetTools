#!/usr/bin/env python
import os
import sys
cmssw = os.getenv("CMSSW_BASE")
repo = '%s/src/monoJetTools/' % cmssw
sys.path.append(repo)

from CondorTools.SubmitDataset import submit,options,mclist
options['year'] = '2018'
options['region'] = 'ZE'
options['parallel'] = True
options['batchsize'] = 100
# options['submit'] = False
#----Submit---#
#submit('egamma',label='DoubleEle_',filelist=True)
submit('st',sub='SyncEx',label='DoubleEle_',filelist=True)
#for mc in mclist: submit(mc)

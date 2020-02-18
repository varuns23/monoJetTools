#!/usr/bin/env python
import os
import sys
cmssw = os.getenv("CMSSW_BASE")
repo = '%s/src/monoJetTools/' % cmssw
sys.path.append(repo)

from CondorTools.SubmitDataset import submit,options,mclist
options['year'] = '2017'
options['region'] = 'WE'
options['parallel'] = True
options['batchsize'] = 150
# options['submit'] = False
#----Submit---#
submit('singleele',label='SingleEle_SE_',filelist=True)
submit('singlepho',label='SingleEle_SP_',filelist=True)
for mc in mclist: submit(mc)


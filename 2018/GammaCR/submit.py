#!/usr/bin/env python
import os
import sys
cmssw = os.getenv("CMSSW_BASE")
repo = '%s/src/monoJetTools/' % cmssw
sys.path.append(repo)

from CondorTools.SubmitDataset import submit,options,mclist
options['year'] = '2018'
options['region'] = 'GA'
options['parallel'] = True
options['batchsize'] = 150
# options['submit'] = False
#----Submit---#
submit('egamma',label='Gamma_')
for mc in mclist: submit(mc)

#!/usr/bin/env python
import os
import sys
cmssw = os.getenv("CMSSW_BASE")
repo = '%s/src/monoJetTools/' % cmssw
sys.path.append(repo)

from CondorTools.SubmitDataset import submit,options,mclist
options['year'] = '2017'
options['region'] = 'GA'
options['parallel'] = False
options['batchsize'] = 150
# options['submit'] = False
#----Submit---#
# submit('singlepho',label='Gamma_')
# for mc in mclist: submit(mc)
for mc in ("gjets","gjets_nlo"): submit(mc)

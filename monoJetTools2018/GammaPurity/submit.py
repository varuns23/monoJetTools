#!/usr/bin/env python
import os
import sys
cmssw = os.getenv("CMSSW_BASE")
repo = '%s/src/monojet/' % cmssw
sys.path.append(repo)

from CondorTools.SubmitDataset import submit,options,mclist
options['year'] = '2018'
options['region'] = 'GP'
options['parallel'] = True
options['batchsize'] = 100
# options['submit'] = False
#----Submit---#
# submit('egamma',label='postGamma_')
submit('gjets')
submit('qcd')


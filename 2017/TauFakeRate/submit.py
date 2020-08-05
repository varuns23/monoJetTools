#!/usr/bin/env python
import os
import sys
cmssw = os.getenv("CMSSW_BASE")
repo = '%s/src/monoJetTools/' % cmssw
sys.path.append(repo)

from CondorTools.SubmitDataset import submit,options,mclist,signalist
options['year'] = '2017'
options['region'] = 'TFR'
options['parallel'] = False
options['batchsize'] = 50
# options['submit'] = False
#----Submit---#
submit('mutau',filelist=True)
for mc in ("dyjets","ttjets","ewk","wjets"): submit(mc)

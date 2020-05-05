#!/usr/bin/env python
import os
import sys
cmssw = os.getenv("CMSSW_BASE")
repo = '%s/src/monoJetTools/' % cmssw
sys.path.append(repo)

from CondorTools.SubmitDataset import submit,options,mclist
options['year'] = '2017'
options['region'] = 'GA'
options['parallel'] = True
options['batchsize'] = 200
# options['submit'] = False
#----Submit---#
submit('singlepho',label='dataBkg_',filelist=True)
#submit('singlepho',sub='B',label='data_', filelist=True)
#submit('singlepho',sub='test',label='data_', filelist=True)
#for mc in mclist: submit(mc)

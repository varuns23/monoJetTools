import os
import sys
from multiprocessing import Process
import SubmitCondor
from dataset import getDataset
labelmap = { 'dyjets_nlo':'DYNLO','dyjets':'DY','ewk':'','gjets':'GJets','qcd':'QCD','st':'ST_','ttjets':'TTJets','wjets_nlo':'WNLO','wjets':'W','zjets_nlo':'ZNLO','zjets':'Z','met_crab':'METCrabData_','met':'METdata_','egamma':'EGdata_','singleele':'SEdata_','singlepho':'SPdata_','signal':'' }
mclist = ['dyjets_nlo','dyjets','ewk','gjets','qcd','st','ttjets','wjets_nlo','wjets','zjets_nlo','zjets']
datalist = ['met','egamma','singleele','singlepho']

options = {
    'year':None,
    'region':None,
    'parallel':False,
    'batchsize':20,
    'submit':True,
    'doData':True,
    'doMC':True
}
    
def submit(data,sub=None,label=None,split=-1,filelist=True,script='analyze'):
    if not options['doData'] and data in datalist: print 'Warning submitting %s. Data is disabled' % data; return
    if not options['doMC'] and data in mclist: print 'Warning submitting %s. MC is disabled' % data; return
    SubmitCondor.NFILE_PER_BATCH = options['batchsize']
    SubmitCondor.DoSubmit = options['submit']
    dataset = getDataset(data)
    if dataset == None: print '%s not found in dataset' % data; return
    if label is None: label = labelmap[data]
    subset = dataset[data]

    if sub is None: sublist = subset.keys()
    elif type(sub) != list: sublist = [sub]
    else: sublist = sub
    for sub in sublist:
        if sub not in subset: print '%s not found in subset' % sub; continue
        for i,input in enumerate(subset[sub]):
            clabel = '%s%s_%i' % (label,sub,i)
            nlabel = '%s%s_%i' % (labelmap[data],sub,i)
            command = [script,input,'post%s.root' % clabel,'-1','10000',nlabel,'split_%i' % split]
            if filelist: command = ['-f'] + command
            if options['region'] is not None: command = ['-r',options['region']] + command
            if options['year'] is not None: command = ['-y',options['year']] + command
            
            if options['parallel']:
                proc = Process(target=SubmitCondor.submit,args=(command,True))
                proc.start()
            else:
                SubmitCondor.submit( command )
    


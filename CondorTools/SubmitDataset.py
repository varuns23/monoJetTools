import os
import sys
from multiprocessing import Process
import SubmitCondor
from dataset import getDataset
from argparse import ArgumentParser,ArgumentTypeError
labelmap = { 'dyjets_nlo':'DYNLO','dyjets':'DY','ewk':'','gjets':'GJets','qcd':'QCD','st':'ST_','ttjets':'TTJets',
             'wjets_nlo':'WNLO','wjets':'W','zjets_nlo':'ZNLO','zjets':'Z',
             'met':'METdata_','egamma':'EGdata_','singleele':'SEdata_','singlepho':'SPdata_','signal':'' }
mclist = ['dyjets_nlo','dyjets','ewk','gjets','qcd','st','ttjets','wjets_nlo','wjets','zjets_nlo','zjets']
datalist = ['met','egamma','singleele','singlepho']

options = {
    'year':"",
    'region':"",
    'parallel':False,
    'batchsize':20,
    'submit':True,
    'data':True,
    'mc':True,
    'error':False
}
def getargs():
    parser = ArgumentParser()
    def parse_bool(v):
        iv = int(v)
        if iv == 1: return True;
        if iv == 0: return False
        raise ArgumentTypeError("Input binary option")
    for key,value in options.iteritems():
        if type(value) == bool: parse_type = parse_bool
        else: parse_type = type(value)
        parser.add_argument("-"+key[0],"--"+key,default=value,type=parse_type)
    for key,value in vars(parser.parse_args()).iteritems():
        options[key] = value
def submit(data,sub=None,label=None,split=-1,filelist=True,script='analyze'):
    getargs()
    if not options['data'] and data in datalist: print 'Warning submitting %s. Data is disabled' % data; return
    if not options['mc'] and data in mclist: print 'Warning submitting %s. MC is disabled' % data; return
    SubmitCondor.NFILE_PER_BATCH = options['batchsize']
    SubmitCondor.DoSubmit = options['submit']
    SubmitCondor.ResubmitError = options['error']
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
            if any(options['region']): command = ['-r',options['region']] + command
            if any(options['year']): command = ['-y',options['year']] + command
            
            if options['parallel']:
                proc = Process(target=SubmitCondor.submit,args=(command,True))
                proc.start()
            else:
                SubmitCondor.submit( command )
    


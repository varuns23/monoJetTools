import os
wd,wf = os.path.split(__file__)

def updirectory(path): path += '/../'; return path
repo_path = os.path.realpath( updirectory(wd) )

#--- Find Nutple Path ---#
def findpath(path):
    if any( directory == 'ntuples' for directory in os.listdir(path) ): return os.path.join(path,'ntuples')
    elif any( directory == 'datasets' for directory in os.listdir(path) ): return findpath( os.path.join(path,'datasets') )
    elif os.path.realpath(path) != repo_path: return findpath( updirectory(path) )
ntuple_path = findpath('.')
if ntuple_path == None: print "Unable to find path to ntuples in current or parent directories"; exit()

EFLAGS = {
    ValueError:False
}
def getDataset(label):
    global ntuple_path
    dataset = {}
    fname = '%s/%s.txt' % (ntuple_path,label)
    if not os.path.isfile(fname): return None
    def checkdir(directory):
        if os.path.isdir(directory): return True
        else: print '%s not a directory' % directory; EFLAGS[ValueError] = True; return False
    with open(fname) as f:
        subset = {}; subname = None
        for line in f.readlines():
            line = line.replace('\n','')
            if '#' in line: continue
            if '>>' in line:
                subname = line.replace('>>','')
                subset[subname] = []
            elif any(line):
                if checkdir(line): subset[subname].append(line)
    dataset[label] = subset
    return dataset
    
def getAllDatasets():
    global ntuple_path
    dataset = {}
    for fname in os.listdir(ntuple_path):
        if fname.endswith('.txt'): dataset.update( getDataset(fname.replace('.txt','')) )
    if EFLAGS[ValueError]: print 'Invalid directories detected, exiting...'; exit()
    return dataset

if __name__ == '__main__':
    datalist = ['egamma','singleele','singlepho','met','met_crab','signal','zjets_nlo','zjets','wjets_nlo','wjets','dyjets_nlo','dyjets','gjets','st','ttjets','ewk','qcd']
    from sys import argv
    if len(argv) == 1: datasets = getAllDatasets()
    else:
        datasets = {}
        for data in argv[1:]: datasets.update( getDataset(data) )
    import re
    def sort_nicely( l ):
        """ Sort the given list in the way that humans expect.
        """
        convert = lambda text: int(text) if text.isdigit() else text
        alphanum_key = lambda key: [ convert(c) for c in re.split('([0-9]+)', key) ]
        l.sort( key=alphanum_key )
    for data in datalist:
        if data not in datasets: continue
        sublist = datasets[data].keys()
        sort_nicely(sublist)
        for sub in sublist:
            print '%s----%s' % (data,sub)
            for directory in datasets[data][sub]:
                print '------%s' % directory

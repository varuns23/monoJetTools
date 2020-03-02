import os
import sys
import re
def sort_nicely( l ):
    """ Sort the given list in the way that humans expect.
        """
    convert = lambda text: int(text) if text.isdigit() else text
    alphanum_key = lambda key: [ convert(c) for c in re.split('([0-9]+)', key) ]
    l.sort( key=alphanum_key )
directory = sys.argv[1]

def fullpath(path):
    dirs = []
    def helper(path,dirs):
        if any( fname.endswith('.root') for fname in os.listdir(path) ): dirs.append(path); return
        for dir in os.listdir(path):
            dir_path = os.path.join(path,dir)
            if os.path.isdir(dir_path): helper(dir_path,dirs)
    helper(path,dirs)
    return dirs

class Query:
    def __init__(self,tag,mapping=None):
        self.tag = re.compile(tag)
        self.mapping = mapping
    def parse(self,sub):
        sub = self.tag.findall(sub)
        if not any(sub): return False
        if self.mapping is not None: self.sub = self.mapping(sub[0])
        
        return self.sub
def getSub(sub):
    def replace(string,old="",new=""): return string.replace(old,new)
    def axial(string):
        mphi = re.findall('Mphi-\d+',string)[0].replace('Mphi-','Mphi')
        mchi = re.findall('Mchi-\d+',string)[0].replace('Mchi-','Mchi')
        return '%s_%s' % (mchi,mphi)
    querylist = [
        Query('\d+to\d+|\d+toInf|MLM|Incl|FXFX'),
        Query('\d+To\d+|\d+ToInf',lambda s:replace(s,'To','to')),
        Query('\d+-\d+|\d+-Inf',lambda s:replace(s,'-','to')),
        Query('2017\w',lambda s:replace(s,'2017','')),
        Query('2018\w',lambda s:replace(s,'2018','')),
        Query('Mphi-\d+_Mchi-\d+',lambda s:axial(s)),
        Query('.*',lambda s:replace(s,'ST_',''))
    ]
    for query in querylist:
        if query.parse(sub):
            return query.sub
    return sub
    
def build_dataset(data,path):
    data = data.lower()
    sublist = os.listdir(path)
    sort_nicely(sublist)
    dataset = {}
    print data
    for sub in sublist:
        sub_path = os.path.join(path,sub)
        sub = getSub(sub)
        dirlist = fullpath(sub_path)
        if not any(dirlist): print "Nothing in %s" % sub_path; continue
        dataset[sub] = dirlist

    sublist = dataset.keys()
    sort_nicely(sublist)
    with open("ntuples/%s.txt" % data,"w") as f:
        for sub in sublist:
            print '>>%s' % sub
            f.write(">>%s\n" % sub)
            for directory in dataset[sub]:
                print directory
                f.write("%s/\n" % directory)
            f.write("\n")
    print

for data in os.listdir(directory):
    build_dataset(data,os.path.join(directory,data))

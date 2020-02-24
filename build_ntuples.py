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
    def __init__(self,tag,map=('','')):
        self.tag = re.compile(tag)
        def mapping(string): return string.replace(map[0],map[1])
        self.map = mapping
    def parse(self,sub):
        sub = self.tag.findall(sub)
        if not any(sub): return False
        self.sub = self.map(sub[0])
        return self.sub
def getSub(sub):
    
    querylist = [
        Query('\d+to\d+|\d+toInf|MLM|Incl|FXFX'),
        Query('\d+To\d+|\d+ToInf',('To','to')),
        Query('\d+-\d+|\d+-Inf',('-','to')),
        Query('2017\w',('2017','')),
        Query('2018\w',('2018','')),
        Query('.*',('ST_',''))
    ]
    for query in querylist:
        if query.parse(sub):
            return query.sub
    return sub
    
def build_dataset(data,path):
    data = data.lower()
    sublist = os.listdir(path)
    sort_nicely(sublist)
    print data
    with open("ntuples/%s.txt" % data,"w") as f:
        for sub in sublist:
            sub_path = os.path.join(path,sub)
            sub = getSub(sub)
            dirlist = fullpath(sub_path)
            if not any(dirlist): print "Nothing in %s" % sub_path; continue
            print '>>%s' % sub
            f.write(">>%s\n" % sub)
            for directory in dirlist:
                print directory
                f.write("%s/\n" % directory)
            f.write("\n")
    print

for data in os.listdir(directory):
    build_dataset(data,os.path.join(directory,data))

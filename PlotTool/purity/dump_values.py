"""
Dump purity/impurity values for Photon Purity
Usage: python PlotTool/purity/dump_values.py fitfile1 fitfile2
"""

from argparse import ArgumentParser
import sys
import re


ptbins = [200,250,300,400,500,600,"Inf"]
iarg = 0
for arg in list(sys.argv):
    if "ptbins" in arg:
        sys.argv.remove(arg)
        for i in range(len(ptbins)-1):
            sys.argv.insert(iarg,arg.replace("ptbins","%sto%s"%(ptbins[i],ptbins[i+1])))
            iarg += 1
    iarg += 1

parser = ArgumentParser("dump_values.py",__doc__)
parser.add_argument("-v","--value",default="postfit/purity")
parser.add_argument("-f","--format",default="%.3f")
parser.add_argument("files",nargs="*",type=TFile,default=[])

def sort_nicely( l ):
    """ Sort the given list in the way that humans expect.
    """
    convert = lambda text: int(text) if text.isdigit() else text
    alphanum_key = lambda key: [ convert(c) for c in re.split('([0-9]+)', key.GetName()) ]
    l.sort( key=alphanum_key )
    
args = parser.parse_args()
sort_nicely(args.files)

for tfile in args.files:
    value = tfile.Get(args.value)
    value = args.format%value.Integral()
    print "%s: %s"%(tfile.GetName(),value)

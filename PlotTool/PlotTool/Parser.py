# Stop ROOT from hijacking PlotTool's parser
import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True

import sys

from argparse import ArgumentParser
parser = ArgumentParser()
parser.add_argument('argv',nargs='*',help='accumlate undefined arguments',default=[])
parser.add_argument('--debug',help='enable debug option',action='store_true',default=False)

if "--debug" in sys.argv: import debug

def parse_args():
    parser.args = ArgumentParser.parse_args(parser)
def add_group(modfile,moddoc,modtype):
    irel = next( i for i,sub in enumerate(modfile.split("/")) if sub == "PlotTool" )
    path = "/".join(modfile.split("/")[irel:])
    return parser.add_argument_group("%s %s"%(path,modtype),moddoc)
    
parser.parse_args = parse_args
parser.add_group = add_group

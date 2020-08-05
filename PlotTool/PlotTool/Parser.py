# Stop ROOT from hijacking PlotTool's parser
import ROOT
ROOT.PyConfig.IgnoreCommandLineOptions = True

import sys

from argparse import ArgumentParser,RawDescriptionHelpFormatter

import textwrap as _textwrap
class LineWrapRawTextHelpFormatter(RawDescriptionHelpFormatter):
    # https://stackoverflow.com/questions/35917547/python-argparse-rawtexthelpformatter-with-line-wrap
    def _split_lines(self, text, width):
        text = self._whitespace_matcher.sub(' ', text).strip()
        return _textwrap.wrap(text, width)
    
parser = ArgumentParser(formatter_class=LineWrapRawTextHelpFormatter)
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

"""
Create alternate export file 
Usage: python PlotTool/store_variable.py variable1 variable2
"""

from PlotTool import *
from ROOT import *
import config
import os

group = parser.add_group(__file__,__doc__,"Script")

if not os.path.isdir('Output/'): os.mkdir('Output/')
def store(variable):

    output = TFile("Output/monojet_%s_%s.root" % (variable,config.version),"RECREATE")
    cwd = os.getcwd()
    regionmap = {
        "SignalRegion":"signal",
        "SingleEleCR":"Wen",
        "SingleMuCR":"Wmn",
        "DoubleEleCR":"Zee",
        "DoubleMuCR":"Zmm",
        "GammaCR":"gjets"
    }
    mcmap = {
        "TTJets":"top",
        "DYJets":"zll"
    }
    def store_region(region):
        label = regionmap[region]
        samples = Region(path=region,autovar=True)
        samples.initiate(variable)

        mcorder = filter(lambda mc:samples[mc].scaled_total/samples.total_bkg > 0.001,samples.MCOrder)

        output.cd()
        samples["Data"].histo.Clone("%s_data" % label).Write()
        for mc in mcorder:
            if mc not in mcmap:mclabel = mc.lower()
            else: mclabel = mcmap[mc]
            samples[mc].histo.Clone("%s_%s" % (label,mclabel.lower())).Write()
    for region in regionmap:
        try: store_region(region)
        except: pass

parser.parse_args()
for variable in parser.args.argv: store(variable)

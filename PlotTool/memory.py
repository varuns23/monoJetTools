from PlotTool import *

samples = Region()

samples.initiate("ChNemPtFrac_8")

for nuisance in samples.nuisances: samples.addUnc(nuisance)

for name,process in samples.processes.iteritems():
    for subprocess in process:
        subprocess.tfile.ls("-m")

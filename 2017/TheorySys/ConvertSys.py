from ROOT import *
import sys
import os
sys.path.append("PlotTool")
from PlotTool import *

region = Region(region="SignalRegion",autovar=True,show=False)

zw_tfile = TFile("RootFiles/wz_unc.root")
zw_systlist = [ key.GetName() for key in zw_tfile.GetListOfKeys() if 'Down' not in key.GetName() and 'monov' not in key.GetName() ]
zg_tfile = TFile("RootFiles/gz_unc.root")
zg_systlist = [ key.GetName() for key in zg_tfile.GetListOfKeys() if 'Down' not in key.GetName() and 'monov' not in key.GetName() ]

def ConvertProcess(region,process):
    region.SampleList = [process]
    region.initiate('ChNemPtFrac',weight='1')
    region.nominal = region[process].histo.Clone()
    
    def ConvertSys(region,process,syst,output):
        print syst
        region.initiate('ChNemPtFrac',weight=syst)
        up = region[process].histo.Clone(syst)
        region.initiate('ChNemPtFrac',weight=syst+'_Down')
        dn = region[process].histo.Clone(syst+'_Down')

        up.Divide(region.nominal)
        dn.Divide(region.nominal)

        output.cd()
        up.Write()
        dn.Write()

    zw_output = TFile("theory_sys/wz_unc.root","recreate")
    for syst in zw_systlist: ConvertSys(region,process,syst,zw_output)
    
    
    # zg_output = TFile("theory_sys/gz_unc.root","recreate")
    # for syst in zg_systlist: ConvertSys(region,process,syst,zg_output)

ConvertProcess(region,"WJets")

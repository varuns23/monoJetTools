import sys
from ROOT import TFile,TH1F

tfile = TFile.Open(sys.argv[1])
cutflow = tfile.Get('h_cutflow')
nbins = cutflow.GetNbinsX()
for ibin in range(1,nbins+1):
    print cutflow.GetXaxis().GetBinLabel(ibin),cutflow[ibin]
    # print cutflow[ibin]


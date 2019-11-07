from ROOT import *
from sys import argv

fn_unknown = TFile.Open(argv[1])
fn_known = TFile.Open(argv[2])

c_unknown = fn_unknown.Get("h_cutflow")
c_known = fn_known.Get("h_cutflow")

print c_unknown.GetBinContent(1)/c_known.GetBinContent(1)

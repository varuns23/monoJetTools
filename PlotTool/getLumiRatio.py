from ROOT import *
from sys import argv

fn_unknown = TFile.Open(argv[1])
fn_known = TFile.Open(argv[2])

c_unknown = fn_unknown.Get("h_cutflow")
c_known = fn_known.Get("h_cutflow")

print fn_unknown.GetName(),c_unknown[1]
print fn_known.GetName(),c_known[1]
print 'ratio',c_unknown[1]/c_known[1]


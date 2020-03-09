#!/usr/bin/env python
from ROOT import TFile
from sys import argv

path=argv[1]
def compare(f1,f2,ibin=1):
    fn_unknown = TFile.Open(f1)
    fn_known = TFile.Open(f2)
    
    c_unknown = fn_unknown.Get("h_cutflow")[ibin]
    c_known = fn_known.Get("h_cutflow")[ibin]
    ratio = c_unknown/c_known
    if ratio == 1: return
    print fn_unknown.GetName(),#c_unknown
    # print fn_known.GetName(),c_known
    print 'ratio',ratio
for f in argv[2:]:compare(f,"%s/%s"%(path,f))

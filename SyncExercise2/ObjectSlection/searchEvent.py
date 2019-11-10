from ROOT import *
import os
import sys

def hasEvents(rfile):
    def validEvent(event):
        return (event == 30682767 or
		event == 79123963 or
		event == 51393664 or
		event == 36533896 or
		event == 35733274 or
		event == 35719982)
    tfile = TFile.Open(rfile)
    tree = tfile.Get("phoJetNtuplizer/eventTree")
    return any(
        validEvent(event.event)
        for event in tree
    )

filelist = os.listdir(sys.argv[1])[100:]
for i,rfile in enumerate(filelist):
    print i,'/',len(filelist)
    if not rfile.endswith('.root'): continue
    if hasEvents( os.path.join(sys.argv[1],rfile) ): print rfile

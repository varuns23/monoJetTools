from ROOT import *
from sys import argv, path
from PlotTool import *
import os
import re

gROOT.SetBatch(1)

samples = Region(show=0)
    
for variable in samples.args:
    isGreater = None
    cut = None
    if (variable.find("+") != -1):
        i = variable.find("+")
        cut = float(variable[i+1:])
        isGreater = True
        variable = variable[:i]
    elif (variable.find("-") != -1):
        i = variable.find("-")
        cut = float(variable[i+1:])
        isGreater = False
        variable = variable[:i]
    samples.initiate(variable)
    
    sumOfBkg = samples.getSumOfBkg()
    signal = samples.histo[samples.signal[0]]

    if (cut != None):
        nbins = signal.GetNbinsX()
        ibin = signal.GetXaxis().FindBin(cut)
        if isGreater:
            sumOfBkg = sumOfBkg.Integral(ibin,nbins)
            signal = signal.Integral(ibin,nbins)
        else:
            sumOfBkg = sumOfBkg.Integral(1,ibin)
            signal = signal.Integral(1,ibin)
    else:
        sumOfBkg = sumOfBkg.Integral()
        signal = signal.Integral()
    
    print variable,"\t",signal/(sumOfBkg)**(0.5)
    

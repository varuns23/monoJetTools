#!/usr/bin/env python

from ROOT import *
from sys import argv
from sys import path
from PlotTool import *
import os

"""
Creates phase plots for 2D histograms
example https://www.hep.wisc.edu/~ekoenig4/MonoJet/Plots2018/SingleEleCRPlots_EWK/phase/data_h_LeptonEtaPhi_10.png
Directions: In region directior with valid root files
Usage: python ../PlotTool/makePhasePlots.py variable_1 variable_2 variable_3 ...
"""

gROOT.SetBatch(1)
samples=Region()
for variable in samples.args:
    samples.initiate(variable)
    bkgPlot = samples.getSumOfBkg().Clone("Sum of Background")
    dataPlot = samples.histo['Data'].Clone("Data")
    ratioPlot = Get2DRatio(dataPlot,bkgPlot).Clone("Data/MC Ratio")
            
    histos = [bkgPlot,dataPlot,ratioPlot]
    label = {}
    label[bkgPlot.GetName()]={"pre":"mc_","zaxis":[bkgPlot.GetMinimum(),bkgPlot.GetMaximum()]}
    label[dataPlot.GetName()]={"pre":"data_","zaxis":[dataPlot.GetMinimum(),dataPlot.GetMaximum()]}
    label[ratioPlot.GetName()]={"pre":"datamc_","zaxis":[0.3,1.7]}
    for hs in histos:
        canvas =TCanvas(hs.GetName(),"",1200,800)
        canvas.SetMargin(0.15,0.15,0.15,0.08)
        hs.Draw("colz")
        hs.SetStats(0)
        hs.GetZaxis().SetRangeUser(label[hs.GetName()]["zaxis"][0],label[hs.GetName()]["zaxis"][1])
        # canvas.SetLogz()
        hs.SetTitle(hs.GetName())
        hs.GetXaxis().SetTitle(samples.name['x'])
        hs.GetXaxis().SetTitleOffset(1.7)
        hs.GetYaxis().SetTitle(samples.name['y'])
        canvas.Write()
        
        dir = os.getcwd().split("/")[-1]
        file_path="/afs/hep.wisc.edu/home/ekoenig4/public_html/MonoJet/Plots2018/"+dir+"Plots_EWK/"
        #print file_path
        sub = "phase"
        directory=os.path.join(os.path.dirname(file_path),sub)
        if not os.path.exists(directory):
            os.mkdir(directory,0755)
            print directory
        canvas.SaveAs(directory+"/"+label[hs.GetName()]["pre"]+variable+".pdf")
        canvas.SaveAs(directory+"/"+label[hs.GetName()]["pre"]+variable+".png")

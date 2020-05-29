#!/bin/sh

variables=$(echo {{calo,pf}{MET,METPhi},j1{Eta,Phi,pT,CHF,NHF},metcut}_{11,12})
phase=$(echo {j1EtaPhi,pfMETvPhi}_{11,12})
plotslice() {
    phislice=$(echo {j1{Eta,Phi,pT},pf{MET,METPhi}}$1_{11,12})
    ./PlotTool/plotter.py --sub Unblinded/$1 $phislice
}

./PlotTool/plotter.py --sub Unblinded $variables
plotslice Posj1Phi
plotslice Negj1Phi

./PlotTool/makePhasePlots.py --sub Unblinded/phase $phase

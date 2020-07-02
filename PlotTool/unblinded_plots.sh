#!/bin/sh

variables=$(echo {{calo,pf}{MET,METPhi},j1{Eta,Phi,pT,CHF,NHF},metcut})
phase=$(echo {j1EtaPhi,pfMETvPhi})
plotslice() {
    phislice=$(echo {j1{Eta,Phi,pT},pf{MET,METPhi}}$1)
    ./PlotTool/plotter.py --sub Unblinded/$1 $phislice -a
}

./PlotTool/plotter.py --sub Unblinded $variables -a
plotslice Posj1Phi
plotslice Negj1Phi

./PlotTool/makePhasePlots.py --sub Unblinded/phase $phase -a

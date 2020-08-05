#!/bin/sh

variables=$(echo {tau,muon}{Pt,Eta,Phi} dilepton{Mall,Pt} isoCategory tau{Iso,Sig}{Ch,Nh,Ph}Mult tau{Iso,Sig}{PFCands,EtaWidth,ChNemPtFrac})

# ISO
# ./PlotTool/plotter.py --sub iso $variables -a +1 --auto-order --ignore-mc 0

# ./PlotTool/plotter.py --sub non-iso $variables -a +2 --auto-order --ignore-mc 0

./PlotTool/plotter.py -a -3 --sub full $(rootls -1 postDY400to600.root:monoJet_0 | awk '{gsub("_0","");print}') --ignore-mc 0 --auto-order

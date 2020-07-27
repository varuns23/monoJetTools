## Plotter

Script for plotting TH1 from monoJetTools
```
./PlotTool/plotter.py hsname_1 hsname_2 hsname_3
```
Advance Uses:
Drawing Signal Distributions 
```
./PlotTool/plotter.py hsname_1 -s signal
```
signal is the name given to the signals in config/signalinfo.py (i.e. axial).

A specific mass point point can be specified after -s signal as (-s signal mchi mphi).
```
./PlotTool/plotter.py hsname_1 -s signal 1 1000
```
Automatically Selecting Histogram Number
```
./PlotTool/plotter.py hsname -a # if trees is in directory 10, this will select directory 10
```
-a option (--autovar) automatically selects the histogram number coresponding to the trees directory (full selection).

You can add an integer as an argument to select plus n or minus n from the trees directory.
```
./PlotTool/plotter.py hsname -a 2 # if trees is in directory 10, this will select directory 12
```
Putting Plots in Subdirectory
```
./PlotTool/plotter.py --sub subdirectory hsname -a
```
Plotter uses the SaveAs script to place plots into a directory structure.

The --sub option allows to put files in a subdirectory within that directory structure.

Overriding Directory Structure
```
./PlotTool/plotter.py --outpath /path/to/new/place/ hsname -a
```
The outpath option will override the directory structure in SaveAs and place plots exactly where you specify
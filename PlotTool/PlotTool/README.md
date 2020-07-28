# Instructions for PlotTool Module

This is an outline of how the PlotTool module works and how it can be used to perform any amount of studies.

## The Basics

The module revolves around the [Region Class](https://github.com/varuns23/monoJetTools/blob/master/PlotTool/PlotTool/Region.py). At the beginning of all PlotTool based scripts (refer to [plotter.py](https://github.com/varuns23/monoJetTools/blob/master/PlotTool/plotter.py)) there should be an import from PlotTool and then ROOT.
```
from PlotTool import *
from ROOT import *
```
The Region class can then be called. When called with no arguments the class assumes you are in predefined directory structure.
```
$CMSSW_BASE/src/monoJetTools/$YEAR/$REGION
```
The class will search for a [config folder](https://github.com/varuns23/monoJetTools/tree/master/2017/config) in the current and above directories until it gets to the base path ($CMSSW_BASE/src/monoJetTools) at which something with throw an error.
Always make sure you create the Region class somewhere it can find the config folder.

Assuming that you have just recently finished all condor jobs for the region you are in. All the postfiles will be in the .output/ directory. When the Region class initiates it will read all the mc filenames from config.mcinfo.xsec along with the appropriate data file name defined at the top of Region.py. It will then use mergeFiles.py to hadd all the split files together and place them in the current directory. It will do this for any missing file it finds in the current directory However, you will need to remove the file from the current directory to get it to hadd again.

There are a few ways to change where the Region looks for postfiles.

Option 1: Current directory as stated above

Option 2: Passing the path to the Region class directly
```
region = Region(path="/path/to/new/postfiles/")
```
Option 3: Using command line option -d/--directory
```
python PlotTool/plotter.py -d /path/to/new/postfiles/
```
Option 4: Defining path in postpath.txt

If a postpath.txt file is defined in the current directory containing a single path. The Region class will use the path defined in the file as the path to the postpath. If there are postfiles contained in the current directory the Region class will use those instead. Include the --postpath command line option to override this and use the postpath.txt file instead.
       

## Region

## Process

## SubProcess

## VariableInfo

## Nuisance

## Transfer


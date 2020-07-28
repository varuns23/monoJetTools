# Instructions for PlotTool Module

This is an outline of how the PlotTool module works and how it can be used to perform any amount of studies.

## The Basics

The module revolves around the [Region Class](https://github.com/varuns23/monoJetTools/blob/master/PlotTool/PlotTool/Region.py). At the beginning of all PlotTool based scripts (refer to [plotter.py](https://github.com/varuns23/monoJetTools/blob/master/PlotTool/plotter.py)) there should be an import from PlotTool and then ROOT.
```python
from PlotTool import *
from ROOT import *
```
The Region class can then be called. When called with no arguments the class assumes you are in predefined directory structure.
```bash
$CMSSW_BASE/src/monoJetTools/$YEAR/$REGION
```
The class will search for a [config folder](https://github.com/varuns23/monoJetTools/tree/master/2017/config) in the current and above directories until it gets to the base path ($CMSSW_BASE/src/monoJetTools) at which something with throw an error.
Always make sure you create the Region class somewhere it can find the config folder.

### PostFile Path
Assuming that you have just recently finished all condor jobs for the region you are in. All the postfiles will be in the .output/ directory. When the Region class initiates it will read all the mc filenames from config.mcinfo.xsec along with the appropriate data file name defined at the top of Region.py. It will then use mergeFiles.py to hadd all the split files together and place them in the current directory. It will do this for any missing file it finds in the current directory However, you will need to remove the file from the current directory to get it to hadd again.

There are a few ways to change where the Region looks for postfiles.

Option 1: Current directory as stated above

Option 2: Passing the path to the Region class directly
```python
region = Region(path="/path/to/new/postfiles/")
```
Option 3: Using command line option -d/--directory
```bash
python PlotTool/plotter.py -d /path/to/new/postfiles/
```
Option 4: Defining path in postpath.txt

If a postpath.txt file is defined in the current directory containing a single path. The Region class will use the path defined in the file as the path to the postpath. If there are postfiles contained in the current directory the Region class will use those instead. Include the --postpath command line option to override this and use the postpath.txt file instead.

### Getting Variables
The initiate method is used to get and scale distributions from postfiles. The initiate method can be used in a few ways.
```python
region = Region()
region.initiate("recoil_10")
```
You can use the -a/--autovar option to select the nhist that the trees directory is in. You can add an integer to the command line to select a relative nhist from the trees directory. 
```bash
python PlotTool/plotter.py recoil -a 1 # if trees directory is monoJet_10 this will select recoil_11
```
You can also pass autovar to the Region class to select the trees directory
```python
region = Region(autovar=True)
```

### Getting Histograms
After the initiate method is called for a variable the Region class holds a dictionary of [Process Classes](https://github.com/varuns23/monoJetTools/blob/d12a6d71c60746fee23cbb9ef2bebd93f1311d78/PlotTool/PlotTool/Process.py#L129) for data and mc. These can be retrieved like so
```python
region = Region()
region.initiate("recoil")
hs_data = region["Data"].histo
hs_zjets = region["ZJets"].histo
# etc ...
```
The Region class also has a list of the mc contained within it for easy loop over mc
```python
for mc in region.MCList: h_mc = region[mc].histo
```

### Using Tree
If the variable selected is contained in the tree you can pass the --branch option on the command line to select the tree variable. With the tree variable you can do a few other things.
#### Specify Cuts
You can specify cuts on tree variables using the -c/--cut option or passing the cut to the initiate function
```bash
# Command Line
python PlotTool/plotter.py recoil -a --branch -c "j1pT>200"
```
```python
# In Script
region = Region(autovar=True)
region.initiate("recoil",cut="j1pT>200")
```
#### Specify Weights
You can specify the weight to use with tree variables using -w/--weight options or passing the weight to the initiate function
```bash
# Command Line
python PlotTool/plotter.py recoil -a --branch -w "nlo_qcd*nlo_ewk*pileup"
```
```python
# In Script
region = Region(autovar=True)
region.initiate("recoil",weight="nlo_qcd*nlo_ewk*pileup")
```

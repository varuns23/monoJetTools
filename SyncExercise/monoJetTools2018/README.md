# Mono-Z' Jet + MET analysis

This repository contains pacakges to analyze ntuples for the signal region and the control region 
for the Mono-Z' Jet + MET analysis.
Jobs are submitted to condor through the SubmitCondor.py script.

SubmitCondor.py creates 2 hidden directories: .output/ and .status/
.status/ holds all the .out, .err, and .log files that condor creates
.output/ holds all the executables and eventually the output root files

To run all data and MC, use the submit.sh file.

Instructions:


```bash
cmsrel CMSSW_10_2_10
cd CMSSW_10_2_10/src
cmsenv
voms-proxy-init --voms=cms --valid=192:00
git clone -b 2018 https://github.com/ekoenig4/ZprimeTools.git ZprimeTools2018
cd ZprimeTools2018
make
```
To run locally

```bash
cd region # any of the 5 regions SignalRegion, SingleEleCR, SingleMuCR, DoubleEleCR, DoubleMuCR
make
./analyze /path/to/Ntuple/Directory/ outputFilename.root numOfEvents reportEvery filerange
#example
./analyze /hdfs/store/user/varuns/NTuples/monoZprime_2018_80X/MonoZprime_Mx10_Mv1000/ postMx10_Mv1000.root -1 1000 1-9
#use -1 to run over all events and for running over the entire directory
````

The postAnalyzer is broken into 4 different purposes

    ZprimeJetsClass.h:
	Holds information about the variables, methods, and histograms that are used and filled in the analysis

    ZprimeJetsInit.C:
	Holds the methods for initiating all the variables and files that are being used

    ZprimeJetsCommon.C:
	Holds the methods that are shared across all the different regions (Signal and Control Regions)

    ZprimeJetsClass.C:
	Holds the methods that are unique to that particular region

Signal Region:
To run the signal samples use the signalsubmit.sh file

Plotting:
The PlotTool python package is setup to work with the file structure created by using this setup
After the condor jobs are finished use the PlotTool/plotter.py file to merge all the correct files together and plot them for a given variable

```bash
python PlotTool/plotter.py j1pT_8
```

To plot the signal distribution over the dataMC use 
```bash
python PlotTool/plotter.py -s Mx10_Mv1000 j1pT_8
```

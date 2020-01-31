# Mono-Z' Jet Post Analyzer

This repository contains pacakges to analyze ntuples for the signal region and the control region 
for the Mono-Z' Jet + MET analysis.
Jobs are submitted to condor through the SubmitCondor.py script.

SubmitCondor.py creates 2 hidden directories: .output/ and .status/
.status/ holds all the .out, .err, and .log files that condor creates
.output/ holds all the executables and eventually the output root files

To run all data and MC, use the submit.py file.

Instructions:

```bash
cmsrel CMSSW_10_2_10
cd CMSSW_10_2_10/src
cmsenv
voms-proxy-init --voms=cms --valid=192:00
git clone https://github.com/ekoenig4/ZprimeTools.git
cd ZprimeTools/PostAnalyzer/

make bin && make
```

To run any year locally 

```bash
cd year/region # any 3 years: 2016, 2017, or 2018, then any of the 5 regions: SignalRegion, SingleEleCR, SingleMuCR, DoubleEleCR, or DoubleMuCR
make
./analyze /path/to/Ntuple/Directory/ outputFilename.root numOfEvents reportEvery filerange
#example
./analyze /hdfs/store/user/varuns/NTuples/monoZprime_2018_80X/MonoZprime_Mx10_Mv1000/ postMx10_Mv1000.root -1 1000 1-9
#use -1 to run over all events and for running over the entire directory
#run analyze without options to run a default run
./analyze
````

The PostAnalyzer uses the inc folder to hold header files, the src folder to hold source files, and the bin folder to hold object files.
Starting from the top and moving down into the each year and region, the PostAnalzyer is broken into 4 sections

    ZprimeAnalysis:
	Holds information about variables, methods, and histograms that are used and filled in the analysis

    Zprime*CR:
	Holds information specific to the control region of its name

    ZprimeYear:
	Holds information specific to the year that it is in, namely tree initiation, construction, and object ID's

    ZprimeClass:
	Holds information specific to event selection

## Submitting to Condor
Submitting to Condor is handled mostly by CondorTools/SubmitCondor.py
This script will create two directories: .output/ and .status/.
.output will have the copy of the script passed, condor_submit files, condor executable, and after jobs are finished the post root files.
.status will have directories named after what is specified with condor_label, in these directories there will be the: .out, .err, .log files that are created by condor
The last option of the script split_N, specifies how many batches should be submitted to condor for the file directory (i.e. split_1 specifies 1 batch, split_2 specifies 2 batches).
N can be set to -1 (split_-1) whichc will split the directory so that there are a certain number of files per batch, the number of files can be changed in SubmitCondor.py with the NFILES_PER_BATCH variable.
Some extra options are -y year and -r region, these are taken into the BATCH_NAME of the condor job to better display in condor_q

```bash
#To submit a file directory to condor
python $CMSSW_BASE/src/ZprimeTools/CondorTools/SubmitCondor.py -y year -r region analyze /path/to/Ntuple/Directory/ outputFilename.root numOfEvents reportEvery condor_label split_N
#example
python $CMSSW_BASE/src/ZprimeTools/CondorTools/SubmitCondor.py -y 2016 -r SR analyze /hdfs/store/user/varuns/NTuples/monoZprime_2018_80X/MonoZprime_Mx10_Mv1000/ postMx10_Mv1000.root -1 1000 Mx10_Mv1000 split_-1
```

If the directories to be submitted are defined in the datasets/ntuples/ directory a python script can be used to submit everything automatically.
Using submit.py will submit all the directories specified in the datasets/ntuples/ using the SubmitDataset python script

There is an options dictionary that is used by SubmitDataset where a year and region can be specified.
The parallel option can be set to True (False by default) which will allow submitting in parallel.
The batchsize option should be set to the number of files to use per batch
The submit option can be set to False (True by default) which will run SubmitCondor without submitting jobs to condor
The submit method from SubmitDataset handles passing datasets to SubmitCondor

The first argument of submit is the name of the dataset to be submitted, this should match the text file contained in datasets/ntuples/
With no other options all subdatasets will be submitted automatically, the sub option can be set to match the specific subdataset that you want to run
The label option can be set to change the output filename
The split option can be set to match the split_N option
The filelist can be set to True (False by default) if you want to submit a list of files instead of a fileRange (need to do this for farmout directories)
The script option can be set to specifiy the script that should be used (analyze by default)

Refer to any submit.py script to get an idea of how it should work

## Plotting
The PlotTool python package is setup to work with the file structure created by using this setup
After the condor jobs are finished use the PlotTool/plotter.py file to merge all the correct files together and plot them for a given variable

```bash
python PlotTool/plotter.py j1pT_8
```

To plot the signal distribution over the dataMC use 
```bash
python PlotTool/plotter.py -s Mx10_Mv1000 j1pT_8
```
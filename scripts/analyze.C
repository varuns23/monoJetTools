#include <TSystem.h>

#include <stdio.h> 
#include <stdlib.h>
#include <iostream>
#include <vector>
#include <string>

#include "Dataset.h"
#include "monoJetClass.h"

using namespace std;

bool isNumber(const char* str) {
  for ( const char* p = &str[0]; *p != '\0'; ++p) {
    if ( isdigit(*p) == 0 ) return false;
  }
  return true;
}

int run(int argc,const char* argv[]) {
  const char* inputFilename = argv[1];
  const char* outputFilename = argv[2];
  Long64_t maxEvents = atof(argv[3]);
  if (maxEvents < -1LL)
    {
      cout<<"Please enter a valid value for maxEvents (parameter 3)."<<endl;
      return 1;
    }
  int reportEvery = atof(argv[4]);
  if (reportEvery < 1)
    {
      cout<<"Please enter a valid value for reportEvery (parameter 4)."<<endl;
      return 1;
    }
  if ( !gSystem->AccessPathName( (string(inputFilename) + string(argv[5])).c_str() ) ) {
    vector<const char*>filelist;
    for (int i = 5; i < argc; i++) filelist.push_back( argv[i] );
    monoJetClass t(inputFilename,outputFilename,filelist); t.Loop(maxEvents,reportEvery);
  } else if ( isNumber(argv[5]) ){
    int nfiles = atoi(argv[5]);
    monoJetClass t(inputFilename,outputFilename,nfiles); t.Loop(maxEvents,reportEvery);
  } else {
    const char* fileRange = argv[5];
    monoJetClass t(inputFilename,outputFilename,fileRange); t.Loop(maxEvents,reportEvery);
  }
  return 0;
}

int run_mc_test(string inputdir,const char* argv[]) {
  cout << "Running MC test in: " << inputdir << endl;
  argv[1] = inputdir.c_str();
  argv[2] = "test_mc.root";
  argv[3] = "5000";
  argv[4] = "100";
  argv[5] = "1";
  return run(6,argv);
}

int run_data_test(string inputdir,const char* argv[]) {
  cout << "Running Data test in: " << inputdir << endl;
  argv[1] = inputdir.c_str();
  argv[2] = "test_data.root";
  argv[3] = "5000";
  argv[4] = "100";
  argv[5] = "1";
  return run(6,argv);
}

int run_signal_test(string inputdir,const char* argv[]) {
  cout << "Running Signal test in: " << inputdir << endl;
  argv[1] = inputdir.c_str();
  argv[2] = "test_signal.root";
  argv[3] = "5000";
  argv[4] = "100";
  argv[5] = "-1";
  return run(6,argv);
}


int setup_test(int argc, const char* argv[]) {
  Dataset dataset;
  string mcdir,datadir;
  string signaldir = "None";
  if ( monoJetClass::REGION == SR ) {
    mcdir = dataset.getDirlist("zjets","400to600")[0];
    datadir = dataset.getDirlist("met",monoJetClass::SRDATA)[0];
    if ( dataset.getSubset("signal").size() != 0 )
      signaldir = dataset.getDirlist("signal","Mx1_Mv1000")[0];
    
  } else if ( monoJetClass::REGION == WE || monoJetClass::REGION == WM ) {
    mcdir = dataset.getDirlist("wjets","400to600")[0];
    if ( monoJetClass::REGION == WE ) {
      if (dataset.contains("singleele"))
	datadir = dataset.getSubset("singleele").begin()->second[0];
      else
	datadir = dataset.getSubset("egamma").begin()->second[0];
    }
    if ( monoJetClass::REGION == WM  ) datadir = dataset.getSubset("met").begin()->second[0];
    
  } else if ( monoJetClass::REGION == ZE || monoJetClass::REGION == ZM ) {
    mcdir = dataset.getDirlist("dyjets","400to600")[0];
    if ( monoJetClass::REGION == ZE ) {
      if (dataset.contains("singleele"))
	datadir = dataset.getSubset("singleele").begin()->second[0];
      else
	datadir = dataset.getSubset("egamma").begin()->second[0];
    }
    if ( monoJetClass::REGION == ZM  ) datadir = dataset.getSubset("met").begin()->second[0];
  } else if (monoJetClass::REGION == GA) {
    mcdir = dataset.getDirlist("gjets","400to600")[0];
    if (dataset.contains("singlepho"))
      datadir = dataset.getSubset("singlepho").begin()->second[0];
    else
      datadir = dataset.getSubset("egamma").begin()->second[0];
  }
  int mc = run_mc_test(mcdir,argv);
  int data = run_data_test(datadir,argv);
  int signal = 0;
  if ( signaldir != "None" ) signal = run_signal_test(signaldir,argv);
  return mc + data;
}

int main(int argc, const char* argv[]) {
  if ( argc == 1 ) return setup_test(argc,argv);
  else             return run(argc,argv);
}

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

int run_zjets_test(string inputdir,const char* argv[]) {
  cout << "Running ZJETS test in: " << inputdir << endl;
  argv[1] = inputdir.c_str();
  argv[2] = "test_zjets.root";
  argv[3] = "5000";
  argv[4] = "100";
  argv[5] = "1";
  return run(6,argv);
}

int run_wjets_test(string inputdir,const char* argv[]) {
  cout << "Running Wjets test in: " << inputdir << endl;
  argv[1] = inputdir.c_str();
  argv[2] = "test_wjets.root";
  argv[3] = "5000";
  argv[4] = "100";
  argv[5] = "1";
  return run(6,argv);
}

int run_gjets_test(string inputdir,const char* argv[]) {
  cout << "Running Gjets test in: " << inputdir << endl;
  argv[1] = inputdir.c_str();
  argv[2] = "test_gjets.root";
  argv[3] = "5000";
  argv[4] = "100";
  argv[5] = "-1";
  return run(6,argv);
}


int setup_test(int argc, const char* argv[]) {
  Dataset dataset;
  string zjetsdir,wjetsdir,gjetsdir;
  zjetsdir = dataset.getDirlist("zjets","400to600")[0];
  wjetsdir = dataset.getDirlist("wjets","400to600")[0];
  gjetsdir = dataset.getDirlist("gjets","400to600")[0];
  int zjets = run_zjets_test(zjetsdir,argv);
  int wjets = run_wjets_test(wjetsdir,argv);
  int gjets = run_gjets_test(gjetsdir,argv);
  return zjets;
}

int main(int argc, const char* argv[]) {
  if ( argc == 1 ) return setup_test(argc,argv);
  else             return run(argc,argv);
}

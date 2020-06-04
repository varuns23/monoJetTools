#ifndef Dataset_C
#define Dataset_C

#include <sys/stat.h>
#include <TSystemDirectory.h>
#include <TSystem.h>
#include <TString.h>
#include <TList.h>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include "TString.h"
#include <array>

#include "Dataset.h"

using namespace std;

bool isDir(const TString &dir)
{
  return gSystem->OpenDirectory(dir) != 0;
  // struct stat buffer;
  // return (stat (dir, &buffer) == 0);
}

bool contains_substr(TString str,TString delim) {
  return strstr(str,delim) != NULL;
}

const std::map<TString,Type> Dataset::datamap = {
  {"egamma",Data},{"singleele",Data},{"singlepho",Data},{"met",Data},
  {"zprime",Signal},{"axial",Signal},
  {"zjets",ZJets},{"wjets",WJets},{"dyjets",DYJets},{"gjets",GJets},
  {"zjets_nlo",ZJets},{"wjets_nlo",WJets},{"dyjets_nlo",DYJets},
  {"qcd",QCD},{"ttjets",TTJets},{"st",ST},{"ewk",EWK}
};

Dataset::SubsetList Dataset::dataset_;

TString exec(TString cmd) {
  return (TString)gSystem->GetFromPipe(cmd);
}

TString FindNtuples(TString path="./",TString repo=(TString(getenv("CMSSW_BASE"))+"/src/monoJetTools")) {
  TString realpath = exec("realpath "+path);
  if ( isDir(realpath+"/ntuples/") ) return path+"ntuples/";
  if ( isDir(realpath+"/datasets/ntuples/") ) return path+"datasets/ntuples/";
  if ( realpath == repo ) return "";
  if ( path == "./" ) path = "";
  return FindNtuples("../"+path);
}


Dataset::SubsetList::SubsetList() {
  TString ntuples = FindNtuples();
  // TString ntuple_path1 = "ntuples/";
  // TString ntuple_path2 = "../datasets/ntuples/";
  // if ( isDir(ntuple_path1) ) ntuples = ntuple_path1;
  // else if ( isDir(ntuple_path2) ) ntuples = ntuple_path2;
  cout << "Using path " << ntuples << " for datasets." << endl;
  TSystemDirectory dir(ntuples,ntuples);
  TList* filelist = dir.GetListOfFiles();
  TIter fileiter(filelist);
  TSystemFile* file;
  while ( (file = (TSystemFile*)fileiter()) ) {
    TString filename = (TString)file->GetName();
    if ( contains_substr(filename,".txt") ) {
      addDataset(ntuples,filename);
    }
  }
}

void Dataset::SubsetList::addDataset(TString path,TString filename) {
  ifstream infile( (path+filename) );
  if (!infile.is_open()) {
    cout << "Unable to read " << path+filename << endl;
    return;
  }
  TString data = filename.Remove( filename.Length()-4,4 );
  if ( datamap.find(data) == datamap.end() ) {
    cout << "Unable to find "+data+" in known datalist" << endl;
    return;
  }
  TString line;
  Subset subset; TString subname;
  while ( infile >> line ) {
    if ( contains_substr(line,"#") ) continue;
    if ( contains_substr(line,">>") ) {
      subname = line.Remove(0,2);
      subset[subname] = vector<TString>();
    } else {
      if ( true ) {
	subset[subname].push_back(line);
      }
    }
  }
  (*this)[data] = subset;
}

Dataset::Dataset() {
  type = nType;
  isInclusive = false;
  isMC = true;
  isSignal = false;
  isNLO = false;
  path = "";
  dataset = "";
  subset = "";
  PID = 0;
}

void Dataset::setTypeInfo(TString path) {
  for (auto pair : datamap) {
    TString data = pair.first;
    Type type = pair.second;
    
    Subset subset = dataset_[data];
    for (auto& sub : subset) {
      for (TString directory : sub.second) {
	if ( contains_substr(path,directory) ) {
	  dataset = data;
	  this->path = path;
	  this->subset = sub.first;
	  this->type = type;
	  isNLO = contains_substr(data,"nlo");
	  if ( type == WJets || type == DYJets ) {
	    isInclusive = contains_substr(sub.first,"MLM") || contains_substr(sub.first,"Incl");
	  } else {
	    isInclusive = false;
	  }
	}
      }
    }
  }
}

void Dataset::setInfo(TString path) {
  setTypeInfo(path);
  isMC = (type != Data);
  isSignal = (type == Signal);
  if (type == WJets) PID = 24;
  else if (type == ZJets || type == DYJets) PID = 23;
  else if (type == GJets) PID = 22;
  
}

void Dataset::printDataset() {
  for (auto pair : datamap) {
    TString data = pair.first;
    if (!this->contains(data)) continue;
    Subset subset = dataset_[data];
    for (auto& sub : subset) {
      cout << data << "----" << sub.first << endl;
      for (TString directory : sub.second) {
	cout << "------" << directory << endl;
      }
    }
  }
}

#endif

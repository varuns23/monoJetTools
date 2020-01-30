#ifndef Dataset_C
#define Dataset_C

#include <sys/stat.h>
#include <TSystemDirectory.h>
#include <TList.h>
#include <fstream>
#include <algorithm>

#include "Dataset.h"

using namespace std;

bool isDir(const string &s)
{
  struct stat buffer;
  return (stat (s.c_str(), &buffer) == 0);
}

bool contains_substr(string str,string delim) {
  return strstr(str.c_str(),delim.c_str()) != NULL;
}

const vector<std::string> Dataset::datalist = {"egamma","singleele","singlepho","met","signal","zjets","zjets_nlo","wjets","wjets_nlo","dyjets","dyjets_nlo","gjets","ttjets","st","ewk","qcd"};
const std::map<std::string,Type> Dataset::typemap = {
  {"egamma",Data},{"singleele",Data},{"singlepho",Data},{"met",Data},{"signal",Signal},{"zjets",ZJets},{"zjets_nlo",ZJets_NLO},{"wjets",WJets},{"wjets_nlo",WJets_NLO},
  {"dyjets",DYJets},{"dyjets_nlo",DYJets_NLO},{"qcd",QCD},{"ttjets",TTJets},{"st",ST},{"gjets",GJets},{"ewk",EWK}
};
Dataset::SubsetList Dataset::dataset;

Dataset::SubsetList::SubsetList() {
  string ntuples;
  string ntuple_path1 = "ntuples/";
  string ntuple_path2 = "../datasets/ntuples/";
  if ( isDir(ntuple_path1) ) ntuples = ntuple_path1;
  else if ( isDir(ntuple_path2) ) ntuples = ntuple_path2;
  cout << "Using path " << ntuples << " for datasets." << endl;
  TSystemDirectory dir(ntuples.c_str(),ntuples.c_str());
  TList* filelist = dir.GetListOfFiles();
  TIter fileiter(filelist);
  TSystemFile* file;
  while ( (file = (TSystemFile*)fileiter()) ) {
    string filename = (string)file->GetName();
    if ( contains_substr(filename,".txt") ) {
      addDataset(ntuples,filename);
    }
  }
}

void Dataset::SubsetList::addDataset(string path,string filename) {
  ifstream infile( (path+filename).c_str() );
  if (!infile.is_open()) {
    cout << "Unable to read " << path+filename << endl;
    return;
  }
  string data = filename.erase( filename.length()-4,4 );
  if ( std::find(datalist.begin(),datalist.end(),data) == datalist.end() ) {
    cout << "Unable to find "+data+" in known datalist" << endl;
							   return;
  }
  string line;
  Subset subset; string subname;
  while ( infile >> line ) {
    if ( contains_substr(line,"#") ) continue;
    if ( contains_substr(line,">>") ) {
      subname = line.erase(0,2);
      subset[subname] = vector<string>();
    } else {
      if ( isDir(line) ) {
	subset[subname].push_back(line);
      }
    }
  }
  (*this)[data] = subset;
}

Dataset::Dataset() {
  type = nType;
  region = nRegion;
  year = 0;
  isInclusive = false;
  isData = false;
  isSignal = false;
  isNLO = false;
  PID = 0;
}

void Dataset::setTypeInfo(string path) {
  for (string data : datalist) {
    Subset subset = dataset[data];
    for (auto& sub : subset) {
      for (string directory : sub.second) {
	if ( contains_substr(path,directory) ) {
	  type = typemap.find(data)->second;
	  isNLO = contains_substr(data,"NLO");
	  if ( type == WJets || type == DYJets ) {
	    isInclusive = contains_substr(sub.first,"MLM");
	  } else {
	    isInclusive = false;
	  }
	}
      }
    }
  }
}

void Dataset::setInfo(string path) {
  setTypeInfo(path);
  isData = (type == Data);
  isSignal = (type == Signal);
  if (type == WJets || type == WJets_NLO) PID = 24;
  else if (type == ZJets || type == ZJets_NLO || type == DYJets || DYJets_NLO) PID = 23;
}

void Dataset::printDataset() {
  for (string data : datalist) {
    if (!this->contains(data)) continue;
    Subset subset = dataset[data];
    for (auto& sub : subset) {
      cout << data << "----" << sub.first << endl;
      for (string directory : sub.second) {
	cout << "------" << directory << endl;
      }
    }
  }
}

#endif

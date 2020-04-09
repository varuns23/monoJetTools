#ifndef Dataset_C
#define Dataset_C

#include <sys/stat.h>
#include <TSystemDirectory.h>
#include <TList.h>
#include <fstream>
#include <algorithm>
#include <iostream>
#include <cstdio>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>

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

const std::map<std::string,Type> Dataset::datamap = {
  {"egamma",Data},{"singleele",Data},{"singlepho",Data},{"met",Data},
  {"zprime",Signal},{"axial",Signal},
  {"zjets",ZJets},{"wjets",WJets},{"dyjets",DYJets},{"gjets",GJets},
  {"zjets_nlo",ZJets},{"wjets_nlo",WJets},{"dyjets_nlo",DYJets},
  {"qcd",QCD},{"ttjets",TTJets},{"st",ST},{"ewk",EWK}
};

Dataset::SubsetList Dataset::dataset_;

void eraseAllSubStr(std::string & mainStr, const std::string & toErase) {
  size_t pos = std::string::npos;
  
  // Search for the substring in string in a loop untill nothing is found
  while ((pos  = mainStr.find(toErase) )!= std::string::npos) {
    // If found then erase it from string
    mainStr.erase(pos, toErase.length());
  }
}

std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    result.erase(std::remove(result.begin(), result.end(), '\n'), result.end());
    return result;
}

string FindNtuples(string path="./",string repo=(string(getenv("CMSSW_BASE"))+"/src/monoJetTools")) {
  string realpath = exec(("realpath "+path).c_str());
  if ( isDir(realpath+"/ntuples/") ) return path+"ntuples/";
  if ( isDir(realpath+"/datasets/ntuples/") ) return path+"datasets/ntuples/";
  if ( realpath == repo ) return "";
  if ( path == "./" ) path = "";
  return FindNtuples("../"+path);
}


Dataset::SubsetList::SubsetList() {
  string ntuples = FindNtuples();
  // string ntuple_path1 = "ntuples/";
  // string ntuple_path2 = "../datasets/ntuples/";
  // if ( isDir(ntuple_path1) ) ntuples = ntuple_path1;
  // else if ( isDir(ntuple_path2) ) ntuples = ntuple_path2;
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
  if ( datamap.find(data) == datamap.end() ) {
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
  isInclusive = false;
  isMC = true;
  isSignal = false;
  isNLO = false;
  path = "";
  dataset = "";
  subset = "";
  PID = 0;
}

void Dataset::setTypeInfo(string path) {
  for (auto pair : datamap) {
    string data = pair.first;
    Type type = pair.second;
    
    Subset subset = dataset_[data];
    for (auto& sub : subset) {
      for (string directory : sub.second) {
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

void Dataset::setInfo(string path) {
  setTypeInfo(path);
  isMC = (type != Data);
  isSignal = (type == Signal);
  if (type == WJets) PID = 24;
  else if (type == ZJets || type == DYJets) PID = 23;
  else if (type == GJets) PID = 22;
  
}

void Dataset::printDataset() {
  for (auto pair : datamap) {
    string data = pair.first;
    if (!this->contains(data)) continue;
    Subset subset = dataset_[data];
    for (auto& sub : subset) {
      cout << data << "----" << sub.first << endl;
      for (string directory : sub.second) {
	cout << "------" << directory << endl;
      }
    }
  }
}

#endif

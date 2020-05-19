#ifndef Dataset_h
#define Dataset_h

#include <iostream>
#include <map>
#include <vector>
#include "TString.h"

#include "monoJetEnums.h"

class Dataset {
  typedef std::map<TString,std::vector<TString>> Subset;
  struct SubsetList : public std::map<TString,Subset> {
    SubsetList();
    void addDataset(TString path,TString filename);
  };
  static SubsetList dataset_;
public:
  static const std::map<TString,Type> datamap;

  Type type;
  bool isInclusive;
  bool isMC;
  bool isSignal;
  bool isNLO;
  TString path;
  TString dataset;
  TString subset;
  int PID;

  Dataset();
  void printDataset();

  void setInfo(TString path);
  void setTypeInfo(TString path);
  static inline Subset getSubset(TString data) { return dataset_[data]; }
  static inline std::vector<TString> getDirlist(TString data,TString subset) { return dataset_[data][subset]; }
  static inline bool contains(TString data,TString subset="") {
    if (dataset_.find(data) != dataset_.end()) {
      if (subset == "") return true;
      return dataset_[data].find(subset) != dataset_[data].end();
    }
    return false;
  }
};

#endif

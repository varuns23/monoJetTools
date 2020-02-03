#ifndef Dataset_h
#define Dataset_h

#include <iostream>
#include <map>
#include <vector>
#include <string>

#include "monoJetEnums.h"

class Dataset {
  typedef std::map<std::string,std::vector<std::string>> Subset;
  struct SubsetList : public std::map<std::string,Subset> {
    SubsetList();
    void addDataset(std::string path,std::string filename);
  };
  static SubsetList dataset_;
public:
  static const std::vector<std::string> datalist;
  static const std::map<std::string,Type> typemap;

  Type type;
  Region region;
  int year;
  bool isInclusive;
  bool isData;
  bool isSignal;
  bool isNLO;
  std::string dataset;
  std::string subset;
  int PID;

  Dataset();
  void printDataset();

  void setInfo(std::string path);
  void setTypeInfo(std::string path);
  static inline Subset getSubset(std::string data) { return dataset_[data]; }
  static inline std::vector<std::string> getDirlist(std::string data,std::string subset) { return dataset_[data][subset]; }
  static inline bool contains(std::string data,std::string subset="") {
    if (dataset_.find(data) != dataset_.end()) {
      if (subset == "") return true;
      return dataset_[data].find(subset) != dataset_[data].end();
    }
    return false;
  }
  inline const char* GetTypeName() { return TypeName[type]; }
  inline const char* GetRegionName() { return RegionName[region]; }
  inline int GetYear() { return year; }
};

#endif

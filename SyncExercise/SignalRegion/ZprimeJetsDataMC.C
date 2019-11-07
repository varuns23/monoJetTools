#define ZprimeJetsClass_cxx
#include "ZprimeJetsClass.h"

ZprimeJetsCommon::DataMC::DataMC(string filename) : ZprimeJetsCommon::DataMC::DataMC() {
  string sampleID[] = {"Run2018","monoZprime","WJets","ZJets","DYJets","QCD","TTJets","GJets","WW","WZ","ZZ"};
  string inclusiveID[] = {"WJetsToLNu_Incl","DYJetsToLL_Incl"};
  for (int i = 0; i < Total; i++)
    if (filename.find(sampleID[i]) != string::npos) {
      type = static_cast<Type>(i);
    }
  isData = (type == Data);
  isSignal = (type == Signal);
  isInclusive = false;
  if (type == WJets || type == DYJets)
    for (int i = 0; i < 2; i++)
      if (filename.find(inclusiveID[i]) != string::npos)
	isInclusive = true;
  if (type == WJets) PID = 24;
  if (type == ZJets) PID = 23;
}

bool ZprimeJetsCommon::DataMC::isW_or_ZJet() { return type == WJets || type == ZJets; }

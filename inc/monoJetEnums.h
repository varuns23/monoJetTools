#ifndef monoJetEnums_h
#define monoJetEnums_h

enum Type {Data,Signal,ZJets,WJets,DYJets,QCD,TTJets,ST,GJets,EWK,nType};
static const char * TypeName[] = {"Data","Signal","ZJets","WJets","DYJets","QCD","TTJets","ST","GJets","EWK"};

enum Region{SR,WE,WM,ZE,ZM,GA,nRegion};
static const char * RegionName[] = {"SignalRegion","SingleElectron","SingleMuon","DoubleElectron","DoubleMuon","Gamma"};

enum CRobject{Electron,Muon,Photon,nCRobject,None};
static const char * CRobjectName[] = {"Electron","Muon","Photon"};

#endif

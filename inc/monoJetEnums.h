#ifndef monoJetEnums_h
#define monoJetEnums_h

enum Type {Data,Signal,ZJets,WJets,DYJets,QCD,TTJets,ST,GJets,EWK,nType};
static const char * TypeName[] = {"Data","Signal","ZJets","WJets","DYJets","QCD","TTJets","ST","GJets","EWK"};

enum Region{SR,WE,WM,ZE,ZM,GA,TFR,nRegion};
static const char * RegionName[] = {"SignalRegion","SingleElectron","SingleMuon","DoubleElectron","DoubleMuon","Gamma","TauFakeRate"};

enum CRobject{Electron,Muon,Tau,Photon,MuTau,nCRobject,None};
static const char * CRobjectName[] = {"Electron","Muon","Tau","Photon","MuTau"};

enum Hadron{Charged=211,Neutral=130,Gamma=22};
#endif

#ifndef monoJetEnums_h
#define monoJetEnums_h

enum Type {Data,Signal,ZJets,ZJets_NLO,WJets,WJets_NLO,DYJets,DYJets_NLO,QCD,TTJets,ST,GJets,EWK,nType};
static const char * TypeName[] = {"Data","Signal","ZJets","ZJets_NLO","WJets","WJets_NLO","DYJets","DYJets_NLO","QCD","TTJets","ST","GJets","EWK"};

enum Region{SR,WE,WM,ZE,ZM,GA,nRegion};
static const char * RegionName[] = {"SignalRegion","SingleElectron","SingleMuon","DoubleElectron","DoubleMuon","Gamma"};

#endif

#define monoJetCommon_cxx
#include "monoJetCommon.h"

void monoJetCommon::QCDVariations(double event_weight) {
  /* 14 Histograms
     up  dn
     d1K_NLO     11  12
     d2K_NLO     13  14
     d3K_NLO     15  16
     d1K_EW      23  24
     d2K_EW      25  26
     d3K_EW      27  28
     dK_NLO_mix  29  30 
  */
  string uncnames[7] = {"QCD_Scale","QCD_Shape","QCD_Proc","NNLO_EWK","NNLO_Miss","NNLO_Sud","QCD_EWK_Mix"};
  // Initializing
  if ( !scaleUncs->contains(uncnames[0]) ) {
    string hnames[7] = {"d1K_NLO","d2K_NLO","d3K_NLO","d1kappa_EW","d2kappa_EW","d3kappa_EW","dK_NLO_mix"};
    TFile* file = NULL;
    string prefix = "";
    if (sample.isW_or_ZJet()) {
      if (sample.type == WJets) {
	file = TFile::Open("RootFiles/WJets_NLO_EWK.root");
	prefix = "evj_pTV_";
      } else if (sample.type == ZJets) {
	file = TFile::Open("RootFiles/ZJets_NLO_EWK.root");
	prefix = "vvj_pTV_";
      } else if (sample.type == DYJets) {
	file = TFile::Open("RootFiles/DYJets_NLO_EWK.root");
	prefix = "eej_pTV_";
      }
      histomap["K_NLO_QCD"] = (TH1F*)file->Get( (prefix+"K_NLO").c_str() );
      histomap["K_EW"]      = (TH1F*)file->Get( (prefix+"kappa_EW").c_str() );
    }
    
    for (int i = 0; i < 7; i++) {
      string name = uncnames[i];
      TH1F* histo = NULL;
      if (sample.isW_or_ZJet()) histo = (TH1F*)file->Get( (prefix+hnames[i]).c_str() );
      scaleUncs->addUnc(name,histo);
    }
  }
  
  for (int i = 0; i < 7; i++) {
    string name = uncnames[i];
    float weightUp = event_weight;
    float weightDn = event_weight;
    
    if (sample.isW_or_ZJet()) {
      float unc = scaleUncs->getBin(name,bosonPt);
      float k_qcd = histomap.getBin("K_NLO_QCD",bosonPt);
      float k_ewk = histomap.getBin("K_EW",bosonPt);
      if ( name.find("NNLO") != string::npos || name.find("EWK") != string::npos ) unc *= k_ewk;
      else if ( name.find("QCD") != string::npos ) unc *= k_qcd;
      weightUp += unc;
      weightDn -= unc;
    }
    scaleUncs->setUnc(name,weightUp,weightDn);
  }
}

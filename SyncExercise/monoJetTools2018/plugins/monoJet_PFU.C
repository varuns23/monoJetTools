#define monoJetCommon_cxx
#include "monoJetCommon.h"

void monoJetCommon::PFUncertainty(double event_weight) {
  // 6 Histograms
  //   Trk ECAL HCAL
  //up  0   1    2
  //dn  3   4    5
  string pf_uncs[3] = {"tracker","ecal","hcal"};
  string uncname = "PFU_";
  if ( !shapeUncs->contains(uncname+pf_uncs[0]) ) {
    for (int i = 0; i < 3; i++) {
      string name = uncname + pf_uncs[i];
      shapeUncs->addUnc(name);
      
      initTree(shapeUncs->getTreeUp(name));
      initTree(shapeUncs->getTreeDn(name));
    }
  }
  
  vector<double> j1PFConsPtNorm;
  vector<double> j1PFConsPtUnc;

  double jetPtNorm = jetPt->at(jetCand[0]);
  for(int i=0;i<j1PFConsPID.size();i++) {
    j1PFConsPtNorm.push_back(j1PFConsPt[i]);
    if ( abs(j1PFConsPID[i]) == 211 || abs(j1PFConsPID[i]) == 13 ) {
      //Tracker Uncertainty
      //deltaPt=(1/100)*sqrt((0.015*Pt)^2+(0.5)^2)
      double deltaPt = (1/100.)*sqrt(pow(0.015*j1PFConsPt[i],2)+pow(0.5,2));
      j1PFConsPtUnc.push_back(deltaPt);
      h_TrackerPtUnc->Fill(j1PFConsPtNorm[i],j1PFConsPtUnc[i]);
      // cout << "Tracker Unc: " << deltaPt << endl;
    }
    else if ( abs(j1PFConsPID[i]) == 22 || abs(j1PFConsPID[i]) == 11 ) {
      //ECAL Uncertainty
      //deltaPt=(1/100)*sqrt((2.8)^2/Pt+(12.8/Pt)^2+(0.3)^2)
      double deltaPt = (1/100.)*sqrt(pow(2.8,2)/j1PFConsPt[i]+pow(12.8/j1PFConsPt[i],2)+pow(0.3,2));
      j1PFConsPtUnc.push_back(deltaPt);
      h_EcalPtUnc->Fill(j1PFConsPtNorm[i],j1PFConsPtUnc[i]);
      // cout << "ECAL Unc: " << deltaPt << endl;
    }
    else if ( abs(j1PFConsPID[i]) == 130 ) {
      //HCAL Uncertainty
      //deltaPt=(1/100)*sqrt((115)^2/Pt+(5.5)^2)
      double deltaPt = (1/100.)*sqrt(pow(115,2)/j1PFConsPt[i]+pow(5.5,2));
      j1PFConsPtUnc.push_back(deltaPt);
      h_HcalPtUnc->Fill(j1PFConsPtNorm[i],j1PFConsPtUnc[i]);
      // cout << "HCAL Unc: " << deltaPt << endl;
    }
    else {
      j1PFConsPtUnc.push_back(0);
    }
  }
  
  // cout << "ChNemPtFrac:" << endl;
  // cout << "\tNorm: " << ChNemPtFrac << endl;
  weight = event_weight;
  int UncType[2] = {1,-1};
  for (int i = 0; i < 2; i++) {
    int variation = UncType[i];
    // Tracker
    jetPt->at(jetCand[0]) = 0;
    j1PFConsPt.clear();
    for (int j = 0; j < j1PFConsPID.size(); j++) {
      if ( ( abs(j1PFConsPID[j]) == 221 || abs(j1PFConsPID[j]) == 13 ) )
	j1PFConsPt.push_back( j1PFConsPtNorm[j]*(1 + variation*j1PFConsPtUnc[j]) );
      else j1PFConsPt.push_back(j1PFConsPtNorm[j]);
      jetPt->at(jetCand[0]) += j1PFConsPt[j];
    }
    getPt123Frac();
    if (variation == 1)  shapeUncs->fillUp(uncname+"tracker");
    if (variation == -1) shapeUncs->fillDn(uncname+"tracker");
    // cout << "\tTracker " << (variation == 1 ? "Up " : "Down ") << ChNemPtFrac << endl;
    
    // ECAL
    jetPt->at(jetCand[0]) = 0;
    j1PFConsPt.clear();
    for (int j = 0; j < j1PFConsPID.size(); j++) {
      if ( ( abs(j1PFConsPID[j]) == 22 || abs(j1PFConsPID[j]) == 11 ) ) 
	j1PFConsPt.push_back( j1PFConsPtNorm[j]*(1 + variation*j1PFConsPtUnc[j]) );
      else j1PFConsPt.push_back(j1PFConsPtNorm[j]);
      jetPt->at(jetCand[0]) += j1PFConsPt[j];
    }
    getPt123Frac();
    if (variation == 1)  shapeUncs->fillUp(uncname+"ecal");
    if (variation == -1) shapeUncs->fillDn(uncname+"ecal");
    // cout << "\tECAL " << (variation == 1 ? "Up " : "Down ") << ChNemPtFrac << endl;
    
    // HCAL
    jetPt->at(jetCand[0]) = 0;
    j1PFConsPt.clear();
    for (int j = 0; j < j1PFConsPID.size(); j++) {
      if ( ( abs(j1PFConsPID[j]) == 130 ) )
	j1PFConsPt.push_back( j1PFConsPtNorm[j]*(1 + variation*j1PFConsPtUnc[j]) );
      else j1PFConsPt.push_back(j1PFConsPtNorm[j]);
      jetPt->at(jetCand[0]) += j1PFConsPt[j];
    }
    getPt123Frac();
    if (variation == 1)  shapeUncs->fillUp(uncname+"hcal");
    if (variation == -1) shapeUncs->fillDn(uncname+"hcal");
    // cout << "\tHCAL " << (variation == 1 ? "Up " : "Down ") << ChNemPtFrac << endl;
  }
  
  jetPt->at(jetCand[0]) = jetPtNorm;
  j1PFConsPt.clear();
  for (double consPt : j1PFConsPtNorm) j1PFConsPt.push_back(consPt);
  getPt123Frac();
  // cout << "\tReset: " << ChNemPtFrac << endl;
}//Closing the Loop function

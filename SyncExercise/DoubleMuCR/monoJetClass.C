#define monoJetClass_cxx
#include "monoJetClass.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include <TLorentzVector.h>
#include <TMath.h>
#include <iostream>
#include <vector>

using namespace std;

int main(int argc, const char* argv[]) { 
   if (argc == 1) {
      printf("Running Test\n");
      argv[1] = "/hdfs/store/user/ekoenig/MonoZprimeJet/NTuples/2018/MC2018_Autumn18_June2019/DYJets/DYJetsToLL_M-50_HT-400to600_TuneCP5_PSweights_13TeV-madgraphMLM-pythia8/0000/";
      argv[2] = "test.root";
      argv[3] = "5000";
      argv[4] = "100";
      argv[5] = "1-1";
   }
   Long64_t maxEvents = atof(argv[3]);
   if (maxEvents < -1LL)
   {
      cout<<"Please enter a valid value for maxEvents (parameter 3)."<<endl;
      return 1;
   }
   int reportEvery = atof(argv[4]);
   if (reportEvery < 1)
   {
      cout<<"Please enter a valid value for reportEvery (parameter 4)."<<endl;
      return 1;
   }
   monoJetClass t(argv[1],argv[2],argv[5]);
   t.Loop(maxEvents,reportEvery);
   return 0;
}

void monoJetClass::Loop(Long64_t maxEvents, int reportEvery) {
   if (fChain == 0) return;

   Long64_t nentries = fChain->GetEntries();
   cout<<"Coming in:"<<endl;
   cout<<"nentries:"<<nentries<<endl;
   Long64_t nentriesToCheck = nentries;

   int nTotal = 0;
   int nTotalEvents = 0;
   int nTwoMu = 0;
   int nOneTightMu = 0;
   int nDiMuCharge = 0;
   int nDiMuMass = 0;
   int nEleVeto = 0;
   int nPhoVeto = 0;
   int nTauVeto = 0;
   int nBtagVeto = 0;
   int nMinDphiJR = 0;
   int nMetCut = 0;
   int nRecoil = 0;
   int nJetCand = 0;
   int nJetVeto = 0;


   if (maxEvents != -1LL && nentries > maxEvents)
     nentriesToCheck = maxEvents;
   nTotal = nentriesToCheck;
   cout<<"Running over "<<nTotal<<" events."<<endl;
   Long64_t nbytes = 0, nb = 0;

   for (Long64_t jentry=0; jentry<nentriesToCheck;jentry++) {
     Long64_t ientry = LoadTree(jentry);
     if (ientry < 0) break;
     nb = fChain->GetEntry(jentry);   nbytes += nb;

     jetCand     .clear();

     vector<int> jetlist = getJetCand(30.0, 2.4, 0.8, 0.1);

     lepindex_leading = -1;
     lepindex_subleading = -1;
     dilepton_pt = dilepton_mass = Recoil=-99;

     nTotalEvents++;

     vector<int> mulist = muon_veto_looseID(0, 0, 0, 10.0);
     if(mulist.size() == 2){
       nTwoMu++;
       vector<int> mulist_leading = muon_veto_tightID(0, 20.0);

       if(mulist_leading.size() > 0){
	 nOneTightMu++;

	 bool muPairSet = false;
	 TLorentzVector e1, e2;
	 for(int j=0; j<mulist.size(); ++j){

	   //Event must have exactly two muons with opposite charge
	   if(muCharge->at(mulist_leading[0])*muCharge->at(mulist[j]) == -1){

	     e1.SetPtEtaPhiE(muPt->at(mulist_leading[0]), muEta->at(mulist_leading[0]), muPhi->at(mulist_leading[0]), muE->at(mulist_leading[0]));
	     e2.SetPtEtaPhiE(muPt->at(mulist[j]), muEta->at(mulist[j]), muPhi->at(mulist[j]), muE->at(mulist[j]));
	     lepindex_leading = mulist_leading[0];
	     lepindex_subleading = mulist[j];
	     muPairSet = true;
	   }
	 }

	 if(muPairSet){
	   nDiMuCharge++;

	   TLorentzVector ll = e1+e2;
	   dilepton_mass = ll.M();
	   dilepton_pt = ll.Pt();

	   if(dilepton_mass > 60. && dilepton_mass < 120.){
	     nDiMuMass++;

	     vector<int> elelist = electron_veto_looseID(0, lepindex_leading, lepindex_subleading, 10.0);
	     if(elelist.size() == 0){
	       nEleVeto++;

	       vector<int> pholist = pho_veto_looseID(lepindex_leading,lepindex_subleading,15.0);
	       if(pholist.size() == 0){ 
		 nPhoVeto++;

		 vector<int> taulist = tau_veto(lepindex_leading,lepindex_subleading);
		 if(taulist.size() == 0){
		   nTauVeto++;

		   vector<int> bjetlist = bjet_veto(lepindex_leading,lepindex_subleading);
		   if(bjetlist.size() == 0){
		     nBtagVeto++;

		     TLorentzVector met_4vec;
		     met_4vec.SetPtEtaPhiE(pfMET,0.,pfMETPhi,pfMET);
		     TLorentzVector leptoMET_4vec = ll+met_4vec;
		     Double_t leptoMET = fabs(leptoMET_4vec.Pt());
		     Double_t leptoMET_phi = leptoMET_4vec.Phi();
		     Recoil = leptoMET;

		     double minDPhiJetMET_first4 = TMath::Pi();
		     bool mindphijr = getMinDphiJR(lepindex_leading, lepindex_subleading, leptoMET_phi);
		     if(mindphijr){
		       nMinDphiJR++;

		       double metcut = (fabs(pfMET-caloMET))/Recoil;
		       if(metcut < 0.5){
			 nMetCut++;

			 if(Recoil > 250.){
			   nRecoil++;

			   jetCand = getJetCand(100,2.4,0.8,0.1);
			   if(jetCand.size()>0){
			     nJetCand++;
			     if(JetVetoDecision(jetCand[0], lepindex_leading,lepindex_subleading)){
			       nJetVeto++;
			     }
			   } //jet cand
			 } // recoil
		       } //metcut
		     } //mindphijr
		   } //b-veto
		 } //tau veto
	       } //pho veto
	     } //Ele veto
	   } //dilepton mass
	 } // dileptons with opposite charge
       }
     }

     if (jentry%reportEvery == 0){
       cout<<"Finished entry "<<jentry<<"/"<<(nentriesToCheck-1)<<endl;
     }
   }

   h_cutflow->SetBinContent(1,nTotalEvents); 
   h_cutflow->SetBinContent(2,nTwoMu);
   h_cutflow->SetBinContent(3,nOneTightMu);
   h_cutflow->SetBinContent(4,nDiMuCharge);
   h_cutflow->SetBinContent(5,nDiMuMass);
   h_cutflow->SetBinContent(6,nEleVeto);
   h_cutflow->SetBinContent(7,nPhoVeto);
   h_cutflow->SetBinContent(8,nTauVeto);
   h_cutflow->SetBinContent(9,nBtagVeto);
   h_cutflow->SetBinContent(10,nMinDphiJR);
   h_cutflow->SetBinContent(11,nMetCut);
   h_cutflow->SetBinContent(12,nRecoil);
   h_cutflow->SetBinContent(13,nJetCand);
   h_cutflow->SetBinContent(14,nJetVeto);

}//Closing the Loop function

void monoJetClass::initTree(TTree* tree) {
  tree->Branch("weight",&weight);
  tree->Branch("ChNemPtFrac",&ChNemPtFrac,"Ch + NEM P_{T}^{123} Fraction");
  tree->Branch("h_recoil",&Recoil,"Recoil (GeV)");
  tree->Branch("jetPt",&l_jetPt,"Leading Jet P_{T} (GeV)");
  tree->Branch("ChNemPt",&ChNemPt,"Ch + NEM Leading Jet P_{T} (GeV)");
}

void monoJetClass::BookHistos(const char* outputFilename) {

  output = new TFile(outputFilename, "RECREATE");
  output->cd();

  h_cutflow = new TH1D("h_cutflow","h_cutflow",14,0,14);h_cutflow->Sumw2();
  h_cutflow->GetXaxis()->SetBinLabel(1,"Total Events");
  h_cutflow->GetXaxis()->SetBinLabel(2,"Two Mu");
  h_cutflow->GetXaxis()->SetBinLabel(3,"One Tight Mu");
  h_cutflow->GetXaxis()->SetBinLabel(4,"DiLep Charge");
  h_cutflow->GetXaxis()->SetBinLabel(5,"DiLep Mass"); 
  h_cutflow->GetXaxis()->SetBinLabel(6,"Ele Veto");
  h_cutflow->GetXaxis()->SetBinLabel(7,"Pho Veto");
  h_cutflow->GetXaxis()->SetBinLabel(8,"Tau Veto");
  h_cutflow->GetXaxis()->SetBinLabel(9,"B Veto");
  h_cutflow->GetXaxis()->SetBinLabel(10,"MinDphiJR");
  h_cutflow->GetXaxis()->SetBinLabel(11,"DpFCalo");
  h_cutflow->GetXaxis()->SetBinLabel(12,"Recoil");
  h_cutflow->GetXaxis()->SetBinLabel(13,"Jet Selection");
  h_cutflow->GetXaxis()->SetBinLabel(14,"Jet veto");

  BookCommon(-1,"");
}

void monoJetClass::fillHistos(int histoNumber,double event_weight){
  fillCommon(histoNumber,event_weight);
  //CR Histograms
  if (histoNumber == bHisto) tree->Fill();
}
bool monoJetClass::JetVetoDecision(int jetindex, int leading_lep_index, int subleading_lep_index) {

  bool jetVeto=false;


  float dR_leading_mu = deltaR(jetEta->at(jetindex), jetPhi->at(jetindex), muEta->at(leading_lep_index),  muPhi->at(leading_lep_index));
  float dR_subleading_mu = deltaR(jetEta->at(jetindex), jetPhi->at(jetindex), muEta->at(subleading_lep_index), muPhi->at(subleading_lep_index));

  //   float dR_muon   = 0.;
  //   float dR_photon = 0.;

  if(dR_leading_mu > 0.4 && dR_subleading_mu > 0.4)
    jetVeto = true;


  return jetVeto;
}

vector<int> monoJetClass::muon_veto_tightID(int jet_index,  float muPtCut){
  // bool veto_passed = true; //pass veto if no good muon found
  vector<int> mu_cands;
  mu_cands.clear();

  for(int i = 0; i < nMu; i++){
    //muon pt and eta cut
    if(muPt->at(i) > muPtCut && (fabs(muEta->at(i)) < 2.4)){
      // muon tight ID and Iso
      if(muIDbit->at(i)>>3&1 == 1 && muIDbit->at(i)>>9&1 == 1){

	mu_cands.push_back(i);
      }
    }
  }
  return mu_cands;
}

//Veto failed if a muon is found that passes Loose Muon ID, Loose Muon Isolation, and muPtcut, and does not overlap the candidate photon within dR of 0.5
vector<int> monoJetClass::muon_veto_looseID(int jet_index, int leading_ele_index, int subleading_ele_index, float muPtCut){
  vector<int> mu_cands;
  mu_cands.clear();

  for(int i = 0; i < nMu; i++){
    // mu pt and eta cut
    if(muPt->at(i) > muPtCut && (fabs(muEta->at(i)) < 2.4)){
      if(muIDbit->at(i)>>0&1 == 1 && muIDbit->at(i)>>7&1==1){

	mu_cands.push_back(i);
      }
    }
  }  
  return mu_cands;
}

vector<int> monoJetClass::electron_veto_looseID(int jet_index, int leading_mu_index, int subleading_mu_index, float elePtCut){
  vector<int> ele_cands;
  ele_cands.clear();
  float ele_dz  = 0.0;
  float ele_dxy = 0.0;

  for(int i = 0; i < nEle; i++){
    if(elePt->at(i) > elePtCut && fabs(eleSCEta->at(i)) < 2.5){
      //Electron passes veto Electron ID cuts
      if(eleIDbit->at(i)>>3&1 == 1){

	//Electron does not overlap muon 
	double dR_leading = deltaR(eleSCEta->at(i),eleSCPhi->at(i),muEta->at(leading_mu_index),muPhi->at(leading_mu_index));
	double dR_subleading = deltaR(eleSCEta->at(i),eleSCPhi->at(i),muEta->at(subleading_mu_index),muPhi->at(subleading_mu_index));

	if(dR_leading > 0.4 && dR_subleading > 0.4 ){
	  //Ele DZ and D0 selection
	  if ((fabs(eleSCEta->at(i)) <= 1.479) && (fabs(eleD0->at(i)) < 0.05) && (fabs(eleDz->at(i)) < 0.1 )) {
	    ele_cands.push_back(i);
	  }
	  else if( (fabs(eleSCEta->at(i)) > 1.479) && (fabs(eleD0->at(i)) < 0.1) && (fabs(eleDz->at(i)) < 0.2 )){
	    ele_cands.push_back(i);
	  }
	}
      }
    }
  }
  return ele_cands;
}

vector<int> monoJetClass::pho_veto_looseID(int leading_mu_index, int subleading_mu_index, float phoPtCut){
  vector<int> pho_cands;
  pho_cands.clear();

  for(int i = 0; i < nPho; i++){
    // passes pt cut
    if((phoEt->at(i) > phoPtCut) && (fabs(phoSCEta->at(i)) < 2.5)){
      if(phoIDbit->at(i)>>0&1==1){

	double dR_leadingMu    = deltaR(phoSCEta->at(i),phoSCPhi->at(i), muEta->at(leading_mu_index), muPhi->at(leading_mu_index));  
	double dR_subleadingMu = deltaR(phoSCEta->at(i),phoSCPhi->at(i), muEta->at(subleading_mu_index), muPhi->at(subleading_mu_index)); 
	if( dR_leadingMu > 0.5 && dR_subleadingMu > 0.5)
	  pho_cands.push_back(i);
      }
    }
  }
  return pho_cands;
}

vector<int> monoJetClass::tau_veto(int leading_mu_index, int subleading_mu_index){
  vector<int> tau_cands;
  tau_cands.clear();

  for(int i = 0; i < nTau; i++){
    if((tau_Pt->at(i) > 18.0) && (fabs(tau_Eta->at(i)) < 2.3)){

      if(((tau_IDbits->at(i)>>0&1) == 1) && ((tau_IDbits->at(i)>>13&1) == 1)){

	double dR_leadingMu    = deltaR(tau_Eta->at(i), tau_Phi->at(i), muEta->at(leading_mu_index), muPhi->at(leading_mu_index));  
	double dR_subleadingMu = deltaR(tau_Eta->at(i), tau_Phi->at(i), muEta->at(subleading_mu_index), muPhi->at(subleading_mu_index)); 
	if( dR_leadingMu > 0.4 && dR_subleadingMu > 0.4)
	  tau_cands.push_back(i);
      }
    }
  }
  return tau_cands;
}

vector<int> monoJetClass::bjet_veto(int leading_mu_index, int subleading_mu_index){
  vector<int> bjet_cands;
  bjet_cands.clear();

  for(int i = 0; i < nJet; i++){
    if((jetPt->at(i) > 20.0) && (fabs(jetEta->at(i)) < 2.4)){
      if(jetID->at(i)>>0&1 == 1){
	if((jetDeepCSVTags_b->at(i) + jetDeepCSVTags_bb->at(i)) > 0.4184){

	  double dR_leadingMu    = deltaR(jetEta->at(i), jetPhi->at(i), muEta->at(leading_mu_index), muPhi->at(leading_mu_index));  
	  double dR_subleadingMu = deltaR(jetEta->at(i), jetPhi->at(i), muEta->at(subleading_mu_index), muPhi->at(subleading_mu_index)); 
	  if( dR_leadingMu > 0.4 && dR_subleadingMu > 0.4)
	    bjet_cands.push_back(i);
	}
      }
    }
  }
  return bjet_cands;
}

bool monoJetClass::getMinDphiJR(int lead_lep_index, int sublead_lep_index, double lepMET_phi){
  bool decision = false;
  
  vector<int> tmpJetlist;
  tmpJetlist.clear();

  for(int i=0; i<nJet; i++){ 

    bool kinematic = (jetPt->at(i) > 30. && fabs(jetEta->at(i)) < 2.4);
    bool tightJetID = false;   
    if (jetID->at(i)>>0&1 == 1) tightJetID = true;
     
    double dR_leadLep    = deltaR(jetEta->at(i), jetPhi->at(i), muEta->at(lead_lep_index), muPhi->at(lead_lep_index));  
    double dR_subLeadLep = deltaR(jetEta->at(i), jetPhi->at(i), muEta->at(sublead_lep_index), muPhi->at(sublead_lep_index)); 

    if(kinematic && tightJetID && dR_leadLep > 0.4 && dR_subLeadLep > 0.4)
      tmpJetlist.push_back(i);
  }

  int count=0;
  for(int k=0; k < tmpJetlist.size(); k++){
    if(DeltaPhi(jetPhi->at( tmpJetlist.at(k) ), lepMET_phi) > 0.5) 
      count++;
  }
  
  if(count >= 4 || count == tmpJetlist.size()) 
    decision = true;

  return decision;
}

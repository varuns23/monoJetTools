#define monoJetClass_cxx
#include "monoJetClass.h"

using namespace std;

int main(int argc, const char* argv[]) { 
  if (argc == 1) {
    printf("Running Test\n");
    argv[1] = "/hdfs/store/user/varuns/NTuples/MC/MC2018_RunIIAutumn18_102X_JECv19/ZJets/ZJetsToNuNu_HT400-600/0000/";
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
  int nOneLoosePho = 0;
  int nOnePho = 0;
  int nPhoPt = 0;
  int nEleVeto = 0;
  int nMuVeto = 0;
  int nTauVeto = 0;
  int nBtagVeto = 0;
  int nMinDphiJR = 0;
  int npfCaloCut = 0;
  int nRecoil = 0;
  int nJetCand = 0;
  int nJetVeto = 0;


  if (maxEvents != -1LL && nentries > maxEvents)
    nentriesToCheck = maxEvents;
  nTotal = nentriesToCheck;
  Long64_t nbytes = 0, nb = 0;
  cout<<"Running over "<<nTotal<<" events."<<endl;
  for (Long64_t jentry=0; jentry<nentriesToCheck;jentry++) {
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   nbytes += nb;

    
    jetCand     .clear(); 
    nTotalEvents++;   

    int loose_pho = pho_looseID(15.0);
    if(loose_pho != -1){
      nOneLoosePho++;

      int tight_pho = pho_tightID(loose_pho, 25.0); 
      if(tight_pho != -1){
	nOnePho++;

	int lepindex = tight_pho;
	if( phoEt->at(lepindex) > 175 ){
	  nPhoPt++;
		 
	  TLorentzVector lep_4vec;
	  lep_4vec.SetPtEtaPhiE(phoEt->at(lepindex),phoEta->at(lepindex),phoPhi->at(lepindex),phoE->at(lepindex));
	  lepton_pt = lep_4vec.Pt();
	  TLorentzVector met_4vec;
	  met_4vec.SetPtEtaPhiE(pfMET,0.,pfMETPhi,pfMET);
	  TLorentzVector leptoMET_4vec = lep_4vec + met_4vec;
	  double leptoMET = fabs(leptoMET_4vec.Pt());
	  double leptoMETphi = leptoMET_4vec.Phi();
	  Recoil = leptoMET;

	    bool vetoElectrons = ele_veto_looseID(0, lepindex, 15.);
	    if(vetoElectrons){
	      nEleVeto++;
	      
	      bool vetoMuons = muon_veto_looseID(0, lepindex, 10.);
	      if(vetoMuons){
		nMuVeto++;

		bool vetoTaus = tau_veto(0, lepindex);
		if(vetoTaus){
		  nTauVeto++;

		  bool vetoBjets = bjet_veto(lepindex);
		  if(vetoBjets){
		    nBtagVeto++;

		    bool mindphijr = getMinDphiJR(lepindex, leptoMETphi);

		    if(mindphijr){
		      nMinDphiJR++;

		      double metcut = (fabs(pfMET-caloMET))/Recoil;
		      if(metcut < 0.5){
			npfCaloCut++;

			if(Recoil > 250.){
			  nRecoil++;

			  jetCand = getJetCand(100,2.4,0.8,0.1);
			  if(jetCand.size()>0){
			    nJetCand++;

			    bool isoJet = JetVetoDecision(jetCand[0], lepindex);
			    if(isoJet){
			      nJetVeto++;

			    }//nJetVeto 
			  } //nJetCand
			} //Recoil
		      }// npfCaloCut
		    }// mindphijr
		  } //bjet veto
		}// tau veto
	      }// veto photons
	    }//veto muons
	}//Photon Pt cut
      }// one tight pho
    }//one loose pho

    if (jentry%reportEvery == 0)
      cout<<"Finished entry "<<jentry<<"/"<<(nentriesToCheck - 1)<<endl;
  }
   
  h_cutflow->SetBinContent(1,nTotalEvents);
  h_cutflow->SetBinContent(2,nOneLoosePho);
  h_cutflow->SetBinContent(3,nOnePho);
  h_cutflow->SetBinContent(4,nPhoPt);
  h_cutflow->SetBinContent(5,nEleVeto);
  h_cutflow->SetBinContent(6,nMuVeto);
  h_cutflow->SetBinContent(7,nTauVeto);
  h_cutflow->SetBinContent(8,nBtagVeto);
  h_cutflow->SetBinContent(9,nMinDphiJR);
  h_cutflow->SetBinContent(10,npfCaloCut);
  h_cutflow->SetBinContent(11,nRecoil);
  h_cutflow->SetBinContent(12,nJetCand);
  h_cutflow->SetBinContent(13,nJetVeto);
}

void monoJetClass::initTree(TTree* tree) {
}

void monoJetClass::BookHistos(const char* outputFilename) {

  output = new TFile(outputFilename, "RECREATE");
  output->cd();

  h_cutflow = new TH1D("h_cutflow","h_cutflow",14,0,14);h_cutflow->Sumw2();
  h_cutflow->GetXaxis()->SetBinLabel(1,"Total Events");
  h_cutflow->GetXaxis()->SetBinLabel(2,"1-lpho");
  h_cutflow->GetXaxis()->SetBinLabel(3,"1-tpho");
  h_cutflow->GetXaxis()->SetBinLabel(4,"PhoPtcut");
  h_cutflow->GetXaxis()->SetBinLabel(5,"eleVeto");
  h_cutflow->GetXaxis()->SetBinLabel(6,"muVeto"); 
  h_cutflow->GetXaxis()->SetBinLabel(7,"tauVeto");
  h_cutflow->GetXaxis()->SetBinLabel(8,"btagVeto");
  h_cutflow->GetXaxis()->SetBinLabel(9,"minDPhiJR");
  h_cutflow->GetXaxis()->SetBinLabel(10,"pfcaloCut");
  h_cutflow->GetXaxis()->SetBinLabel(11,"Recoil");
  h_cutflow->GetXaxis()->SetBinLabel(12,"JetCand");
  h_cutflow->GetXaxis()->SetBinLabel(13,"JetVeto");
}

int monoJetClass::pho_looseID(float phoEtCut) {
  vector<int> tmpcands; tmpcands.clear();

  for (int i = 0; i < nPho; i++) {
    bool kinematics = phoEt->at(i) > phoEtCut && fabs(phoEta->at(i)) < 2.5;
    bool IdandIso   = (phoIDbit->at(i)>>0&1==1);
    bool eleVeto = phoEleVeto->at(i);
    if (kinematics && IdandIso && eleVeto)
      tmpcands.push_back(i);
  }

  if (tmpcands.size() == 1)
    return tmpcands[0];
  return -1;
}

int monoJetClass::pho_tightID(int loosePho,float phoEtCut) {
  
  bool kinematics = phoEt->at(loosePho) > phoEtCut && fabs(phoEta->at(loosePho)) < 1.4442;
  bool IdandIso   = (phoIDbit->at(loosePho)>>1&1==1);
  bool eleVeto = phoEleVeto->at(loosePho);
  if (kinematics && IdandIso && eleVeto)
    return loosePho;
  return -1;
}

bool monoJetClass::JetVetoDecision(int jetindex, int lep_index) {

  bool jetVeto=false;

  float dR_lep = deltaR(jetEta->at(jetindex), jetPhi->at(jetindex), phoEta->at(lep_index), phoPhi->at(lep_index));

  //   float dR_muon   = 0.;
  //   float dR_photon = 0.;

  if(dR_lep > 0.4)
    jetVeto = true;

  return jetVeto;
}

bool monoJetClass::muon_veto_looseID(int jet_index, int pho_index, float muPtCut){
  bool decision = true;
  int countMu = 0; 

  for(int i = 0; i < nMu; i++) {
    bool kinematics = (muPt->at(i) > muPtCut && (fabs(muEta->at(i)) < 2.4));
    bool IdandIso   = (muIDbit->at(i)>>0&1==1 && muIDbit->at(i)>>7&1==1); //loose Id and loose Iso

    //Muon overlap with pho
    double dR_pho = deltaR(muEta->at(i),muPhi->at(i),phoEta->at(pho_index),phoPhi->at(pho_index)); 

    if(kinematics && IdandIso && dR_pho > 0.4)
      countMu++;
  }

  if(countMu > 0)
    decision = false;

  return decision;
}

bool monoJetClass::ele_veto_looseID(int jet_index, int lep_index, float elePtCut){
  
  for(int i = 0; i < nEle; i++){
    if(elePt->at(i) > elePtCut && fabs(eleSCEta->at(i)) < 2.5){
      //Electron passes veto Electron ID cuts
      if(eleIDbit->at(i)>>3&1 == 1){

	//Electron does not overlap phoon 
	double dR_Pho = deltaR(eleSCEta->at(i),eleSCPhi->at(i),phoEta->at(lep_index),phoPhi->at(lep_index));

	if(dR_Pho > 0.5 ){
	  //Ele DZ and D0 selection
	  if ((fabs(eleSCEta->at(i)) <= 1.479) && (fabs(eleD0->at(i)) < 0.05) && (fabs(eleDz->at(i)) < 0.1 )) {
	    return false;
	  }
	  else if( (fabs(eleSCEta->at(i)) > 1.479) && (fabs(eleD0->at(i)) < 0.1) && (fabs(eleDz->at(i)) < 0.2 )){
	    return false;
	  }
	}
      }
    }
  }
  return true;
}

bool monoJetClass::tau_veto(int jet_index, int lep_index){
  bool decision = true;
  int countTaus = 0;

  for(int i = 0; i < nTau; i++){
    bool kinematics = (tau_Pt->at(i) > 18.0 && fabs(tau_Eta->at(i)) < 2.3);
    bool IdandIso   = ((tau_IDbits->at(i)>>0&1) == 1 && (tau_IDbits->at(i)>>13&1) == 1);

    double dR_Pho   = deltaR(tau_Eta->at(i), tau_Phi->at(i), phoEta->at(lep_index), phoPhi->at(lep_index));  

    if( kinematics && IdandIso && dR_Pho > 0.4)
      countTaus++;
  }
  if(countTaus>0)
    decision = false;

  return decision;
}

bool monoJetClass::bjet_veto(int lep_index){
  bool decision = true;
  int countBjet = 0;

  for(int i = 0; i < nJet; i++){
    bool kinematics = (jetPt->at(i) > 20.0) && (fabs(jetEta->at(i)) < 2.4);
    bool Id   = jetID->at(i)>>0&1 == 1;
    bool btag = (jetDeepCSVTags_b->at(i) + jetDeepCSVTags_bb->at(i)) > 0.4184;

    double dR_Pho    = deltaR(jetEta->at(i), jetPhi->at(i), phoEta->at(lep_index), phoPhi->at(lep_index));  

    if(kinematics && Id && btag && dR_Pho > 0.4)
      countBjet++;
  }
  if(countBjet > 0)
    decision = false;

  return decision;
}


bool monoJetClass::getMinDphiJR(int lep_index, double lepMET_phi){
  bool decision = false;

  vector<int> tmpJetlist;
  tmpJetlist.clear();

  for(int i=0; i<nJet; i++){ 
    bool kinematic = (jetPt->at(i) > 30. && fabs(jetEta->at(i)) < 2.4);
    bool tightJetID = (jetID->at(i)>>0&1 == 1);

    double dR_Lep    = deltaR(jetEta->at(i), jetPhi->at(i), phoEta->at(lep_index), phoPhi->at(lep_index));  

    if(kinematic && tightJetID && dR_Lep > 0.4 )
      tmpJetlist.push_back(i);
  }

  int count=0;
  for(int k=0; k < tmpJetlist.size(); k++){
    int jc = tmpJetlist.at(k);
    if(DeltaPhi(jetPhi->at(jc), lepMET_phi) > 0.5) 
      count++;
  }

  if(count >= 4 || count == tmpJetlist.size()) 
    decision = true;

  return decision;
}

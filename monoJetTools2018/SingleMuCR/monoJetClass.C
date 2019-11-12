#define monoJetClass_cxx
#include "monoJetClass.h"

using namespace std;

int main(int argc, const char* argv[]) { 
  if (argc == 1) {
    printf("Running Test\n");
    argv[1] = "/hdfs/store/user/ekoenig/MonoZprimeJet/NTuples/2018/MC2018_Autumn18_June2019/WJets/WJetsToLNu_HT-400To600_TuneCP5_13TeV-madgraphMLM-pythia8/0000/";
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
  int nOneLooseMu = 0;
  int nOneMu = 0;
  int nMT = 0;
  int nEleVeto = 0;
  int nPhoVeto = 0;
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
  cout<<"Running over "<<nTotal<<" events."<<endl;
  Long64_t nbytes = 0, nb = 0;
  for (Long64_t jentry=0; jentry<nentriesToCheck;jentry++) {
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   nbytes += nb;

    jetCand     .clear();
    lepindex = -1;

    nTotalEvents++;   

    int losse_mu = mu_looseID(10.0);
    if(losse_mu != -1){
      nOneLooseMu++;

      int tight_mu = mu_tightID(losse_mu, 20.0);
      if(tight_mu != -1){
	nOneMu++;

	int lepindex = tight_mu;
	TLorentzVector lep_4vec;
	lep_4vec.SetPtEtaPhiE(muPt->at(lepindex),muEta->at(lepindex),muPhi->at(lepindex),muE->at(lepindex));
	lepton_pt = lep_4vec.Pt();
	TLorentzVector met_4vec;
	met_4vec.SetPtEtaPhiE(pfMET,0.,pfMETPhi,pfMET);
	TLorentzVector leptoMET_4vec = lep_4vec + met_4vec;
	Recoil = fabs(leptoMET_4vec.Pt());
	double leptoMET_phi = leptoMET_4vec.Phi();
	  
	float dPhiLepMet = DeltaPhi(muPhi->at(lepindex),pfMETPhi);
	float lepMET_MT = sqrt(2*muPt->at(lepindex)*pfMET*(1-TMath::Cos(dPhiLepMet)));
	if (lepMET_MT < 160){
	  nMT++;

	  bool vetoElectrons = ele_veto(0, lepindex, 10.);
	  if(vetoElectrons){
	    nEleVeto++;

	    bool vetoPhotons = pho_veto(0, lepindex, 15.);
	    if(vetoPhotons){
	      nPhoVeto++;

	      bool vetoTaus = tau_veto(0, lepindex);
	      if(vetoTaus){
		nTauVeto++;

		bool vetoBjets = bjet_veto(lepindex);
		if(vetoBjets){
		  nBtagVeto++;

		  bool mindphijr = getMinDphiJR(lepindex, leptoMET_phi);

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
	  }//veto ele
	}//MT cut
      }// one tight mu
    }//one loose mu

    if (jentry%reportEvery == 0)
      cout<<"Finished entry "<<jentry<<"/"<<(nentriesToCheck - 1)<<endl;
  }

  
  h_cutflow->SetBinContent(1,nTotalEvents);
  h_cutflow->SetBinContent(2,nOneLooseMu);
  h_cutflow->SetBinContent(3,nOneMu);
  h_cutflow->SetBinContent(4,nMT);
  h_cutflow->SetBinContent(5,nEleVeto);
  h_cutflow->SetBinContent(6,nPhoVeto);
  h_cutflow->SetBinContent(7,nTauVeto);
  h_cutflow->SetBinContent(8,nBtagVeto);
  h_cutflow->SetBinContent(9,nMinDphiJR);
  h_cutflow->SetBinContent(10,npfCaloCut);
  h_cutflow->SetBinContent(11,nRecoil);
  h_cutflow->SetBinContent(12,nJetCand);
  h_cutflow->SetBinContent(13,nJetVeto);
}

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


  float MetBins[45]={200.,220.,240.,260.,280.,300.,320.,340.,360.,380.,400.,420.,440.,460.,480.,500.,520.,540.,560.,580.,600.,620.,640.,660.,680.,700.,720.,740.,760.,
		     780.,800.,820.,840.,860.,880.,900.,920.,940.,960.,980.,1000.,1400.,1800.,2000.,2500.};

  float LeptonPtBins[25] = {20.,40.,60.,80.,100.,120.,140.,160.,180.,200.,250.,300.,350.,400.,500.,600.,700.,800.,900.,1000.,1100.,1200.,1300.,1400.,1500.};

  h_cutflow = new TH1D("h_cutflow","h_cutflow",14,0,14);h_cutflow->Sumw2();
  h_cutflow->GetXaxis()->SetBinLabel(1,"Total Events");
  h_cutflow->GetXaxis()->SetBinLabel(2,"1-lmu");
  h_cutflow->GetXaxis()->SetBinLabel(3,"1-tmu");
  h_cutflow->GetXaxis()->SetBinLabel(4,"MTcut");
  h_cutflow->GetXaxis()->SetBinLabel(5,"eleVeto"); 
  h_cutflow->GetXaxis()->SetBinLabel(6,"phoVeto");
  h_cutflow->GetXaxis()->SetBinLabel(7,"tauVeto");
  h_cutflow->GetXaxis()->SetBinLabel(8,"btagVeto");
  h_cutflow->GetXaxis()->SetBinLabel(9,"minDPhiJR");
  h_cutflow->GetXaxis()->SetBinLabel(10,"pfcaloCut");
  h_cutflow->GetXaxis()->SetBinLabel(11,"Recoil");
  h_cutflow->GetXaxis()->SetBinLabel(12,"JetCand");
  h_cutflow->GetXaxis()->SetBinLabel(13,"JetVeto");
}

void monoJetClass::fillHistos(int histoNumber,double event_weight) {
}

bool monoJetClass::JetVetoDecision(int jetindex, int lep_index) {

   bool jetVeto=false;

   float dR_lep = deltaR(jetEta->at(jetindex), jetPhi->at(jetindex), muEta->at(lep_index), muPhi->at(lep_index));

   //   float dR_muon   = 0.;
   //   float dR_photon = 0.;

   if(dR_lep > 0.4)
      jetVeto = true;

   return jetVeto;
}

int monoJetClass::mu_tightID(int loose_mu,  float muPtCut){
  // bool veto_passed = true; //pass veto if no good muon found

  //muon pt and eta cut
  if(muPt->at(loose_mu) > muPtCut && (fabs(muEta->at(loose_mu)) < 2.4)){
    // muon tight ID and Iso
    if(muIDbit->at(loose_mu)>>3&1 == 1 && muIDbit->at(loose_mu)>>9&1 == 1){
      return loose_mu;
    }
  }
  return -1;
}

//Veto failed if a muon is found that passes Loose Muon ID, Loose Muon Isolation, and muPtcut, and does not overlap the candidate photon within dR of 0.5
int monoJetClass::mu_looseID(float muPtCut){
  vector<int> tmpcands; tmpcands.clear();

  int mu_cand = -1;
  for(int i = 0; i < nMu; i++){
    // mu pt and eta cut
    if(muPt->at(i) > muPtCut && (fabs(muEta->at(i)) < 2.4)){
      if(muIDbit->at(i)>>0&1 == 1 && muIDbit->at(i)>>7&1==1){
	tmpcands.push_back(i);
      }
    }
  }
  if (tmpcands.size() == 1)
    mu_cand = tmpcands[0];
  return mu_cand;
}

bool monoJetClass::ele_veto(int jet_index,int mu_index,float elePtCut) {

  for(int i = 0; i < nEle; i++){
    if(elePt->at(i) > elePtCut && fabs(eleSCEta->at(i)) < 2.5){
      //Electron passes veto Electron ID cuts
      if(eleIDbit->at(i)>>3&1 == 1){

	//Electron does not overlap muon 
	double dR_Mu = deltaR(eleSCEta->at(i),eleSCPhi->at(i),muEta->at(mu_index),muPhi->at(mu_index));

	if(dR_Mu > 0.5 ){
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


bool monoJetClass::pho_veto(int jet_index, int lep_index, float phoPtCut){
   bool decision = true;
   int countPho  = 0;

   for(int i = 0; i < nPho; i++){
      bool kinematics = (phoEt->at(i) > phoPtCut && (fabs(phoEta->at(i)) < 2.5));
      bool IdandIso   = (phoIDbit->at(i)>>0&1==1); 

      double dR_Mu    = deltaR(phoEta->at(i),phoPhi->at(i),muEta->at(lep_index),muPhi->at(lep_index));  

      if(kinematics && IdandIso && dR_Mu > 0.5)
	 countPho++;
   }

   if(countPho > 0)
      decision = false;

   return decision;
}

bool monoJetClass::tau_veto(int jet_index, int lep_index){
   bool decision = true;
   int countTaus = 0;

   for(int i = 0; i < nTau; i++){
      bool kinematics = (tau_Pt->at(i) > 18.0 && fabs(tau_Eta->at(i)) < 2.3);
      bool IdandIso   = ((tau_IDbits->at(i)>>0&1) == 1 && (tau_IDbits->at(i)>>13&1) == 1);

      double dR_Mu   = deltaR(tau_Eta->at(i), tau_Phi->at(i), muEta->at(lep_index), muPhi->at(lep_index));  

      if( kinematics && IdandIso && dR_Mu > 0.4)
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

      double dR_Mu    = deltaR(jetEta->at(i), jetPhi->at(i), muEta->at(lep_index), muPhi->at(lep_index));  

      if(kinematics && Id && btag && dR_Mu > 0.4)
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

      double dR_Lep    = deltaR(jetEta->at(i), jetPhi->at(i), muEta->at(lep_index), muPhi->at(lep_index));  

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

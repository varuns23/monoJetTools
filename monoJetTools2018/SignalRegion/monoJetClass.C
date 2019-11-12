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
  int nEleVeto = 0;
  int nMuVeto = 0;
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

    nTotalEvents++;

    bool vetoElectrons = ele_veto(0, 10.);
    if(vetoElectrons){
      nEleVeto++;

      bool vetoMuons = muon_veto(0,10.);
      if (vetoMuons){
	nMuVeto++;

	bool vetoPhotons = pho_veto(0, 15.);
	if(vetoPhotons){
	  nPhoVeto++;

	  bool vetoTaus = tau_veto(0);
	  if(vetoTaus){
	    nTauVeto++;

	    bool vetoBjets = bjet_veto();
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

		      bool isoJet = JetVetoDecision(jetCand[0]);
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
      }// veto muons
    }//veto ele

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

  h_cutflow = new TH1D("h_cutflow","h_cutflow",11,0,11);h_cutflow->Sumw2();
  h_cutflow->GetXaxis()->SetBinLabel(1,"Total Events");
  h_cutflow->GetXaxis()->SetBinLabel(2,"eleVeto");
  h_cutflow->GetXaxis()->SetBinLabel(3,"muonVeto");
  h_cutflow->GetXaxis()->SetBinLabel(4,"phoVeto");
  h_cutflow->GetXaxis()->SetBinLabel(5,"tauVeto");
  h_cutflow->GetXaxis()->SetBinLabel(6,"btagVeto");
  h_cutflow->GetXaxis()->SetBinLabel(7,"minDPhiJR");
  h_cutflow->GetXaxis()->SetBinLabel(8,"pfcaloCut");
  h_cutflow->GetXaxis()->SetBinLabel(9,"Recoil");
  h_cutflow->GetXaxis()->SetBinLabel(10,"JetCand");
  h_cutflow->GetXaxis()->SetBinLabel(11,"JetVeto");
}

void monoJetClass::fillHistos(int histoNumber,double event_weight) {
}

bool monoJetClass::JetVetoDecision(int jetindex) {

  bool jetVeto=false;

  float dR_lep = deltaR(jetEta->at(jetindex), jetPhi->at(jetindex), muEta->at(lep_index), muPhi->at(lep_index));

  //   float dR_muon   = 0.;
  //   float dR_photon = 0.;

  if(dR_lep > 0.4)
    jetVeto = true;

  return jetVeto;
}

bool monoJetClass::ele_veto(int jet_index,float elePtCut) {

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

bool monoJetClass::muon_veto_looseID(int jet_index,float muPtCut){
   bool decision = true;
   int countMu = 0; 

   for(int i = 0; i < nMu; i++) {
      bool kinematics = (muPt->at(i) > muPtCut && (fabs(muEta->at(i)) < 2.4));
      bool IdandIso   = (muIDbit->at(i)>>0&1==1 && muIDbit->at(i)>>7&1==1); //loose Id and loose Iso

      //Muon overlap with ele
      double dR_ele = deltaR(muEta->at(i),muPhi->at(i),eleEta->at(ele_index),elePhi->at(ele_index)); 

      if(kinematics && IdandIso && dR_ele > 0.4)
	 countMu++;
   }

   if(countMu > 0)
      decision = false;

   return decision;
}

bool monoJetClass::pho_veto(int jet_index,float phoPtCut){
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

bool monoJetClass::tau_veto(int jet_index,){
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

bool monoJetClass::bjet_veto(){
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

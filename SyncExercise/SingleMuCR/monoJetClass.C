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

      int losse_ele = ele_looseID(10.0);
      if(losse_ele != -1){
	 nOneLooseEle++

	   int tight_ele = ele_tightID(losse_ele, 40.0) 
	   if(tight_ele != -1){
	      nOneEle++;

	      int lepindex = tight_ele;
	      if(pfMET > 50.){
		 nMET++;

		 bool vetoMuons = muon_veto_looseID(0, lepindex, 10.);
		 if(vetoMuons){
		    nMuVeto++;

		    bool vetoPhotons = pho_veto_looseID(0, lepindex, 15.);
		    if(vetoPhotons){
		       nPhoVeto++;

		       bool vetoTaus = tau_veto(0, lepindex);
		       if(vetoTaus){
			  nTauVeto++;

			  bool vetoBjets = bjet_veto(lepindex);
			  if(vetoBjets){
			     nBtagVeto++;

			     TLorentzVector lep_4vec;
			     lep_4vec.SetPtEtaPhiE(elePt->at(lepindex),eleSCEta->at(lepindex),eleSCPhi->at(lepindex),eleE->at(lepindex));
			     lepton_pt = lep_4vec.Pt();
			     TLorentzVector met_4vec;
			     met_4vec.SetPtEtaPhiE(pfMET,0.,pfMETPhi,pfMET);
			     TLorentzVector leptoMET_4vec = lep_4vec + met_4vec;
			     double leptoMET = fabs(leptoMET_4vec.Pt());
			     double leptoMETphi = leptoMET_4vec.Phi();
			     Recoil = leptoMET;

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

		 


     lepindex = mulist[0];
		 TLorentzVector lep_4vec;
		 lep_4vec.SetPtEtaPhiE(muPt->at(mulist[0]),muEta->at(mulist[0]),muPhi->at(mulist[0]),muE->at(mulist[0]));
		 lepton_pt = lep_4vec.Pt();
		 TLorentzVector met_4vec;
		 met_4vec.SetPtEtaPhiE(pfMET,0.,pfMETPhi,pfMET);
		 TLorentzVector leptoMET_4vec = lep_4vec + met_4vec;
		 double leptoMET = fabs(leptoMET_4vec.Pt());
		 double leptoMETphi = leptoMET_4vec.Phi();
		 Recoil = leptoMET;

		 float dPhiLepMet = DeltaPhi(muPhi->at(lepindex),pfMETPhi);
		 float lepMET_MT = sqrt(2*muPt->at(lepindex)*pfMET*(1-TMath::Cos(dPhiLepMet)));
		       if (lepMET_MT < 160) 
     

     if (jentry%reportEvery == 0)
	cout<<"Finished entry "<<jentry<<"/"<<(nentriesToCheck - 1)<<endl;
  }
  
  h_cutflow->SetBinContent(1,nTotalEvents); 
  h_cutflow->SetBinContent(2,nFilters);
  h_cutflow->SetBinContent(3,nHLT);
  h_cutflow->SetBinContent(4,nJetSelection);
  h_cutflow->SetBinContent(5,nCRSelection);
  h_cutflow->SetBinContent(6,nMET200);
  h_cutflow->SetBinContent(7,nNoElectrons);
  h_cutflow->SetBinContent(8,lepMET_MT160);
  h_cutflow->SetBinContent(9,nMETcut);
  h_cutflow->SetBinContent(10,nbtagVeto);
  h_cutflow->SetBinContent(11,nDphiJetMET);
  h_cutflow->SetBinContent(12,eleHEMVeto);
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

   h_cutflow = new TH1D("h_cutflow","h_cutflow",12,0,12);h_cutflow->Sumw2();
   h_cutflow->GetXaxis()->SetBinLabel(1,"Total Events");
   h_cutflow->GetXaxis()->SetBinLabel(2,"metFilters");
   h_cutflow->GetXaxis()->SetBinLabel(3,"Trigger");
   h_cutflow->GetXaxis()->SetBinLabel(4,"GoodJet");
   h_cutflow->GetXaxis()->SetBinLabel(5,"CRSelection"); 
   h_cutflow->GetXaxis()->SetBinLabel(6,"leptoMetCut");
   h_cutflow->GetXaxis()->SetBinLabel(7,"NoElectrons");
   h_cutflow->GetXaxis()->SetBinLabel(8,"lepMET160");
   h_cutflow->GetXaxis()->SetBinLabel(9,"caloMET cut");
   h_cutflow->GetXaxis()->SetBinLabel(10,"B-JetVeto");
   h_cutflow->GetXaxis()->SetBinLabel(11,"DeltaPhiCut");
   h_cutflow->GetXaxis()->SetBinLabel(12,"EleHEMVeto");

   h_lepMET_MT = new TH1F("h_lepMET_MT","h_lepMET_MT; transverse mass of the lepton-Emiss system",40,0,400);h_lepMET_MT->Sumw2();
   h_tightMuISO = new TH1F("h_tightMuISO","tightMuISO Scale Factor;tightMuISO Scale Factor",50,0.95,1.05); h_tightMuISO->Sumw2();
   h_tightMuID = new TH1F("h_tightMuID","tightMuID Scale Factor;tightMuID Scale Factor",50,0.95,1.05); h_tightMuID->Sumw2();

   BookCommon(-1,"");
   for(int i = 0; i<nHisto; i++){
      char ptbins[100];
      sprintf(ptbins, "_%d", i);
      string histname(ptbins);

      auto dir = output->mkdir( ("ZprimeJet"+histname).c_str() );
      dir->cd();
      if (i == bHisto) {
	 auto treedir = dir->mkdir("trees");
	 treedir->cd();
	 tree = new TTree("norm","norm");
	 initTree(tree);
	 scaleUncs = new ScaleUncCollection(tree);
	 shapeUncs = new ShapeUncCollection(treedir);
	 dir->cd();
      }
      //Common Histograms
      BookCommon(i,histname);
      //CR Histograms
      h_LeptonPt[i] = new TH1F(("h_LeptonPt"+histname).c_str(),"h_LeptonPt",24,LeptonPtBins);h_LeptonPt[i]->Sumw2();
      h_LeptonEta[i] = new TH1F(("h_LeptonEta"+histname).c_str(),"h_LeptonEta",30,-3.0,3.0);h_LeptonEta[i]->Sumw2();
      h_LeptonPhi[i] = new TH1F(("h_LeptonPhi"+histname).c_str(),"h_LeptonPhi",50,-3.1416,3.1416);h_LeptonPhi[i]->Sumw2();
      h_LeptonEtaPhi[i] = new TH2F(("h_LeptonEtaPhi"+histname).c_str(),"h_LeptonEtaPhi",50,-3.0,3.0,50,-3.1416,3.1416);
      h_recoil[i] = new TH1F(("h_recoil"+histname).c_str(), "Recoil (GeV)",44,MetBins);h_recoil[i] ->Sumw2();
   }
}

void monoJetClass::fillHistos(int histoNumber,double event_weight) {
   fillCommon(histoNumber,event_weight);
   //CR Histograms
   if(lepindex >= 0){ 
      h_LeptonPt[histoNumber]->Fill(muPt->at(lepindex),event_weight);
      h_LeptonEta[histoNumber]->Fill(muEta->at(lepindex),event_weight);
      h_LeptonPhi[histoNumber]->Fill(muPhi->at(lepindex),event_weight);
      h_LeptonEtaPhi[histoNumber]->Fill(muEta->at(lepindex),muPhi->at(lepindex),event_weight);
   }
   if(lepton_pt > 0){
      h_recoil[histoNumber]->Fill(Recoil,event_weight);}
   weight = event_weight;
   if (histoNumber == bHisto) tree->Fill();
}

vector<int> monoJetClass::JetVetoDecision(int jet_index, int mu_index) {
   bool jetVeto=true;
   vector<int> jetindex;
   for(int i = 0; i < nJet; i++){
      double deltar_mu = deltaR(jetEta->at(i),jetPhi->at(i),muEta->at(mu_index),muPhi->at(mu_index));
      bool tightJetID = false;
      if ((*jetID)[i]>>0&1 == 1) tightJetID = true;
      if(deltar_mu>0.4 && jetPt->at(i) >30.0 && fabs(jetEta->at(i)) < 2.5 && tightJetID)
	 jetindex.push_back(i);
   }
   return jetindex;
}

vector<int> monoJetClass::electron_veto_tightID(int jet_index, float elePtCut) {
   vector<int> ele_cands;
   ele_cands.clear();
   for(int i = 0; i < nEle; i++) {
      //Electron passes Tight Electron ID cuts
      if(eleIDbit->at(i)>>2&1 == 1) {
	 //Electron passes eta cut
	 if (fabs(eleEta->at(i)) < 2.5) {
	    //Electron passes pt cut
	    if(elePt->at(i) > elePtCut) {
	       //Electron does not overlap photon
	       if(deltaR(eleEta->at(i),elePhi->at(i),jetEta->at(jet_index),jetPhi->at(jet_index)) > 0.5)
		  ele_cands.push_back(i);
	    }
	 }
      }
   }
   return ele_cands;
}

vector<int> monoJetClass::muon_veto_tightID(int jet_index, float muPtCut) {
   // bool veto_passed = true; //pass veto if no good muon found
   vector<int> mu_cands;
   mu_cands.clear();

   for(int i = 0; i < nMu; i++) {
      if(muIDbit->at(i)>>3&1 == 1 && muIDbit->at(i)>>9&1 == 1) {
	 //Muon passes eta cut
	 if (fabs(muEta->at(i)) < 2.4) {
	    //Muon passes pt cut
	    if(muPt->at(i) > muPtCut) {
	       //Muon does not overlap photon
	       if(deltaR(muEta->at(i),muPhi->at(i),jetEta->at(jet_index),jetPhi->at(jet_index)) > 0.5)
		  mu_cands.push_back(i);
	    }
	 }
      }
   }
   return mu_cands;
}

vector<int> monoJetClass::electron_veto_looseID(int jet_index, int mu_index, float elePtCut) {
   vector<int> ele_cands;
   ele_cands.clear();
   for(int i = 0; i < nEle; i++) {
      //Electron passes Loose Electron ID cuts
      if(eleIDbit->at(i)>>0&1 == 1) {
	 //Electron passes eta cut
	 if (fabs(eleEta->at(i)) < 2.5) {
	    //Electron passes pt cut
	    if(elePt->at(i) > elePtCut) {
	       //Electron does not overlap photon
	       if(deltaR(eleEta->at(i),elePhi->at(i),jetEta->at(jet_index),jetPhi->at(jet_index)) > 0.5)
		  ele_cands.push_back(i);
	    }
	 }
      }
   }
   return ele_cands;
}

//Veto failed if a muon is found that passes Loose Muon ID, Loose Muon Isolation, and muPtcut, and does not overlap the candidate photon within dR of 0.5
vector<int> monoJetClass::muon_veto_looseID(int jet_index, int ele_index, float muPtCut) {
   vector<int> mu_cands;
   mu_cands.clear();
   for(int i = 0; i < nMu; i++) {
      if(muIDbit->at(i)>>0&1==1) {
	 //Muon passes eta cut
	 if (fabs(muEta->at(i)) < 2.4) {
	    //Muon passes pt cut
	    if(muPt->at(i) > muPtCut) {
	       //cout <<"Passed Pt Cut" << endl;
	       //Muon does not overlap photon
	       if(deltaR(muEta->at(i),muPhi->at(i),jetEta->at(jet_index),jetPhi->at(jet_index)) > 0.5)
		  mu_cands.push_back(i);
	    }
	 }
      }
   }
   return mu_cands;
}

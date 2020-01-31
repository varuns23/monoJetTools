//For use with Ntuples made from JetAnalyzer
////Required arguments: 1 is folder containing input files, 2 is output file path, 3 is maxEvents (-1 to run over all events), 4 is reportEvery
////
////To compile using rootcom to an executable named 'analyze':
////$ ./rootcom monoJetClass analyze
////
////To run, assuming this is compiled to an executable named 'analyze':
////$ ./analyze /hdfs/store/user/uhussain/monoJet_Ntuples/ /cms/uhussain/MonoJet/CMSSW_8_0_8/src/LightZPrimeAnalysis/JetAnalyzer/test/output.root -1 10000
////Runs over every event in the folder monoJet_Ntuples, reporting progress every 10000 events
////and storing the resulting histograms in the file output.root.
////
//
#define monoJetClass_cxx
#include "monoJetClass.h"
#include "Utilities.h"

using namespace std;

void monoJetClass::Loop(Long64_t maxEvents, int reportEvery) {
  if (fChain == 0) return;

  Long64_t nentries = fChain->GetEntries();
  cout<<"Coming in:"<<endl;
  cout<<"nentries:"<<nentries<<endl;
  Long64_t nentriesToCheck = nentries;
  
  // if (!sample.isData) SetScalingHistos();

  if (maxEvents != -1LL && nentries > maxEvents)
    nentriesToCheck = maxEvents;
  int nTotal = nentriesToCheck;
  Long64_t nbytes = 0, nb = 0;
  cout<<"Running over "<<nTotal<<" events."<<endl;  
  for (Long64_t jentry=0; jentry<nentriesToCheck;jentry++) {
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   nbytes += nb;

    initVars();

    // float event_weight = 1.;
    // if (!sample.isData) {
    //   ApplyPileup(event_weight);
    //   if (isW_or_ZJet()) {
    // 	SetBoson(sample.PID);
    // 	ApplyKFactor(event_weight);
    //   }
    // }

    // float weightNorm = event_weight;
    vector<int> jetlist = monoJetAnalysis::getJetCand();
    cutflow->Fill(0);
    if(getMetTrigger()){
      cutflow->Fill(1);

      vector<int> mulistLoose = getLooseMu();
      if(mulistLoose.size() == 2){
	cutflow->Fill(2);
	vector<int> mulistTight = getTightMu(mulistLoose);

	if(mulistTight.size() > 0){
	  cutflow->Fill(3);

	  if(CRSelection(mulistTight,mulistLoose)){
	    cutflow->Fill(4);

	    TLorentzVector ll = lep1 + lep2;
	    dilepton_mass = ll.M();
	    dilepton_pt = ll.Pt();

	    if(dilepton_mass > 60. && dilepton_mass < 120.){
	      cutflow->Fill(5);

	      if(getMetFilter()){
		cutflow->Fill(6);

		vector<int> elelist = getLooseEle();
		if(elelist.size() == 0){
		  cutflow->Fill(7);

		  vector<int> pholist = pho_veto_looseID(leadLepIndx, subleadLepIndx);
		  if(pholist.size() == 0){ 
		    cutflow->Fill(8);

		    vector<int> taulist = tau_veto_looseID(leadLepIndx, subleadLepIndx);
		    if(taulist.size() == 0){
		      cutflow->Fill(9);

		      vector<int> bjetlist = bjet_veto(jetlist, leadLepIndx, subleadLepIndx);
		      if(bjetlist.size() == 0){
			cutflow->Fill(10);

			TLorentzVector met_4vec;
			met_4vec.SetPtEtaPhiE(pfMET, 0., pfMETPhi, pfMET);
			TLorentzVector leptoMET_4vec = ll+met_4vec;
			recoil     = fabs(leptoMET_4vec.Pt());
		        recoilPhi = leptoMET_4vec.Phi();

			bool mindphijr = getMinDphiJR(jetlist, leadLepIndx, subleadLepIndx, recoilPhi);
			if(mindphijr){
			  cutflow->Fill(11);

			  double metcut = (fabs(pfMET-caloMET))/recoil;
			  if(metcut < 0.5){
			    cutflow->Fill(12);

			    if(recoil > 250.){
			      cutflow->Fill(13);

			      jetCand = getJetCand(jetlist, leadLepIndx, subleadLepIndx);
			      if(jetCand.size()>0){
				cutflow->Fill(14);

			      } //jet cand
			    } // recoil
			  } //metcut
			} //mindphijr
		      } //b-veto
		    } //tau veto
		  } //pho veto
		} //Ele veto
	      } //Met Filter
	    } //dilepton mass
	  } // dileptons with opposite charge
	} // one tight muon
      } // two muons
    } //trigger

    if (jentry%reportEvery == 0){
      cout<<"Finished entry "<<jentry<<"/"<<(nentriesToCheck-1)<<endl;
    }
  }
   
}//Closing the Loop function

void monoJetClass::BookHistos(const char* outputFilename) {
  
  output = new TFile(outputFilename, "RECREATE");
  output->cd();
  
  cutflow = new Cutflow({"Total Events","Trigger","Two Mu","One Tight Mu","DiLep Charge","DiLep Mass","Met Filter",
	"Ele Veto","Pho Veto","Tau Veto","B Veto","MinDPhiJR","dPfCalo","Recoil","Jet Selection"});

  // BookHistos(-1,"");
  // for(int i = 0; i<nHisto; i++) {
  //   char ptbins[100];
  //   sprintf(ptbins, "_%d", i);
  //   string histname(ptbins);
  //   auto dir = output->mkdir( ("monoJet"+histname).c_str() );
  //   dir->cd();
  //   if (i == bHisto) {
  //     auto treedir = dir->mkdir("trees");
  //     treedir->cd();
  //     tree = new TTree("norm","norm");
  //     initTree(tree);
  //     scaleUncs.setTree(tree);
  //     shapeUncs.setDir(treedir);
  //     dir->cd();
  //   }
  //   BookHistos(i,histname);
  // }
}

void monoJetClass::fillHistos(int nhist,float event_weight) {
  monoJetYear::fillHistos(nhist,event_weight);
  monoJetDoubleMuCR::fillHistos(nhist,event_weight);
  weight = event_weight;
  if (nhist == bHisto) tree->Fill();
}

vector<int> monoJetClass::getTightMu(vector<int> looseMu){
  vector<int> mu_cands;
  mu_cands.clear();

  for(int i = 0; i < looseMu.size(); i++){
    int muCand = looseMu.at(i);
    bool kinematics = (muPt->at(muCand) > 20.0 && (fabs(muEta->at(muCand)) < 2.4)); // muon pt & eta
    bool IdIso = (muIDbit->at(muCand)>>3&1 == 1 && muIDbit->at(muCand)>>9&1 == 1);   // muon tight ID and Iso

    if(kinematics && IdIso)
      mu_cands.push_back(muCand);
  }  
  return mu_cands;
} 

vector<int> monoJetClass::getJetCand(vector<int> jetlist, int lead_lepIndex, int sublead_lepIndex){
  vector<int> jet_cands;
  jet_cands.clear();

  for(int j; j<jetlist.size(); j++){
    int i = jetlist.at(j);

    bool kinematics = (jetPt->at(i) > 100.0 && fabs(jetEta->at(i)) < 2.4);
    bool Id = (jetNHF->at(i) < 0.8 && jetCHF->at(i) > 0.1); 

    float dR_lead_mu = deltaR(jetEta->at(i), jetPhi->at(i), muEta->at(lead_lepIndex),  muPhi->at(lead_lepIndex));
    float dR_sublead_mu = deltaR(jetEta->at(i), jetPhi->at(i), muEta->at(sublead_lepIndex), muPhi->at(sublead_lepIndex));

    if(kinematics && Id && dR_lead_mu > 0.4 && dR_sublead_mu > 0.4)
      jet_cands.push_back(i);
  }

  return jet_cands;
}

vector<int> monoJetClass::pho_veto_looseID(int leadLep_index, int subleadLep_index){
  vector<int> pho_cands;
  pho_cands.clear();

  for(int i = 0; i < nPho; i++){
    // passes pt cut
    bool kinematics = ((phoEt->at(i) > 15.0) && (fabs(phoSCEta->at(i)) < 2.5));
    bool IdIso = (phoIDbit->at(i)>>0&1==1);

    double dR_leadLep    = deltaR(phoSCEta->at(i),phoSCPhi->at(i), muEta->at(leadLep_index), muPhi->at(leadLep_index));  
    double dR_subleadLep = deltaR(phoSCEta->at(i),phoSCPhi->at(i), muEta->at(subleadLep_index), muPhi->at(subleadLep_index)); 
    if( kinematics && IdIso && dR_leadLep > 0.5 && dR_subleadLep > 0.5)
      pho_cands.push_back(i);
  }
  return pho_cands;
}

vector<int> monoJetClass::tau_veto_looseID(int leadLep_index, int subleadLep_index){
  vector<int> tau_cands;
  tau_cands.clear();

  for(int i = 0; i < nTau; i++){
    bool kinematics = ((tau_Pt->at(i) > 18.0) && (fabs(tau_Eta->at(i)) < 2.3));
    bool IdIso = (((tau_IDbits->at(i)>>0&1) == 1) && ((tau_IDbits->at(i)>>13&1) == 1));

    double dR_leadLep    = deltaR(tau_Eta->at(i), tau_Phi->at(i), muEta->at(leadLep_index), muPhi->at(leadLep_index));  
    double dR_subleadLep = deltaR(tau_Eta->at(i), tau_Phi->at(i), muEta->at(subleadLep_index), muPhi->at(subleadLep_index)); 
    if( kinematics && IdIso && dR_leadLep > 0.4 && dR_subleadLep > 0.4)
      tau_cands.push_back(i);
  }
  return tau_cands;
}

vector<int> monoJetClass::bjet_veto(vector<int> jetlist, int leadLep_index, int subleadLep_index){
  vector<int> bjet_cands;
  bjet_cands.clear();

  for(int j = 0; j < jetlist.size(); j++){
    int i = jetlist.at(j);
    bool kinematic = (jetPt->at(i) > 20.0 && fabs(jetEta->at(i)) < 2.4);
    bool btagged = ((jetDeepCSVTags_b->at(i) + jetDeepCSVTags_bb->at(i)) > 0.4941);

    double dR_leadLep    = deltaR(jetEta->at(i), jetPhi->at(i), muEta->at(leadLep_index), muPhi->at(leadLep_index));  
    double dR_subleadLep = deltaR(jetEta->at(i), jetPhi->at(i), muEta->at(subleadLep_index), muPhi->at(subleadLep_index)); 

    if(kinematic && btagged &&  dR_leadLep > 0.4 && dR_subleadLep > 0.4)
      bjet_cands.push_back(i);
  }
  return bjet_cands;
}


bool monoJetClass::getMinDphiJR(vector<int> jetlist, int lead_lepIndex, int sublead_lepIndex, double lepMET_phi){
  bool decision = false;

  vector<int> tmpJetlist;
  tmpJetlist.clear();

  for(int j=0; j<jetlist.size(); j++){ 

    int i = jetlist.at(j);

    bool kinematic = (jetPt->at(i) > 30. && fabs(jetEta->at(i)) < 2.4);

    double dR_leadLep    = deltaR(jetEta->at(i), jetPhi->at(i), muEta->at(lead_lepIndex), muPhi->at(lead_lepIndex));  
    double dR_subLeadLep = deltaR(jetEta->at(i), jetPhi->at(i), muEta->at(sublead_lepIndex), muPhi->at(sublead_lepIndex)); 

    if(kinematic && dR_leadLep > 0.4 && dR_subLeadLep > 0.4)
      tmpJetlist.push_back(i);
  }

  int count=0;
  for(int k=0; k < tmpJetlist.size(); k++){
    if(deltaPhi(jetPhi->at(tmpJetlist.at(k)), lepMET_phi) > 0.5) 
      count++;
  }

  if(count >= 4 || count == tmpJetlist.size()) 
    decision = true;

  return decision;
}

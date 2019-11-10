#define monoJetClass_cxx
#include "monoJetClass.h"

using namespace std;

int main(int argc, const char* argv[]) { 
  if (argc == 1) {
    printf("Running Test\n");
    argv[1] = "/hdfs/store/user/varuns/NTuples/MC/MC2018_RunIIAutumn18_102X_JECv19/ZJets/ZJetsToNuNu_HT400-600/0000/";
    argv[2] = "test.root";
    argv[3] = "-1";
    argv[4] = "1000";
    argv[5] = "607-608/725-725/734-734/827-827/";
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

  if (maxEvents != -1LL && nentries > maxEvents)
    nentriesToCheck = maxEvents;
  int nTotal = nentriesToCheck;
  Long64_t nbytes = 0, nb = 0;
  cout<<"Running over "<<nTotal<<" events."<<endl;
  Long64_t eventlist[] = { 22660,44482,38781,2647,13512,13991 };
  // for (Long64_t jentry=0; jentry<nentriesToCheck;jentry++) {
  for (Long64_t jentry : eventlist ) {
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   nbytes += nb;
    // if (jentry % reportEvery == 0) printf("Processesing event: %i/%i\n",jentry,nTotal);

    bool check_event = (event == 30682767 ||
			event == 79123963 ||
			event == 51393664 ||
			event == 36533896 ||
			event == 35733274 ||
			event == 35719982);
    if ( !check_event ) continue;
    float recoil,recoilPhi,ak4pt0,ak4eta0,leadbtag,mupt0,mueta0,elpt0,eleta0,gpt0,geta0;
    recoil = recoilPhi = ak4pt0 = ak4eta0 = leadbtag = mupt0 = mueta0 = elpt0 = eleta0 = gpt0 = geta0 = 0.0;
    int nLooseMu,nTightMu,nLooseEl,nTightEl,nLooseGam,nTightGam;
    nLooseMu = nTightMu = nLooseEl = nTightEl = nLooseGam = nTightGam = 0;
    TLorentzVector lep_vec,mu_vec,ele_vec,pho_vec,met_vec,recoil_vec;
    
    if ( nJet > 0 ) {
      ak4pt0 = jetPt->at(0);
      ak4eta0 = jetEta->at(0);
    }
    
  //  auto bjetlist = bjetveto();
  //  if ( bjetlist.size() != 0 ) {
  //    leadbtag = jetDeepCSVTags_b->at(bjetlist[0]) + jetDeepCSVTags_bb->at(bjetlist[0]);
  //  }
   leadbtag =bjetveto();

    auto looseMus = muon_looseID(10);
    auto tightMus = muon_tightID(20);
    nLooseMu = looseMus.size();
    nTightMu = tightMus.size();
    if ( nTightMu > 0 ){
      int index = 0;
      mupt0 = muPt->at(index);
      mueta0 = muEta->at(index);
      float muphi0 = muPhi->at(index);
      float mue0 = muE->at(index);
      mu_vec.SetPtEtaPhiE(mupt0,mueta0,muphi0,mue0);
      lep_vec += mu_vec;
    }
    auto looseEle = ele_looseID(10);
    auto tightEle = ele_tightID(40);
    nLooseEl = looseEle.size();
    nTightEl = tightEle.size();
    if ( nTightEl >0 ){
      int index = 0;
      elpt0 = elePt->at(index);
      eleta0 = eleEta->at(index);
      float elphi0 = elePhi->at(index);
      float ele0 = eleE->at(index);
      ele_vec.SetPtEtaPhiE(elpt0,eleta0,elphi0,ele0);
      lep_vec += ele_vec;
    }
    auto looseGams = pho_looseID(15);
    auto tightGams = pho_tightID(25);
    nLooseGam = looseGams.size();
    nTightGam = tightGams.size();
    if ( nTightGam > 0 ){
      int index = 0;
      gpt0 = phoEt->at(index);
      geta0 = phoEta->at(index);
      float gphi0 = phoPhi->at(index);
      float ge0 = phoE->at(index);
      pho_vec.SetPtEtaPhiE(gpt0,geta0,gphi0,ge0);
      lep_vec += pho_vec;
    }
//    lep_vec = mu_vec + ele_vec + pho_vec;
    met_vec.SetPtEtaPhiE(pfMET,0,pfMETPhi,pfMET);
    recoil_vec = met_vec + lep_vec;
    recoil = fabs( recoil_vec.Pt() );
    recoilPhi = recoil_vec.Phi();

    // printf("jentry:     %i\n",jentry);
    printf("event:      %i\n",event);
    printf("met:        %f\n",pfMET);
    printf("met_phi:    %f\n",pfMETPhi);
    printf("recoil:     %f\n",recoil);
    printf("recoil_phi: %f\n",recoilPhi);
    printf("ak4pt0:     %f\n",ak4pt0);
    printf("ak4eta0:    %f\n",ak4eta0);
    printf("leadbtag:   %f\n",leadbtag);
    printf("nLooseMu:   %i\n",nLooseMu);
    printf("nTightMu:   %i\n",nTightMu);
    printf("mupt0:      %f\n",mupt0);
    printf("mueta0:     %f\n",mueta0);
    printf("nLooseEl:   %i\n",nLooseEl);
    printf("nTightEl:   %i\n",nTightEl);
    printf("elpt0:      %f\n",elpt0);
    printf("eleta0:     %f\n",eleta0);
    printf("nLooseGam:  %i\n",nLooseGam);
    printf("nTightGam:  %i\n",nTightGam);
    printf("gpt0:       %f\n",gpt0);
    printf("geta0:      %f\n",geta0);
    printf("\n");
  }
}

void monoJetClass::initTree(TTree* tree) {
}

void monoJetClass::BookHistos(const char* outputFilename) {

  output = new TFile(outputFilename, "RECREATE");
  output->cd();
}

vector<int> monoJetClass::ele_looseID(float elePtCut) {
  vector<int> tmpcands;
  tmpcands.clear();

  for(int i = 0; i < nEle; i++) {

    bool kinematics = (elePt->at(i) > elePtCut && fabs(eleEta->at(i)) < 2.5);
    bool IdandIso   = (eleIDbit->at(i)>>0&1 == 1);
    //Ele DZ and D0 selection
    if ( kinematics && IdandIso && (fabs(eleEta->at(i)) <= 1.479) && 
	 (fabs(eleD0->at(i)) < 0.05) && (fabs(eleDz->at(i)) < 0.1 )){
      tmpcands.push_back(i);
    }
    else if(  kinematics && IdandIso && (fabs(eleEta->at(i)) > 1.479) && 
	      (fabs(eleD0->at(i)) < 0.1) && (fabs(eleDz->at(i)) < 0.2 )){
      tmpcands.push_back(i);
    }
  }
  return tmpcands;
}


vector<int> monoJetClass::ele_tightID(float elePtCut) {
  vector<int> tmpcands; tmpcands.clear();

  for (int i = 0; i < nEle; i++) {

    bool kinematics = (elePt->at(i) > elePtCut && fabs(eleEta->at(i)) < 2.5);
    bool IdandIso   = (eleIDbit->at(i)>>2&1 == 1);

    //Electron passes dz/d0 cut
    if( kinematics && IdandIso && (fabs(eleEta->at(i)) <= 1.479) && 
	(fabs(eleD0->at(i)) < 0.05) && (fabs(eleDz->at(i)) < 0.1)){
      tmpcands.push_back(i);
    }
    else if( kinematics && IdandIso && (fabs(eleEta->at(i)) > 1.479) && 
	     (fabs(eleD0->at(i)) < 0.1)   && (fabs(eleDz->at(i)) < 0.2)){
      tmpcands.push_back(i);
    }
  }

  return tmpcands;
}


//Veto failed if a muon is found that passes Loose Muon ID, Loose Muon Isolation, and muPtcut, and does not overlap the candidate photon within dR of 0.5
vector<int> monoJetClass::muon_looseID(float muPtCut){
  vector<int> tmpcands; tmpcands.clear();

  for(int i = 0; i < nMu; i++){
    // mu pt and eta cut
    if(muPt->at(i) > muPtCut && (fabs(muEta->at(i)) < 2.4)){
      if(muIDbit->at(i)>>0&1 == 1 && muIDbit->at(i)>>7&1==1){
	tmpcands.push_back(i);
      }
    }
  }
  return tmpcands;
}

vector<int> monoJetClass::muon_tightID(float muPtCut){
  vector<int> tmpcands; tmpcands.clear();

  for (int i = 0; i < nMu; i++) {
    //muon pt and eta cut
    if(muPt->at(i) > muPtCut && (fabs(muEta->at(i)) < 2.4)){
      // muon tight ID and Iso
      if(muIDbit->at(i)>>3&1 == 1 && muIDbit->at(i)>>9&1 == 1){
        tmpcands.push_back(i);
      }
    }
  }
  return tmpcands;
}

vector<int> monoJetClass::pho_looseID(float phoECut) {
  vector<int> tmpcands; tmpcands.clear();

  for (int i = 0; i < nPho; i++) {
    bool kinematics = phoE->at(i) > phoECut && fabs(phoEta->at(i)) < 2.5;
    bool IdandIso   = (phoIDbit->at(i)>>0&1==1);
    bool eleVeto = phoEleVeto->at(i);
    if (kinematics && IdandIso && eleVeto)
      tmpcands.push_back(i);
  }
  return tmpcands;
}

vector<int> monoJetClass::pho_tightID(float phoECut) {
  vector<int> tmpcands; tmpcands.clear();

  for (int i = 0; i < nPho; i++) {
    bool kinematics = phoEt->at(i) > phoECut && fabs(phoEta->at(i)) < 1.4442;
    bool IdandIso   = (phoIDbit->at(i)>>1&1==1);
    bool eleVeto = phoEleVeto->at(i);
    if (kinematics && IdandIso && eleVeto)
      tmpcands.push_back(i);
  }
  return tmpcands;
}

float monoJetClass::bjetveto(){
  vector<int> tmpcands; tmpcands.clear();
  vector<int> jets; jets.clear();

  for(int i = 0; i < nJet; i++){
    bool kinematics = (jetPt->at(i) > 20.0) && (fabs(jetEta->at(i)) < 2.4);
    bool Id   = jetID->at(i)>>0&1 == 1;
    //btag = (jetDeepCSVTags_b->at(i) + jetDeepCSVTags_bb->at(i)) > 0.4184;

    //if(kinematics && Id && btag)
    if(kinematics && Id){

      jets.push_back(i);
    }
  }

  int bJC = jets[0];
  for(size_t i =1; i < jets.size(); i++){
    float btag1 = (jetDeepCSVTags_b->at(bJC) + jetDeepCSVTags_bb->at(bJC));
    float btag2 = (jetDeepCSVTags_b->at(jets[i]) + jetDeepCSVTags_bb->at(jets[i]));
    if(btag1 < btag2  )
      bJC = jets[i];
  }

  return (jetDeepCSVTags_b->at(bJC) + jetDeepCSVTags_bb->at(bJC));
}

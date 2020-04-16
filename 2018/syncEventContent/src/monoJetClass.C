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

  if (isMC) SetScalingHistos();
  //Long64_t eventlist[] = { 39354, 24399, 22878, 29961 }; // 2017-MC
  //  Long64_t eventlist[] = { 252791, 554072, 334599, 335078, 340641, 327871 }; // 2018-MC

  if (maxEvents != -1LL && nentries > maxEvents)
    nentriesToCheck = maxEvents;
  int nTotal = nentriesToCheck;
  Long64_t nbytes = 0, nb = 0;
  cout<<"Running over "<<nTotal<<" events."<<endl;  
  for (Long64_t jentry=0; jentry<nentriesToCheck;jentry++) {
  //for (Long64_t jentry : eventlist ){ 
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   nbytes += nb;

    //-|MC-2017
    //-|    bool check_event = (
    //-|	event == 61228926 ||
    //-|	event == 79507460 ||
    //-|	event == 81725069 ||
    //-|	event == 29088699
    //-|	);
    //-|MC-2018
    //-|    bool check_event = (
    //-|	event == 30682767 ||
    //-|	event == 79123963 ||
    //-|	event == 51393664 ||
    //-|	event == 36533896 || 
    //-|	event == 35733274 ||
    //-|	event == 35719982 
    //-|	);
    //-|Data-2018
    bool  check_event = (event == 612492923 || event ==  614386085 || event == 615057653 || event == 614950316 || event == 614022292);

      
    if ( !check_event ) continue;



    initVars();

    float Recoil, RecoilPhi, ak4pt0, ak4eta0, leadbtag, mupt0, mueta0, elpt0, eleta0, gpt0, geta0;
    Recoil = RecoilPhi = ak4pt0 = ak4eta0 = leadbtag = mupt0 = mueta0 = elpt0 = eleta0 = gpt0 = geta0 = 0.0;

    int nLooseMu, nTightMu, nLooseEl, nTightEl, nLooseGam, nTightGam;
    nLooseMu = nTightMu = nLooseEl = nTightEl = nLooseGam = nTightGam = 0;
    TLorentzVector lep4vec, mu4vec, ele4vec, pho4vec, met4vec, Recoil4vec;

    if ( nJet > 0 ) {
      ak4pt0 = jetPt->at(0);
      ak4eta0 = jetEta->at(0);
    }  

    vector<int> jetlist;
    jetlist.clear();

    for(int i=0; i<nJet; i++){
      if( (jetID->at(i)>>0&1) == 1)
	jetlist.push_back(i);
    }

    int bJC = jetlist.at(0);
    for(size_t i =1; i < jetlist.size(); i++){
      float btag1 = (jetDeepCSVTags_b->at(bJC) + jetDeepCSVTags_bb->at(bJC));
      float btag2 = (jetDeepCSVTags_b->at(jetlist.at(i)) + jetDeepCSVTags_bb->at(jetlist.at(i)));
      if(btag1 < btag2  )
	bJC = jetlist.at(i);
    }
    leadbtag = jetDeepCSVTags_b->at(bJC) + jetDeepCSVTags_bb->at(bJC);

    auto looseMus = getLooseMu();
    auto tightMus = getTightMu();
    nLooseMu = looseMus.size();
    nTightMu = tightMus.size();

    if ( nTightMu > 0 ){
      int index = 0;
      mupt0 = muPt->at(index);
      mueta0 = muEta->at(index);
      float muphi0 = muPhi->at(index);
      float mue0 = muE->at(index);
      mu4vec.SetPtEtaPhiE(mupt0,mueta0,muphi0,mue0);
      lep4vec += mu4vec;
    }  

    auto looseEle = getLooseEle();
    auto tightEle = getTightEle();
    nLooseEl = looseEle.size();
    nTightEl = tightEle.size();
    if ( nTightEl >0 ){
      int index = 0;
      elpt0 = eleCalibEt->at(index);
      eleta0 = eleSCEta->at(index);
      float elphi0 = eleSCPhi->at(index);
      float ele0 = eleCalibE->at(index);
      ele4vec.SetPtEtaPhiE(elpt0,eleta0,elphi0,ele0);
      lep4vec += ele4vec;
    }  


    auto looseGams = getLoosePho();
    auto tightGams = getTightPho(15.0);
    nLooseGam = looseGams.size();
    nTightGam = tightGams.size();
    if ( nTightGam > 0 ){
      int index = 0;
      gpt0 = phoCalibEt->at(index);
      geta0 = phoSCEta->at(index);
      float gphi0 = phoSCPhi->at(index);
      float ge0 = phoCalibE->at(index);
      pho4vec.SetPtEtaPhiE(gpt0,geta0,gphi0,ge0);
      lep4vec += pho4vec;
    }
    //    lep_vec = mu_vec + ele_vec + pho_vec;
    met4vec.SetPtEtaPhiE(pfMET,0,pfMETPhi,pfMET);
    Recoil4vec = met4vec + lep4vec;
    Recoil = fabs( Recoil4vec.Pt() );
    RecoilPhi = Recoil4vec.Phi();

    printf("jentry:     %i\n",jentry);
    printf("event:      %i\n",event);
    printf("met:        %f\n",pfMET);
    printf("met_phi:    %f\n",pfMETPhi);
    printf("recoil:     %f\n",Recoil);
    printf("recoil_phi: %f\n",RecoilPhi);
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

    if (jentry%reportEvery == 0){
      cout<<"Finished entry "<<jentry<<"/"<<(nentriesToCheck-1)<<endl;
    }
  }

}//Closing the Loop function

void monoJetClass::BookHistos(const char* outputFilename) {

  output = new TFile(outputFilename, "RECREATE");
  output->cd();

  cutflow = new Cutflow({"Total Events","Triggers","One Loose Electron","One Tight Electron","pfMET50","Electron MET M_{T}","MET Filters",
      "Muon Veto","Photon Veto","Tau Veto","BJet Veto","Jet Selection","dPFCaloMET","minDPhiJetMET","Recoil250"});

  BookHistos(-1,"");
  for(int i = 0; i<nHisto; i++) {
    char ptbins[100];
    sprintf(ptbins, "_%d", i);
    string histname(ptbins);
    auto dir = output->mkdir( ("monoJet"+histname).c_str() );
    dir->cd();
    if (i == bHisto) {
      auto treedir = dir->mkdir("trees");
      treedir->cd();
      tree = new TTree("tree","tree");
      initTree(tree);
      scaleUncs.setTree(tree);
      shapeUncs.setDir(treedir);
      dir->cd();
    }
    BookHistos(i,histname);
  }
}

void monoJetClass::fillHistos(int nhist,float event_weight) {
  monoJetYear::fillHistos(nhist,event_weight);
  monoJetSingleEleCR::fillHistos(nhist,event_weight);
  weight = event_weight;
  if (nhist == bHisto) tree->Fill();
}

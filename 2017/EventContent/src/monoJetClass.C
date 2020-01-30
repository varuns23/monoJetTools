//For use with Ntuples made from JetAnalyzer
////Required arguments: 1 is folder containing input files, 2 is output file path, 3 is maxEvents (-1 to run over all events), 4 is reportEvery
////
////To compile using rootcom to an executable named 'analyze':
////$ ./rootcom monoJetClass analyze
////
////To run, assuming this is compiled to an executable named 'analyze':
////$ ./analyze /hdfs/store/user/uhussain/monoJet_Ntuples/ /cms/uhussain/MonomonoJetJet/CMSSW_8_0_8/src/LightZPrimeAnalysis/JetAnalyzer/test/output.root -1 10000
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

    float recoil,recoilPhi,ak4pt0,ak4eta0,leadbtag,mupt0,mueta0,elpt0,eleta0,gpt0,geta0;
    recoil = recoilPhi = ak4pt0 = ak4eta0 = leadbtag = mupt0 = mueta0 = elpt0 = eleta0 = gpt0 = geta0 = 0.0;
    int nLooseMu,nTightMu,nLooseEl,nTightEl,nLooseGam,nTightGam;
    nLooseMu = nTightMu = nLooseEl = nTightEl = nLooseGam = nTightGam = 0;
    TLorentzVector lep_vec,mu_vec,ele_vec,pho_vec,met_vec,recoil_vec;

    bool check_event = (event == 61228926 ||
	event == 79507460 ||
	event == 81725069 ||
	event == 29088699);

    if ( !check_event ) continue;


    if ( nJet > 0 ) {
      ak4pt0 = jetPt->at(0);
      ak4eta0 = jetEta->at(0);
    }

    vector<int> jetlist = monoJetAnalysis::getJetCand();
    int bJC = jetlist.at(0);
    for(size_t i =1; i < jetlist.size(); i++){
      float btag1 = (jetDeepCSVTags_b->at(bJC) + jetDeepCSVTags_bb->at(bJC));
      float btag2 = (jetDeepCSVTags_b->at(jetlist.at(i)) + jetDeepCSVTags_bb->at(jetlist.at(i)));
      if(btag1 < btag2  )
	bJC = jetlist.at(i);
    }
    leadbtag = jetDeepCSVTags_b->at(bJC) + jetDeepCSVTags_bb->at(bJC);

    auto looseMus = getLooseMu();
    auto tightMus = monoJetAnalysis::getTightMu();
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

    auto looseEle = getLooseEle();
    auto tightEle = getTightEle();
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

    auto looseGams = getLoosePho();
    auto tightGams = getTightPho();
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

    printf("jentry:     %i\n",jentry);
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

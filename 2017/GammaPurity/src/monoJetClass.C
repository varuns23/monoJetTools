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
  
  if (!sample.isData) SetScalingHistos();

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
    
    float event_weight = 1.;
    if (!sample.isData) {
      ApplyPileup(event_weight);
    }
    
    cutflow->Fill(0,event_weight);
    if ((HLTPho>>11&1) == 1) {
      cutflow->Fill(1,event_weight);
      if (getMetFilter()) {
	cutflow->Fill(2,event_weight);
	phoindex = getPhoIndex();
	photon_pt = phoEt->at(phoindex);
	photon_eta = phoEta->at(phoindex);
	photon_phi = phoPhi->at(phoindex);
	photon_sieie = phoSigmaIEtaIEtaFull5x5->at(phoindex);
	if (phoindex != -1) {
	  cutflow->Fill(3,event_weight);
	  jetindex = getJetIndex(phoindex);
	  if (jetindex != -1) {
	    cutflow->Fill(4,event_weight);
	    if (pfMET < 60) {
	      cutflow->Fill(5,event_weight);
	      fillHistos(5,event_weight);
	    }
	  }
	}
      }
    }
    
    if (jentry%reportEvery == 0){
      cout<<"Finished entry "<<jentry<<"/"<<(nentriesToCheck-1)<<endl;
    }
  }
   
}//Closing the Loop function

void monoJetClass::BookHistos(const char* outputFilename) {
  
  output = new TFile(outputFilename, "RECREATE");
  output->cd();
  
  cutflow = new Cutflow({"Total Events","Triggers","MET Filters","Photon Selection","Jet Selection","MET60"});

  BookHistos(-1,"");
  for(int i = 5; i<nHisto; i++) {
    char ptbins[100];
    sprintf(ptbins, "_%d", i);
    string histname(ptbins);
    auto dir = output->mkdir( ("monoJet"+histname).c_str() );
    dir->cd();
    if (i == bHisto) {
      auto treedir = dir->mkdir("trees");
      treedir->cd();
      tree = new TTree("norm","norm");
      initTree(tree);
      scaleUncs.setTree(tree);
      shapeUncs.setDir(treedir);
      dir->cd();
    }
    BookHistos(i,histname);
  }
}

void monoJetClass::fillHistos(int nhist,float event_weight) {
  // monoJetYear::fillHistos(nhist,event_weight);
  // monoJetGammaCR::fillHistos(nhist,event_weight);
  weight = event_weight;
  if (nhist == bHisto) tree->Fill();
}


int monoJetClass::getJetIndex(int phoindex) {
    for (int ijet = 0; ijet < nJet; ijet++) {
      if ((jetID->at(ijet)>>0&1) == 0) {
	if (jetPt->at(ijet) > 100 && fabs(jetEta->at(ijet)) < 2.4) {
	  if (deltaR(jetEta->at(ijet),jetPhi->at(ijet),phoSCEta->at(phoindex),phoSCPhi->at(phoindex)) > 0.4) {
	    return ijet;
	  }
	}
      }
    }
    return -1;
}

int monoJetClass::getPhoIndex() {
  for (int ipho = 0; ipho < nPho; ipho++) {
    if (phoEt->at(ipho) > 200.0 && fabs(phoEta->at(ipho)) < 1.479) {
      if (phoHoverE->at(ipho) < 0.035) {
	float pt = phoEt->at(ipho);
	bool PFChIso = phoPFChIso->at(ipho) < 1.416;
	bool PFNeuIso = phoPFNeuIso->at(ipho) < 2.491 + 0.0126*pt + 0.000026*pt*pt;
	bool PFPhoIso = phoPFPhoIso->at(ipho) < 2.952 + 0.0040*pt;
	bool PFIso = PFChIso && PFNeuIso && PFPhoIso;
	
	if (sample.type == GJets && PFIso) return ipho;
	else if (sample.type == Data && !PFIso) return ipho;
      }
    }
  }
  return -1;
}

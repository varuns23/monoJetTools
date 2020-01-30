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
      // if (isW_or_ZJet()) {
      // 	SetBoson(sample.PID);
      // 	ApplyKFactor(event_weight);
      // }
    }

    fillEvent(0,genWeight);
    if (getEGammaTrigger() && inclusiveCut()) {
      fillEvent(1,event_weight);

      vector<int> looselist = getLooseEle();
      if (looselist.size() == 2) {
	fillEvent(2,event_weight);

	vector<int> tightlist = getTightEle(looselist);
	if (CRSelection(tightlist,looselist)) {
	  fillEvent(3,event_weight);

	  if (dilepton_mass > diLeptonMassCutLow && dilepton_mass < diLeptonMassCutHigh) {
	    fillEvent(4,event_weight);

	    if (muon_veto(leadLepIndx,subleadLepIndx)) {
	      fillEvent(5,event_weight);

	      if (photon_veto(leadLepIndx,subleadLepIndx)) {
		fillEvent(6,event_weight);

		if (tau_veto(leadLepIndx,subleadLepIndx)) {
		  fillEvent(7,event_weight);

		  if (bjet_veto(leadLepIndx,subleadLepIndx)) {
		    fillEvent(8,event_weight);

		    vector<int> jetlist = jet_veto(leadLepIndx,subleadLepIndx);
		    jetCand = getJetCand(jetlist,leadLepIndx,subleadLepIndx);
		    setJetCand(jetCand);
		    if (jetCand.size() > 0) {
		      fillEvent(9,event_weight);

		      float dpfcalo = fabs(pfMET-caloMET)/recoil;
		      h_metcut->Fill(dpfcalo,event_weight);
		      if (dpfcalo < metRatioCut) {
			fillEvent(10,event_weight);

			float mindPhiJetMET = dPhiJetMETmin(jetlist,recoilPhi);
			h_dphimin->Fill(mindPhiJetMET,event_weight);
			if (mindPhiJetMET > dPhiJetMETCut) {
			  fillEvent(11,event_weight);

			  if (recoil > recoilCut) {
			    fillEvent(12,event_weight);
			    
			  }
			}
		      }
		    }
		  }
		}
	      }
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
  
  cutflow = new Cutflow({"Total Events","Trigger","MET Filters","Two Loose Electrons","One Tight Electron","ZMass",
	"Muon Veto","Photon Veto","Tau Veto","BJet Veto","Jet Selection","dPFCaloMET","mindPhiJetMET","Recoil250"});

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
  monoJetYear::fillHistos(nhist,event_weight);
  monoJetDoubleEleCR::fillHistos(nhist,event_weight);
  weight = event_weight;
  if (nhist == bHisto) tree->Fill();
}

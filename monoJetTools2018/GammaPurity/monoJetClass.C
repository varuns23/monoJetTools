#define monoJetClass_cxx
#include "monoJetClass.h"

using namespace std;

int main(int argc, const char* argv[]) { 
  if (argc == 1) {
    printf("Running Test\n");
    argv[1] = "/hdfs/store/user/varuns/NTuples/MC/MC2017_12Apr2018_JECv32/GJets/GJets_HT400-600/0000/";
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

  int nTotalEvents = 0;
  int nJetCand = 0;
  int nPhoCand = 0;
  int nMET60 = 0;


  if (maxEvents != -1LL && nentries > maxEvents)
    nentriesToCheck = maxEvents;
  int nTotal = nentriesToCheck;
  Long64_t nbytes = 0, nb = 0;
  cout<<"Running over "<<nTotal<<" events."<<endl;
  for (Long64_t jentry=0; jentry<nentriesToCheck;jentry++) {
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   nbytes += nb;
    float event_weight = 1;
    
    nTotalEvents += event_weight;

    jetindex = getJetIndex();
    if (jetindex != -1) {
      nJetCand += event_weight;

      phoindex = getPhoIndex();
      if (phoindex != -1) {
	nPhoCand += event_weight;

	if (pfMET < 60) {
	  nMET60 += event_weight;
	  fillHistos(0,event_weight);
	}
      }
    }

    if (jentry%reportEvery == 0)
      cout<<"Finished entry "<<jentry<<"/"<<(nentriesToCheck - 1)<<endl;
  }
  h_cutflow->SetBinContent(1,nTotalEvents);
  h_cutflow->SetBinContent(2,nJetCand);
  h_cutflow->SetBinContent(3,nPhoCand);
  h_cutflow->SetBinContent(4,nMET60);
}

void monoJetClass::initTree(TTree* tree) {
}

void monoJetClass::BookHistos(const char* outputFilename) {

  output = new TFile(outputFilename, "RECREATE");
  output->cd();

  h_cutflow = new TH1D("h_cutflow","h_cutflow",4,0,4);h_cutflow->Sumw2();
  h_cutflow->GetXaxis()->SetBinLabel(1,"Total Events");
  h_cutflow->GetXaxis()->SetBinLabel(2,"Jet Selection");
  h_cutflow->GetXaxis()->SetBinLabel(3,"EM Selection");
  h_cutflow->GetXaxis()->SetBinLabel(4,"MET60");

  h_photonPt = new TH1F("photonPt","photonPt;Photon P_{T} (GeV)",50,0,1000);
  h_phoSigmaIEtaIEta = new TH1F("phoSigmaIEtaIEta","phoSigmaIEtaIEta;Photon #sigma_{i#eta i#eta}",50,0,0.1);
}

void monoJetClass::fillHistos(int nhist,float event_weight) {
  h_photonPt->Fill(phoEt->at(phoindex),event_weight);
  h_phoSigmaIEtaIEta->Fill(phoSigmaIEtaIEtaFull5x5->at(phoindex),event_weight);
}

int monoJetClass::getJetIndex() {
    for (int ijet = 0; ijet < nJet; ijet++) {
      if (jetPt->at(ijet) > 100 && fabs(jetEta->at(ijet)) < 2.5) {
	return ijet;
      }
    }
    return -1;
}

int monoJetClass::getPhoIndex() {
  for (int ipho = 0; ipho < nPho; ipho++) {
    if (phoEt->at(ipho) > 175 && fabs(phoEta->at(ipho)) < 1.5) {
      if (phoHoverE->at(ipho) < 0.0396) {
	if (phoSigmaIEtaIEtaFull5x5->at(ipho) < 0.015) {
	  if (phoPFChIso->at(ipho) < 11.0) {
	    float phoPt = phoEt->at(ipho);
	    float phoPFNeuIsoCut = 2.725 + 0.0148 * phoPt + 0.000017 * (phoPt * phoPt);
	    if (phoPFNeuIso->at(ipho) < phoPFNeuIsoCut) {
	      float phoPFPhoIsoCut = 2.571 + 0.0047 * phoPt;
	      if (phoPFNeuIso->at(ipho) < phoPFPhoIsoCut) {
		if (phohasPixelSeed->at(ipho) == 1) {

		  if ( sample.type == GJets ) {
		    if (phoSigmaIEtaIEtaFull5x5->at(ipho) < 0.01022 && phoPFChIso->at(ipho) < 0.441) {
		      return ipho;
		    } 
		  } else {
		    return ipho;
		  }
		  
		}
	      }
	    }
	  }
	}
      }
    }
  }
  return -1;
}

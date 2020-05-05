//////////////////////////////////////////////////////////
// This class has been automatically generated on
// Wed Mar  8 10:15:33 2017 by ROOT version 6.06/01
// from TTree EventTree/Event data (tag V08_00_24_00)
//////////////////////////////////////////////////////////

#ifndef monoJetClass_h
#define monoJetClass_h

#include "monoJetYear.h"
#include "monoJetGammaCR.h"

using namespace std;
class monoJetClass : public monoJetYear, public monoJetGammaCR {
public :
  static const int nHisto = 12;
  static const int bHisto = 11;

  float photon_phoiso;
  
  static const int nPhoPtBins = 8;                
  const float phoPtBins[nPhoPtBins+1] = {230., 250., 280., 320., 375., 425., 475., 550., 2000.};

  TH1F *h_phoPFIso[nHisto],*h_phoPt_ptbins[nHisto][nPhoPtBins],*h_phoPFIso_ptbins[nHisto][nPhoPtBins];
  TH2F *h_phoPFIsoSigmaIEtaIEta[nHisto];
 
  monoJetClass(const char* file1,const char* file2,int nfiles) : monoJetAnalysis(file1,file2,nfiles) {
    BookHistos(file2); };
  monoJetClass(const char* inputFilename,const char* outputFilename,const char* fileRange) : monoJetAnalysis(inputFilename,outputFilename,fileRange) {
    BookHistos(outputFilename);
  }
  monoJetClass(const char* inputFilename,const char* outputFilename,vector<const char*> filelist) : monoJetAnalysis(inputFilename,outputFilename,filelist) {
    BookHistos(outputFilename);
  }
  void     Loop(Long64_t maxEvents, int reportEvery);
  void BookHistos(const char* file2);
  void BookHistos(int i,string histname) {
    monoJetYear::BookHistos(i,histname);
    monoJetGammaCR::BookHistos(i,histname);
  }
  void fillHistos(int nhist,float event_weight=1.0);
  void initVars() {
    monoJetYear::initVars();
    monoJetGammaCR::initVars();
    photon_phoiso = -1;
  }
  void initTree(TTree* tree) {
    monoJetYear::initTree(tree);
    monoJetGammaCR::initTree(tree);
    tree->Branch("photonPt",&photon_pt,"Photon P_{T} (GeV)");
    tree->Branch("photonSigmaIEtaIEta",&photon_sieie,"Photon #sigma_{i#eta i#eta}");
    tree->Branch("phoPFIso",&photon_phoiso);
  }

  int getPhoIndex();
  int getJetIndex(int phoindex);

  bool CutBasedPFPhotonID_noPhoIso(Int_t ipho, TString phoWP);
  double EAcharged(double eta);
  double EAneutral(double eta);
  double EAphoton(double eta);
  inline double phoPFChIso_RhoCor(int ipho)  { return TMath::Max(((*phoPFChIso)[ipho]  - rho*EAcharged((*phoSCEta)[ipho])), 0.0); }
  inline double phoPFNeuIso_RhoCor(int ipho) { return TMath::Max(((*phoPFNeuIso)[ipho] - rho*EAneutral((*phoSCEta)[ipho])), 0.0); }
  inline double phoPFPhoIso_RhoCor(int ipho) { return TMath::Max(((*phoPFPhoIso)[ipho] - rho*EAphoton( (*phoSCEta)[ipho])), 0.0); }

  inline bool getTightPhoID(int ipho) {
    if ( fabs((*phoSCEta)[ipho]) <= 1.4442 ) { // EB
      return ( 
	      ((*phoHoverE)[ipho]                <  0.02148 ) &&
	      // ((*phoSigmaIEtaIEtaFull5x5)[ipho]  <  0.00996 ) &&
	      ((*phoEleVeto)[ipho]                == 1) &&
	      (phoPFChIso_RhoCor(ipho)  < 0.65 )  &&
	      (phoPFNeuIso_RhoCor(ipho) < (0.317 + (0.01512 * (*phoEt)[ipho]) + (0.00002259 * pow((*phoEt)[ipho], 2.0))) )
	      //                    (phoPFPhoIso_RhoCor(ipho) < (2.044 + (0.004017 * (*phoEt)[ipho])) )
	       );
    } else {
      return ( 
	      ((*phoHoverE)[ipho]                <  0.0321 ) &&
	      // ((*phoSigmaIEtaIEtaFull5x5)[ipho]  <  0.0271 ) &&
	      ((*phoEleVeto)[ipho]                == 1) &&
	      (phoPFChIso_RhoCor(ipho)  < 0.517 )  &&
	      (phoPFNeuIso_RhoCor(ipho) < (2.716 + (0.01117 * (*phoEt)[ipho]) + (0.000023 * pow((*phoEt)[ipho], 2.0))) )
	      //                    (phoPFPhoIso_RhoCor(ipho) < (3.032 + (0.0037 * (*phoEt)[ipho])) )
	       );
    }
  }
  inline bool getMediumPhoID(int ipho) {
    if( fabs((*phoSCEta)[ipho]) <= 1.4442){ // EB
      return ( 
	      ((*phoHoverE)[ipho]                <  0.02197 ) &&
	      // ((*phoSigmaIEtaIEtaFull5x5)[ipho]  <  0.01015 ) &&
	      ((*phoEleVeto)[ipho]                == 1) &&
	      (phoPFChIso_RhoCor(ipho)  < 1.141 )  &&
	      (phoPFNeuIso_RhoCor(ipho) < (1.189 + (0.01512 * (*phoEt)[ipho]) + (0.00002259 * pow((*phoEt)[ipho], 2.0))) )
	      //                    (phoPFPhoIso_RhoCor(ipho) < (2.08 + (0.004017 * (*phoEt)[ipho])) )
	       );
    } else { // EE
      return ( 
	      ((*phoHoverE)[ipho]                <  0.0326 ) &&
	      // ((*phoSigmaIEtaIEtaFull5x5)[ipho]  <  0.0272 ) &&
	      ((*phoEleVeto)[ipho]                == 1) &&
	      (phoPFChIso_RhoCor(ipho)  < 1.051 )  &&
	      (phoPFNeuIso_RhoCor(ipho) < (2.718 + (0.01117 * (*phoEt)[ipho]) + (0.000023 * pow((*phoEt)[ipho], 2.0))) )
	      //                    (phoPFPhoIso_RhoCor(ipho) < (3.867 + (0.0037 * (*phoEt)[ipho])) )
	       );
    }
    
  }
  inline bool getLoosePhoID(int ipho) {
    if( fabs((*phoSCEta)[ipho]) < 1.4442){ // EB
      return ( 
	      ((*phoHoverE)[ipho]                <  0.04596 ) &&
	      // ((*phoSigmaIEtaIEtaFull5x5)[ipho]  <  0.0106 ) &&
	      ((*phoEleVeto)[ipho]                == 1) &&
	      (phoPFChIso_RhoCor(ipho)  < 1.694 )  &&
	      (phoPFNeuIso_RhoCor(ipho) < (24.032 + (0.01512 * (*phoEt)[ipho]) + (0.00002259 * pow((*phoEt)[ipho], 2.0))) )
	      //                    (phoPFPhoIso_RhoCor(ipho) < (2.876 + (0.004017 * (*phoEt)[ipho])) )
	       );
    }
    if( fabs((*phoSCEta)[ipho]) > 1.4442){ // EE
      return ( 
	      ((*phoHoverE)[ipho]                <  0.0590 ) &&
	      // ((*phoSigmaIEtaIEtaFull5x5)[ipho]  <  0.0272 ) &&
	      ((*phoEleVeto)[ipho]                == 1) &&
	      (phoPFChIso_RhoCor(ipho)  < 2.089 )  &&
	      (phoPFNeuIso_RhoCor(ipho) < (19.722 + (0.01117 * (*phoEt)[ipho]) + (0.000023 * pow((*phoEt)[ipho], 2.0))) )
	      //                    (phoPFPhoIso_RhoCor(ipho) < (4.162 + (0.0037 * (*phoEt)[ipho])) )
	       );
    }
  }
};

#endif

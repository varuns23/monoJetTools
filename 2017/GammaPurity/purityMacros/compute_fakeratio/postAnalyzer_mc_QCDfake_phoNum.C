////postAnalyzer.C
//For use with Ntuples made from ggNtuplizer
//Required arguments: 1 is folder containing input files, 2 is output file path, 3 is maxEvents (-1 to run over all events), 4 is reportEvery
//
//To compile using rootcom to an executable named 'analyze':
//$ ./rootcom postAnalyzer analyze
//
//To run, assuming this is compiled to an executable named 'analyze':
//$ ./analyze /hdfs/store/user/jjbuch/LatestNtuples/ /afs/hep.wisc.edu/user/jjbuchanan/private/CMSSW_7_4_9/src/output.root -1 10000
//Runs over every event in the folder LatestNtuples, reporting progress every 10000 events
//and storing the resulting histograms in the file output.root.
//
//To plot, for example, single photon trigger efficiency as a function of photon pt:
//$ root -l
//root[0] TFile *f = new TFile("output.root");
//root[1] TGraphAsymmErrors *efficiency = new TGraphAsymmErrors((TH1F*)f->Get("Photon_Et_300_2"),(TH1F*)f->Get("Photon_Et_300_1"));
//root[2] efficiency->Draw("AP")
//root[3] efficiency->SetTitle("Single photon trigger efficiency")
//root[4] efficiency->GetXaxis()->SetTitle("Photon p_{T}")
//root[5] efficiency->Draw("AP")
//

#define postAnalyzer_cxx
#include "postAnalyzer_mc_QCDfake_phoNum.h"
#include <TH2.h>
#include <TStyle.h>
#include <TCanvas.h>
#include "TH1F.h"
#include <iostream>
#include <bitset>
#include <climits>
#include <cstring>
#include "Math/Minimizer.h"
#include "Math/Factory.h"
#include "Math/Functor.h"
#include "TStopwatch.h"
#include <algorithm>
#include <vector>
#include <iterator>
#include <list>
#include <set>
using namespace std;
using std::vector;
int main(int argc, const char* argv[])
{
  Long64_t maxEvents = atof(argv[3]);
  if (maxEvents < -1LL)
  {
    std::cout<<"Please enter a valid value for maxEvents (parameter 3)."<<std::endl;
    return 1;
  }
  int reportEvery = atof(argv[4]);
  if (reportEvery < 1)
  {
    std::cout<<"Please enter a valid value for reportEvery (parameter 4)."<<std::endl;
    return 1;
  }
  postAnalyzer t(argv[1],argv[2]);
  t.Loop(maxEvents,reportEvery);
  return 0;
}

void postAnalyzer::Loop(Long64_t maxEvents, int reportEvery)
{
  if (fChain == 0) return;
  int nTotal;
  nTotal = 0;

  std::vector<int> phoCandNum;
  phoCandNum.clear();
  std::vector<int> phoCandNum_QCD;
  phoCandNum_QCD.clear();
  std::vector<int> phoCandNum_QCDsbUp;
  phoCandNum_QCDsbUp.clear();
  std::vector<int> phoCandNum_QCDsbDown;
  phoCandNum_QCDsbDown.clear();
  
  std::vector<int> genPhotons;
  genPhotons.clear();
  
  Long64_t nentries = fChain->GetEntries();
  std::cout<<"Coming in: "<<std::endl;
  std::cout<<"nentries:"<<nentries<<std::endl;
  //Look at up to maxEvents events, or all if maxEvents == -1.
  Long64_t nentriesToCheck = nentries;
  if (maxEvents != -1LL && nentries > maxEvents)
    nentriesToCheck = maxEvents;
  nTotal = nentriesToCheck;
  Long64_t nbytes = 0, nb = 0;

  std::cout<<"Running over "<<nTotal<<" events."<<std::endl;
  TStopwatch sw;
  sw.Start();
  for (Long64_t jentry=0; jentry<nentriesToCheck;jentry++)
  {
    
    event_.clear();
    event_info.clear();
    
    Long64_t ientry = LoadTree(jentry);
    if (ientry < 0) break;
    nb = fChain->GetEntry(jentry);   nbytes += nb;

    //Blinding policy - not needed for QCD fake rate estimate
    // if (run > 274421 && (event % 4) != 0) continue;
    
    //=1.0 for real data
    double event_weight=1.0;
    
    phoCandNum   = getPhoCandNum(175.0,1.4442);
    phoCandNum_QCD = getPhoCandNum_QCDsideband(175.0,1.4442);
    phoCandNum_QCDsbUp = getPhoCandNum_QCDsbUp(175.0,1.4442);
    phoCandNum_QCDsbDown = getPhoCandNum_QCDsbDown(175.0,1.4442);
    
    // Match gen photons to reco photons
    genPhotons.clear();
    for(int p=0;p<nMC;p++)
    {
      bool status = mcPID->at(p) == 22 && (((mcStatusFlag->at(p)>>0)&1)==1 || ((mcStatusFlag->at(p)>>1)&1)==1);
      if(status)
        genPhotons.push_back(p);
    }
    float dRmin = 9999.9;
    float dRmin_QCD = 9999.9;
    float dRmin_QCDsbUp = 9999.9;
    float dRmin_QCDsbDown = 9999.9;
    for(int p = 0; p < genPhotons.size(); p++)
    {
      float dRtemp = 9999.9;
      if(phoCandNum.size() > 0)
        dRtemp = dR(mcEta->at(genPhotons[p]),mcPhi->at(genPhotons[p]),phoSCEta->at(phoCandNum[0]),phoSCPhi->at(phoCandNum[0]));
      if(dRtemp < dRmin)
        dRmin = dRtemp;
      dRtemp = 9999.9;
      if(phoCandNum_QCD.size() > 0)
        dRtemp = dR(mcEta->at(genPhotons[p]),mcPhi->at(genPhotons[p]),phoSCEta->at(phoCandNum_QCD[0]),phoSCPhi->at(phoCandNum_QCD[0]));
      if(dRtemp < dRmin_QCD)
        dRmin_QCD = dRtemp;
      dRtemp = 9999.9;
      if(phoCandNum_QCDsbUp.size() > 0)
        dRtemp = dR(mcEta->at(genPhotons[p]),mcPhi->at(genPhotons[p]),phoSCEta->at(phoCandNum_QCDsbUp[0]),phoSCPhi->at(phoCandNum_QCDsbUp[0]));
      if(dRtemp < dRmin_QCDsbUp)
        dRmin_QCDsbUp = dRtemp;
      dRtemp = 9999.9;
      if(phoCandNum_QCDsbDown.size() > 0)
        dRtemp = dR(mcEta->at(genPhotons[p]),mcPhi->at(genPhotons[p]),phoSCEta->at(phoCandNum_QCDsbDown[0]),phoSCPhi->at(phoCandNum_QCDsbDown[0]));
      if(dRtemp < dRmin_QCDsbDown)
        dRmin_QCDsbDown = dRtemp;
    }

    //Sequential cuts
    //Systematics bins:
    //    0       1      2       3       4       5       6        7         8
    // Standard  sbUP  sbDown  metUP  metDown  binUP  binDown  sieieUp  sieieDown
    // if(metFilters==0)
    // {
      // if(HLTPho>>12&1 == 1)
      // {
        if(pfMET < 30)
        {
          if(dRmin < 0.1)
          {
            // Trigger inefficiency factor
            // event_weight*=(1.013 - 0.0001168*phoEt->at(phoCandNum[0]));
            event_weight = 1.0;
            event_weight *= sieie_correction((*phoSigmaIEtaIEtaFull5x5)[phoCandNum[0]], 0);
              fillHistosNum(0,phoCandNum[0],event_weight);
              fillHistosNum(1,phoCandNum[0],event_weight);
              fillHistosNum(2,phoCandNum[0],event_weight);
              fillHistosNum(5,phoCandNum[0],event_weight);
              fillHistosNum(6,phoCandNum[0],event_weight);
            event_weight = 1.0;
            event_weight *= sieie_correction((*phoSigmaIEtaIEtaFull5x5)[phoCandNum[0]], 1);
              fillHistosNum(7,phoCandNum[0],event_weight);
            event_weight = 1.0;
            event_weight *= sieie_correction((*phoSigmaIEtaIEtaFull5x5)[phoCandNum[0]], 2);
              fillHistosNum(8,phoCandNum[0],event_weight);
          }
          if(dRmin_QCD < 0.1)
          {
            event_weight = 1.0;
            event_weight *= sieie_correction((*phoSigmaIEtaIEtaFull5x5)[phoCandNum_QCD[0]], 0);
              fillHistosNum_QCD(0,phoCandNum_QCD[0],event_weight);
              fillHistosNum_QCD(5,phoCandNum_QCD[0],event_weight);
              fillHistosNum_QCD(6,phoCandNum_QCD[0],event_weight);
            event_weight = 1.0;
            event_weight *= sieie_correction((*phoSigmaIEtaIEtaFull5x5)[phoCandNum_QCD[0]], 1);
              fillHistosNum_QCD(7,phoCandNum_QCD[0],event_weight);
            event_weight = 1.0;
            event_weight *= sieie_correction((*phoSigmaIEtaIEtaFull5x5)[phoCandNum_QCD[0]], 2);
              fillHistosNum_QCD(8,phoCandNum_QCD[0],event_weight);
          }
          if(dRmin_QCDsbUp < 0.1)
          {
            event_weight = 1.0;
            event_weight *= sieie_correction((*phoSigmaIEtaIEtaFull5x5)[phoCandNum_QCDsbUp[0]], 0);
              fillHistosNum_QCD(1,phoCandNum_QCDsbUp[0],event_weight);
          }
          if(dRmin_QCDsbDown < 0.1)
          {
            event_weight = 1.0;
            event_weight *= sieie_correction((*phoSigmaIEtaIEtaFull5x5)[phoCandNum_QCDsbDown[0]], 0);
              fillHistosNum_QCD(2,phoCandNum_QCDsbDown[0],event_weight);
          }
        }
        if(pfMET < 36)
        {
          if(dRmin < 0.1)
          {
            // Trigger inefficiency factor
            // event_weight*=(1.013 - 0.0001168*phoEt->at(phoCandNum[0]));
            event_weight = 1.0;
            event_weight *= sieie_correction((*phoSigmaIEtaIEtaFull5x5)[phoCandNum[0]], 0);
              fillHistosNum(3,phoCandNum[0],event_weight);
          }
          if(dRmin_QCD < 0.1)
          {
            event_weight = 1.0;
            event_weight *= sieie_correction((*phoSigmaIEtaIEtaFull5x5)[phoCandNum_QCD[0]], 0);
              fillHistosNum_QCD(3,phoCandNum_QCD[0],event_weight);
          }
        }
        if(pfMET < 25)
        {
          if(dRmin < 0.1)
          {
            // Trigger inefficiency factor
            // event_weight*=(1.013 - 0.0001168*phoEt->at(phoCandNum[0]));
            event_weight = 1.0;
            event_weight *= sieie_correction((*phoSigmaIEtaIEtaFull5x5)[phoCandNum[0]], 0);
              fillHistosNum(4,phoCandNum[0],event_weight);
          }
          if(dRmin_QCD < 0.1)
          {
            event_weight = 1.0;
            event_weight *= sieie_correction((*phoSigmaIEtaIEtaFull5x5)[phoCandNum_QCD[0]], 0);
              fillHistosNum_QCD(4,phoCandNum_QCD[0],event_weight);
          }
        }
      // }
    // }

    tree->Fill();
    
    if (jentry%reportEvery == 0)
      {
        std::cout<<"Finished entry "<<jentry<<"/"<<(nentriesToCheck-1)<<std::endl;
      }
  }

  if((nentriesToCheck-1)%reportEvery != 0)
    std::cout<<"Finished entry "<<(nentriesToCheck-1)<<"/"<<(nentriesToCheck-1)<<std::endl;
  sw.Stop();
  std::cout<<"All events checked."<<std::endl;
  //Report
  std::cout << "RealTime : " << sw.RealTime() / 60.0 << " minutes" << std::endl;
    std::cout << "CPUTime  : " << sw.CpuTime()  / 60.0 << " minutes" << std::endl;
  std::cout << std::endl;
  std::cout<<"Total number of events: "<<nTotal<<std::endl;
  std::cout << "Number of events inspected: " << nTotal << std::endl;
  std::cout<<std::endl;
}

void postAnalyzer::BookHistos(const char* file2)
{
  fileName = new TFile(file2, "RECREATE");
  tree = new TTree("ADD","ADD");
  tree->Branch("event_","std::vector<unsigned int>",&event_);
  tree->Branch("event_info","std::vector<double>",&event_info);
  fileName->cd();
  
  // Float_t PtBins[7]={175.,200.,250., 300., 400., 600., 1000.0};
  // Float_t MetBins[7]={170.,200.,250., 300., 400., 600., 1000.0};

  //Set up the histos to be filled with method fillHistos
  for(int i=0; i<10; i++)
  {
    char ptbins_i[100];
    sprintf(ptbins_i, "_%d", i);
    std::string histname_i(ptbins_i);
    for(int j=0; j<9; j++)
    {
      char ptbins_j[100];
      sprintf(ptbins_j,"_%d",j);
      std::string histname_j(ptbins_j);
      int nsieiebins = 100;
      if(j==5) nsieiebins = 200;
      if(j==6) nsieiebins = 50;
      h_sieie_num[i][j] = new TH1F(("h_sieie_num"+histname_i+histname_j).c_str(),"h_sieie_num",nsieiebins,0.,0.025);h_sieie_num[i][j]->Sumw2();
      h_sieie_num_QCD[i][j] = new TH1F(("h_sieie_num_QCD"+histname_i+histname_j).c_str(),"h_sieie_num_QCD",nsieiebins,0.,0.025);h_sieie_num_QCD[i][j]->Sumw2();
      h_nVtx_num[i][j] = new TH1F(("h_nVtx_num"+histname_i+histname_j).c_str(),"h_nVtx_num",30,0,30);h_nVtx_num[i][j]->Sumw2();
      h_nVtx_num_QCD[i][j] = new TH1F(("h_nVtx_num_QCD"+histname_i+histname_j).c_str(),"h_nVtx_num_QCD",30,0,30);h_nVtx_num_QCD[i][j]->Sumw2();
      h_phoPt_num[i][j] = new TH1F(("h_phoPt_num"+histname_i+histname_j).c_str(),"h_phoPt_num",165,0.,1000.);h_phoPt_num[i][j]->Sumw2();
      h_phoPt_num_QCD[i][j] = new TH1F(("h_phoPt_num_QCD"+histname_i+histname_j).c_str(),"h_phoPt_num_QCD",165,0.,1000.);h_phoPt_num_QCD[i][j]->Sumw2();
      h_pfMET_num[i][j] = new TH1F(("h_pfMET_num"+histname_i+histname_j).c_str(),"h_pfMET_num",300,0.,300.);h_pfMET_num[i][j]->Sumw2();
      h_pfMET_num_QCD[i][j] = new TH1F(("h_pfMET_num_QCD"+histname_i+histname_j).c_str(),"h_pfMET_num_QCD",300,0.,300.);h_pfMET_num_QCD[i][j]->Sumw2();
    }
  }
}

void postAnalyzer::fillHistosNum(int sysNumber, int index,double event_weight)
{
  Float_t uncorrectedPhoEt = ((*phoSCRawE)[index]/TMath::CosH((*phoSCEta)[index]));
  int histoNumber = 0;
  h_sieie_num[histoNumber][sysNumber]->Fill(phoSigmaIEtaIEtaFull5x5->at(index),event_weight);
  h_nVtx_num[histoNumber][sysNumber]->Fill(nVtx,event_weight);
  h_phoPt_num[histoNumber][sysNumber]->Fill(uncorrectedPhoEt,event_weight);
  h_pfMET_num[histoNumber][sysNumber]->Fill(pfMET,event_weight);
  
  if(uncorrectedPhoEt > 175.0 && uncorrectedPhoEt <= 200.0)
    histoNumber = 1;
  else if(uncorrectedPhoEt > 200.0 && uncorrectedPhoEt <= 250.0)
    histoNumber = 2;
  else if(uncorrectedPhoEt > 250.0 && uncorrectedPhoEt <= 300.0)
    histoNumber = 3;
  else if(uncorrectedPhoEt > 300.0 && uncorrectedPhoEt <= 400.0)
    histoNumber = 4;
  else if(uncorrectedPhoEt > 400.0)
    histoNumber = 5;
  if(histoNumber > 0)
  {
    h_sieie_num[histoNumber][sysNumber]->Fill(phoSigmaIEtaIEtaFull5x5->at(index),event_weight);
    h_nVtx_num[histoNumber][sysNumber]->Fill(nVtx,event_weight);
    h_phoPt_num[histoNumber][sysNumber]->Fill(uncorrectedPhoEt,event_weight);
    h_pfMET_num[histoNumber][sysNumber]->Fill(pfMET,event_weight);
  }
  if(nVtx <= 11)
    histoNumber = 6;
  else if(nVtx > 11 && nVtx <= 15)
    histoNumber = 7;
  else if(nVtx > 15 && nVtx <= 20)
    histoNumber = 8;
  else if(nVtx > 20)
    histoNumber = 9;
  if(histoNumber > 4)
  {
    h_sieie_num[histoNumber][sysNumber]->Fill(phoSigmaIEtaIEtaFull5x5->at(index),event_weight);
    h_nVtx_num[histoNumber][sysNumber]->Fill(nVtx,event_weight);
    h_phoPt_num[histoNumber][sysNumber]->Fill(uncorrectedPhoEt,event_weight);
    h_pfMET_num[histoNumber][sysNumber]->Fill(pfMET,event_weight);
  }
}

void postAnalyzer::fillHistosNum_QCD(int sysNumber, int index,double event_weight)
{
  Float_t uncorrectedPhoEt = ((*phoSCRawE)[index]/TMath::CosH((*phoSCEta)[index]));
  int histoNumber = 0;
  h_sieie_num_QCD[histoNumber][sysNumber]->Fill(phoSigmaIEtaIEtaFull5x5->at(index),event_weight);
  h_nVtx_num_QCD[histoNumber][sysNumber]->Fill(nVtx,event_weight);
  h_phoPt_num_QCD[histoNumber][sysNumber]->Fill(uncorrectedPhoEt,event_weight);
  h_pfMET_num_QCD[histoNumber][sysNumber]->Fill(pfMET,event_weight);
  
  if(uncorrectedPhoEt > 175.0 && uncorrectedPhoEt <= 200.0)
    histoNumber = 1;
  else if(uncorrectedPhoEt > 200.0 && uncorrectedPhoEt <= 250.0)
    histoNumber = 2;
  else if(uncorrectedPhoEt > 250.0 && uncorrectedPhoEt <= 300.0)
    histoNumber = 3;
  else if(uncorrectedPhoEt > 300.0 && uncorrectedPhoEt <= 400.0)
    histoNumber = 4;
  else if(uncorrectedPhoEt > 400.0)
    histoNumber = 5;
  if(histoNumber > 0)
  {
    h_sieie_num_QCD[histoNumber][sysNumber]->Fill(phoSigmaIEtaIEtaFull5x5->at(index),event_weight);
    h_nVtx_num_QCD[histoNumber][sysNumber]->Fill(nVtx,event_weight);
    h_phoPt_num_QCD[histoNumber][sysNumber]->Fill(uncorrectedPhoEt,event_weight);
    h_pfMET_num_QCD[histoNumber][sysNumber]->Fill(pfMET,event_weight);
  }
  if(nVtx <= 11)
    histoNumber = 6;
  else if(nVtx > 11 && nVtx <= 15)
    histoNumber = 7;
  else if(nVtx > 15 && nVtx <= 20)
    histoNumber = 8;
  else if(nVtx > 20)
    histoNumber = 9;
  if(histoNumber > 5)
  {
    h_sieie_num_QCD[histoNumber][sysNumber]->Fill(phoSigmaIEtaIEtaFull5x5->at(index),event_weight);
    h_nVtx_num_QCD[histoNumber][sysNumber]->Fill(nVtx,event_weight);
    h_phoPt_num_QCD[histoNumber][sysNumber]->Fill(uncorrectedPhoEt,event_weight);
    h_pfMET_num_QCD[histoNumber][sysNumber]->Fill(pfMET,event_weight);
  }
}

std::vector<int> postAnalyzer::getPhoCandNum(double phoPtCut, double phoEtaCut){

  std::vector<int> tmpCand;
  tmpCand.clear();
    
  //Loop over photons
  for(int p=0;p<nPho;p++)
  {
    Float_t uncorrectedPhoEt = ((*phoSCRawE)[p]/TMath::CosH((*phoSCEta)[p]));
    bool kinematic = uncorrectedPhoEt > phoPtCut  && fabs((*phoSCEta)[p])<phoEtaCut;
    bool photonId = (
         ((*phoHoverE)[p]                <  0.0260   ) &&
         // ((*phoSigmaIEtaIEtaFull5x5)[p]  <  0.01040 ) &&
         ((*phohasPixelSeed)[p]              ==  0      ) &&
         ( TMath::Max( ( (*phoYuPFChWorstIso)[p]  - rho*EAchargedworst((*phoSCEta)[p]) ), 0.0) < 1.146 )  &&
         ( TMath::Max( ( (*phoPFNeuIso)[p] - rho*EAneutral((*phoSCEta)[p]) ), 0.0) < (2.792 + (0.0112 * uncorrectedPhoEt) + (0.000028 * pow(uncorrectedPhoEt, 2.0))) )  &&
         ( TMath::Max( ( (*phoPFPhoIso)[p] - rho*EAphoton((*phoSCEta)[p])  ), 0.0) < (2.176 + (0.0043 * uncorrectedPhoEt)) ) 
    );
      //No timing or mipTotEnergy cuts
    bool noncoll = (*phoSigmaIEtaIEtaFull5x5)[p] > 0.001 && (*phoSigmaIPhiIPhiFull5x5)[p] > 0.001;

    if(photonId && kinematic && noncoll)
    {
      tmpCand.push_back(p);
    }
  }

  return tmpCand;

}

std::vector<int> postAnalyzer::getPhoCandNum_QCDsideband(double phoPtCut, double phoEtaCut){

  std::vector<int> tmpCand;
  tmpCand.clear();
    
  //Loop over photons
  for(int p=0;p<nPho;p++)
  {
    Float_t uncorrectedPhoEt = ((*phoSCRawE)[p]/TMath::CosH((*phoSCEta)[p]));
    bool kinematic = uncorrectedPhoEt > phoPtCut  && fabs((*phoSCEta)[p])<phoEtaCut;
    bool photonId = (
         ((*phoHoverE)[p]                <  0.0260   ) &&
         // ((*phoSigmaIEtaIEtaFull5x5)[p]  <  0.01040 ) &&
         ((*phohasPixelSeed)[p]              ==  0      ) &&
         ( TMath::Max( ( (*phoYuPFChWorstIso)[p]  - rho*EAchargedworst((*phoSCEta)[p]) ), 0.0) > 8.0 )  &&
         ( TMath::Max( ( (*phoYuPFChWorstIso)[p]  - rho*EAchargedworst((*phoSCEta)[p]) ), 0.0) < 14.0 )  &&
         ( TMath::Max( ( (*phoPFNeuIso)[p] - rho*EAneutral((*phoSCEta)[p]) ), 0.0) < (2.792 + (0.0112 * uncorrectedPhoEt) + (0.000028 * pow(uncorrectedPhoEt, 2.0))) )  &&
         ( TMath::Max( ( (*phoPFPhoIso)[p] - rho*EAphoton((*phoSCEta)[p])  ), 0.0) < (2.176 + (0.0043 * uncorrectedPhoEt)) )
    );
      
    bool noncoll = fabs((*phoSeedTime)[p]) < 3. && (*phoMIPTotEnergy)[p] < 4.9 && (*phoSigmaIEtaIEtaFull5x5)[p] > 0.001 && (*phoSigmaIPhiIPhiFull5x5)[p] > 0.001;

    if(photonId && kinematic && noncoll)
    {
      tmpCand.push_back(p);
    }
  }

  return tmpCand;

}

std::vector<int> postAnalyzer::getPhoCandNum_QCDsbUp(double phoPtCut, double phoEtaCut){

  std::vector<int> tmpCand;
  tmpCand.clear();
    
  //Loop over photons
  for(int p=0;p<nPho;p++)
  {
    Float_t uncorrectedPhoEt = ((*phoSCRawE)[p]/TMath::CosH((*phoSCEta)[p]));
    bool kinematic = uncorrectedPhoEt > phoPtCut  && fabs((*phoSCEta)[p])<phoEtaCut;
    bool photonId = (
         ((*phoHoverE)[p]                <  0.0260   ) &&
         // ((*phoSigmaIEtaIEtaFull5x5)[p]  <  0.01040 ) &&
         ((*phohasPixelSeed)[p]              ==  0      ) &&
         ( TMath::Max( ( (*phoYuPFChWorstIso)[p]  - rho*EAchargedworst((*phoSCEta)[p]) ), 0.0) > 8.0 )  &&
         ( TMath::Max( ( (*phoYuPFChWorstIso)[p]  - rho*EAchargedworst((*phoSCEta)[p]) ), 0.0) < 16.0 )  &&
         ( TMath::Max( ( (*phoPFNeuIso)[p] - rho*EAneutral((*phoSCEta)[p]) ), 0.0) < (2.792 + (0.0112 * uncorrectedPhoEt) + (0.000028 * pow(uncorrectedPhoEt, 2.0))) )  &&
         ( TMath::Max( ( (*phoPFPhoIso)[p] - rho*EAphoton((*phoSCEta)[p])  ), 0.0) < (2.176 + (0.0043 * uncorrectedPhoEt)) )
    );
      
    bool noncoll = fabs((*phoSeedTime)[p]) < 3. && (*phoMIPTotEnergy)[p] < 4.9 && (*phoSigmaIEtaIEtaFull5x5)[p] > 0.001 && (*phoSigmaIPhiIPhiFull5x5)[p] > 0.001;

    if(photonId && kinematic && noncoll)
    {
      tmpCand.push_back(p);
    }
  }

  return tmpCand;

}

std::vector<int> postAnalyzer::getPhoCandNum_QCDsbDown(double phoPtCut, double phoEtaCut){

  std::vector<int> tmpCand;
  tmpCand.clear();
    
  //Loop over photons
  for(int p=0;p<nPho;p++)
  {
    Float_t uncorrectedPhoEt = ((*phoSCRawE)[p]/TMath::CosH((*phoSCEta)[p]));
    bool kinematic = uncorrectedPhoEt > phoPtCut  && fabs((*phoSCEta)[p])<phoEtaCut;
    bool photonId = (
         ((*phoHoverE)[p]                <  0.0260   ) &&
         // ((*phoSigmaIEtaIEtaFull5x5)[p]  <  0.01040 ) &&
         ((*phohasPixelSeed)[p]              ==  0      ) &&
         ( TMath::Max( ( (*phoYuPFChWorstIso)[p]  - rho*EAchargedworst((*phoSCEta)[p]) ), 0.0) > 8.0 )  &&
         ( TMath::Max( ( (*phoYuPFChWorstIso)[p]  - rho*EAchargedworst((*phoSCEta)[p]) ), 0.0) < 12.0 )  &&
         ( TMath::Max( ( (*phoPFNeuIso)[p] - rho*EAneutral((*phoSCEta)[p]) ), 0.0) < (2.792 + (0.0112 * uncorrectedPhoEt) + (0.000028 * pow(uncorrectedPhoEt, 2.0))) )  &&
         ( TMath::Max( ( (*phoPFPhoIso)[p] - rho*EAphoton((*phoSCEta)[p])  ), 0.0) < (2.176 + (0.0043 * uncorrectedPhoEt)) )
    );
      
    bool noncoll = fabs((*phoSeedTime)[p]) < 3. && (*phoMIPTotEnergy)[p] < 4.9 && (*phoSigmaIEtaIEtaFull5x5)[p] > 0.001 && (*phoSigmaIPhiIPhiFull5x5)[p] > 0.001;

    if(photonId && kinematic && noncoll)
    {
      tmpCand.push_back(p);
    }
  }

  return tmpCand;

}

// Effective area to be needed in PF Iso for photon ID
//https://indico.cern.ch/event/629088/contributions/2603837/attachments/1464422/2264124/80X_PhotonID_EfficiencyStudy_23-05-2017.pdf
// Double_t postAnalyzer::EAcharged(Double_t eta){
//   Float_t EffectiveArea = 0.0;
//   if(fabs(eta) >= 0.0   && fabs(eta) < 1.0   ) EffectiveArea = 0.0360;
//   if(fabs(eta) >= 1.0   && fabs(eta) < 1.479 ) EffectiveArea = 0.0377;
//   return EffectiveArea;
// }

Double_t postAnalyzer::EAchargedworst(Double_t eta){
  Float_t EffectiveArea = 0.0;
  if(fabs(eta) >= 0.0   && fabs(eta) < 1.0   ) EffectiveArea = 0.1064;
  if(fabs(eta) >= 1.0   && fabs(eta) < 1.479 ) EffectiveArea = 0.1026;
  return EffectiveArea;
}

Double_t postAnalyzer::EAneutral(Double_t eta){
  Float_t EffectiveArea = 0.0;
  if(fabs(eta) >= 0.0   && fabs(eta) < 1.0   ) EffectiveArea = 0.0597;
  if(fabs(eta) >= 1.0   && fabs(eta) < 1.479 ) EffectiveArea = 0.0807;
  return EffectiveArea;
}

Double_t postAnalyzer::EAphoton(Double_t eta){
  Float_t EffectiveArea = 0.0;
  if(fabs(eta) >= 0.0   && fabs(eta) < 1.0   ) EffectiveArea = 0.1210;
  if(fabs(eta) >= 1.0   && fabs(eta) < 1.479 ) EffectiveArea = 0.1107;
  return EffectiveArea;
}

double postAnalyzer::DeltaPhi(double phi1, double phi2)
{
  double pi = TMath::Pi();
  double dphi = fabs(phi1-phi2);
  if(dphi>pi)
    dphi = 2.0*pi - dphi;
  return dphi;
}

bool postAnalyzer::electron_veto_looseID(int pho_index, float elePtCut)
{
  bool veto_passed = true; //pass veto if no good electron found
  bool pass_SigmaIEtaIEtaFull5x5 = false;
  bool pass_dEtaIn = false;
  bool pass_dPhiIn = false;
  bool pass_HoverE = false;
  bool pass_iso = false;
  bool pass_ooEmooP = false;
  bool pass_d0 = false;
  bool pass_dz = false;
  bool pass_missingHits = false;
  bool pass_convVeto = false;
  //Explicitly stating types to avoid a TMath::Max conversion issue
  Float_t EA = 0.0;
  Float_t zero = 0.0;
  Float_t EAcorrIso = 999.9;
  for(int i = 0; i < nEle; i++)
    {
      //Make sure these get reset for every electron
      pass_SigmaIEtaIEtaFull5x5 = false;
      pass_dEtaIn = false;
      pass_dPhiIn = false;
      pass_HoverE = false;
      pass_iso = false;
      pass_ooEmooP = false;
      pass_d0 = false;
      pass_dz = false;
      pass_missingHits = false;
      pass_convVeto = false;
      //Find EA for corrected relative iso.
      if(abs(eleSCEta->at(i)) <= 1.0)
  EA = 0.1752;
      else if(1.0 < abs(eleSCEta->at(i)) && abs(eleSCEta->at(i)) <= 1.479)
  EA = 0.1862;
      else if(1.479 < abs(eleSCEta->at(i)) && abs(eleSCEta->at(i)) <= 2.0)
  EA = 0.1411;
      else if(2.0 < abs(eleSCEta->at(i)) && abs(eleSCEta->at(i)) <= 2.2)
  EA = 0.1534;
      else if(2.2 < abs(eleSCEta->at(i)) && abs(eleSCEta->at(i)) <= 2.3)
  EA = 0.1903;
      else if(2.3 < abs(eleSCEta->at(i)) && abs(eleSCEta->at(i)) <= 2.4)
  EA = 0.2243;
      else if(2.4 < abs(eleSCEta->at(i)) && abs(eleSCEta->at(i)) < 2.5)
  EA = 0.2687;
      EAcorrIso = (elePFChIso->at(i) + TMath::Max(zero,elePFNeuIso->at(i) + elePFPhoIso->at(i) - rho*EA))/(elePt->at(i));

      if(abs(eleSCEta->at(i)) <= 1.479)
  {
    pass_SigmaIEtaIEtaFull5x5 = eleSigmaIEtaIEtaFull5x5->at(i) < 0.0103;
    pass_dEtaIn = abs(eledEtaAtVtx->at(i)) < 0.0105;
    pass_dPhiIn = abs(eledPhiAtVtx->at(i)) < 0.115;
    pass_HoverE = eleHoverE->at(i) < 0.104;
    pass_iso = EAcorrIso < 0.0893;
    pass_ooEmooP = eleEoverPInv->at(i) < 0.102;
    pass_d0 = abs(eleD0->at(i)) < 0.0261;
    pass_dz = abs(eleDz->at(i)) < 0.41;
    pass_missingHits = eleMissHits->at(i) <= 2;
    pass_convVeto = eleConvVeto->at(i) == 1;
  }
      else if(1.479 < abs(eleSCEta->at(i)) < 2.5)
  {
    pass_SigmaIEtaIEtaFull5x5 = eleSigmaIEtaIEtaFull5x5->at(i) < 0.0301;
    pass_dEtaIn = abs(eledEtaAtVtx->at(i)) < 0.00814;
    pass_dPhiIn = abs(eledPhiAtVtx->at(i)) < 0.182;
    pass_HoverE = eleHoverE->at(i) < 0.0897;
    pass_iso = EAcorrIso < 0.121;
    pass_ooEmooP = eleEoverPInv->at(i) < 0.126;
    pass_d0 = abs(eleD0->at(i)) < 0.118;
    pass_dz = abs(eleDz->at(i)) < 0.822;
    pass_missingHits = eleMissHits->at(i) <= 1;
    pass_convVeto = eleConvVeto->at(i) == 1;
  }
      //Electron passes Loose Electron ID cuts
      if(pass_SigmaIEtaIEtaFull5x5 && pass_dEtaIn && pass_dPhiIn && pass_HoverE && pass_iso && pass_ooEmooP && pass_d0 && pass_dz && pass_missingHits && pass_convVeto)
  {
    //Electron passes pt cut
    if(elePt->at(i) > elePtCut)
      {
        //Electron does not overlap photon
        if(dR(eleSCEta->at(i),eleSCPhi->at(i),phoSCEta->at(pho_index),phoSCPhi->at(pho_index)) > 0.5)
    {
      veto_passed = false;
      break;
    }
      }
  }
    }
  return veto_passed;
}

bool postAnalyzer::muon_veto_looseID(int pho_index, float muPtCut)
{
  bool veto_passed = true; //pass veto if no good muon found
  bool pass_PFMuon = false;
  bool pass_globalMuon = false;
  bool pass_trackerMuon = false;
  bool pass_iso = false;
  //Explicitly stating types to avoid a TMath::Max conversion issue
  Float_t zero = 0.0;
  Float_t muPhoPU = 999.9;
  Float_t tightIso_combinedRelative = 999.9;
  for(int i = 0; i < nMu; i++)
    {
      pass_PFMuon = muIsPFMuon->at(i);
      pass_globalMuon = muIsGlobalMuon->at(i);
      pass_trackerMuon = muIsTrackerMuon->at(i);
      muPhoPU = muPFNeuIso->at(i) + muPFPhoIso->at(i) - 0.5*muPFPUIso->at(i);
      tightIso_combinedRelative = (muPFChIso->at(i) + TMath::Max(zero,muPhoPU))/(muPt->at(i));
      pass_iso = tightIso_combinedRelative < 0.25;
      //Muon passes Loose Muon ID and PF-based combined relative, dBeta-corrected Loose Muon Isolation cuts
      if(pass_PFMuon && (pass_globalMuon || pass_trackerMuon))
  {
    //Muon passes pt cut
    if(muPt->at(i) > muPtCut)
      {
        //Muon does not overlap photon
        if(dR(muEta->at(i),muPhi->at(i),phoSCEta->at(pho_index),phoSCPhi->at(pho_index)) > 0.5)
    {
      veto_passed = false;
      break;
    }
      }
  }
    }
  return veto_passed;
}

std::vector<int> postAnalyzer::JetVetoDecision(int pho_index) {

  bool jetVeto=true;
  std::vector<int> jetindex;
  float value =0.0;

  for(int i = 0; i < nJet; i++)
    {

      if(0.0 < abs(jetEta->at(i)) && abs(jetEta->at(i)) <2.5)
        value =-0.8;
      if(2.5 <= abs(jetEta->at(i)) && abs(jetEta->at(i)) <2.75)
        value =-0.95;
      if(2.75 <= abs(jetEta->at(i)) && abs(jetEta->at(i)) <3.0)
        value =-0.97;
      if(3.00 <= abs(jetEta->at(i)) && abs(jetEta->at(i)) <5.0)
        value =-0.99;

      double deltar = 0.0 ;
      if(pho_index>=0){
        deltar= dR(jetEta->at(i),jetPhi->at(i),phoSCEta->at(pho_index),phoSCPhi->at(pho_index));
      }
      if(deltar>0.4 && jetPt->at(i) >30.0 && jetPFLooseId->at(i)==1 && jetPUID->at(i)>value)
        {
          jetindex.push_back(i);
        }

      
    }
                                                                                                                                          
  return jetindex;

}

bool postAnalyzer::dPhiJetMET_veto(std::vector<int> jets)
{
  //Pass veto if no jet is found within DeltaPhi(jet,MET) < 0.5
  bool passes = false;
  int njetsMax = jets.size();
  //Only look at first four jets
  if(njetsMax > 4)
    njetsMax = 4;
  int j = 0;
  for(; j < njetsMax; j++)
    {
      //fail veto if a jet is found with DeltaPhi(jet,MET) < 0.5
      if(DeltaPhi(jetPhi->at(jets[j]),pfMETPhi) < 0.5)
  break;
    }
  if(j==njetsMax)
    passes = true;

  return passes;
}

double postAnalyzer::dR(double eta1, double phi1, double eta2, double phi2)
{
  double deltaeta = abs(eta1 - eta2);
  double deltaphi = DeltaPhi(phi1, phi2);
  double deltar = sqrt(deltaeta*deltaeta + deltaphi*deltaphi);
  return deltar;
}

// shift: 0 = Central value, 1 = High uncertainty edge, 2 = Low uncertainty edge
double postAnalyzer::sieie_correction(Float_t sieie, int shift)
{
  Double_t central[15] = {1.06087,1.00142,1.10568,1.0218,1.05634,1.14023,1.02781,1.03304,0.991074,0.972227,0.926501,0.865083,0.837679,0.865209,0.915784};
  Double_t up[15]      = {1.23995,1.12062,1.19775,1.08778,1.10846,1.18255,1.05289,1.05087,1.00817,0.991021,0.952444,0.907244,0.904722,0.96769,1.07827};
  Double_t down[15]    = {0.881795,0.882233,1.01362,0.955815,1.00422,1.09792,1.00274,1.0152,0.973974,0.953432,0.900558,0.822922,0.770635,0.762728,0.753297};
  
  double correction = -99999999;
  // Covering sieie from 0.00700 to 0.01075, in increments of 0.00025
  int binNumber = floor((sieie - 0.007) / 0.00025);
  if(binNumber < 0 || binNumber > 14)
    correction = 1.0;
  else if(shift == 0)
    correction = central[binNumber];
  else if(shift == 1)
    correction = up[binNumber];
  else if(shift == 2)
    correction = down[binNumber];
  
  return correction;
}
#define monoJetCommon_cxx
#include "monoJetCommon.h"

void monoJetCommon::BookCommon(int i, string histname) {
  
  float MtBins[51]={180.,200.,220.,240.,260.,280.,300.,320.,340.,360.,380.,400.,420.,440.,460.,480.,500.,520.,540.,560.,580.,600.,620.,640.,660.,680.,700.,720.,740.,760.,
		    780.,800.,820.,840.,860.,880.,900.,920.,940.,960.,980.,1000.,1050.,1100.,1200.,1300.,1400.,1500.,2000.,2500.,3000.};
  
  float MetBins[45]={200.,220.,240.,260.,280.,300.,320.,340.,360.,380.,400.,420.,440.,460.,480.,500.,520.,540.,560.,580.,600.,620.,640.,660.,680.,700.,720.,740.,760.,
		     780.,800.,820.,840.,860.,880.,900.,920.,940.,960.,980.,1000.,1400.,1800.,2000.,2500.};
  
  float PtBins[49]={200.,220.,240.,260.,280.,300.,320.,340.,360.,380.,400.,420.,440.,460.,480.,500.,520.,540.,560.,580.,600.,620.,640.,660.,680.,700.,720.,740.,760.,
		    780.,800.,820.,840.,860.,880.,900.,920.,940.,960.,980.,1000.,1050.,1100.,1200.,1300.,1400.,1500.,2000.,2500.};
  
  float Pt123Bins[59]={0.,20.,40.,60.,80.,100.,120.,140.,160.,180.,200.,220.,240.,260.,280.,300.,320.,340.,360.,380.,400.,420.,440.,460.,480.,500.,520.,540.,560.,580.,
		       600.,620.,640.,660.,680.,700.,720.,740.,760.,780.,800.,820.,840.,860.,880.,900.,920.,940.,960.,980.,1000.,1050.,1100.,1200.,1300.,1400.,1500.,2000.,2500.};
  
  float PtFracBins[51] = {0.0, 0.02, 0.04, 0.06, 0.08, 0.1, 0.12, 0.14, 0.16, 0.18, 0.2, 0.22, 0.24, 0.26, 0.28, 0.3, 0.32, 0.34, 0.36, 0.38, 0.4, 0.42, 0.44, 0.46, 0.48, 0.5,
			  0.52, 0.54, 0.56, 0.58, 0.6, 0.62, 0.64, 0.66, 0.68, 0.7, 0.72, 0.74, 0.76, 0.78, 0.8, 0.82, 0.84, 0.86, 0.88, 0.9, 0.92, 0.94, 0.96, 0.98, 1.0};
  
  float BosonPtBins[25] = {150,170,200,230,260,290,320,350,390,430,470,510,550,590,640,690,740,790,840,900,960,1020,1090,1160,1250};

  if (i == -1) {
    
    h_metcut  = new TH1F("h_metcut","h_metcut; |pfMET-caloMET|/pfMET", 50,0,1.2);h_metcut->Sumw2();
    h_dphimin = new TH1F("h_dphimin","h_dphimin; Minimum dPhiJetMET",50,0,3.2);h_dphimin->Sumw2();
    h_metFilters = new TH1F("h_metFilters","metFilters",8,0.5,8.5); h_metFilters->Sumw2();
    h_kfactor = new TH1F("h_kfactor","h_kfactor;kfactor",50,0,2); h_kfactor->Sumw2();
    h_pileup = new TH1F("h_pileup","h_pileup;Pileup Weight",50,0,2); h_pileup->Sumw2();
    h_genZPt = new TH1F("h_genZPt","genZPt;Gen Z Boson P_{T}",24,BosonPtBins); h_genZPt->Sumw2();
    h_genZPtwK = new TH1F("h_genZPtwK","genZPtwK;Gen Z Boson P_{T}",24,BosonPtBins); h_genZPtwK->Sumw2();
    h_genWPt = new TH1F("h_genWPt","genWPt;Gen W Boson P_{T}",24,BosonPtBins); h_genWPt->Sumw2();
    h_genWPtwK = new TH1F("h_genWPtwK","genWPtwK;Gen W Boson P_{T}",24,BosonPtBins); h_genWPtwK->Sumw2();
  
    // Uncertainty Plots
    h_EcalPtUnc=new TH2F("EcalPtUnc","ECAL P_{T} Uncertainty;Photon P_{T} (GeV);Uncertainty",50,0.,2500.,50,0.,1.);
    h_TrackerPtUnc=new TH2F("TrackerPtUnc","Tracker P_{T} Uncertainty;Charged Hadrons P_{T} (GeV);Uncertainty",50,0.,2500.,50,0.,1.);
    h_HcalPtUnc=new TH2F("HcalPtUnc","HCAL P_{T} Uncertainty;Neutral Hadron P_{T} (GeV);Uncertainty",50,0.,2500.,50,0.,1.);
  
    h_nlo_ewk=new TH1F(("nloEWK"+histname).c_str(),"nloEWK",100,-0.6,0.1); h_nlo_ewk->Sumw2();
  
  } else {

    h_eventWeight[i] = new TH1F(("eventWeight"+histname).c_str(),"eventWeight",50,0,2); h_eventWeight[i]->Sumw2();
    h_puTrueNoWeight[i] = new TH1F(("puTrueNoWeight"+histname).c_str(),"puTrue;true number of iteractions",100,0,100);h_puTrueNoWeight[i]->Sumw2();
    h_puTrueReWeight[i] = new TH1F(("puTrueReWeight"+histname).c_str(),"puTrue;true number of iteractions",100,0,100);h_puTrueReWeight[i]->Sumw2();
    h_genHT[i] = new TH1F(("genHT"+histname).c_str(),"genHT;genHT",100,0,2500);h_genHT[i]->Sumw2();
    h_nJets[i]   = new TH1F(("nJets"+histname).c_str(), "nJets;Number of Jets", 50, 0, 100);h_nJets[i]->Sumw2();
    h_pfMETall[i] =  new TH1F(("pfMETall"+histname).c_str(), "pfMET",50,0,2000);h_pfMETall[i] ->Sumw2(); 
    h_pfMET200[i] = new TH1F(("pfMET200"+histname).c_str(), "pfMET",50,170,1500);h_pfMET200[i] ->Sumw2(); 
    h_pfMET[i] = new TH1F(("pfMET"+histname).c_str(), "E_{T}^{miss} (GeV)",44,MetBins);h_pfMET[i] ->Sumw2();
    h_pfMETPhi[i] = new TH1F(("pfMETPhi"+histname).c_str(), "pfMETPhi",50,-4,4);h_pfMETPhi[i]->Sumw2();
    h_j1TotConsPt[i]  = new TH1F(("j1TotConsPt"+histname).c_str(), "j1pT;Leading Jet Total Constituent P_{T}", 48,PtBins);h_j1TotConsPt[i]->Sumw2();
    h_j1Pt[i]  = new TH1F(("j1pT"+histname).c_str(), "j1pT;p_{T} of Leading Jet (GeV)", 48,PtBins);h_j1Pt[i]->Sumw2();
    h_j1Eta[i] = new TH1F(("j1Eta"+histname).c_str(), "j1Eta; #eta of Leading Jet", 50, -3.0, 3.0);h_j1Eta[i]->Sumw2();
    h_j1Phi[i] = new TH1F(("j1Phi"+histname).c_str(), "j1Phi; #phi of Leading Jet", 50, -3.0, 3.0);h_j1Phi[i]->Sumw2();
    h_j1etaWidth[i] = new TH1F(("j1etaWidth"+histname).c_str(),"j1etaWidth; #eta width of Leading Jet", 50,0,0.25);h_j1etaWidth[i] ->Sumw2();
    h_j1phiWidth[i] = new TH1F(("j1phiWidth"+histname).c_str(),"j1phiWidth; #phi width of Leading Jet", 50, 0,0.5);h_j1phiWidth[i]->Sumw2();
    h_j1nCons[i] = new TH1F (("j1nCons"+histname).c_str(),"j1nCons; Number of Constituents of Leading Jet",25, 0, 50);h_j1nCons[i]->Sumw2();
    h_Pt123[i] = new TH1F(("Pt123"+histname).c_str(),"Pt123;P_{T} sum carried by 3 leading daughters of the Pencil Jet",58,Pt123Bins);h_Pt123[i]->Sumw2();
    h_PF123PtFraction[i]= new TH1F(("PF123PtFraction"+histname).c_str(), "PF123PtFraction;P_{T} fraction carried by 3 leading daughters of the Pencil Jet" ,50,0,1);h_PF123PtFraction[i]->Sumw2(); 
    h_PtRawFrac[i]= new TH1F(("PtRawFrac"+histname).c_str(), "PtRawFrac;Raw P_{T}^{123} Fraction" ,50,0,1.1);h_PtRawFrac[i]->Sumw2(); 
    h_j1TotPFCands[i] = new TH1F(("j1TotPFCands"+histname).c_str(),"j1TotPFCands;# of all PF candidates in Leading Jet",25,0,50);h_j1TotPFCands[i]->Sumw2();
    h_j1ChPFCands[i] = new TH1F(("j1ChPFCands"+histname).c_str(),"j1ChPFCands;# of PF charged hadrons in Leading Jet",25,0,50);h_j1ChPFCands[i]->Sumw2();
    h_j1NeutPFCands[i] = new TH1F(("j1NeutPFCands"+histname).c_str(),"j1NeutPFCands;# of PF neutral hadrons in Leading Jet",15,0,15);h_j1NeutPFCands[i]->Sumw2();
    h_j1GammaPFCands[i] = new TH1F(("j1GammaPFCands"+histname).c_str(),"j1GammaPFCands;# of PF gammas in Leading Jet",20,0,40);h_j1GammaPFCands[i]->Sumw2();
    h_j1CHF[i] = new TH1F(("j1CHF"+histname).c_str(),"j1CHF;Charged Hadron Energy Fraction in Leading Jet",50,0,1.1);h_j1CHF[i]->Sumw2(); 
    h_j1NHF[i] = new TH1F(("j1NHF"+histname).c_str(),"j1NHF;Neutral Hadron Energy Fraction in Leading Jet",50,0,1.1);h_j1NHF[i]->Sumw2(); 
    h_j1ChMultiplicity[i] = new TH1F(("j1ChMultiplicity"+histname).c_str(),"j1ChMultiplicity;Charged Multiplicity of Leading Jet",25,0,50);h_j1ChMultiplicity[i]->Sumw2();
    h_j1NeutMultiplicity[i] = new TH1F(("j1NeutMultiplicity"+histname).c_str(),"j1NeutMultiplicity;Neutral Multiplicity of Leading Jet",25,0,50);h_j1NeutMultiplicity[i]->Sumw2(); 
    h_j1Mt[i]  = new TH1F(("j1Mt"+histname).c_str(), "j1Mt;M_{T} of Leading Jet (GeV)", 50,MtBins);h_j1Mt[i]->Sumw2(); 
    h_nVtx[i] = new TH1F(("nVtx"+histname).c_str(),"nVtx;nVtx",70,0,70);h_nVtx[i]->Sumw2(); 
    h_nVtx2[i] = new TH1F(("nVtx2"+histname).c_str(),"nVtx;nVtx",40,0,80);h_nVtx2[i]->Sumw2();
    h_j1Mass[i] = new TH1F(("j1Mass"+histname).c_str(),"j1Mass;Leading Jet Mass (GeV)",50,0,200);h_j1Mass[i]->Sumw2();
    h_j1JEC[i] = new TH1F(("j1JEC"+histname).c_str(),"j1JEC;Leading Jet JEC Uncertainty",50,0,0.1); h_j1JEC[i]->Sumw2();
    h_ChNemPtFrac[i] = new TH1F(("ChNemPtFrac"+histname).c_str(),"ChNemPtFrac;Ch + NEM P_{T}^{123} Fraction",50,0,1.1);h_ChNemPtFrac[i]->Sumw2();
    h_ChNemPt[i]  = new TH1F(("ChNemPt"+histname).c_str(), "ChNemPt;Ch + NEM Leading Jet P_{T} (GeV)", 58,Pt123Bins);h_ChNemPt[i]->Sumw2();
    h_ChNemPt123[i] = new TH1F(("ChNemPt123"+histname).c_str(),"ChNemPt123;Ch + NEM P^{123}_{T} (GeV)",58,Pt123Bins);h_ChNemPt123[i]->Sumw2();
    h_ChPercCons[i] = new TH1F(("ChPercCons"+histname).c_str(),"ChPercCons;Charged Hadron Constituent Percent",50,0,1.1);h_ChPercCons[i]->Sumw2();
    h_NhPercCons[i] = new TH1F(("NhPercCons"+histname).c_str(),"NhPercCons;Neutral Hadron Constituent Percent",50,0,1.1);h_NhPercCons[i]->Sumw2();
    h_GammaPercCons[i] = new TH1F(("GammaPercCons"+histname).c_str(),"GammaPercCons;Photon Constituent Percent",50,0,1.1);h_GammaPercCons[i]->Sumw2();

    h_ChargedPt[i] = new TH1F(("ChargedPt"+histname).c_str(),"ChargedPt;Charged Constituent P_{T}",58,Pt123Bins);h_ChargedPt[i]->Sumw2();
    h_NeutralPt[i] = new TH1F(("NeutralPt"+histname).c_str(),"NeutralPt;Neutral Constituent P_{T}",58,Pt123Bins);h_NeutralPt[i]->Sumw2();
    h_PhotonPt[i] = new TH1F(("PhotonPt"+histname).c_str(),"PhotonPt;Photon Constituent P_{T}",58,Pt123Bins);h_PhotonPt[i]->Sumw2();

    h_ChPercPt[i] = new TH1F(("ChPercPt"+histname).c_str(), "ChPercPt; Charged Constituent P_{T} Percentage" ,50,0,1.1);h_ChPercPt[i]->Sumw2();
    h_NhPercPt[i] = new TH1F(("NhPercPt"+histname).c_str(), "NhPercPt; Nharged Constituent P_{T} Percentage" ,50,0,1.1);h_NhPercPt[i]->Sumw2();
    h_GammaPercPt[i] = new TH1F(("GammaPercPt"+histname).c_str(), "GammaPercPt; Gammaarged Constituent P_{T} Percentage" ,50,0,1.1);h_GammaPercPt[i]->Sumw2();
    
    h_bChNemPtFrac[i] = new TH1F(("bChNemPtFrac"+histname).c_str(),"bChNemPtFrac;Barrel Ch + NEM P_{T}^{123} Fraction",50,0,1.1);h_bChNemPtFrac[i]->Sumw2();
    h_bPF123PtFraction[i]= new TH1F(("bPF123PtFraction"+histname).c_str(), "bPF123PtFraction;Barrel P_{T}^{123} Fraction" ,50,0,1.1);h_bPF123PtFraction[i]->Sumw2();
    
    h_eChNemPtFrac[i] = new TH1F(("eChNemPtFrac"+histname).c_str(),"eChNemPtFrac;Endcap Ch + NEM P_{T}^{123} Fraction",50,0,1.1);h_eChNemPtFrac[i]->Sumw2();
    h_ePF123PtFraction[i]= new TH1F(("ePF123PtFraction"+histname).c_str(), "ePF123PtFraction;Endcap P_{T}^{123} Fraction" ,50,0,1.1);h_ePF123PtFraction[i]->Sumw2();
    
    h_j1EtaPhi[i] = new TH2F(("j1EtaPhi"+histname).c_str(),"j1EtaPhi;Leading Jet #eta;Leading Jet #phi",50,-3.0,3.0,50,-3.1416,3.1416);
  }
}

void monoJetCommon::fillCommon(int histoNumber,double event_weight) {
  if (sample.isData) event_weight = 1;
  else {
    h_genHT[histoNumber]->Fill(genHT,event_weight);
    h_puTrueNoWeight[histoNumber]->Fill(puTrue->at(0),noweight);
    h_puTrueReWeight[histoNumber]->Fill(puTrue->at(0),event_weight);
  }

  h_eventWeight[histoNumber]->Fill(event_weight,event_weight);
  h_nVtx[histoNumber]->Fill(nVtx,event_weight);
  h_nVtx2[histoNumber]->Fill(nVtx,event_weight);
  h_nJets[histoNumber]->Fill(nJet,event_weight);
  h_pfMETall[histoNumber]->Fill(pfMET,event_weight);
  h_pfMET200[histoNumber]->Fill(pfMET,event_weight);
  h_pfMET[histoNumber]->Fill(pfMET,event_weight);
  h_pfMETPhi[histoNumber]->Fill(pfMETPhi,event_weight);
  if(jetCand.size()>0){
    h_j1TotConsPt[histoNumber]->Fill(j1TotConsPt,event_weight);
    h_j1Pt[histoNumber]->Fill(jetPt->at(jetCand[0]),event_weight);
    h_j1Eta[histoNumber]->Fill(jetEta->at(jetCand[0]),event_weight);
    h_j1Phi[histoNumber]->Fill(jetPhi->at(jetCand[0]),event_weight);
    h_j1EtaPhi[histoNumber]->Fill(jetEta->at(jetCand[0]),jetPhi->at(jetCand[0]),event_weight);
    h_Pt123[histoNumber]->Fill(Pt123,event_weight);
    h_PF123PtFraction[histoNumber]->Fill(Pt123Fraction,event_weight);
    h_PtRawFrac[histoNumber]->Fill(PtRawFrac,event_weight);
    h_j1TotPFCands[histoNumber]->Fill(TotalPFCandidates,event_weight);
    h_j1ChPFCands[histoNumber]->Fill(ChargedPFCandidates,event_weight);
    h_j1NeutPFCands[histoNumber]->Fill(NeutralPFCandidates,event_weight);
    h_j1GammaPFCands[histoNumber]->Fill(GammaPFCandidates,event_weight);
    h_j1CHF[histoNumber]->Fill(jetCHF->at(jetCand[0]),event_weight);
    h_j1NHF[histoNumber]->Fill(jetNHF->at(jetCand[0]),event_weight);
    h_j1ChMultiplicity[histoNumber]->Fill(jetNChargedHad->at(jetCand[0]),event_weight);
    h_j1NeutMultiplicity[histoNumber]->Fill(jetNNeutralHad->at(jetCand[0]),event_weight);
    h_j1Mt[histoNumber]->Fill(jetMt->at(jetCand[0]),event_weight);
    h_j1etaWidth[histoNumber]->Fill(jetetaWidth->at(jetCand[0]),event_weight);
    h_j1phiWidth[histoNumber]->Fill(jetphiWidth->at(jetCand[0]),event_weight);
    h_j1nCons[histoNumber]->Fill(jetNPhoton->at(jetCand[0])+jetNCharged->at(jetCand[0])+jetNNeutral->at(jetCand[0]),event_weight);
    h_j1Mass[histoNumber]->Fill(jetMass->at(jetCand[0]),event_weight);
    h_j1JEC[histoNumber]->Fill(jetJECUnc->at(jetCand[0]),event_weight);
    h_ChNemPtFrac[histoNumber]->Fill(ChNemPtFrac,event_weight);
    h_ChNemPt[histoNumber]->Fill(ChNemPt,event_weight);
    h_ChNemPt123[histoNumber]->Fill(ChNemPt123,event_weight);
    h_ChPercCons[histoNumber]->Fill(ChargedPFCandidates/(float)TotalPFCandidates,event_weight);
    h_NhPercCons[histoNumber]->Fill(NeutralPFCandidates/(float)TotalPFCandidates,event_weight);
    h_GammaPercCons[histoNumber]->Fill(GammaPFCandidates/(float)TotalPFCandidates,event_weight);

    h_NeutralPt[histoNumber]->Fill(hadronPt[0],event_weight);
    h_ChargedPt[histoNumber]->Fill(hadronPt[1],event_weight);
    h_PhotonPt[histoNumber]->Fill(hadronPt[2],event_weight);
      
    h_NhPercPt[histoNumber]->Fill( hadronPt[0]/jetPt->at(jetCand[0]) ,event_weight);
    h_ChPercPt[histoNumber]->Fill( hadronPt[1]/jetPt->at(jetCand[0]) ,event_weight);
    h_GammaPercPt[histoNumber]->Fill( hadronPt[2]/jetPt->at(jetCand[0]) ,event_weight);
    if (fabs(jetEta->at(jetCand[0])) <= 1.479) {
      h_bChNemPtFrac[histoNumber]->Fill(ChNemPtFrac,event_weight);
      h_bPF123PtFraction[histoNumber]->Fill(Pt123Fraction,event_weight);
    } else if (fabs(jetEta->at(jetCand[0])) < 2.5) {
      h_eChNemPtFrac[histoNumber]->Fill(ChNemPtFrac,event_weight);
      h_ePF123PtFraction[histoNumber]->Fill(Pt123Fraction,event_weight);
    }
    
  }
}

void monoJetCommon::getPt123Frac() {
  Pt123Fraction=Pt123=ChNemPtFrac=ChNemPt=ChNemPt123=j1TotConsPt=0.0;
  for (int i = 0; i < 3; i++){
    hadronPt[i] = 0.;
  }
  // Neutral, Charged, Photon
  int HadronID[3] = {130,211,22};
  double HadronPtFirst3[3] = {0,0,0};
  
  for (int i = 0; i < j1PFConsPID.size(); i++) {
    if (i < 3)
      Pt123 += j1PFConsPt.at(i);
    j1TotConsPt += j1PFConsPt.at(i);
    for (int j = 0; j < 3; j++)
      if (abs(j1PFConsPID.at(i)) == HadronID[j]) {
	if (i < 3)
	  HadronPtFirst3[j] += j1PFConsPt.at(i);
	hadronPt[j] += j1PFConsPt.at(i);
      }
  }
  l_jetPt = jetPt->at(jetCand[0]);
  Pt123Fraction = Pt123/jetPt->at(jetCand[0]);
  PtRawFrac = Pt123/jetRawPt->at(jetCand[0]);
  ChNemPtFrac = (HadronPtFirst3[1]+HadronPtFirst3[2])/(hadronPt[1]+hadronPt[2]);
  ChNemPt123 = HadronPtFirst3[1]+HadronPtFirst3[2];
  ChNemPt = hadronPt[1]+hadronPt[2];
}

void monoJetCommon::AllPFCand(vector<int> jetCand) {
  //getPFCandidatesMethod for the Pencil Jet -> jetCand[0]
  TotalPFCandidates=ChargedPFCandidates=NeutralPFCandidates=GammaPFCandidates=0;

  Pt123Fraction=Pt123=ChNemPtFrac=ChNemPt=ChNemPt123=j1TotConsPt=0.0;
  for (int i = 0; i < 3; i++){
    hadronPt[i] = 0.;
  }
  //We are using these conditions so we only calculate the following quantities for the signal we are interested in
  //This will also make it faster to process the events
  if(jetCand.size()>0) {
    j1PFConsEt=jetConstEt->at(jetCand[0]);
    j1PFConsPt=jetConstPt->at(jetCand[0]);
    j1PFConsEta=jetConstEta->at(jetCand[0]);
    j1PFConsPhi=jetConstPhi->at(jetCand[0]);
    j1PFConsPID=jetConstPdgId->at(jetCand[0]);
    vector<int> PFCandidates = getPFCandidates();
    
    //cout<<"Vector of Pairs should have size 4: "<<PFCandidates.size()<<endl;
    if(PFCandidates.size()>0) {
      TotalPFCandidates=PFCandidates.at(0);
      // cout<<"TotalPFCandidates: "<<TotalPFCandidates<<endl;
    }
    
    if(PFCandidates.size()>1)
      ChargedPFCandidates=PFCandidates.at(1);
    //cout<<"TotalChargedPFCandidates: "<<ChargedPFCandidates<<endl;}
    
    if(PFCandidates.size()>2)
      GammaPFCandidates=PFCandidates.at(2);
    //cout<<"TotalGammaPFCandidates: "<<GammaPFCandidates<<endl;}
    
    if(PFCandidates.size()>3)
      NeutralPFCandidates=PFCandidates.at(3);
    //cout<<"TotalNeutralPFCandidates: "<<NeutralPFCandidates<<endl;}
    
    getPt123Frac();
  }
}


//Function to calculate regular deltaR separate from jet width variable 'dR'
double monoJetCommon::deltaR(double eta1, double phi1, double eta2, double phi2) {
  double deltaeta = abs(eta1 - eta2);
  double deltaphi = DeltaPhi(phi1, phi2);
  double deltar = sqrt(deltaeta*deltaeta + deltaphi*deltaphi);
  return deltar;
}

//Gives the (minimum) separation in phi between the specified phi values
//Must return a positive value
float monoJetCommon::DeltaPhi(float phi1, float phi2) {
  float pi = TMath::Pi();
  float dphi = fabs(phi1-phi2);
  if(dphi>pi)
    dphi = 2.0*pi - dphi;
  return dphi;
}

float monoJetCommon::dPhiJetMETmin(vector<int> jets) {
  float dPhimin=TMath::Pi();
  int njetsMax = jets.size();
  if(njetsMax > 4)
    njetsMax = 4;
  for(int j=0;j< njetsMax; j++)
    {
      float dPhi = DeltaPhi((*jetPhi)[j],pfMETPhi);
      //cout<<"DeltaPhi: "<<dPhi<<endl;
      if(dPhi < dPhimin){
        dPhimin = dPhi;
      }
    }
  return dPhimin;
}

vector<int> monoJetCommon::getJetCand(double jetPtCut, double jetEtaCut, double jetNHFCut, double jetCHFCut) {
  vector<int> tmpCand;
  tmpCand.clear();
  for(int p=0;p<nJet;p++){
    bool kinematic = (*jetPt)[p] > jetPtCut && (*jetNHF)[p] < jetNHFCut && (*jetCHF)[p] > jetCHFCut && fabs((*jetEta)[p])<jetEtaCut;
    bool tightJetID = false;
    if ((*jetID)[p]>>0&1 == 1) tightJetID = true;
    if(kinematic && tightJetID)
      tmpCand.push_back(p);
  }
  return tmpCand;
}


//Return a vector of pairs. "0" = #pfCands, "1"=#chargedPFCands , "3"=#neutralPFCands,"2"=#photonPFCands
//get PF Candidates of the selected Jet ->jetCand[0]
vector<int>monoJetCommon::getPFCandidates() {
  vector<int>PFCands;
  for(int i = 0;i < nJet; i++) {
    int TotPFCands;
    if(i == 0) {
      TotPFCands = j1PFConsPID.size();
      PFCands.push_back(TotPFCands);
      int ChPFCands,NeuPFCands,GammaPFCands;
      ChPFCands=NeuPFCands=GammaPFCands=0;
      for(int j=0;j<TotPFCands;j++) {
        if(abs(j1PFConsPID.at(j))==211)
          ChPFCands++;
        if(j1PFConsPID.at(j)==22)
          GammaPFCands++;
        if(j1PFConsPID.at(j)==130)
          NeuPFCands++;
      }
      PFCands.push_back(ChPFCands);
      PFCands.push_back(GammaPFCands);
      PFCands.push_back(NeuPFCands);
    }
  }
  return PFCands;
}
bool monoJetCommon::btagVeto() {
  bool btagVeto = true;
  for(int i = 0; i < nJet; i++)
    if(jetPt->at(i) >30.0 && fabs(jetEta->at(i)) < 2.5 && jetCSV2BJetTags->at(i) > 0.8838)
      btagVeto = false;
  return btagVeto;
}

bool monoJetCommon::dPhiJetMETcut(vector<int> jets) {
  //reject jet if it is found within DeltaPhi(jet,MET) < 0.5 
  bool passes = false;
  int njetsMax = jets.size();
  //Only look at first four jets (because that's what monojet analysis do)
  if(njetsMax > 4)
    njetsMax = 4;
  int j = 0;
  for(; j < njetsMax; j++)
    if(DeltaPhi((*jetPhi)[j],pfMETPhi) < 0.5)
      break;
  if(j==njetsMax)
    passes = true;
  return passes;
}

double monoJetCommon::getKfactor(double bosonPt) {
  double EWK_corrected_weight = histomap.getBin("ewkCorrection",bosonPt);
  double NNLO_weight = histomap.getBin("NNLOCorrection",bosonPt);
  double kfactor = 1;
  if(EWK_corrected_weight!=0 && NNLO_weight!=0)
    kfactor = (EWK_corrected_weight/NNLO_weight);
  else
    kfactor= sample.type == WJets ? 1.21 : 1.23;
  h_kfactor->Fill(kfactor);
  return kfactor;
}

bool monoJetCommon::inclusiveCut() {
  if (sample.isInclusive)
    return genHT < 100;
  return true;
}

bool monoJetCommon::getJetHEMVeto(double jetPtCut){

  bool pass = true;
  for(int p=0;p<nJet;p++)
    {
      bool kinematic = (*jetPt)[p] > jetPtCut && (*jetEta)[p] < -1.4 && (*jetEta)[p] > -3.0 && (*jetPhi)[p] > -1.57 && (*jetPhi)[p] < -0.87 ;
      bool tightJetID = false;
      if ((*jetID)[p]>>0&1 == 1) tightJetID = true;
      if(kinematic) // not chekcing ID here.                                                                                                                                         
        pass = false;
    }

  return pass;
}

bool monoJetCommon::getEleHEMVeto(double elePtCut){

  bool pass = true;
  for(int p=0;p<nEle;p++)
    {
      bool kinematic = (*elePt)[p] > elePtCut && (*eleEta)[p] < -1.4 && (*eleEta)[p] > -3.0 && (*elePhi)[p] > -1.57 && (*elePhi)[p] < -0.87 ;
      if(kinematic) // not chekcing ID here.                                                                                                                                         
        pass = false;
    }

  return pass;
}

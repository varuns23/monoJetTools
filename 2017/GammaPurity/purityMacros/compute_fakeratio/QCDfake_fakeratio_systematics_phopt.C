#include <fstream>
#include <vector>
#include <iomanip>
#include <algorithm>
#include "TFile.h"
#include "TH1D.h"
#include "TF1.h"
#include "TGraph.h"
#include "TGraphErrors.h"
#include "TMultiGraph.h"
#include "TStyle.h"
#include "TCanvas.h"
#include "TMath.h"
#include "TString.h"
#include "THStack.h"
#include "TText.h"
#include "TPaveText.h"
#include "TLegend.h"
#include "TLatex.h"

//RooFit headers
#include "RooHistPdf.h"
#include "RooRealVar.h"
#include "RooPlot.h"
#include "RooDataHist.h"
#include "RooAddPdf.h"
#include "RooExtendPdf.h"
#include "RooNDKeysPdf.h"
#include "RooGlobalFunc.h"
//boost headers
#include <boost/lexical_cast.hpp>
#include <boost/format.hpp>

using namespace std;
using namespace ROOT;
using namespace RooFit;

// i = event_bin, j = sys_bin, full_lumi in fb-1
// event_bin_names[10] = {"All","175to200","200to250","250to300","300to400","400to1000","0to11","11to15","15to20","20toInf"}
// void plot_sideband_purity(int i, int j, float full_lumi){
void plot_sideband_purity(int i, float full_lumi){
  string event_bin = boost::lexical_cast<string>(i);
  // string sys_bin = boost::lexical_cast<string>(j);
  
  Float_t int_lumi = full_lumi;
  Float_t scale_factor = 0.984;
  
  TFile *GJets_40to100 = new TFile("GJets_QCDfake_phoNum_40To100.root");
  TFile *GJets_100to200 = new TFile("GJets_QCDfake_phoNum_100To200.root");
  TFile *GJets_200to400 = new TFile("GJets_QCDfake_phoNum_200To400.root");
  TFile *GJets_400to600 = new TFile("GJets_QCDfake_phoNum_400To600.root");
  TFile *GJets_600toInf = new TFile("GJets_QCDfake_phoNum_600ToInf.root");
  TFile *f_data = new TFile("data_QCDfake_all.root");

  // TString histoname_num_QCD = TString("h_sieie_num_QCD_"+event_bin+"_"+sys_bin);
  TString histoname_num_QCD = TString("h_sieie_num_QCD_"+event_bin+"_0");
  TH1D *sieie_num_QCDsideband_GJets_40to100 = (TH1D*)GJets_40to100->Get(histoname_num_QCD);
  TH1D *sieie_num_QCDsideband_GJets_100to200 = (TH1D*)GJets_100to200->Get(histoname_num_QCD);
  TH1D *sieie_num_QCDsideband_GJets_200to400 = (TH1D*)GJets_200to400->Get(histoname_num_QCD);
  TH1D *sieie_num_QCDsideband_GJets_400to600 = (TH1D*)GJets_400to600->Get(histoname_num_QCD);
  TH1D *sieie_num_QCDsideband_GJets_600toInf = (TH1D*)GJets_600toInf->Get(histoname_num_QCD);
  TH1D *sieie_num_QCDsideband_data = (TH1D*)f_data->Get(histoname_num_QCD);
  
  sieie_num_QCDsideband_GJets_40to100->Scale(20730.0*full_lumi*1000.0/4269126.0);
  sieie_num_QCDsideband_GJets_100to200->Scale(9226.0*full_lumi*1000.0/5131808.0);
  sieie_num_QCDsideband_GJets_200to400->Scale(2300.0*full_lumi*1000.0/10036339.0);
  sieie_num_QCDsideband_GJets_400to600->Scale(277.4*full_lumi*1000.0/2435892.0);
  sieie_num_QCDsideband_GJets_600toInf->Scale(93.38*full_lumi*1000.0/2117687.0);
  sieie_num_QCDsideband_GJets_40to100->Add(sieie_num_QCDsideband_GJets_100to200);
  sieie_num_QCDsideband_GJets_40to100->Add(sieie_num_QCDsideband_GJets_200to400);
  sieie_num_QCDsideband_GJets_40to100->Add(sieie_num_QCDsideband_GJets_400to600);
  sieie_num_QCDsideband_GJets_40to100->Add(sieie_num_QCDsideband_GJets_600toInf);
  
  TCanvas *c = new TCanvas("c", "canvas",700,640);
  gStyle->SetOptStat(0);
  gStyle->SetLegendBorderSize(0);
  c->SetLeftMargin(0.15);
  c->cd();
  
  sieie_num_QCDsideband_GJets_40to100->GetXaxis()->SetTitle("#sigma_{i#etai#eta}");
  sieie_num_QCDsideband_GJets_40to100->GetYaxis()->SetTitle("Yield for 35.9 fb^{-1}");
  sieie_num_QCDsideband_GJets_40to100->GetYaxis()->SetTitleOffset(1.8);
  sieie_num_QCDsideband_GJets_40to100->SetTitle("");
  sieie_num_QCDsideband_GJets_40to100->SetStats(0);
  sieie_num_QCDsideband_GJets_40to100->SetLineWidth(2);
  sieie_num_QCDsideband_GJets_40to100->SetLineColor(kRed);
  sieie_num_QCDsideband_data->GetXaxis()->SetTitle("#sigma_{i#etai#eta}");
  sieie_num_QCDsideband_data->GetYaxis()->SetTitle("Yield for 35.9 fb^{-1}");
  sieie_num_QCDsideband_data->GetYaxis()->SetTitleOffset(1.8);
  sieie_num_QCDsideband_data->SetTitle("");
  sieie_num_QCDsideband_data->SetStats(0);
  sieie_num_QCDsideband_data->SetLineWidth(2);
  sieie_num_QCDsideband_data->SetLineColor(kBlack);
  sieie_num_QCDsideband_data->SetMarkerStyle(8);
  
  sieie_num_QCDsideband_data->GetXaxis()->SetLabelFont(42);
  sieie_num_QCDsideband_data->GetXaxis()->SetLabelSize(0.040);
  sieie_num_QCDsideband_data->GetXaxis()->SetTitleFont(42);
  sieie_num_QCDsideband_data->GetXaxis()->SetTitleSize(0.045);
  sieie_num_QCDsideband_data->GetYaxis()->SetLabelFont(42);
  sieie_num_QCDsideband_data->GetYaxis()->SetLabelSize(0.040);
  sieie_num_QCDsideband_data->GetYaxis()->SetTitleFont(42);
  sieie_num_QCDsideband_data->GetYaxis()->SetTitleSize(0.045);
  
  TH1F* h_sieie_remainder = (TH1F*)sieie_num_QCDsideband_data->Clone("h_sieie_remainder");
  h_sieie_remainder->Add(sieie_num_QCDsideband_GJets_40to100,-1.0); // Subtract sieie_num_QCDsideband_GJets_40to100 from sieie_num_QCDsideband_data
  h_sieie_remainder->SetLineColor(kGreen);
  
  for(int i = 1; i <= h_sieie_remainder->GetNbinsX(); i++){
    if(h_sieie_remainder->GetBinContent(i) < 9.0e-7)
      h_sieie_remainder->SetBinContent(i,9.0e-7);
    if(sieie_num_QCDsideband_GJets_40to100->GetBinContent(i) < 1.0e-7)
      sieie_num_QCDsideband_GJets_40to100->SetBinContent(i,1.0e-7);
    if(sieie_num_QCDsideband_data->GetBinContent(i) < 1.0e-6)
      sieie_num_QCDsideband_data->SetBinContent(i,1.0e-6);
  }
  
  string lowPt = "undefined"+event_bin;
  string highPt = "undefined"+event_bin;
  if (i == 1){
    lowPt = "175";
    highPt = "200";
  }
  else if (i == 2){
    lowPt = "200";
    highPt = "250";
  }
  else if (i == 3){
    lowPt = "250";
    highPt = "300";
  }
  else if (i == 4){
    lowPt = "300";
    highPt = "400";
  }
  else if (i == 5){
    lowPt = "400";
    highPt = "1000";
  }
  
  double upper_limit = 0.0;
  if (highPt == "200") upper_limit = 7000.0; // good
  else if (highPt == "250") upper_limit = 5000.0; // good
  else if (highPt == "300") upper_limit = 1500.0; // good
  else if (highPt == "400") upper_limit = 800.0;
  else if (highPt == "1000") upper_limit = 250.0;
  sieie_num_QCDsideband_GJets_40to100->GetYaxis()->SetRangeUser(0.0,upper_limit);
  sieie_num_QCDsideband_GJets_40to100->Draw("HIST");
  h_sieie_remainder->Draw("HIST SAME");
  sieie_num_QCDsideband_data->Draw("SAME 0 E1");
  
  TString legTitle = TString(lowPt+" < Photon #it{p}_{T} < "+highPt);
  TLegend *leg = new TLegend(0.508711,0.672104,0.892665,0.872757,legTitle);
  leg->AddEntry(sieie_num_QCDsideband_data,"Data","P E1");
  leg->AddEntry(sieie_num_QCDsideband_GJets_40to100,"Gen-matched #gamma+jets MC","L");
  leg->AddEntry(h_sieie_remainder,"Fake template","L");
  leg->SetFillStyle(0);
  leg->Draw();
  
  TLatex *texS = new TLatex(0.61023,0.907173,"35.9 fb^{-1} (13 TeV)");
  texS->SetNDC();
  texS->SetTextFont(42);
  texS->SetTextSize(0.045);
  texS->Draw();
  TLatex *texS1 = new TLatex(0.18092,0.837173,"#bf{CMS} #it{Preliminary}");
  texS1->SetNDC();
  texS1->SetTextFont(42);
  texS1->SetTextSize(0.045);
  texS1->Draw();
  
  TString saveas_name = TString("sideband_templates_"+lowPt+"to"+highPt+".png");
  c->SaveAs(saveas_name);
  saveas_name = TString("sideband_templates_"+lowPt+"to"+highPt+".pdf");
  c->SaveAs(saveas_name);
}

// i = event_bin, j = sys_bin, full_lumi in fb-1 (NOT pb-1)
// event_bin_names[10] = {"All","175to200","200to250","250to300","300to400","400to1000","0to11","11to15","15to20","20toInf"}
//Systematics bins:
//    0       1      2       3       4       5       6         7          8
// Standard  sbUP  sbDown  metUP  metDown  binUP  binDown  sieieLeft  sieieRight
std::vector<Float_t> run_template_fit(int i, int j, float full_lumi)
{
  string event_bin = boost::lexical_cast<string>(i);
  string sys_bin = boost::lexical_cast<string>(j);

  TFile *GJets_40to100 = new TFile("GJets_QCDfake_phoNum_40To100.root");
  TFile *GJets_100to200 = new TFile("GJets_QCDfake_phoNum_100To200.root");
  TFile *GJets_200to400 = new TFile("GJets_QCDfake_phoNum_200To400.root");
  TFile *GJets_400to600 = new TFile("GJets_QCDfake_phoNum_400To600.root");
  TFile *GJets_600toInf = new TFile("GJets_QCDfake_phoNum_600ToInf.root");
  TFile *f_data = new TFile("data_QCDfake_all.root");

  TString histoname_num = TString("h_sieie_num_"+event_bin+"_"+sys_bin);
  TString histoname_num_QCD = TString("h_sieie_num_QCD_"+event_bin+"_"+sys_bin);
  TH1D *sieie_num_data = (TH1D*)f_data->Get(histoname_num);
  TH1D *sieie_num_QCDsideband_data = (TH1D*)f_data->Get(histoname_num_QCD);
  //Real photon contribution
  TH1D *sieie_num_GJets_40to100 = (TH1D*)GJets_40to100->Get(histoname_num);
  TH1D *sieie_num_GJets_100to200 = (TH1D*)GJets_100to200->Get(histoname_num);
  TH1D *sieie_num_GJets_200to400 = (TH1D*)GJets_200to400->Get(histoname_num);
  TH1D *sieie_num_GJets_400to600 = (TH1D*)GJets_400to600->Get(histoname_num);
  TH1D *sieie_num_GJets_600toInf = (TH1D*)GJets_600toInf->Get(histoname_num);
  sieie_num_GJets_40to100->Scale(20730.0*full_lumi*1000.0/4269126.0);
  sieie_num_GJets_100to200->Scale(9226.0*full_lumi*1000.0/5131808.0);
  sieie_num_GJets_200to400->Scale(2300.0*full_lumi*1000.0/10036339.0);
  sieie_num_GJets_400to600->Scale(277.4*full_lumi*1000.0/2435892.0);
  sieie_num_GJets_600toInf->Scale(93.38*full_lumi*1000.0/2117687.0);
  sieie_num_GJets_40to100->Add(sieie_num_GJets_100to200);
  sieie_num_GJets_40to100->Add(sieie_num_GJets_200to400);
  sieie_num_GJets_40to100->Add(sieie_num_GJets_400to600);
  sieie_num_GJets_40to100->Add(sieie_num_GJets_600toInf);
  // Real photon contribution to QCD sideband
  TH1D *sieie_num_QCDsideband_GJets_40to100 = (TH1D*)GJets_40to100->Get(histoname_num_QCD);
  TH1D *sieie_num_QCDsideband_GJets_100to200 = (TH1D*)GJets_100to200->Get(histoname_num_QCD);
  TH1D *sieie_num_QCDsideband_GJets_200to400 = (TH1D*)GJets_200to400->Get(histoname_num_QCD);
  TH1D *sieie_num_QCDsideband_GJets_400to600 = (TH1D*)GJets_400to600->Get(histoname_num_QCD);
  TH1D *sieie_num_QCDsideband_GJets_600toInf = (TH1D*)GJets_600toInf->Get(histoname_num_QCD);
  sieie_num_QCDsideband_GJets_40to100->Scale(20730.0*full_lumi*1000.0/4269126.0);
  sieie_num_QCDsideband_GJets_100to200->Scale(9226.0*full_lumi*1000.0/5131808.0);
  sieie_num_QCDsideband_GJets_200to400->Scale(2300.0*full_lumi*1000.0/10036339.0);
  sieie_num_QCDsideband_GJets_400to600->Scale(277.4*full_lumi*1000.0/2435892.0);
  sieie_num_QCDsideband_GJets_600toInf->Scale(93.38*full_lumi*1000.0/2117687.0);
  sieie_num_QCDsideband_GJets_40to100->Add(sieie_num_QCDsideband_GJets_100to200);
  sieie_num_QCDsideband_GJets_40to100->Add(sieie_num_QCDsideband_GJets_200to400);
  sieie_num_QCDsideband_GJets_40to100->Add(sieie_num_QCDsideband_GJets_400to600);
  sieie_num_QCDsideband_GJets_40to100->Add(sieie_num_QCDsideband_GJets_600toInf);
  
  if (j == 7 || j == 8){
    TH1D *sieie_num_GJets_40to100_central = (TH1D*)GJets_40to100->Get(TString("h_sieie_num_"+event_bin+"_0"));
    TH1D *sieie_num_GJets_100to200_central = (TH1D*)GJets_100to200->Get(TString("h_sieie_num_"+event_bin+"_0"));
    TH1D *sieie_num_GJets_200to400_central = (TH1D*)GJets_200to400->Get(TString("h_sieie_num_"+event_bin+"_0"));
    TH1D *sieie_num_GJets_400to600_central = (TH1D*)GJets_400to600->Get(TString("h_sieie_num_"+event_bin+"_0"));
    TH1D *sieie_num_GJets_600toInf_central = (TH1D*)GJets_600toInf->Get(TString("h_sieie_num_"+event_bin+"_0"));
    sieie_num_GJets_40to100_central->Scale(20730.0*full_lumi*1000.0/4269126.0);
    sieie_num_GJets_100to200_central->Scale(9226.0*full_lumi*1000.0/5131808.0);
    sieie_num_GJets_200to400_central->Scale(2300.0*full_lumi*1000.0/10036339.0);
    sieie_num_GJets_400to600_central->Scale(277.4*full_lumi*1000.0/2435892.0);
    sieie_num_GJets_600toInf_central->Scale(93.38*full_lumi*1000.0/2117687.0);
    sieie_num_GJets_40to100_central->Add(sieie_num_GJets_100to200_central);
    sieie_num_GJets_40to100_central->Add(sieie_num_GJets_200to400_central);
    sieie_num_GJets_40to100_central->Add(sieie_num_GJets_400to600_central);
    sieie_num_GJets_40to100_central->Add(sieie_num_GJets_600toInf_central);
    TH1D *sieie_num_GJets_40to100_sieieUp;
    TH1D *sieie_num_GJets_40to100_sieieDown;
    if (j == 7){
      sieie_num_GJets_40to100_sieieUp = (TH1D*)sieie_num_GJets_40to100->Clone("sieie_num_GJets_40to100_sieieUp");
    }
    else {
      sieie_num_GJets_40to100_sieieUp = (TH1D*)GJets_40to100->Get(TString("h_sieie_num_"+event_bin+"_7"));
      TH1D *sieie_num_GJets_100to200_sieieUp = (TH1D*)GJets_100to200->Get(TString("h_sieie_num_"+event_bin+"_7"));
      TH1D *sieie_num_GJets_200to400_sieieUp = (TH1D*)GJets_200to400->Get(TString("h_sieie_num_"+event_bin+"_7"));
      TH1D *sieie_num_GJets_400to600_sieieUp = (TH1D*)GJets_400to600->Get(TString("h_sieie_num_"+event_bin+"_7"));
      TH1D *sieie_num_GJets_600toInf_sieieUp = (TH1D*)GJets_600toInf->Get(TString("h_sieie_num_"+event_bin+"_7"));
      sieie_num_GJets_40to100_sieieUp->Scale(20730.0*full_lumi*1000.0/4269126.0);
      sieie_num_GJets_100to200_sieieUp->Scale(9226.0*full_lumi*1000.0/5131808.0);
      sieie_num_GJets_200to400_sieieUp->Scale(2300.0*full_lumi*1000.0/10036339.0);
      sieie_num_GJets_400to600_sieieUp->Scale(277.4*full_lumi*1000.0/2435892.0);
      sieie_num_GJets_600toInf_sieieUp->Scale(93.38*full_lumi*1000.0/2117687.0);
      sieie_num_GJets_40to100_sieieUp->Add(sieie_num_GJets_100to200_sieieUp);
      sieie_num_GJets_40to100_sieieUp->Add(sieie_num_GJets_200to400_sieieUp);
      sieie_num_GJets_40to100_sieieUp->Add(sieie_num_GJets_400to600_sieieUp);
      sieie_num_GJets_40to100_sieieUp->Add(sieie_num_GJets_600toInf_sieieUp);
    }
    if (j == 8){
      sieie_num_GJets_40to100_sieieDown = (TH1D*)sieie_num_GJets_40to100->Clone("sieie_num_GJets_40to100_sieieDown");
    }
    else {
      sieie_num_GJets_40to100_sieieDown = (TH1D*)GJets_40to100->Get(TString("h_sieie_num_"+event_bin+"_8"));
      TH1D *sieie_num_GJets_100to200_sieieDown = (TH1D*)GJets_100to200->Get(TString("h_sieie_num_"+event_bin+"_8"));
      TH1D *sieie_num_GJets_200to400_sieieDown = (TH1D*)GJets_200to400->Get(TString("h_sieie_num_"+event_bin+"_8"));
      TH1D *sieie_num_GJets_400to600_sieieDown = (TH1D*)GJets_400to600->Get(TString("h_sieie_num_"+event_bin+"_8"));
      TH1D *sieie_num_GJets_600toInf_sieieDown = (TH1D*)GJets_600toInf->Get(TString("h_sieie_num_"+event_bin+"_8"));
      sieie_num_GJets_40to100_sieieDown->Scale(20730.0*full_lumi*1000.0/4269126.0);
      sieie_num_GJets_100to200_sieieDown->Scale(9226.0*full_lumi*1000.0/5131808.0);
      sieie_num_GJets_200to400_sieieDown->Scale(2300.0*full_lumi*1000.0/10036339.0);
      sieie_num_GJets_400to600_sieieDown->Scale(277.4*full_lumi*1000.0/2435892.0);
      sieie_num_GJets_600toInf_sieieDown->Scale(93.38*full_lumi*1000.0/2117687.0);
      sieie_num_GJets_40to100_sieieDown->Add(sieie_num_GJets_100to200_sieieDown);
      sieie_num_GJets_40to100_sieieDown->Add(sieie_num_GJets_200to400_sieieDown);
      sieie_num_GJets_40to100_sieieDown->Add(sieie_num_GJets_400to600_sieieDown);
      sieie_num_GJets_40to100_sieieDown->Add(sieie_num_GJets_600toInf_sieieDown);
    }
    
    TH1D *sieie_num_QCDsideband_GJets_40to100_central = (TH1D*)GJets_40to100->Get(TString("h_sieie_num_QCD_"+event_bin+"_0"));
    TH1D *sieie_num_QCDsideband_GJets_100to200_central = (TH1D*)GJets_100to200->Get(TString("h_sieie_num_QCD_"+event_bin+"_0"));
    TH1D *sieie_num_QCDsideband_GJets_200to400_central = (TH1D*)GJets_200to400->Get(TString("h_sieie_num_QCD_"+event_bin+"_0"));
    TH1D *sieie_num_QCDsideband_GJets_400to600_central = (TH1D*)GJets_400to600->Get(TString("h_sieie_num_QCD_"+event_bin+"_0"));
    TH1D *sieie_num_QCDsideband_GJets_600toInf_central = (TH1D*)GJets_600toInf->Get(TString("h_sieie_num_QCD_"+event_bin+"_0"));
    sieie_num_QCDsideband_GJets_40to100_central->Scale(20730.0*full_lumi*1000.0/4269126.0);
    sieie_num_QCDsideband_GJets_100to200_central->Scale(9226.0*full_lumi*1000.0/5131808.0);
    sieie_num_QCDsideband_GJets_200to400_central->Scale(2300.0*full_lumi*1000.0/10036339.0);
    sieie_num_QCDsideband_GJets_400to600_central->Scale(277.4*full_lumi*1000.0/2435892.0);
    sieie_num_QCDsideband_GJets_600toInf_central->Scale(93.38*full_lumi*1000.0/2117687.0);
    sieie_num_QCDsideband_GJets_40to100_central->Add(sieie_num_QCDsideband_GJets_100to200_central);
    sieie_num_QCDsideband_GJets_40to100_central->Add(sieie_num_QCDsideband_GJets_200to400_central);
    sieie_num_QCDsideband_GJets_40to100_central->Add(sieie_num_QCDsideband_GJets_400to600_central);
    sieie_num_QCDsideband_GJets_40to100_central->Add(sieie_num_QCDsideband_GJets_600toInf_central);
    TH1D *sieie_num_QCDsideband_GJets_40to100_sieieUp;
    TH1D *sieie_num_QCDsideband_GJets_40to100_sieieDown;
    if (j == 7){
      sieie_num_QCDsideband_GJets_40to100_sieieUp = (TH1D*)sieie_num_QCDsideband_GJets_40to100->Clone("sieie_num_QCDsideband_GJets_40to100_sieieUp");
    }
    else {
      sieie_num_QCDsideband_GJets_40to100_sieieUp = (TH1D*)GJets_40to100->Get(TString("h_sieie_num_QCD_"+event_bin+"_7"));
      TH1D *sieie_num_QCDsideband_GJets_100to200_sieieUp = (TH1D*)GJets_100to200->Get(TString("h_sieie_num_QCD_"+event_bin+"_7"));
      TH1D *sieie_num_QCDsideband_GJets_200to400_sieieUp = (TH1D*)GJets_200to400->Get(TString("h_sieie_num_QCD_"+event_bin+"_7"));
      TH1D *sieie_num_QCDsideband_GJets_400to600_sieieUp = (TH1D*)GJets_400to600->Get(TString("h_sieie_num_QCD_"+event_bin+"_7"));
      TH1D *sieie_num_QCDsideband_GJets_600toInf_sieieUp = (TH1D*)GJets_600toInf->Get(TString("h_sieie_num_QCD_"+event_bin+"_7"));
      sieie_num_QCDsideband_GJets_40to100_sieieUp->Scale(20730.0*full_lumi*1000.0/4269126.0);
      sieie_num_QCDsideband_GJets_100to200_sieieUp->Scale(9226.0*full_lumi*1000.0/5131808.0);
      sieie_num_QCDsideband_GJets_200to400_sieieUp->Scale(2300.0*full_lumi*1000.0/10036339.0);
      sieie_num_QCDsideband_GJets_400to600_sieieUp->Scale(277.4*full_lumi*1000.0/2435892.0);
      sieie_num_QCDsideband_GJets_600toInf_sieieUp->Scale(93.38*full_lumi*1000.0/2117687.0);
      sieie_num_QCDsideband_GJets_40to100_sieieUp->Add(sieie_num_QCDsideband_GJets_100to200_sieieUp);
      sieie_num_QCDsideband_GJets_40to100_sieieUp->Add(sieie_num_QCDsideband_GJets_200to400_sieieUp);
      sieie_num_QCDsideband_GJets_40to100_sieieUp->Add(sieie_num_QCDsideband_GJets_400to600_sieieUp);
      sieie_num_QCDsideband_GJets_40to100_sieieUp->Add(sieie_num_QCDsideband_GJets_600toInf_sieieUp);
    }
    if (j == 8){
      sieie_num_QCDsideband_GJets_40to100_sieieDown = (TH1D*)sieie_num_QCDsideband_GJets_40to100->Clone("sieie_num_QCDsideband_GJets_40to100_sieieDown");
    }
    else {
      sieie_num_QCDsideband_GJets_40to100_sieieDown = (TH1D*)GJets_40to100->Get(TString("h_sieie_num_QCD_"+event_bin+"_8"));
      TH1D *sieie_num_QCDsideband_GJets_100to200_sieieDown = (TH1D*)GJets_100to200->Get(TString("h_sieie_num_QCD_"+event_bin+"_8"));
      TH1D *sieie_num_QCDsideband_GJets_200to400_sieieDown = (TH1D*)GJets_200to400->Get(TString("h_sieie_num_QCD_"+event_bin+"_8"));
      TH1D *sieie_num_QCDsideband_GJets_400to600_sieieDown = (TH1D*)GJets_400to600->Get(TString("h_sieie_num_QCD_"+event_bin+"_8"));
      TH1D *sieie_num_QCDsideband_GJets_600toInf_sieieDown = (TH1D*)GJets_600toInf->Get(TString("h_sieie_num_QCD_"+event_bin+"_8"));
      sieie_num_QCDsideband_GJets_40to100_sieieDown->Scale(20730.0*full_lumi*1000.0/4269126.0);
      sieie_num_QCDsideband_GJets_100to200_sieieDown->Scale(9226.0*full_lumi*1000.0/5131808.0);
      sieie_num_QCDsideband_GJets_200to400_sieieDown->Scale(2300.0*full_lumi*1000.0/10036339.0);
      sieie_num_QCDsideband_GJets_400to600_sieieDown->Scale(277.4*full_lumi*1000.0/2435892.0);
      sieie_num_QCDsideband_GJets_600toInf_sieieDown->Scale(93.38*full_lumi*1000.0/2117687.0);
      sieie_num_QCDsideband_GJets_40to100_sieieDown->Add(sieie_num_QCDsideband_GJets_100to200_sieieDown);
      sieie_num_QCDsideband_GJets_40to100_sieieDown->Add(sieie_num_QCDsideband_GJets_200to400_sieieDown);
      sieie_num_QCDsideband_GJets_40to100_sieieDown->Add(sieie_num_QCDsideband_GJets_400to600_sieieDown);
      sieie_num_QCDsideband_GJets_40to100_sieieDown->Add(sieie_num_QCDsideband_GJets_600toInf_sieieDown);
    }
    
    int maxbin = sieie_num_GJets_40to100_central->GetMaximumBin();
    int maxbin_QCDsideband = sieie_num_QCDsideband_GJets_40to100_central->GetMaximumBin();
    for(int bin_number = 1; bin_number <= sieie_num_GJets_40to100_central->GetNbinsX(); bin_number++){
      if ((bin_number < maxbin && j == 7) || (bin_number > maxbin && j == 8)){
        sieie_num_GJets_40to100->SetBinContent(bin_number, sieie_num_GJets_40to100_sieieUp->GetBinContent(bin_number));
      }
      else if ((bin_number <= maxbin && j == 8) || (bin_number >= maxbin && j == 7)){
        sieie_num_GJets_40to100->SetBinContent(bin_number, sieie_num_GJets_40to100_sieieDown->GetBinContent(bin_number));
      }
      if ((bin_number < maxbin_QCDsideband && j == 7) || (bin_number > maxbin_QCDsideband && j == 8)){
        sieie_num_QCDsideband_GJets_40to100->SetBinContent(bin_number, sieie_num_QCDsideband_GJets_40to100_sieieUp->GetBinContent(bin_number));
      }
      else if ((bin_number <= maxbin_QCDsideband && j == 8) || (bin_number >= maxbin_QCDsideband && j == 7)){
        sieie_num_QCDsideband_GJets_40to100->SetBinContent(bin_number, sieie_num_QCDsideband_GJets_40to100_sieieDown->GetBinContent(bin_number));
      }
    }
  }
  
  sieie_num_QCDsideband_GJets_40to100->GetXaxis()->SetTitle("#sigma_{i#etai#eta}");
  sieie_num_QCDsideband_GJets_40to100->GetYaxis()->SetTitle("Yield for 35.9 fb^{-1}");
  sieie_num_QCDsideband_GJets_40to100->GetYaxis()->SetTitleOffset(1.8);
  sieie_num_QCDsideband_GJets_40to100->SetTitle("");
  sieie_num_QCDsideband_GJets_40to100->SetStats(0);
  sieie_num_QCDsideband_GJets_40to100->SetLineWidth(2);
  sieie_num_QCDsideband_GJets_40to100->SetLineColor(kRed);
  sieie_num_QCDsideband_data->GetXaxis()->SetTitle("#sigma_{i#etai#eta}");
  sieie_num_QCDsideband_data->GetYaxis()->SetTitle("Yield for 35.9 fb^{-1}");
  sieie_num_QCDsideband_data->GetYaxis()->SetTitleOffset(1.8);
  sieie_num_QCDsideband_data->SetTitle("");
  sieie_num_QCDsideband_data->SetStats(0);
  sieie_num_QCDsideband_data->SetLineWidth(2);
  sieie_num_QCDsideband_data->SetLineColor(kBlack);
  sieie_num_QCDsideband_data->SetMarkerStyle(8);
  
  sieie_num_QCDsideband_data->GetXaxis()->SetLabelFont(42);
  sieie_num_QCDsideband_data->GetXaxis()->SetLabelSize(0.040);
  sieie_num_QCDsideband_data->GetXaxis()->SetTitleFont(42);
  sieie_num_QCDsideband_data->GetXaxis()->SetTitleSize(0.045);
  sieie_num_QCDsideband_data->GetYaxis()->SetLabelFont(42);
  sieie_num_QCDsideband_data->GetYaxis()->SetLabelSize(0.040);
  sieie_num_QCDsideband_data->GetYaxis()->SetTitleFont(42);
  sieie_num_QCDsideband_data->GetYaxis()->SetTitleSize(0.045);

  TH1F* h_sieie_remainder = (TH1F*)sieie_num_QCDsideband_data->Clone("h_sieie_remainder");
  h_sieie_remainder->Add(sieie_num_QCDsideband_GJets_40to100,-1.0); // Subtract sieie_num_QCDsideband_GJets_40to100 from sieie_num_QCDsideband_data
  h_sieie_remainder->SetLineColor(kGreen);
  
  // Set all bins to > 0 for template fitting
  for(int bin = 1; bin <= h_sieie_remainder->GetNbinsX(); bin++){
    if(sieie_num_data->GetBinContent(bin) <= 1.0e-06)
      sieie_num_data->SetBinContent(bin,1.0e-06);
    if(sieie_num_GJets_40to100->GetBinContent(bin) <= 9.0e-07)
      sieie_num_GJets_40to100->SetBinContent(bin,9.0e-07);
    if(h_sieie_remainder->GetBinContent(bin) <= 1.0e-07)
      h_sieie_remainder->SetBinContent(bin,1.0e-07);
  }
  
  // Plot the fake templates
  if (j == 0 && i >= 1 && i <= 5){
    TCanvas *c = new TCanvas("c", "canvas",700,640);
    gStyle->SetOptStat(0);
    gStyle->SetLegendBorderSize(0);
    c->SetLeftMargin(0.15);
    c->cd();
  
    string lowPt = "undefined"+event_bin;
    string highPt = "undefined"+event_bin;
    if (i == 1){
      lowPt = "175";
      highPt = "200";
    }
    else if (i == 2){
      lowPt = "200";
      highPt = "250";
    }
    else if (i == 3){
      lowPt = "250";
      highPt = "300";
    }
    else if (i == 4){
      lowPt = "300";
      highPt = "400";
    }
    else if (i == 5){
      lowPt = "400";
      highPt = "1000";
    }
    
    double upper_limit = 0.0;
    if (highPt == "200") upper_limit = 7000.0; // good
    else if (highPt == "250") upper_limit = 5000.0; // good
    else if (highPt == "300") upper_limit = 1500.0; // good
    else if (highPt == "400") upper_limit = 800.0;
    else if (highPt == "1000") upper_limit = 250.0;
    sieie_num_QCDsideband_GJets_40to100->GetYaxis()->SetRangeUser(0.0,upper_limit);
    sieie_num_QCDsideband_GJets_40to100->Draw("HIST");
    h_sieie_remainder->Draw("HIST SAME");
    sieie_num_QCDsideband_data->Draw("SAME 0 E1");
    
    TString legTitle = TString(lowPt+" < Photon #it{p}_{T} < "+highPt);
    TLegend *leg = new TLegend(0.508711,0.672104,0.892665,0.872757,legTitle);
    leg->AddEntry(sieie_num_QCDsideband_data,"Data","P E1");
    leg->AddEntry(sieie_num_QCDsideband_GJets_40to100,"Gen-matched #gamma+jets MC","L");
    leg->AddEntry(h_sieie_remainder,"Fake template","L");
    leg->SetFillStyle(0);
    leg->Draw();
    
    TLatex *texS = new TLatex(0.61023,0.907173,"35.9 fb^{-1} (13 TeV)");
    texS->SetNDC();
    texS->SetTextFont(42);
    texS->SetTextSize(0.045);
    texS->Draw();
    TLatex *texS1 = new TLatex(0.18092,0.837173,"#bf{CMS} #it{Preliminary}");
    texS1->SetNDC();
    texS1->SetTextFont(42);
    texS1->SetTextSize(0.045);
    texS1->Draw();
    
    TString saveas_name = TString("sideband_templates_"+lowPt+"to"+highPt+".png");
    c->SaveAs(saveas_name);
    saveas_name = TString("sideband_templates_"+lowPt+"to"+highPt+".pdf");
    c->SaveAs(saveas_name);
  }
  
  //Perform template fit of photon template+QCD template to uncorrected numerator

  //Clone histograms for fitting
  TH1D* h_data = (TH1D*)sieie_num_data->Clone();
  TH1D* h_qcd = (TH1D*)h_sieie_remainder->Clone();
  TH1D* h_gjet = (TH1D*)sieie_num_GJets_40to100->Clone();
  //Set all bin values to be nonzero for the sake of RooFit
  //Variable being fit (abscissa)
  //Fit will be within full range of sigmaietaieta up to 0.025, though final estimate of fake ratio
  //will be based on integral of fit up to sigmaietaieta 0.0104
  RooRealVar sieie("sieie","sigma_ietaieta",0.0000,0.0250);
  sieie.setRange("fullrange",0.0000,0.0250);
  //Variables that will contain real and fake estimates - can vary between 0 and ndataentries
  Double_t ndataentries = h_data->Integral();
  RooRealVar realnum("realnum","realnum",0,ndataentries);
  RooRealVar fakenum("fakenum","fakenum",0,ndataentries);
  //Histogram pdfs to be fit - Real photons, QCD fakes
  RooDataHist faketemplate("faketemplate","fake template",sieie,h_qcd);
  RooHistPdf fakepdf("fakepdf","test hist fake pdf",sieie,faketemplate);
  RooExtendPdf extpdffake("Fake","extpdffake",fakepdf,fakenum,"fullrange");
  RooDataHist realtemplate("realtemplate","real template",sieie,h_gjet);
  RooHistPdf realpdf("realpdf","test hist real pdf",sieie,realtemplate);
  RooExtendPdf extpdfreal("Real","extpdfreal",realpdf,realnum,"fullrange");
  //Data distribution to be fitted to
  RooDataHist data("data","data to be fitted to",sieie,h_data);
  //Fit real+fake templates to data
  RooAddPdf model("model","real photons + QCD fakes",RooArgList(extpdfreal,extpdffake));
  model.fitTo(data,RooFit::Minos());
  //Estimates and errors, full range
  Double_t fakevalue = fakenum.getValV();
  Double_t fakeerror = max(fabs(fakenum.getErrorHi()),fabs(fakenum.getErrorLo()));
  Double_t realvalue = realnum.getValV();
  Double_t realerror = max(fabs(realnum.getErrorHi()),fabs(realnum.getErrorLo()));
  cout<<"fakevalue="<<fakevalue<<"+"<<fabs(fakenum.getErrorHi())<<"-"<<fabs(fakenum.getErrorLo())<<endl;
  cout<<"realvalue="<<realvalue<<"+"<<fabs(realnum.getErrorHi())<<"-"<<fabs(realnum.getErrorLo())<<endl;
  if (realnum.getErrorHi() == 0 || realnum.getErrorLo() == 0 || fakenum.getErrorHi() == 0 || fakenum.getErrorLo() == 0){
    model.fitTo(data);
    cout<<"fakevalue="<<fakevalue<<"+"<<fabs(fakenum.getErrorHi())<<"-"<<fabs(fakenum.getErrorLo())<<endl;
    cout<<"realvalue="<<realvalue<<"+"<<fabs(realnum.getErrorHi())<<"-"<<fabs(realnum.getErrorLo())<<endl;
    fakevalue = fakenum.getValV();
    fakeerror = max(fabs(fakenum.getErrorHi()),fabs(fakenum.getErrorLo()));
    realvalue = realnum.getValV();
    realerror = max(fabs(realnum.getErrorHi()),fabs(realnum.getErrorLo()));
  }

  sieie.setRange("signal",0.0000,0.0104);
  //Find fraction of fake/real pdfs in partial range, normalized to 1
  RooAbsReal* int_full_fakepdf = fakepdf.createIntegral(sieie,RooFit::NormSet(sieie),Range("fullrange"));
  RooAbsReal* int_sgnl_fakepdf = fakepdf.createIntegral(sieie,NormSet(sieie),Range("signal"));
  RooAbsReal* int_full_realpdf = realpdf.createIntegral(sieie,NormSet(sieie),Range("fullrange"));
  RooAbsReal* int_sgnl_realpdf = realpdf.createIntegral(sieie,NormSet(sieie),Range("signal"));
  Double_t frac_fakeInSig = int_sgnl_fakepdf->getVal();
  Double_t frac_realInSig = int_sgnl_realpdf->getVal();
  Double_t fakeInSig = fakevalue*frac_fakeInSig;
  Double_t fakeInSig_error = fakeerror*frac_fakeInSig;
  Double_t realInSig = realvalue*frac_realInSig;
  Double_t realInSig_error = realerror*frac_realInSig;
  //Multiply the numerator observed in data by fakeFractionInNum to get the estimated numerator
  //contribution coming from QCD fakes (as opposed to real photons)
  Double_t fakeFractionInNum = fakeInSig/(realInSig+fakeInSig);
  Double_t fakeFraction_error = fakeFractionInNum*sqrt(pow((fakeInSig_error/fakeInSig),2) + pow((sqrt(pow(realInSig_error,2)+pow(fakeInSig_error,2))/(realInSig+fakeInSig)),2));
  
  std::vector<Float_t> fraction_and_error;
  fraction_and_error.clear();
  fraction_and_error.push_back(fakeFractionInNum);
  fraction_and_error.push_back(fakeFraction_error);
    
  // Plot template fits
  if (i >= 1 && i <= 5){
    TCanvas *c = new TCanvas("c", "canvas",700,640);
    gStyle->SetOptStat(0);
    gStyle->SetLegendBorderSize(0);
    c->SetLeftMargin(0.15);
    c->SetLogy();
    c->cd();
    
    string lowPt = "undefined"+event_bin;
    string highPt = "undefined"+event_bin;
    if (i == 1){
      lowPt = "175";
      highPt = "200";
    }
    else if (i == 2){
      lowPt = "200";
      highPt = "250";
    }
    else if (i == 3){
      lowPt = "250";
      highPt = "300";
    }
    else if (i == 4){
      lowPt = "300";
      highPt = "400";
    }
    else if (i == 5){
      lowPt = "400";
      highPt = "1000";
    }
  
    sieie_num_GJets_40to100->Scale(realvalue/(sieie_num_GJets_40to100->Integral()));
    h_sieie_remainder->Scale(fakevalue/(h_sieie_remainder->Integral()));

    sieie_num_data->GetXaxis()->SetTitle("");
    sieie_num_data->GetXaxis()->SetTickLength(0);
    sieie_num_data->GetXaxis()->SetLabelOffset(999);
    sieie_num_data->GetYaxis()->SetTitle("");
    sieie_num_data->GetYaxis()->SetTickLength(0);
    sieie_num_data->GetYaxis()->SetLabelOffset(999);
    sieie_num_data->SetStats(0);
    sieie_num_data->SetTitle("");

    sieie_num_data->SetMarkerColor(kWhite);
    sieie_num_data->SetLineColor(kWhite);
    sieie_num_GJets_40to100->SetLineColor(kRed);
    sieie_num_GJets_40to100->SetLineWidth(3);
    sieie_num_GJets_40to100->SetLineStyle(kDashed);
    h_sieie_remainder->SetLineColor(kGreen);
    h_sieie_remainder->SetLineWidth(3);
    h_sieie_remainder->SetLineStyle(kDashed);
    TH1D* h_gjet_temp = (TH1D*)h_sieie_remainder->Clone();
    TH1D* h_real_temp = (TH1D*)sieie_num_GJets_40to100->Clone();
    h_real_temp->Add(h_gjet_temp);
    h_real_temp->SetLineColor(kBlue);
    h_real_temp->SetLineStyle(kSolid);
    sieie_num_data->GetYaxis()->SetRangeUser(1.0,1000000.0); // y-axis on log scale
    // sieie_num_data->GetYaxis()->SetRangeUser(1.0,120000.0);
    sieie_num_data->Draw();
    h_real_temp->Draw("HIST SAME");
    sieie_num_GJets_40to100->Draw("HIST SAME");
    h_sieie_remainder->Draw("HIST SAME");
    sieie_num_data->SetLineWidth(3);
    sieie_num_data->SetMarkerColor(kBlack);
    sieie_num_data->SetMarkerStyle(8);
    sieie_num_data->SetLineColor(kBlack);
    sieie_num_data->Draw("SAME 0 E1");

    TLatex *texS = new TLatex(0.61023,0.907173,"35.9 fb^{-1} (13 TeV)");
    texS->SetNDC();
    texS->SetTextFont(42);
    texS->SetTextSize(0.045);
    texS->Draw();
    TLatex *texS1 = new TLatex(0.18092,0.837173,"#bf{CMS} #it{Preliminary}");
    texS1->SetNDC();
    texS1->SetTextFont(42);
    texS1->SetTextSize(0.045);
    texS1->Draw();

    TString fakeFractionInNum_text = TString(boost::lexical_cast<string>(boost::format("%.4f") % fakeFractionInNum));
    TString fakeFraction_error_text = TString(boost::lexical_cast<string>(boost::format("%.4f") % fakeFraction_error));
    TLatex *fakeratio_title = new TLatex(0.59,0.55,"B/(S+B) =");
    fakeratio_title->SetNDC();
    fakeratio_title->SetTextFont(42);
    fakeratio_title->SetTextSize(0.06);
    fakeratio_title->Draw();
    TLatex *fakeratio = new TLatex(0.52,0.48,fakeFractionInNum_text+" #pm "+fakeFraction_error_text);
    fakeratio->SetNDC();
    fakeratio->SetTextFont(42);
    fakeratio->SetTextSize(0.06);
    fakeratio->Draw();
    
    TString leg_title = TString(lowPt + " < Photon #it{p}_{T} < " + highPt);
    TLegend *leg = new TLegend(0.548711,0.672104,0.932665,0.872757,leg_title);
    leg->AddEntry(sieie_num_data,"Numerator (data)","P E1");
    leg->AddEntry(h_real_temp,"Fit","L");
    leg->AddEntry(sieie_num_GJets_40to100,"Real (GJet MC)","L");
    leg->AddEntry(h_sieie_remainder,"Fake (data)","L");
    leg->SetFillStyle(0);
    leg->Draw();

    c->Update();

    double xmin = c->GetUxmin();
    double ymin = c->GetUymin();
    double xmax = c->GetUxmax();
    double ymax = c->GetUymax();

    TGaxis *xaxis = new TGaxis(xmin,pow(10.0,ymin),xmax,pow(10.0,ymin),xmin,xmax,510);
    // TGaxis *xaxis = new TGaxis(xmin,ymin,xmax,ymin,xmin,xmax,510);
    xaxis->SetTitle("Photon #sigma_{i#etai#eta}");
    xaxis->SetLabelFont(42);
    xaxis->SetLabelSize(0.040);
    xaxis->SetTitleFont(42);
    xaxis->SetTitleSize(0.045);
    xaxis->Draw("SAME");

    TGaxis *xaxis_top = new TGaxis(xmin,pow(10.0,ymax),xmax,pow(10.0,ymax),xmin,xmax,510,"-");
    // TGaxis *xaxis_top = new TGaxis(xmin,ymax,xmax,ymax,xmin,xmax,510,"-");
    xaxis_top->SetTitle("");
    xaxis_top->SetLabelOffset(999);
    xaxis_top->Draw("SAME");

    TGaxis *yaxis = new TGaxis(xmin,pow(10.0,ymin),xmin,pow(10.0,ymax),pow(10.0,ymin),pow(10.0,ymax),510,"G");
    // TGaxis *yaxis = new TGaxis(xmin,ymin,xmin,ymax,ymin,ymax,510);
    yaxis->SetTitle("Events");
    yaxis->SetLabelFont(42);
    yaxis->SetLabelSize(0.040);
    yaxis->SetTitleFont(42);
    yaxis->SetTitleSize(0.045);
    yaxis->SetTitleOffset(1.3);
    yaxis->Draw("SAME");

    TGaxis *yaxis_right = new TGaxis(xmax,pow(10.0,ymin),xmax,pow(10.0,ymax),pow(10.0,ymin),pow(10.0,ymax),510,"G+");
    // TGaxis *yaxis_right = new TGaxis(xmax,ymin,xmax,ymax,ymin,ymax,510,"+");
    yaxis_right->SetTitle("");
    yaxis_right->SetLabelOffset(999);
    yaxis_right->Draw("SAME");
    
    string suffix = "";
    if (j == 1) suffix = "_sbUp";
    else if (j == 2) suffix = "_sbDown";
    else if (j == 3) suffix = "_metUp";
    else if (j == 4) suffix = "_metDown";
    else if (j == 5) suffix = "_binUp";
    else if (j == 6) suffix = "_binDown";
    else if (j == 7) suffix = "_sieieLeft";
    else if (j == 8) suffix = "_sieieRight";
    TString saveas_name = TString("num_fit_" + lowPt + "to" + highPt + suffix + ".png");
    c->SaveAs(saveas_name);
    saveas_name = TString("num_fit_" + lowPt + "to" + highPt + suffix + ".pdf");
    c->SaveAs(saveas_name);
  }
  
  return fraction_and_error;
}



//Systematics bins:
//    0       1      2       3       4       5       6         7          8           9            10
// Standard  sbUP  sbDown  metUP  metDown  binUP  binDown  sieieLeft  sieieRight  templateUp  templateDown
std::vector<Float_t> QCDfake_getSysbContents_phopt(int sysb, Float_t frac_175to200, Float_t frac_200to250, Float_t frac_250to300, Float_t frac_300to400, Float_t frac_400to1000, Float_t frac_error_175to200, Float_t frac_error_200to250, Float_t frac_error_250to300, Float_t frac_error_300to400, Float_t frac_error_400to1000)
{
  // gStyle->SetLegendBorderSize(0);

  TFile *f_data = new TFile("data_QCDfake_all.root");

  string sys_bin_string = boost::lexical_cast<string>(sysb);
  // For sysbins 9 and 10, we start from the central value here and compute the shifts below
  if(sysb > 8)
    sys_bin_string = "0";
  TString sieie_175to200_num_data_name = TString("h_sieie_num_1_"+sys_bin_string);
  TString sieie_200to250_num_data_name = TString("h_sieie_num_2_"+sys_bin_string);
  TString sieie_250to300_num_data_name = TString("h_sieie_num_3_"+sys_bin_string);
  TString sieie_300to400_num_data_name = TString("h_sieie_num_4_"+sys_bin_string);
  TString sieie_400to1000_num_data_name = TString("h_sieie_num_5_"+sys_bin_string);
  TString sieie_175to200_den_data_name = TString("h_sieie_den_1_"+sys_bin_string);
  TString sieie_200to250_den_data_name = TString("h_sieie_den_2_"+sys_bin_string);
  TString sieie_250to300_den_data_name = TString("h_sieie_den_3_"+sys_bin_string);
  TString sieie_300to400_den_data_name = TString("h_sieie_den_4_"+sys_bin_string);
  TString sieie_400to1000_den_data_name = TString("h_sieie_den_5_"+sys_bin_string);

  TH1D *sieie_175to200_num_data = (TH1D*)f_data->Get(sieie_175to200_num_data_name);
  TH1D *sieie_200to250_num_data = (TH1D*)f_data->Get(sieie_200to250_num_data_name);
  TH1D *sieie_250to300_num_data = (TH1D*)f_data->Get(sieie_250to300_num_data_name);
  TH1D *sieie_300to400_num_data = (TH1D*)f_data->Get(sieie_300to400_num_data_name);
  TH1D *sieie_400to1000_num_data = (TH1D*)f_data->Get(sieie_400to1000_num_data_name);
  TH1D *sieie_175to200_den_data = (TH1D*)f_data->Get(sieie_175to200_den_data_name);
  TH1D *sieie_200to250_den_data = (TH1D*)f_data->Get(sieie_200to250_den_data_name);
  TH1D *sieie_250to300_den_data = (TH1D*)f_data->Get(sieie_250to300_den_data_name);
  TH1D *sieie_300to400_den_data = (TH1D*)f_data->Get(sieie_300to400_den_data_name);
  TH1D *sieie_400to1000_den_data = (TH1D*)f_data->Get(sieie_400to1000_den_data_name);

  Float_t den_175to200 = sieie_175to200_den_data->Integral();
  Float_t den_200to250 = sieie_200to250_den_data->Integral();
  Float_t den_250to300 = sieie_250to300_den_data->Integral();
  Float_t den_300to400 = sieie_300to400_den_data->Integral();
  Float_t den_400to1000 = sieie_400to1000_den_data->Integral();

  Float_t numUncorr_175to200 = sieie_175to200_num_data->Integral(1,sieie_175to200_num_data->FindBin(0.0104));
  Float_t numUncorr_200to250 = sieie_200to250_num_data->Integral(1,sieie_200to250_num_data->FindBin(0.0104));
  Float_t numUncorr_250to300 = sieie_250to300_num_data->Integral(1,sieie_250to300_num_data->FindBin(0.0104));
  Float_t numUncorr_300to400 = sieie_300to400_num_data->Integral(1,sieie_300to400_num_data->FindBin(0.0104));
  Float_t numUncorr_400to1000 = sieie_400to1000_num_data->Integral(1,sieie_400to1000_num_data->FindBin(0.0104));
  Float_t numCorr_175to200 = numUncorr_175to200*frac_175to200;
  Float_t numCorr_200to250 = numUncorr_200to250*frac_200to250;
  Float_t numCorr_250to300 = numUncorr_250to300*frac_250to300;
  Float_t numCorr_300to400 = numUncorr_300to400*frac_300to400;
  Float_t numCorr_400to1000 = numUncorr_400to1000*frac_400to1000;
  Float_t ratio_175to200 = numCorr_175to200/den_175to200;
  Float_t ratio_200to250 = numCorr_200to250/den_200to250;
  Float_t ratio_250to300 = numCorr_250to300/den_250to300;
  Float_t ratio_300to400 = numCorr_300to400/den_300to400;
  Float_t ratio_400to1000 = numCorr_400to1000/den_400to1000;
  
  Float_t yerror_den_175to200 = sqrt(den_175to200);
  Float_t yerror_den_200to250 = sqrt(den_200to250);
  Float_t yerror_den_250to300 = sqrt(den_250to300);
  Float_t yerror_den_300to400 = sqrt(den_300to400);
  Float_t yerror_den_400to1000 = sqrt(den_400to1000);
  Float_t yerror_numUncorr_175to200 = sqrt(numUncorr_175to200);
  Float_t yerror_numUncorr_200to250 = sqrt(numUncorr_200to250);
  Float_t yerror_numUncorr_250to300 = sqrt(numUncorr_250to300);
  Float_t yerror_numUncorr_300to400 = sqrt(numUncorr_300to400);
  Float_t yerror_numUncorr_400to1000 = sqrt(numUncorr_400to1000);
  Float_t yerror_numCorr_175to200 = numCorr_175to200*sqrt(pow(yerror_numUncorr_175to200/numUncorr_175to200,2)+pow(frac_error_175to200/frac_175to200,2));
  Float_t yerror_numCorr_200to250 = numCorr_200to250*sqrt(pow(yerror_numUncorr_200to250/numUncorr_200to250,2)+pow(frac_error_200to250/frac_200to250,2));
  Float_t yerror_numCorr_250to300 = numCorr_250to300*sqrt(pow(yerror_numUncorr_250to300/numUncorr_250to300,2)+pow(frac_error_250to300/frac_250to300,2));
  Float_t yerror_numCorr_300to400 = numCorr_300to400*sqrt(pow(yerror_numUncorr_300to400/numUncorr_300to400,2)+pow(frac_error_300to400/frac_300to400,2));
  Float_t yerror_numCorr_400to1000 = numCorr_400to1000*sqrt(pow(yerror_numUncorr_400to1000/numUncorr_400to1000,2)+pow(frac_error_400to1000/frac_400to1000,2));
  Float_t yerror_ratio_175to200 = ratio_175to200*sqrt(pow(yerror_numCorr_175to200/numCorr_175to200,2)+pow(yerror_den_175to200/den_175to200,2));
  Float_t yerror_ratio_200to250 = ratio_200to250*sqrt(pow(yerror_numCorr_200to250/numCorr_200to250,2)+pow(yerror_den_200to250/den_200to250,2));
  Float_t yerror_ratio_250to300 = ratio_250to300*sqrt(pow(yerror_numCorr_250to300/numCorr_250to300,2)+pow(yerror_den_250to300/den_250to300,2));
  Float_t yerror_ratio_300to400 = ratio_300to400*sqrt(pow(yerror_numCorr_300to400/numCorr_300to400,2)+pow(yerror_den_300to400/den_300to400,2));
  Float_t yerror_ratio_400to1000 = ratio_400to1000*sqrt(pow(yerror_numCorr_400to1000/numCorr_400to1000,2)+pow(yerror_den_400to1000/den_400to1000,2));

  if(sysb == 9)
  {
    ratio_175to200 += yerror_ratio_175to200;
    ratio_200to250 += yerror_ratio_200to250;
    ratio_250to300 += yerror_ratio_250to300;
    ratio_300to400 += yerror_ratio_300to400;
    ratio_400to1000 += yerror_ratio_400to1000;
  }
  else if(sysb == 10)
  {
    ratio_175to200 -= yerror_ratio_175to200;
    ratio_200to250 -= yerror_ratio_200to250;
    ratio_250to300 -= yerror_ratio_250to300;
    ratio_300to400 -= yerror_ratio_300to400;
    ratio_400to1000 -= yerror_ratio_400to1000;
  }

  std::vector<Float_t> bin_contents;
  bin_contents.clear();
  bin_contents.push_back(ratio_175to200);
  bin_contents.push_back(ratio_200to250);
  bin_contents.push_back(ratio_250to300);
  bin_contents.push_back(ratio_300to400);
  bin_contents.push_back(ratio_400to1000);

  return bin_contents;
}

void QCDfake_fakeratio_systematics_phopt()
{
  // Plot QCD sideband distributions
  for(int i = 1; i <= 5; i++){
    // plot_sideband_purity(i,0,35.9);
    plot_sideband_purity(i,35.9);
  }
  
  // Get QCD fake ratio values for use in postAnalyzers
  std::vector<Float_t> fraction_and_error;
  std::vector<Float_t> frac_175to200_sysbins;
  std::vector<Float_t> frac_error_175to200_sysbins;
  std::vector<Float_t> frac_200to250_sysbins;
  std::vector<Float_t> frac_error_200to250_sysbins;
  std::vector<Float_t> frac_250to300_sysbins;
  std::vector<Float_t> frac_error_250to300_sysbins;
  std::vector<Float_t> frac_300to400_sysbins;
  std::vector<Float_t> frac_error_300to400_sysbins;
  std::vector<Float_t> frac_400to1000_sysbins;
  std::vector<Float_t> frac_error_400to1000_sysbins;
  fraction_and_error.clear();
  frac_175to200_sysbins.clear();
  frac_error_175to200_sysbins.clear();
  frac_200to250_sysbins.clear();
  frac_error_200to250_sysbins.clear();
  frac_250to300_sysbins.clear();
  frac_error_250to300_sysbins.clear();
  frac_300to400_sysbins.clear();
  frac_error_300to400_sysbins.clear();
  frac_400to1000_sysbins.clear();
  frac_error_400to1000_sysbins.clear();
  for(int i = 1; i <= 5; i++){
    for(int j = 0; j <= 8; j++){
      cout<<"Now fitting bin="<<i<<", sysbin="<<j<<endl;
      fraction_and_error = run_template_fit(i,j,35.9);
      cout<<"Done fitting bin="<<i<<", sysbin="<<j<<endl;
      if(i == 1){
        frac_175to200_sysbins.push_back(fraction_and_error[0]);
        frac_error_175to200_sysbins.push_back(fraction_and_error[1]);
      }
      else if(i == 2){
        frac_200to250_sysbins.push_back(fraction_and_error[0]);
        frac_error_200to250_sysbins.push_back(fraction_and_error[1]);
      }
      else if(i == 3){
        frac_250to300_sysbins.push_back(fraction_and_error[0]);
        frac_error_250to300_sysbins.push_back(fraction_and_error[1]);
      }
      else if(i == 4){
        frac_300to400_sysbins.push_back(fraction_and_error[0]);
        frac_error_300to400_sysbins.push_back(fraction_and_error[1]);
      }
      else if(i == 5){
        frac_400to1000_sysbins.push_back(fraction_and_error[0]);
        frac_error_400to1000_sysbins.push_back(fraction_and_error[1]);
      }
    }
  }
  
  // Get systematic shifts
  TH1F *h_sysb[11];
  Float_t maxshifts[5] = {0.,0.,0.,0.,0.};
  Float_t minshifts[5] = {0.,0.,0.,0.,0.};
  Float_t PtBins[6] = {175.,200.,250.,300.,400.,1000};
  std::vector<Float_t> ratio_bin_contents_standard = QCDfake_getSysbContents_phopt(0,frac_175to200_sysbins[0],frac_200to250_sysbins[0],frac_250to300_sysbins[0],frac_300to400_sysbins[0],frac_400to1000_sysbins[0],frac_error_175to200_sysbins[0],frac_error_200to250_sysbins[0],frac_error_250to300_sysbins[0],frac_error_300to400_sysbins[0],frac_error_400to1000_sysbins[0]);
  
  std::vector<Float_t> bin1;
  std::vector<Float_t> bin2;
  std::vector<Float_t> bin3;
  std::vector<Float_t> bin4;
  std::vector<Float_t> bin5;
  bin1.clear();
  bin2.clear();
  bin3.clear();
  bin4.clear();
  bin5.clear();
  //Systematics bins:
  //    0       1      2       3       4       5       6         7          8           9            10
  // Standard  sbUP  sbDown  metUP  metDown  binUP  binDown  sieieLeft  sieieRight  templateUp  templateDown
  for(int j = 0; j <= 10; j++) // j = systematics index
  {
    // Define each systematics histo
    char ptbins[100];
    sprintf(ptbins,"_%d",j);
    std::string histname(ptbins);
    h_sysb[j] = new TH1F(("h_sysb"+histname).c_str(),"",5,PtBins);h_sysb[j]->Sumw2();

    // Evaluate and store systematic shifts
    // For sysbins 9 and 10, give the central values + errors, as the shifted values will be computed in the function
    int jj = j;
    if (j > 8)
      jj = 0;
    std::vector<Float_t> ratio_bin_contents_shifted = QCDfake_getSysbContents_phopt(j,frac_175to200_sysbins[jj],frac_200to250_sysbins[jj],frac_250to300_sysbins[jj],frac_300to400_sysbins[jj],frac_400to1000_sysbins[jj],frac_error_175to200_sysbins[jj],frac_error_200to250_sysbins[jj],frac_error_250to300_sysbins[jj],frac_error_300to400_sysbins[jj],frac_error_400to1000_sysbins[jj]);
    bin1.push_back(ratio_bin_contents_shifted[0]);
    bin2.push_back(ratio_bin_contents_shifted[1]);
    bin3.push_back(ratio_bin_contents_shifted[2]);
    bin4.push_back(ratio_bin_contents_shifted[3]);
    bin5.push_back(ratio_bin_contents_shifted[4]);
    
    for(int k = 1; k <= 5; k++) // k = bin number (within a systematics histo)
    {
      // Set bin values of systematic shift histos
      Float_t variation_over_nominal = ratio_bin_contents_shifted[k-1]/ratio_bin_contents_standard[k-1];
      h_sysb[j]->SetBinContent(k,variation_over_nominal);
      // Find max and min shifts
      Float_t percentage_shift = (ratio_bin_contents_shifted[k-1]-ratio_bin_contents_standard[k-1])/ratio_bin_contents_standard[k-1];
      if(percentage_shift > maxshifts[k-1])
        maxshifts[k-1] = percentage_shift;
      if(percentage_shift < minshifts[k-1])
        minshifts[k-1] = percentage_shift;
    }
    h_sysb[j]->SetStats(0);
    h_sysb[j]->SetTitle("");
    h_sysb[j]->GetXaxis()->SetTitle("");
    h_sysb[j]->GetYaxis()->SetTitle("");
    h_sysb[j]->SetLineWidth(3);
  }
  
  // Draw fake ratio plot, with vertical errors = envelope of shifts
  TCanvas *c = new TCanvas("c", "canvas",700,640);
  gStyle->SetOptStat(0);
  // gStyle->SetLegendBorderSize(0);
  c->SetLeftMargin(0.15);
  c->cd();
  
  TGraphAsymmErrors *fakeratio_vs_phoPt = new TGraphAsymmErrors();
  // std::vector<Float_t> comparison_spring16_bincontents;
  // comparison_spring16_bincontents.clear();
  // std::vector<Float_t> comparison_spring16_yerrorsUp;
  // comparison_spring16_yerrorsUp.clear();
  // std::vector<Float_t> comparison_spring16_yerrorsDown;
  // comparison_spring16_yerrorsDown.clear();
  // TGraphAsymmErrors *comparison_spring15 = new TGraphAsymmErrors();
  // TGraphAsymmErrors *comparison_spring16 = new TGraphAsymmErrors();
  // Float_t comparison_spring15_bincontents[5] = {0.0373351,0.0315396,0.0288305,0.0285249,0.0331253};
  // Float_t comparison_spring15_yerrorsUp[5] = {0.00143408,0.000926085,0.00123564,0.0018032,0.00742877};
  // Float_t comparison_spring15_yerrorsDown[5] = {0.000711638,0.000931054,0.00065762,0.00102958,0.00311613};
  for(int k = 0; k < 5; k++)
  {
    Float_t meanPhoPt = (PtBins[k+1]+PtBins[k])/2.0;
    fakeratio_vs_phoPt->SetPoint(k,meanPhoPt,ratio_bin_contents_standard[k]);
    Float_t xerror = (PtBins[k+1]-PtBins[k])/2.0;
    fakeratio_vs_phoPt->SetPointError(k,xerror,xerror,fabs(minshifts[k])*ratio_bin_contents_standard[k],maxshifts[k]*ratio_bin_contents_standard[k]);
    
    // comparison_spring16_bincontents.push_back(ratio_bin_contents_standard[k]);
    // comparison_spring16_yerrorsUp.push_back(maxshifts[k]*ratio_bin_contents_standard[k]);
    // comparison_spring16_yerrorsDown.push_back(fabs(minshifts[k])*ratio_bin_contents_standard[k]);
    // comparison_spring15->SetPoint(k,meanPhoPt,comparison_spring15_bincontents[k]);
    // comparison_spring15->SetPointError(k,xerror,xerror,comparison_spring15_yerrorsUp[k],comparison_spring15_yerrorsDown[k]);
    // comparison_spring16->SetPoint(k,meanPhoPt,comparison_spring16_bincontents[k]);
    // comparison_spring16->SetPointError(k,xerror,xerror,comparison_spring16_yerrorsUp[k],comparison_spring16_yerrorsDown[k]);
  }
  
  // cout<<"Float_t comparison_spring16_bincontents[5] = {";
  // for(int k = 0; k < 5; k++){
  //   if(k > 0)
  //     cout<<",";
  //   cout<<comparison_spring16_bincontents[k];
  // }
  // cout<<"};"<<endl;
  // cout<<"Float_t comparison_spring16_yerrorsUp[5] = {";
  // for(int k = 0; k < 5; k++){
  //   if(k > 0)
  //     cout<<",";
  //   cout<<comparison_spring16_yerrorsUp[k];
  // }
  // cout<<"};"<<endl;
  // cout<<"Float_t comparison_spring16_yerrorsDown[5] = {";
  // for(int k = 0; k < 5; k++){
  //   if(k > 0)
  //     cout<<",";
  //   cout<<comparison_spring16_yerrorsDown[k];
  // }
  // cout<<"};"<<endl;
  
  fakeratio_vs_phoPt->SetLineColor(kBlue);
  fakeratio_vs_phoPt->SetMarkerColor(kBlack);
  fakeratio_vs_phoPt->SetMarkerStyle(kFullCircle);
  fakeratio_vs_phoPt->SetLineWidth(3);
  // fakeratio_vs_phoPt->SetMarkerSize(2);
  fakeratio_vs_phoPt->GetXaxis()->SetTitle("");
  fakeratio_vs_phoPt->GetXaxis()->SetTickLength(0);
  fakeratio_vs_phoPt->GetXaxis()->SetLabelOffset(999);
  fakeratio_vs_phoPt->GetYaxis()->SetTitle("");
  fakeratio_vs_phoPt->GetYaxis()->SetTickLength(0);
  fakeratio_vs_phoPt->GetYaxis()->SetLabelOffset(999);
  fakeratio_vs_phoPt->SetTitle("");
  fakeratio_vs_phoPt->GetXaxis()->SetRangeUser(175.0,1000.0);
  // fakeratio_vs_phoPt->GetYaxis()->SetRangeUser(0.0,0.055);
  fakeratio_vs_phoPt->GetYaxis()->SetRangeUser(0.0,0.2);
  fakeratio_vs_phoPt->Draw("AP");
  
  // comparison_spring15->SetLineColor(kRed);
  // comparison_spring15->SetMarkerColor(kBlack);
  // comparison_spring15->SetMarkerStyle(kFullCircle);
  // comparison_spring15->SetLineWidth(3);
  // // comparison_spring15->SetMarkerSize(2);
  // comparison_spring15->GetXaxis()->SetTitle("");
  // comparison_spring15->GetXaxis()->SetTickLength(0);
  // comparison_spring15->GetXaxis()->SetLabelOffset(999);
  // comparison_spring15->GetYaxis()->SetTitle("");
  // comparison_spring15->GetYaxis()->SetTickLength(0);
  // comparison_spring15->GetYaxis()->SetLabelOffset(999);
  // comparison_spring15->SetTitle("");
  // comparison_spring15->GetXaxis()->SetRangeUser(175.0,1000.0);
  // comparison_spring15->GetYaxis()->SetRangeUser(0.0,0.055);
  // comparison_spring15->Draw("P SAME");
  
  // comparison_spring16->SetLineColor(kRed);
  // comparison_spring16->SetMarkerColor(kBlack);
  // comparison_spring16->SetMarkerStyle(kFullCircle);
  // comparison_spring16->SetLineWidth(3);
  // // comparison_spring16->SetMarkerSize(2);
  // comparison_spring16->GetXaxis()->SetTitle("");
  // comparison_spring16->GetXaxis()->SetTickLength(0);
  // comparison_spring16->GetXaxis()->SetLabelOffset(999);
  // comparison_spring16->GetYaxis()->SetTitle("");
  // comparison_spring16->GetYaxis()->SetTickLength(0);
  // comparison_spring16->GetYaxis()->SetLabelOffset(999);
  // comparison_spring16->SetTitle("");
  // comparison_spring16->GetXaxis()->SetRangeUser(175.0,1000.0);
  // comparison_spring16->GetYaxis()->SetRangeUser(0.0,0.055);
  // comparison_spring16->Draw("P SAME");
  
  TLatex *texS = new TLatex(0.61023,0.907173,"35.9 fb^{-1} (13 TeV)");
  texS->SetNDC();
  texS->SetTextFont(42);
  texS->SetTextSize(0.045);
  texS->Draw();
  TLatex *texS1 = new TLatex(0.18092,0.837173,"#bf{CMS} #it{Preliminary}");
  texS1->SetNDC();
  texS1->SetTextFont(42);
  texS1->SetTextSize(0.045);
  texS1->Draw();

  TLegend *leg = new TLegend(0.181948,0.182496,0.565903,0.288532,"");
  leg->AddEntry(fakeratio_vs_phoPt,"Corrected Num./Den.","P E L");
  // leg->AddEntry(comparison_spring15,"Spring15","P E L");
  // leg->AddEntry(comparison_spring16,"Spring16","P E L");
  // leg->AddEntry(fakeratio_vs_phoPt,"Ashim Re-tuned","P E L");
  leg->SetFillStyle(0);
  leg->Draw();

  c->Update();

  double xmin = c->GetUxmin();
  double ymin = c->GetUymin();
  double xmax = c->GetUxmax();
  double ymax = c->GetUymax();

  TGaxis *xaxis = new TGaxis(xmin,ymin,xmax,ymin,xmin,xmax,510);
  xaxis->SetTitle("Photon #it{p}_{T} [GeV]");
  xaxis->SetLabelFont(42);
  xaxis->SetLabelSize(0.040);
  xaxis->SetTitleFont(42);
  xaxis->SetTitleSize(0.045);
  xaxis->Draw("SAME");
  TGaxis *xaxis_top = new TGaxis(xmin,ymax,xmax,ymax,xmin,xmax,510,"-");
  xaxis_top->SetTitle("");
  xaxis_top->SetLabelOffset(999);
  xaxis_top->Draw("SAME");
  TGaxis *yaxis = new TGaxis(xmin,ymin,xmin,ymax,ymin,ymax,510);
  yaxis->SetTitle("QCD fake ratio");
  yaxis->SetLabelFont(42);
  yaxis->SetLabelSize(0.040);
  yaxis->SetTitleFont(42);
  yaxis->SetTitleSize(0.045);
  yaxis->SetTitleOffset(1.5);
  yaxis->Draw("SAME");
  TGaxis *yaxis_right = new TGaxis(xmax,ymin,xmax,ymax,ymin,ymax,510,"+");
  yaxis_right->SetTitle("");
  yaxis_right->SetLabelOffset(999);
  yaxis_right->Draw("SAME");
  
  c->SaveAs("QCDfake_fakeratio_phoPt.pdf");
  c->SaveAs("QCDfake_fakeratio_phoPt.png");
  
  // Draw systematic shifts
  c = new TCanvas("c", "canvas",700,640);
  gStyle->SetOptStat(0);
  c->SetLeftMargin(0.15);
  c->SetBottomMargin(0.15);
  c->cd();

  Int_t ci1  = 3001;
  Int_t ci2  = 3002;
  Int_t ci3  = 3003;
  Int_t ci4  = 3004;
  Int_t ci5  = 3005;
  Int_t ci6  = 3006;
  Int_t ci7  = 3007;
  Int_t ci8  = 3008;
  Int_t ci9  = 3009;
  Int_t ci10  = 3010;
  TColor *color1  = new TColor( ci1,178./255,223./255,138./255);
  TColor *color2  = new TColor( ci2, 51./255,160./255, 44./255);
  TColor *color3  = new TColor( ci3,251./255,154./255,153./255);
  TColor *color4  = new TColor( ci4,227./255, 26./255, 28./255);
  TColor *color5  = new TColor( ci5,166./255,206./255,227./255);
  TColor *color6  = new TColor( ci6, 31./255,120./255,180./255);
  TColor *color7  = new TColor( ci7,200./255, 33./255,209./255);
  TColor *color8  = new TColor( ci8,204./255,153./255,207./255);
  TColor *color9  = new TColor( ci9,253./255,191./255,111./255);
  TColor *color10 = new TColor(ci10,255./255,127./255,  0./255);
  h_sysb[0]->SetLineColor(kBlack);
  h_sysb[1]->SetLineColor(ci1);
  h_sysb[2]->SetLineColor(ci2);
  h_sysb[3]->SetLineColor(ci3);
  h_sysb[4]->SetLineColor(ci4);
  h_sysb[5]->SetLineColor(ci5);
  h_sysb[6]->SetLineColor(ci6);
  h_sysb[7]->SetLineColor(ci7);
  h_sysb[8]->SetLineColor(ci8);
  h_sysb[9]->SetLineColor(ci9);
  h_sysb[10]->SetLineColor(ci10);
  
  TH1F *hs = c->DrawFrame(175.,0.8,1000.,1.4,"");
  hs->SetXTitle("Photon #it{p}_{T} [GeV]");
  hs->SetYTitle("Fake ratio variation / nominal");
  hs->GetXaxis()->SetLabelSize(0.045);
  hs->GetXaxis()->SetTitleSize(0.050);
  hs->GetXaxis()->SetTitleOffset(1.1);
  hs->GetYaxis()->SetLabelSize(0.045);
  hs->GetYaxis()->SetTitleSize(0.050);
  hs->GetYaxis()->SetTitleOffset(1.3);

  h_sysb[0]->Draw("hist same");
  h_sysb[1]->Draw("hist same");
  h_sysb[2]->Draw("hist same");
  h_sysb[3]->Draw("hist same");
  h_sysb[4]->Draw("hist same");
  h_sysb[5]->Draw("hist same");
  h_sysb[6]->Draw("hist same");
  h_sysb[7]->Draw("hist same");
  h_sysb[8]->Draw("hist same");
  h_sysb[9]->Draw("hist same");
  h_sysb[10]->Draw("hist same");

  texS = new TLatex(0.61023,0.907173,"35.9 fb^{-1} (13 TeV)");
  texS->SetNDC();
  texS->SetTextFont(42);
  texS->SetTextSize(0.045);
  texS->Draw("same");
  texS1 = new TLatex(0.18092,0.837173,"#bf{CMS} #it{Preliminary}");
  texS1->SetNDC();
  texS1->SetTextFont(42);
  texS1->SetTextSize(0.045);
  texS1->Draw("same");

  leg = new TLegend(0.5,0.580326,0.7851,0.880914 );
  leg->SetFillColor(kWhite);
  leg->SetTextSize(0.030);
  leg->AddEntry( h_sysb[0],"No shift", "L");
  leg->AddEntry( h_sysb[1],"Sideband up", "L");
  leg->AddEntry( h_sysb[2],"Sideband down", "L");
  leg->AddEntry( h_sysb[3],"MET up", "L");
  leg->AddEntry( h_sysb[4],"MET down", "L");
  leg->AddEntry( h_sysb[5],"Binning up", "L");
  leg->AddEntry( h_sysb[6],"Binning down","L");
  leg->AddEntry( h_sysb[8],"#sigmai#etai#eta right", "L");
  leg->AddEntry( h_sysb[7],"#sigmai#etai#eta left", "L");
  leg->AddEntry( h_sysb[9],"Template up","L");
  leg->AddEntry( h_sysb[10],"Template down","L");
  leg->Draw("same");

  c->SaveAs("QCDfake_systematics_phoPt.pdf");
  c->SaveAs("QCDfake_systematics_phoPt.png");
  
  
  // Print out final QCD fake ratio values for use in postAnalyzers
  cout<<"  Float_t bin1[11] = {";
  for(int i = 0; i < bin1.size(); i++){
    if(i > 0)
      cout<<",";
    cout<<bin1[i];
  }
  cout<<"};"<<endl;
  cout<<"  Float_t bin2[11] = {";
  for(int i = 0; i < bin2.size(); i++){
    if(i > 0)
      cout<<",";
    cout<<bin2[i];
  }
  cout<<"};"<<endl;
  cout<<"  Float_t bin3[11] = {";
  for(int i = 0; i < bin3.size(); i++){
    if(i > 0)
      cout<<",";
    cout<<bin3[i];
  }
  cout<<"};"<<endl;
  cout<<"  Float_t bin4[11] = {";
  for(int i = 0; i < bin4.size(); i++){
    if(i > 0)
      cout<<",";
    cout<<bin4[i];
  }
  cout<<"};"<<endl;
  cout<<"  Float_t bin5[11] = {";
  for(int i = 0; i < bin5.size(); i++){
    if(i > 0)
      cout<<",";
    cout<<bin5[i];
  }
  cout<<"};"<<endl;
}

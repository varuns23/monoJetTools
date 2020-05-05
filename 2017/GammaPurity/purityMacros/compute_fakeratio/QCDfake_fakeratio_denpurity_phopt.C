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
void plot_den_purity(int i, float full_lumi){
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

  // TString histoname_den_QCD = TString("h_sieie_den_QCD_"+event_bin+"_"+sys_bin);
  TString histoname_den = TString("h_sieie_den_"+event_bin+"_0");
  TH1D *sieie_den_GJets_40to100 = (TH1D*)GJets_40to100->Get(histoname_den);
  TH1D *sieie_den_GJets_100to200 = (TH1D*)GJets_100to200->Get(histoname_den);
  TH1D *sieie_den_GJets_200to400 = (TH1D*)GJets_200to400->Get(histoname_den);
  TH1D *sieie_den_GJets_400to600 = (TH1D*)GJets_400to600->Get(histoname_den);
  TH1D *sieie_den_GJets_600toInf = (TH1D*)GJets_600toInf->Get(histoname_den);
  TH1D *sieie_den_data = (TH1D*)f_data->Get(histoname_den);
  
  sieie_den_GJets_40to100->Scale(20730.0*full_lumi*1000.0/4269126.0);
  sieie_den_GJets_100to200->Scale(9226.0*full_lumi*1000.0/5131808.0);
  sieie_den_GJets_200to400->Scale(2300.0*full_lumi*1000.0/10036339.0);
  sieie_den_GJets_400to600->Scale(277.4*full_lumi*1000.0/2435892.0);
  sieie_den_GJets_600toInf->Scale(93.38*full_lumi*1000.0/2117687.0);
  sieie_den_GJets_40to100->Add(sieie_den_GJets_100to200);
  sieie_den_GJets_40to100->Add(sieie_den_GJets_200to400);
  sieie_den_GJets_40to100->Add(sieie_den_GJets_400to600);
  sieie_den_GJets_40to100->Add(sieie_den_GJets_600toInf);
  
  TCanvas *c = new TCanvas("c", "canvas",700,640);
  gStyle->SetOptStat(0);
  gStyle->SetLegendBorderSize(0);
  c->SetLeftMargin(0.15);
  c->cd();
  
  sieie_den_GJets_40to100->GetXaxis()->SetTitle("#sigma_{i#etai#eta}");
  sieie_den_GJets_40to100->GetYaxis()->SetTitle("Yield for 35.9 fb^{-1}");
  sieie_den_GJets_40to100->GetYaxis()->SetTitleOffset(1.8);
  sieie_den_GJets_40to100->SetTitle("");
  sieie_den_GJets_40to100->SetStats(0);
  sieie_den_GJets_40to100->SetLineWidth(2);
  sieie_den_GJets_40to100->SetLineColor(kRed);
  sieie_den_data->GetXaxis()->SetTitle("#sigma_{i#etai#eta}");
  sieie_den_data->GetYaxis()->SetTitle("Yield for 35.9 fb^{-1}");
  sieie_den_data->GetYaxis()->SetTitleOffset(1.8);
  sieie_den_data->SetTitle("");
  sieie_den_data->SetStats(0);
  sieie_den_data->SetLineWidth(2);
  sieie_den_data->SetLineColor(kBlack);
  sieie_den_data->SetMarkerStyle(8);
  
  sieie_den_data->GetXaxis()->SetLabelFont(42);
  sieie_den_data->GetXaxis()->SetLabelSize(0.040);
  sieie_den_data->GetXaxis()->SetTitleFont(42);
  sieie_den_data->GetXaxis()->SetTitleSize(0.045);
  sieie_den_data->GetYaxis()->SetLabelFont(42);
  sieie_den_data->GetYaxis()->SetLabelSize(0.040);
  sieie_den_data->GetYaxis()->SetTitleFont(42);
  sieie_den_data->GetYaxis()->SetTitleSize(0.045);
  
  TH1F* h_sieie_remainder = (TH1F*)sieie_den_data->Clone("h_sieie_remainder");
  h_sieie_remainder->Add(sieie_den_GJets_40to100,-1.0); // Subtract sieie_den_GJets_40to100 from sieie_den_data
  h_sieie_remainder->SetLineColor(kGreen);
  
  for(int i = 1; i <= h_sieie_remainder->GetNbinsX(); i++){
    if(h_sieie_remainder->GetBinContent(i) < 9.0e-7)
      h_sieie_remainder->SetBinContent(i,9.0e-7);
    if(sieie_den_GJets_40to100->GetBinContent(i) < 1.0e-7)
      sieie_den_GJets_40to100->SetBinContent(i,1.0e-7);
    if(sieie_den_data->GetBinContent(i) < 1.0e-6)
      sieie_den_data->SetBinContent(i,1.0e-6);
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
  // sieie_den_GJets_40to100->GetYaxis()->SetRangeUser(0.0,upper_limit);
  sieie_den_data->Draw("0 E1");
  sieie_den_GJets_40to100->Draw("HIST SAME");
  h_sieie_remainder->Draw("HIST SAME");
  
  TString legTitle = TString(lowPt+" < Photon #it{p}_{T} < "+highPt);
  TLegend *leg = new TLegend(0.508711,0.672104,0.892665,0.872757,legTitle);
  leg->AddEntry(sieie_den_data,"Data","P E1");
  leg->AddEntry(sieie_den_GJets_40to100,"Gen-matched #gamma+jets MC","L");
  leg->AddEntry(h_sieie_remainder,"Data - #gamma+jets","L");
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
  
  TString saveas_name = TString("_templates_"+lowPt+"to"+highPt+".png");
  c->SaveAs(saveas_name);
  saveas_name = TString("_templates_"+lowPt+"to"+highPt+".pdf");
  c->SaveAs(saveas_name);
}

void QCDfake_fakeratio_denpurity_phopt()
{
  // Plot QCD sideband distributions
  for(int i = 1; i <= 5; i++){
    // plot_sideband_purity(i,0,35.9);
    plot_den_purity(i,35.9);
  }
  
}

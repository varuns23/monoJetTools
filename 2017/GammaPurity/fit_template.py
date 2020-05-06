#!/usr/bin/env python
from ROOT import *
from argparse import ArgumentParser

parser = ArgumentParser()
parser.add_argument("-t","--template",help="Template file to fit",type=TFile,required=True)

varmap = {
    "photonPFIso":RooRealVar("photonPFIso","Photon PF Isolation",0,25)
}
def make_pdf(histo,roovar,varnum,label):
    template = RooDataHist("%s_template"%label,"%s template"%label,RooArgList(roovar),histo)
    pdf = RooHistPdf("%s_pdf"%label,"%s pdf"%label,RooArgSet(roovar),template)
    extpdf = RooExtendPdf(label,"%s extend pdf"%label,pdf,varnum,"full range")
    return template,pdf,extpdf
def fit_template(template,roovar=varmap["photonPFIso"]):
    h_data = template.Get("data")
    h_gjet = template.Get("signal_gjets")
    h_qcd = template.Get("fake_qcd")

    # Variables that will contain real and fake estimates: can vary between 0 and ndata
    ndata = h_data.Integral()
    realnum = RooRealVar("realnum","realnum",0,ndata)
    fakenum = RooRealVar("fakenum","fakenum",0,ndata)

    fake_template,fake_pdf,fake_extpdf = make_pdf(h_qcd,roovar,fakenum,"fake")
    real_template,real_pdf,real_extpdf = make_pdf(h_gjet,roovar,realnum,"real")

    data = RooDataHist("data","data used in fitting",RooArgList(roovar),h_data)

    model = RooAddPdf("model","real photons + QCD fakes",RooArgList(real_extpdf,fake_extpdf))
    model.fitTo(data,RooFit.Minos())

if __name__ == "__main__":
    args = parser.parse_args()
    fit_template(args.template)

# Gamma Purity for Mono-Jet

Extension for monoJetTools for running and calculating the photon purity
Main histograms and photon pt binning are defined in ../../inc/monoJetGammaPurity.h
The current photon pt binning is as follows:
```
static const int nPhoPtBins = 8;                
const float phoPtBins[nPhoPtBins+1] = {230., 250., 280., 320., 375., 425., 475., 550., 2000.};
string PtBinNames[nPhoPtBins];
```	
To change the binning, simply modify the values in the float array and change the nPhoPtBins to work with new array.
PtBinNames will be created the first time BookHistos is called in the analysis code, which will define the string tags that are used when defining histogram names
If a new histogram is to be added to the different photon pt binnings, follow the format of the *h_phoPt_ptbins[maxHisto][nPhoPtBins].
Then in the source file (../../src/monoJetGammaPurity.C) initialize the new histogram along the other pt binned histograms in monoJetGammaPurity::BookHistos, and fill it with the others in monoJetGammaPurity::fillHistos

The photon ID is split into the CutBasedPhotonIso and CutBasedPhotonId (no isolation requirement), both of which have a second version: CutBasedPhotonIso_noPhoIso (isolation without the photon PF Iso) and CutBasedPhotonIso_noSieie (id without the sieie cut). By using a combination of these, most of the photon IDs required for signal, background, and denominator templates can be constructed.


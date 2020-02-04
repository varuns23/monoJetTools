#ifndef VariableBins_h
#define VariableBins_h

const int nMtBins = 50;
const float MtBins[nMtBins+1]={180.,200.,220.,240.,260.,280.,300.,320.,340.,360.,380.,400.,420.,440.,460.,480.,500.,520.,540.,560.,580.,600.,620.,640.,660.,680.,
			       700.,720.,740.,760.,780.,800.,820.,840.,860.,880.,900.,920.,940.,960.,980.,1000.,1050.,1100.,1200.,1300.,1400.,1500.,2000.,2500.,3000.};

const int nMetBins = 22;
const float MetBins[nMetBins+1]={250.,  280.,  310.,  340.,  370.,  400.,  430.,  470.,  510., 550.,  590.,  640.,  690.,
				 740.,  790.,  840.,  900.,  960., 1020., 1090., 1160., 1250., 1400.};

const int nPtBins = 29;
const float PtBins[nPtBins+1]={100.,120.,140.,160.,180.,200.,225.,250.,  280.,  310.,  340.,  370.,  400.,  430.,  470.,  510., 550.,  590.,
			       640.,690.,740.,  790.,  840.,  900.,  960., 1020., 1090., 1160., 1250., 1400.};

const int nBosonPtBins = 24;
const float BosonPtBins[nBosonPtBins+1] = {150,170,200,230,260,290,320,350,390,430,470,510,550,590,640,690,740,790,840,900,960,1020,1090,1160,1250};

const int nLeadingLeptonPtBins = 24;
const float LeadingLeptonPtBins[25+1] = {20.,40.,60.,80.,100.,120.,140.,160.,180.,200.,250.,300.,350.,400.,500.,600.,700.,800.,900.,1000.,1100.,1200.,1300.,1400.,1500.};

const int nSubLeadingLeptonPtBins = 25;
const float subLeadingLeptonPtBins[nSubLeadingLeptonPtBins+1] = {10.,20.,40.,60.,80.,100.,120.,140.,160.,180.,200.,250.,300.,350.,
								 400.,500.,600.,700.,800.,900.,1000.,1100.,1200.,1300.,1400.,1500.};

const int nEtaBins = 30;  const float lEta = -3.0;    const float uEta = 3.0;
const int nPhiBins = 30;  const float lPhi = -3.1416; const float uPhi = 3.1416;
const int nCandBins = 50; const float lCand = 0;      const float uCand = 50;

#endif

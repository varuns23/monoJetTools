#define monoJetCommon_cxx
#include "monoJetCommon.h"

void monoJetCommon::PSWeights(double event_weight) {
  /* 44 Histograms
     up  dn
     isrRed        0   2
     fsrRed        1   3
     isrDef        4   6
     fsrDef        5   7
     isrCon        8  10 
     fsrCon        9  11
     fsr_G2GG_muR  12 13
     fsr_G2QQ_muR  14 15
     fsr_Q2QG_muR  16 17
     fsr_X2XG_muR  18 19
     fsr_G2GG_cNS  20 21
     fsr_G2QQ_cNS  22 23
     fsr_Q2QG_cNS  24 25
     fsr_X2XG_cNS  26 27 
     isr_G2GG_muR  28 29
     isr_G2QQ_muR  30 31
     isr_Q2QG_muR  32 33
     isr_X2XG_muR  34 35
     isr_G2GG_cNS  36 37
     isr_G2QQ_cNS  38 39
     isr_Q2QG_cNS  40 41
     isr_X2XG_cNS  42 43
  */
  
  string psw_uncs[22] = { "isrRed",   
			  "fsrRed",      
			  "isrDef",    
			  "fsrDef",  
			  "isrCon",
			  "fsrCon",      
			  "fsr_G2GG_muR",
			  "fsr_G2QQ_muR",
			  "fsr_Q2QG_muR",
			  "fsr_X2XG_muR",
			  "fsr_G2GG_cNS",
			  "fsr_G2QQ_cNS",
			  "fsr_Q2QG_cNS",
			  "fsr_X2XG_cNS",
			  "isr_G2GG_muR",
			  "isr_G2QQ_muR",
			  "isr_Q2QG_muR",
			  "isr_X2XG_muR",
			  "isr_G2GG_cNS",
			  "isr_G2QQ_cNS",
			  "isr_Q2QG_cNS",
			  "isr_X2XG_cNS"  };
  
  string uncname = "PSW_";
  // Initializing
  if ( !scaleUncs->contains(uncname+psw_uncs[0]) ) {
    for (int i = 0; i < 22; i++) {
      string name = uncname + psw_uncs[i];
      scaleUncs->addUnc(name,NULL);
    }
  }
  
  int nPS = 22;
  for (int i = 0; i < nPS; i++) {
    string name = uncname + psw_uncs[i];
    float weightUp = event_weight;
    float weightDn = event_weight;
    if ( !sample.isData && !sample.isSignal ) {
      if ( i < 2 ) {
	weightUp *= psWeight->at(2 + i);
	weightDn *= psWeight->at(2 + i + 2);
      } else if ( i < 4 ) {
	weightUp *= psWeight->at(2 + i + 2);
	weightDn *= psWeight->at(2 + i + 4);
      } else if ( i < 6 ) {
	weightUp *= psWeight->at(2 + i + 4);
	weightDn *= psWeight->at(2 + i + 6);
      } else {
	weightUp *= psWeight->at(2 + 2*i);
	weightDn *= psWeight->at(2 + 2*i + 1);
      }
    }
    scaleUncs->setUnc(name,weightUp,weightDn);
  }
}

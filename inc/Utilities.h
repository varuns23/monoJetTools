#ifndef Utilities_h
#define Utilities_h

#include <TH1F.h>
#include <iostream>

float deltaPhi(float phi1, float phi2);

float deltaR(float eta1, float phi1, float eta2, float phi2);

float getMt(float pt1,float phi1,float pt2,float phi2);

std::vector<std::string> split(std::string str,std::string delim);

bool fileSelection(std::string filename,std::string fileRange);

TH1F* MakeTH1F(TH1F* temp);

#endif

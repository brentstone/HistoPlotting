#ifndef HISTOPLOTTING_PLOTHELP_H
#define HISTOPLOTTING_PLOTHELP_H

#include <TH1.h>
#include <TH2.h>
#include <TFile.h>

using namespace std;
namespace PlotHelp {

  TH1 * getEff(TFile *f, TString sN, TString num, TString den, float rebin = 0, int nR = 0, double * rebins = 0);
  TH1 * getTotBkgDist(vector<pair<TString,TFile*>> bkgs, TString hist);
  TH1 * getEffHistByMX(TString num, TString den, TFile *f, vector<int> masses);

  void drawEffs(vector<pair<TString,TFile*>> samps, bool isSig, TString pre, vector<TString> sels, TString numS, TString denS,
  		TString canName="slurm", float rebin=0, int nR = 0, double * rebins = 0);

  TH2 * getSoBHist(pair<TString,TFile*> sSamp, vector<pair<TString,TFile*>> bSamps, int type,
  		vector<TString> wps);
}

#endif

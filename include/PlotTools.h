#ifndef HISTOPLOTTING_PLOTTOOLS_H
#define HISTOPLOTTING_PLOTTOOLS_H

#include <TH1.h>
#include <TH2.h>
#include <TGraphAsymmErrors.h>


namespace PlotTools {

  void toUnderflow(TH1 * h);
  void toOverflow (TH1 * h);
  void toUnderflowX(TH2 * h);
  void toOverflowX (TH2 * h);
  void normalize  (TH1 * h);
  TH1* rebin      (TH1* h, int n);
  TH1* rebin      (TH1* h, int n, double * bins);

  TGraphAsymmErrors* getBinomErrors(const TH1* num, const TH1* den);
  TGraphAsymmErrors* getPoissonErrors(const TH1* hist);
  TGraphAsymmErrors* getRatioPoissonErrors(const TH1* hnum, const TH1* hden);
  void getRatioPoissonErrors(int dN, double mN, double mE, double& eL, double& eH);
}

#endif

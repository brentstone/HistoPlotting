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
  TH2* rebinX     (TH2* h, int n);
  TH2* rebinY     (TH2* h, int n);
  TH2* rebinX     (TH2* h, int n, double * bins);
  TH2* rebinY     (TH2* h, int n, double * bins);
  TH2* rebin2D    (TH2* h, TString name, TString title, int nx, double * xbins, int ny, double * ybins);

  //returns the size of the (lower,upper) error bars
  std::pair<double,double> getBinomError(const double num, const double den);
  TGraphAsymmErrors* getBinomErrors(const TH1* num, const TH1* den);
  void getPoissonErrors(const unsigned int N, double& eU, double& eD);
  TGraphAsymmErrors* getPoissonErrors(const TH1* hist, const bool drawTrailingZeros);
  TGraphAsymmErrors* getRatioPoissonErrors(const TH1* hnum, const TH1* hden,
          const bool drawTrailingZeros);
  void getRatioPoissonErrors(int dN, double mN, double mE, double& eL, double& eH);

  TGraph* getRocCurve(const TH1* xHist, const TH1 * yHist, bool cutGreaterThan,
          TString xName="", TString yName ="");
  TH1* getIntegral(const TH1* hist, bool greaterThan, bool normalize = false);
}

#endif

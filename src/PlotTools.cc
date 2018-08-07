#include "../include/PlotTools.h"
#include "iostream"
#include <TEfficiency.h>
#include <TMath.h>
#include <TRandom3.h>
#include <Math/QuantFuncMathCore.h>
using namespace std;

namespace PlotTools {



  void toUnderflow(TH1 * h) {
      h->SetBinContent(1,h->GetBinContent(1) +h->GetBinContent(0) );
      h->SetBinContent(0,0);
      if(h->GetSumw2()->fN){
        (*h->GetSumw2())[1] += (*h->GetSumw2())[0];
        h->SetBinError(0,0);
      }
  }
  void toOverflow(TH1 * h) {
    int nBins = h->GetNbinsX();
      h->SetBinContent(nBins,h->GetBinContent(nBins) +h->GetBinContent(nBins+1) );
      h->SetBinContent(nBins+1,0);
      if(h->GetSumw2()->fN){
        (*h->GetSumw2())[nBins] += (*h->GetSumw2())[nBins +1];
        h->SetBinError(nBins+1,0);
      }

  }
  void toUnderflowX(TH2 * h) {
    for(int iBY = 0; iBY <= h->GetNbinsY()+1; ++iBY){
      int firstBin = h->GetBin(1,iBY);
      int underBin = h->GetBin(0,iBY);
      h->SetBinContent(firstBin,h->GetBinContent(firstBin)+h->GetBinContent(underBin));
      h->SetBinContent(underBin,0);
      if(h->GetSumw2()->fN){
        (*h->GetSumw2())[firstBin] += (*h->GetSumw2())[underBin];
        h->SetBinError(underBin,0);
      }
    }
  }
  void toOverflowX(TH2 * h) {
    int nBins = h->GetNbinsX();
    for(int iBY = 0; iBY <= h->GetNbinsY()+1; ++iBY){
      int lastBin = h->GetBin(nBins,iBY);
      int overBin = h->GetBin(nBins+1,iBY);
      h->SetBinContent(lastBin,h->GetBinContent(lastBin)+h->GetBinContent(overBin));
      h->SetBinContent(overBin,0);
      if(h->GetSumw2()->fN){
        (*h->GetSumw2())[lastBin] += (*h->GetSumw2())[overBin];
        h->SetBinError(overBin,0);
      }
    }
  }
  void normalize(TH1 * h) {
    if(h->GetSumw2()->fN == 0) h->Sumw2();
    int nBins = h->GetNbinsX();
    double norm = h->Integral(0,nBins+1);
    if(norm)
      h->Scale(1/norm);
  }
  TH1* rebin(TH1* h, int n){
    return h->Rebin(n);
  }
  TH1* rebin(TH1* h, int n, double * bins){
    return h->Rebin(n,"",bins);
  }

  std::pair<double,double> getBinomError(const double num, const double den){
	  const double eff = num/den;
	  if(den < 1.0 || eff < 0.0 || eff > 1.0 ) return std::make_pair(0,0);
	  return std::make_pair( eff - TEfficiency::ClopperPearson((unsigned int)den, (unsigned int)num, 0.683, false),
			  TEfficiency::ClopperPearson((unsigned int)den, (unsigned int)num, 0.683, true) - eff);
  }

  TGraphAsymmErrors* getBinomErrors(const TH1* num, const TH1* den){
    int npoints = den->GetNbinsX();
    float x[npoints], y[npoints], errx[npoints], erryl[npoints], erryh[npoints];
    float npass = 0.0;
    float ntotal = 0.0;

    const bool scaleErrors = num->GetSumw2()->GetSize() && den->GetSumw2()->GetSize();

    for(int ibin = 1; ibin < npoints+1; ibin++) {
      x[ibin-1] = den->GetBinCenter(ibin);
      npass = num->GetBinContent(ibin);
      ntotal = den->GetBinContent(ibin);

      //If it is a weighted histogram...correct for it
      if(scaleErrors && den->GetSumw2()->fArray[ibin]) {
          const float sf =  ntotal/den->GetSumw2()->fArray[ibin];
          npass *= sf;
          ntotal *= sf;
      }

      y[ibin-1] = ntotal < 1.0 ? 0.0 : npass/ntotal;
      errx[ibin-1] = den->GetBinWidth(ibin)/2;
      if(y[ibin-1]==0.0) {
        erryl[ibin-1] = 0.0; erryh[ibin-1] = 0.0;
      } else {
    	  auto errors = getBinomError(npass,ntotal);
        erryl[ibin-1] = errors.first;
        erryh[ibin-1] = errors.second;
      }
    }

    TGraphAsymmErrors *gr = new TGraphAsymmErrors(npoints, x, y, errx, errx, erryl, erryh);
    gr->SetTitle("");
    gr->GetXaxis()->SetTitle(num->GetXaxis()->GetTitle());
    gr->GetYaxis()->SetTitle("Efficiency");
    return gr;
  }
  
  void getPoissonErrors(const unsigned int N, double& eU, double& eD){
      const double alpha = 1 - 0.6827;
      double L =  (N == 0) ? 0  : (ROOT::Math::gamma_quantile(alpha/2,N,1.));
      double U =  ROOT::Math::gamma_quantile_c(alpha/2,N+1,1) ;
      eU =  U - double(N);
      eD =  double(N)- L;
  }

  TGraphAsymmErrors* getPoissonErrors(const TH1* h){
      TGraphAsymmErrors* gr = new TGraphAsymmErrors();
      int curPt = 0;
      for(int ibin = 1; ibin <= h->GetNbinsX(); ++ibin){
         int dN = h->GetBinContent(ibin);
         if(dN < 0) continue;
         double eU,eD;
         getPoissonErrors(dN,eU,eD);
         gr->SetPoint(curPt,h->GetBinCenter(ibin),dN);
         gr->SetPointEYhigh(curPt, eU);
         gr->SetPointEYlow(curPt, eD);
         if(dN == 0) {
           gr->SetPointEXlow(curPt, 0);
           gr->SetPointEXhigh(curPt, 0);
         }
         curPt++;
      }
    return gr;
  }

  TGraphAsymmErrors* getRatioPoissonErrors(const TH1* hD, const TH1* hM) {
      TGraphAsymmErrors* gr = new TGraphAsymmErrors();
      int curPt = 0;
      for(int ibin = 1; ibin <= hD->GetNbinsX(); ++ibin){
          int dN = hD->GetBinContent(ibin);
          if(dN < 0) continue;
          double mN = hM->GetBinContent(ibin);
          double mE = hM->GetBinError(ibin);
          double eL = 0, eH = 0;
          gr->SetPoint(curPt, hD->GetBinCenter(ibin), double(dN)/mN);
          getRatioPoissonErrors(dN, mN, mE, eL, eH);
          gr->SetPointEYhigh(curPt, eH);
          gr->SetPointEYlow(curPt, eL);
          curPt++;
      }
    return gr;
  }

  void getRatioPoissonErrors(int dN, double mN, double mE, double& eL, double& eH){
    if(mN <= 0) return;
    if(dN < 0)  return;
    eL = 0;
    eH = 0;

    if(mN <= 0) return;
    if(dN < 0)  return;

    const double alpha = 1 - 0.6827;
    static TRandom3 * rand = new TRandom3(1234);

    const int nEntries = 10000;
    vector<float> h;
    h.reserve(nEntries);
    vector<float> hL;
    hL.reserve(nEntries);

    for(unsigned int i = 0; i < nEntries; ++i){
      double ndL = 0;
      for(int iD = 0; iD < dN; ++iD){
        ndL -= TMath::Log(rand->Uniform());
      }
      double nd = ndL -  TMath::Log(rand->Uniform());
      double nm = rand->Gaus(mN,mE);
      if(nm < 0) nm = fabs(nm);
      h.push_back(nd/nm);
      hL.push_back(ndL/nm);
    }



    if(dN){
        std::sort(hL.begin(), hL.end());
        eL = hL[int( double(nEntries)*alpha/2  )];
        eL = double(dN)/mN - eL;
     }
    std::sort(h.begin(), h.end());
    eH = h[int( double(nEntries)* (1 - alpha/2)  )];
    eH = eH - double(dN)/mN;
  }


  TGraph* getRocCurve(const TH1* xHist, const TH1 * yHist, bool cutGreaterThan,TString xName, TString yName) {
	  const unsigned int nBins = xHist->GetNbinsX() ;
	  if(int(nBins) != yHist->GetNbinsX()) {
		  std:: cout <<" PlotTools::getRocCurve -> cannot make a ROC curve when the histograms have different number of bins!" <<std::endl;
		  return 0;
	  }

	  const double xHistInt = xHist->Integral(0,-1);
	  const double yHistInt = yHist->Integral(0,-1);

	  double xeff[nBins], yeff[nBins];

	  for(unsigned int iB =1; iB <= nBins; ++iB ){
		  const double xPass = cutGreaterThan ? xHist->Integral(iB,-1) : xHist->Integral(0,iB);
		  const double yPass = cutGreaterThan ? yHist->Integral(iB,-1) : yHist->Integral(0,iB);
		  xeff[iB -1] = xPass/xHistInt;
		  yeff[iB -1] = yPass/yHistInt;
	  }
	  auto * gr = new TGraph(nBins,xeff,yeff);;
	  gr->GetXaxis()->SetTitle(TString::Format("#varepsilon(%s %s X)",xName.Data(),cutGreaterThan ? ">" : "<"));
	  gr->GetYaxis()->SetTitle(TString::Format("#varepsilon(%s %s X)",yName.Data(),cutGreaterThan ? ">" : "<"));
	  gr->GetXaxis()->SetRangeUser(0,1);
	  gr->GetYaxis()->SetRangeUser(0,1);
	  return gr;
  }

  TH1* getIntegral(const TH1* hist, bool greaterThan, bool normalize) {
	  const unsigned int nBins = hist->GetNbinsX() ;
	  const double histInt = hist->Integral(0,-1);
	  TH1 * histI = (TH1*)hist->Clone();

	  for(unsigned int iB =1; iB <= nBins; ++iB ){
		  double pass=0;
		  if(iB == 1){
			  pass = greaterThan ? hist->Integral(0,-1) : hist->Integral(0,iB);
		  } else if(iB == nBins){
			  pass = greaterThan ? hist->Integral(iB,-1) : hist->Integral(0,-1);
		  } else{
			  pass = greaterThan ? hist->Integral(iB,-1) : hist->Integral(0,iB);
		  }
		  histI->SetBinContent(iB,normalize ? pass/histInt : pass );
		  histI->SetBinError(iB,0);
	  }
	  histI->SetBinContent(0,0);
	  histI->SetBinError(0,0);
	  histI->SetBinContent(nBins+1,0);
	  histI->SetBinError(nBins+1,0);
	  return histI;
  }


}

#include "../include/PlotTools.h"
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


  TGraphAsymmErrors* getBinomErrors(const TH1* num, const TH1* den){
    int npoints = den->GetNbinsX();
    float x[npoints], y[npoints], errx[npoints], erryl[npoints], erryh[npoints];
    float npass = 0.0;
    float ntotal = 0.0;
    for(int ibin = 1; ibin < npoints+1; ibin++) {
      x[ibin-1] = den->GetBinCenter(ibin);
      npass = num->GetBinContent(ibin);
      ntotal = den->GetBinContent(ibin);
      y[ibin-1] = ntotal < 1.0 ? 0.0 : npass/ntotal;
      errx[ibin-1] = den->GetBinWidth(ibin)/2;
      if(y[ibin-1]==0.0) {
        erryl[ibin-1] = 0.0; erryh[ibin-1] = 0.0;
      } else {
        erryl[ibin-1] = y[ibin-1] - TEfficiency::ClopperPearson((unsigned int)ntotal, (unsigned int)npass, 0.683, false);
        erryh[ibin-1] = TEfficiency::ClopperPearson((unsigned int)ntotal, (unsigned int)npass, 0.683, true) - y[ibin-1];
      }
    }

    TGraphAsymmErrors *gr = new TGraphAsymmErrors(npoints, x, y, errx, errx, erryl, erryh);
    gr->SetTitle("");
    gr->GetXaxis()->SetTitle(num->GetXaxis()->GetTitle());
    gr->GetYaxis()->SetTitle("Efficiency");
    return gr;
  }
  
  TGraphAsymmErrors* getPoissonErrors(const TH1* h){
    TGraphAsymmErrors* gr = new TGraphAsymmErrors(h);
    const double alpha = 1 - 0.6827;
    for(int ibin = 0; ibin < gr->GetN(); ++ibin) {
      int dN = gr->GetY()[ibin];
      double L =  (dN == 0) ? 0  : (ROOT::Math::gamma_quantile(alpha/2,dN,1.));
      double U =  ROOT::Math::gamma_quantile_c(alpha/2,dN+1,1) ;
      gr->SetPointEYhigh(ibin, U - double(dN));
      gr->SetPointEYlow(ibin, double(dN)- L);
      if(dN == 0) {
        gr->SetPointEXlow(ibin, 0);
        gr->SetPointEXhigh(ibin, 0);
      }
    }
    return gr;
  }

  TGraphAsymmErrors* getRatioPoissonErrors(const TH1* hD, const TH1* hM) {

    TGraphAsymmErrors* gr = new TGraphAsymmErrors(hD);
    for(int ibin = 0; ibin < gr->GetN(); ++ibin) {
      int dN = gr->GetY()[ibin];
      double mN = hM->GetBinContent(ibin+1);
      double mE = hM->GetBinError(ibin+1);
      double eL = 0, eH = 0;
      gr->SetPoint(ibin, gr->GetX()[ibin], double(dN)/mN);
      getRatioPoissonErrors(dN, mN, mE, eL, eH);
      gr->SetPointEYhigh(ibin, eH);
      gr->SetPointEYlow(ibin, eL);
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

}

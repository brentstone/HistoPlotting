

#ifndef HISTOPLOTTING_PLOTTER_H
#define HISTOPLOTTING_PLOTTER_H

#include <vector>
#include <iostream>

#include "TH1.h"
#include "TGraph.h"
#include "TLegend.h"
#include "TAxis.h"
#include "TCanvas.h"

#include "../include/Drawing.h"


class Plotter{
public:
  enum ErrorType {GAUS,POISSON, BINOM};
  Plotter() : totStack(0){};

  TH1* addHist(const TH1 * hist, TString title, int lineColor = -1, int lineStyle = 1, int lineWidth = 3, int markerStyle = 20, int markerSize = 1,  bool drawMarker = true, bool drawErrorBars = true,bool poissonErrors = false, TString drawOption = "");
  TH1* addHistLine(const TH1 * hist, TString title, int lineColor = -1, int lineStyle = 1, int lineWidth = 3){return addHist(hist,title,lineColor,lineStyle,lineWidth,20,1,false,false);}
  TH1* addStackHist(const TH1 * hist, TString title, int fillColor =-1, int fillStyle =1001, int lineColor = 1, int lineWidth =2);


  TCanvas * draw(bool save=false, TString printName = "plot.pdf");
  //If denIDX < 0, use the stack
//  TCanvas * drawRatio(int denIDX = 0, TString stackTitle = "stack", ErrorType errType=GAUS, bool save=false, TString printName = "plot.pdf");
  TCanvas * drawRatio(int denIDX = 0, TString stackTitle = "stack", bool doBinomErrors=false, bool save=false, TString printName = "plot.pdf");
  TCanvas * drawSplitRatio(int denIDX = 0, TString stackTitle = "stack", bool doBinomErrors=false, bool save=false, TString printName = "plot.pdf");

  //Drawing options
  void setUnderflow(bool doIt) {doUnderflow = doIt;}
  void setOverflow(bool doIt) {doOverflow = doIt;}
  void scale(double scale);
  void normalize();
  void rebin(int n);
  void rebin(int n, double * bins);

  //Style accessors
  void setMinMax(double iMin, double iMax) {topStyle.yAxis_min = iMin; topStyle.yAxis_max= iMax;}
  void setBottMinMax(double iMin, double iMax) {botStyle.yAxis_min = iMin; botStyle.yAxis_max= iMax;}
  void setXTitle(TString title) {topStyle.xTitle = title;}
  void setYTitle(TString title) {topStyle.yTitle = title;}
  void setYTitleBot(TString title) {botStyle.yTitle = title;}



private:

  void prepHist(std::vector<Drawing::Drawable1D>& drawables);
  TString prepRat(std::vector<Drawing::Drawable1D>& drawables, int denIDX, TString stackTitle,bool doBinomErrors);

  //Drawing options
  bool doUnderflow = true;
  bool doOverflow = true;

  Drawing::PadStyle topStyle;
  Drawing::PadStyle botStyle;



  TH1 * totStack;
  std::vector<TH1*>    hists;
  std::vector<TString> histTitles;
  std::vector<TString> histDrawOptions;
  std::vector<bool> histDoPoissonErrors;

  std::vector<TH1*>    stackHists;
  std::vector<TString> stackHistTitles;
};




#endif

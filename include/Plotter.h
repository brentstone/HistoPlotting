

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
  Plotter() {};

  //Adding content...must be done before drawing!
  void addDrawable(Drawing::Drawable1D& input);
  TGraph* addGraph(const TGraph * hist, TString title, int lineColor = -1, int lineStyle = 1, int lineWidth = 4, int markerStyle = 20, int markerSize = 1,  bool drawMarker = true, bool drawErrorBars = true,bool poissonErrors = false, TString drawOption = "");
  TGraph* addGraphLine(const TGraph * hist, TString title, int lineColor = -1, int lineStyle = 1, int lineWidth = 4){return addGraph(hist,title,lineColor,lineStyle,lineWidth,20,1,false,false);}
  TH1* addHist(const TH1 * hist, TString title, int lineColor = -1, int lineStyle = 1, int lineWidth = 4, int markerStyle = 20, int markerSize = 1,  bool drawMarker = true, bool drawErrorBars = true,bool poissonErrors = false, TString drawOption = "");
  TH1* addHistLine(const TH1 * hist, TString title, int lineColor = -1, int lineStyle = 1, int lineWidth = 4){return addHist(hist,title,lineColor,lineStyle,lineWidth,20,1,false,false);}
  TH1* addStackHist(const TH1 * hist, TString title, int fillColor =-1, int fillStyle =1001, int lineColor = 1, int lineWidth =2);
  void addText(TString text, float posX=0.5, float posY =0.5, float textSize = -1, int color = -1, int angle = -1, int font = -1 ) //negative means default
  { textList.push_back(std::make_tuple(text,posX,posY,textSize,color,angle,font));}

  TCanvas * draw(bool save=false, TString printName = "plot.pdf");
  //If denIDX < 0, use the stack
//  TCanvas * drawRatio(int denIDX = 0, TString stackTitle = "stack", ErrorType errType=GAUS, bool save=false, TString printName = "plot.pdf");
  TCanvas * drawRatio(int denIDX = 0, TString stackTitle = "stack", bool doBinomErrors=false, bool save=false, TString printName = "plot.pdf");
  TCanvas * drawSplitRatio(int denIDX = 0, TString stackTitle = "stack", bool doBinomErrors=false, bool save=false, TString printName = "plot.pdf");

  float getStackIntegral() const {return totStack.obj ? ((TH1*)totStack.obj)->Integral(0,-1) : 0.0;}
  const TH1 * getTotStack() const {return (const TH1*)totStack.obj;}

  //Drawing options
  void setUnderflow(bool doIt) {doUnderflow = doIt;}
  void setOverflow(bool doIt) {doOverflow = doIt;}
  void scale(double scale);
  void normalize();
  void rebin(int n);
  void rebin(int n, double * bins);

  //Style accessors
  void setMinMax(double iMin, double iMax) {topStyle.yAxis_min = iMin; topStyle.yAxis_max= iMax;}
  void setBotMinMax(double iMin, double iMax) {botStyle.yAxis_min = iMin; botStyle.yAxis_max= iMax;}
  void setXTitle(TString title) {topStyle.xTitle = title;botStyle.xTitle = title;}
  void setYTitle(TString title) {topStyle.yTitle = title;}
  void setYTitleBot(TString title) {botStyle.yTitle = title;}
  void setLegendPos(float x1, float y1, float x2, float y2)
  {topStyle.leg_x1 = x1;topStyle.leg_x2 = x2;topStyle.leg_y1 = y1;topStyle.leg_y2 = y2;}
  void setLegendNColumns(int nCol = 1) {topStyle.leg_nColumns = nCol;}



  void setCMSLumi(int cmsLumiPos = 33, TString lumiText = "35.9 fb^{-1} (13 TeV)",TString extraText = "", float extraTextOff = 1.6) {
	  topStyle.addCMSLumi = true; topStyle.cmsLumiPos = cmsLumiPos; topStyle.lumiText = lumiText; topStyle.extraText = extraText;
	  topStyle.extraTextOff= extraTextOff;
  }

  void setCMSLumiExtraText(TString extraText) { topStyle.extraText = extraText;}
  void setCMSLumiLumiText(TString lumiText)  { topStyle.lumiText = lumiText;}
  void setCMSLumiPosition(int cmsLumiPos, float extraTextOff = 1.6 )  { topStyle.cmsLumiPos = cmsLumiPos;topStyle.extraTextOff= extraTextOff;}

  void setCanvasSize(int canvasWidth=-1,int canvasHeight=-1) {topStyle.canvasHeight =canvasHeight; topStyle.canvasWidth =canvasWidth;}
  void setMargins(float topMarginSize=-1, float botMarginSize=-1, float leftMarginSize=-1, float rightMarginSize=-1)
  {topStyle.topMarginSize =topMarginSize; topStyle.botMarginSize =botMarginSize;topStyle.leftMarginSize =leftMarginSize;topStyle.rightMarginSize =rightMarginSize;}
  void setAxisTextSize(float axisTextSize = -1) {topStyle.axisTextSize =axisTextSize;}


  //Axis information...only available after drawing!
  TAxis * xAxis() {return topStyle.xAxis;}
  TAxis * yAxis() {return topStyle.yAxis;}
  TAxis * botXAxis() {return botStyle.xAxis;}
  TAxis * botYAxis() {return botStyle.yAxis;}

  std::vector<Drawing::Drawable1D>& getHists() {return hists;}


private:

  void prepHist(std::vector<Drawing::Drawable1D>& drawables);
  TString prepRat(std::vector<Drawing::Drawable1D>& drawables, int denIDX, TString stackTitle,bool doBinomErrors);

  //Drawing options
  bool doUnderflow = true;
  bool doOverflow = true;

  Drawing::PadStyle topStyle;
  Drawing::PadStyle botStyle;

  Drawing::Drawable1D totStack;
  std::vector<Drawing::Drawable1D> hists;
  std::vector<Drawing::Drawable1D> stackHists;
  std::vector<Drawing::TLatexDef> textList;

  static int nGraphs;

  const float labelSF = 1.0; //text size scale up...for what doesnt work in the stylesheet
};




#endif

#ifndef HISTOPLOTTING_DRAWING_H
#define HISTOPLOTTING_DRAWING_H

#include "../include/Drawable.h"
#include <vector>
#include <tuple>
#include <TLegend.h>
#include <TPad.h>
namespace Drawing {
typedef std::tuple<TString,float,float,float,int,float,int> TLatexDef; //string posx, posy, size color angle font
typedef std::tuple<int,const TObject *,TString,TString> TLegendEntryDef; //string posx, posy, size color angle font

struct PadStyle {
  TString   xTitle          ="DEF";
  TString   yTitle          = "DEF";
  double    yAxis_min       =0;
  double    yAxis_max       =-1; //if -1 use default
  double    leg_x1          = 0.65;
  double    leg_y1          = -0.045;//make it positive if you dont want it to be dynamic
  double    leg_x2          = 0.93;
  double    leg_y2          = 0.93;
  int       leg_nColumns    = 1;
  TAxis *   xAxis           =0;
  TAxis *   yAxis           =0;
  TLegend * legend          =0;

  bool      addCMSLumi      = false;
  int       cmsLumiPos      = 33;
  TString   lumiText        = "14 TeV, 200 PU";
  float     extraTextOff    = 1.6;
  TString   extraText       = "";

  int       canvasHeight    = -1; // -1: default from style
  int       canvasWidth     = -1; // -1: default from style
  float     topMarginSize   = -1; // -1: default from style
  float     botMarginSize   = -1; // -1: default from style
  float     leftMarginSize  = -1; // -1: default from style
  float     rightMarginSize = -1; // -1: default from style
  float     axisTextSize    = -1; // -1: default from style,, 0: based on top margin

};

void applyGStyle(TH1* h);

TLegend * buildLegend(const std::vector<Drawable1D>& drawables,const std::vector<TLegendEntryDef>& exEntries, double x1, double y1, double x2, double y2, int nColumns);

void drawPane(TPad * pad, std::vector<Drawable1D>& drawables,std::vector<TLegendEntryDef>& exEntries, PadStyle * style, bool doBuildLegend);
void drawTLatex(TPad * pad, const std::vector<TLatexDef>& textList);

TCanvas * setupSinglePaneCanvas(TString printName, const PadStyle * style);

TCanvas* newAlbum(Int_t numItems, const Char_t* name, const Char_t* title, Int_t width, Int_t height, Int_t rows, Int_t columns);
TCanvas* drawAll(std::vector<TObject*>& plots, TString name, TString drawOption="");
}

#endif

#ifndef HISTOPLOTTING_DRAWING_H
#define HISTOPLOTTING_DRAWING_H

#include "../include/Drawable.h"
#include <vector>
#include <TLegend.h>
#include <TPad.h>
namespace Drawing {
struct PadStyle {
  TString xTitle ="DEF";
  TString yTitle = "DEF";
  double yAxis_min =0;
  double yAxis_max =-1; //if -1 use default
  double leg_x1 = 0.65;
  double leg_y1 = -0.045;//make it positive if you dont want it to be dynamic
  double leg_x2 = 0.93;
  double leg_y2 = 0.93;
  int leg_nColumns = 1;
  TAxis * xAxis=0;
  TAxis * yAxis=0;
  TLegend * legend =0;

};

TLegend * buildLegend(const std::vector<Drawable1D>& drawables, double x1, double y1, double x2, double y2, int nColumns);

void drawPane(TPad * pad, std::vector<Drawable1D>& drawables, PadStyle * style, bool doBuildLegend);
}

#endif

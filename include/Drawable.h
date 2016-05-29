#ifndef HISTOPLOTTING_DRAWABLE_H
#define HISTOPLOTTING_DRAWABLE_H

#include <vector>
#include <TH1.h>
#include <TString.h>

namespace Drawing {
  struct Drawable1D {
    enum Type {NONE,HIST1D, GRAPH, STACK};
    TString drawOpt = "";
    TString title = "";
    Type type = NONE;
    TObject * obj;
    const TH1 * totStack = 0; //used only for thstacks
    std::vector<TString> stackTitles; //used only for thstacks
    TH1 * graphAxisHist=0; //used only for TGraphs
  };

Drawable1D  makeStack(std::vector<TH1*>& hists,std::vector<TString>& titles, TH1 * totStack);
Drawable1D  makeHist(TH1* hist, TString title, TString drawOpt, bool doPoissonErrors = false);
Drawable1D  makeRatio(const TH1* num, const TH1 * den, TString title, TString drawOpt, bool doBinomErrors = false, bool doPoissonErrors = false);
double      getMax(const Drawable1D& drawable);

}

#endif

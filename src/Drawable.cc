#include "../include/Drawable.h"
#include "../include/PlotTools.h"

#include <THStack.h>
namespace Drawing {
  Drawable1D  makeStack(std::vector<TH1*>& hists,std::vector<TString>& titles, TH1 * totStack){
    Drawable1D newDrawable;
    newDrawable.type = Drawable1D::STACK;
    newDrawable.drawOpt = "";
    newDrawable.title = "";
    THStack * stack = new THStack;
//    for(int iH = hists.size() -1; iH >=0; --iH){
    for(unsigned int iH = 0; iH < hists.size(); ++iH){
      stack->Add(hists[iH],"HIST");

    }
    newDrawable.obj = stack;
    newDrawable.totStack = totStack;
    newDrawable.stackTitles = titles;
    return newDrawable;
  }

  Drawable1D  makeHist(TH1* hist, TString title, TString drawOpt, bool doPoissonErrors){
    Drawable1D newDrawable;

    if(doPoissonErrors){
      auto * graph = PlotTools::getPoissonErrors(hist);
      graph->SetLineColor  (hist->GetLineColor());
      graph->SetLineWidth  (hist->GetLineWidth());
      graph->SetLineStyle  (hist->GetLineStyle());
      graph->SetMarkerStyle(hist->GetMarkerStyle());
      graph->SetMarkerColor(hist->GetMarkerColor());
      graph->SetMarkerSize (hist->GetMarkerSize());
      newDrawable.type = Drawable1D::GRAPH;
      newDrawable.drawOpt = drawOpt.Contains("P") ? "P" : "";
      if(drawOpt.Contains("E")) newDrawable.drawOpt  += "0";
      newDrawable.obj = graph;
      newDrawable.graphAxisHist = (TH1*)hist->Clone();
    } else {
      newDrawable.type = Drawable1D::HIST1D;
      newDrawable.drawOpt = drawOpt;
      newDrawable.obj = hist;
    }
    newDrawable.title = title;
    return newDrawable;
  }

  Drawable1D  makeRatio(const TH1* num, const TH1 * den, TString title, TString drawOpt, bool doBinomErrors, bool doPoissonErrors){
    Drawable1D newDrawable;
    if(drawOpt.Contains("E") && (doPoissonErrors || doBinomErrors)){
      auto * graph = doBinomErrors ? PlotTools::getBinomErrors(num,den) :PlotTools::getRatioPoissonErrors(num,den);
      graph->SetLineColor  (num->GetLineColor());
      graph->SetLineWidth  (num->GetLineWidth());
      graph->SetLineStyle  (num->GetLineStyle());
      graph->SetMarkerStyle(num->GetMarkerStyle());
      graph->SetMarkerColor(num->GetMarkerColor());
      graph->SetMarkerSize (num->GetMarkerSize());
      newDrawable.type = Drawable1D::GRAPH;
      newDrawable.drawOpt = drawOpt.Contains("P") ? "P" : "";
      if(drawOpt.Contains("E")) newDrawable.drawOpt  += "0";
      newDrawable.obj = graph;
      newDrawable.graphAxisHist = (TH1*)num->Clone();
    } else {
      TH1 * hist = (TH1*)num->Clone();
      hist->Divide(den);
      newDrawable.type = Drawable1D::HIST1D;
      newDrawable.drawOpt = drawOpt;
      newDrawable.obj = hist;
    }
    newDrawable.title = title;
    return newDrawable;
  }

  double getMax(const Drawable1D& drawable){

    if(drawable.type == Drawable1D::STACK || drawable.type == Drawable1D::HIST1D){
      const TH1 * hist = drawable.type == Drawable1D::STACK  ? drawable.totStack : (const TH1*)drawable.obj;
      double max = 0;
      for(int iB = 0; iB <= hist->GetNbinsX()+1; ++iB) max = std::max(hist->GetBinContent(iB) + hist->GetBinError(iB), max );
      return max;
    }

    double max = 0;
    double x, y;
    const TGraphAsymmErrors * graph = (const TGraphAsymmErrors *)drawable.obj;
      for(int iB = 0; iB <= graph->GetN(); ++iB){
        graph->GetPoint(iB,x,y);
        max = std::max(y + graph->GetErrorYhigh(iB), max );
      }
    return max;
  }

}



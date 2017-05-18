#include "../include/Drawing.h"
#include <THStack.h>
#include <TGraphAsymmErrors.h>
#include "TCanvas.h"
#include "TLatex.h"
#include <iostream>

namespace Drawing{

TLegend * buildLegend(const std::vector<Drawable1D>& drawables, double x1, double y1, double x2, double y2, int nColumns){
  int stackIDX = -1;
  for(unsigned int iD = 0; iD < drawables.size(); ++iD){
    if(drawables[iD].type == Drawing::STACK){
      stackIDX = iD; break;
    }
  }
  int nEntries = drawables.size();
  if(stackIDX >= 0) nEntries = drawables.size() -1 + drawables[stackIDX].stackTitles.size();

  TLegend * legend = new TLegend(x1,y1 < 0 ? y2 + y1*nEntries : y1,x2,y2);
  legend->SetFillStyle(0);
  legend->SetBorderSize(0);
  legend->SetNColumns(nColumns);

  for(int iD = drawables.size() -1; iD >= 0; --iD){
//  for(unsigned int iD = 0; iD < drawables.size(); ++iD){
    if(int(iD) == stackIDX) continue;//If it is a stack wait!
    TString opt = "";
    if(drawables[iD].type == Drawing::GRAPH){
      if(drawables[iD].drawOpt.Contains("0",TString::kIgnoreCase)) opt = "E";
      if(drawables[iD].drawOpt.Contains("P",TString::kIgnoreCase)) opt += "P";
      if(drawables[iD].drawOpt.Contains("L",TString::kIgnoreCase)) opt += "L";
      if(drawables[iD].drawOpt.Contains("E",TString::kIgnoreCase)) opt += "L";
    } else {
      if(drawables[iD].drawOpt.Contains("E",TString::kIgnoreCase)) opt = "E";
      if(drawables[iD].drawOpt.Contains("P",TString::kIgnoreCase)) opt += "P";
      else opt += "L";
    }

    legend->AddEntry(drawables[iD].obj,drawables[iD].title,opt);
  }
  if(stackIDX >= 0){
    const TList * list = ((THStack*)drawables[stackIDX].obj)->GetHists();
//    for(int iH = 0; iH < list->GetSize(); ++iH){
    for(int iH = list->GetSize() -1; iH >=0; --iH){
      legend->AddEntry(list->At(iH),drawables[stackIDX].stackTitles[iH],"f");
    }
  }
  return legend;
}


void drawPane(TPad * pad, std::vector<Drawable1D>& drawables, PadStyle * style, bool doBuildLegend){
  if(!drawables.size()) throw std::invalid_argument("Drawing::drawPane -> Need to provide histograms");;;
  if(doBuildLegend) style->legend = buildLegend(drawables, style->leg_x1,style->leg_y1, style->leg_x2,style->leg_y2, style->leg_nColumns);
  double max = 0;
  for(const auto& d : drawables) max = std::max(max,getMax(d));
  pad->cd();
  //Initial drawing with axis
  Drawable1D * first = &drawables[0];
  if(first->type == Drawing::STACK){
    THStack * h = (THStack*)first->obj;
    h->Draw("HIST");
    style->xAxis = h->GetXaxis();
    style->yAxis = h->GetYaxis();
    h->SetMinimum(style->yAxis_min);
    h->SetMaximum(style->yAxis_max < 0 ? max*1.15 : style->yAxis_max);
  }
  if(first->type == Drawing::HIST1D){
    TH1 * h = (TH1*)first->obj;
    h->Draw(TString::Format("HIST %s",first->drawOpt.Data()));
    style->xAxis = h->GetXaxis();
    style->yAxis = h->GetYaxis();
    h->SetMinimum(style->yAxis_min);
    h->SetMaximum(style->yAxis_max < 0 ? max*1.15 : style->yAxis_max);
  }
  if(first->type == Drawing::GRAPH){
    first->graphAxisHist->Draw("AXIS");
    TGraphAsymmErrors * h = (TGraphAsymmErrors*)first->obj;
    h->Draw(TString::Format("SAME %s",first->drawOpt.Data()));
    style->xAxis = first->graphAxisHist->GetXaxis();
    style->yAxis = first->graphAxisHist->GetYaxis();
    first->graphAxisHist->SetMinimum(style->yAxis_min);
    first->graphAxisHist->SetMaximum(style->yAxis_max < 0 ? max*1.15 : style->yAxis_max);
  }
  for(unsigned int iD = 1; iD < drawables.size(); ++iD){
    Drawable1D * draw = &drawables[iD];
    if(draw->type == Drawing::HIST1D){
      TH1 * h = (TH1*)draw->obj;
      h->Draw(TString::Format("SAME AHIST %s",draw->drawOpt.Data()));
    }
    if(draw->type == Drawing::GRAPH){
      TGraphAsymmErrors * h = (TGraphAsymmErrors*)draw->obj;
      h->Draw(TString::Format("SAME %s",draw->drawOpt.Data()));
    }
  }

  if(doBuildLegend)style->legend->Draw();

}

void drawTLatex(TPad * pad, const std::vector<TLatexDef>& textList){
	pad->cd();
	for(const auto& text : textList ){
  	  TLatex latex;
  	  latex.SetNDC();
  	  if(std::get<3>(text) >= 0 )latex.SetTextSize(std::get<3>(text));
  	  if(std::get<4>(text) >= 0 )latex.SetTextColor(std::get<4>(text));
  	  if(std::get<5>(text) >= 0 )latex.SetTextAngle(std::get<5>(text));
  	  if(std::get<6>(text) >= 0 )latex.SetTextFont(std::get<6>(text));
	  latex.DrawLatex(std::get<1>(text),std::get<2>(text),std::get<0>(text));
	}
}

TCanvas* setupSinglePaneCanvas(TString printName, const PadStyle * style){
	TCanvas * c = 0;
	if(style->canvasHeight > 0 && style->canvasWidth > 0  ){
		c =new TCanvas(printName,printName,style->canvasWidth,style->canvasHeight);
		c->SetWindowSize(style->canvasWidth + (style->canvasWidth - c->GetWw()), style->canvasHeight + (style->canvasHeight - c->GetWh()));
	} else{
		c =new TCanvas(printName,printName);
	}
	if(style->leftMarginSize >= 0) c->SetLeftMargin(style->leftMarginSize);
	if(style->rightMarginSize >= 0) c->SetRightMargin(style->rightMarginSize);
	if(style->topMarginSize >= 0) c->SetTopMargin(style->topMarginSize);
	if(style->botMarginSize >= 0) c->SetBottomMargin(style->botMarginSize);

	return c;
}
}

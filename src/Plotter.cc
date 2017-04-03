#include "THStack.h"
#include "../include/Plotter.h"
#include "../include/StyleInfo.h"
#include "../include/PlotTools.h"

TH1* Plotter::addHist(const TH1 * hist, TString title, int lineColor, int lineStyle, int lineWidth, int markerStyle, int markerSize, bool drawMarker, bool drawErrorBars, bool poissonErrors, TString drawOption){
  TH1* h = (TH1*)hist->Clone();
  //style
  h->SetLineColor(lineColor >= 0 ? lineColor : StyleInfo::getLineColor(hists.size()) );
  h->SetLineWidth(lineWidth);

  if(drawMarker){
    h->SetMarkerColor(lineColor >= 0 ? lineColor : StyleInfo::getLineColor(hists.size()) );
    h->SetMarkerStyle(markerStyle);
    h->SetMarkerSize(markerSize);
  } else {
    h->SetLineStyle(lineStyle);
    h->SetMarkerStyle(0);
    h->SetMarkerSize(0);
  }

  hists.push_back(h);
  histTitles.push_back(title);
  if(drawOption == ""){
    if(drawErrorBars) drawOption = "E1X0 ";
    if(drawMarker) drawOption += "P ";
  }
  histDrawOptions.push_back(drawOption);
  histDoPoissonErrors.push_back(poissonErrors);
  return h;
}

TH1* Plotter::addStackHist(const TH1 * hist, TString title, int fillColor, int fillStyle, int lineColor, int lineWidth){
  TH1* h = (TH1*)hist->Clone();

  if(!totStack) totStack= (TH1*)h->Clone();
  else totStack->Add(h);

  //style
  h->SetLineWidth(lineWidth);
  h->SetLineColor(lineColor);
  h->SetMarkerStyle(0);
  h->SetMarkerSize(0);
  h->SetFillColor(fillColor >= 0 ? fillColor : StyleInfo::getFillColor(stackHists.size()));
  h->SetFillStyle(fillStyle);

  stackHists.push_back(h);
  stackHistTitles.push_back(title);
  return h;
}

TCanvas * Plotter::draw(bool save, TString printName){
  if(hists.size() == 0 && stackHists.size() == 0) return 0;

  if(doUnderflow){ for(auto * h : hists) PlotTools::toUnderflow(h); for(auto * h : stackHists) PlotTools::toUnderflow(h);}
  if(doOverflow) { for(auto * h : hists) PlotTools::toOverflow(h); for(auto * h : stackHists) PlotTools::toOverflow(h);}

  TCanvas * c =new TCanvas(printName,printName);
  std::vector<Drawing::Drawable1D> drawables;
  prepHist(drawables);
  Drawing::drawPane(c,drawables,&topStyle,true);
  if(stackHists.size()){
    topStyle.yAxis->SetTitle(topStyle.yTitle == "DEF" ? stackHists[0]->GetYaxis()->GetTitle() : topStyle.yTitle.Data());
    topStyle.xAxis->SetTitle(topStyle.xTitle == "DEF" ? stackHists[0]->GetXaxis()->GetTitle() : topStyle.xTitle.Data());
  } else {
    topStyle.yAxis->SetTitle(topStyle.yTitle == "DEF" ? hists[0]->GetYaxis()->GetTitle() : topStyle.yTitle.Data());
    topStyle.xAxis->SetTitle(topStyle.xTitle == "DEF" ? hists[0]->GetXaxis()->GetTitle() : topStyle.xTitle.Data());
  }

  if(save) c->Print(printName);
  return c;
}

TCanvas * Plotter::drawRatio(int denIDX, TString stackTitle,bool doBinomErrors, bool save, TString printName){
  if(hists.size() == 0 && stackHists.size() == 0)throw std::invalid_argument("Plotter::drawRatio -> Need to provide histograms");;

  if(doUnderflow){ for(auto * h : hists) PlotTools::toUnderflow(h); for(auto * h : stackHists) PlotTools::toUnderflow(h);}
  if(doOverflow) { for(auto * h : hists) PlotTools::toOverflow(h); for(auto * h : stackHists) PlotTools::toOverflow(h);}

  std::vector<Drawing::Drawable1D> ratDrawables;
  TString denTitle = prepRat(ratDrawables,denIDX,stackTitle,doBinomErrors);
  TCanvas * c =new TCanvas(printName,printName);
  Drawing::drawPane(c,ratDrawables,&topStyle,true);

  topStyle.yAxis->SetTitle(topStyle.yTitle == "DEF" ? TString::Format("N/N(%s)",denTitle.Data()).Data() : topStyle.yTitle.Data());
  if(stackHists.size()){
    topStyle.xAxis->SetTitle(topStyle.xTitle == "DEF" ? stackHists[0]->GetXaxis()->GetTitle() : topStyle.xTitle.Data());
  } else {
    topStyle.xAxis->SetTitle(topStyle.xTitle == "DEF" ? hists[0]->GetXaxis()->GetTitle() : topStyle.xTitle.Data());
  }

  if(save) c->Write(printName);
  return c;
}

TCanvas * Plotter::drawSplitRatio(int denIDX, TString stackTitle,bool doBinomErrors, bool save, TString printName){
  if(hists.size() == 0 && stackHists.size() == 0) return 0;

  if(doUnderflow){ for(auto * h : hists) PlotTools::toUnderflow(h); for(auto * h : stackHists) PlotTools::toUnderflow(h);}
  if(doOverflow) { for(auto * h : hists) PlotTools::toOverflow(h); for(auto * h : stackHists) PlotTools::toOverflow(h);}

  double topScale = (.75)*(1/.66);
  if(topStyle.leg_y1 < 0) topStyle.leg_y1 *= (1/.66);
  topStyle.leg_y2 -= .05;


  TCanvas * c =new TCanvas(printName,printName);
  TPad *pad1 = new TPad("pad1", "pad1", 0, 0.33, 1, 1.0);
  pad1->SetBottomMargin(.025); // Upper and lower plot are joined
  pad1->SetTopMargin(.1);
  pad1->SetLeftMargin(.14);
  pad1->Draw();             // Draw the upper pad: pad1
  pad1->cd();               // pad1 becomes the current pad
  std::vector<Drawing::Drawable1D> drawables;
  prepHist(drawables);
  Drawing::drawPane(pad1,drawables,&topStyle,true);
  topStyle.xAxis->SetLabelOffset(2);


  topStyle.yAxis->SetLabelSize(topStyle.yAxis->GetLabelSize()*topScale );
  topStyle.yAxis->SetTitleOffset(topStyle.yAxis->GetTitleOffset()*.70);
  topStyle.yAxis->SetTitleSize(topStyle.yAxis->GetTitleSize()*topScale);

  if(stackHists.size()){
    topStyle.yAxis->SetTitle(topStyle.yTitle == "DEF" ? stackHists[0]->GetYaxis()->GetTitle() : topStyle.yTitle.Data());
  } else {
    topStyle.yAxis->SetTitle(topStyle.yTitle == "DEF" ? hists[0]->GetYaxis()->GetTitle() : topStyle.yTitle.Data());
  }

  c->cd();
  TPad *pad2 = new TPad("pad2", "pad2", 0, 0, 1, 0.33);
  pad2->SetTopMargin(.005);
  pad2->SetLeftMargin(.14);
  pad2->SetBottomMargin(.3);
  pad2->Draw();             // Draw the upper pad: pad1
  pad2->cd();               // pad1 becomes the current pad


  std::vector<Drawing::Drawable1D> ratDrawables;
  TString denTitle = prepRat(ratDrawables,denIDX,stackTitle,doBinomErrors);
  Drawing::drawPane(pad2,ratDrawables,&botStyle,false);
  double botScale = (.75)*(1/.33);
  botStyle.yAxis->SetLabelSize(botStyle.yAxis->GetLabelSize()*botScale);
  botStyle.yAxis->SetTitleSize(botStyle.yAxis->GetTitleSize()*botScale);
  botStyle.yAxis->SetTitleOffset(botStyle.yAxis->GetTitleOffset()*.33);
  botStyle.xAxis->SetLabelSize(botStyle.xAxis->GetLabelSize()*botScale);
  botStyle.xAxis->SetTitleSize(botStyle.xAxis->GetTitleSize()*botScale);

  botStyle.yAxis->SetTitle(topStyle.yTitle == "DEF" ? TString::Format("N/N(%s)",denTitle.Data()).Data() : botStyle.yTitle.Data());
  if(stackHists.size()){
    botStyle.xAxis->SetTitle(topStyle.xTitle == "DEF" ? stackHists[0]->GetXaxis()->GetTitle() : botStyle.xTitle.Data());
  } else {
    botStyle.xAxis->SetTitle(topStyle.xTitle == "DEF" ? hists[0]->GetXaxis()->GetTitle() : botStyle.xTitle.Data());
  }

  if(save) c->Write(printName);
  return c;
}



void Plotter::scale(double scale){
  for(auto * h : hists) h->Scale(scale);
  for(auto * h : stackHists) h->Scale(scale);
}
void Plotter::normalize() {
  for(auto * h : hists) PlotTools::normalize(h);
  if(totStack){
    double stackNorm = totStack->Integral(0,-1);
    for(auto * h : stackHists) h->Scale(1/stackNorm);
    totStack->Scale(1/stackNorm);
  }
}

void Plotter::rebin(int n){
  for(auto * h : hists) PlotTools::rebin(h,n);
  for(auto * h : stackHists) PlotTools::rebin(h,n);
}
void Plotter::rebin(int n, double * bins){
  for(auto * h : hists) PlotTools::rebin(h,n,bins);
  for(auto * h : stackHists) PlotTools::rebin(h,n,bins);
}

void Plotter::prepHist(std::vector<Drawing::Drawable1D>& drawables){
  if(stackHists.size())drawables.push_back(Drawing::makeStack(stackHists, stackHistTitles,totStack));
  if(hists.size())
  for(int iH = hists.size() -1; iH >= 0; --iH){
    drawables.push_back(Drawing::makeHist(hists[iH], histTitles[iH],histDrawOptions[iH],histDoPoissonErrors[iH]));
  }
}

TString Plotter::prepRat(std::vector<Drawing::Drawable1D>& drawables, int denIDX, TString stackTitle,bool doBinomErrors){
  TH1 * den = 0;
  TString denTitle;
  if(denIDX < 0){
    den = totStack;
    denTitle = stackTitle;
  }  else if(denIDX < int(hists.size())) {
    den = hists[denIDX];
    denTitle = histTitles[denIDX];
  } else {throw std::invalid_argument("Plotter::prepRat -> Need to provide a denominator");;}

  if(denIDX >= 0 && totStack) drawables.push_back(Drawing::makeRatio(totStack, den,stackTitle,"",doBinomErrors));
  if(hists.size())
  for(int iH = hists.size() -1; iH >= 0; --iH){
    if(int(iH) == denIDX) continue;
    drawables.push_back(Drawing::makeRatio(hists[iH], den,histTitles[iH],histDrawOptions[iH],doBinomErrors,histDoPoissonErrors[iH]));
  }
  return denTitle;
}

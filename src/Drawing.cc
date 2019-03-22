#include "../include/Drawing.h"
#include <THStack.h>
#include <TGraphAsymmErrors.h>
#include "TCanvas.h"
#include "TLatex.h"
#include "TStyle.h"
#include "TMath.h"
#include <iostream>

namespace Drawing{

TLegend * buildLegend(const std::vector<Drawable1D>& drawables,const std::vector<TLegendEntryDef>& exEntries, double x1, double y1, double x2, double y2, int nColumns){
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

    int curEntry = 0;

    auto addEx =[&] () {
        for(const auto& exE :exEntries){
            if(std::get<0>(exE)==curEntry){
                legend->AddEntry(std::get<1>(exE),std::get<2>(exE),std::get<3>(exE));
                curEntry++;
            }
        }
    };

    for(int iD = drawables.size() -1; iD >= 0; --iD){
        addEx();

        //  for(unsigned int iD = 0; iD < drawables.size(); ++iD){
        if(int(iD) == stackIDX) continue;//If it is a stack wait!
        TString opt = "";
        if(drawables[iD].type == Drawing::GRAPH){
            if(drawables[iD].drawOpt.Contains("0",TString::kIgnoreCase)) opt = "E";
            if(drawables[iD].drawOpt.Contains("P",TString::kIgnoreCase)) opt += "P";
            if(drawables[iD].drawOpt.Contains("L",TString::kIgnoreCase)) opt += "L";
            if(drawables[iD].drawOpt.Contains("C",TString::kIgnoreCase)) opt += "L";
            if(drawables[iD].drawOpt.Contains("E",TString::kIgnoreCase)) opt += "L";
            if(drawables[iD].drawOpt.Contains("2",TString::kIgnoreCase)) opt += "F";
            if(drawables[iD].drawOpt.Contains("3",TString::kIgnoreCase)) opt += "F";
            if(drawables[iD].drawOpt.Contains("4",TString::kIgnoreCase)) opt += "F";
            if(drawables[iD].drawOpt.Contains("5",TString::kIgnoreCase)) opt += "F";
        } else {
            if(drawables[iD].drawOpt.Contains("E",TString::kIgnoreCase)) opt = "E";
            if(drawables[iD].drawOpt.Contains("P",TString::kIgnoreCase)) opt += "P";
            if(drawables[iD].drawOpt.Contains("X",TString::kIgnoreCase)) opt += "L";
            else opt += "L";
        }

        legend->AddEntry(drawables[iD].obj,drawables[iD].title,opt);
        curEntry++;
    }
    if(stackIDX >= 0){
        const TList * list = ((THStack*)drawables[stackIDX].obj)->GetHists();
        //    for(int iH = 0; iH < list->GetSize(); ++iH){
        for(int iH = list->GetSize() -1; iH >=0; --iH){
            addEx();
            legend->AddEntry(list->At(iH),drawables[stackIDX].stackTitles[iH],"f");
            curEntry++;
        }
    }
    addEx();
    return legend;
}



void drawPane(TPad * pad, std::vector<Drawable1D>& drawables,std::vector<TLegendEntryDef>& exEntries, PadStyle * style){
    if(!drawables.size()) throw std::invalid_argument("Drawing::drawPane -> Need to provide histograms");;;
    if(style->drawLegend) style->legend = buildLegend(drawables,exEntries, style->leg_x1,style->leg_y1, style->leg_x2,style->leg_y2, style->leg_nColumns);
    double max = 0;
    for(const auto& d : drawables) max = std::max(max,getMax(d));
    pad->cd();
    //Initial drawing with axis
    auto procAxis = [&] (Drawable1D * first ){
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
            h = (TH1*)h->Clone(TString(h->GetName())+"_axis"  );
            h->Draw("AXIS");
            style->xAxis = h->GetXaxis();
            style->yAxis = h->GetYaxis();
            h->SetMinimum(style->yAxis_min);
            h->SetMaximum(style->yAxis_max < 0 ? max*1.15 : style->yAxis_max);
        }
        if(first->type == Drawing::GRAPH){
            auto h = (TH1*)first->graphAxisHist;
            h->Draw("AXIS");
            style->xAxis = h->GetXaxis();
            style->yAxis = h->GetYaxis();
            h->SetMinimum(style->yAxis_min);
            h->SetMaximum(style->yAxis_max < 0 ? max*1.15 : style->yAxis_max);
        }
    };

    //do the axis first
    bool filled = false;
    for(auto& d : drawables ){
        if(d.type == Drawing::GRAPH) continue;
        procAxis(&d);
        filled = true;
        break;
    }
    if(!filled)
        for(auto& d : drawables ){
            procAxis(&d);
            break;
        }
    for(unsigned int iD = 0; iD < drawables.size(); ++iD){
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

    if(style->drawLegend)style->legend->Draw();

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

void applyGStyle(TH1* h){

    auto applyToAxis = [&](TAxis* axis, TString axL){
        axis->SetTitleColor(gStyle->GetTitleColor(axL));
        axis->SetTitleFont   (gStyle->GetTitleFont  (axL));
        axis->SetTitleSize   (gStyle->GetTitleSize  (axL));
        axis->SetTitleFont   (gStyle->GetTitleFont  (axL));
        axis->SetTitleSize   (gStyle->GetTitleSize  (axL));
        axis->SetLabelColor  (gStyle->GetLabelColor (axL));
        axis->SetLabelFont   (gStyle->GetLabelFont  (axL));
        axis->SetLabelOffset (gStyle->GetLabelOffset(axL));
        axis->SetLabelSize   (gStyle->GetLabelSize  (axL));
        axis->SetAxisColor   (gStyle->GetAxisColor  (axL));
        axis->SetTickLength  (gStyle->GetTickLength (axL));
        axis->SetNdivisions  (gStyle->GetNdivisions (axL));
    };
    applyToAxis(h->GetXaxis(),"X");
    applyToAxis(h->GetYaxis(),"Y");
    h->GetXaxis()->SetTitleOffset(gStyle->GetTitleXOffset());
    h->GetYaxis()->SetTitleOffset(gStyle->GetTitleYOffset());
}


TCanvas* newAlbum(Int_t numItems, const Char_t* name, const Char_t* title, Int_t width, Int_t height, Int_t rows, Int_t columns)
{
    if (rows > 0 && columns > 0) {
    } else if (rows > 0) {
        columns   = TMath::CeilNint(1.0 * numItems / rows);
    } else if (columns > 0) {
        rows      = TMath::CeilNint(1.0 * numItems / columns);
    } else {
        columns   = TMath::CeilNint(TMath::Sqrt(numItems));
        rows      = TMath::CeilNint(1.0 * numItems / columns);
    }
    if (width < 0) {
        if      (columns == 1)  width   = 550;
        else if (columns == 2)  width   = 1100;
        else if (columns == 3)  width   = 1500;
        else                    width   = 1780;
    }
    if (height < 0) {
        if      (rows == 1)     height  = 550;
        else if (rows == 2)     height  = 950;
        else                    height  = 1000;
    }
    TCanvas*    canvas = new TCanvas(name,title,0,0,width,height);  //MakeCanvas(name, title, width, height);
    if (columns > 1 || rows > 1)
        canvas->Divide(columns, rows, 0.00001f, 0.00001f);
    return canvas;
}

TCanvas* drawAll(std::vector<TObject*>& plots, TString name, TString drawOption)
{
    if (plots.size() == 0) {
        std::cout << "WARNING : Nothing to draw." << std::endl;
        return 0;
    }

    Int_t             numCols   = 0;
    Int_t             numRows   = 0;


    Int_t             numPlots  = plots.size();
    TCanvas*          canvas    = newAlbum(numPlots, name, 0, -1, -1, numRows, numCols);
    for (Int_t iPlot = 0; iPlot < numPlots; ++iPlot) {
        TObject*        object    = plots[iPlot];
        if (object == 0)          continue;

        TVirtualPad*    pad       = canvas->cd(iPlot + 1);

        if (object->InheritsFrom("TH1")) {
            TH1*        histo     = dynamic_cast<TH1*>(object);
            if (histo->GetMaximum() > histo->GetMinimum()) {
                if (histo->GetDimension() > 1 && pad->GetRightMargin() < 0.1)
                    pad->SetRightMargin(0.2f);

                //-- 2D histograms ----------------------------------------------------
                histo->Draw(drawOption);

                if (pad->GetLogx())
                    histo->GetXaxis()->SetMoreLogLabels();
                if (pad->GetLogy() && histo->GetMaximum() < 1e2*histo->GetMinimum())
                    histo->GetYaxis()->SetMoreLogLabels();
                if (pad->GetLogz() && histo->GetMaximum() < 1e2*histo->GetMinimum())
                    histo->GetZaxis()->SetMoreLogLabels();
            }
        }
        else if (object->InheritsFrom("TCanvas")) {
            TCanvas*      sub     = dynamic_cast<TCanvas*>(object);
            sub->DrawClonePad();
        } else {
            object->Draw(drawOption);
        }
    }

    canvas->Update();
    return canvas;
}

}

#ifndef HISTOPLOTTING_DRAWABLE_H
#define HISTOPLOTTING_DRAWABLE_H

#include <vector>
#include <TH1.h>
#include <TGraph.h>
#include <TString.h>

namespace Drawing {
enum Type {NONE,HIST1D, GRAPH, STACK};
struct Drawable1D {
	Drawable1D(){};
	Drawable1D(TString drawOpt,TString title, Type type, TObject * obj, bool doPoisson ) :
		drawOpt(drawOpt),title(title),type(type),obj(obj),doPoisson(doPoisson) {}
	TString drawOpt = "";
	TString title = "";
	Type type = NONE;
	TObject * obj = 0;
	bool doPoisson = false;
	const TH1 * totStack = 0; //used only for thstacks
	std::vector<TString> stackTitles; //used only for thstacks
	TH1 * graphAxisHist=0; //used only for TGraphs

	//Used for easy access
	const char *  getXTitle() const {
		switch(type) {
		case HIST1D :
			return ((TH1*) obj)->GetXaxis()->GetTitle();
		case GRAPH :
			return ((TGraph*) obj)->GetXaxis()->GetTitle();
		default:
			return "";
		}
	}
	const char * getYTitle() const {
		switch(type) {
		case HIST1D :
			return ((TH1*) obj)->GetYaxis()->GetTitle();
		case GRAPH :
			return ((TGraph*) obj)->GetYaxis()->GetTitle();
		default:
			return "";
		}
	}

};
Drawable1D  makeStack(std::vector<Drawable1D>& hists,Drawable1D& totStack);
Drawable1D  makeStack(std::vector<TH1*>& hists,std::vector<TString>& titles, TH1 * totStack);

Drawable1D  convertToPoisson(Drawable1D& input); //To do the error conversion
Drawable1D  makeHist(TH1* hist, TString title, TString drawOpt, bool doPoissonErrors = false);
Drawable1D  makeRatio(const Drawable1D num, const TH1* den, bool doBinomErrors = false);
Drawable1D  makeRatio(TH1* num, const TH1 * den, TString title, TString drawOpt = "", bool doBinomErrors = false, bool doPoissonErrors = false);
double      getMax(const Drawable1D& drawable);

}

#endif

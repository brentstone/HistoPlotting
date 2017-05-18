

#ifndef HISTOPLOTTING_STYLEINFO_H
#define HISTOPLOTTING_STYLEINFO_H

#include "TPad.h"

namespace StyleInfo{
const int defLineColors[] =
{
     1 //black
    ,857 //blue
    ,634 //red
    ,800 //green
    ,921 //grey
    ,6 //purple
    ,2 //bright red
	,5 //yellow


};
const unsigned int nDefLineColors = 8;
const int defFillColors[] =
{
//     390 //blue
//    ,903 //blue
//    ,906 //red
//    ,591 //green
//    ,411 //yellow

		591 //blue
		,390 //yllow
		,906 //pink
		,903  //dark red
		,411 //green


};
const unsigned int nDefFillColors = 5;


int getLineColor(int indx);
int getFillColor(int indx);

//void CMS_lumi( TPad* pad, int iPeriod=3, int iPosX=10 );
//iPos
//first digit: Text allignment: 1/2/3 left/center/right
//Second digit: Text position: 1/2/3 left/center/right
//One digit: out of frame: 0/1/2 extra text left/extra text below/extra text below right
void CMS_lumi( TPad* pad, int iPosX=10, TString lumiText = "19.7 fb^{-1} (8 TeV)",TString extraText = "", float extraTextOff = 1.6 );
}



#endif

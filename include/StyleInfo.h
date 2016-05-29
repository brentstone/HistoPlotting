

#ifndef HISTOPLOTTING_STYLEINFO_H
#define HISTOPLOTTING_STYLEINFO_H

namespace StyleInfo{
const int defLineColors[] =
{
     1 //black
    ,4 //blue
    ,2 //red
    ,8 //green
    ,5 //yellow
    ,6 //purple


};
const unsigned int nDefLineColors = 6;
const int defFillColors[] =
{
    390 //blue
    ,903 //blue
    ,906 //red
    ,591 //green
    ,411 //yellow


};
const unsigned int nDefFillColors = 5;


int getLineColor(int indx);
int getFillColor(int indx);

}



#endif

#include "TStyle.h"
#include "../include/StyleInfo.h"

int StyleInfo::getLineColor(int indx) {
  return defLineColors[indx % ( nDefLineColors)];
}

int StyleInfo::getFillColor(int indx) {
  return defFillColors[indx % ( nDefFillColors)];
}

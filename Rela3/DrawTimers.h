#ifndef __DRAW_TIMERS_H__
#define __DRAW_TIMERS_H__

#include <deque>
#include "hpdf.h"
int InitPdf( HPDF_Doc *pdf,HPDF_Outline *root);
HPDF_Page CreateNewPage(char *Name,HPDF_Doc &pdf,HPDF_Outline &root);
void DrawTimer(HPDF_Doc *pdf,HPDF_Page *page, int x, std::deque<std::pair<double, double> > &deqFactors,char *Name);

#endif
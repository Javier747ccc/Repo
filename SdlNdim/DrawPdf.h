#pragma once
#include <deque>
#include "hpdf.h"

#ifndef __DRAW_PDF__H__
#define __DRAW_PDF__H__
int InitPdf(HPDF_Doc *pdf, HPDF_Outline *root);
HPDF_Page CreateNewPage(char *Name, HPDF_Doc &pdf, HPDF_Outline &root);
void DrawTimer(HPDF_Doc *pdf, HPDF_Page *page, int x, std::deque<std::pair<double, double> > &deqFactors, char *Name);



#endif # //__DRAW_PDF__H__

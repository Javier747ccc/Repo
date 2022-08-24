#include <stdlib.h>
#include <stdio.h>
#include <deque>
#include "hpdf.h"

int InitPdf(HPDF_Doc *pdf, HPDF_Outline *root);
HPDF_Page CreateNewPage(char *Name, HPDF_Doc &pdf, HPDF_Outline &root);

void error_handler(HPDF_STATUS   error_no,
	HPDF_STATUS   detail_no,
	void         *user_data)
{
	printf("ERROR: error_no=%04X, detail_no=%u\n", (HPDF_UINT)error_no,
		(HPDF_UINT)detail_no);

}



int InitPdf(HPDF_Doc *pdf, HPDF_Outline *root)
{
	*pdf = HPDF_New(error_handler, NULL);

	if (!*pdf)
	{
		printf("ERROR: cannot create pdf object.\n");
		return 1;
	}
	HPDF_Page page;
	HPDF_REAL height;
	HPDF_REAL width;
	HPDF_Font font;
	try
	{
		/* Add a new page object. */
		*root = HPDF_CreateOutline(*pdf, NULL, "Main", NULL);
		HPDF_Outline_SetOpened(*root, HPDF_TRUE);
	}
	catch (...)
	{
		HPDF_Free(*pdf);
		return 1;
	}
	return 0;
}

HPDF_Page CreateNewPage(char *Name, HPDF_Doc &pdf, HPDF_Outline &root)
{
	HPDF_Page page = HPDF_AddPage(pdf);

	HPDF_Outline outline = HPDF_CreateOutline(pdf, root, Name, NULL);
	HPDF_Destination dst = HPDF_Page_CreateDestination(page);
	HPDF_Destination_SetXYZ(dst, 0, HPDF_Page_GetHeight(page), 1);
	HPDF_Outline_SetDestination(outline, dst);
	return page;
}


void DrawTimer(HPDF_Doc *pdf, HPDF_Page *page, int x, std::deque<std::pair<double, double> > &deqFactors, char *Name)
{
	const HPDF_UINT16 DASH_MODE1[] = { 3 };
	HPDF_REAL height = HPDF_Page_GetHeight(*page);
	HPDF_REAL width = HPDF_Page_GetWidth(*page);
	HPDF_Page_MoveTo(*page, x, height - 40);
	HPDF_Page_LineTo(*page, x, 80);
	HPDF_Page_Stroke(*page);
	HPDF_Font font = HPDF_GetFont(*pdf, "Courier-Oblique", NULL);

	HPDF_Page_SetLineWidth(*page, 1);
	HPDF_Page_SetDash(*page, DASH_MODE1, 1, 1);
	for (int i = 0; i < 40; i++)
	{
		HPDF_Page_MoveTo(*page, 0, (double)i*20.0 + 40);
		HPDF_Page_LineTo(*page, width, (double)i*20.0 + 40);
		HPDF_Page_Stroke(*page);
	}

	HPDF_Page_SetDash(*page, NULL, 0, 0);
	HPDF_Page_SetLineWidth(*page, 1);
	double Factor = 1;
	double y = 40;
	char StrTemp[256];
	double LastFactor = 0;
	double IncFactor = 1;
	double Time = 0;
	double RemainTime = 0;
	double Len;
	double PosY = 80;
	int Num = 0;
	for (std::deque<std::pair<double, double> >::iterator it = deqFactors.begin(); it != deqFactors.end(); it++)
	{
		std::pair <double, double> pr = *it;
		Len = pr.first;
		if (Len < 0)
			Len = 5000;
		Factor = it->second;

		sprintf(StrTemp, "%f", Factor);
		HPDF_Page_BeginText(*page);
		//HPDF_Page_SetFontAndSize (*page, font, 10);
		HPDF_Page_TextOut(*page, x + 10, PosY, StrTemp);
		HPDF_Page_EndText(*page);

		HPDF_Page_MoveTo(*page, x - 10, PosY);
		HPDF_Page_LineTo(*page, x + 10, PosY);
		HPDF_Page_Stroke(*page);
		if (RemainTime > 0)
		{

			PosY += 20 * ((1 - RemainTime)*Factor);
			Num++;
		}

		for (int i = 0; i < (int)(Len - RemainTime); i++)
		{
			HPDF_Page_MoveTo(*page, x - 5, PosY);
			HPDF_Page_LineTo(*page, x + 5, PosY);
			HPDF_Page_Stroke(*page);

			sprintf(StrTemp, "%d", Num);
			HPDF_Page_BeginText(*page);
			//HPDF_Page_SetFontAndSize (*page, font, 10);
			HPDF_Page_TextOut(*page, x - 15, PosY, StrTemp);
			HPDF_Page_EndText(*page);

			Num++;

			PosY += (20 * Factor);
			if (PosY > height - 40)
				break;
		}
		HPDF_Page_MoveTo(*page, x - 5, PosY);
		HPDF_Page_LineTo(*page, x + 5, PosY);
		HPDF_Page_Stroke(*page);

		sprintf(StrTemp, "%d", Num);
		HPDF_Page_BeginText(*page);
		//HPDF_Page_SetFontAndSize (*page, font, 10);
		HPDF_Page_TextOut(*page, x - 15, PosY, StrTemp);
		HPDF_Page_EndText(*page);
		RemainTime += Len - (int)Len;
		PosY += 20 * (RemainTime*Factor);
		//RemainTime = 1-RemainTime;
		//Num++;
	}
}

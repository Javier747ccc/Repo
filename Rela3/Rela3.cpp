// Rela3.cpp: define el punto de entrada de la aplicación de consola.
//

#include <stdlib.h>
#include <stdio.h>
#include <iostream>
#include "CFocos.h"
#include "DrawTimers.h"



void Test1(std::deque<std::deque<std::pair<double, double> >> *deqFactors)
{
    
    std::deque<std::pair<double, double> > deqF;
    


    CFocos fcc;
    for (int i = 0; i < 5; i++)
	{

		fcc.NewFoco();
	}
	fcc.ChangeAmp(0, 0.5);
    fcc.ChangeAmp(2, 0.5);

    cout << std::fixed;
    cout.precision(4); 
    
    fcc.PrintState();
    fcc.DrawTimes(deqFactors);
    unsigned int ms = 0;
    

	for (int i = 0; i < 10000; i++,ms++)
	{
		fcc.inc();
	}

    cout << endl;
    fcc.PrintTimes();
	fcc.ChangeAmp(0, -1.0);
    fcc.ChangeAmp(2, -1.0);
    
    fcc.DrawTimes(deqFactors);
    
    cout << endl;
    fcc.PrintState();
    fcc.SetZeroAlarm(0, true);
    try
    {
        for (int i = 0; i < 1000; i++,ms++)
	    {
            fcc.ActualizeMainTime(0, ms);
		    fcc.inc();
	    }
        cout << endl;
        fcc.PrintTimes();
        cout << endl;
        for (int i = 0; i < 100000; i++,ms++)
	    {
            fcc.ActualizeMainTime(0, ms);
		    fcc.inc();
	    }
    
        fcc.PrintState();
        cout << endl;
        fcc.PrintTimes();
        cout << endl;
    }
    catch (const std::exception & ex)
    {
        cout << ex.what() << " at: " << (double)ms/1000.0 << endl;
        cout << endl;
        fcc.PrintTimes();
        fcc.DrawTimes(deqFactors);
        //cin.get();
        deqFactors->push_back(deqF);
        return;
    }
	cout << endl;
    fcc.PrintTimes();
    deqFactors->push_back(deqF);

    

}

void Test2()
{
    CFocos f;
    for (int i = 0; i < 30; i++)
	{

		f.NewFoco();
	}
}


int main(int argc, char* argv[])
{
    double t = 1/sqrt(1-(0.5*0.5)/1);

    std::deque<std::deque<std::pair<double, double> >> deqFactors;

	Test1(&deqFactors);

    HPDF_Page page;
    HPDF_Font font;
    HPDF_Doc pdf;
    HPDF_Outline root;

    InitPdf(&pdf,&root);
    page = CreateNewPage("Una",pdf,root);
	font = HPDF_GetFont (pdf, "Courier-Bold", NULL);
	HPDF_Page_SetFontAndSize (page, font, 10);

    HPDF_REAL width = HPDF_Page_GetWidth (page);


    
    std::deque<std::pair<double, double> > deqF;
    deqF.push_back(std::make_pair(10,1.154));
    deqF.push_back(std::make_pair(-1,.866));

    int i=1;
    for (std::deque<std::deque<std::pair<double, double> >>::iterator it = deqFactors.begin();it!=deqFactors.end();++it,++i)
    {
        DrawTimer(&pdf,&page, i*width/8, *it,"Tierra");
    }





    HPDF_SaveToFile (pdf, "focos.pdf");
	HPDF_Free (pdf); 



    

	return 0;
}


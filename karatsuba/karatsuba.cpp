// karatsuba.cpp : Defines the entry point for the console application.
//



// fast Karatsuba multiplication
// 21 Jan 1999, Carl Burch, cburch@cmu.edu
//
// This program implements a reasonably efficient multiplication
// algorithm (Karatsuba multiplication) and times it against the
// traditional grade-school technique.
//
// (c) 1999, Carl Burch
// This may not be copied without retaining this copyright notice,
// and it may not be distributed in modified form.
#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <windows.h>
#include <time.h>
#include <math.h>
#include "gmp.h"
//#include "longmath.h"
#include "lm8.h"




char *walsh[] = {
	{"0000000000000000"},
	{"0101010101010101"},
	{"0011001100110011"},
	{"0110011001100110"},
	{"0000111100001111"},
	{"0101101001011010"},
	{"0011110000111100"},
	{"0110100101101001"},
	{"0000000011111111"},
	{"0101010110101010"},
	{"0011001111001100"},
	{"0110011010011001"},
	{"0000111111110000"},
	{"0101101010100101"},
	{"0011110011000011"},
	{"0110100110010110"}};

char *mrog[]={

	{"10111000101110001011100010111000"},
	{"01010000010100000101000001010000"},
	{"11001100110011001100110011001100"},
	{"00100100001001000010010000100100"},
	{"10000010100000101000001010000010"},
	{"01101010011010100110101001101010"},
	{"11110110111101101111011011110110"},
	{"00011110000111100001111000011110"},
	{"10111000010001111011100001000111"},
	{"01010000101011110101000010101111"},
	{"11001100001100111100110000110011"},
	{"00100100110110110010010011011011"},
	{"10000010011111011000001001111101"},
	{"01101010100101010110101010010101"},
	{"11110110000010011111011000001001"},
	{"00011110111000010001111011100001"},
	{"10111000101110000100011101000111"},
	{"01010000010100001010111110101111"},
	{"11001100110011000011001100110011"},
	{"00100100001001001101101111011011"},
	{"10000010100000100111110101111101"},
	{"01101010011010101001010110010101"},
	{"11110110111101100000100100001001"},
	{"00011110000111101110000111100001"},
	{"10111000010001110100011110111000"},
	{"01010000101011111010111101010000"},
	{"11001100001100110011001111001100"},
	{"00100100110110111101101100100100"},
	{"10000010011111010111110110000010"},
	{"01101010100101011001010101101010"},
	{"11110110000010010000100111110110"},
	{"00011110111000011110000100011110"}};


int ac_mrog[32][32]={
{32,  0,   0, -16,   0, -16,   0,   0,  32,   0,   0, -16,   0, -16,   0,   0,  32,   0,   0, -16,   0, -16,   0,   0,  32,   0,   0, -16,   0, -16,   0,   0},
{32,  0,  16,   0,   0,   0,  16,   0,  32,   0,  16,   0,   0,   0,  16,   0,  32,   0,  16,   0,   0,   0,  16,   0,  32,   0,  16,   0,   0,   0,  16,   0},
{32,  0, -32,   0,  32,   0, -32,   0,  32,   0, -32,   0,  32,   0, -32,   0,  32,   0, -32,   0,  32,   0, -32,   0,  32,   0, -32,   0,  32,   0, -32,   0},
{32,  0,   0,  16,   0,  16,   0,   0,  32,   0,   0,  16,   0,  16,   0,   0,  32,   0,   0,  16,   0,  16,   0,   0,  32,   0,   0,  16,   0,  16,   0,   0},
{32,  0,  16,   0,   0,   0,  16,   0,  32,   0,  16,   0,   0,   0,  16,   0,  32,   0,  16,   0,   0,   0,  16,   0,  32,   0,  16,   0,   0,   0,  16,   0},
{32,-16,   0,   0,   0,   0,   0, -16,  32, -16,   0,   0,   0,   0,   0, -16,  32, -16,   0,   0,   0,   0,   0, -16,  32, -16,   0,   0,   0,   0,   0, -16},
{32,  0,   0,  16,   0,  16,   0,   0,  32,   0,   0,  16,   0,  16,   0,   0,  32,   0,   0,  16,   0,  16,   0,   0,  32,   0,   0,  16,   0,  16,   0,   0},
{32, 16,   0, -16, -32, -16,   0,  16,  32,  16,   0, -16, -32, -16,   0,  16,  32,  16,   0, -16, -32, -16,   0,  16,  32,  16,   0, -16, -32, -16,   0,  16},
{32,  8,   0,  -8,   0,   8,   0,  -8, -32,  -8,   0,   8,   0,  -8,   0,   8,  32,   8,   0,  -8,   0,   8,   0,  -8, -32,  -8,   0,   8,   0,  -8,   0,   8},
{32, -8,  16,  -8,   0,   8, -16,   8, -32,   8, -16,   8,   0,  -8,  16,  -8,  32,  -8,  16,  -8,   0,   8, -16,   8, -32,   8, -16,   8,   0,  -8,  16,  -8},
{32,  8, -16,  -8,   0,   8,  16,  -8, -32,  -8,  16,   8,   0,  -8, -16,   8,  32,   8, -16,  -8,   0,   8,  16,  -8, -32,  -8,  16,   8,   0,  -8, -16,   8},
{32, -8, -16,  24,   0, -24,  16,   8, -32,   8,  16, -24,   0,  24, -16,  -8,  32,  -8, -16,  24,   0, -24,  16,   8, -32,   8,  16, -24,   0,  24, -16,  -8},
{32,  8,   0,   8,   0,  -8,   0,  -8, -32,  -8,   0,  -8,   0,   8,   0,   8,  32,   8,   0,   8,   0,  -8,   0,  -8, -32,  -8,   0,  -8,   0,   8,   0,   8},
{32,-24,  16,  -8,   0,   8, -16,  24, -32,  24, -16,   8,   0,  -8,  16, -24,  32, -24,  16,  -8,   0,   8, -16,  24, -32,  24, -16,   8,   0,  -8,  16, -24},
{32,  8,   0,   8,   0,  -8,   0,  -8, -32,  -8,   0,  -8,   0,   8,   0,   8,  32,   8,   0,   8,   0,  -8,   0,  -8, -32,  -8,   0,  -8,   0,   8,   0,   8},
{32,  8,   0,  -8,   0,   8,   0,  -8, -32,  -8,   0,   8,   0,  -8,   0,   8,  32,   8,   0,  -8,   0,   8,   0,  -8, -32,  -8,   0,   8,   0,  -8,   0,   8},
{32,  4,   0, -12,   0,  -4,   0,  -4,   0,   4,   0,   4,   0,  12,   0,  -4, -32,  -4,   0,  12,   0,   4,   0,   4,   0,  -4,   0,  -4,   0, -12,   0,   4},
{32, -4,  16,  -4,   0,   4,   0,   4,   0,  -4,   0,  -4,   0,   4, -16,   4, -32,   4, -16,   4,   0,  -4,   0,  -4,   0,   4,   0,   4,   0,  -4,  16,  -4},
{32,  4, -24,  -4,  16,   4,  -8,  -4,   0,   4,   8,  -4, -16,   4,  24,  -4, -32,  -4,  24,   4, -16,  -4,   8,   4,   0,  -4,  -8,   4,  16,  -4, -24,   4},
{32, -4,  -8,  20,   0,  -4,   8,   4,   0,  -4,  -8,   4,   0, -20,   8,   4, -32,   4,   8, -20,   0,   4,  -8,  -4,   0,   4,   8,  -4,   0,  20,  -8,  -4},
{32,  4,   8,   4,   0,  -4,   8,  -4,   0,   4,  -8,   4,   0,  -4,  -8,  -4, -32,  -4,  -8,  -4,   0,   4,  -8,   4,   0,  -4,   8,  -4,   0,   4,   8,   4},
{32,-20,   8,  -4,   0,   4,  -8,   4,   0,  -4,   8,  -4,   0,   4,  -8,  20, -32,  20,  -8,   4,   0,  -4,   8,  -4,   0,   4,  -8,   4,   0,  -4,   8, -20},
{32,  4,   0,  12,   0,   4,   0,  -4,   0,   4,   0,  -4,   0, -12,   0,  -4, -32,  -4,   0, -12,   0,  -4,   0,   4,   0,  -4,   0,   4,   0,  12,   0,   4},
{32, 12,   0, -12, -16,  -4,   0,   4,   0,  -4,   0,   4,  16,  12,   0, -12, -32, -12,   0,  12,  16,   4,   0,  -4,   0,   4,   0,  -4, -16, -12,   0,  12},
{32,  4,   0, -12,   0,  -4,   0,  -4,   0,   4,   0,   4,   0,  12,   0,  -4, -32,  -4,   0,  12,   0,   4,   0,   4,   0,  -4,   0,  -4,   0, -12,   0,   4},
{32, -4,  16,  -4,   0,   4,   0,   4,   0,  -4,   0,  -4,   0,   4, -16,   4, -32,   4, -16,   4,   0,  -4,   0,  -4,   0,   4,   0,   4,   0,  -4,  16,  -4},
{32,  4, -24,  -4,  16,   4,  -8,  -4,   0,   4,   8,  -4, -16,   4,  24,  -4, -32,  -4,  24,   4, -16,  -4,   8,   4,   0,  -4,  -8,   4,  16,  -4, -24,   4},
{32, -4,  -8,  20,   0,  -4,   8,   4,   0,  -4,  -8,   4,   0, -20,   8,   4, -32,   4,   8, -20,   0,   4,  -8,  -4,   0,   4,   8,  -4,   0,  20,  -8,  -4},
{32,  4,   8,   4,   0,  -4,   8,  -4,   0,   4,  -8,   4,   0,  -4,  -8,  -4, -32,  -4,  -8,  -4,   0,   4,  -8,   4,   0,  -4,   8,  -4,   0,   4,   8,   4},
{32,-20,   8,  -4,   0,   4,  -8,   4,   0,  -4,   8,  -4,   0,   4,  -8,  20, -32,  20,  -8,   4,   0,  -4,   8,  -4,   0,   4,  -8,   4,   0,  -4,   8, -20},
{32,  4,   0,  12,   0,   4,   0,  -4,   0,   4,   0,  -4,   0, -12,   0,  -4, -32,  -4,   0, -12,   0,  -4,   0,   4,   0,  -4,   0,   4,   0,  12,   0,   4},
{32, 12,   0, -12, -16,  -4,   0,   4,   0,  -4,   0,   4,  16,  12,   0, -12, -32, -12,   0,  12,  16,   4,   0,  -4,   0,   4,   0,  -4, -16, -12,   0,  12}
};


unsigned __int64 modinv64( unsigned __int64 a, unsigned __int64 m ); // modular inverse



void cccc(void)
{
	double c,rpm,L,PI,S,t,Vm,Vkm,G,m,a,R,e,g,h;// a=(v*v)/R; 
	G = 6.667E-11;
	m = 5.98E24;
	PI = 3.1415926535;
	t = 2 * 3600;
	R = pow ( ((G*m*t*t)/(2*PI*2*PI)), 1.0 / 3.0 );
	h = R - 6378000;

	//R = 6378000;
	//R = 6378000+35800000;
	//h = 1682500;
	//R = 6378000+h;
	g = G*(m/(R*R));
	e = (2*PI*R);
	Vm = sqrt(g*R);
	t = (e/Vm)/3600;
	Vkm=(Vm*3600)/1000;
	a=(Vm*Vm)/R;


/*
					
	2.0 * 3600 =		2*PI*R
					-------------------------
					(sqrt((G*(m/(R*R)))*(R)))


								
	(G*m/R) * (t*3600)2 = (2*PI*R)2

	(t*3600)2 = R*(2*PI*R)2
				----------
				G*m
								

	G*m * (t*3600)2 = R*(2*PI*R)2



	G*m*((t*3600)(t*3600))
	---------------------- = R3
	(2*PI)*(2*PI)

*/
	

	
	

	


	// v = a*t;t = v/a
	// 2e = a * t^2; t = sqrt(2e/a);

	a = 0.5;
	e = 0;
	for (t = 0,e = 0;Vm<7033.0;t+=1.0)
	{
		Vm = a*t;	
		Vkm=(Vm*3600)/1000;
		e = 0.5*a*t*t;
		if (e>=1682500.0)
			continue;
	}
	/*	********************-----*	*/


	Vm = 10*t;
	Vkm=(Vm*3600)/1000;

//	p = p0 exp(-mgh/kt);

	//	Una pila de combustible de 10 kW que tenga un 
	//	rendimiento global del 45%, consume 7,40 Nm3 
	//	por hora de H2 (10·0,333/0,45), 
	//	o 0,667 kg/hora de H2 (10·0,033/0,45), y produce 6,00 kg 
	//	de agua a la hora (0,667·(18/2)). 

/*
	S = 50000.0;
	R = pow(((3.0*S)/(4.0*PI)),1.0/3.0);
*/


	g = 9.8;
	double t1 = 120;
	double mm = 5;
	double mc = 1000;
	double mT = mm + mc;

	double mcs = mc/t1;
	rpm = 60000;
	R = 15;
	L = (2 * PI * R)/100.0;
	double Va= L * (rpm/60.0);
	double Eg = (mm+mc)*g;
	double Ec = Va*(mc/t1);
	
	Vm = -g*t1 + Va * log((mT)/((mT)-mc));

	h = -(0.5*g*t1*t1) + 
				Va*t1*log(mT) + 
				((Va/mcs) * ((mT - (mcs*t1))*log(mT -(mcs*t1)) + mcs*t1 - mT*log(mT)));

	
	
	
	

	
	

	

	
	mm = 1;
	mc = 10000;
	double mi = mm+mc;
	t = 0.0;
	a = 1.2*g;
	Va = 2000;
	double ci;


	a = 2.0*g;
	Va = 3000;
	ci = 0;
	mi = 1000;
	mc = mi*0.95;

	for (t=1;mc>0;t+=1.0)
	{
		mT = mm + mc;
		c = mT*(1/((Va/a)+1));
		double c2 = (mT/((Va/a)));
		ci= ((mi*a)/(Va+a*t));
		mc-=c;
		
	}
	Vm = (a-g)*t;
	h = ((a-g)*t*t)/2.0;


	
	
	
	a = 3*g;
	h = 0;
	double hmax = 0,aa,aaa,tt;
	Va = 1500;
	mi = 1000;
	mc = mi*0.95;
	h=0;
	Vm = 0;
	for (t=1;mc>0;t+=1.0)
	{
		c = ((mi*a)/(Va+a*t));
		mc-=c;
		Vm+= (a-g);
		h+=((a-g)/2.0);
		if (h>hmax)
		{
			hmax=h;
			aa = a/9.8;
			tt = t;
		}
		//a+=0.20;
	}




	Vm = -g*t + Va * log((mi)/(mi-mc));

	h = -(0.5*g*t*t) + 
				Va*t*log(mi) + 
				((Va/mcs) * ((mi - (mcs*t))*log(mi -(mcs*t)) + mcs*t - mi*log(mi)));




	#define masaT 300.0
	rpm = 30000;
	R = 10;
	L = (2 * PI * R)/100.0;
	Va= L * (rpm/60.0);
	

	double maxa,maxh,maxVKm,maxt;
	maxt = maxa = maxh = maxVKm = 0;
	for (a = g;a<(10.0*g);a+=.01)
	{
		
		mi = masaT;
		t = 0;
		Vm = 0;
		while(mi>(masaT * 0.10))
		{
			mc = (mi*a*a)/(Va*Va);
			mi-=mc;
			
			Vm = (a-g)*t;
			Vkm=(Vm*3600)/1000;
			h = ((a-g)*t*t)/2.0;
			t++;
		}
		if (h>maxh)
		//if (Vkm>maxVKm)
		{
			maxh = h;
			maxa = a;
			maxVKm = Vkm;
			maxt = t;
		}
	}



	G = 6.6725985E-11;
	m = 5.98E24;
	PI = 3.1415926535;
	t = 2 * 3600;
	R = pow ( ((G*m*t*t)/(2*PI*2*PI)), 1.0 / 3.0 );
	g = G*(m/(R*R));
	
	double hbuscada = R - 6378000.0;
	double VmBuscada = sqrt(g*R);

	

	a = maxa;
	a = 2*9.8 ;
	mi = masaT;
	t = 0;
	Vm = 0;
	h = 0;
	int uno = 0;
	while(mi>(masaT*0.1))
	{
		R = 6378000.0 + h;
		if (h<hbuscada )
		{
			g = G*(m/(R*R));
		}

		mc = (mi*a*a)/(Va*Va);
		mi-=mc;
		
		Vm+= (a-g);
		Vkm=(Vm*3600)/1000;
		if (h<hbuscada )
		{
			h+=Vm;
		}
		if (Vkm>500)
		{
			if (uno==0)
			{
				uno=1;
				a = 9.8;
			}
		}
		if (h>=100000)
		{
			if (uno==1)
			{
				uno=2;
				a = 2*9.8;
			}
		}
		
		if (h>=hbuscada )
		{
			if (uno==2)
			{
				uno=3;
			}
		}

		if (Vm>=VmBuscada)
		{
			break;
		}
		t++;
	}
}



void ref()
{
	float fref = 14.31818;
	float PDfref,M,N,tasa_error,fs,MOut,NOut,FOut,error;
	float fb;
	unsigned int PD;
	float fmin = 100.0;
	float fmax = 220.0;


	//fb = 80.055535;
	//fb = 25.175;
	fb = 77.573;


	for (PD = 1;PD<=32;PD<<=1)
	{
		PDfref = fb * (float)PD;
		if ((PDfref>=fmin)&&(PDfref<fmax))
			break;
	}
	
	tasa_error = 1000;
	for (M=3;M<=257;M++)
	{
		for (N=3;N<=257;N++)
		{
			fs = (fref * (M/N))/PD;
			error = fabs(fs-fb);
			if (error<tasa_error)
			{
				tasa_error = fabs(fs-fb);
				MOut = M;
				NOut = N;
				FOut = fs;
			}
		}
	}
}



void chips()
{
	typedef struct 
	{
		int n,m,pd,vld;
	}TMN;
	TMN valores_mn;


	float eleva2[6]={1,2,4,8,16,32};
	float upd,um,un;
	float error;
	float result,rupd,rum,run,rvld;
	int a,b,c,vld;
	float fclk;
	float fout;
	float frecuenciaBuscada ,uTotalVertical , uTotalHorizontal , uFreqVertical;
		
		
	fclk=14318180;
	frecuenciaBuscada = 80055535;
	frecuenciaBuscada = 80132912;
	//frecuenciaBuscada = 25175000;
	//frecuenciaBuscada = uTotalVertical * uTotalHorizontal * uFreqVertical;

    
	error = frecuenciaBuscada/2;

	//TRACE1(" error %f\n",error);

	result=0;

	for (a=0;a<6;a++)
	{
		if (((frecuenciaBuscada*eleva2[a])>100000000) && ((frecuenciaBuscada*eleva2[a])<220000000))
		{
			upd=eleva2[a];
			for (b=3;b<257;b++)
			{
				for (c=3;c<257;c++)
				{
					for (vld=1;vld<4;vld+=3)
					{

						um=(float)b;
						un=(float)c;
						result=(fclk*um*vld)/(un*upd);

						if (frecuenciaBuscada>=result)
						{
							if ((frecuenciaBuscada-result)<error)
							{
								error=frecuenciaBuscada-result;
								rupd=upd;
								rum=um;
								run=un;
								rvld=vld;
								fout = result;
							}
						}
						else
						{
							if ((result-frecuenciaBuscada)<error)
							{
								error=result-frecuenciaBuscada;
								rupd=upd;
								rum=um;
								run=un;
								rvld=vld;
								fout = result;
							}
						}
					}
				}
			}
		}
	}

	valores_mn.m=rum;
	valores_mn.n=run;
	valores_mn.pd=rupd;
	valores_mn.vld=rvld;

	result=(fclk*valores_mn.m*valores_mn.vld)/(valores_mn.n*valores_mn.pd);

	error = 100*(frecuenciaBuscada-result)/frecuenciaBuscada;


	printf("FCLK      = %d\nF.Buscada = %d\nN         = %d\nM         = %d\nPD        = %d\nVLD       = %d\nF.Sal     = %d\nDif(Hz)   = %d\n",
		(int)fclk,
		(int)frecuenciaBuscada,
		valores_mn.n,
		valores_mn.m,
		valores_mn.pd,
		valores_mn.vld,
		(int)result,
		(int)(frecuenciaBuscada-result));


	printf("HSync = %f\n",(result/1032.0));


	


	valores_mn.m=233+2;
	valores_mn.n=19+2;
	valores_mn.pd=2;
	valores_mn.vld=1;

	result=(fclk*valores_mn.m*valores_mn.vld)/(valores_mn.n*valores_mn.pd);
	printf("??? = %f\n",result);
	printf("HSync ??? = %f\n",(result/1032.0));


}

unsigned __int64 modinv( unsigned __int64 a, unsigned __int64 m ) // modular inverse
// returns i in range 1..m-1 such that i*a = 1 mod m
// a must be in range 1..m-1
{
  unsigned __int64 j=1,i=0,b=m,c=a,x,y;
  while ( c != 0 )
  {
    x = b / c;
    y = b - x*c;
    b = c;
    c = y;
    y = j;
    j = i - j*x;
    i = y;
  }
  if ( i < 0 )
    i += m;
  return i;
}


void Div64(__int64 &RetCociente,__int64 &RetResto, __int64 &Dividendo,__int64 &Divisor)
{
	int count;
	__int64 Sumatorio;
	__int64 Temp;
	__int64 Cociente;
	__int64 Resto;

	Cociente = 0;
	Resto = Dividendo;
	Sumatorio = 1;
	
	for (count = 0;Sumatorio<Dividendo;)
	{
		count++;
		Resto>>=1;
		
		if (Resto<=Divisor)//if (Resto<=Divisor)
		{
			printf("%X\n",Cociente);
			printf("%X\n",Sumatorio);
			Cociente = Cociente + Sumatorio;
			printf("%X\n",Cociente);
			
			printf("%X\n",Divisor);
			printf("%X\n",Cociente);
			Temp = Divisor * Cociente;
			printf("%X\n",Temp);
			Resto = Dividendo - Temp;

			if (Resto<Divisor)
			{
				printf("%X\n",Resto);
				printf("%X\n",Cociente);
				break;
			}

			Sumatorio=1;
		}
		else
		{
			printf("%X\n",Sumatorio);
			Sumatorio<<=1;
			
		}
	}
}

unsigned __int64 exp_mod64(unsigned __int64 x, unsigned __int64 a, unsigned __int64 n) 
{
  unsigned __int64 value = 1;
  unsigned __int64 tmp;
  tmp = x % n;
  while (a > 0) 
  {
    if (a & 1) 
	{
      value = (value * tmp) % n;
    }
	tmp = (tmp * tmp);
	tmp = (tmp % n);
    //tmp = (tmp * tmp) % n;
    a = a>>1;
  }
  return value;
}


int main() 
{
/*
    int             a[6 * MAX_DIGITS]; // first multiplicand
    int             b[6 * MAX_DIGITS]; // second multiplicand

	int             c[6 * MAX_DIGITS]; // first multiplicand
    int             d[6 * MAX_DIGITS]; // second multiplicand

    int             r[6 * MAX_DIGITS]; // result goes here
*/
	LNUM	a,c,d;
	LNUM	b;
	double vv,aa,tt,xx,vvk,xxk,mm,ee;
	//cccc();	

/*	

	aa = 3*9.8;tt = 5*60;
	xx = 0.5 * aa * tt*tt;

	vv = aa * tt;
	vvk = (vv*3600.0)/1000.0;
	xxk = xx/1000.0;


	cccc();

	srand( (unsigned)time( NULL ) );


	int w[32][32];
	int r[64],i,x,y;

	for (y=0;y<32;y++)
	{
		for (x=0;x<32;x++)
		{
			(mrog[y][x]=='1') ? w[y][x]=1 : w[y][x]=-1;
		}
	}
	for (x=0;x<64;x++) r[x] = 0;

	for (y=1;y<32;y++)	
		for (x=0;x<32;x++)	
			r[x]+=w[y][x];


*/
/*	

	for (x=0;x<32;x++)	r[x]+=w[0][x];
	for (x=0;x<32;x++)	r[x]+=w[4][x];
	for (x=0;x<32;x++)	r[x]+=w[6][x];
	for (x=0;x<32;x++)	r[x]+=w[7][x];
	for (x=0;x<32;x++)	r[x]+=w[10][x];
	//for (x=0;x<32;x++)	r[x]+=(-1*(w[10][x]));
	for (x=0;x<32;x++)	r[x]+=w[12][x];
	for (x=0;x<32;x++)	r[x]+=w[13][x];
	for (x=0;x<32;x++)	r[x]+=w[22][x];
	for (x=0;x<32;x++)	r[x]+=w[30][x];

	for (x=1;x<33;x++)	r[x]+=w[22][x-1];
*/
/*
	for (x=0;x<32;x++)
	{
		i = (rand() & 3)-1;
		r[x]+=i;
	}
*/
	
/*

	int e=0;
	for (x=0;x<32;x++)
	{
		e+=r[x];
	}

	for (y=0;y<32;y++)
	{
		i=0;
		for (x=0;x<32;x++)
		{
			i+=(r[x]*w[y][x]);
		}

		//i+=e;
		
	}



*/	

	


	mpz_t az,bz,cz,dz;
	

	Init(a); 
	Init(b);
	Init(c);
	Init(d);
/*
	
	SetFromInt(a,-0x00644410);
	//SetFromInt(a,0);
	SetFromInt(b,-0x76545674);
	
	mpz_init_set_str (az,"-00644410",16);
	//mpz_init_set_str (az,"0",16);
	mpz_init_set_str (bz,"-76545674",16);
	mpz_init(cz);
	
	Add(c,a,b);
	mpz_add(cz,az,bz);
	pNum(a);
	pNum(b);
	pNum(c);
	gmp_printf("%08ZX\n",cz);
	mpz_clear(az);
	mpz_clear(bz);
	mpz_clear(cz);

*/
	char str[(MAX_DIGITS*2)+1];
	mpz_init(cz);
	sprintf(str,"fedcba9876543210fedcba9876543210");
	//sprintf(str,"fedcba9876543210");
	SetFromStr(a,str);
	mpz_init_set_str (az,str,16);
	
	sprintf(str,"fedcba9876543210fedcba9876543210");
	//sprintf(str,"fedcba9876543210");


	SetFromStr(b,str);
	mpz_init_set_str (bz,str,16);

	Mul(c,a,b);
	//Add(c,a,b);
	//Sbb(a,c,b);
	mpz_mul(cz,az,bz);
	//mpz_add(cz,az,bz);
	
	for (int i=63;i>=0;i--)
	{
		printf("%d",i/10);
	}
	printf("\n");
	for (i=63;i>=0;i--)
	{
		printf("%d",i%10);
	}
	printf("\n");
	pNum(c);
	gmp_printf("%0ZX\n",cz);

	const char HexChar[]="0123456789ABCDEF";

	
	srand( (unsigned)time( NULL ) );

	

	for (int ii=0;ii<250;ii++)
	{
		
		memset(str,0,(MAX_DIGITS*2)+1);
		mpz_init(cz);
		for( int i = 0;   i < MAX_DIGITS;i++ )
		{
			str[i] = HexChar[rand() & 0x0f];
		}
		str[i] = 0;
		SetFromStr(a,str);
		mpz_init_set_str (az,str,16);
		memset(str,0,(MAX_DIGITS*2)+1);
		for( i = 0;   i < MAX_DIGITS;i++ )
		{
			str[i] = HexChar[rand() & 0x0f];
		}
		str[i] = 0;


		
		SetFromStr(b,str);
		mpz_init_set_str (bz,str,16);

		Mul(c,a,b);
		mpz_mul(cz,az,bz);
		
		OutputNum(str,c);
		mpz_init_set_str (dz,str,16);

		if (mpz_cmp(cz,dz)!=0)
		{
			printf("Error\n");
			pNum(c);
			gmp_printf("%032ZX\n",cz);
			gmp_printf("%032ZX\n",dz);
			Mul(c,a,b);	
			
			
			exit(1);
		}
		
		mpz_clear(az);
		mpz_clear(bz);
		mpz_clear(cz);
		mpz_clear(dz);
		printf("%d\n",ii);
	}


	SetFromInt(a,0x556644);
	SetFromInt(b,0x1122);

	Div(c,d, a,b);

	pNum(c) ;
	pNum(d) ;
	
	SetFromInt(a,0x12345678);
	SetFromInt(b,0x111111);
	SetFromInt(c,0x4321);
	
	exp_mod(d, a,b,c);
	pNum(d);
	__int64 r64 = modinv64( 0x12345678, 0x4321);
	modinv( d ,a,c);
	pNum(d);
	

	mpz_init(az);
	mpz_init(bz);
	mpz_init(cz);
	mpz_init(dz);
	mpz_init_set_str (az,"12345678",16);
	mpz_init_set_str (bz,"111111",16);
	mpz_init_set_str (cz,"4321",16);
	mpz_powm( dz, az, bz, cz );
	gmp_printf("%08ZX\n",dz);


	return 0;
}






int CompareNumInt64(LNUM a,__int64 n)
{
	char *p;
	p = (char *)&n;
	return (memcmp(a->buf,p,a->Len));

}





/*
10111000101110001011100010111000
01010000010100000101000001010000
11001100110011001100110011001100
00100100001001000010010000100100
10000010100000101000001010000010
01101010011010100110101001101010
11110110111101101111011011110110

4  4  4  3  3  3  3  0  4  4  4  3  3  3  3  0  4  4  4  3  3  3  3  0  4  4  4  3  3  3  3  0
3  3  3  4  4  4  4  0  3  3  3  4  4  4  4  0  3  3  3  4  4  4  4  0  3  3  3  4  4  4  4  0



1  1  1 -1 -1 -1 -1 -7  1  1  1 -1 -1 -1 -1 -7  1  1  1 -1 -1 -1 -1 -7  1  1  1 -1 -1 -1 -1 -7
1 -1  1  1  1 -1 -1 -1  1 -1  1  1  1 -1 -1 -1  1 -1  1  1  1 -1 -1 -1  1 -1  1  1  1 -1 -1 -1

1 -1  1 -1 -1  1  1  7  1 -1  1 -1 -1  1  1  7  1 -1  1 -1 -1  1  1  7  1 -1  1 -1 -1  1  1  1

10100111101001111010011110100111
*/

/*
int IntCompare(LNUM a,int b)
{
	char *pb;
	pa = a->buf[0];
	pa|= (a->buf[1]<<8);
	pa|= (a->buf[2]<<16);
	pa|= (a->buf[3]<<24);

	return (pa-b);
}
*/


unsigned __int64 modinv64( unsigned __int64 a, unsigned __int64 m ) // modular inverse
// returns i in range 1..m-1 such that i*a = 1 mod m
// a must be in range 1..m-1
{
  unsigned __int64 j=1,i=0,b=m,c=a,x,y;
  while ( c != 0 )
  {
    x = b / c;
    y = b - x*c;
    b = c;
    c = y;
    y = j;
    j = i - j*x;
    i = y;
  }
  if ( i < 0 )
    i += m;
  return i;
}



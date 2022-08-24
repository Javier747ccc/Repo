#include <stdio.h>
#include <math.h>
#include <windows.h>
#include "lm.h"


#define CalcFHTLen(_NumLen) ((((_NumLen)*1)*2)/1)
#define MY_PI      3.1415926535897932384626434
#define MY_SQRT_2  0.7071067811865475244008443621
#define MY_SQRT2   1.41421356237309504880



static double BASE;
static double SineTable[80];
static double *FHTNum1=NULL, *FHTNum2=NULL;
double MaxFFTError;




#define NextTrigPow(Pr,Pi,Nr,Ni) {double temp;temp = Pr;Pr = Pr * Nr - Pi   * Ni + Pr;Pi = Pi * Nr + temp * Ni + Pi;}
#define FHT_F2Butterfly(N1,N2,C,S){double D1,D2;int i1=N1, i2=N2;D1=Left[i1];D2=Left[i2];{double temp;Left[i1] =D1+(temp=Right[i1]);D1=D1-temp;Left[i2] =D2+(temp=Right[i2]);D2=D2-temp;}Right[i1]=D1*(C)+D2*(S);Right[i2]=D1*(S)-D2*(C);}



static int Log2(int Num)
{
	int x=-1;
	if (Num==0) return 0;
	while (Num) {x++;Num/=2;}
	return x;
}


void FHT_F(double *Data,int Len)
/*
** Recursive Decimation in Frequency style Fast Hartley Transform
*/
{
	int x,Len2,Len4;
	double Sin0,Cos0;
	double Sin,Cos;
	double *Left,*Right;
	double d0,d1,d2,d3;
	double d02,d13;
	double t1,t2;
	double sq;

	Len/=2;Left=&Data[0];Right=&Data[Len];
	if (Len==2)
	{
		d0=Data[0]; d1=Data[1];
		d2=Data[2]; d3=Data[3];
		d02=d0+d2; d13=d1+d3;
		Data[0]=d02+d13; Data[1]=d02-d13;

		d02=d0-d2; d13=d1-d3;
		Data[2]=d02+d13; Data[3]=d02-d13;
		return;
	}

	t1=Left[0];t2=Right[0];
	Left[0]=t1+t2;Right[0]=t1-t2;
	t1=Left[Len/2];t2=Right[Len/2];
	Left[Len/2]=t1+t2;Right[Len/2]=t1-t2;

	x=Log2(Len)+1;
	Sin0=SineTable[x];
	Cos0=SineTable[x+1];Cos0=-2.0*Cos0*Cos0;
	Sin=Sin0;Cos=1.0+Cos0;

	Len2=Len/2;
	Len4=Len/4;
	for (x=1;x<Len4;x++)
	{
		FHT_F2Butterfly(x,Len-x,Cos,Sin);
		FHT_F2Butterfly(Len2-x,Len2+x,Sin,Cos);
		NextTrigPow(Cos,Sin,Cos0,Sin0);
	}

	/* Now do the two Len/4 points the loop missed */
	if (Len4)
	{
		sq=MY_SQRT_2; /* variable allows optimizations */
		FHT_F2Butterfly(Len4,Len-Len4,sq,sq);
	}

	if (Len>=2) FHT_F(Left, Len);
	if (Len>=2) FHT_F(Right,Len);
}


/* Macro for the DiT FHT */
#define FHT_T2Butterfly(N1,N2,C,S) {double Rx,Ri;int i1=N1,i2=N2;Rx=Right[i1];Ri=Right[i2];{double cas1,Lx;cas1=Rx*(C)+Ri*(S);Lx=Left[i1];   Left[i1]  = Lx+cas1;Right[i1] = Lx-cas1;}{double cas2,Li;cas2=Rx*(S)-Ri*(C);Li=Left[i2];Left[i2]  = Li+cas2;Right[i2] = Li-cas2;}}

/* Macro for the DiT FHT */
#define FHT_T1Butterfly(N1,N2,C,S) {int i1=N1,i2=N2;double cas1=Right[i1]*(C)+Right[i2]*(S);double temp=Left[i1];Left[i1] = temp + cas1;Right[i2]= temp - cas1;}

void FHT_T(double *Data,int Len)
/*
** recursive Decimation in Time style Fast Hartley Transform
*/
{
	int x,Len2,Len4;
	double Sin0,Cos0;
	double Sin,Cos;
	double *Left,*Right;
	double d45,d67,sd0123,dd0123;
	double ss0123,ds0123,ss4567,ds4567;
	double s01,s23,d01,d23;
	double s45,s67;
	double sq;

	Len/=2;Right=&Data[Len];Left=&Data[0];
	if (Len==4)
	{
		d01 = Data[0] - Data[1];
		s01 = Data[0] + Data[1];
		d23 = Data[2] - Data[3];
		s23 = Data[2] + Data[3];
		ds0123 = (s01 - s23);
		ss0123 = (s01 + s23);
		dd0123 = (d01 - d23);
		sd0123 = (d01 + d23);
		s45 = Data[4] + Data[5];
		s67 = Data[6] + Data[7];
		d45 = Data[4] - Data[5];
		d67 = Data[6] - Data[7];
		ds4567 = (s45 - s67);
		ss4567 = (s45 + s67);
		Data[4] = ss0123 - ss4567;
		Data[0] = ss0123 + ss4567;
		Data[6] = ds0123 - ds4567;
		Data[2] = ds0123 + ds4567;
		d45 *= MY_SQRT2;
		d67 *= MY_SQRT2;
		Data[5] = sd0123 - d45;
		Data[1] = sd0123 + d45;
		Data[7] = dd0123 - d67;
		Data[3] = dd0123 + d67;
		return;
	}

	FHT_T(&Left[0], Len);
	FHT_T(&Right[0],Len);

	/* Do the special x=0 loop below. */
	FHT_T1Butterfly(0,0,1.0,0.0);

	x=Log2(Len)+1;
	Sin0=SineTable[x];
	Cos0=SineTable[x+1];Cos0=-2.0*Cos0*Cos0;
	Sin=Sin0;Cos=1.0+Cos0;

	Len2=Len/2;
	Len4=Len/4;
	for (x=1;x<Len4;x++)
	{
		FHT_T2Butterfly(x,Len-x,Cos,Sin);
		FHT_T2Butterfly(Len2-x,Len2+x,Sin,Cos);
		NextTrigPow(Cos,Sin,Cos0,Sin0);
	}

	/* Now do the two Len/4 points the loop missed */
	if (Len4)
	{
		sq=MY_SQRT_2; /* variable allows optimizations */
		FHT_T2Butterfly(Len4,Len-Len4,sq,sq);
	}
	/* Now do the Len/2 point the loop couldn't do. */
	if (Len2)
		FHT_T1Butterfly(Len2,Len2,0.0,1.0);
}







void InitFHT(unsigned int Len)
{
	int x;
	unsigned int P;
	SineTable[0]=-9.0;
	x=1;P=1;
	while (P<=Len*4)
	{
		SineTable[x]=sin(MY_PI/P);
		P*=2;
		x++;
	}
	
}

void InitFFT(unsigned long int Len,int Base,int BaseDig)
{
	int Bytes;


	BASE  = Base;
	BASE  = 0x100;
	MaxFFTError = -BASE;

	Bytes = sizeof(double)*CalcFHTLen(Len);

	FHTNum1=(double*)malloc(Bytes);
	FHTNum2=(double*)malloc(Bytes);

	if ((FHTNum1==NULL) || (FHTNum2==NULL))
	{
		exit(0);
	}


	if (BaseDig > 4)
	{
		exit(0);
	}

	
	InitFHT(Len);
}



static void Convolution(double *FHTNum1, double *FHTNum2,size_t Len2)
{
	double h1p,h1m,h2p,h2m;
	double s1,d1;
	size_t Step=2;
	size_t Step2=Step*2;
	double dn=0.5 / Len2;
	size_t x,y;

	while (Step < Len2)
	{
		for (x=Step,y=Step2-1;x<Step2;x+=2,y-=2)
		{
			h1p=FHTNum1[x];
			h1m=FHTNum1[y];
			s1=h1p+h1m;
			d1=h1p-h1m;
			h2p=FHTNum2[x];
			h2m=FHTNum2[y];
			FHTNum1[x]=(h2p*s1+h2m*d1)*dn;
			FHTNum1[y]=(h2m*s1-h2p*d1)*dn;
		}
		Step*=2;
		Step2*=2;
	}
	FHTNum1[0]   = FHTNum1[0]   * 2.0 * dn * FHTNum2[0];
	FHTNum1[1]   = FHTNum1[1]   * 2.0 * dn * FHTNum2[1];
}



void FFTMul(buftype *prod, buftype *a, buftype *b, int Len)
{
	int x, Len2 = Len * 2;
	double Carry, RawPyramid, Pyramid, PyramidError;
	double inv;

	for (x = 0; x < Len; x++)    FHTNum1[x] = a[x];
	for (x = Len; x < Len2; x++) FHTNum1[x] = 0.0;
	
	FHT_F(FHTNum1,Len2);
	
	if (a == b)
	{
		Convolution(FHTNum1,FHTNum1,Len2);
	}
	else
	{
		for (x = 0; x < Len; x++)    FHTNum2[x] = b[x];
		for (x = Len; x < Len2; x++) FHTNum2[x] = 0.0;
		FHT_F(FHTNum2,Len2);
		
		Convolution(FHTNum1,FHTNum2,Len2);
	}

	/*
	** Now do an Inverse FFT
	*/
	
	
	FHT_T(FHTNum1,Len2);
	

	/*
	** Now round the results, and release our carries, and store
	** the results in the 'prod' array.  Also do the normalization
	** we didn't do in the FHT.  And, as usual, it's slightly faster
	** to multiply by the reciprocal, instead of doing the division.
	*/
	Carry = 0.0;
	inv =1.0; /* already normalized during scrambled convolution */
	
	for (x = 0; x < Len2; x++)
	{
		RawPyramid = FHTNum1[x] * inv + Carry;
		Pyramid = floor(RawPyramid + 0.5);
		PyramidError = fabs(RawPyramid - Pyramid);
		if (PyramidError > MaxFFTError)
		{
			MaxFFTError = PyramidError;
		}
		Carry = floor(Pyramid / BASE);
		prod[x] = (buftype)(Pyramid - Carry * BASE);
	}
}





void DeInitFFT(unsigned long int Len)
{
	free(FHTNum1);free(FHTNum2);
}

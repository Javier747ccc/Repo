#include <stdio.h>
#include <stdlib.h>
#include <afx.h>
#include "rsa.h"

static int nnews = 0;

CLongMath::CLongMath ()
{
	nnews++;

	int nMaxCLen,i;
	share = 0;
	nMaxCLen = MaxcharLenDefault;
	(nMaxCLen%2==0) ? (nMaxLen = MaxcharLenDefault/2) 
		:(nMaxLen = MaxcharLenDefault/2)+1;
	nMaxLen = (MaxcharLenDefault/2)+1;
	for (i=0;i<2;i++)
	{
		Regs[i] = new ULONG[nMaxLen];
		memset(Regs[i],0,nMaxLen*sizeof(ULONG));
		wLin = new WORD[nMaxLen];
		nLen[i] = 0;
		Negative[i] = 0;
	}
	error = 0;
}

CLongMath::CLongMath (int Numero) // copy constructor
{
	nnews++;

	share = 0;
	nMaxLen = MaxcharLenDefault;
	if (Numero<0x10000)
		nLen[1] = 1;
	else
		nLen[1] = 2;
	Regs[0] = new ULONG[nMaxLen];
	Regs[1] = new ULONG[nMaxLen];
	
	wLin = new WORD[nMaxLen];
	if (Numero>=0)
	{
		Negative[1] = 1;
	}
	else
	{
		Numero*=-1;
		Negative[1] = -1;
	}
	Regs[1][0] = LOWORD(Numero);
	Regs[1][1] = HIWORD(Numero);
	error = 0;

#ifdef _DEBUG
	GeneraDisplay('H');
#endif
}


CLongMath ::CLongMath( const CLongMath & x ) // copy constructor
{
	nnews++;


	share = 0;
	nMaxLen = x.nMaxLen;
	nLen[0] = x.nLen[0];
	Regs[0] = new ULONG[nMaxLen];
	nLen[1] = x.nLen[1];
	Regs[1] = new ULONG[nMaxLen];
	memcpy(Regs[1],x.Regs[1],nMaxLen*sizeof(ULONG));
	wLin = new WORD[nMaxLen];
	Negative[0] = x.Negative[0];
	Negative[1] = x.Negative[1];
	error = 0;

#ifdef _DEBUG
	GeneraDisplay('H');
#endif

}


CLongMath::~CLongMath()
{
	nnews--;
	if (share)
		share--;
	else
	{
		delete Regs[0];
		delete Regs[1];
		delete wLin;
	}

}

char *CLongMath::resultado(int nRegOut)
{
	char *pc;
	for (int i=0;i<nMaxLen;i++)
	{
		wLin[i] = (WORD)Regs[nRegOut][i];
	}
	pc = (char *)wLin;
	pc = strrev(pc);


	return pc;
}

void CLongMath::CargaLinea(int nRegIn ,unsigned char *Linea)
{
	WORD *wLinea;
	unsigned char *pc;
	int i,j,len;

	wLinea = (WORD *)Linea;
	len = strlen((char*)Linea);

	nLen[nRegIn] = (len/sizeof(ULONG))+1;

	memset(wLin,0,nMaxLen*sizeof(WORD));
	pc = (unsigned char *)wLin;
	memset(Regs[nRegIn],0,nMaxLen*sizeof(ULONG));

	for (i = len-1,j=0;i>=0;i--,j++)
	{
		pc[i] = Linea[j];
	}





	for (i=0;i<nMaxLen,((wLinea[i]&0xff)!=0);i++)
	{
		Regs[nRegIn][i] = (ULONG)wLin[i];
		if (((wLin[i]&0xff00)==0)||((wLin[i]&0xff)==0))
			break;
	}
}



/*


BOOL CLongMath::ShL()
{
	unsigned int carry,Numero;
	int L;

	carry = 0;
	for (L=0;L<nLen[1];L++)
	{
		Numero = Regs[1][L];
		Numero<<=1; 
		Numero|=carry;
		carry = HIWORD(Numero);
		Numero&=0xffff;
		Regs[1][L] = Numero;
	}
	if (carry>0)
	{
		Regs[1][L]|= carry;
		nLen[1]++;
	}
	return TRUE;
}
*/
BOOL CLongMath::ShR(int n)
{
	unsigned int Numero1,Numero2;
	int L,Top,d,m;
	int N1;

	d = n/16;
	m = n%16;

	N1 = 0x123456;
	N1>>=5;


	Top = nLen[1];
	for (L=0;L<Top;L++)
	{
		Numero1 = (Regs[1][L+d]>>m);
		Numero2 = (Regs[1][L+d+1]);
		Numero2&=((1<<m)-1);
		Numero2<<=(16-m);
		Numero1|= Numero2;
		Regs[1][L] = Numero1;
	}
	QuitaCeros();

	return TRUE;
}


CLongMath& CLongMath::operator =(const LONG& x)
{
	LONG y;
	if (x<0)
	{
		y=~x+1;
		Negative[1]=-1;
	}
	else
	{
		y = x;
		Negative[1]=1;
	}
	Regs[1][0] = LOWORD(y);
	Regs[1][1] = HIWORD(y);

	if (y<0x10000)
		nLen[1] = 1;
	else
		nLen[1] = 2;

#ifdef _DEBUG
	GeneraDisplay('H');
#endif

	return *this;
}
#ifdef _DEBUG
void CLongMath::GeneraDisplay(char Tipo)
{
	char StrTemp[8];
	int i;

	sprintf(Caracteres,"");
	if (nLen[1]<=0)
		return;

	if (Negative[1]>0)
		sprintf(Caracteres,"+");
	if (Negative[1]<0)
		sprintf(Caracteres,"-");

	switch(Tipo)
	{
		case 'H':
		case 'h':
			for (i=nLen[1]-1;i>=0;i--)
			{
				sprintf(StrTemp,"%X",Regs[1][i]);
				strcat(Caracteres,StrTemp);
			}
			break;
		case 'D':
		case 'd':
			break;
	}
}

#endif

CLongMath& CLongMath::operator =(const CLongMath& x)
{
	memcpy(Regs[1],x.Regs[1],nMaxLen*sizeof(ULONG));
	nLen[1] = x.nLen[1];
	Negative[1] = x.Negative[1];
#ifdef _DEBUG
	GeneraDisplay('H');
#endif
	return *this;
}

CLongMath::AddUnsigned(const CLongMath& x)
{
	unsigned int carry;
	carry = 0;
	int len = max(nLen[1],x.nLen[1]);
	nLen[1] = len;
	for (int i=0;i<len;i++)
	{
		Regs[1][i] = Regs[1][i] + x.Regs[1][i] + carry;
		carry = HIWORD(Regs[1][i]);
		Regs[1][i]&=0xffff;
	}
	if (carry>0)
	{
		Regs[1][i] = carry;
		nLen[1]++;
	}
}

CLongMath::SubUnsigned(const CLongMath& x)
{
	unsigned int carry;
	carry = 0;
	int len = max(nLen[1],x.nLen[1]);
	nLen[1] = len;
	for (int i=0;i<len;i++)
	{
		unsigned int n = Regs[1][i];
		n+=((~x.Regs[1][i])-carry+1);
		carry = HIWORD(n);
		if (carry>0)
		{
			carry=1;
		}
		Regs[1][i] = n;
		Regs[1][i]&=0xffff;
	}
	if (carry>0)
	{
		error = 1;
	}
	QuitaCeros();

}


CLongMath::QuitaCeros()
{
	int i;
	for (i=nLen[1]-1;i>=0;i--)
	{
		if (Regs[1][i]!=0)
			break;
		nLen[1]--;
	}	
#ifdef _DEBUG
	GeneraDisplay('H');
#endif

}

int CLongMath::CmpSigned( const CLongMath &x)
{
	if ( Negative[1] == x.Negative[1] )
	{
		// Tienen el mismo signo
		return (CmpUnsigned(x));
	}
	else
	{
		// Tienen distinto signo
		return Negative[1];
	}
}

int CLongMath::CmpUnsigned( const CLongMath &x)
{
	
	if (nLen[1]>x.nLen[1])
		return +1;
	else if (nLen[1]<x.nLen[1])
		return -1;

	// Tienen la misma longitud

	int len = nLen[1];
	for (int i=len-1;i>=0;i--)
	{
		if (Regs[1][i]==x.Regs[1][i])
			continue;
		if (Regs[1][i]>x.Regs[1][i])
			return +1;
		return -1;
	}
	return 0;
}

CLongMath &CLongMath::operator -=(const CLongMath& x)
{
	CLongMath Temp = x;
	int compare;

	if ( Negative[1] == Temp.Negative[1] )
	{
		// Tienen el mismo signo
		// -1-(-2) = 1
		// -2-(-1) = -1
		// +1-(+2) = -1
		// +2-(+1) = 1
		
		compare = CmpUnsigned( Temp );
		switch(compare)
		{
			case 1:
			// El primer numero eliminando el signo es mayor
			// El signo del resultado es el del primero
				// -2-(-1) = -1
				// +2-(+1) = 1
				SubUnsigned(Temp);
				break;
			case 0:
				// sabemos que (*this-x)==0
				nLen[1]=0;
					
				break;
			case -1:
			// El segundo numero eliminando el signo es mayor
			// El signo del resultado es el del segundo
				// -1-(-2) = 1
				// +1-(+2) = -1

				Temp = *this;
				*this = x;
				SubUnsigned(Temp);
				Negative[1]*=-1;
				break;
		}
	}
	else // ( Negative[1] != Temp.Negative[1] )
	{
		// +1-(-2) = 3
		// -1-(+2) = -3
		// 2-(-1) = 3
		// -2-(1) = -3
		AddUnsigned(Temp);
		
	}
	QuitaCeros();

	return *this;
}


CLongMath &CLongMath::operator +=(const CLongMath& x)
{
	CLongMath Temp = x;
	int compare;

	if ( Negative[1] == Temp.Negative[1] )
	{
		// Tienen el mismo signo
		// -1+(-2)	= -3
		// 1+2		= 3
		// -2+(-1)	= -3
		// 2+1		= 3
		AddUnsigned(Temp);
	}
	else // ( Negative[1] != Temp.Negative[1] )
	{
		// 1+(-2)	= -1
		// -1+(2)	= 1
		// -2+(-1)	= -3
		// 2+1		= 3
		compare = CmpUnsigned( Temp );
		switch(compare)
		{
			case 1:
			// El primer numero eliminando el signo es mayor
			// El signo del resultado es el del primero
				SubUnsigned(Temp);
				break;
			case 0:
				// sabemos que (*this-x)==0
				nLen[1]=0;
					
				break;
			case -1:
			// El segundo numero eliminando el signo es mayor
			// El signo del resultado es el del segundo
				Temp = *this;
				*this = x;
				SubUnsigned(Temp);
				break;
		}
	}
	QuitaCeros();
	return *this;
}

CLongMath &CLongMath::operator *=(const CLongMath& x)
{
	unsigned int n,carry,carry2,Numero;
	int L1,L2;

	
	memset(Regs[0],0,nMaxLen*sizeof(ULONG));
	carry = 0;
	nLen[0] = nLen[1]+x.nLen[1];
	for (L2=0;L2<x.nLen[1];L2++)
	{
		Numero = x.Regs[1][L2];
		carry = 0;
		for (L1=0;L1<nLen[1];L1++)
		{
			n = ((Regs[1][L1] * Numero) + carry);
			carry = HIWORD(n);
			n&=0xffff;


			Regs[0][L1+L2]+= n;
			carry2 = HIWORD(Regs[0][L1+L2]);
			Regs[0][L1+L2]&=0xffff;
			Regs[0][L1+L2+1]+= carry2;

		}
		if (carry>0)
		{
			Regs[0][L1+L2]+= carry;
			nLen[0]++;
		}
	}
	memcpy(Regs[1],Regs[0],nLen[0]*sizeof(ULONG));
	nLen[1] = nLen[0];
	Negative[1] = Negative[1] * x.Negative[1];
	QuitaCeros();
	return *this;
}

CLongMath &CLongMath::operator /=(const CLongMath& divisor)
{
	CLongMath *Dividendo = this;
	CLongMath Divisor = divisor;
	CLongMath Cociente;
	CLongMath Resto = *Dividendo;
	CLongMath Sumatorio;
	

	int count = 0;

	for (Sumatorio=1,Cociente = 0;Sumatorio < *Dividendo;)
	{
		count++;
		Resto>>=1;
		if (Resto<=Divisor)
		{
			Cociente+=Sumatorio;
			Resto = *Dividendo - (Divisor*Cociente);
			if (Resto<Divisor)
			{
				break;
			}

			Sumatorio = 1;
		}
		else
		{
			Sumatorio*=2;
		}
	}


	memcpy(Regs[1],Cociente.Regs[1],nMaxLen*sizeof(ULONG));
	nLen[1] = Cociente.nLen[1];
	QuitaCeros();
	Negative[1] = Cociente.Negative[1] * Divisor.Negative[1];
	
	return *this;
}

CLongMath &CLongMath::operator %=(const CLongMath& divisor)
{
	CLongMath *Dividendo = this;
	CLongMath Divisor = divisor;
	CLongMath Cociente;
	CLongMath Resto = *Dividendo;
	CLongMath Sumatorio;
	int count = 0;

	for (Sumatorio=1,Cociente = 0;Sumatorio < *Dividendo;)
	{
		count++;
		Resto>>=1;
		if (Resto<=Divisor)
		{
			Cociente+=Sumatorio;
			Resto = *Dividendo - (Divisor*Cociente);
			if (Resto<Divisor)
			{
				break;
			}

			Sumatorio = 1;
		}
		else
		{
			Sumatorio*=2;
		}
	}


	memcpy(Regs[1],Resto.Regs[1],nMaxLen*sizeof(ULONG));
	nLen[1] = Resto.nLen[1];
	QuitaCeros();

	return *this;
}



CLongMath operator +( const CLongMath& x, const CLongMath& y )
{
	CLongMath result = x;
	result += y;
	return result;
}

CLongMath operator -( const CLongMath& x, const CLongMath& y )
{
	CLongMath result = x;
	result -= y;
	return result;
}


CLongMath operator *( const CLongMath& x, const CLongMath& y )
{
	CLongMath result = x;
	result *= y;
	return result;
}

CLongMath operator /( const CLongMath& x, const CLongMath& y )
{
	CLongMath result = x;
	result /= y;
	return result;
}

CLongMath operator exp( const CLongMath& x, const CLongMath& y )
{
	CLongMath result = x;
	CLongMath i = 0;
	while (1)
	{
		result *= y;
		i+=1;
	}
	return result;
}



int operator >( CLongMath& x, CLongMath& y )
{
	int n;
	n = x.CmpSigned(y);
	if (n>0)
		return 1;
	else
		return 0;
}





int operator <( CLongMath& x, CLongMath& y )
{
	int n;
	n = y.CmpSigned(x);
	if (n>0)
		return 1;
	else
		return 0;
}



int operator >=( CLongMath& x, CLongMath& y )
{
	int n;
	n = x.CmpSigned(y);
	if (n>=0)
		return 1;
	else
		return 0;
}

int operator <=( CLongMath& x, CLongMath& y )
{
	int n;
	n = y.CmpSigned(x);
	if (n>=0)
		return 1;
	else
		return 0;
}


int operator ==( CLongMath& x, CLongMath& y )
{
	int n;
	n = x.CmpSigned(y);
	if (n==0)
		return 1;
	else
		return 0;
}


/*


int operator >( const CLongMath& x, const CLongMath& y )
{
	CLongMath X = x;
	int n;
	n = X.CmpSigned(y);
	if (n>0)
		return 1;
	else
		return 0;
}





int operator <( const CLongMath& x, const CLongMath& y )
{
	CLongMath Y = y;
	int n;
	n = Y.CmpSigned(x);
	if (n>0)
		return 1;
	else
		return 0;
}



int operator >=( const CLongMath& x, const CLongMath& y )
{
	CLongMath X = x;
	int n;
	n = X.CmpSigned(y);
	if (n>=0)
		return 1;
	else
		return 0;
}

int operator <=( const CLongMath& x, const CLongMath& y )
{
	CLongMath Y = y;
	int n;
	n = Y.CmpSigned(x);
	if (n>=0)
		return 1;
	else
		return 0;
}


int operator ==( const CLongMath& x, const CLongMath& y )
{
	CLongMath X = x;
	int n;
	n = X.CmpSigned(y);
	if (n==0)
		return 1;
	else
		return 0;
}
*/
CLongMath &CLongMath::operator >>=( int n )
{
	ShR(n);
	return *this;
}




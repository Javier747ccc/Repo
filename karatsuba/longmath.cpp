#include <stdio.h>
#include <math.h>
#include <memory.h>
#include <windows.h>
#include "longmath.h"

const char Hex[]="0123456789ABCDEF";
const char hex[]="0123456789abcdef";


void QuitaCeros(LNUM a)
{
	int i=abs(a->Len)-1;
	for (;i>=0;i--)
	{
		if (a->buf[i]!=0)
			break;
		a->Len--;
	}
}

void AddUnsigned(LNUM ret, LNUM a, LNUM b) 
{
    int  i,aLen,bLen,c;
	LNUM *pa,*pb;

	aLen = nabs(a->Len);
	bLen = nabs(b->Len);

	if (aLen>=bLen)
	{	
		pa = (LNUM *)a;pb = (LNUM *)b;
	}
	else
	{	
		pa = (LNUM *)b;pb = (LNUM *)a;
		// swap aLen bLen
		aLen^= bLen; bLen = aLen ^ bLen; aLen^= bLen;
	}
	
	ret->Len = aLen;

	unsigned int memLen = (ret->Len + 1) * sizeof(buftype);
	if (ret->SizeMem < memLen)
	{
		ret->buf = (buftype*)realloc(ret->buf,memLen);
		ret->SizeMem = memLen;
	}
	buftype sl,rl;
	buftype *p1 = (*pa)->buf;
	buftype *p2 = (*pb)->buf;
	buftype *pret = ret->buf;
	int cy1,cy2;
	for(i = 0, c = 0; i < aLen; i++)
	{
		if (i<bLen)
		{

			sl = *p1 + *p2;
			cy1 = sl < *p1;
			rl = sl + c;
			cy2 = rl < sl;
			c = cy1 | cy2;
		}
		else
		{
			rl = *p1 + c;
			cy1 = rl < *p1;
			c = cy1;
		}
		*pret  = rl;
		p1++;p2++;pret++;
	}
	if (c>0)
	{
		ret->buf[i] = 1;
		ret->Len++;
	}
}

int SbbUnsigned(LNUM ret, LNUM a, LNUM b) 
{
	int  i,aLen,bLen,posneg,c;
	LNUM *pa,*pb;

	aLen = abs(a->Len);
	bLen = abs(b->Len);
	if (CompareUnsigned(a,b)>=0)		  
	{									  
		pa = (LNUM *)a;pb = (LNUM *)b;
		posneg = 1;
	}
	else
	{
		pa = (LNUM *)b;pb = (LNUM *)a;
		// swap aLen bLen
		aLen^= bLen; bLen = aLen ^ bLen; aLen^= bLen;
		posneg = -1;
	}
	ret->Len = aLen;
	unsigned int memLen = aLen * sizeof(buftype);
	if (ret->SizeMem < memLen)
	{
		ret->buf = (buftype*)realloc(ret->buf,memLen);
		ret->SizeMem = memLen;
	}

	buftype *p1 = (*pa)->buf;
	buftype *p2 = (*pb)->buf;
	buftype *pret = ret->buf;
	buftype sl,rl;
	int cy1,cy2;

    for(i = 0,c = 0; i < aLen; i++)
	{
		if (i<bLen)
		{
			sl = *p1 - *p2;
			cy1 = sl > *p1;
			rl = sl - c;
			cy2 = rl > sl;
			c = cy1 | cy2;
		}
		else
		{
			rl = *p1 - c;
			cy1 = rl > *p1;
			c = cy1;
		}
		*pret  = rl;
		p1++;p2++;pret++;
    }
	return posneg;
}


//void Add(void ret, LNUM a, LNUM b) 
void Add(LNUM ret, LNUM a, LNUM b) 
{
	int s;
	s = (a->Len * b->Len);
	if (s<0)
	{
		// Positivo + Negativo
		// Negativo + Positivo 
		if (SbbUnsigned(ret,a,b)>=0)
		{
			if (a->Len<0) ret->Len*=-1;
		}
		else
		{
			if (b->Len<0) ret->Len*=-1;
		}
		return;
	}
	if (s==0)
	{
		// Uno de los dos argumentos, ó los dos son cero
		if ((a->Len==0)&&(b->Len==0))
		{
			ret->Len = 0;
			return;
		}
		if (a->Len==0)
		{
			FromCopy(ret,b);
			return;
		}
		FromCopy(ret,a);
		return;
	}
	// Positivo + Positivo
	// Negativo + Negativo
	AddUnsigned(ret, a,b);
	if (a->Len<0) ret->Len*=-1;
	QuitaCeros(ret);
}


void Sbb(LNUM ret, LNUM a, LNUM b) 
{
	int s;
	s = (a->Len * b->Len);
	if (s<0)
	{
		// Positivo - Negativo
		// Negativo - Positivo 
		AddUnsigned(ret,a,b);
		if (a->Len<0)
			ret->Len*= -1;
		return;
	}
	if (s==0)
	{
		// Uno de los dos argumentos, ó los dos son cero
		if ((a->Len==0)&&(b->Len==0))
		{
			ret->Len = 0;
			return;
		}
		if (a->Len==0)
		{
			FromCopy(ret,b);
			ret->Len*=-1;
			return;
		}
		FromCopy(ret,a);
		return;
	}
	// Positivo - Positivo
	// Negativo - Negativo
	if (SbbUnsigned(ret,a,b)<0)
	{
		ret->Len*=-1;
	}
	QuitaCeros(ret);
}


void DameMedioH(LNUM Ret,LNUM a,int d)
{
	Ret->Len = d/2;
	Ret->buf = &a->buf[d/2];
}

void DameMedioL(LNUM Ret,LNUM a,int d)
{
	Ret->Len = d/2;
	Ret->buf = a->buf;
}


int CmpNumZero(LNUM a)
{
	if (a->Len==0)
		return 0;
	for (int i=0;i<a->Len;i++)
	{
		if (a->buf[i]!=0)
			return 1;
	}
	return 0;
	
}

void gS(LNUM ret,LNUM a, LNUM b, int d) 
{
	int             i, j;

    for(i = 0; i < 2 * d; i++) ret->buf[i] = 0;
    for(i = 0; i < d; i++) 
	{
        for(j = 0; j < d; j++) 
		{
			ret->buf[i + j] += (a->buf[i] * b->buf[j]);
		}
    }
}
void gradeSchool(LNUM ret,LNUM a, LNUM b, int d) 
{
/*
	int             i, j;

    for(i = 0; i < 2 * d; i++) ret[i] = 0;
    for(i = 0; i < d; i++) 
	{
        for(j = 0; j < d; j++) ret[i + j] += a[i] * b[j];
    }
*/

    int             i, j;
#ifdef bufsize8
	unsigned short wret;
#endif
#ifdef bufsize16
	unsigned int wret;
#endif
#ifdef bufsize32
	unsigned __int64 wret;
#endif

	buftype C;
	buftype sum;
    for(i = 0; i < 2 * d; i++) ret->buf[i] = 0;
    
	for(i = 0; i < d; i++) 
	{
		C = 0;
        for(j = 0; j < d; j++) 
		{
			wret = a->buf[i];
			wret *=b->buf[j];
			sum = ret->buf[i + j] + (buftype)wret + C;
			C = (buftype)(wret>>bufbitlen);
			if (sum<ret->buf[i + j])
			{
				C++;
			}
			ret->buf[i + j] = sum;
		}
		ret->buf[i+j] = C;
    }
}


void karatsubaMul(LNUM ret, LNUM a, LNUM b,int d) 
{
	LNUM ar,al,br,bl,asum,bsum,arbr,albl,asumbsum,MiddleRet;

	
	
	if ((a->Len == 0)||(b->Len == 0))
	{
		memset(ret->buf,0,2*d*bufsize);
		return;
	}
	

	if (d<=1)
	{
		gradeSchool(ret, a, b, d) ;
	}
	else
	{
		DameMedioH(al,a,d);// low-order half of a
		DameMedioL(ar,a,d);// high-order half of a
		DameMedioH(bl,b,d);// low-order half of b
		DameMedioL(br,b,d);// high-order half of b
		
		arbr->Len = d; arbr->buf = &ret->buf[d * 0];//x1
		albl->Len = d; albl->buf = &ret->buf[d * 1];//x2

		asumbsum->Len = 2*d; asumbsum->SizeMem = d<<8; asumbsum->buf = &ret->buf[d * 2];
		asum->Len = d; asum->SizeMem = d<<8;asum->buf = &ret->buf[d * 9];
		bsum->Len = d; bsum->SizeMem = d<<8;bsum->buf = &ret->buf[d * 10];
		Add(asum,al,ar);
		Add(bsum,bl,br);
		asum->Len = d;
		bsum->Len = d;
		karatsubaMul(arbr, ar, br,d/2);
		karatsubaMul(albl, al, bl,d/2);
		karatsubaMul(asumbsum, asum, bsum,d/2);
		DameMedioL(ar,asum,d);
		DameMedioH(al,asum,d);
		int alnz,blnz;
		DameMedioL(br,bsum,d);
		DameMedioH(bl,bsum,d);
		alnz = CmpNumZero(al);
		blnz = CmpNumZero(bl);
		MiddleRet->buf = &asumbsum->buf[d/2];
		MiddleRet->Len = d; 
		MiddleRet->SizeMem = d<<8;
		if (alnz>0)
		{
			Add(MiddleRet,MiddleRet,br);
			asumbsum->Len = 2*d;
		}
		if (blnz>0)
		{
			Add(MiddleRet,MiddleRet,ar);
			asumbsum->Len = 2*d;
		}
		MiddleRet->buf = &asumbsum->buf[d];
		MiddleRet->Len = d; 
		if ((alnz>0)&&(blnz>0))
		{
			MiddleRet->buf[0]++;
		}
		Sbb(asumbsum,asumbsum,arbr);
		asumbsum->Len = 2*d;
		Sbb(asumbsum,asumbsum,albl);
		asumbsum->Len = 2*d;

		MiddleRet->Len = 2*d; 
		MiddleRet->buf = &ret->buf[d/2];
		Add(MiddleRet,MiddleRet,asumbsum);
	}

	ret->Len = 2 * d;

	if (!(((a->Len>0)&&(b->Len>0)) ||
			((a->Len<0)&&(b->Len<0))))
	{
		ret->Len*=-1;
	}
	memset(&ret->buf[2*d],0,8*d*bufsize);
	
	return;
}


void Mul(LNUM r, LNUM a, LNUM b) 
{

	int aLen,bLen,Len;
	LNUM Ret,A,B;


	aLen = abs(a->Len);
	bLen = abs(b->Len);

	Len = ((aLen-bLen)>=0) ? 
		aLen%2==0 ? aLen : aLen+1 : 
		bLen%2==0 ? bLen : bLen+1 ;

	InitLen(Ret,Len*32);
	InitLen(A,Len);
	InitLen(B,Len);
	memcpy(A->buf,a->buf,aLen*bufsize);
	memcpy(B->buf,b->buf,bLen*bufsize);

	
	karatsubaMul(Ret,A, B, Len); // compute product w/o regard to carry
	FromCopy(r,Ret);
	Free(Ret);
	Free(A);
	Free(B);

	int n = (a->Len * b->Len);
	if (n<0)
	{
		r->Len*=-1;
	}
	
}


void Init(LNUM n)
{
	n->buf = (buftype * )malloc(sizeof(buftype));
	memset(n->buf,0,sizeof(buftype));
	n->Len = 1;
	n->SizeMem = sizeof(buftype);
}


void InitLen(LNUM n,int memlen)
{
	n->buf = (buftype * )malloc(memlen * sizeof(buftype));
	memset(n->buf,0,memlen * sizeof(buftype));
	n->Len = memlen;
	n->SizeMem = memlen * sizeof(buftype);
}




void Free(LNUM n)
{
	if (n->SizeMem>0)
		free(n->buf);
	n->Len = 0;
	n->SizeMem = 0;
}


void FromCopy(LNUM n,LNUM l) 
{
	int Len,i;
	unsigned int memLen;
	Len = abs(l->Len);
	for (;Len>0;Len--)
	{
		if (l->buf[Len-1]!=0)
			break;
	}
	memLen = Len*sizeof(buftype);

	if (n->SizeMem<memLen)
	{
		if (n->SizeMem>0)
			free(n->buf);
		n->buf = (buftype*)malloc(memLen);
		memset(n->buf,0,memLen);
		n->SizeMem = memLen;
	}

	
	for (i=0;i<Len;i++) 
		n->buf[i] = l->buf[i];
	if (l->Len<0)
		n->Len = -Len;
	else
		n->Len = Len;
}
int findchar(char *buffer,char c)
{
	char *pc = (char *)strchr( buffer, c);
	if (pc==NULL)
		return -1;
	return (pc-buffer);
}

void SetFromStr(LNUM a,char *cadena) 
{
	int len = strlen(cadena);
	unsigned int n;
	int i=(len-(2*bufsize));

	while ((i%(2*bufsize))!=0)
	{
		len++;
	}
	a->Len = len/(2*bufsize);
	if (a->SizeMem<((len/(2*bufsize))*sizeof(buftype)))
	{
		if (a->SizeMem>0)
			free(a->buf);
		a->SizeMem = ((len/(2*bufsize))*sizeof(buftype));
		a->buf = (buftype*)malloc(a->SizeMem);
	}
	memset(a->buf,0,a->SizeMem);

	for (int j=0;i>=0;i-=(2*bufsize),j++)
	{
		n =0;
		for (int k=0;k<(2*bufsize);k++)
		{
			
			int pos = findchar((char *)Hex, cadena[i+k]);
			if (pos<0)
			{
				pos = findchar((char *)hex, cadena[i+k]);
				if (pos<0)
				{
					return;
				}
			}
			n<<=4;
			n|=pos;
			
		}
		a->buf[j] = n;
	}

}
void SetFromInt(LNUM n,int l) 
{
	int i;
	if (n->SizeMem<(4*sizeof(buftype)))
	{
		if (n->SizeMem>0)
			free(n->buf);
		n->SizeMem = 4*sizeof(buftype);
		n->buf = (buftype*)malloc(n->SizeMem);
	}
	n->Len = 32/bufbitlen;
	
	
	memset(n->buf,0,n->SizeMem);


	if (l<0)
	{
		n->Len*=-1;
	}
	l = abs(l);
	
	for (i=0;i<abs(n->Len);i++)
	{
		n->buf[i]=l & bufmask;
		l>>=bufbitlen;
	}
	for (i=abs(n->Len);i>0;i--)
	{
		if (n->buf[i-1]!=0)
			break;
	}
	if (n->Len<0)
		n->Len = -1*i;
	else
		n->Len = i;
}



int CompareUnsigned(LNUM a,LNUM b)
{
	int aLen,bLen;
	aLen = a->Len;
	if (aLen<0)
		aLen*=-1;
	bLen = b->Len;
	if (bLen<0)
		bLen*=-1;

	if (aLen > bLen)
		return 1;
	if (aLen < bLen)
		return -1;
	for (int i=aLen-1;i>=0;i--)
	{
		if (a->buf[i]==b->buf[i])
		continue;
		if (a->buf[i]>b->buf[i])
			return 1;
		return -1;
	}
	return 0;	
}
int Compare(LNUM a,LNUM b)
{
	

	if ((a->Len * b->Len)<0)
	{
		// Positivo  Negativo
		// Negativo  Positivo 
		if (a->Len > b->Len)
			return 1;
		if (a->Len < b->Len)
			return -1;
		return 0;
	}
	// Positivo  Positivo
	// Negativo  Negativo
	return CompareUnsigned(a,b);
}








void Shr(LNUM a) 
{
    int             i;
	int c;

	c = 0;
	for(i = 0; i < a->Len-1;i++) 
	{
		c = (a->buf[i+1] & 1)<<(bufbitlen-1);
        a->buf[i]>>=1;
		a->buf[i]|=c;
    }
	a->buf[i]>>=1;
	if (a->buf[i]==0)
	{
		a->Len--;
	}
	QuitaCeros(a);
}


void Shl(LNUM a) 
{
    int             i;
	int c1,c2;

	c1 = c2 = 0;

	for(i = 0; i < a->Len;i++) 
	{
		c1 = (a->buf[i] & (1<<bufbitlen))>>(bufbitlen-1);
		a->buf[i]<<=1;
		a->buf[i]|=c2;
		c2 = c1;
	}
	if (c1>0)
	{
		a->Len++;
		unsigned int memLen = a->Len * sizeof(buftype);
		if (a->SizeMem < memLen)
		{
			a->buf = (buftype*)realloc(a->buf,a->Len);
			a->SizeMem = a->Len;
		}
		a->buf[i]=1;
	}
}


void Div(LNUM RetCociente,LNUM RetResto, LNUM Dividendo,LNUM Divisor)
{
	int count;
	LNUM Sumatorio;
	LNUM Temp;
	LNUM Cociente;
	LNUM Resto;

	Init(Cociente);
	Init(Resto);
	Init(Temp);
	Init(Sumatorio);

	

	SetFromInt(Cociente,0);
	FromCopy(Resto,Dividendo);

	if (Compare(Dividendo,Divisor)>=0)//if (Resto<=Divisor)
	{
		SetFromInt(Sumatorio,1);
		for (count = 0;Compare(Sumatorio,Dividendo)<0;)
		{
			count++;
			Shr(Resto);
			if (Compare(Resto,Divisor)<=0)//if (Resto<=Divisor)
			{
				Add(Cociente, Cociente, Sumatorio);
		
				Mul(Temp, Divisor, Cociente);
				Sbb(Resto, Dividendo , Temp);

				if (Compare(Resto,Divisor)<0)
				{
					break;
				}

				SetFromInt(Sumatorio,1);
			}
			else
			{
				Shl(Sumatorio);
				
			}
		}
	}
	if (RetCociente!=NULL)
	{
		FromCopy(RetCociente,Cociente);
	}
	if (RetResto!=NULL)
	{
		FromCopy(RetResto,Resto);
	}
	Free(Cociente);
	Free(Resto);
	Free(Temp);
	Free(Sumatorio);

}
 
int CmpZero(LNUM n)
{
	if (n->Len==0)
		return 0;
	if (n->Len<0)
		return -1;
	return 1;
}

int NotZero(LNUM n)
{
	for (int i=0;i<n->Len;i++)
	{
		if (n->buf[i]!=0)
			return 1;
	}
	return 0;
}


void modinv( LNUM ret,LNUM a, LNUM m ) // modular inverse
// returns i in range 1..m-1 such that i*a = 1 mod m
// a must be in range 1..m-1
{
	LNUM j,b,c,x,y,tmp;
	Init(j);
	Init(b);
	Init(c);
	Init(x);
	Init(y);
	Init(tmp);
	SetFromInt(j,1);
	SetFromInt(ret,0);
	FromCopy(b,m);
	FromCopy(c,a);

	while ( NotZero(c))
	{
		Div(x,NULL,b,c);
		Mul(tmp,x,c);
		Sbb(y,b,tmp);
		FromCopy(b,c);
		FromCopy(c,y);
		FromCopy(y,j);
		Mul(tmp,j,x);
		Sbb(j,ret,tmp);
		FromCopy(ret,y);
	}
	if ( CmpZero(ret) < 0 )
	{
		Add(ret,ret,m);
	}
}



void exp_mod(LNUM s, LNUM x, LNUM y, LNUM n)
{
	LNUM t,u;
	Init(t);
	Init(u);
	Init(s);
	SetFromInt(s,1);
	//t=x; u=y;
	Div(NULL,t,x,n);
	FromCopy(u,y);


	while(NotZero(u)!=0)
	{
		if (u->buf[0]&1) 
		{
			Mul(s,s,t);

			Div(NULL,s,s,n);
		}
		Shr(u);
		Mul(t,t,t);
		Div(NULL,t,t,n);
	}
}


void exp_mod_ui(LNUM s, LNUM x, int y, LNUM n)
{
	LNUM t;
	Init(t);
	Init(s);
	SetFromInt(s,1);
	Div(NULL,t,x,n);
	while(y!=0)
	{
		if (y & 1) 
		{
			Mul(s,s,t);
			Div(NULL,s,s,n);
		}
		y>>=1;
		Mul(t,t,t);
		Div(NULL,t,t,n);
	}
}

void OutputNum(char *str,LNUM a) 
{
	if (a->Len<0)
		printf("-");
	int i=abs(a->Len)-1;
	for (;i>=0;i--)
	{
		if (a->buf[i]!=0)
			break;
	}

	for (int j=0;i>=0;i--,j+=(2*bufsize))
	{
		#ifdef bufsize8
			sprintf(&str[j],"%02X",a->buf[i]);
		#endif
		#ifdef bufsize16
			sprintf(&str[j],"%04X",a->buf[i]);
		#endif
		#ifdef bufsize32
			sprintf(&str[j],"%08X",a->buf[i]);
		#endif
	}
	str[j] = 0;
}



void printNum(char *cadena,LNUM a) 
{
	
	printf("%s = ",cadena);
	if (a->Len<0)
		printf("-");
	int i=abs(a->Len)-1;
	for (;i>=0;i--)
	{
		if (a->buf[i]!=0)
			break;
		printf("00");
	}

	
	for (;i>=0;i--)
	{
		#ifdef bufsize8
			printf("%02X",a->buf[i]);
		#endif
		#ifdef bufsize16
			printf("%04X",a->buf[i]);
		#endif
		#ifdef bufsize32
			printf("%08X",a->buf[i]);
		#endif
		
	}
	printf("\n");
}

void pNum(LNUM a) 
{
	if (a->Len<0)
		printf("-");
	int i=abs(a->Len)-1;
	for (;i>=0;i--)
	{
		if (a->buf[i]!=0)
			break;
	}

	for (;i>=0;i--)
	{
		#ifdef bufsize8
			printf("%02X",a->buf[i]);
		#endif
		#ifdef bufsize16
			printf("%04X",a->buf[i]);
		#endif
		#ifdef bufsize32
			printf("%08X",a->buf[i]);
		#endif
	}
	printf("\n");
}





#include <stdio.h>
#include <memory.h>
#include <windows.h>
#include <math.h>


#define LNUM_FUNCIONS


#include "lm.h"

const char Hex[]="0123456789ABCDEF";
const char hex[]="0123456789abcdef";

#define __LNUMDOUBLE__
#ifdef bufsize32
#define __64__
#endif

#ifdef LNUM_FUNCIONS
#include "functions.h"
#endif

int Sbbsss,Addsss;

__int64 NumMul;


void lmulmod(LNUM c, LNUM a,LNUM b, LNUM n);
void pfast_mul( buftype *a, buftype *x, buftype *y, unsigned int xLen,unsigned int yLen,unsigned int keep );
void pfast_mulSQ( buftype *a, buftype *x, unsigned int xLen,unsigned int keep );
void fast_mul( LNUM a, LNUM x, LNUM y, unsigned int keep );
void iDiv(LNUM q,LNUM r, LNUM Dividendo,LNUM Divisor);
void QuitaCeros(LNUM a)
{
	int i=nabs(a->Len)-1;
	for (;i>=0;i--)
	{
		if (a->buf[i]!=0)
			break;
		if (a->Len==0)
			break;
		if (a->Len>0)
			a->Len--;
		else
			a->Len++;

	}
}
#ifndef LNUM_FUNCIONS

#define p__addu(ret,pa,pb,aLen,bLen,c) AddBuf(ret,pa,pb,aLen,bLen,&c)
#define p__sbbu(ret,pa,pb,aLen,bLen,c) SbbBuf(ret,pa,pb,aLen,bLen,&c)
#define p__inc(a,SizeA,c) IncBuf(a,SizeA,&c)

#ifdef LNUMCODE32


void mul32(unsigned long *ret ,unsigned long a, unsigned long b)
{
	unsigned long p1, p2, p3, p4,sum;
	p1 = lo(a) * lo(b);
	p2 = lo(a) * hi(b);
	p3 = hi(a) * lo(b);
	p4 = hi(a) * hi(b);
	sum = hi(p1) + lo(p2) + lo(p3);
	ret[0] = lo(p1) + lh(sum);
	ret[1] = hi(sum) + hi(p2) + hi(p3) + p4 ;
}

void add6432(unsigned long *ret ,unsigned long a)
{
	unsigned long r;
	r = *ret;
	*ret+= a;
	if (*ret<r)
		ret[1]++;
}

void Sbb6432(unsigned long *ret ,unsigned long a)
{
	unsigned long r;
	r = *ret;
	*ret-= a;
	if (*ret>r)
		ret[1]--;
}

//ret = src+(a*b)+c
void muladd6432(unsigned long *ret,unsigned long int src,unsigned long int a,unsigned long int b,unsigned long int c)
{
	unsigned long p1, p2, p3, p4,sum;
	p1 = lo(a) * lo(b);
	p2 = lo(a) * hi(b);
	p3 = hi(a) * lo(b);
	p4 = hi(a) * hi(b);
	sum = hi(p1) + lo(p2) + lo(p3);
	ret[0] = lo(p1) + lh(sum);
	ret[1] = hi(sum) + hi(p2) + hi(p3) + p4 ;

	sum = ret[0];
	ret[0]+= src;
	if (ret[0]<sum)
		ret[1]++;
	sum = ret[0];
	ret[0]+= c;
	if (ret[0]<sum)
		ret[1]++;
}

void mulsbb6432(unsigned long *ret,unsigned long int src,unsigned long int a,unsigned long int b,unsigned long int c)
{
	unsigned long p1, p2, p3, p4,sum;
	p1 = lo(a) * lo(b);
	p2 = lo(a) * hi(b);
	p3 = hi(a) * lo(b);
	p4 = hi(a) * hi(b);
	sum = hi(p1) + lo(p2) + lo(p3);
	ret[0] = lo(p1) + lh(sum);
	ret[1] = hi(sum) + hi(p2) + hi(p3) + p4 ;

	sum = ret[0];
	ret[0]+= src;
	if (ret[0]<sum)
		ret[1]++;
	sum = ret[0];
	ret[0]+= c;
	if (ret[0]<sum)
		ret[1]++;
}



void Sbb6464(unsigned long *ret ,unsigned long *a)
{
	unsigned long r,c,i;
	c = 0;
	for (i=0;i<2;i++)
	{
		r = ret[i];
		ret[i]-= (a[i]+c);
		if (ret[i]>r)
			c=1;
	}
}

#endif //LNUMCODE32
int CmpBufUnsigned(buftype *pa,buftype *pb,int aLen,int bLen)
{
	int i;
	if (aLen<0)
		aLen*=-1;
	if (bLen<0)
		bLen*=-1;

	if (aLen > bLen)
		return 1;
	if (aLen < bLen)
		return -1;
	for (i=aLen-1;i>=0;i--)
	{
		if (pa[i]==pb[i])
		continue;
		if (pa[i]>pb[i])
			return 1;
		return -1;
	}
	return 0;
}


void AddBuf(buftype *ret,buftype *pa,buftype *pb,int aLen,int bLen,int *c)
{
	int i;
	int cy1,cy2;
	buftype sl,rl,*p1,*p2,*pret;

	pret = ret;

	if (aLen>=bLen)
	{	
		p1 = pa;p2 = pb;
	}
	else
	{	
		p1 = pb;p2 = pa;
		Swap(aLen,bLen);
		//aLen^= bLen; bLen = aLen ^ bLen; aLen^= bLen;
	}


	for(i = 0 ; i < bLen; i++)
	{
		sl = *p1 + *p2;
		cy1 = sl < *p1;
		rl = sl + *c;
		cy2 = rl < sl;
		*c = cy1 | cy2;

		*pret  = rl;
		p1++;p2++;pret++;
	}
	for(; i < aLen; i++)
	{
		rl = *p1 + *c;
		cy1 = rl < *p1;
		*c = cy1;

		*pret  = rl;
		p1++;p2++;pret++;
	}
}


void IncBuf(buftype *a,int SizeA,int *c)
{
	int i,cy1,sz1;
	buftype rl,*p1;
	p1=a;sz1=SizeA;
	for(i = 0 ; i < sz1; i++)
	{
		rl = *p1 + *c;cy1 = rl < *p1;*c = cy1;
		*p1  = rl;
		p1++;
	}
}



void SbbBuf(buftype *ret,buftype *pa,buftype *pb,int aLen,int bLen,int *posneg)
{
	int i;
	int cy1,cy2,c;
	buftype sl,rl,*p1,*p2,*pret;

	pret = ret;
	if (CmpBufUnsigned(pa,pb,aLen,bLen)>=0)		  
	{									  
		p1 = pa;p2 = pb;
		*posneg = 1;
	}
	else
	{
		p1 = pb;p2 = pa;
		Swap(aLen,bLen);
		*posneg = -1;
	}

	for(i = 0, c = 0; i < bLen; i++)
	{
		sl = *p1 - *p2;
		cy1 = sl > *p1;
		rl = sl - c;
		cy2 = rl > sl;
		c = cy1 | cy2;
		*pret  = rl;
		p1++;p2++;pret++;
	}
	for(; i < aLen; i++)
	{
		rl = *p1 - c;
		cy1 = rl > *p1;
		c = cy1;
		*pret  = rl;
		p1++;p2++;pret++;
	}

}

#endif //LNUM_FUNCIONS

void AddUnsigned(LNUM ret, LNUM a, LNUM b) 
{
    int  aLen,bLen,mLen;
	unsigned int memLen;
	int c;

	aLen = nabs(a->Len);
	bLen = nabs(b->Len);
	mLen = MAX(aLen,bLen);

	ret->Len = mLen;

	memLen = (ret->Len + 1) * sizeof(buftype);
	if (ret->SizeMem < memLen)
	{
		ret->buf = (buftype*)realloc(ret->buf,memLen);
		ret->SizeMem = memLen;
	}
	c = 0;
	p__addu(ret->buf,a->buf,b->buf,aLen,bLen,c);
	if (c>0)
	{
		ret->buf[mLen] = 1;
		ret->Len++;
	}
}


int SbbUnsigned(LNUM ret, LNUM a, LNUM b) 
{
	int  aLen,bLen,mLen,posneg;
	unsigned int memLen;

	aLen = nabs(a->Len);
	bLen = nabs(b->Len);
	mLen = MAX(aLen,bLen);
	ret->Len = mLen;
	memLen = mLen * sizeof(buftype);
	if (ret->SizeMem < memLen)
	{
		ret->buf = (buftype*)realloc(ret->buf,memLen);
		ret->SizeMem = memLen;
	}
	posneg = 0;
	p__sbbu(ret->buf,a->buf,b->buf,aLen,bLen,posneg);
	return posneg;
}


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



int NotZero(LNUM n)
{
	int i,nLen;
	nLen = nabs(n->Len);
	if (nLen==0)
		return 0;
	for (i=0;i<nLen;i++)
	{
		if (n->buf[i]!=0)
			return 1;
	}
	return 0;
}

int CmpZero(LNUM n)
{
	if (n->Len==0)
		return 0;
	if (n->Len<0)
		return -1;
	return 1;
}




int BufNotZero(buftype *n,int Len)
{
	int i;
	if (Len==0)
		return 0;
	for (i=0;i<Len;i++)
	{
		if (n[i]!=0)
			return 1;
	}
	return 0;
}
void pkaratsubaSQ(buftype *ret, buftype *a,int d) 
{
	buftype *ar,*al,*asum,*arar,*alal;
	buftype *asumasum,*MiddleRet;
	int c,n,d2,c1,c2,casumasum;

	d2 = d/2;

#ifdef bufsize8
	if (d<=64)
	{
		pfast_mulSQ(ret, a, d, 2*d*bufbitlen);
		return;
	}
#endif
#ifdef bufsize16
	if (d<=32)
	{
		pfast_mulSQ(ret, a, d, 2*d*bufbitlen);
		return;
	}
#endif
#ifdef bufsize32
	if (d<=16)
	{
		pfast_mulSQ(ret, a, d, 2*d*bufbitlen);
		return;
	}
#endif

	
	al = &a[d2];
	ar = a;
	
		
	arar = &ret[d * 0];//x1
	alal = &ret[d * 1];//x2
	asumasum = &ret[d * 2];
	asum = &ret[d * 6];

	memset(asum,0,d*bufsize);
	c1 = 0;
	p__addu(asum,al,ar,d2,d2,c1);
	

	pkaratsubaSQ(arar, ar,d2);
	pkaratsubaSQ(alal, al,d2);
	memset(asumasum,0,2*d*bufsize);

	casumasum = 0;
	// asumbsum puede tener mas de 2*d, el carry puede llegar a ser = 3
	pkaratsubaSQ(asumasum, asum,d2);
	
	if (c1>0)
	{
//			1	FF
//			1	FF
//			-----
//		2  FE	01	
//		   FF
//		   FF
//		1
//		---------
//		3  FC   01

		c = Shl_1(asum,d2);
		if (c>0)
			casumasum++;

		c = 0;
		p__addu(&asumasum[d2],&asumasum[d2],asum,d2,d2,c);
		if (c>0)
			casumasum++;
		casumasum++;
	}

	
	
	MiddleRet = &asumasum[d];

	if (casumasum)
	{
		asumasum[d] = casumasum;
		n=0;
		p__sbbu(asumasum ,asumasum,arar,d+1,d,n);
		n=0;
		p__sbbu(asumasum ,asumasum,alal,d+1,d,n);
	}
	else
	{
		p__sbbu(asumasum ,asumasum,arar,d,d,n);
		if (n<0)
		{
			*MiddleRet++;
		}
		p__sbbu(asumasum ,asumasum,alal,d,d,n);
		if (n<0)
		{
			*MiddleRet++;
		}
	}
	MiddleRet = &ret[d2];
	c=0;	
	if (casumasum>0)
	{
		p__addu(MiddleRet,MiddleRet,asumasum,d+1,d+1,c);
		if (c>0)
		{
			p__inc(&MiddleRet[d+1],d-1,c);
		}
	}
	else
	{
		p__addu(MiddleRet,MiddleRet,asumasum,d,d,c);
		if (c>0)
		{
			p__inc(&MiddleRet[d],d,c);
		}
	}
}


void pkaratsubaMul(buftype *ret, buftype *a, buftype *b,int d) 
{
	buftype *ar,*al,*br,*bl,*asum,*bsum,*arbr,*albl;
	buftype *asumbsum,*MiddleRet;
	int c,n,d2,c1,c2,casumbsum;

	d2 = d/2;

#ifdef bufsize8
	if (d<=1)
	{
		pfast_mul(ret, a, b, d, d, 2*d*bufbitlen);
		return;
	}
#endif
#ifdef bufsize16
	if (d<=32)
	{
		pfast_mul(ret, a, b, d, d, 2*d*bufbitlen);
		return;
	}
#endif
#ifdef bufsize32
	if (d<=16)
	{
		pfast_mul(ret, a, b, d, d, 2*d*bufbitlen);
		return;
	}
#endif

	
	al = &a[d2];
	ar = a;
	bl = &b[d2];
	br = b;
		
	arbr = &ret[d * 0];//x1
	albl = &ret[d * 1];//x2
	asumbsum = &ret[d * 2];
	asum = &ret[d * 6];
	bsum = &ret[d * 7];

	memset(asum,0,d*bufsize);
	c1=0;
	p__addu(asum,al,ar,d2,d2,c1);

	memset(bsum,0,d*bufsize);
	c2=0;
	p__addu(bsum,bl,br,d2,d2,c2);



	pkaratsubaMul(arbr, ar, br,d2);
	pkaratsubaMul(albl, al, bl,d2);
	memset(asumbsum,0,2*d*bufsize);

	casumbsum = 0;
	// asumbsum puede tener mas de 2*d, el carry puede llegar a ser = 3

//			1	B			1	B
//			1	A			0	A
//			-----			-----
//			  A*B			  A*B
//			A				A
//			B				0
//		1				1
//		---------		---------

	pkaratsubaMul(asumbsum, asum, bsum,d2);
	if (c1>0)
	{
		c = 0;
		p__addu(&asumbsum[d2],&asumbsum[d2],bsum,d2,d2,c);
		if (c>0)
		{
			casumbsum++;
		}
	}
	if (c2>0)
	{
		c = 0;
		p__addu(&asumbsum[d2],&asumbsum[d2],asum,d2,d2,c);
		if (c>0)
		{
			casumbsum++;
		}
	}
	if ((c1>0)&&(c2>0))
	{
		casumbsum++;
	}
	MiddleRet = &asumbsum[d];

	if (casumbsum)
	{
		asumbsum[d] = casumbsum;
		n=0;
		p__sbbu(asumbsum ,asumbsum,arbr,d+1,d,n);
		n=0;
		p__sbbu(asumbsum ,asumbsum,albl,d+1,d,n);
	}
	else
	{
		p__sbbu(asumbsum ,asumbsum,arbr,d,d,n);
		if (n<0)
		{
			*MiddleRet++;
		}
		p__sbbu(asumbsum ,asumbsum,albl,d,d,n);
		if (n<0)
		{
			*MiddleRet++;
		}
	}
	MiddleRet = &ret[d2];
	c=0;	
	if (casumbsum>0)
	{
		p__addu(MiddleRet,MiddleRet,asumbsum,d+1,d+1,c);
		if (c>0)
		{
			p__inc(&MiddleRet[d+1],d-1,c);
		}
	}
	else
	{
		p__addu(MiddleRet,MiddleRet,asumbsum,d,d,c);
		if (c>0)
		{
			p__inc(&MiddleRet[d],d,c);
		}
	}
}


void karatsubaMul(LNUM ret, LNUM a, LNUM b,int d) 
{
	unsigned int memlen;
	
	if ((a->Len == 0)||(b->Len == 0))
	{
		ret->Len = 0;
		return;
	}
	memlen = d*10*bufsize;

	if (ret->SizeMem<memlen)
	{
		free(ret->buf);
		ret->buf = malloc(memlen);
		ret->SizeMem = memlen;

	}

	if (a==b)
		pkaratsubaSQ(ret->buf, a->buf,d);
	else
		pkaratsubaMul(ret->buf, a->buf, b->buf,d);	
	ret->Len = 2 * d;

	if (!(((a->Len>0)&&(b->Len>0)) ||
			((a->Len<0)&&(b->Len<0))))
	{
		ret->Len*=-1;
	}
	memset(&ret->buf[2*d],0,d*bufbitlen);
	return;
}


// Si n es multiplo de 2^x ,retorna x
// En caso contrario retorna 0
unsigned long int Is2n(unsigned long int n)
{
	int i,j,k,l;
	for (i=1,j=1,k=0,l=0;i<bufbitlen;i++)
	{
		if (n & j)
		{
			if (k>0)
				return FALSE;
			k = i;
		}
		j<<=1;
	}
	if (k==0)
		return 0;
	return k-1;
}

void pMulRusa(LNUM r, LNUM a, LNUM b,unsigned int keep)
{
	int aLen,bLen,nn,nbAA,i;
	LNUM AA,BB;

	//	1	45			+	64		64
	//	0	22				128
	//	1	11			+	256		320
	//	1	5			+	512		832
	//	0	2				1024
	//	1	1			+	2048	2880

	LNUMInit(AA);LNUMInit(BB);
	
	keep = nabs(keep );
	aLen = nabs(a->Len);
	bLen = nabs(b->Len);
	FromCopy(AA,a);
	FromCopy(BB,b);

	SetFromInt(r,0);
	if ((aLen==0)||(bLen==0))
	{
		return;
	}
	nbAA = nbits(AA);
	nn = 0;
	ShlNB(BB,nbAA-1);
	for (i=nbAA-1;i>=0;i--)
	{
		if (test(AA,i))
		{
			if (nn>0)
				ShrNB(BB,nn);
			Add(r,r,BB);
			nn = 0;
		}
		nn++;
	}
	nn = (a->Len * b->Len);
	if (nn<0)
	{
		r->Len*=-1;
	}
	QuitaCeros(r);
}


void Mul2n(LNUM r, LNUM a, LNUM b,unsigned int keep) 
{

	int aLen,bLen,n;
	LNUM A,B;


	keep = nabs(keep );
	aLen = nabs(a->Len);
	bLen = nabs(b->Len);
	//printf("MUL2n(%d,%d,%d)\n",aLen,bLen,keep/32);
	if ((aLen==0)||(bLen==0))
	{
		SetFromInt(r,0);
		return;
	}

	if (aLen==1)
	{
		n = Is2n(*a->buf);
		if (n>0)
		{
			FromCopy(r,b);
			r->Len= nabs(r->Len);
			ShlNB(r,n);
			n = (a->Len * b->Len);
			if (n<0)
			{
				// El resultado es negativo
				r->Len*=-1;
			}
			return;
		}
	}
	if (bLen==1)
	{
		n = Is2n(*b->buf);
		if (n>0)
		{
			FromCopy(r,a);
			r->Len= nabs(r->Len);
			ShlNB(r,n);
			n = (a->Len * b->Len);
			if (n<0)
			{
				// El resultado es negativo
				r->Len*=-1;
			}
			return;
		}
	}

	if (aLen==bLen)
	{
		n = Is2n(aLen);
#ifdef bufsize8
		if (n>5)
#endif
#ifdef bufsize16
		if (n>4)
#endif
#ifdef bufsize32
		if (n>3)
#endif
		{
			if (a==b)
			{
				memcpy(A,a,sizeof(LNUM));
				A->Len = aLen;
				
				karatsubaMul(r,A, A, aLen);
				
				QuitaCeros(r);
				Norm(r,keep);
				if (r->Len<0)
					r->Len*=-1;
				
			}
			else
			{
				memcpy(A,a,sizeof(LNUM));
				memcpy(B,b,sizeof(LNUM));
				A->Len = aLen;
				B->Len = bLen;
				karatsubaMul(r,A, B, aLen);
				QuitaCeros(r);
				Norm(r,keep);
				n = (a->Len * b->Len);
				if (n<0)
				{
					r->Len*=-1;
				}
			}
			return;
		}
	}
	memcpy(A,a,sizeof(LNUM));
	memcpy(B,b,sizeof(LNUM));
	A->Len = aLen;
	B->Len = bLen;
	fast_mul(r,A, B,keep);
	n = (a->Len * b->Len);
	if (n<0)
	{
		r->Len*=-1;
	}
	QuitaCeros(r);


}


void Mul(LNUM r, LNUM a, LNUM b) 
{
	int aLen,bLen;
	aLen = nabs(a->Len);
	bLen = nabs(b->Len);
	Mul2n(r,a,b,(aLen+bLen)*bufbitlen);
}


void LNUMInit(LNUM n)
{
	n->buf = (buftype * )malloc(sizeof(buftype));
	memset(n->buf,0,sizeof(buftype));
	n->Len = 1;
	n->SizeMem = sizeof(buftype);
}

void InitLen(LNUM n,int memlen)
{
	/*
	if (n->buf!=NULL)
		free(n->buf);
*/

	free(n->buf);
	n->buf = (buftype * )malloc(memlen * sizeof(buftype));
	memset(n->buf,0,memlen * sizeof(buftype));
	n->Len = memlen;
	n->SizeMem = memlen * sizeof(buftype);
}




void LNUMFree(LNUM n)
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
	Len = nabs(l->Len);
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

unsigned long int ULIntFromLNUM(LNUM n)
{
	unsigned long int Num;
	unsigned char *p1,*p2;
	int i;
	
	Num = 0;
	p1 = (unsigned char *)&Num;
	p2 = (unsigned char *)n->buf;
	for (i = 0;i<4;i++)
	{
		if (i>=(n->Len*bufsize))
			break;
		*p1 = *p2;
		p1++;p2++;
	}
	return Num;
}

void SetFromInt(LNUM n,int l) 
{
	int i;
	if (n->SizeMem<4)
	{
		if (n->SizeMem>0)
			free(n->buf);
		n->SizeMem = 4;
		n->buf = (buftype*)malloc(n->SizeMem);
	}
	n->Len = 32/bufbitlen;
	
	
	memset(n->buf,0,n->SizeMem);


	if (l<0)
	{
		n->Len*=-1;
	}
	l = nabs(l);
	
	for (i=0;i<nabs(n->Len);i++)
	{
		n->buf[i]=l & bufmask;
		l>>=bufbitlen;
	}
	for (i=nabs(n->Len);i>0;i--)
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
	int aLen,bLen,i;
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
	for (i=aLen-1;i>=0;i--)
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


int test(LNUM a,unsigned int i)
{
	buftype n;
	unsigned int b;
	n = a->buf[(i/(bufbitlen))];
	b = (( n & (1<<(i%(bufbitlen))))==0) ? 0:1;
	return b;
}

unsigned long int nbits(LNUM a)
{
	buftype n,b;
	int i,l;
	
	i = -1;
	l = a->Len;
	while ((i<0)&&(l>=0))
	{
		l--;
		n = a->buf[l];
		for (i = bufbitlen-1;i>=0;i--)
		{
			b = (( n & (1<<i))==0) ? 0:1;
			if (b!=0)
				break;
		}
	}
	return (l*(bufbitlen)+i+1);
}

int Shl_1(buftype *a,unsigned int Len) 
{
    int i;
	int c1,c2;
	
	c1 = c2 = 0;
	for(i = 0; i < Len;i++) 
	{
		c1 = (a[i] & bufHbit)>>bufbitlenminus1;
		a[i]<<=1;
		a[i]|=c2;
		c2 = c1;
	}
	return c1;
}


void ShlBuf(buftype *a,unsigned int Len,unsigned int BytesAMover,unsigned int bitsADesplazar) 
{
    int i;
	int c1,c2;
	int bufbitlenminus ,newsize;
	
	bufbitlenminus = bufbitlen-bitsADesplazar;
	newsize = (Len+BytesAMover);
	if (bitsADesplazar)
		newsize++;

	if (BytesAMover>0)
	{
		for (i=(Len-1);i>=0;i--)
		{
			a[i+BytesAMover] = a[i];
			a[i] = 0;
		}
	}
	
	if (bitsADesplazar)
	{
		a[newsize-1] = 0;
	}

	Len = newsize;
	if (bitsADesplazar>0)
	{
		c1 = c2 = 0;
		for(i = 0; i < Len;i++) 
		{
			c1 = (a[i] & (bufmask<<bufbitlenminus))>>bufbitlenminus;
			a[i]<<=bitsADesplazar;
			a[i]|=c2;
			c2 = c1;
		}
		if (c1>0)
		{
			a[i] = c1;
		}
	}
}

unsigned int ShrBuf(buftype *a,unsigned int nBitsIni,unsigned int n) 
{
	int nBytesFin;
	unsigned int BytesADespl,r,m,i,Len;

	Len = ((nBitsIni-1)/bufbitlen)+1;

	nBytesFin = ((nBitsIni-n-1)/(bufbitlen))+1;
	r = n/(bufbitlen);
	BytesADespl = nabs(Len)-r;

	if (r>0)
	{
		for (i=0;i<BytesADespl;i++)
		{
			a[i] = a[i+r];
		}
	}
	r = n%(bufbitlen);
	if (r>0)
	{
		for (i=0;i<BytesADespl;i++)
		{
			a[i]>>=r;
			if ((i+1)<BytesADespl)
			{
				m = (a[i+1]<<((bufbitlen)-r));
				a[i]|=m;
			}
		}
	}
	return nBytesFin;
}

void ShrNB(LNUM a,unsigned int n) 
{
	int nBitsIni ,nBytesFin;

	nBitsIni = nbits(a);
	if ((nBitsIni-(int)n)<=0)
	{
		SetFromInt(a,0);
		return;
	}

	nBytesFin = ShrBuf(a->buf,nBitsIni,n) ;
	
	if (a->Len<0)
		nBytesFin*=-1;
	a->Len = nBytesFin;
	QuitaCeros(a);
}


void ShlNB(LNUM a,unsigned int n) 
{
    int aLen;
	unsigned int BytesAMover,bitsADesplazar,newsize;
	int bufbitlenminus ;
	
	aLen = nabs(a->Len);
	if (aLen==0)
		return;

	BytesAMover = n/bufbitlen;
	bitsADesplazar = n%bufbitlen;
	bufbitlenminus = bufbitlen-bitsADesplazar;

	newsize = (aLen+BytesAMover);
	if (bitsADesplazar)
		newsize++;
	

	if (a->SizeMem<(newsize*bufsize))
	{
		a->SizeMem = newsize*bufsize;
		a->buf = (buftype*)realloc(a->buf,a->SizeMem);
	}
	ShlBuf(a->buf,aLen,BytesAMover,bitsADesplazar);
	if (a->Len<0)
		newsize*=-1;
	a->Len = newsize;
	QuitaCeros(a);
}

void Norm(LNUM a,unsigned int nBits)
{
	unsigned int Len,aLen;
	if (nBits==0)
	{
		a->Len = 0;
		return;
	}

	aLen = nabs(a->Len);
	
	Len = ((nBits-1)/(bufbitlen))+1;
	if (Len < aLen)
	{
		aLen = Len;
		if (nBits < aLen*bufbitlen)
			a->buf[aLen-1] &= ((1<<(nBits % (bufbitlen)))-1);
		if (a->Len<0)
			aLen*=-1;
		a->Len = aLen;
	}

}

/*
void mulmod(LNUM Ret, LNUM a, LNUM b, LNUM m)
{
    LNUM  z,y,t,argB;
	LNUMInit(z);
	LNUMInit(y);
	LNUMInit(t);
	//LNUMInit(argA);
	LNUMInit(argB);

	FromCopy(y,a);
	//FromCopy(argA,a);
	FromCopy(argB,b);
	SetFromInt(z,0);

    for (;;) 
	{
		if (test(argB,0)!=0)
		{
			Add(z,z,y);
			if (((CompareUnsigned(z,y)<0)||(CompareUnsigned(z,m)>=0)))
			{
				Sbb(z,z,m);
			}
        }
		ShrNB(argB,1);
		if (Zero(argB)) 
			break;
		FromCopy(t,y);
		ShlNB(y,1);
		if ((CmpZero(t)<0)||(CompareUnsigned(y,m)>=0))
			Sbb(y,y,m);
    }
	FromCopy(Ret,z);
	LNUMFree(z);
	LNUMFree(y);
	LNUMFree(t);
	//LNUMFree(argA);
	LNUMFree(argB);
	

}
*/

//Ret = ( a * b ) % (2^n);
/*
void MulModFFT2n(LNUM Ret, LNUM a, LNUM b, DWORD n)
{
	LNUM A,B,GRet;
	int nb,aLen,bLen;
	
	
	nb = n/(bufbitlen);
	InitLen(A,nb);InitLen(B,nb);

	FromCopy(A,a);
	FromCopy(B,b);
	aLen = nabs(A->Len);
	bLen = nabs(B->Len);

	if ((nb>aLen)&&(nb>bLen))
	{
		nb>>=1;
	}



	InitLen(GRet,nb*2);
	InitFFT(nb,0x10000,1);
	FFTMul(GRet->buf,A->buf,B->buf,nb);
	DeInitFFT(nb);
	
	FromCopy(Ret,GRet);
	Norm(Ret,n);
	LNUMFree(A);LNUMFree(B);
}
*/
//r=r+(x*m)+c
#define pAddMs(r,x,m,c)(r + ((x * m) + c))




unsigned long int padd_mul1(buftype *ret, buftype *x, buftype y, unsigned int xLen)
{
	unsigned long int j; 
	buftype *pw,c;
	#ifdef bufsize32
	  #ifdef __64__
			unsigned __int64 w,v,m;
	  #else
			unsigned long int w[2],v,m;
			buftype r;

	  #endif
	#else
	  unsigned long int w,v,m;
	#endif

	Addsss+=xLen;

	c = 0; // carry
	m = y;
	pw = &w;
    for ( j=0; j<xLen; j++ )
    {
		// c:ret->buf[j] = ret[j] + (y * x->buf[j]) + c;
		#ifdef bufsize32
			#ifdef __64__
				//w = ret[j]+((x[j] * m) + c);
				w = pAddMs(ret[j],x[j],m,c);
				ret[j] = pw[0];
				c = pw[1];
				NumMul++;
			#else
			#ifdef WIN32
				v = x[j];
				pw = &ret[j];
				__asm 
				{
					mov eax,v;
					mov edx,m;
					mov ebx,pw;
					mul edx;
					add eax,c;
					jnc LAddMul1
					inc edx
LAddMul1:			
					mov ecx,dword ptr [ebx];
					add eax,ecx;
					jnc LAddMul2
					inc edx
LAddMul2:			

					mov dword ptr [ebx],eax;
					mov c,edx
				}
			#else
				muladd6432(w,ret[j],m,v,c);

				c = w[1];
				ret[j]= w[0];
			#endif

			#endif
		#else
			w = ret[j]+((x[j] * m) + c);
			ret[j] = pw[0];
			c = pw[1];
			NumMul++;
		#endif
	}
	return c;
}

#define pSbbMs(r,x,m,c)(r - ((x * m) + c))

int psbb_mul1(buftype *ret, buftype *x, buftype y, unsigned int xLen)
{
	unsigned long int j; 
	buftype c,*pw,r;

	#ifdef bufsize32
	  #ifdef __64__
			unsigned __int64 m,w;
			
	  #else
			unsigned long int m,w[2],v[2];
		  #endif
	#else 
			unsigned long int m,w;
	#endif

	Sbbsss+=xLen;
	


	c = 0; m = y;pw = &w;
    for ( j=0; j<xLen; j++ )
    {
		// c:ret->buf[j] = ret[j] - (y * x->buf[j]) + c;
		#ifdef bufsize32
			#ifdef __64__
				//w=(ret[j]-((x[j]*m)+c));
				w = pSbbMs(ret[j],x[j],m,c);
				ret[j]= pw[0];
				c = -pw[1];
				NumMul++;
			#else 
				#ifdef WIN32
				{						
					unsigned long int pret,xx;
					//r = ret[j];
					pret = &ret[j];
					//pret = &r;
					xx = x[j];
					__asm 
					{
						mov eax,xx;
						mov edx,m;
						mov ebx,pret;
						mov ecx,dword ptr [ebx];
						mul edx;
						//Hasta aqui tenemos que edx:eax = (x[j] * y)
						clc;
						add eax,c
						jnc LSbbMul1
						inc edx
LSbbMul1:
						//Hasta aqui tenemos que edx:eax = (x[j] * y) + c
						clc;
						sbb	ecx,eax
						jnc LSbbMul2
						inc edx
LSbbMul2:
						//Hasta aqui tenemos que edx:ecx = Ret[j]-((x[j] * y) + c)

						mov dword ptr [ebx],ecx;
						mov c,edx
					}
				}
				#else
					w[0] = ret[j];
					w[1] = 0;
					mul32(v,x[j],m);
					add6432(v,c);
					Sbb6464(w,v);
					c = -w[1];
					ret[j]= w[0];
				#endif
			#endif
		#else
			w=(ret[j]-((x[j]*m)+c));
			ret[j]= pw[0];
			c = -pw[1];
			NumMul++;

		#endif
	}
	
	return (c);
}
/*
buftype pfast_mul_ui( buftype *ret, buftype *x, buftype y, unsigned int xLen)
{
	buftype *pw,c;
	unsigned int i;
	#ifdef bufsize32
	  #ifdef __64__
			unsigned __int64 w,v,m;
	  #else
			unsigned long int w[2],v,m;
			buftype r;
	  #endif
	#else
	  unsigned long int w,v,m;
	#endif

	c = 0;pw = &w;m = y;
	for (i = 0;i<xLen;i++)
	{
		w = (x[i] * m) + c;
		ret[i] = pw[0];
		c = pw[1];
	}
	return c;
}

*/


void pfast_mulSQ( buftype *a, buftype *x, unsigned int xLen,unsigned int keep )
{
	// Ret = (x*y) % (2**keep)
	unsigned long int i,j,limit; // size of result in words
	unsigned long int min1,min2;
	unsigned long int c; // carry
	limit = (keep+BPU-1)/BPU; // size of result in words

	memset(a,0,limit*bufsize);


	min1 = xLen; 
	if (min1>limit) 
		min1 = limit;

	for (i=0; i<min1; i++)
	{
		min2 = i+xLen; if (min2>limit) min2 = limit;
		c = padd_mul1(&a[i],x,x[i],min2-i);
		j = min2;
		while ( c && j<limit )
		{
			a[j]+= c;
			c = a[j++] < c;
		}
	}
	// eliminate unwanted bits
	keep %= BPU; if (keep) a[limit-1] &= (1<<keep)-1;
}



void pfast_mul( buftype *a, buftype *x, buftype *y, unsigned int xLen,unsigned int yLen,unsigned int keep )
{
	// Ret = (x*y) % (2**keep)
	unsigned long int i,j,limit; // size of result in words
	unsigned long int min1,min2;
	unsigned long int c; // carry
	limit = (keep+BPU-1)/BPU; // size of result in words

	memset(a,0,limit*bufsize);


	min1 = xLen; 
	if (min1>limit) 
		min1 = limit;

	for (i=0; i<min1; i+=1)
	{
		min2 = i+yLen; if (min2>limit) min2 = limit;
		c = padd_mul1(&a[i],y,x[i],min2-i);
		j = min2;
		while ( c && j<limit )
		{
			a[j]+= c;
			c = a[j++] < c;
		}
	}
	// eliminate unwanted bits
	keep %= BPU; if (keep) a[limit-1] &= (1<<keep)-1;
}


void fast_mul( LNUM a, LNUM x, LNUM y, unsigned int keep )
{
	unsigned long int limit; 
	limit = (keep+BPU-1)/BPU; // size of result in words
	if ((limit*bufsize)>a->SizeMem)
	{
		//free(a->buf);
		InitLen(a,limit); 
	}
	else
	{
		a->Len = limit;
	}
	pfast_mul( a->buf, x->buf, y->buf, x->Len,y->Len,keep);
	// calcula n
	while (limit && a->buf[limit-1]==0) limit--;
	a->Len = limit;
}




void MontyMul( LNUM Ret, LNUM a,LNUM b,TMonty *Mn)
{
	LNUM T,k;
	LNUMInit(T);
	LNUMInit(k);

	

	Mul2n(T,a,b,Mn->N*2);
	//a[s]+= (a[s] * zminv) * zn;
	Mul2n(k,T,Mn->n1,Mn->N);
	Mul2n(Ret,k,Mn->m,Mn->N*2);

/*


	fast_mul(T,a,b,Mn->N*2);
	fast_mul(k,T,Mn->n1,Mn->N);
	fast_mul(Ret,k,Mn->m,Mn->N*2);
*/	
/*
	MulModFFT2n(T,a,b,Mn->N*2);
	MulModFFT2n(k,T,Mn->n1,Mn->N);
	MulModFFT2n(Ret,k,Mn->m,Mn->N*2);
*/
	//x += T;
	Add(Ret,Ret,T);

	//x.value->shr( N );
	ShrNB(Ret,Mn->N);

	//if (x>=m) x -= m;
	if (Compare(Ret,Mn->m)>=0)
	{
		Sbb(Ret,Ret,Mn->m);
	}
	LNUMFree(T);
	LNUMFree(k);
}

void MontyInit(TMonty *Mn,LNUM M)
{
	LNUM Temp;
/*
	if (LoadLNUM("MntyInit.dat",Mn->R,Mn->R1,Mn->m,Mn->n1,Temp,NULL)==TRUE)
	{
		Mn->N = ULIntFromLNUM(Temp);
		return;
	}
*/	
	

	LNUMInit(Mn->R);
	LNUMInit(Mn->R1);
	LNUMInit(Mn->m);
	LNUMInit(Mn->n1);
	
	FromCopy(Mn->m,M);
	Mn->N = 0;
	SetFromInt(Mn->R,1);
	LNUMInit(Temp);

	//while ( R < M ) { R += R; N += 1; }
	while ( Compare(Mn->R,M)<0)
	{
		Add(Mn->R, Mn->R, Mn->R);
		Mn->N++;
	}
	Sbb(Temp,Mn->R,Mn->m);
	
	//R1 = modinv( R-m, m );
	modinv( Mn->R1,Temp, Mn->m );

	//n1 = R - modinv( m, R );
	modinv( Temp,Mn->m, Mn->R );
	Sbb(Mn->n1,Mn->R,Temp);
	

	SetFromInt(Temp,Mn->N);
//	SaveLNUM("MntyInit.dat",Mn->R,Mn->R1,Mn->m,Mn->n1,Temp,NULL);
	

	LNUMFree(Temp);

	
}

void MontyFree(TMonty *Mn)
{
	LNUMFree(Mn->R);
	LNUMFree(Mn->R1);
	LNUMFree(Mn->m);
	LNUMFree(Mn->n1);
}
/*
void MontExp( LNUM Ret,LNUM x, LNUM e, TMonty *Mn)
{
	long i;
	buftype j;
	unsigned long k = 0;
	LNUM loca,b;
	LNUMInit(loca);
	LNUMInit(b);
	SetFromInt(loca,0);

	NumMods = 0;	

	Sbb(b,Mn->R,Mn->m);
	lmulmod(loca,x,Mn->R,Mn->m);

	k = 0;
	for (i = e->Len-1; i>=0; i--)
	{
		j = e->buf[i];
		
		if (!k)
		{
			k = 1;
			while ((k << 1) <= j)	k <<= 1;
		}
		if (k==1)
			k<<=1;

		while (k>>=1)
		{
			MontyMul(b,b,b,Mn);

			if (j & k)
			{
				MontyMul(b,loca, b, Mn);
			}
			//k >>= 1;
		}
		k = 1<<(bufbitlen-1);
	}
	
	//if (e[0] < 0 && zinv(b, n, &b))
	//	zhalt("undefined quotient in zexpmod");
		
	lmulmod(Ret,b,Mn->R1,Mn->m);
	FromCopy(Ret,b);
	LNUMFree(loca);
	LNUMFree(b);
}
*/

void MontExp( LNUM Ret,LNUM x, LNUM e, TMonty *Mn)
{
	
	LNUM result,t; 
	unsigned long int bits,i;

	LNUMInit(result);
	LNUMInit(t);
	//printf("MontExp\n");

	//vlong result = R-m;
	Sbb(result,Mn->R,Mn->m);
	lmulmod(t,x,Mn->R,Mn->m);
	bits = nbits(e);
	i = 0;
	while (1)
	{
		if (test(e,i))
		{
			MontyMul( result, result,t,Mn);
			//printNum("result",result);
		}
		i++;
		if ( i == bits ) 
			break;
		MontyMul( t, t,	t,Mn);
		//printNum("t",t);10000000000000001
	}

	//return ( result * R1 ) % m;
	lmulmod(Ret,result,Mn->R1,Mn->m);
	LNUMFree(result);
	LNUMFree(t);
}







#define NBITS			bufbitlen
//#define RADIX           (1L<<NBITS)
//#define RADIXM          (RADIX-1)
#define RADIXM          bufmask
#define H_RADIX         (1L<<(NBITS-1))

#define NBITSH          (NBITS>>1)
#define RADIXROOT       (1L<<NBITSH)
#define RADIXROOTM      (RADIXROOT-1)

static double fradix = (double)RADIXM + 1.0;
/*
static double epsilon = 0.0;
static double fudge = -1.0;

void zstart()
{
	double local_one = (double) 1;
	double local_half = 1 / (double) 2;
	epsilon = local_one;
	fudge = local_one + epsilon;

 	while (local_one != fudge)
	{
		epsilon = epsilon * local_half;
		fudge = local_one + epsilon;
	}
	epsilon += epsilon;
	if ((epsilon * fradix) > local_one)
	{
		fprintf(stderr,
			"zstart failure: recompile with smaller NBITS\n");
		exit(0);
	}
	epsilon *= 3;
	fudge = fradix + epsilon * fradix;
}

*/
#define zaddmulp(  a,  b,  d,  t) { \
    register unsigned long b1 = b & RADIXROOTM; \
    register unsigned long d1 = d & RADIXROOTM; \
    register unsigned long bd,b1d1,m,aa= (*a) + (*t); \
	register unsigned long ld = (d>>NBITSH); \
	register unsigned long lb = (b>>NBITSH); \
 \
        bd=lb*ld; \
        b1d1=b1*d1; \
        m=(lb+b1)*(ld+d1) - bd - b1d1; \
        aa += ( b1d1+ ((m&RADIXROOTM)<<NBITSH)); \
        (*t) = (aa >> NBITS) + bd + (m>>NBITSH); \
        (*a) = aa & RADIXM; \
}

/*
void zsubmul(buftype r,	LNUM A,LNUM B)
{
	buftype rd;
	long i;
	unsigned __int64 carry;
	long a64,rd64,b64,*pa64;
	unsigned short int *usix,*usim,usic;
	unsigned __int64 ret;
	__int64 iret;

	buftype *a,*b;

	carry = bufmask;
	carry++;


	rd=(-1*r);
	rd&=bufmask;

	a = A->buf;
	b = B->buf;
	
	usix = a;
	usim = b;

	for (i = B->Len-1; i >= 0; i--)
	{
		{
			//zaddmulp(a, *b, rd, &carry);
		}
		ret = *b;
		ret*=rd;
		ret+=carry;
		ret+=*a;
		*a=ret; 
		carry = ret>>bufbitlen;

		a++;
		carry += RADIXM - (*b);
		//carry &=bufmask;
		b++;
	}

	carry-=bufmask;
	carry--;
	carry+=*a;
	*a= carry;
	
	return ;
}
*/
// r-=(b*c);
// ret = signo
/*
int SubMul(LNUM r, LNUM a,buftype b)
{
	LNUM tmp;
	buftype c;
	int posneg;
	LNUMInit(tmp);
	InitLen(tmp,a->Len+1);
	c = padd_mul1(tmp->buf,a->buf,b,a->Len);
	if (c>0)
	{
		tmp->buf[a->Len] = c;
	}
	else
		tmp->Len--;


	Sbb(r,r,tmp);
	LNUMFree(tmp);
	return r->Len;
}
*/

void SubMul(LNUM r, LNUM a,buftype b)
{
	buftype c;
	int posneg,aLen;
	
	aLen = nabs(a->Len);
	//psbb_mul1_64(r->buf,a->buf,b,a->Len,&c);
	
	c = psbb_mul1(r->buf,a->buf,b,a->Len);
	

	
//	printf("%08X  %08X",r->buf[aLen],c);
	r->buf[aLen]-= c;
	aLen++;

	for (;aLen>=0;aLen--)
	{
		if (r->Len==0)
			break;
		if (r->buf[aLen-1]!=0)
			break;
	}
	r->Len=aLen;
//	printf("%08X\n",aLen);
	

	

	return;
}



void divide4( buftype *ret ,buftype *rem,buftype *x, buftype *y)
{
	buftype m[8],s[4];
	int c;

	memset(ret,0,bufsize*4);
	memcpy(rem,x,bufsize*4);
	
	memcpy(m,y,bufsize*4);
	memset(s,0,bufsize*4);s[0]=1;
	if ((m[0]==0)&&(m[1]==0)&&(m[2]==0)&&(m[3]==0))
	{
		ret[0]=bufmask;
		ret[1]=bufmask;
		ret[2]=bufmask;
		ret[3]=bufmask;
		return;
	}
	
	while ( bufcmp4(rem , m )>0)
	{
		bufshl4(m);
		bufshl4(s);
		if (m[4]>0) break;
	}
	while ( bufcmp4(rem , y )>=0)
	{
		while ( bufcmp4(rem , m )<0)
		{
			bufshr4(m);
			bufshr4(s);
		}
		c = 0;p__sbbu(rem ,rem,m,4,4,c);
		c = 0;p__addu(ret ,ret,s,4,4,c);
	}
}


buftype BufAuxDiv(buftype *DD,buftype *Ds,int DDLen,int DsLen)
{
	int i,j,c;
	double fret,fret8;
	#ifdef bufsize8
	buftype bufDD[8],bufDs[8],bufDiv[8],bufRem[8];
	#else
	buftype bufDD[4],bufDs[4],bufDiv[4],bufRem[4];
	#endif

	switch(DDLen)
	{
		case 2:
			bufDD[3]=DD[1];bufDD[2]=DD[0];bufDD[1]=0;bufDD[0]=0;
			break;

		case 3:
			#ifdef bufsize8
			bufDD[7]=DD[2];bufDD[6]=DD[1];bufDD[5]=DD[0];bufDD[4]=DD[-1];bufDD[3]=DD[-2];bufDD[2]=DD[-3];bufDD[1]=DD[-4];bufDD[0]=0;
			#else
			bufDD[3]=DD[2];bufDD[2]=DD[1];bufDD[1]=DD[0];bufDD[0]=0;
			#endif
			break;
	}
	c = 1;p__inc(bufDD,3,c);
	//pinc4(bufDD);

	#ifdef bufsize8
		bufDs[7]=0;
	#else
		bufDs[3]=0;
	#endif
	
	switch(DsLen)
	{
		case 1:
			bufDs[2]=Ds[0];bufDs[1]=0;bufDs[0]=0;
			break;
		case 2:
			bufDs[2]=Ds[1];bufDs[1]=Ds[0];bufDs[0]=0;
			break;
		case 3:
			#ifdef bufsize8
			bufDs[6]=Ds[2];bufDs[5]=Ds[1];bufDs[4]=Ds[0];bufDs[3]=Ds[-1];bufDs[2]=Ds[-2];bufDs[1]=Ds[-3];bufDs[0]=Ds[-4];
			#else
			bufDs[2]=Ds[2];bufDs[1]=Ds[1];bufDs[0]=Ds[0];
			#endif
			break;
	}
#ifdef	__LNUMDOUBLE__
	
	#ifdef bufsize8
	fdivide8(&fret,bufDD,bufDs,fradix);
	#else
	fdivide4(&fret,bufDD,bufDs,fradix);
	{
		unsigned __int64 DD64,Ds64,r64,rst64A,rst64B;
		double fDs;

		DD64 = bufDD[3];
		DD64<<=32;
		DD64|= bufDD[2];

		Ds64 = bufDs[3];
		Ds64<<=32;
		Ds64|= bufDs[2];
		

		r64 = DD64/Ds64;
		rst64A = DD64-(r64 * Ds64);
		rst64B = (r64*bufDs[1])>>32;
		if (rst64B>rst64A)
			r64--;
		if (r64>bufmask)
			r64 = bufmask;
		return (buftype)r64;
	}
	#endif
	if (fret>fradix)
		fret=fradix-1;
	return (buftype)fret;
#else
	divide4(bufDiv,bufRem,bufDD,bufDs);
	if (bufDiv[1]>0)
		return bufmask;
	return bufDiv[0];
#endif
}

void Div(LNUM q,LNUM r, LNUM Dividendo,LNUM Divisor)
{
	static long i,j;
	buftype qq;
	LNUM a,b,c,ppcc;

	long sa;
	long sb;
	long sq;
	buftype *p;
	buftype *pc;
	long sign;
	//double btopinv;
	//double aux;
	buftype *Dv,*DD;
	int DvLen,DDLen;


	fradix = (double)bufmask + 1.0;



	LNUMInit(a);LNUMInit(b);
	FromCopy(a,Dividendo);
	FromCopy(b,Divisor);
	
	sign = (a->Len < 0 ? 2 : 0) | (b->Len < 0 ? 1 : 0);
	if (a->Len < 0)
		a->Len = -a->Len;
	sa = a->Len;
	if (b->Len < 0)
		b->Len = -b->Len;
	sb = b->Len;
	LNUMInit(c);
	InitLen(c,(sa > sb ? sa : sb) + 1);
	if (r!=NULL)
		InitLen(r, sb);
	if (q!=NULL)
		InitLen(q, (sa > sb ? sa : sb) + 1);
	p = &b->buf[sb-1];

//	if ((sb == 1) && (*p < RADIX))
//		zintoz(zsdiv(a, *p, &c), &r);
//	else
	{
		sq = sa - sb;

		DvLen = sb;	
		if (DvLen>3) 
			DvLen = 3;
		Dv = p-(DvLen-1);
		if (sb==1)
		{
			DvLen = DvLen;
		}
		p = a->buf;
		pc = c->buf;
		memcpy(pc,p,sa*bufsize);pc+=sa;
		p = pc;
		sa = 1 - sb;
		if (sq<0)
			memset(p,0,-sq*bufsize);
		*pc = 0;
		ppcc->Len = b->Len;
		ppcc->SizeMem = -1;
		for (i = sq; i >= 0; i--)
		{
			DD = pc-1;	DDLen = 2;
			if (i > sa)
			{
				DD--;DDLen++;
			}

			qq = BufAuxDiv(DD,Dv,DDLen,DvLen);
			if (qq > 0)
			{
				ppcc->buf = &c->buf[i];
				SubMul(ppcc,b,qq);
				
				while (*pc >= bufNeg)
				{
					qq--;
					Add(ppcc,ppcc,b);
					ppcc->Len--;
				}
				QuitaCeros(ppcc);
			}
			ppcc->Len+=1;
			pc--;
			if (q!=NULL)
				q->buf[i] = qq;
		}	
		sb--;
		while ((sb > 0) && (!(c->buf[sb])))	sb--;
		sb++;
		if (r!=NULL)
		{
			r->Len = sb;
			p = &r->buf[0];
			pc = &c->buf[0];
			for (i = sb; i > 0; i--) *p++ = *pc++;
		}
		sq++;
		if (q!=NULL)
		{
			while ((sq > 1) && (!(q->buf[sq])))
				sq--;
			q->Len = sq+1;
		}
		else
		{
			while (sq > 1)
				sq--;
		}

		
		
	}	/* non-trivial case */
/*
	if (sign)
	{
		if ((sign <= 2) && (!((r[0] == 1) && !(r[1]))))
		{
			if (sign == 1)
				zsub(r, b, &r);
			else
				zsub(b, r, &r);
		}
		else
			znegate(&r);	
	}
*/
	if (q!=NULL)
	{
		QuitaCeros(q);
	}
	LNUMFree(a);LNUMFree(b); LNUMFree(c);
	

}

/*
void oDiv(LNUM RetCociente,LNUM RetResto, LNUM Dividendo,LNUM Divisor)
{
	LNUM m,s,Cociente,Resto;
	int i,j,k;

	LNUMInit(m);
	LNUMInit(s);

	if (RetCociente!=NULL)
	{
		LNUMInit(Cociente);
		SetFromInt(Cociente,0);
	}
	LNUMInit(Resto);


	
	FromCopy(Resto,Dividendo);
	FromCopy(m,Divisor);
	SetFromInt(s,1);

	
	i=j=k=0;
	while ( Compare(Resto,m)>0)
	{
		ShlNB(m,1);
		ShlNB(s,1);
	}
	while ( Compare(Resto,Divisor)>=0)
	{
		i++;
		while ( Compare(Resto,m)<0)
		{
			j++;
			ShrNB(m,1);
			ShrNB(s,1);
		}
		Sbb(Resto, Resto, m);

		if (RetCociente!=NULL)
		{
			Add(Cociente, Cociente, s);
		}
	}
	if (RetCociente!=NULL)
	{
		FromCopy(RetCociente,Cociente);
	}
	if (RetResto!=NULL)
	{
		FromCopy(RetResto,Resto);
		QuitaCeros(RetResto);
	}

	LNUMFree(m);
	LNUMFree(s);
	if (RetCociente!=NULL)
	{
		LNUMFree(Cociente);
		QuitaCeros(RetCociente);
	}
	LNUMFree(Resto);
	
	

}
*/

void ModExp(LNUM Ret,LNUM x, LNUM e,LNUM m)
{
	TMonty Mn;
	int i,n;
	

	MontyInit(&Mn,m);
	/*
	printNum("Mn.m",Mn.m);
	printNum("Mn.n1",Mn.n1);
	printNum("Mn.R",Mn.R);
	printNum("Mn.R1",Mn.R1);
	*/
	MontExp( Ret,x, e, &Mn);
	MontyFree(&Mn);
}






	



void modinv( LNUM ret,LNUM a, LNUM m ) // modular inverse
// returns i in range 1..m-1 such that i*a = 1 mod m
// a must be in range 1..m-1
{
	LNUM j,b,c,x,y,tmp,i;
	int count;
	LNUMInit(j);LNUMInit(b);LNUMInit(c);LNUMInit(x);LNUMInit(y);LNUMInit(i);LNUMInit(tmp);
	SetFromInt(j,1);
	SetFromInt(i,0);
	FromCopy(b,m);
	FromCopy(c,a);
	
	count = 0;
	while ( NotZero(c))
	{
		//oDiv(x,NULL,b,c);
		
		iDiv(x,NULL,b,c);
		//Div(x,NULL,b,c);
		//printNum("x",x);
		Mul(tmp,x,c);

		Sbb(y,b,tmp);

		
		FromCopy(b,c);
		
		FromCopy(c,y);
		
		FromCopy(y,j);
		Mul(tmp,j,x);
		Sbb(j,i,tmp);
		FromCopy(i,y);
		count++;
	}

	if ( CmpZero(i) < 0 )
	{
		Add(i,i,m);
	}
	FromCopy(ret,i);
	LNUMFree(j);LNUMFree(b);LNUMFree(c);LNUMFree(x);LNUMFree(y);LNUMFree(i);LNUMFree(tmp);
}

//c = (a ^ 2) % n;
void lsqmod(LNUM c, LNUM a, LNUM n)
{
	LNUM ctmp;
	int sb;
	LNUMInit(ctmp);
	//mulmod(c,a,a,n);

	
//	Mod(a,a,n);
	Mul2n(ctmp,a,a,(2*a->Len)*bufbitlen);
	Mod(c,ctmp,n);
	

	LNUMFree(ctmp);
}
/*
void lmulmod(LNUM c, LNUM a,LNUM b, LNUM n)
{
	LNUM ctmp;
	buftype carry;
	int i;

	LNUMInit(ctmp);
	
	//mulmod(c,a,b,n);

	InitLen(ctmp,(a->Len+b->Len));
	//InitLen(ctmp,a->Len+b->Len);
	for (i=0;i<b->Len;i++)
	{
		carry = padd_mul1(&ctmp->buf[i],a->buf,b->buf[i],a->Len);
		ctmp->Len = a->Len+i;
		if (carry) 
		{
			ctmp->buf[i+a->Len]=carry;
			ctmp->Len++;
		}
		Mod(c,ctmp,n);
		FromCopy(ctmp,c);
	}

	LNUMFree(ctmp);
}
*/
//c = (a * b) % n;
void lmulmod(LNUM c, LNUM a,LNUM b, LNUM n)
{
	LNUM ctmp;
	LNUMInit(ctmp);
	//mulmod(c,a,b,n);

	Mul2n(ctmp,a,b,(a->Len+b->Len)*bufbitlen);
	Mod(c,ctmp,n);
	LNUMFree(ctmp);
}


#define nBitsUi(b,e){int k;k= 1<<31;b=32;while (!(k & e)) {k>>=1;b--;}};


void lexp_mod_ui(LNUM b, LNUM x, unsigned long int e, LNUM n)
{
	unsigned long int bits,i,k;
	LNUM loca;
	LNUMInit(loca);
	
	Mod(loca, x, n);
	FromCopy(b,loca);


	//{k = 1<<31;bits=32;while (!(k & e)) {k>>=1;bits--;}};
	nBitsUi(bits,e);
	i = bits-1;
	k = (1<<(i-1));
	while (i)
	{
		lsqmod(b,b,n);//b=(b*b)%n
		if (e & k)
		{
			lmulmod(b,b, loca, n);//b=(b*l)%n
		}
		i--;
		k>>=1;
	}
	LNUMFree(loca);
}
void lexp_mod(LNUM b, LNUM x, LNUM e, LNUM n)
{
	unsigned long int bits,k;
	long int i,j;
	LNUM loca,bpower;
	LNUMInit(loca);
	LNUMInit(bpower);
	NumMul=0;
	Sbbsss = Addsss = 0;

	Mod(loca, x, n);
	SetFromInt(b,1);
	FromCopy(bpower,loca);

	bits = nbits(e);

	j = 0;
	for (i=0;i<bits-1;i++)
	{
		if (test(e,i))
		{
			lmulmod(b,b, bpower, n);//b=(b*p)%n
		}
		lsqmod(bpower,bpower,n);//p=(p*p)%n
		//printNum("bpower",bpower);
	}
	lmulmod(b,b, bpower, n);//b=(b*p)%n

	LNUMFree(loca);
	LNUMFree(bpower);
}


void lexp_mod_old(LNUM b, LNUM x, LNUM e, LNUM n)
{
	unsigned long int bits,i,k;
	LNUM loca,bcmp;
	LNUMInit(loca);
	LNUMInit(bcmp);
	NumMul=0;

	Mod(loca, x, n);
	FromCopy(b,loca);

	bits = nbits(e);

	i = bits-1;
	//i = 0;
	//l = x%n
	while (i)
	{
		lsqmod(b,b,n);//b=(b*b)%n
		if (test(e,--i))
		{
			lmulmod(b,b, loca, n);//b=(b*l)%n
			//printNum("b",b);
		}
	}

	LNUMFree(loca);
}


/*
void lexp_mod(LNUM b, LNUM x, LNUM e, LNUM n)
{
	long i;
	buftype c;
	buftype j;
	buftype k = 0;
	LNUM loca;
	LNUMInit(loca);
	SetFromInt(loca,0);

	NumMods = 0;	


	Mod(loca, x, n);
	FromCopy(b,loca);
	k = 0;c = 0;
	for (i = e->Len-1; i>=0; i--)
	{
		j = e->buf[i];
		
		if (!k)
		{
			k = 1;
			while ((k << 1) <= j)	k <<= 1;
		}

		while ((c==1)||(k>>=1))
		{
			c = 0;
			//mulmod(b, b, b, n);
			lsqmod(b,b,n);
			if (j & k)
			{
				lmulmod(b,loca, b, n);
			}
		}
		//k = 1<<(bufbitlen-1);
		c = 1;
		k = 1<<(bufbitlen-1);
	}
	LNUMFree(loca);
}
*/
/*
void exp_mod(LNUM s, LNUM x, LNUM y, LNUM n)
{
	LNUM t,u;
	LNUM t2,s2;
	LNUMInit(t);
	LNUMInit(u);
	LNUMInit(s);
	LNUMInit(t2);
	LNUMInit(s2);
	
	//t=x; u=y;
	SetFromInt(s,1);
	FromCopy(t,x);
	FromCopy(u,y);
	


	while(NotZero(u)!=0)
	{
		if (u->buf[0]&1) 
		{
			Mul2n(s2,s,t,0x80*bufbitlen);
			//Mul(s2,s,t);
			Mod(s,s2,n);
			
		}
		ShrNB(u,1);
		Mul2n(t2,t,t,0x80*bufbitlen);
		//Mul(t2,t,t);
		Mod(t,t2,n);
		
	}
}

*/
int findchar(char *buffer,char c)
{
	char *pc = (char *)strchr( buffer, c);
	if (pc==NULL)
		return -1;
	return (pc-buffer);
}

void InitRnd()
{
	srand( (unsigned)time( NULL ) );
}

void SetFromRnd(LNUM a,int Len) 
{
	int i,memlen;
	char *buf;
	InitLen(a,Len);
	memlen = Len*bufsize;
	buf = a->buf;
	for (i=0;i<memlen;i++)
	{
		buf[i] = rand();
	}
}
void SetFromStr(LNUM a,char *strcadena) 
{
	int rlen = strlen(strcadena);
	int j,k,pos,len;
	unsigned int n,d;
	int i;
	char *cadena;

	a->Len = rlen/(2*bufsize);
	n = rlen % (bufsize*2);
	if (n>0)
		a->Len++;
	len = (a->Len*bufsize)*2;

	d = (len-rlen);
	cadena = strcadena;
	if (d>0)
	{
		cadena = malloc(len);
		for (i=rlen-1;i>=0;i--)
			cadena[i+d] = strcadena[i];
		memset(cadena,'0',d);
	}

	
	if (a->SizeMem<((len/(2*bufsize))*sizeof(buftype)))
	{
		if (a->SizeMem>0)
			free(a->buf);
		a->SizeMem = ((len/(2*bufsize))*sizeof(buftype));
		a->buf = (buftype*)malloc(a->SizeMem);
	}
	memset(a->buf,0,a->SizeMem);
	
	i=(len-(2*bufsize));

	for (j=0;i>=0;i-=(2*bufsize),j++)
	{
		n =0;
		for (k=0;k<(2*bufsize);k++)
		{
			pos = findchar((char *)Hex, cadena[i+k]);
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
	if (d>0)
	{
		free(cadena);
	}

}

void SetFromText(LNUM a,char *strcadena,int RSAByteslen) 
{
	int len,i;
	char *buf;
	if (a->SizeMem<RSAByteslen)
	{
		
		free(a->buf);
		a->buf=malloc(RSAByteslen);
		a->SizeMem=RSAByteslen;
	}

	buf = a->buf;
	len = strlen(strcadena);
	for (i=0;i<len && i<RSAByteslen;i++)
	{
		buf[i]=strcadena[i];
	}
	for (;i<RSAByteslen;i++)
	{
		buf[i]=0x11;
	}
	a->Len = RSAByteslen/bufsize;
	if ((RSAByteslen%bufsize)!=0)
		a->Len++;

}

buftype divide2b(buftype *bufDD,buftype *bufDs)
{
#ifdef bufsize32 
	#ifdef __64__
		unsigned __int64 DD64,Ds64,r64,rst64A,rst64B;
	#else 
	#ifdef WIN32
		unsigned __int64 DD64,Ds64,r64,rst64A,rst64B;
	#endif
	#endif
#endif
#ifdef bufsize16
	unsigned int DD64,Ds64,r64,rst64A,rst64B;
#endif
#ifdef bufsize8
	unsigned short int DD64,Ds64,r64,rst64A,rst64B;
#endif

	DD64 = bufDD[2];
	DD64<<=bufbitlen;
	DD64|= bufDD[1];

	Ds64 = bufDs[2];
	Ds64<<=bufbitlen;
	Ds64|= bufDs[1];

	r64 = DD64/Ds64;
	rst64A = DD64-(r64 * Ds64);
	rst64B = (r64*bufDs[0])>>bufbitlen;
	if (rst64B>rst64A)
		r64--;
	if (r64>bufmask)
		r64 = bufmask;
	return (buftype)r64;
}


void iDiv(LNUM q,LNUM r, LNUM Dividendo,LNUM Divisor)
{
	int DifLen,i,DDSign,DsSign;
	buftype *PosDs,BufDiv[4],*PosDD;
	LNUM ppcc,CDividendo;
	buftype ret;

	DDSign = Dividendo->Len;
	DsSign = Divisor->Len;

	Dividendo->Len = nabs(Dividendo->Len);
	Divisor->Len = nabs(Divisor->Len);

	DifLen	= Dividendo->Len - Divisor->Len;
	if (DifLen==0)
	{
		if (CompareUnsigned(Dividendo,Divisor)<0)
		{
			Dividendo->Len = DDSign;
			Divisor->Len = DsSign;
			if (q!=NULL)
				SetFromInt(q,0);
			if (r!=NULL)
				FromCopy(r,Dividendo);
			return;
		}
	}
	if (DifLen<0)
	{
		Dividendo->Len = DDSign;
		Divisor->Len = DsSign;
		if (q!=NULL)
			SetFromInt(q,0);
		if (r!=NULL)
			SetFromInt(r,0);
		return;
	}

	LNUMInit(CDividendo);
	InitLen(CDividendo,Dividendo->Len+2);
	FromCopy(CDividendo,Dividendo);
	CDividendo->buf[Dividendo->Len] = 0;

	if (q!=NULL)
		InitLen(q, DifLen + 1);

	BufDiv[3]=0;
	BufDiv[2]=0;
	BufDiv[1]=Divisor->buf[Divisor->Len-1];
	BufDiv[0]=Divisor->buf[Divisor->Len-2];
	ppcc->Len = Divisor->Len;
	ppcc->SizeMem = -1;
	ppcc->buf = &CDividendo->buf[DifLen];
	PosDD = &CDividendo->buf[CDividendo->Len-2];
	for (i=DifLen;i>=0;i--)
	{
		ret = divide2b(PosDD,BufDiv);
		if (ret > 0)
		{
			SubMul(ppcc,Divisor,ret);
			if (PosDD[2] >= bufNeg)
			{
				while (PosDD[2] >= bufNeg)
				{
					ret--;
					Add(ppcc,ppcc,Divisor);
					
				}
				ppcc->Len--;
			}
			
			QuitaCeros(ppcc);
		}

		if (q!=NULL)
			q->buf[i] = ret;

		ppcc->Len++;
		ppcc->buf--;
		PosDD--;
	}
	if (r!=NULL)
	{
		ppcc->Len--;
		ppcc->buf++;
		FromCopy(r,ppcc);
	}
	LNUMFree(CDividendo);

	Dividendo->Len = DDSign;
	Divisor->Len = DsSign;

	if (q!=NULL)
	{
		if ((DDSign*DsSign)<0)
			q->Len*=-1;
	}
}



/*
El algoritmo Diffie-Hellman
1. Introducción
En 1976 W. Diffie y M.E. Hellman encontraron un algoritmo de intercambio de claves de forma segura. Para ello es necesario encontrar un sistema de cifrado computacionalmente fácil de forma que el descifrado sea computacionalmente difícil, a menos que se disponga de la clave.


--------------------------------------------------------------------------------
2. Algoritmo de ejemplo
Paso 1:
El usuario A elige una clave pública Apub y el usuario B elige una clave pública Bpub. 

Paso 2:
El usuario A elige una clave privada Apriv y el usuario B elige una clave privada Bpriv. 

Paso 3:
A partir de las claves públicas el usuario A calcula Atx=(Bpub^Apriv) % Apub y se lo envía a B, mientras que B calcula Btx=(Bpub^Bpriv) % APub y se lo envía a A. 

Paso 4:
Finalmente los usuarios A y B pueden calcular una clave conocida por ambos. 

Aclave = (Btx^Apriv) % Apub
Bclave = (Atx^Bpriv) % Apub

donde CA = CB = C. 


--------------------------------------------------------------------------------
3. Ejemplo práctico
Supongamos un usuario A con clave pública Apub=7 y clave privada Apriv=9, y un usuario B con clave pública Bpub=5 y clave privada Bpriv=11.

El usuario A calcula Atx=(Bpub^Apriv) % Apub = 5^9 % 7 = 6 y lo envía al usuario B. 

El usuario B calcula Btx=(Bpub^Bpriv) % APub = 5^11 % 7 = 3 y lo envía al usuario A. 

Los usuarios A y B calculan la clave:

- Aclave = (Btx^Apriv) % Apub = 3^9 % 7 = 6
- Bclave = (Atx^Bpriv) % Apub = 6^11 % 7 = 6
*/
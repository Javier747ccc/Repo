#ifndef LNUM_FUNCION_MACROS
#define LNUM_FUNCION_MACROS
//#define mul32(ret ,a, b) {unsigned long p1, p2, p3, p4,sum; p1 = lo(a) * lo(b); p2 = lo(a) * hi(b); p3 = hi(a) * lo(b); p4 = hi(a) * hi(b); sum = hi(p1) + lo(p2) + lo(p3); ret[0] = lo(p1) + lh(sum);  ret[1] = hi(sum) + hi(p2) + hi(p3) + p4 ;};
//#define add6432(ret ,a){unsigned long r;r = *ret;*ret+= a;if (*ret<r){ret[1]++;};}




#define mul32(ret ,a, b)						\
{												\
	unsigned long p1, p2, p3, p4,sum;			\
	p1 = lo(a) * lo(b);							\
	p2 = lo(a) * hi(b);							\
	p3 = hi(a) * lo(b);							\
	p4 = hi(a) * hi(b);							\
	sum = hi(p1) + lo(p2) + lo(p3);				\
	ret[0] = lo(p1) + lh(sum);					\
	ret[1] = hi(sum) + hi(p2) + hi(p3) + p4 ;	\
}


#define add6432(ret ,a)							\
{												\
	unsigned long r;							\
	r = *ret;									\
	*ret+= a;									\
	if (*ret<r)									\
		ret[1]++;								\
}

#define Sbb6432(ret ,a)							\
{												\
	unsigned long r;							\
	r = *ret;									\
	*ret-= a;									\
	if (*ret>r)									\
		ret[1]--;								\
}

#define Sbb6464(ret ,a)							\
{												\
	unsigned long r,c,i;						\
	c = 0;										\
	for (i=0;i<2;i++)							\
	{											\
		r = ret[i];								\
		ret[i]-= (a[i]+c);						\
		if (ret[i]>r)							\
			c=1;								\
	}											\
}

//ret = src+(a*b)+c
#define muladd6432(ret,src,a,b,c)				\
{												\
	unsigned long p1, p2, p3, p4,sum;			\
	p1 = lo(a) * lo(b);							\
	p2 = lo(a) * hi(b);							\
	p3 = hi(a) * lo(b);							\
	p4 = hi(a) * hi(b);							\
	sum = hi(p1) + lo(p2) + lo(p3);				\
	ret[0] = lo(p1) + lh(sum);					\
	ret[1] = hi(sum) + hi(p2) + hi(p3) + p4 ;	\
	sum = ret[0];								\
	ret[0]+= src;								\
	if (ret[0]<sum)								\
		ret[1]++;								\
	sum = ret[0];								\
	ret[0]+= c;									\
	if (ret[0]<sum)								\
		ret[1]++;								\
}



#define paddu(ret ,a,b,sizeA,sizeB,c)			\
{												\
	int i,cy1,cy2,sz1,sz2;						\
	buftype sl,rl,*p1,*p2,*pret;				\
	pret = ret;									\
	if (sizeA>=sizeB)							\
	{	p1=a;p2=b;sz1=sizeA;sz2=sizeB;}			\
	else										\
	{	p1=b;p2=a;sz1=sizeB;sz2=sizeA;}			\
	for(i = 0 ; i < sz2; i++){					\
		sl = *p1 + *p2;	cy1 = sl < *p1;			\
		rl = sl + c;cy2 = rl < sl;c = cy1 | cy2;\
		*pret  = rl;							\
		p1++;p2++;pret++;}						\
	for(; i < sz1; i++){						\
		rl = *p1 + c;cy1 = rl < *p1;c = cy1;	\
		*pret  = rl;							\
		p1++;p2++;pret++;}						\
}
/*

#define paddu(ret ,a,b,sizeA,sizeB,c)			\
{												\
	int i,sz1,sz2;								\
	buftype sl,rl,*p1,*p2,*pret;				\
	pret = ret;									\
	if (sizeA>=sizeB)							\
	{	p1=a;p2=b;sz1=sizeA;sz2=sizeB;}			\
	else										\
	{	p1=b;p2=a;sz1=sizeB;sz2=sizeA;}			\
	for(i = 0 ; i < sz2; i++){					\
		rl=p1[i];sl=rl;rl+=(p2[i]+c);			\
		if (rl<sl) c=1;							\
		else c=0;								\
		pret[i]  = rl;}							\
	for(; i < sz1; i++){						\
		rl=p1[i];sl=rl;rl+=c;					\
		if (rl<sl) c=1;							\
		else c=0;								\
		pret[i]  = rl;}							\
}
*/

#define p_inc(a,sizeA,c)						\
{												\
	int i,cy1,sz1;								\
	buftype rl,*p1;							\
	p1=a;sz1=sizeA;								\
	for(i = 0 ; i < sz1; i++){					\
		rl = *p1 + c;cy1 = rl < *p1;c = cy1;	\
		*p1  = rl;								\
		p1++;}									\
}


#define psbbu(ret ,a,b,sizeA,sizeB,Neg)			\
{												\
	int c,i,cy1,cy2,sz1,sz2;					\
	buftype sl,rl,*p1,*p2,*pret;				\
	c = 0;										\
	pret = ret;									\
	if (sizeA!=sizeB)							\
		(sizeA>sizeB) ? (c=1):(c=-1);			\
	else										\
	{											\
		for (i=(sizeA-1);i>=0;i--)				\
		{										\
			if (a[i]==b[i]) continue;			\
			(a[i]>b[i]) ? (c=1):(c=-1);			\
			break;								\
		}										\
	}											\
	if (c>=0)									\
	{	p1=a;p2=b;sz1=sizeA;sz2=sizeB;Neg=1;}	\
	else										\
	{	p1=b;p2=a;sz1=sizeB;sz2=sizeA;Neg=-1;}	\
	for(i = 0, c = 0; i < sz2; i++){			\
		sl = *p1 - *p2;	cy1 = sl > *p1;			\
		rl = sl - c;cy2 = rl > sl;c = cy1 | cy2;\
		*pret  = rl;							\
		p1++;p2++;pret++;}						\
	for(; i < sz1; i++){						\
		rl = *p1 - c;cy1 = rl > *p1;c = cy1;	\
		*pret  = rl;							\
		p1++;p2++;pret++;}						\
}




#define psbb_mul1_64(ret, x, y, xLen,carry)	\
{											\
	unsigned long int j;					\
	unsigned __int64 m,w;					\
	buftype c,*pw;							\
	c = 0;pw = &w;	m = y;					\
    for ( j=0; j<xLen; j++ )				\
    {										\
		w=(ret[j]-((x[j]*m)+c));			\
		ret[j]= pw[0];						\
		c = -pw[1];							\
	}										\
	*carry = c;								\
}


#define fdivide8( ret ,x, y,frad)			\
{											\
	unsigned long int *xx,*yy;				\
	double fDD,fDs;							\
	xx = x;yy = y;							\
	fDD =xx[1]*4294967296.0;				\
	fDD+=xx[0];								\
	fDs =yy[1]*4294967296.0;				\
	fDs+=yy[0];								\
	fDD/=fDs;								\
	*ret = fDD;								\
}

/*
#define fdivide8( ret ,x, y,frad)			\
{											\
	buftype *xx,*yy;						\
	double fDD,fDs;							\
	xx = x;yy = y;							\
	fDD =xx[7]*frad;						\
	fDD+=xx[6];fDD*=frad;					\
	fDD+=xx[5];fDD*=frad;					\
	fDD+=xx[4];fDD*=frad;					\
	fDD+=xx[3];fDD*=frad;					\
	fDD+=xx[2];fDD*=frad;					\
	fDD+=xx[1];fDD*=frad;					\
	fDD+=xx[0];								\
	fDs =yy[7]*frad;						\
	fDs+=yy[6];fDs*=frad;					\
	fDs+=yy[5];fDs*=frad;					\
	fDs+=yy[4];fDs*=frad;					\
	fDs+=yy[3];fDs*=frad;					\
	fDs+=yy[2];fDs*=frad;					\
	fDs+=yy[1];fDs*=frad;					\
	fDs+=yy[0];								\
	fDD/=fDs;								\
	*ret = fDD;								\
}
*/
#define fdivide4( ret ,x, y,frad)			\
{											\
	buftype *xx,*yy;						\
	double fDD,fDs;							\
	xx = x;yy = y;							\
	fDD =xx[3]*frad;						\
	fDD+=xx[2];fDD*=frad;					\
	fDD+=xx[1];fDD*=frad;					\
	fDD+=xx[0];								\
	fDs =yy[3]*frad;						\
	fDs+=yy[2];fDs*=frad;					\
	fDs+=yy[1];fDs*=frad;					\
	fDs+=yy[0];								\
	fDD/=fDs;								\
	*ret = fDD;								\
}

#define fdivide2( ret ,x, y,frad)			\
{											\
	buftype *xx,*yy;						\
	double fDD,fDs;							\
	xx = x;yy = y;							\
	fDD =xx[1]*frad;						\
	fDD+=xx[0];								\
	fDs =yy[1]*frad;						\
	fDs+=yy[0];								\
	fDD/=fDs;								\
	*ret = fDD;								\
}


#define p__addu(ret,pa,pb,aLen,bLen,c) paddu(ret,pa,pb,aLen,bLen,c)
#define p__sbbu(ret,pa,pb,aLen,bLen,c) psbbu(ret,pa,pb,aLen,bLen,c)
#define p__inc(a,SizeA,c) p_inc(a,SizeA,c)


#endif //LNUM_FUNCION_MACROS
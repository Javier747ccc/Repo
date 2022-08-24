#define LNUMCODE32

#ifdef LNUMCODE8
	#include "lm8.h"
#endif

#ifdef LNUMCODE16
	#include "lm16.h"
#endif

#ifdef LNUMCODE32
	#include "lm32.h"
#endif

#ifdef  __cplusplus
extern "C" {
#endif

#define MAX_DIGITS 256

#define MAX(x,y) ((x) > (y) ? (x) : (y))
//#define pswap(a,b)(a^= b; b = a ^ b; a^= b)

#define Swap(a, b) (a ^= b, b ^= a, a ^= b)


#define bufcmp1(a,b)((a[0]>b[0]) ? 1 : (a[0]<b[0]) ? -1 : 0)
#define bufcmp2(a,b)((a[1]>b[1]) ? 1 : (a[1]<b[1]) ? -1 : bufcmp1(a,b))
#define bufcmp3(a,b)((a[2]>b[2]) ? 1 : (a[2]<b[2]) ? -1 : bufcmp2(a,b))
#define bufcmp4(a,b)((a[3]>b[3]) ? 1 : (a[3]<b[3]) ? -1 : bufcmp3(a,b))

#define getLbit(a)((a&1)<<bufbitlenminus1)
#define bufshr1(a) a[3]>>=1;
#define bufshr2(a) a[2]=a[2]>>1|getLbit(a[3]);bufshr1(a)
#define bufshr3(a) a[1]=a[1]>>1|getLbit(a[2]);bufshr2(a)
#define bufshr4(a) a[0]=a[0]>>1|getLbit(a[1]);bufshr3(a)

#define getHbit(a)(a>>bufbitlenminus1)
#define bufshl1(a) a[0]<<=1;
#define bufshl2(a) a[1]=a[1]<<1|getHbit(a[0]);bufshl1(a)
#define bufshl3(a) a[2]=a[2]<<1|getHbit(a[1]);bufshl2(a)
#define bufshl4b(a) a[3]=a[3]<<1|getHbit(a[2]);bufshl3(a)
#define bufshl4(a) a[4] = getHbit(a[3]);bufshl4b(a)





/*
void FFTMul(buftype *prod, buftype *a, buftype *b, int Len);
void InitFFT(unsigned long int Len,int Base,int BaseDig);
void DeInitFFT(unsigned long int Len);
*/


typedef struct 
{
	int Len;
	unsigned int SizeMem;
	buftype *buf;
	unsigned int op;
}PLNUM;




#define nabs(n) ((n<0) ? -1*n:n)

typedef PLNUM LNUM[1];
typedef struct 
{
	unsigned long int N;
	LNUM R,R1,m,n1;
	LNUM T,k;   // work registers
}TMonty;



void    karatsuba(LNUM a, LNUM b, LNUM ret, int d);
void	gradeSchool(LNUM ret,LNUM a, LNUM b) ;
void	Sbb(LNUM r, LNUM a, LNUM b) ;
void	Add(LNUM r, LNUM a, LNUM b) ;
void	Mul(LNUM r, LNUM a, LNUM b) ;
void	SetFromInt(LNUM ,int l) ;
void	SetFromStr(LNUM n,char *cadena) ;
void	SetFromText(LNUM a,char *strcadena,int RSAByteslen);
void	lexp_mod(LNUM s, LNUM x, LNUM y, LNUM n);
void	lexp_mod_ui(LNUM s, LNUM x, unsigned long int y, LNUM n);
void	modinv( LNUM ret,LNUM a, LNUM m ); // modular inverse
void	Div(LNUM Cociente,LNUM Resto, LNUM Dividendo,LNUM Divisor);
void	iDiv(LNUM Cociente,LNUM Resto, LNUM Dividendo,LNUM Divisor);
void	LNUMInit(LNUM n);
void	InitLen(LNUM n,int memlen);
int		Compare(LNUM a,LNUM b);
int		CompareUnsigned(LNUM a,LNUM b);
void	FromCopy(LNUM n,LNUM l) ;
void	LNUMFree(LNUM n);
void	printNum(char *cadena,LNUM a) ;
void	OutputNum(char *str,LNUM a) ;
void	pNum(LNUM a) ;
void	ShlNB(LNUM a,unsigned int n) ;
void	ShrNB(LNUM a,unsigned int n) ;
int		NotZero(LNUM n);
void	ModExp(LNUM Ret,LNUM x, LNUM e,LNUM m);
void	Norm(LNUM a,unsigned int nBits);
#define Zero(n) !NotZero(n)


#define Mod(m,d,s) iDiv(NULL,m,d,s);


#define CalcFHTLen(_NumLen) ((((_NumLen)*1)*2)/1)
#define MY_PI      3.1415926535897932384626434
#define MY_SQRT_2  0.7071067811865475244008443621
#define MY_SQRT2   1.41421356237309504880

typedef unsigned char uchar ;






#define BPU bufbitlen       // Number of bits in an unsigned
#define BPU2 bufbitlen/2    // Number of bits in an unsigned/2
#define lo(x) ( (x) & maskL ) // lower half of unsigned
#define hi(x) ( (x) >> (BPU2) )         // upper half
#define lh(x) ( (x) << (BPU2) )         // make upper half


#ifdef  __cplusplus
}
#endif

#ifndef _LONGMATH_INCLUDE_
#define _LONGMATH_INCLUDE_

#define MAX_DIGITS 256

#define bufbitlen 32

#if bufbitlen==8
	typedef unsigned char buftype ;
	#define bufmask 0xFF
	#define bufsize 1
	#define bufsize8
#else 
	#if bufbitlen==16
		typedef unsigned short buftype ;
		#define bufmask 0xFFFF
		#define bufsize 2
		#define bufsize16
	#else 
		#if bufbitlen==32
			typedef unsigned int buftype ;
			#define bufmask 0xFFFFFFFF
			#define bufsize 4
			#define bufsize32
		#endif
	#endif
#endif

typedef struct 
{
	int Len;
	unsigned int SizeMem;
	buftype *buf;
	unsigned int op;
}PLNUM;


#define nabs(n) ((n<0) ? -1*n:n)

typedef PLNUM LNUM[1];



void    karatsuba(LNUM a, LNUM b, LNUM ret, int d);
void    gradeSchool(LNUM a, LNUM b, LNUM ret, int d);
void	Sbb(LNUM r, LNUM a, LNUM b) ;
void	Add(LNUM r, LNUM a, LNUM b) ;
void	Mul(LNUM r, LNUM a, LNUM b) ;
void	SetFromInt(LNUM ,int l) ;
void	SetFromStr(LNUM n,char *cadena) ;
void	exp_mod(LNUM s, LNUM x, LNUM y, LNUM n);
void	exp_mod_ui(LNUM s, LNUM x, int y, LNUM n);
void	modinv( LNUM ret,LNUM a, LNUM m ); // modular inverse
void	Div(LNUM Cociente,LNUM Resto, LNUM Dividendo,LNUM Divisor);
void	Init(LNUM n);
void	InitLen(LNUM n,int memlen);
int		Compare(LNUM a,LNUM b);
int		CompareUnsigned(LNUM a,LNUM b);
void	FromCopy(LNUM n,LNUM l) ;
void	Free(LNUM n);
void    pNum(LNUM a);
void	printNum(char *cadena,LNUM a) ;
void	OutputNum(char *str,LNUM a) ;



#endif //_LONGMATH_INCLUDE_
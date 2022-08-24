#ifndef _LONGMATH_INCLUDE_
#define _LONGMATH_INCLUDE_

#ifdef  __cplusplus
extern "C" {
#endif




#define bufbitlen 32
#define bufbitlenminus1 31
typedef unsigned int buftype ;
#define bufmask 0xFFFFFFFF
#define bufNeg	0x80000000
#define maskH	0xFFFF0000
#define maskL	0x0000FFFF
#define bufHbit	bufNeg
#define bufsize 4
#define bufsize32



#ifdef  __cplusplus
}
#endif


#endif //_LONGMATH_INCLUDE_
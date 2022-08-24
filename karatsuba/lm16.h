#ifndef _LONGMATH_INCLUDE_
#define _LONGMATH_INCLUDE_

#ifdef  __cplusplus
extern "C" {
#endif




#define bufbitlen 16
#define bufbitlenminus1 15
typedef unsigned short buftype ;
#define bufmask 0xFFFF
#define bufNeg	0x8000
#define maskH	0xFF00

#define maskL	0x00FF
#define bufHbit	bufNeg
#define bufsize 2
#define bufsize16



#ifdef  __cplusplus
}
#endif


#endif //_LONGMATH_INCLUDE_
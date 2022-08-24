#ifndef _LONGMATH_INCLUDE_
#define _LONGMATH_INCLUDE_

#ifdef  __cplusplus
extern "C" {
#endif




#define bufbitlen 8
#define bufbitlenminus1 7

typedef unsigned char buftype ;
#define bufmask 0xFF
#define bufNeg	0x80
#define maskH	0xF0
#define maskL	0x0F
#define bufHbit	bufNeg
#define bufsize 1
#define bufsize8



#ifdef  __cplusplus
}
#endif


#endif //_LONGMATH_INCLUDE_
#ifndef ___DBASE_DEFINES_H___
#define ___DBASE_DEFINES_H___

#include <stdio.h>


#define LEN_48					48
#ifndef NULL
#define NULL	0
#endif //NULL

union REGDATATYP
{
	unsigned int UINT;
	unsigned int INT;
	char * CHAR;
};

unsigned int CallGateControl(union REGDATATYP RegEAX,union REGDATATYP RegEBX,union REGDATATYP RegECX,union REGDATATYP RegEDX);
void DumpHex(char *buf,int rows,char *Name);

#define DEF_GET_ROOT_CLUSTER		0x0705	//Peticion DBRootCluster 
#define DEF_READ_ABS_SECTORS		0x0100	//Lectura absoluta de sectores
#define DEF_WRITE_ABS_SECTORS		0x0200	//Escritura absoluta de sectores

//#define kmalloc			malloc
#define krealloc		realloc
//#define kfree			free

void *kmalloc(size_t size,const char *comentario=NULL);
void kfree(void *pnt);
int DameDebugMem();
void PrintDebugMem();



#endif //___DBASE_DEFINES_H___


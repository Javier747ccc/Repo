#include <stdio.h>
#include <math.h>
#include <memory.h>
#include <windows.h>
#include <time.h>


#include "lm.h"


#define __PRUEBAS__


BOOL SaveLNUM(char *filename,LNUM a,...)
{
	FILE *Fich;
	LNUM *n;
	unsigned int Len[0x100];
	int Count,i;
	

	
	va_list arglist;
	va_start(arglist, a);

	Fich = fopen(filename,"wb");

	n = (LNUM *)a;
	Count = 0;
	while( n != NULL )
	{
		Len[Count] = n[0]->Len;
		Count++;
		n = (LNUM *)va_arg( arglist, LNUM*);
	}
	fwrite(&Count,sizeof(int),1,Fich);
	for (i=0;i<Count;i++)
	{
		fwrite(&Len[i],sizeof(int),1,Fich);
	}
	va_start(arglist, a);
	n = (LNUM *)a;
	while( n != NULL )
	{
		fwrite(n[0]->buf,sizeof(buftype),n[0]->Len,Fich);
		n = (LNUM *)va_arg( arglist, LNUM*);
	}



	va_end( arglist ); 
	fclose(Fich);
	return TRUE;

}

BOOL LoadLNUM(char *filename,LNUM a,...)
{
	FILE *Fich;
	LNUM *n;
	unsigned int Len[0x100];
	int Count,i;
	buftype *buf;
	

	
	va_list arglist;
	va_start(arglist, a);

	Fich = fopen(filename,"rb");
	if (Fich==NULL)
		return FALSE;

	n = (LNUM *)a;

	fread(&Count,sizeof(unsigned int),1,Fich);
	fread(Len,sizeof(unsigned int ),Count,Fich);
	for (i=0;i<Count;i++)
	{
		buf = malloc(sizeof(buftype)*Len[i]);
		fread(buf,sizeof(buftype),Len[i],Fich);
		n[0]->buf = buf;
		n[0]->Len = Len[i];
		n[0]->SizeMem = sizeof(buftype)*Len[i];


		n = (LNUM *)va_arg( arglist, LNUM*);
	}
	va_end( arglist ); 
	fclose(Fich);
	return TRUE;
}

int printd(int n,const char *format, ...)
{
	#ifdef P1
		va_list arglist;
		va_start(arglist, format);
		vprintf(format,arglist);
	#endif
	#ifdef P2
		if (n==2)
		{
			va_list arglist;
			va_start(arglist, format);
			vprintf(format,arglist);
		}
	#endif
	return 0;
} 




BOOL LNUMInitAll(LNUM a ,...)
{
	LNUM *n;

	va_list arglist;
	va_start(arglist, a);
	n = (LNUM *)a;
	while( n != NULL )
	{
		n[0]->buf = (buftype * )malloc(sizeof(buftype));
		memset(n[0]->buf,0,sizeof(buftype));
		n[0]->Len = 1;
		n[0]->SizeMem = sizeof(buftype);

		n = (LNUM *)va_arg( arglist, LNUM*);
	}
	return TRUE;
}



void OutputNum(char *str,LNUM a) 
{
	int i,j;
	if (a->Len<0)
		printf("-");
	i=abs(a->Len)-1;
	for (;i>=0;i--)
	{
		if (a->buf[i]!=0)
			break;
	}

	for (j=0;i>=0;i--,j+=(2*bufsize))
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
	int i;
	printf("%s = ",cadena);
	if (a->Len<0)
		printf("-");
	i=abs(a->Len)-1;
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

void printNumBin(char *cadena,LNUM a) 
{
	int bits,i,count1s;
	count1s = 0;
	printf("%s = ",cadena);
	bits = nbits(a);
	for (i=bits-1;i>=0;i--)
	{
		if (test(a,i))
		{
			printf("1");count1s++;
		}
		else
			printf("0");
	}
}

void pNum(LNUM a) 
{
	int i;
	if (a->Len<0)
		printf("-");
	i=abs(a->Len)-1;
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

#ifdef __PRUEBAS__

/*
void ccc()
{
	double c,rpm,L,PI,S,t,Vm,Vkm,G,m,a,R,e,g,h,b,v;// a=(v*v)/R; 
	G = 6.667E-11;
	m = 5.98E24;
	PI = 3.1415926535;
	t = 2 * 3600;
	R = pow ( ((G*m*t*t)/(2*PI*2*PI)), 1.0 / 3.0 );
	h = R - 6378000;

	R = 6378000;
	//R = 6378000+35800000;
	//h = 1682500;
	//R = 6378000+h;
	g = G*(m/(R*R));
	e = (2*PI*R);
	Vm = sqrt(g*R);
	t = (e/Vm)/3600;
	Vkm=(Vm*3600)/1000;
	a=(Vm*Vm)/R;

	v = Vm;
	for (t=1;t<10;t+=1.0)
	{
		b = sqrt(R*R+v*v);
		b-= R;
		v+=Vm;
		e = 0.5 * 9.8 * t * t;
	}

}
*/

unsigned int __fastcall imulmod(unsigned int a, unsigned int BB, unsigned int m)
{
    /* return a*b (mod m).
     * take special care of unsigned quantities and overflow.
     */
    unsigned int z;
    unsigned int y = a;
    int t;
	unsigned int b;
	b=BB;

    z = 0;
    for (;;) 
	{
        if (b & 1) 
		{
            z += y;
            if (z < y || z >= m) 
				z -=m;
        }
        b >>= 1;
        if (!b) 
			break;
        t = y;
        y <<= 1;
        if (t < 0 || y >= m) 
			y -= m;
    }
    return z;
}

/*
unsigned __int64 iMod(unsigned __int64 Dividendo,unsigned __int64 Divisor)
{
	unsigned __int64  m,Resto;
	int i,j,k,df;

	Resto = Dividendo;
	m = Divisor;

	
	i=j=k=0;
	while ( Resto>m>0)
	{
		m<<=1;
		i++;
	}
	
	i = 0;j=0;
	while ( Resto>=Divisor)
	{
		i++;
		while ( Resto<m)
		{
			j++;
			m>>=1;
		}
		Resto-=m;
	}
	return Resto;
}
*/



int main1() 
{
	LNUM	a,b,c,d;
	LNUM	m,e,x;
	clock_t start, finish;
	double  duration;
	TMonty Mn;
	

	LNUMInitAll(a,b,c,d,m,e,x,NULL);
	//SetFromStr(m,"e1608e8209f9e3085211834b20d3fa50df211e997c6fc51ab4dedc3841b644fe5770f9c9b10e15b44c6a2738ccaa3b739f721b564e47e46a907497dd518199e2b6cdfcbb6bcda026f51a8b91376b9adbc5d1509c03e208f7fc39c8c61e38d91a3794de0bd9fa0a05502f711a22095e0c0e6511d6eaa777d4a84223b77e064799");
	//SetFromStr(x,"1dda17367b3d0aa0bdab5908c4ef70f5f398e17d8254b8e1d13403b2cc85f5fe734c64313175e8f9d917208a20ac43d44f525a3214101748082c0e4c30eac2e7ccf45859d238b5de810cdd68e91db804717e3b1428b930e3611641b10a548a378d14423668b0fa5d93d04ef4194dafb842444da9a3d57037a947736347b847db");
	
	SetFromStr(m,"96b1632c9457ecda2abcbdd7ab700cfbcb2bafb3e1dd6dd9fcefadb89980ae81f7520e04d78ba0ab843a841e89b97ddadc84a05f4f6c319c35a266a73ee420dacc58369b7ddc0eed7de3a484329e062794776787ecfa1f59dac8c52b51313a4f00475dd8da5ceae060a14a079c80b39d8f98ea5f49524c449c2ec68f87b43e3cb4e2254cf9341ca65f7c662af6218fed2200b6c2bfc58839aecc0a34878694bee7220e382a53c0043a3a4e013dddde32192b57571c11e39492d90575ee3c17729306e39d8ee60be2b14e9521b3e950ed226a85a5d42bb86b2f9223ea9803f5852018f31267b77337819607890486ae98839f5dd07bd0e5a5c3e579856b804ad1");
	SetFromStr(x,"1dda17367b3d0aa0bdab5908c4ef70f5f398e17d8254b8e1d13403b2cc85f5fe734c64313175e8f9d917208a20ac43d44f525a3214101748082c0e4c30eac2e7ccf45859d238b5de810cdd68e91db804717e3b1428b930e3611641b10a548a378d14423668b0fa5d93d04ef4194dafb842444da9a3d57037a947736347b847db1dda17367b3d0aa0bdab5908c4ef70f5f398e17d8254b8e1d13403b2cc85f5fe734c64313175e8f9d917208a20ac43d44f525a3214101748082c0e4c30eac2e7ccf45859d238b5de810cdd68e91db804717e3b1428b930e3611641b10a548a378d14423668b0fa5d93d04ef4194dafb842444da9a3d57037a947736347b847db");
	
	MontyInit(&Mn,m);
	SetFromInt(e,0x10001);
	printNum("x",x) ;



start = clock();
	//MontyInit(&Mn,m);
	MontExp( c,x, e, &Mn);
	printNum("c",c) ;
	//SetFromStr(e,"c0d769988308b9e608b2cb70a12a3fe8aac31ff79cadd62d3fcde10891d16fbf60fa5e00e1ea774fca829a04d3ba651c4db5c90653fcd7c906c97c97f681ba34d320911b3a1f196ec0256d25c7209ee2fc5c7792aea90905b6f465283fbd7949f62d861958bccc7a93df020839e28e857484920695a52d7c3a030682277229c1");
	SetFromStr(e,"2c823655ab7710d91b7865e63697c0edfa036c41083c5bbee00b76d72857aeb36a764cf7b726618d675fd53823dbaac5a30e7543d174966e9a14de9715675220edec3c2ccf0ed945e242038c9c5aba5ca4686426eca2aff1dd4221a27ab7bc8e2af9e9492cadf71dd0ccb2cdeeb6c0d68fdc4d955955ba7b255a57817b468c7ef3b00cf0b9f067f07d85d4b331e7390f320972c624d42d4d2ddd912b7e7e6a8e4538baa7bf883c17be775111fd4950fa555939f21cd0a283d741e2f02eb5a803252c0f668603c7b8558199d684f216a118265c27b3d1d844e74604b35e8b48fd2978c5712034cb5c11e45f1f6e542f81a1c2e6203a7445d9b507ece5da436731");
	//MontyInit(&Mn,m);
	MontExp( x,c, e, &Mn);

finish = clock();

	printNum("x",x) ;

	MontyFree(&Mn);

		

	LNUMFree(a); 
	LNUMFree(b);
	LNUMFree(c);
	LNUMFree(d);
	duration = (double)(finish - start) / CLOCKS_PER_SEC;
	printf( "%2.5f seconds\n", duration );
	printf( "\n");
}

int main2() 
{
	LNUM	RNT,RNA,RN[8];
	LNUM	m,e,x,c;
	clock_t start, finish;
	double  duration;
	int i,NumSUM;
	buftype buf;

	
	LNUMInit(m);LNUMInit(x);LNUMInit(c);LNUMInit(e);

/*
	SetFromStr(m,"923bf9e1");
	SetFromStr(x,"2233");
	iDiv(c,e, m,x);
*/
/*	
	SetFromStr(m,"1122");
	SetFromStr(x,"3344");
	karatsubaMul(c,m,x,2);
*/	
/*
	SetFromStr(m,"e1608e8209f9e3085211834b20d3fa50df211e997c6fc51ab4dedc3841b644fe5770f9c9b10e15b44c6a2738ccaa3b739f721b564e47e46a907497dd518199e2b6cdfcbb6bcda026f51a8b91376b9adbc5d1509c03e208f7fc39c8c61e38d91a3794de0bd9fa0a05502f711a22095e0c0e6511d6eaa777d4a84223b77e064799");
	SetFromStr(x,"1dda17367b3d0aa0bdab5908c4ef70f5f398e17d8254b8e1d13403b2cc85f5fe734c64313175e8f9d917208a20ac43d44f525a3214101748082c0e4c30eac2e7ccf45859d238b5de810cdd68e91db804717e3b1428b930e3611641b10a548a378d14423668b0fa5d93d04ef4194dafb842444da9a3d57037a947736347b847db");
*/
	SetFromStr(m,"96b1632c9457ecda2abcbdd7ab700cfbcb2bafb3e1dd6dd9fcefadb89980ae81f7520e04d78ba0ab843a841e89b97ddadc84a05f4f6c319c35a266a73ee420dacc58369b7ddc0eed7de3a484329e062794776787ecfa1f59dac8c52b51313a4f00475dd8da5ceae060a14a079c80b39d8f98ea5f49524c449c2ec68f87b43e3cb4e2254cf9341ca65f7c662af6218fed2200b6c2bfc58839aecc0a34878694bee7220e382a53c0043a3a4e013dddde32192b57571c11e39492d90575ee3c17729306e39d8ee60be2b14e9521b3e950ed226a85a5d42bb86b2f9223ea9803f5852018f31267b77337819607890486ae98839f5dd07bd0e5a5c3e579856b804ad1");
	SetFromStr(x,"1dda17367b3d0aa0bdab5908c4ef70f5f398e17d8254b8e1d13403b2cc85f5fe734c64313175e8f9d917208a20ac43d44f525a3214101748082c0e4c30eac2e7ccf45859d238b5de810cdd68e91db804717e3b1428b930e3611641b10a548a378d14423668b0fa5d93d04ef4194dafb842444da9a3d57037a947736347b847db1dda17367b3d0aa0bdab5908c4ef70f5f398e17d8254b8e1d13403b2cc85f5fe734c64313175e8f9d917208a20ac43d44f525a3214101748082c0e4c30eac2e7ccf45859d238b5de810cdd68e91db804717e3b1428b930e3611641b10a548a378d14423668b0fa5d93d04ef4194dafb842444da9a3d57037a947736347b847db");
	
	
	
	//SetFromText(x,"Hola esto es una prueba",256);

	//SetFromStr(m,"c6a40b43559632e006006982bb5c9ecd56469a932985daf886ff84b03942c6148e4fd495277099ddadb923a1715a5f1cc077cc3c281b3460d9b75c740e0848bd41182f6d4289cfce4edf6890bf93ced189e81397cf3bbcc12ae8b8459d53d2e1ad3ebafbf9868a4ee887590d21a5f302d34b94257f59aea61ae31fa73e3657af5df6fef03d179333a17dfa0b6216c7427d2dca9e68c46a14305a5e2290a3e908b55f3a4d44ce87dac4e627b0aca6ab15112b3ff3d482f7e98b11ff8373be8dbd47415414982c851dcefcfed6c4384eb4a8fc817c19304512888761adaf521e1dec40fb6afa1b81872f452ac65414524cc55b18a413e9687b51dff20fb5cadee5");
	//SetFromStr(m,"db86683857ea6cfacddc4a7a5b81c922f9559bef3646796fe43759424a8f301e6f6d9a14d753dfbdd0000d50bf4a983edfce5135f2ecff6d714e7adc07596d50909b13752ec7afad34dec378a44dc8f226eaac82d47a5099ff4dbaae18b7b3cfd375bf77cbddb85e362432235e3a6dd6f6aa0fa34a4c8f40ec33c4310593e367d21630fdc3c73c540df1eb162eab129b40b87926a55fd28c9b5fbc9cabb37eba7325b6fbcf210aa12fe4326ccc2c31f71b9703701ecf5825fc181209307635ddeb8c5abaf3692e2dc357338f19a2e5cbb1b432185e78dffc6b6cb5357f891af74eb5f95c3f2cf5ea285a38796ff47cddc8a897f03cbe9c4c9fff76dfca4ef32a7721b0981dae2b2073347faba77fcf3b9e0e65ce19e8ac683f780b9a15bc7b198c6a4f3468eedfa629e6ede346de89e2fd5b22726ece40fcd4c9fd1c3771886fb3dab0d8f3e0b13b169d1dece33f1ccd09afe4164ff1f83869e326636ea61a07e1900ff80c20a5ace97105502a951745c57dd9bf547c5d52b616f855f315fec0f8412ff53e92c47a7e939a9475060f57759d66fcfa2908a6f6658f5ba59a56686521249a310246685bfde8d981aa37708a6870ae1c1833141fb6dcbbe1c493935150f67325d365e84284d0d8698c5c0b918c17d360fb9917a6f03d9b907e48e31ebd06a537954590c3e39f521a006573cc007940b39ee569e3c5bd1e44bd9a63");
	//SetFromStr(x,"1dda17367b3d0aa0bdab5908c4ef70f5f398e17d8254b8e1d13403b2cc85f5fe734c64313175e8f9d917208a20ac43d44f525a3214101748082c0e4c30eac2e7ccf45859d238b5de810cdd68e91db804717e3b1428b930e3611641b10a548a378d14423668b0fa5d93d04ef4194dafb842444da9a3d57037a947736347b847db1dda17367b3d0aa0bdab5908c4ef70f5f398e17d8254b8e1d13403b2cc85f5fe734c64313175e8f9d917208a20ac43d44f525a3214101748082c0e4c30eac2e7ccf45859d238b5de810cdd68e91db804717e3b1428b930e3611641b10a548a378d14423668b0fa5d93d04ef4194dafb842444da9a3d57037a947736347b847db1dda17367b3d0aa0bdab5908c4ef70f5f398e17d8254b8e1d13403b2cc85f5fe734c64313175e8f9d917208a20ac43d44f525a3214101748082c0e4c30eac2e7ccf45859d238b5de810cdd68e91db804717e3b1428b930e3611641b10a548a378d14423668b0fa5d93d04ef4194dafb842444da9a3d57037a947736347b847db1dda17367b3d0aa0bdab5908c4ef70f5f398e17d8254b8e1d13403b2cc85f5fe734c64313175e8f9d917208a20ac43d44f525a3214101748082c0e4c30eac2e7ccf45859d238b5de810cdd68e91db804717e3b1428b930e3611641b10a548a378d14423668b0fa5d93d04ef4194dafb842444da9a3d57037a947736347b847db");


	SetFromInt(e,0x10001);
	printNum("x",x) ;

start = clock();


	lexp_mod(c,x,e,m);
	//lexp_mod_ui(c,x,0x10001,m);
	printNum("c",c) ;
	//lexp_mod_ui(c,x,0x10001,m);
/*
	finish = clock();
	duration = (double)(finish - start) / CLOCKS_PER_SEC;
	MontyFree(&Mn);
	printf( "%2.5f seconds\n", duration );
	printf( "\n", duration );
	return 0;
*/
	//SetFromStr(e,"c0d769988308b9e608b2cb70a12a3fe8aac31ff79cadd62d3fcde10891d16fbf60fa5e00e1ea774fca829a04d3ba651c4db5c90653fcd7c906c97c97f681ba34d320911b3a1f196ec0256d25c7209ee2fc5c7792aea90905b6f465283fbd7949f62d861958bccc7a93df020839e28e857484920695a52d7c3a030682277229c1");
	//SetFromStr(e,"187d3afd261999c4b4d8615a9b00b47e79502a9019feeca725cf62deb95095ee9f419bd1bd5fb6f306d42aa81a5ee8f302a6927ab139ed18b96e26d88add580b87c634fa7a987e29955f507111eb37828402e3f1c86048c3952e87a7ca4c1bfc103bef7b1ebc02484843eb6ff4a01206cc0796ad01a9dec540545a0b99f2b088b170165108d91e0ac66f796945158282dd4a77016fab6be291a6b174ccd6be497075019fbfb090ee0058b5a35b7786af695d845daaa238d42bf64774ccfa32f229c7c38b5d821b4bb1a1f5b77e05a51aff1cfa05abec521867a77de394f1a797752969c2f7ad3d55bce0abb228621ed7e2fda3cd820945886ac4e5eefb33a97e9b469ea79287042aea49502d88f7934b1f1901396b601d18a09adce572bdba4137afd52efec6e099216a8fce26288b9d38269c6355c467fc03d280b9ceb5f96c10576d7f3633117676a45671eaf8c66377a1df9a943c3951bc98911d3032c37e1e8e0370a932ab61cc322bba531021563c251fa764c07f09e844cf861c491c22482e1189f78d3ecfc2fbe2ecd00063194469ecbc6cf1dd6ed95de12f18b4d81ef4d3919006bb8526b4f3dfe04a623367d8b03a0ea904833db8c10e22860c28c8334b94d990070727a2eb5978be3aeba51efe6b51b097444cc11d18a40fe517a0ce33b9751e1b4384cbc7fb8fd35771c909bb625e2b7c507f15f1d99da7627739");
	//SetFromStr(e,"008956e272abba2d72b6b3923fd14b3880c02610b9aefbdd69f7ecb2d124c6062e303701c41e80bbe1893b786726f854ca8c4847ed4fcd7f69771e4fcdede5cc650d9cab2ddfd368da3bae3b9dd8d759278731ee5a07ee459349179e50837671579770c7d76fae95f1f8d39bbe7885447faf9791d65b36a68a320cd6d5cb2dc7713fe8c4d1919651efdd742ec6563d3d1fe4ad6b543dafb779fb104a9468e1b2a48825d3fccce25ef06c7a5dd401337e389768bbe202634883aee1c3e67de0f4a937ad91ef4df7d000d2be02ba044df57eb87d55273b6dfa1ae678f18971685575c9d5f3cde2befb2a5e9636e05b7bb01403828bb85a0031f92e98d8fd7cb12d");
	SetFromStr(e,"2c823655ab7710d91b7865e63697c0edfa036c41083c5bbee00b76d72857aeb36a764cf7b726618d675fd53823dbaac5a30e7543d174966e9a14de9715675220edec3c2ccf0ed945e242038c9c5aba5ca4686426eca2aff1dd4221a27ab7bc8e2af9e9492cadf71dd0ccb2cdeeb6c0d68fdc4d955955ba7b255a57817b468c7ef3b00cf0b9f067f07d85d4b331e7390f320972c624d42d4d2ddd912b7e7e6a8e4538baa7bf883c17be775111fd4950fa555939f21cd0a283d741e2f02eb5a803252c0f668603c7b8558199d684f216a118265c27b3d1d844e74604b35e8b48fd2978c5712034cb5c11e45f1f6e542f81a1c2e6203a7445d9b507ece5da436731");
/*
	InitRnd();
	
	buf = e->buf[e->Len-1];
	buf>>=2;

	LNUMInit(RNT);LNUMInit(RNA);
	SetFromInt(RNT,0);
	NumSUM=0;
	while (CompareUnsigned(e,RNT)>0)
	{
		LNUMInit(RN[NumSUM]);
		SetFromRnd(RN[NumSUM],e->Len);
		RN[NumSUM]->buf[e->Len-1] = buf;
		FromCopy(RNA,RNT);
		Add(RNT,RNT,RN[NumSUM]);
		NumSUM++;
	}
	Sbb(RN[NumSUM-1],e,RNA);

	SetFromInt(RNT,0);

	for (i=0;i<NumSUM;i++)
	{
		lexp_mod(x,c,RN[i],m);
		FromCopy(RN[i],x);
	}
	SetFromInt(RNT,1);
	for (i=0;i<NumSUM;i++)
	{
		lmulmod(RNT,RNT, RN[i], m);
	}
	printNum("x",RNT) ;
	//printNumBin("e",e) ;
*/
	lexp_mod(x,c,e,m);

finish = clock();

	printNum("x",x) ;
	LNUMFree(x); LNUMFree(c);LNUMFree(e);LNUMFree(m);

	duration = (double)(finish - start) / CLOCKS_PER_SEC;
	printf( "%2.5f seconds\n", duration );
	printf( "\n");

}



__int64 lexp_mod64(__int64 x, unsigned int e, __int64 n)
{
	unsigned long int bits,i,k;
	__int64 l,b;

	if (e==0)
		return 1;
	

	l = x%n;
	if (e==1)
		return l;
	b = l;
	k = 1<<31;bits=32;while (!(k & e)) {k>>=1;bits--;};
	i = bits-1;
	k = (1<<(i-1));
	printf("%X\n",b);
	while (1)
	{
		b = (b*b)%n;
		i--;
		if (e & k)
		{
			b = (b*l)%n;
		}
		
		if ( i == 0 ) 
			break;
		k>>=1;
	}
	return b;
}

#define bits_per_b	4
__int64 lexp_mod64B(__int64 x, unsigned int e, __int64 l,__int64 n)
{
	unsigned long int bits,i,k;
	__int64 b;

	b = x;
	k = 1<<(bits_per_b-1);
	i=bits_per_b;
	
	while (1)
	{
		b = (b*b)%n;
		printf("%X\n",b);
		if (e & k)
		{
			b = (b*l)%n;
			printf(">%X\n",b);
		}
		i--;
		if ( i == 0 ) 
			break;
		k>>=1;
	}
	return b;
}




__int64 lexp_mod64C(__int64 x, unsigned int e, __int64 n)
{
	unsigned long int bits,i,k,q;
	__int64 l,b,t;


	if (e==0)
		return 1;

	l = x%n;
	
	k = 1<<31;bits=32;while (!(k & e)) {k>>=1;bits--;};
	k = (bits%bits_per_b);
	i = bits/bits_per_b;
	if (k>0)
	{
		i++;
	}
	k = ((1<<(bits_per_b))-1)<<((i-1)*bits_per_b);
	b = 1;
	
	while (1)
	{
		
		t = (e & k);
		t >>= ((i-1)*bits_per_b);
		b = lexp_mod64B(b,t,l,n);
		i--;
		if ( i == 0 ) 
			break;
		k>>=bits_per_b;
	}
	return b;
}

unsigned long exp_mod(unsigned long x, unsigned long y, unsigned long n)
{
	unsigned long s,t,u;
	int i;

	s=1; t=x; u=y;
	while(u) {
		if (u&1) s=(s*t)%n;
		u>>=1;
		t=(t*t)%n;
	}
	return s;
}

unsigned __int64 SqMulRusa(unsigned __int64 a)
{
	//	1	45			+	45		45
	//	0	22				90

	//	1	11			+	180		225
	//	1	5			+	360		585
	//	0	2				720
	//	1	1			+	1440	2025
}

unsigned __int64 MulRusa(unsigned __int64 a,unsigned __int64 b)
{
	unsigned __int64 w;
	int nn;
	w = 0;nn = 0;
	while((a!=0)&&(b!=0))
	{
		if ((a&1)==1)
		{
			if (nn>0)
				b<<=nn;
			w+=b;
			
			nn = 0;
		}
		a>>=1;
		nn++;
		
	}
	return w;
}



double calcPi()
{
	unsigned int i;
	double a,b,r;
	a = 2.0;b = 1.0;
	r = 1;
	for (i=0;i<0x10000000;i++)
	{
		r*=(a/b);
		a+=2.0;b+=2.0;
	}
	a = 2.0;b = 3.0;
	for (i=0;i<0x10000000;i++)
	{
		r*=(a/b);
		a+=2.0;b+=2.0;
	}
	return r*2;

}

unsigned __int64 divide( unsigned __int64 x, unsigned __int64 y)
{
	unsigned __int64 m,s,ret,rem;
	int i,j,k;
	ret = 0;
	rem = x;
	
	m = y;
	s = 1;
	
	i=j=k=0;
	while ( rem > m )
	{
		m<<=1;
		s<<=1;
	}
	while ( rem>=y )
	{
		i++;
		while ( rem < m)
		{
			j++;
			m>>=1;
			s>>=1;
		}
		rem-=m;
		ret+=s;
	}
	return ret;
}




/*
void MMxPr()
{
	unsigned __int64 dato1,dato2,dato3,dato4;
	unsigned __int64 *pdato1,*pdato2,*pdato3,*pdato4;
	dato1=0x88884444;
	dato2=0x22226666;
	pdato1=&dato1;pdato2=&dato2;pdato3=&dato3;pdato4=&dato4;
	__asm mov esi,pdato1
	__asm mov edi,pdato2
	__asm mov ebx,pdato3

	__asm _emit 0fh;	//movq mm0, [edi]
	__asm _emit 6fh;
	__asm _emit 07h;

	__asm _emit 0fh;	//movq mm1, [esi]
	__asm _emit 6fh;
	__asm _emit 0eh;

	__asm _emit 0fh;	//movd mm0, [edi]
	__asm _emit 6eh;
	__asm _emit 07h;

	__asm _emit 0fh;	//movd mm1, [esi]
	__asm _emit 6eh;
	__asm _emit 0eh;

	
	__asm _emit 0fh;	//punpckldq mm0,mm0
	__asm _emit 062h;
	__asm _emit 0C0h;


	__asm _emit 0fh;	//pmaddwd mm0,mm1
	__asm _emit 0F5h;
	__asm _emit 0C1h;



	__asm _emit 0Fh;	//PMULHW mm0,mm1
	__asm _emit 0E5h
	__asm _emit 0C1h

	__asm _emit 0Fh;	//PMULLW mm0,mm1
	__asm _emit 0D5h
	__asm _emit 0C1h
	__asm _emit 0fh;
	__asm _emit 0fch;
	__asm _emit 0c1h;
	__asm _emit 0fh;	 //movd [ebx],mm0
	__asm _emit 7eh;
	__asm _emit 03h;

	__asm _emit 0fh;	 //movq [ebx],mm0
	__asm _emit 7fh;
	__asm _emit 03h;
}

*/


int main() 
{
	__int64 x,e,n,r1,r2,r3;
	double dpp;
	unsigned int DD,Ds,r;
/*
	HANDLE mhandle;
	mhandle  = GetCurrentProcess();
	SetPriorityClass(mhandle,HIGH_PRIORITY_CLASS);
*/

	main2();
}

		


  
#endif //__PRUEBAS__



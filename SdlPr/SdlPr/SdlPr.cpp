// SdlPr.cpp : Defines the entry point for the console application.
//

#include <windows.h>
#include <stdlib.h>
#include "SdlPr.h"



#include "SDL_main.h"
#include "SDL.h"
#include "SDL_TTF.h"
#include "SDL_thread.h"
#include "freeImage.h"
#define JPEG2000_SAVE

#ifdef JPEG2000_SAVE
#include "openjpeg.h"
#endif

#include <deque>
#include <math.h>
#include <time.h>
#ifdef WIN32
#include <windows.h>
#endif
#define NumCPUs		1
#define NPass		2
/*
#define PanWidth	1368
#define PanHeight	512
*/
#define PanWidth	1280
#define PanHeight	720

#define YOUNG			1
#define ROUNDFOCOS		2
#define FOCUSRAY		4
#define UNAREND			8
#define DOSFOCOS		16
#define TRESFOCOS		32
#define RANDOMFOCOS		64
#define HOLOFOCOS		128
#define RECHOLO			256
#define DEFTEST			512
#define RANDOMFOCOS2	1024
#define SAVETEST		2048
#define TRIANGLE		4096


#define nFocusRay	17
#define FocusRaySep	8
#define GENTEST

#define pi 3.14159265

int		MapaWidth = PanWidth;
int		MapaHeight =PanHeight;


#define SIN_HZ	0.08 * NPass

bool EndAllProcess = false;


float FACMUL = 2048.0;
float SinHz = SIN_HZ ;
//float MapFalloff = 0.997;
float MapFalloff = 1 ;
int DensidadFocos = 2;
float rayangle,rayfocus;
bool TestOn = true;
float GeneralsinGR = 0;
float Generalsin = 0;

bool bSaveJpeg = false;
#ifdef GENTEST
float LineTest[8192];
//float *LineTest = NULL;

#endif

SDL_mutex *mut;
SDL_mutex *mutPinta;
SDL_mutex *mutWait;

SDL_cond  *cndPinta;
SDL_cond  *cndWait;

//bool pinta;


SDL_Surface *image;
SDL_Surface *imageBack;
SDL_Surface *screen;
TTF_Font* fuente;

static int CommonPos = 0;


int DobleRend;

int Experimentos	= YOUNG | DEFTEST;

typedef struct
{
        unsigned char b,g,r,a;
}TColorRGB;

typedef struct TMAPPoint
{
	float Acc;
	float Pos;
	float Mat;
}TMAPPoint;

typedef struct
{
	int posXY;
	float sinGR;
	float IncGR;
	float sin;
	float OffTimeOut;
	float MaxSin;
	float Intens;
	int Delete;
}TFoco;
std::deque<TFoco> Focos;

//TMAPPoint mapa[MapaWidth*MapaHeight];
TMAPPoint *mapa[NumCPUs];

int (*pIdle)() = NULL; 
void SetIdleCallBack(int (*pnt)());


void RedrawScreen()
{
	SDL_LockMutex ( mutPinta);
	SDL_CondSignal(cndPinta);
	SDL_UnlockMutex ( mutPinta );
}


/*
void LoadGenericTexture(const char *filename,unsigned int *Width,unsigned int *Height,unsigned int *bpp,unsigned char **bits)
{
	
	FIBITMAP *bm = GenericLoader(filename, 0) ;
	*Width = FreeImage_GetWidth(bm);
	*Height = FreeImage_GetHeight(bm);
	unsigned int pitch = FreeImage_GetPitch(bm);
	*bits = (unsigned char *)FreeImage_GetBits(bm);
	*bpp = FreeImage_GetBPP(bm);
	unsigned char *buf = *bits;
	switch(*bpp)
	{
		case 24:
			for (int j=0;j<(*Width)*(*Height);j++)
			{
				xorSwap(buf[j*3],buf[j*3+2]);
			}
			break;
		case 32:
			
//			for (int j=0;j<(*Width)*(*Height);j++)
//			{
//				xorSwap(buf[j*4],buf[j*4+3]);
//				xorSwap(buf[j*4+1],buf[j*4+2]);
//			}
			
			break;
	}
}
*/
void CreaFoco(int x, int y, float Duracion, float sin_ini = 0.0, float Intensidad = 1.0, float incGr = SIN_HZ)
{
	TFoco foco;
	memset(&foco,0,sizeof(TFoco));
	foco.sinGR = sin_ini;
	foco.OffTimeOut = 1.0;
	foco.posXY = y*MapaWidth+x;
	foco.MaxSin = Duracion;
	foco.Intens = Intensidad;
	foco.IncGR = incGr;
	Focos.push_back(foco);
}

void CreaDobleRend(int y,int AnchoAgujeros,int Separacion)
{
	int i;
	for (int nc=0;nc<NumCPUs;nc++)
	{
		for (i=0;i<MapaWidth/2-(Separacion/2+AnchoAgujeros);i++)
			mapa[nc][y*MapaWidth + i].Mat = 0;
		for (i+=AnchoAgujeros;i<MapaWidth/2+(Separacion/2);i++)
			mapa[nc][y*MapaWidth + i].Mat = 0;
		for (i+=AnchoAgujeros;i<MapaWidth;i++)
			mapa[nc][y*MapaWidth + i].Mat = 0;
	}

	for (i=MapaWidth/2-128;i<MapaWidth/2+128;i++)
	{
		CreaFoco(i,MapaHeight-16,-1);
	}

	for (int nc=0;nc<NumCPUs;nc++)
	{
		for (int i=0;i<MapaWidth;i++)
			mapa[nc][(MapaHeight-15)*MapaWidth + i].Mat = 0;
	}
		
}

void CreaFocusRay(int y,int Ancho,float angle,float focus)
{
	
	float a = 0;
	//float dinc = (3.141592)/(float)nFocusRay;
	float dinc = 0;
	float Line[nFocusRay];
	for (int i=0;i<nFocusRay/2;i++)
	{
		Line[nFocusRay/2+i]=a;
		Line[nFocusRay/2-i]=a;
		a+=dinc;
		dinc+=focus;
	}
	float l = 0;
	for (int i=0;i<nFocusRay;i++)
	{
		CreaFoco(MapaWidth/2 -((nFocusRay/2)*FocusRaySep) + (i*FocusRaySep),y+(MapaHeight/8),-1,Line[i]+l);
		l-=angle;
	}

	for (int nc=0;nc<NumCPUs;nc++)
	{
		for (int i=0;i<MapaWidth;i++)
			mapa[nc][(y+(MapaHeight/8)+1)*MapaWidth + i].Mat = 0;
	}
}

void CreaUnaRend(int y,int AnchoAgujero)
{
	int i;
	for (int nc=0;nc<NumCPUs;nc++)
	{
		for (i=0;i<MapaWidth/2-(AnchoAgujero);i++)
			mapa[nc][y*MapaWidth + i].Mat = 0;
		for (i+=AnchoAgujero;i<MapaWidth/2;i++)
			mapa[nc][y*MapaWidth + i].Mat = 0;
		for (i+=AnchoAgujero;i<MapaWidth;i++)
			mapa[nc][y*MapaWidth + i].Mat = 0;
	}

	//CreaFoco(MapaWidth/2,y+MapaHeight/6,-1);
	for (i = MapaWidth / 2 - 64; i<MapaWidth / 2 + 64; i++)
	{
		CreaFoco(i, y + MapaHeight / 6, -1);
	}

	for (int nc=0;nc<NumCPUs;nc++)
	{
		for (int i=0;i<MapaWidth;i++)
			mapa[nc][(y+(MapaHeight/6)+3 )*MapaWidth + i].Mat = 0;
	}

}
void CreaDosFocos(int y, int Separacion, float Hz1 = SIN_HZ, float Hz2 = SIN_HZ)
{
	CreaFoco(MapaWidth / 2 - Separacion / 2, y, -1, 0, 1, Hz1);
	CreaFoco(MapaWidth / 2 + Separacion / 2, y, -1, pi, 1, Hz1);
	/*
	for (int nc=0;nc<NumCPUs;nc++)
	{
		for (int i=0;i<MapaWidth;i++)
			mapa[nc][(y+1)*MapaWidth + i].Mat = 0;
	}
	*/
}
void CreaTresFocos(int y,int Separacion)
{
	CreaFoco(MapaWidth/2-Separacion,y,-1);
	CreaFoco(MapaWidth/2,y-(1.76 * Separacion),-1,3.14159265);
	CreaFoco(MapaWidth/2+Separacion,y,-1);
	/*
	for (int nc=0;nc<NumCPUs;nc++)
	{
		for (int i=0;i<MapaWidth;i++)
			mapa[nc][(y+1)*MapaWidth + i].Mat = 0;
	}
	*/
}

void CreaTriangle(int y,int Separacion)
{
	float Sep = (float)Separacion;
	float c = sqrt(4.0*Sep*Sep - Sep*Sep);
	CreaFoco(MapaWidth/2-Separacion,y,-1);
	CreaFoco(MapaWidth/2,y-(int)c,-1);
	CreaFoco(MapaWidth/2+Separacion,y,-1);
}
void CreanFocosRand(int nFocos)
{
	for (int i=0;i<nFocos;i++)
	{
		int x = rand()%(MapaWidth/2);
		int y = rand()%(MapaHeight/2);
		x-=MapaWidth/4;
		y-=MapaHeight/4;
		CreaFoco(MapaWidth/2-x,MapaHeight/2-y,-1);
	}
}

void CreanLineFocosRand(int y,int nFocos)
{
	for (int i=0;i<nFocos;i++)
	{
		int x = rand()%(MapaWidth/2);
		x-=MapaWidth/4;
		CreaFoco(MapaWidth/2-x,y,-1);
	}
	/*
	for (int nc=0;nc<NumCPUs;nc++)
	{
		for (int i=0;i<MapaWidth;i++)
			mapa[nc][(y+1)*MapaWidth + i].Mat = 0;
	}
	*/
}

void CreaHoloFocos()
{

	float Up = 0;
	for (int i=0;i<MapaWidth;i++)
	{
		if (LineTest[i]>Up)
			Up = LineTest[i];
	}
	for (int i=0;i<MapaWidth;i++)
	{
		float pnt = LineTest[i]/Up;
		//CreaFoco(i,(MapaHeight/3),-1,0,(pnt>0.8)?1.0:0);
		CreaFoco(i,(MapaHeight/3),-1,0,pnt);
		for (int nc=0;nc<NumCPUs;nc++)
		{
			mapa[nc][((MapaHeight/3)-1)*MapaWidth + i].Mat = 0;
		}
	}
}

void Clear()
{
	float fMap1 = MapFalloff;
	float fMap2 = 0.940;
	for (int nc=0;nc<NumCPUs;nc++)
	{
		memset(mapa[nc],0,sizeof(TMAPPoint)*MapaWidth*MapaHeight);
	}

	//fMap1 = 1.000;
	for (int nc=0;nc<NumCPUs;nc++)
	{
		for (int i=0;i<MapaWidth * MapaHeight;i++)
		{
			mapa[nc][i].Mat = fMap1;
		}
	}
	
	
	for (int nc=0;nc<NumCPUs;nc++)
	{
		for (int y=0;y<MapaHeight;y++)
		{
			for (int x=0;x<MapaWidth;x++)
			{
				if (x<(MapaWidth/16))
				{
					mapa[nc][(y*MapaWidth+x)].Mat = fMap2 + ((fMap1-fMap2)/((float)MapaWidth/16.0))*(float)x;
				}
				if (x>(MapaWidth-(MapaWidth/16)))
				{
					float negx = (float)(MapaWidth-x);
					mapa[nc][(y*MapaWidth+x)].Mat = fMap2 + ((fMap1-fMap2)/((float)MapaWidth/16.0))*(float)negx;
				}
				if (y<(MapaHeight/16))
				{
					mapa[nc][(y*MapaWidth+x)].Mat = fMap2 + ((fMap1-fMap2)/((float)MapaHeight/16.0))*(float)y;
				}
				if (y>(MapaHeight-(MapaHeight/16)))
				{
					float negy = (float)(MapaHeight-y);
					mapa[nc][(y*MapaWidth+x)].Mat = fMap2 + ((fMap1-fMap2)/((float)MapaHeight/16.0))*(float)negy;
				}
			}
		}
	}
	
}
void BorraFocos()
{
	Clear();
	Focos.clear();
}


void InitExperiment(int nExperiment)
{
	static int dosFocos = 0;
	switch(nExperiment & (YOUNG|FOCUSRAY|UNAREND|DOSFOCOS|TRESFOCOS|RANDOMFOCOS|RANDOMFOCOS2|HOLOFOCOS|TRIANGLE))
	{
		case YOUNG:
			CreaDobleRend(DobleRend,20,100);
			break;
		case UNAREND:
			CreaUnaRend(DobleRend,10);
			break;
		case FOCUSRAY:
			CreaFocusRay(MapaHeight/2+MapaHeight/4,20,rayangle,rayfocus);
			break;
		case DOSFOCOS:
			if (dosFocos==0)
				CreaDosFocos(MapaHeight / 2 + MapaHeight / 4, 64);
			else
				CreaDosFocos(MapaHeight / 2 + MapaHeight / 4, 64, SIN_HZ, SIN_HZ/2);
			dosFocos++;
			if (dosFocos > 1)
				dosFocos = 0;
			break;
		case TRESFOCOS:
			CreaTresFocos(MapaHeight/2+MapaHeight/4,64);
			break;
		case RANDOMFOCOS:
			CreanLineFocosRand(MapaHeight-64,rand()%8);
			break;
		case RANDOMFOCOS2:
			CreanFocosRand(rand()%8);
			break;
		case HOLOFOCOS:
			CreaHoloFocos();
			break;
		case TRIANGLE:
			CreaTriangle(MapaHeight/2+MapaHeight/4,64);
			break;
	}
	if (nExperiment & DEFTEST)
	{
		TestOn = true;
		/*
		if (LineTest!=NULL)
			delete LineTest;
		LineTest = new float (MapaWidth);
		*/
		memset(LineTest,0,sizeof(float)*MapaWidth);
	}
}


void OscilaFocos()
{
	float s = sin(GeneralsinGR);
	GeneralsinGR+=SinHz;
	Generalsin = s;
	for (std::deque<TFoco>::iterator it = Focos.begin();it!=Focos.end();)
	{
		if ((it->MaxSin<0)||(it->sinGR<it->MaxSin))
		{
			s = sin(it->sinGR) * NPass ;
			//it->sinGR+=SinHz;
			it->sinGR += it->IncGR;
			it->sin = s * it->Intens;
			if (it->OffTimeOut<1.0)
			{
				if (it->OffTimeOut<=0.0)
				{
					it=Focos.erase(it);
					continue;
				}
				else
					it->OffTimeOut-=0.005;
			}
			it++;
		}
		else
		{/*
			int posXY = it->posXY;
			mapa[posXY].Pos = 0;
			mapa[posXY].Acc = 0;
			it=Focos.erase(it);
			continue;*/
			
			if (it->sinGR<(it->MaxSin+4*3.141593))
			{
				int n = it->Delete;
				float n2 = (1.0/((float)n+1.0));
				s = sin(it->sinGR);
				it->sinGR+=SinHz;
				it->sin = s * it->Intens * n2;
				it->Delete++;
				it++;
			}
			else
			{
				
				int posXY = it->posXY;
				for (int nc=0;nc<NumCPUs;nc++)
				{
					mapa[nc][posXY].Pos = 0;
					mapa[nc][posXY].Acc = 0;
				}
				it=Focos.erase(it);
				continue;
			}
		}
	}
}



void PrepareScene(int nThread)
{

	if (Experimentos & ROUNDFOCOS)
	{
		if ((rand()%100)<DensidadFocos)
		{
			CreaFoco(1+rand()%(MapaWidth-2),1+rand()%(MapaHeight-2),(float)2*3.141593);
		}
	}

	OscilaFocos();
	for (std::deque<TFoco>::iterator it = Focos.begin();it!=Focos.end();it++)
	{
		
		int posxy = it->posXY;
		float s = it->sin;
		mapa[nThread][posxy].Pos = s;
	}
}
void CreateScene(int nThread)
{
	Uint32 *pixels = (Uint32 *)imageBack->pixels; 
	Uint32 *buf = pixels;
	TColorRGB color;
	color.a = 0xFF;
	TMAPPoint *p1,*p2;
	p1 = &mapa[nThread][0];
	p2 = &mapa[nThread][MapaWidth];
	TMAPPoint *p[NPass*2];
	for (int i=0;i<NPass*2;i++)
	{
		p[i] = &mapa[nThread][i*MapaWidth];
	}
	int c ;float acc;float mat ;
	int Len = imageBack->w*(imageBack->h-NPass*4);
	//for (int i=0;i<Len;i++,buf++,p1++,p2++,p3++,p4++)
	for (int i=0;i<Len;i++,buf++)
	{
		acc = p[3][-1].Pos + p[3][1].Pos  + p[3][-MapaWidth].Pos  + p[3][MapaWidth].Pos;
		acc/=4.0;
		acc-= p[3]->Pos;
		p[3]->Acc+=acc;
		p[3]++;
		// Calculo las posiciones de la linea actual con las aceleraciones ya calculadas en la linea anterior
		p[2]->Pos+=p[2]->Acc;
		mat = p[2]->Mat;
		p[2]->Pos*=mat;
		p[2]->Acc*=mat;
		p[2]++;

		acc = p[1][-1].Pos + p[1][1].Pos  + p[1][-MapaWidth].Pos  + p[1][MapaWidth].Pos;
		acc/=4.0;
		acc-= p[1]->Pos;
		p[1]->Acc+=acc;
		p[1]++;
		// Calculo las posiciones de la linea actual con las aceleraciones ya calculadas en la linea anterior
		p[0]->Pos+=p[0]->Acc;
		mat = p[0]->Mat;
		p[0]->Pos*=mat;
		p[0]->Acc*=mat;
		p[0]++;
		/*
		for (int i=NPass-1;i>=0;i--)
		{
			acc = p[i*2+1][-1].Pos + p[i*2+1][1].Pos  + p[i*2+1][-MapaWidth].Pos  + p[i*2+1][MapaWidth].Pos;
			acc/=4.0;
			acc-= p[i*2+1]->Pos;
			p[i*2+1]->Acc+=acc;
			// Calculo las posiciones de la linea actual con las aceleraciones ya calculadas en la linea anterior
			p[i*2]->Pos+=p[i*2]->Acc;
			mat = p[i*2]->Mat;
			p[i*2]->Pos*=mat;
			p[i*2]->Acc*=mat;
			p[i*2]++;
			p[i*2+1]++;
		}
		*/
		mat = p[0]->Mat;
		if (mat==0)
		{
			*buf = 0xFFFFFFFF;
			continue;
		}
		c = (int)(p[0]->Pos*FACMUL);
/*
		acc = p4[-1].Pos + p4[1].Pos  + p4[-MapaWidth].Pos  + p4[MapaWidth].Pos;
		acc/=4.0;
		acc-= p4->Pos;
		p4->Acc+=acc;
		// Calculo las posiciones de la linea actual con las aceleraciones ya calculadas en la linea anterior
		p3->Pos+=p3->Acc;
		mat = p3->Mat;
		p3->Pos*=mat;
		p3->Acc*=mat;

		// Primero calculo las aceleraciones de la linea posterior
		// cuando p1 alcance estas lineas la aceleración ya estará calculada
		acc = p2[-1].Pos + p2[1].Pos  + p2[-MapaWidth].Pos  + p2[MapaWidth].Pos;
		acc/=4.0;
		acc-= p2->Pos;
		p2->Acc+=acc;
		// Calculo las posiciones de la linea actual con las aceleraciones ya calculadas en la linea anterior
		p1->Pos+=p1->Acc;
		mat = p1->Mat;
		p1->Pos*=mat;
		p1->Acc*=mat;

		if (mat==0)
		{
			*buf = 0xFFFFFFFF;
			continue;
		}
		c = (int)(p1->Pos*FACMUL);
*/
		if (c>255)
		{
			c=255;
		}
		if (c<-255)
		{
			c=-255;
		}
		if (c>=0)
		{
			color.r = 0;color.g = 0;color.b = c;
		}
		else
		{
			color.r = -c;color.g = -c;color.b = 0;
		}
		*((TColorRGB *)buf) = color;
	}
/*
	if (nThread+1<=NumCPUs)
		memcpy(mapa[nThread+1],mapa[nThread],sizeof(TMAPPoint)*imageBack->w*imageBack->h);
*/	

	if (Experimentos & DEFTEST)
	{
		if (TestOn)
		{
			if (Experimentos & RECHOLO)
			{
				for (int i=0;i<MapaWidth;i++)
				{
					LineTest[i]+=(fabs(mapa[nThread][(MapaHeight/3)*MapaWidth+i].Pos )+ Generalsin );
				}
			}
			else
			{
				for (int i=0;i<MapaWidth;i++)
				{
					LineTest[i]+=fabs(mapa[nThread][(MapaHeight/3)*MapaWidth+i].Pos);
				}
			}
		}
			
		buf = pixels;
		for (int i=0;i<MapaWidth;i++)
		{
			int y = (MapaHeight/3);
			int pos = y*MapaWidth + i ;
			buf[pos]=0xFF008080;
			y-=(int)LineTest[i];
			pos = y*MapaWidth + i ;
			if (y<1)
			{
				TestOn = false;
				continue;
			}
			
			buf[pos-1]=0xFF008080;
			buf[pos] = 0xFFFFFFFF;
			buf[pos+1] = 0xFF008080;
			buf[pos - PanWidth] = 0xFF008080;
			buf[pos + PanWidth] = 0xFF008080;
		}
	}
	buf = pixels;
	int y = (MapaHeight/2+MapaHeight/4 + MapaHeight/8);
	for (std::deque<TFoco>::iterator it = Focos.begin();it!=Focos.end();it++)
	{
		int posxy = it->posXY;
		float s = it->sin;// * it->Intens;
		int pos = posxy+(int)(s*16.0)*PanWidth;
		if ((pos>=0)&&(pos<MapaWidth*MapaHeight))
		{
			buf[pos] = 0xFF00FF00;
		}
	}
}
int DrawScene()
{
	SDL_Rect offset;
	offset.x = 0;
	offset.y = 0;
	char StrTemp[256];

	int tnew = GetTickCount();
	static unsigned int nFot = 0;
	static unsigned int FotPS = 0;
	static unsigned int told = 0;
	{
		if ((tnew-told)>1000)
		{
			told = tnew;
			FotPS = nFot;
			nFot = 0;
			printf("\rF/S : %d",FotPS);
		}
	}
	sprintf(StrTemp,"F/S : %d",FotPS);
	/*
	SDL_Color color;
	SDL_Surface* txt_img;
	color.b = 255;color.g = 255;color.r = 255; color.unused = 255;
	txt_img = TTF_RenderText_Blended(fuente, StrTemp, color);
	if(txt_img == NULL) 
	{
	  printf("Fallo al renderizar el texto");
	  exit(-1);
	}
	int w,h;
	if (TTF_SizeText(fuente,StrTemp,&w,&h))
	{
		printf(TTF_GetError());
	}
	

	SDL_Rect rect;
	rect.x = 10; rect.y = 10; rect.w = w; rect.h = h; 

	SDL_mutexP(mut);
	SDL_BlitSurface(txt_img, NULL, image, &rect);
	SDL_FreeSurface(txt_img);
	*/
	SDL_BlitSurface( image, NULL, screen , &offset );
	SDL_mutexV(mut);
	
        
	//Update the screen
	if( SDL_Flip( screen ) == -1 )
	{
		return -1;		
	}
	nFot++;
	return 1;
}




void presskey( Uint8 *keys)
{

	CommonPos = 1;
	while(CommonPos == 1)
	{
		Sleep(1);
	}


	if ( keys[SDLK_F6] ) 
	{  
		DobleRend+=10;
		BorraFocos();
		InitExperiment(Experimentos);
		
	}
	if ( keys[SDLK_F5] ) 
	{  
		DobleRend-=10;
		BorraFocos();
		InitExperiment(Experimentos);
		
	}

	if ( keys[SDLK_KP_MULTIPLY	] ) 
	{  
		MapFalloff+=0.001;
		if (MapFalloff>=1)
		{
			MapFalloff = 1;
		}
		Clear();
	}
	if ( keys[SDLK_KP_DIVIDE] ) 
	{
		MapFalloff-=0.001;
		if (MapFalloff<=0)
		{
			MapFalloff = 0;
		}
		Clear();
	}
	if ( keys[SDLK_RETURN] ) 
	{
		BorraFocos();
		InitExperiment(Experimentos);
	}
	if ( keys[SDLK_ESCAPE] ) 
	{
//		DrawLinePos.nPoint=0;	
	}
	if ( keys[SDLK_RIGHT] ) 
	{
		if (Experimentos & ROUNDFOCOS)
		{
			if (DensidadFocos<100)
				DensidadFocos++;
		}
		if (Experimentos & FOCUSRAY)
		{
			rayangle+=0.02;
			BorraFocos();
			InitExperiment(Experimentos);
		}
	}
	if ( keys[SDLK_LEFT] ) 
	{
		if (Experimentos & ROUNDFOCOS)
		{
			if (DensidadFocos>1)
				DensidadFocos--;
			
		}
		if (Experimentos & FOCUSRAY)
		{
			rayangle-=0.02;
			BorraFocos();
			InitExperiment(Experimentos);
		}
	}
	if ( keys[SDLK_UP] ) 
	{
		rayfocus+=0.001;
		BorraFocos();
		InitExperiment(Experimentos);
	}
	if ( keys[SDLK_DOWN] ) 
	{
		rayfocus-=0.001;
		BorraFocos();
		InitExperiment(Experimentos);
	}
	if ( keys[SDLK_KP_PLUS] ) 
	{
			FACMUL+=32.0;
	}
	if ( keys[SDLK_KP_MINUS] ) 
	{
			FACMUL-=32.0;
	}
	if ( keys[SDLK_PAGEDOWN] ) 
	{
			SinHz+=0.01;
	}
	if ( keys[SDLK_PAGEUP] ) 
	{
			SinHz-=0.01;
	}
	if ( keys[SDLK_1] ) 
	{		
			BorraFocos();
			Experimentos = YOUNG | DEFTEST;
			InitExperiment(Experimentos);
	}
	if ( keys[SDLK_2] ) 
	{		
			BorraFocos();
			Experimentos = FOCUSRAY | DEFTEST;
			InitExperiment(Experimentos);
	}
	if ( keys[SDLK_3] ) 
	{		
			BorraFocos();
			Experimentos = ROUNDFOCOS;
			InitExperiment(Experimentos);
	}
	if ( keys[SDLK_4] ) 
	{		
			BorraFocos();
			Experimentos = UNAREND | DEFTEST;
			InitExperiment(Experimentos);
	}
	if ( keys[SDLK_5] ) 
	{		
			BorraFocos();
			Experimentos = DOSFOCOS | DEFTEST;
			InitExperiment(Experimentos);
	}
	if ( keys[SDLK_6] ) 
	{
			BorraFocos();
			Experimentos = TRESFOCOS | DEFTEST;
			InitExperiment(Experimentos);
	}
	if ( keys[SDLK_7] ) 
	{
			BorraFocos();
			Experimentos = RANDOMFOCOS | DEFTEST;
			InitExperiment(Experimentos);
	}
	if ( keys[SDLK_8] ) 
	{
			BorraFocos();
			Experimentos = RANDOMFOCOS2 ;
			InitExperiment(Experimentos);
	}
	if ( keys[SDLK_9] ) 
	{
			BorraFocos();
			Experimentos = TRIANGLE ;
			InitExperiment(Experimentos);
	}
	if ( keys[SDLK_h] ) 
	{
		if (Experimentos & HOLOFOCOS)
		{
			BorraFocos();
		}
		if (Experimentos & RECHOLO)
		{
			BorraFocos();
			Experimentos = HOLOFOCOS;
			InitExperiment(Experimentos);
		}
		else
			Experimentos|=RECHOLO;
	}
	if ( keys[SDLK_s] ) 
	{		
		/*
			MapaWidth = 256;
			MapaHeight = 256;
			BorraFocos();
			Experimentos = YOUNG | DEFTEST | SAVETEST;
			InitExperiment(Experimentos);
		*/
		bSaveJpeg = true;
	}
	if ( keys[SDLK_l] ) 
	{
	
	}

	CommonPos = 0;
}


#ifdef JPEG2000_SAVE

void j2k_error_callback(const char *msg, void *client_data) 
{
}
void j2k_warning_callback(const char *msg, void *client_data) 
{
}

void j2k_info_callback(const char *msg, void *client_data) 
{
}
bool SaveJpeg2000()
{
	opj_image_cmptparm_t *cmptparm;	
	opj_image_t *oimage = NULL;
	opj_cparameters_t parameters;	/* compression parameters */
	opj_cio_t *cio = NULL;
	opj_codestream_info_t cstr_info;
	opj_event_mgr_t event_mgr;		/* event manager */
	bool bSuccess;


	memset(&event_mgr, 0, sizeof(opj_event_mgr_t));
	event_mgr.error_handler = j2k_error_callback;
	event_mgr.warning_handler = j2k_warning_callback;
	event_mgr.info_handler = j2k_info_callback;

	TColorRGB *pixels = (TColorRGB *)imageBack->pixels; 

	opj_set_default_encoder_parameters(&parameters);
	parameters.cp_cinema = OFF;
	parameters.prog_order = LRCP;
	parameters.tcp_mct = 0;

	/* convert wx image into opj image */
	cmptparm = (opj_image_cmptparm_t*) malloc(3 * sizeof(opj_image_cmptparm_t));

	/* initialize opj image components */	
	memset(&cmptparm[0], 0, 3 * sizeof(opj_image_cmptparm_t));
	for(int i = 0; i < 3; i++) {		
		cmptparm[i].prec = 8;
		cmptparm[i].bpp = 8;
		cmptparm[i].sgnd = false;
		cmptparm[i].dx = parameters.subsampling_dx;
		cmptparm[i].dy = parameters.subsampling_dx;
		cmptparm[i].w = MapaWidth;
		cmptparm[i].h = MapaHeight;
	}

	/* create the image */
	oimage = opj_image_create(3, &cmptparm[0], CLRSPC_SRGB);
	if(!oimage) {
		if (cmptparm)
			free(cmptparm);
		return false;
	}

	/* set image offset and reference grid */
	oimage->x0 = 0;
	oimage->y0 = 0;
	oimage->x1 = MapaWidth;
	oimage->y1 = MapaHeight;

	int area = MapaWidth * MapaHeight;
	for (int i = 0; i < area; i++) 
	{
			oimage->comps[0].data[i] = 0xFF;
			oimage->comps[1].data[i] = pixels[i].g;
			oimage->comps[2].data[i] = pixels[i].b;
	}
	
	/* get a J2K compressor handle */
	opj_cinfo_t* cinfo = opj_create_compress(CODEC_J2K);

	/* catch events using our callbacks and give a local context */
	opj_set_event_mgr((opj_common_ptr)cinfo, &event_mgr, stderr);

	/* setup the encoder parameters using the current image and user parameters */
	opj_setup_encoder(cinfo, &parameters, oimage);
	/* open a byte stream for writing */
	/* allocate memory for all tiles */
	cio = opj_cio_open((opj_common_ptr)cinfo, NULL, 0);

	/* encode the image */
	bSuccess = opj_encode_with_info(cinfo, cio, oimage, &cstr_info);
	if (!bSuccess) 
	{
		opj_cio_close(cio);
		opj_destroy_compress(cinfo);
		opj_image_destroy(oimage);
		return false;
	}
	/* write the buffer to stream */
	//stream.Write(cio->buffer, codestream_length);
	FILE *Fich = fopen("out.jp2","wb");
	int codestream_length = cio_tell(cio);
	fwrite(cio->buffer,1,codestream_length,Fich);
	fclose(Fich);

	/* close and free the byte stream */
	opj_cio_close(cio);
	opj_destroy_compress(cinfo);
	/* free image data */
	opj_image_destroy(oimage);
	return true;
}


#endif



int MainLoop(void)
{
	int done=0;
	Uint8 *keys;
	while(done == 0)
	{
		SDL_Event event;
		while ( SDL_PollEvent(&event) )
		{
			if ( event.type == SDL_QUIT )  {  done = 1;  }
			if ( event.type == SDL_KEYDOWN )
			{
				if ( event.key.keysym.sym == SDLK_ESCAPE ) { done = 1; }
				keys = SDL_GetKeyState(NULL);
				presskey( keys);
			}
		}
		/*
		if (pIdle)
			pIdle();
			*/
		

		
		
		SDL_LockMutex ( mutPinta);
		int nRet = SDL_CondWaitTimeout(cndPinta, mutPinta, 10);
		SDL_UnlockMutex ( mutPinta );
		if (nRet==0)
		{
			DrawScene();
		}
		
	}
	return 0;
}

void SetIdleCallBack(int (*pnt)())
{
	pIdle = pnt;
}

int Idle()
{
	return 0;
}

int Thread1(void *data)
{
	int nThread = (int)data;
	for(;!EndAllProcess;)
	{
		PrepareScene(nThread);
		CreateScene(nThread);
		if (CommonPos==1)
		{
			CommonPos = 2;
		}
		while(CommonPos==2)
		{
			Sleep(1);
		}
		if (bSaveJpeg)
		{
#ifdef JPEG2000_SAVE		
			SaveJpeg2000();
#endif			
			bSaveJpeg = false;
		}

		SDL_mutexP(mut);
		SDL_BlitSurface(imageBack, NULL, image, NULL);
		SDL_mutexV(mut);
		RedrawScreen();
	}
	return 1;
}



int main(int argc, char* argv[])
{
	DobleRend = MapaHeight/2+MapaHeight/4;
	
	rayangle = 0.00;
	rayfocus = 0.08;
	GeneralsinGR = 0;
	//LineTest = new float (MapaWidth );
	for (int i=0;i<NumCPUs;i++)
	{
		mapa[i] = new TMAPPoint[MapaWidth*MapaHeight];
	}
	

	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("error al iniciar SDL: %s\n", SDL_GetError());
		exit(1);
	}
	

	//screen = SDL_SetVideoMode(PanWidth, PanHeight, 32, SDL_FULLSCREEN);
	screen = SDL_SetVideoMode(PanWidth, PanHeight, 32, 0);
	if (screen == NULL)
	{
		printf("error al setear el modo de video: %s\n", SDL_GetError());
		exit(1);
	}

	if (TTF_Init() == -1) 
	{
		printf("Fallo al inicializar SDL_TTF");
		exit(1);
	}

	
	fuente = TTF_OpenFont("Arial-Rounded-MT-Bold.ttf", 14);
	if(fuente == NULL) 
	{
	  printf("Fallo al abrir la fuente");
	  exit(1);
	}

	TTF_SetFontStyle(fuente, TTF_STYLE_BOLD);
		 

	SDL_WM_SetCaption("Hola mundo!", "Hola Mundo!");

	image = SDL_CreateRGBSurface(SDL_HWSURFACE, PanWidth, PanHeight, 32,0,0,0,0);
	imageBack = SDL_CreateRGBSurface(SDL_HWSURFACE, PanWidth, PanHeight, 32,0,0,0,0);
	srand( (unsigned)GetTickCount() );
	Clear();
	Experimentos = ROUNDFOCOS;
	InitExperiment(Experimentos);

	mut=SDL_CreateMutex();

	mutWait = SDL_CreateMutex();
	cndWait = SDL_CreateCond();

	mutPinta = SDL_CreateMutex();
	cndPinta = SDL_CreateCond();



	//SetIdleCallBack(&Idle);
	SDL_Thread *Th1 = SDL_CreateThread(&Thread1, (void *)0);
	//SDL_Thread *Th2 = SDL_CreateThread(&Thread1, (void *)1);
	MainLoop();

	EndAllProcess = true;
	SDL_WaitThread(Th1, NULL);
	//SDL_WaitThread(Th2, NULL);

	//delete LineTest;

	SDL_FreeSurface(image);
	SDL_FreeSurface(imageBack);


	TTF_CloseFont(fuente);
	TTF_Quit();
   	SDL_Quit();
	return 0;

}


// sdlrtel3d.cpp : Defines the entry point for the console application.
//

#include <math.h>
#include <time.h>

	#include "fftw3.h"
#include "SDL.h"
#include "SDL_draw.h" 


#define DEF_TTF
#ifdef DEF_TTF
#include "SDL_TTF.h"
#endif


#define FPS_LIMIT	30

int PanWidth = 256;
int PanHeight = 256;


fftw_complex  *fftout = NULL;
fftw_complex  *Cin = NULL;
#define DegToRad(a)((a*pi)/180.0)
#define RadToDeg(a)((180.0/pi)*a)
#define pi 3.1415926535897932384626433832795
#define xorSwap(x,y) {(x)=(x)^(y); (y)=(x)^(y); (x)=(x)^(y);}

#define VelC	300000000.0	//Vel c en metros
#define Herzios		5000000.0		// 5 Mhz
//#define SampPerSec	1000000000.0	// 1 GigaSamp/s
#define SampPerSec	1000000000.0	// 100 MegaSamp/s

#define OneHzLen	VelC/Herzios
#define OnePixelLen	OneHzLen
#define AllPanXLen	OneHzLen*PanWidth
#define SamplesPerHz	(SampPerSec/Herzios)


FILE _iob[] = { *stdin, *stdout, *stderr };

extern "C" FILE * __cdecl __iob_func(void)
{
	return _iob;
}

SDL_Surface *screen;
SDL_Surface *imageBack;
int MouseX = 0;
int MouseY = 0;
double MouseAngle;
double Giro = 0;
char *SaveBuf = NULL;
double *Line1 = NULL;
double DelayRayAngle;

#ifdef DEF_TTF
TTF_Font* fuente;
#endif

typedef struct
{
        unsigned char b,g,r,a;
    void Set(unsigned char _r,unsigned char _g ,unsigned char _b, unsigned char _a)
    {
        r = _r;g = _g; b = _b; a = _a;
    }
    
}TColorRGB;


void presskey( Uint8 *keys)
{
	if ( keys[SDLK_1] ) 
	{
	}
}

void DrawText(char *Texto,int x,int y)
{
#ifdef DEF_TTF
	SDL_Color color;
	SDL_Surface* txt_img;
	color.b = 255;color.g = 255;color.r = 255; color.unused = 255;
	txt_img = TTF_RenderText_Blended(fuente, Texto, color);
	if(txt_img == NULL) 
	{
	  printf("Fallo al renderizar el texto");
	  exit(-1);
	}
	int w,h;
	if (TTF_SizeText(fuente,Texto,&w,&h))
	{
		printf(TTF_GetError());
	}
	SDL_Rect rect;
	rect.x = x; rect.y = y; rect.w = w; rect.h = h; 

	SDL_BlitSurface(txt_img, NULL, imageBack, &rect);
	SDL_UpdateRect(imageBack, 0, 0, 0, 0); 
	SDL_FreeSurface(txt_img);
#endif

}



void MoveMouse(unsigned short x,unsigned short y)
{
	MouseX = x;
	MouseY = y;

	MouseAngle = atan2(PanWidth/2.0-x,PanHeight/2.0-y);

}

void PressMouse(unsigned char button,unsigned char state,unsigned short x,unsigned short y)
{
	switch(state)
	{
		case SDL_PRESSED:
			switch(button)
			{
				case SDL_BUTTON_LEFT:
					break;
				case SDL_BUTTON_MIDDLE:
					break;
				case SDL_BUTTON_RIGHT:
					break;
					
			}
			break;
		case SDL_RELEASED:
			break;
	}
}
inline void InvertImage(unsigned char *Image)
{
	int Pos;
	for (int y=0;y<PanHeight/2;y++)
	{
		for (int x=0;x<PanWidth /2;x++)
		{
			Pos = y*PanWidth + x;
			xorSwap(Image[Pos],Image[(PanHeight/2)*PanWidth + PanWidth/2 + Pos]);
			xorSwap(Image[Pos + PanWidth/2],Image[(PanHeight/2)*PanWidth + Pos]);
		}
	}
}

char *SavePhoto(double Giro,char *&Inibuffer,double *&InitLine1, unsigned char *Image = NULL)
{
	static TColorRGB *buffer = NULL;
	static double *bufferLine1 = NULL;
	TColorRGB color;

    unsigned char *OrgImg = &Image[PanWidth*PanHeight];
    
	if (buffer==NULL)
	{
		buffer = new TColorRGB[PanWidth * PanHeight * (SamplesPerHz +1)];
		if (buffer==NULL)
			return NULL;
        Inibuffer= (char *)buffer;
	}
	for (int i=0;i<PanWidth*PanHeight;i++)
	{
        
		int ii1 = (int)(fftout[i][0]/(3.0));
		if (ii1>255) ii1 = 255;	if (ii1<-255)ii1 = -255;
        
		if (ii1>=0)
		{
            color.Set(ii1, ii1,0,0xff);
			*buffer = color;
		}
		else
		{
            color.Set(0,-ii1, -ii1,0xff);
			*buffer = color;
		}
        /*
        if (OrgImg[i]>0)
        {
            color.Set(0xff,0xff,0xff,0xff);
            *buffer = color;
        }
        else
         {
            color.Set(0,0,0,0xff);
            *buffer = color;
        }
        */

		buffer++;
	}
	return Inibuffer;
}

void DrawFFTfromSource(unsigned char *Image)
{
	if (fftout == NULL)
	{
		int len = PanWidth * PanHeight * 2;

		fftout = new fftw_complex[len];
		Cin = new fftw_complex[len];
	}
	static int nCount = 0;
	Uint32 *pixels = (Uint32 *)imageBack->pixels; 
	Uint32 *buf = pixels;
	
	if (Giro<2*pi)
	{
		for (int i=0;i<(PanWidth *PanHeight );i++)
		{
			Cin[i][0] = (double)Image[i]*sin(Giro);
			Cin[i][1] = (double)Image[i]*cos(Giro);
		}
		
		fftw_plan p = fftw_plan_dft_2d(PanWidth,PanHeight , Cin, fftout, FFTW_FORWARD, FFTW_ESTIMATE);
		
		fftw_execute(p); // repeat as needed 
		fftw_destroy_plan(p); 

		

	
		SavePhoto(Giro,SaveBuf,Line1,Image);
	}
    
	memcpy(buf,&SaveBuf[nCount * PanWidth * PanHeight * sizeof(TColorRGB)],PanWidth * PanHeight * sizeof(TColorRGB));
	Giro+=(Herzios/SampPerSec)*2*pi;
	nCount++;
	if (nCount >= SamplesPerHz)
	{
		nCount = 0;
	}
	
}
void DrawSuelo(unsigned char *Image)
{
	DrawFFTfromSource(Image);
}

int DrawScene(unsigned char *Image)
{ 
	if (imageBack != NULL)
	{
		Uint32 c_white = SDL_MapRGB(imageBack->format, 255, 255, 255);
		Uint32 c_gray = SDL_MapRGB(imageBack->format, 128, 128, 128);


		SDL_FillRect(imageBack, NULL, 0);


		DrawSuelo(Image);
		SDL_UpdateRect(imageBack, 0, 0, 0, 0);
	}
	return 1;
}

Uint32 DrawSceneCallback(Uint32 interval, void *Image)
{

	double L = PanHeight/2 - MouseY;
	double D = L * sin(MouseAngle);
	DelayRayAngle = D;


	//double dis = OnePixelLen*cos(MouseAngle);// distancia extra a recorrer
	//DelayRayAngle = dis/VelC;				// Tiempo se retraso en segundos

	DrawScene((unsigned char *)Image);

	unsigned int tnew = SDL_GetTicks();
	static unsigned int nFot = 0;
	static unsigned int nTotalFot = 0;
	static unsigned int FotPS = 0;
	static unsigned int told = 0;
	{
		if ((tnew-told)>1000)
		{
			told = tnew;
			FotPS = nFot;
			nFot = 0;
			printf("\rF/S : %d      ",FotPS);
			
		}
	}
	/*
	char StrTemp[256];
	sprintf(StrTemp,"F/S : %d          ",FotPS);
	DrawText(StrTemp,10,16);

	
	sprintf(StrTemp,"MHZ : %3.5f          ",Herzios/1000000.0);
	DrawText(StrTemp,10,32);

	double Seg = nTotalFot/SampPerSec;
	sprintf(StrTemp,"SEC : %3.10f          ",Seg);
	DrawText(StrTemp,10,48);

	sprintf(StrTemp,"PanLen: %3.2f meters",AllPanXLen);
	DrawText(StrTemp,10,64);


	sprintf(StrTemp,"Mouse Angle:%3.5fº : %3.5f Rad",RadToDeg(MouseAngle), MouseAngle);
	DrawText(StrTemp,10,80);
	int x1,y1,x2,y2;

	double a = OneHzLen;
	L = sqrt(a*tan(pi/2 - MouseAngle) + a*a);
	sprintf(StrTemp,"Len One Herz:%3.5f m",L);
	DrawText(StrTemp,10,96);

	x1 = PanWidth/2;
	y1 = PanHeight/2;
	x2 = PanWidth/2 + (int)(1000 * cos(-MouseAngle));
	y2 = PanHeight/2 + (int)(1000 * sin(-MouseAngle));
	SDraw_Line(imageBack, x1,y1,x2,y2, 0xFF00AAAAAA);

	x1 = MouseX;
	y1 = MouseY;
	x2 = MouseX + (int)(1000 * cos(-MouseAngle));
	y2 = MouseY + (int)(1000 * sin(-MouseAngle));
	SDraw_Line(imageBack, x1,y1,x2,y2, 0xFF00AAAAAA);

	x1 = MouseX;
	y1 = MouseY;
	x2 = MouseX - (int)(1000 * cos(-MouseAngle));
	y2 = MouseY - (int)(1000 * sin(-MouseAngle));
	SDraw_Line(imageBack, x1,y1,x2,y2, 0xFF00AAAA00,0,0,PanWidth,PanHeight/2);



	x1 = PanWidth/2;
	y1 = PanHeight/2;
	x2 = PanWidth/2 - (int)(1000 * sin(MouseAngle));
	y2 = PanHeight/2 - (int)(1000 * cos(MouseAngle));
	SDraw_Line(imageBack, x1,y1,x2,y2, 0xFF00AAAAAA);
	x2 = PanWidth/2 + (int)(1000 * sin(MouseAngle));
	y2 = PanHeight/2 + (int)(1000 * cos(MouseAngle));
	SDraw_Line(imageBack, x1,y1,x2,y2, 0xFF00AAAAAA);



	
*/
	nFot++;
	nTotalFot++;

	SDL_BlitSurface(imageBack, NULL, screen, NULL);
	SDL_UpdateRect(screen, 0, 0, 0, 0); 
	return 1;
}

SDL_TimerID InitLoop()
{
	SDL_Surface *SDLImage = SDL_LoadBMP("Imagen2_768.bmp");
	PanWidth = SDLImage->w;
	PanHeight = SDLImage->h;
	SDL_LockSurface(SDLImage);
	unsigned char *Image = new unsigned char[PanWidth *PanHeight * 2];

	memcpy(Image, SDLImage->pixels, PanWidth * PanHeight);
	memcpy(&Image[PanWidth * PanHeight], SDLImage->pixels, PanWidth * PanHeight);
	InvertImage(Image);
	SDL_UnlockSurface(SDLImage);
	SDL_FreeSurface(SDLImage);


	SDL_TimerID my_timer_id = SDL_AddTimer((33 / 10) * 10, DrawSceneCallback, Image);
	return my_timer_id;
}
int MainLoop(void)
{
	int done=0;
	Uint8 *keys;

	


	SDL_Event event;
	while(done == 0)
	{
		
		while ( SDL_PollEvent(&event) )
		{
			switch(event.type)
			{ 
				case SDL_MOUSEBUTTONDOWN:
				case SDL_MOUSEBUTTONUP:
					PressMouse(event.button.button,event.button.state,event.button.x,event.button.y);
					break;
				case SDL_MOUSEMOTION:
					MoveMouse(event.button.x,event.button.y);
					break;
				case SDL_QUIT:
					done = 1;
					break;
				case SDL_KEYDOWN:
					if ( event.key.keysym.sym == SDLK_ESCAPE ) 
						done = 1; 
					else
					{
						keys = SDL_GetKeyState(NULL);
						presskey( keys);
					}
					break;
			}
		}
	}
	
	return 0;
}



int main(int argc, char *argv[])
{
	double L;
	double a = OneHzLen;
	for (double alpha = 0;alpha<90;alpha+=1)
	{
		L = sqrt(a*tan(DegToRad(alpha)) + a*a);
		printf("alpha=%f ; L = %f m\n",alpha,L);
	}
	for (double alpha = 1;alpha<90;alpha+=1)
	{
		double ralpha = DegToRad(alpha);
		double a = OneHzLen / tan(ralpha);
		printf("Angle = %f ; Len = %f\n",alpha,a);
	}
	srand( (unsigned)time( NULL ) );
	

#ifdef DEF_TTF
	if (TTF_Init() == -1) 
	{
		printf("Fallo al inicializar SDL_TTF");
		exit(1);
	}

	
	fuente = TTF_OpenFont("arial-black.ttf", 14);
	if(fuente == NULL) 
	{
	  printf("Fallo al abrir la fuente");
	  exit(1);
	}

	TTF_SetFontStyle(fuente, TTF_STYLE_BOLD);
#endif		 
	
	if (SDL_Init(SDL_INIT_VIDEO | SDL_SWSURFACE | SDL_INIT_TIMER) < 0)
	{
		printf("error al iniciar SDL: %s\n", SDL_GetError());
		return 1;
	}
	SDL_TimerID timer_id = InitLoop();

	screen = SDL_SetVideoMode(PanWidth, PanHeight, 32, SDL_HWSURFACE);
	imageBack = SDL_CreateRGBSurface(SDL_HWSURFACE, PanWidth, PanHeight, 32, 0, 0, 0, 0);

	if (screen == NULL)
	{
		printf("error al setear el modo de video: %s\n", SDL_GetError());
		return 1;
	}
	SDL_WM_SetCaption("SDLRTel!", "SDLRTel!");

	MainLoop();


#ifdef DEF_TTF
	TTF_CloseFont(fuente);
	TTF_Quit();
#endif

	SDL_RemoveTimer(timer_id);

	if (SaveBuf)
		delete SaveBuf;

	SDL_Quit();
	return 0;
}



// sdlrtel3d.cpp : Defines the entry point for the console application.
//
#include <windows.h>
#include <math.h>
#include <time.h>
#include "SDL_main.h"
#include "SDL.h"
#include "SDL_thread.h"

#include "fftw3.h"
#include "SDL_ttf.h"

struct KEYs
{
	bool PressUp;
	bool PressDown;
	bool PressLEFT;
	bool PressRIGHT;
	bool PressPageUp;
	bool PressPageDw;

	bool center;
	int Sel;
	int done;
	KEYs()
	{
		PressUp = false;
		PressDown = false;
		PressLEFT = false;
		PressRIGHT = false;
		PressPageUp = false;
		PressPageDw = false;
		center = true; 
		Sel = 0;
		done = 0; 
	}
	
}Rkeys;

//#define DEF_TTF
#ifdef DEF_TTF
#include "SDL_TTF.h"
#endif


#define FPS_LIMIT	30

int PanWidth = 256;
int PanHeight = 256;

unsigned char *imageOrg = NULL;
SDL_Surface *imageBack;
SDL_Surface *screen;
SDL_Window* gWindow = NULL;
TTF_Font *font;


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


		SDL_Rect offset;
		offset.x = 0;
		offset.y = 0;

		SDL_BlitSurface(imageBack, NULL, screen, &offset);


		SDL_UpdateWindowSurface(gWindow);
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
	nFot++;
	nTotalFot++;

	SDL_UpdateWindowSurface(gWindow);

	/*
	SDL_BlitSurface(imageBack, NULL, screen, NULL);
	SDL_UpdateRect(screen, 0, 0, 0, 0); 
	*/
	return 1;
}
void InitLoop()
{
	SDL_Surface *SDLImage = SDL_LoadBMP("Imagen2_256.bmp");
	PanWidth = SDLImage->w;
	PanHeight = SDLImage->h;
	SDL_LockSurface(SDLImage);
	imageOrg = new unsigned char[PanWidth *PanHeight * 2];

	memcpy(imageOrg, SDLImage->pixels, PanWidth * PanHeight);
	memcpy(&imageOrg[PanWidth * PanHeight], SDLImage->pixels, PanWidth * PanHeight);
	InvertImage(imageOrg);
	SDL_UnlockSurface(SDLImage);
	SDL_FreeSurface(SDLImage);
}

bool InitSdl()
{
	//Initialization flag
	bool success = true;

	//Initialize SDL
	if (SDL_Init(SDL_INIT_VIDEO) < 0)
	{
		printf("SDL could not initialize! SDL_Error: %s\n", SDL_GetError());
		success = false;
	}
	else
	{
		InitLoop();
		//Create window
		gWindow = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, PanWidth, PanHeight, SDL_WINDOW_SHOWN);
		if (gWindow == NULL)
		{
			printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
			success = false;
		}
		else
		{
			//Get window surface
			screen = SDL_GetWindowSurface(gWindow);

			if (TTF_Init() == -1)
			{
				printf("TTF_Init: %s\n", TTF_GetError());
				exit(2);
			}

			font = TTF_OpenFont("Arial-th.ttf", 16);
			if (!font)
			{
				printf("TTF_OpenFont: %s\n", TTF_GetError());
				exit(2);
			}
		}
	}

	return success;
}
/*
int DrawScene(fftw_complex  *display, int tickCount)
{

	static unsigned int nFot = 0;
	static unsigned int FotPS = 0;
	static unsigned int told = 0;
	static int nFotograma = 0;
	static unsigned char *pixels24 = NULL;
	SDL_Rect offset;
	offset.x = 0;
	offset.y = 0;


	int tnew = tickCount;

	if ((tnew - told)>1000)
	{
		told = tnew;
		FotPS = nFot;
		nFot = 0;
		printf("\rF/S : %d ", FotPS);
	}
	if (Rkeys.Sel == 1)
	{
		DrawScene((unsigned char *)Image);
		CreateFFtScene(display);
		Normalize(display);

	}
	Normalize(display);

	CreateDisplayScene(display);



	Uint32 *pixels = (Uint32 *)imageBack->pixels;
	Uint32 *buf = pixels;



	SDL_Color color = { 255,255,255 };
	SDL_Surface *text_surface;
	std::ostringstream Texto;
	Texto << "Dist = " << zPos;
	text_surface = TTF_RenderText_Solid(font, Texto.str().c_str(), color);


	if (Sel > 0)
	{
		InvertImage((TColorRGB *)pixels);
	}

	SDL_BlitSurface(imageBack, NULL, screen, &offset);
	SDL_BlitSurface(text_surface, NULL, screen, &offset);

	if (pixels24 == NULL)
		pixels24 = new unsigned char[PanWidth * PanHeight * 3];

	TColorRGB *pixels32 = (TColorRGB *)pixels;
	for (int i = 0; i < PanWidth * PanHeight; i++)
	{
		pixels24[i * 3 + 2] = pixels32[i].r;
		pixels24[i * 3 + 1] = pixels32[i].g;
		pixels24[i * 3] = pixels32[i].b;
	}

	writer.WriteFrame(pixels24, nFotograma++);


	//Update the screen
	SDL_UpdateWindowSurface(gWindow);

	nFot++;
	return 0;
}

*/
int MainLoop()
{
	static int firstT = 0;

	if (firstT == 0)
		firstT = GetTickCount();


	const Uint8 *keys;

	while (Rkeys.done == 0)
	{
		SDL_Event event;
		while (SDL_PollEvent(&event))
		{
			if (event.type == SDL_QUIT) { Rkeys.done = 1; }
			if (event.type == SDL_KEYDOWN)
			{
				switch (event.key.keysym.sym)
				{
					//case SDLK_LEFT:  x--; break;
					//case SDLK_RIGHT: x++; break;
				case SDLK_UP:		Rkeys.PressUp = true; break;
				case SDLK_DOWN:		Rkeys.PressDown = true; break;
				case SDLK_LEFT:		Rkeys.PressLEFT = true; break;
				case SDLK_RIGHT:	Rkeys.PressRIGHT = true; break;
				case SDLK_PAGEUP:	Rkeys.PressPageUp = true; break;
				case SDLK_PAGEDOWN:	Rkeys.PressPageDw = true; break;
				case SDLK_c:		Rkeys.center = true; break;
				case SDLK_1:		Rkeys.Sel = 0; break;
				case SDLK_2:		Rkeys.Sel = 1; break;
				case SDLK_ESCAPE:	Rkeys.done = 1; break;

				}
			}
			if (event.type == SDL_KEYUP)
			{
				switch (event.key.keysym.sym)
				{
				case SDLK_UP:		Rkeys.PressUp = false; break;
				case SDLK_DOWN:		Rkeys.PressDown = false; break;
				case SDLK_LEFT:		Rkeys.PressLEFT = false; break;
				case SDLK_RIGHT:	Rkeys.PressRIGHT = false; break;
				case SDLK_PAGEUP:	Rkeys.PressPageUp = false; break;
				case SDLK_PAGEDOWN:	Rkeys.PressPageDw = false; break;
				}
			}
		}

		

		int tnew = GetTickCount() - firstT;
		DrawScene(imageOrg);

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
	


	InitSdl();
	imageBack = SDL_CreateRGBSurface(0, PanWidth, PanHeight, 32, 0, 0, 0, 0);
	MainLoop();

	SDL_Quit();
	return 0;
}



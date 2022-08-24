// SdlRtel.cpp: define el punto de entrada de la aplicación de consola.
//

#include <math.h>
#include "SDL.h"
#include "SDL_draw.h" 
#include "fftw3.h"


#define DEF_TTF
#ifdef DEF_TTF
#include "SDL_TTF.h"
#endif



#define PanWidth	1024
#define PanHeight	512
#define pi 3.1415926535897932384626433832795

#define DegToRad(a)((a*pi)/180.0)
#define RadToDeg(a)((180.0/pi)*a)


SDL_Surface *screen;

#ifdef DEF_TTF
TTF_Font* fuente;
#endif

int nRays;
double RayAngles[1024];
double Suelo[1024];
float angle = 0;
double MouseAngle;
int MouseX = 0;
int MouseY = 0;

#define VelC	300000000.0	//Vel c en metros
#define Hercios	50000000.0 // 10 Mhz

#define SampPerSec	100.0

#define MaxRecordSamples	0x10000
double Antenas[2][MaxRecordSamples];
int nRecord = 0;

void drawPoint(Uint32 *buf,int x,int y, Uint32 color)
{
    if ((x<0)||(x>PanWidth)||(y<0)||(y>PanHeight))
        return;
    buf[y*PanWidth + x] = 0xffffffff;
}

void BresenhamLine(Uint32 *buf,int x1,int y1,int x2,int y2, Uint32 color)
{
    int dx = x2 - x1;
    int dy = y2 - y1;

    //initialize varibales
    int d;
    int dL;
    int dU;

    if (dy > 0)
    {
        if (dy > dx)
        {
                d = dy - 2*dx;
                dL = -2*dx;
                dU = 2*dy - 2*dx;

                for (int x = x1, y = y1; y <= y2; y++)
                {
                        drawPoint(buf, x,y,color);
                        if (d >= 1)
                        {
                                d += dL;
                        }
                        else
                        {
                                x++;
                                d += dU;
                        }
                }              
        } 
        else 
        {
                //+shallow
                d = 2*dy - dx;
                dL = 2*dy;
                dU = 2*dy - 2*dx;

                for (int x = x1, y = y1; x <= x2; x++) 
                {
                        drawPoint(buf,x,y,color);

                        // if choosing L, next y will stay the same, we only need
                        // to update d by dL
                        if (d <= 0) 
                        {
                                d += dL;
                        // otherwise choose U, y moves up 1
                        } 
                        else 
                        {
                                y++;
                                d += dU;
                        }
                }
        }
    } 
    else 
    {
        if (-dy > dx)
        {
                //-steep
                d = dy - 2*dx;
                //south
                dL = 2*dx;
                //southeast
                dU = 2*dy - 2*dx;

                for (int x = x1, y = y1; y >= y2; --y)
                {
                        drawPoint(buf,x,y,color);

                        //if choosing L, next x will stay the same, we only need
                        //to update d
                        if (d >= 1)
                        {
                                d -= dL;
                        } 
                        else 
                        {
                                x++;
                                d -= dU;
                        }
                }
        } 
        else 
        {
                //-shallow
                d = 2*dy - dx;
                dL = 2*dy;
                dU = 2*dy - 2*dx;

                for (int x = x1, y = y1; x <= x2; x++)
                {
                        drawPoint(buf,x,y,color);

                        if (d >= 0)
                        {
                                d += dL;
                        } 
                        else 
                        {
                                --y;
                                d -= dU;
                        }
                }
        }
    }
}


void presskey( Uint8 *keys)
{
}

void MoveMouse(unsigned short x,unsigned short y)
{
	Uint32 c_white = SDL_MapRGB(screen->format, 255,255,255); 
	MouseX = x;
	MouseY = y;
	MouseAngle = atan2(PanWidth/2.0-x,PanHeight/2.0-0);
	printf("\r%3.5f",RadToDeg(MouseAngle));
}

void PressMouse(unsigned char button,unsigned char state,unsigned short x,unsigned short y)
{
	switch(state)
	{
		case SDL_PRESSED:
			switch(button)
			{
				case SDL_BUTTON_LEFT:
					RayAngles[nRays] = atan2(PanWidth/2.0-x,PanHeight/2.0-0);
					printf("\n%3.5f",RadToDeg(RayAngles[nRays]));
					nRays++;
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


void DrawSuelo()
{
	memset(Suelo,0,sizeof(float)*1024);
	Uint32 c_white = SDL_MapRGB(screen->format, 255,0,0); 
	Uint32 c_green = SDL_MapRGB(screen->format, 255, 0, 0);

	double  d = 0;
	double t = 0;
	double a = 0;
	for (int i=0;i<nRays;i++)
	{
		Draw_Line(screen, PanWidth/2-(PanHeight/2)*tan(RayAngles[i]),0,PanWidth/2,PanHeight/2, c_white); 
	}
	int LastX = 0;
	int LastY = 0;
	Draw_Line(screen, 0,(PanHeight/2 + PanHeight/8),PanWidth-1,(PanHeight/2 + PanHeight/8), c_white); 
	for (int j= 0;j<PanWidth;j++)
	{
		
		float PosAlpha =0;
		for (int i=0;i<nRays;i++)
		{
			d = j*sin(RayAngles[i]);// distancia extra a recorrer
			t = d/VelC;				// Tiempo se retraso
			a = Hercios*t*2*pi;		//Radianes de más recorrido
			PosAlpha+= sin(angle+a);
		}
		Suelo[j] = PosAlpha;

		
		int x = j;
		int y = (PanHeight/2 + PanHeight/8)+32*PosAlpha;

		if (j==(PanWidth/2-PanWidth/4))
		{
			Draw_Circle(screen,j,(PanHeight/2 + PanHeight/8),10,c_white);
			Antenas[0][nRecord] = PosAlpha;
		}
		if (j==(PanWidth/2+PanWidth/4))
		{
			Draw_Circle(screen,j,(PanHeight/2 + PanHeight/8),10,c_white);
			Antenas[1][nRecord] = PosAlpha;
		}
		Draw_Line(screen, LastX,LastY,x,y, c_white); 

		LastX = x;
		LastY = y;
	}

	double Baseline = (PanWidth/2+PanWidth/4) - (PanWidth/2-PanWidth/4);

	d = Baseline*sin(MouseAngle);// distancia extra a recorrer
	t = d/VelC;				// Tiempo se retraso en segundos
	//a = Hercios*t*2*pi;		//Radianes de más recorrido


	nRecord++;
	if (nRecord >= MaxRecordSamples)
		nRecord = 0;



	static fftw_complex  fftout[PanWidth];
	static fftw_complex  Cin[PanWidth];

	for (int i = 0; i<(PanWidth); i++)
	{
		Cin[i][0] = (double)Suelo[i];
		Cin[i][1] = (double)0;
	}
	fftw_plan p = fftw_plan_dft_1d(PanWidth, Cin, fftout, FFTW_BACKWARD, FFTW_ESTIMATE);
	fftw_execute(p);
	fftw_destroy_plan(p);

	LastX = 0;
	LastY = 0;

	for (int x = 0; x < PanWidth; x++)
	{
		float f = sqrt(fftout[0][x] * fftout[0][x] + fftout[1][x] * fftout[1][x])/10.0;

		if (f > 100)
			f = 100;

		int y = (PanHeight / 2 + PanHeight / 4 ) + f;
		Draw_Line(screen, LastX, LastY, x, y, c_white);
		LastX = x;
		LastY = y;
	}
    int Cuad = PanHeight/20;
    int px = fftout[MouseX/2][0] * 1;
    int py = fftout[MouseX/2][1] * 1;

    int mX = MouseX;
    int mY = PanHeight - Cuad;
    //Draw_Line(screen, mX, mY, mX + px, mY + py, c_white);
    BresenhamLine((Uint32*)screen->pixels,mX, mY, mX + px, mY + py, c_white);
		


}

int DrawScene()
{
	Uint32 c_white = SDL_MapRGB(screen->format, 255,255,255); 
	SDL_FillRect( SDL_GetVideoSurface(), NULL, 0 );
	Draw_Line(screen, MouseX,0,PanWidth/2,PanHeight/2, c_white); 
	Draw_Line(screen, 0,PanHeight/2,PanWidth-1,PanHeight/2, c_white); 
	Draw_Line(screen, PanWidth/2,0,PanWidth/2,PanHeight/2, c_white); 
	DrawSuelo();
	SDL_UpdateRect(screen, 0, 0, 0, 0); 
	return 1;
}

void DrawText(char *Texto,int x,int y)
{
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

	SDL_BlitSurface(txt_img, NULL, screen, &rect);
	SDL_UpdateRect(screen, 0, 0, 0, 0); 

}

int MainLoop(void)
{
	int done=0;
	Uint8 *keys;
	while(done == 0)
	{
		SDL_Event event;
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
		
		DrawScene();
		angle+=(2.0*pi/SampPerSec);




		unsigned int tnew = SDL_GetTicks();
		static unsigned int nFot = 0;
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
		char StrTemp[256];
		sprintf(StrTemp,"F/S : %d          ",FotPS);
		DrawText(StrTemp,10,10);




		nFot++;



	}
	return 0;
}



int main(int argc, char *argv[])
{
	if (SDL_Init(SDL_INIT_VIDEO|SDL_SWSURFACE ) < 0)
	{
		printf("error al iniciar SDL: %s\n", SDL_GetError());
		return 1;
	}
	nRays = 0;
	memset(RayAngles,0,sizeof(float)*1024);
	screen = SDL_SetVideoMode(PanWidth, PanHeight, 32, SDL_HWSURFACE);
	if (screen == NULL)
	{
		printf("error al setear el modo de video: %s\n", SDL_GetError());
		return 1;
	}
	SDL_WM_SetCaption("SDLRTel!", "SDLRTel!");


#ifdef DEF_TTF
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
#endif		 

	MainLoop();


#ifdef DEF_TTF
	TTF_CloseFont(fuente);
	TTF_Quit();
#endif

	SDL_Quit();
	return 0;
}


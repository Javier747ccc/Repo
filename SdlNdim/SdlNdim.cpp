// SdlNdim.cpp : Defines the entry point for the console application.
//

#include <windows.h>
#include <stdlib.h>
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <vector>
#include <map>
#include <math.h>
#include <time.h> 
#include <utility>
#include <deque>
#include <ctime>
#include "SDL_main.h"
#include "SDL.h"
#include "SDL_thread.h"
#include "assimp/cimport.h"
#include "assimp/scene.h"
#include "assimp/postprocess.h"
#include "fftw3.h"
#include "SDL_ttf.h"
#include "RawAVIStream.h"
#include "DrawPdf.h"



#define PanWidth	256	
#define PanHeight	256

#define PI	3.14159265
#define AccelPoints 10.0
#define bigBanPosZ	100000
#define nFocos	1



class Vector3D
{
private:
	double _x;
	double _y;
	double _z;
public:
	Vector3D()
	{
		_x = _y = _z = 0;
	}
	Vector3D(double x, double y, double z)
	{
		_x = x;
		_y = y;
		_z = z;
	}
	double GetX() { return _x; }
	double GetY() { return _y; }
	double GetZ() { return _z; }
	void SetX(double x) { _x = x; }
	void SetY(double y) { _y = y; }
	void SetZ(double z) { _z = z; }
};

class CFoco
{
private:
	Vector3D pos3D;
	unsigned int nID;
	std::vector<double> distances;

public:
	CFoco(int n_id) 
	{
		nID = n_id;
	};
	double getDistance(int nFoco)
	{
		return (distances[nFoco]);
	}
	double *getDistanceList()
	{
		return &distances.front();
	}
	void  AddDistance( double distance)
	{
		distances.push_back(distance);
	}
	void setPos3D(double x,double y, double z)
	{
		pos3D.SetX(x);
		pos3D.SetY(y);
		pos3D.SetZ(z);
	}
	Vector3D getPos3D()
	{
		return (pos3D);
	}
};





#define xorSwap(x,y) {(x)=(x)^(y); (y)=(x)^(y); (x)=(x)^(y);}
#define Zoom 0.4
SDL_Surface *imageBack;
SDL_Surface *screen;
SDL_Window* gWindow = NULL;
std::vector<Vector3D> vPos;
bool PressUp = false;
bool PressDown = false;
bool PressLEFT = false;
bool PressRIGHT = false;
bool PressPageUp = false;
bool PressPageDw = false;
bool center = false;
char *SaveBuf = NULL;
int Sel = 1;
const struct aiScene* scene;
const struct aiMesh* pmesh; 
TTF_Font *font;
RawAVIWriter writer(_T("out.avi"),PanWidth,PanHeight);
unsigned int Frame = 0;
double zPos; 

int nPosition = 0;

std::vector<CFoco> vFocos;



//           B
//           /\
//        c /  \ a
//         /    \
//        A------C
//            b
//                    b^2 +C^2 -a^2
//      A = arc_cos ( -------------)
//                        2bc

double CalcAngle(double a,double b,double c)
{
    return acos((b*b + c*c - a*a)/(2*b*c));
}

double *dA; double *dB; double *dC ;double *dD ; double *dAB;double *dBC;


typedef struct
{
        unsigned char b,g,r,a;
    void Set(unsigned char _r,unsigned char _g ,unsigned char _b, unsigned char _a)
    {
        r = _r;g = _g; b = _b; a = _a;
    }
    
}TColorRGB;


void Swap(TColorRGB *V1, TColorRGB *V2)
{
	TColorRGB Temp = *V1;
	*V1 = *V2;
	*V2 = Temp;
}
inline void InvertImage(TColorRGB *Image)
{
	int Pos;
	for (int y = 0; y<PanHeight / 2; y++)
	{
		for (int x = 0; x<PanWidth / 2; x++)
		{
			Pos = y * PanWidth + x;
			Swap(&Image[Pos], &Image[(PanHeight / 2)*PanWidth + PanWidth / 2 + Pos]);
			Swap(&Image[Pos + PanWidth / 2], &Image[(PanHeight / 2)*PanWidth + Pos]);
		}
	}
}



void CreateFFtScene(fftw_complex  *display)
{
	Uint32 *pixels = (Uint32 *)imageBack->pixels; 
    
	
    int a  =0;
    fftw_plan p = fftw_plan_dft_2d(PanWidth,PanHeight , display, display, FFTW_FORWARD, FFTW_ESTIMATE);
		
    
    fftw_execute(p); 
	fftw_destroy_plan(p); 
    return;
}




void Normalize(fftw_complex *display)
{
	double Max = 0;
	double Min = 0;

	for (int n = 0; n < 2; n++)
	{
		for (int i = 0; i < PanWidth*PanHeight; i++)
		{
			if (display[i][n] > Max)
				Max = display[i][n];
			if (display[i][n] < Min)
				Min = display[i][n];
			if (-Min > Max)
				Max = -Min;
		}
	}
	
	for (int i = 0; i < PanWidth*PanHeight; i++)
	{
		display[i][0] /= (Max/2);
		display[i][1] /= (Max/2);
	}
}

void CreateDisplayScene(fftw_complex  *display)
{
	Uint32 *pixels = (Uint32 *)imageBack->pixels; 
	TColorRGB *b = (TColorRGB *)pixels;
    for (int i = 0;i <PanWidth*PanHeight;i++,b++)    
    {
		int c = display[i][0] * 128;
		if (c >= 0)
		{
			b->Set(0, 0, c, 0xff);
		}
		else
		{
			b->Set(0, -c, 0, 0xff);
		}
    }
}

int DrawScene(fftw_complex  *display, int tickCount )
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
    
    if ((tnew-told)>1000)
	{
		told = tnew;
		FotPS = nFot;
		nFot = 0;
		printf("\rF/S : %d ",FotPS);
	}
    //Normalize(display);
	if (Sel ==1)
	{
		CreateFFtScene(display);
		Normalize(display);

	}
	Normalize(display);

	CreateDisplayScene(display);



    Uint32 *pixels = (Uint32 *)imageBack->pixels; 
	Uint32 *buf = pixels;



    SDL_Color color={255,255,255};
    SDL_Surface *text_surface;
    std::ostringstream Texto;
    Texto << "Dist = " << zPos;
    text_surface = TTF_RenderText_Solid(font,Texto.str().c_str(),color); 
    
    
	if (Sel > 0)
	{
		InvertImage((TColorRGB *)pixels);
	}
    
	SDL_BlitSurface(imageBack, NULL, screen , &offset );
    SDL_BlitSurface(text_surface, NULL, screen , &offset );

	if (pixels24 == NULL)
		pixels24 = new unsigned char[PanWidth * PanHeight * 3];

	TColorRGB *pixels32 = (TColorRGB *)pixels;
	for (int i = 0; i < PanWidth * PanHeight; i++)
	{
		pixels24[i * 3 + 2] = pixels32[i].r;
		pixels24[i * 3 + 1] = pixels32[i].g;
		pixels24[i * 3 ] = pixels32[i].b;
	}

	writer.WriteFrame(pixels24, nFotograma++);
	
    
    //Update the screen
	SDL_UpdateWindowSurface(gWindow);
	
    nFot++;
    return 0;
}


void CreateDistances(fftw_complex *display, std::vector<Vector3D> &positions, int tickCount)
{
    static float sinPos = 0;
    memset(display, 0, PanWidth*PanWidth*2*sizeof(double));

	
	Vector3D &xyz = positions[nPosition];
	if (nPosition > positions.size()-1)
		nPosition = 0;


#ifdef PP
	for (CFoco &f : vFocos)
	{
		double *distances = f.getDistanceList();
		for (int i = 0; i < PanWidth * PanHeight; i++)
		{
			display[i][0] += sin(
				[i] + sinPos);
			display[i][1] += sin(distances[i] + sinPos + PI / 2);
		}
	}
	
	CFoco &f = vFocos[0];
	double *distances = f.getDistanceList();
	for (int i = 0; i < PanWidth * PanHeight; i++)
	{

		distances[i]+=100;
	}
	

#else
	int i = 0;
	//for (CFoco &f : vFocos)
	//CFoco &f = vFocos[0];
	{
		float x1 = xyz.GetX() * 500;
		float y1 = xyz.GetY() * 500;
		float z1 = xyz.GetZ() * 500;
		for (int y = -PanHeight / 2; y < PanHeight / 2; y++)
		{
			for (int x = -PanWidth / 2; x < PanWidth / 2; x++)
			{
				float point = 0.0;
				float x2 = (float)x / Zoom;
				float y2 = (float)y / Zoom;
				double D = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1) + (z1*z1));

				display[(y + PanHeight / 2)*PanWidth + (x + PanWidth / 2)][0]+= sin(D + sinPos);
				display[(y + PanHeight / 2)*PanWidth + (x + PanWidth / 2)][1]+= sin(D + sinPos + PI/2);
			}
		}
	}
#endif	
    sinPos+=.1;
    
}
int MainLoop(std::vector<Vector3D> &positions)
{
	static int firstT = 0;
	
	if (firstT==0)
		firstT = GetTickCount();


    int done=0;
	const Uint8 *keys;
	fftw_complex *display = new fftw_complex[PanWidth * PanHeight* 2];

    while(done == 0)
	{
		SDL_Event event;
		while ( SDL_PollEvent(&event) )
		{
			if ( event.type == SDL_QUIT )  {  done = 1;  }
			if ( event.type == SDL_KEYDOWN )
			{
				switch (event.key.keysym.sym)
				{
					//case SDLK_LEFT:  x--; break;
					//case SDLK_RIGHT: x++; break;
					case SDLK_UP:		PressUp=true; break;
					case SDLK_DOWN:		PressDown = true; break;
                    case SDLK_LEFT:		PressLEFT = true; break;
					case SDLK_RIGHT:	PressRIGHT = true; break;
					case SDLK_PAGEUP:	PressPageUp = true; break;
					case SDLK_PAGEDOWN:	PressPageDw = true; break;
					case SDLK_c:		center = true; break;
					case SDLK_1:		Sel = 0; break;
					case SDLK_2:		Sel = 1; break;
                    case SDLK_ESCAPE:	done = 1; break;

				}
			}
            if ( event.type == SDL_KEYUP )
			{
                switch (event.key.keysym.sym)
				{
					case SDLK_UP:    PressUp=false; break;
					case SDLK_DOWN:  PressDown = false; break;
                    case SDLK_LEFT:  PressLEFT = false; break;
					case SDLK_RIGHT: PressRIGHT = false; break;
					case SDLK_PAGEUP:	PressPageUp = false; break;
					case SDLK_PAGEDOWN:	PressPageDw = false; break;
                }
            }
		}
		
		Vector3D &xyz = positions[0];

		zPos = xyz.GetZ();
		double xPos = xyz.GetX();
		double yPos = xyz.GetY();

		if (center == true)
		{
			if (xPos != 0)
			{
				xPos -= (xPos / 16);
				if ((xPos < 1)&&(xPos>-1))
					xPos = 0;
			}
			if (yPos != 0)
			{
				yPos -= (yPos / 16);

				if ((yPos < 1) && (yPos>-1))
					yPos = 0;
			}
			if ((xPos == 0) && (yPos == 0))
				center = false;
		}
		else
		{

			if (PressPageUp)
				zPos += (zPos / 16.0);
			if (PressPageDw)
				zPos -= (zPos / 16.0);
			if (zPos > 100000)
				zPos = 100000;
			if (zPos < 0.01)
				zPos = 0.01;

			if (PressLEFT)
			{
				xPos -= 150;
			}
			if (PressRIGHT)
			{
				xPos += 150;
			}
			if (PressUp)
			{
				yPos -= 150;
			}
			if (PressDown)
			{
				yPos += 150;
			}
		}
		
	
		xyz.SetX(xPos);
		xyz.SetY(yPos);
		xyz.SetZ(zPos);
		
		int tnew = GetTickCount()- firstT;
        CreateDistances(display,positions, tnew);
    	DrawScene(display,tnew);

    }
    return 0;
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
            	
            if(TTF_Init()==-1) 
            {
                printf("TTF_Init: %s\n", TTF_GetError());
                exit(2);
            }

            font=TTF_OpenFont("Arial-th.ttf", 16);
            if(!font) 
            {
                printf("TTF_OpenFont: %s\n", TTF_GetError());
                exit(2);
            }
		}
	}

	return success;
}

void CreateObjects(std::vector<Vector3D> &vPos)
{
	CFoco f(0);
	f.setPos3D(0, 0, 5000);
	vFocos.push_back(f);
	for (unsigned int i = 1; i < nFocos; i++)
	{
		double x1 = std::rand()-16384;
		double y1 = std::rand()-16384;
		double z1 = std::rand();
		CFoco f(i);
		f.setPos3D(x1, y1, z1);
		vFocos.push_back(f);
	}
	int i = 0;
	for (int y = -PanHeight / 2; y < PanHeight / 2; y++)
	{
		for (int x = -PanWidth / 2; x < PanWidth / 2; x++,i++)
		{
			double point = 0.0;
			double x2 = (double)x / Zoom;
			double y2 = (double)y / Zoom;
			for (CFoco &f : vFocos)
			{
				Vector3D &v = f.getPos3D();
				double x1 = v.GetX();
				double y1 = v.GetY();
				double z1 = v.GetZ();

				double D = sqrt((x2 - x1) * (x2 - x1) + (y2 - y1) * (y2 - y1) + (z1*z1));
				f.AddDistance(D);
			}
		}
	}
}

void LoadSvgPath(std::string filename, std::vector<Vector3D> &positions)
{
	std::ifstream t(filename);
	std::string str((std::istreambuf_iterator<char>(t)),
		std::istreambuf_iterator<char>());

	std::size_t f1 = str.find("d=\"m ");
	if (f1 != std::string::npos)
	{
		std::size_t f2 = str.find("z\"");
		if (f1 != std::string::npos)
		{
			std::string poly = str.substr(f1 + 5, f2 - f1 -5);
			for (;;)
			{
				std::size_t f3 = poly.find(' ');
				if (f3 == std::string::npos)
					break;
				std::string xy(poly.substr(0, f3));
				std::size_t f4 = xy.find(',');
				if (f4 != std::string::npos)
				{
					std::string sx = xy.substr(0, f4);
					std::string sy = xy.substr(f4+1);
					double x = strtod(sx.c_str(), NULL);
					double y = strtod(sy.c_str(), NULL);
					std::cout << x << "," << y << std::endl;
					Vector3D xyz(x, y, 10000);
					positions.push_back(xyz);

				}

				int a = 0;
				poly.erase(0, f3+1);

			}
		}
	}
	t.close();
}

int main(int argc, char* argv[])
{
    InitSdl();

	std::vector<Vector3D> positions;
	Vector3D xyz(0, 0, 10000);
	positions.push_back(xyz);
	//LoadSvgPath("Drawing.svg",positions);

    Frame=0;

    imageBack = SDL_CreateRGBSurface(0, PanWidth, PanHeight, 32,0,0,0,0);

    

	//vPos.push_back(Vector3D(0,0,100000));

	CreateObjects(vPos);

    MainLoop(positions);

    TTF_CloseFont(font);
    TTF_Quit();

	return 0;
}


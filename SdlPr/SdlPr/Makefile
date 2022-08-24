PROGNAME = SdlPr
CC = g++
INCLUDE = -I. -Iinclude
LIBDIR = -Llib


LIBS = -lSDL.dll -lSDL_ttf -lfreeimage
CFLAGS = -O3


all: main 
	$(CC) SdlPr.o SDL_win32_main.o $(LIBDIR) $(LIBS) $(CFLAGS) -o bin/$(PROGNAME)G.exe 

main:
	$(CC) -c $(CFLAGS) $(INCLUDE) SdlPr.cpp
	$(CC) -c $(CFLAGS) $(INCLUDE) SDL_win32_main.c

clean:
	del *.o *.exe



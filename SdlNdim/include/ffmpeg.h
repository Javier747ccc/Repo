#ifndef __FFMPEG_H__
#define __FFMPEG_H__

#define _CRT_SECURE_NO_WARNINGS
#define __STDC_CONSTANT_MACROS
#define __STDC_FORMAT_MACROS

int InitFFMpeg(std::string filename,int Width,int Height);
void EndFFMPeg();
void WriteFrame(unsigned int *pixels);

#endif
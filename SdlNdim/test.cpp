#include "RawAVIStream.h"
#include <iostream>
#include <Windows.h>

static void ProcessFrame(unsigned char *rgb, int width, int height)
{
	for (int i = 0; i < width * height; i++)
		rgb[i * 3 + 1] = 255;
}

void Run()
{
	RawAVIReader reader(_T("SAMPLE.AVI"));

	int width, height, frames;
	reader.GetSize(width, height, frames);

	RawAVIWriter writer(_T("out.avi"), width, height);

	unsigned char *rgb = new unsigned char[3 * width * height];
	for (int i = 0; i < frames; i++)
	{
		reader.ReadFrame(rgb, i);
		ProcessFrame(rgb, width, height);
		writer.WriteFrame(rgb, i);
	}
	delete []rgb;
}

void main()
{
	std::cout << "This test program demonstrates usage of RawAVIStream class" << std::endl;
	std::cout << "The library is a thin wrapper around Microsoft AVIFile API" << std::endl;
	std::cout << "Author: Arkadi Kagan" << std::endl;

	CoInitialize(NULL);
	Run();
	CoUninitialize();
}

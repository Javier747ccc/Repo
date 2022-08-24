////////////////////////////////////////////////////////////////////////////
// This library is built to be used as a way to perform image processing
// on AVI files through exclusive use of Microsoft AVIFile API
// Author: Arkadi Kagan
////////////////////////////////////////////////////////////////////////////

#include "RawAVIStream.h"
#include <windows.h>
#include <vfw.h>
#include <iostream>

// This two structures are defined to hide Win32 definitions from the library users.
// The only Win32 reminder is TCHAR. So far it seems right to use TCHAR rather than
// forcing WCHAR or CHAR API set explicitly.
struct tagAVIReader
{
	PGETFRAME getframe;
	BITMAPINFOHEADER *bi;
	int frames;
};
struct tagAVIWriter
{
	PAVISTREAM ppavi;
	int width, height;
};

// This function prints out formatted error message and terminates process.
// We use FormatMessage, following MSDN sample.
// We use std::cout because:
// 1. it is standard C++
// 2. Microsoft attempts to deprecate printf
static void Error(DWORD error)
{
	LPVOID lpMsgBuf;
	FormatMessage( 
		FORMAT_MESSAGE_ALLOCATE_BUFFER | 
		FORMAT_MESSAGE_FROM_SYSTEM | 
		FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL,
		error,
		0, // Default language
		(LPTSTR) &lpMsgBuf,
		0,
		NULL 
	);
	std::cerr << "Error " << error << ": " << lpMsgBuf << std::endl;
	LocalFree( lpMsgBuf );
	exit(error);
}
static void Check(HRESULT result)
{
	if (result != S_OK)
		Error(result);
}
static void CheckNull(void *result)
{
	if (result == NULL)
		Error(GetLastError());
}

RawAVIReader::RawAVIReader(TCHAR *filename)
{
	PAVISTREAM ppavi;
	long format_length;
	avi = new tagAVIReader;

	// According to MSDN, this function keeps a counter of calls and
	// can be called several times safely.
	AVIFileInit();

	// We assume only a single video stream in AVI.
	// Alternative to this call is AVIFileOpen/AVIFileGetStream. However, in this
	// project we are only interested in video processing.
	Check(AVIStreamOpenFromFile(&ppavi, filename, streamtypeVIDEO, 0, OF_READ, NULL));

	// Get video frame format as defined in the given file. We are specially
	// interested in width and height parameters.
	Check(AVIStreamFormatSize(ppavi, 0, &format_length));
	avi->bi = new BITMAPINFOHEADER;
	Check(AVIStreamReadFormat(ppavi, 0, avi->bi, &format_length));

	avi->frames = AVIStreamLength(ppavi);

	// Adjust BITMAPINFOHEADER and let AVIFile API to know what output we expect.
	// The "getframe" pointer is internally IGetFrame interface and will be used
	// to access all the stream frames.
	avi->bi->biBitCount = 24;
	avi->bi->biCompression = BI_RGB;
	avi->bi->biSizeImage = 0;
	CheckNull(avi->getframe = AVIStreamGetFrameOpen(ppavi, avi->bi));

	// Since all the AVIFile API is COM based and we don't need the stream pointer
	// anymore, we can release this pointer. The underlying object will be destroyed
	// when its reference in "getframe" interface is released.
	AVIStreamRelease(ppavi);
}

RawAVIReader::~RawAVIReader()
{
	Check(AVIStreamGetFrameClose(avi->getframe));
	delete avi->bi;
	delete avi;

	AVIFileExit();
}

void RawAVIReader::GetSize(int &width, int &height, int &frames)
{
	frames = avi->frames;
	width = avi->bi->biWidth;
	height = avi->bi->biHeight;
}

void RawAVIReader::ReadFrame(unsigned char *frame, int index)
{
	// According to MSDN, this buffer is valid at least until no other AVIFile API is called.
	void *buffer = AVIStreamGetFrame(avi->getframe, index);
	// According to the format we have specified in our call to AVIStreamGetFrameOpen, the buffer
	// content is as follow:
	//   BITMAPINFOHEADER
	//   Raw data as a pixel sequence. Each pixel is a three bytes RGB triple.
	CopyMemory(frame, (BYTE*)buffer + ((BITMAPINFOHEADER*)buffer)->biSize, ((BITMAPINFOHEADER*)buffer)->biSizeImage);
}

RawAVIWriter::RawAVIWriter(TCHAR *filename, int width, int height)
{
	BITMAPINFOHEADER bi;
	PAVIFILE pavifile;
	AVISTREAMINFO info;

	avi = new tagAVIWriter;

	// According to MSDN, this function keeps a counter of calls and
	// can be called several times safely.
	AVIFileInit();

	// Create empty AVI file
	Check(AVIFileOpen(&pavifile, filename, OF_WRITE | OF_CREATE, NULL));

	// Create a video stream with minimum data needed.
	ZeroMemory(&info, sizeof(info));
	info.fccType = streamtypeVIDEO;
	info.dwScale = 1;
	info.dwRate = 25;	// This is rather arbitrary, although inspired by PAL TV standard
	Check(AVIFileCreateStream(pavifile, &avi->ppavi, &info));
	AVIFileRelease(pavifile);

	// Set data format to raw RGB, compatible to the format specified in RawAVIReader above.
	// Unfortunately, we can't specify MPEG compression, although MPEG schema is designed
	// for streaming. If we do specify compression, it must be done outside somehow.
	// We specify only minimal data needed to render a valid AVI video.
	ZeroMemory(&bi, sizeof(bi));
	bi.biSize = sizeof(bi);
	bi.biWidth = width;
	bi.biHeight = height;
	bi.biPlanes = 1;
	bi.biBitCount = 24;
	bi.biCompression = BI_RGB;
	bi.biXPelsPerMeter = 1000;
	bi.biYPelsPerMeter = 1000;
	Check(AVIStreamSetFormat(avi->ppavi, 0, &bi, sizeof(bi)));

	avi->width = width;
	avi->height = height;
}
RawAVIWriter::~RawAVIWriter()
{
	AVIStreamClose(avi->ppavi);
	delete avi;

	AVIFileExit();
}

void RawAVIWriter::WriteFrame(unsigned char *frame, int index)
{
	AVIStreamWrite(avi->ppavi, index, 1, frame,
		3 * avi->width * avi->height,
		0, NULL, NULL);
}

// This function is not needed for raw data processing.
// However, we find it handy to have a compression option in our library.
// Unfortunately, we can't specify compression if we are intended to
// feed-in raw data to our video stream.
void aviCompress(TCHAR *source, TCHAR *target)
{
	AVICOMPRESSOPTIONS options;
	PAVISTREAM ppavi;
	AVIStreamOpenFromFile(&ppavi, source, streamtypeVIDEO, 0, OF_READ, NULL);

	memset(&options, 0, sizeof(options));
	options.fccType = streamtypeVIDEO;
	options.fccHandler = mmioFOURCC('M','S','V','C');
	options.dwKeyFrameEvery = 2;
	options.dwQuality = 1;
	options.dwFlags = AVICOMPRESSF_KEYFRAMES;
	AVISave(target, NULL, NULL, 1, ppavi, &options);
}

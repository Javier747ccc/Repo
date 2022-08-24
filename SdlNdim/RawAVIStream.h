////////////////////////////////////////////////////////////////////////////
// This library is built to be used as a way to perform image processing
// on AVI files through exclusive use of Microsoft AVIFile API
// Author: Arkadi Kagan
////////////////////////////////////////////////////////////////////////////

#ifndef __RAW_AVI_STREAM_H
#define __RAW_AVI_STREAM_H

#include <tchar.h>

struct tagAVIReader;
struct tagAVIWriter;

// We build this library with intention to get maximum performance from the given API.
// In order to achive this goal, we have to keep different artifacts for reading and writing.

class RawAVIReader
{
private:
	struct tagAVIReader *avi;
public:
	RawAVIReader(TCHAR *filename);
	~RawAVIReader();
	void GetSize(int &width, int &height, int &frames);
	void ReadFrame(unsigned char *frame, int index);
};

class RawAVIWriter
{
private:
	struct tagAVIWriter *avi;
public:
	RawAVIWriter(TCHAR *filename, int width, int height);
	~RawAVIWriter();
	void WriteFrame(unsigned char *frame, int index);
};

void aviCompress(TCHAR *source, TCHAR *target);

#endif // __RAW_AVI_STREAM_H

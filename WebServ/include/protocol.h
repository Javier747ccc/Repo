#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

#include <deque>
#include "utl.h"

enum 
{
BUFFER_CHAR = 0x11,
BUFFER_EOF,
};
template <class T>
class TBuf
{
private:
	int Len;
	int BlockMemLen;
	T *Buffer;
public:
	TBuf()
	{
		Len = 0;
		Buffer=NULL;
		BlockMemLen = 1024;
	};
	void clear()
	{
		if (Len>0)
		{
			delete Buffer;
			Buffer = NULL;
			Len = 0;
		}
	}
	void push_back(T car)
	{


		int BlockPos = Len % BlockMemLen;
		int nBlock = (Len / BlockMemLen);
		if (BlockPos==0)
		{
			T *pNewBuf = new T[(nBlock+1) * BlockMemLen];
			if (Buffer!=NULL)
			{
				dmemcpy(pNewBuf,Buffer,nBlock * BlockMemLen);
				delete Buffer;
			}
			Buffer = pNewBuf;
		}
		Buffer[Len++] = car;
	}
	T pop_back()
	{
		if (Len>0)
		{
			Len--;
			T c = Buffer[Len];
			if (Len==0)
			{
				delete Buffer;
				Buffer = NULL;
			}
			return c;
		}
		else
			return 0;
	}
	int size(){return Len;};
	T *GetBuf(){return &Buffer[0];};
	void SetBuf(T *pbuf,int len)
	{
		clear();
		int nBlocks = (len / BlockMemLen)+1;
		Buffer = new T[nBlocks * BlockMemLen];
		dmemcpy(Buffer,pbuf,len);
	};
	
	void assign(int count,T val)
	{
		clear();
		int nBlocks = (count / BlockMemLen)+1;
		Buffer = new T[nBlocks * BlockMemLen];
		int valsize = sizeof(T);
		for (int i=0;i<count;i++)
		{
			Buffer[i] = val;
		}
		Len = count;
	};
};

typedef struct
{
	unsigned int Magic;
	int Type;
	unsigned int Len;
}Theader;

template <class T>
struct SBuffer
{
protected:
	//TBuf<T> Buffer;
	vector<T> Buffer;
	int sizeofheader;
	void SetHeader(const T *d)
	{
		Buffer.clear();
		for (int i=0;i<sizeofheader;i++)
		{
			Buffer.push_back(d[i]);
		}
	}
public:
	
	SBuffer()
	{
		int R = (sizeof(Theader) % sizeof(T)) ? 1:0;
		sizeofheader = (sizeof(Theader)/sizeof(T)+R);
	}
	SBuffer(int type)
	{
		int R = (sizeof(Theader) % sizeof(T)) ? 1:0;
		sizeofheader = (sizeof(Theader)/sizeof(T)+R);
		Buffer.assign(sizeofheader,0);
		SetMagic((char *)"_JCU");
		SetType(type);
		SetLen(0);
	}
	~SBuffer()
	{
		clear();
	}
	void clear()
	{
		Buffer.clear();
		Buffer.assign(sizeofheader,0);
		SetMagic((char *)"_JCU");
		SetType(0);
		SetLen(0);
	}
	static int GetSizeOfHeader()
	{
		int R = (sizeof(Theader) % sizeof(T)) ? 1:0;
		int s = (sizeof(Theader)/sizeof(T)+R);
		return s;
	}
	Theader *GetHeader()
	{
		char *p = &Buffer[0];
		return (Theader *)p;
	}
	
	T *GetCharBuffer(){return &Buffer[0];};
	const int GetCharBufferLen(){return Buffer.size()*sizeof(T);};
	const T *GetBuffer(){return Buffer.GetBuf();};
	const int GetBufferLen(){return Buffer.size();};
	//const char *GetData(){return &Buffer.GetBuf()[GetSizeOfHeader()];};
	T *GetData() { return &Buffer[GetSizeOfHeader()]; };

	const unsigned int GetDataLen(){return GetHeader()->Len;};
	
	void SetMagic(char *magic)
	{
		Theader *pheader = GetHeader();
		pheader->Magic = *(unsigned int*)magic;
		//GetHeader()->Magic = *(unsigned int*)magic;
	}
	bool IsTheMagic(char *magic)
	{
		return (GetHeader()->Magic == *(unsigned int*)magic);
	}
	const char *GetMagic()
	{
		return (const char *)GetHeader()->Magic;
	}
	void SetLen(unsigned int len)
	{
		Theader *p = GetHeader();
		GetHeader()->Len = len;
	}
	unsigned int GetLen()
	{
		return GetHeader()->Len;
	}
	
	int GetType()
	{
		return GetHeader()->Type;
	}
	void SetType(int type)
	{
		GetHeader()->Type = type;
	}
	
	void SetData( T *d,int Len)
	{
		Buffer.insert(Buffer.end(), d, &d[Len]);

		SetLen(Len);
		
	}
	void SetCharBuf(T *d)
	{
		int Len;
		for (Len=0;*d!=0;Len++)
		{
			Buffer.push_back(*d);
			d++;
		}
		SetLen(Len);
	}
	SBuffer<T> &operator = ( const char *buf)
	{
		Theader *h = (Theader *)buf;
		SetHeader(buf);
		SetData(&buf[sizeofheader],h->Len);
		return *this;
	}
/*
	SBuffer<T>*& operator = ( const char *buf)
	{
		Theader *h = (Theader *)buf;
		Buffer.clear();
		SetLineBuf(buf,sizeofheader + h->Len);
		return this;
	}
*/	
};
#endif // __PROTOCOL_H__

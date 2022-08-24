#ifndef __PROCESA_TRAMAS_H__
#define __PROCESA_TRAMAS_H__
#define HAVE_STRUCT_TIMESPEC 1

#include <vector> 
#include <sstream>
#include <strstream>
#include <fstream>
#include <iostream>

#define SENDLOG	

using namespace std;

#ifdef LINUX_MODE
#include <sys/types.h>
#include <sys/socket.h>
#else
#include <Winsock2.h>
#endif
#include "defines.h"
#include "pthread.h"
#include "semaphore.h"
#include "regex.h"
#include "protocol.h"



std::string TrimString(const std::string cadena,const char car=' ');
char *TrimString(const char *cadena,const char car=' ');
char *TrimString(const char *cadena,const char *cars=" ");

class TIP
{
public:
	unsigned char IP[4];
	char strIP[32];
	bool logged;
	TIP(string s)
	{
		memset(IP,0,4);
		logged = false;
		size_t l = s.length();
		memset(strIP,0,32);
		memcpy(strIP, s.c_str(), l);
	}
	TIP(unsigned char *b)
	{
		memcpy(IP, (char *)b, 4);
		logged = false;
		memset(strIP, 0, 32);
		sprintf_s(strIP, "%u.%u.%u.%u", b[0], b[1], b[2], b[3]);
	}
	void SetLogged(bool b)
	{
		logged = b;
	}
	bool GetLogged()
	{
		return logged;
	}
};


void printRegExError(int Error);
class CProcesaTramas
{
public:
	typedef struct
	{
		string regex;
		regex_t re;
		vector<int> args;
		
	}TRegs;
	typedef struct
	{
		int nArg;
		std::string str;
	}TArgs;
	

private:
	template <class T>
	class TVBuffer
	{
	private:
		SBuffer<T> Buffer;
	public:
		SBuffer<T> &GetBuffer(){return Buffer;};
	};

	vector<TRegs> Regs;
	//ofstream flog;
	pthread_mutex_t mutex;
	sem_t sem;
	std::vector<char> BufferIn;
	std::vector<std::vector<char> > VectorBufferIn;
	bool SendBufferFlag;
	deque<SBuffer<char>*> BufferOut;
	SOCKET socket;
	SOCKADDR_IN accept_sin;
	bool closeflag;
	bool _RcvInProgress;
	string _login;
	
public:
	bool Echo;
	bool binary;
	bool FinalizaProceso;
	void SetLogin(string l) { _login = l; }
	char *datos;
	TIP *tip;
	CProcesaTramas()
	{
		Echo = false;
		binary = false;
		SendBufferFlag = false;
		closeflag = false;
		socket = 0;
		_RcvInProgress = false;
		sem_init(&sem, 0, 1);
		FinalizaProceso = false;
		tip = NULL;
		if (pthread_mutex_init(&mutex, NULL) != 0)
		{
			printf("\n mutex init failed\n");

		}

		
	}
	CProcesaTramas(char *p)
	{
		datos = p;
		CProcesaTramas();
	}
	CProcesaTramas(TIP *t)
	{
		CProcesaTramas();
		SetIpDataPointer(t);
	}
	void SetIpDataPointer(TIP *d)
	{
		tip = d;
	}
	
	void setAutorizado(bool aut)
	{
		if (tip != NULL)
		{
			tip->SetLogged(aut);
		}
	}
	bool GetAutorizado()
	{
		if (tip != NULL)
		{
			return tip->GetLogged();
		}
		return false;
	}

	virtual void Init()
	{
		
	}

	void Incdata()
	{
	}
	void SetCloseFlag(bool st)
	{
		closeflag = st;
	}
	bool GetCloseFlag()
	{
		return closeflag;
	}


	void OkRx()
	{
		sem_post(&sem);
	}

	void SetBinary(bool state)
	{
		binary = state;
	}
	SOCKADDR_IN &GetSin(){return accept_sin;};
	bool LoadParser(string file)
	{
	
		return true;
	}
	
	bool ParserLine(std::string Line)
	{
		return false;
	}
	
	bool LoadRegEx(std::string file)
	{
		return true;
	}
	virtual ~CProcesaTramas()
	{
		if (mutex!=NULL)
			pthread_mutex_destroy(&mutex);
		sem_destroy(&sem);
	}
	void SetEcho(bool state){Echo = state;};
	void SetSocket(SOCKET s){socket = s;};
	void SetAddrSin(SOCKADDR_IN in){ accept_sin = in ;};
	SOCKET GetSocket(){return socket;};
	virtual void ProcessLine(int nReg,vector<TArgs> args)
	{
	}
	virtual void ProcessBinaryLine(std::vector<char> &bufline)
	{
	}
	virtual void LineNotFound(std::string Line)
	{
	}
	virtual void AnalyzeLine(string line)
	{
	}
	

	void Insert(char ch)
	{
		pthread_mutex_lock(&mutex);
		BufferIn.insert(BufferIn.begin(),ch);
		pthread_mutex_unlock(&mutex);
	}
	void InsertBufVector(std::vector<char> line)
	{
		pthread_mutex_lock(&mutex);
		VectorBufferIn.insert(VectorBufferIn.begin(),line);
		pthread_mutex_unlock(&mutex);
	}
	

	int SendToServer(char *buf,unsigned int len,bool sendnow = false)
	{
		//SBuffer<char> vbuffer(BUFFER_CHAR);
		while(buf[len-1]==0)
			len--;
		if (len<=0)
			return 0;
		SBuffer<char> *vbuf = new SBuffer<char>(BUFFER_CHAR);
		vbuf->SetData(buf,len);
		BufferOut.push_back(vbuf);
		SendBufferFlag = sendnow;
		return 1;
	};
	int SendBinaryToServer(char *buf,unsigned int len,bool sendnow = false)
	{
		SBuffer<char> *vbuf = new SBuffer<char>(BUFFER_CHAR);
		vbuf->SetData(buf,len);
		BufferOut.push_back(vbuf);
		SendBufferFlag = sendnow;
		return 1;
	};

	int SendTextToServer(const char *format,...)
	{
		va_list args;
		va_start( args, format );

		int len = 2048;
		char *buffer = (char*)kmalloc( len * sizeof(char) );
		len = vsnprintf( buffer, 255, format, args ); 
		
		SendToServer(buffer,len);
		kfree( buffer );
		return 0;
	}
	void SendEofToServer()
	{
		SBuffer<char> *vbuf = new SBuffer<char>(BUFFER_EOF);
		BufferOut.push_back(vbuf);
		SendBufferFlag = true;
	}

	void ClearLog()
	{
		//DeleteFile((LPCWSTR)"WebServ.log");
		DeleteFile("WebServ.log");
		//DelDirectoryExtensions("./", "*.log");
	}
	void SendLog(const char *pcb, unsigned int len, SOCKET Sock=0)
	{
		ofstream fb("WebServ.log", ios::app | ios::binary);
		fb << ">>>>> (" << Sock << ") <<<<<<" << endl;
		fb.write(pcb, len);
		fb << endl << "----End----" << endl;
		fb.close();
		ostringstream StrSock;
		StrSock << "S" << Sock << ".log";
		ofstream fbs(StrSock.str(), ios::app | ios::binary);
		fbs.write(pcb, len);
		fbs << endl << "----End----" << endl;
		fbs.close();

	}
	int SendBufferToServer()
	{
		char *pbuf;
		if (SendBufferFlag)
		{
			if (BufferOut.size())
			{
				int nToSend = 0;
				for (deque<SBuffer<char>*>::iterator it = BufferOut.begin(); it != BufferOut.end(); it++)
				{
					SBuffer<char> *pvbuffer = *it;
					nToSend += pvbuffer->GetDataLen();
				}
				if (nToSend > 0)
				{
					pbuf = new char[nToSend];
					int pos = 0;
					for (deque<SBuffer<char>*>::iterator it = BufferOut.begin(); it != BufferOut.end(); it++)
					{
						SBuffer<char> *pvbuffer = *it;
						const char *pcb = pvbuffer->GetData();
						int Len = pvbuffer->GetDataLen();
						memcpy(&pbuf[pos], pcb, Len);
						pos += Len;
					}
					SOCKET sk = GetSocket();
					send(sk, pbuf, nToSend, 0);
#ifdef SENDLOG
					SendLog(pbuf, nToSend, sk);
#endif //SENDLOG
					delete pbuf;
				}
				BufferOut.clear();
				SendBufferFlag = false;
				return 1;
			}
		}
		return 0;
	}

	int PopBinaryBuffer(std::vector<char> &line)
	{
		pthread_mutex_lock(&mutex);
		size_t l = VectorBufferIn.size();
		if (l > 0)
		{
			
			line = VectorBufferIn.back();
			VectorBufferIn.pop_back();
			pthread_mutex_unlock(&mutex);
			return 1;
		}
		else
		{
			pthread_mutex_unlock(&mutex);
			return -1;
		}
		
	}

	int PopNextBufChar(void)
	{
		char caracter;
		caracter = -1;
		
		pthread_mutex_lock(&mutex);
		if (BufferIn.size() > 0)
		{
			caracter = BufferIn.back();
			BufferIn.pop_back();
			pthread_mutex_unlock(&mutex);
			return caracter;
		}
		else
		{
			pthread_mutex_unlock(&mutex);
			return -1;
		}
	}
	template<class T> static void *StaticRun(void * param)
	{
		static_cast<T*>(param)->Run();
		return 0;
	}

	virtual void Run()
	{
		
		Incdata();
		std::string Line = "";

		while (GetCloseFlag()==false)
		{
			int caracter = PopNextBufChar();
			
			if (caracter >= 0)
			{
				Line += (char)caracter;
				if (caracter == '\n')
				{
					//cout << Line;
					AnalyzeLine(Line);
					Line.clear();
				}
			}
			else
			{
				//sem_wait(&sem);
				Sleep(10);
				int a = 0;
			}
		}
		SetCloseFlag(false);
	}

};


#endif //__PROCESA_TRAMAS_H__

#ifndef __PSERVER_H__
#define __PSERVER_H__

#define HAVE_STRUCT_TIMESPEC 1
#include <vector>
#include <map>
#include "pthread.h"
#include "semaphore.h"
#include "procesatramas.h"
#include "mysql.h"

#define TCPBUFSIZE 4096
class CServer
{
private:
	typedef struct 
	{
		int port;
		void *pserver;
		SOCKET socket;
		void *ClassPointer;

	}ThreadCallArgs;
	int nProcessUp,nProcessToUp;

	pthread_mutex_t mutex;

	MYSQL *mysql;
public:
	char *datos;
	CServer()
	{
		nProcessUp = 0;
		nProcessToUp = 0;
		InitDB();

		if (pthread_mutex_init(&mutex, NULL) != 0)
		{
			printf("\n mutex init failed\n");
			
		}
	};
	~CServer();
	int WaitAllProcessUP(unsigned int TimeOut)
	{
		DWORD dwStart = GetTickCount();
		while(1)
		{
			if (nProcessUp == nProcessToUp)
				return 1;
				
			if( (int)GetTickCount() - (int)dwStart >= (int)TimeOut)
				return 0;
			Sleep(1);
		}
	}
	

	template<class T> unsigned int  TCPServerThread(int port)
	{
		SOCKET SockTCP;
		sockaddr_in  local_sin, accept_sin;
		map<string, TIP *> MapIP;
		int accept_sin_len;
		int iResult = 0;

		DelDirectoryExtensions("./", "log");

		// Create a TCP/IP socket, WinSocket.
		if ((SockTCP = socket (AF_INET, SOCK_STREAM, IPPROTO_TCP)) == INVALID_SOCKET) 
		{
			return -1;
		}

		// Fill out the local socket's address information.
		local_sin.sin_family = AF_INET;
		local_sin.sin_port = htons (port);  
		local_sin.sin_addr.s_addr = htonl (INADDR_ANY);

		// Associate the local address with WinSocket.

		iResult = bind(SockTCP, (struct sockaddr *) &local_sin, sizeof(local_sin));
		
		if (iResult == SOCKET_ERROR) 
		{
			closesocket (SockTCP );
			return -1;
		}
		
		// Establish a socket to listen for incoming connections.
		if (listen (SockTCP , SOMAXCONN) == SOCKET_ERROR) 
		{
			closesocket (SockTCP );
			return -1;
		}

		accept_sin_len = sizeof (accept_sin);

		printf("TCPServerThread on port %d OK\n",port);
		nProcessUp++;
		
			
		for (;;)
		{
			
			SOCKET ClientSock = accept (SockTCP, (struct sockaddr *) &accept_sin, (int *) &accept_sin_len);
			if (ClientSock == INVALID_SOCKET) 
			{
				return -1;
			}

			ostringstream ssIP;
			ssIP << (int)accept_sin.sin_addr.S_un.S_un_b.s_b1 << "." << (int)accept_sin.sin_addr.S_un.S_un_b.s_b2 << "." << (int)accept_sin.sin_addr.S_un.S_un_b.s_b3 << "." << (int)accept_sin.sin_addr.S_un.S_un_b.s_b4;

			unsigned char b[4] = { accept_sin.sin_addr.S_un.S_un_b.s_b1,accept_sin.sin_addr.S_un.S_un_b.s_b2,accept_sin.sin_addr.S_un.S_un_b.s_b3,accept_sin.sin_addr.S_un.S_un_b.s_b4 };
			TIP *Tip = NULL;
			T *process = new T((char *)"Hola");
			

			auto it = MapIP.find(ssIP.str());
			if (it == MapIP.end())
			{
				Tip = new TIP(b);
				MapIP[ssIP.str()] = Tip;
			}
			else
			{
				Tip = it->second;
			}
			process->SetIpDataPointer(Tip);
			process->SetSocket(ClientSock);
			process->SetAddrSin(accept_sin);

			pthread_t t1;
			// Este thread analiza las peticiones llegadas que están almacenadas en el buffer
			if (pthread_create(&t1, NULL, &StaticRunP<T>, (void *) process) != 0)
			{
				return -1;
			}
			
			
			
			ThreadCallArgs *thargs = new ThreadCallArgs;
			
			thargs->socket = ClientSock ;
			thargs->pserver = this;
			thargs->ClassPointer = (void *)process;
			pthread_t t2;
			// Este thread escucha por TCP y almacena todo en un buffer

			if (pthread_create(&t2, NULL, &StaticTCPThread<T>, (void *) thargs) != 0)
			{
				return -1;
			}
			
			
			
		}
		
		closesocket (SockTCP);
		return true;
	}


		
	template<class T> bool AddTcp(int port)
	{
		nProcessToUp++;
		ThreadCallArgs *args = new ThreadCallArgs();
		args->port = port;
		args->pserver = this;
		pthread_t t;
		if (pthread_create(&t, NULL, &StaticTcpRun<T>, (void *) args) == 0)
		{
			return true;
		}
		return false;
	}
	

	
	
	template<class T> static void *StaticTcpRun(void * param)
	{
		ThreadCallArgs *callargs = reinterpret_cast<ThreadCallArgs *>(param);
		static CServer* pserver = static_cast<CServer*>(callargs->pserver);
		static_cast<CServer*>(pserver)->TCPServerThread<T>(callargs->port);
		delete callargs ;
		return 0;
	} 
	
	
	
	template<class T> unsigned int  TcpRxThread(SOCKET Sock,T *ClassPtr)
	{
		int index = 0,                      // Integer index
		iReturn;							// Return value of recv function
		char szServerA[2048];               // ASCII string 
		std::string Linea;


		SOCKADDR_IN tt = ClassPtr->GetSin();
		ostringstream ssIP,ssnet;
		ssIP << (int)tt.sin_addr.S_un.S_un_b.s_b1 << "." << (int)tt.sin_addr.S_un.S_un_b.s_b2 << "." << (int)tt.sin_addr.S_un.S_un_b.s_b3 << "." << (int)tt.sin_addr.S_un.S_un_b.s_b4;

		pthread_mutex_lock(&mutex);
		cout << "From IP:" << ssIP.str() << "Sock:" << Sock << endl;
		pthread_mutex_unlock(&mutex);
		Linea.clear();

		BOOL CR = FALSE;
		std::string strhex;
		int len = 0;

		for (;;)
		{
			// Receive data from the client.
			strhex = "";
			iReturn = recv (Sock, szServerA, sizeof (szServerA), 0);
			if (iReturn==0)
			{
				// Socket Cerrado
				break;
			}
			if (iReturn==SOCKET_ERROR)
			{
				int nRet = WSAGetLastError();
				Sleep(0);
				break;
			}
			if (ClassPtr->Echo==TRUE)
			{
				szServerA[iReturn]=0;
				std::string Line(szServerA);
				SendTCP(Sock,Line);
			}
			if (ClassPtr->binary==true)
			{
				std::vector<char> buf;
				for (int i=0;i<iReturn;i++)
				{
					buf.push_back(szServerA[i]);
				}

				ClassPtr->SendLog(szServerA, iReturn, Sock);
				ClassPtr->InsertBufVector(buf);

			}
			else
			{
				ClassPtr->SendLog(szServerA, iReturn, Sock);
				for (int i=0;i<iReturn;i++)
				{
					
					ClassPtr->Insert(szServerA[i]);
				}
				ClassPtr->OkRx();
			}
		} 
		pthread_mutex_lock(&mutex);
		cout << "Close IP:" << ssIP.str() << "Sock:" << Sock << endl;
		pthread_mutex_unlock(&mutex);
		shutdown (Sock, 0x02);
		closesocket (Sock);
		return 1;
	}



	
	template<class T> static void *StaticRunP(void * param)
	{
		static_cast<T*>(param)->Run();
		return 0;
	} 
		

	template<class T> static void *StaticTCPThread(void * param)
	{
		ThreadCallArgs *thargs = (ThreadCallArgs *)param;
		T *process = reinterpret_cast<T*>(thargs->ClassPointer);
		static_cast<CServer*>(thargs->pserver)->TcpRxThread<T>(thargs->socket,process );
		closesocket(thargs->socket);
		process->SetCloseFlag(true);
		process->OkRx();
		while(process->GetCloseFlag()==true){Sleep(10);};
		delete process;
		return 0;
	} 

	int SendTCP(SOCKET Sock,std::string Line);
	bool InitDB();
	void CreateTable();

};





#endif //__PSERVER_H__

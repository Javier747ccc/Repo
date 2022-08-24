#ifndef __CWEB_H__
#define __CWEB_H__
#include <iostream>
#include <deque>
#include <vector>
#include <map>



enum ConnectType
{
	CT_Error = -1,
	CT_None,
	CT_Html,
	CT_Css,
	CT_Ico,
	CT_Gif,
	CT_Jpeg,
	CT_Bmp,
	CT_Png,
	CT_Text,
	CT_Js,
	CT_Php
};

enum CommandType
{
	Cm_None,
	Cm_Post,
	Cm_Get
};

using namespace std;
#define HAVE_STRUCT_TIMESPEC 1

#include <stdio.h>
#include "procesatramas.h"

class CWebProcesaTramas : public CProcesaTramas
{
private:
	bool EndThreads;
	int ActiveThreads;
	bool LoginAutorizado;
	unsigned int IDUsuario;
	unsigned int TramaLenght;
	bool ProcessLenght;
	int ActualCommand;
	string Action;
	
public:
	CWebProcesaTramas()
	{
		EndThreads = false;
		ActiveThreads = 0;
		IDUsuario = 0;
		LoginAutorizado = false;
		TramaLenght = 0;
		ProcessLenght = false;
		Action = "";
		
	}
	void Run();
	bool IsEmailInBD(string &email);
	void AnalyzeLine(string line);
	void SendOK(string buf);
	void SendError();
	void SendPage(string pathname, bool Processpage=false);
	void SendHtml(string html, bool Processpage = false);
	virtual void ProcessPage(string &html);
	virtual void ProcessVariables(string &html);
	virtual void CargaPagina(string &html);
	virtual void CallAction(int ActualCommand, string ActionFile,string Params);
	~CWebProcesaTramas()
	{
		EndThreads = true;
		while (ActiveThreads)
		{
			Sleep(1);
		}
		/*
		stringstream ip;
		ip << (int)GetSin().sin_addr.S_un.S_un_b.s_b1 << "."
			<< (int)GetSin().sin_addr.S_un.S_un_b.s_b2 << "."
			<< (int)GetSin().sin_addr.S_un.S_un_b.s_b3 << "."
			<< (int)GetSin().sin_addr.S_un.S_un_b.s_b4 << ends;

		cout << "Conexion " << ip.str() << " cerrada" << endl;
		*/
	}
	void PhpProcess(string &pbuf);
	ConnectType LoadPage(string filename, vector<char> &VecBuf);
	virtual int AutorizaFromHtmlForm(vector <string > &PathVector);
	virtual void Lista1(unsigned int ID, string &html);

};

#endif // __CWEB_H__
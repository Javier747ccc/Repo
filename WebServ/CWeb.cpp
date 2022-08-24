#include <stdio.h>
#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <algorithm>
#include "procesatramas.h"
#include "CWeb.h"
#include "Area81.h"


using namespace std;

int CWebProcesaTramas::AutorizaFromHtmlForm(vector <string > &PathVector)
{
	return 0;
}

void CWebProcesaTramas::SendOK(string buf)
{
	ostringstream ToSend;
	ToSend << "HTTP/1.1 200 OK" << endl;
	ToSend << "Date: Tue, 04 Sep 2018 18:01:02 GMT" << endl;
	ToSend << "Server : jcu / 1.0" << endl;
	ToSend << "Content-Length: " << buf.size() << endl;
	ToSend << "Content-Type:text/html" << endl;
	ToSend << endl;


	SendToServer((char *)ToSend.str().c_str(), (unsigned int)ToSend.str().length(), false);
	SendToServer((char *)(char *)&buf[0], (unsigned int) buf.size(), false);
	SendToServer((char *)"\r\n\r\n", 4, true);
	SendBufferToServer();
}

void CWebProcesaTramas::SendError()
{
	ostringstream ToSend;

	ToSend << "HTTP / 1.1 404 Not Found" << endl;
	ToSend << "Date : Sat, 08 Sep 2018 08:39 : 19 GMT" << endl;
	ToSend << "Server : JCU / 1.0" << endl;
	ToSend << "Content - Length : 289" << endl;
	ToSend << "Keep - Alive : timeout = 15, max = 99" << endl;
	ToSend << "Connection : Keep - Alive" << endl;
	ToSend << "Content - Type : text / html; charset = iso - 8859 - 1" << endl;
	ToSend << endl;
	ToSend << "<!DOCTYPE HTML PUBLIC \"-//IETF//DTD HTML 2.0//EN\">" << endl;
	ToSend << "<html><head>" << endl;
	ToSend << "<title>404 Not Found< / title>" << endl;
	ToSend << "< / head><body>" << endl;
	ToSend << "<h1>Not Found< / h1>" << endl;
	ToSend << "<p>The requested URL / favicon.ico was not found on this server.< / p>" << endl;
	ToSend << "<hr>" << endl;
	ToSend << "<address>Apache / 2.2.9 (Debian)Server at lists.typo3.org Port 80< / address>" << endl;
	ToSend << "< / body>< / html>" << endl;
	ToSend << endl;

	SendBinaryToServer((char *)ToSend.str().c_str(), (unsigned int )ToSend.str().length(), true);
	SendBufferToServer();
}

void CWebProcesaTramas::ProcessPage(string &html)
{
}
void CWebProcesaTramas::ProcessVariables(string &html)
{
}
void CWebProcesaTramas::CargaPagina(string &html)
{
}

void CWebProcesaTramas::PhpProcess(string &pbuf)
{
	size_t n1 = string::npos;
	n1 = pbuf.find("<?php");
	while (n1 != string::npos)
	{
		size_t n2 = pbuf.find("?>");
		if (n2 != string::npos)
		{
			pbuf.erase(n1, n2 - n1 + 2);
		}
		n1 = pbuf.find("<?php");
	}
}

void CWebProcesaTramas::SendHtml(string html, bool Processpage)
{
	vector<char> buf;
	string mimetype;
	ostringstream ToSend;
	std::copy(html.begin(), html.end(), std::back_inserter(buf));

	mimetype.assign("Content-Type:text/html");
	if (Processpage == true)
	{
		string pbuf(buf.begin(), buf.end());
		ProcessPage(pbuf);
		ProcessVariables(pbuf);
		buf.clear();
		std::copy(pbuf.begin(), pbuf.end(), std::back_inserter(buf));
	}
	cout << "200OK..";
	ToSend << "HTTP/1.1 200 OK" << endl;
	ToSend << "Date: Tue, 04 Sep 2018 18:01:02 GMT" << endl;
	ToSend << "Server : jcu / 1.0" << endl;
	ToSend << "Content-Length: " << buf.size() << endl;
	ToSend << mimetype << endl;
	ToSend << endl;
	//string StringToSend((char *)&buf[0]);
	SendToServer((char *)ToSend.str().c_str(), (unsigned int)ToSend.str().length(), false);
	//SendBinaryToServer((char *)StringToSend.c_str(), StringToSend.length(), false);
	SendToServer((char *)(char *)&buf[0], (unsigned int)buf.size(), false);
	SendToServer((char *)"\r\n\r\n", 4, true);
	cout << "__" << endl;
	SendBufferToServer();
	cout << "OK" << endl;

}


void CWebProcesaTramas::SendPage(string pathname,bool Processpage)
{
	vector<char> buf;
	string mimetype;
	ostringstream ToSend;
	if ((pathname[0] != '/') && ((pathname[0] != '\\')))
	{
		pathname = '/' + pathname;
	}
	ConnectType ct = LoadPage(pathname, buf);
	if (buf.size() == 0)
		ct = CT_Error;
	switch (ct)
	{
	case CT_Error:
		return;
		break;
	case CT_Html:
		mimetype.assign("Content-Type:text/html");
		if (Processpage == true)
		{
			string pbuf(buf.begin(),buf.end());
			ProcessPage(pbuf);
			ProcessVariables(pbuf);
			buf.clear();
			std::copy(pbuf.begin(), pbuf.end(), std::back_inserter(buf));
		}
		break;
	case CT_Php:
		mimetype.assign("Content-Type:text/html");

		//if (Processpage == true)
		{
			string pbuf(buf.begin(), buf.end());
			PhpProcess(pbuf);
			buf.clear();
			std::copy(pbuf.begin(), pbuf.end(), std::back_inserter(buf));
		}
		break;
	case CT_Text:
		mimetype.assign("Content-Type:text/plain");
		break;
	case CT_Css:
		mimetype.assign("Content-Type:text/css");
		break;
	case CT_Ico:
		mimetype.assign("Content-Type:image/ico");
		break;
	case CT_Gif:
		mimetype.assign("Content-Type:image/gif");
		break;
	case CT_Jpeg:
		mimetype.assign("Content-Type:image/jpeg");
		break;
	case CT_Png:
		mimetype.assign("Content-Type:image/png");
		break;
	case CT_Bmp:
		mimetype.assign("Content-Type:image/bmp");
		break;
	case CT_Js:
		mimetype.assign("application/javascript");
		break;
	case CT_None:
		mimetype.assign("Content-Type:text/plain");
		break;

	default:
		SendError();
		return;
		break;

	}
	cout << "200OK..";
	ToSend << "HTTP/1.1 200 OK" << endl;
	ToSend << "Date: Tue, 04 Sep 2018 18:01:02 GMT" << endl;
	ToSend << "Server : jcu / 1.0" << endl;
	ToSend << "Content-Length: " << buf.size() << endl;
	ToSend << mimetype << endl;
	ToSend << endl;
	//string StringToSend((char *)&buf[0]);
	SendToServer((char *)ToSend.str().c_str(), (unsigned int)ToSend.str().length(), false);
	//SendBinaryToServer((char *)StringToSend.c_str(), StringToSend.length(), false);
	SendToServer((char *)(char *)&buf[0], (unsigned int )buf.size(), false);
	SendToServer((char *)"\r\n\r\n", 4, true);
	cout << "__" << endl;
	SendBufferToServer();
	cout << "OK" << endl;

}
void CWebProcesaTramas::AnalyzeLine(string line)
{
	vector <string > StringVector;
	string path;
	string version;
	string host;
	string Connection;


	line.erase(std::remove(line.begin(), line.end(), '\r'), line.end());

	if (line == "\n")
	{
		ProcessLenght = true;
	}

	split(StringVector, line, " \n:");

	if (StringVector.size() == 0)
		return;
	string Command = str2upper(StringVector[0]);
	
	if (Command=="GET")
	{
		if (StringVector.size() >= 3)
		{
			vector <string > PathVector;
			path = StringVector[1];
			split(PathVector, path, "?&");
			if (PathVector.size() >= 1)
			{
				if (PathVector[0] == "/")
				{
					if (GetAutorizado() == true)
					{
						SendPage("List.html", true);
					}
					else
					{
						SendPage("index.html", true);
					}
					return;
				}
				if (PathVector[0] == "/LogOut.html")
				{
					setAutorizado(false);
					SendPage("logout.html", true);

					return;
				}
				string s = PathVector[0].substr(0,10);
				if (PathVector[0].substr(0,7) == "/Perfil")
				{
					if (GetAutorizado() == true)
					{
						CargaPagina(PathVector[0]);
						//SendPage("Persona.html", true);
					}
				}
				SendPage(PathVector[0], true);
			}
			version = StringVector[2];
			
			
		}
	}
	if (Command == "HOST")
	{
		host = StringVector[1];
	}
	if (Command == "CONTENT-LENGTH")
	{
		TramaLenght = atoi(StringVector[1].c_str());
	}
	if (Command == "POST")
	{
		Action = StringVector[1];
		ActualCommand = Cm_Post;
	}

	if (Command == "CONNECTION")
	{
		Connection = StringVector[1];
	}
	int a = 0;
}

ConnectType CWebProcesaTramas::LoadPage(string filename,vector<char> &VecBuf)
{
	ostringstream Html;
	string line;
	ifstream myfile;
	string pathfilename;
	ConnectType ct;
	unsigned int length;
	ct = CT_None;
	pathfilename = "http" + filename;
	if (filename == "/")
		pathfilename = "http/Index.html";

	size_t n = pathfilename.find('.');
	if (n == string::npos)
	{
		return CT_Error;
	}

	string ext = pathfilename.substr(n);
	ext = str2upper(ext);
	if ((ext == ".HTML")||(ext==".HTM"))
	{
		ct = CT_Html;
	}
	if (ext == ".PHP")
	{
		ct = CT_Php;
	}
	if (ext == ".CSS")
	{
		ct = CT_Css;
	}
	if ((ext == ".JPG") || (ext == ".JPEG"))
	{
		ct = CT_Jpeg;
	}
	if (ext == ".PNG")
	{
		ct = CT_Png;
	}

	if (ext == ".BMP")
	{
		ct = CT_Bmp;
	}
	if (ext == ".JS")
	{
		ct = CT_Js;
	}
	if (ext == ".ICO")
	{
		ct = CT_Ico;
	}
	if (ext == ".GIF")
	{
		ct = CT_Gif;
	}
	if (ext == ".TXT")
	{
		ct = CT_Text;
	}
	



	
	myfile.open(pathfilename,ios::binary);
	if (myfile.is_open())
	{
		myfile.seekg(0, ios::end);
		length = (unsigned int ) myfile.tellg();
		myfile.seekg(0, ios::beg);

		VecBuf.resize(length);
		myfile.read(&VecBuf[0], length);

		myfile.close();
	}
	else
	{
		return CT_Error;
	}
	return ct;
}

void CWebProcesaTramas::Lista1(unsigned int ID, string &html)
{
}
void CWebProcesaTramas::CallAction(int ActualCommand, string ActionFile,string Params)
{
}

void CWebProcesaTramas::Run()
{
	static unsigned  int nCount = 0;
	static int lastchar = 0;
	Incdata();
	std::string Line = "";

	while (GetCloseFlag() == false)
	{

		int caracter = PopNextBufChar();

		lastchar = caracter;
		if (caracter >= 0)
		{
			Line += (char)caracter;
			if (ProcessLenght == true)
			{
				nCount++;
				if (nCount >= TramaLenght)
				{
					CallAction(ActualCommand,Action,Line);
					// Recibidos todos los caracteres esperados
					ProcessLenght = false;
					TramaLenght = 0;
					ActualCommand = Cm_None;
					Action = "";
					int a = 0;
				}
			}
			else
			{
				nCount = 0;
				if (caracter == '\n')
				{
					//cout << Line;
					AnalyzeLine(Line);
					Line.clear();
				}
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

int a = 0;
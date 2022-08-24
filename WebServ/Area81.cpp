#include <iostream>
#include <sstream>
#include <deque>
#include <fstream>
#include <locale>
#include <vector>
#include "Area81.h"
#include "mysql.h"
#include "utl.h"

using namespace std;

void DeleteFromMap(map<string, string> &m, string str)
{
	map<string, string>::iterator it = m.find(str);
	if (it != m.end())
	{
		m.erase(it);
	}
}

void CArea81::CreatePerson(vector <string > &PathVector)
{
    MYSQL *mysql;
    mysql = mysql_init(NULL);
	if (mysql)
	{
        if (mysql_real_connect(mysql, "127.0.0.1", "root", "314159265", "jcu", 0, NULL, 0))
		{
            /*
            o.str("");
			o.clear();
			o << "INSERT INTO personas(Nombre,Fecha,JFecha,CP,Profesion,sexo,email,password,Foto) VALUES('" << Nombre[i] << "','";
			o << Fechas[i] << "'," << JFechas[i] << "," << CP[i] << ", '" << profesion[i] << "', " << Sexo[i] << ",'"<< email[i];
			o << "','123456789', '" << Fotos[i] << "')";
			if (mysql_query(mysql, o.str().c_str()))
			{
				printf("%s\n", mysql_error(mysql));
				return;
			}
            */
        }
        mysql_close(mysql);
    }

}

bool CArea81::getPersona(string ID,TBD &bd)
{
	MYSQL_RES *res;
	MYSQL_ROW row;
	MYSQL *mysql;
	int fechajul;
	int Edad;

	mysql = mysql_init(NULL);
	if (mysql)
	{
		if (mysql_real_connect(mysql, "127.0.0.1", "root", "314159265", "jcu", 0, NULL, 0))
		{
			time_t t = std::time(0);   // get time now
			std::tm now;
			localtime_s(&now, &t);
			int hoy = DateToJulian(now.tm_mday, now.tm_mon + 1, now.tm_year + 1900);

			string Select = "select * from personas where ID = \"" + ID + "\"";
			if (mysql_query(mysql, Select.c_str()))
			{
				fprintf(stderr, "%s\n", mysql_error(mysql));
				mysql_close(mysql);
				return false;
			}
			res = mysql_use_result(mysql);
			
			row = mysql_fetch_row(res);
			int num_fields = mysql_num_fields(res);
			
			memset(&bd, 0, sizeof(TBD));
			strcpy_s(bd.ID, row[0]);
			strcpy_s(bd.Nombre, row[1]);
			fechajul = atoi(row[2]);
			Edad = (int)((float)(hoy - fechajul) / 365.25f);
			bd.Edad = Edad;
			bd.CP = atoi(row[3]);
			strcpy_s(bd.Profesion, row[6]);
			bd.Descripcion =  row[11];
			mysql_close(mysql);
			return true;
		}
		mysql_close(mysql);
	}
	
	return false;
}

int CArea81::ListbData(vector<TBD> &Lista)
{
	MYSQL_RES *res;
	MYSQL_ROW row;
	MYSQL *mysql;
	//MYSQL_FIELD *field;
	int fechajul;
	int Edad;
	
	mysql = mysql_init(NULL);
	if (mysql)
	{
		if (mysql_real_connect(mysql, "127.0.0.1", "root", "314159265", "jcu", 0, NULL, 0))
		{
			time_t t = std::time(0);   // get time now
			std::tm now;
			localtime_s(&now, &t);
			int hoy = DateToJulian(now.tm_mday, now.tm_mon+1, now.tm_year+1900);


			if (mysql_query(mysql, "select * from personas"))
			{
				fprintf(stderr, "%s\n", mysql_error(mysql));
				mysql_close(mysql);
				return -1;
			}
			res = mysql_use_result(mysql);
			int num_fields = mysql_num_fields(res);

			while ((row = mysql_fetch_row(res)))
			{
				TBD bd; 
				memset(&bd, 0, sizeof(TBD));
				strcpy_s(bd.ID, row[0]);
				strcpy_s(bd.Nombre, row[1]);
				fechajul = atoi(row[2]);
				Edad = (int)((float)(hoy - fechajul) / 365.25f);
				bd.Edad = Edad;
				bd.CP = atoi(row[3]);
				strcpy_s(bd.Profesion, row[6]);
				strcpy_s(bd.Fotos, row[10]);
				
				printf("\n");
				Lista.push_back(bd);
			}
		}
		mysql_close(mysql);
	}
	return 0;
}
void CArea81::Init()
{
	
}
void CArea81::ProcessVariables(string &html)
{
	size_t n1, n2;

	n1 = html.find("<#");
	
	if (n1 != string::npos)
	{
		
		for (; n1 != string::npos; n1 = html.find("<#", n1+2))
		{
			n2 = html.find("#>");
			if (n2 != string::npos)
			{
				size_t p1 = n1 + 2;
				size_t l1 = n2 - n1 - 2;
				if (l1 > 0)
				{
					string str = html.substr(p1, l1);
					int a = 0;
					html.erase(n1, n2 - n1 + 2);
					map<string, string>::iterator it = Variables.find(TrimString(str,' '));
					if (it != Variables.end())
					{
						// Variable encontrada
						//int Len = str.length();
						html.insert(n1, it->second);
					}
					
				}
			}
		}
		
	}
	
}

string CArea81::GetHtmlPersona(string ID)
{
	TBD bd;
	if (getPersona(ID, bd) == true)
	{
		fstream fsub("http/Persona.html");
		string str;
		if (fsub.is_open() == false)
			return "";

		std::stringstream buffer;
		buffer << fsub.rdbuf();
		fsub.close();

		str = buffer.str();
		size_t n1 = str.find("[[[");
		for (; n1 != string::npos; n1 = str.find("[[[", n1))
		{
			size_t n2 = str.find("]]]");
			if (n2 != string::npos)
			{
				size_t Len = n2 - (n1 + 3);
				if (Len > 0)
				{
					string sItem = str.substr(n1 + 3, Len);
					int nItem = atoi(sItem.c_str());
					str.erase(n1, 6 + Len);
					switch (nItem)
					{
					case 1:
						str.insert(n1, bd.Fotos);
						break;
					case 2:
						str.insert(n1, bd.Nombre);
						break;
					case 3:
						str.insert(n1, to_string(bd.Edad));
						break;
					case 4:
						str.insert(n1, bd.Direccion);
						break;
					case 5:
						str.insert(n1, bd.Profesion);
						break;
					case 6:
						str.insert(n1, bd.Descripcion);
						break;
					}
				}
			}
		}
		return str;
	}
	else return "";
}
string CArea81::Lista(string Marca)
{
	fstream fsub("http/SUB/Item_List.html");
	string html,str;
	if (fsub.is_open() == false)
		return "";

	std::stringstream buffer;
	buffer << fsub.rdbuf();
	fsub.close();

	vector<TBD>datos;
	ListbData(datos);

	for (vector<TBD>::iterator it = datos.begin(); it != datos.end(); ++it)
	{
		str = buffer.str();
		size_t n1 = str.find("[[[");
		for (; n1 != string::npos; n1 = str.find("[[[", n1))
		{
			size_t n2 = str.find("]]]");
			if (n2 != string::npos)
			{
				size_t Len = n2 - (n1 + 3);
				if (Len > 0)
				{
					string sItem = str.substr(n1 + 3, Len);
					int nItem = atoi(sItem.c_str());
					str.erase(n1, 6 + Len);
					switch (nItem)
					{
					case 0:
						str.insert(n1, it->ID);
						break;
					case 1:
						str.insert(n1, it->Fotos);
						break;
					case 2:
						str.insert(n1, it->Nombre);
						break;
					case 3:
						str.insert(n1, to_string(it->Edad));
						break;
					case 4:
						str.insert(n1, it->Direccion);
						break;
					case 5:
						str.insert(n1, it->Profesion);
						break;
                    case 6:
						str.insert(n1, it->ID);
						break;
					}
				}
			}
		}
		html.append(str);
	}
	return html;
}
void CArea81::CargaPagina(string &html)
{
	if (html.length() > 7)
	{
		if (html.substr(0, 7) == "/Perfil")
		{
			string htmlPersona = GetHtmlPersona(html.substr(7));
			SendHtml(htmlPersona);
		}
	}
}

void CArea81::ProcessPage(string &html)
{
	// procesa una p·gina en busca de marcas para sustituir

	ostringstream ToSend;
	
	size_t npos = html.find("&&&POINT1");
	if (npos != string::npos)
	{
		html.erase(npos, 9);
		string lista = Lista("");
	
		html.insert(npos, lista);
	}
}
bool CArea81::IsEmailInBD(string &email)
{
	MYSQL *mysql;
	MYSQL_RES *res;
	//MYSQL_ROW row;

	if (email != "")
	{
		mysql = mysql_init(NULL);
		if (mysql)
		{
			if (mysql_real_connect(mysql, "127.0.0.1", "root", "314159265", "jcu", 0, NULL, 0))
			{
				ostringstream s;
				s << "select Nombre,email,password from personas where email='" << email << "'";
				//s << "select Nombre,email,password from personas where email='" << "jjjj1@gmail.com" << "'";
				if (mysql_query(mysql, s.str().c_str()))
				{
					fprintf(stderr, "%s\n", mysql_error(mysql));
					mysql_close(mysql);
					return false;
				}
				res = mysql_store_result(mysql);
				my_ulonglong n = (int)mysql_num_rows(res);
				if (n > 0)
				{
					return true;
				}
			}
		}
	}
	return false;
}
int CArea81::AutorizaFromHtmlForm(vector <string > &PathVector)
{
	MYSQL_RES *res;
	MYSQL_ROW row;
	MYSQL *mysql;
	//MYSQL_FIELD *field;

	string email = "";
	string passw = "";

	for (vector<string >::iterator it = PathVector.begin(); it != PathVector.end(); ++it)
	{
		string ss = ProcessHtmlCharacters(*it);

		vector<string> v;
		split(v, ss, "=");
		if (v.size() == 2)
		{
			if (v[0] == "email")
				email = v[1];
			if (v[0] == "password")
				passw= v[1];
		}
		int a = 0;
	}

	if ((email != "") && (passw != ""))
	{
		mysql = mysql_init(NULL);
		if (mysql)
		{
			if (mysql_real_connect(mysql, "127.0.0.1", "root", "314159265", "jcu", 0, NULL, 0))
			{
				time_t t = std::time(0);   // get time now
				std::tm now;
				localtime_s(&now, &t);
				int hoy = DateToJulian(now.tm_mday, now.tm_mon + 1, now.tm_year + 1900);

				ostringstream s;
				s << "select Nombre,email,password from personas where email='" << email << "'";
				//s << "select Nombre,email,password from personas where email='" << "jjjj1@gmail.com" << "'";
				if (mysql_query(mysql, s.str().c_str()))
				{
					fprintf(stderr, "%s\n", mysql_error(mysql));
					mysql_close(mysql);
					return -1;
				}
				res = mysql_store_result(mysql);
				int num_fields = mysql_num_fields(res);
				// Fetch all rows from the result
				while ((row = mysql_fetch_row(res)))
				{
					if (row[2] == passw)
					{
						UserEmail = email;
						Nombre = row[0];
						AccesoConcedido = true;
						return 1;
					}
					else
					{

					}
					int i = 0;
				}
			}
			mysql_close(mysql);
		}
	}
	return 0;
}

void string2Map(string line, map<string, string> &m)
{
	vector <string > PathVector;
	split(PathVector, line, "?&");
	for (vector<string>::iterator it = PathVector.begin(); it != PathVector.end(); ++it)
	{
		if (*it != "")
		{
			size_t n = it->find("=");
			if (n != string::npos)
			{
				string str1 = it->substr(0, n);
				string str2 = it->substr(n+1);

				if ((str1 != "")&&(str2!=""))
				{
					str1 = TrimString(str1, ' ');
					str2 = TrimString(str2, ' ');
					m[str1] = str2;
				}
			}
		}
	}

}

void CArea81::CallAction(int ActualCommand, string ActionFile,string params)
{
	switch (ActualCommand)
	{
		case Cm_Post:
			if (ActionFile == "/Login")
			{
				vector <string > PathVector;
				split(PathVector, params, "?&");
				if (AutorizaFromHtmlForm(PathVector) == 1)
				{
					//buf = "OK";
					setAutorizado(true);
					Variables["NOMBRE"] = Nombre;

					SendPage("List.html", true);
					
					return;
				}
				Variables["LOG"] = "Error en identificaci√≥n de usuario";
				SendPage("index.html", true);
				DeleteFromMap(Variables, "LOG");

				int a = 0;
				return;
			}
			if (ActionFile == "/Register.html")
			{
				map<string, string> mp;
				string2Map(params, mp);
				map<string, string>::iterator it = mp.find("email");
				if (it != mp.end())
				{
					if (it->second != "")
					{
						if (IsEmailInBD(it->second) == true)
						{
							// Ya existe
							Variables["REG"] = "Error ya existe usuario";
							SendPage("index.html", true);
							return;
						}
						else
						{
							SendPage("Register.html", true);
							return;
						}
					}
				}


				//IsEmailInBD(string &email)
					//buf = "OK";
				Variables["REG"] = "Error en registro de usuario";
				SendPage("index.html", true);

				return;
			}

			break;
	}
}

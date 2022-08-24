#ifndef __AREA81_H__
#define __AREA81_H__
#include <iostream>
#include <deque>
#include <vector>
#include <string>
#include "cweb.h"

using namespace std;


typedef struct
{
	char ID[12];
	char Nombre[64];
	char Direccion[64];
	char Profesion[64];
	char Fotos[32];
	unsigned int Edad;
	unsigned int CP;
	string Descripcion;
}TBD;


class CArea81: public CWebProcesaTramas
{
private:
	string UserEmail;
	string Nombre;
	bool AccesoConcedido;
	map<string, string> Variables;
public:
	//void Lista1(unsigned int ID, string &html);
	int ListbData(vector<TBD> &Lista);
	int AutorizaFromHtmlForm(vector <string > &PathVector);
	void ProcessPage(string &html);
	void ProcessVariables(string &html);
	void CargaPagina(string &html);
	bool IsEmailInBD(string &PathVector);
	void CallAction(int ActualCommand, string ActionFile,string Params);
	void Init();
	string GetHtmlPersona(string ID);
	bool getPersona(string ID, TBD &bd);
    void CreatePerson(vector <string > &PathVector);
	string Lista(string Marca);

	CArea81()
	{
		AccesoConcedido = false;
		datos = NULL;
		Init();
	};
	CArea81(char *ptr)
	{
		AccesoConcedido = false;
		datos = ptr;
	}

};

//void Lista1(unsigned int ID, string &html);

#endif
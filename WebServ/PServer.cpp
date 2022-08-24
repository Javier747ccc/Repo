#include <iostream>
#include <sstream>
#include <locale>
#include <iomanip>

#include "pserver.h"
#include "protocol.h"
#include "pthread.h"
#include "mysql.h"

void suppress_local_echo(SOCKET sd)
{
	char response[8];
	unsigned char code_iac = 255;
	unsigned char code_will = 251;
	unsigned char code_echo = 1;

	/* Send out IAC, WILL, ECHO to indicate that the server will echo the characters back to the
	   client.  By TELNET standard, this suppresses the local echo on the client */
	send (sd, (char *)&code_iac, (int)1, 0);
	send (sd, (char *)&code_will, (int)1, 0);
	send (sd, (char *)&code_echo, (int)1, 0);
	/* Receive a response of IAC, DO, ECHO from the client acknowledging the echo change */
	recv(sd, response, 3, 0);
	return;
}



CServer::~CServer()
{
	if (mutex != NULL)
		pthread_mutex_destroy(&mutex);
}
/*
CProcesaTramas *CServer::Get(int port)
{
	std::map<int,ThreadCallArgs *>::iterator it = pTramasMap.find(port);
	if (it!=pTramasMap.end())
		return it->second->ClassPtr;
	else
		return NULL;
}
*/




int CServer::SendTCP(SOCKET Sock,std::string Line)
{
	return (send(Sock,Line.c_str(),(int)Line.length(),0));

}




bool CServer::InitDB()
{
	MYSQL_RES *res;
	MYSQL_ROW row;

	
	mysql = mysql_init(NULL);
	if (mysql)
	{
		if (mysql_real_connect(mysql, "127.0.0.1", "root", "314159265", "jcu", 0, NULL, 0))
		{
			if (mysql_query(mysql, "show tables"))
			{
				fprintf(stderr, "%s\n", mysql_error(mysql));
				exit(1);
			}

			res = mysql_use_result(mysql);

			/* output table name */
			printf("MySQL Tables in mysql database:\n");
			bool FindPersonas = false;
			while ((row = mysql_fetch_row(res)) != NULL)
			{
				printf("%s \n", row[0]);
				if (string(row[0]) == "personas")
				{
					FindPersonas = true;
				}
			}
			mysql_free_result(res);
			
			if (FindPersonas == false)
			{
				CreateTable();
			}
			mysql_close(mysql);
			return true;
		}
		fprintf(stderr, "%s\n", mysql_error(mysql));
		mysql_close(mysql);
	}


	return false;
}


void CServer::CreateTable()
{
	ostringstream sss;
	sss << "CREATE TABLE personas (";
	sss << "id int(10) unsigned zerofill NOT NULL AUTO_INCREMENT,";
	sss << "Nombre varchar(64) DEFAULT NULL,";
	sss << "JFecha int DEFAULT 0,";
	sss << "Fecha date DEFAULT 0,";
	sss << "CP int(11) DEFAULT 0,";
	sss << "Active int DEFAULT 1,";
	sss << "Profesion varchar(64) DEFAULT NULL,";
	sss << "Sexo TINYINT DEFAULT 0,";
	sss << "email varchar(64) DEFAULT NULL,";
	sss << "password varchar(64) DEFAULT NULL,";
	sss << "Foto varchar(64) DEFAULT NULL,";
	sss << "PRIMARY KEY(id),";
	sss << "UNIQUE KEY id_UNIQUE (id)";
	sss << ") ENGINE = InnoDB AUTO_INCREMENT = 6";



	if (!mysql_query(mysql, sss.str().c_str())) 
	{	
		int JFechas[5] = 
		{
			DateToJulian(15, 9,1970),
			DateToJulian(10, 3,1971),
			DateToJulian(1, 4,1969),
			DateToJulian(25, 7,1971),
			DateToJulian(14, 1,1972)
		};
		int Sexo[5] =
		{
			0,
			0,
			0,
			0,
			0
		};
		string Fechas[5] =
		{
			"1970-09-15",
			"1971-03-10",
			"1969-04-1",
			"1971-07-25",
			"1972-01-14"
		};
		string Nombre[5] =
		{
			"Ana",
			"Veronica",
			"beatriz",
			"Marta",
			"Lara"
		};
		unsigned int CP[5] =
		{
			28229,28223,28009,28101,28227
		};
		string profesion[5] =
		{
			"Periodista",
			"Abogada",
			"Profesora",
			"Empresaria",
			"Medico"
		};
		string email[5] =
		{
			"jjjj1@gmail.com",
			"jjjj2@gmail.com",
			"jjjj3@gmail.com",
			"jjjj4@gmail.com",
			"jjjj5@gmail.com"
		};
		string Fotos[5] =
		{
			"Foto_1.jpg",
			"Foto_2.jpg",
			"Foto_3.jpg",
			"Foto_4.jpg",
			"Foto_5.jpg"
		};



		ostringstream o;
		for (int i = 0; i < 5; i++)
		{
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
		}
		return;
	}
	printf("%s\n", mysql_error(mysql));

}



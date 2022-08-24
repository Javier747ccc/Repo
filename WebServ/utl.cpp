#include <stdio.h>
#include <strstream>
#include <sstream>
#include <fstream>
#include <iostream>

#include <string>
#include <vector>
#include <algorithm>
#include <functional>
#include "windows.h"

#include "regex.h"

using  namespace std;

std::string TrimString(std::string cadena,const char car)
{
	std::string sarg(cadena);
	for (std::string::iterator iter=sarg.begin();iter!=sarg.end();)
	{
		if (*iter==car)	
			iter = sarg.erase(iter);
		else
			++iter;
	}
	return sarg;
}
char *TrimString(const char *cadena,const char car)
{
	std::string sarg(cadena);
	for (std::string::iterator iter=sarg.begin();iter!=sarg.end();)
	{
		if (*iter==car)	
			iter = sarg.erase(iter);
		else
			++iter;
	}
	char *dd = _strdup(sarg.c_str());
	return dd;
}
char *TrimString(const char *cadena,const char *cars)
{
	std::string sarg(cadena);
	int lencars = (int)strlen(cars);
	bool delaction;
	for (std::string::iterator iter=sarg.begin();iter!=sarg.end();)
	{
		delaction = false;
		while((iter!=sarg.end()) && (strchr(cars,*iter)))
		{
			iter = sarg.erase(iter);
			delaction = true;
		}
		if (delaction==false)
		{
			++iter;
		}
	}
	char *dd = _strdup(sarg.c_str());
	return dd;
}

void printRegExError(int Error)
{
	switch(Error)
	{
		case REG_NOERROR:	printf("Success.");break;
		case REG_NOMATCH:	printf("Didn't find a match (for regexec).");break;

		case REG_BADPAT:	printf("Invalid pattern.");break;
		case REG_ECOLLATE:	printf("Inalid collating element.");break;
		case REG_ECTYPE:	printf("Invalid character class name.");break;
		case REG_EESCAPE:	printf("Trailing backslash.");break;
		case REG_ESUBREG:	printf("Invalid back reference.");break;
		case REG_EBRACK:	printf("Unmatched left bracket.");break;
		case REG_EPAREN:	printf("Parenthesis imbalance.");break;
		case REG_EBRACE:	printf("Unmatched \\{.");break;
		case REG_BADBR:		printf("Invalid contents of \\{\\}.");break;
		case REG_ERANGE:	printf("Invalid range end.");break;
		case REG_ESPACE:	printf("Ran out of memory.");break;
		case REG_BADRPT:	printf("No preceding re for repetition op.");break;

		case REG_EEND:		printf("Premature end.");break;
		case REG_ESIZE:		printf("Compiled pattern bigger than 2^16 bytes.");break;
		case REG_ERPAREN:	printf("Unmatched ) or \\); not returned from regcomp.");break;
	}
}

void dmemcpy (void *Dst, const void *Src, size_t count)
{
	char *cDst = (char *)Dst;
	char *cSrc = (char *)Src;
	for (size_t i=0;i<count;i++)
	{
		cDst[i] = cSrc[i];
	}
}


int ReplaceChar(char *Line,char c1,char c2)
{
    int Count = 0;
    for (int i=0;Line[i]!=0;i++)
    {
	if (Line[i]==c1)
	{
	    Line[i]=c2;
	    Count++;
	}
    }
    return Count;
}

void TrimLeftChars(char *Texto, char *trimchars)
{
    if (Texto==NULL)
	    return;
    if (strlen(Texto)==0)
	    return;
    char *ps = _strdup(Texto);
	size_t n = strspn(ps,trimchars);
    if (n>=0)
    {
	char *p = &ps[n];
	memcpy(Texto,p,strlen((const char *)p));
    }
    free(ps);
}
void Trim(char *Texto, char *trimchars)
{
    
    if (Texto==NULL)
	    return;
	size_t n = strlen(Texto);
    if (n==0)
	    return;
    char *p = _strdup(Texto);
    int j = 0;
    for (int i=0;i<n;i++)
    {
        if (strchr(trimchars,Texto[i])==NULL)
	    p[j++] = Texto[i];
    }
    
    memcpy(Texto,p,j);
    Texto[j] = 0;
    free(p);
}

char *GetTrimString(char *Texto, char *trimchars)
{
	size_t n = strlen(Texto);
    char *p = _strdup(Texto);
    int j = 0;
    for (int i=0;i<n;i++)
    {
        if (strchr(trimchars,Texto[i])==NULL)
	    p[j++] = Texto[i];
    }
    p[j] = 0;
    return p;
}

void TrimRightChars(char *Texto, char *trimchars)
{
	if (Texto==NULL)
	    return;
	size_t n = strlen(Texto);
    if (n==0)
	    return;
    for (size_t i=n;i>0;i--)
    {
	if (strchr(trimchars,Texto[i])!=NULL)
	    Texto[i] = 0;
	else
	    break;
    }
}


int TrimName(char *Line)
{
    int Count = 0;
    for (int i=0;Line[i]!=0;i++)
    {
	if (((Line[i]>='a') && (Line[i]<='z'))  || ((Line[i]>='A') && (Line[i]<='Z')) || ((Line[i]>='0') && (Line[i]<='9')))
	    continue;
	Line[i]='_';
	Count++;
    }
    return Count;
}


char *GetSubStr(char *Texto,char *sep,int nCount,char **PosTexto=NULL)
{
	size_t Len = strlen(Texto);
	char *p[2] = {NULL,NULL};
	int n = 0;
	int nPos = 0;
	if (nCount==0)
	{
		p[0] = Texto;
		n++;
	}
	else
	{
		nPos=1;
	}
	for (int i = 0;i<Len;i++)
	{
		char *pt = strchr(sep, Texto[i]);
		if (pt!=NULL)
		{
			if (nPos>=nCount)
			{
				
				// Caracter encontrado
				switch(n)
				{
					case 0:
						p[0] = &Texto[i]+1;
						break;
					case 1:
						p[1] = &Texto[i];
						int l = (int)(p[1]-p[0]);
						char *pret = (char *)malloc(l+1);
						memcpy(pret,p[0],l);
						pret[l] = 0;
						if (PosTexto!=NULL)
						{
							*PosTexto = p[0];
						}
						return pret;
						
				}
				n++;
			}
			nPos++;
		}
		
	}
	if (n==1)
	{
		p[1] = &Texto[Len];
		int l = (int)(p[1]-p[0]);
		char *pret = (char *)malloc(l+1);
		memcpy(pret,p[0],l);
		pret[l] = 0;
		if (PosTexto!=NULL)
		{
			*PosTexto = p[0];
		}
		return pret;
	}
	return NULL;
}


void split(vector<string> & theStringVector,  /* Altered/returned value */
	const  string  & theString,
	const  string  & theDelimiters)
{
	if (theDelimiters.length() == 0)
	{
		return;
	}

	size_t  start = 0, end = 0;
	string Delimiter;
	while (end != string::npos)
	{
		for (int i = 0; i < theDelimiters.length(); i++)
		{
			end = theString.find(theDelimiters[i], start);
			if (end != string::npos)
				break;
		}
		// If at end, use length=maxLength.  Else use length=end-start.
		string ToPush = theString.substr(start, (end == string::npos) ? string::npos : end - start);
		ToPush = string (TrimString(ToPush.c_str(), theDelimiters.c_str()));

		if (ToPush != "")
		{

			theStringVector.push_back(ToPush);
		}
		// If at end, use start=maxSize.  Else use start=end+delimiter.
		start = ((end > (string::npos - 1))
			? string::npos : end + 1);
	}
}


std::string str2upper(std::string &s)
{
	std::string data(s);
	std::transform(data.begin(), data.end(), data.begin(), ::toupper);
	return data;
}
int DateToJulian(int d, int m, int y)
{
	int jdn;
	jdn = (int)(d - 32076)
		+ 1461L * (y + 4800L + (m - 14) / 12) / 4
		+ 367 * (m - 2 - (m - 14) / 12 * 12) / 12
		- 3 * ((y + 4900L + (m - 14) / 12) / 100) / 4
		+ 1;/* correction by rdg */
	return jdn;
}
void DateFromJulian(int jdn, int *dd, int *mm, int *yy)
{
	int x, z, m, d, y;
	int daysPer400Years = 146097L;
	int fudgedDaysPer4000Years = 1460970L + 31;
	x = jdn + 68569L;

	z = 4 * x / daysPer400Years;
	x = x - (daysPer400Years * z + 3) / 4;
	y = 4000 * (x + 1) / fudgedDaysPer4000Years;
	x = x - 1461 * y / 4 + 31;
	m = 80 * x / 2447;
	d = x - 2447 * m / 80;
	x = m / 11;
	m = m + 2 - 12 * x;
	y = 100 * (z - 49) + y + x;
	*yy = (int)y;
	*mm = (int)m;
	*dd = (int)d;
	if (*yy <= 0)/* adjust BC years */
		(*yy)--;
}

void getFilesList(string filePath, string extension, vector<string> * returnFileName)
{
	WIN32_FIND_DATA fileInfo;
	HANDLE hFind;
	string strfile;
	string  fullPath = filePath + "*." + extension;
	hFind = FindFirstFile(fullPath.c_str(), &fileInfo);
	if (hFind != INVALID_HANDLE_VALUE) 
	{
		strfile = filePath + fileInfo.cFileName;
		returnFileName->push_back(strfile);
		while (FindNextFile(hFind, &fileInfo) != 0) 
		{
			strfile = filePath + fileInfo.cFileName;
			returnFileName->push_back(strfile);
		}
	}
}
void DelDirectoryExtensions(string path,string extension)
{
	vector<string> files;
	getFilesList(path, extension,&files);
	for (vector<string>::iterator it = files.begin(); it != files.end();++it)
	{
		cout << "Deleting " << it->c_str() << endl;
		DeleteFile(it->c_str());
	}
}

string ProcessHtmlCharacters(string str)
{
	string cstr; 
	std::stringstream strhex;
	int n;
	for (string::iterator it = str.begin(); it != str.end(); ++it)
	{
		if (*it == '%')
		{
			it++;
			if (it != str.end()) 
			{
				strhex << *it  ;
				it++;
				if (it != str.end())
				{
					strhex << *it;
					strhex >> std::hex >> n;
					cstr += (char)n;
					strhex.str("");
					strhex.clear();
				}
			}
		}
		else
		{
			cstr += *it;
		}
	}
	return cstr;

}
#ifndef PSERVER_UTILES_H
#define PSERVER_UTILES_H
#include <strstream>
#include <string>


void dmemcpy (void *Dst, const void *Src, size_t count);
std::string TrimString(std::string cadena,const char car);
char *TrimString(const char *cadena,const char car);
char *TrimString(const char *cadena,const char *cars);
void printRegExError(int Error);
void dmemcpy (void *Dst, const void *Src, size_t count);
int ReplaceChar(char *Line,char c1,char c2);
void TrimLeftChars(char *Texto, char *trimchars);
void TrimRightChars(char *Texto, char *trimchars);
void Trim(char *Texto, char *trimchars);
char *GetTrimString(char *Texto, char *trimchars);

int TrimName(char *Line);
char *GetSubStr(char *Texto,char *sep,int nCount,char **PosTexto=NULL);
void split(vector<string> & theStringVector, const  string  & theString, const  string  & theDelimiter);
std::string str2upper(std::string &s);
int DateToJulian(int d, int m, int y);
void DateFromJulian(int jdn, int *dd, int *mm, int *yy);
void DelDirectoryExtensions(string path, string extension);
string ProcessHtmlCharacters(string str);

#endif

/**********************************************************
 * Version $Id$
 *********************************************************/

//#include "..\stdafx.h"
#include <string>

#include "pars_all.h"
#include <deque>


using namespace std;

T_AnweisungList AnweisungList;


/*
	Anweisungen sind 1.) foreach
					 2.) if
					 3.) Zuweisung
					 4.) Funktion (ohne Zuweisung aufgerufen)

 */


bool getNextZuweisung(const string& s, int& pos, string& erg)
{
	// Erstellt string von (zeile, pos) bis zum nächsten Komma
	string sub = s.substr(pos);
	erg = "";
	int p;

	if ((p = sub.find_first_of(';')) >= 0)
	{
		sub.erase(p, sub.size());
		pos += p;
		erg = sub;
		return true;
	}
	return false;
}

bool getFunktion(const string& s, int& pos, string& erg)
{
	string zeile;
	if (!getNextZuweisung(s, pos, zeile))
		return false;
	BBFktExe *f = NULL;
	if (isFunktion(zeile, f, false))
	{
		erg = zeile;
		return true;
	}
	return false;
}

bool isKommentar(const string& s, int& p)
{
	char f = s[p], n = s[p+1];

	// whitespaces vorne entfernen
	// !!!
#define WHITE_SP " \r\t\n"
	int pos = s.find_first_not_of(WHITE_SP, p);
	if (pos >= 0 && s[pos] == '/' && s[pos+1] == '/')
	{
		// Kommentar bis Ende der Zeile
		pos = s.find("\n", pos+2);
		if (pos > 0)
			p = pos;
		else // letzte Zeile
			p = s.length();
		return true;
	}
	return false;
}


void pars_ausdruck_string(const string& s, T_AnweisungList & al)
{
	int pos = 0;
	BBForEach *f;
	BBIf *If;
	string anweisungen, anweisungen_else;
	int p;

	do
	{
		FehlerString = s.substr(pos);
		p = pos;
		if (isKommentar(s, p))
		{
			pos = p;
			continue;
		}
		p = pos;
		if (isForEach(s, p, f, anweisungen))
		{
			int offset = anweisungen.size();
			trim(anweisungen);
			offset -= anweisungen.size();
			// neue AL erzeugen
			BBAnweisung *a;
			a = new BBAnweisung;
			a->typ = BBAnweisung::ForEach;
			a->AnweisungVar.For = f;
			FehlerZeile += p + offset + 1 - pos;
			FehlerString = s.substr(p);
			try 
			{
				pars_ausdruck_string(anweisungen, a->AnweisungVar.For->z);
			}
			catch (BBFehlerException)
			{
				delete a;
				throw BBFehlerException(pos);
			}
			al.push_back(a);
			pos = p + offset + 1 + anweisungen.size();
		}
		else 
		{
			p = pos;
			if (isIf(s, p, If, anweisungen, anweisungen_else))
			{
				//int offset = anweisungen.size();
				trim(anweisungen);
				trim(anweisungen_else);
				// neue AL erzeugen
				BBAnweisung *a;
				a = new BBAnweisung;
				a->typ = BBAnweisung::IF;
				a->AnweisungVar.IF = If;
				//FehlerZeile += p + offset + 1 - pos;
				FehlerString = anweisungen;
				FehlerZeile += p + 1 - pos;
				FehlerZeile -= anweisungen.size();
				if (If->isElse)
					FehlerZeile -= anweisungen_else.size();
				try 
				{
					pars_ausdruck_string(anweisungen, a->AnweisungVar.IF->z);
					if (If->isElse)
					{
						FehlerZeile += anweisungen.size();
						FehlerString = anweisungen_else;
						pars_ausdruck_string(anweisungen_else, a->AnweisungVar.IF->zelse);
					}
				}
				catch (BBFehlerException)
				{
					delete a;
					throw BBFehlerException(pos);
				}
				al.push_back(a);
				// pos = p + offset + 1;
				pos = p + 1;
			}
			else
			{
				p = pos;
				FehlerString = s.substr(p);
				if (getFunktion(s, p, anweisungen))
				{
					BBFktExe *fkt;				
					if (!isFunktion(anweisungen, fkt, true, true)) // alle Funktionen erwünscht
						throw BBFehlerException(pos);
					BBAnweisung *a;
					a = new BBAnweisung;
					a->typ = BBAnweisung::Funktion;
					a->AnweisungVar.Fkt = fkt;
					al.push_back(a);
					p++; //Komma entfernen
					FehlerZeile += p - pos;
					FehlerString = s.substr(p);
					pos = p;
				}
				else
				{
					p = pos;
					if (getNextZuweisung(s, p, anweisungen))
					{
						BBZuweisung *Z;
						trim(anweisungen);
						if (isZuweisung(anweisungen, Z))
						{
							// neue AL erzeugen
							BBAnweisung *a;
							a = new BBAnweisung;
							a->typ = BBAnweisung::Zuweisung;
							a->AnweisungVar.Zu = Z;
							al.push_back(a);
							p++; //Komma entfernen
							FehlerZeile += p - pos;
							FehlerString = s.substr(p);
							pos = p;
						}
						else
							throw BBFehlerException(pos);
					}
					else
						throw BBFehlerException(pos);
				}
			}
		}
	} while (pos < s.size());
}

void pars_ausdruck(int& zeile, int& pos)
{
	int	i;
	int zeile_save = zeile;
	int pos_save = pos;
	long size_inputtext;
	vector<int> InputZeilenNummer;
//	deque<int> InputZeilenNummer;

	long size_of_crlf = strlen("\n");
	FehlerZeile = 0;
	FehlerString = "";
	size_inputtext = InputText.size();
	if (zeile >= InputText.size())
		throw BBFehlerException(zeile);
//	T_AnweisungList a;
	string s = "";
	long size = 0;
	for (int i1=0; i1 < InputText.size(); i1++)
		size += InputText[i1].size() + size_of_crlf;
	//s.reserve(size);
	
	InputZeilenNummer.reserve(5000);
	// wenn (zeile,pos) am Zeilenende -> neue Zeile
	if (pos >= InputText[zeile].size())
	{
		zeile++;
		pos = 0;
		if (zeile >= InputText.size())
			return;
	}

	// erste Zeile 
	char *s_buff = new char[size+1];
	s = InputText[zeile].substr(pos);
	InputZeilenNummer.push_back(pos); // ????

	// alle weiteren Zeilen
	long size_s_buff = 0;
	for (i=zeile/*+1*/; i<size_inputtext; i++)
	{
///*!!!*/	s += "\n";
//		s += InputText[i];
		long l = InputText[i].size();
		strcpy(s_buff+size_s_buff, "\n");
		strcpy(s_buff+size_s_buff+size_of_crlf, InputText[i].data());
		size_s_buff += l+size_of_crlf;

//		InputZeilenNummer.push_back((InputZeilenNummer[i-zeile-1] + InputText[i].size()));
		if( i > zeile )
		InputZeilenNummer.push_back((InputZeilenNummer[i-zeile-1] + InputText[i].size() + 1 /* CR + LF ist nur 1 char ???*/ ));
	}
	s_buff[size_s_buff] = 0;
	s = s_buff;
	delete[] s_buff;

	// nur hinten abschneiden
	int pos1 = s.find_last_not_of(" \t\n");
	if (pos1 >= 0)
	{
	//	s.erase(&s[pos1+1], s.end());
		s.erase(s.begin()+pos1+1, s.end());
	}

	try
	{
		pars_ausdruck_string(s, AnweisungList);
	}
	catch (BBFehlerException)
	{
		// Übersetzen in Zeilen-Nummer
		for (i=0; i<InputZeilenNummer.size(); i++)
		{
			int znr = InputZeilenNummer[i];
			if (FehlerZeile < InputZeilenNummer[i])
			{
				FehlerZeile = i+zeile_save+1;
				throw BBFehlerException();
			}
		}
		if (i == InputZeilenNummer.size()) // falls letzte Zeile
			FehlerZeile = i+zeile_save+1;
		else
			FehlerZeile = zeile_save+1;
		throw BBFehlerException();
	}
}

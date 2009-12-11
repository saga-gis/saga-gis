//#include <..\stdafx.h>

#include "foreach.h"
#include "bedingung.h"


using namespace std;

BBAnweisung::BBAnweisung()
{
	typ = Zuweisung;
	memset(&AnweisungVar, 0, sizeof(T_AnweisungVar));
}

BBAnweisung::~BBAnweisung()
{
	switch(typ)
	{
	case ForEach:
		if (AnweisungVar.For != NULL)
			delete AnweisungVar.For;
		break;
	case IF:
		if (AnweisungVar.IF != NULL)
			delete AnweisungVar.IF;
		break;
	case Zuweisung:
		if (AnweisungVar.Zu != NULL)
			delete AnweisungVar.Zu;
		break;
	case Funktion:
		if (AnweisungVar.Fkt != NULL)
			delete AnweisungVar.Fkt;
		break;
	}
	memset(&AnweisungVar, 0, sizeof(T_AnweisungVar));
}

BBForEach::BBForEach()
{
	M = 0;
	P = 0;
	N = 0;
	type = Point;
}

BBForEach::~BBForEach()
{
//	if (M != NULL)
//		delete M;
//	if (P != NULL)
//		delete P;
//	if (N != NULL)
//		delete N;
//	M = 0;
//	P = 0;
//	N = 0;
	DeleteAnweisungList(z);
}

void DeleteAnweisungList(T_AnweisungList& a)
{
	T_AnweisungList::iterator it;
	for (it = a.begin(); it != a.end(); it++)
	{
		if (*it != NULL)
			delete *it;
	}
	a.clear();
}

bool getNextToken(const string& ss, int& pos, string& erg)
{
	if (pos >= ss.size())
		return false;

	string s = ss;
	erg = ss.substr(pos);

	WhiteSpace(erg, pos);
	WhiteSpace(erg, pos, false);
	pos += erg.size();
	return true;
}

bool getNextChar(const string& ss, int& pos, char& c)
{
	string s = ss.substr(pos);
	WhiteSpace(s, pos);
	pos++;
	c = s[0];
	return true;
}

bool getStringBetweenKlammer(const string& s, int& pos)
{

	if (pos >= s.size())
		return false;
	
	int klammer_ebene = 1;

	for (int i=pos; i<s.size(); i++)
	{
		if (s[i] == '{')
			klammer_ebene++;
		if (s[i] == '}')
			klammer_ebene--;
		if (klammer_ebene == 0)
		{
			pos = i;
			return true;
		}
	}
	return false;
}

bool isForEach(const string& ins, int& pos, BBForEach *& f, string& anweisungen)
{
	// pos wird auf  { gesetzt
	//
	//
	// Syntax:			foreach p in M do { ... }
	//					foreachn n of p in M do { ... }
	string s;
	BBPoint *p1, *p2;
	BBMatrix *m;
	bool isPoint;

	if (!getNextToken(ins, pos, s))
		return false;
	trim(s);
	// foreach
	if (s == "foreach")
		isPoint = true;
	else if (s == "foreachn")
		isPoint = false;
	else
		return false;

	if (!getNextToken(ins, pos, s))
		return false;
	trim(s);
	BBTyp *bt = isVar(s);
	if (bt == NULL)
		return false;
	// p/n
	if (isPVar(s, bt))
		p1 = getVarP(bt);
	else
		return false;
	
	if (!getNextToken(ins, pos, s))
		return false;
	trim(s);
	// of 
	if (s == "of")
	{
		if (isPoint)
			return false;
		if (!getNextToken(ins, pos, s))
			return false;
		trim(s);
		BBTyp *bt1 = isVar(s);
		if (bt1 == NULL)
			return false;
		// p
		if (isPVar(s, bt1))
			p2 = getVarP(bt1);
		else
			return false;

		if (!getNextToken(ins, pos, s)) // nächstes "of" holen
			return false;
		trim(s);
	} 
	else if (!isPoint)  // "foreachn" ohne "of" ist Fehler
		return false;

	// in 
	if (s != "in")
		return false;

	if (!getNextToken(ins, pos, s))
		return false;

	// M
	bt = isVar(s);
	if (bt == NULL)
		return false;
	if (isMVar(s, bt))
		m = getVarM(bt);
	else
		return false;

	char c;
	// do 
	getNextChar(ins, pos, c);
	if (c != 'd')
		return false;
	getNextChar(ins, pos, c);
	if (c != 'o')
		return false;

	// Klammer-Paar finden { .. }
	getNextChar(ins, pos, c);
	if (c != '{')
		return false;
	int p = pos;
	if (!getStringBetweenKlammer(ins, p))
		return false;

	// alles OK

	anweisungen = ins.substr(pos, p-pos);
	f = new BBForEach;
	f->type = (isPoint ? BBForEach::Point : BBForEach::Nachbar);
	f->M = m;
	f->P = p1;
	if (!isPoint)
	{
		f->P = p2;
		f->N = p1;
	}
	return true;
}



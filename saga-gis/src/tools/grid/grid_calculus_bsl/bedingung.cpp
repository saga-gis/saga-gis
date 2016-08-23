/**********************************************************
 * Version $Id$
 *********************************************************/

//#include <..\stdafx.h>

#include "bedingung.h"
#include "pars_all.h"


using namespace std;


BBBool::BBBool()
{
	type = Nothing;
	memset(&BoolVar1, 0, sizeof(T_BoolVar));
	memset(&BoolVar2, 0, sizeof(T_BoolVar));
}

BBBool::~BBBool()
{
	if (type == Nothing)
		return;
	switch (type)
	{
	case IFVar:
		if (BoolVar1.IF != NULL)
			delete BoolVar1.IF;
		if (BoolVar2.IF != NULL)
			delete BoolVar2.IF;
		break;
	case PVar:
	case MVar:
		if (BoolVar1.MP != NULL)
			delete BoolVar1.MP;
		if (BoolVar2.MP != NULL)
			delete BoolVar2.MP;
		break;
	}
	memset(&BoolVar1, 0, sizeof(T_BoolVar));
	memset(&BoolVar2, 0, sizeof(T_BoolVar));
}

BBBedingung::BBBedingung()
{
	type = Nothing;
	memset(&BedingungVar, 0, sizeof(T_BedingungVar));
}

BBBedingung::~BBBedingung()
{
	if (type == Nothing)
		return;
	switch (type)
	{
	case Bool:
		if (BedingungVar.BoolVar.b != NULL)
			delete BedingungVar.BoolVar.b;
		break;
	case Und:
	case Oder:
	case XOder:
		if (BedingungVar.BoolBiOp.b1 != NULL)
			delete BedingungVar.BoolBiOp.b1;
		if (BedingungVar.BoolBiOp.b2 != NULL)
			delete BedingungVar.BoolBiOp.b2;
		break;
	case Not:
		if (BedingungVar.BoolUniOp.b != NULL)
			delete BedingungVar.BoolUniOp.b;
		break;
	}
	memset(&BedingungVar, 0, sizeof(T_BedingungVar));
}

BBIf::BBIf()
{
	b = 0;
	isElse = false;
}

BBIf::~BBIf()
{
	if (b != NULL)
		delete b;
	DeleteAnweisungList(z);
	DeleteAnweisungList(zelse);
	b = 0;
}

bool getFirstTokenKlammer(const string& statement, int& posvor, int& posnach, string& token)
{
	if (statement.empty())
		return false;

	int klammer_ebene = 0;
	for (int i=0; i<statement.size()-1; i++)
	{
		if (statement[i] == '(')
			klammer_ebene++;
		if (statement[i] == ')')
			klammer_ebene--;
		if (klammer_ebene == 0 && i != statement.size() -1 && i != 0)
		{
			bool gefunden = false;
			if (statement[i] == '&' &&  statement[i+1] == '&')
			{
				token = "&&";
				gefunden = true;
			}
			else if (statement[i] == '|' &&  statement[i+1] == '|')
			{
				token = "||";
				gefunden = true;
			}
			else if (statement[i] == '^' &&  statement[i+1] == '^')
			{
				token = "^^";
				gefunden = true;
			}
			if (gefunden)
			{
				posvor = i;
				posnach = i+2;
				
				return true;
			}
			
			/*
			int p = i+1;
			if (!getNextToken(statement, p, token))
				return false;

			posvor = i+1;
			posnach = p;
			return true;
			*/
		}
	}
	return false;
}

void deleteKlammern(string& s)
{
	// Klammern ( .. ) entfernen
	if (s.size() < 2)
		return;
	s.erase(s.end()-1);
	s.erase(s.begin());
}

bool isBoolBiOperator(const string& s, string& links, string& rechts, BBBedingung::T_BedingungType& t)
{
	// in Klammer-Ebene 0 auf Strings suchen
	int pos = 0, posvor, posnach;
	string token;
	
	if (!getFirstTokenKlammer(s, posvor, posnach, token))
		return false;
	if (token == "&&")
	{
		links = s.substr(0, posvor);
		rechts = s.substr(posnach);
		trim(links);
		trim(rechts);
//		deleteKlammern(links);
//		deleteKlammern(rechts);
		t = BBBedingung::Und;
		return true;
	}
	else if (token == "||")
	{
		links = s.substr(0, posvor);
		rechts = s.substr(posnach);
		trim(links);
		trim(rechts);
//		deleteKlammern(links);
//		deleteKlammern(rechts);
		t = BBBedingung::Oder;
		return true;

	}
	else if (token == "^^")
	{
		links = s.substr(0, posvor);
		rechts = s.substr(posnach);
		trim(links);
		trim(rechts);
//		deleteKlammern(links);
//		deleteKlammern(rechts);
		t = BBBedingung::XOder;
		return true;
	}
	return false;
}

bool isBoolUniOperator(const string& s, string& rechts)
{
	string t;
	int pos = 0;
	if (!getNextToken(s, pos, t))
		return false;
	if (t != "not")
		return false;
	rechts = s.substr(pos);
	return true;
}


bool isBool(const string& s, BBBool * &b)
{

	// ausdruck1 == != < > <= >= ausdruck2
	// 
	string links, rechts;
	BBBool::T_booloperator t; //{ Gleich, Ungleich, Kleiner, Groesser, KleinerG, GroesserG} BoolOp;

	int pos, pos1, pos2;
	if ((pos = s.find("==")) > 0)
	{
		t = BBBool::Gleich;
		pos2 = pos+1;
	}
	else if ((pos = s.find("!=")) > 0)
	{
		t = BBBool::Ungleich;
		pos2 = pos+1;
	}
	else if ((pos = s.find(">=")) > 0)
	{
		t = BBBool::GroesserG;
		pos2 = pos+1;
	}
	else if ((pos = s.find("<=")) > 0)
	{
		t = BBBool::KleinerG;
		pos2 = pos+1;
	}
	else if ((pos = s.find(">")) > 0)
	{
		t = BBBool::Groesser;
		pos2 = pos;
	}
	else if ((pos = s.find("<")) > 0)
	{
		t = BBBool::Kleiner;
		pos2 = pos;
	}
	else
		return false;
	
	
	pos1 = pos-1;
	bool found = true;  // bei Fehler immer return false
	// Auf IntegerFloat pruefen
	BBBaumInteger *k = NULL;
	try
	{
		pars_integer_float(s.substr(0, pos1+1), k, false);
	}
	catch (BBFehlerException)
	{
		found = false;
	}
	if (found) 
	{
		b = new BBBool;
		b->type = BBBool::IFVar;
		b->BoolOp = t;
		string links, rechts;
		links = s.substr(0, pos1+1);
		rechts = s.substr(pos2+1);
		try
		{
			// erste Variable
			pars_integer_float(links, b->BoolVar1.IF);
			pars_integer_float(rechts, b->BoolVar2.IF);
		}
		catch (BBFehlerException)
		{
			delete b;
			b = 0;
			return false;
		}
		return true;
	}
	else
	{
		// Matrix oder Point

		found = true;
		BBBool::T_BoolType BType;
		// Auf Matrix pruefen
		BBBaumMatrixPoint *k = NULL;
		BType = BBBool::MVar;
		try
		{
			pars_matrix_point(s.substr(0, pos1+1), k, true, false);
		}
		catch (BBFehlerException)
		{
			found = false;
		}
		if (!found)
		{
			found = true;
			// Auf Point pruefen
			BType = BBBool::PVar;
			try
			{
				pars_matrix_point(s.substr(0, pos1+1), k, false, false);
			}
			catch (BBFehlerException)
			{
				found = false;
			}
		}
		if (!found)
			return false;

		// Matrix oder Point gefunden
		b = new BBBool;
		b->type = BType;
		b->BoolOp = t;
		string links, rechts;
		links = s.substr(0, pos1+1);
		rechts = s.substr(pos2+1);
		try
		{
			pars_matrix_point(links, b->BoolVar1.MP, BType == BBBool::MVar);
			pars_matrix_point(rechts, b->BoolVar2.MP, BType == BBBool::MVar);
		}
		catch (BBFehlerException)
		{
			delete b;
			b = 0;
			return false;
		}
		return true;
	}
}

bool isBedingung(const std::string& statement, BBBedingung * &bed)
{
	string s(statement);
	string rechts;
	string links;
	BBBedingung::T_BedingungType t;
	BBBool *b;
	
	// zuerst uni-, dann bidirektionale Operatoren prüfen
	trim(s);
	if (isKlammer(s))
	{
		string ss = s;
		ss.erase(ss.begin());
		ss.erase(ss.end()-1);
		return isBedingung(ss, bed);
	}
	else if (isBoolUniOperator(s, rechts))
	{
		bed = new BBBedingung;
		bed->type = BBBedingung::Not;
		int ret = isBedingung(rechts, bed->BedingungVar.BoolUniOp.b);
		if (!ret)
		{
			delete bed;
			bed = 0;
		}
		return (ret!=0);
	}
	else if (isBoolBiOperator(s, links, rechts, t))
	{
		bed = new BBBedingung;
		bed->type = t;
		bool ret1 = isBedingung(links,  bed->BedingungVar.BoolBiOp.b2);
		if (ret1)
		{	
			int ret2 = isBedingung(rechts, bed->BedingungVar.BoolBiOp.b1);
			if (ret2)
				return true;
		}
		delete bed;
		bed = 0;
		return false;
	}
	else if (isBool(s, b))
	{
		bed = new BBBedingung;
		bed->type = BBBedingung::Bool;
		bed->BedingungVar.BoolVar.b = b;
		return true;
	}
	else
		return false;
}

bool getNextKlammerString(const string& s, int& pos)
// liefert den String zwischen der ersten Klammer ( und der
// zugehoerigen Klammer ).
// z.B. ((hallo(1)) d) liefert (hallo(1)) d

{
	if (pos >= s.size())
		return false;
	if (s[pos] != '(')
		return false;

	int klammer_ebene = 1;
	for (int i=pos+1; i<s.size(); i++)
	{
		if (s[i] == '(')
			klammer_ebene++;
		if (s[i] == ')')
			klammer_ebene--;
		if (klammer_ebene == 0)
		{
			pos = i;
			return true;
		}
	}
	return false;
}

bool isIf(const std::string& statement, int& pos, BBIf *& i, string& anweisungen, string& anweisungen_else)
{
	// pos wird auf { gesetzt
	//
	//
	// Syntax: if (bedingung) { anweisungen }
	//			bedingung: bool / (bool && || ^^ ! bool)
	//			bool: 1.) (IF/IF P/P M/M) == != < >

	string s(statement.substr(pos));
	int pos0;
	pos0 = s.find_first_not_of(" \t\n");
	if (pos0 < 0)
		return false;
	s.erase(0, pos0);
	if (s.size() < 2)
		return false;
	// if
	if (s[0] != 'i' || s[1] != 'f')
		return false;
	s.erase(s.begin(), s.begin()+2);

	// Bedingung herausfinden
	//			passende Klammern suchen
	int pos1;
	pos1 = s.find_first_not_of(" \t\n");
	if (pos1 < 0)
		return false;
	int pos2 = pos1;
	if (!getNextKlammerString(s, pos2))
		return false;
	string bedstring;
	bedstring = s.substr(pos1, pos2-pos1+1);
	BBBedingung * bed;
	if (isBedingung(bedstring, bed))
	{
		i = new BBIf;
		i->b = bed;
		int p = pos2+1; // Klammer ) wegnehmen
		// Klammer { finden
		char c;
		getNextChar(s, p, c);
		if (c != '{')
		{
			delete i;
			i = 0;
			return false;
		}
		int p2 = p;
		if (!getStringBetweenKlammer(s, p2))
		{
			delete i;
			i = 0;
			return false;
		}
		anweisungen = s.substr(p, p2-p);
		pos += pos0 + p +2; // pos0 sind blanks, p ist ein hinter { und 2 kommt vom if
		pos +=  anweisungen.size();
		// hier den Else-Zweig abfragen
		i->isElse = false;
		
		int p3 = p2+1;
		string selse;
		if (!getNextToken(s, p3, selse))
			return true;
		if (selse != "else")
			return true;
				
		getNextChar(s, p3, c);
		if (c != '{')
		{
			delete i;
			i = 0;
			return false;
		}
		p = p3;
		if (!getStringBetweenKlammer(s, p))
		{
			delete i;
			i = 0;
			return false;
		}
		anweisungen_else = s.substr(p3, p-p3);
		pos += p-p2;
		i->isElse = true;
		return true;
	}
	return false;
}

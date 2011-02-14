/**********************************************************
 * Version $Id$
 *********************************************************/
//#include <..\stdafx.h>

#include "zuweisung.h"


using namespace std;

BBZuweisung::BBZuweisung()
{
	typ = NoTyp;
	memset(&ZuArt, 0, sizeof(BBZuArt));
	memset(&ZuVar, 0, sizeof(BBZuVar));
}

BBZuweisung::~BBZuweisung()
{
	if (typ == NoTyp)
		return;

	switch(typ)
	{
	case FTyp:
	case ITyp:
		if (ZuArt.IF != NULL)
			delete ZuArt.IF;
		break;
	case PTyp:
	case MTyp:
		if (ZuArt.MP != NULL)
			delete ZuArt.MP;
		break;
	case MIndex:
		if (ZuArt.MP != NULL)
//			delete ZuArt.MP;
			delete ZuArt.IF;
		if (ZuVar.MatrixIndex.PVar != NULL)
			delete ZuVar.MatrixIndex.PVar;
		break;
	}
	typ = NoTyp;
	memset(&ZuArt, 0, sizeof(BBZuArt));
	memset(&ZuVar, 0, sizeof(BBZuVar));
}


// Verarbeitung der einzelnen Zeilen
bool isZuweisung(const string& statement, BBZuweisung *&Z)
{
	// Syntax: Variablen-Name = Audruck
	if (statement.empty())
		return false;
	string s(statement);
	int pos = s.find_first_of('=');
	if (pos <= 0)
		return false;

	// erster Substring muß Variablen-Name sein
	BBZuweisung::BBZuweisungTyp t;
	BBMatrix *bbm;
	BBBaumMatrixPoint *bbp;
	string sub1 = s.substr(0, pos);
	trim(sub1);
	BBTyp *b = isVar(sub1);
	if (b == NULL)
	{
		// überprüfen, ob erster Token ein MatrixIndex (M[p]) ist
		if (!isMatrixIndex(sub1, bbm, bbp))
			return false;
		else
			t = BBZuweisung::MIndex;
	}

	string sub2 = s.substr(pos+1);
	trim(sub2);
	if (sub2.empty())
		return false;

	if (b != NULL)
	{
		// Typ herausfinden
		switch (b->type)
		{
		case BBTyp::IType:
			t = BBZuweisung::ITyp;
			break;
		case BBTyp::FType:
			t = BBZuweisung::FTyp;
			break;
		case BBTyp::MType:
			t = BBZuweisung::MTyp;
			break;
		case BBTyp::PType:
			t = BBZuweisung::PTyp;
			break;
		}
	}
	if (t == BBZuweisung::PTyp || t == BBZuweisung::MTyp)
	{
		BBBaumMatrixPoint *k = NULL;
		try
		{
			pars_matrix_point(sub2, k, /*false*/ t == BBZuweisung::MTyp);
		}
		catch (BBFehlerException)
		{
			return false;
		}
		Z = new BBZuweisung;
		Z->typ = t;
		if (t == BBZuweisung::PTyp)
			Z->ZuVar.PVar = getVarP(b);
		else
			Z->ZuVar.MVar = getVarM(b);
		try
		{
			pars_matrix_point(sub2, Z->ZuArt.MP, /*false*/t == BBZuweisung::MTyp);
		}
		catch (BBFehlerException)
		{
			return false;
		}
	}
	else if (t == BBZuweisung::ITyp || t == BBZuweisung::FTyp)
	{
		BBBaumInteger *k = NULL;
		try
		{
			pars_integer_float(sub2, k, false);
		}
		catch (BBFehlerException)
		{
			return false;
		}
		Z = new BBZuweisung;
		Z->typ = t;
		if (t == BBZuweisung::ITyp)
			Z->ZuVar.IVar = getVarI(b);
		else
			Z->ZuVar.FVar = getVarF(b);
		try
		{
			pars_integer_float(sub2, Z->ZuArt.IF);
		}
		catch (BBFehlerException)
		{
			return false;
		}
	}
	else if (t == BBZuweisung::MIndex)
	{
		Z = new BBZuweisung;
		Z->typ = t;
		Z->ZuVar.MatrixIndex.PVar = bbp;
		Z->ZuVar.MatrixIndex.MVar = bbm;
		try
		{
			pars_integer_float(sub2, Z->ZuArt.IF);
		}
		catch (BBFehlerException)
		{
			return false;
		}
	}
	return true;
}

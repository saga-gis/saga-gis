
//#include "..\stdafx.h"
#include <iostream>

#include <vector>
#include "ausdruck.h"
#include "funktion.h"



using namespace std;


class compare_BB_Funktion : public greater<BBFunktion *> 
{
public:
    bool operator()(const BBFunktion * &x, const BBFunktion * &y) const
	{
		return x->name < y->name;
	};
};

BBBaumInteger::BBBaumInteger()
{
	typ = NoOp; 
	memset(&k, 0, sizeof(BBKnoten));
}

BBBaumInteger::~BBBaumInteger()
{
	if (typ == NoOp)
		return;
	switch(typ)
	{
	case BIOperator:
		if (k.BiOperator.links != NULL)
			delete k.BiOperator.links;
		if (k.BiOperator.rechts != NULL)
			delete k.BiOperator.rechts;
		break;
	case UniOperator:
		if (k.UniOperator.rechts != NULL)
			delete k.UniOperator.rechts;
		break;
	case MIndex:
		if (k.MatrixIndex.P != NULL)
			delete k.MatrixIndex.P;
		break;
	case Funktion:
		if (k.func != NULL)
			delete k.func;
		break;
	case IZahl:
	case FZahl: 
	case IVar:
	case FVar:
		break;
	}
	memset(&k, 0, sizeof(BBKnoten));
}

BBBaumMatrixPoint::BBBaumMatrixPoint() : typ(NoOp) , isMatrix(true)
{
	memset(&k, 0, sizeof(BBKnoten));	
}

BBBaumMatrixPoint::~BBBaumMatrixPoint()
{
	if (typ == NoOp)
		return;
	switch(typ)
	{
	case BIOperator:
		if (k.BiOperator.links != NULL)
			delete k.BiOperator.links;
		if (k.BiOperator.rechts != NULL)
			delete k.BiOperator.rechts;
		break;
	case UniOperator:
		if (k.UniOperator.rechts != NULL)
			delete k.UniOperator.rechts;
		break;
	case IFAusdruck:
		if (k.IntFloatAusdruck.b != NULL)
			delete k.IntFloatAusdruck.b;
		break;
	case MVar:
	case PVar:
		break;
	}
	memset(&k, 0, sizeof(BBKnoten));
}

bool getFirstCharKlammer(const string& statement, const string& cmp, char& c, int& pos)
{
	if (statement.empty())
		return false;

	int klammer_ebene = 0, klammerE_ebene = 0;
	for (int i=0; i<statement.size()-1; i++)
	{
		if (statement[i] == '(')
			klammer_ebene++;
		if (statement[i] == ')')
			klammer_ebene--;
		if (statement[i] == '[')
			klammerE_ebene++;
		if (statement[i] == ']')
			klammerE_ebene--;
		if (klammer_ebene == 0 && klammerE_ebene == 0 && i != statement.size() -1 && i != 0)
		{

			//int p = cmp.find_first_of(statement[i]);
			//if (cmp.find_first_of(statement[i]) >= 0)
			int j;
			for (j=0; j<cmp.size(); j++)
				if (cmp[j] == statement[i])
					break;
			if (j < cmp.size())
			{
				c = statement[i];
				pos = i;
				return true;
			}
		}
	}
	return false;
}

bool getLastCharKlammer(const string& statement, const string& cmp, char& c, int& pos)
{
	if (statement.empty())
		return false;

	int char_found = -1;
	int klammer_ebene = 0, klammerE_ebene = 0;
	for (int i=0; i<statement.size()-1; i++)
	{
		if (statement[i] == '(')
			klammer_ebene++;
		if (statement[i] == ')')
			klammer_ebene--;
		if (statement[i] == '[')
			klammerE_ebene++;
		if (statement[i] == ']')
			klammerE_ebene--;
		if (klammer_ebene == 0 && klammerE_ebene == 0 && i != statement.size() -1 && i != 0)
		{
			int j;
			for (j=0; j<cmp.size(); j++)
				if (cmp[j] == statement[i])
					char_found = i;
		}
	}
	if (char_found > 0)
	{
		c = statement[char_found];
		pos = char_found;
		return true;
	}	
	return false;
}

bool isKlammer(const string& statement)
{
	// klammer-Level zählen
	if (statement.empty())
		return false;

	if (statement[0] != '(' || statement[statement.size()-1] != ')')
		return false;
	int klammer_ebene = 0;
	for (int i=0; i<statement.size()-1; i++)
	{
		if (statement[i] == '(')
			klammer_ebene++;
		if (statement[i] == ')')
			klammer_ebene--;
		if (klammer_ebene == 0 && i != statement.size() -1)
			return false;
	}
	return true;
}

//++++++++++++++ Integer/ Float ++++++++++++++++++++++++++

bool isBiOperator(const string& statement, char& c, int& pos)
{
	// Klammern zählen, da nur zwischen Klammer-Level NULL
	// ein Operator stehen darf

	// den Operator mit der niedrigsten Priorität zuerst ausführen, da er
	// in der Baum-Struktur "oben" stehen muß !
	if (getFirstCharKlammer(statement, "+", c, pos))
		return true;
	if (getLastCharKlammer(statement, "-", c, pos))
		return true;
	if (getFirstCharKlammer(statement, "*", c, pos))
		return true;
	if (getLastCharKlammer(statement, "/", c, pos))
		return true;
	if (getFirstCharKlammer(statement, "^", c, pos))
		return true;
//-->
	if (getFirstCharKlammer(statement, "%", c, pos))
		return true;
//<--
	return false;
}

bool isUniOperator(const string& statement, char& c)
{
	c = statement[0];
	return (c == '-' || c == '+');
}

bool isMatrixIndex(const string& statement, BBMatrix *& bm, BBBaumMatrixPoint *& bp, bool getMem /* = true */)
{
	// wenn X[p] enthält und X = Matrix und p = Point
	if (statement.empty())
		return false;
	string s(statement);
	int pos1, pos2;
	pos1 = s.find('[');
	if (pos1 > 0)
	{
		pos2 = s.find(']');
		if ( pos2 > pos1 && pos2 == s.size()-1 )
		{
			// ersten Teil
			string m, p;
			m = s.substr(0, pos1);
			p = s.substr(pos1+1, pos2-pos1-1);
			BBTyp *tm;
			BBBaumMatrixPoint *bmp;
			if (isMVar(m, tm))
			{
				try
				{
					// erst Testen
					pars_matrix_point(p, bmp, false, false);
				}
				catch (BBFehlerException)
				{
					return false;
				}
				if (!getMem) // falls nichts allokieren -> test erfolgreich
					return true;
				try
				{
					// dann allokieren
					pars_matrix_point(p, bmp, false);
				}
				catch (BBFehlerException)
				{
					return false;
				}

				bm = (BBMatrix *) tm;
				bp = bmp;
				return true;
			}

/*			if (isMVar(m, tm) && isPVar(p, tp))
			{
				bm = (BBMatrix *) tm;
				bp = (BBPoint *) tp;
				return true;
			}
			*/
		}
	}
	return false;
}

bool isFZahl(const string& statement)
{
	// Format: [+-]d[d][.[d[dd]][e|E +|- d[d]]]
	if (statement .size() > 50)
		return false;
	char buff[100];
	double f;
	int anz = sscanf(statement.data(), "%f%s", &f, buff);
	return  (anz == 1);
}

bool isIZahl(const string& statement)
{
	if (statement.empty())
		return false;

	string s(statement);

	// eventuel voranstehenden +-
	if (s[0] == '+')
		s.erase(s.begin());
	else if (s[0] == '-')
		s.erase(s.begin());

	if (s.empty())
		return false;
	int p =	s.find_first_not_of("1234567890");
	if (p >= 0)
		return false;
	
	return true;
}

bool isFVar(const string& statement, BBTyp * & b)
{
	b = isVar(statement);
	if (b == NULL)
		return false;
	if (b->type == BBTyp::FType)
		return true;
	return false;
}

bool isIVar(const string& statement, BBTyp * & b)
{
	b = isVar(statement);
	if (b == NULL)
		return false;
	if (b->type == BBTyp::IType)
		return true;
	return false;
}

bool isPVar(const string& statement, BBTyp * & b)
{
	b = isVar(statement);
	if (b == NULL)
		return false;
	if (b->type == BBTyp::PType)
		return true;
	return false;
}

bool isMVar(const string& statement, BBTyp * & b)
{
	b = isVar(statement);
	if (b == NULL)
		return false;
	if (b->type == BBTyp::MType)
		return true;
	return false;
}

BBFunktion *isFktName(const string& s)
{
	if (FunktionList.empty())
		return NULL;
	T_FunktionList::iterator it;
	for (it = FunktionList.begin(); it != FunktionList.end(); it++)
	{
		if ((*it)->name == s)
			return (*it);
	}
	return NULL;
}

bool getNextFktToken(const string& s, int& pos, string& erg)
{
	// Syntax xx[,xx[,xx...]]
	if (pos >= s.size())
		return false;
	string ss(s.substr(pos));
	int pos1 = ss.find_first_of(',');
	if (pos1 >= 0)
	{
		erg = ss.substr(0, pos1);
		pos += pos1;
	}
	else
	{
		erg = ss;
		pos = s.size();
	}
	if (erg.empty())
		return false;
	return true;
}

bool isFunktion (const string& statement, BBFktExe * & fktexe, bool getMem /* = true */, bool AlleFunktionen /* = true */)
{
	// Syntax: fktname([arg1[, arg2]])
	string s(statement);
	int pos1, pos2;
	pos1 = s.find_first_of('(');
	pos2 = s.find_last_of(')');
	if (pos1 <= 0 || pos2 != s.size()-1)
		return false;

	// Variablen-Name 
	string sub1, sub2;
	sub1 = s.substr(0, pos1);
	trim(sub1);
	sub2 = s.substr(pos1+1, pos2-pos1-1);
	trim(sub2);
	if (sub1.empty())
		return false;
	BBFunktion *fkt = isFktName(sub1);
	if (fkt == NULL)
		return false;

	if (!AlleFunktionen)
	{ // nur diejenigen Funktionen mit Return-Typ
		if (fkt->ret.typ == BBArgumente::NoOp) // kein Return-Typ
			return false;
	}
	if (sub2.empty()) // keine Argumente
	{
		if (!fkt->args.empty())
			return false;
		if (getMem)
		{
			fktexe = new BBFktExe;
			fktexe->args = fkt->args;
			fktexe->f = fkt;  // vorher ... = NULL;
		}
		return true;
	}
	else
	{
		// Argumente zählen
		// 1. Float/Integer lassen sich konvertieren
		// 2. Matrix
		// 3. Point
		if (getMem)
		{
			fktexe = new BBFktExe;
			fktexe->args = fkt->args; // vector kopieren
			fktexe->f = fkt;
		}
		int anz = fkt->args.size();
		int possub2 = 0;
		for (int i=0; i<anz; i++)
		{
			// finde Token
			string ss;
			if (!getNextFktToken(sub2, possub2, ss))
				return false;
//			BBTyp *bt = isVar(ss);
//			if (bt == NULL)
//				return false;
			if (fkt->args[i].typ == BBArgumente::ITyp ||
				fkt->args[i].typ == BBArgumente::FTyp)
			{
//				if (bt->type != BBTyp::IType || 
//					bt->type != BBTyp::FType)
//					return false;
				try
				{
					BBBaumInteger *b;
					pars_integer_float(ss, b, getMem);
					if (getMem)
						fktexe->args[i].ArgTyp.IF = b;

				}
				catch (BBFehlerException)
				{
					if (getMem)
						delete fktexe;
					return false;
				}
			}
			else 
			{
/*				if (fkt->args[i].typ == BBArgumente::MTyp &&
					bt->type != BBTyp::MType) 
					return false;
				if (fkt->args[i].typ == BBArgumente::PTyp &&
					bt->type != BBTyp::PType) 
					return false;
*/
				try
				{
					BBBaumMatrixPoint *b;
					pars_matrix_point(ss, b, fkt->args[i].typ == BBArgumente::MTyp, getMem);
					if (getMem)
						fktexe->args[i].ArgTyp.MP = b;
				}
				catch (BBFehlerException)
				{
					if (getMem)
						delete fktexe;
					return false;
				}
			}
			possub2++; // Komma entfernen
		}
		if (possub2 < sub2.size()) // zuviel Parameter angegeben
		{
			if (getMem)
				delete fktexe;
			return false;
		}
	}
	return true;
}

// ------------- Hauptroutine -------------------

static char c;
static BBTyp *b;
static BBMatrix *bm;
static BBPoint *bp;
static BBBaumMatrixPoint *bmp;
static int pos;
static BBFktExe *bfkt;

void pars_integer_float(const string& statement, BBBaumInteger * & Knoten, int getMem /* = true */)
{
	string s(statement);
	trim(s);
	if (s.empty())
		throw BBFehlerException();
	if (isKlammer(s))
	{
		s.erase(s.begin());
		s.erase(s.end()-1);

		pars_integer_float(s, Knoten, getMem);
	}
	else if (isMatrixIndex(s, bm, bmp, getMem!=0))
	{
		if (getMem)
		{
			Knoten = new BBBaumInteger;
			Knoten->typ = BBBaumInteger::MIndex;
			Knoten->k.MatrixIndex.M = bm;
			Knoten->k.MatrixIndex.P = bmp;
		}
	}
	else if (isBiOperator(s, c, pos))
	{
		string links  = s.substr(0, pos);
		string rechts = s.substr(pos+1, s.size()-pos-1);
		if (links.empty() || rechts.empty())
			throw BBFehlerException();

		if (getMem)
		{
			Knoten = new BBBaumInteger;
			Knoten->typ = BBBaumInteger::BIOperator;
			switch(c)
			{
			case '+':
				Knoten->k.BiOperator.OpTyp = BBBaumInteger::BBKnoten::BBBiOperator::Plus;
				break;
			case '-':
				Knoten->k.BiOperator.OpTyp = BBBaumInteger::BBKnoten::BBBiOperator::Minus;
				break;
			case '*':
				Knoten->k.BiOperator.OpTyp = BBBaumInteger::BBKnoten::BBBiOperator::Mal;
				break;
			case '/':
				Knoten->k.BiOperator.OpTyp = BBBaumInteger::BBKnoten::BBBiOperator::Geteilt;
				break;
			case '^':
				Knoten->k.BiOperator.OpTyp = BBBaumInteger::BBKnoten::BBBiOperator::Hoch;
				break;
//-->
			case '%':
				Knoten->k.BiOperator.OpTyp = BBBaumInteger::BBKnoten::BBBiOperator::Modulo;
				break;
//<--
			}
			pars_integer_float(links, Knoten->k.BiOperator.links);
			pars_integer_float(rechts, Knoten->k.BiOperator.rechts);
		}
		else
		{
			pars_integer_float(links, Knoten, getMem);
			pars_integer_float(rechts, Knoten, getMem);
		}
	}
	else if (isUniOperator(s, c))
	{
		s.erase(s.begin());
		if (getMem)
		{
			Knoten = new BBBaumInteger;
			Knoten->typ = BBBaumInteger::UniOperator;
			Knoten->k.UniOperator.OpTyp = (c == '+' ? BBBaumInteger::BBKnoten::BBUniOperator::Plus : BBBaumInteger::BBKnoten::BBUniOperator::Minus);
			pars_integer_float(s, Knoten->k.UniOperator.rechts);
		}
		else
			pars_integer_float(s, Knoten->k.UniOperator.rechts, getMem);

	}	
	else if (isFZahl(s))
	{
		if (getMem)
		{
			Knoten = new BBBaumInteger;
			Knoten->typ = BBBaumInteger::FZahl;
			Knoten->k.FZahl = atof(s.data());
		}
	}
	else if (isIZahl(s))
	{
		if (getMem)
		{
			Knoten = new BBBaumInteger;
			Knoten->typ = BBBaumInteger::IZahl;
			Knoten->k.IZahl = (int)atof(s.data());
		}
	}

	else if (isFVar(s, b))
	{
		if (getMem)
		{
			Knoten = new BBBaumInteger;
			Knoten->typ = BBBaumInteger::FVar;
			Knoten->k.FVar = getVarF(b);
		}
	}
	else if (isIVar(s, b))
	{
		if (getMem)
		{
			Knoten = new BBBaumInteger;
			Knoten->typ = BBBaumInteger::IVar;
			Knoten->k.IVar = getVarI(b);
		}
	}
	else if (isFunktion (s, bfkt, getMem!=0, false)) // nur die Funktionen mit Return-Typ
	{
		if (getMem)
		{
			Knoten = new BBBaumInteger;
			Knoten->typ = BBBaumInteger::Funktion;
			Knoten->k.func = bfkt;
		}
	}
	else
		throw BBFehlerException();
}

bool isIntFloatAusdruck(const string& s)
{
	try
	{
		BBBaumInteger *knoten = NULL;
		pars_integer_float(s, knoten, false);
	}
	catch (BBFehlerException)
	{
		return false;
	}
	return true;
}

//++++++++++++++ Point ++++++++++++++++++++++++++
// Operator p/p + -
// Operator p/i i/p p/f f/p * /

//++++++++++++++ Matrix ++++++++++++++++++++++++++
// Operator M/M + -
// Operator M/i i/M M/f f/M * /

void pars_matrix_point(const string& statement, BBBaumMatrixPoint * &Knoten, bool matrix, bool getMem /* = true */)
{
	string s(statement);
	trim(s);
	if (s.empty())
		throw BBFehlerException();
	if (isKlammer(s))
	{
		s.erase(s.begin());
		s.erase(s.end()-1);

		pars_matrix_point(s, Knoten, matrix, getMem);
	}
	else if (isUniOperator(s, c))
	{
		s.erase(s.begin());
		if (getMem)
		{
			Knoten = new BBBaumMatrixPoint;
			Knoten->typ = BBBaumMatrixPoint::UniOperator;
			Knoten->k.UniOperator.OpTyp = (c == '+' ? BBBaumMatrixPoint::BBKnoten::BBUniOperator::Plus : BBBaumMatrixPoint::BBKnoten::BBUniOperator::Minus);
			Knoten->isMatrix = matrix;
			pars_matrix_point(s, Knoten->k.UniOperator.rechts, matrix);
		}
		else
			pars_matrix_point(s, Knoten, matrix, getMem);
	}	
	else if (isBiOperator(s, c, pos))
	{
		string links  = s.substr(0, pos);
		string rechts = s.substr(pos+1, s.size()-pos-1);
		if (links.empty() || rechts.empty())
			throw BBFehlerException();
		if (getMem)
		{
			Knoten = new BBBaumMatrixPoint;
			Knoten->typ = BBBaumMatrixPoint::BIOperator;
			Knoten->isMatrix = matrix;
			switch(c)
			{
			case '+':
				Knoten->k.BiOperator.OpTyp = BBBaumMatrixPoint::BBKnoten::BBBiOperator::Plus;
				break;
			case '-':
				Knoten->k.BiOperator.OpTyp = BBBaumMatrixPoint::BBKnoten::BBBiOperator::Minus;
				break;
			case '*':
				Knoten->k.BiOperator.OpTyp = BBBaumMatrixPoint::BBKnoten::BBBiOperator::Mal;
				break;
			case '/':
				Knoten->k.BiOperator.OpTyp = BBBaumMatrixPoint::BBKnoten::BBBiOperator::Geteilt;
				break;
			case '^':
				throw BBFehlerException();
				break;
			case '%':
				throw BBFehlerException();
				break;
			}
			pars_matrix_point(links, Knoten->k.BiOperator.links, matrix);
			pars_matrix_point(rechts, Knoten->k.BiOperator.rechts, matrix);
			if (c == '+' || c == '-')
			{
				// Operator nur zwischen zwei Points
				if (matrix && (	(Knoten->k.BiOperator.rechts)->typ != BBBaumMatrixPoint::MVar ||
								(Knoten->k.BiOperator.links )->typ != BBBaumMatrixPoint::MVar ))
				{
						throw BBFehlerException();
				}
				if (!matrix && ((Knoten->k.BiOperator.rechts)->typ != BBBaumMatrixPoint::PVar ||
								(Knoten->k.BiOperator.links )->typ != BBBaumMatrixPoint::PVar ))
				{
						throw BBFehlerException();
				}
			}
			if (c == '*' || c == '/')
			{
				// Operator nur zwischen i/f und p, Reihenfolge egal
				int pvar = 0;
				int mvar = 0;

				if ((Knoten->k.BiOperator.rechts)->typ == BBBaumMatrixPoint::PVar)
					pvar++;
				if ((Knoten->k.BiOperator.rechts)->typ == BBBaumMatrixPoint::MVar)
					mvar++;
				if ((Knoten->k.BiOperator.links)->typ == BBBaumMatrixPoint::PVar)
					pvar++;
				if ((Knoten->k.BiOperator.links)->typ == BBBaumMatrixPoint::MVar)
					mvar++;

				if (matrix &&  (mvar != 1 || pvar != 0))
					throw BBFehlerException();
				if (!matrix && (pvar != 1 || mvar != 0))
					throw BBFehlerException();
			}
		}
		else
		{
			pars_matrix_point(links, Knoten, matrix, getMem);
			pars_matrix_point(rechts, Knoten, matrix, getMem);
		}
	}
	else if (matrix && isMVar(s, b))
	{
		if (getMem)
		{
			Knoten = new BBBaumMatrixPoint;
			Knoten->typ = BBBaumMatrixPoint::MVar;
			Knoten->k.M = getVarM(b);
			Knoten->isMatrix = matrix;
		}
	}
	else if (!matrix && isPVar(s, b))
	{
		if (getMem)
		{
			Knoten = new BBBaumMatrixPoint;
			Knoten->typ = BBBaumMatrixPoint::PVar;
			Knoten->k.P = getVarP(b);
			Knoten->isMatrix = matrix;
		}
	}
	else if (isIntFloatAusdruck(s))
	{
		if (getMem)
		{
			Knoten = new BBBaumMatrixPoint;
			Knoten->typ = BBBaumMatrixPoint::IFAusdruck;
			Knoten->isMatrix = matrix;
			pars_integer_float(s, Knoten->k.IntFloatAusdruck.b);
		}
		else
		{
			BBBaumInteger *k = NULL;
			pars_integer_float(s, k, getMem);
		}
	}
	else
		throw BBFehlerException();
}


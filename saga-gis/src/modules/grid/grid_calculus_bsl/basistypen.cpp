/**********************************************************
 * Version $Id$
 *********************************************************/

#include <iostream>
#include <algorithm>
#include <sstream>
#include <sstream>
#include <locale>

#include "basistypen.h"

#include <assert.h>

#define WHITE_SP " \t\n"
#define WHITE_SP_TRENNER " ,;\t\n"

using namespace std;

T_VarList VarList;
T_InputText InputText;
T_InputText InputGrids;
int FehlerZeile;
int FehlerPos1;
int FehlerPos2;
string FehlerString("");
bool isSyntaxCheck;

class compare_BB_greater : public greater<BBTyp *> 
{
public:
// o.c.   bool operator()(const BBTyp * &x, const BBTyp * &y) const
    bool operator()(BBTyp *x, BBTyp *y) const
	{
		return x->name < y->name;
	};
};/**/

class compare_BBType
{
public:
	bool operator() (const BBTyp *x, const BBTyp *y)
	{
		return x->name < y->name;
	}
};

class compare_BBType2
{
public:
	bool operator() (BBTyp *x, BBTyp y)
	{
		return x->name < y.name;
	}
};

class compare_BBType3
{
public:
	bool operator() (BBTyp x, BBTyp y)
	{
		return x.name < y.name;
	}
};

void GetInputText(string bsl )
// wandelt EditControl-Text in Vector von Strings um
{
/*	size = e.GetLineCount();
	char *lpszBuffer = new char [1000];
	int buffanz = 1000;
	lpszBuffer[0] = 0;
	InputText.clear();
	for (int i=0; i<size; i++)
	{
		l= e.LineLength(i);
		if (l > buffanz-2)
		{
			delete [] lpszBuffer;
			buffanz = l+2;
			lpszBuffer = new char [buffanz];
		}
		lpszBuffer[0] = buffanz;
		charcopied = e.GetLine( i, lpszBuffer );
		lpszBuffer[charcopied] = 0;
		string s(lpszBuffer);
		InputText.push_back(s);
	}
	delete [] lpszBuffer;*/
}

void WhiteSpace(string& s, int& pos, bool vorn/* = true*/)
// entfertn entweder vorne die Wildcards oder schneidet ab dem
// ersten Whitespace/Trenner den Rest ab
//
// Funktionweise:	
//		vorn == true (default):
//			Es werden soviele Stellen zu "pos" addiert, wie Wildcards 
//			am Anfang von "s" stehen
//		vorn == false:
//			Es wird der String s ab dem ersten vorkommenden Wildcard abgeschnitten
//
// 
{
	if (vorn)
	{
		int pos1 = s.find_first_not_of(WHITE_SP);
		if (pos1 > 0)
		{
			s.erase(s.begin(), s.begin() + pos1);	// o.c
	// o.c	s.erase(s.begin(), &s[pos1]);
			pos += pos1;
		}
	}
	else
	{
		int pos1 = s.find_first_of(WHITE_SP_TRENNER);
		if (pos1 > 0)
		{
			s.erase(s.begin() + pos1, s.end());	// o.c
	// o.c	s.erase(&s[pos1], s.end());
		}
	}
}

void trim(string& s)
{
	// vorne Whitespaces entfernen
	int pos1 = s.find_first_not_of(WHITE_SP);
	if (pos1 > 0)
	{
		s.erase(s.begin(), s.begin() + pos1);
	}

	// hinter Whitespaces entfernen
	pos1 = s.find_last_not_of(WHITE_SP);
	if (pos1 >= 0)
	{
		s.erase(s.begin() + pos1+1, s.end());	// o.c.
	}
}

bool isNextToken(int zeile, int pos, string& cmp)
{
	string s = InputText[zeile].substr(pos);
	if (!isNotEnd(zeile, pos, s))
		return false;

	WhiteSpace(s, pos);
	return s == cmp;
}

bool isNextChar(int zeile, int pos, const char c)
{
	string s = InputText[zeile].substr(pos);
	if (!isNotEnd(zeile, pos, s))
		return false;

	WhiteSpace(s, pos);
	return s[0] == c;
}

bool getNextToken(int &zeile, int& pos, string& erg)
{

	string s = InputText[zeile];
	erg = InputText[zeile].substr(pos);
	if (!isNotEnd(zeile, pos, erg))
		return false;
	
	WhiteSpace(erg, pos);
	WhiteSpace(erg, pos, false);
	pos += erg.size();
	return true;
}

bool getNextZeile(int &zeile, int& pos, string& erg)
{
	// Erstellt string von (zeile, pos) bis zum nächsten Komma
	if (zeile >= (int)InputText.size())
		return false;
	string sub = InputText[zeile].substr(pos);
	erg = "";
	int p;
	do 
	{
		if ((p = sub.find_first_of(';')) >= 0)
		{
			sub.erase(p, sub.size());
			pos = p;
			erg += sub;
			return true;
		}
		erg += sub;
		p = sub.size()+pos;
		if (!isNotEnd(zeile,p, sub))
			return false;
	} while (true);
	return false;
}

bool getNextChar(int& zeile, int& pos, char& c)
{
	string s = InputText[zeile].substr(pos);
	if (!isNotEnd(zeile, pos, s))
		return false;

	WhiteSpace(s, pos);
	pos++;
	c = s[0];
	return true;
}


bool isNotEnd(int& zeile, int& pos, string& s)
{
	if (zeile >= (int)InputText.size())
		return false;
	int pos1;
	if (pos >= (int)InputText[zeile].size() || (pos1 = InputText[zeile].substr(pos).find_first_not_of(WHITE_SP)) < 0)
	{
		int p;
		do
		{
			zeile++;
			if (zeile >= (int)InputText.size())
				return false;
			p = InputText[zeile].find_first_not_of(WHITE_SP);
		}
		while (p < 0);

		pos = 0;
		s = InputText[zeile];
	}
	else
	{
		// überprüfen, ob s nur aus Whitespaces besteht

	}
	return true;
}

void DeleteVarList(void)
{
	if (VarList.empty())
		return;
	T_VarList::iterator it;
	for (it = VarList.begin(); it != VarList.end(); it++)
	{
		delete (*it);
	}
	VarList.clear();
}

// wandelt Strings in Variablen um (aus InputText wird VarList)
void ParseVars(int& zeile, int& pos)
{
	// Syntax: Type varname [, varname [...] ] ;
	string subz;
	BBTyp::T_type t;
	char c;
	DeleteVarList();
	int zeile_old = zeile;
	int pos_old = pos;
	FehlerZeile = zeile;
	while (getNextToken(zeile, pos, subz))
	{
		// Typ angeben
		if (subz == "Integer")
			t = BBTyp::IType;
		else if (subz == "Float")
			t = BBTyp::FType;
		else if (subz == "Point")
			t = BBTyp::PType;
		else if (subz == "Matrix")
			t = BBTyp::MType;
		else
		{
			zeile = zeile_old;
			pos = pos_old;
			break;
		}

		// Variablen-Name, durch Komma getrennt
		while (getNextToken(zeile, pos, subz))
		{
			FehlerZeile = zeile;
			// einfügen in VarList
			BBTyp *bt;
			switch(t)
			{
			case BBTyp::IType:
				bt = new BBInteger;
				bt->name = subz;
				bt->type = t;
				break;
			case BBTyp::FType:
				bt = new BBFloat;
				bt->name = subz;
				bt->type = t;
				break;
			case BBTyp::PType:
				bt = new BBPoint;
				bt->name = subz;
				bt->type = t;
				break;
			case BBTyp::MType:
				// falls Name () 
				{
					int l = subz.size();
					if (subz[l-1] == ')' && subz[l-2] == '(')
					{
						// Matrix: M()
						subz.erase(l-2, 2);
						bt = new BBMatrix(NULL);
					}
					else if (subz[l-1] == ')')
					{
						printf("loadig files not suported");
						return;
						/*
						int posk;
						// Matrix: M("filename")
						if ((posk = subz.find('(')) <= 0)
							throw BBFehlerException(zeile);
						// Filename herausfinden
						string filename;
						filename = subz.substr(posk+1);
						if (filename.size() < 1)
							throw BBFehlerException(zeile);
						filename.erase(filename.size()-1);
						if (filename[0] !='"' || filename[filename.size()-1] != '"')
							throw BBFehlerException(zeile);
						// Variablen-Name herausfinden
						filename.erase(0, 1);
						filename.erase(filename.size()-1);
						subz.erase(posk);
						// Datei laden
						bt = new BBMatrix();
//
//						surferDoc sd;
						CFile file;
						if (!file.Open(filename.data(), CFile::modeRead))
						{
							string out;
							ostringstream ostr(out);
							ostr << "Die Datei >" << filename << "< existiert nicht!" << ends;
							AfxMessageBox(ostr.str().data());
							delete bt;
							throw BBFehlerException(zeile);
						}
						if (!isSyntaxCheck)
						{
							CArchive archive(&file, CArchive::load);
//							sd.readGrid(archive, *(((BBMatrix *) bt)->M));
						}
						*/
					}
					else
						bt = new BBMatrix;
					bt->name = subz;
					bt->type = t;
				}
				break;
			}
			if (isVar(subz) != NULL) // falls Name bereits vorhanden
			{
				delete bt;
				throw BBFehlerException(zeile);
			}
			VarList.push_back(bt);

			// überprüfen auf Komma
			if (!isNextChar(zeile, pos, ','))
				break;
			// wenn Komma, nochmal
			if (!getNextChar(zeile, pos, c))
				throw BBFehlerException(zeile);
		}
		// überprüfen auf Semikolon;
		if (!getNextChar(zeile, pos, c) || c != ';')
			throw BBFehlerException(zeile);
		zeile_old = zeile;
		pos_old = pos;
	}
}

// überprüft, ob string eine gültige Variable ist
BBTyp *isVar(const string& s)
{
	if (VarList.empty())
		return NULL;
//	bool exist;
	T_VarList::iterator it;
	for (it = VarList.begin(); it != VarList.end(); it++)
	{
		string ss = (*it)->name;
		if ((*it)->name == s)
			return (*it);
	}
	return NULL;
/*	BBTyp bt;
	bt.name = s;
	exist = binary_search(VarList.begin(), VarList.end(), &bt, compare_BBType() );
	if (exist)
	{
		it = search(VarList.begin(), VarList.end(), &bt, &bt, compare_BBType2() );
		if (it != VarList.end())
			return (*it);
		else
			return NULL;
	}
	else
		return NULL;
*/
}


// ermittelt den Variablen-Typ einer Variablen
BBTyp::T_type getVarType(BBTyp *s)
{
	return s->type;
}

// liefert die jeweilige Variable zurück
BBInteger *getVarI(BBTyp *s)
{
	return ((BBInteger *) s);
}

BBFloat *getVarF(BBTyp *s)
{
	return ((BBFloat *) s);
}

BBMatrix *getVarM(BBTyp *s)
{
	return ((BBMatrix *) s);
}

BBPoint *getVarP(BBTyp *s)
{
	return ((BBPoint *) s);
}

void setMatrixVariables(BBMatrix *M)
{
	BBInteger *i;
	BBFloat *f;
	BBTyp *b;

	// xanz
	b = isVar(M->name + ".xanz");
	assert(b != NULL);
	i =	getVarI(b);
	assert(i->i == NULL);
	i->i = &(M->M->xanz);
	// yanz
	b = isVar(M->name + ".yanz");
	assert(b != NULL);
	i =	getVarI(b);
	assert(i->i == NULL);
	i->i = &(M->M->yanz);
	// xll
	b = isVar(M->name + ".xll");
	assert(b != NULL);
	f =	getVarF(b);
	assert(f->f == NULL);
	f->f = &(M->M->xll);
	// yll
	b = isVar(M->name + ".yll");
	assert(b != NULL);
	f =	getVarF(b);
	assert(f->f == NULL);
	f->f = &(M->M->yll);
	// dxy
	b = isVar(M->name + ".dxy");
	assert(b != NULL);
	f =	getVarF(b);
	assert(f->f == NULL);
	f->f = &(M->M->dxy);
}

bool GetMemoryGrids(CSG_Parameters *BSLParameters)
{
	T_VarList::iterator it;

	for (it = VarList.begin(); it != VarList.end(); it++)
	{
		BBTyp::T_type t = getVarType(*it);

		if (t == BBTyp::MType)
		{	
			BBMatrix *M = getVarM(*it);

			if (!M->isMem) // falls noch erzeugt werden muß
			{
				CSG_Grid	*pInput	= (*BSLParameters)(M->name.c_str())->asGrid();
				GridWerte	*pGrid	= new GridWerte();

				pGrid->Create(*pInput);

				pGrid->xanz	= pGrid->Get_NX();
				pGrid->yanz	= pGrid->Get_NY();
				pGrid->dxy	= pGrid->Get_Cellsize();
				pGrid->xll	= pGrid->Get_XMin();
				pGrid->yll	= pGrid->Get_YMin();

				pGrid->calcMinMax();

				M->M		= pGrid;
				M->isMem	= true;

				setMatrixVariables(M);
			}
		}
	}

	bool ret = (it == VarList.end());
	VarList.sort(compare_BB_greater());
	return ret;
}

bool FindMemoryGrids(void)
{
	InputGrids.clear();
			
	T_VarList::iterator it;
	for (it = VarList.begin(); it != VarList.end(); it++)
	{
		BBTyp::T_type t = getVarType(*it);
		if (t == BBTyp::MType)
		{	
			BBMatrix *M = getVarM(*it);
			if (!M->isMem) 
			{
				InputGrids.push_back(M->name);
			}
		}

	}
	bool ret = (it == VarList.end());
	return ret;
}

void AddMatrixPointVariables(bool pointer2matrix)
{
//	if (pointer2matrix)
//	{
		// Alle anderen Matirx/Point-Variablen hinzu
		T_VarList::iterator it;
		for (it = VarList.begin(); it != VarList.end(); it++)
		{
			if ((*it)->type == BBTyp::MType)
			{
				BBMatrix *m = getVarM(*it);
				BBTyp *bt;
				bt = new BBInteger((m->isMem ? &(((BBMatrix *)(*it))->M->xanz) : NULL));
				bt->name = (*it)->name + string(".xanz");
				VarList.push_back(bt);
				bt = new BBInteger((m->isMem ? &(((BBMatrix *)(*it))->M->yanz) : NULL));
				bt->name = (*it)->name + string(".yanz");
				VarList.push_back(bt);
				bt = new BBFloat((m->isMem ? &(((BBMatrix *)(*it))->M->dxy) : NULL));
				bt->name = (*it)->name + string(".dxy");
				VarList.push_back(bt);
				bt = new BBFloat((m->isMem ? &(((BBMatrix *)(*it))->M->xll) : NULL));
				bt->name = (*it)->name + string(".xll");
				VarList.push_back(bt);
				bt = new BBFloat((m->isMem ? &(((BBMatrix *)(*it))->M->yll) : NULL));
				bt->name = (*it)->name + string(".yll");
				VarList.push_back(bt);
			}
			else if ((*it)->type == BBTyp::PType)
			{
				BBTyp *bt;
				bt = new BBInteger(&(((BBPoint *)(*it))->v.x));
				bt->name = (*it)->name + string(".x");
				VarList.push_back(bt);
				bt = new BBInteger(&(((BBPoint *)(*it))->v.y));
				bt->name = (*it)->name + string(".y");
				VarList.push_back(bt);
			}
		}
/*	}
	else
	{
		// Alle anderen Matirx/Point-Variablen hinzu
		T_VarList::iterator it;
		for (it = VarList.begin(); it != VarList.end(); it++)
		{
			if ((*it)->type == BBTyp::MType)
			{
				BBTyp *bt;
				bt = new BBInteger();
				bt->name = (*it)->name + string(".xanz");
				VarList.push_back(bt);
				bt = new BBInteger();
				bt->name = (*it)->name + string(".yanz");
				VarList.push_back(bt);
				bt = new BBFloat();
				bt->name = (*it)->name + string(".dxy");
				VarList.push_back(bt);
				bt = new BBFloat();
				bt->name = (*it)->name + string(".xll");
				VarList.push_back(bt);
				bt = new BBFloat();
				bt->name = (*it)->name + string(".yll");
				VarList.push_back(bt);
			}
			else if ((*it)->type == BBTyp::PType)
			{
				BBTyp *bt;
				bt = new BBInteger();
				bt->name = (*it)->name + string(".x");
				VarList.push_back(bt);
				bt = new BBInteger();
				bt->name = (*it)->name + string(".y");
				VarList.push_back(bt);
			}
		}

	}

  
*/
		
	// kommt später
	//if (!pointer2matrix)
		VarList.sort(compare_BB_greater());
}

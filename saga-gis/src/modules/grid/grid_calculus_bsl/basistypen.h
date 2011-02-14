#ifndef __BASISTYPEN_H 
#define __BASISTYPEN_H 

#include <vector>
#include <list>
#include <string>
#include "grid_bsl.h"
#include "MLB_Interface.h"
//#include "grid.h"
//#include "Parameters.h"


#pragma warning (disable : 4786 )

extern int FehlerZeile;
extern std::string FehlerString;
extern int FehlerPos1;
extern int FehlerPos2;
extern bool isSyntaxCheck;

class BBFehlerException
{
public:
	BBFehlerException()
	{
		FehlerPos1 = 0;
		FehlerPos2 = 0;
	};

	BBFehlerException(int z, int p1 = 0, int p2 = 0)
	{
		FehlerPos1 = p1;
		FehlerPos2 = p2;
	};
};

struct T_Point
{
	long x;
	long y;
};

class BBTyp
{
public:
	std::string name;
	enum T_type {IType, FType, PType, MType} type;

	BBTyp() {};
	virtual ~BBTyp() {};
};

class BBInteger : public BBTyp
{
public:
	BBInteger()
	{ 
		type = IType; 
		isMem = true;
		i = new long;
		*i = 0;
	};
	
	BBInteger(int ii)
	{ 
		type = IType; 
		isMem = true;
		i = new long;
		*i = ii;
	};
	
	BBInteger(long *ii)
	{
		type = IType; 
		isMem = false;
		i = ii;
	};
	~BBInteger() 
	{
		if (isMem)
			delete i;
	};
	
	// z.B. M.xanz auch ein int ist und *i auch darauf zeigen soll
	bool isMem;
	long *i;
};

class BBFloat : public BBTyp
{
public:
	BBFloat()
	{ 
		type = FType; 
		isMem = true;
		f = new double;
		*f = 0.0;
	};

	BBFloat(double ff)
	{ 
		type = FType; 
		isMem = true;
		f = new double;
		*f = ff;
	};
	
	BBFloat(double *ff)
	{ 
		type = FType; 
		isMem = false;
		f = ff;
	};

	~BBFloat() 
	{
		if (isMem)
			delete f;
	};

	// Datenelement ist nur ein Pointer, da 
	// z.B. M.dxy auch ein double ist und *f auch darauf zeigen soll
	bool isMem;
	double *f;
};


class BBPoint : public BBTyp
{
public:
	BBPoint() 
	{ 
		type = PType; 
	};
	
	BBPoint(const T_Point& vv) : v(vv) 
	{ 
		type = PType; 
	};
	
	~BBPoint() { };

	T_Point v;
};


class BBMatrix : public BBTyp
{
public:
	BBMatrix() 
	{
		type = MType; 
		isMem = true;
		M = new GridWerte;
	};
	
	BBMatrix(GridWerte *m) 
	{
		type = MType; 
		isMem = false;
		M = m;
			
	};

	~BBMatrix() 
	{
		if (isMem)
			delete M;
	};

	bool isMem;
	GridWerte *M;
};


// Liste aller Variablen 
typedef std::list<BBTyp *> T_VarList;
typedef std::vector<std::string> T_InputText;

extern T_VarList Varlist;
extern T_InputText InputText;
extern T_InputText InputGrids;
void DeleteVarList(void);


void WhiteSpace(std::string& s, int& pos, bool vorn = true);
void trim(std::string& s);
bool isNextToken(int zeile, int pos, std::string& cmp);
bool isNextChar(int zeile, int pos, const char c);
bool getNextToken(int &zeile, int& pos, std::string& erg);
bool getNextZeile(int &zeile, int& pos, std::string& erg);
bool getNextChar(int& zeile, int& pos, char& c);
bool isNotEnd(int& zeile, int& pos, std::string& s);






// wandelt Strings in Variablen um (aus InputText wird VarList)
void ParseVars(int& zeile, int& pos);

// überprüft, ob string eine gültige Variable ist
BBTyp *isVar(const std::string& s);

// ermittelt den Variablen-Typ einer Variablen
BBTyp::T_type getVarType(BBTyp *s);

// liefert die jeweilige Variable zurück
BBInteger *getVarI(BBTyp *s);
BBFloat *getVarF(BBTyp *s);
BBMatrix *getVarM(BBTyp *s);
BBPoint *getVarP(BBTyp *s);

bool GetMemoryGrids(CSG_Parameters *BSLParameters);
bool FindMemoryGrids(void);
void AddMatrixPointVariables(bool pointer2matrix);

#endif

/**********************************************************
 * Version $Id$
 *********************************************************/
#ifndef __FUNKTION_BB_H 
#define __FUNKTION_BB_H 



#include "ausdruck.h"


class BBArgumente
{
public:
	BBArgumente();
	~BBArgumente();
	enum ArgumentTyp { NoOp, ITyp, FTyp, MTyp, PTyp } typ;
	union BBArgs
	{
		BBBaumMatrixPoint *MP;
		BBBaumInteger *IF;
	} ArgTyp; 
};

typedef std::vector<BBArgumente> T_FktArgumente;

class BBFunktion
{
public:
	BBFunktion();
	virtual ~BBFunktion();
	virtual void fkt(void) = 0;
	T_FktArgumente args;
	BBArgumente ret;
	const char *name;
};

// Um einen Funktionsaufruf zu speichern, ist folgendes notwending:
// BBFunktion *f;
// vector<Argumente> args;
// 
struct BBFktExe
{
	BBFktExe();
	~BBFktExe();

	BBFunktion *f;
	T_FktArgumente args;
};

typedef std::list<BBFunktion *> T_FunktionList;
extern T_FunktionList FunktionList;

void InitFunktionen(void);
void DeleteFunktionen(void);

bool operator<(const BBArgumente& x, const BBArgumente& y);
bool operator==(const BBArgumente& x, const BBArgumente& y);


#endif

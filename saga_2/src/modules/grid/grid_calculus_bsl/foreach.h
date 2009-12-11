#ifndef __FOREACH_H 
#define __FOREACH_H 

#include "zuweisung.h"
#include <string>

class BBForEach;
class BBIf;

class BBAnweisung
{
public:
	BBAnweisung();
	~BBAnweisung();

	enum T_AnweisungTyp {ForEach, IF, Zuweisung, Funktion } typ;

	union T_AnweisungVar
	{
		BBForEach *For;
		BBIf		*IF;
		BBZuweisung *Zu;
		BBFktExe *Fkt;
	} AnweisungVar;
};

typedef std::list<BBAnweisung *> T_AnweisungList;
void DeleteAnweisungList(T_AnweisungList& a);


class BBForEach
{
public:
	BBForEach();
	~BBForEach();

	enum ForEachType { Point, Nachbar } type;

	BBMatrix *M;
	BBPoint *P;
	BBPoint *N;
	T_AnweisungList z;
};

bool getNextToken(const std::string& ss, int& pos, std::string& erg);
bool getNextChar(const std::string& ss, int& pos, char& c);
bool getStringBetweenKlammer(const std::string& s, int& pos);


bool isForEach(const std::string& ins, int& pos, BBForEach *& f, std::string& anweisungen);




#endif

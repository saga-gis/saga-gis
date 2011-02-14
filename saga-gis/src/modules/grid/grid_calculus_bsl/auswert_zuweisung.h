/**********************************************************
 * Version $Id$
 *********************************************************/
#ifndef __AUSWERT_ZUWEISUNG_H 
#define __AUSWERT_ZUWEISUNG_H 

#include "pars_all.h"

class BBFehlerAusfuehren
{
public:
	BBFehlerAusfuehren() { Text = ""; };
	BBFehlerAusfuehren(std::string s) { Text = s; };
	~BBFehlerAusfuehren() {};
	std::string Text;
};


class BBFehlerUserbreak
{
public:
	BBFehlerUserbreak() { Text = ""; };
	BBFehlerUserbreak(std::string s) { Text = s; };
	~BBFehlerUserbreak() {};
	std::string Text;
};

class BBFehlerMatrixNotEqual
{
public:
	BBFehlerMatrixNotEqual() {};
	~BBFehlerMatrixNotEqual() {};

};

int fround(double f);
int auswert_funktion_integer(BBFktExe *func);
double auswert_funktion_float(BBFktExe *func);


int auswert_integer(BBBaumInteger& b);
double auswert_float(BBBaumInteger& b);
bool auswert_point(BBBaumMatrixPoint& b, T_Point& vret, double& fret);
bool auswert_matrix(BBBaumMatrixPoint& b, GridWerte& mret, double& fret);

void ausfuehren_zuweisung(BBZuweisung& z);



#endif

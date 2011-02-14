/**********************************************************
 * Version $Id$
 *********************************************************/
#ifndef __BEDINGUNG_H 
#define __BEDINGUNG_H 

#include "foreach.h"

// Syntax: if (bedingung) { anweisungen }
//			bedingung: bool / (bool && || ^^ ! bool)
//			bool: 1.) (IF/IF P/P M/M) == != < >


class BBBool
{
public:
	enum T_BoolType { IFVar, PVar, MVar, Nothing } type;
	BBBool();
	~BBBool();

	union T_BoolVar
	{
		BBBaumInteger *IF;
		BBBaumMatrixPoint *MP;
	} BoolVar1, BoolVar2;

	enum T_booloperator { Gleich, Ungleich, Kleiner, Groesser, KleinerG, GroesserG} BoolOp;
};

class BBBedingung
{
public:
	enum T_BedingungType { Bool, Und, Oder, XOder, Not, Nothing} type;

	BBBedingung();
	~BBBedingung();

	union T_BedingungVar
	{
		struct BedBool
		{
			BBBool *b;
		} BoolVar;

		struct BedBiOperator
		{
			BBBedingung *b1;
			BBBedingung *b2;
		} BoolBiOp;

		struct BedUniOperator
		{
			BBBedingung *b;
		} BoolUniOp;
	} BedingungVar;
};


class BBIf
{
public:
	BBIf();
	~BBIf();
	
	BBBedingung *b;
	T_AnweisungList z, zelse;
	bool isElse;
};

bool getNextKlammerString(const std::string& statement, int& pos);
bool isBedingung(const std::string& s, BBBedingung * &bed);
bool isIf(const std::string& statement, int& pos, BBIf *& i, std::string& anweisungen, std::string& anweisungen_else);

#endif

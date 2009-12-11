#ifndef __AUSDRUCK_H
#define __AUSDRUCK_H


#include "basistypen.h"

// Baum-Struktur für Integer
class BBFunktion;
struct BBFktExe;
class BBBaumMatrixPoint;

class BBBaumInteger
{
public:
	enum KnotenTyp { NoOp, BIOperator, UniOperator, MIndex, IZahl, FZahl, 
		Funktion, IVar, FVar } typ;
	BBBaumInteger();
	~BBBaumInteger();

	union BBKnoten
	{
		struct BBBiOperator
		{
			enum OperatorType {Plus, Minus, Mal, Geteilt, Hoch, Modulo} OpTyp;
			BBBaumInteger *links;
			BBBaumInteger *rechts;
		} BiOperator;

		struct BBUniOperator
		{
			enum OperatorType {Plus, Minus } OpTyp;
			BBBaumInteger *rechts;
		} UniOperator;

		struct BBMatrixIndex
		{
			BBMatrix *M;
			//BBPoint *P;
			BBBaumMatrixPoint *P;
		} MatrixIndex;

		int IZahl;
		double FZahl;
		BBFktExe *func;
		BBInteger *IVar;
		BBFloat *FVar;

	} k;
	// bi-operator + - * / ^

	// uni-operator - +
	// Integer-Zahl
	// double-Zahl
	// Integer-Variable
	// Float-Variable

	// kommt später:
	// Funktion (später) mit Rückgabewerte Int oder Float

};

bool isKlammer(const std::string& statement);
bool isBiOperator(const std::string& statement, char& c, int& pos);
bool isUniOperator(const std::string& statement, char& c);
bool isMatrixIndex(const std::string& statement, BBMatrix *& bm, BBBaumMatrixPoint *& bp, bool getMem = true);
bool isFZahl(const std::string& statement);
bool isIZahl(const std::string& statement);
bool isFVar (const std::string& statement, BBTyp * &b);
bool isIVar (const std::string& statement, BBTyp * &b);
bool isPVar (const std::string& statement, BBTyp * &b);
bool isMVar (const std::string& statement, BBTyp * &b);
BBFunktion *isFktName(const std::string& s);
bool isFunktion (const std::string& statement, BBFktExe * &b, bool getMem = true, bool AlleFunktionen = true);

void pars_integer_float(const std::string& statement, BBBaumInteger * &Knoten, int getmem = true);



//***************** Punkt und Matrix *****************
// Operator p/p -> + -
// Operator p/i i/p p/f f/p -> * /
// uni - Operator nur für p/f/i
//++++++++++++++ Matrix ++++++++++++++++++++++++++
// Operator M/M + -
// Operator M/i i/M M/f f/M * /
// uni - Operator nur für f/i



class BBBaumMatrixPoint
{
public:
	enum KnotenTyp { NoOp, BIOperator, UniOperator, IFAusdruck,
					MVar, PVar} typ;
	BBBaumMatrixPoint();
	~BBBaumMatrixPoint();

	union BBKnoten
	{
		struct BBBiOperator
		{
			enum OperatorType {Plus, Minus, Mal, Geteilt} OpTyp;
			BBBaumMatrixPoint *links;
			BBBaumMatrixPoint *rechts;
		} BiOperator;

		struct BBUniOperator
		{
			enum OperatorType {Plus, Minus } OpTyp;
			BBBaumMatrixPoint *rechts;
		} UniOperator;

		struct BBIntFloatAusdruck
		{
			BBBaumInteger *b;
		} IntFloatAusdruck;

		BBMatrix *M;
		BBPoint *P;
	} k;
	bool isMatrix;
	// bi-operator + - * / ^
};


void pars_matrix_point(const std::string& statement, 
					   BBBaumMatrixPoint * &Knoten, 
					   bool matrix = true,
					   bool getMem = true );



#endif

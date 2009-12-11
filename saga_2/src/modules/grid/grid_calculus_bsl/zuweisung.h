#ifndef __ZUWEISUNG_H 
#define __ZUWEISUNG_H 

#include "funktion.h"

//***************** Zuweisung *****************
class BBZuweisung
{
public:
	enum BBZuweisungTyp {NoTyp, FTyp, ITyp, PTyp, MTyp, MIndex} typ;
	BBZuweisung();
	~BBZuweisung();

	union BBZuArt
	{
		BBBaumMatrixPoint *MP;
		BBBaumInteger *IF;
	} ZuArt;

	union BBZuVar
	{
		BBInteger *IVar;
		BBFloat *FVar;
		BBPoint *PVar;
		BBMatrix *MVar;
		struct BBMatrixIndex
		{
			BBBaumMatrixPoint *PVar;
			BBMatrix *MVar;
		} MatrixIndex;
	} ZuVar;

};


bool isZuweisung(const std::string& statement, BBZuweisung *&Z);




#endif

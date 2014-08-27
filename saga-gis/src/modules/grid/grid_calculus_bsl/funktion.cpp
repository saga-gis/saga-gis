/**********************************************************
 * Version $Id$
 *********************************************************/

//#include <..\stdafx.h>
#include <iostream>
#include <sstream>

#include "funktion.h"
#include "auswert_zuweisung.h"

#include "diverses.h"
#include "interpolation.h"

#include "funktion_numerisch.h"
#include "funktion_statistisch.h"

#include "bsl_interpreter.h"

using namespace std;

T_FunktionList FunktionList;

BBArgumente::BBArgumente()
{
	ArgTyp.IF = NULL;
	ArgTyp.MP = NULL;
	typ = NoOp;
}
BBArgumente::~BBArgumente()
{
	return;
}

BBFunktion::BBFunktion()
{
}
	
BBFunktion::~BBFunktion()
{		
}

BBFktExe::BBFktExe()
{
	f = 0;
}

BBFktExe::~BBFktExe()
{
	for (int i=0; i<f->args.size(); i++)
	{
		switch (f->args[i].typ)
		{
		case BBArgumente::ITyp:
		case BBArgumente::FTyp:
			if (f->args[i].ArgTyp.IF)
				delete f->args[i].ArgTyp.IF;
			f->args[i].ArgTyp.IF = NULL;	
			break;
		case BBArgumente::MTyp:
		case BBArgumente::PTyp:
			if (f->args[i].ArgTyp.MP)
				delete f->args[i].ArgTyp.MP;
			f->args[i].ArgTyp.MP = NULL;
			break;		
		}
	}
}

//****************************************************************
//				Definition von Funktionen
//****************************************************************
#define __GET_MIN(a, b)				(((a) < (b)) ? (a) : (b))
#define __GET_MAX(a, b)				(((a) > (b)) ? (a) : (b))

class BBFunktion_showMatrix : public BBFunktion
//
// showMatrix: zeigt einen Grid in einem neuen Fenster
// 
{
public:
	BBFunktion_showMatrix()
	{
		name =  "showMatrix";
		// Argumente
		BBArgumente a;
		a.typ = BBArgumente::MTyp;
		args.push_back(a);

		// Return-Typ
		ret.typ = BBArgumente::NoOp;
	}; 
	virtual void fkt(void) 
	{
		
		args[0].ArgTyp.MP->k.M->M->Set_Name(CSG_String((char *)args[0].ArgTyp.MP->k.M->name.c_str()));
		g_Add_Grid( (args[0].ArgTyp.MP->k.M->M) );
		
	};

};


class BBFunktion_saveMatrix : public BBFunktion
//
// saveMatrix: zeigt einen Grid in einem neuen Fenster
// 
{
public:
	BBFunktion_saveMatrix()
	{
		name =  "saveMatrix";
		// Argumente: Matrix, Pfad
		BBArgumente a;
		a.typ = BBArgumente::MTyp;
		args.push_back(a);
		a.typ = BBArgumente::ITyp;
		args.push_back(a);

		// Return-Typ
		ret.typ = BBArgumente::NoOp;
	}; 
	virtual void fkt(void) 
	{
		if (args[0].ArgTyp.MP->typ != BBBaumMatrixPoint::MVar)
			throw BBFehlerAusfuehren();

		int filename_number = auswert_integer(*(args[1].ArgTyp.IF));
		char filename[40];
		sprintf(filename, "OutputGrid%03d.grd", filename_number);
	
		args[0].ArgTyp.MP->k.M->M->Save(filename,2);


	};

};

class BBFunktion_getMemory : public BBFunktion
//
// getMemory: ruft GridWerte.getMem auf
// 
{
public:
	BBFunktion_getMemory()
	{
		name =  "getMemory";
		// Argumente
		BBArgumente a;
		a.typ = BBArgumente::MTyp; // x
		args.push_back(a);
		a.typ = BBArgumente::ITyp; // x
		args.push_back(a);
		a.typ = BBArgumente::ITyp; // y
		args.push_back(a);

		// Return-Typ
		ret.typ = BBArgumente::NoOp; // kein Return-Wert
	}; 
	virtual void fkt(void) 
	{
		int x, y;
		x = auswert_integer(*(args[1].ArgTyp.IF));
		y = auswert_integer(*(args[2].ArgTyp.IF));

		// MP darf nur aus einer Matrix bestehen
		if (args[0].ArgTyp.MP->typ != BBBaumMatrixPoint::MVar)
			throw BBFehlerAusfuehren();

		args[0].ArgTyp.MP->k.M->M->xanz = x;
		args[0].ArgTyp.MP->k.M->M->yanz = y;
		args[0].ArgTyp.MP->k.M->M->getMem();
	};
	BBBaumInteger baum;
};

class BBFunktion_max3 : public BBFunktion
//
// max3: ruft (Diverses::) max3 auf
// 
{
public:
	BBFunktion_max3()
	{
		name =  "max3";
		// Argumente
		BBArgumente a;
		a.typ = BBArgumente::FTyp; // x1
		args.push_back(a);
		a.typ = BBArgumente::FTyp; // x2
		args.push_back(a);
		a.typ = BBArgumente::FTyp; // x3
		args.push_back(a);

		// Return-Typ
		ret.typ = BBArgumente::FTyp; 
		ret.ArgTyp.IF = new BBBaumInteger;
		ret.ArgTyp.IF->typ = BBBaumInteger::FZahl;
		ret.ArgTyp.IF->k.FZahl = 0;
	}; 
	~BBFunktion_max3()
	{
		delete ret.ArgTyp.IF;
	}
	virtual void fkt(void) 
	{
		double x, y, z;
		x = auswert_float(*(args[0].ArgTyp.IF));
		y = auswert_float(*(args[1].ArgTyp.IF));
		z = auswert_float(*(args[2].ArgTyp.IF));

		ret.ArgTyp.IF->k.FZahl = max3(x,y,z);
	};
};


class BBFunktion_min3 : public BBFunktion
//
// min3: ruft (Diverses::) min3 auf
// 
{
public:
	BBFunktion_min3()
	{
		name =  "min3";
		// Argumente
		BBArgumente a;
		a.typ = BBArgumente::FTyp; // x1
		args.push_back(a);
		a.typ = BBArgumente::FTyp; // x2
		args.push_back(a);
		a.typ = BBArgumente::FTyp; // x3
		args.push_back(a);

		// Return-Typ
		ret.typ = BBArgumente::FTyp; 
		ret.ArgTyp.IF = new BBBaumInteger;
		ret.ArgTyp.IF->typ = BBBaumInteger::FZahl;
		ret.ArgTyp.IF->k.FZahl = 0;
	}; 
	~BBFunktion_min3()
	{
		delete ret.ArgTyp.IF;
	}
	virtual void fkt(void) 
	{
		double x, y, z;
		x = auswert_float(*(args[0].ArgTyp.IF));
		y = auswert_float(*(args[1].ArgTyp.IF));
		z = auswert_float(*(args[2].ArgTyp.IF));

		ret.ArgTyp.IF->k.FZahl = min3(x,y,z);
	};
};


class BBFunktion_max8 : public BBFunktion
//
// max8: berechnet das Maximum aller Nachbarn
// 
{
public:
	BBFunktion_max8()
	{
		name =  "max8";
		// Argumente
		BBArgumente a;
		a.typ = BBArgumente::PTyp; // x1
		args.push_back(a);
		a.typ = BBArgumente::MTyp; // x2
		args.push_back(a);

		// Return-Typ
		ret.typ = BBArgumente::FTyp; 
		ret.ArgTyp.IF = new BBBaumInteger;
		ret.ArgTyp.IF->typ = BBBaumInteger::FZahl;
		ret.ArgTyp.IF->k.FZahl = 0;
	}; 
	~BBFunktion_max8()
	{
		delete ret.ArgTyp.IF;
	}
	virtual void fkt(void) 
	{
		GridWerte *W;

		// MP darf nur aus einer Matrix bestehen
		if (args[1].ArgTyp.MP->typ != BBBaumMatrixPoint::MVar)
			throw BBFehlerAusfuehren("Funktion >max8<");
		W = args[1].ArgTyp.MP->k.M->M;

		// get point p
		bool ret1;
		T_Point p;
		double f;
		ret1 = auswert_point(*(args[0].ArgTyp.MP), p, f);
		if (!ret1)
			throw BBFehlerAusfuehren("Funktion >max8<");


		double hoehe = -1e30f;
		for (int i=-1; i<=1; i++)
		{
			for (int j=-1; j<=1; j++)
			{
				int x = p.x + i;
				int y = p.y + j;
				if (innerhalb(x, y, *W) && (i != 0 || j != 0))
				{	
					hoehe = __GET_MAX(hoehe, (*W)(x,y) );
				}
			}
		}
		ret.ArgTyp.IF->k.FZahl = hoehe;
	};
};

class BBFunktion_min8 : public BBFunktion
//
// max8: berechnet das Maximum aller Nachbarn
// 
{
public:
	BBFunktion_min8()
	{
		name =  "min8";
		// Argumente
		BBArgumente a;
		a.typ = BBArgumente::PTyp; // x1
		args.push_back(a);
		a.typ = BBArgumente::MTyp; // x2
		args.push_back(a);

		// Return-Typ
		ret.typ = BBArgumente::FTyp; 
		ret.ArgTyp.IF = new BBBaumInteger;
		ret.ArgTyp.IF->typ = BBBaumInteger::FZahl;
		ret.ArgTyp.IF->k.FZahl = 0;
	}; 
	~BBFunktion_min8()
	{
		delete ret.ArgTyp.IF;
	}
	virtual void fkt(void) 
	{
		GridWerte *W;

		// MP darf nur aus einer Matrix bestehen
		if (args[1].ArgTyp.MP->typ != BBBaumMatrixPoint::MVar)
			throw BBFehlerAusfuehren("Funktion >max8<");
		W = args[1].ArgTyp.MP->k.M->M;

		// get point p
		bool ret1;
		T_Point p;
		double f;
		ret1 = auswert_point(*(args[0].ArgTyp.MP), p, f);
		if (!ret1)
			throw BBFehlerAusfuehren("Funktion >max8<");


		double hoehe = 1e30f;
		for (int i=-1; i<=1; i++)
		{
			for (int j=-1; j<=1; j++)
			{
				int x = p.x + i;
				int y = p.y + j;
				if (innerhalb(x, y, *W) && (i != 0 || j != 0))
				{	
					hoehe = __GET_MIN(hoehe, (*W)(x,y));
				}
			}
		}
		ret.ArgTyp.IF->k.FZahl = hoehe;
	};
};

class BBFunktion_max9 : public BBFunktion
//
// max8: berechnet das Maximum aller Nachbarn
// 
{
public:
	BBFunktion_max9()
	{
		name =  "max9";
		// Argumente
		BBArgumente a;
		a.typ = BBArgumente::PTyp; // x1
		args.push_back(a);
		a.typ = BBArgumente::MTyp; // x2
		args.push_back(a);

		// Return-Typ
		ret.typ = BBArgumente::FTyp; 
		ret.ArgTyp.IF = new BBBaumInteger;
		ret.ArgTyp.IF->typ = BBBaumInteger::FZahl;
		ret.ArgTyp.IF->k.FZahl = 0;
	}; 
	~BBFunktion_max9()
	{
		delete ret.ArgTyp.IF;
	}
	virtual void fkt(void) 
	{
		GridWerte *W;

		// MP darf nur aus einer Matrix bestehen
		if (args[1].ArgTyp.MP->typ != BBBaumMatrixPoint::MVar)
			throw BBFehlerAusfuehren("Funktion >max8<");
		W = args[1].ArgTyp.MP->k.M->M;

		// get point p
		bool ret1;
		T_Point p;
		double f;
		ret1 = auswert_point(*(args[0].ArgTyp.MP), p, f);
		if (!ret1)
			throw BBFehlerAusfuehren("Funktion >max8<");


		double hoehe = -1e30f;
		for (int i=-1; i<=1; i++)
		{
			for (int j=-1; j<=1; j++)
			{
				int x = p.x + i;
				int y = p.y + j;
				if (innerhalb(x, y, *W))
				{	
					hoehe = __GET_MAX(hoehe, (*W)(x,y));
				}
			}
		}
		ret.ArgTyp.IF->k.FZahl = hoehe;
	};
};

class BBFunktion_min9 : public BBFunktion
//
// max8: berechnet das Maximum aller Nachbarn
// 
{
public:
	BBFunktion_min9()
	{
		name =  "min9";
		// Argumente
		BBArgumente a;
		a.typ = BBArgumente::PTyp; // x1
		args.push_back(a);
		a.typ = BBArgumente::MTyp; // x2
		args.push_back(a);

		// Return-Typ
		ret.typ = BBArgumente::FTyp; 
		ret.ArgTyp.IF = new BBBaumInteger;
		ret.ArgTyp.IF->typ = BBBaumInteger::FZahl;
		ret.ArgTyp.IF->k.FZahl = 0;
	}; 
	~BBFunktion_min9()
	{
		delete ret.ArgTyp.IF;
	}
	virtual void fkt(void) 
	{
		GridWerte *W;

		// MP darf nur aus einer Matrix bestehen
		if (args[1].ArgTyp.MP->typ != BBBaumMatrixPoint::MVar)
			throw BBFehlerAusfuehren("Funktion >max8<");
		W = args[1].ArgTyp.MP->k.M->M;

		// get point p
		bool ret1;
		T_Point p;
		double f;
		ret1 = auswert_point(*(args[0].ArgTyp.MP), p, f);
		if (!ret1)
			throw BBFehlerAusfuehren("Funktion >max8<");


		double hoehe = 1e30f;
		for (int i=-1; i<=1; i++)
		{
			for (int j=-1; j<=1; j++)
			{
				int x = p.x + i;
				int y = p.y + j;
				if (innerhalb(x, y, *W))
				{	
					hoehe = __GET_MIN(hoehe, (*W)(x,y));
				}
			}
		}
		ret.ArgTyp.IF->k.FZahl = hoehe;
	};
};

class BBFunktion_isRand : public BBFunktion
//
// ruft (Diverses::) isRand auf
// 
{
public:
	BBFunktion_isRand()
	{
		name =  "isRand";
		// Argumente
		BBArgumente a;
		a.typ = BBArgumente::PTyp; // x1
		args.push_back(a);
		a.typ = BBArgumente::MTyp; // x2
		args.push_back(a);

		// Return-Typ
		ret.typ = BBArgumente::ITyp; 
		ret.ArgTyp.IF = new BBBaumInteger;
		ret.ArgTyp.IF->typ = BBBaumInteger::IZahl;
		ret.ArgTyp.IF->k.IZahl = 0;
	}; 
	~BBFunktion_isRand()
	{
		delete ret.ArgTyp.IF;
	}
	virtual void fkt(void) 
	{
		int x, y;

		// MP darf nur aus einer Matrix bestehen
		if (args[1].ArgTyp.MP->typ != BBBaumMatrixPoint::MVar)
			throw BBFehlerAusfuehren("Funktion >isRand<");
		bool ret1;
		T_Point p;
		double f;
		ret1 = auswert_point(*(args[0].ArgTyp.MP), p, f);
		if (!ret1)
			throw BBFehlerAusfuehren("Funktion >isRand<");
		x = args[1].ArgTyp.MP->k.M->M->xanz;
		y = args[1].ArgTyp.MP->k.M->M->yanz;
		ret.ArgTyp.IF->k.IZahl = ( (p.x <= 0 || p.y <= 0 || p.x >= x-1 || p.y >= y-1) ? 1 : 0);
	};
};



class BBFunktion_setRandN : public BBFunktion
//
// setRandN : setzt alle Randpunkte auf den nächsten Nachbarn
// 
{
public:
	BBFunktion_setRandN()
	{
		name =  "setRandN";
		// Argumente
		BBArgumente a;
		a.typ = BBArgumente::MTyp; // x2
		args.push_back(a);

		// Return-Typ
		ret.typ = BBArgumente::NoOp;
	}; 
	~BBFunktion_setRandN()
	{
	}
	virtual void fkt(void) 
	{
		int x, y;

		// MP darf nur aus einer Matrix bestehen
		if (args[0].ArgTyp.MP->typ != BBBaumMatrixPoint::MVar)
			throw BBFehlerAusfuehren("Funktion >setRandN<");
		GridWerte *G = args[0].ArgTyp.MP->k.M->M;
		x = 0;
		for (y=1; y<G->yanz-1; y++)
			G->Set_Value(x,y, G->asDouble(x+1,y));
		x = G->xanz-1;
		for (y=1; y<G->yanz-1; y++)
			G->Set_Value(x,y, G->asDouble(x-1,y));
		y = 0;
		for (x=1; x<G->xanz-1; x++)
			G->Set_Value(x,y, G->asDouble(x,y+1));
		y = G->yanz-1;
		for (x=1; x<G->xanz-1; x++)
			G->Set_Value(x,y, G->asDouble(x,y-1));

		// die vier Randpunkte setzen
		G->Set_Value(        0,        0, G->asDouble(        1,        1));
		G->Set_Value(G->xanz-1,        0, G->asDouble(G->xanz-2,        1));
		G->Set_Value(        0,G->yanz-1, G->asDouble(        1,G->yanz-2));
		G->Set_Value(G->xanz-1,G->yanz-1, G->asDouble(G->xanz-2,G->yanz-2));
	};
};


class BBFunktion_setRandI : public BBFunktion
//
// setRandI : setzt alle Randpunkte durch lineare extrapolation
// 
{
public:
	BBFunktion_setRandI()
	{
		name =  "setRandI";
		// Argumente
		BBArgumente a;
		a.typ = BBArgumente::MTyp; // x2
		args.push_back(a);

		// Return-Typ
		ret.typ = BBArgumente::NoOp;
	}; 
	~BBFunktion_setRandI()
	{
	}
	virtual void fkt(void) 
	{
		// MP darf nur aus einer Matrix bestehen
		if (args[0].ArgTyp.MP->typ != BBBaumMatrixPoint::MVar)
			throw BBFehlerAusfuehren("Funktion >setRandN<");
		GridWerte *G = args[0].ArgTyp.MP->k.M->M;
		GridWerte H;
		H = *G;
		H.xanz -= 2;
		H.yanz -= 2;
		H.xll += G->dxy;
		H.yll += G->dxy;
		H.getMem();
		for (int i=1; i<G->yanz-1; i++)
			for (int j=1; j<G->xanz-1; j++)
				H.Set_Value(j-1,i-1, G->asDouble(j,j));
		LinRand(H, *(args[0].ArgTyp.MP->k.M->M));
	};
};


class BBFunktion_showValue : public BBFunktion
//
// setRandI : setzt alle Randpunkte durch lineare extrapolation
// 
{
public:
	BBFunktion_showValue()
	{
		name =  "showValue";
		// Argumente
		BBArgumente a;
		a.typ = BBArgumente::FTyp; // x2
		args.push_back(a);

		// Return-Typ
		ret.typ = BBArgumente::NoOp;
	}; 
	~BBFunktion_showValue()
	{
	}
	virtual void fkt(void) 
	{
		string s("");
		ostringstream os1(s);
		double x;
		x = auswert_float(*(args[0].ArgTyp.IF));
		os1 << "Value = " << x << ends;

		//Hier in output frnster
//		AfxMessageBox(os1.str().data());
	};
};
/*
class BBFunktion_spline : public BBFunktion
//
// ruft (Diverses::) isRand auf
// 
{
public:
	BBFunktion_spline()
	{
		name =  "spline";
		// Argumente
		BBArgumente a;
		a.typ = BBArgumente::MTyp; // x1
		args.push_back(a);
		a.typ = BBArgumente::FTyp; // x2: faktor
		args.push_back(a);
		a.typ = BBArgumente::MTyp; // x3: neues Grid
		args.push_back(a);

		// Return-Typ
		ret.typ = BBArgumente::NoOp;
	}; 
	~BBFunktion_spline()
	{
	}
	virtual void fkt(void) 
	{
		int x, y;

		// MP darf nur aus einer Matrix bestehen
		if (args[0].ArgTyp.MP->typ != BBBaumMatrixPoint::MVar)
			throw BBFehlerAusfuehren("Funktion >spline<");
		// MP darf nur aus einer Matrix bestehen
		if (args[2].ArgTyp.MP->typ != BBBaumMatrixPoint::MVar)
			throw BBFehlerAusfuehren("Funktion >spline<");

		// Übergabe-Parameter extrahieren
		GridWerte *sourceGrid = args[0].ArgTyp.MP->k.M->M;
		GridWerte *destGrid   = args[2].ArgTyp.MP->k.M->M;
		double factor = auswert_float(*(args[1].ArgTyp.IF));

		// Anzahl der Punkte berechnen
		// z.B. Abstand: 3 (=0, 1, 2, 3), faktor 0.4
		// --> 0, 0.4, 0.8, 1.2, 1.6, 2.0, 2.4, 2.8
		// aber: 3 / 0.4 = 30 / 4 = 7.5  = 7 + 1
		// oder
		//	Abstand: 2 (0, 1, 2), faktor 0.4
		// --> 0, 0.4, 0.8, 1.2, 1.6, 2.0
		// aber 2 / 0.4 = 5 
		//  Abstand 1 (0, 1), faktor 0.4
		//--> 0, 0,4, 0.8
		//  aber 1 / 0.4 = 2,5
		//
		// ==> Formel: floor((xanz-1)/faktor))+1

		x = floor( (sourceGrid->xanz-1)/factor ) + 1;
		y = floor( (sourceGrid->yanz-1)/factor ) + 1;

		Interpolation *I;
		I = new QSpline(*sourceGrid);
		I->setParams(0, 0, factor, x, y); // von 0,0 in factor-Schritten über x,y Punkte
		I->interpol(*destGrid);
		delete I;
		destGrid->calcMinMax();
		
	};
};
*/
//****************************************************************
//			Initialisierung der Funktionen
//****************************************************************

void InitFunktionen(void)
{
	FunktionList.push_back(new BBFunktion_showMatrix() );
	FunktionList.push_back(new BBFunktion_saveMatrix() );
	FunktionList.push_back(new BBFunktion_getMemory() );
	FunktionList.push_back(new BBFunktion_max3() );
	FunktionList.push_back(new BBFunktion_min3() );
	FunktionList.push_back(new BBFunktion_max8() );
	FunktionList.push_back(new BBFunktion_min8() );
	FunktionList.push_back(new BBFunktion_max9() );
	FunktionList.push_back(new BBFunktion_min9() );
	FunktionList.push_back(new BBFunktion_isRand() );
	FunktionList.push_back(new BBFunktion_setRandN() );
	FunktionList.push_back(new BBFunktion_setRandI() );
	FunktionList.push_back(new BBFunktion_showValue() );
//	FunktionList.push_back(new BBFunktion_spline() );


	// aus funktion_numerisch.h
	FunktionList.push_back(new BBFunktion_sin() );
	FunktionList.push_back(new BBFunktion_cos() );
	FunktionList.push_back(new BBFunktion_tan() );
	FunktionList.push_back(new BBFunktion_sinargs() );
	FunktionList.push_back(new BBFunktion_cosargs() );
	FunktionList.push_back(new BBFunktion_tanargs() );
	FunktionList.push_back(new BBFunktion_asin() );
	FunktionList.push_back(new BBFunktion_acos() );
	FunktionList.push_back(new BBFunktion_atan() );
	FunktionList.push_back(new BBFunktion_asinargs() );
	FunktionList.push_back(new BBFunktion_acosargs() );
	FunktionList.push_back(new BBFunktion_atanargs() );
	FunktionList.push_back(new BBFunktion_log() );
	FunktionList.push_back(new BBFunktion_ln() );
	FunktionList.push_back(new BBFunktion_exp() );

	// aus funktion_statistisch.h
	FunktionList.push_back(new BBFunktion_setStatistikDaten() );
	FunktionList.push_back(new BBFunktion_resetStatistikDaten() );
	FunktionList.push_back(new BBFunktion_calcMittelwert() );
	FunktionList.push_back(new BBFunktion_calcVarianz() );

}

void DeleteFunktionen(void)
{
	T_FunktionList::iterator it;
	for (it = FunktionList.begin(); it != FunktionList.end(); it++)
	{
		delete (*it);
	}
}

class InitFunktionenOnce
{
public:
	InitFunktionenOnce()
	{
		InitFunktionen();
	}
	~InitFunktionenOnce()
	{
		DeleteFunktionen();
	}
};

InitFunktionenOnce initfunktionenonce;

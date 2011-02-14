/**********************************************************
 * Version $Id$
 *********************************************************/
#ifndef __FUNKTION_NUMERISCH_H 
#define __FUNKTION_NUMERISCH_H 

#include <math.h>


//****************** SINUS **************************
class BBFunktion_sin : public BBFunktion
//
// sin: ruft sinus(...) auf
// 
{
public:
	BBFunktion_sin()
	{
		name =  "sin";
		// Argumente
		BBArgumente a;
		a.typ = BBArgumente::FTyp; // x1
		args.push_back(a);

		// Return-Typ
		ret.typ = BBArgumente::FTyp; 
		ret.ArgTyp.IF = new BBBaumInteger;
		ret.ArgTyp.IF->typ = BBBaumInteger::FZahl;
		ret.ArgTyp.IF->k.FZahl = 0;
	}; 
	~BBFunktion_sin()
	{
		delete ret.ArgTyp.IF;
	}
	virtual void fkt(void) 
	{
		double x;
		x = auswert_float(*(args[0].ArgTyp.IF));

		ret.ArgTyp.IF->k.FZahl = sin(x);
	};
};

//****************** COSINUS **************************
class BBFunktion_cos : public BBFunktion
//
// cos: ruft cosinus(...) auf
// 
{
public:
	BBFunktion_cos()
	{
		name =  "cos";
		// Argumente
		BBArgumente a;
		a.typ = BBArgumente::FTyp; // x1
		args.push_back(a);

		// Return-Typ
		ret.typ = BBArgumente::FTyp; 
		ret.ArgTyp.IF = new BBBaumInteger;
		ret.ArgTyp.IF->typ = BBBaumInteger::FZahl;
		ret.ArgTyp.IF->k.FZahl = 0;
	}; 
	~BBFunktion_cos()
	{
		delete ret.ArgTyp.IF;
	}
	virtual void fkt(void) 
	{
		double x;
		x = auswert_float(*(args[0].ArgTyp.IF));

		ret.ArgTyp.IF->k.FZahl = cos(x);
	};
};

//****************** TANGENS **************************
class BBFunktion_tan : public BBFunktion
//
// tan: ruft tangens(...) auf
// 
{
public:
	BBFunktion_tan()
	{
		name =  "tan";
		// Argumente
		BBArgumente a;
		a.typ = BBArgumente::FTyp; // x1
		args.push_back(a);

		// Return-Typ
		ret.typ = BBArgumente::FTyp; 
		ret.ArgTyp.IF = new BBBaumInteger;
		ret.ArgTyp.IF->typ = BBBaumInteger::FZahl;
		ret.ArgTyp.IF->k.FZahl = 0;
	}; 
	~BBFunktion_tan()
	{
		delete ret.ArgTyp.IF;
	}
	virtual void fkt(void) 
	{
		double x;
		x = auswert_float(*(args[0].ArgTyp.IF));

		ret.ArgTyp.IF->k.FZahl = tan(x);
	};
};

//****************** SINUS **************************
class BBFunktion_sinargs : public BBFunktion
//
// sin: ruft sinus(...) auf
// 
{
public:
	BBFunktion_sinargs()
	{
		name =  "sinargs";
		// Argumente
		BBArgumente a;
		a.typ = BBArgumente::FTyp; // x1
		args.push_back(a);

		// Return-Typ
		ret.typ = BBArgumente::FTyp; 
		ret.ArgTyp.IF = new BBBaumInteger;
		ret.ArgTyp.IF->typ = BBBaumInteger::FZahl;
		ret.ArgTyp.IF->k.FZahl = 0;
	}; 
	~BBFunktion_sinargs()
	{
		delete ret.ArgTyp.IF;
	}
	virtual void fkt(void) 
	{
		double x;
		x = auswert_float(*(args[0].ArgTyp.IF));

		ret.ArgTyp.IF->k.FZahl = sin(x*M_PI/180.0);
	};
};

//****************** COSINUS **************************
class BBFunktion_cosargs : public BBFunktion
//
// cos: ruft cosinus(...) auf
// 
{
public:
	BBFunktion_cosargs()
	{
		name =  "cosargs";
		// Argumente
		BBArgumente a;
		a.typ = BBArgumente::FTyp; // x1
		args.push_back(a);

		// Return-Typ
		ret.typ = BBArgumente::FTyp; 
		ret.ArgTyp.IF = new BBBaumInteger;
		ret.ArgTyp.IF->typ = BBBaumInteger::FZahl;
		ret.ArgTyp.IF->k.FZahl = 0;
	}; 
	~BBFunktion_cosargs()
	{
		delete ret.ArgTyp.IF;
	}
	virtual void fkt(void) 
	{
		double x;
		x = auswert_float(*(args[0].ArgTyp.IF));

		ret.ArgTyp.IF->k.FZahl = cos(x*M_PI/180.0);
	};
};

//****************** TANGENS **************************
class BBFunktion_tanargs : public BBFunktion
//
// tan: ruft tangens(...) auf
// 
{
public:
	BBFunktion_tanargs()
	{
		name =  "tanargs";
		// Argumente
		BBArgumente a;
		a.typ = BBArgumente::FTyp; // x1
		args.push_back(a);

		// Return-Typ
		ret.typ = BBArgumente::FTyp; 
		ret.ArgTyp.IF = new BBBaumInteger;
		ret.ArgTyp.IF->typ = BBBaumInteger::FZahl;
		ret.ArgTyp.IF->k.FZahl = 0;
	}; 
	~BBFunktion_tanargs()
	{
		delete ret.ArgTyp.IF;
	}
	virtual void fkt(void) 
	{
		double x;
		x = auswert_float(*(args[0].ArgTyp.IF));

		ret.ArgTyp.IF->k.FZahl = tan(x*M_PI/180.0);
	};
};


//****************** SINUS **************************
class BBFunktion_asin : public BBFunktion
//
// sin: ruft arcussinus(...) auf
// 
{
public:
	BBFunktion_asin()
	{
		name =  "asin";
		// Argumente
		BBArgumente a;
		a.typ = BBArgumente::FTyp; // x1
		args.push_back(a);

		// Return-Typ
		ret.typ = BBArgumente::FTyp; 
		ret.ArgTyp.IF = new BBBaumInteger;
		ret.ArgTyp.IF->typ = BBBaumInteger::FZahl;
		ret.ArgTyp.IF->k.FZahl = 0;
	}; 
	~BBFunktion_asin()
	{
		delete ret.ArgTyp.IF;
	}
	virtual void fkt(void) 
	{
		double x;
		x = auswert_float(*(args[0].ArgTyp.IF));

		ret.ArgTyp.IF->k.FZahl = asin(x);
	};
};

//****************** COSINUS **************************
class BBFunktion_acos : public BBFunktion
//
// acos: ruft arcuscosinus(...) auf
// 
{
public:
	BBFunktion_acos()
	{
		name =  "acos";
		// Argumente
		BBArgumente a;
		a.typ = BBArgumente::FTyp; // x1
		args.push_back(a);

		// Return-Typ
		ret.typ = BBArgumente::FTyp; 
		ret.ArgTyp.IF = new BBBaumInteger;
		ret.ArgTyp.IF->typ = BBBaumInteger::FZahl;
		ret.ArgTyp.IF->k.FZahl = 0;
	}; 
	~BBFunktion_acos()
	{
		delete ret.ArgTyp.IF;
	}
	virtual void fkt(void) 
	{
		double x;
		x = auswert_float(*(args[0].ArgTyp.IF));

		ret.ArgTyp.IF->k.FZahl = acos(x);
	};
};

	
//****************** TANGENS **************************
class BBFunktion_atan : public BBFunktion
//
// tan: ruft tangens(...) auf
// 
{
public:
	BBFunktion_atan()
	{
		name =  "atan";
		// Argumente
		BBArgumente a;
		a.typ = BBArgumente::FTyp; // x1
		args.push_back(a);

		// Return-Typ
		ret.typ = BBArgumente::FTyp; 
		ret.ArgTyp.IF = new BBBaumInteger;
		ret.ArgTyp.IF->typ = BBBaumInteger::FZahl;
		ret.ArgTyp.IF->k.FZahl = 0;
	}; 
	~BBFunktion_atan()
	{
		delete ret.ArgTyp.IF;
	}
	virtual void fkt(void) 
	{
		double x;
		x = auswert_float(*(args[0].ArgTyp.IF));

		ret.ArgTyp.IF->k.FZahl = atan(x);
	};
};

//****************** SINUS **************************
class BBFunktion_asinargs : public BBFunktion
//
// asin: ruft sinus(...) auf
// 
{
public:
	BBFunktion_asinargs()
	{
		name =  "asinargs";
		// Argumente
		BBArgumente a;
		a.typ = BBArgumente::FTyp; // x1
		args.push_back(a);

		// Return-Typ
		ret.typ = BBArgumente::FTyp; 
		ret.ArgTyp.IF = new BBBaumInteger;
		ret.ArgTyp.IF->typ = BBBaumInteger::FZahl;
		ret.ArgTyp.IF->k.FZahl = 0;
	}; 
	~BBFunktion_asinargs()
	{
		delete ret.ArgTyp.IF;
	}
	virtual void fkt(void) 
	{
		double x;
		x = auswert_float(*(args[0].ArgTyp.IF));

		ret.ArgTyp.IF->k.FZahl = asin(x)*180.0/M_PI;
	};
};

//****************** COSINUS **************************
class BBFunktion_acosargs : public BBFunktion
//
// cos: ruft cosinus(...) auf
// 
{
public:
	BBFunktion_acosargs()
	{
		name =  "acosargs";
		// Argumente
		BBArgumente a;
		a.typ = BBArgumente::FTyp; // x1
		args.push_back(a);

		// Return-Typ
		ret.typ = BBArgumente::FTyp; 
		ret.ArgTyp.IF = new BBBaumInteger;
		ret.ArgTyp.IF->typ = BBBaumInteger::FZahl;
		ret.ArgTyp.IF->k.FZahl = 0;
	}; 
	~BBFunktion_acosargs()
	{
		delete ret.ArgTyp.IF;
	}
	virtual void fkt(void) 
	{
		double x;
		x = auswert_float(*(args[0].ArgTyp.IF));

		ret.ArgTyp.IF->k.FZahl = acos(x)/M_PI*180.0;
	};
};

	
//****************** TANGENS **************************
class BBFunktion_atanargs : public BBFunktion
//
// tan: ruft tangens(...) auf
// 
{
public:
	BBFunktion_atanargs()
	{
		name =  "atanargs";
		// Argumente
		BBArgumente a;
		a.typ = BBArgumente::FTyp; // x1
		args.push_back(a);

		// Return-Typ
		ret.typ = BBArgumente::FTyp; 
		ret.ArgTyp.IF = new BBBaumInteger;
		ret.ArgTyp.IF->typ = BBBaumInteger::FZahl;
		ret.ArgTyp.IF->k.FZahl = 0;
	}; 
	~BBFunktion_atanargs()
	{
		delete ret.ArgTyp.IF;
	}
	virtual void fkt(void) 
	{
		double x;
		x = auswert_float(*(args[0].ArgTyp.IF));
		ret.ArgTyp.IF->k.FZahl = atan(x)/M_PI*180.0;
	};
};

//****************** LOG **************************
class BBFunktion_log : public BBFunktion
//
// ruft log10(...) auf
// 
{
public:
	BBFunktion_log()
	{
		name =  "log";
		// Argumente
		BBArgumente a;
		a.typ = BBArgumente::FTyp; // x1
		args.push_back(a);

		// Return-Typ
		ret.typ = BBArgumente::FTyp; 
		ret.ArgTyp.IF = new BBBaumInteger;
		ret.ArgTyp.IF->typ = BBBaumInteger::FZahl;
		ret.ArgTyp.IF->k.FZahl = 0;
	}; 
	~BBFunktion_log()
	{
		delete ret.ArgTyp.IF;
	}
	virtual void fkt(void) 
	{
		double x;
		x = auswert_float(*(args[0].ArgTyp.IF));
		if (x < 0)
			throw	BBFehlerAusfuehren("Argument vom Logarithmus ist negativ!");
		ret.ArgTyp.IF->k.FZahl = log10(x);
	};
};

//****************** LN **************************
class BBFunktion_ln : public BBFunktion
//
// ruft log(...) auf
// 
{
public:
	BBFunktion_ln()
	{
		name =  "ln";
		// Argumente
		BBArgumente a;
		a.typ = BBArgumente::FTyp; // x1
		args.push_back(a);

		// Return-Typ
		ret.typ = BBArgumente::FTyp; 
		ret.ArgTyp.IF = new BBBaumInteger;
		ret.ArgTyp.IF->typ = BBBaumInteger::FZahl;
		ret.ArgTyp.IF->k.FZahl = 0;
	}; 
	~BBFunktion_ln()
	{
		delete ret.ArgTyp.IF;
	}
	virtual void fkt(void) 
	{
		double x;
		x = auswert_float(*(args[0].ArgTyp.IF));
		if (x < 0)
			throw	BBFehlerAusfuehren("Argument vom Logarithmus ist negativ!");
		ret.ArgTyp.IF->k.FZahl = log(x);
	};
};


//****************** LN **************************
class BBFunktion_exp : public BBFunktion
//
// ruft exp(...) auf
// 
{
public:
	BBFunktion_exp()
	{
		name =  "exp";
		// Argumente
		BBArgumente a;
		a.typ = BBArgumente::FTyp; // x1
		args.push_back(a);

		// Return-Typ
		ret.typ = BBArgumente::FTyp; 
		ret.ArgTyp.IF = new BBBaumInteger;
		ret.ArgTyp.IF->typ = BBBaumInteger::FZahl;
		ret.ArgTyp.IF->k.FZahl = 0;
	}; 
	~BBFunktion_exp()
	{
		delete ret.ArgTyp.IF;
	}
	virtual void fkt(void) 
	{
		double x;
		x = auswert_float(*(args[0].ArgTyp.IF));
		ret.ArgTyp.IF->k.FZahl = exp(x);
	};
};



#endif

/**********************************************************
 * Version $Id$
 *********************************************************/
#ifndef __FUNKTION_STATISTISCH_H 
#define __FUNKTION_STATISTISCH_H 


std::vector<double> StatistikVektor;


//****************** setStatistikDaten **************************
class BBFunktion_setStatistikDaten : public BBFunktion
//
// setzt den Vektor, von dem die Statistik berechnet wird
// 
{
public:
	BBFunktion_setStatistikDaten()
	{
		name =  "setStatistikDaten";
		// Argumente
		BBArgumente a;
		a.typ = BBArgumente::FTyp; // x1
		args.push_back(a);

		ret.typ = BBArgumente::NoOp; 
	}; 
	~BBFunktion_setStatistikDaten()
	{
	}
	virtual void fkt(void) 
	{
		double x;
		x = auswert_float(*(args[0].ArgTyp.IF));
		StatistikVektor.push_back(x);
	};
};


class BBFunktion_resetStatistikDaten : public BBFunktion
//
// löscht den Vektor, von dem die Statistik berechnet wird
// 
{
public:
	BBFunktion_resetStatistikDaten()
	{
		name =  "resetStatistikDaten";
		ret.typ = BBArgumente::NoOp; 
	}; 
	~BBFunktion_resetStatistikDaten()
	{
	}
	virtual void fkt(void) 
	{
		StatistikVektor.clear();
	};
};


class BBFunktion_calcMittelwert : public BBFunktion
//
// 
{
public:
	BBFunktion_calcMittelwert()
	{
		name =  "calcMittelwert";
		ret.typ = BBArgumente::FTyp; 
		ret.ArgTyp.IF = new BBBaumInteger;
		ret.ArgTyp.IF->typ = BBBaumInteger::FZahl;
		ret.ArgTyp.IF->k.FZahl = 0;
	}; 
	~BBFunktion_calcMittelwert()
	{
		delete ret.ArgTyp.IF;
	}
	virtual void fkt(void) 
	{
		double summe = 0.0f;
		for (int i=0; i<StatistikVektor.size(); i++)
			summe += StatistikVektor[i];
		summe /= StatistikVektor.size();
		ret.ArgTyp.IF->k.FZahl = summe;		
	};
};

class BBFunktion_calcVarianz : public BBFunktion
 
{
public:
	BBFunktion_calcVarianz()
	{
		name =  "calcVarianz";
		ret.typ = BBArgumente::FTyp; 
		ret.ArgTyp.IF = new BBBaumInteger;
		ret.ArgTyp.IF->typ = BBBaumInteger::FZahl;
		ret.ArgTyp.IF->k.FZahl = 0;
	}; 
	~BBFunktion_calcVarianz()
	{
		delete ret.ArgTyp.IF;
	}
	virtual void fkt(void) 
	{
		// Mittelwert
		double sum_x = 0.0f, sum_x2 = 0.0f;
		int n = StatistikVektor.size();
		for (int i=0; i<n; i++)
		{
			sum_x += StatistikVektor[i];
			sum_x2 += StatistikVektor[i]*StatistikVektor[i];
		}
		sum_x = sum_x*sum_x;

		ret.ArgTyp.IF->k.FZahl = (sum_x2 - sum_x/n)/(n-1);
	};
};


#endif

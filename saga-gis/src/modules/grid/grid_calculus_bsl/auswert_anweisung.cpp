/**********************************************************
 * Version $Id$
 *********************************************************/

//#include "..\stdafx.h"

#include "auswert_anweisung.h"



void ausfuehren_anweisung(T_AnweisungList& a)
{
	T_AnweisungList::iterator it;
	for (it = a.begin(); it != a.end(); it++)
	{
		switch((*it)->typ)
		{
		case BBAnweisung::ForEach:
			ausfuehren_foreach(*((*it)->AnweisungVar.For));
			break;
		case BBAnweisung::IF:
			ausfueren_bedingung(*((*it)->AnweisungVar.IF));
			break;
		case BBAnweisung::Zuweisung:
			ausfuehren_zuweisung(*((*it)->AnweisungVar.Zu));
			break;
		case BBAnweisung::Funktion:
			auswert_funktion_integer((*it)->AnweisungVar.Fkt);
			break;
		}
	}
}

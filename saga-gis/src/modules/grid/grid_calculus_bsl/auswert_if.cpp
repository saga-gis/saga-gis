
//#include <..\stdafx.h>

#include "auswert_if.h"
#include <assert.h>



bool auswert_bool_IFVar(BBBaumInteger *IF1, BBBaumInteger *IF2, BBBool::T_booloperator b)
{
	switch (b)
	{
	case BBBool::Gleich:
		return auswert_float(*IF1) == auswert_float(*IF2);
	case BBBool::Ungleich:
			return auswert_float(*IF1) != auswert_float(*IF2);
	case BBBool::Kleiner:
		return auswert_float(*IF1) < auswert_float(*IF2);
	case BBBool::Groesser:
		return auswert_float(*IF1) > auswert_float(*IF2);
	case BBBool::KleinerG:
		return auswert_float(*IF1) <= auswert_float(*IF2);
	case BBBool::GroesserG:
		return auswert_float(*IF1) >= auswert_float(*IF2);
	}
	return false;
}

bool auswert_bool_PVar(BBBaumMatrixPoint *P1, BBBaumMatrixPoint *P2, BBBool::T_booloperator b)
{
	T_Point v1, v2;
	double f;
	int ret1 = auswert_point(*P1, v1, f);
	int ret2 = auswert_point(*P2, v2, f);
	assert(ret1 && ret2);

	switch (b)
	{
	case BBBool::Gleich:
		return (v1.x == v2.x && v1.y == v2.y);
	case BBBool::Ungleich:
		return (v1.x != v2.x || v1.y != v2.y);
	case BBBool::Kleiner:
		return (v1.x < v2.x);
	case BBBool::Groesser:
		return (v1.x > v2.x);
	case BBBool::KleinerG:
		return (v1.x <= v2.x);
	case BBBool::GroesserG:		
		return (v1.x >= v2.x);
	}
	return false;
}

bool auswert_bool_MVar(BBBaumMatrixPoint *M1, BBBaumMatrixPoint *M2, BBBool::T_booloperator b)
{
	GridWerte v1, v2;
	double f;
	int ret1 = auswert_matrix(*M1, v1, f);
	int ret2 = auswert_matrix(*M2, v2, f);
	assert(ret1 && ret2);

	switch (b)
	{
	case BBBool::Gleich:
		return (v1.xanz == v2.xanz && v1.yanz == v2.yanz);
	case BBBool::Ungleich:
		return (v1.xanz != v2.xanz || v1.yanz != v2.yanz);
	case BBBool::Kleiner:
		return (v1.xanz < v2.xanz);
	case BBBool::Groesser:
		return (v1.xanz > v2.xanz);
	case BBBool::KleinerG:
		return (v1.xanz <= v2.xanz);
	case BBBool::GroesserG:		
		return (v1.xanz >= v2.xanz);
	}
	return false;
}

bool auswert_bool(BBBool& b)
{
	assert(b.type != BBBool::Nothing);

	switch(b.type)
	{
	case BBBool::IFVar:
		return auswert_bool_IFVar(b.BoolVar1.IF, b.BoolVar2.IF, b.BoolOp);
	case BBBool::PVar:
		return auswert_bool_PVar(b.BoolVar1.MP, b.BoolVar2.MP, b.BoolOp);
	case BBBool::MVar:
		return auswert_bool_MVar(b.BoolVar1.MP, b.BoolVar2.MP, b.BoolOp);
	}
	return false;
}

bool auswert_bedingung(BBBedingung *b)
{
	assert(b->type != BBBedingung::Nothing);
	int ret1, ret2;
	
	switch (b->type)
	{
	case BBBedingung::Bool:
		return auswert_bool(*(b->BedingungVar.BoolVar.b));
	case BBBedingung::Und:
		return	auswert_bedingung(b->BedingungVar.BoolBiOp.b1) && 
				auswert_bedingung(b->BedingungVar.BoolBiOp.b2);
	case BBBedingung::Oder:
		return	auswert_bedingung(b->BedingungVar.BoolBiOp.b1) || 
				auswert_bedingung(b->BedingungVar.BoolBiOp.b2);
	case BBBedingung::XOder:
		ret1 = auswert_bedingung(b->BedingungVar.BoolBiOp.b1);
		ret2 = auswert_bedingung(b->BedingungVar.BoolBiOp.b2);
		return ((ret1 && !ret2) || (!ret1 && ret2));
	case BBBedingung::Not:
		return	! auswert_bedingung(b->BedingungVar.BoolUniOp.b);
	}
	assert(false);
	return false;
}

void ausfueren_bedingung(BBIf& b)
{

	if (auswert_bedingung(b.b))
	{
		ausfuehren_anweisung(b.z);
	}
	else
	{
		if (b.isElse)
			ausfuehren_anweisung(b.zelse);
	}
}

/**********************************************************
 * Version $Id$
 *********************************************************/

#include "pars_all.h"
#include "auswert_zuweisung.h"
#include "diverses.h"
#include <assert.h>


int auswert_integer(BBBaumInteger& b)
{
	if (b.typ == BBBaumInteger::NoOp)
		throw BBFehlerAusfuehren();
	double f;
	T_Point p;

	switch(b.typ)
	{
	case BBBaumInteger::BIOperator:
		switch(b.k.BiOperator.OpTyp)
		{
		case BBBaumInteger::BBKnoten::BBBiOperator::Plus:
			return auswert_integer(*b.k.BiOperator.links) + 
				auswert_integer(*b.k.BiOperator.rechts);
		case BBBaumInteger::BBKnoten::BBBiOperator::Minus:
			return auswert_integer(*b.k.BiOperator.links) - 
				auswert_integer(*b.k.BiOperator.rechts);
		case BBBaumInteger::BBKnoten::BBBiOperator::Mal:
			return auswert_integer(*b.k.BiOperator.links) * 
				auswert_integer(*b.k.BiOperator.rechts);
		case BBBaumInteger::BBKnoten::BBBiOperator::Geteilt:
			return auswert_integer(*b.k.BiOperator.links) / 
				auswert_integer(*b.k.BiOperator.rechts);
		case BBBaumInteger::BBKnoten::BBBiOperator::Hoch:
			return fround(pow(	(double)auswert_integer(*b.k.BiOperator.links),  
								(double)auswert_integer(*b.k.BiOperator.rechts)));
//-->
		case BBBaumInteger::BBKnoten::BBBiOperator::Modulo:
			return auswert_integer(*b.k.BiOperator.links) %  
								auswert_integer(*b.k.BiOperator.rechts) ;
//<--
		}
		break;
	case BBBaumInteger::UniOperator:
		switch (b.k.UniOperator.OpTyp)
		{
		case BBBaumInteger::BBKnoten::BBUniOperator::Plus:
			return auswert_integer(*b.k.UniOperator.rechts);
		case BBBaumInteger::BBKnoten::BBUniOperator::Minus: 
			return (- auswert_integer(*b.k.UniOperator.rechts));
		} 
		break;
	case BBBaumInteger::MIndex:
		if (b.k.MatrixIndex.P->isMatrix) //muss Point sein
		{
			assert(false);
			break;
		}
		auswert_point(*b.k.MatrixIndex.P, p, f);

		return fround( (double)(*b.k.MatrixIndex.M->M)(p.x,p.y) );



	case BBBaumInteger::IZahl:
		return b.k.IZahl;
	case BBBaumInteger::FZahl:
		return fround(b.k.FZahl);
	case BBBaumInteger::Funktion:
		if (b.k.func->f->ret.typ == BBArgumente::ITyp)
			return auswert_funktion_integer(b.k.func);
		else if (b.k.func->f->ret.typ == BBArgumente::FTyp)
			return fround(auswert_funktion_float(b.k.func));
		else if (b.k.func->f->ret.typ == BBArgumente::NoOp)
		{
			auswert_funktion_integer(b.k.func);
			return 0;
		}
		assert(false);
		break;
	case BBBaumInteger::IVar:
		return *(b.k.IVar->i);
	case BBBaumInteger::FVar:
		return fround(*(b.k.FVar->f));
	}
	assert(false);
	return 0;
}

// ************ Float *******************
double auswert_float(BBBaumInteger& b)
{
	if (b.typ == BBBaumInteger::NoOp)
		throw BBFehlerAusfuehren();

	double f;
	T_Point p;

	switch(b.typ)
	{
	case BBBaumInteger::BIOperator:
		switch(b.k.BiOperator.OpTyp)
		{
		case BBBaumInteger::BBKnoten::BBBiOperator::Plus:
			return auswert_float(*b.k.BiOperator.links) + 
				auswert_float(*b.k.BiOperator.rechts);
		case BBBaumInteger::BBKnoten::BBBiOperator::Minus:
			return auswert_float(*b.k.BiOperator.links) - 
				auswert_float(*b.k.BiOperator.rechts);
		case BBBaumInteger::BBKnoten::BBBiOperator::Mal:
			return auswert_float(*b.k.BiOperator.links) * 
				auswert_float(*b.k.BiOperator.rechts);
		case BBBaumInteger::BBKnoten::BBBiOperator::Geteilt:
			return auswert_float(*b.k.BiOperator.links) / 
				auswert_float(*b.k.BiOperator.rechts);
		case BBBaumInteger::BBKnoten::BBBiOperator::Hoch:
			return pow(	auswert_float(*b.k.BiOperator.links),  
						auswert_float(*b.k.BiOperator.rechts));
		case BBBaumInteger::BBKnoten::BBBiOperator::Modulo:
			return fmod(	auswert_float(*b.k.BiOperator.links),  
						auswert_float(*b.k.BiOperator.rechts));
		}
		break;
	case BBBaumInteger::UniOperator:
		switch (b.k.UniOperator.OpTyp)
		{
		case BBBaumInteger::BBKnoten::BBUniOperator::Plus:
			return auswert_float(*b.k.UniOperator.rechts);
		case BBBaumInteger::BBKnoten::BBUniOperator::Minus: 
			return (- auswert_float(*b.k.UniOperator.rechts));
		} 
		break;
	case BBBaumInteger::MIndex:
		if (b.k.MatrixIndex.P->isMatrix) //muss Point sein
		{
			assert(false);
			break;
		}
		auswert_point(*b.k.MatrixIndex.P, p, f);
		
		return (*b.k.MatrixIndex.M->M)(p.x,p.y);

		//return b.k.MatrixIndex.M->M->Z[p.y][p.x];


	case BBBaumInteger::IZahl:
		return b.k.IZahl;
	case BBBaumInteger::FZahl:
		return b.k.FZahl;
	case BBBaumInteger::Funktion:
		if (b.k.func->f->ret.typ == BBArgumente::ITyp)
			return auswert_funktion_integer(b.k.func);
		else if (b.k.func->f->ret.typ == BBArgumente::FTyp)
			return auswert_funktion_float(b.k.func);
		else if (b.k.func->f->ret.typ == BBArgumente::NoOp)
		{
			auswert_funktion_integer(b.k.func);
			return 0;
		}
		assert(false);
		break;
	case BBBaumInteger::IVar:
		return *(b.k.IVar->i);
	case BBBaumInteger::FVar:
		return *(b.k.FVar->f);
	}
	assert(false);
	return 0;
}

// ************ Point *******************
bool auswert_point(BBBaumMatrixPoint& b, T_Point& vret, double& fret)
{
	if (b.typ == BBBaumInteger::NoOp)
		throw BBFehlerAusfuehren();
	if (b.isMatrix)
		throw BBFehlerAusfuehren();

	T_Point v, v2;
	double f, f2;
	bool ret1, ret2;
	switch(b.typ)
	{
	case BBBaumMatrixPoint::BIOperator:
		switch (b.k.BiOperator.OpTyp)
		{
		case BBBaumMatrixPoint::BBKnoten::BBBiOperator::Plus:
			ret1 = auswert_point(*b.k.BiOperator.links, v, f);
			ret2 = auswert_point(*b.k.BiOperator.rechts, v2, f);
			assert(ret1 && ret2);
			v.x += v2.x;
			v.y += v2.y;
			vret = v;
			return true;
		case BBBaumMatrixPoint::BBKnoten::BBBiOperator::Minus:
			ret1 = auswert_point(*b.k.BiOperator.links, v, f);
			ret2= auswert_point(*b.k.BiOperator.rechts, v2, f);
			assert(ret1 && ret2);
			v.x -= v2.x;
			v.y -= v2.y;
			vret = v;
			return true;
		case BBBaumMatrixPoint::BBKnoten::BBBiOperator::Mal:
			ret1 = auswert_point(*b.k.BiOperator.links, v, f);
			ret2= auswert_point(*b.k.BiOperator.rechts, v2, f2);
			assert((ret1 && !ret2) || (!ret1 && ret2));
			if (ret1)
			{
				v.x *= f2;
				v.y *= f2;
			}
			else 
			{
				v.x = v2.x * f;
				v.y = v2.y * f;
			}
			vret = v;
			return true;
		case BBBaumMatrixPoint::BBKnoten::BBBiOperator::Geteilt:
			ret1 = auswert_point(*b.k.BiOperator.links, v, f);
			ret2= auswert_point(*b.k.BiOperator.rechts, v2, f2);
			assert((ret1 && !ret2) || (!ret1 && ret2));
			if (ret1)
			{
				v.x /= f2;
				v.y /= f2;
			}
			else 
			{
				v.x = v2.x / f;
				v.y = v2.y / f;
			}
			vret = v;
			return true;
		}
		break;
	case BBBaumMatrixPoint::UniOperator:
		switch (b.k.UniOperator.OpTyp)
		{
		case BBBaumMatrixPoint::BBKnoten::BBUniOperator::Plus:
			ret1 = auswert_point(*b.k.UniOperator.rechts, v, f);
			assert(ret1);
			vret = v;
			return true;
		case BBBaumMatrixPoint::BBKnoten::BBUniOperator::Minus:
			ret1 = auswert_point(*b.k.UniOperator.rechts, v, f);
			assert(ret1);
			vret.x = -v.x;
			vret.y = -v.y;
			return true;
		} 
		break;
	case BBBaumMatrixPoint::IFAusdruck:
		fret = auswert_float(*b.k.IntFloatAusdruck.b);
		return false;
	case BBBaumMatrixPoint::MVar:
		assert(false);
		break;
	case BBBaumMatrixPoint::PVar:
		vret = b.k.P->v;
		return true;
	}
	assert(false);
	return false;
}

// ************ Matrix *******************
// Groesse von mret bestimmen !!
// Aufruf M = m+3*m2;  ?? 
bool auswert_matrix(BBBaumMatrixPoint& b, GridWerte& mret, double& fret)
{
	if (b.typ == BBBaumInteger::NoOp)
		throw BBFehlerAusfuehren();
	if (!b.isMatrix) // ?????
		throw BBFehlerAusfuehren();

	GridWerte m, m2;
	double f, f2;
	bool ret1, ret2;
	switch(b.typ)
	{
	case BBBaumMatrixPoint::BIOperator:
		switch (b.k.BiOperator.OpTyp)
		{
		case BBBaumMatrixPoint::BBKnoten::BBBiOperator::Plus:
			ret1 = auswert_matrix(*b.k.BiOperator.links, m, f);
			ret2 = auswert_matrix(*b.k.BiOperator.rechts, m2, f);
			assert(ret1 && ret2);
			
			mret = m;
			mret.getMem();


			if (!(m.xanz == m2.xanz &&
				m.yanz == m2.yanz &&
				mret.xanz == m2.xanz &&
				mret.yanz == m2.yanz ))
				throw BBFehlerMatrixNotEqual();
			{
				for (int i=0; i<m.yanz; i++)
					for (int j=0; j<m.xanz; j++)
						mret.Set_Value(j,i, m(j,i) + m2(j,i));
			}
			return true;
		case BBBaumMatrixPoint::BBKnoten::BBBiOperator::Minus:
			ret1 = auswert_matrix(*b.k.BiOperator.links, m, f);
			ret2= auswert_matrix(*b.k.BiOperator.rechts, m2, f);
			assert(ret1 && ret2);
			if (!(m.xanz == m2.xanz &&
				m.yanz == m2.yanz &&
				mret.xanz == m2.xanz &&
				mret.yanz == m2.yanz ))
				throw BBFehlerMatrixNotEqual();
			{
				for (int i=0; i<m.yanz; i++)
					for (int j=0; j<m.xanz; j++)
						mret.Set_Value(j,i, m(j,i) - m2(j,i));
			}
			return true;
		case BBBaumMatrixPoint::BBKnoten::BBBiOperator::Mal:
			ret1 = auswert_matrix(*b.k.BiOperator.links, m, f);
			ret2= auswert_matrix(*b.k.BiOperator.rechts, m2, f2);
			assert((ret1 && !ret2) || (!ret1 && ret2));
			if (ret1)
			{
				if (!(mret.xanz == m.xanz &&
					  mret.yanz == m.yanz))
					throw BBFehlerMatrixNotEqual();
				for (int i=0; i<m.yanz; i++)
					for (int j=0; j<m.xanz; j++)
						mret.Set_Value(j,i, m(j,i) * f2);
			}
			else 
			{
				if (!(mret.xanz == m2.xanz &&
					  mret.yanz == m2.yanz))
					throw BBFehlerMatrixNotEqual();
				for (int i=0; i<m2.yanz; i++)
					for (int j=0; j<m2.xanz; j++)
						mret.Set_Value(j,i, m2(j,i) * f);
			}
			return true;
		case BBBaumMatrixPoint::BBKnoten::BBBiOperator::Geteilt:
			ret1 = auswert_matrix(*b.k.BiOperator.links, m, f);
			ret2= auswert_matrix(*b.k.BiOperator.rechts, m2, f2);
			assert((ret1 && !ret2) || (!ret1 && ret2));
			if (ret1)
			{
				if (!(mret.xanz == m.xanz &&
					  mret.yanz == m.yanz))
					throw BBFehlerMatrixNotEqual();
				for (int i=0; i<m.yanz; i++)
					for (int j=0; j<m.xanz; j++)
						mret.Set_Value(j,i, m(j,i) / f2);
			}
			else 
			{
				if (!(mret.xanz == m2.xanz &&
					  mret.yanz == m2.yanz))
					throw BBFehlerMatrixNotEqual();
				for (int i=0; i<m2.yanz; i++)
					for (int j=0; j<m2.xanz; j++)
						mret.Set_Value(j,i, m2(j,i) / f);
			}
			return true;
		}
		break;
	case BBBaumMatrixPoint::UniOperator:
		switch (b.k.UniOperator.OpTyp)
		{
		case BBBaumMatrixPoint::BBKnoten::BBUniOperator::Plus:
			ret1 = auswert_matrix(*b.k.UniOperator.rechts, mret, f);
			assert(ret1);
			return true;
		case BBBaumMatrixPoint::BBKnoten::BBUniOperator::Minus:
			ret1 = auswert_matrix(*b.k.UniOperator.rechts, mret, f);
			assert(ret1);
			{
				for (int i=0; i<mret.yanz; i++)
					for (int j=0; j<mret.xanz; j++)
						mret.Set_Value(j,i, -mret(j,i));
			}
			return true;
		} 
		break;
	case BBBaumMatrixPoint::IFAusdruck:
		fret = auswert_float(*b.k.IntFloatAusdruck.b);
		return true; // ?????? false; 
	case BBBaumMatrixPoint::MVar:
		copyGrid(mret, *(b.k.M->M), true);
		return true;
		// ???? break;
	case BBBaumMatrixPoint::PVar:
		assert(false);
		return true;
	}
	assert(false);
	return false;
}

// ************ Funktion Integer *******************

int auswert_funktion_integer(BBFktExe *func)
{
	// Argumente kopieren
	assert(func->f->ret.typ == BBArgumente::ITyp ||
		func->f->ret.typ == BBArgumente::NoOp );

	int l;
	l = func->f->args.size();
	for (int i=0; i<l; i++)
		func->f->args[i].ArgTyp = func->args[i].ArgTyp;
	func->f->fkt();

	// falls kein Argument angegeben
	if (func->f->ret.typ == BBArgumente::NoOp)
		return 0;

	// sonst auswerten
	return auswert_integer(*(func->f->ret.ArgTyp.IF));
}

// ************ Funktion Float *******************

double auswert_funktion_float(BBFktExe *func)
{
	// Argumente kopieren
	assert(func->f->ret.typ == BBArgumente::FTyp);

	int l;
	l = func->f->args.size();
	for (int i=0; i<l; i++)
		func->f->args[i].ArgTyp = func->args[i].ArgTyp;
	func->f->fkt();
	return auswert_float(*(func->f->ret.ArgTyp.IF));
}

int fround(double f)
{
	return (floor(f+0.5f));
}

// ************ ZUWEISUNG *******************

void ausfuehren_zuweisung(BBZuweisung& z)
{
	double f;
	if (z.typ == BBZuweisung::NoTyp)
		throw BBFehlerAusfuehren();
	switch(z.typ)
	{
	case BBZuweisung::FTyp:
		*(z.ZuVar.FVar->f) = auswert_float(*z.ZuArt.IF);
		break;
	case BBZuweisung::ITyp:
		*(z.ZuVar.IVar->i) = auswert_integer(*z.ZuArt.IF);
		break;
	case BBZuweisung::PTyp:
		if (!auswert_point(*z.ZuArt.MP, z.ZuVar.PVar->v, f))
			throw BBFehlerAusfuehren();
		break;
	case BBZuweisung::MTyp:
		if (!auswert_matrix(*z.ZuArt.MP, *(z.ZuVar.MVar->M), f))
			throw BBFehlerAusfuehren();
		break;
	case BBZuweisung::MIndex:
		{
			T_Point p;
			if (!auswert_point(*z.ZuVar.MatrixIndex.PVar, p, f))
				throw BBFehlerAusfuehren();
			
		//	z.ZuVar.MatrixIndex.MVar->M->Z[p.y][p.x] = auswert_float(*z.ZuArt.IF);

			(*z.ZuVar.MatrixIndex.MVar->M).Set_Value(p.x, p.y, auswert_float(*z.ZuArt.IF), true);

		}
		break;
	}
}

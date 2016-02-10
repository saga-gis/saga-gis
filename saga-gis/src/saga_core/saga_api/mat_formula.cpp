/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//           Application Programming Interface           //
//                                                       //
//                  Library: SAGA_API                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    mat_formula.cpp                    //
//                                                       //
//         Copyright (C) 2002 by Andre Ringeler          //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'.                              //
//                                                       //
// This library is free software; you can redistribute   //
// it and/or modify it under the terms of the GNU Lesser //
// General Public License as published by the Free       //
// Software Foundation, version 2.1 of the License.      //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, write to the Free Software Foundation, Inc.,     //
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     aringel@saga-gis.org                   //
//                                                       //
//    contact:    Andre Ringeler                         //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// Based on                                              //
// FORMULC.C 2.22           as of 2/19/98                //
// Copyright (c) 1993 - 98 by Harald Helfgott            //
//                                                       //
// Modified for Grid Data by Andre Ringeler 2001         //
// Modified for Function-Fitting by Andre Ringeler 2002  //
// Converted to C++ by Andre Ringeler 2002               //
//                                                       //
// Modified to fit SAGA needs by Olaf Conrad 2002        //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <ctype.h>

#include "mat_tools.h"
#include "grid.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define MAX_PARMS			3
#define MAX_CTABLE			255

#define STD_FNC_NUM			19

//---------------------------------------------------------
#define GET_VALUE_BUFSIZE	500

//---------------------------------------------------------
#define EPSILON				1e-9


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
static double f_atan2(double x, double val)
{
	return( atan2(x, val) );
}

//---------------------------------------------------------
static double f_pow(double x, double val)
{
	return( pow(x, val) );
}

//---------------------------------------------------------
static double f_gt(double x, double val)
{
	return( x > val ? 1.0 : 0.0 );
}

//---------------------------------------------------------
static double f_lt(double x, double val)
{
	return( x < val ? 1.0 : 0.0 );
}

//---------------------------------------------------------
static double f_eq(double x, double val)
{
	return( fabs(x - val) < EPSILON ? 1.0 : 0.0 );
}

//---------------------------------------------------------
static double f_pi(void)
{
	return( M_PI );
}

//---------------------------------------------------------
static double f_int(double x)
{
	return( (int)(x) );
}

//---------------------------------------------------------
static double f_sqr(double x)
{
	return( x*x );
}

//---------------------------------------------------------
static double f_fmod(double x, double val)
{
	return( fmod(x, val) );
}

//---------------------------------------------------------
static double f_rand_u(double min, double max)
{
	return( CSG_Random::Get_Uniform(min, max) );
}

//---------------------------------------------------------
static double f_rand_g(double mean, double stdv)
{
	return( CSG_Random::Get_Gaussian(mean, stdv) );
}

//---------------------------------------------------------
static double f_and(double x, double y)
{
	return( x != 0.0 && y != 0.0 ? 1.0 : 0.0 );
}

//---------------------------------------------------------
static double f_or(double x, double y)
{
	return( x != 0.0 || y != 0.0 ? 1.0 : 0.0 );
}

//---------------------------------------------------------
static double f_ifelse(double condition, double x, double y)
{
	return( condition ? x : y );
//	return( fabs(condition) >= EPSILON ? x : y );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
static CSG_Formula::TSG_Formula_Item gSG_Functions[MAX_CTABLE]	=
{
	{SG_T("exp"   ), (TSG_PFNC_Formula_1) exp     , 1, 0},	//  1
	{SG_T("ln"    ), (TSG_PFNC_Formula_1) log     , 1, 0},	//  2
	{SG_T("sin"   ), (TSG_PFNC_Formula_1) sin     , 1, 0},	//  3
	{SG_T("cos"   ), (TSG_PFNC_Formula_1) cos     , 1, 0},	//  4
	{SG_T("tan"   ), (TSG_PFNC_Formula_1) tan     , 1, 0},	//  5
	{SG_T("asin"  ), (TSG_PFNC_Formula_1) asin    , 1, 0},	//  6
	{SG_T("acos"  ), (TSG_PFNC_Formula_1) acos    , 1, 0},	//  7
	{SG_T("atan"  ), (TSG_PFNC_Formula_1) atan    , 1, 0},	//  8
	{SG_T("atan2" ), (TSG_PFNC_Formula_2) f_atan2 , 2, 0},	//  9
	{SG_T("abs"   ), (TSG_PFNC_Formula_1) fabs    , 1, 0},	// 10
	{SG_T("sqrt"  ), (TSG_PFNC_Formula_1) sqrt    , 1, 0},	// 11
	{SG_T("gt"    ), (TSG_PFNC_Formula_2) f_gt    , 2, 0},	// 12
	{SG_T("lt"    ), (TSG_PFNC_Formula_2) f_lt    , 2, 0},	// 13
	{SG_T("eq"    ), (TSG_PFNC_Formula_2) f_eq    , 2, 0},	// 14
	{SG_T("pi"    ), (TSG_PFNC_Formula_0) f_pi    , 0, 0},	// 15
	{SG_T("int"   ), (TSG_PFNC_Formula_1) f_int   , 1, 0},	// 16
	{SG_T("mod"   ), (TSG_PFNC_Formula_2) f_fmod  , 2, 0},	// 17
	{SG_T("ifelse"), (TSG_PFNC_Formula_3) f_ifelse, 3, 0},	// 18
	{SG_T("log"   ), (TSG_PFNC_Formula_1) log10   , 1, 0},	// 19
	{SG_T("pow"   ), (TSG_PFNC_Formula_2) f_pow   , 2, 0},	// 20
	{SG_T("sqr"   ), (TSG_PFNC_Formula_1) f_sqr   , 1, 0},	// 21
	{SG_T("rand_u"), (TSG_PFNC_Formula_2) f_rand_u, 2, 1},	// 22
	{SG_T("rand_g"), (TSG_PFNC_Formula_2) f_rand_g, 2, 1},	// 23
	{SG_T("and"   ), (TSG_PFNC_Formula_2) f_and   , 2, 0},	// 24
	{SG_T("or"    ), (TSG_PFNC_Formula_2) f_or    , 2, 0},	// 25
	{NULL          ,                      NULL    , 0, 0}
};


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Formula::CSG_Formula(void)
{
	m_Formula.code		= NULL;
	m_Formula.ctable	= NULL;

	m_bError			= false;

	i_ctable			= NULL;
	i_error				= NULL;
}

//---------------------------------------------------------
CSG_Formula::~CSG_Formula(void)
{
	Destroy();
}

//---------------------------------------------------------
bool CSG_Formula::Destroy(void)
{
	SG_FREE_SAFE(m_Formula.code);
	SG_FREE_SAFE(m_Formula.ctable);

	m_bError			= false;

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_Formula::Get_Help_Operators(bool bHTML, const CSG_String Additional[][2])
{
	const int	nOperators	= 33;

	CSG_String	Operators[nOperators][2]	=
	{
		{	"+"              , _TL("Addition")	},
		{	"-"              , _TL("Subtraction")	},
		{	"*"              , _TL("Multiplication")	},
		{	"/"              , _TL("Division")	},
		{	"abs(x)"         , _TL("Absolute Value")	},
		{	"mod(x, y)"      , _TL("Returns the floating point remainder of x/y")	},
		{	"int(x)"         , _TL("Returns the integer part of floating point value x")	},
		{	"sqr(x)"         , _TL("Square")	},
		{	"sqrt(x)"        , _TL("Square Root")	},
		{	"exp(x)"         , _TL("Exponential")	},
		{	"pow(x, y)"      , _TL("Returns x raised to the power of y")	},
		{	"x ^ y"          , _TL("Returns x raised to the power of y")	},
		{	"ln(x)"          , _TL("Natural Logarithm")	},
		{	"log(x)"         , _TL("Base 10 Logarithm")	},
		{	"pi()"           , _TL("Returns the value of Pi")	},
		{	"sin(x)"         , _TL("Sine")	},
		{	"cos(x)"         , _TL("Cosine")	},
		{	"tan(x)"         , _TL("Tangent")	},
		{	"asin(x)"        , _TL("Arcsine")	},
		{	"acos(x)"        , _TL("Arccosine")	},
		{	"atan(x)"        , _TL("Arctangent")	},
		{	"atan2(x, y)"    , _TL("Arctangent of x/y")	},
		{	"gt(x, y)"       , _TL("Returns true (1), if x is greater than y, else false (0)")	},
		{	"x > y"          , _TL("Returns true (1), if x is greater than y, else false (0)")	},
		{	"lt(x, y)"       , _TL("Returns true (1), if x is less than y, else false (0)")	},
		{	"x < y"          , _TL("Returns true (1), if x is less than y, else false (0)")	},
		{	"eq(x, y)"       , _TL("Returns true (1), if x equals y, else false (0)")	},
		{	"x = y"          , _TL("Returns true (1), if x equals y, else false (0)")	},
		{	"and(x, y)"      , _TL("Returns true (1), if both x and y are true (i.e. not 0)")	},
		{	"or(x, y)"       , _TL("Returns true (1), if at least one of both x and y is true (i.e. not 0)")	},
		{	"ifelse(c, x, y)", _TL("Returns x, if condition c is true (i.e. not 0), else y")	},
		{	"rand_u(x, y)"   , _TL("Random number, uniform distribution with minimum x and maximum y")	},
		{	"rand_g(x, y)"   , _TL("Random number, Gaussian distribution with mean x and standard deviation y")	}
	};

	//-----------------------------------------------------
	int			i;
	CSG_String	s;

	if( bHTML )
	{
		s	+= "<table border=\"0\">";

		for(i=0; i<nOperators; i++)
		{
			CSG_String	op	= Operators[i][0]; op.Replace("<", "&lt;");

			s	+= "<tr><td><b>" + op + "</b></td><td>" + Operators[i][1] + "</td></tr>";
		}

		if( Additional )
		{
			for(i=0; !Additional[i][0].is_Empty(); i++)
			{
				CSG_String	op	= Additional[i][0]; op.Replace("<", "&lt;");

				s	+= "<tr><td><b>" + op + "</b></td><td>" + Additional[i][1] + "</td></tr>";
			}
		}

		s	+= "</table>";
	}
	else
	{
		for(i=0; i<nOperators; i++)
		{
			s	+= Operators[i][0] + " - " + Operators[i][1] + "\n";
		}

		if( Additional )
		{
			for(i=0; !Additional[i][0].is_Empty(); i++)
			{
				s	+= Additional[i][0] + " - " + Additional[i][1] + "\n";
			}
		}
	}

	return( s );
}

//---------------------------------------------------------
bool CSG_Formula::Get_Error(CSG_String &Message)
{
	if( m_bError )
	{
		Message	 = CSG_String::Format("%s %s %d\n", _TL("Error in formula"), _TL("at position"), m_Error_Position);

		if( m_Error_Position < 0 || m_Error_Position >= (int)m_sFormula.Length() )
		{
			Message	+= m_sFormula;
		}
		else
		{
			Message	+= m_sFormula.Left (m_Error_Position) + " ["
					+  m_sFormula      [m_Error_Position] + "] "
					+  m_sFormula.Right(m_sFormula.Length() - (m_Error_Position + 1));
		}

		Message	+= "\n";
		Message	+= m_sError;
		Message	+= "\n";

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CSG_Formula::_Set_Error(const SG_Char *Error)
{
	if( Error == NULL || *Error == SG_T('\0') )
	{
		m_bError	= false;
		m_sError	.Clear();
	}
	else
	{
		m_bError	= true;
		m_sError	= Error;
	}
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Formula::Set_Variable(SG_Char Var, double Value)
{
	m_Parameters[Var - 'a']	= Value;
}

//---------------------------------------------------------
bool CSG_Formula::Set_Formula(const CSG_String &Formula)
{
	if( Formula.Length() > 0 )
	{
		Destroy();

		m_sFormula	= Formula;
		m_Formula	= _Translate(Formula, SG_T("abcdefghijklmnopqrstuvwxyz"), &m_Length, &m_Error_Position);

		if( m_Formula.code != NULL )
		{
			return( true );
		}
	}

	Destroy();

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CSG_Formula::Get_Value(void) const
{
	return( _Get_Value(m_Parameters, m_Formula) );
}

//---------------------------------------------------------
double CSG_Formula::Get_Value(double x) const
{
	double	Parameters[32];

	memcpy(Parameters, m_Parameters, 32 * sizeof(double));

	Parameters['x'-'a']	= x;

	return( _Get_Value(Parameters, m_Formula) );
}

//---------------------------------------------------------
double CSG_Formula::Get_Value(const CSG_Vector &Values) const
{
	return( Get_Value(Values.Get_Data(), Values.Get_N()) );
}

//---------------------------------------------------------
double CSG_Formula::Get_Value(double *Values, int nValues) const
{
	double	Parameters[32];

	for(int i=0; i<nValues; i++)
	{
		Parameters[i]	= Values[i];
	}

	return( _Get_Value(Parameters, m_Formula) );
}

//---------------------------------------------------------
double CSG_Formula::Get_Value(SG_Char *Args, ...) const
{
	double	Parameters[32];

	va_list	ap;

	va_start(ap, Args);

	while( *Args )
	{
		Parameters[(*Args++) - 'a']	= va_arg(ap, double);
	}

	va_end(ap);

	return( _Get_Value(Parameters, m_Formula) );
}

//---------------------------------------------------------
double CSG_Formula::_Get_Value(const double *Parameters, TMAT_Formula func) const
{
	double	x, y, z, buffer[GET_VALUE_BUFSIZE];

	register double		*bufp		= buffer;	// points to the first free space in the buffer
	register SG_Char	*function	= func.code;
	register double		*ctable		= func.ctable;
	register double		result;

	if( !function )
	{
	//	_Set_Error(_TL("empty coded function"));

		return( 0 );
	}

	for( ; ; )
	{
		switch( *function++ )
		{
		case SG_T('\0'):
			goto finish;	// there is a reason for this "goto": this function must be as fast as possible

		case 'D': 
			*bufp++	= ctable[*function++];
			break;

		case 'V': 
			*bufp++	= Parameters[(*function++) - 'a'];
			break;

		case 'M':
			result	= -(*--bufp);
			*bufp++	= result;
			break;

		case '+':
			y		= *(--bufp);
			result	= y + *(--bufp);
			*bufp++	= result;
			break;

		case '-':
			y		= *--bufp;
			result	= *(--bufp) - y;
			*bufp++	= result;
			break;

		case '*':
			y		= *(--bufp);
			result	= *(--bufp) * y;
			*bufp++ = result;
			break;

		case '/':
			y		= *--bufp;
			result	= *(--bufp) / y;
			*bufp++	= result;
			break;

		case '^':
			y		= *--bufp;
			result	= pow(*(--bufp), y);
			*bufp++	= result;
			break;

		case '=':
			y		= *--bufp;
			result	= y == *(--bufp) ? 1.0 : 0.0;
			*bufp++	= result;
			break;

		case '>':
			y		= *--bufp;
			result	= y <  *(--bufp) ? 1.0 : 0.0;
			*bufp++	= result;
			break;

		case '<':
			y		= *--bufp;
			result	= y >  *(--bufp) ? 1.0 : 0.0;
			*bufp++	= result;
			break;

		case '&':
			y		= *--bufp;
			result	= y && *(--bufp) ? 1.0 : 0.0;
			*bufp++	= result;
			break;

		case '|':
			y		= *--bufp;
			result	= y || *(--bufp) ? 1.0 : 0.0;
			*bufp++	= result;
			break;

		case 'F':
			switch (gSG_Functions[*function].n_pars)
			{
			case 0:
				*bufp++	= ((TSG_PFNC_Formula_0)gSG_Functions[*function++].f)();
				break;

			case 1:
				x		= *--bufp;
				*bufp++	= ((TSG_PFNC_Formula_1)gSG_Functions[*function++].f)(x);
				break;

			case 2:
				y		= *--bufp;
				x		= *--bufp;
				*bufp++	= ((TSG_PFNC_Formula_2)gSG_Functions[*function++].f)(x, y);
				break;

			case 3:
				z		= *--bufp;
				y		= *--bufp;
				x		= *--bufp;
				*bufp++	= ((TSG_PFNC_Formula_3)gSG_Functions[*function++].f)(x, y, z);
				break;

			default:
			//	_Set_Error(_TL("I2: too many parameters"));
				return( 0 );
			}
			break;

		default:
		//	_Set_Error(_TL("I1: unrecognizable operator"));
			return( 0 );
		}
	}

finish:

	if( (bufp - buffer) != 1 )
	{
	//	_Set_Error(_TL("I3: corrupted buffer"));
	}

	return( buffer[0] );
} 


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const SG_Char * CSG_Formula::Get_Used_Variables(void)
{
	static CSG_String	ret;

	ret.Clear();

	for(int i=0; i<'z'-'a'; i++)
	{
		if( m_Vars_Used[i] == true )
		{
			ret.Append((SG_Char)(i + 'a'));
		}
	}

	return( ret );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// If the function exists, it is deleted and a non-negative
// value is returned. Otherwise, -1 is returned.
// Original library functions may not be deleted.
//
int CSG_Formula::Del_Function(SG_Char *name)
{
	int		place	 = _Get_Function(name);

	if( place == -1 )
	{
		return( -1 );	// there is an error message already
	}

	if( place < STD_FNC_NUM )
	{
		_Set_Error(_TL("original functions may not be deleted"));

		return( -1 );
	}

	free(gSG_Functions[place].name);

	TSG_Formula_Item	*pFunction;

	for(pFunction=&gSG_Functions[place]; pFunction->f!=NULL; pFunction++)
	{
		pFunction->name		= (pFunction + 1)->name;
		pFunction->f		= (pFunction + 1)->f;
		pFunction->n_pars	= (pFunction + 1)->n_pars;
	}

	_Set_Error();

	return( (int)(pFunction - gSG_Functions) );
}

//---------------------------------------------------------
// int varying;  Does the result of the function vary
// even when the parameters stay the same?
// varying = 1 for e.g. random - number generators.
// Result: 0 is rendered if there is an error
// 1 is rendered otherwise
//
int CSG_Formula::Add_Function(const SG_Char *name, TSG_PFNC_Formula_1 f, int n_pars, int varying)
{
	TSG_Formula_Item *where;
	
	if( n_pars < 0 || n_pars > 3 )
	{
		_Set_Error(_TL("invalid number of parameters"));

		return( 0 );
	}

	for(where=gSG_Functions; where->f != NULL && SG_STR_CMP(name, where->name); where++)
	{
		;
	}

	if( where->f != NULL )
	{
		where->f		= f;
		where->varying	= varying;
		where->n_pars	= n_pars;   /*old function is superseded */

		_Set_Error();

		return( 1 );
	}
	else if( (where - gSG_Functions) >= MAX_CTABLE - 1 )
	{
		_Set_Error(_TL("function table full"));

		return 0;
	}
	else 
	{
		where->name	=(SG_Char *)calloc(SG_STR_LEN(name) + 1, sizeof(SG_Char));

		if( where->name == NULL )
		{
			_Set_Error(_TL("no memory"));

			return( 0 );
		}

		SG_STR_CPY(where->name, name);
		where->f		= f;
		where->varying	= varying;
		where->n_pars	= n_pars;

		_Set_Error();

		return( 1 );
	}
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_Formula::_Get_Function(int i, SG_Char *name, int *n_pars, int *varying)
{
	if( !gSG_Functions[i].f )
	{
		_Set_Error(_TL("index out of bounds"));

		return( 0 );
	}

	SG_STR_CPY(name, gSG_Functions[i].name);

	*n_pars		= gSG_Functions[i].n_pars;
	*varying	= gSG_Functions[i].varying;

	_Set_Error();

	return( 1 );
}

//---------------------------------------------------------
// If the function exists, _Get_Function() returns the index
// of its name in the table. Otherwise, it returns -1.
//
int CSG_Formula::_Get_Function(SG_Char *name)
{
	TSG_Formula_Item	*pFunction;

	for(pFunction=gSG_Functions; pFunction->f && SG_STR_CMP(name, pFunction->name); pFunction++)
	{}

	if( pFunction->f == NULL )
	{
		_Set_Error(_TL("function not found"));

		return( -1 );
	}

	_Set_Error();

	return( (int)(pFunction - gSG_Functions) );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline int CSG_Formula::_is_Operand(SG_Char c)
{
	return(	(c == SG_T('+'))
		||	(c == SG_T('-'))
		||	(c == SG_T('*'))
		||	(c == SG_T('/'))
		||	(c == SG_T('^'))
		||	(c == SG_T('='))
		||	(c == SG_T('<'))
		||	(c == SG_T('>'))
		||	(c == SG_T('&'))
		||	(c == SG_T('|'))
	);
}

//---------------------------------------------------------
inline int CSG_Formula::_is_Operand_Code(SG_Char c)
{
	return(	(c == SG_T('+'))
		||	(c == SG_T('-'))
		||	(c == SG_T('*'))
		||	(c == SG_T('/'))
		||	(c == SG_T('^'))
		||	(c == SG_T('='))
		||	(c == SG_T('<'))
		||	(c == SG_T('>'))
		||	(c == SG_T('&'))
		||	(c == SG_T('|'))
		||	(c == SG_T('M'))
	);
}

//---------------------------------------------------------
inline int CSG_Formula::_is_Number(SG_Char c)
{
	return( isdigit(c) || c == '.' || c == 'E' );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
// Interpreting functions                                //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Formula::TMAT_Formula CSG_Formula::_Translate(const SG_Char *sourc, const SG_Char *args, int *leng, int *error)
{
	SG_Char *result;
	SG_Char *source;
	const SG_Char *scan, *scarg;
	SG_Char *code;
	SG_Char *nfunc; 
	size_t size_estim; 

	double *ctable;
	TMAT_Formula returned; 

	//-----------------------------------------------------
	*leng			= 0;
	*error			= 0; 
	returned.code	= NULL;
	returned.ctable	= NULL;

	i_error = NULL;

	//-----------------------------------------------------
	source	= (SG_Char *)SG_Malloc((SG_STR_LEN(sourc) + 1) * sizeof(SG_Char));

	if( source == NULL )
	{
		_Set_Error(_TL("no memory"));

		return( returned );
	}

	SG_STR_CPY(source, sourc);

	for(scan=source; *scan!=SG_T('\0'); scan++)
	{
		if( islower(*scan) && !isalpha(*(scan + 1)) && (scan == source || !isalpha(*(scan - 1))) )
		{
			for(scarg=args; *scarg!=SG_T('\0') && *scarg != *scan; scarg++)
			{}

			if( *scarg == SG_T('\0') )
			{
				_Set_Error(_TL("undeclared parameter"));

				i_error	= scan;
				*error	= i_error - source;
				
				SG_Free(source);

				return (returned);
			}
		}
	}

	//-----------------------------------------------------
	size_estim = max_size(source); 

	if( !(code =(SG_Char *) SG_Malloc(size_estim)) )
	{
		_Set_Error(_TL("no memory"));

		*error	= -1;

		SG_Free(source);

		return (returned);
	}
	
	
	//-----------------------------------------------------
	i_pctable = 0;

	if( !(i_ctable = (double *)SG_Malloc(MAX_CTABLE * sizeof(double))) )
	{
		_Set_Error(_TL("no memory"));

		*error = -1;

		SG_Free(source);
		SG_Free(code);

		return (returned);
	}

	ctable = i_ctable;

	//-----------------------------------------------------
	_Set_Error();

	result = i_trans(code, (SG_Char *)source, (SG_Char *)source + SG_STR_LEN(source));

	if( !result || m_bError )
	{
		*error	= i_error ? i_error - source : -1;

		SG_Free(source);
		SG_Free(code);
		SG_Free(i_ctable);

		return (returned);
	}
	else 
	{
		*result	= SG_T('\0');
		*error	= -1;
		*leng	= result - code;

		if( ((*leng) + 1) * sizeof(SG_Char) > size_estim )
		{
			_Set_Error(_TL("I4: size estimate too small"));

			SG_Free(source);

			return( returned );
		}
		else if( ((*leng) + 1) * sizeof(SG_Char) < size_estim )
		{
			nfunc	= (SG_Char *) SG_Malloc(((*leng) + 1) * sizeof(SG_Char));

			if (nfunc) 
			{
				memcpy(nfunc, code, ((*leng) + 1) * sizeof(SG_Char));
				SG_Free(code);
				code = nfunc;
			}
		}

		if( i_pctable < MAX_CTABLE )
		{
			ctable	= (double *)SG_Malloc(i_pctable * sizeof(double));

			if( ctable )
			{
				memcpy(ctable, i_ctable, i_pctable * sizeof(double));

				SG_Free(i_ctable);
			}
			else 
			{
				ctable	= i_ctable;
			}
		}
		else 
		{
			ctable	= i_ctable;
		}

		returned.code	= code;
		returned.ctable	= ctable;

		_Set_Error();

		SG_Free(source);

		return (returned);
	}
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
SG_Char *CSG_Formula::i_trans(SG_Char *function, SG_Char *begin, SG_Char *end)
{
	int pars;     
	SG_Char *scan;
	SG_Char *tempu, *temp3;
	SG_Char *temps	= NULL;
	SG_Char tempch;
	double tempd;
	SG_Char *endf;     
	
	int n_function;
	int space;
	int i;
	
	SG_Char *paramstr[MAX_PARMS];
	SG_Char *par_buf;
	
	if (begin >= end)
	{
		_Set_Error(_TL("missing operand"));
		i_error = begin;
		return NULL;
	}
	
	for (pars = 0, scan = begin; scan < end && pars >= 0; scan++)
	{
		if (*scan == SG_T('(')) pars++;
		else if (*scan == SG_T(')'))
			pars--;
	}
	if (pars < 0 || pars > 0)
	{
		_Set_Error(_TL("unmatched parentheses"));
		i_error = scan - 1;
		return NULL;
	}
	
	for (pars = 0, scan = end - 1; scan >= begin; scan--)
	{
		if (*scan == SG_T('(')) pars++;
		else if (*scan == SG_T(')'))
			pars--;
		else if (!pars &&(*scan == SG_T('+') ||((*scan == SG_T('-')) && scan != begin))
			&&(scan == begin || *(scan - 1) != SG_T('E')))
			break;
	}
	
	if (scan >= begin)
	{                                 
		if ((tempu = i_trans(function, begin, scan)) &&      
			(temp3 = i_trans(tempu, scan + 1, end)))
		{
			*temp3++ = *scan; 
			temp3 = comp_time(function, temp3, 2); 
			if( m_bError )
				return NULL;   
			else 
				return temp3;  
		}
		else 
			return NULL;  
	}
	
	for (pars = 0, scan = end - 1; scan >= begin; scan--)
	{
		if (*scan == SG_T('(')) pars++;
		else if (*scan == SG_T(')'))
			pars--;
		else if (!pars &&(*scan == SG_T('*') || *scan == SG_T('/')))
			break;
	}
	if (scan >= begin)
	{                                 
		if ((tempu = i_trans(function, begin, scan)) &&      
			(temp3 = i_trans(tempu, scan + 1, end)))
		{
			*temp3++ = *scan; 
			temp3 = comp_time(function, temp3, 2); 
			if (m_bError)
				return NULL;   
			else 
				return temp3;  
		}
		else 
			return NULL;  
	}
	
	/* unary minus */
	if (*begin == SG_T('-'))
	{
		tempu = i_trans(function, begin + 1, end);
		if (tempu)
		{
			*tempu++ = SG_T('M');
			tempu = comp_time(function, tempu, 1); 
			if (m_bError)
				return NULL; 
			else 
				return tempu;
		}
		else 
			return NULL;
	}
	
	for (pars = 0, scan = end - 1; scan >= begin; scan--)
	{
		if (*scan == SG_T('(')) pars++;
		else if (*scan == SG_T(')'))
			pars--;
		else if (!pars &&(*scan == SG_T('^')))
			break;
		else if (!pars &&(*scan == SG_T('=')))
			break;
		else if (!pars &&(*scan == SG_T('>')))
			break;
		else if (!pars &&(*scan == SG_T('<')))
			break;
		else if (!pars &&(*scan == SG_T('&')))
			break;
		else if (!pars &&(*scan == SG_T('|')))
			break;
	}

	if (scan >= begin)
	{                                 
		if ((tempu = i_trans(function, begin, scan)) &&      
			(temp3 = i_trans(tempu, scan + 1, end)))
		{
			*temp3++ = *scan; 
			temp3 = comp_time(function, temp3, 2); 
			if (m_bError)
				return NULL;   
			else 
				return temp3;  
		}
		else 
			return NULL;  
	}
	
	/* erase white space */
	while (isspace(*begin))
		begin++;
	while (isspace(*(end - 1)))
		end--;
	
	if (*begin == SG_T('(') && *(end - 1) == SG_T(')'))
		return i_trans(function, begin + 1, end - 1);
	
	if (end == begin + 1 && islower(*begin))
	{
		*function++ = SG_T('V');
		*function++ = *begin;
		return function;
	}
	
	tempch = *end;
	*end = SG_T('\0');
	tempd = SG_STR_TOD(begin, (SG_Char**) &tempu);
	*end = tempch;
	if ((SG_Char*) tempu == end)
	{
		*function++ = SG_T('D');
		if (i_pctable < MAX_CTABLE)
		{
			i_ctable[i_pctable] = tempd;
			*function++ =(SG_Char) i_pctable++;
		}
		else
		{
			_Set_Error(_TL("too many constants"));
			i_error = begin;
			return NULL;
		}
		return function;
	}
	
				/*function*/
	if (!isalpha(*begin) && *begin != SG_T('_'))
	{
		_Set_Error(_TL("syntax error"));
		i_error = begin;
		return NULL;
	}
	for (endf = begin + 1; endf < end &&(isalnum(*endf) || *endf == SG_T('_'));
	endf++)
		;
	tempch = *endf;
	*endf = SG_T('\0');
	if ((n_function = _Get_Function(begin)) == -1)
	{
		*endf = tempch;
		i_error = begin;
		return NULL;
	}
	*endf = tempch;
	if (*endf != SG_T('(') || *(end - 1) != SG_T(')'))
	{
		_Set_Error(_TL("improper function syntax"));
		i_error = endf;
		return NULL;
	}
	if (gSG_Functions[n_function].n_pars == 0)
	{
		/*function without parameters(e.g. pi()) */
		space = 1;
		for (scan = endf + 1; scan <(end - 1); scan++)
			if (!isspace(*scan))
				space = 0;
			if (space)
			{
				*function++ = SG_T('F');
				*function++ = n_function;
				function = comp_time(function - 2, function, 0);
				if (m_bError)
					return NULL; /* internal error in comp_time */
				else 
					return function;
			}
			else 
			{
				i_error = endf + 1;
				_Set_Error(_TL("too many parameters"));
				return NULL;
			}
	}
	else 
	{	/*function with parameters*/
		tempch = *(end - 1);
		*(end - 1) = SG_T('\0');
		par_buf =(SG_Char *) SG_Malloc(sizeof(SG_Char) * (SG_STR_LEN(endf + 1) + 1));

		if (!par_buf)
		{    
			_Set_Error(_TL("no memory")); 
			i_error = NULL;  
			return NULL;   
		}
		
		SG_STR_CPY(par_buf, endf + 1);
		*(end - 1) = tempch;
		
		for (i = 0; i < gSG_Functions[n_function].n_pars; i++)
		{
			if ((temps = my_strtok((i == 0) ? par_buf : NULL)) == NULL)
				break; 
			paramstr[i] = temps;
		}

		if (temps == NULL)
		{
			SG_Free(par_buf);
			i_error = end - 2;
			_Set_Error(_TL("too few parameters"));
			return NULL;
		}

		if ((temps = my_strtok(NULL)) != NULL)
		{
			SG_Free(par_buf);
			i_error =(temps - par_buf) +(endf + 1); 
			_Set_Error(_TL("too many parameters"));
			return NULL;
		}
		
		tempu = function;
		for (i = 0; i < gSG_Functions[n_function].n_pars; i++)
			if( !(tempu = i_trans(tempu, paramstr[i], paramstr[i] + SG_STR_LEN(paramstr[i]))) )
			{
				i_error =(i_error - par_buf) +(endf + 1); 
				SG_Free(par_buf);
				
				return NULL; 
			}

		/* OK */
		SG_Free(par_buf);
		*tempu++ = SG_T('F');
		*tempu++ = n_function;
		tempu = comp_time(function, tempu, gSG_Functions[n_function].n_pars);
		if (m_bError)
			return NULL; /* internal error in comp_time */
		else 
			return tempu;
	}
}

//---------------------------------------------------------
SG_Char *CSG_Formula::comp_time(SG_Char *function, SG_Char *fend, int npars)
{
	SG_Char *scan;
	SG_Char temp;
	double tempd;
	int i;
	TMAT_Formula trans;
	
	scan = function;
	for (i = 0; i < npars; i++)
	{
		if (*scan++ != SG_T('D'))
			return fend;
		scan++;
	}
	
	if (!((scan == fend -(sizeof((SG_Char) SG_T('F')) + sizeof(SG_Char))
		&& *(fend - 2) == SG_T('F') && gSG_Functions[*(fend - 1)].varying == 0) ||
		(scan == fend - sizeof(SG_Char)
		&& _is_Operand_Code(*(fend - 1))))
		)
		return fend;
	
	temp = *fend;
	*fend = SG_T('\0');
	
	trans.code = function;
	trans.ctable = i_ctable;
	tempd = _Get_Value(m_Parameters, trans);
	*fend = temp;
	*function++ = SG_T('D');
	i_pctable -= npars;
	*function++ =(SG_Char) i_pctable;
	i_ctable[i_pctable++] = tempd;
	
	return function;
}

//---------------------------------------------------------
int CSG_Formula::max_size(const SG_Char *source)
{
	int numbers		= 0;
	int functions	= 0;
	int operators	= 0;
	int variables	= 0;

	const size_t var_size	= 2*sizeof(SG_Char);
	const size_t num_size	= sizeof(SG_Char) + sizeof(double);
	const size_t op_size	= sizeof(SG_Char);
	const size_t end_size	= sizeof(SG_T('\0'));

	const SG_Char *scan;

    for(int i=0; i<SG_T('z')-SG_T('a'); i++)
	{
		m_Vars_Used[i]	= false;
	}

	for(scan=source; *scan; scan++)
	{
		if( isalpha(*scan) && (*scan != SG_T('E')) )
		{
			if( isalpha(*(scan + 1)) )
			{
				// it is a function name, it will be counted later on
			}
			else if( *(scan + 1) == SG_T('(') )
			{
				functions++;
			}
			else
			{
				variables++;
				m_Vars_Used[(int)(*scan - SG_T('a'))] = true;
			}
		}
	}

	if( _is_Operand(*source) )
	{
		operators++;
	}

	if( *source != SG_T('\0') )
	{
		for(scan=source+1; *scan; scan++)
		{
			if( _is_Operand(*scan) && *(scan - 1) != SG_T('E') )
			{
				operators++;
			}
		}
	}

	scan	= source;
	while( *scan )
	{
		if( _is_Number(*scan) || ((*scan == SG_T('+') || *scan == SG_T('-')) && scan > source && *(scan - 1) == SG_T('E')) )
		{
			numbers++;
			scan++;

			while( _is_Number(*scan) ||((*scan == SG_T('+') || *scan == SG_T('-')) && scan > source && *(scan - 1) == SG_T('E')) )
				scan++;
		}
		else 
		{
			scan++;
		}
	}

	return( numbers*num_size + operators*op_size + functions*num_size + variables*var_size + end_size );
}

//---------------------------------------------------------
SG_Char *CSG_Formula::my_strtok(SG_Char *s)
{
	int pars;
	static SG_Char *token = NULL;
	SG_Char *next_token;
	
	if (s != NULL)
		token = s;
	else if (token != NULL)
		s = token;
	else 
		return NULL;
	
	for (pars = 0; *s != SG_T('\0') &&(*s != SG_T(',') || pars != 0); s++)
	{
		if (*s == SG_T('(')) ++pars;
		if (*s == SG_T(')'))
			--pars;
	}
	if (*s == SG_T('\0'))
	{
		next_token = NULL;
		s = token;
		
		token = next_token;
		return s;
	}
	else 
	{
		*s = SG_T('\0');
		next_token = s + 1;
		s = token;
		
		token = next_token;
		return s;
	}
} 


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

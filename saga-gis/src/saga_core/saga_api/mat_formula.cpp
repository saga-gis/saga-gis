
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
// Software Foundation, either version 2.1 of the        //
// License, or (at your option) any later version.       //
//                                                       //
// This library is distributed in the hope that it will  //
// be useful, but WITHOUT ANY WARRANTY; without even the //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU Lesser General Public //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU Lesser     //
// General Public License along with this program; if    //
// not, see <http://www.gnu.org/licenses/>.              //
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
static double f_min(double a, double b)
{
	return( a < b ? a : b );
}

//---------------------------------------------------------
static double f_max(double a, double b)
{
	return( a > b ? a : b );
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
static CSG_Formula::TSG_Function gSG_Functions[MAX_CTABLE]	=
{
	{ "exp"   , (TSG_Formula_Function_1)exp     , 1, false },	//  1
	{ "ln"    , (TSG_Formula_Function_1)log     , 1, false },	//  2
	{ "sin"   , (TSG_Formula_Function_1)sin     , 1, false },	//  3
	{ "cos"   , (TSG_Formula_Function_1)cos     , 1, false },	//  4
	{ "tan"   , (TSG_Formula_Function_1)tan     , 1, false },	//  5
	{ "asin"  , (TSG_Formula_Function_1)asin    , 1, false },	//  6
	{ "acos"  , (TSG_Formula_Function_1)acos    , 1, false },	//  7
	{ "atan"  , (TSG_Formula_Function_1)atan    , 1, false },	//  8
	{ "atan2" , (TSG_Formula_Function_1)f_atan2 , 2, false },	//  9
	{ "abs"   , (TSG_Formula_Function_1)fabs    , 1, false },	// 10
	{ "sqrt"  , (TSG_Formula_Function_1)sqrt    , 1, false },	// 11
	{ "gt"    , (TSG_Formula_Function_1)f_gt    , 2, false },	// 12
	{ "lt"    , (TSG_Formula_Function_1)f_lt    , 2, false },	// 13
	{ "eq"    , (TSG_Formula_Function_1)f_eq    , 2, false },	// 14
	{ "pi"    , (TSG_Formula_Function_1)f_pi    , 0, false },	// 15
	{ "int"   , (TSG_Formula_Function_1)f_int   , 1, false },	// 16
	{ "mod"   , (TSG_Formula_Function_1)f_fmod  , 2, false },	// 17
	{ "ifelse", (TSG_Formula_Function_1)f_ifelse, 3, false },	// 18
	{ "log"   , (TSG_Formula_Function_1)log10   , 1, false },	// 19
	{ "pow"   , (TSG_Formula_Function_1)f_pow   , 2, false },	// 20
	{ "sqr"   , (TSG_Formula_Function_1)f_sqr   , 1, false },	// 21
	{ "rand_u", (TSG_Formula_Function_1)f_rand_u, 2,  true },	// 22
	{ "rand_g", (TSG_Formula_Function_1)f_rand_g, 2,  true },	// 23
	{ "and"   , (TSG_Formula_Function_1)f_and   , 2, false },	// 24
	{ "or"    , (TSG_Formula_Function_1)f_or    , 2, false },	// 25
	{ "min"   , (TSG_Formula_Function_1)f_min   , 2, false },	// 26
	{ "max"   , (TSG_Formula_Function_1)f_max   , 2, false },	// 27
	{  NULL   , (TSG_Formula_Function_1) NULL   , 0, false }
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

	m_ctable			= NULL;
	m_error				= NULL;

	//-----------------------------------------------------
	m_Functions	= (TSG_Function *)SG_Calloc(MAX_CTABLE, sizeof(TSG_Function));

	for(int i=0; i<MAX_CTABLE; i++)
	{
		m_Functions[i].Name        = gSG_Functions[i].Name;
		m_Functions[i].Function    = gSG_Functions[i].Function;
		m_Functions[i].nParameters = gSG_Functions[i].nParameters;
		m_Functions[i].bVarying    = gSG_Functions[i].bVarying;
	}
}

//---------------------------------------------------------
CSG_Formula::~CSG_Formula(void)
{
	Destroy();

	SG_Free(m_Functions);
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
	const int	nOperators	= 35;

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
		{	"min(x, y)"      , _TL("Returns the minimum of values x and y")	},
		{	"max(x, y)"      , _TL("Returns the maximum of values x and y")	},
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
void CSG_Formula::_Set_Error(const CSG_String &Error)
{
	if( Error.is_Empty() )
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
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Formula::Set_Variable(char Var, double Value)
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
		m_Formula	= _Translate(Formula, "abcdefghijklmnopqrstuvwxyz", &m_Length, &m_Error_Position);

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
double CSG_Formula::Get_Value(const char *Args, ...) const
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
double CSG_Formula::_Get_Value(const double *Parameters, TSG_Formula func) const
{
	double	x, y, z, buffer[GET_VALUE_BUFSIZE];

	register double *bufp     = buffer;	// points to the first free space in the buffer
	register char   *function = func.code;
	register double *ctable   = func.ctable;
	register double result;

	if( !function )
	{
		return( 0 );	// _Set_Error(_TL("empty coded function"));
	}

	for( ; ; )
	{
		switch( *function++ )
		{
		case '\0':
			return( buffer[0] );

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
			switch (m_Functions[*function].nParameters)
			{
			case 0:
				*bufp++	= ((TSG_Formula_Function_0)m_Functions[*function++].Function)();
				break;

			case 1:
				x		= *--bufp;
				*bufp++	= ((TSG_Formula_Function_1)m_Functions[*function++].Function)(x);
				break;

			case 2:
				y		= *--bufp;
				x		= *--bufp;
				*bufp++	= ((TSG_Formula_Function_2)m_Functions[*function++].Function)(x, y);
				break;

			case 3:
				z		= *--bufp;
				y		= *--bufp;
				x		= *--bufp;
				*bufp++	= ((TSG_Formula_Function_3)m_Functions[*function++].Function)(x, y, z);
				break;

			default:
				return( 0 );	// _Set_Error(_TL("I2: too many parameters"));
			}
			break;

		default:
			return( 0 );	// _Set_Error(_TL("I1: unrecognizable operator"));
		}
	}

//	if( (bufp - buffer) != 1 )	// _Set_Error(_TL("I3: corrupted buffer"));

	return( buffer[0] );
} 


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const char * CSG_Formula::Get_Used_Variables(void)
{
	static CSG_String	ret;

	ret.Clear();

	for(int i=0; i<'z'-'a'; i++)
	{
		if( m_Vars_Used[i] == true )
		{
			ret.Append((char)(i + 'a'));
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
// int varying;  Does the result of the function vary
// even when the parameters stay the same?
// varying = 1 for e.g. random - number generators.
// Result: 0 is rendered if there is an error
// 1 is rendered otherwise
//
bool CSG_Formula::Add_Function(const char *Name, TSG_Formula_Function_1 Function, int nParameters, bool bVarying)
{
	if( nParameters < 0 || nParameters > 3 )
	{
		_Set_Error(_TL("invalid number of parameters"));

		return( false );
	}

	TSG_Function	*pFunction;

	for(pFunction=m_Functions; pFunction->Function && strcmp(Name, pFunction->Name); pFunction++)
	{}

	if( pFunction->Function != NULL )   // old function is superseded
	{
		pFunction->Function    = Function;
		pFunction->nParameters = nParameters;
		pFunction->bVarying    = bVarying;

		_Set_Error();

		return( true );
	}

	if( (pFunction - m_Functions) >= MAX_CTABLE - 1 )
	{
		_Set_Error(_TL("function table full"));

		return( false );
	}

	pFunction->Name        = Name;
	pFunction->Function    = Function;
	pFunction->nParameters = nParameters;
	pFunction->bVarying    = bVarying;

	_Set_Error();

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSG_Formula::_Get_Function(int i, char *Name, int *nParameters, int *bVarying)
{
	if( !m_Functions[i].Function )
	{
		_Set_Error(_TL("index out of bounds"));

		return( 0 );
	}

	strcpy(Name  , m_Functions[i].Name);
	*nParameters = m_Functions[i].nParameters;
	*bVarying    = m_Functions[i].bVarying ? 1 : 0;

	_Set_Error();

	return( 1 );
}

//---------------------------------------------------------
// If the function exists, _Get_Function() returns the index
// of its name in the table. Otherwise, it returns -1.
//
int CSG_Formula::_Get_Function(const char *Name)
{
	TSG_Function	*pFunction;

	for(pFunction=m_Functions; pFunction->Function && strcmp(Name, pFunction->Name); pFunction++)
	{}

	if( pFunction->Function == NULL )
	{
		_Set_Error(_TL("function not found"));

		return( -1 );
	}

	_Set_Error();

	return( (int)(pFunction - m_Functions) );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline int CSG_Formula::_is_Operand(char c)
{
	return(	(c == '+')
		||	(c == '-')
		||	(c == '*')
		||	(c == '/')
		||	(c == '^')
		||	(c == '=')
		||	(c == '<')
		||	(c == '>')
		||	(c == '&')
		||	(c == '|')
	);
}

//---------------------------------------------------------
inline int CSG_Formula::_is_Operand_Code(char c)
{
	return(	(c == '+')
		||	(c == '-')
		||	(c == '*')
		||	(c == '/')
		||	(c == '^')
		||	(c == '=')
		||	(c == '<')
		||	(c == '>')
		||	(c == '&')
		||	(c == '|')
		||	(c == 'M')
	);
}

//---------------------------------------------------------
inline int CSG_Formula::_is_Number(char c)
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
CSG_Formula::TSG_Formula CSG_Formula::_Translate(const char *sourc, const char *args, int *leng, int *error)
{
	const char	*scan, *scarg;
	char		*result, *source, *code, *nfunc; 
	size_t		size_estim; 
	double		*ctable;
	TSG_Formula	returned; 

	//-----------------------------------------------------
	*leng			= 0;
	*error			= 0; 
	m_error			= NULL;
	returned.code	= NULL;
	returned.ctable	= NULL;

	//-----------------------------------------------------
	source	= (char *)SG_Malloc((strlen(sourc) + 1) * sizeof(char));

	if( source == NULL )
	{
		_Set_Error(_TL("no memory"));

		return( returned );
	}

	strcpy(source, sourc);

	for(scan=source; *scan!='\0'; scan++)
	{
		if( islower(*scan) && !isalpha(*(scan + 1)) && (scan == source || !isalpha(*(scan - 1))) )
		{
			for(scarg=args; *scarg!='\0' && *scarg != *scan; scarg++)
			{}

			if( *scarg == '\0' )
			{
				_Set_Error(_TL("undeclared parameter"));

				m_error	= scan;
				*error	= (int)(m_error - source);
				
				SG_Free(source);

				return (returned);
			}
		}
	}

	//-----------------------------------------------------
	size_estim = _max_size(source); 

	if( !(code = (char *)SG_Malloc(size_estim)) )
	{
		_Set_Error(_TL("no memory"));

		*error	= -1;

		SG_Free(source);

		return (returned);
	}
	
	
	//-----------------------------------------------------
	m_pctable = 0;

	if( !(m_ctable = (double *)SG_Malloc(MAX_CTABLE * sizeof(double))) )
	{
		_Set_Error(_TL("no memory"));

		*error = -1;

		SG_Free(source);
		SG_Free(code);

		return (returned);
	}

	ctable = m_ctable;

	//-----------------------------------------------------
	_Set_Error();

	result = _i_trans(code, (char *)source, (char *)source + strlen(source));

	if( !result || m_bError )
	{
		*error	= (int)(m_error ? m_error - source : -1);

		SG_Free(source);
		SG_Free(code);
		SG_Free(m_ctable);

		return (returned);
	}
	else 
	{
		*result	= '\0';
		*error	= -1;
		*leng	= (int)(result - code);

		if( ((*leng) + 1) * sizeof(char) > size_estim )
		{
			_Set_Error(_TL("I4: size estimate too small"));

			SG_Free(source);

			return( returned );
		}
		else if( ((*leng) + 1) * sizeof(char) < size_estim )
		{
			nfunc	= (char *)SG_Malloc(((*leng) + 1) * sizeof(char));

			if (nfunc) 
			{
				memcpy(nfunc, code, ((*leng) + 1) * sizeof(char));
				SG_Free(code);
				code = nfunc;
			}
		}

		if( m_pctable < MAX_CTABLE )
		{
			ctable	= (double *)SG_Malloc(m_pctable * sizeof(double));

			if( ctable )
			{
				memcpy(ctable, m_ctable, m_pctable * sizeof(double));

				SG_Free(m_ctable);
			}
			else 
			{
				ctable	= m_ctable;
			}
		}
		else 
		{
			ctable	= m_ctable;
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
char * CSG_Formula::_i_trans(char *function, char *begin, char *end)
{
	char	tempch, *scan, *endf, *tempu, *temp3, *temps = NULL, *par_buf, *paramstr[MAX_PARMS];
	int		i, pars, space, n_function;
	double	tempd;
	
	if( begin >= end )
	{
		_Set_Error(_TL("missing operand"));
		m_error = begin;
		return NULL;
	}
	
	for(pars = 0, scan = begin; scan < end && pars >= 0; scan++)
	{
		if( *scan == '(' ) { pars++; } else
		if( *scan == ')' ) { pars--; }
	}

	if( pars < 0 || pars > 0 )
	{
		_Set_Error(_TL("unmatched parentheses"));
		m_error = scan - 1;
		return NULL;
	}
	
	for(pars = 0, scan = end - 1; scan >= begin; scan--)
	{
		if( *scan == '(' ) pars++; else
		if( *scan == ')' ) pars--; else
		if( !pars && (*scan == '+' || ((*scan == '-') && scan != begin)) && (scan == begin || *(scan - 1) != 'E') )
			break;
	}
	
	if( scan >= begin )
	{                                 
		if ((tempu = _i_trans(function, begin, scan)) &&      
			(temp3 = _i_trans(tempu, scan + 1, end)))
		{
			*temp3++ = *scan; 
			temp3 = _comp_time(function, temp3, 2); 
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
		if (*scan == '(') pars++;
		else if (*scan == ')')
			pars--;
		else if (!pars &&(*scan == '*' || *scan == '/'))
			break;
	}
	if (scan >= begin)
	{                                 
		if ((tempu = _i_trans(function, begin, scan)) &&      
			(temp3 = _i_trans(tempu, scan + 1, end)))
		{
			*temp3++ = *scan; 
			temp3 = _comp_time(function, temp3, 2); 
			if (m_bError)
				return NULL;   
			else 
				return temp3;  
		}
		else 
			return NULL;  
	}
	
	/* unary minus */
	if (*begin == '-')
	{
		tempu = _i_trans(function, begin + 1, end);
		if (tempu)
		{
			*tempu++ = 'M';
			tempu = _comp_time(function, tempu, 1); 
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
		if (*scan == '(') pars++;
		else if (*scan == ')')
			pars--;
		else if (!pars &&(*scan == '^'))
			break;
		else if (!pars &&(*scan == '='))
			break;
		else if (!pars &&(*scan == '>'))
			break;
		else if (!pars &&(*scan == '<'))
			break;
		else if (!pars &&(*scan == '&'))
			break;
		else if (!pars &&(*scan == '|'))
			break;
	}

	if (scan >= begin)
	{                                 
		if ((tempu = _i_trans(function, begin, scan)) &&      
			(temp3 = _i_trans(tempu, scan + 1, end)))
		{
			*temp3++ = *scan; 
			temp3 = _comp_time(function, temp3, 2); 
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
	
	if (*begin == '(' && *(end - 1) == ')')
		return _i_trans(function, begin + 1, end - 1);
	
	if (end == begin + 1 && islower(*begin))
	{
		*function++ = 'V';
		*function++ = *begin;
		return function;
	}
	
	tempch = *end;
	*end = '\0';
	tempd = strtod(begin, (char **)&tempu);
	*end = tempch;

	if( (char *)tempu == end )
	{
		*function++ = 'D';
		if (m_pctable < MAX_CTABLE)
		{
			m_ctable[m_pctable] = tempd;
			*function++ = (char)m_pctable++;
		}
		else
		{
			_Set_Error(_TL("too many constants"));
			m_error = begin;
			return NULL;
		}
		return function;
	}
	
				/*function*/
	if (!isalpha(*begin) && *begin != '_')
	{
		_Set_Error(_TL("syntax error"));
		m_error = begin;
		return NULL;
	}

	for(endf = begin + 1; endf < end &&(isalnum(*endf) || *endf == '_'); endf++)
	{}

	tempch = *endf;
	*endf = '\0';
	if ((n_function = _Get_Function(begin)) == -1)
	{
		*endf = tempch;
		m_error = begin;
		return NULL;
	}
	*endf = tempch;
	if (*endf != '(' || *(end - 1) != ')')
	{
		_Set_Error(_TL("improper function syntax"));
		m_error = endf;
		return NULL;
	}

	if( m_Functions[n_function].nParameters == 0 )
	{
		/*function without parameters(e.g. pi()) */
		space = 1;
		for (scan = endf + 1; scan <(end - 1); scan++)
			if (!isspace(*scan))
				space = 0;
			if (space)
			{
				*function++ = 'F';
				*function++ = n_function;
				function = _comp_time(function - 2, function, 0);
				if (m_bError)
					return NULL; /* internal error in _comp_time */
				else 
					return function;
			}
			else 
			{
				m_error = endf + 1;
				_Set_Error(_TL("too many parameters"));
				return NULL;
			}
	}
	else 
	{	/*function with parameters*/
		tempch = *(end - 1);
		*(end - 1) = '\0';
		par_buf = (char *)SG_Malloc(sizeof(char) * (strlen(endf + 1) + 1));

		if (!par_buf)
		{    
			_Set_Error(_TL("no memory")); 
			m_error = NULL;  
			return NULL;   
		}
		
		strcpy(par_buf, endf + 1);
		*(end - 1) = tempch;
		
		for (i = 0; i < m_Functions[n_function].nParameters; i++)
		{
			if ((temps = _my_strtok((i == 0) ? par_buf : NULL)) == NULL)
				break; 
			paramstr[i] = temps;
		}

		if (temps == NULL)
		{
			SG_Free(par_buf);
			m_error = end - 2;
			_Set_Error(_TL("too few parameters"));
			return NULL;
		}

		if ((temps = _my_strtok(NULL)) != NULL)
		{
			SG_Free(par_buf);
			m_error =(temps - par_buf) +(endf + 1); 
			_Set_Error(_TL("too many parameters"));
			return NULL;
		}
		
		tempu = function;
		for (i = 0; i < m_Functions[n_function].nParameters; i++)
			if( !(tempu = _i_trans(tempu, paramstr[i], paramstr[i] + strlen(paramstr[i]))) )
			{
				m_error =(m_error - par_buf) +(endf + 1); 
				SG_Free(par_buf);
				
				return NULL; 
			}

		/* OK */
		SG_Free(par_buf);
		*tempu++ = 'F';
		*tempu++ = n_function;
		tempu = _comp_time(function, tempu, m_Functions[n_function].nParameters);
		if (m_bError)
			return NULL; /* internal error in _comp_time */
		else 
			return tempu;
	}
}

//---------------------------------------------------------
char * CSG_Formula::_comp_time(char *function, char *fend, int npars)
{
	char		*scan, temp;
	int			i;
	double		tempd;
	TSG_Formula trans;

	scan = function;
	for (i = 0; i < npars; i++)
	{
		if (*scan++ != 'D')
			return fend;
		scan++;
	}

	if (!((scan == fend -(sizeof((char) 'F') + sizeof(char))
		&& *(fend - 2) == 'F' && m_Functions[*(fend - 1)].bVarying == 0) ||
		(scan == fend - sizeof(char)
		&& _is_Operand_Code(*(fend - 1))))
		)
		return fend;
	
	temp = *fend;
	*fend = '\0';

	trans.code = function;
	trans.ctable = m_ctable;
	tempd = _Get_Value(m_Parameters, trans);
	*fend = temp;
	*function++ = 'D';
	m_pctable -= npars;
	*function++ =(char) m_pctable;
	m_ctable[m_pctable++] = tempd;
	
	return function;
}

//---------------------------------------------------------
int CSG_Formula::_max_size(const char *source)
{
	int numbers		= 0;
	int functions	= 0;
	int operators	= 0;
	int variables	= 0;

	const size_t var_size	= 2 * sizeof(char);
	const size_t num_size	= sizeof(char) + sizeof(double);
	const size_t op_size	= sizeof(char);
	const size_t end_size	= sizeof('\0');

    for(int i=0; i<'z'-'a'; i++)
	{
		m_Vars_Used[i]	= false;
	}

	for(const char *scan=source; *scan; scan++)
	{
		if( isalpha(*scan) && (*scan != 'E') )
		{
			if( isalpha(*(scan + 1)) || isdigit(*(scan + 1)) )
			{
				// must be a function name (combination of letters and digits, e.g. sin(..), atan2(..))
			}
			else if( *(scan + 1) == '(' )
			{
				functions++;
			}
			else
			{
				variables++;
				m_Vars_Used[(int)(*scan - 'a')] = true;
			}
		}
	}

	if( _is_Operand(*source) )
	{
		operators++;
	}

	if( *source != '\0' )
	{
		for(const char *scan=source+1; *scan; scan++)
		{
			if( _is_Operand(*scan) && *(scan - 1) != 'E' )
			{
				operators++;
			}
		}
	}

	const char *scan	= source;

	while( *scan )
	{
		if( _is_Number(*scan) || ((*scan == '+' || *scan == '-') && scan > source && *(scan - 1) == 'E') )
		{
			numbers++;
			scan++;

			while( _is_Number(*scan) ||((*scan == '+' || *scan == '-') && scan > source && *(scan - 1) == 'E') )
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
char * CSG_Formula::_my_strtok(char *s)
{
	static char *token = NULL;

	if( s != NULL )
		token = s;
	else if( token != NULL )
		s = token;
	else 
		return( NULL );
	
	for(int pars=0; *s != '\0' && (*s != ',' || pars != 0); s++)
	{
		if (*s == '(') ++pars;
		if (*s == ')') --pars;
	}

	if( *s == '\0' )
	{
		s     = token;
		token = NULL;
	}
	else 
	{
		*s    = '\0';
		char *next_token = s + 1;
		s     = token;
		token = next_token;
	}

	return( s );
} 


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

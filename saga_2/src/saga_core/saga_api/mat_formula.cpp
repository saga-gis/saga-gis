
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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

// MEMORY LEAK detected:
//   Somewhere in this module is a (small) memory leak
//   (probably due to dirty formula string handling)
// OC 26.05.2006 !!!!

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>
#include <errno.h>
#include <ctype.h>

#include "mat_tools.h"
#include "grid.h"


#define MAXPAR 3
#define Max_ctable 255


static double f_pi		(void);
static double f_atan2	(double x, double val);
static double f_fmod	(double x, double val);
static double f_gt		(double x, double val);
static double f_lt		(double x, double val);
static double f_eq		(double x, double val);
static double f_int		(double x);
static double f_ifelse	(double sw, double x, double y);
static double f_N		(double a, double x, double y);

#define STD_LIB_NUM 20

static CSG_Formula::TMAT_Formula_Item gSG_Functions[Max_ctable]	=
{
	{"exp"		,							exp		, 1, 0},	//  1
	{"ln"		,							log		, 1, 0},	//  2
	{"sin"		,							sin		, 1, 0},	//  3
	{"cos"		,							cos		, 1, 0},	//  4
	{"tan"		,							tan		, 1, 0},	//  5
	{"asin"		,							asin	, 1, 0},	//  6
	{"acos"		,							acos	, 1, 0},	//  7
	{"atan"		,							atan	, 1, 0},	//  8
	{"atan2"	, (TMAT_Formula_Function_1)	f_atan2	, 2, 0},	//  9
	{"abs"		,							fabs	, 1, 0},	// 10
	{"sqrt"		,							sqrt	, 1, 0},	// 11
	{"gt"		, (TMAT_Formula_Function_1)	f_gt	, 2, 0},	// 12
	{"lt"		, (TMAT_Formula_Function_1)	f_lt	, 2, 0},	// 13
	{"eq"		, (TMAT_Formula_Function_1)	f_eq	, 2, 0},	// 14
	{"pi"		, (TMAT_Formula_Function_1)	f_pi	, 0, 0},	// 15
	{"int"		, (TMAT_Formula_Function_1)	f_int	, 1, 0},	// 16
	{"ifelse"	, (TMAT_Formula_Function_1)	f_ifelse, 3, 0},	// 17
	{"mod"		, (TMAT_Formula_Function_1)	f_fmod	, 2, 0},	// 18
	{"N"		, (TMAT_Formula_Function_1)	f_N		, 3, 0},	// 19
	{NULL		,							NULL	, 0, 0}
};

CSG_Formula::CSG_Formula()
{
	errmes = NULL;
}

inline int CSG_Formula::isoper(char c)
{
	return ((c == '+') ||(c == '-') ||(c == '*') ||(c == '/')
		||(c == '^'));
}

inline int CSG_Formula::is_code_oper(BYTE c)
{
	return ((c == '+') ||(c == '-') ||(c == '*') ||(c == '/')
		||(c == '^') ||(c == 'M'));
}

inline int CSG_Formula::isin_real(char c)
{
	return (isdigit(c) || c == '.' || c == 'E');
}

//---------------------------------------------------------
bool CSG_Formula::Set_Formula(const char *Source)
{
	if( Source )
	{
		m_Formula	= Source;

		function	= translate(Source, "abcdefghijklmnopqrstxyz", &Length, &Error_Pos);

		return( fnot_empty(function) != 0 );
	}

	return( false );
}

double CSG_Formula::Val()
{
	fset_error(NULL);
	return value(function);
}

void CSG_Formula::Set_Variable(char Var, double Value)
{
	param[Var - 'a'] = Value;
}

double CSG_Formula::Val(double x)
{
	fset_error(NULL);
	param['x'-'a'] = x;
	return value(function);
}

double CSG_Formula::Val(char *Args, ...)
{
	va_list ap;
	double result;
	fset_error(NULL);
	va_start(ap, Args);
	while (*Args)
		param[(*Args++) - 'a'] = va_arg(ap, double);
	va_end(ap);
	result = value(function);
	return result;
}

double CSG_Formula::Val(double *vals, int n)
{
	double result;
	
	for (int i = 0; i < n; i++)
		param[i] = vals[i];	 
	fset_error(NULL);
	result = value(function);
	return result;
}

//---------------------------------------------------------
CSG_String CSG_Formula::Get_Help_Operators(void)
{
	return(LNG(
		"+ Addition\n"
		"- Subtraction\n"
		"* Multiplication\n"
		"/ Division\n"
		"^ power\n"
		"abs(x)          - absolute value\n"
		"sqrt(x)         - square root\n"
		"ln(x)           - natural logarithm\n"
		"exp(x)          - exponential\n"
		"sin(x)          - sine\n"
		"cos(x)          - cosine\n"
		"tan(x)          - tangent\n"
		"asin(x)         - arcsine\n"
		"acos(x)         - arccosine\n"
		"atan(x)         - arctangent\n"
		"atan2(x, y)     - arctangent of x/y\n"
		"gt(x, y)        - if x>y the result is 1.0, else 0.0\n"
		"lt(x, y)        - if x<y the result is 1.0, else 0.0\n"
		"eq(x, y)        - if x=y the result is 1.0, else 0.0\n"
		"mod(x, y)       - returns the floating point remainder of x/y\n"
		"ifelse(c, x, y) - if c=1 the result is x, else y\n"
		"int(x)          - integer part of floating point value x\n"
		"pi()            - returns the value of Pi\n"
	));
}

//---------------------------------------------------------
CSG_String CSG_Formula::Get_Help_Usage(void)
{
	return(LNG(
		"Use single characters to define the parameters of your function f(x)\n"
		"Example: 'a + b * x'\n"
	));
}

//---------------------------------------------------------
bool CSG_Formula::Get_Error(int *pos, const char **msg)
{
	if( !fnot_empty(function) )
	{
		if( pos )
		{
			*pos	= Error_Pos;
		}

		if( msg )
		{
			*msg	= errmes;
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
const char * CSG_Formula::Get_Used_Var(void)
{
	static CSG_String	ret;

	ret.Clear();

	for(int i=0; i<'z'-'a'; i++)
	{
		if( used_vars[i] == true )
		{
			ret.Append((char)(i + 'a'));
		}
	}

	return( ret );
}
/*{	
	int		i, count;
	char	*ret;

	for(i=0, count=0; i<'z'-'a'; i++)
	{
		if( used_vars[i] == true )
		{
			count++;
		}
	}

	ret	= (char *)malloc(count + 1);

	for(i=0, count=0; i<'z'-'a'; i++)
	{
		if( used_vars[i] == true )
		{
			ret[count++]	= (char)(i + 'a');
		}
	}

	ret[count]	= (char)0;

	return ret;
}*/

int CSG_Formula::Del_Function(char *name)
/* If the function exists, it is deleted and a non - negative value
    is returned. */
/* Otherwise, -1 is returned. */
/* Original library functions may not be deleted. */
{
	int place;
	TMAT_Formula_Item *scan;
	
	if ((place = where_table(name)) == -1)
		return -1; /* there is an error message already */
	if (place < STD_LIB_NUM)
	{
		fset_error("original functions may not be deleted");
		return -1;
	}
	free(gSG_Functions[place].name);
	for (scan = &gSG_Functions[place]; scan->f != NULL; scan++)
	{
		scan->name  =(scan + 1)->name;
		scan->f     =(scan + 1) -> f;
		scan->n_pars =(scan + 1) -> n_pars;
	}
	fset_error(NULL);
	return scan - gSG_Functions;
} /*end of fdel */


//---------------------------------------------------------
/**
 * int varying;  Does the result of the function vary
 * even when the parameters stay the same?
 * varying = 1 for e.g. random - number generators.
 * Result: 0 is rendered if there is an error
 * 1 is rendered otherwise
*/
int CSG_Formula::Add_Function(char *name, TMAT_Formula_Function_1 f, int n_pars, int varying)
{
	TMAT_Formula_Item *where;
	
	if (n_pars < 0 || n_pars>3)
	{
		fset_error("invalid number of parameters");
		return 0;
	}
	for (where = gSG_Functions; where->f != NULL && strcmp(name, where->name); where++)
	{
		;
	}
	if (where->f != NULL)
	{
		where->f = f;
		where->varying = varying;
		where->n_pars = n_pars;   /*old function is superseded */
		fset_error(NULL);
		return 1;
	} else if ((where - gSG_Functions) >= Max_ctable - 1)
	{
		fset_error("function table full");
		return 0;
	}
	else 
	{
		where->name =(char *) calloc(strlen(name) + 1, sizeof(char));
		if (where->name == NULL)
		{
			fset_error("no memory");
			return 0;
		}
		strcpy(where->name, name);
		where->f = f;
		where->varying = varying;
		where->n_pars = n_pars;
		fset_error(NULL);
		return 1;
	}
}  /* end of fnew */


//-----------------------------------------------------------------------------


int CSG_Formula::read_table(int i, char *name, int *n_pars, int *varying)
{
	if (!gSG_Functions[i].f)
	{
		fset_error("index out of bounds");
		return 0;
	}
	else 
	{
		strcpy(name, gSG_Functions[i].name);
		*n_pars = gSG_Functions[i].n_pars;
		*varying = gSG_Functions[i].varying;
		fset_error(NULL);
		return 1;
	}
}

int CSG_Formula::where_table(char *name)
/* If the function exists, where_table() returns the index of its name
    in the table. Otherwise, it returns -1. */
{
	TMAT_Formula_Item *table_p;
	
	for (table_p = gSG_Functions; table_p->f != NULL &&
        strcmp(name, table_p->name); table_p++)
		;
		if (table_p->f == NULL) /*The end of the table has been reached,
			but name[] is not there. */
		{
			fset_error("function not found");
			return -1;
		}
		else 
		{
			fset_error(NULL);
			return table_p - gSG_Functions;
		}
}


void CSG_Formula::fset_error(char *s)
/* fset_error(NULL) and fset_error("") erase
   any previous error message */
{
	if (s == NULL || *s == '\0')
	errmes = NULL; /* an empty error message means
	that there is no error */
	else 
		errmes = s;
}

const char *CSG_Formula::fget_error(void)
{
	return errmes;
}


#define BUFSIZE 500
double CSG_Formula::value(TMAT_Formula func)
{
	double buffer[BUFSIZE];
	register double *bufp = buffer;
	/* points to the first free space in the buffer */
	double x, y, z;
	register double result;
	register BYTE *function = func.code;
	register double *ctable = func.ctable;
	
	if (!function)
	{
		fset_error("empty coded function");
		return 0;
		/* non - existent function; result of
		an unsuccesful call to translate */
	}
	for (;;)
	{
		switch (*function++)
		{
			case '\0':goto finish; /* there is a reason for this "goto":
				this function must be as fast as possible */
			case 'D': 
				*bufp++ = ctable[*function++];
				break;
			case 'V': 
				*bufp++ = param[(*function++) - 'a'];
				break;
			case 'M':result = -(*--bufp);
				*bufp++ = result;
				break;
			case '+':y = *(--bufp);
				result = y + *(--bufp);
				*bufp++ = result;
				break;
			case '-':y = *--bufp;
				result= *(--bufp) - y;
				*bufp++ = result;
				break;
			case '*':y = *(--bufp);
				result = *(--bufp) * y;
				*bufp++ = result;
				break;
			case '/':y = *--bufp;
				result = *(--bufp) / y;
				*bufp++ = result;
				break;
			case '^':y = *--bufp;
				result = pow(*(--bufp), y);
				*bufp++ = result;
				break;
			case 'F':switch (gSG_Functions[*function].n_pars)
					 {
			case 0:*bufp++ =((TMAT_Formula_Function_0)gSG_Functions[*function++].f)();
				break;
			case 1:x = *--bufp;
				*bufp++ = gSG_Functions[*function++].f(x);
				break;
			case 2:y = *--bufp;
				x = *--bufp;
				*bufp++ =((TMAT_Formula_Function_2)gSG_Functions[*function++].f)(x, y);
				break;
			case 3:z = *--bufp;
				y = *--bufp;
				x = *--bufp;
				*bufp++ =((TMAT_Formula_Function_3)gSG_Functions[*function++].f)(x, y, z);
				break;
			default:fset_error("I2: too many parameters\n");
				return 0;
					 }
				break;
			default:fset_error("I1: unrecognizable operator");
				return 0;
		}
	}
finish: if ((bufp - buffer) != 1)
			fset_error("I3: corrupted buffer");
		return buffer[0];
} 


void CSG_Formula::destrf(TMAT_Formula old)
{
	fset_error(NULL);
	SG_Free(old.code);
	SG_Free(old.ctable);
}

void CSG_Formula::make_empty(TMAT_Formula f)
{
	fset_error(NULL);
	f.code = NULL;
	f.ctable = NULL;
}

int CSG_Formula::fnot_empty(TMAT_Formula f)
{
	return (f.code != NULL);
}

/*********************************************************/
/* Interpreting functions                                */


int CSG_Formula::max_size(const char *source)
{
	int numbers = 0;
	int functions = 0;
	int operators = 0;
	int variables = 0;
	
	const size_t var_size = 2*sizeof(BYTE);
	const size_t num_size = sizeof(BYTE) + sizeof(double);
	const size_t op_size = sizeof(BYTE);
	const size_t end_size = sizeof('\0');
	
	const char *scan;
    for (int i =0; i < 'z'-'a'; i++)
		used_vars[i]= false;
	
	for (scan = source; *scan; scan++)
		if (isalpha(*scan) &&(*scan != 'E'))
		{
			if (isalpha(*(scan + 1)))
			; /* it is a function name,
			it will be counted later on */
			else
				if (
					*(scan + 1) == '(')  functions++;
				else
				{
					variables++;
					used_vars[(int)(*scan - 'a')] = true;
				}
		}
		
		if (isoper(*source))
			operators++;
		if (*source != '\0')
			for (scan = source + 1; *scan; scan++)
				if (isoper(*scan) && *(scan - 1) != 'E')
					operators++;
				
				scan = source;
				while (*scan)
					if (isin_real(*scan) ||((*scan == '+' || *scan == '-') &&
						scan>source && *(scan - 1) == 'E'))
					{
						numbers++;
						scan++;
						while (isin_real(*scan) ||((*scan == '+' || *scan == '-') &&
							scan>source && *(scan - 1) == 'E'))
							scan++;
					}
					else 
						scan++;
					
					return (numbers*num_size + operators*op_size + functions*num_size
						+ variables*var_size + end_size);
}


CSG_Formula::TMAT_Formula CSG_Formula::translate(const char *sourc, const char *args, int *leng, int *error)
{
	BYTE *result;
	char *source;
	const char *scan, *scarg;
	BYTE *function;
	BYTE *nfunc; 
	size_t size_estim; 
	
	double *ctable;
	TMAT_Formula returned; 
	
	
	i_error = NULL;
	
	source =(char *) SG_Malloc(strlen(sourc) + 1);
	if (source == NULL) 
	{
		fset_error("no memory");
		*leng = 0;
		*error = 0; 
		returned.code = NULL;
		returned.ctable = NULL;
		return (returned);
	}
	strcpy(source, sourc);
	
	for (scan = source; *scan != '\0'; scan++)
	{
		if (islower(*scan) && !isalpha(*(scan + 1)) &&
			(scan == source || !isalpha(*(scan - 1)))) 
		{
			for (scarg = args; *scarg != '\0' && *scarg != *scan; scarg++)
				;
			if (*scarg == '\0') 
			{
				i_error = scan;
				
				fset_error("undeclared parameter");
				*leng = 0;
				*error = i_error - source;
				returned.code = NULL;
				returned.ctable = NULL;
				SG_Free(source);
				return (returned);
			}
		}
	}  
	
	size_estim = max_size(source); 
	
	if (!(function =(BYTE *) SG_Malloc(size_estim))) 
	{
		fset_error("no memory");
		*leng = 0;
		*error = -1;
		returned.code = NULL;
		returned.ctable = NULL;
		SG_Free(source);
		return (returned);
	}
	
	
	i_pctable = 0;
	if (!(i_ctable =(double *) SG_Malloc(Max_ctable * sizeof(double)))) 
	{
		fset_error("no memory");
		SG_Free(function);
		*leng = 0;
		*error = -1;
		returned.code = NULL;
		returned.ctable = NULL;
		SG_Free(source);
		return (returned);
	}
	ctable = i_ctable;
	
	fset_error(NULL);
	
	result = i_trans(function, (char *) source, (char *) source + strlen(source));
	
	if (!result || fget_error()) 
	{
		SG_Free(function);
		SG_Free(i_ctable);
		*leng = 0;
		if (i_error)
			*error = i_error - source;
		else 
			*error = -1; 
		returned.code = NULL;
		returned.ctable = NULL;
		SG_Free(source);
		return (returned);
	}
	else 
	{ 
		*result = '\0';
		*error = -1;
		*leng = result - function;
		
		
		if (((*leng) + 1) * sizeof(BYTE) > size_estim)
			
		{
			fset_error("I4: size estimate too small");
			returned.code = NULL;
			returned.ctable = NULL;
			SG_Free(source);
			return (returned);
		}
		else if (((*leng) + 1) * sizeof(BYTE) < size_estim) 
		{
			nfunc =(BYTE *) SG_Malloc(((*leng) + 1) * sizeof(BYTE));
			if (nfunc) 
			{
				memcpy(nfunc, function, ((*leng) + 1) * sizeof(BYTE));
				SG_Free(function);
				function = nfunc;
			}
							 } 
		if (i_pctable < Max_ctable) 
		{
			ctable =(double *) SG_Malloc(i_pctable * sizeof(double));
			if (ctable) 
			{
				memcpy(ctable, i_ctable, i_pctable * sizeof(double));
				SG_Free(i_ctable);
			}
			else 
				ctable = i_ctable;
		}
		else 
			ctable = i_ctable;
		
		returned.code = function;
		returned.ctable = ctable;
		fset_error(NULL);
		SG_Free(source);
		return (returned);
	} 
}  

BYTE *CSG_Formula::comp_time(BYTE *function, BYTE *fend, int npars)
 
{
	BYTE *scan;
	BYTE temp;
	double tempd;
	int i;
	TMAT_Formula trans;
	
	scan = function;
	for (i = 0; i < npars; i++)
	{
		if (*scan++ != 'D')
			return fend;
		scan++;
	}
	
	if (!((scan == fend -(sizeof((BYTE) 'F') + sizeof(BYTE))
		&& *(fend - 2) == 'F' && gSG_Functions[*(fend - 1)].varying == 0) ||
		(scan == fend - sizeof(BYTE)
		&& is_code_oper(*(fend - 1))))
		)
		return fend;
	
	temp = *fend;
	*fend = '\0';
	
	trans.code = function;
	trans.ctable = i_ctable;
	tempd = value(trans);
	*fend = temp;
	*function++ = 'D';
	i_pctable -= npars;
	*function++ =(BYTE) i_pctable;
	i_ctable[i_pctable++] = tempd;
	
	return function;
} 

char *CSG_Formula::my_strtok(char *s)
{
	int pars;
	static char *token = NULL;
	char *next_token;
	
	if (s != NULL)
		token = s;
	else if (token != NULL)
		s = token;
	else 
		return NULL;
	
	for (pars = 0; *s != '\0' &&(*s != ',' || pars != 0); s++)
	{
		if (*s == '(') ++pars;
		if (*s == ')')
			--pars;
	}
	if (*s == '\0')
	{
		next_token = NULL;
		s = token;
		
		token = next_token;
		return s;
	}
	else 
	{
		*s = '\0';
		next_token = s + 1;
		s = token;
		
		token = next_token;
		return s;
	}
} 



BYTE *CSG_Formula::i_trans(BYTE *function, char *begin, char *end)
{
	int pars;     
	char *scan;
	BYTE *tempu, *temp3;
	char *temps	= NULL;
	char tempch;
	double tempd;
	char *endf;     
	
	int n_function;
	int space;
	int i;
	
	char *paramstr[MAXPAR];
	char *par_buf;
	
	if (begin >= end)
	{
		fset_error("missing operand");
		i_error = begin;
		return NULL;
	}
	
	for (pars = 0, scan = begin; scan < end && pars >= 0; scan++)
	{
		if (*scan == '(') pars++;
		else if (*scan == ')')
			pars--;
	}
	if (pars < 0 || pars > 0)
	{
		fset_error("unmatched parentheses");
		i_error = scan - 1;
		return NULL;
	}
	
	for (pars = 0, scan = end - 1; scan >= begin; scan--)
	{
		if (*scan == '(') pars++;
		else if (*scan == ')')
			pars--;
		else if (!pars &&(*scan == '+' ||((*scan == '-') && scan != begin))
			&&(scan == begin || *(scan - 1) != 'E'))
			break;
	}
	
	if (scan >= begin)
	{                                 
		if ((tempu = i_trans(function, begin, scan)) &&      
			(temp3 = i_trans(tempu, scan + 1, end)))
		{
			*temp3++ = *scan; 
			temp3 = comp_time(function, temp3, 2); 
			if (fget_error())
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
		if ((tempu = i_trans(function, begin, scan)) &&      
			(temp3 = i_trans(tempu, scan + 1, end)))
		{
			*temp3++ = *scan; 
			temp3 = comp_time(function, temp3, 2); 
			if (fget_error())
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
		tempu = i_trans(function, begin + 1, end);
		if (tempu)
		{
			*tempu++ = 'M';
			tempu = comp_time(function, tempu, 1); 
			if (fget_error())
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
	}
	
	if (scan >= begin)
	{                                 
		if ((tempu = i_trans(function, begin, scan)) &&      
			(temp3 = i_trans(tempu, scan + 1, end)))
		{
			*temp3++ = *scan; 
			temp3 = comp_time(function, temp3, 2); 
			if (fget_error())
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
		return i_trans(function, begin + 1, end - 1);
	
	if (end == begin + 1 && islower(*begin))
	{
		*function++ = 'V';
		*function++ = *begin;
		return function;
	}
	
	tempch = *end;
	*end = '\0';
	tempd = strtod(begin, (char**) &tempu);
	*end = tempch;
	if ((char*) tempu == end)
	{
		*function++ = 'D';
		if (i_pctable < Max_ctable)
		{
			i_ctable[i_pctable] = tempd;
			*function++ =(BYTE) i_pctable++;
		}
		else
		{
			fset_error("too many constants");
			i_error = begin;
			return NULL;
		}
		return function;
	}
	
				/*function*/
	if (!isalpha(*begin) && *begin != '_')
	{
		fset_error("syntax error");
		i_error = begin;
		return NULL;
	}
	for (endf = begin + 1; endf < end &&(isalnum(*endf) || *endf == '_');
	endf++)
		;
	tempch = *endf;
	*endf = '\0';
	if ((n_function = where_table(begin)) == -1)
	{
		*endf = tempch;
		i_error = begin;
		return NULL;
	}
	*endf = tempch;
	if (*endf != '(' || *(end - 1) != ')')
	{
		fset_error("improper function syntax");
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
				*function++ = 'F';
				*function++ = n_function;
				function = comp_time(function - 2, function, 0);
				if (fget_error())
					return NULL; /* internal error in comp_time */
				else 
					return function;
			}
			else 
			{
				i_error = endf + 1;
				fset_error("too many parameters");
				return NULL;
			}
	}
	else 
	{    /*function with parameters*/
		tempch = *(end - 1);
		*(end - 1) = '\0';
		par_buf =(char *) SG_Malloc(strlen(endf + 1) + 1);
		if (!par_buf)
		{    
			fset_error("no memory"); 
			i_error = NULL;  
			return NULL;   
		}
		
		strcpy(par_buf, endf + 1);
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
			fset_error("too few parameters");
			return NULL;
		}
		if ((temps = my_strtok(NULL)) != NULL)
		{
			SG_Free(par_buf);
			i_error =(temps - par_buf) +(endf + 1); 
			fset_error("too many parameters");
			return NULL;
		}
		
		tempu = function;
		for (i = 0; i < gSG_Functions[n_function].n_pars; i++)
			if (!(tempu = i_trans(tempu, paramstr[i],
				paramstr[i] + strlen(paramstr[i]))))
			{
				i_error =(i_error - par_buf) +(endf + 1); 
				SG_Free(par_buf);
				
				return NULL; 
			}
			/* OK */
			SG_Free(par_buf);
			*tempu++ = 'F';
			*tempu++ = n_function;
			tempu = comp_time(function, tempu, gSG_Functions[n_function].n_pars);
			if (fget_error())
				return NULL; /* internal error in comp_time */
			else 
				return tempu;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define eps 1e-9

//---------------------------------------------------------
static double f_pi(void)
{
	return( M_PI );
}

//---------------------------------------------------------
static double f_atan2(double x, double val)
{
	return( atan2(x, val) );
}

//---------------------------------------------------------
static double f_fmod(double x, double val)
{
	return( fmod(x, val) );
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
	return( fabs(x - val) < eps ? 1.0 : 0.0 );
}

//---------------------------------------------------------
static double f_int(double x)
{
	return( (int)(x) );
}

//---------------------------------------------------------
static double f_ifelse(double condition, double x, double y)
{
	return( fabs(condition - 1.0) < eps ? x : y );
}

//---------------------------------------------------------
static double f_N(double a, double x, double y)
{
/*	int xpos, ypos;
	xpos =(int)(_x_ + x);
	ypos =(int)(_y_ + y);

  if (_last_ < 0)
		return a;
		
		  if (xpos >= 0 && ypos >= 0 && xpos <grid[_last_]->Get_NX() && ypos < grid[_last_]->Get_NY())
		  
			return grid[_last_]->asDouble(xpos, ypos);
			
			  else
			  {
			  double nix = 0.0;
			  return 1.0/nix;
			}
*/	
	return 1.0;
}

/*class func
{
public:
	virtual double operator()(double n);
};*/


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

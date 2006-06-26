
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
//                     mat_tools.h                       //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
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
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__SAGA_API__mat_tools_H
#define HEADER_INCLUDED__SAGA_API__mat_tools_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <math.h>

#include "geo_tools.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef M_PI
#define M_PI						3.141592653589793
#endif

#define M_PI_045					(M_PI / 4.0)
#define M_PI_090					(M_PI / 2.0)
#define M_PI_135					(M_PI * 3.0 / 4.0)
#define M_PI_180					(M_PI)
#define M_PI_225					(M_PI * 5.0 / 4.0)
#define M_PI_270					(M_PI * 3.0 / 2.0)
#define M_PI_315					(M_PI * 7.0 / 4.0)
#define M_PI_360					(M_PI * 2.0)

#define M_RAD_TO_DEG				(180.0 / M_PI)
#define M_DEG_TO_RAD				(M_PI / 180.0)

//---------------------------------------------------------
#define M_EULER						2.718281828459045

//---------------------------------------------------------
#define N_MEGABYTE_BYTES			0x100000

//---------------------------------------------------------
#define M_ALMOST_ZERO				0.001l
#define M_TINY						(1.0e-20)

//---------------------------------------------------------
#define M_SQR(x)					((x) * (x))
#define M_SQRT(x)					sqrt((double)(x))
#define M_GET_DIST(x, y)			sqrt((double)((x)*(x) + (y)*(y)))

#define M_GET_MIN(a, b)				(((a) < (b)) ? (a) : (b))
#define M_GET_MAX(a, b)				(((a) > (b)) ? (a) : (b))
#define M_SET_MINMAX(min, max, x)	if( min > x ) { min = x; } else if( max < x ) { max = x; }


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
SAGA_API_DLL_EXPORT double			MAT_Square				(double x);

//---------------------------------------------------------
SAGA_API_DLL_EXPORT int *			MAT_Create_Index		(int nValues, int    *Values, bool bAscending);
SAGA_API_DLL_EXPORT bool			MAT_Create_Index		(int nValues, int    *Values, bool bAscending, int *Index);

SAGA_API_DLL_EXPORT int *			MAT_Create_Index		(int nValues, double *Values, bool bAscending);
SAGA_API_DLL_EXPORT bool			MAT_Create_Index		(int nValues, double *Values, bool bAscending, int *Index);

SAGA_API_DLL_EXPORT int *			MAT_Create_Index		(int nValues, int (*Compare)(const int iElement_1, const int iElement_2), bool bAscending);
SAGA_API_DLL_EXPORT bool			MAT_Create_Index		(int nValues, int (*Compare)(const int iElement_1, const int iElement_2), bool bAscending, int *Index);

//---------------------------------------------------------
SAGA_API_DLL_EXPORT void **			MATRIX_Alloc			(int ny, int nx, int Value_Size);
SAGA_API_DLL_EXPORT void			MATRIX_Free				(void **Matrix);

SAGA_API_DLL_EXPORT bool			MATRIX_Invert			(int nSize, double **Matrix);
SAGA_API_DLL_EXPORT bool			MATRIX_LU_Decomposition	(int nSize, double **Matrix, int *Index);
SAGA_API_DLL_EXPORT void			MATRIX_Solve			(int nSize, double **Matrix, int *Index, double *Vector);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CMAT_Spline
{
public:
	CMAT_Spline(void);
	virtual ~CMAT_Spline(void);

	void						Destroy				(void);

	void						Add_Value			(double  x, double  y);
	void						Set_Values			(double *x, double *y, int n);

	void						Initialize			(double y_A = 1.0e30, double y_B = 1.0e30);

	bool						Get_Value			(double x, double &y);


protected:

	bool						m_bSplined;

	int							m_nPoints, m_nBuffer;

	double						*m_xPoints, *m_yPoints, *m_zPoints;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum
{
	REGRESSION_Linear	= 0,	// Y = a + b * X
	REGRESSION_Rez_X,			// Y = a + b / X
	REGRESSION_Rez_Y,			// Y = a / (b - X)
	REGRESSION_Pow,				// Y = a * X^b
	REGRESSION_Exp,				// Y = a * e^(b * X)
	REGRESSION_Log				// Y = a + b * ln(X)
}
TMAT_Regression_Type;

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CMAT_Regression
{
public:
	CMAT_Regression(void);
	virtual ~CMAT_Regression(void);

	void						Destroy				(void);

	void						Set_Values			(int nValues, double *x, double *y);
	void						Add_Values			(double x, double y);

	int							Get_Count			(void)			{	return( m_nValues );	}

	double						Get_xValue			(int iValue)	{	return( iValue >= 0 && iValue < m_nValues ? m_x[iValue] : 0.0 );	}
	double						Get_yValue			(int iValue)	{	return( iValue >= 0 && iValue < m_nValues ? m_y[iValue] : 0.0 );	}
	bool						Get_Values			(int iValue, double &x, double &y)
	{
		if( iValue >= 0 && iValue < m_nValues )
		{
			x	= m_x[iValue];
			y	= m_y[iValue];

			return( true );
		}

		return( false );
	}

	double						Get_xMin			(void)			{	return( m_xMin );	}
	double						Get_xMax			(void)			{	return( m_xMax );	}
	double						Get_xMean			(void)			{	return( m_xMean );	}
	double						Get_xVariance		(void)			{	return( m_xVar );	}
	double						Get_x				(double y);	// returns INF on error, this can be checked using the _finite() function (libc, include <float.h>)...

	double						Get_yMin			(void)			{	return( m_yMin );	}
	double						Get_yMax			(void)			{	return( m_yMax );	}
	double						Get_yMean			(void)			{	return( m_yMean );	}
	double						Get_yVariance		(void)			{	return( m_yVar );	}
	double						Get_y				(double x);	// returns INF on error, this can be checked using the _finite() function (libc, include <float.h>)...

	double						Get_Constant		(void)			{	return( m_RConst );	}
	double						Get_Coefficient		(void)			{	return( m_RCoeff );	}
	double						Get_R				(void)			{	return( m_R );		}
	double						Get_R2				(void)			{	return( m_R*m_R );	}

	const char *				asString			(void);

	TMAT_Regression_Type		Get_Type			(void)			{	return( m_Type );	}

	bool						Calculate			(TMAT_Regression_Type Type = REGRESSION_Linear);
	bool						Calculate			(int nValues, double *x, double *y, TMAT_Regression_Type Type = REGRESSION_Linear);


protected:

	int							m_nValues, m_nBuffer;

	double						m_RConst, m_RCoeff, m_R,
								m_xMin, m_xMax, m_xMean, m_xVar, *m_x,
								m_yMin, m_yMax, m_yMean, m_yVar, *m_y;

	TMAT_Regression_Type		m_Type;


	bool						_Get_MinMeanMax		(double &xMin, double &xMean, double &xMax, double &yMin, double &yMean, double &yMax);

	double						_Y_Transform		(double x);
	double						_X_Transform		(double y);

	bool						_Linear				(void);

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CMAT_Regression_Multiple
{
public:
	CMAT_Regression_Multiple(void);
	virtual ~CMAT_Regression_Multiple(void);

	void						Destroy				(void);

	bool						Calculate			(class CTable *pValues);

	class CTable *				Get_Result			(void)	{	return( m_pResult );	}

	int							Get_Ordered			(int iOrder);
	int							Get_Order			(int iVariable);
	double						Get_R2				(int iVariable);
	double						Get_R2_Change		(int iVariable);
	double						Get_RConst			(void);
	double						Get_RCoeff			(int iVariable);


protected:

	class CTable				*m_pResult;


	bool						_Get_Regression		(class CTable *pValues);

	bool						_Get_Correlation	(class CTable *pValues);
	bool						_Get_Correlation	(int nValues, int nVariables, double **X, double *Y, int &iMax, double &rMax);

	bool						_Eliminate			(int nValues, double *X, double *Y);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CMAT_Grid_Radius
{
public:
	CMAT_Grid_Radius(int max_Radius = 0);
	~CMAT_Grid_Radius(void);

	bool						Create		(int max_Radius);
	void						Destroy		(void);

	int							Get_Maximum	(void)			{	return( max_Radius );	}

	int							Get_nPoints	(int iRadius)	{	return( iRadius >= 0 && iRadius < max_Radius ? nPoints[iRadius] : 0 );	}

	double						Get_Point	(int iRadius, int iPoint, int &x, int &y)
	{
		if( iRadius >= 0 && iRadius < max_Radius && iPoint >= 0 && iPoint < nPoints[iRadius] )
		{
			x	= Points[iRadius][iPoint].x;
			y	= Points[iRadius][iPoint].y;

			return( Points[iRadius][iPoint].d );	// Distance...
		}

		return( -1.0 );
	}


private:

	int							max_Radius, *nPoints;

	typedef struct
	{
		int						x, y;
		double					d;
	}
	TMAT_Grid_Radius;

	TMAT_Grid_Radius			**Points;

};


///////////////////////////////////////////////////////////
//														 //
//				Formula Parser (A. Ringeler)			 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef double (*TMAT_Formula_Function_0)(void);
typedef double (*TMAT_Formula_Function_1)(double);
typedef double (*TMAT_Formula_Function_2)(double, double);
typedef double (*TMAT_Formula_Function_3)(double, double, double);

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CMAT_Formula
{
public:
	CMAT_Formula(void);

	static CAPI_String			Get_Help_Operators	(void);
	static CAPI_String			Get_Help_Usage		(void);

	bool						Get_Error			(int *Pos = NULL, const char **Msg = NULL);

	double						Val(double *Vals, int n);
	double						Val(void);
	double						Val(char *Args, ...);
	double						Val(double x);

	void						Set_Variable(char Variable, double Value);

	bool						Set_Formula(const char *Formula);
	CAPI_String					Get_Formula(void)	{	return( m_Formula );	}

	int							Del_Function(char *Name);
	int							Add_Function(char *Name, TMAT_Formula_Function_1 f, int N_of_Pars, int Varying);

	const char *				Get_Used_Var(void);


	//-----------------------------------------------------
	typedef struct 
	{
		char					*name;
		TMAT_Formula_Function_1	f;			
		int						n_pars;		
		int						varying;	// Does the result of the function vary even when the parameters stay the same? varying = 1 for e.g. random - number generators.
	}
	TMAT_Formula_Item;


private:

	//-----------------------------------------------------
	typedef struct 
	{
		BYTE					*code;
		double					*ctable;
	}
	TMAT_Formula;


	//-----------------------------------------------------
	CAPI_String					m_Formula;

	const char					*i_error; 
	bool						used_vars['z' - 'a' + 1];
	char						*errmes;
	int							i_pctable;	// number of items in a table of constants - used only by the translating functions
	int							Error_Pos;
	int							Length;
	double						*i_ctable;	// current table of constants - used only by the translating functions
	double						param['z' - 'a' + 1];
	TMAT_Formula				function;


	TMAT_Formula				translate(const char *source, const char *args, int *length, int *error);
	int							fnot_empty(TMAT_Formula);
	double						grid_value(TMAT_Formula func);
	void						destrf(TMAT_Formula);
	void						make_empty(TMAT_Formula);
	const char *				fget_error(void);
	int							read_table(int i, char *name, int *n_pars, int *varying);
	int							where_table(char *name);
	void						fset_error(char *s);
	double						value(TMAT_Formula func);
	int							max_size(const char *source);
	char *						my_strtok(char *s);
	BYTE *						i_trans(BYTE *function, char *begin, char *end);
	BYTE *						comp_time(BYTE *function, BYTE *fend, int npars);

	int							isoper(char c);
	int							is_code_oper(BYTE c);
	int							isin_real(char c);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CMAT_Trend
{
public:
	CMAT_Trend(void);
	virtual ~CMAT_Trend(void);

	bool						Set_Formula			(const char *Formula = NULL);
	CAPI_String					Get_Formula			(void);

	int							Get_Parameter_Count	(void) const	{	return( m_Params.m_Count );		}
	double *					Get_Parameters		(void) const	{	return( m_Params.m_A );			}

	void						Clr_Data			(void);
	void						Set_Data			(double *xData, double *yData, int nData, bool bAdd = false);
	void						Set_Data			(const CAPI_dPoints &Data, bool bAdd = false);
	void						Add_Data			(double x, double y);
	int							Get_Data_Count		(void) const	{	return( m_Data.Get_Count() );	}
	double						Get_Data_X			(int Index)		{	return( m_Data.Get_X(Index) );	}
	double						Get_Data_Y			(int Index)		{	return( m_Data.Get_Y(Index) );	}

	bool						Set_Max_Iterations	(int Iterations);
	int							Get_Max_Iterations	(void)			{	return( m_Iter_Max);	}
	bool						Set_Max_Lambda		(double Lambda);
	double						Get_Max_Lambda		(void)			{	return( m_Lambda_Max);	}

	bool						Get_Trend			(double *xData, double *yData, int nData, const char *Formula = NULL);
	bool						Get_Trend			(const CAPI_dPoints &Data, const char *Formula = NULL);
	bool						Get_Trend			(void);

	bool						is_Okay				(void)			{	return( m_bOkay );		}

	CAPI_String					Get_Error			(void);

	double						Get_ChiSquare		(void);
	double						Get_R2				(void);

	double						Get_Value			(double x);


private:

	//-----------------------------------------------------
	class SAGA_API_DLL_EXPORT CFncParams
	{
	public:
		CFncParams(void);
		virtual ~CFncParams(void);

		bool					Create		(const char *Variables, int nVariables);
		bool					Destroy		(void);

		int						m_Count;

		char					*m_Variables;

		double					*m_A, *m_Atry, *m_dA, *m_dA2, *m_Beta, **m_Alpha, **m_Covar;

	};


	//-----------------------------------------------------
	bool						m_bOkay;

	int							m_Iter_Max;

	double						m_ChiSqr, m_ChiSqr_o, m_Lambda, m_Lambda_Max;

	CAPI_dPoints				m_Data;

	CFncParams					m_Params;

	CMAT_Formula				m_Formula;


	bool						_Fit_Function		(void);
	bool						_Get_Gaussj			(void);
	bool						_Get_mrqcof			(double *Parameters, double **Alpha, double *Beta);

	void						_Get_Function		(double x, double *Parameters, double &y, double *dy_da);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__mat_tools_H

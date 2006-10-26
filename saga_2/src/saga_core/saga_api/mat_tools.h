
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
#ifndef SWIG

#include <math.h>

#endif	// #ifdef SWIG

//---------------------------------------------------------
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
#define M_GET_LENGTH(x, y)			sqrt((double)((x)*(x) + (y)*(y)))

#define M_GET_MIN(a, b)				(((a) < (b)) ? (a) : (b))
#define M_GET_MAX(a, b)				(((a) > (b)) ? (a) : (b))
#define M_SET_MINMAX(min, max, x)	if( min > x ) { min = x; } else if( max < x ) { max = x; }


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
SAGA_API_DLL_EXPORT double		SG_Get_Square		(double x);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef int (* TSG_PFNC_Compare) (const int iElement_1, const int iElement_2);

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Index
{
public:
	CSG_Index(void);
	virtual ~CSG_Index(void);

								CSG_Index			(int nValues, int    *Values, bool bAscending = true);
	bool						Create				(int nValues, int    *Values, bool bAscending = true);

								CSG_Index			(int nValues, double *Values, bool bAscending = true);
	bool						Create				(int nValues, double *Values, bool bAscending = true);

								CSG_Index			(int nValues, TSG_PFNC_Compare fCompare, bool bAscending = true);
	bool						Create				(int nValues, TSG_PFNC_Compare fCompare, bool bAscending = true);

	bool						Destroy				(void);

	bool						is_Okay				(void)						const	{	return( m_nValues > 0 );	}
	int							Get_Count			(void)						const	{	return( m_nValues );		}
	int							Get_Index			(int i, bool bInv = false)	const	{	return( i >= 0 && i < m_nValues ? m_Index[bInv ? m_nValues-1-i : i] : -1 );	}
	int							operator []			(int i)						const	{	return( i >= 0 && i < m_nValues ? m_Index[i] : -1 );	}


private:

	void						*m_Values;

	int							m_nValues, *m_Index, m_iCompare;

	TSG_PFNC_Compare			m_fCompare;


	void						_On_Construction	(void);

	bool						_Set_Array			(int nValues);
	bool						_Set_Index			(bool bAscending);

	int							_Compare			(const int iElement_1, const int iElement_2);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Vector
{
public:
	CSG_Vector(void);
	virtual ~CSG_Vector(void);

								CSG_Vector			(const CSG_Vector &Vector);
	bool						Create				(const CSG_Vector &Vector);

								CSG_Vector			(int n, double *Data = NULL);
	bool						Create				(int n, double *Data = NULL);

	bool						Destroy				(void);

	int							Get_N				(void)	const	{	return( m_n );		}
	double *					Get_Data			(void)	const	{	return( m_z );		}
	double						operator ()			(int x)	const	{	return( m_z[x] );	}
	double &					operator []			(int x)			{	return( m_z[x] );	}

	CSG_String					asString			(void);

	bool						is_Equal			(const CSG_Vector &Vector)	const;

	bool						Assign				(double Scalar);
	bool						Assign				(const CSG_Vector &Vector);
	bool						Add					(double Scalar);
	bool						Add					(const CSG_Vector &Vector);
	bool						Subtract			(const CSG_Vector &Vector);
	bool						Multiply			(double Scalar);
	bool						Multiply			(const CSG_Vector &Vector);
	double						Multiply_Scalar		(const CSG_Vector &Vector)	const;
	bool						Multiply			(const class CSG_Matrix &Matrix);

	bool						operator ==			(const CSG_Vector &Vector)	const;
	CSG_Vector &				operator =			(double Scalar);
	CSG_Vector &				operator =			(const CSG_Vector &Vector);
	CSG_Vector &				operator +=			(double Scalar);
	CSG_Vector &				operator +=			(const CSG_Vector &Vector);
	CSG_Vector &				operator -=			(double Scalar);
	CSG_Vector &				operator -=			(const CSG_Vector &Vector);
	CSG_Vector &				operator *=			(double Scalar);
	CSG_Vector &				operator *=			(const CSG_Vector &Vector);
	CSG_Vector &				operator *=			(const class CSG_Matrix &Matrix);
	CSG_Vector					operator +			(double Scalar)				const;
	CSG_Vector					operator +			(const CSG_Vector &Vector)	const;
	CSG_Vector					operator -			(double Scalar)				const;
	CSG_Vector					operator -			(const CSG_Vector &Vector)	const;
	CSG_Vector					operator *			(double Scalar)				const;
	double						operator *			(const CSG_Vector &Vector)	const;

	bool						Set_Zero			(void);
	bool						Set_Unity			(void);

	double						Get_Length			(void)						const;
	double						Get_Angle			(const CSG_Vector &Vector)	const;
	CSG_Vector					Get_Unity			(void)						const;


private:

	int							m_n;

	double						*m_z;


	void						_On_Construction	(void);

};

//---------------------------------------------------------
SAGA_API_DLL_EXPORT CSG_Vector	operator *			(double Scalar, const CSG_Vector &Vector);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Matrix
{
public:
	CSG_Matrix(void);
	virtual ~CSG_Matrix(void);

								CSG_Matrix			(const CSG_Matrix &Matrix);
	bool						Create				(const CSG_Matrix &Matrix);

								CSG_Matrix			(int nx, int ny, double *Data = NULL);
	bool						Create				(int nx, int ny, double *Data = NULL);

	bool						Destroy				(void);

	int							Get_NX				(void)			const	{	return( m_nx );			}
	int							Get_NY				(void)			const	{	return( m_ny );			}
	double **					Get_Data			(void)			const	{	return( m_z );			}
	double						operator ()			(int y, int x)	const	{	return( m_z[y][x] );	}
	double *					operator []			(int y)			const	{	return( m_z[y] );		}

	CSG_String					asString			(void);

	bool						is_Square			(void)	const	{	return( m_nx > 0 && m_nx == m_ny );	}
	bool						is_Equal			(const CSG_Matrix &Matrix)	const;

	bool						Assign				(double Scalar);
	bool						Assign				(const CSG_Matrix &Matrix);
	bool						Add					(double Scalar);
	bool						Add					(const CSG_Matrix &Matrix);
	bool						Subtract			(const CSG_Matrix &Matrix);
	bool						Multiply			(double Scalar);
	CSG_Vector					Multiply			(const CSG_Vector &Vector)	const;
	CSG_Matrix					Multiply			(const CSG_Matrix &Matrix)	const;

	bool						operator ==			(const CSG_Matrix &Matrix)	const;
	CSG_Matrix &				operator =			(double Scalar);
	CSG_Matrix &				operator =			(const CSG_Matrix &Matrix);
	CSG_Matrix &				operator +=			(double Scalar);
	CSG_Matrix &				operator +=			(const CSG_Matrix &Matrix);
	CSG_Matrix &				operator -=			(double Scalar);
	CSG_Matrix &				operator -=			(const CSG_Matrix &Matrix);
	CSG_Matrix &				operator *=			(double Scalar);
	CSG_Matrix &				operator *=			(const CSG_Matrix &Matrix);
	CSG_Matrix					operator +			(double Scalar)				const;
	CSG_Matrix					operator +			(const CSG_Matrix &Matrix)	const;
	CSG_Matrix					operator -			(double Scalar)				const;
	CSG_Matrix					operator -			(const CSG_Matrix &Matrix)	const;
	CSG_Matrix					operator *			(double Scalar)				const;
	CSG_Vector					operator *			(const CSG_Vector &Vector)	const;
	CSG_Matrix					operator *			(const CSG_Matrix &Matrix)	const;

	bool						Set_Zero			(void);
	bool						Set_Identity		(void);
	bool						Set_Transpose		(void);
	bool						Set_Inverse			(bool bSilent = true, int nSubSquare = 0);

	double						Get_Determinant		(void)						const;
	CSG_Matrix					Get_Transpose		(void)						const;
	CSG_Matrix					Get_Inverse			(bool bSilent = true, int nSubSquare = 0)	const;


private:

	int							m_nx, m_ny;

	double						**m_z;


	void						_On_Construction	(void);

};

//---------------------------------------------------------
SAGA_API_DLL_EXPORT CSG_Matrix	operator *			(double Scalar, const CSG_Matrix &Matrix);

//---------------------------------------------------------
SAGA_API_DLL_EXPORT bool		SG_Matrix_Solve		(CSG_Matrix &Matrix, CSG_Vector &Vector, bool bSilent = true);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Grid_Radius
{
public:
	CSG_Grid_Radius(int maxRadius = 0);
	~CSG_Grid_Radius(void);

	bool						Create				(int maxRadius);
	void						Destroy				(void);

	int							Get_Maximum			(void)			{	return( m_maxRadius );	}

	int							Get_nPoints			(void)			{	return( m_nPoints );	}
	int							Get_nPoints			(int iRadius)	{	return( iRadius >= 0 && iRadius < m_maxRadius ? m_nPoints_R[iRadius] : 0 );	}

	double						Get_Point			(int iPoint, int &x, int &y)
	{
		if( iPoint >= 0 && iPoint < m_nPoints )
		{
			x	= m_Points[iPoint].x;
			y	= m_Points[iPoint].y;

			return( m_Points[iPoint].d );				// Distance...
		}

		return( -1.0 );
	}

	double						Get_Point			(int iPoint, int xOffset, int yOffset, int &x, int &y)
	{
		double	d;

		if( (d = Get_Point(iPoint, x, y)) >= 0.0 )
		{
			x	+= xOffset;
			y	+= yOffset;
		}

		return( d );
	}

	double						Get_Point			(int iRadius, int iPoint, int &x, int &y)
	{
		if( iRadius >= 0 && iRadius <= m_maxRadius && iPoint >= 0 && iPoint < m_nPoints_R[iRadius] )
		{
			x	= m_Points_R[iRadius][iPoint].x;
			y	= m_Points_R[iRadius][iPoint].y;

			return( m_Points_R[iRadius][iPoint].d );	// Distance...
		}

		return( -1.0 );
	}

	double						Get_Point			(int iRadius, int iPoint, int xOffset, int yOffset, int &x, int &y)
	{
		double	d;

		if( (d = Get_Point(iRadius, iPoint, x, y)) >= 0.0 )
		{
			x	+= xOffset;
			y	+= yOffset;
		}

		return( d );
	}


private:

	int							m_maxRadius, m_nPoints, *m_nPoints_R;

	typedef struct
	{
		int						x, y;

		double					d;
	}
	TSG_Grid_Radius;

	TSG_Grid_Radius				*m_Points, **m_Points_R;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Spline
{
public:
	CSG_Spline(void);
	virtual ~CSG_Spline(void);

	void						Destroy				(void);

	bool						Create				(double *xValues, double *yValues, int nValues, double yA = 1.0e30, double yB = 1.0e30);
	bool						Create				(double yA = 1.0e30, double yB = 1.0e30);

	void						Add					(double x, double y);

	double						Get_xMin			(void)	{	return( m_nValues > 0 ? m_Values[0            ].x : 0.0 );	}
	double						Get_xMax			(void)	{	return( m_nValues > 0 ? m_Values[m_nValues - 1].x : 0.0 );	}

	bool						Get_Value			(double x, double &y);
	double						Get_Value			(double x);


protected:

	bool						m_bCreated;

	int							m_nValues, m_nBuffer;

	TSG_Point_3D				*m_Values;


	bool						_Create				(double yA, double yB);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum ESG_Regression_Type
{
	REGRESSION_Linear	= 0,	// Y = a + b * X
	REGRESSION_Rez_X,			// Y = a + b / X
	REGRESSION_Rez_Y,			// Y = a / (b - X)
	REGRESSION_Pow,				// Y = a * X^b
	REGRESSION_Exp,				// Y = a * e^(b * X)
	REGRESSION_Log				// Y = a + b * ln(X)
}
TSG_Regression_Type;

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Regression
{
public:
	CSG_Regression(void);
	virtual ~CSG_Regression(void);

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

	TSG_Regression_Type			Get_Type			(void)			{	return( m_Type );	}

	bool						Calculate			(TSG_Regression_Type Type = REGRESSION_Linear);
	bool						Calculate			(int nValues, double *x, double *y, TSG_Regression_Type Type = REGRESSION_Linear);


protected:

	int							m_nValues, m_nBuffer;

	double						m_RConst, m_RCoeff, m_R,
								m_xMin, m_xMax, m_xMean, m_xVar, *m_x,
								m_yMin, m_yMax, m_yMean, m_yVar, *m_y;

	TSG_Regression_Type			m_Type;


	bool						_Get_MinMeanMax		(double &xMin, double &xMean, double &xMax, double &yMin, double &yMean, double &yMax);

	double						_Y_Transform		(double x);
	double						_X_Transform		(double y);

	bool						_Linear				(void);

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Regression_Multiple
{
public:
	CSG_Regression_Multiple(void);
	virtual ~CSG_Regression_Multiple(void);

	void						Destroy				(void);

	bool						Calculate			(const class CSG_Table &Values);

	class CSG_Table *			Get_Result			(void)	{	return( m_pResult );	}

	int							Get_Ordered			(int iOrder);
	int							Get_Order			(int iVariable);
	double						Get_R2				(int iVariable);
	double						Get_R2_Change		(int iVariable);
	double						Get_RConst			(void);
	double						Get_RCoeff			(int iVariable);


protected:

	class CSG_Table				*m_pResult;


	bool						_Get_Regression		(const class CSG_Table &Values);

	bool						_Get_Correlation	(const class CSG_Table &Values);
	bool						_Get_Correlation	(int nValues, int nVariables, double **X, double *Y, int &iMax, double &rMax);

	bool						_Eliminate			(int nValues, double *X, double *Y);

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
class SAGA_API_DLL_EXPORT CSG_Formula
{
public:
	CSG_Formula(void);

	static CSG_String			Get_Help_Operators	(void);
	static CSG_String			Get_Help_Usage		(void);

	bool						Get_Error			(int *Pos = NULL, const char **Msg = NULL);

	double						Val(double *Vals, int n);
	double						Val(void);
	double						Val(char *Args, ...);
	double						Val(double x);

	void						Set_Variable(char Variable, double Value);

	bool						Set_Formula(const char *Formula);
	CSG_String					Get_Formula(void)	{	return( m_Formula );	}

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
	CSG_String					m_Formula;

	const char					*i_error; 
	bool						used_vars[256];	//['z' - 'a' + 1];
	char						*errmes;
	int							i_pctable;	// number of items in a table of constants - used only by the translating functions
	int							Error_Pos;
	int							Length;
	double						*i_ctable;	// current table of constants - used only by the translating functions
	double						param[256];	//['z' - 'a' + 1];
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
class SAGA_API_DLL_EXPORT CSG_Trend
{
public:
	CSG_Trend(void);
	virtual ~CSG_Trend(void);

	bool						Set_Formula			(const char *Formula = NULL);
	CSG_String					Get_Formula			(void);

	int							Get_Parameter_Count	(void) const	{	return( m_Params.m_Count );		}
	double *					Get_Parameters		(void) const	{	return( m_Params.m_A );			}

	void						Clr_Data			(void);
	void						Set_Data			(double *xData, double *yData, int nData, bool bAdd = false);
	void						Set_Data			(const CSG_Points &Data, bool bAdd = false);
	void						Add_Data			(double x, double y);
	int							Get_Data_Count		(void) const	{	return( m_Data.Get_Count() );	}
	double						Get_Data_X			(int Index)		{	return( m_Data.Get_X(Index) );	}
	double						Get_Data_Y			(int Index)		{	return( m_Data.Get_Y(Index) );	}

	bool						Set_Max_Iterations	(int Iterations);
	int							Get_Max_Iterations	(void)			{	return( m_Iter_Max);	}
	bool						Set_Max_Lambda		(double Lambda);
	double						Get_Max_Lambda		(void)			{	return( m_Lambda_Max);	}

	bool						Get_Trend			(double *xData, double *yData, int nData, const char *Formula = NULL);
	bool						Get_Trend			(const CSG_Points &Data, const char *Formula = NULL);
	bool						Get_Trend			(void);

	bool						is_Okay				(void)			{	return( m_bOkay );		}

	CSG_String					Get_Error			(void);

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

		bool					Create				(const char *Variables, int nVariables);
		bool					Destroy				(void);

		int						m_Count;

		char					*m_Variables;

		double					*m_A, *m_Atry, *m_dA, *m_dA2, *m_Beta, **m_Alpha, **m_Covar;

	};


	//-----------------------------------------------------
	bool						m_bOkay;

	int							m_Iter_Max;

	double						m_ChiSqr, m_ChiSqr_o, m_Lambda, m_Lambda_Max;

	CSG_Points					m_Data;

	CFncParams					m_Params;

	CSG_Formula					m_Formula;


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

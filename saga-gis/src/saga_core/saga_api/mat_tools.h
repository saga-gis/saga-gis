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

#define M_SET_SIGN(x, sign)			((sign) < 0 ? (x < 0 ? x : -x) : (x > 0 ? x : -x))


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

	bool						Add_Cols			(int nCols);
	bool						Add_Rows			(int nRows);
	bool						Add_Col				(          double *Data = NULL);
	bool						Add_Col				(          const CSG_Vector &Data);
	bool						Add_Row				(          double *Data = NULL);
	bool						Add_Row				(          const CSG_Vector &Data);
	bool						Ins_Col				(int iCol, double *Data = NULL);
	bool						Ins_Col				(int iCol, const CSG_Vector &Data);
	bool						Ins_Row				(int iRow, double *Data = NULL);
	bool						Ins_Row				(int iRow, const CSG_Vector &Data);
	bool						Set_Col				(int iCol, double *Data);
	bool						Set_Col				(int iCol, const CSG_Vector &Data);
	bool						Set_Row				(int iRow, double *Data);
	bool						Set_Row				(int iRow, const CSG_Vector &Data);
	bool						Del_Col				(int iCol);
	bool						Del_Row				(int iRow);
	CSG_Vector					Get_Col				(int iCol)		const;
	CSG_Vector					Get_Row				(int iRow)		const;

	int							Get_NX				(void)			const	{	return( m_nx );			}
	int							Get_NCols			(void)			const	{	return( m_nx );			}
	int							Get_NY				(void)			const	{	return( m_ny );			}
	int							Get_NRows			(void)			const	{	return( m_ny );			}

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
SAGA_API_DLL_EXPORT bool		SG_Matrix_Solve				(CSG_Matrix &Matrix, CSG_Vector &Vector, bool bSilent = true);
SAGA_API_DLL_EXPORT bool		SG_Matrix_Eigen_Reduction	(const CSG_Matrix &Matrix, CSG_Matrix &Eigen_Vectors, CSG_Vector &Eigen_Values, bool bSilent = true);


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
class SAGA_API_DLL_EXPORT CSG_Simple_Statistics
{
public:
	CSG_Simple_Statistics(void);
	CSG_Simple_Statistics(bool bHoldValues);
	CSG_Simple_Statistics(const CSG_Simple_Statistics &Statistics);

	bool						Create				(bool bHoldValues = false);
	bool						Create				(const CSG_Simple_Statistics &Statistics);

	void						Invalidate			(void);

	bool						is_Evaluated		(void)		{	return( m_bEvaluated );	}

	int							Get_Count			(void)		{	return( m_nValues );	}
	double						Get_Weights			(void)		{	return( m_Weights );	}

	double						Get_Minimum			(void)		{	if( !m_bEvaluated )	_Evaluate(); return( m_Minimum	);	}
	double						Get_Maximum			(void)		{	if( !m_bEvaluated )	_Evaluate(); return( m_Maximum	);	}
	double						Get_Range			(void)		{	if( !m_bEvaluated )	_Evaluate(); return( m_Range	);	}
	double						Get_Sum				(void)		{	if( !m_bEvaluated )	_Evaluate(); return( m_Sum		);	}
	double						Get_Mean			(void)		{	if( !m_bEvaluated )	_Evaluate(); return( m_Mean		);	}
	double						Get_Variance		(void)		{	if( !m_bEvaluated )	_Evaluate(); return( m_Variance	);	}
	double						Get_StdDev			(void)		{	if( !m_bEvaluated )	_Evaluate(); return( m_StdDev	);	}

	void						Add_Value			(double Value, double Weight = 1.0);

	double						Get_Value			(int i)		{	return( i >= 0 && i < (int)m_Values.Get_Size() ? ((double *)m_Values.Get_Array())[i] : Get_Mean() );	}

	CSG_Simple_Statistics &		operator  =			(const CSG_Simple_Statistics &Statistics)	{	Create(Statistics);	return( *this );	}
	CSG_Simple_Statistics &		operator +=			(double Value)								{	Add_Value(Value);	return( *this );	}


protected:

	bool						m_bEvaluated;

	int							m_nValues;

	double						m_Weights, m_Sum, m_Sum2, m_Minimum, m_Maximum, m_Range, m_Mean, m_Variance, m_StdDev;

	CSG_Array					m_Values;


	void						_Evaluate			(void);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Class_Statistics
{
private:

	typedef struct SClass
	{
		int		Count;

		double	Value;
	}
	TClass;


public:
	 CSG_Class_Statistics(void);
	~CSG_Class_Statistics(void);

	void			Create					(void);
	void			Destroy					(void);

	void			Reset					(void)	{	m_Array.Set_Array(0, (void **)&m_Classes, false);	}

	int				Get_Count				(void)	{	return( (int)m_Array.Get_Size() );	}

	int				Get_Class_Count			(int i)	{	return( i >= 0 && i < Get_Count() ? m_Classes[i].Count : 0 );	}
	double			Get_Class_Value			(int i)	{	return( i >= 0 && i < Get_Count() ? m_Classes[i].Value : 0 );	}

	bool			Get_Class				(int i, double &Value, int &Count)
	{
		if( i >= 0 && i < Get_Count() )
		{
			Count	= m_Classes[i].Count;
			Value	= m_Classes[i].Value;

			return( true );
		}

		return( false );
	}

	bool			Get_Class				(int i, int &Value, int &Count)
	{
		if( i >= 0 && i < Get_Count() )
		{
			Count	=      m_Classes[i].Count;
			Value	= (int)m_Classes[i].Value;

			return( true );
		}

		return( false );
	}

	void			Add_Value				(double Value);

	int				Get_Majority			(void);
	bool			Get_Majority			(double &Value);
	bool			Get_Majority			(double &Value, int &Count);

	int				Get_Minority			(void);
	bool			Get_Minority			(double &Value);
	bool			Get_Minority			(double &Value, int &Count);


private:

	CSG_Array		m_Array;

	TClass			*m_Classes;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum ESG_Cluster_Analysis_Method
{
	SG_CLUSTERANALYSIS_Minimum_Distance	= 0,
	SG_CLUSTERANALYSIS_Hill_Climbing,
	SG_CLUSTERANALYSIS_Combined
};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Cluster_Analysis
{
public:
	CSG_Cluster_Analysis(void);
	~CSG_Cluster_Analysis(void);
	
	bool					Create				(int nFeatures);
	bool					Destroy				(void);

	bool					Add_Element			(void);
	bool					Set_Feature			(int iElement, int iFeature, double Value);

	int						Get_Cluster			(int iElement)	const	{	return( iElement >= 0 && iElement < Get_nElements() ? m_Cluster[iElement] : -1 );	}

	bool					Execute				(int Method, int nClusters);

	int						Get_nElements		(void)	const	{	return( (int)m_Features.Get_Size() );	}
	int						Get_nFeatures		(void)	const	{	return( m_nFeatures );	}
	int						Get_nClusters		(void)	const	{	return( m_nClusters );	}

	int						Get_Iteration		(void)	const	{	return( m_Iteration );	}

	double					Get_SP				(void)	const	{	return( m_SP );			}

	int						Get_nMembers		(int iCluster)					const	{	return( m_nMembers[iCluster] );	}
	double					Get_Variance		(int iCluster)					const	{	return( m_Variance[iCluster] );	}
	double					Get_Centroid		(int iCluster, int iFeature)	const	{	return( m_Centroid[iCluster][iFeature] );	}


private:

	int						*m_Cluster, m_Iteration, m_nFeatures, m_nClusters, *m_nMembers;

	double					*m_Variance, **m_Centroid, m_SP;

	CSG_Array				m_Features;


	bool					Minimum_Distance	(bool bInitialize);
	bool					Hill_Climbing		(bool bInitialize);

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

	TSG_Point_Z					*m_Values;


	bool						_Create				(double yA, double yB);

};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Thin_Plate_Spline
{
public:
	CSG_Thin_Plate_Spline(void);
	virtual ~CSG_Thin_Plate_Spline(void);

	bool					Destroy				(void);

	bool					Set_Point_Count		(int Count)	{	return( m_Points.Set_Count(Count) );	}
	int						Get_Point_Count		(void)		{	return( m_Points.Get_Count() );			}

	CSG_Points_Z &			Get_Points			(void)		{	return( m_Points );	}

	bool					Add_Point			(double x, double y, double z)	{	return( m_Points.Add(  x,   y, z) );	}
	bool					Add_Point			(const TSG_Point &p, double z)	{	return( m_Points.Add(p.x, p.y, z) );	}

	bool					Set_Point			(int Index, double x, double y, double z)
	{
		if( Index >= 0 && Index < m_Points.Get_Count() )
		{
			m_Points[Index].x	= x;
			m_Points[Index].y	= y;
			m_Points[Index].z	= z;

			return( true );
		}

		return( false );
	}

	bool					Set_Point			(int Index, const TSG_Point &p, double z)	{	return( Set_Point(Index, p.x, p.y, z) );	}

	bool					Create				(double Regularization = 0.0, bool bSilent = true);

	bool					is_Okay				(void)	{	return( m_V.Get_N() > 0 );		}

	double					Get_Value			(double x, double y);


private:

	CSG_Points_Z			m_Points;

	CSG_Vector				m_V;


	double					_Get_hDistance		(TSG_Point_Z A, TSG_Point_Z B);
	double					_Get_Base_Funtion	(double x);
	double					_Get_Base_Funtion	(TSG_Point_Z A, double x, double y);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum ESG_Test_Distribution_Type
{
	TESTDIST_TYPE_Left	= 0,
	TESTDIST_TYPE_Right,
	TESTDIST_TYPE_Middle,
	TESTDIST_TYPE_TwoTail
}
TSG_Test_Distribution_Type;

//---------------------------------------------------------
class CSG_Test_Distribution
{
public:

	static double			Get_F_Tail_from_R2	(double R2, int nPredictors, int nSamples, TSG_Test_Distribution_Type Type = TESTDIST_TYPE_Right);

	static double			Get_F_Tail			(double F    , int dfn, int dfd, TSG_Test_Distribution_Type Type = TESTDIST_TYPE_Right);
	static double			Get_F_Inverse		(double alpha, int dfn, int dfd, TSG_Test_Distribution_Type Type = TESTDIST_TYPE_Right);

	static double			Get_T_Tail			(double T    , int df, TSG_Test_Distribution_Type Type = TESTDIST_TYPE_Right);
	static double			Get_T_Inverse		(double alpha, int df, TSG_Test_Distribution_Type Type = TESTDIST_TYPE_Right);

	static double			Get_Norm_P			(double Z);
	static double			Get_Norm_Z			(double P);


private:

	static double			Get_Gamma			(double F, double dfn, double dfd);
	static double			Get_Log_Gamma		(double a);

	static double			Get_T_P				(double T, int df);
	static double			Get_T_Z				(double T, int df);
	static double			Get_T_Inv			(double p, int df);

	static double			_Change_Tail_Type	(double p, TSG_Test_Distribution_Type from, TSG_Test_Distribution_Type to, bool bNegative);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// Values: (matrix) array with number of variables = number of rows (x), number of samples = number of columns (y)
SAGA_API_DLL_EXPORT CSG_Matrix	SG_Get_Correlation_Matrix		(const CSG_Matrix &Values, bool bCovariances = false);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum ESG_Regression_Correction
{
	REGRESSION_CORR_None	= 0,
	REGRESSION_CORR_Smith,
	REGRESSION_CORR_Wherry_1,
	REGRESSION_CORR_Wherry_2,
	REGRESSION_CORR_Olkin_Pratt,
	REGRESSION_CORR_Pratt,
	REGRESSION_CORR_Claudy_3
}
TSG_Regression_Correction;

//---------------------------------------------------------
SAGA_API_DLL_EXPORT double		SG_Regression_Get_Adjusted_R2	(double R2, int nSamples, int nPredictors, TSG_Regression_Correction Correction = REGRESSION_CORR_Wherry_1);


///////////////////////////////////////////////////////////
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

	int							Get_Count			(void)			const	{	return( m_nValues );	}

	double						Get_xValue			(int iValue)	const	{	return( iValue >= 0 && iValue < m_nValues ? m_x[iValue] : 0.0 );	}
	double						Get_yValue			(int iValue)	const	{	return( iValue >= 0 && iValue < m_nValues ? m_y[iValue] : 0.0 );	}
	bool						Get_Values			(int iValue, double &x, double &y)	const
	{
		if( iValue >= 0 && iValue < m_nValues )
		{
			x	= m_x[iValue];
			y	= m_y[iValue];

			return( true );
		}

		return( false );
	}

	double						Get_xMin			(void)		const	{	return( m_xMin );	}
	double						Get_xMax			(void)		const	{	return( m_xMax );	}
	double						Get_xMean			(void)		const	{	return( m_xMean );	}
	double						Get_xVariance		(void)		const	{	return( m_xVar );	}
	double						Get_x				(double y)	const;	// returns INF on error, this can be checked using the _finite() function (libc, include <float.h>)...

	double						Get_yMin			(void)		const	{	return( m_yMin );	}
	double						Get_yMax			(void)		const	{	return( m_yMax );	}
	double						Get_yMean			(void)		const	{	return( m_yMean );	}
	double						Get_yVariance		(void)		const	{	return( m_yVar );	}
	double						Get_y				(double x)	const;	// returns INF on error, this can be checked using the _finite() function (libc, include <float.h>)...

	double						Get_Constant		(void)		const	{	return( m_RConst );	}
	double						Get_Coefficient		(void)		const	{	return( m_RCoeff );	}
	double						Get_R				(void)		const	{	return( m_R );		}
	double						Get_R2				(void)		const	{	return( m_R*m_R );	}

	const SG_Char *				asString			(void);

	TSG_Regression_Type			Get_Type			(void)		const	{	return( m_Type );	}

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


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum ESG_Multiple_Regression_Info_Vars
{
	MLR_VAR_ID	= 0,
	MLR_VAR_NAME,
	MLR_VAR_RCOEFF,
	MLR_VAR_R,
	MLR_VAR_R2,
	MLR_VAR_R2_ADJ,
	MLR_VAR_SE,
	MLR_VAR_T,
	MLR_VAR_SIG,
	MLR_VAR_P
};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Regression_Multiple
{
public:
	CSG_Regression_Multiple(void);
	virtual ~CSG_Regression_Multiple(void);

	void						Destroy				(void);

	bool						Calculate			(const CSG_Matrix &Samples                           , CSG_Strings *pNames = NULL);
	bool						Calculate_Forward	(const CSG_Matrix &Samples, double P_in              , CSG_Strings *pNames = NULL);
	bool						Calculate_Backward	(const CSG_Matrix &Samples, double P_out             , CSG_Strings *pNames = NULL);
	bool						Calculate_Stepwise	(const CSG_Matrix &Samples, double P_in, double P_out, CSG_Strings *pNames = NULL);

	CSG_String					Get_Info			(void)			const;

	class CSG_Table *			Get_Regression		(void)			const	{	return( m_pRegression );	}
	class CSG_Table *			Get_Model			(void)			const	{	return( m_pModel );			}
	class CSG_Table *			Get_Steps			(void)			const	{	return( m_pSteps );			}

	double						Get_R2				(void)			const;
	double						Get_R2_Adj			(void)			const;
	double						Get_StdError		(void)			const;
	double						Get_F				(void)			const;
	double						Get_P				(void)			const;
	int							Get_DegFreedom		(void)			const;
	int							Get_nSamples		(void)			const;
	int							Get_nPredictors		(void)			const;
	int							Get_Predictor		(int i)			const	{	return( i >= 0 && i < Get_nPredictors() ? m_Predictor[i] : -1 );	}

	double						Get_RConst			(void)			const;
	const SG_Char *				Get_Name			(int iVariable)	const;
	double						Get_ID				(int iVariable)	const	{	return( Get_Parameter(iVariable, MLR_VAR_ID    ) );	}
	double						Get_RCoeff			(int iVariable)	const	{	return( Get_Parameter(iVariable, MLR_VAR_RCOEFF) );	}
	double						Get_R2_Partial		(int iVariable)	const	{	return( Get_Parameter(iVariable, MLR_VAR_R2    ) );	}
	double						Get_R2_Partial_Adj	(int iVariable)	const	{	return( Get_Parameter(iVariable, MLR_VAR_R2_ADJ) );	}
	double						Get_StdError		(int iVariable)	const	{	return( Get_Parameter(iVariable, MLR_VAR_SE    ) );	}
	double						Get_T				(int iVariable)	const	{	return( Get_Parameter(iVariable, MLR_VAR_T     ) );	}
	double						Get_P				(int iVariable)	const	{	return( Get_Parameter(iVariable, MLR_VAR_SIG   ) );	}

	double						Get_Parameter		(int iVariable, int Parameter)	const;


protected:

	int							*m_bIncluded, *m_Predictor, m_nPredictors;

	CSG_Strings					m_Names;

	class CSG_Table				*m_pRegression, *m_pModel, *m_pSteps;


	bool						_Initialize			(const CSG_Matrix &Samples, CSG_Strings *pNames, bool bInclude);

	double						_Get_F				(int nPredictors, int nSamples, double r2_full, double r2_reduced);
	double						_Get_P				(int nPredictors, int nSamples, double r2_full, double r2_reduced);

	bool						_Get_Regression		(const class CSG_Matrix &Samples);

	bool						_Set_Step_Info		(const CSG_Matrix &X);
	bool						_Set_Step_Info		(const CSG_Matrix &X, double R2_prev, int iVariable, bool bIn);
	int							_Get_Step_In		(CSG_Matrix &X, double P_in , double &R2, const CSG_Matrix &Samples);
	int							_Get_Step_Out		(CSG_Matrix &X, double P_out, double &R2);

	bool						__Get_Forward		(const class CSG_Matrix &Samples, double p_in);
	bool						__Get_Forward		(int nSamples, int nPredictors, double **X, double *Y, int &iMax, double &rMax);
	bool						__Eliminate			(int nSamples, double *X, double *Y);

};


///////////////////////////////////////////////////////////
//														 //
//				Formula Parser (A. Ringeler)			 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef double (*TSG_PFNC_Formula_0)(void);
typedef double (*TSG_PFNC_Formula_1)(double);
typedef double (*TSG_PFNC_Formula_2)(double, double);
typedef double (*TSG_PFNC_Formula_3)(double, double, double);

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Formula
{
public:
	CSG_Formula(void);
	virtual ~CSG_Formula(void);

	static CSG_String			Get_Help_Operators	(void);
	static CSG_String			Get_Help_Usage		(void);

	bool						Get_Error			(int *pPosition = NULL, CSG_String *pMessage = NULL);
	bool						Get_Error			(CSG_String &Message);

	int							Add_Function		(SG_Char *Name, TSG_PFNC_Formula_1 f, int N_of_Pars, int Varying);
	int							Del_Function		(SG_Char *Name);

	bool						Set_Formula			(const SG_Char *Formula);
	CSG_String					Get_Formula			(void)		{	return( m_sFormula );	}

	void						Set_Variable		(SG_Char Variable, double Value);

	double						Get_Value			(void);
	double						Get_Value			(double x);
	double						Get_Value			(double *Values, int nValues);
	double						Get_Value			(SG_Char *Arguments, ...);

	const SG_Char *				Get_Used_Variables	(void);


	//-----------------------------------------------------
	typedef struct 
	{
		SG_Char					*name;
		TSG_PFNC_Formula_1		f;			
		int						n_pars;		
		int						varying;	// Does the result of the function vary even when the parameters stay the same? varying = 1 for e.g. random - number generators.
	}
	TSG_Formula_Item;


private:

	//-----------------------------------------------------
	typedef struct 
	{
		SG_Char					*code;
		double					*ctable;
	}
	TMAT_Formula;


	//-----------------------------------------------------
	bool						m_bError, m_Vars_Used[256];

	int							m_Error_Position, m_Length;

	TMAT_Formula				m_Formula;

	CSG_String					m_sFormula, m_sError;

	const SG_Char				*i_error;
	int							i_pctable;			// number of items in a table of constants - used only by the translating functions

	double						m_Parameters[256],
								*i_ctable;			// current table of constants - used only by the translating functions


	void						_Set_Error			(const SG_Char *Error = NULL);

	double						_Get_Value			(TMAT_Formula Function);

	int							_is_Operand			(SG_Char c);
	int							_is_Operand_Code	(SG_Char c);
	int							_is_Number			(SG_Char c);

	int							_Get_Function		(int i, SG_Char *name, int *n_pars, int *varying);
	int							_Get_Function		(SG_Char *name);

	TMAT_Formula				_Translate			(const SG_Char *source, const SG_Char *args, int *length, int *error);

	int							max_size(const SG_Char *source);
	SG_Char *					my_strtok(SG_Char *s);
	SG_Char *					i_trans(SG_Char *function, SG_Char *begin, SG_Char *end);
	SG_Char *					comp_time(SG_Char *function, SG_Char *fend, int npars);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum ESG_Trend_String
{
	SG_TREND_STRING_Formula	= 0,
	SG_TREND_STRING_Function,
	SG_TREND_STRING_Formula_Parameters,
	SG_TREND_STRING_Complete
};

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Trend
{
public:
	CSG_Trend(void);
	virtual ~CSG_Trend(void);

	bool						Set_Formula			(const SG_Char *Formula = NULL);
	CSG_String					Get_Formula			(int Type = SG_TREND_STRING_Complete);

	int							Get_Parameter_Count	(void) const	{	return( m_Params.m_Count );		}
	double *					Get_Parameters		(void) const	{	return( m_Params.m_A );			}

	void						Clr_Data			(void);
	void						Set_Data			(double *xData, double *yData, int nData, bool bAdd = false);
	void						Set_Data			(const CSG_Points &Data, bool bAdd = false);
	void						Add_Data			(double x, double y);
	int							Get_Data_Count		(void) const	{	return( m_Data.Get_Count() );	}
	double						Get_Data_X			(int Index)		{	return( m_Data.Get_X(Index) );	}
	double						Get_Data_Y			(int Index)		{	return( m_Data.Get_Y(Index) );	}
	double						Get_Data_XMin		(void)			{	return( m_xMin );	}
	double						Get_Data_XMax		(void)			{	return( m_xMax );	}
	double						Get_Data_YMin		(void)			{	return( m_yMin );	}
	double						Get_Data_YMax		(void)			{	return( m_yMax );	}

	bool						Set_Max_Iterations	(int Iterations);
	int							Get_Max_Iterations	(void)			{	return( m_Iter_Max);	}
	bool						Set_Max_Lambda		(double Lambda);
	double						Get_Max_Lambda		(void)			{	return( m_Lambda_Max);	}

	bool						Get_Trend			(double *xData, double *yData, int nData, const SG_Char *Formula = NULL);
	bool						Get_Trend			(const CSG_Points &Data, const SG_Char *Formula = NULL);
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

		bool					Create				(const SG_Char *Variables, int nVariables);
		bool					Destroy				(void);

		int						m_Count;

		SG_Char					*m_Variables;

		double					*m_A, *m_Atry, *m_dA, *m_dA2, *m_Beta, **m_Alpha, **m_Covar;

	};


	//-----------------------------------------------------
	bool						m_bOkay;

	int							m_Iter_Max;

	double						m_ChiSqr, m_ChiSqr_o, m_Lambda, m_Lambda_Max, m_xMin, m_xMax, m_yMin, m_yMax;

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

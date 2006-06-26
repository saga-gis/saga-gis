
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
//                        grid.h                         //
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
#ifndef HEADER_INCLUDED__SAGA_API__grid_H
#define HEADER_INCLUDED__SAGA_API__grid_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "dataobject.h"


///////////////////////////////////////////////////////////
//														 //
//						Data Types						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum
{
	GRID_TYPE_Undefined		= 0,
	GRID_TYPE_Byte,
	GRID_TYPE_Char,
	GRID_TYPE_Word,
	GRID_TYPE_Short,
	GRID_TYPE_DWord,
	GRID_TYPE_Int,
	GRID_TYPE_Long,
	GRID_TYPE_Float,
	GRID_TYPE_Double,
	GRID_TYPE_Bit,
	GRID_TYPE_Count
}
TGrid_Type;

//---------------------------------------------------------
const char GRID_TYPE_NAMES[GRID_TYPE_Count][32]	=
{
	"ASCII",
	"BYTE_UNSIGNED",
	"BYTE",
	"SHORTINT_UNSIGNED",
	"SHORTINT",
	"INTEGER_UNSIGNED",
	"INTEGER",
	"LONGINT",
	"FLOAT",
	"DOUBLE",
	"BIT"
};

//---------------------------------------------------------
const char GRID_TYPE_SIZES[GRID_TYPE_Count]	= 
{
	0,
	sizeof(BYTE),
	sizeof(char),
	sizeof(WORD),
	sizeof(short),
	sizeof(DWORD),
	sizeof(int),
	sizeof(long),
	sizeof(float),
	sizeof(double),
	-1
};


///////////////////////////////////////////////////////////
//														 //
//					Memory Handling						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum
{
	GRID_MEMORY_Normal					= 0,
	GRID_MEMORY_Cache,
	GRID_MEMORY_Compression
}
TGrid_Memory_Type;


///////////////////////////////////////////////////////////
//														 //
//						Grid File						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum
{
	GRID_FILE_FORMAT_Undefined			= 0,
	GRID_FILE_FORMAT_Binary,
	GRID_FILE_FORMAT_ASCII
}
TGrid_File_Format;

//---------------------------------------------------------
typedef enum
{
	GRID_FILE_KEY_NAME					= 0,
	GRID_FILE_KEY_DESCRIPTION,
	GRID_FILE_KEY_UNITNAME,
	GRID_FILE_KEY_DATAFILE_NAME,
	GRID_FILE_KEY_DATAFILE_OFFSET,
	GRID_FILE_KEY_DATAFORMAT,
	GRID_FILE_KEY_BYTEORDER_BIG,
	GRID_FILE_KEY_POSITION_XMIN,
	GRID_FILE_KEY_POSITION_YMIN,
	GRID_FILE_KEY_CELLCOUNT_X,
	GRID_FILE_KEY_CELLCOUNT_Y,
	GRID_FILE_KEY_CELLSIZE,
	GRID_FILE_KEY_Z_FACTOR,
	GRID_FILE_KEY_NODATA_VALUE,
	GRID_FILE_KEY_TOPTOBOTTOM,
	GRID_FILE_KEY_Count
}
TGrid_File_Key;

//---------------------------------------------------------
const char	GRID_FILE_KEY_NAMES[GRID_FILE_KEY_Count][32]	=
{
	"NAME",
	"DESCRIPTION",
	"UNIT",
	"DATAFILE_NAME",
	"DATAFILE_OFFSET",
	"DATAFORMAT",
	"BYTEORDER_BIG",
	"POSITION_XMIN",
	"POSITION_YMIN",
	"CELLCOUNT_X",
	"CELLCOUNT_Y",
	"CELLSIZE",
	"Z_FACTOR",
	"NODATA_VALUE",
	"TOPTOBOTTOM"
};

//---------------------------------------------------------
#define GRID_FILE_KEY_TRUE			"TRUE"
#define GRID_FILE_KEY_FALSE			"FALSE"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum
{
	GRID_INTERPOLATION_NearestNeighbour	= 0,
	GRID_INTERPOLATION_Bilinear,
	GRID_INTERPOLATION_InverseDistance,
	GRID_INTERPOLATION_BicubicSpline,
	GRID_INTERPOLATION_BSpline,
	GRID_INTERPOLATION_Undefined
}
TGrid_Interpolation;

//---------------------------------------------------------
typedef enum
{
	GRID_OPERATION_Addition				= 0,
	GRID_OPERATION_Subtraction,
	GRID_OPERATION_Multiplication,
	GRID_OPERATION_Division
}
TGrid_Operation;


///////////////////////////////////////////////////////////
//														 //
//					CGrid_System						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CGrid_System
{
public:
	CGrid_System(void);
	CGrid_System(const CGrid_System &System);
	CGrid_System(double Cellsize, const CGEO_Rect &Extent);
	CGrid_System(double Cellsize, double xMin, double yMin, double xMax, double yMax);
	CGrid_System(double Cellsize, double xMin, double yMin, int NX, int NY);

	~CGrid_System(void);


	//-----------------------------------------------------
	bool						is_Valid			(void);

	const char *				Get_Name			(bool bShort = true);

	double						Get_Cellsize		(void)	const	{	return( m_Cellsize );				}
	double						Get_Cellarea		(void)	const	{	return( m_Cellarea );				}
	const CGEO_Rect &			Get_Extent			(void)	const	{	return( m_Extent );					}

	int							Get_NX				(void)	const	{	return( m_NX );						}
	int							Get_NY				(void)	const	{	return( m_NY );						}
	long						Get_NCells			(void)	const	{	return( m_NCells );					}

	double						Get_XMin			(void)	const	{	return( m_Extent.m_rect.xMin );		}
	double						Get_XMax			(void)	const	{	return( m_Extent.m_rect.xMax );		}
	double						Get_XRange			(void)	const	{	return( m_Extent.Get_XRange() );	}

	double						Get_YMin			(void)	const	{	return( m_Extent.m_rect.yMin );		}
	double						Get_YMax			(void)	const	{	return( m_Extent.m_rect.yMax );		}
	double						Get_YRange			(void)	const	{	return( m_Extent.Get_YRange() );	}


	//-----------------------------------------------------
	bool						operator ==			(const CGrid_System &System) const;
	void						operator =			(const CGrid_System &System);

	bool						Assign				(const CGrid_System &System);
	bool						Assign				(double Cellsize, const CGEO_Rect &Extent);
	bool						Assign				(double Cellsize, double xMin, double yMin, double xMax, double yMax);
	bool						Assign				(double Cellsize, double xMin, double yMin, int NX, int NY);

	bool						is_Equal			(const CGrid_System &System) const;
	bool						is_Equal			(double Cellsize, const TGEO_Rect &Extent) const;


	//-----------------------------------------------------
	double						Fit_xto_Grid_System	(double x)	const	{	return( Get_XMin() + m_Cellsize * (int)(0.5 + (x - Get_XMin()) / m_Cellsize) );	}
	double						Fit_yto_Grid_System	(double y)	const	{	return( Get_YMin() + m_Cellsize * (int)(0.5 + (y - Get_YMin()) / m_Cellsize) );	}

	TGEO_Point					Fit_to_Grid_System	(TGEO_Point ptWorld)	const
	{
		ptWorld.x	= Fit_xto_Grid_System(ptWorld.x);
		ptWorld.y	= Fit_yto_Grid_System(ptWorld.y);

		return( ptWorld );
	}


	//-----------------------------------------------------
	double						Get_xGrid_to_World	(int xGrid)	const	{	return( Get_XMin() + xGrid * m_Cellsize );	}
	double						Get_yGrid_to_World	(int yGrid)	const	{	return( Get_YMin() + yGrid * m_Cellsize );	}

	TGEO_Point					Get_Grid_to_World	(int xGrid, int yGrid)	const
	{
		TGEO_Point	pt;

		pt.x	= Get_xGrid_to_World(xGrid);
		pt.y	= Get_yGrid_to_World(yGrid);

		return( pt );
	}


	//-----------------------------------------------------
	int							Get_xWorld_to_Grid	(double xWorld)	const	{	return( (int)(0.5 + (xWorld - Get_XMin()) / m_Cellsize) );	}
	int							Get_yWorld_to_Grid	(double yWorld)	const	{	return( (int)(0.5 + (yWorld - Get_YMin()) / m_Cellsize) );	}

	bool						Get_World_to_Grid	(int &xGrid, int &yGrid, double xWorld, double yWorld)	const
	{
		return( is_InGrid(xGrid = Get_xWorld_to_Grid(xWorld), yGrid = Get_yWorld_to_Grid(yWorld)) );
	}

	bool						Get_World_to_Grid	(int &xGrid, int &yGrid, TGEO_Point ptWorld)	const
	{
		return( is_InGrid(xGrid = Get_xWorld_to_Grid(ptWorld.x), yGrid = Get_yWorld_to_Grid(ptWorld.y)) );
	}


	//-----------------------------------------------------
	bool						Get_Neighbor_Pos	(int Direction, int x, int y, int &xPos, int &yPos)	const
	{
		return( is_InGrid(xPos = Get_xTo(Direction, x), yPos = Get_yTo(Direction, y)) );
	}

	int							Get_xTo				(int Direction, int x = 0)	const
	{
		static int	ix[8]	= { 0, 1, 1, 1, 0,-1,-1,-1 };

		return( x + ix[Direction % 8] );
	}

	int							Get_yTo				(int Direction, int y = 0)	const
	{
		static int	iy[8]	= { 1, 1, 0,-1,-1,-1, 0, 1 };

		return( y + iy[Direction % 8] );
	}

	int							Get_xFrom			(int Direction, int x = 0)	const	{	return( Get_xTo(Direction + 4, x) );	}
	int							Get_yFrom			(int Direction, int y = 0)	const	{	return( Get_yTo(Direction + 4, y) );	}
	int							Get_xToSave			(int Direction, int x)		const	{	return( (x = Get_xTo  (Direction, x)) < 0 ? 0 : (x >= m_NX ? m_NX - 1 : x) );	}
	int							Get_yToSave			(int Direction, int y)		const	{	return( (y = Get_yTo  (Direction, y)) < 0 ? 0 : (y >= m_NY ? m_NY - 1 : y) );	}
	int							Get_xFromSave		(int Direction, int x)		const	{	return( (x = Get_xFrom(Direction, x)) < 0 ? 0 : (x >= m_NX ? m_NX - 1 : x) );	}
	int							Get_yFromSave		(int Direction, int y)		const	{	return( (y = Get_yFrom(Direction, y)) < 0 ? 0 : (y >= m_NY ? m_NY - 1 : y) );	}

	bool						is_InGrid			(int x, int y)				const	{	return(	x >= 0    && x < m_NX        && y >= 0    && y < m_NY );		}
	bool						is_InGrid			(int x, int y, int Rand)	const	{	return(	x >= Rand && x < m_NX - Rand && y >= Rand && y < m_NY - Rand );	}

	double						Get_Length			(int Direction)				const	{	return( Direction % 2 ? m_Diagonal : m_Cellsize );	}
	double						Get_UnitLength		(int Direction)				const	{	return( Direction % 2 ? sqrt(2.0)  : 1.0 );			}


private:	///////////////////////////////////////////////

	int							m_NX, m_NY, m_NCells;

	double						m_Cellsize, m_Cellarea, m_Diagonal;

	CGEO_Rect					m_Extent;

	CAPI_String					m_Name;

};


///////////////////////////////////////////////////////////
//														 //
//						CGrid							 //
//														 //
///////////////////////////////////////////////////////////

/**
  * CGrid is the data object created for raster handling.
*/
//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CGrid : public CDataObject
{
//---------------------------------------------------------
public:		///////////////////////////////////////////////

	//-----------------------------------------------------
	CGrid(void);

								CGrid	(const CGrid &Grid);
	bool						Create	(const CGrid &Grid);

								CGrid	(const char *File_Name, TGrid_Type Type = GRID_TYPE_Undefined, TGrid_Memory_Type Memory_Type = GRID_MEMORY_Normal);
	bool						Create	(const char *File_Name, TGrid_Type Type = GRID_TYPE_Undefined, TGrid_Memory_Type Memory_Type = GRID_MEMORY_Normal);

								CGrid	(CGrid *pGrid, TGrid_Type Type = GRID_TYPE_Undefined, TGrid_Memory_Type Memory_Type = GRID_MEMORY_Normal);
	bool						Create	(CGrid *pGrid, TGrid_Type Type = GRID_TYPE_Undefined, TGrid_Memory_Type Memory_Type = GRID_MEMORY_Normal);

								CGrid	(const CGrid_System &System, TGrid_Type Type = GRID_TYPE_Undefined, TGrid_Memory_Type Memory_Type = GRID_MEMORY_Normal);
	bool						Create	(const CGrid_System &System, TGrid_Type Type = GRID_TYPE_Undefined, TGrid_Memory_Type Memory_Type = GRID_MEMORY_Normal);

								CGrid	(TGrid_Type Type, int NX, int NY, double Cellsize = 0.0, double xMin = 0.0, double yMin = 0.0, TGrid_Memory_Type Memory_Type = GRID_MEMORY_Normal);
	bool						Create	(TGrid_Type Type, int NX, int NY, double Cellsize = 0.0, double xMin = 0.0, double yMin = 0.0, TGrid_Memory_Type Memory_Type = GRID_MEMORY_Normal);


	//-----------------------------------------------------
	virtual ~CGrid(void);

	virtual bool				Destroy	(void);


	//-----------------------------------------------------
	/** Data object type information.
	*/
	virtual TDataObject_Type	Get_ObjectType	(void)			{	return( DATAOBJECT_TYPE_Grid );		}


	//-----------------------------------------------------
	// Data-Info...

	TGrid_Type					Get_Type		(void)	const	{	return( m_Type );					}

	int							Get_nValueBytes	(void)	const	{	return( GRID_TYPE_SIZES[m_Type] );	}

	void						Set_Description	(const char *String);
	const char *				Get_Description	(void);

	void						Set_Unit		(const char *String);
	const char *				Get_Unit		(void);

	const CGrid_System &		Get_System		(void)	const	{	return( m_System );					}

	int							Get_NX			(void)	const	{	return( m_System.Get_NX() );		}
	int							Get_NY			(void)	const	{	return( m_System.Get_NY() );		}
	long						Get_NCells		(void)	const	{	return( m_System.Get_NCells() );	}

	double						Get_Cellsize	(void)	const	{	return( m_System.Get_Cellsize() );	}
	double						Get_Cellarea	(void)	const	{	return( m_System.Get_Cellarea() );	}

	const CGEO_Rect &			Get_Extent		(void)	const	{	return( m_System.Get_Extent() );	}

	double						Get_XMin		(void)	const	{	return( m_System.Get_XMin() );		}
	double						Get_XMax		(void)	const	{	return( m_System.Get_XMax() );		}
	double						Get_XRange		(void)	const	{	return( m_System.Get_XRange() );	}

	double						Get_YMin		(void)	const	{	return( m_System.Get_YMin() );		}
	double						Get_YMax		(void)	const	{	return( m_System.Get_YMax() );		}
	double						Get_YRange		(void)	const	{	return( m_System.Get_YRange() );	}

	double						Get_ZMin		(bool bZFactor = false);
	double						Get_ZMax		(bool bZFactor = false);
	double						Get_ZRange		(bool bZFactor = false);

	void						Set_ZFactor		(double Value);
	double						Get_ZFactor		(void);

	double						Get_ArithMean	(bool bZFactor = false);
	double						Get_Variance	(bool bZFactor = false);

	void						Set_NoData_Value			(double Value);
	void						Set_NoData_Value_Range		(double loValue, double hiValue);
	double						Get_NoData_Value			(void)	{	return( m_NoData_Value );			}
	double						Get_NoData_hiValue			(void)	{	return( m_NoData_hiValue );			}

	bool						Update_Statistics			(bool bEnforce = false);

	virtual bool				Save	(const char *File_Name, int Format = GRID_FILE_FORMAT_Binary);
	virtual bool				Save	(const char *File_Name, int Format, int xA, int yA, int xN, int yN);


	//-----------------------------------------------------
	// Checks...

	virtual bool				is_Valid		(void);

	TGEO_Intersection			is_Intersecting	(const CGEO_Rect &Extent) const;
	TGEO_Intersection			is_Intersecting	(const TGEO_Rect &Extent) const;
	TGEO_Intersection			is_Intersecting	(double xMin, double yMin, double xMax, double yMax) const;

	bool						is_Compatible	(const CGrid_System &System) const;
	bool						is_Compatible	(int NX, int NY, double Cellsize, double xMin, double yMin) const;

	bool						is_InGrid(int x, int y, bool bCheckNoData = true)	{	return( m_System.is_InGrid(x, y) && (!bCheckNoData || (bCheckNoData && !is_NoData(x, y))) );	}
	bool						is_InGrid_byPos	(double xPos, double yPos)			{	return( xPos >= Get_XMin() && xPos <= Get_XMax() && yPos >= Get_YMin() && yPos <= Get_YMax() );	}
	bool						is_InGrid_byPos	(TGEO_Point Position)				{	return( is_InGrid_byPos(Position.x, Position.y) );	}


	//-----------------------------------------------------
	// Memory...

	int							Get_Buffer_Size				(void)					{	return( LineBuffer_Count * _LineBuffer_Get_nBytes() );	}
	bool						Set_Buffer_Size				(int Size);

	bool						Set_Cache					(bool bOn);
	bool						is_Cached					(void);

	bool						Set_Compression				(bool bOn);
	bool						is_Compressed				(void);
	double						Get_Compression_Ratio		(void);


	//-----------------------------------------------------
	// Operations...

	void						Assign_NoData				(void);

	virtual bool				Assign						(double Value = 0.0);
	virtual bool				Assign						(CDataObject *pObject);
	virtual bool				Assign						(CGrid *pGrid, TGrid_Interpolation Interpolation);

	void						Flip						(void);
	void						Mirror						(void);
	void						Invert						(void);

	void						Normalise					(void);
	void						DeNormalise					(double ArithMean, double Variance);

	int							Get_Gradient_NeighborDir	(int x, int y, bool bMustBeLower = true);
	bool						Get_Gradient				(int x, int y, double &Decline, double &Azimuth);


	//-----------------------------------------------------
	// Set update flag when modified...

	virtual void				Set_Modified	(bool bFlag = true)
	{
		CDataObject::Set_Modified(bFlag);

		if( bFlag )
		{
			m_bUpdate	= true;

			m_bSorted	= false;
		}
	}


	//-----------------------------------------------------
	// Sort...

	void						Sort_Discard	(void);

	bool						Get_Sorted		(long Position, int &x, int &y, bool bDown = true, bool bCheckNoData = true)
	{
		if( Position >= 0 && Position < Get_NCells() && (m_bSorted || _Sort_Execute()) )
		{
			if( !bDown )
			{
				Position	= Get_NCells() - Position - 1;
			}

			if( m_Sort_2b )
			{
				x	= m_Sort_2b[0][Position];
				y	= m_Sort_2b[1][Position];
			}
			else
			{
				x	= m_Sort_4b[0][Position];
				y	= m_Sort_4b[1][Position];
			}

			return( bCheckNoData ? !is_NoData(x, y) : true );
		}

		x	= y	= 0;

		return( false );
	}

	double						Get_Percentile	(double Percent, bool bZFactor = false);


	//-----------------------------------------------------
	// No Data Value...

	virtual bool				is_NoData_Value	(double Value)
	{
		return( m_NoData_Value < m_NoData_hiValue ? m_NoData_Value <= Value && Value <= m_NoData_hiValue : Value == m_NoData_Value );
	}

	virtual bool				is_NoData		(int x, int y)	{	return( is_NoData_Value(asDouble(x, y)) );	}
	virtual bool				is_NoData		(long n)		{	return( is_NoData_Value(asDouble(   n)) );	}

	virtual void				Set_NoData		(int x, int y)	{	Set_Value(x, y, m_NoData_Value );	}
	virtual void				Set_NoData		(long n)		{	Set_Value(   n, m_NoData_Value );	}


	//-----------------------------------------------------
	// Operators...

	virtual void				operator  =		(CGrid &Grid);
	virtual void				operator +=		(CGrid &Grid);
	virtual void				operator -=		(CGrid &Grid);
	virtual void				operator *=		(CGrid &Grid);
	virtual void				operator /=		(CGrid &Grid);

	virtual void				operator +=		(double Value);
	virtual void				operator -=		(double Value);
	virtual void				operator *=		(double Value);
	virtual void				operator /=		(double Value);

	virtual CGrid &				operator ()		(int x, int y)
	{
		x_Reference	= x;
		y_Reference	= y;

		return( *this ); 
	}

	virtual void				operator =		(double const &Value)
	{
		if( is_InGrid(x_Reference, y_Reference) )
		{
			Set_Value(x_Reference, y_Reference, Value);
		}
	}

	virtual						operator double	(void)
	{
		return( is_InGrid(x_Reference, y_Reference) ? asDouble(x_Reference, y_Reference, false) : m_NoData_Value );
	}


	//-----------------------------------------------------
	// Get Value...

	double						Get_Value(double xPos, double yPos, int Interpolation = GRID_INTERPOLATION_BSpline, bool bZFactor = false);
	double						Get_Value(TGEO_Point Position     , int Interpolation = GRID_INTERPOLATION_BSpline, bool bZFactor = false);
	bool						Get_Value(double xPos, double yPos, double &Value, int Interpolation = GRID_INTERPOLATION_BSpline, bool bZFactor = false);
	bool						Get_Value(TGEO_Point Position     , double &Value, int Interpolation = GRID_INTERPOLATION_BSpline, bool bZFactor = false);

	virtual BYTE				asByte	(int x, int y, bool bZFactor = false)	{	return( (BYTE )asDouble(x, y, bZFactor) );	}
	virtual BYTE				asByte	(      long n, bool bZFactor = false)	{	return( (BYTE )asDouble(   n, bZFactor) );	}
	virtual char				asChar	(int x, int y, bool bZFactor = false)	{	return( (char )asDouble(x, y, bZFactor) );	}
	virtual char				asChar	(      long n, bool bZFactor = false)	{	return( (char )asDouble(   n, bZFactor) );	}
	virtual short				asShort	(int x, int y, bool bZFactor = false)	{	return( (short)asDouble(x, y, bZFactor) );	}
	virtual short				asShort	(      long n, bool bZFactor = false)	{	return( (short)asDouble(   n, bZFactor) );	}
	virtual int					asInt	(int x, int y, bool bZFactor = false)	{	return( (int  )asDouble(x, y, bZFactor) );	}
	virtual int					asInt	(      long n, bool bZFactor = false)	{	return( (int  )asDouble(   n, bZFactor) );	}
	virtual long				asLong	(int x, int y, bool bZFactor = false)	{	return( (long )asDouble(x, y, bZFactor) );	}
	virtual long				asLong	(      long n, bool bZFactor = false)	{	return( (long )asDouble(   n, bZFactor) );	}
	virtual float				asFloat	(int x, int y, bool bZFactor = false)	{	return( (float)asDouble(x, y, bZFactor) );	}
	virtual float				asFloat	(      long n, bool bZFactor = false)	{	return( (float)asDouble(   n, bZFactor) );	}

	//-----------------------------------------------------
	virtual double				asDouble(      long n, bool bZFactor = false)
	{
		return( asDouble(n % Get_NX(), n / Get_NX(), bZFactor) );
	}

	virtual double				asDouble(int x, int y, bool bZFactor = false)
	{
		double	Result;

		if( m_Memory_Type == GRID_MEMORY_Normal )
		{
			switch( m_Type )
			{
				default:				Result	= 0.0;							break;
				case GRID_TYPE_Byte:	Result	= ((BYTE   **)m_Values)[y][x];	break;
				case GRID_TYPE_Char:	Result	= ((char   **)m_Values)[y][x];	break;
				case GRID_TYPE_Word:	Result	= ((WORD   **)m_Values)[y][x];	break;
				case GRID_TYPE_Short:	Result	= ((short  **)m_Values)[y][x];	break;
				case GRID_TYPE_DWord:	Result	= ((DWORD  **)m_Values)[y][x];	break;
				case GRID_TYPE_Int:		Result	= ((int    **)m_Values)[y][x];	break;
				case GRID_TYPE_Long:	Result	= ((long   **)m_Values)[y][x];	break;
				case GRID_TYPE_Float:	Result	= ((float  **)m_Values)[y][x];	break;
				case GRID_TYPE_Double:	Result	= ((double **)m_Values)[y][x];	break;
				case GRID_TYPE_Bit:		Result	=(((BYTE   **)m_Values)[y][x / 8] & m_Bitmask[x % 8]) == 0 ? 0.0 : 1.0;	break;
			}
		}
		else
		{
			Result	= _LineBuffer_Get_Value(x, y);
		}

		if( bZFactor )
		{
			Result	*= m_zFactor;
		}

		return( Result );
	}


	//-----------------------------------------------------
	// Set Value...

	virtual void				Add_Value(int x, int y, double Value)	{	Set_Value(x, y, asDouble(x, y) + Value );	}
	virtual void				Add_Value(      long n, double Value)	{	Set_Value(   n, asDouble(   n) + Value );	}

	virtual void				Mul_Value(int x, int y, double Value)	{	Set_Value(x, y, asDouble(x, y) * Value );	}
	virtual void				Mul_Value(      long n, double Value)	{	Set_Value(   n, asDouble(   n) * Value );	}

	//-----------------------------------------------------
	virtual void				Set_Value(      long n, double Value)
	{
		Set_Value(n % Get_NX(), n / Get_NX(), Value);
	}

	virtual void				Set_Value(int x, int y, double Value)
	{
		if( m_Memory_Type == GRID_MEMORY_Normal )
		{
			switch( m_Type )
			{
			    default:																break;
				case GRID_TYPE_Byte:	((BYTE   **)m_Values)[y][x]	= (BYTE  )Value;	break;
				case GRID_TYPE_Char:	((char   **)m_Values)[y][x]	= (char  )Value;	break;
				case GRID_TYPE_Word:	((WORD   **)m_Values)[y][x]	= (WORD  )Value;	break;
				case GRID_TYPE_Short:	((short  **)m_Values)[y][x]	= (short )Value;	break;
				case GRID_TYPE_DWord:	((DWORD  **)m_Values)[y][x]	= (DWORD )Value;	break;
				case GRID_TYPE_Int:		((int    **)m_Values)[y][x]	= (int   )Value;	break;
				case GRID_TYPE_Long:	((long   **)m_Values)[y][x]	= (long  )Value;	break;
				case GRID_TYPE_Float:	((float  **)m_Values)[y][x]	= (float )Value;	break;
				case GRID_TYPE_Double:	((double **)m_Values)[y][x]	= (double)Value;	break;
				case GRID_TYPE_Bit:		((BYTE   **)m_Values)[y][x / 8]	= Value != 0.0
						? ((BYTE  **)m_Values)[y][x / 8] |   m_Bitmask[x % 8]
						: ((BYTE  **)m_Values)[y][x / 8] & (~m_Bitmask[x % 8]);
					break;
			}
		}
		else
		{
			_LineBuffer_Set_Value(x, y, Value);
		}

		Set_Modified();
	}


//---------------------------------------------------------
private:	///////////////////////////////////////////////

	void						**m_Values;

	bool						m_bCreated, m_bUpdate, m_bSorted, m_Memory_bLock,
								Cache_bTemp, Cache_bSwap, Cache_bFlip;

	unsigned short				**m_Sort_2b;	// Index sorted by data values (NX && NY < 2^8)...

	int							**m_Sort_4b,	// Index sorted by data values (NX || NY >= 2^8)...
								x_Reference, y_Reference,
								LineBuffer_Count;

	long						Cache_Offset;

	double						m_zMin, m_zMax, m_zFactor,
								m_ArithMean, m_Variance,
								m_NoData_Value, m_NoData_hiValue;

	FILE						*Cache_Stream;

	TGrid_Type					m_Type;

	TGrid_Memory_Type			m_Memory_Type;

	CGrid_System				m_System;

	CAPI_String					m_Description, m_Unit, Cache_Path;


	//-----------------------------------------------------
	static	BYTE				m_Bitmask[8];

	//-----------------------------------------------------
	typedef struct
	{
		bool	bModified;
		int		y;
		char	*Data;
	}
	TGrid_Memory_Line;

	TGrid_Memory_Line			*LineBuffer;


	//-----------------------------------------------------
	void						_On_Construction		(void);

	void						_Set_Properties			(TGrid_Type m_Type, int NX, int NY, double Cellsize, double xMin, double yMin);

	bool						_Sort_Execute			(void);
	bool						_Sort_Index				(long *Index);


	//-----------------------------------------------------
	// Memory handling...

	int							_Get_nLineBytes			(void)	{	return( m_Type == GRID_TYPE_Bit ? Get_NX() / 8 + 1 : Get_NX() * Get_nValueBytes() );	}

	bool						_Memory_Create			(TGrid_Memory_Type aMemory_Type);
	void						_Memory_Destroy			(void);

	void						_LineBuffer_Create		(void);
	void						_LineBuffer_Destroy		(void);
	int							_LineBuffer_Get_nBytes	(void)	{	return( GRID_TYPE_SIZES[m_Type] * Get_NX() );	}
	void						_LineBuffer_Flush		(void);
	TGrid_Memory_Line *			_LineBuffer_Get_Line	(int y);
	void						_LineBuffer_Set_Value	(int x, int y, double Value);
	double						_LineBuffer_Get_Value	(int x, int y);

	bool						_Array_Create			(void);
	void						_Array_Destroy			(void);

	bool						_Cache_Create			(const char *FilePath, TGrid_Type File_Type, long Offset, bool bSwap, bool bFlip);
	bool						_Cache_Create			(void);
	bool						_Cache_Destroy			(bool bMemory_Restore);
	void						_Cache_LineBuffer_Save	(TGrid_Memory_Line *pLine);
	void						_Cache_LineBuffer_Load	(TGrid_Memory_Line *pLine, int y);

	bool						_Compr_Create			(void);
	bool						_Compr_Destroy			(bool bMemory_Restore);
	void						_Compr_LineBuffer_Save	(TGrid_Memory_Line *pLine);
	void						_Compr_LineBuffer_Load	(TGrid_Memory_Line *pLine, int y);


	//-----------------------------------------------------
	// File access...

	void						_Swap_Bytes				(char *Bytes, int nBytes);

	bool						_Load					(const char *File_Name, TGrid_Type m_Type, TGrid_Memory_Type aMemory_Type);

	bool						_Load_Binary			(FILE *Stream, TGrid_Type File_Type, bool bFlip, bool bSwapBytes);
	bool						_Save_Binary			(FILE *Stream, int xA, int yA, int xN, int yN, TGrid_Type File_Type, bool bFlip, bool bSwapBytes);
	bool						_Load_ASCII				(FILE *Stream, TGrid_Memory_Type aMemory_Type, bool bFlip = false);
	bool						_Save_ASCII				(FILE *Stream, int xA, int yA, int xN, int yN, bool bFlip = false);
	bool						_Load_Native			(const char *File_Header, TGrid_Memory_Type aMemory_Type);
	bool						_Save_Native			(const char *File_Header, int xA, int yA, int xN, int yN, bool bBinary = true);

	int							_Load_Native_Get_Key	(FILE *Stream, CAPI_String &Line);

	bool						_Load_Surfer			(const char *File_Name, TGrid_Memory_Type aMemory_Type);

	//-----------------------------------------------------
	void						_Operation_Arithmetic	(CGrid *pGrid, TGrid_Operation Operation);
	void						_Operation_Arithmetic	(double Value, TGrid_Operation Operation);


	//-----------------------------------------------------
	// Interpolation subroutines...

	double						_Get_ValAtPos_NearestNeighbour	(int x, int y, double dx, double dy);
	double						_Get_ValAtPos_BiLinear			(int x, int y, double dx, double dy);
	double						_Get_ValAtPos_InverseDistance	(int x, int y, double dx, double dy);
	double						_Get_ValAtPos_BiCubicSpline		(int x, int y, double dx, double dy);
	double						_Get_ValAtPos_BSpline			(int x, int y, double dx, double dy);
	bool						_Get_ValAtPos_Fill4x4Submatrix	(int x, int y, double z_xy[4][4]);

	bool						_Assign_Interpolated	(CGrid *pSource, TGrid_Interpolation Interpolation);
	bool						_Assign_MeanValue		(CGrid *pSource);

};


///////////////////////////////////////////////////////////
//														 //
//						Functions						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/** Safe grid construction */
SAGA_API_DLL_EXPORT CGrid *			API_Create_Grid		(void);

/** Safe grid construction */
SAGA_API_DLL_EXPORT CGrid *			API_Create_Grid		(const CGrid &Grid);

/** Safe grid construction */
SAGA_API_DLL_EXPORT CGrid *			API_Create_Grid		(const char *FileName, TGrid_Type Type = GRID_TYPE_Undefined, TGrid_Memory_Type Memory_Type = GRID_MEMORY_Normal);

/** Safe grid construction */
SAGA_API_DLL_EXPORT CGrid *			API_Create_Grid		(CGrid *pGrid, TGrid_Type Type = GRID_TYPE_Undefined, TGrid_Memory_Type Memory_Type = GRID_MEMORY_Normal);

/** Safe grid construction */
SAGA_API_DLL_EXPORT CGrid *			API_Create_Grid		(CGrid_System &System, TGrid_Type Type, TGrid_Memory_Type Memory_Type = GRID_MEMORY_Normal);

/** Safe grid construction */
SAGA_API_DLL_EXPORT CGrid *			API_Create_Grid		(TGrid_Type Type, int NX, int NY, double Cellsize = 0.0, double xMin = 0.0, double yMin = 0.0, TGrid_Memory_Type Memory_Type = GRID_MEMORY_Normal);

//---------------------------------------------------------
/** Get default directory for grid caching */
SAGA_API_DLL_EXPORT const char *	API_Grid_Cache_Get_Directory	(void);

/** Set default directory for grid caching */
SAGA_API_DLL_EXPORT void			API_Grid_Cache_Set_Directory	(const char *Directory);

SAGA_API_DLL_EXPORT void			API_Grid_Cache_Set_Automatic	(bool bOn);
SAGA_API_DLL_EXPORT bool			API_Grid_Cache_Get_Automatic	(void);

SAGA_API_DLL_EXPORT void			API_Grid_Cache_Set_Confirm		(int Confirm);
SAGA_API_DLL_EXPORT int				API_Grid_Cache_Get_Confirm		(void);

SAGA_API_DLL_EXPORT void			API_Grid_Cache_Set_Threshold	(int nBytes);
SAGA_API_DLL_EXPORT void			API_Grid_Cache_Set_Threshold_MB	(double nMegabytes);
SAGA_API_DLL_EXPORT int				API_Grid_Cache_Get_Threshold	(void);
SAGA_API_DLL_EXPORT double			API_Grid_Cache_Get_Threshold_MB	(void);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__grid_H

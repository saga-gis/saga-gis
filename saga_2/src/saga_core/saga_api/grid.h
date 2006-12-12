
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
typedef enum ESG_Grid_Type
{
	GRID_TYPE_Undefined		= 0,
	GRID_TYPE_Byte,
	GRID_TYPE_Char,
	GRID_TYPE_Word,
	GRID_TYPE_Short,
	GRID_TYPE_DWord,
	GRID_TYPE_Int,
	GRID_TYPE_Float,
	GRID_TYPE_Double,
	GRID_TYPE_Bit,
	GRID_TYPE_Count
}
TSG_Grid_Type;

//---------------------------------------------------------
const SG_Char gSG_Grid_Type_Names[GRID_TYPE_Count][32]	=
{
	SG_T("ASCII"),
	SG_T("BYTE_UNSIGNED"),
	SG_T("BYTE"),
	SG_T("SHORTINT_UNSIGNED"),
	SG_T("SHORTINT"),
	SG_T("INTEGER_UNSIGNED"),
	SG_T("INTEGER"),
	SG_T("FLOAT"),
	SG_T("DOUBLE"),
	SG_T("BIT")
};

//---------------------------------------------------------
const SG_Char gSG_Grid_Type_Sizes[GRID_TYPE_Count]	= 
{
	0,
	1,
	1,
	2,
	2,
	4,
	4,
	4,
	8,
	-1
};


///////////////////////////////////////////////////////////
//														 //
//					Memory Handling						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum ESG_Grid_Memory_Type
{
	GRID_MEMORY_Normal					= 0,
	GRID_MEMORY_Cache,
	GRID_MEMORY_Compression
}
TSG_Grid_Memory_Type;


///////////////////////////////////////////////////////////
//														 //
//						Grid File						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum ESG_Grid_File_Format
{
	GRID_FILE_FORMAT_Undefined			= 0,
	GRID_FILE_FORMAT_Binary,
	GRID_FILE_FORMAT_ASCII
}
TSG_Grid_File_Format;

//---------------------------------------------------------
typedef enum ESG_Grid_File_Key
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
TSG_Grid_File_Key;

//---------------------------------------------------------
const SG_Char	gSG_Grid_File_Key_Names[GRID_FILE_KEY_Count][32]	=
{
	SG_T("NAME"),
	SG_T("DESCRIPTION"),
	SG_T("UNIT"),
	SG_T("DATAFILE_NAME"),
	SG_T("DATAFILE_OFFSET"),
	SG_T("DATAFORMAT"),
	SG_T("BYTEORDER_BIG"),
	SG_T("POSITION_XMIN"),
	SG_T("POSITION_YMIN"),
	SG_T("CELLCOUNT_X"),
	SG_T("CELLCOUNT_Y"),
	SG_T("CELLSIZE"),
	SG_T("Z_FACTOR"),
	SG_T("NODATA_VALUE"),
	SG_T("TOPTOBOTTOM")
};

//---------------------------------------------------------
#define GRID_FILE_KEY_TRUE		SG_T("TRUE")
#define GRID_FILE_KEY_FALSE		SG_T("FALSE")


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum ESG_Grid_Interpolation
{
	GRID_INTERPOLATION_NearestNeighbour	= 0,
	GRID_INTERPOLATION_Bilinear,
	GRID_INTERPOLATION_InverseDistance,
	GRID_INTERPOLATION_BicubicSpline,
	GRID_INTERPOLATION_BSpline,
	GRID_INTERPOLATION_Undefined
}
TSG_Grid_Interpolation;

//---------------------------------------------------------
typedef enum ESG_Grid_Operation
{
	GRID_OPERATION_Addition				= 0,
	GRID_OPERATION_Subtraction,
	GRID_OPERATION_Multiplication,
	GRID_OPERATION_Division
}
TSG_Grid_Operation;


///////////////////////////////////////////////////////////
//														 //
//					CSG_Grid_System						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/**
  * CSG_Grid_System is used by the CSG_Grid class to provide
  * information about the number of rows and columns, the
  * cell size and the georeference, which define the grid.
  * It offers various functions, which help when working
  * with grids.
  * @see CSG_Grid
*/
//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Grid_System
{
public:
	CSG_Grid_System(void);
	CSG_Grid_System(const CSG_Grid_System &System);
	CSG_Grid_System(double Cellsize, const CSG_Rect &Extent);
	CSG_Grid_System(double Cellsize, double xMin, double yMin, double xMax, double yMax);
	CSG_Grid_System(double Cellsize, double xMin, double yMin, int NX, int NY);

	~CSG_Grid_System(void);


	//-----------------------------------------------------
	bool						is_Valid			(void)	const;

	const SG_Char *				Get_Name			(bool bShort = true);

	double						Get_Cellsize		(void)	const	{	return( m_Cellsize );				}
	double						Get_Cellarea		(void)	const	{	return( m_Cellarea );				}
	const CSG_Rect &			Get_Extent			(void)	const	{	return( m_Extent );					}

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
	bool						operator ==			(const CSG_Grid_System &System) const;
	void						operator =			(const CSG_Grid_System &System);

	bool						Assign				(const CSG_Grid_System &System);
	bool						Assign				(double Cellsize, const CSG_Rect &Extent);
	bool						Assign				(double Cellsize, double xMin, double yMin, double xMax, double yMax);
	bool						Assign				(double Cellsize, double xMin, double yMin, int NX, int NY);

	bool						is_Equal			(const CSG_Grid_System &System) const;
	bool						is_Equal			(double Cellsize, const TSG_Rect &Extent) const;


	//-----------------------------------------------------
	/// Aligns the world coordinate x to the rows of the grid system and returns it.
	double						Fit_xto_Grid_System	(double x)	const	{	return( Get_XMin() + m_Cellsize * (int)(0.5 + (x - Get_XMin()) / m_Cellsize) );	}

	/// Aligns the world coordinate y to the columns of the grid system and returns it.
	double						Fit_yto_Grid_System	(double y)	const	{	return( Get_YMin() + m_Cellsize * (int)(0.5 + (y - Get_YMin()) / m_Cellsize) );	}

	/// Aligns the world coordinate ptWorld to the rows and columns of the grid system and returns it.
	TSG_Point					Fit_to_Grid_System	(TSG_Point ptWorld)	const
	{
		ptWorld.x	= Fit_xto_Grid_System(ptWorld.x);
		ptWorld.y	= Fit_yto_Grid_System(ptWorld.y);

		return( ptWorld );
	}


	//-----------------------------------------------------
	double						Get_xGrid_to_World	(int xGrid)	const	{	return( Get_XMin() + xGrid * m_Cellsize );	}
	double						Get_yGrid_to_World	(int yGrid)	const	{	return( Get_YMin() + yGrid * m_Cellsize );	}

	TSG_Point					Get_Grid_to_World	(int xGrid, int yGrid)	const
	{
		TSG_Point	pt;

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

	bool						Get_World_to_Grid	(int &xGrid, int &yGrid, TSG_Point ptWorld)	const
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

	CSG_Rect					m_Extent;

	CSG_String					m_Name;

};


///////////////////////////////////////////////////////////
//														 //
//						CSG_Grid						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/**
  * CSG_Grid is the data object created for raster handling.
*/
//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Grid : public CSG_Data_Object
{
//---------------------------------------------------------
public:		///////////////////////////////////////////////

	//-----------------------------------------------------
	CSG_Grid(void);

								CSG_Grid	(const CSG_Grid &Grid);
	bool						Create		(const CSG_Grid &Grid);

								CSG_Grid	(const SG_Char *File_Name, TSG_Grid_Type Type = GRID_TYPE_Undefined, TSG_Grid_Memory_Type Memory_Type = GRID_MEMORY_Normal);
	bool						Create		(const SG_Char *File_Name, TSG_Grid_Type Type = GRID_TYPE_Undefined, TSG_Grid_Memory_Type Memory_Type = GRID_MEMORY_Normal);

								CSG_Grid	(CSG_Grid *pGrid, TSG_Grid_Type Type = GRID_TYPE_Undefined, TSG_Grid_Memory_Type Memory_Type = GRID_MEMORY_Normal);
	bool						Create		(CSG_Grid *pGrid, TSG_Grid_Type Type = GRID_TYPE_Undefined, TSG_Grid_Memory_Type Memory_Type = GRID_MEMORY_Normal);

								CSG_Grid	(const CSG_Grid_System &System, TSG_Grid_Type Type = GRID_TYPE_Undefined, TSG_Grid_Memory_Type Memory_Type = GRID_MEMORY_Normal);
	bool						Create		(const CSG_Grid_System &System, TSG_Grid_Type Type = GRID_TYPE_Undefined, TSG_Grid_Memory_Type Memory_Type = GRID_MEMORY_Normal);

								CSG_Grid	(TSG_Grid_Type Type, int NX, int NY, double Cellsize = 0.0, double xMin = 0.0, double yMin = 0.0, TSG_Grid_Memory_Type Memory_Type = GRID_MEMORY_Normal);
	bool						Create		(TSG_Grid_Type Type, int NX, int NY, double Cellsize = 0.0, double xMin = 0.0, double yMin = 0.0, TSG_Grid_Memory_Type Memory_Type = GRID_MEMORY_Normal);


	//-----------------------------------------------------
	virtual ~CSG_Grid(void);

	virtual bool				Destroy	(void);


	//-----------------------------------------------------
	/** Data object type information.
	*/
	virtual TSG_Data_Object_Type	Get_ObjectType	(void)	const	{	return( DATAOBJECT_TYPE_Grid );		}


	//-----------------------------------------------------
	// Data-Info...

	TSG_Grid_Type				Get_Type		(void)	const	{	return( m_Type );					}

	int							Get_nValueBytes	(void)	const	{	return( gSG_Grid_Type_Sizes[m_Type] );	}

	void						Set_Description	(const SG_Char *String);
	const SG_Char *				Get_Description	(void)	const;

	void						Set_Unit		(const SG_Char *String);
	const SG_Char *				Get_Unit		(void)	const;

	const CSG_Grid_System &		Get_System		(void)	const	{	return( m_System );					}

	int							Get_NX			(void)	const	{	return( m_System.Get_NX() );		}
	int							Get_NY			(void)	const	{	return( m_System.Get_NY() );		}
	long						Get_NCells		(void)	const	{	return( m_System.Get_NCells() );	}

	double						Get_Cellsize	(void)	const	{	return( m_System.Get_Cellsize() );	}
	double						Get_Cellarea	(void)	const	{	return( m_System.Get_Cellarea() );	}

	const CSG_Rect &			Get_Extent		(void)	const	{	return( m_System.Get_Extent() );	}

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
	double						Get_ZFactor		(void)	const;

	double						Get_ArithMean	(bool bZFactor = false);
	double						Get_Variance	(bool bZFactor = false);

	void						Set_NoData_Value		(double Value);
	void						Set_NoData_Value_Range	(double loValue, double hiValue);
	double						Get_NoData_Value		(void)	const	{	return( m_NoData_Value );			}
	double						Get_NoData_hiValue		(void)	const	{	return( m_NoData_hiValue );			}

	bool						Update_Statistics		(bool bEnforce = false);

	virtual bool				Save	(const SG_Char *File_Name, int Format = GRID_FILE_FORMAT_Binary);
	virtual bool				Save	(const SG_Char *File_Name, int Format, int xA, int yA, int xN, int yN);


	//-----------------------------------------------------
	// Checks...

	virtual bool				is_Valid		(void)	const;

	TSG_Intersection			is_Intersecting	(const CSG_Rect &Extent) const;
	TSG_Intersection			is_Intersecting	(const TSG_Rect &Extent) const;
	TSG_Intersection			is_Intersecting	(double xMin, double yMin, double xMax, double yMax) const;

	bool						is_Compatible	(CSG_Grid *pGrid) const;
	bool						is_Compatible	(const CSG_Grid_System &System) const;
	bool						is_Compatible	(int NX, int NY, double Cellsize, double xMin, double yMin) const;

	bool						is_InGrid(int x, int y, bool bCheckNoData = true)	const	{	return( m_System.is_InGrid(x, y) && (!bCheckNoData || (bCheckNoData && !is_NoData(x, y))) );	}
	bool						is_InGrid_byPos	(double xPos, double yPos)			const	{	return( xPos >= Get_XMin() && xPos <= Get_XMax() && yPos >= Get_YMin() && yPos <= Get_YMax() );	}
	bool						is_InGrid_byPos	(TSG_Point Position)				const	{	return( is_InGrid_byPos(Position.x, Position.y) );	}


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
	virtual bool				Assign						(CSG_Data_Object *pObject);
	virtual bool				Assign						(CSG_Grid *pGrid, TSG_Grid_Interpolation Interpolation);

	void						Flip						(void);
	void						Mirror						(void);
	void						Invert						(void);

	void						Normalise					(void);
	void						DeNormalise					(double ArithMean, double Variance);

	int							Get_Gradient_NeighborDir	(int x, int y, bool bMustBeLower = true)			const;
	bool						Get_Gradient				(int x, int y, double &Decline, double &Azimuth)	const;


	//-----------------------------------------------------
	// Set update flag when modified...

	virtual void				Set_Modified	(bool bFlag = true)
	{
		CSG_Data_Object::Set_Modified(bFlag);

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

	virtual bool				is_NoData_Value	(double Value)	const
	{
		return( m_NoData_Value < m_NoData_hiValue ? m_NoData_Value <= Value && Value <= m_NoData_hiValue : Value == m_NoData_Value );
	}

	virtual bool				is_NoData		(int x, int y)	const	{	return( is_NoData_Value(asDouble(x, y)) );	}
	virtual bool				is_NoData		(long n)		const	{	return( is_NoData_Value(asDouble(   n)) );	}

	virtual void				Set_NoData		(int x, int y)	{	Set_Value(x, y, m_NoData_Value );	}
	virtual void				Set_NoData		(long n)		{	Set_Value(   n, m_NoData_Value );	}


	//-----------------------------------------------------
	// Operators...

	virtual CSG_Grid &			operator  =		(const CSG_Grid &Grid);
	virtual CSG_Grid &			operator  =		(double Value);

	virtual CSG_Grid			operator +		(const CSG_Grid &Grid)	const;
	virtual CSG_Grid			operator +		(double Value)			const;
	virtual CSG_Grid &			operator +=		(const CSG_Grid &Grid);
	virtual CSG_Grid &			operator +=		(double Value);
	virtual CSG_Grid &			Add				(const CSG_Grid &Grid);
	virtual CSG_Grid &			Add				(double Value);

	virtual CSG_Grid			operator -		(const CSG_Grid &Grid)	const;
	virtual CSG_Grid			operator -		(double Value)			const;
	virtual CSG_Grid &			operator -=		(const CSG_Grid &Grid);
	virtual CSG_Grid &			operator -=		(double Value);
	virtual CSG_Grid &			Subtract		(const CSG_Grid &Grid);
	virtual CSG_Grid &			Subtract		(double Value);

	virtual CSG_Grid			operator *		(const CSG_Grid &Grid)	const;
	virtual CSG_Grid			operator *		(double Value)			const;
	virtual CSG_Grid &			operator *=		(const CSG_Grid &Grid);
	virtual CSG_Grid &			operator *=		(double Value);
	virtual CSG_Grid &			Multiply		(const CSG_Grid &Grid);
	virtual CSG_Grid &			Multiply		(double Value);

	virtual CSG_Grid			operator /		(const CSG_Grid &Grid)	const;
	virtual CSG_Grid			operator /		(double Value)			const;
	virtual CSG_Grid &			operator /=		(const CSG_Grid &Grid);
	virtual CSG_Grid &			operator /=		(double Value);
	virtual CSG_Grid &			Divide			(const CSG_Grid &Grid);
	virtual CSG_Grid &			Divide			(double Value);

	virtual double				operator ()		(int x, int y) const	{	return( asDouble(x, y) );	}


	//-----------------------------------------------------
	// Get Value...

	double						Get_Value(double xPos, double yPos,                int Interpolation = GRID_INTERPOLATION_BSpline, bool bZFactor = false, bool bByteWise = false) const;
	double						Get_Value(TSG_Point Position      ,                int Interpolation = GRID_INTERPOLATION_BSpline, bool bZFactor = false, bool bByteWise = false) const;
	bool						Get_Value(double xPos, double yPos, double &Value, int Interpolation = GRID_INTERPOLATION_BSpline, bool bZFactor = false, bool bByteWise = false) const;
	bool						Get_Value(TSG_Point Position      , double &Value, int Interpolation = GRID_INTERPOLATION_BSpline, bool bZFactor = false, bool bByteWise = false) const;

	virtual BYTE				asByte	(int x, int y, bool bZFactor = false) const	{	return( (BYTE )asDouble(x, y, bZFactor) );	}
	virtual BYTE				asByte	(      long n, bool bZFactor = false) const	{	return( (BYTE )asDouble(   n, bZFactor) );	}
	virtual char				asChar	(int x, int y, bool bZFactor = false) const	{	return( (char )asDouble(x, y, bZFactor) );	}
	virtual char				asChar	(      long n, bool bZFactor = false) const	{	return( (char )asDouble(   n, bZFactor) );	}
	virtual short				asShort	(int x, int y, bool bZFactor = false) const	{	return( (short)asDouble(x, y, bZFactor) );	}
	virtual short				asShort	(      long n, bool bZFactor = false) const	{	return( (short)asDouble(   n, bZFactor) );	}
	virtual int					asInt	(int x, int y, bool bZFactor = false) const	{	return( (int  )asDouble(x, y, bZFactor) );	}
	virtual int					asInt	(      long n, bool bZFactor = false) const	{	return( (int  )asDouble(   n, bZFactor) );	}
	virtual float				asFloat	(int x, int y, bool bZFactor = false) const	{	return( (float)asDouble(x, y, bZFactor) );	}
	virtual float				asFloat	(      long n, bool bZFactor = false) const	{	return( (float)asDouble(   n, bZFactor) );	}

	//-----------------------------------------------------
	virtual double				asDouble(      long n, bool bZFactor = false) const
	{
		return( asDouble(n % Get_NX(), n / Get_NX(), bZFactor) );
	}

	virtual double				asDouble(int x, int y, bool bZFactor = false) const
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
								LineBuffer_Count;

	long						Cache_Offset;

	double						m_zMin, m_zMax, m_zFactor,
								m_ArithMean, m_Variance,
								m_NoData_Value, m_NoData_hiValue;

	CSG_File					Cache_Stream;

	TSG_Grid_Type				m_Type;

	TSG_Grid_Memory_Type		m_Memory_Type;

	CSG_Grid_System				m_System;

	CSG_String					m_Description, m_Unit, Cache_Path;


	//-----------------------------------------------------
	static	BYTE				m_Bitmask[8];

	//-----------------------------------------------------
	typedef struct
	{
		bool	bModified;
		int		y;
		char	*Data;
	}
	TSG_Grid_Line;

	TSG_Grid_Line				*LineBuffer;


	//-----------------------------------------------------
	void						_On_Construction		(void);

	void						_Set_Properties			(TSG_Grid_Type m_Type, int NX, int NY, double Cellsize, double xMin, double yMin);

	bool						_Sort_Execute			(void);
	bool						_Sort_Index				(long *Index);


	//-----------------------------------------------------
	// Memory handling...

	int							_Get_nLineBytes			(void)	{	return( m_Type == GRID_TYPE_Bit ? Get_NX() / 8 + 1 : Get_NX() * Get_nValueBytes() );	}

	bool						_Memory_Create			(TSG_Grid_Memory_Type aMemory_Type);
	void						_Memory_Destroy			(void);

	void						_LineBuffer_Create		(void);
	void						_LineBuffer_Destroy		(void);
	int							_LineBuffer_Get_nBytes	(void) const	{	return( gSG_Grid_Type_Sizes[m_Type] * Get_NX() );	}
	void						_LineBuffer_Flush		(void);
	TSG_Grid_Line *				_LineBuffer_Get_Line	(int y)							const;
	void						_LineBuffer_Set_Value	(int x, int y, double Value);
	double						_LineBuffer_Get_Value	(int x, int y)					const;

	bool						_Array_Create			(void);
	void						_Array_Destroy			(void);

	bool						_Cache_Create			(const SG_Char *FilePath, TSG_Grid_Type File_Type, long Offset, bool bSwap, bool bFlip);
	bool						_Cache_Create			(void);
	bool						_Cache_Destroy			(bool bMemory_Restore);
	void						_Cache_LineBuffer_Save	(TSG_Grid_Line *pLine)			const;
	void						_Cache_LineBuffer_Load	(TSG_Grid_Line *pLine, int y)	const;

	bool						_Compr_Create			(void);
	bool						_Compr_Destroy			(bool bMemory_Restore);
	void						_Compr_LineBuffer_Save	(TSG_Grid_Line *pLine)			const;
	void						_Compr_LineBuffer_Load	(TSG_Grid_Line *pLine, int y)	const;


	//-----------------------------------------------------
	// File access...

	void						_Swap_Bytes				(char *Bytes, int nBytes)			const;

	bool						_Load					(const SG_Char *File_Name, TSG_Grid_Type m_Type, TSG_Grid_Memory_Type aMemory_Type);

	bool						_Load_Binary			(CSG_File &Stream, TSG_Grid_Type File_Type, bool bFlip, bool bSwapBytes);
	bool						_Save_Binary			(CSG_File &Stream, int xA, int yA, int xN, int yN, TSG_Grid_Type File_Type, bool bFlip, bool bSwapBytes);
	bool						_Load_ASCII				(CSG_File &Stream, TSG_Grid_Memory_Type aMemory_Type, bool bFlip = false);
	bool						_Save_ASCII				(CSG_File &Stream, int xA, int yA, int xN, int yN, bool bFlip = false);
	bool						_Load_Native			(const SG_Char *File_Header, TSG_Grid_Memory_Type aMemory_Type);
	bool						_Save_Native			(const SG_Char *File_Header, int xA, int yA, int xN, int yN, bool bBinary = true);

	int							_Load_Native_Get_Key	(CSG_File &Stream, CSG_String &Line);

	bool						_Load_Surfer			(const SG_Char *File_Name, TSG_Grid_Memory_Type aMemory_Type);

	//-----------------------------------------------------
	CSG_Grid &					_Operation_Arithmetic	(const CSG_Grid &Grid, TSG_Grid_Operation Operation);
	CSG_Grid &					_Operation_Arithmetic	(double Value        , TSG_Grid_Operation Operation);


	//-----------------------------------------------------
	// Interpolation subroutines...

	double						_Get_ValAtPos_NearestNeighbour	(int x, int y, double dx, double dy)				 const;
	double						_Get_ValAtPos_BiLinear			(int x, int y, double dx, double dy, bool bByteWise) const;
	double						_Get_ValAtPos_InverseDistance	(int x, int y, double dx, double dy, bool bByteWise) const;
	double						_Get_ValAtPos_BiCubicSpline		(int x, int y, double dx, double dy, bool bByteWise) const;
	double						_Get_ValAtPos_BiCubicSpline		(double dx, double dy, double z_xy[4][4])			 const;
	double						_Get_ValAtPos_BSpline			(int x, int y, double dx, double dy, bool bByteWise) const;
	double						_Get_ValAtPos_BSpline			(double dx, double dy, double z_xy[4][4])			 const;
	bool						_Get_ValAtPos_Fill4x4Submatrix	(int x, int y, double z_xy[4][4])	 const;
	bool						_Get_ValAtPos_Fill4x4Submatrix	(int x, int y, double z_xy[4][4][4]) const;

	bool						_Assign_Interpolated	(CSG_Grid *pSource, TSG_Grid_Interpolation Interpolation);
	bool						_Assign_MeanValue		(CSG_Grid *pSource);

};


///////////////////////////////////////////////////////////
//														 //
//						Functions						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/** Safe grid construction */
SAGA_API_DLL_EXPORT CSG_Grid *		SG_Create_Grid		(void);

/** Safe grid construction */
SAGA_API_DLL_EXPORT CSG_Grid *		SG_Create_Grid		(const CSG_Grid &Grid);

/** Safe grid construction */
SAGA_API_DLL_EXPORT CSG_Grid *		SG_Create_Grid		(const SG_Char *FileName, TSG_Grid_Type Type = GRID_TYPE_Undefined, TSG_Grid_Memory_Type Memory_Type = GRID_MEMORY_Normal);

/** Safe grid construction */
SAGA_API_DLL_EXPORT CSG_Grid *		SG_Create_Grid		(CSG_Grid *pGrid, TSG_Grid_Type Type = GRID_TYPE_Undefined, TSG_Grid_Memory_Type Memory_Type = GRID_MEMORY_Normal);

/** Safe grid construction */
SAGA_API_DLL_EXPORT CSG_Grid *		SG_Create_Grid		(const CSG_Grid_System &System, TSG_Grid_Type Type = GRID_TYPE_Undefined, TSG_Grid_Memory_Type Memory_Type = GRID_MEMORY_Normal);

/** Safe grid construction */
SAGA_API_DLL_EXPORT CSG_Grid *		SG_Create_Grid		(TSG_Grid_Type Type, int NX, int NY, double Cellsize = 0.0, double xMin = 0.0, double yMin = 0.0, TSG_Grid_Memory_Type Memory_Type = GRID_MEMORY_Normal);

//---------------------------------------------------------
/** Get default directory for grid caching */
SAGA_API_DLL_EXPORT const SG_Char *	SG_Grid_Cache_Get_Directory		(void);

/** Set default directory for grid caching */
SAGA_API_DLL_EXPORT void			SG_Grid_Cache_Set_Directory		(const SG_Char *Directory);

SAGA_API_DLL_EXPORT void			SG_Grid_Cache_Set_Automatic		(bool bOn);
SAGA_API_DLL_EXPORT bool			SG_Grid_Cache_Get_Automatic		(void);

SAGA_API_DLL_EXPORT void			SG_Grid_Cache_Set_Confirm		(int Confirm);
SAGA_API_DLL_EXPORT int				SG_Grid_Cache_Get_Confirm		(void);

SAGA_API_DLL_EXPORT void			SG_Grid_Cache_Set_Threshold		(int nBytes);
SAGA_API_DLL_EXPORT void			SG_Grid_Cache_Set_Threshold_MB	(double nMegabytes);
SAGA_API_DLL_EXPORT int				SG_Grid_Cache_Get_Threshold		(void);
SAGA_API_DLL_EXPORT double			SG_Grid_Cache_Get_Threshold_MB	(void);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__grid_H


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
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
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
/** \file grid.h
* Classes for raster data management.
* @see CSG_Data_Object
* @see CSG_Grid_System
* @see CSG_Grid
*/


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "table.h"
#include "grid_pyramid.h"


///////////////////////////////////////////////////////////
//														 //
//						Grid File						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum ESG_Grid_File_Format
{
	GRID_FILE_FORMAT_Undefined			= 0,
	GRID_FILE_FORMAT_Binary_old,
	GRID_FILE_FORMAT_Binary,
	GRID_FILE_FORMAT_ASCII,
	GRID_FILE_FORMAT_Compressed,
	GRID_FILE_FORMAT_GeoTIFF
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
	GRID_FILE_KEY_Z_OFFSET,
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
	SG_T("Z_OFFSET"),
	SG_T("NODATA_VALUE"),
	SG_T("TOPTOBOTTOM")
};

//---------------------------------------------------------
#define GRID_FILE_KEY_TRUE	SG_T("TRUE")
#define GRID_FILE_KEY_FALSE	SG_T("FALSE")


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
typedef enum ESG_Grid_Resampling
{
	GRID_RESAMPLING_NearestNeighbour	= 0,
	GRID_RESAMPLING_Bilinear,
	GRID_RESAMPLING_BicubicSpline,
	GRID_RESAMPLING_BSpline,

	GRID_RESAMPLING_Mean_Nodes,
	GRID_RESAMPLING_Mean_Cells,
	GRID_RESAMPLING_Minimum,
	GRID_RESAMPLING_Maximum,
	GRID_RESAMPLING_Majority,

	GRID_RESAMPLING_Undefined
}
TSG_Grid_Resampling;

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

								CSG_Grid_System		(const CSG_Grid_System &System);
	bool						Create				(const CSG_Grid_System &System);

								CSG_Grid_System		(double Cellsize, const CSG_Rect &Extent);
	bool						Create				(double Cellsize, const CSG_Rect &Extent);

								CSG_Grid_System		(double Cellsize, double xMin, double yMin, double xMax, double yMax);
	bool						Create				(double Cellsize, double xMin, double yMin, double xMax, double yMax);

								CSG_Grid_System		(double Cellsize, double xMin, double yMin, int NX, int NY);
	bool						Create				(double Cellsize, double xMin, double yMin, int NX, int NY);

	~CSG_Grid_System(void);

	bool						Destroy				(void);

	//-----------------------------------------------------
	bool						Assign				(const CSG_Grid_System &System);
	bool						Assign				(double Cellsize, const CSG_Rect &Extent);
	bool						Assign				(double Cellsize, double xMin, double yMin, double xMax, double yMax);
	bool						Assign				(double Cellsize, double xMin, double yMin, int NX, int NY);

	//-----------------------------------------------------
	bool						is_Valid			(void)	const;

	const SG_Char *				Get_Name			(bool bShort = true);

	double						Get_Cellsize		(void)	const	{	return( m_Cellsize );	}
	double						Get_Cellarea		(void)	const	{	return( m_Cellarea );	}

	int							Get_NX				(void)	const	{	return( m_NX );			}
	int							Get_NY				(void)	const	{	return( m_NY );			}
	sLong						Get_NCells			(void)	const	{	return( m_NCells );		}

	const CSG_Rect &			Get_Extent			(bool bCells = false)	const	{	return( bCells ? m_Extent_Cells : m_Extent );	}

	double						Get_XMin			(bool bCells = false)	const	{	return( bCells ? m_Extent_Cells.Get_XMin  () : m_Extent.Get_XMin  () );	}
	double						Get_XMax			(bool bCells = false)	const	{	return( bCells ? m_Extent_Cells.Get_XMax  () : m_Extent.Get_XMax  () );	}
	double						Get_XRange			(bool bCells = false)	const	{	return( bCells ? m_Extent_Cells.Get_XRange() : m_Extent.Get_XRange() );	}

	double						Get_YMin			(bool bCells = false)	const	{	return( bCells ? m_Extent_Cells.Get_YMin  () : m_Extent.Get_YMin  () );	}
	double						Get_YMax			(bool bCells = false)	const	{	return( bCells ? m_Extent_Cells.Get_YMax  () : m_Extent.Get_YMax  () );	}
	double						Get_YRange			(bool bCells = false)	const	{	return( bCells ? m_Extent_Cells.Get_YRange() : m_Extent.Get_YRange() );	}


	//-----------------------------------------------------
	bool						operator ==			(const CSG_Grid_System &System) const;
	void						operator =			(const CSG_Grid_System &System);

	bool						is_Equal			(const CSG_Grid_System &System) const;
	bool						is_Equal			(double Cellsize, const TSG_Rect &Extent) const;


	//-----------------------------------------------------
	/// Aligns the world coordinate x to the rows of the grid system and returns it.
	double						Fit_xto_Grid_System	(double x)	const	{	return( Get_XMin() + m_Cellsize * Get_xWorld_to_Grid(x) );	}

	/// Aligns the world coordinate y to the columns of the grid system and returns it.
	double						Fit_yto_Grid_System	(double y)	const	{	return( Get_YMin() + m_Cellsize * Get_yWorld_to_Grid(y) );	}

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
	int							Get_xWorld_to_Grid	(double xWorld)	const	{	return( (int)floor(0.5 + (xWorld - Get_XMin()) / m_Cellsize) );	}
	int							Get_yWorld_to_Grid	(double yWorld)	const	{	return( (int)floor(0.5 + (yWorld - Get_YMin()) / m_Cellsize) );	}

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

	static int					Get_xTo				(int Direction, int x = 0)
	{
		static int	ix[8]	= { 0, 1, 1, 1, 0,-1,-1,-1 };

		Direction	%= 8;

		if( Direction < 0 )
		{
			Direction	+= 8;
		}

		return( x + ix[Direction] );
	}

	static int					Get_yTo				(int Direction, int y = 0)
	{
		static int	iy[8]	= { 1, 1, 0,-1,-1,-1, 0, 1 };

		Direction	%= 8;

		if( Direction < 0 )
		{
			Direction	+= 8;
		}

		return( y + iy[Direction] );
	}

	static int					Get_xFrom			(int Direction, int x = 0)			{	return( Get_xTo(Direction + 4, x) );	}
	static int					Get_yFrom			(int Direction, int y = 0)			{	return( Get_yTo(Direction + 4, y) );	}

	int							Get_xToSave			(int Direction, int x)		const	{	return( (x = Get_xTo  (Direction, x)) < 0 ? 0 : (x >= m_NX ? m_NX - 1 : x) );	}
	int							Get_yToSave			(int Direction, int y)		const	{	return( (y = Get_yTo  (Direction, y)) < 0 ? 0 : (y >= m_NY ? m_NY - 1 : y) );	}
	int							Get_xFromSave		(int Direction, int x)		const	{	return( (x = Get_xFrom(Direction, x)) < 0 ? 0 : (x >= m_NX ? m_NX - 1 : x) );	}
	int							Get_yFromSave		(int Direction, int y)		const	{	return( (y = Get_yFrom(Direction, y)) < 0 ? 0 : (y >= m_NY ? m_NY - 1 : y) );	}

	bool						is_InGrid			(int x, int y)				const	{	return(	x >= 0    && x < m_NX        && y >= 0    && y < m_NY );		}
	bool						is_InGrid			(int x, int y, int Rand)	const	{	return(	x >= Rand && x < m_NX - Rand && y >= Rand && y < m_NY - Rand );	}

	double						Get_Length			(int Direction)				const	{	return( Direction % 2 ? m_Diagonal : m_Cellsize );	}
	static double				Get_UnitLength		(int Direction)						{	return( Direction % 2 ? sqrt(2.0)  : 1.0 );			}

	static int					Set_Precision		(int Decimals);
	static int					Get_Precision		(void);


private:	///////////////////////////////////////////////

	int							m_NX, m_NY;

	sLong						m_NCells;

	double						m_Cellsize, m_Cellarea, m_Diagonal;

	CSG_Rect					m_Extent, m_Extent_Cells;

	CSG_String					m_Name;

	static int					m_Precision;

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
class SAGA_API_DLL_EXPORT CSG_Grid_File_Info
{
//---------------------------------------------------------
public:		///////////////////////////////////////////////

	//-----------------------------------------------------
	CSG_Grid_File_Info(void);

								CSG_Grid_File_Info		(const CSG_Grid_File_Info &Info);
	bool						Create					(const CSG_Grid_File_Info &Info);

								CSG_Grid_File_Info		(const CSG_String &FileName);
	bool						Create					(const CSG_String &FileName);

								CSG_Grid_File_Info		(CSG_File &Stream);
	bool						Create					(CSG_File &Stream);

								CSG_Grid_File_Info		(const CSG_Grid &Grid);
	bool						Create					(const CSG_Grid &Grid);

	bool						Save					(const CSG_String &FileName, bool bBinary = true);
	bool						Save					(const CSG_File   &Stream  , bool bBinary = true);

	static bool					Save					(const CSG_String &FileName, const CSG_Grid &Grid, bool bBinary = true);
	static bool					Save					(const CSG_File   &Stream  , const CSG_Grid &Grid, bool bBinary = true);

	bool						Save_AUX_XML			(const CSG_String &FileName);
	bool						Save_AUX_XML			(CSG_File &Stream);

	//-----------------------------------------------------
	bool						m_bFlip, m_bSwapBytes;

	sLong						m_Offset;

	double						m_zScale, m_zOffset, m_NoData[2];

	TSG_Data_Type				m_Type;

	CSG_String					m_Name, m_Description, m_Unit, m_Data_File;

	CSG_Grid_System				m_System;

	CSG_Projection				m_Projection;


private:

	void						_On_Construction		(void);

	int							_Get_Key				(CSG_File &Stream, CSG_String &Line);

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

									CSG_Grid		(const CSG_Grid &Grid);
	bool							Create			(const CSG_Grid &Grid);

									CSG_Grid		(const CSG_String &FileName   , TSG_Data_Type Type = SG_DATATYPE_Undefined, bool bCached = false, bool bLoadData = true);
	bool							Create			(const CSG_String &FileName   , TSG_Data_Type Type = SG_DATATYPE_Undefined, bool bCached = false, bool bLoadData = true);

									CSG_Grid		(CSG_Grid *pGrid              , TSG_Data_Type Type = SG_DATATYPE_Undefined, bool bCached = false);
	bool							Create			(CSG_Grid *pGrid              , TSG_Data_Type Type = SG_DATATYPE_Undefined, bool bCached = false);

									CSG_Grid		(const CSG_Grid_System &System, TSG_Data_Type Type = SG_DATATYPE_Undefined, bool bCached = false);
	bool							Create			(const CSG_Grid_System &System, TSG_Data_Type Type = SG_DATATYPE_Undefined, bool bCached = false);

									CSG_Grid		(TSG_Data_Type Type, int NX, int NY, double Cellsize = 0.0, double xMin = 0.0, double yMin = 0.0, bool bCached = false);
	bool							Create			(TSG_Data_Type Type, int NX, int NY, double Cellsize = 0.0, double xMin = 0.0, double yMin = 0.0, bool bCached = false);


	//-----------------------------------------------------
	virtual ~CSG_Grid(void);

	virtual bool					Destroy			(void);


	//-----------------------------------------------------
	virtual bool					Save			(const CSG_String &File, int Format = 0);
	virtual bool					Save			(const char       *File, int Format = 0)	{	return( Save(CSG_String(File), Format) );	}
	virtual bool					Save			(const wchar_t    *File, int Format = 0)	{	return( Save(CSG_String(File), Format) );	}

	//-----------------------------------------------------
	/** Data object type information.
	*/
	virtual TSG_Data_Object_Type	Get_ObjectType	(void)	const	{	return( SG_DATAOBJECT_TYPE_Grid );		}

	//-----------------------------------------------------
	// Data-Info...

	TSG_Data_Type					Get_Type		(void)	const	{	return( m_Type );					}

	int								Get_nValueBytes	(void)	const	{	return( (int)m_nBytes_Value );	}
	int								Get_nLineBytes	(void)	const	{	return( (int)m_nBytes_Line  );	}

	void							Set_Unit		(const CSG_String &Unit);
	const CSG_String &				Get_Unit		(void)	const	{	return( m_Unit );	}


	//-----------------------------------------------------
	// Georeference...

	const CSG_Grid_System &			Get_System		(void)	const	{	return( m_System );					}

	int								Get_NX			(void)	const	{	return( m_System.Get_NX() );		}
	int								Get_NY			(void)	const	{	return( m_System.Get_NY() );		}
	sLong							Get_NCells		(void)	const	{	return( m_System.Get_NCells() );	}

	double							Get_Cellsize	(void)	const	{	return( m_System.Get_Cellsize() );	}
	double							Get_Cellarea	(void)	const	{	return( m_System.Get_Cellarea() );	}

	const CSG_Rect &				Get_Extent		(bool bCells = false)	const	{	return( m_System.Get_Extent(bCells) );	}

	double							Get_XMin		(bool bCells = false)	const	{	return( m_System.Get_XMin  (bCells) );	}
	double							Get_XMax		(bool bCells = false)	const	{	return( m_System.Get_XMax  (bCells) );	}
	double							Get_XRange		(bool bCells = false)	const	{	return( m_System.Get_XRange(bCells) );	}

	double							Get_YMin		(bool bCells = false)	const	{	return( m_System.Get_YMin  (bCells) );	}
	double							Get_YMax		(bool bCells = false)	const	{	return( m_System.Get_YMax  (bCells) );	}
	double							Get_YRange		(bool bCells = false)	const	{	return( m_System.Get_YRange(bCells) );	}


	//-----------------------------------------------------
	// Values...

	void							Set_Scaling			(double Scale = 1.0, double Offset = 0.0);
	double							Get_Scaling			(void)	const;
	double							Get_Offset			(void)	const;
	bool							is_Scaled			(void)	const	{	return( m_zScale != 1.0 || m_zOffset != 0.0 );	}

	double							Get_Mean			(void);
	double							Get_Min				(void);
	double							Get_Max				(void);
	double							Get_Range			(void);
	double							Get_StdDev			(void);
	double							Get_Variance		(void);
	double							Get_Quantile		(double   Quantile, bool bFromHistogram = true);
	double							Get_Percentile		(double Percentile, bool bFromHistogram = true);

	const CSG_Simple_Statistics &	Get_Statistics		(void);
	bool							Get_Statistics		(const CSG_Rect &rWorld, CSG_Simple_Statistics &Statistics, bool bHoldValues = false)	const;

	const CSG_Histogram &			Get_Histogram		(size_t nClasses = 0);
	bool							Get_Histogram		(const CSG_Rect &rWorld, CSG_Histogram &Histogram, size_t nClasses = 0)	const;

	sLong							Get_Data_Count		(void);
	sLong							Get_NoData_Count	(void);


	//-----------------------------------------------------
	// Checks...

	virtual bool					is_Valid			(void)	const;

	TSG_Intersection				is_Intersecting		(const CSG_Rect &Extent) const;
	TSG_Intersection				is_Intersecting		(const TSG_Rect &Extent) const;
	TSG_Intersection				is_Intersecting		(double xMin, double yMin, double xMax, double yMax) const;

	bool							is_Compatible		(CSG_Grid *pGrid) const;
	bool							is_Compatible		(const CSG_Grid_System &System) const;
	bool							is_Compatible		(int NX, int NY, double Cellsize, double xMin, double yMin) const;

	bool							is_InGrid			(int    x, int    y, bool bCheckNoData = true)	const	{	return( m_System.is_InGrid(x, y) && (!bCheckNoData || !is_NoData(x, y)) );	}
	bool							is_InGrid_byPos		(double x, double y, bool bCheckNoData = true)	const	{	return( Get_Extent(true).Contains(x, y) && (!bCheckNoData || !is_NoData(m_System.Get_xWorld_to_Grid(x), m_System.Get_yWorld_to_Grid(y))) );	}
	bool							is_InGrid_byPos		(const TSG_Point &p, bool bCheckNoData = true)	const	{	return( is_InGrid_byPos(p.x, p.y, bCheckNoData) );	}


	//-----------------------------------------------------
	// Memory...

	sLong							Get_Memory_Size			(void)		const	{	return( Get_NCells() * Get_nValueBytes() );	}
	double							Get_Memory_Size_MB		(void)		const	{	return( (double)Get_Memory_Size() / N_MEGABYTE_BYTES );	}

	bool							Set_Cache				(bool bOn);
	bool							is_Cached				(void)		const	{	return( m_Cache_Stream != NULL );	}


	//-----------------------------------------------------
	// Operations...

	void							Assign_NoData				(void);

	virtual bool					Assign						(double Value = 0.0);
	virtual bool					Assign						(CSG_Data_Object *pObject);
	virtual bool					Assign						(CSG_Grid *pGrid, TSG_Grid_Resampling Interpolation);

	void							Flip						(void);
	void							Mirror						(void);
	void							Invert						(void);

	bool							Normalise					(void);
	bool							DeNormalise					(double Minimum, double Maximum);

	bool							Standardise					(void);
	bool							DeStandardise				(double Mean, double StdDev);

	int								Get_Gradient_NeighborDir	(int x, int y, bool bDown = true, bool bNoEdges = true)	const;
	bool							Get_Gradient				(int x, int y      , double &Slope, double &Aspect)	const;
	bool							Get_Gradient				(double x, double y, double &Slope, double &Aspect, TSG_Grid_Resampling Interpolation)	const;
	bool							Get_Gradient				(const TSG_Point &p, double &Slope, double &Aspect, TSG_Grid_Resampling Interpolation)	const;


	//-----------------------------------------------------
	// Set update flag when modified...

	virtual void					Set_Modified	(bool bModified = true)
	{
		CSG_Data_Object::Set_Modified(bModified);

		if( bModified )
		{
			Set_Update_Flag();
		}
	}


	//-----------------------------------------------------
	// Index...

	bool							Set_Index		(bool bOn = true)
	{
		if( !bOn )
		{
			SG_FREE_SAFE(m_Index);

			return( true );
		}

		return( _Get_Index() );
	}

	sLong							Get_Sorted		(sLong Position, bool bDown = true, bool bCheckNoData = true)
	{
		if( Position >= 0 && Position < Get_NCells() && _Get_Index() )
		{
			Position	= m_Index[bDown ? Get_NCells() - Position - 1 : Position];

			if( !bCheckNoData || !is_NoData(Position) )
			{
				return( Position );
			}
		}

		return( -1 );
	}

	bool							Get_Sorted		(sLong Position, sLong &i, bool bDown = true, bool bCheckNoData = true)
	{
		return( (i = Get_Sorted(Position, bDown, bCheckNoData)) >= 0 );
	}

	bool							Get_Sorted		(sLong Position, int &x, int &y, bool bDown = true, bool bCheckNoData = true)
	{
		if( (Position = Get_Sorted(Position, bDown, bCheckNoData)) >= 0 )
		{
			x	= (int)(Position % Get_NX());
			y	= (int)(Position / Get_NX());

			return( true );
		}

		return( false );
	}


	//-----------------------------------------------------
	// No Data Value...

	virtual bool					is_NoData		(int x, int y)	const	{	return( is_NoData_Value(asDouble(x, y, false)) );	}
	virtual bool					is_NoData		(sLong      i)	const	{	return( is_NoData_Value(asDouble(   i, false)) );	}

	virtual void					Set_NoData		(int x, int y)	{	Set_Value(x, y, Get_NoData_Value(), false);	}
	virtual void					Set_NoData		(sLong      i)	{	Set_Value(   i, Get_NoData_Value(), false);	}


	//-----------------------------------------------------
	// Operators...

	virtual CSG_Grid &				operator  =		(const CSG_Grid &Grid);
	virtual CSG_Grid &				operator  =		(double Value);

	virtual CSG_Grid				operator +		(const CSG_Grid &Grid)	const;
	virtual CSG_Grid				operator +		(double Value)			const;
	virtual CSG_Grid &				operator +=		(const CSG_Grid &Grid);
	virtual CSG_Grid &				operator +=		(double Value);
	virtual CSG_Grid &				Add				(const CSG_Grid &Grid);
	virtual CSG_Grid &				Add				(double Value);

	virtual CSG_Grid				operator -		(const CSG_Grid &Grid)	const;
	virtual CSG_Grid				operator -		(double Value)			const;
	virtual CSG_Grid &				operator -=		(const CSG_Grid &Grid);
	virtual CSG_Grid &				operator -=		(double Value);
	virtual CSG_Grid &				Subtract		(const CSG_Grid &Grid);
	virtual CSG_Grid &				Subtract		(double Value);

	virtual CSG_Grid				operator *		(const CSG_Grid &Grid)	const;
	virtual CSG_Grid				operator *		(double Value)			const;
	virtual CSG_Grid &				operator *=		(const CSG_Grid &Grid);
	virtual CSG_Grid &				operator *=		(double Value);
	virtual CSG_Grid &				Multiply		(const CSG_Grid &Grid);
	virtual CSG_Grid &				Multiply		(double Value);

	virtual CSG_Grid				operator /		(const CSG_Grid &Grid)	const;
	virtual CSG_Grid				operator /		(double Value)			const;
	virtual CSG_Grid &				operator /=		(const CSG_Grid &Grid);
	virtual CSG_Grid &				operator /=		(double Value);
	virtual CSG_Grid &				Divide			(const CSG_Grid &Grid);
	virtual CSG_Grid &				Divide			(double Value);

	virtual double					operator ()		(int x, int y) const	{	return( asDouble(x, y) );	}


	//-----------------------------------------------------
	// Get Value...

	double							Get_Value	(double x, double y,                TSG_Grid_Resampling Resampling = GRID_RESAMPLING_BSpline                      , bool bByteWise = false) const;
	double							Get_Value	(const TSG_Point &p,                TSG_Grid_Resampling Resampling = GRID_RESAMPLING_BSpline                      , bool bByteWise = false) const;
	bool							Get_Value	(double x, double y, double &Value, TSG_Grid_Resampling Resampling = GRID_RESAMPLING_BSpline, bool bNoData = false, bool bByteWise = false) const;
	bool							Get_Value	(const TSG_Point &p, double &Value, TSG_Grid_Resampling Resampling = GRID_RESAMPLING_BSpline, bool bNoData = false, bool bByteWise = false) const;

	virtual BYTE					asByte		(int x, int y, bool bScaled = true) const	{	return( SG_ROUND_TO_BYTE (asDouble(x, y, bScaled)) );	}
	virtual BYTE					asByte		(     sLong i, bool bScaled = true) const	{	return( SG_ROUND_TO_BYTE (asDouble(   i, bScaled)) );	}
	virtual char					asChar		(int x, int y, bool bScaled = true) const	{	return( SG_ROUND_TO_CHAR (asDouble(x, y, bScaled)) );	}
	virtual char					asChar		(     sLong i, bool bScaled = true) const	{	return( SG_ROUND_TO_CHAR (asDouble(   i, bScaled)) );	}
	virtual short					asShort		(int x, int y, bool bScaled = true) const	{	return( SG_ROUND_TO_SHORT(asDouble(x, y, bScaled)) );	}
	virtual short					asShort		(     sLong i, bool bScaled = true) const	{	return( SG_ROUND_TO_SHORT(asDouble(   i, bScaled)) );	}
	virtual int						asInt		(int x, int y, bool bScaled = true) const	{	return( SG_ROUND_TO_INT  (asDouble(x, y, bScaled)) );	}
	virtual int						asInt		(     sLong i, bool bScaled = true) const	{	return( SG_ROUND_TO_INT  (asDouble(   i, bScaled)) );	}
	virtual sLong					asLong		(int x, int y, bool bScaled = true) const	{	return( SG_ROUND_TO_SLONG(asDouble(x, y, bScaled)) );	}
	virtual sLong					asLong		(     sLong i, bool bScaled = true) const	{	return( SG_ROUND_TO_SLONG(asDouble(   i, bScaled)) );	}
	virtual float					asFloat		(int x, int y, bool bScaled = true) const	{	return( (float)          (asDouble(x, y, bScaled)) );	}
	virtual float					asFloat		(     sLong i, bool bScaled = true) const	{	return( (float)          (asDouble(   i, bScaled)) );	}

	//-----------------------------------------------------
	virtual double					asDouble(     sLong i, bool bScaled = true) const
	{
		return( asDouble((int)(i % Get_NX()), (int)(i / Get_NX()), bScaled) );
	}

	virtual double					asDouble(int x, int y, bool bScaled = true) const
	{
		double	Value;

		if( is_Cached() )
		{
			Value	= _Cache_Get_Value(x, y);
		}
		else switch( m_Type )
		{
			case SG_DATATYPE_Float : Value = (double)((float  **)m_Values)[y][x]; break;
			case SG_DATATYPE_Double: Value = (double)((double **)m_Values)[y][x]; break;
			case SG_DATATYPE_Byte  : Value = (double)((BYTE   **)m_Values)[y][x]; break;
			case SG_DATATYPE_Char  : Value = (double)((char   **)m_Values)[y][x]; break;
			case SG_DATATYPE_Word  : Value = (double)((WORD   **)m_Values)[y][x]; break;
			case SG_DATATYPE_Short : Value = (double)((short  **)m_Values)[y][x]; break;
			case SG_DATATYPE_DWord : Value = (double)((DWORD  **)m_Values)[y][x]; break;
			case SG_DATATYPE_Int   : Value = (double)((int    **)m_Values)[y][x]; break;
			case SG_DATATYPE_Long  : Value = (double)((sLong  **)m_Values)[y][x]; break;
            case SG_DATATYPE_ULong : Value = (double)((uLong  **)m_Values)[y][x]; break;
			case SG_DATATYPE_Bit   : Value = (double)(((BYTE  **)m_Values)[y][x / 8] & m_Bitmask[x % 8]) == 0 ? 0.0 : 1.0;	break;

			default:
				return( 0.0 );
		}

		if( bScaled && is_Scaled() )
		{
			Value	= m_zOffset + m_zScale * Value;
		}

		return( Value );
	}


	//-----------------------------------------------------
	// Set Value...

	virtual void					Add_Value(int x, int y, double Value)	{	Set_Value(x, y, asDouble(x, y) + Value );	}
	virtual void					Add_Value(sLong      i, double Value)	{	Set_Value(   i, asDouble(   i) + Value );	}

	virtual void					Mul_Value(int x, int y, double Value)	{	Set_Value(x, y, asDouble(x, y) * Value );	}
	virtual void					Mul_Value(sLong      i, double Value)	{	Set_Value(   i, asDouble(   i) * Value );	}

	//-----------------------------------------------------
	virtual void					Set_Value(sLong      i, double Value, bool bScaled = true)
	{
		Set_Value((int)(i % Get_NX()), (int)(i / Get_NX()), Value, bScaled);
	}

	virtual void					Set_Value(int x, int y, double Value, bool bScaled = true)
	{
		if( bScaled && is_Scaled() )
		{
			Value	= (Value - m_zOffset) / m_zScale;
		}

		if( is_Cached() )
		{
			_Cache_Set_Value(x, y, Value);
		}
		else switch( m_Type )
		{
			case SG_DATATYPE_Float : ((float  **)m_Values)[y][x] = (float          )(Value); break;
			case SG_DATATYPE_Double: ((double **)m_Values)[y][x] = (double         )(Value); break;
			case SG_DATATYPE_Byte  : ((BYTE   **)m_Values)[y][x] = SG_ROUND_TO_BYTE (Value); break;
			case SG_DATATYPE_Char  : ((char   **)m_Values)[y][x] = SG_ROUND_TO_CHAR (Value); break;
			case SG_DATATYPE_Word  : ((WORD   **)m_Values)[y][x] = SG_ROUND_TO_WORD (Value); break;
			case SG_DATATYPE_Short : ((short  **)m_Values)[y][x] = SG_ROUND_TO_SHORT(Value); break;
			case SG_DATATYPE_DWord : ((DWORD  **)m_Values)[y][x] = SG_ROUND_TO_DWORD(Value); break;
			case SG_DATATYPE_Int   : ((int    **)m_Values)[y][x] = SG_ROUND_TO_INT  (Value); break;
			case SG_DATATYPE_Long  : ((sLong  **)m_Values)[y][x] = SG_ROUND_TO_SLONG(Value); break;
			case SG_DATATYPE_ULong : ((uLong  **)m_Values)[y][x] = SG_ROUND_TO_ULONG(Value); break;
			case SG_DATATYPE_Bit   : ((BYTE   **)m_Values)[y][x / 8] = Value != 0.0
					? ((BYTE  **)m_Values)[y][x / 8] |   m_Bitmask[x % 8]
					: ((BYTE  **)m_Values)[y][x / 8] & (~m_Bitmask[x % 8]);
				break;

			default:
				return;
		}

		Set_Modified();
	}

	//-----------------------------------------------------
	CSG_Vector					Get_Row					(int y)	const;
	bool						Set_Row					(int y, const CSG_Vector &Values);


//---------------------------------------------------------
protected:	///////////////////////////////////////////////

	virtual bool				On_Update				(void);
	virtual bool				On_Reload				(void);
	virtual bool				On_Delete				(void);


//---------------------------------------------------------
private:	///////////////////////////////////////////////

	void						**m_Values;

	bool						m_Cache_bTemp, m_Cache_bSwap, m_Cache_bFlip;

	size_t						m_nBytes_Value, m_nBytes_Line;

	sLong						*m_Index, m_Cache_Offset;

	double						m_zOffset, m_zScale;

	FILE						*m_Cache_Stream;

	TSG_Data_Type				m_Type;

	CSG_String					m_Unit, m_Cache_File;

	CSG_Simple_Statistics		m_Statistics;

	CSG_Histogram				m_Histogram;

	CSG_Grid_System				m_System;


	//-----------------------------------------------------
	static	BYTE				m_Bitmask[8];


	//-----------------------------------------------------
	void						_On_Construction		(void);

	void						_Set_Properties			(TSG_Data_Type Type, int NX, int NY, double Cellsize, double xMin, double yMin);

	bool						_Set_Index				(void);
	bool						_Get_Index				(void)
	{
		if( Get_Update_Flag() )
		{
			Update();
		}

		return( m_Index || _Set_Index() );
	}


	//-----------------------------------------------------
	// Memory handling...

	bool						_Memory_Create			(bool bCached);
	void						_Memory_Destroy			(void);

	bool						_Array_Create			(void);
	void						_Array_Destroy			(void);

	bool						_Cache_Check			(void);
	bool						_Cache_Create			(const CSG_String &File, TSG_Data_Type Data_Type, sLong Offset, bool bSwap, bool bFlip);
	bool						_Cache_Create			(void);
	bool						_Cache_Destroy			(bool bMemory_Restore);
	void						_Cache_Set_Value		(int x, int y, double Value);
	double						_Cache_Get_Value		(int x, int y)	const;


	//-----------------------------------------------------
	// File access...

	void						_Swap_Bytes				(char *Bytes, int nBytes)	const;

	bool						_Load_External			(const CSG_String &FileName, bool bCached, bool bLoadData);
	bool						_Load_PGSQL				(const CSG_String &FileName, bool bCached, bool bLoadData);

	bool						_Load_Native			(const CSG_String &FileName, bool bCached, bool bLoadData);
	bool						_Save_Native			(const CSG_String &FileName, TSG_Grid_File_Format Format);

	bool						_Load_Compressed		(const CSG_String &FileName, bool bCached, bool bLoadData);
	bool						_Save_Compressed		(const CSG_String &FileName);

	bool						_Load_Binary			(CSG_File &Stream, TSG_Data_Type File_Type, bool bFlip, bool bSwapBytes);
	bool						_Save_Binary			(CSG_File &Stream, TSG_Data_Type File_Type, bool bFlip, bool bSwapBytes);
	bool						_Load_ASCII				(CSG_File &Stream, bool bCached, bool bFlip = false);
	bool						_Save_ASCII				(CSG_File &Stream, bool bFlip = false);

	bool						_Load_Surfer			(const CSG_String &FileName, bool bCached, bool bLoadData);


	//-----------------------------------------------------
	CSG_Grid &					_Operation_Arithmetic	(const CSG_Grid &Grid, TSG_Grid_Operation Operation);
	CSG_Grid &					_Operation_Arithmetic	(double Value        , TSG_Grid_Operation Operation);

	bool						_Assign_Interpolated	(CSG_Grid *pSource, TSG_Grid_Resampling Interpolation);
	bool						_Assign_MeanValue		(CSG_Grid *pSource, bool bAreaProportional);
	bool						_Assign_ExtremeValue	(CSG_Grid *pSource, bool bMaximum);
	bool						_Assign_Majority		(CSG_Grid *pSource);


	//-----------------------------------------------------
	// Interpolation subroutines...

	bool						_Get_ValAtPos_NearestNeighbour	(double &Value, int x, int y, double dx, double dy                )	const;
	bool						_Get_ValAtPos_BiLinear			(double &Value, int x, int y, double dx, double dy, bool bByteWise)	const;
	bool						_Get_ValAtPos_BiCubicSpline		(double &Value, int x, int y, double dx, double dy, bool bByteWise)	const;
	bool						_Get_ValAtPos_BSpline			(double &Value, int x, int y, double dx, double dy, bool bByteWise)	const;

	double						_Get_ValAtPos_BiCubicSpline		(double dx, double dy, double z_xy[4][4])	const;
	double						_Get_ValAtPos_BSpline			(double dx, double dy, double z_xy[4][4])	const;

	bool						_Get_ValAtPos_Fill4x4Submatrix	(int x, int y, double z_xy[4][4]   )	const;
	bool						_Get_ValAtPos_Fill4x4Submatrix	(int x, int y, double z_xy[4][4][4])	const;

};


///////////////////////////////////////////////////////////
//														 //
//						Functions						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define SG_GRID_PTR_SAFE_SET_NODATA(g, x, y)	{ if( g && g->is_InGrid(x, y, false) ) { g->Set_NoData(x, y   ); } }
#define SG_GRID_PTR_SAFE_SET_VALUE(g, x, y, z)	{ if( g && g->is_InGrid(x, y, false) ) { g->Set_Value (x, y, z); } }

//---------------------------------------------------------
/** Safe grid construction */
SAGA_API_DLL_EXPORT CSG_Grid *		SG_Create_Grid		(void);

/** Safe grid construction */
SAGA_API_DLL_EXPORT CSG_Grid *		SG_Create_Grid		(const CSG_Grid &Grid);

/** Safe grid construction */
SAGA_API_DLL_EXPORT CSG_Grid *		SG_Create_Grid		(const CSG_String &FileName,    TSG_Data_Type Type = SG_DATATYPE_Undefined, bool bCached = false, bool bLoadData = true);

/** Safe grid construction */
SAGA_API_DLL_EXPORT CSG_Grid *		SG_Create_Grid		(CSG_Grid *pGrid,               TSG_Data_Type Type = SG_DATATYPE_Undefined, bool bCached = false);

/** Safe grid construction */
SAGA_API_DLL_EXPORT CSG_Grid *		SG_Create_Grid		(const CSG_Grid_System &System, TSG_Data_Type Type = SG_DATATYPE_Undefined, bool bCached = false);

/** Safe grid construction */
SAGA_API_DLL_EXPORT CSG_Grid *		SG_Create_Grid		(TSG_Data_Type Type, int NX, int NY, double Cellsize = 0.0, double xMin = 0.0, double yMin = 0.0, bool bCached = false);

//---------------------------------------------------------
/** Get default directory for grid caching */
SAGA_API_DLL_EXPORT const SG_Char *	SG_Grid_Cache_Get_Directory		(void);

/** Set default directory for grid caching */
SAGA_API_DLL_EXPORT void			SG_Grid_Cache_Set_Directory		(const SG_Char *Directory);

SAGA_API_DLL_EXPORT void			SG_Grid_Cache_Set_Mode			(int Mode);
SAGA_API_DLL_EXPORT int				SG_Grid_Cache_Get_Mode			(void);

SAGA_API_DLL_EXPORT void			SG_Grid_Cache_Set_Threshold		(int nBytes);
SAGA_API_DLL_EXPORT void			SG_Grid_Cache_Set_Threshold_MB	(double nMegabytes);
SAGA_API_DLL_EXPORT sLong			SG_Grid_Cache_Get_Threshold		(void);
SAGA_API_DLL_EXPORT double			SG_Grid_Cache_Get_Threshold_MB	(void);

//---------------------------------------------------------
SAGA_API_DLL_EXPORT bool					SG_Grid_Set_File_Format_Default		(int Format);
SAGA_API_DLL_EXPORT TSG_Grid_File_Format	SG_Grid_Get_File_Format_Default		(void);
SAGA_API_DLL_EXPORT CSG_String				SG_Grid_Get_File_Extension_Default	(void);


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSG_Grid_Stack : public CSG_Stack
{
public:
	CSG_Grid_Stack(void) : CSG_Stack(sizeof(TSG_Point_Int))	{}

	//-----------------------------------------------------
	virtual bool			Push			(int  x, int  y)
	{
		TSG_Point_Int	*pPoint	= (TSG_Point_Int *)Get_Record_Push();

		if( pPoint )
		{
			pPoint->x	= x;
			pPoint->y	= y;

			return( true );
		}

		return( false );
	}

	//-----------------------------------------------------
	virtual bool			Pop				(int &x, int &y)
	{
		TSG_Point_Int	*pPoint	= (TSG_Point_Int *)Get_Record_Pop();

		if( pPoint )
		{
			x	= pPoint->x;
			y	= pPoint->y;

			return( true );
		}

		return( false );
	}

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define SG_GRIDCELLADDR_PARM_SQUARE		0x01
#define SG_GRIDCELLADDR_PARM_CIRCLE		0x02
#define SG_GRIDCELLADDR_PARM_ANNULUS	0x04
#define SG_GRIDCELLADDR_PARM_SECTOR		0x08
#define SG_GRIDCELLADDR_PARM_SIZEDBL	0x10
#define SG_GRIDCELLADDR_PARM_MAPUNIT	0x20
#define SG_GRIDCELLADDR_PARM_WEIGHTING	0x40
#define SG_GRIDCELLADDR_PARM_DEFAULT	(SG_GRIDCELLADDR_PARM_SQUARE|SG_GRIDCELLADDR_PARM_CIRCLE)

//---------------------------------------------------------
class SAGA_API_DLL_EXPORT CSG_Grid_Cell_Addressor
{
public:
	CSG_Grid_Cell_Addressor(void);

	bool						Destroy				(void);

	static bool					Enable_Parameters	(class CSG_Parameters &Parameters);
	static bool					Add_Parameters		(class CSG_Parameters &Parameters, const CSG_String &Parent = "", int Style = SG_GRIDCELLADDR_PARM_DEFAULT);
	bool						Set_Parameters		(class CSG_Parameters &Parameters, int Type = 0);

	bool						Set_Square			(class CSG_Parameters &Parameters);
	bool						Set_Circle			(class CSG_Parameters &Parameters);
	bool						Set_Annulus			(class CSG_Parameters &Parameters);
	bool						Set_Sector			(class CSG_Parameters &Parameters);

	CSG_Distance_Weighting &	Get_Weighting		(void)			{	return( m_Weighting );		}

	bool						is_Square			(void)	const	{	return( m_Type == 0 );	}
	bool						is_Circle			(void)	const	{	return( m_Type == 1 );	}
	bool						is_Annulus			(void)	const	{	return( m_Type == 2 );	}
	bool						is_Sector			(void)	const	{	return( m_Type == 3 );	}

	bool						Set_Radius			(double Radius, bool bSquare = false);
	bool						Set_Square			(double Radius);
	bool						Set_Circle			(double Radius);
	bool						Set_Annulus			(double Radius_Inner, double Radius_Outer);
	bool						Set_Sector			(double Radius, double Direction, double Tolerance);

	double						Get_Radius			(bool bOuter = true)		const	{	return( bOuter ? m_Radius : m_Radius_0 );	}
	double						Get_Radius_Inner	(void)						const	{	return( m_Radius_0  );	}
	double						Get_Radius_Outer	(void)						const	{	return( m_Radius    );	}
	double						Get_Direction		(void)						const	{	return( m_Direction );	}
	double						Get_Tolerance		(void)						const	{	return( m_Tolerance );	}

	int							Get_Count			(void)						const	{	return( m_Kernel.Get_Count() );	}
	int							Get_X				(int Index, int Offset = 0)	const	{	return( Index >= 0 && Index < m_Kernel.Get_Count() ? m_Kernel[Index].asInt   (0) + Offset : Offset );	}
	int							Get_Y				(int Index, int Offset = 0)	const	{	return( Index >= 0 && Index < m_Kernel.Get_Count() ? m_Kernel[Index].asInt   (1) + Offset : Offset );	}
	double						Get_Distance		(int Index                )	const	{	return( Index >= 0 && Index < m_Kernel.Get_Count() ? m_Kernel[Index].asDouble(2)          : -1.    );	}
	double						Get_Weight			(int Index                )	const	{	return( Index >= 0 && Index < m_Kernel.Get_Count() ? m_Kernel[Index].asDouble(3)          :  0.    );	}
	bool						Get_Values			(int Index, int &x, int &y, double &Distance, double &Weight, bool bOffset = false)	const
	{
		if( Index >= 0 && Index < m_Kernel.Get_Count() )
		{
			CSG_Table_Record	&Cell	= m_Kernel[Index];

			x			= bOffset ? x + Cell.asInt(0) : Cell.asInt(0);
			y			= bOffset ? y + Cell.asInt(1) : Cell.asInt(1);
			Distance	= Cell.asDouble(2);
			Weight		= Cell.asDouble(3);

			return( true );
		}

		return( false );
	}


private:

	int							m_Type;

	double						m_Radius, m_Radius_0, m_Direction, m_Tolerance;

	CSG_Distance_Weighting		m_Weighting;

	CSG_Table					m_Kernel;


	bool						_Set_Kernel			(int Type, double Radius, double Radius_Inner, double Direction, double Tolerance);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__SAGA_API__grid_H

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
//                       grid.cpp                        //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "grid.h"
#include "data_manager.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
BYTE	CSG_Grid::m_Bitmask[8]	= { 0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80 };


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Grid * SG_Create_Grid(void)
{
	return( new CSG_Grid );
}

//---------------------------------------------------------
CSG_Grid * SG_Create_Grid(const CSG_Grid &Grid)
{
	CSG_Grid	*pGrid	= new CSG_Grid(Grid);

	if( !pGrid->is_Valid() ) { delete(pGrid); return( NULL ); } return( pGrid );
}

//---------------------------------------------------------
CSG_Grid * SG_Create_Grid(const CSG_String &FileName, TSG_Data_Type Type, bool bCached, bool bLoadData)
{
	CSG_Grid	*pGrid	= new CSG_Grid(FileName, Type, bCached, bLoadData);

	if( !pGrid->is_Valid() ) { delete(pGrid); return( NULL ); } return( pGrid );
}

//---------------------------------------------------------
CSG_Grid * SG_Create_Grid(CSG_Grid *pGrid, TSG_Data_Type Type, bool bCached)
{
	pGrid	= new CSG_Grid(pGrid, Type, bCached);

	if( !pGrid->is_Valid() ) { delete(pGrid); return( NULL ); } return( pGrid );
}

//---------------------------------------------------------
CSG_Grid * SG_Create_Grid(const CSG_Grid_System &System, TSG_Data_Type Type, bool bCached)
{
	CSG_Grid	*pGrid	= new CSG_Grid(System, Type, bCached);

	if( !pGrid->is_Valid() ) { delete(pGrid); return( NULL ); } return( pGrid );
}

//---------------------------------------------------------
CSG_Grid * SG_Create_Grid(TSG_Data_Type Type, int NX, int NY, double Cellsize, double xMin, double yMin, bool bCached)
{
	CSG_Grid	*pGrid	= new CSG_Grid(Type, NX, NY, Cellsize, xMin, yMin, bCached);

	if( !pGrid->is_Valid() ) { delete(pGrid); return( NULL ); } return( pGrid );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/**
  * Default constructor.
*/
//---------------------------------------------------------
CSG_Grid::CSG_Grid(void)
{
	_On_Construction();
}

//---------------------------------------------------------
/**
  * Copy constructor.
*/
//---------------------------------------------------------
CSG_Grid::CSG_Grid(const CSG_Grid &Grid)
{
	_On_Construction();

	Create(Grid);
}

//---------------------------------------------------------
/**
  * Create a grid from file.
*/
//---------------------------------------------------------
CSG_Grid::CSG_Grid(const CSG_String &FileName, TSG_Data_Type Type, bool bCached, bool bLoadData)
{
	_On_Construction();

	Create(FileName, Type, bCached, bLoadData);
}

//---------------------------------------------------------
/**
  * Create a grid similar to 'pGrid'.
*/
//---------------------------------------------------------
CSG_Grid::CSG_Grid(CSG_Grid *pGrid, TSG_Data_Type Type, bool bCached)
{
	_On_Construction();

	Create(pGrid, Type, bCached);
}

//---------------------------------------------------------
/**
  * Create a grid using 'System'.
*/
//---------------------------------------------------------
CSG_Grid::CSG_Grid(const CSG_Grid_System &System, TSG_Data_Type Type, bool bCached)
{
	_On_Construction();

	Create(System, Type, bCached);
}

//---------------------------------------------------------
/**
  * Create a grid with specified parameters.
  * This constructor initializes the grid's data space with 'NX' x 'NY' cells of the size indicated by 'Type'.
  * If 'DX/DY' are equal or less zero then both will be set to 1.0. 'xMin/yMin' specify the coordinates of the
  * lower left corner of the grid.
*/
//---------------------------------------------------------
CSG_Grid::CSG_Grid(TSG_Data_Type Type, int NX, int NY, double Cellsize, double xMin, double yMin, bool bCached)
{
	_On_Construction();

	Create(Type, NX, NY, Cellsize, xMin, yMin, bCached);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Grid::_On_Construction(void)
{
	m_Type			= SG_DATATYPE_Undefined;

	m_Values		= NULL;

	m_Cache_Stream	= NULL;
	m_Cache_Offset	= 0;
	m_Cache_bSwap	= false;
	m_Cache_bFlip	= false;

	m_zScale		= 1.0;
	m_zOffset		= 0.0;

	m_Index			= NULL;

	m_pOwner		= NULL;

	Set_Update_Flag();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid::Create(const CSG_Grid &Grid)
{
	if( Create(Grid.Get_System(), Grid.Get_Type()) )
	{
		Set_Name              (Grid.Get_Name       ());
		Set_Description       (Grid.Get_Description());
		Set_Unit              (Grid.Get_Unit       ());
		Set_NoData_Value_Range(Grid.Get_NoData_Value(), Grid.Get_NoData_hiValue());

		for(int y=0; y<Get_NY() && SG_UI_Process_Set_Progress(y, Get_NY()); y++)
		{
			#pragma omp parallel for
			for(int x=0; x<Get_NX(); x++)
			{
				if( Grid.is_NoData(x, y) )
				{
					Set_NoData(x, y);
				}
				else
				{
					Set_Value(x, y, Grid.asDouble(x, y, false));
				}
			}
		}

		Set_Scaling(Grid.Get_Scaling(), Grid.Get_Offset());

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Grid::Create(CSG_Grid *pGrid, TSG_Data_Type Type, bool bCached)
{
	if( pGrid && Create(Type, pGrid->Get_NX(), pGrid->Get_NY(), pGrid->Get_Cellsize(), pGrid->Get_XMin(), pGrid->Get_YMin(), bCached) )
	{
		Set_NoData_Value_Range(pGrid->Get_NoData_Value(), pGrid->Get_NoData_hiValue());

		Get_Projection()	= pGrid->Get_Projection();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Grid::Create(const CSG_Grid_System &System, TSG_Data_Type Type, bool bCached)
{
	return( Create(Type, System.Get_NX(), System.Get_NY(), System.Get_Cellsize(), System.Get_XMin(), System.Get_YMin(), bCached) );
}

//---------------------------------------------------------
bool CSG_Grid::Create(const CSG_String &FileName, TSG_Data_Type Type, bool bCached, bool bLoadData)
{
	Destroy();

	SG_UI_Msg_Add(CSG_String::Format("%s: %s...", _TL("Loading grid"), FileName.c_str()), true);

	m_Type	= Type;

	if( _Load_PGSQL     (FileName, bCached, bLoadData)
	||  _Load_Native    (FileName, bCached, bLoadData)
	||  _Load_Compressed(FileName, bCached, bLoadData)
	||  _Load_Surfer    (FileName, bCached, bLoadData)
	||  _Load_External  (FileName, bCached, bLoadData) )
	{
		Set_Max_Samples(SG_DataObject_Get_Max_Samples() > 0 ? SG_DataObject_Get_Max_Samples() : Get_NCells());

		Set_Modified(false);
		Set_Update_Flag();

		SG_UI_Process_Set_Ready();
		SG_UI_Msg_Add(_TL("okay"), false, SG_UI_MSG_STYLE_SUCCESS);

		return( true );
	}

	Destroy();

	SG_UI_Process_Set_Ready();
	SG_UI_Msg_Add(_TL("failed"), false, SG_UI_MSG_STYLE_FAILURE);

	return( false );
}

//---------------------------------------------------------
bool CSG_Grid::Create(TSG_Data_Type Type, int NX, int NY, double Cellsize, double xMin, double yMin, bool bCached)
{
	Destroy();

	_Set_Properties(Type, NX, NY, Cellsize, xMin, yMin);

	return( _Memory_Create(bCached) );
}


///////////////////////////////////////////////////////////
//														 //
//					Destruction							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/**
  * The destructor.
*/
CSG_Grid::~CSG_Grid(void)
{
	Destroy();
}

/**
  * Destroys the data space of CSG_Grid.
*/
bool CSG_Grid::Destroy(void)
{
	_Memory_Destroy();

	m_Type		= SG_DATATYPE_Undefined;

	m_zScale	= 1.0;
	m_zOffset	= 0.0;

	m_Unit		.Clear();

	m_System	.Assign(0.0, 0.0, 0.0, 0, 0);

	return( CSG_Data_Object::Destroy() );
}


///////////////////////////////////////////////////////////
//														 //
//						Header							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Grid::_Set_Properties(TSG_Data_Type Type, int NX, int NY, double Cellsize, double xMin, double yMin)
{
	m_Type	= Type;

	switch( m_Type )
	{
	case SG_DATATYPE_Bit   : Set_NoData_Value(          0.0);	break;
	case SG_DATATYPE_Byte  : Set_NoData_Value(          0.0);	break;
	case SG_DATATYPE_Char  : Set_NoData_Value(       -127.0);	break;
	case SG_DATATYPE_Word  : Set_NoData_Value(      65535.0);	break;
	case SG_DATATYPE_Short : Set_NoData_Value(     -32767.0);	break;
	case SG_DATATYPE_DWord : Set_NoData_Value( 4294967295.0);	break;
	case SG_DATATYPE_Int   : Set_NoData_Value(-2147483647.0);	break;
	case SG_DATATYPE_ULong : Set_NoData_Value( 4294967295.0);	break;
	case SG_DATATYPE_Long  : Set_NoData_Value(-2147483647.0);	break;
	case SG_DATATYPE_Float : Set_NoData_Value(     -99999.0);	break;
	case SG_DATATYPE_Double: Set_NoData_Value(     -99999.0);	break;
	case SG_DATATYPE_Color : Set_NoData_Value( 4294967295.0);	break;

	default:
	m_Type = SG_DATATYPE_Float;	Set_NoData_Value(  -99999.0);	break;
	}

	m_System.Assign(Cellsize > 0.0 ? Cellsize : 1.0, xMin, yMin, NX, NY);

	m_Statistics.Invalidate();

	m_nBytes_Value	= SG_Data_Type_Get_Size(m_Type);
	m_nBytes_Line	= m_Type == SG_DATATYPE_Bit ? 1 + Get_NX() / 8 : Get_NX() * m_nBytes_Value;

	Set_Max_Samples(SG_DataObject_Get_Max_Samples() > 0 ? SG_DataObject_Get_Max_Samples() : Get_NCells());
}

//---------------------------------------------------------
void CSG_Grid::Set_Unit(const CSG_String &Unit)
{
	m_Unit	= Unit;
}

//---------------------------------------------------------
void CSG_Grid::Set_Scaling(double Scale, double Offset)
{
	if( (Scale != m_zScale && Scale != 0.0) || Offset != m_zOffset )
	{
		if( Scale != 0.0 )
		{
			m_zScale	= Scale;
		}

		m_zOffset	= Offset;

		Set_Update_Flag();
	}
}

//---------------------------------------------------------
double CSG_Grid::Get_Scaling(void) const
{
	return( m_zScale );
}

//---------------------------------------------------------
double CSG_Grid::Get_Offset(void) const
{
	return( m_zOffset );
}


///////////////////////////////////////////////////////////
//														 //
//						Checks							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid::is_Valid(void) const
{
	if(	m_System.is_Valid() && m_Type != SG_DATATYPE_Undefined )
	{
		return( m_Values != NULL || is_Cached() );
	}

	return( false );
}

//---------------------------------------------------------
TSG_Intersection CSG_Grid::is_Intersecting(const CSG_Rect &Extent) const
{
	return( Get_Extent().Intersects(Extent.m_rect) );
}

TSG_Intersection CSG_Grid::is_Intersecting(const TSG_Rect &Extent) const
{
	return( Get_Extent().Intersects(Extent) );
}

TSG_Intersection CSG_Grid::is_Intersecting(double xMin, double yMin, double xMax, double yMax) const
{
	return( is_Intersecting(CSG_Rect(xMin, yMin, xMax, yMax)) );
}

//---------------------------------------------------------
bool CSG_Grid::is_Compatible(CSG_Grid *pGrid) const
{
	return( pGrid && is_Compatible(pGrid->Get_System()) );
}

bool CSG_Grid::is_Compatible(const CSG_Grid_System &System) const
{
	return( m_System == System );
}

bool CSG_Grid::is_Compatible(int NX, int NY, double Cellsize, double xMin, double yMin) const
{
	return(	is_Compatible(CSG_Grid_System(Cellsize, xMin, yMin, NX, NY)) );
}


///////////////////////////////////////////////////////////
//														 //
//		Value access by Position (-> Interpolation)		 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CSG_Grid::Get_Value(const TSG_Point &p, TSG_Grid_Resampling Resampling, bool bByteWise) const
{
	double	Value;

	return( Get_Value(p.x, p.y, Value, Resampling, false, bByteWise) ? Value : Get_NoData_Value() );
}

double CSG_Grid::Get_Value(double x, double y, TSG_Grid_Resampling Resampling, bool bByteWise) const
{
	double	Value;

	return( Get_Value(x, y, Value, Resampling, false, bByteWise) ? Value : Get_NoData_Value() );
}

bool CSG_Grid::Get_Value(const TSG_Point &p, double &Value, TSG_Grid_Resampling Resampling, bool bNoData, bool bByteWise) const
{
	return( Get_Value(p.x, p.y, Value, Resampling, bNoData, bByteWise) );
}

//---------------------------------------------------------
bool CSG_Grid::Get_Value(double x, double y, double &Value, TSG_Grid_Resampling Resampling, bool bNoData, bool bByteWise) const
{
	if(	m_System.Get_Extent(true).Contains(x, y) )
	{
		int	ix = (int)floor(x = (x - Get_XMin()) / Get_Cellsize()); double dx = x - ix;
		int	iy = (int)floor(y = (y - Get_YMin()) / Get_Cellsize()); double dy = y - iy;

		if( bNoData || is_InGrid(ix + (int)(0.5 + dx), iy + (int)(0.5 + dy)) )
		{
			switch( Resampling )
			{
			case GRID_RESAMPLING_NearestNeighbour: return( _Get_ValAtPos_NearestNeighbour(Value, ix, iy, dx, dy           ) );
			case GRID_RESAMPLING_Bilinear        : return( _Get_ValAtPos_BiLinear        (Value, ix, iy, dx, dy, bByteWise) );
			case GRID_RESAMPLING_BicubicSpline   : return( _Get_ValAtPos_BiCubicSpline   (Value, ix, iy, dx, dy, bByteWise) );
			case GRID_RESAMPLING_BSpline: default: return( _Get_ValAtPos_BSpline         (Value, ix, iy, dx, dy, bByteWise) );
			}
		}
	}

	Value	= Get_NoData_Value();

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline bool CSG_Grid::_Get_ValAtPos_NearestNeighbour(double &Value, int x, int y, double dx, double dy) const
{
	if( is_InGrid(x = x + (int)(0.5 + dx), y = y + (int)(0.5 + dy)) )
	{
		Value	= asDouble(x, y);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define BILINEAR_ADD(ix, iy, d)			if( is_InGrid(ix, iy) ) {\
	n += d; z += d * asDouble(ix, iy);\
}

//---------------------------------------------------------
#define BILINEAR_ADD_BYTE(ix, iy, d)	if( is_InGrid(ix, iy) ) {\
	n += d; sLong v = asInt(ix, iy);\
	z[0] += d * SG_GET_BYTE_0(v);\
	z[1] += d * SG_GET_BYTE_1(v);\
	z[2] += d * SG_GET_BYTE_2(v);\
	z[3] += d * SG_GET_BYTE_3(v);\
}

//---------------------------------------------------------
inline bool CSG_Grid::_Get_ValAtPos_BiLinear(double &Value, int x, int y, double dx, double dy, bool bByteWise) const
{
	if( !bByteWise )
	{
		double	z = 0.0, n = 0.0;

		BILINEAR_ADD(x    , y    , (1.0 - dx) * (1.0 - dy));
		BILINEAR_ADD(x + 1, y    , (      dx) * (1.0 - dy));
		BILINEAR_ADD(x    , y + 1, (1.0 - dx) * (      dy));
		BILINEAR_ADD(x + 1, y + 1, (      dx) * (      dy));

		if( n > 0.0 )
		{
			Value	= z / n;

			return( true );
		}
	}

	//-----------------------------------------------------
	else
	{
		double	n = 0.0;
		CSG_Vector	z(4);

		BILINEAR_ADD_BYTE(x    , y    , (1.0 - dx) * (1.0 - dy));
		BILINEAR_ADD_BYTE(x + 1, y    , (      dx) * (1.0 - dy));
		BILINEAR_ADD_BYTE(x    , y + 1, (1.0 - dx) * (      dy));
		BILINEAR_ADD_BYTE(x + 1, y + 1, (      dx) * (      dy));

		if( n > 0.0 )
		{
			z	*= 1. / n;

			Value	= SG_GET_LONG(z[0], z[1], z[2], z[3]);

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline double CSG_Grid::_Get_ValAtPos_BiCubicSpline(double dx, double dy, double v_xy[4][4]) const
{
	#define BiCubicSpline(d, v) (v[1] + 0.5 * d * (v[2] - v[0] + d * (2 * v[0] - 5 * v[1] + 4 * v[2] - v[3] + d * (3 * (v[1] - v[2]) + v[3] - v[0]))))

	double	v_x[4];

	for(int ix=0; ix<4; ix++)
	{
		v_x[ix]	= BiCubicSpline(dy, v_xy[ix]);
	}

	return( BiCubicSpline(dx, v_x) );
}

//---------------------------------------------------------
inline bool CSG_Grid::_Get_ValAtPos_BiCubicSpline(double &Value, int x, int y, double dx, double dy, bool bByteWise) const
{
	if( !bByteWise )
	{
		double	v_xy[4][4];

		if( _Get_ValAtPos_Fill4x4Submatrix(x, y, v_xy) )
		{
			Value	= _Get_ValAtPos_BiCubicSpline(dx, dy, v_xy);

			return( true );
		}
	}
	else
	{
		double	v_xy[4][4][4];

		if( _Get_ValAtPos_Fill4x4Submatrix(x, y, v_xy) )
		{
			Value	= SG_GET_LONG(
				_Get_ValAtPos_BiCubicSpline(dx, dy, v_xy[0]),
				_Get_ValAtPos_BiCubicSpline(dx, dy, v_xy[1]),
				_Get_ValAtPos_BiCubicSpline(dx, dy, v_xy[2]),
				_Get_ValAtPos_BiCubicSpline(dx, dy, v_xy[3])
			);

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline double CSG_Grid::_Get_ValAtPos_BSpline(double dx, double dy, double v_xy[4][4]) const
{
	double	Rx[4], Ry[4];

	for(int i=0; i<4; i++)
	{
		double	d, s;

		s	= 0.0;
		if( (d = i - dx + 1.0) > 0.0 )	s	+=        d*d*d;
		if( (d = i - dx + 0.0) > 0.0 )	s	+= -4.0 * d*d*d;
		if( (d = i - dx - 1.0) > 0.0 )	s	+=  6.0 * d*d*d;
		if( (d = i - dx - 2.0) > 0.0 )	s	+= -4.0 * d*d*d;
	//	if( (d = i - dx - 3.0) > 0.0 )	s	+=        d*d*d;
		Rx[i]	= s / 6.0;

		s	= 0.0;
		if( (d = i - dy + 1.0) > 0.0 )	s	+=        d*d*d;
		if( (d = i - dy + 0.0) > 0.0 )	s	+= -4.0 * d*d*d;
		if( (d = i - dy - 1.0) > 0.0 )	s	+=  6.0 * d*d*d;
		if( (d = i - dy - 2.0) > 0.0 )	s	+= -4.0 * d*d*d;
	//	if( (d = i - dy - 3.0) > 0.0 )	s	+=        d*d*d;
		Ry[i]	= s / 6.0;
	}

	double	z	= 0.0;

	for(int iy=0; iy<4; iy++)
	{
		for(int ix=0; ix<4; ix++)
		{
			z	+= v_xy[ix][iy] * Rx[ix] * Ry[iy];
		}
	}

	return( z );
}

//---------------------------------------------------------
inline bool CSG_Grid::_Get_ValAtPos_BSpline(double &Value, int x, int y, double dx, double dy, bool bByteWise) const
{
	if( !bByteWise )
	{
		double	v_xy[4][4];

		if( _Get_ValAtPos_Fill4x4Submatrix(x, y, v_xy) )
		{
			Value	= _Get_ValAtPos_BSpline(dx, dy, v_xy);

			return( true );
		}
	}
	else
	{
		double	v_xy[4][4][4];

		if( _Get_ValAtPos_Fill4x4Submatrix(x, y, v_xy) )
		{
			Value	= SG_GET_LONG(
				_Get_ValAtPos_BSpline(dx, dy, v_xy[0]),
				_Get_ValAtPos_BSpline(dx, dy, v_xy[1]),
				_Get_ValAtPos_BSpline(dx, dy, v_xy[2]),
				_Get_ValAtPos_BSpline(dx, dy, v_xy[3])
			);

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline bool CSG_Grid::_Get_ValAtPos_Fill4x4Submatrix(int x, int y, double v_xy[4][4]) const
{
	int		ix, iy, jx, jy, nNoData	= 0;

	//-----------------------------------------------------
	for(iy=0, jy=y-1; iy<4; iy++, jy++)
	{
		for(ix=0, jx=x-1; ix<4; ix++, jx++)
		{
			if( is_InGrid(jx, jy) )
			{
				v_xy[ix][iy]	= asDouble(jx, jy);
			}
			else
			{
				v_xy[ix][iy]	= Get_NoData_Value();

				nNoData++;
			}
		}
	}

	//-----------------------------------------------------
	for(int i=0; nNoData>0 && nNoData<16 && i<16; i++)	// guess missing values as average of surrounding data values
	{
		double	t_xy[4][4];

		for(iy=0; iy<4; iy++)	for(ix=0; ix<4; ix++)
		{
			t_xy[ix][iy]	= v_xy[ix][iy];
		}

		for(iy=0; iy<4; iy++)	for(ix=0; ix<4; ix++)
		{
			if( is_NoData_Value(t_xy[ix][iy]) )
			{
				int		n	= 0;
				double	s	= 0.0;

				for(jy=iy-1; jy<=iy+1; jy++)	for(jx=ix-1; jx<=ix+1; jx++)
				{
					if( is_InGrid(jx + x - 1, jy + y - 1) )
					{
						s	+= asDouble(jx + x - 1, jy + y - 1);
						n	++;
					}
					else if( jy >= 0 && jy < 4 && jx >= 0 && jx < 4 && !is_NoData_Value(t_xy[jx][jy]) )
					{
						s	+= t_xy[jx][jy];
						n	++;
					}
				}

				if( n > 0 )
				{
					v_xy[ix][iy]	= s / n;

					nNoData--;
				}
			}
		}
	}

	//-----------------------------------------------------
	return( nNoData == 0 );
}

//---------------------------------------------------------
inline bool CSG_Grid::_Get_ValAtPos_Fill4x4Submatrix(int x, int y, double v_xy[4][4][4]) const
{
	int		ix, iy, jx, jy, nNoData	= 0;

	//-----------------------------------------------------
	for(iy=0, jy=y-1; iy<4; iy++, jy++)
	{
		for(ix=0, jx=x-1; ix<4; ix++, jx++)
		{
			if( is_InGrid(jx, jy) )
			{
				int	z	= asInt(jx, jy);

				v_xy[0][ix][iy]	= SG_GET_BYTE_0(z);
				v_xy[1][ix][iy]	= SG_GET_BYTE_1(z);
				v_xy[2][ix][iy]	= SG_GET_BYTE_2(z);
				v_xy[3][ix][iy]	= SG_GET_BYTE_3(z);
			}
			else
			{
				v_xy[0][ix][iy]	= -1;

				nNoData++;
			}
		}
	}

	//-----------------------------------------------------
	for(int i=0; nNoData>0 && nNoData<16 && i<16; i++)	// guess missing values as average of surrounding data values
	{
		double	t_xy[4][4][4];

		for(iy=0; iy<4; iy++)	for(ix=0; ix<4; ix++)
		{
			t_xy[0][ix][iy]	= v_xy[0][ix][iy];
			t_xy[1][ix][iy]	= v_xy[1][ix][iy];
			t_xy[2][ix][iy]	= v_xy[2][ix][iy];
			t_xy[3][ix][iy]	= v_xy[3][ix][iy];
		}

		for(iy=0; iy<4; iy++)	for(ix=0; ix<4; ix++)
		{
			if( t_xy[0][ix][iy] < 0 )
			{
				int		n	= 0;
				double	s[4]; s[0] = s[1] = s[2] = s[3] = 0.0;

				for(jy=iy-1; jy<=iy+1; jy++)	for(jx=ix-1; jx<=ix+1; jx++)
				{
					if( is_InGrid(jx + x - 1, jy + y - 1) )
					{
						int	z	= asInt(jx + x - 1, jy + y - 1);

						s[0]	+= SG_GET_BYTE_0(z);
						s[1]	+= SG_GET_BYTE_1(z);
						s[2]	+= SG_GET_BYTE_2(z);
						s[3]	+= SG_GET_BYTE_3(z);
						n		++;
					}
					else if( jy >= 0 && jy < 4 && jx >= 0 && jx < 4 && !is_NoData_Value(t_xy[0][jx][jy]) )
					{
						s[0]	+= t_xy[0][jx][jy];
						s[1]	+= t_xy[1][jx][jy];
						s[2]	+= t_xy[2][jx][jy];
						s[3]	+= t_xy[3][jx][jy];
						n		++;
					}
				}

				if( n > 0 )
				{
					v_xy[0][ix][iy]	= s[0] / n;
					v_xy[1][ix][iy]	= s[1] / n;
					v_xy[2][ix][iy]	= s[2] / n;
					v_xy[3][ix][iy]	= s[3] / n;

					nNoData--;
				}
			}
		}
	}

	//-----------------------------------------------------
	return( nNoData == 0 );
}


///////////////////////////////////////////////////////////
//														 //
//						Statistics						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid::On_Update(void)
{
	if( is_Valid() )
	{
		SG_FREE_SAFE(m_Index);

		m_Statistics.Invalidate();
		m_Histogram.Destroy();

		double	Offset = Get_Offset(), Scaling = is_Scaled() ? Get_Scaling() : 0.0;

		if( Get_Max_Samples() > 0 && Get_Max_Samples() < Get_NCells() )
		{
			double	d	= (double)Get_NCells() / (double)Get_Max_Samples();

			for(double i=0; i<(double)Get_NCells(); i+=d)
			{
				double	Value	= asDouble((sLong)i, false);

				if( !is_NoData_Value(Value) )
				{
					m_Statistics	+= Scaling ? Offset + Scaling * Value : Value;
				}
			}

			m_Statistics.Set_Count(m_Statistics.Get_Count() >= Get_Max_Samples() ? Get_NCells()	// any no-data cells ?
				: (sLong)(Get_NCells() * (double)m_Statistics.Get_Count() / (double)Get_Max_Samples())
			);
		}
		else
		{
			for(int y=0; y<Get_NY() && SG_UI_Process_Set_Progress(y, Get_NY()); y++)
			{
				for(int x=0; x<Get_NX(); x++)
				{
					double	Value	= asDouble(x, y, false);

					if( !is_NoData_Value(Value) )
					{
						m_Statistics	+= Scaling ? Offset + Scaling * Value : Value;
					}
				}
			}

			SG_UI_Process_Set_Ready();
		}
	}

	return( true );
}

//---------------------------------------------------------
double CSG_Grid::Get_Mean(void)
{
	Update();	return( m_Statistics.Get_Mean() );
}

double CSG_Grid::Get_Min(void)
{
	Update();	return( m_Statistics.Get_Minimum() );
}

double CSG_Grid::Get_Max(void)
{
	Update();	return( m_Statistics.Get_Maximum() );
}

double CSG_Grid::Get_Range(void)
{
	Update();	return( m_Statistics.Get_Range() );
}

double CSG_Grid::Get_StdDev(void)
{
	Update();	return( m_Statistics.Get_StdDev() );
}

double CSG_Grid::Get_Variance(void)
{
	Update();	return( m_Statistics.Get_Variance() );
}

//---------------------------------------------------------
sLong CSG_Grid::Get_Data_Count(void)
{
	Update();	return( m_Statistics.Get_Count() );
}

sLong CSG_Grid::Get_NoData_Count(void)
{
	Update();	return( Get_NCells() - m_Statistics.Get_Count() );
}

//---------------------------------------------------------
double CSG_Grid::Get_Quantile(double Quantile, bool bFromHistogram)
{
	if( Quantile <= 0. ) { return( Get_Min() ); }
	if( Quantile >= 1. ) { return( Get_Max() ); }

	if( bFromHistogram )
	{
		return( Get_Histogram().Get_Quantile(Quantile) );
	}
	else
	{
		sLong	n	= (sLong)(Quantile * (Get_Data_Count() - 1));

		if( Get_Sorted(n, n, false) )
		{
			return( asDouble(n) );
		}
	}

	return( Get_NoData_Value() );
}

//---------------------------------------------------------
double CSG_Grid::Get_Percentile(double Percentile, bool bFromHistogram)
{
	return( Get_Quantile(0.01 * Percentile, bFromHistogram) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
/**
  * Returns the statistics for the whole data set. It is
  * automatically updated if necessary. Statistics give no
  * access to parameters like quantiles that need values
  * to be kept internally. Use Get_Quantile() function instead.
*/
const CSG_Simple_Statistics & CSG_Grid::Get_Statistics(void)
{
	Update();	return( m_Statistics );
}

//---------------------------------------------------------
/**
  * Calculate statistics for the region specified with rWorld.
  * Returns false, if there is no overlapping. Set bHoldValues
  * to true, if you need to obtain quantiles.
*/
//---------------------------------------------------------
bool CSG_Grid::Get_Statistics(const CSG_Rect &rWorld, CSG_Simple_Statistics &Statistics, bool bHoldValues) const
{
	int	xMin	= Get_System().Get_xWorld_to_Grid(rWorld.Get_XMin()); if( xMin <  0        ) xMin = 0;
	int	yMin	= Get_System().Get_yWorld_to_Grid(rWorld.Get_YMin()); if( yMin <  0        ) yMin = 0;
	int	xMax	= Get_System().Get_xWorld_to_Grid(rWorld.Get_XMax()); if( xMax >= Get_NX() ) xMax = Get_NX() - 1;
	int	yMax	= Get_System().Get_yWorld_to_Grid(rWorld.Get_YMax()); if( yMax >= Get_NY() ) yMax = Get_NY() - 1;

	if( xMin > xMax || yMin > yMax )
	{
		return( false );	// no overlap
	}

	Statistics.Create(bHoldValues);

	int		nx		= 1 + (xMax - xMin);
	int		ny		= 1 + (yMax - yMin);
	sLong	nCells	= nx * ny;

	double	Offset = Get_Offset(), Scaling = is_Scaled() ? Get_Scaling() : 0.0;

	if( Get_Max_Samples() > 0 && Get_Max_Samples() < nCells )
	{
		double	d = (double)nCells / (double)Get_Max_Samples();

		for(double i=0; i<(double)nCells; i+=d)
		{
			int	y	= yMin + (int)i / nx;
			int	x	= xMin + (int)i % nx;

			double	Value	= asDouble(x, y, false);

			if( !is_NoData_Value(Value) )
			{
				Statistics	+= Scaling ? Offset + Scaling * Value : Value;
			}
		}
	}
	else
	{
		for(int y=yMin; y<=yMax; y++)
		{
			for(int x=xMin; x<=xMax; x++)
			{
				double	Value	= asDouble(x, y, false);

				if( !is_NoData_Value(Value) )
				{
					Statistics	+= Scaling ? Offset + Scaling * Value : Value;
				}
			}
		}
	}

	return( Statistics.Get_Count() > 0 );
}

//---------------------------------------------------------
#define SG_GRID_HISTOGRAM_CLASSES_DEFAULT	255

//---------------------------------------------------------
/**
* Returns the histogram for the whole data set. It is
* automatically updated if necessary.
*/
const CSG_Histogram & CSG_Grid::Get_Histogram(size_t nClasses)
{
	if( nClasses > 1 && nClasses != m_Histogram.Get_Class_Count() )
	{
		m_Histogram.Destroy();
	}

	if( m_Histogram.Get_Statistics().Get_Count() < 1 )
	{
		m_Histogram.Create(nClasses > 1 ? nClasses : SG_GRID_HISTOGRAM_CLASSES_DEFAULT, Get_Min(), Get_Max(), this, (size_t)Get_Max_Samples());
	}

	return( m_Histogram );
}

//---------------------------------------------------------
bool CSG_Grid::Get_Histogram(const CSG_Rect &rWorld, CSG_Histogram &Histogram, size_t nClasses)	const
{
	CSG_Simple_Statistics	Statistics;

	if( !Get_Statistics(rWorld, Statistics) )
	{
		return( false );
	}

	int	xMin	= Get_System().Get_xWorld_to_Grid(rWorld.Get_XMin()); if( xMin <  0        ) xMin = 0;
	int	yMin	= Get_System().Get_yWorld_to_Grid(rWorld.Get_YMin()); if( yMin <  0        ) yMin = 0;
	int	xMax	= Get_System().Get_xWorld_to_Grid(rWorld.Get_XMax()); if( xMax >= Get_NX() ) xMax = Get_NX() - 1;
	int	yMax	= Get_System().Get_yWorld_to_Grid(rWorld.Get_YMax()); if( yMax >= Get_NY() ) yMax = Get_NY() - 1;

	if( xMin > xMax || yMin > yMax )
	{
		return( false );	// no overlap
	}

	Histogram.Create(nClasses > 1 ? nClasses : SG_GRID_HISTOGRAM_CLASSES_DEFAULT, Statistics.Get_Minimum(), Statistics.Get_Maximum());

	int		nx		= 1 + (xMax - xMin);
	int		ny		= 1 + (yMax - yMin);
	sLong	nCells	= nx * ny;

	double	Offset = Get_Offset(), Scaling = is_Scaled() ? Get_Scaling() : 0.0;

	if( Get_Max_Samples() > 0 && Get_Max_Samples() < nCells )
	{
		double	d = (double)nCells / (double)Get_Max_Samples();

		for(double i=0; i<(double)nCells; i+=d)
		{
			int	y	= yMin + (int)i / nx;
			int	x	= xMin + (int)i % nx;

			double	Value	= asDouble(x, y, false);

			if( !is_NoData_Value(Value) )
			{
				Histogram	+= Scaling ? Offset + Scaling * Value : Value;
			}
		}
	}
	else
	{
		for(int y=yMin; y<=yMax; y++)
		{
			for(int x=xMin; x<=xMax; x++)
			{
				double	Value	= asDouble(x, y, false);

				if( !is_NoData_Value(Value) )
				{
					Histogram	+= Scaling ? Offset + Scaling * Value : Value;
				}
			}
		}
	}

	return( Histogram.Update() );
}


///////////////////////////////////////////////////////////
//														 //
//						Index							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid::_Set_Index(void)
{
	//-----------------------------------------------------
	if( m_Index == NULL && (m_Index = (sLong *)SG_Malloc((size_t)Get_NCells() * sizeof(sLong))) == NULL )
	{
		SG_UI_Msg_Add_Error(_TL("could not create index: insufficient memory"));

		return( false );
	}

	//-----------------------------------------------------
	const sLong	M	= 7;

	sLong	i, j, k, l, ir, n, *istack, jstack, nstack, indxt, itemp, nData;
	double	a;

	//-----------------------------------------------------
	SG_UI_Process_Set_Text(CSG_String::Format("%s: %s", _TL("Create index"), Get_Name()));

	for(i=0, j=0, nData=Get_NCells(); i<Get_NCells(); i++)
	{
		if( is_NoData(i) )
		{
			m_Index[--nData]	= i;
		}
		else // if( !is_NoData(i) )
		{
			m_Index[j++]	= i;
		}
	}

	if( nData <= 0 )
	{
	//	SG_FREE_SAFE(m_Index);

		return( false );	// nothing to do
	}

	//-----------------------------------------------------
	l		= 0;
	n		= 0;
	ir		= nData - 1;

	nstack	= 64;
	istack	= (sLong *)SG_Malloc((size_t)nstack * sizeof(sLong));
	jstack	= 0;

	for(;;)
	{
		if( ir - l < M )
		{
			if( !SG_UI_Process_Set_Progress((double)(n += M - 1), (double)nData) )
			{
				SG_FREE_SAFE(istack);
				SG_FREE_SAFE(m_Index);

				SG_UI_Msg_Add_Error(_TL("index creation stopped by user"));
				SG_UI_Process_Set_Ready();

				return( false );
			}

			for(j=l+1; j<=ir; j++)
			{
				indxt	= m_Index[j];
				a		= asDouble(indxt);

				for(i=j-1; i>=0; i--)
				{
					if( asDouble(m_Index[i]) <= a )
					{
						break;
					}

					m_Index[i + 1]	= m_Index[i];
				}

				m_Index[i + 1]	= indxt;
			}

			if( jstack == 0 )
			{
				break;
			}

			ir		= istack[jstack--];
			l		= istack[jstack--];
		}

		//-------------------------------------------------
		else
		{
			#define SORT_SWAP(a,b)	{itemp=(a);(a)=(b);(b)=itemp;}

			k		= (l + ir) >> 1;

			SORT_SWAP(m_Index[k], m_Index[l + 1]);

			if( asDouble( m_Index[l + 1]) > asDouble(m_Index[ir]) )
				SORT_SWAP(m_Index[l + 1],            m_Index[ir]);

			if( asDouble( m_Index[l    ]) > asDouble(m_Index[ir]) )
				SORT_SWAP(m_Index[l    ],            m_Index[ir]);

			if( asDouble( m_Index[l + 1]) > asDouble(m_Index[l ]) )
				SORT_SWAP(m_Index[l + 1],            m_Index[l ]);

			i		= l + 1;
			j		= ir;
			indxt	= m_Index[l];
			a		= asDouble(indxt);

			for(;;)
			{
				do	i++;	while(asDouble(m_Index[i]) < a);
				do	j--;	while(asDouble(m_Index[j]) > a);

				if( j < i )
				{
					break;
				}

				SORT_SWAP(m_Index[i], m_Index[j]);
			}

			#undef SORT_SWAP

			m_Index[l]	= m_Index[j];
			m_Index[j]	= indxt;
			jstack		+= 2;

			if( jstack >= nstack )
			{
				nstack	+= 64;
				istack	= (sLong *)SG_Realloc(istack, (size_t)nstack * sizeof(int));
			}

			if( ir - i + 1 >= j - l )
			{
				istack[jstack]		= ir;
				istack[jstack - 1]	= i;
				ir					= j - 1;
			}
			else
			{
				istack[jstack]		= j - 1;
				istack[jstack - 1]	= l;
				l					= i;
			}
		}
	}

	//-----------------------------------------------------
	SG_Free(istack);

	SG_UI_Process_Set_Ready();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

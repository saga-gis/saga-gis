
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
#include "grid.h"


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
	return( new CSG_Grid(Grid) );
}

//---------------------------------------------------------
CSG_Grid * SG_Create_Grid(const CSG_String &File_Name, TSG_Data_Type Type, TSG_Grid_Memory_Type Memory_Type)
{
	return( new CSG_Grid(File_Name, Type, Memory_Type) );
}

//---------------------------------------------------------
CSG_Grid * SG_Create_Grid(CSG_Grid *pGrid, TSG_Data_Type Type, TSG_Grid_Memory_Type Memory_Type)
{
	return( new CSG_Grid(pGrid, Type, Memory_Type) );
}

//---------------------------------------------------------
CSG_Grid * SG_Create_Grid(const CSG_Grid_System &System, TSG_Data_Type Type, TSG_Grid_Memory_Type Memory_Type)
{
	return( new CSG_Grid(System, Type, Memory_Type) );
}

//---------------------------------------------------------
CSG_Grid * SG_Create_Grid(TSG_Data_Type Type, int NX, int NY, double Cellsize, double xMin, double yMin, TSG_Grid_Memory_Type Memory_Type)
{
	return( new CSG_Grid(Type, NX, NY, Cellsize, xMin, yMin, Memory_Type) );
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
	: CSG_Data_Object()
{
	_On_Construction();
}

//---------------------------------------------------------
/**
  * Copy constructor.
*/
//---------------------------------------------------------
CSG_Grid::CSG_Grid(const CSG_Grid &Grid)
	: CSG_Data_Object()
{
	_On_Construction();

	Create(Grid);
}

//---------------------------------------------------------
/**
  * Create a grid from file.
*/
//---------------------------------------------------------
CSG_Grid::CSG_Grid(const CSG_String &File_Name, TSG_Data_Type Type, TSG_Grid_Memory_Type Memory_Type)
	: CSG_Data_Object()
{
	_On_Construction();

	Create(File_Name, Type, Memory_Type);
}

//---------------------------------------------------------
/**
  * Create a grid similar to 'pGrid'.
*/
//---------------------------------------------------------
CSG_Grid::CSG_Grid(CSG_Grid *pGrid, TSG_Data_Type Type, TSG_Grid_Memory_Type Memory_Type)
	: CSG_Data_Object()
{
	_On_Construction();

	Create(pGrid, Type, Memory_Type);
}

//---------------------------------------------------------
/**
  * Create a grid using 'System'.
*/
//---------------------------------------------------------
CSG_Grid::CSG_Grid(const CSG_Grid_System &System, TSG_Data_Type Type, TSG_Grid_Memory_Type Memory_Type)
	: CSG_Data_Object()
{
	_On_Construction();

	Create(System, Type, Memory_Type);
}

//---------------------------------------------------------
/**
  * Create a grid with specified parameters.
  * This constructor initializes the grid's data space with 'NX' x 'NY' cells of the size indicated by 'Type'.
  * If 'DX/DY' are equal or less zero then both will be set to 1.0. 'xMin/yMin' specify the coordinates of the
  * lower left corner of the grid.
*/
//---------------------------------------------------------
CSG_Grid::CSG_Grid(TSG_Data_Type Type, int NX, int NY, double Cellsize, double xMin, double yMin, TSG_Grid_Memory_Type Memory_Type)
	: CSG_Data_Object()
{
	_On_Construction();

	Create(Type, NX, NY, Cellsize, xMin, yMin, Memory_Type);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Grid::_On_Construction(void)
{
	m_bCreated			= false;

	m_Type				= SG_DATATYPE_Undefined;
	m_Memory_Type		= GRID_MEMORY_Normal;
	m_Memory_bLock		= false;

	m_Values			= NULL;

	LineBuffer			= NULL;
	LineBuffer_Count	= 5;

	m_zFactor			= 1.0;

	m_bIndexed			= false;
	m_Index				= NULL;

	Set_Update_Flag();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid::Create(const CSG_Grid &Grid)
{
	if( Create((CSG_Grid *)&Grid, ((CSG_Grid *)&Grid)->Get_Type()) )
	{
		return( Assign((CSG_Data_Object *)&Grid) );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Grid::Create(CSG_Grid *pGrid, TSG_Data_Type Type, TSG_Grid_Memory_Type Memory_Type)
{
	return( Create(Type, pGrid->Get_NX(), pGrid->Get_NY(), pGrid->Get_Cellsize(), pGrid->Get_XMin(), pGrid->Get_YMin(), Memory_Type) );
}

//---------------------------------------------------------
bool CSG_Grid::Create(const CSG_Grid_System &System, TSG_Data_Type Type, TSG_Grid_Memory_Type Memory_Type)
{
	return( Create(Type, System.Get_NX(), System.Get_NY(), System.Get_Cellsize(), System.Get_XMin(), System.Get_YMin(), Memory_Type) );
}

//---------------------------------------------------------
bool CSG_Grid::Create(const CSG_String &File_Name, TSG_Data_Type Type, TSG_Grid_Memory_Type Memory_Type)
{
	return( _Load(File_Name, Type, Memory_Type) );
}

//---------------------------------------------------------
bool CSG_Grid::Create(TSG_Data_Type Type, int NX, int NY, double Cellsize, double xMin, double yMin, TSG_Grid_Memory_Type Memory_Type)
{
	Destroy();

	_Set_Properties(Type, NX, NY, Cellsize, xMin, yMin);

	if( _Memory_Create(Memory_Type) )
	{
		m_bCreated	= true;
	}

	return( m_bCreated );
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

	m_bCreated		= false;

	m_Type			= SG_DATATYPE_Undefined;
	m_Memory_Type	= GRID_MEMORY_Normal;

	m_zFactor		= 1.0;

	m_Description	.Clear();
	m_Unit			.Clear();

	m_System		.Assign(0.0, 0.0, 0.0, 0, 0);

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
	case SG_DATATYPE_Bit:		Set_NoData_Value(          0.0);	break;
	case SG_DATATYPE_Byte:		Set_NoData_Value(        255.0);	break;
	case SG_DATATYPE_Char:		Set_NoData_Value(       -127.0);	break;
	case SG_DATATYPE_Word:		Set_NoData_Value(      65535.0);	break;
	case SG_DATATYPE_Short:		Set_NoData_Value(     -32767.0);	break;
	case SG_DATATYPE_DWord:		Set_NoData_Value( 4294967295.0);	break;
	case SG_DATATYPE_Int:		Set_NoData_Value(-2147483647.0);	break;
	case SG_DATATYPE_ULong:		Set_NoData_Value( 4294967295.0);	break;
	case SG_DATATYPE_Long:		Set_NoData_Value(-2147483647.0);	break;
	case SG_DATATYPE_Float:		Set_NoData_Value(     -99999.0);	break;
	case SG_DATATYPE_Double:	Set_NoData_Value(     -99999.0);	break;
	case SG_DATATYPE_Color:		Set_NoData_Value( 4294967295.0);	break;

	default:
	m_Type = SG_DATATYPE_Float;	Set_NoData_Value(      -99999.0);	break;
	}

	m_System.Assign(Cellsize > 0.0 ? Cellsize : 1.0, xMin, yMin, NX, NY);

	m_zStats.Invalidate();

}

//---------------------------------------------------------
void CSG_Grid::Set_Description(const SG_Char *String)
{
	m_Description	= String ? String : SG_T("");
}

const SG_Char * CSG_Grid::Get_Description(void) const
{
	return( m_Description.c_str() );
}

//---------------------------------------------------------
void CSG_Grid::Set_Unit(const SG_Char *String)
{
	m_Unit	= String ? String : SG_T("");
}

const SG_Char * CSG_Grid::Get_Unit(void) const
{
	return( m_Unit.c_str() );
}

//---------------------------------------------------------
void CSG_Grid::Set_ZFactor(double Value)
{
	m_zFactor		= Value;
}

//---------------------------------------------------------
double CSG_Grid::Get_ZFactor(void) const
{
	return( m_zFactor );
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
		switch( m_Memory_Type )
		{
		default:
			return( m_Values != NULL );

		case GRID_MEMORY_Cache:
			return( Cache_Stream.is_Open() );
		}
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
double CSG_Grid::Get_Value(TSG_Point Position, int Interpolation, bool bZFactor, bool bByteWise, bool bOnlyValidCells) const
{
	double	Value;

	return( Get_Value(Position.x, Position.y, Value, Interpolation, bZFactor, bByteWise, bOnlyValidCells) ? Value : Get_NoData_Value() );
}

double CSG_Grid::Get_Value(double xPosition, double yPosition, int Interpolation, bool bZFactor, bool bByteWise, bool bOnlyValidCells) const
{
	double	Value;

	return( Get_Value(xPosition, yPosition, Value, Interpolation, bZFactor, bByteWise, bOnlyValidCells) ? Value : Get_NoData_Value() );
}

bool CSG_Grid::Get_Value(TSG_Point Position, double &Value, int Interpolation, bool bZFactor, bool bByteWise, bool bOnlyValidCells) const
{
	return( Get_Value(Position.x, Position.y, Value, Interpolation, bZFactor, bByteWise, bOnlyValidCells) );
}

//---------------------------------------------------------
bool CSG_Grid::Get_Value(double xPosition, double yPosition, double &Value, int Interpolation, bool bZFactor, bool bByteWise, bool bOnlyValidCells) const
{
	if(	m_System.Get_Extent(true).Contains(xPosition, yPosition) )
	{
		int		x	= (int)(xPosition	= (xPosition - Get_XMin()) / Get_Cellsize());
		int		y	= (int)(yPosition	= (yPosition - Get_YMin()) / Get_Cellsize());

		double	dx	= xPosition - x;
		double	dy	= yPosition - y;

		if( !bOnlyValidCells || is_InGrid(x + (int)(0.5 + dx), y + (int)(0.5 + dy)) )
		{
			switch( Interpolation )
			{
			case GRID_INTERPOLATION_NearestNeighbour:
				Value	= _Get_ValAtPos_NearestNeighbour(x, y, dx, dy);
				break;

			case GRID_INTERPOLATION_Bilinear:
				Value	= _Get_ValAtPos_BiLinear		(x, y, dx, dy, bByteWise);
				break;

			case GRID_INTERPOLATION_InverseDistance:
				Value	= _Get_ValAtPos_InverseDistance	(x, y, dx, dy, bByteWise);
				break;

			case GRID_INTERPOLATION_BicubicSpline:
				Value	= _Get_ValAtPos_BiCubicSpline	(x, y, dx, dy, bByteWise);
				break;

			default:
			case GRID_INTERPOLATION_BSpline:
				Value	= _Get_ValAtPos_BSpline			(x, y, dx, dy, bByteWise);
				break;
			}

			if( !is_NoData_Value(Value) )
			{
				if( bZFactor )
				{
					Value	*= m_zFactor;
				}

				return( true );
			}
		}
	}

	return( false );
}

//---------------------------------------------------------
inline double CSG_Grid::_Get_ValAtPos_NearestNeighbour(int x, int y, double dx, double dy) const
{
	x	+= (int)(0.5 + dx);
	y	+= (int)(0.5 + dy);

	if( is_InGrid(x, y) )
	{
		return( asDouble(x, y) );
	}

	return( Get_NoData_Value() );
}

//---------------------------------------------------------
#define BILINEAR_ADD(ix, iy, d)			if( is_InGrid(ix, iy) ) { n += d;\
											z += d * asDouble(ix, iy); }

#define BILINEAR_ADD_BYTE(ix, iy, d)	if( is_InGrid(ix, iy) ) { n += d; v = asInt(ix, iy);\
											z[0] += d * SG_GET_BYTE_0(v);\
											z[1] += d * SG_GET_BYTE_1(v);\
											z[2] += d * SG_GET_BYTE_2(v);\
											z[3] += d * SG_GET_BYTE_3(v); }

inline double CSG_Grid::_Get_ValAtPos_BiLinear(int x, int y, double dx, double dy, bool bByteWise) const
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
			return( z / n );
		}
	}
	else
	{
		long	v;
		double	z[4], n = 0.0;

		z[0] = z[1] = z[2] = z[3] = 0.0;

		BILINEAR_ADD_BYTE(x    , y    , (1.0 - dx) * (1.0 - dy));
		BILINEAR_ADD_BYTE(x + 1, y    , (      dx) * (1.0 - dy));
		BILINEAR_ADD_BYTE(x    , y + 1, (1.0 - dx) * (      dy));
		BILINEAR_ADD_BYTE(x + 1, y + 1, (      dx) * (      dy));

		if( n > 0.0 )
		{
			z[0]	/= n;
			z[1]	/= n;
			z[2]	/= n;
			z[3]	/= n;

			return( SG_GET_LONG(z[0], z[1], z[2], z[3]) );
		}
	}

	return( Get_NoData_Value() );
}

//---------------------------------------------------------
#define INVERSEDIST_ADD(ix, iy, dsx, dsy)		if( is_InGrid(ix, iy) ) { d = 1.0 / sqrt((dsx)*(dsx) + (dsy)*(dsy)); n += d;\
													z += d * asDouble(ix, iy); }

#define INVERSEDIST_ADD_BYTE(ix, iy, dsx, dsy)	if( is_InGrid(ix, iy) ) { d = 1.0 / sqrt((dsx)*(dsx) + (dsy)*(dsy)); n += d; v = asInt(ix, iy);\
													z[0] += d * SG_GET_BYTE_0(v);\
													z[1] += d * SG_GET_BYTE_1(v);\
													z[2] += d * SG_GET_BYTE_2(v);\
													z[3] += d * SG_GET_BYTE_3(v); }

inline double CSG_Grid::_Get_ValAtPos_InverseDistance(int x, int y, double dx, double dy, bool bByteWise) const
{
	if( dx > 0.0 || dy > 0.0 )
	{
		if( !bByteWise )
		{
			double	z = 0.0, n = 0.0, d;

			INVERSEDIST_ADD(x    , y    ,       dx,       dy);
			INVERSEDIST_ADD(x + 1, y    , 1.0 - dx,       dy);
			INVERSEDIST_ADD(x    , y + 1,       dx, 1.0 - dy);
			INVERSEDIST_ADD(x + 1, y + 1, 1.0 - dx, 1.0 - dy);

			if( n > 0.0 )
			{
				return( z / n );
			}
		}
		else
		{
			long	v;
			double	z[4], n = 0.0, d;

			z[0] = z[1] = z[2] = z[3] = 0.0;

			INVERSEDIST_ADD_BYTE(x    , y    ,       dx,       dy);
			INVERSEDIST_ADD_BYTE(x + 1, y    , 1.0 - dx,       dy);
			INVERSEDIST_ADD_BYTE(x    , y + 1,       dx, 1.0 - dy);
			INVERSEDIST_ADD_BYTE(x + 1, y + 1, 1.0 - dx, 1.0 - dy);

			if( n > 0.0 )
			{
				z[0]	/= n;
				z[1]	/= n;
				z[2]	/= n;
				z[3]	/= n;

				return( SG_GET_LONG(z[0], z[1], z[2], z[3]) );
			}
		}
	}
	else
	{
		return( asDouble(x, y) );
	}

	return( Get_NoData_Value() );
}

//---------------------------------------------------------
inline double CSG_Grid::_Get_ValAtPos_BiCubicSpline(double dx, double dy, double z_xy[4][4]) const
{
	double	a0, a2, a3, b1, b2, b3, c[4];

	for(int i=0; i<4; i++)
	{
		a0		= z_xy[0][i] - z_xy[1][i];
		a2		= z_xy[2][i] - z_xy[1][i];
		a3		= z_xy[3][i] - z_xy[1][i];

		b1		= -a0 / 3.0 + a2       - a3 / 6.0;
		b2		=  a0 / 2.0 + a2 / 2.0;
		b3		= -a0 / 6.0 - a2 / 2.0 + a3 / 6.0;

		c[i]	= z_xy[1][i] + b1 * dx + b2 * dx*dx + b3 * dx*dx*dx;
	}

	a0		= c[0] - c[1];
	a2		= c[2] - c[1];
	a3		= c[3] - c[1];

	b1		= -a0 / 3.0 + a2       - a3 / 6.0;
	b2		=  a0 / 2.0 + a2 / 2.0;
	b3		= -a0 / 6.0 - a2 / 2.0 + a3 / 6.0;

	return( c[1] + b1 * dy + b2 * dy*dy + b3 * dy*dy*dy );
}

inline double CSG_Grid::_Get_ValAtPos_BiCubicSpline(int x, int y, double dx, double dy, bool bByteWise) const
{
	if( !bByteWise )
	{
		double	z_xy[4][4];

		if( _Get_ValAtPos_Fill4x4Submatrix(x, y, z_xy) )
		{
			return( _Get_ValAtPos_BiCubicSpline(dx, dy, z_xy) );
		}
	}
	else
	{
		double	z_xy[4][4][4];

		if( _Get_ValAtPos_Fill4x4Submatrix(x, y, z_xy) )
		{
			return( SG_GET_LONG(
				_Get_ValAtPos_BiCubicSpline(dx, dy, z_xy[0]),
				_Get_ValAtPos_BiCubicSpline(dx, dy, z_xy[1]),
				_Get_ValAtPos_BiCubicSpline(dx, dy, z_xy[2]),
				_Get_ValAtPos_BiCubicSpline(dx, dy, z_xy[3])
			));
		}
	}

	return( Get_NoData_Value() );
}

//---------------------------------------------------------
inline double CSG_Grid::_Get_ValAtPos_BSpline(double dx, double dy, double z_xy[4][4]) const
{
	int		i, ix, iy;
	double	z, px, py, Rx[4], Ry[4];

	for(i=0, px=-1.0-dx, py=-1.0-dy; i<4; i++, px++, py++)
	{
		Rx[i]	= 0.0;
		Ry[i]	= 0.0;

		if( (z = px + 2.0) > 0.0 )
			Rx[i]	+=        z*z*z;
		if( (z = px + 1.0) > 0.0 )
			Rx[i]	+= -4.0 * z*z*z;
		if( (z = px + 0.0) > 0.0 )
			Rx[i]	+=  6.0 * z*z*z;
		if( (z = px - 1.0) > 0.0 )
			Rx[i]	+= -4.0 * z*z*z;
		if( (z = py + 2.0) > 0.0 )
			Ry[i]	+=        z*z*z;
		if( (z = py + 1.0) > 0.0 )
			Ry[i]	+= -4.0 * z*z*z;
		if( (z = py + 0.0) > 0.0 )
			Ry[i]	+=  6.0 * z*z*z;
		if( (z = py - 1.0) > 0.0 )
			Ry[i]	+= -4.0 * z*z*z;

		Rx[i]	/= 6.0;
		Ry[i]	/= 6.0;
	}

	for(iy=0, z=0.0; iy<4; iy++)
	{
		for(ix=0; ix<4; ix++)
		{
			z	+= z_xy[ix][iy] * Rx[ix] * Ry[iy];
		}
	}

	return( z );
}

inline double CSG_Grid::_Get_ValAtPos_BSpline(int x, int y, double dx, double dy, bool bByteWise) const
{
	if( !bByteWise )
	{
		double	z_xy[4][4];

		if( _Get_ValAtPos_Fill4x4Submatrix(x, y, z_xy) )
		{
			return( _Get_ValAtPos_BSpline(dx, dy, z_xy) );
		}
	}
	else
	{
		double	z_xy[4][4][4];

		if( _Get_ValAtPos_Fill4x4Submatrix(x, y, z_xy) )
		{
			return( SG_GET_LONG(
				_Get_ValAtPos_BSpline(dx, dy, z_xy[0]),
				_Get_ValAtPos_BSpline(dx, dy, z_xy[1]),
				_Get_ValAtPos_BSpline(dx, dy, z_xy[2]),
				_Get_ValAtPos_BSpline(dx, dy, z_xy[3])
			));
		}
	}

	return( Get_NoData_Value() );
}

//---------------------------------------------------------
inline bool CSG_Grid::_Get_ValAtPos_Fill4x4Submatrix(int x, int y, double z_xy[4][4]) const
{
	int		ix, iy, jx, jy, nNoData;

	for(iy=0, jy=y-1, nNoData=0; iy<4; iy++, jy++)
	{
		for(ix=0, jx=x-1; ix<4; ix++, jx++)
		{
			if( is_InGrid(jx, jy) )
			{
				z_xy[ix][iy]	= asDouble(jx, jy);
			}
			else
			{
				z_xy[ix][iy]	= Get_NoData_Value();

				nNoData++;
			}
		}
	}

	//-----------------------------------------------------
	if( nNoData >= 16 )
	{
		return( false );
	}

	if( nNoData > 0 )
	{
		double	t_xy[4][4];

		for(iy=0; iy<4; iy++)
		{
			for(ix=0; ix<4; ix++)
			{
				t_xy[ix][iy]	= z_xy[ix][iy];
			}
		}

		do
		{
			for(iy=0; iy<4; iy++)
			{
				for(ix=0; ix<4; ix++)
				{
					if( is_NoData_Value(z_xy[ix][iy]) )
					{
						int		n	= 0;

						for(jy=iy-1; jy<=iy+1; jy++)
						{
							if( jy >= 0 && jy < 4 )
							{
								for(jx=ix-1; jx<=ix+1; jx++)
								{
									if( jx >= 0 && jx < 4 && !(jx == ix && jy == iy) && !is_NoData_Value(z_xy[jx][jy]) )
									{
										if( n == 0 )
										{
											t_xy[ix][iy]	 = z_xy[jx][jy];
										}
										else
										{
											t_xy[ix][iy]	+= z_xy[jx][jy];
										}

										n++;
									}
								}
							}
						}

						if( n > 0 )
						{
							if( n > 1 )
							{
								t_xy[ix][iy]	/= n;
							}

							nNoData--;
						}
					}
				}
			}

			for(iy=0; iy<4; iy++)
			{
				for(ix=0; ix<4; ix++)
				{
					if( t_xy[ix][iy] != z_xy[ix][iy] )
					{
						z_xy[ix][iy]	= t_xy[ix][iy];
					}
				}
			}
		}
		while( nNoData > 0 );
	}

	return( true );
}

//---------------------------------------------------------
inline bool CSG_Grid::_Get_ValAtPos_Fill4x4Submatrix(int x, int y, double z_xy[4][4][4]) const
{
	int		ix, iy, jx, jy, nNoData;

	for(iy=0, jy=y-1, nNoData=0; iy<4; iy++, jy++)
	{
		for(ix=0, jx=x-1; ix<4; ix++, jx++)
		{
			if( is_InGrid(jx, jy) )
			{
				int		v	= asInt(jx, jy);

				z_xy[0][ix][iy]	= SG_GET_BYTE_0(v);
				z_xy[1][ix][iy]	= SG_GET_BYTE_1(v);
				z_xy[2][ix][iy]	= SG_GET_BYTE_2(v);
				z_xy[3][ix][iy]	= SG_GET_BYTE_3(v);
			}
			else
			{
				z_xy[0][ix][iy]	= Get_NoData_Value();

				nNoData++;
			}
		}
	}

	//-----------------------------------------------------
	if( nNoData >= 16 )
	{
		return( false );
	}

	if( nNoData > 0 )
	{
		double	t_xy[4][4][4];

		for(iy=0; iy<4; iy++)
		{
			for(ix=0; ix<4; ix++)
			{
				t_xy[0][ix][iy]	= z_xy[0][ix][iy];
				t_xy[1][ix][iy]	= z_xy[1][ix][iy];
				t_xy[2][ix][iy]	= z_xy[2][ix][iy];
				t_xy[3][ix][iy]	= z_xy[3][ix][iy];
			}
		}

		do
		{
			for(iy=0; iy<4; iy++)
			{
				for(ix=0; ix<4; ix++)
				{
					if( is_NoData_Value(z_xy[0][ix][iy]) )
					{
						int		n	= 0;

						for(jy=iy-1; jy<=iy+1; jy++)
						{
							if( jy >= 0 && jy < 4 )
							{
								for(jx=ix-1; jx<=ix+1; jx++)
								{
									if( jx >= 0 && jx < 4 && !(jx == ix && jy == iy) && !is_NoData_Value(z_xy[0][jx][jy]) )
									{
										if( n == 0 )
										{
											t_xy[0][ix][iy]	 = z_xy[0][jx][jy];
											t_xy[1][ix][iy]	 = z_xy[1][jx][jy];
											t_xy[2][ix][iy]	 = z_xy[2][jx][jy];
											t_xy[3][ix][iy]	 = z_xy[3][jx][jy];
										}
										else
										{
											t_xy[0][ix][iy]	+= z_xy[0][jx][jy];
											t_xy[1][ix][iy]	+= z_xy[1][jx][jy];
											t_xy[2][ix][iy]	+= z_xy[2][jx][jy];
											t_xy[3][ix][iy]	+= z_xy[3][jx][jy];
										}

										n++;
									}
								}
							}
						}

						if( n > 0 )
						{
							if( n > 1 )
							{
								t_xy[0][ix][iy]	/= n;
								t_xy[1][ix][iy]	/= n;
								t_xy[2][ix][iy]	/= n;
								t_xy[3][ix][iy]	/= n;
							}

							nNoData--;
						}
					}
				}
			}

			for(iy=0; iy<4; iy++)
			{
				for(ix=0; ix<4; ix++)
				{
					if( t_xy[0][ix][iy] != z_xy[0][ix][iy] )
					{
						z_xy[0][ix][iy]	= t_xy[0][ix][iy];
						z_xy[1][ix][iy]	= t_xy[1][ix][iy];
						z_xy[2][ix][iy]	= t_xy[2][ix][iy];
						z_xy[3][ix][iy]	= t_xy[3][ix][iy];
					}
				}
			}
		}
		while( nNoData > 0 );
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Statistics						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CSG_Grid::Get_ZMin(bool bZFactor)
{
	Update();	return( (bZFactor ? m_zFactor : 1.0) * m_zStats.Get_Minimum() );
}

double CSG_Grid::Get_ZMax(bool bZFactor)
{
	Update();	return( (bZFactor ? m_zFactor : 1.0) * m_zStats.Get_Maximum() );
}

double CSG_Grid::Get_ZRange(bool bZFactor)
{
	Update();	return( (bZFactor ? m_zFactor : 1.0) * m_zStats.Get_Range() );
}

double CSG_Grid::Get_ArithMean(bool bZFactor)
{
	Update();	return( (bZFactor ? m_zFactor : 1.0) * m_zStats.Get_Mean() );
}

double CSG_Grid::Get_StdDev(bool bZFactor)
{
	Update();	return( (bZFactor ? m_zFactor : 1.0) * m_zStats.Get_StdDev() );
}

double CSG_Grid::Get_Variance(void)
{
	Update();	return( m_zStats.Get_Variance() );
}

int CSG_Grid::Get_NoData_Count(void)
{
	Update();	return( Get_NCells() - m_zStats.Get_Count() );
}

//---------------------------------------------------------
bool CSG_Grid::On_Update(void)
{
	if( is_Valid() )
	{
		m_zStats.Invalidate();

		for(int y=0; y<Get_NY() && SG_UI_Process_Set_Progress(y, Get_NY()); y++)
		{
			for(int x=0; x<Get_NX(); x++)
			{
				double	z	= asDouble(x, y);

				if( !is_NoData_Value(z) )
				{
					m_zStats.Add_Value(z);
				}
			}
		}

		SG_UI_Process_Set_Ready();
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Index							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Grid::Set_Value_And_Sort(long n, double Value)
{
	if( !m_bIndexed )
	{
		Set_Value(n, Value);

		Set_Index(true);

		return;
	}

	//-----------------------------------------------------
	if( Value == asDouble(n) )
		return;

	long	i, j;

	for(i=0, j=-1; i<Get_NCells() && j<0; i++)	// find index, could be faster...
	{
		if( n == m_Index[i] )
		{
			j	= i;
		}
	}

	if( j > 0 )
	{
		if( Value < asDouble(n) )
		{
			for(i=j-1; i>=0; i--, j--)
			{
				if( Value < asDouble(m_Index[i]) )
				{
					m_Index[j]	= m_Index[i];
				}
				else
				{
					m_Index[j]	= n;
					break;
				}
			}
		}
		else
		{
			for(i=j+1; i<Get_NCells(); i++, j++)
			{
				if( Value > asDouble(m_Index[i]) )
				{
					m_Index[j]	= m_Index[i];
				}
				else
				{
					m_Index[j]	= n;
					break;
				}
			}
		}

		Set_Value(n, Value);

		m_bIndexed	= true;
	}
}

//---------------------------------------------------------
void CSG_Grid::Set_Value_And_Sort(int x, int y, double Value)
{
	Set_Value_And_Sort(x + y * Get_NX(), Value);
}

//---------------------------------------------------------
double CSG_Grid::Get_Percentile(double Percent, bool bZFactor)
{
	int		x, y;

	if( Percent < 0.0 )
	{
		Percent	= 0.0;
	}
	else if( Percent > 100.0 )
	{
		Percent	= 100.0;
	}

	if( Get_Sorted((int)(Percent * Get_NCells() / 100.0), x, y, true) )
	{
		return( asDouble(x, y, bZFactor) );
	}

	return( 0.0 );
}

//---------------------------------------------------------
bool CSG_Grid::Set_Index(bool bOn)
{
	if( bOn && !m_bIndexed && Get_NoData_Count() < Get_NCells() )
	{
		m_bIndexed	= true;

		if( _Set_Index() == false )
		{
			Set_Index(false);

			return( false );
		}
	}
	else if( !bOn || Get_NoData_Count() >= Get_NCells() )
	{
		m_bIndexed	= false;

		if( m_Index )
		{
			SG_Free(m_Index);
			m_Index		= NULL;
		}
	}

	return( m_bIndexed );
}

//---------------------------------------------------------
#define SORT_SWAP(a,b)	{itemp=(a);(a)=(b);(b)=itemp;}

bool CSG_Grid::_Set_Index(void)
{
	const int	M	= 7;

	int		i, j, k, l, ir, n, nCells, *istack, jstack, nstack, indxt, itemp;
	double	a;

	//-----------------------------------------------------
	SG_UI_Process_Set_Text(CSG_String::Format(SG_T("%s: %s"), LNG("Create index"), Get_Name()));

	if( m_Index == NULL )
	{
		m_Index		= (long *)SG_Calloc(Get_NCells(), sizeof(long));

		if( m_Index == NULL )
		{
			SG_UI_Msg_Add_Error(LNG("could not create index: insufficient memory"));

			SG_UI_Process_Set_Ready();

			return( false );
		}

		for(i=0, l=0; i<Get_NCells(); i++)
		{
			if(  is_NoData(i) )
			{
				m_Index[l++]	= i;
			}
		}

		for(i=0, j=l; i<Get_NCells(); i++)
		{
			if( !is_NoData(i) )
			{
				m_Index[j++]	= i;
			}
		}
	}
	else
	{
		l	= 0;
	}

	//-----------------------------------------------------
	if( (nCells = Get_NCells() - l) <= 1 )
	{
		return( false );
	}

	//-----------------------------------------------------
	n		= 0;
	ir		= Get_NCells() - 1;

	nstack	= 64;
	istack	= (int *)SG_Malloc(nstack * sizeof(int));
	jstack	= 0;

	for(;;)
	{
		if( ir - l < M )
		{
			if( !SG_UI_Process_Set_Progress(n += M - 1, nCells) )
			{
				SG_Free(istack);

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

			m_Index[l]	= m_Index[j];
			m_Index[j]	= indxt;
			jstack		+= 2;

			if( jstack >= nstack )
			{
				nstack	+= 64;
				istack	= (int *)SG_Realloc(istack, nstack * sizeof(int));
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
#undef SORT_SWAP


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

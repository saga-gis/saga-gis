
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
CSG_Grid * SG_Create_Grid(const SG_Char *File_Name, TSG_Grid_Type Type, TSG_Grid_Memory_Type Memory_Type)
{
	return( new CSG_Grid(File_Name, Type, Memory_Type) );
}

//---------------------------------------------------------
CSG_Grid * SG_Create_Grid(CSG_Grid *pGrid, TSG_Grid_Type Type, TSG_Grid_Memory_Type Memory_Type)
{
	return( new CSG_Grid(pGrid, Type, Memory_Type) );
}

//---------------------------------------------------------
CSG_Grid * SG_Create_Grid(const CSG_Grid_System &System, TSG_Grid_Type Type, TSG_Grid_Memory_Type Memory_Type)
{
	return( new CSG_Grid(System, Type, Memory_Type) );
}

//---------------------------------------------------------
CSG_Grid * SG_Create_Grid(TSG_Grid_Type Type, int NX, int NY, double Cellsize, double xMin, double yMin, TSG_Grid_Memory_Type Memory_Type)
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
CSG_Grid::CSG_Grid(const SG_Char *File_Name, TSG_Grid_Type Type, TSG_Grid_Memory_Type Memory_Type)
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
CSG_Grid::CSG_Grid(CSG_Grid *pGrid, TSG_Grid_Type Type, TSG_Grid_Memory_Type Memory_Type)
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
CSG_Grid::CSG_Grid(const CSG_Grid_System &System, TSG_Grid_Type Type, TSG_Grid_Memory_Type Memory_Type)
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
CSG_Grid::CSG_Grid(TSG_Grid_Type Type, int NX, int NY, double Cellsize, double xMin, double yMin, TSG_Grid_Memory_Type Memory_Type)
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

	m_Type				= GRID_TYPE_Undefined;
	m_Memory_Type		= GRID_MEMORY_Normal;
	m_Memory_bLock		= false;

	m_Values			= NULL;

	LineBuffer			= NULL;
	LineBuffer_Count	= 5;

	m_zFactor			= 1.0;
	m_NoData_Value		= -99999.0;
	m_NoData_hiValue	= -999.0;

	m_bSorted			= false;

	m_Sort_2b			= NULL;
	m_Sort_4b			= NULL;

	m_bUpdate			= true;
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
bool CSG_Grid::Create(CSG_Grid *pGrid, TSG_Grid_Type Type, TSG_Grid_Memory_Type Memory_Type)
{
	return( Create(Type, pGrid->Get_NX(), pGrid->Get_NY(), pGrid->Get_Cellsize(), pGrid->Get_XMin(), pGrid->Get_YMin(), Memory_Type) );
}

//---------------------------------------------------------
bool CSG_Grid::Create(const CSG_Grid_System &System, TSG_Grid_Type Type, TSG_Grid_Memory_Type Memory_Type)
{
	return( Create(Type, System.Get_NX(), System.Get_NY(), System.Get_Cellsize(), System.Get_XMin(), System.Get_YMin(), Memory_Type) );
}

//---------------------------------------------------------
bool CSG_Grid::Create(const SG_Char *File_Name, TSG_Grid_Type Type, TSG_Grid_Memory_Type Memory_Type)
{
	return( _Load(File_Name, Type, Memory_Type) );
}

//---------------------------------------------------------
bool CSG_Grid::Create(TSG_Grid_Type Type, int NX, int NY, double Cellsize, double xMin, double yMin, TSG_Grid_Memory_Type Memory_Type)
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

	m_Type			= GRID_TYPE_Undefined;
	m_Memory_Type	= GRID_MEMORY_Normal;

	m_zFactor		= 1.0;

	m_Description	.Clear();
	m_Unit			.Clear();

	return( CSG_Data_Object::Destroy() );
}


///////////////////////////////////////////////////////////
//														 //
//						Header							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Grid::_Set_Properties(TSG_Grid_Type Type, int NX, int NY, double Cellsize, double xMin, double yMin)
{
	m_Type	= Type > GRID_TYPE_Undefined && Type < GRID_TYPE_Count ? Type : GRID_TYPE_Float;

	m_System.Assign(Cellsize > 0.0 ? Cellsize : 1.0, xMin, yMin, NX, NY);

	m_zMin	= m_zMax	= 0.0;
}

//---------------------------------------------------------
void CSG_Grid::Set_Description(const SG_Char *String)
{
	m_Description.Printf(String ? String : SG_T(""));
}

const SG_Char * CSG_Grid::Get_Description(void) const
{
	return( m_Description.c_str() );
}

//---------------------------------------------------------
void CSG_Grid::Set_Unit(const SG_Char *String)
{
	m_Unit.Printf(String ? String : SG_T(""));
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
//					No Data Values						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Grid::Set_NoData_Value(double Value)
{
	Set_NoData_Value_Range(Value, Value);
}

void CSG_Grid::Set_NoData_Value_Range(double loValue, double hiValue)
{
	double	d;

	if( loValue > hiValue )
	{
		d			= loValue;
		loValue		= hiValue;
		hiValue		= d;
	}

	if( !m_bUpdate )
	{
		m_bUpdate	= loValue != m_NoData_Value || hiValue != m_NoData_hiValue;
	}

	m_NoData_Value		= loValue;
	m_NoData_hiValue	= hiValue;
}


///////////////////////////////////////////////////////////
//														 //
//						Checks							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grid::is_Valid(void) const
{
	if(	m_System.is_Valid() && m_Type > 0 && m_Type < GRID_TYPE_Count )
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
double CSG_Grid::Get_Value(TSG_Point Position, int Interpolation, bool bZFactor, bool bByteWise) const
{
	double	Value;

	return( Get_Value(Position.x, Position.y, Value, Interpolation, bZFactor, bByteWise) ? Value : m_NoData_Value );
}

double CSG_Grid::Get_Value(double xPosition, double yPosition, int Interpolation, bool bZFactor, bool bByteWise) const
{
	double	Value;

	return( Get_Value(xPosition, yPosition, Value, Interpolation, bZFactor, bByteWise) ? Value : m_NoData_Value );
}

bool CSG_Grid::Get_Value(TSG_Point Position, double &Value, int Interpolation, bool bZFactor, bool bByteWise) const
{
	return( Get_Value(Position.x, Position.y, Value, Interpolation, bZFactor, bByteWise) );
}

//---------------------------------------------------------
inline bool CSG_Grid::Get_Value(double xPosition, double yPosition, double &Value, int Interpolation, bool bZFactor, bool bByteWise) const
{
	int		x, y;
	double	dx, dy;

	if(	Get_Extent().Contains(xPosition, yPosition) )
	{
		x	= (int)(xPosition	= (xPosition - Get_XMin()) / Get_Cellsize());
		y	= (int)(yPosition	= (yPosition - Get_YMin()) / Get_Cellsize());

		dx	= xPosition - x;
		dy	= yPosition - y;

		switch( Interpolation )
		{
		default:
			return( false );

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

		case GRID_INTERPOLATION_BSpline:
			Value	= _Get_ValAtPos_BSpline			(x, y, dx, dy, bByteWise);
			break;
		}

		if( Value != m_NoData_Value )
		{
			if( bZFactor )
			{
				Value	*= m_zFactor;
			}

			return( true );
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

	return( m_NoData_Value );
}

//---------------------------------------------------------
#define BILINEAR_ADD(ix, iy, d)			if( is_InGrid(ix, iy) ) { n += d;\
											z += d * asDouble(ix, iy); }

#define BILINEAR_ADD_BYTE(ix, iy, d)	if( is_InGrid(ix, iy) ) { n += d; v = asLong(ix, iy);\
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

	return( m_NoData_Value );
}

//---------------------------------------------------------
#define INVERSEDIST_ADD(ix, iy, dsx, dsy)		if( is_InGrid(ix, iy) ) { d = 1.0 / sqrt((dsx)*(dsx) + (dsy)*(dsy)); n += d;\
													z += d * asDouble(ix, iy); }

#define INVERSEDIST_ADD_BYTE(ix, iy, dsx, dsy)	if( is_InGrid(ix, iy) ) { d = 1.0 / sqrt((dsx)*(dsx) + (dsy)*(dsy)); n += d; v = asLong(ix, iy);\
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

	return( m_NoData_Value );
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
		double	z_xy[4][4][4], z[4];

		if( _Get_ValAtPos_Fill4x4Submatrix(x, y, z_xy) )
		{
			z[0]	= _Get_ValAtPos_BiCubicSpline(dx, dy, z_xy[0]);
			z[1]	= _Get_ValAtPos_BiCubicSpline(dx, dy, z_xy[1]);
			z[2]	= _Get_ValAtPos_BiCubicSpline(dx, dy, z_xy[2]);
			z[3]	= _Get_ValAtPos_BiCubicSpline(dx, dy, z_xy[3]);

			return( SG_GET_LONG(z[0], z[1], z[2], z[3]) );
		}
	}

	return( _Get_ValAtPos_BiLinear(x, y, dx, dy, bByteWise) );
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
		double	z_xy[4][4][4], z[4];

		if( _Get_ValAtPos_Fill4x4Submatrix(x, y, z_xy) )
		{
			z[0]	= _Get_ValAtPos_BSpline(dx, dy, z_xy[0]);
			z[1]	= _Get_ValAtPos_BSpline(dx, dy, z_xy[1]);
			z[2]	= _Get_ValAtPos_BSpline(dx, dy, z_xy[2]);
			z[3]	= _Get_ValAtPos_BSpline(dx, dy, z_xy[3]);

			return( SG_GET_LONG(z[0], z[1], z[2], z[3]) );
		}
	}

	return( _Get_ValAtPos_BiLinear(x, y, dx, dy, bByteWise) );
}

//---------------------------------------------------------
inline bool CSG_Grid::_Get_ValAtPos_Fill4x4Submatrix(int x, int y, double z_xy[4][4]) const
{
	int		ix, iy, px, py;

	for(iy=0, py=y-1; iy<4; iy++, py++)
	{
		for(ix=0, px=x-1; ix<4; ix++, px++)
		{
			if( !is_InGrid(px, py) )
			{
				return( false );
			}

			z_xy[ix][iy]	= asDouble(px, py);
		}
	}

	return( true );
}

inline bool CSG_Grid::_Get_ValAtPos_Fill4x4Submatrix(int x, int y, double z_xy[4][4][4]) const
{
	for(int iy=0, py=y-1; iy<4; iy++, py++)
	{
		for(int ix=0, px=x-1; ix<4; ix++, px++)
		{
			if( !is_InGrid(px, py) )
			{
				return( false );
			}

			long	v	= asLong(px, py);

			z_xy[0][ix][iy]	= SG_GET_BYTE_0(v);
			z_xy[1][ix][iy]	= SG_GET_BYTE_1(v);
			z_xy[2][ix][iy]	= SG_GET_BYTE_2(v);
			z_xy[3][ix][iy]	= SG_GET_BYTE_3(v);
		}
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
	Update_Statistics();

	return( bZFactor ? m_zFactor * m_zMin : m_zMin );
}

//---------------------------------------------------------
double CSG_Grid::Get_ZMax(bool bZFactor)
{
	Update_Statistics();

	return( bZFactor ? m_zFactor * m_zMax : m_zMax );
}

//---------------------------------------------------------
double CSG_Grid::Get_ZRange(bool bZFactor)
{
	return( Get_ZMax(bZFactor) - Get_ZMin(bZFactor) );
}

//---------------------------------------------------------
double CSG_Grid::Get_ArithMean(bool bZFactor)
{
	Update_Statistics();

	return( bZFactor ? m_zFactor * m_ArithMean : m_ArithMean );
}

//---------------------------------------------------------
double CSG_Grid::Get_Variance(bool bZFactor)
{
	Update_Statistics();

	return( bZFactor ? m_zFactor * m_Variance : m_Variance);
}

//---------------------------------------------------------
bool CSG_Grid::Update_Statistics(bool bEnforce)
{
	int		x, y;
	long	nValues;
	double	z;

	if( is_Valid() && (m_bUpdate || bEnforce) )
	{
		m_ArithMean	= 0.0;
		m_Variance	= 0.0;
		nValues		= 0;

		for(y=0; y<Get_NY() && SG_UI_Process_Set_Progress(y, Get_NY()); y++)
		{
			for(x=0; x<Get_NX(); x++)
			{
				if( !is_NoData_Value(z = asDouble(x, y)) )
				{
					if( nValues == 0 )
					{
						m_zMin	= m_zMax	= z;
					}
					else if( m_zMin > z )
					{
						m_zMin	= z;
					}
					else if( m_zMax < z )
					{
						m_zMax	= z;
					}

					m_ArithMean	+= z;
					m_Variance	+= z * z;
					nValues++;
				}
			}
		}

		if( nValues > 0 )
		{
			m_ArithMean	/= (double)nValues;
			m_Variance	= m_Variance / (double)nValues - m_ArithMean * m_ArithMean;
		}

		m_bUpdate	= false;

		SG_UI_Process_Set_Ready();
	}

	return( m_bUpdate == false );
}


///////////////////////////////////////////////////////////
//														 //
//						Sort							 //
//														 //
///////////////////////////////////////////////////////////

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

	Get_Sorted((int)(Percent * Get_NCells() / 100.0), x, y, true);

	return( asDouble(x, y, bZFactor) );
}

//---------------------------------------------------------
bool CSG_Grid::_Sort_Execute(void)
{
	long	i, j, *Index;

	//-----------------------------------------------------
	SG_UI_Process_Set_Text(CSG_String::Format(SG_T("%s: %s"), LNG("Create index"), Get_Name()));

	Index	= (long *)SG_Calloc(Get_NCells(), sizeof(long));

	//-----------------------------------------------------
	if( (m_bSorted = _Sort_Index(Index)) == false )
	{
		Sort_Discard();
	}
	else if( Get_NX() < 65536 && Get_NY() < 65536 )
	{
		if( m_Sort_4b )
		{
			Sort_Discard();
		}

		if( !m_Sort_2b )
		{
			m_Sort_2b		= (unsigned short **)SG_Calloc(2			, sizeof(unsigned short *));
			m_Sort_2b[0]	= (unsigned short  *)SG_Calloc(Get_NCells(), sizeof(unsigned short));
			m_Sort_2b[1]	= (unsigned short  *)SG_Calloc(Get_NCells(), sizeof(unsigned short));
		}

		for(i=0; i<Get_NCells(); i++)
		{
			j				= Index[Get_NCells() - i - 1];
			m_Sort_2b[0][i]	= (unsigned short)(j % Get_NX());
			m_Sort_2b[1][i]	= (unsigned short)(j / Get_NX());
		}
	}
	else
	{
		if( m_Sort_2b )
		{
			Sort_Discard();
		}

		if( !m_Sort_4b )
		{
			m_Sort_4b		= (int **)SG_Calloc(2           , sizeof(int *));
			m_Sort_4b[0]	= (int  *)SG_Calloc(Get_NCells(), sizeof(int));
			m_Sort_4b[1]	= (int  *)SG_Calloc(Get_NCells(), sizeof(int));
		}

		for(i=0; i<Get_NCells(); i++)
		{
			j				= Index[Get_NCells() - i - 1];
			m_Sort_4b[0][i]	= (int)(j % Get_NX());
			m_Sort_4b[1][i]	= (int)(j / Get_NX());
		}
	}

	//-----------------------------------------------------
	SG_Free(Index);

	SG_UI_Process_Set_Ready();
	SG_UI_Process_Set_Text(LNG("ready"));

	return( m_bSorted );
}

//---------------------------------------------------------
void CSG_Grid::Sort_Discard(void)
{
	m_bSorted	= false;

	if( m_Sort_2b )
	{
		SG_Free(m_Sort_2b[0]);
		SG_Free(m_Sort_2b[1]);
		SG_Free(m_Sort_2b);
		m_Sort_2b	= NULL;
	}

	if( m_Sort_4b )
	{
		SG_Free(m_Sort_4b[0]);
		SG_Free(m_Sort_4b[1]);
		SG_Free(m_Sort_4b);
		m_Sort_4b	= NULL;
	}
}

//---------------------------------------------------------
#define SORT_SWAP(a,b)	{itemp=(a);(a)=(b);(b)=itemp;}

bool CSG_Grid::_Sort_Index(long *Index)
{
	const int	M	= 7;

	int		i, j, k, l, ir, n, nCells, *istack, jstack, nstack, indxt, itemp;
	double	a;

	//-----------------------------------------------------
	for(i=0, l=0; i<Get_NCells(); i++)
	{
		if(  is_NoData(i) )
		{
			Index[l++]	= i;
		}
	}

	if( (nCells = Get_NCells() - l) > 1 )
	{
		for(i=0, j=l; i<Get_NCells(); i++)
		{
			if( !is_NoData(i) )
			{
				Index[j++]	= i;
			}
		}

		//-------------------------------------------------
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
					indxt	= Index[j];
					a		= asDouble(indxt);

					for(i=j-1; i>=0; i--)
					{
						if( asDouble(Index[i]) <= a )
						{
							break;
						}

						Index[i + 1]	= Index[i];
					}

					Index[i + 1]	= indxt;
				}

				if( jstack == 0 )
				{
					break;
				}

				ir		= istack[jstack--];
				l		= istack[jstack--];
			}

			//---------------------------------------------
			else
			{
				k		= (l + ir) >> 1;

				SORT_SWAP(Index[k], Index[l + 1]);

				if( asDouble( Index[l + 1])	> asDouble(Index[ir]) )
					SORT_SWAP(Index[l + 1],            Index[ir]);

				if( asDouble( Index[l    ])	> asDouble(Index[ir]) )
					SORT_SWAP(Index[l    ],            Index[ir]);

				if( asDouble( Index[l + 1])	> asDouble(Index[l ]) )
					SORT_SWAP(Index[l + 1],            Index[l ]);

				i		= l + 1;
				j		= ir;
				indxt	= Index[l];
				a		= asDouble(indxt);

				for(;;)
				{
					do	i++;	while(asDouble(Index[i]) < a);
					do	j--;	while(asDouble(Index[j]) > a);

					if( j < i )
					{
						break;
					}

					SORT_SWAP(Index[i], Index[j]);
				}

				Index[l]	= Index[j];
				Index[j]	= indxt;
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

		//-------------------------------------------------
		SG_Free(istack);

		return( true );
	}

	return( false );
}
#undef SORT_SWAP


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


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
BYTE	CGrid::m_Bitmask[8]	= { 0x1, 0x2, 0x4, 0x8, 0x10, 0x20, 0x40, 0x80 };


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid * SG_Create_Grid(void)
{
	return( new CGrid );
}

//---------------------------------------------------------
CGrid * SG_Create_Grid(const CGrid &Grid)
{
	return( new CGrid(Grid) );
}

//---------------------------------------------------------
CGrid * SG_Create_Grid(const char *File_Name, TGrid_Type Type, TGrid_Memory_Type Memory_Type)
{
	return( new CGrid(File_Name, Type, Memory_Type) );
}

//---------------------------------------------------------
CGrid * SG_Create_Grid(CGrid *pGrid, TGrid_Type Type, TGrid_Memory_Type Memory_Type)
{
	return( new CGrid(pGrid, Type, Memory_Type) );
}

//---------------------------------------------------------
CGrid * SG_Create_Grid(const CGrid_System &System, TGrid_Type Type, TGrid_Memory_Type Memory_Type)
{
	return( new CGrid(System, Type, Memory_Type) );
}

//---------------------------------------------------------
CGrid * SG_Create_Grid(TGrid_Type Type, int NX, int NY, double Cellsize, double xMin, double yMin, TGrid_Memory_Type Memory_Type)
{
	return( new CGrid(Type, NX, NY, Cellsize, xMin, yMin, Memory_Type) );
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
CGrid::CGrid(void)
	: CDataObject()
{
	_On_Construction();
}

//---------------------------------------------------------
/**
  * Copy constructor.
*/
//---------------------------------------------------------
CGrid::CGrid(const CGrid &Grid)
	: CDataObject()
{
	_On_Construction();

	Create(Grid);
}

//---------------------------------------------------------
/**
  * Create a grid from file.
*/
//---------------------------------------------------------
CGrid::CGrid(const char *File_Name, TGrid_Type Type, TGrid_Memory_Type Memory_Type)
	: CDataObject()
{
	_On_Construction();

	Create(File_Name, Type, Memory_Type);
}

//---------------------------------------------------------
/**
  * Create a grid similar to 'pGrid'.
*/
//---------------------------------------------------------
CGrid::CGrid(CGrid *pGrid, TGrid_Type Type, TGrid_Memory_Type Memory_Type)
	: CDataObject()
{
	_On_Construction();

	Create(pGrid, Type, Memory_Type);
}

//---------------------------------------------------------
/**
  * Create a grid using 'System'.
*/
//---------------------------------------------------------
CGrid::CGrid(const CGrid_System &System, TGrid_Type Type, TGrid_Memory_Type Memory_Type)
	: CDataObject()
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
CGrid::CGrid(TGrid_Type Type, int NX, int NY, double Cellsize, double xMin, double yMin, TGrid_Memory_Type Memory_Type)
	: CDataObject()
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
void CGrid::_On_Construction(void)
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
bool CGrid::Create(const CGrid &Grid)
{
	if( Create((CGrid *)&Grid, ((CGrid *)&Grid)->Get_Type()) )
	{
		return( Assign((CDataObject *)&Grid) );
	}

	return( false );
}

//---------------------------------------------------------
bool CGrid::Create(CGrid *pGrid, TGrid_Type Type, TGrid_Memory_Type Memory_Type)
{
	return( Create(Type, pGrid->Get_NX(), pGrid->Get_NY(), pGrid->Get_Cellsize(), pGrid->Get_XMin(), pGrid->Get_YMin(), Memory_Type) );
}

//---------------------------------------------------------
bool CGrid::Create(const CGrid_System &System, TGrid_Type Type, TGrid_Memory_Type Memory_Type)
{
	return( Create(Type, System.Get_NX(), System.Get_NY(), System.Get_Cellsize(), System.Get_XMin(), System.Get_YMin(), Memory_Type) );
}

//---------------------------------------------------------
bool CGrid::Create(const char *File_Name, TGrid_Type Type, TGrid_Memory_Type Memory_Type)
{
	return( _Load(File_Name, Type, Memory_Type) );
}

//---------------------------------------------------------
bool CGrid::Create(TGrid_Type Type, int NX, int NY, double Cellsize, double xMin, double yMin, TGrid_Memory_Type Memory_Type)
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
CGrid::~CGrid(void)
{
	Destroy();
}

/**
  * Destroys the data space of CGrid.
*/
bool CGrid::Destroy(void)
{
	_Memory_Destroy();

	m_bCreated		= false;

	m_Type			= GRID_TYPE_Undefined;
	m_Memory_Type	= GRID_MEMORY_Normal;

	m_zFactor		= 1.0;

	m_Description	.Clear();
	m_Unit			.Clear();

	return( CDataObject::Destroy() );
}


///////////////////////////////////////////////////////////
//														 //
//						Header							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CGrid::_Set_Properties(TGrid_Type Type, int NX, int NY, double Cellsize, double xMin, double yMin)
{
	m_Type	= Type > GRID_TYPE_Undefined && Type < GRID_TYPE_Count ? Type : GRID_TYPE_Float;

	m_System.Assign(Cellsize > 0.0 ? Cellsize : 1.0, xMin, yMin, NX, NY);

	m_zMin	= m_zMax	= 0.0;
}

//---------------------------------------------------------
void CGrid::Set_Description(const char *String)
{
	m_Description.Printf(String ? String : "");
}

const char * CGrid::Get_Description(void)
{
	return( m_Description.c_str() );
}

//---------------------------------------------------------
void CGrid::Set_Unit(const char *String)
{
	m_Unit.Printf(String ? String : "");
}

const char * CGrid::Get_Unit(void)
{
	return( m_Unit.c_str() );
}

//---------------------------------------------------------
void CGrid::Set_ZFactor(double Value)
{
	m_zFactor		= Value;
}

//---------------------------------------------------------
double CGrid::Get_ZFactor(void)
{
	return( m_zFactor );
}


///////////////////////////////////////////////////////////
//														 //
//					No Data Values						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CGrid::Set_NoData_Value(double Value)
{
	Set_NoData_Value_Range(Value, Value);
}

void CGrid::Set_NoData_Value_Range(double loValue, double hiValue)
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
bool CGrid::is_Valid(void) const
{
	if(	m_System.is_Valid() && m_Type > 0 && m_Type < GRID_TYPE_Count )
	{
		switch( m_Memory_Type )
		{
		default:
			return( m_Values != NULL );

		case GRID_MEMORY_Cache:
			return( Cache_Stream != NULL );
		}
	}

	return( false );
}

//---------------------------------------------------------
TSG_Intersection CGrid::is_Intersecting(const CSG_Rect &Extent) const
{
	return( Get_Extent().Intersects(Extent.m_rect) );
}

TSG_Intersection CGrid::is_Intersecting(const TSG_Rect &Extent) const
{
	return( Get_Extent().Intersects(Extent) );
}

TSG_Intersection CGrid::is_Intersecting(double xMin, double yMin, double xMax, double yMax) const
{
	return( is_Intersecting(CSG_Rect(xMin, yMin, xMax, yMax)) );
}

//---------------------------------------------------------
bool CGrid::is_Compatible(CGrid *pGrid) const
{
	return( pGrid && is_Compatible(pGrid->Get_System()) );
}

bool CGrid::is_Compatible(const CGrid_System &System) const
{
	return( m_System == System );
}

bool CGrid::is_Compatible(int NX, int NY, double Cellsize, double xMin, double yMin) const
{
	return(	is_Compatible(CGrid_System(Cellsize, xMin, yMin, NX, NY)) );
}


///////////////////////////////////////////////////////////
//														 //
//		Value access by Position (-> Interpolation)		 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CGrid::Get_Value(TSG_Point Position, int Interpolation, bool bZFactor)
{
	double	Value;

	return( Get_Value(Position.x, Position.y, Value, Interpolation, bZFactor) ? Value : m_NoData_Value );
}

double CGrid::Get_Value(double xPosition, double yPosition, int Interpolation, bool bZFactor)
{
	double	Value;

	return( Get_Value(xPosition, yPosition, Value, Interpolation, bZFactor) ? Value : m_NoData_Value );
}

bool CGrid::Get_Value(TSG_Point Position, double &Value, int Interpolation, bool bZFactor)
{
	return( Get_Value(Position.x, Position.y, Value, Interpolation, bZFactor) );
}

//---------------------------------------------------------
inline bool CGrid::Get_Value(double xPosition, double yPosition, double &Value, int Interpolation, bool bZFactor)
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
			Value	= _Get_ValAtPos_BiLinear		(x, y, dx, dy);
			break;

		case GRID_INTERPOLATION_InverseDistance:
			Value	= _Get_ValAtPos_InverseDistance	(x, y, dx, dy);
			break;

		case GRID_INTERPOLATION_BicubicSpline:
			Value	= _Get_ValAtPos_BiCubicSpline	(x, y, dx, dy);
			break;

		case GRID_INTERPOLATION_BSpline:
			Value	= _Get_ValAtPos_BSpline			(x, y, dx, dy);
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
inline double CGrid::_Get_ValAtPos_NearestNeighbour(int x, int y, double dx, double dy)
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
#define BILINEAR_ADD(ix, iy, d)	if( is_InGrid(ix, iy) ) { z += d * asDouble(ix, iy); n += d; }

inline double CGrid::_Get_ValAtPos_BiLinear(int x, int y, double dx, double dy)
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

	return( m_NoData_Value );
}

//---------------------------------------------------------
#define INVERSEDIST_ADD(ix, iy, dsx, dsy)	if( is_InGrid(ix, iy) ) { d = 1.0 / sqrt(dsx*dsx + dsy*dsy); z += d * asDouble(ix, iy); n += d; }

inline double CGrid::_Get_ValAtPos_InverseDistance(int x, int y, double dx, double dy)
{
	double	z = 0.0, n = 0.0, d;

	if( dx > 0.0 || dy > 0.0 )
	{
		INVERSEDIST_ADD(x    , y    , (      dx), (      dy));
		INVERSEDIST_ADD(x + 1, y    , (1.0 - dx), (      dy));
		INVERSEDIST_ADD(x    , y + 1, (      dx), (1.0 - dy));
		INVERSEDIST_ADD(x + 1, y + 1, (1.0 - dx), (1.0 - dy));

		if( n > 0.0 )
		{
			return( z / n );
		}
	}
	else
	{
		return( asDouble(x, y) );
	}

	return( m_NoData_Value );
}

//---------------------------------------------------------
inline double CGrid::_Get_ValAtPos_BiCubicSpline(int x, int y, double dx, double dy)
{
	int		i;
	double	a0, a2, a3, b1, b2, b3, c[4], z_xy[4][4];

	if( _Get_ValAtPos_Fill4x4Submatrix(x, y, z_xy) )
	{
		for(i=0; i<4; i++)
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

	return( _Get_ValAtPos_BiLinear(x, y, dx, dy) );
}

//---------------------------------------------------------
inline double CGrid::_Get_ValAtPos_BSpline(int x, int y, double dx, double dy)
{
	int		i, ix, iy;
	double	z, px, py, Rx[4], Ry[4], z_xy[4][4];

	if( _Get_ValAtPos_Fill4x4Submatrix(x, y, z_xy) )
	{
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

	return( _Get_ValAtPos_BiLinear(x, y, dx, dy) );
}

//---------------------------------------------------------
inline bool CGrid::_Get_ValAtPos_Fill4x4Submatrix(int x, int y, double z_xy[4][4])
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


///////////////////////////////////////////////////////////
//														 //
//						Statistics						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CGrid::Get_ZMin(bool bZFactor)
{
	Update_Statistics();

	return( bZFactor ? m_zFactor * m_zMin : m_zMin );
}

//---------------------------------------------------------
double CGrid::Get_ZMax(bool bZFactor)
{
	Update_Statistics();

	return( bZFactor ? m_zFactor * m_zMax : m_zMax );
}

//---------------------------------------------------------
double CGrid::Get_ZRange(bool bZFactor)
{
	return( Get_ZMax(bZFactor) - Get_ZMin(bZFactor) );
}

//---------------------------------------------------------
double CGrid::Get_ArithMean(bool bZFactor)
{
	Update_Statistics();

	return( bZFactor ? m_zFactor * m_ArithMean : m_ArithMean );
}

//---------------------------------------------------------
double CGrid::Get_Variance(bool bZFactor)
{
	Update_Statistics();

	return( bZFactor ? m_zFactor * m_Variance : m_Variance);
}

//---------------------------------------------------------
bool CGrid::Update_Statistics(bool bEnforce)
{
	int		x, y;
	long	nValues;
	double	z;

	if( is_Valid() && (m_bUpdate || bEnforce) )
	{
		m_ArithMean	= 0.0;
		m_Variance	= 0.0;
		nValues		= 0;

		for(y=0; y<Get_NY() && SG_Callback_Process_Set_Progress(y, Get_NY()); y++)
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

		SG_Callback_Process_Set_Ready();
	}

	return( m_bUpdate == false );
}


///////////////////////////////////////////////////////////
//														 //
//						Sort							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CGrid::Get_Percentile(double Percent, bool bZFactor)
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
bool CGrid::_Sort_Execute(void)
{
	long	i, j, *Index;

	//-----------------------------------------------------
	SG_Callback_Process_Set_Text(CSG_String::Format("%s: %s", LNG("Create index"), Get_Name()));

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

	SG_Callback_Process_Set_Ready();
	SG_Callback_Process_Set_Text(LNG("ready"));

	return( m_bSorted );
}

//---------------------------------------------------------
void CGrid::Sort_Discard(void)
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

bool CGrid::_Sort_Index(long *Index)
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
				if( !SG_Callback_Process_Set_Progress(n += M - 1, nCells) )
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

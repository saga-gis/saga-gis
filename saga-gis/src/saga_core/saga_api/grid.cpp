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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
#include "data_manager.h"
#include "module_library.h"


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
CSG_Grid * SG_Create_Grid(const CSG_String &File_Name, TSG_Data_Type Type, TSG_Grid_Memory_Type Memory_Type, bool bLoadData)
{
	CSG_Grid	*pGrid	= new CSG_Grid(File_Name, Type, Memory_Type, bLoadData);

	if( pGrid->is_Valid() )
	{
		return( pGrid );
	}

	delete(pGrid);

	return( NULL );
}

//---------------------------------------------------------
CSG_Grid * SG_Create_Grid(CSG_Grid *pGrid, TSG_Data_Type Type, TSG_Grid_Memory_Type Memory_Type)
{
	return( new CSG_Grid(pGrid, Type, Memory_Type) );
}

//---------------------------------------------------------
CSG_Grid * SG_Create_Grid(const CSG_Grid_System &System, TSG_Data_Type Type, TSG_Grid_Memory_Type Memory_Type)
{
	CSG_Grid	*pGrid	= new CSG_Grid(System, Type, Memory_Type);

	if( pGrid && !pGrid->is_Valid() )
	{
		delete(pGrid);

		pGrid	= NULL;
	}

	return( pGrid );
}

//---------------------------------------------------------
CSG_Grid * SG_Create_Grid(TSG_Data_Type Type, int NX, int NY, double Cellsize, double xMin, double yMin, TSG_Grid_Memory_Type Memory_Type)
{
	CSG_Grid	*pGrid	= new CSG_Grid(Type, NX, NY, Cellsize, xMin, yMin, Memory_Type);

	if( pGrid && !pGrid->is_Valid() )
	{
		delete(pGrid);

		pGrid	= NULL;
	}

	return( pGrid );
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
CSG_Grid::CSG_Grid(const CSG_String &File_Name, TSG_Data_Type Type, TSG_Grid_Memory_Type Memory_Type, bool bLoadData)
	: CSG_Data_Object()
{
	_On_Construction();

	Create(File_Name, Type, Memory_Type, bLoadData);
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

	m_LineBuffer		= NULL;
	m_LineBuffer_Count	= 5;

	m_zScale			= 1.0;
	m_zOffset			= 0.0;

	m_Index				= NULL;
	m_bIndex			= false;

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
bool CSG_Grid::Create(CSG_Grid *pGrid, TSG_Data_Type Type, TSG_Grid_Memory_Type Memory_Type)
{
	if( pGrid && Create(Type, pGrid->Get_NX(), pGrid->Get_NY(), pGrid->Get_Cellsize(), pGrid->Get_XMin(), pGrid->Get_YMin(), Memory_Type) )
	{
		Set_NoData_Value_Range(pGrid->Get_NoData_Value(), pGrid->Get_NoData_hiValue());

		Get_Projection()	= pGrid->Get_Projection();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Grid::Create(const CSG_Grid_System &System, TSG_Data_Type Type, TSG_Grid_Memory_Type Memory_Type)
{
	return( Create(Type, System.Get_NX(), System.Get_NY(), System.Get_Cellsize(), System.Get_XMin(), System.Get_YMin(), Memory_Type) );
}

//---------------------------------------------------------
bool CSG_Grid::Create(const CSG_String &File_Name, TSG_Data_Type Type, TSG_Grid_Memory_Type Memory_Type, bool bLoadData)
{
	Destroy();

	SG_UI_Msg_Add(CSG_String::Format("%s: %s...", _TL("Load grid"), File_Name.c_str()), true);

	//-----------------------------------------------------
	bool	bResult	= File_Name.BeforeFirst(':').Cmp("PGSQL") && SG_File_Exists(File_Name) && _Load(File_Name, Type, Memory_Type, bLoadData);

	if( bResult )
	{
		// nop
	}

	//-----------------------------------------------------
	else if( File_Name.BeforeFirst(':').Cmp("PGSQL") == 0 )	// database source
	{
		CSG_String	s(File_Name);

		s	= s.AfterFirst(':');	CSG_String	Host  (s.BeforeFirst(':'));
		s	= s.AfterFirst(':');	CSG_String	Port  (s.BeforeFirst(':'));
		s	= s.AfterFirst(':');	CSG_String	DBName(s.BeforeFirst(':'));
		s	= s.AfterFirst(':');	CSG_String	Table (s.BeforeFirst(':'));
		s	= s.AfterFirst(':');	CSG_String	rid   (s.BeforeFirst(':').AfterFirst('='));

		CSG_Module	*pModule	= SG_Get_Module_Library_Manager().Get_Module("db_pgsql", 0);	// CGet_Connections

		if(	pModule != NULL )
		{
			SG_UI_ProgressAndMsg_Lock(true);

			//---------------------------------------------
			CSG_Table	Connections;
			CSG_String	Connection	= DBName + " [" + Host + ":" + Port + "]";

			pModule->Settings_Push();

			if( pModule->On_Before_Execution() && SG_MODULE_PARAMETER_SET("CONNECTIONS", &Connections) && pModule->Execute() )	// CGet_Connections
			{
				for(int i=0; !bResult && i<Connections.Get_Count(); i++)
				{
					if( !Connection.Cmp(Connections[i].asString(0)) )
					{
						bResult	= true;
					}
				}
			}

			pModule->Settings_Pop();

			//---------------------------------------------
			if( bResult && (bResult = (pModule = SG_Get_Module_Library_Manager().Get_Module("db_pgsql", 33)) != NULL) == true )	// CPGIS_Raster_Load_Band
			{
				pModule->Settings_Push();

				bResult	= pModule->On_Before_Execution()
					&& SG_MODULE_PARAMETER_SET("CONNECTION", Connection)
					&& SG_MODULE_PARAMETER_SET("TABLES"    , Table)
					&& SG_MODULE_PARAMETER_SET("RID"       , rid)
					&& SG_MODULE_PARAMETER_SET("GRID"      , this)
					&& pModule->Execute();

				pModule->Settings_Pop();
			}

			SG_UI_ProgressAndMsg_Lock(false);
		}
	}

	//-----------------------------------------------------
	if( bResult )
	{
		m_bCreated	= true;

		Set_Modified(false);
		Set_Update_Flag();

		SG_UI_Msg_Add(_TL("okay"), false, SG_UI_MSG_STYLE_SUCCESS);

		return( true );
	}

	//-----------------------------------------------------
	Destroy();

	SG_UI_Msg_Add(_TL("failed"), false, SG_UI_MSG_STYLE_FAILURE);

	return( false );
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

	m_zScale		= 1.0;
	m_zOffset		= 0.0;

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
	case SG_DATATYPE_Byte:		Set_NoData_Value(          0.0);	break;
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
void CSG_Grid::Set_Unit(const SG_Char *String)
{
	m_Unit	= String ? String : SG_T("");
}

const SG_Char * CSG_Grid::Get_Unit(void) const
{
	return( m_Unit.c_str() );
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
		switch( m_Memory_Type )
		{
		default:
			return( m_Values != NULL );

		case GRID_MEMORY_Cache:
			return( m_Cache_Stream.is_Open() );
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
double CSG_Grid::Get_Value(TSG_Point Position, int Interpolation, bool bByteWise, bool bOnlyValidCells) const
{
	double	Value;

	return( Get_Value(Position.x, Position.y, Value, Interpolation, bByteWise, bOnlyValidCells) ? Value : Get_NoData_Value() );
}

double CSG_Grid::Get_Value(double xPosition, double yPosition, int Interpolation, bool bByteWise, bool bOnlyValidCells) const
{
	double	Value;

	return( Get_Value(xPosition, yPosition, Value, Interpolation, bByteWise, bOnlyValidCells) ? Value : Get_NoData_Value() );
}

bool CSG_Grid::Get_Value(TSG_Point Position, double &Value, int Interpolation, bool bByteWise, bool bOnlyValidCells) const
{
	return( Get_Value(Position.x, Position.y, Value, Interpolation, bByteWise, bOnlyValidCells) );
}

//---------------------------------------------------------
bool CSG_Grid::Get_Value(double xPosition, double yPosition, double &Value, int Interpolation, bool bByteWise, bool bOnlyValidCells) const
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

			return( !is_NoData_Value(Value) );
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
		sLong	v;
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
			sLong	v;
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
	int		ix, iy, jx, jy, nNoData	= 0;

	//-----------------------------------------------------
	for(iy=0, jy=y-1; iy<4; iy++, jy++)
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
	if( nNoData > 0 && nNoData < 16 )	// guess missing values as average of surrounding data values
	{
		for(int i=0; nNoData>0 && i<16; i++)	// avoid the possibility of endless loop
		{
			double	t_xy[4][4];

			for(iy=0; iy<4; iy++)
			{
				for(ix=0; ix<4; ix++)
				{
					t_xy[ix][iy]	= z_xy[ix][iy];

					if( is_NoData_Value(z_xy[ix][iy]) )
					{
						int		n	= 0;
						double	s	= 0.0;

						for(jy=iy-1; jy<=iy+1; jy++)
						{
							if( jy >= 0 && jy < 4 )
							{
								for(jx=ix-1; jx<=ix+1; jx++)
								{
									if( jx >= 0 && jx < 4 && !is_NoData_Value(z_xy[jx][jy]) )
									{
										s	+= z_xy[jx][jy];
										n	++;
									}
								}
							}
						}

						if( n > 0 )
						{
							t_xy[ix][iy]	= s / n;

							nNoData--;
						}
					}
				}
			}

			for(iy=0; iy<4; iy++)
			{
				for(ix=0; ix<4; ix++)
				{
					z_xy[ix][iy]	= t_xy[ix][iy];
				}
			}
		}
	}

	//-----------------------------------------------------
	return( nNoData == 0 );
}

//---------------------------------------------------------
inline bool CSG_Grid::_Get_ValAtPos_Fill4x4Submatrix(int x, int y, double z_xy[4][4][4]) const
{
	int		ix, iy, jx, jy, nNoData	= 0;

	//-----------------------------------------------------
	for(iy=0, jy=y-1; iy<4; iy++, jy++)
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
	if( nNoData > 0 && nNoData < 16 )	// guess missing values as average of surrounding data values
	{
		for(int i=0; nNoData>0 && i<16; i++)	// avoid the possibility of endless loop
		{
			double	t_xy[4][4][4];

			for(iy=0; iy<4; iy++)
			{
				for(ix=0; ix<4; ix++)
				{
					t_xy[0][ix][iy]	 = z_xy[0][ix][iy];
					t_xy[1][ix][iy]	 = z_xy[1][ix][iy];
					t_xy[2][ix][iy]	 = z_xy[2][ix][iy];
					t_xy[3][ix][iy]	 = z_xy[3][ix][iy];

					if( is_NoData_Value(z_xy[0][ix][iy]) )
					{
						int		n	= 0;
						double	s[4]; s[0] = s[1] = s[2] = s[3] = 0;

						for(jy=iy-1; jy<=iy+1; jy++)
						{
							if( jy >= 0 && jy < 4 )
							{
								for(jx=ix-1; jx<=ix+1; jx++)
								{
									if( jx >= 0 && jx < 4 && !is_NoData_Value(z_xy[0][jx][jy]) )
									{
										s[0]	+= z_xy[0][jx][jy];
										s[1]	+= z_xy[1][jx][jy];
										s[2]	+= z_xy[2][jx][jy];
										s[3]	+= z_xy[3][jx][jy];
										n		++;
									}
								}
							}
						}

						if( n > 0 )
						{
							t_xy[0][ix][iy]	= s[0] / n;
							t_xy[1][ix][iy]	= s[1] / n;
							t_xy[2][ix][iy]	= s[2] / n;
							t_xy[3][ix][iy]	= s[3] / n;

							nNoData--;
						}
					}
				}
			}

			for(iy=0; iy<4; iy++)
			{
				for(ix=0; ix<4; ix++)
				{
					z_xy[0][ix][iy]	= t_xy[0][ix][iy];
					z_xy[1][ix][iy]	= t_xy[1][ix][iy];
					z_xy[2][ix][iy]	= t_xy[2][ix][iy];
					z_xy[3][ix][iy]	= t_xy[3][ix][iy];
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
		m_zStats.Invalidate();

		for(int y=0; y<Get_NY() && SG_UI_Process_Get_Okay(); y++)
		{
			for(int x=0; x<Get_NX(); x++)
			{
				if( !is_NoData(x, y) )
				{
					m_zStats.Add_Value(asDouble(x, y));
				}
			}
		}

		m_bIndex	= false;
		SG_FREE_SAFE(m_Index);
	}

	return( true );
}

//---------------------------------------------------------
double CSG_Grid::Get_ZMin(void)
{
	Update();	return( m_zStats.Get_Minimum() );
}

double CSG_Grid::Get_ZMax(void)
{
	Update();	return( m_zStats.Get_Maximum() );
}

double CSG_Grid::Get_ZRange(void)
{
	Update();	return( m_zStats.Get_Range() );
}

double CSG_Grid::Get_Mean(void)
{
	Update();	return( m_zStats.Get_Mean() );
}

double CSG_Grid::Get_StdDev(void)
{
	Update();	return( m_zStats.Get_StdDev() );
}

double CSG_Grid::Get_Variance(void)
{
	Update();	return( m_zStats.Get_Variance() );
}

//---------------------------------------------------------
sLong CSG_Grid::Get_Data_Count(void)
{
	Update();	return( m_zStats.Get_Count() );
}

sLong CSG_Grid::Get_NoData_Count(void)
{
	Update();	return( Get_NCells() - m_zStats.Get_Count() );
}

//---------------------------------------------------------
double CSG_Grid::Get_Percentile(double Percent)
{
	Percent	= Percent <= 0.0 ? 0.0 : Percent >= 100.0 ? 1.0 : Percent / 100.0;

	sLong	n	= (sLong)(Percent * (Get_Data_Count() - 1));

	if( Get_Sorted(n, n, false) )
	{
		return( asDouble(n) );
	}

	return( Get_NoData_Value() );
}


///////////////////////////////////////////////////////////
//														 //
//						Index							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define SORT_SWAP(a,b)	{itemp=(a);(a)=(b);(b)=itemp;}

bool CSG_Grid::_Set_Index(void)
{
	if( Get_Data_Count() <= 0 )
	{
		return( false );	// nothing to do
	}

	//-----------------------------------------------------
	if( m_Index == NULL && (m_Index = (sLong *)SG_Malloc(Get_NCells() * sizeof(sLong))) == NULL )
	{
		SG_UI_Msg_Add_Error(_TL("could not create index: insufficient memory"));

		return( false );
	}

	//-----------------------------------------------------
	const sLong	M	= 7;

	sLong	i, j, k, l, ir, n, *istack, jstack, nstack, indxt, itemp;
	double	a;

	//-----------------------------------------------------
	SG_UI_Process_Set_Text(CSG_String::Format(SG_T("%s: %s"), _TL("Create index"), Get_Name()));

	for(i=0, j=0, k=Get_Data_Count(); i<Get_NCells(); i++)
	{
		if( is_NoData(i) )
		{
			m_Index[k++]	= i;
		}
		else // if( !is_NoData(i) )
		{
			m_Index[j++]	= i;
		}
	}

	//-----------------------------------------------------
	l		= 0;
	n		= 0;
	ir		= Get_Data_Count() - 1;

	nstack	= 64;
	istack	= (sLong *)SG_Malloc(nstack * sizeof(sLong));
	jstack	= 0;

	for(;;)
	{
		if( ir - l < M )
		{
			if( !SG_UI_Process_Set_Progress((double)(n += M - 1), (double)Get_Data_Count()) )
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
				istack	= (sLong *)SG_Realloc(istack, nstack * sizeof(int));
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

	m_bIndex	= true;

	return( true );
}
#undef SORT_SWAP


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

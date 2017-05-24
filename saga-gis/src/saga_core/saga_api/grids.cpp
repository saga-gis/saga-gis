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
//                      grids.cpp                        //
//                                                       //
//                 Copyright (C) 2017 by                 //
//                      Olaf Conrad                      //
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
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "saga_api.h"
#include "grids.h"
#include "data_manager.h"
#include "tool_library.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Grids * SG_Create_Grids(void)
{
	return( new CSG_Grids );
}

//---------------------------------------------------------
CSG_Grids * SG_Create_Grids(const CSG_Grids &Grids)
{
	return( new CSG_Grids(Grids) );
}

//---------------------------------------------------------
CSG_Grids * SG_Create_Grids(const CSG_String &FileName, bool bLoadData)
{
	CSG_Grids	*pGrids	= new CSG_Grids(FileName, bLoadData);

	if( pGrids->is_Valid() )
	{
		return( pGrids );
	}

	delete(pGrids);

	return( NULL );
}

//---------------------------------------------------------
CSG_Grids * SG_Create_Grids(int NX, int NY, int NZ, double Cellsize, double xMin, double yMin, double zMin, TSG_Data_Type Type)
{
	return( new CSG_Grids(NX, NY, NZ, Cellsize, xMin, yMin, zMin, Type) );
}

//---------------------------------------------------------
CSG_Grids * SG_Create_Grids(const CSG_Grid_System &System, int NZ, double zMin, TSG_Data_Type Type)
{
	return( new CSG_Grids(System, NZ, zMin, Type) );
}

//---------------------------------------------------------
CSG_Grids * SG_Create_Grids(const CSG_Grid_System &System, const CSG_Table &Attributes, int zAttribute, TSG_Data_Type Type)
{
	return( new CSG_Grids(System, Attributes, zAttribute, Type) );
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
CSG_Grids::CSG_Grids(void)
{
	_On_Construction();
}

//---------------------------------------------------------
/**
  * The destructor.
*/
CSG_Grids::~CSG_Grids(void)
{
	Destroy();

	delete(m_pGrids[0]);	// The Dummy
}

//---------------------------------------------------------
/**
  * Copy constructor.
*/
//---------------------------------------------------------
CSG_Grids::CSG_Grids(const CSG_Grids &Grid)
{
	_On_Construction();

	Create(Grid);
}

//---------------------------------------------------------
/**
  * Create a grid collection from file.
*/
//---------------------------------------------------------
CSG_Grids::CSG_Grids(const CSG_String &FileName, bool bLoadData)
{
	_On_Construction();

	Create(FileName, bLoadData);
}

//---------------------------------------------------------
/**
  * Create a grid collection with specified dimensions'.
*/
//---------------------------------------------------------
CSG_Grids::CSG_Grids(int NX, int NY, int NZ, double Cellsize, double xMin, double yMin, double zMin, TSG_Data_Type Type)
{
	_On_Construction();

	Create(NX, NY, NZ, Cellsize, xMin, yMin, zMin, Type);
}

//---------------------------------------------------------
/**
  * Create a grid collection with specified dimensions'.
*/
//---------------------------------------------------------
CSG_Grids::CSG_Grids(const CSG_Grid_System &System, int NZ, double zMin, TSG_Data_Type Type)
{
	_On_Construction();

	Create(System, NZ, zMin, Type);
}

//---------------------------------------------------------
/**
  * Create a grid collection with specified dimensions'.
*/
//---------------------------------------------------------
CSG_Grids::CSG_Grids(const CSG_Grid_System &System, const CSG_Table &Attributes, int zAttribute, TSG_Data_Type Type)
{
	_On_Construction();

	Create(System, Attributes, zAttribute, Type);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Grids::_On_Construction(void)
{
	m_pGrids	= (CSG_Grid **)m_Grids.Create(1);
	m_pGrids[0]	= SG_Create_Grid();	// The Dummy
	m_pGrids[0]->Set_Owner(this);

	Destroy();

	Set_Update_Flag();
}

//---------------------------------------------------------
/**
  * Destroys the data space of the CSG_Grids object.
*/
bool CSG_Grids::Destroy(void)
{
	for(size_t i=1; i<m_Grids.Get_Size(); i++)
	{
		delete(m_pGrids[i]);	// do not delete the dummy before deconstruction
	}

	m_pGrids	= (CSG_Grid **)m_Grids.Get_Array(1);

	m_pGrids[0]->Destroy();	// The Dummy

	m_Attributes.Destroy();
	m_Attributes.Add_Field("Z", SG_DATATYPE_Double);

	m_Z_Attribute	= 0;

	return( CSG_Data_Object::Destroy() );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grids::Create(const CSG_Grids &Grids)
{
	if( Grids.is_Valid() && Create(Grids.Get_System(), Grids.Get_Attributes(), Grids.Get_Z_Attribute(), Grids.Get_Type()) )
	{
		for(size_t i=0; i<Grids.m_Grids.Get_Size(); i++)
		{
			m_pGrids[i]->Assign(Grids.m_pGrids[i], GRID_RESAMPLING_NearestNeighbour);
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Grids::Create(const CSG_String &FileName, bool bLoadData)
{
	return( Load(FileName, bLoadData) );
}

//---------------------------------------------------------
bool CSG_Grids::Create(const CSG_Grid_System &System, int NZ, double zMin, TSG_Data_Type Type)
{
	return( Create(System.Get_NX(), System.Get_NY(), NZ, System.Get_Cellsize(), System.Get_XMin(), System.Get_YMin(), zMin, Type) );
}

//---------------------------------------------------------
bool CSG_Grids::Create(const CSG_Grid_System &System, const CSG_Table &Attributes, int zAttribute, TSG_Data_Type Type)
{
	if( Create(System.Get_NX(), System.Get_NY(), Attributes.Get_Count(), System.Get_Cellsize(), System.Get_XMin(), System.Get_YMin(), Attributes.Get_Minimum(zAttribute), Type) )
	{
		if( Attributes.Get_Field_Count() > 0 )
		{
			m_Attributes.Create(Attributes);

			Set_Z_Attribute(zAttribute);
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Grids::Create(int NX, int NY, int NZ, double Cellsize, double xMin, double yMin, double zMin, TSG_Data_Type Type)
{
	Destroy();

	m_pGrids[0]->Create(Type, NX, NY, Cellsize, xMin, yMin);

	for(int i=0; i<NZ; i++, zMin+=Cellsize)
	{
		if( !Add_Grid(zMin) )
		{
			return( false );
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Header							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Grids::Set_Unit(const CSG_String &Unit)
{
	m_pGrids[0]->Set_Unit(Unit);
}

//---------------------------------------------------------
void CSG_Grids::Set_Scaling(double Scale, double Offset)
{
	for(int i=0; i<Get_Grid_Count(); i++)
	{
		m_pGrids[i]->Set_Scaling(Scale, Offset);
	}

	Set_Update_Flag();
}

//---------------------------------------------------------
bool CSG_Grids::Set_NoData_Value_Range	(double loValue, double hiValue)
{
	if( CSG_Data_Object::Set_NoData_Value_Range(loValue, hiValue) )	// this is a CSG_Data_Object base class property
	{
		for(int i=0; i<Get_Grid_Count(); i++)
		{
			m_pGrids[i]->Set_NoData_Value_Range(loValue, hiValue);
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CSG_Grids::_Synchronize(CSG_Grid *pGrid)
{
	pGrid->Set_Owner(this);

	if( pGrid == m_pGrids[0] )
	{
		Set_Scaling(pGrid->Get_Scaling(), pGrid->Get_Offset());
		Set_NoData_Value_Range(pGrid->Get_NoData_Value(), pGrid->Get_NoData_hiValue());
	}
	else // if( pGrid != m_pGrids[0] )
	{
		pGrid->Set_Scaling(Get_Scaling(), Get_Offset());
		pGrid->Set_NoData_Value_Range(Get_NoData_Value(), Get_NoData_hiValue());
	}
}


///////////////////////////////////////////////////////////
//														 //
//						Checks							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grids::is_Valid(void) const
{
	return( Get_System().is_Valid() );
}

//---------------------------------------------------------
bool CSG_Grids::is_Compatible(CSG_Grid *pGrid) const
{
	return( pGrid && is_Compatible(pGrid->Get_System()) && Get_Type() == pGrid->Get_Type() );
}

bool CSG_Grids::is_Compatible(CSG_Grids *pGrids) const
{
	return( pGrids && is_Compatible(pGrids->Get_System()) && Get_NZ() == pGrids->Get_NZ() );//&& Get_Type() == pGrids->Get_Type() );
}

bool CSG_Grids::is_Compatible(const CSG_Grid_System &System) const
{
	return( Get_System() == System );
}

bool CSG_Grids::is_Compatible(int NX, int NY, double Cellsize, double xMin, double yMin) const
{
	return(	is_Compatible(CSG_Grid_System(Cellsize, xMin, yMin, NX, NY)) );
}


///////////////////////////////////////////////////////////
//														 //
//						Attributes						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grids::Add_Attribute(const CSG_String &Name, TSG_Data_Type Type, int i)
{
	return( m_Attributes.Add_Field(Name, Type, i) );
}

//---------------------------------------------------------
bool CSG_Grids::Del_Attribute(int i)
{
	if( i != m_Z_Attribute && m_Attributes.Get_Field_Count() > 0 && m_Attributes.Del_Field(i) )
	{
		if( m_Z_Attribute > i )
		{
			m_Z_Attribute--;
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Grids::Set_Z_Attribute(int i)
{
	if( i >= 0 && i < m_Attributes.Get_Field_Count() )
	{
		m_Z_Attribute	= i;

		return( Update_Z_Order() );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Grids::Set_Z(int i, double Value)
{
	return( i >= 0 && i < Get_NZ() && m_Attributes[i].Set_Value(m_Z_Attribute, Value) );
}

//---------------------------------------------------------
bool CSG_Grids::Update_Z_Order(void)
{
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grids::Add_Grid(double Z)
{
	if( !Get_System().is_Valid() )	// only allowed for initialized grid systems
	{
		return( false );
	}

	//-----------------------------------------------------
	int	n	= Get_NZ();

	if( n > 0 )	// else use dummy grid (m_pGrids[0] is always present)
	{
		CSG_Grid	*pGrid	= SG_Create_Grid(Get_System(), Get_Type());

		if( !pGrid )
		{
			return( false );
		}

		m_pGrids	= (CSG_Grid **)m_Grids.Get_Array(n + 1);
		m_pGrids[n]	= pGrid;

		_Synchronize(pGrid);
	}

	//-----------------------------------------------------
	m_Attributes.Add_Record()->Set_Value(m_Z_Attribute, Z);

	m_pGrids[n]->Set_Name(CSG_String::Format("%s [%s]", Get_Name(), SG_Get_String(Z, -10).c_str()));

	return( true );
}

//---------------------------------------------------------
bool CSG_Grids::Add_Grid(double Z, CSG_Grid *pGrid, bool bAttach)
{
	if( !pGrid || !pGrid->is_Valid() )
	{
		return( false );
	}

	if( Get_NZ() > 0 && !is_Compatible(pGrid) )	// not allowed
	{
		return( false );
	}

	//-----------------------------------------------------
	int	n	= Get_NZ();

	if( n > 0 )
	{
		if( !bAttach && (pGrid = SG_Create_Grid(*pGrid)) == NULL )	// get a copy
		{
			return( false );
		}

		m_pGrids	= (CSG_Grid **)m_Grids.Get_Array(n + 1);
		m_pGrids[n]	= pGrid;
	}
	else if( bAttach ) // if( n == 0 )
	{
		delete(m_pGrids[0]);
			
		m_pGrids[0]	= pGrid;	// simply replace dummy
	}
	else if( m_pGrids[0]->Create(*pGrid) )	// make dummy a copy of grid
	{
		pGrid	= m_pGrids[0];
	}
	else
	{
		return( false );
	}

	_Synchronize(pGrid);

	//-----------------------------------------------------
	m_Attributes.Add_Record()->Set_Value(m_Z_Attribute, Z);

	m_pGrids[n]->Set_Name(CSG_String::Format("%s [%s]", Get_Name(), pGrid->Get_Name()));

	return( true );
}

//---------------------------------------------------------
bool CSG_Grids::Del_Grid(int i, bool bDetach)
{
	if( i >= 0 && i < Get_NZ() && m_Attributes.Del_Record(i) )	// Get_NZ() is now decreased by one
	{
		if( Get_NZ() > 0 )
		{
			if( bDetach )
			{
				m_pGrids[i]->Set_Owner(NULL);
			}
			else
			{
				delete(m_pGrids[i]);
			}

			for( ; i<Get_NZ(); i++)
			{
				m_pGrids[i]	= m_pGrids[i + 1];
			}

			m_pGrids	= (CSG_Grid **)m_Grids.Get_Array(Get_NZ());
		}
		else if( bDetach ) // if( Get_NZ() == 0 )
		{
			m_pGrids[0]->Set_Owner(NULL);
			m_pGrids[0]	= SG_Create_Grid(*m_pGrids[0]);	// needs a new dummy
			m_pGrids[0]->Set_Owner(this);
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Grids::Del_Grids(void)
{
	for(int i=1; i<Get_NZ(); i++)
	{
		delete(m_pGrids[i]);	// do not delete the dummy before deconstruction
	}

	m_pGrids	= (CSG_Grid **)m_Grids.Get_Array(1);

	m_Attributes.Del_Records();

	return( true );
}

//---------------------------------------------------------
bool CSG_Grids::Set_Grid_Count(int Count)
{
	if( Count < 0 || !Get_System().is_Valid() )	// only allowed for initialized grid systems)
	{
		return( false );
	}

	if( Count == 0 )
	{
		return( Del_Grids() );
	}

	//-----------------------------------------------------
	if( Count < Get_NZ() )
	{
		for(int i=Count; i<Get_NZ(); i++)
		{
			delete(m_pGrids[i]);
		}

		m_pGrids	= (CSG_Grid **)m_Grids.Get_Array(Count);

		m_Attributes.Set_Record_Count(Count);
	}

	//-----------------------------------------------------
	else if( Count > Get_NZ() )
	{
		double	z	= Get_ZMax();

		for(int i=Get_NZ(); i<=Count; i++, z+=Get_Cellsize())
		{
			if( !Add_Grid(z) )
			{
				return( false );
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_Grids::Assign_NoData(void)
{
	for(int i=0; i<Get_Grid_Count(); i++)
	{
		m_pGrids[i]->Assign_NoData();
	}
}

//---------------------------------------------------------
bool CSG_Grids::Assign(double Value)
{
	for(int i=0; i<Get_Grid_Count(); i++)
	{
		m_pGrids[i]->Assign(Value);
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Grids::Assign(CSG_Data_Object *pObject)
{
	if( pObject )
	{
		switch( pObject->Get_ObjectType() )
		{
		case SG_DATAOBJECT_TYPE_Grid:
			{
				bool	bResult	= true;

				for(int i=0; i<Get_Grid_Count(); i++)
				{
					if( !m_pGrids[i]->Assign((CSG_Grid *)pObject) )
					{
						bResult	= false;
					}
				}

				return( bResult );
			}

		case SG_DATAOBJECT_TYPE_Grids:
			return( Assign((CSG_Grids *)pObject) );

		default:
			break;
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Grids::Assign(CSG_Grids *pGrids, TSG_Grid_Resampling Interpolation)
{
	if( pGrids && Get_Grid_Count() == pGrids->Get_Grid_Count() )
	{
		bool	bResult	= true;

		for(int i=0; i<Get_Grid_Count(); i++)
		{
			if( !m_pGrids[i]->Assign(pGrids->m_pGrids[i], Interpolation) )
			{
				bResult	= false;
			}
		}

		return( bResult );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Grids & CSG_Grids::operator = (const CSG_Grids &Grids)
{
	Create(Grids); return( *this );
}

CSG_Grids & CSG_Grids::operator = (double Value)
{
	Assign(Value); return( *this );
}

//---------------------------------------------------------
CSG_Grids & CSG_Grids::operator += (double Value)
{
	return( Add(Value) );
}

CSG_Grids & CSG_Grids::Add(double Value)
{
	for(int i=0; i<Get_Grid_Count(); i++)
	{
		m_pGrids[i]->Add(Value);
	}

	return( *this );
}

//---------------------------------------------------------
CSG_Grids & CSG_Grids::operator -= (double Value)
{
	return( Subtract(Value) );
}

CSG_Grids & CSG_Grids::Subtract(double Value)
{
	for(int i=0; i<Get_Grid_Count(); i++)
	{
		m_pGrids[i]->Subtract(Value);
	}

	return( *this );
}

//---------------------------------------------------------
CSG_Grids & CSG_Grids::operator *= (double Value)
{
	return( Multiply(Value) );
}

CSG_Grids & CSG_Grids::Multiply(double Value)
{
	for(int i=0; i<Get_Grid_Count(); i++)
	{
		m_pGrids[i]->Multiply(Value);
	}

	return( *this );
}

//---------------------------------------------------------
CSG_Grids & CSG_Grids::operator /= (double Value)
{
	return( Divide(Value) );
}

CSG_Grids & CSG_Grids::Divide(double Value)
{
	for(int i=0; i<Get_Grid_Count(); i++)
	{
		m_pGrids[i]->Divide(Value);
	}

	return( *this );
}


///////////////////////////////////////////////////////////
//														 //
//		Value access by Position (-> Interpolation)		 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CSG_Grids::Get_Value(const TSG_Point_Z &p, TSG_Grid_Resampling Resampling) const
{
	double	Value;

	return( Get_Value(p.x, p.y, p.z, Value, Resampling) ? Value : Get_NoData_Value() );
}

double CSG_Grids::Get_Value(double x, double y, double z, TSG_Grid_Resampling Resampling) const
{
	double	Value;

	return( Get_Value(x, y, z, Value, Resampling) ? Value : Get_NoData_Value() );
}

bool CSG_Grids::Get_Value(const TSG_Point_Z &p, double &Value, TSG_Grid_Resampling Resampling) const
{
	return( Get_Value(p.x, p.y, p.z, Value, Resampling) );
}

//---------------------------------------------------------
bool CSG_Grids::Get_Value(double x, double y, double z, double &Value, TSG_Grid_Resampling Resampling) const
{
	if(	Get_System().Get_Extent(true).Contains(x, y) )
	{
		int ix = (int)floor(x = (x - Get_XMin()) / Get_Cellsize()); double dx = x - ix;
		int iy = (int)floor(y = (y - Get_YMin()) / Get_Cellsize()); double dy = y - iy;
		int iz = (int)floor(z = (z - Get_ZMin()) / Get_Cellsize()); double dz = z - iz;

		if( is_InGrid(ix + (int)(0.5 + dx), iy + (int)(0.5 + dy), iz + (int)(0.5 + dz)) )
		{
			switch( Resampling )
			{
			case GRID_RESAMPLING_NearestNeighbour:
				Value	= _Get_ValAtPos_NearestNeighbour(ix, iy, iz, dx, dy, dz);
				break;

			case GRID_RESAMPLING_Bilinear:
				Value	= _Get_ValAtPos_BiLinear        (ix, iy, iz, dx, dy, dz);
				break;

			case GRID_RESAMPLING_BicubicSpline:
				Value	= _Get_ValAtPos_BiCubicSpline   (ix, iy, iz, dx, dy, dz);
				break;

			case GRID_RESAMPLING_BSpline: default:
				Value	= _Get_ValAtPos_BSpline         (ix, iy, iz, dx, dy, dz);
				break;
			}

			return( !is_NoData_Value(Value) );
		}
	}

	return( false );
}

//---------------------------------------------------------
inline double CSG_Grids::_Get_ValAtPos_NearestNeighbour(int x, int y, int z, double dx, double dy, double dz) const
{
	x	+= (int)(0.5 + dx);
	y	+= (int)(0.5 + dy);
	z	+= (int)(0.5 + dz);

	if( is_InGrid(x, y, z) )
	{
		return( asDouble(x, y, z) );
	}

	return( Get_NoData_Value() );
}

//---------------------------------------------------------
#define BILINEAR_ADD(ix, iy, iz, d)	if( is_InGrid(ix, iy, iz) ) { n += d; s += d * asDouble(ix, iy, iz); }

inline double CSG_Grids::_Get_ValAtPos_BiLinear(int x, int y, int z, double dx, double dy, double dz) const
{
	double	s = 0.0, n = 0.0;

	BILINEAR_ADD(x    , y    , z    , (1.0 - dx) * (1.0 - dy) * (1.0 - dz));
	BILINEAR_ADD(x + 1, y    , z    , (      dx) * (1.0 - dy) * (1.0 - dz));
	BILINEAR_ADD(x    , y + 1, z    , (1.0 - dx) * (      dy) * (1.0 - dz));
	BILINEAR_ADD(x + 1, y + 1, z    , (      dx) * (      dy) * (1.0 - dz));

	BILINEAR_ADD(x    , y    , z + 1, (1.0 - dx) * (1.0 - dy) * (      dz));
	BILINEAR_ADD(x + 1, y    , z + 1, (      dx) * (1.0 - dy) * (      dz));
	BILINEAR_ADD(x    , y + 1, z + 1, (1.0 - dx) * (      dy) * (      dz));
	BILINEAR_ADD(x + 1, y + 1, z + 1, (      dx) * (      dy) * (      dz));

	if( n > 0.0 )
	{
		return( s / n );
	}

	return( Get_NoData_Value() );
}

//---------------------------------------------------------
inline double CSG_Grids::_Get_ValAtPos_BiCubicSpline(int x, int y, int z, double dx, double dy, double dz) const
{
	double	v_xyz[4][4][4], v_xy[4], v_x[4];

	if( !_Get_ValAtPos_Fill4x4Submatrix(x, y, z, v_xyz) )
	{
		return( Get_NoData_Value() );
	}

	#define BiCubicSpline(d, v) (v[1] + 0.5 * d * (v[2] - v[0] + d * (2 * v[0] - 5 * v[1] + 4 * v[2] - v[3] + d * (3 * (v[1] - v[2]) + v[3] - v[0]))))

	for(int ix=0; ix<4; ix++)
	{
		for(int iy=0; iy<4; iy++)
		{
			v_xy[iy]	= BiCubicSpline(dz, v_xyz[ix][iy]);
		}

		v_x[ix]	= BiCubicSpline(dy, v_xy);
	}

	return( BiCubicSpline(dx, v_x) );
}

//---------------------------------------------------------
inline double CSG_Grids::_Get_ValAtPos_BSpline(int x, int y, int z, double dx, double dy, double dz) const
{
	double	v_xyz[4][4][4];

	if( !_Get_ValAtPos_Fill4x4Submatrix(x, y, z, v_xyz) )
	{
		return( Get_NoData_Value() );
	}

	double	Rx[4], Ry[4], Rz[4];

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

		s	= 0.0;
		if( (d = i - dz + 1.0) > 0.0 )	s	+=        d*d*d;
		if( (d = i - dz + 0.0) > 0.0 )	s	+= -4.0 * d*d*d;
		if( (d = i - dz - 1.0) > 0.0 )	s	+=  6.0 * d*d*d;
		if( (d = i - dz - 2.0) > 0.0 )	s	+= -4.0 * d*d*d;
	//	if( (d = i - dz - 3.0) > 0.0 )	s	+=        d*d*d;
		Rz[i]	= s / 6.0;
	}

	double	s	= 0.0;

	for(int iz=0; iz<4; iz++)
	{
		for(int iy=0; iy<4; iy++)
		{
			for(int ix=0; ix<4; ix++)
			{
				s	+= v_xyz[ix][iy][iz] * Rx[ix] * Ry[iy] * Rz[iz];
			}
		}
	}

	return( s );
}

//---------------------------------------------------------
inline bool CSG_Grids::_Get_ValAtPos_Fill4x4Submatrix(int x, int y, int z, double v_xyz[4][4][4]) const
{
	int		ix, iy, iz, jx, jy, jz, nNoData	= 0;

	//-----------------------------------------------------
	for(iz=0, jz=z-1; iz<4; iz++, jz++)
	{
		for(iy=0, jy=y-1; iy<4; iy++, jy++)
		{
			for(ix=0, jx=x-1; ix<4; ix++, jx++)
			{
				if( is_InGrid(jx, jy, jz) )
				{
					v_xyz[ix][iy][iz]	= asDouble(jx, jy, jz);
				}
				else
				{
					v_xyz[ix][iy][iz]	= Get_NoData_Value();

					nNoData++;
				}
			}
		}
	}

	//-----------------------------------------------------
	for(int i=0; nNoData>0 && nNoData<16 && i<16; i++)	// guess missing values as average of surrounding data values
	{
		double	t[4][4][4];

		for(iz=0; iz<4; iz++)	for(iy=0; iy<4; iy++)	for(ix=0; ix<4; ix++)
		{
			t[ix][iy][iz]	= v_xyz[ix][iy][iz];
		}

		for(iz=0; iz<4; iz++)	for(iy=0; iy<4; iy++)	for(ix=0; ix<4; ix++)
		{
			if( is_NoData_Value(t[ix][iy][iz]) )
			{
				int		n	= 0;
				double	s	= 0.0;

				for(jz=iz-1; jz<=iz+1; jz++)	for(jy=iy-1; jy<=iy+1; jy++)	for(jx=ix-1; jx<=ix+1; jx++)
				{
					if( is_InGrid(jx + x - 1, jy + y - 1, jz + z - 1) )
					{
						s	+= asDouble(jx + x - 1, jy + y - 1, jz + z - 1);
						n	++;
					}
					else if( jz >= 0 && jz < 4 && jy >= 0 && jy < 4 && jx >= 0 && jx < 4 && !is_NoData_Value(t[jx][jy][jz]) )
					{
						s	+= t[jx][jy][jz];
						n	++;
					}
				}

				if( n > 0 )
				{
					v_xyz[ix][iy][iz]	= s / n;

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
bool CSG_Grids::On_Update(void)
{
	if( is_Valid() )
	{
		m_Statistics.Invalidate();

		for(int z=0; z<Get_NZ(); z++)
		{
			for(int y=0; y<Get_NY(); y++)
			{
				for(int x=0; x<Get_NX(); x++)
				{
					if( !is_NoData(x, y, z) )
					{
						m_Statistics.Add_Value(asDouble(x, y, z));
					}
				}
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
double CSG_Grids::Get_Mean(void)
{
	Update();	return( m_Statistics.Get_Mean() );
}

double CSG_Grids::Get_Min(void)
{
	Update();	return( m_Statistics.Get_Minimum() );
}

double CSG_Grids::Get_Max(void)
{
	Update();	return( m_Statistics.Get_Maximum() );
}

double CSG_Grids::Get_Range(void)
{
	Update();	return( m_Statistics.Get_Range() );
}

double CSG_Grids::Get_StdDev(void)
{
	Update();	return( m_Statistics.Get_StdDev() );
}

double CSG_Grids::Get_Variance(void)
{
	Update();	return( m_Statistics.Get_Variance() );
}

//---------------------------------------------------------
sLong CSG_Grids::Get_Data_Count(void)
{
	Update();	return( m_Statistics.Get_Count() );
}

sLong CSG_Grids::Get_NoData_Count(void)
{
	Update();	return( Get_NCells() - m_Statistics.Get_Count() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grids::On_Reload(void)
{
	return( Create(Get_File_Name(false)) );
}

//---------------------------------------------------------
bool CSG_Grids::On_Delete(void)
{
	CSG_String	FileName	= Get_File_Name(true);

	SG_File_Set_Extension(FileName, "sg-gds-z"); SG_File_Delete(FileName);
	SG_File_Set_Extension(FileName, "sg-gds"  ); SG_File_Delete(FileName);
	SG_File_Set_Extension(FileName, "sg-info" ); SG_File_Delete(FileName);
	SG_File_Set_Extension(FileName, "sg-prj"  ); SG_File_Delete(FileName);

	int	i	= 0;

	do
	{
		SG_File_Set_Extension(FileName, CSG_String::Format("sg-%03d", ++i));
	}
	while( SG_File_Delete(FileName) );

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grids::Load(const CSG_String &FileName, bool bLoadData)
{
	Destroy();

	SG_UI_Msg_Add(CSG_String::Format("%s: %s...", _TL("Loading grid collection"), FileName.c_str()), true);

	bool	bResult	= false;

	Set_Name(SG_File_Get_Name(FileName, false));

	if( SG_File_Cmp_Extension(FileName, "sg-gds") ) // GRIDS_FILETYPE_Normal
	{
		bResult	= _Load_Normal(FileName);
	}
	else // if( SG_File_Cmp_Extension(File_Name, "sg-gds-z") ) // GRIDS_FILETYPE_Compressed
	{
		bResult	= _Load_Compressed(FileName);
	}

	if( !bResult )
	{
		SG_UI_Msg_Add(_TL("failed"), false, SG_UI_MSG_STYLE_FAILURE);
		SG_UI_Process_Set_Ready();

		return( false );
	}

	Set_File_Name(FileName, true);

	Set_Modified(false);

	SG_UI_Msg_Add(_TL("okay"), false, SG_UI_MSG_STYLE_SUCCESS);
	SG_UI_Process_Set_Ready();

	return( true );
}

//---------------------------------------------------------
bool CSG_Grids::Save(const CSG_String &FileName, int Format)
{
	SG_UI_Msg_Add(CSG_String::Format("%s: %s...", _TL("Saving grid collection"), FileName.c_str()), true);

	if( Format == GRIDS_FILE_FORMAT_Undefined )
	{
		Format	= SG_File_Cmp_Extension(FileName, "sg-gds")
			? GRIDS_FILE_FORMAT_Normal
			: GRIDS_FILE_FORMAT_Compressed;
	}

	bool	bResult	= false;

	switch( Format )
	{
	case GRIDS_FILE_FORMAT_Normal    : bResult = _Save_Normal    (FileName); break;
	case GRIDS_FILE_FORMAT_Compressed: bResult = _Save_Compressed(FileName); break;
	}

	//-----------------------------------------------------
	if( !bResult )
	{
		SG_UI_Msg_Add(_TL("failed"), false, SG_UI_MSG_STYLE_FAILURE);
		SG_UI_Process_Set_Ready();

		return( false );
	}

	Set_File_Name(FileName, true);

	Set_Modified(false);

	SG_UI_Msg_Add(_TL("okay"), false, SG_UI_MSG_STYLE_SUCCESS);
	SG_UI_Process_Set_Ready();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grids::_Load_Normal(const CSG_String &_FileName)
{
	CSG_String	FileName(_FileName);

	CSG_File	Stream;

	//-----------------------------------------------------
	if( !Stream.Open(FileName, SG_FILE_R, false) || !_Load_Header(Stream) )
	{
		return( false );
	}

	//-----------------------------------------------------
	for(int i=0; i<Get_NZ() && SG_UI_Process_Set_Progress(i, Get_NZ()); i++)
	{
		SG_File_Set_Extension(FileName, CSG_String::Format("sg-%03d", i + 1));

		if( !Stream.Open(FileName, SG_FILE_R, true) || !_Load_Data(Stream, m_pGrids[i]) )
		{
			return( false );
		}
	}

	//-----------------------------------------------------
	Load_MetaData(FileName);

	Get_Projection().Load(SG_File_Make_Path("", FileName, "sg-prj"), SG_PROJ_FMT_WKT);

	return( true );
}

//---------------------------------------------------------
bool CSG_Grids::_Save_Normal(const CSG_String &_FileName)
{
	CSG_String	FileName(_FileName);

	CSG_File	Stream;

	//-----------------------------------------------------
	SG_File_Set_Extension(FileName, "sg-gds");

	if( !Stream.Open(FileName, SG_FILE_W, false) || !_Save_Header(Stream) )
	{
		return( false );
	}

	//-----------------------------------------------------
	for(int i=0; i<Get_NZ() && SG_UI_Process_Set_Progress(i, Get_NZ()); i++)
	{
		SG_File_Set_Extension(FileName, CSG_String::Format("sg-%03d", i + 1));

		if( !Stream.Open(FileName, SG_FILE_W, true) || !_Save_Data(Stream, m_pGrids[i]) )
		{
			return( false );
		}
	}

	//-----------------------------------------------------
	Save_MetaData(FileName);

	Get_Projection().Save(SG_File_Make_Path("", FileName, "sg-prj"), SG_PROJ_FMT_WKT);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grids::_Load_Compressed(const CSG_String &_FileName)
{
	CSG_File_Zip	Stream(_FileName, SG_FILE_R);

	CSG_String	FileName(SG_File_Get_Name(_FileName, false) + ".");

	//-----------------------------------------------------
	if( !Stream.Get_File(FileName + "sg-gds") || !_Load_Header(Stream) )
	{
		return( false );
	}

	//-----------------------------------------------------
	for(int i=0; i<Get_NZ() && SG_UI_Process_Set_Progress(i, Get_NZ()); i++)
	{
		if( !Stream.Get_File(FileName + CSG_String::Format("sg-%03d", i + 1)) || !_Load_Data(Stream, m_pGrids[i]) )
		{
			return( false );
		}
	}

	//-----------------------------------------------------
	if( Stream.Get_File(FileName + "sg-info") )
	{
		Load_MetaData(Stream);
	}

	if( Stream.Get_File(FileName + "sg-prj") )
	{
		Get_Projection().Load(Stream, SG_PROJ_FMT_WKT);
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Grids::_Save_Compressed(const CSG_String &_FileName)
{
	CSG_File_Zip	Stream(_FileName, SG_FILE_W);

	CSG_String	FileName(SG_File_Get_Name(_FileName, false) + ".");

	//-----------------------------------------------------
	if( !Stream.Add_File(FileName + "sg-gds") || !_Save_Header(Stream) )
	{
		return( false );
	}

	//-----------------------------------------------------
	for(int i=0; i<Get_NZ() && SG_UI_Process_Set_Progress(i, Get_NZ()); i++)
	{
		if( !Stream.Add_File(FileName + CSG_String::Format("sg-%03d", i + 1)) || !_Save_Data(Stream, m_pGrids[i]) )
		{
			return( false );
		}
	}

	//-----------------------------------------------------
	if( Stream.Add_File(FileName + "sg-info") )
	{
		Save_MetaData(Stream);
	}

	if( Stream.Add_File(FileName + "sg-prj") )
	{
		Get_Projection().Save(Stream, SG_PROJ_FMT_WKT);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grids::_Load_Header(CSG_File &Stream)
{
	CSG_MetaData	Header;

	if( !Header.Load(Stream) )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( !Header("NX") || !Header("NY") || !Header("XMIN") || !Header("YMIN") || !Header("CELLSIZE") || !Header("TYPE") )	// necessary minimum information !!!
	{
		return( false );
	}

	CSG_Grid_System	System(Header["CELLSIZE"].Get_Content().asDouble(),
		Header["XMIN"].Get_Content().asDouble(), Header["YMIN"].Get_Content().asDouble(),
		Header["NX"  ].Get_Content().asInt   (), Header["NY"  ].Get_Content().asInt   ()
	);

	TSG_Data_Type	Type	= SG_Data_Type_Get_Type(Header["TYPE"].Get_Content());

	if( !System.is_Valid() || Type == SG_DATATYPE_Undefined )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( Header("NAME"       ) ) Set_Name       (Header["NAME"       ].Get_Content());
	if( Header("DESCRIPTION") ) Set_Description(Header["DESCRIPTION"].Get_Content());
	if( Header("UNIT"       ) ) Set_Unit       (Header["UNIT"       ].Get_Content());

	Set_Scaling(
		Header("SCALE" ) ? Header["SCALE" ].Get_Content().asDouble() : 1.0,
		Header("OFFSET") ? Header["OFFSET"].Get_Content().asDouble() : 0.0
	);

	if( Header("NODATA_MIN") )
	{
		if( Header("NODATA_MAX") )
		{
			Set_NoData_Value_Range(
				Header["NODATA_MIN"].Get_Content().asDouble(),
				Header["NODATA_MAX"].Get_Content().asDouble()
			);
		}
		else
		{
			Set_NoData_Value(
				Header["NODATA_MIN"].Get_Content().asDouble()
			);
		}
	}

	//-----------------------------------------------------
	CSG_Table	Attributes;

	if( Header("ATTRIBUTES") && Header["ATTRIBUTES"]("FIELDS") )
	{
		int		iField;

		const CSG_MetaData	&Fields	= Header["ATTRIBUTES"]["FIELDS"];

		for(iField=0; iField<Fields.Get_Children_Count(); iField++)
		{
			if( Fields[iField].Cmp_Name("FIELD") && Fields[iField].Get_Property("TYPE") )
			{
				Attributes.Add_Field(Fields[iField].Get_Content(), SG_Data_Type_Get_Type(Fields[iField].Get_Property("TYPE")));
			}
		}

		if( Attributes.Get_Field_Count() > 0 && Header["ATTRIBUTES"]("RECORDS") )
		{
			const CSG_MetaData	&Records	= Header["ATTRIBUTES"]["RECORDS"];

			for(int iRecord=0; iRecord<Records.Get_Children_Count(); iRecord++)
			{
				if( Records[iRecord].Cmp_Name("RECORD") )
				{
					CSG_String_Tokenizer	Values(Records[iRecord].Get_Content(), ";");

					CSG_Table_Record	*pRecord	= Attributes.Add_Record();

					for(iField=0; Values.Has_More_Tokens() && iField<Attributes.Get_Field_Count(); iField++)
					{
						pRecord->Set_Value(iField, Values.Get_Next_Token());
					}
				}
			}

			if( Attributes.Get_Count() > 0 )
			{
				int	zAttribute;

				if( !Header["ATTRIBUTES"].Get_Property("ZATTRIBUTE", zAttribute) )
				{
					zAttribute	= 0;
				}

				return( Create(System, Attributes, zAttribute, Type) );
			}
		}
	}

	//-----------------------------------------------------
	if( Header("NZ") )
	{
		return( Create(System, Header["NZ"].Get_Content().asInt(), Header("ZMIN") ? Header["ZMIN"].Get_Content().asDouble() : 0.0, Type) );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Grids::_Save_Header(CSG_File &Stream)
{
	CSG_MetaData	Header;

	Header.Set_Name("GRIDS");

	Header.Add_Property("saga-version", SAGA_VERSION);

	//-----------------------------------------------------
	// general

	Header.Add_Child("NAME"       , Get_Name          ());
	Header.Add_Child("DESCRIPTION", Get_Description   ());
	Header.Add_Child("UNIT"       , Get_Unit          ());

	Header.Add_Child("SCALE"      , Get_Scaling       ());
	Header.Add_Child("OFFSET"     , Get_Offset        ());

	Header.Add_Child("NODATA_MIN" , Get_NoData_Value  ());
	Header.Add_Child("NODATA_MAX" , Get_NoData_hiValue());

	Header.Add_Child("TYPE"       , SG_Data_Type_Get_Identifier(Get_Type()));

	//-----------------------------------------------------
	// grid system

	Header.Add_Child("NX"         , Get_NX      ());
	Header.Add_Child("NY"         , Get_NY      ());
	Header.Add_Child("CELLSIZE"   , Get_Cellsize());
	Header.Add_Child("XMIN"       , Get_XMin    ());
	Header.Add_Child("YMIN"       , Get_YMin    ());

	//-----------------------------------------------------
	// attributes

	CSG_MetaData	&Attributes	= *Header.Add_Child("ATTRIBUTES");

	Attributes.Add_Property("ZATTRIBUTE", m_Z_Attribute);

	int		iField;

	CSG_MetaData	&Fields	= *Attributes.Add_Child("FIELDS");

	for(iField=0; iField<m_Attributes.Get_Field_Count(); iField++)
	{
		Fields.Add_Child("FIELD", m_Attributes.Get_Field_Name(iField))
			->Add_Property("TYPE", SG_Data_Type_Get_Identifier(m_Attributes.Get_Field_Type(iField)));
	}

	CSG_MetaData	&Records	= *Attributes.Add_Child("RECORDS");

	for(int iRecord=0; iRecord<m_Attributes.Get_Count(); iRecord++)
	{
		CSG_String	Values;

		for(iField=0; iField<m_Attributes.Get_Field_Count(); iField++)
		{
			if( iField > 0 )
			{
				Values	+= ";";
			}

			Values	+= m_Attributes[iRecord].asString(iField);
		}

		Records.Add_Child("RECORD", Values)->Add_Property("NAME", m_pGrids[iRecord]->Get_Name());
	}

	//-----------------------------------------------------
	return( Header.Save(Stream) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grids::_Load_Data(CSG_File &Stream, CSG_Grid *pGrid)
{
	TSG_Data_Type	Type	= Get_Type();

	CSG_Array	Line(1, Get_nLineBytes());

	for(int y=0; y<Get_NY(); y++)
	{
		if( !Stream.Read(Line.Get_Array(), Get_nLineBytes()) )
		{
			return( false );
		}

		char	*pValue	= (char *)Line.Get_Array();

		for(int x=0, n=Get_nValueBytes(); x<Get_NX(); x++, pValue+=n)
		{
			switch( Type )
			{
			case SG_DATATYPE_Byte  : pGrid->Set_Value(x, y, *(BYTE   *)pValue, false);	break;
			case SG_DATATYPE_Char  : pGrid->Set_Value(x, y, *(char   *)pValue, false);	break;
			case SG_DATATYPE_Word  : pGrid->Set_Value(x, y, *(WORD   *)pValue, false);	break;
			case SG_DATATYPE_Short : pGrid->Set_Value(x, y, *(short  *)pValue, false);	break;
			case SG_DATATYPE_DWord : pGrid->Set_Value(x, y, *(DWORD  *)pValue, false);	break;
			case SG_DATATYPE_Int   : pGrid->Set_Value(x, y, *(int    *)pValue, false);	break;
			case SG_DATATYPE_Float : pGrid->Set_Value(x, y, *(float  *)pValue, false);	break;
			case SG_DATATYPE_Double: pGrid->Set_Value(x, y, *(double *)pValue, false);	break;
			default:	break;
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Grids::_Save_Data(CSG_File &Stream, CSG_Grid *pGrid)
{
	TSG_Data_Type	Type	= Get_Type();

	CSG_Array	Line(1, Get_nLineBytes());

	for(int y=0; y<Get_NY(); y++)
	{
		char	*pValue	= (char *)Line.Get_Array();

		for(int x=0, n=Get_nValueBytes(); x<Get_NX(); x++, pValue+=n)
		{
			switch( Type )
			{
			case SG_DATATYPE_Byte  : *(BYTE   *)pValue	= pGrid->asByte  (x, y, false);	break;
			case SG_DATATYPE_Char  : *(char   *)pValue	= pGrid->asChar  (x, y, false);	break;
			case SG_DATATYPE_Word  : *(WORD   *)pValue	= pGrid->asShort (x, y, false);	break;
			case SG_DATATYPE_Short : *(short  *)pValue	= pGrid->asShort (x, y, false);	break;
			case SG_DATATYPE_DWord : *(DWORD  *)pValue	= pGrid->asInt   (x, y, false);	break;
			case SG_DATATYPE_Int   : *(int    *)pValue	= pGrid->asInt   (x, y, false);	break;
			case SG_DATATYPE_Float : *(float  *)pValue	= pGrid->asFloat (x, y, false);	break;
			case SG_DATATYPE_Double: *(double *)pValue	= pGrid->asDouble(x, y, false);	break;
			default:	break;
			}
		}

		if( !Stream.Write(Line.Get_Array(), Get_nLineBytes()) )
		{
			return( false );
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

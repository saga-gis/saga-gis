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
	CSG_Grids	*pGrids	= new CSG_Grids(Grids);

	if( !pGrids->is_Valid() ) { delete(pGrids); pGrids = NULL; } return( pGrids );
}

//---------------------------------------------------------
CSG_Grids * SG_Create_Grids(const CSG_Grids *_pGrids, bool bCopyData)
{
	CSG_Grids	*pGrids	= new CSG_Grids(_pGrids, bCopyData);

	if( !pGrids->is_Valid() ) { delete(pGrids); pGrids = NULL; } return( pGrids );
}

//---------------------------------------------------------
CSG_Grids * SG_Create_Grids(const CSG_String &FileName, bool bLoadData)
{
	CSG_Grids	*pGrids	= new CSG_Grids(FileName, bLoadData);

	if( !pGrids->is_Valid() ) { delete(pGrids); pGrids = NULL; } return( pGrids );
}

//---------------------------------------------------------
CSG_Grids * SG_Create_Grids(const CSG_Grid_System &System, int NZ, double zMin, TSG_Data_Type Type)
{
	CSG_Grids	*pGrids	= new CSG_Grids(System, NZ, zMin, Type);

	if( !pGrids->is_Valid() ) { delete(pGrids); pGrids = NULL; } return( pGrids );
}

//---------------------------------------------------------
CSG_Grids * SG_Create_Grids(const CSG_Grid_System &System, const CSG_Table &Attributes, int zAttribute, TSG_Data_Type Type, bool bCreateGrids)
{
	CSG_Grids	*pGrids	= new CSG_Grids(System, Attributes, zAttribute, Type, bCreateGrids);

	if( bCreateGrids && !pGrids->is_Valid() ) { delete(pGrids); pGrids = NULL; } return( pGrids );
}

//---------------------------------------------------------
CSG_Grids * SG_Create_Grids(int NX, int NY, int NZ, double Cellsize, double xMin, double yMin, double zMin, TSG_Data_Type Type)
{
	CSG_Grids	*pGrids	= new CSG_Grids(NX, NY, NZ, Cellsize, xMin, yMin, zMin, Type);

	if( !pGrids->is_Valid() ) { delete(pGrids); pGrids = NULL; } return( pGrids );
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
  * Create a grid collection using the pGrids's grid system, 
  * data type, and attribute field definition. If bCopyData
  * is true, it also copies the data. Otherwise it has no
  * initial data.
*/
//---------------------------------------------------------
CSG_Grids::CSG_Grids(const CSG_Grids *pGrids, bool bCopyData)
{
	_On_Construction();

	Create(pGrids, bCopyData);
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
CSG_Grids::CSG_Grids(const CSG_Grid_System &System, const CSG_Table &Attributes, int zAttribute, TSG_Data_Type Type, bool bCreateGrids)
{
	_On_Construction();

	Create(System, Attributes, zAttribute, Type, bCreateGrids);
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

	m_Attributes.Set_Owner(this);

	m_Index		= NULL;

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

	SG_FREE_SAFE(m_Index);

	m_Attributes.Destroy();
	m_Attributes.Add_Field("Z", SG_DATATYPE_Double);
	m_Z_Attribute	= m_Z_Name	= 0;

	return( CSG_Data_Object::Destroy() );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grids::Create(const CSG_Grids &Grids)
{
	return( Create((CSG_Grids *)&Grids, true) );
}

//---------------------------------------------------------
bool CSG_Grids::Create(const CSG_Grids *pGrids, bool bCopyData)
{
	if( pGrids && pGrids->is_Valid() && Create(pGrids->Get_System(), 0, 0.0, pGrids->Get_Type()) )
	{
		m_Attributes.Create(&pGrids->m_Attributes);
		Set_Z_Attribute (pGrids->Get_Z_Attribute ());
		Set_Z_Name_Field(pGrids->Get_Z_Name_Field());

		if( bCopyData )
		{
			for(int i=0; i<pGrids->Get_NZ(); i++)
			{
				Add_Grid(pGrids->Get_Attributes(i), pGrids->Get_Grid_Ptr(i));
			}
		}

		Get_MetaData_DB().Del_Children();
		Get_MetaData_DB().Add_Children(pGrids->Get_MetaData_DB());

		Get_Projection().Create(pGrids->Get_Projection());

		return( true );
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
	Destroy();

	if( m_pGrids[0]->Create(System, Type) )
	{
		Set_NoData_Value_Range(m_pGrids[0]->Get_NoData_Value(), m_pGrids[0]->Get_NoData_hiValue());

		for(int i=0; i<NZ; i++, zMin+=System.Get_Cellsize())
		{
			if( !Add_Grid(zMin) )
			{
				return( false );
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Grids::Create(const CSG_Grid_System &System, const CSG_Table &Attributes, int zAttribute, TSG_Data_Type Type, bool bCreateGrids)
{
	Destroy();

	if( m_Attributes.Create(&Attributes) && m_pGrids[0]->Create(System, Type) )
	{
		Set_Z_Attribute(zAttribute);

		if( bCreateGrids )
		{
			for(int i=0; i<Attributes.Get_Count(); i++)
			{
				if( !Add_Grid(Attributes[i]) )
				{
					return( false );
				}
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Grids::Create(int NX, int NY, int NZ, double Cellsize, double xMin, double yMin, double zMin, TSG_Data_Type Type)
{
	return( Create(CSG_Grid_System(Cellsize, xMin, yMin, NX, NY), NZ, zMin, Type) );
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
	m_pGrids[0]->Set_Scaling(Scale, Offset);

	for(int i=1; i<Get_Grid_Count(); i++)
	{
		m_pGrids[i]->Set_Scaling(Scale, Offset);
	}

	Set_Update_Flag();
}

//---------------------------------------------------------
bool CSG_Grids::Set_NoData_Value_Range(double loValue, double hiValue)
{
	if( CSG_Data_Object::Set_NoData_Value_Range(loValue, hiValue) )	// this is a CSG_Data_Object base class property
	{
		m_pGrids[0]->Set_NoData_Value_Range(loValue, hiValue);

		for(int i=1; i<Get_Grid_Count(); i++)
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
	if( !Get_Projection().is_Okay() && pGrid->Get_Projection().is_Okay() )
	{
		Get_Projection().Create(pGrid->Get_Projection());
	}

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

		if( m_Z_Name > i )
		{
			m_Z_Name--;
		}
		else if( m_Z_Name == i )
		{
			m_Z_Name	= -1;	// same as m_Z_Attribute
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
bool CSG_Grids::Set_Z_Name_Field(int i)
{
	if( i >= 0 && i < m_Attributes.Get_Field_Count() )
	{
		m_Z_Name	= i;

		return( true );
	}

	return( false );
}

int CSG_Grids::Get_Z_Name_Field(void)	const
{
	return( m_Z_Name >= 0 && m_Z_Name < m_Attributes.Get_Field_Count() ? m_Z_Name : m_Z_Attribute );
}

//---------------------------------------------------------
bool CSG_Grids::Set_Z(int i, double Value)
{
	return( i >= 0 && i < Get_NZ() && m_Attributes[i].Set_Value(m_Z_Attribute, Value) );
}

//---------------------------------------------------------
bool CSG_Grids::Update_Z_Order(void)
{
	bool	bChanged	= false; 

	CSG_Table	Attributes(m_Attributes);

	if( Attributes.Set_Index(m_Z_Attribute, TABLE_INDEX_Ascending) )
	{
		CSG_Array_Pointer	Grids;
		
		CSG_Grid	**pGrids	= (CSG_Grid **)Grids.Create(m_Grids);

		for(int i=0; i<Attributes.Get_Count(); i++)
		{
			int	Index	= Attributes[i].Get_Index();

			if( Index != i )
			{
				bChanged	= true;

				m_pGrids[i]	= pGrids[Index];

				m_Attributes[i].Assign(&Attributes[i]);
			}
		}
	}

	return( bChanged );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grids::Set_Grid_Count(int Count)
{
	if( Count == Get_NZ() )
	{
		return( true );
	}

	if( Count < 0 || !Get_System().is_Valid() )	// only allowed for initialized grid systems)
	{
		return( false );
	}

	if( Count == 0 )
	{
		return( Del_Grids() );
	}

	//-----------------------------------------------------
	SG_FREE_SAFE(m_Index);	// invalidate index

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
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grids::Add_Grid(double Z)
{
	CSG_Table	Attributes(&m_Attributes);

	Attributes.Add_Record();

	Attributes[0].Set_Value(m_Z_Attribute, Z);

	return( Add_Grid(Attributes[0]) );
}

//---------------------------------------------------------
bool CSG_Grids::Add_Grid(double Z, CSG_Grid *pGrid, bool bAttach)
{
	CSG_Table	Attributes(&m_Attributes);

	Attributes.Add_Record();

	Attributes[0].Set_Value(m_Z_Attribute, Z);

	return( Add_Grid(Attributes[0], pGrid, bAttach) );
}

//---------------------------------------------------------
bool CSG_Grids::Add_Grid(CSG_Table_Record &Attributes)
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
	m_Attributes.Add_Record(&Attributes);

	m_pGrids[n]->Set_Name(CSG_String::Format("%s [%s]", Get_Name(), SG_Get_String(Get_Z(n), -10).c_str()));

	SG_FREE_SAFE(m_Index);	// invalidate index

	Update_Z_Order();

	return( true );
}

//---------------------------------------------------------
bool CSG_Grids::Add_Grid(CSG_Table_Record &Attributes, CSG_Grid *pGrid, bool bAttach)
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
	m_Attributes.Add_Record(&Attributes);

	if( !Get_Projection().is_Okay() && pGrid->Get_Projection().is_Okay() )
	{
		Get_Projection()	= pGrid->Get_Projection();
	}

	SG_FREE_SAFE(m_Index);	// invalidate index

	Update_Z_Order();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grids::Del_Grid(int i, bool bDetach)
{
	if( m_Attributes.Del_Record(i) )	// Get_NZ() is now decreased by one
	{
		SG_FREE_SAFE(m_Index);	// invalidate index

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
bool CSG_Grids::Del_Grids(bool bDetach)
{
	SG_FREE_SAFE(m_Index);	// invalidate index

	if( bDetach )
	{
		for(size_t i=0; i<m_Grids.Get_Size(); i++)
		{
			m_pGrids[i]->Set_Owner(NULL);
		}

		m_pGrids[0]	= SG_Create_Grid(*m_pGrids[0]);	// needs a new dummy
		m_pGrids[0]->Set_Owner(this);
	}
	else
	{
		for(size_t i=1; i<m_Grids.Get_Size(); i++)
		{
			delete(m_pGrids[i]);	// do not delete the dummy before deconstruction
		}
	}

	m_pGrids	= (CSG_Grid **)m_Grids.Get_Array(1);

	m_Attributes.Del_Records();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CSG_Grids::Get_Grid_Name(int i, int Style) const
{
	CSG_String	s;

	if( i >= 0 && i < Get_Grid_Count() )
	{
		if( Style == 0 )
		{
			Style	= SG_GRIDS_NAME_OWNER|SG_GRIDS_NAME_GRID;
		}

		if( (Style & SG_GRIDS_NAME_OWNER) != 0 )
		{
			s	= CSG_String(Get_Name());
		}

		if( (Style & SG_GRIDS_NAME_INDEX) != 0 )
		{
			if( !s.is_Empty() )	s	+= ".";

			s.Printf("%s %d", _TL("Band"), i + 1);
		}

		if( (Style & SG_GRIDS_NAME_VALUE) != 0 )
		{
			if( !s.is_Empty() )	s	+= ".";

			s	+= SG_Get_String(Get_Z(i), -10);
		}

		if( (Style & SG_GRIDS_NAME_GRID ) != 0 )
		{
			if( !s.is_Empty() )	s	+= ".";

			s	+= m_Attributes[i].asString(Get_Z_Name_Field());
		}
	}

	return( s );
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
double CSG_Grids::Get_Value(const TSG_Point_Z &p, TSG_Grid_Resampling Resampling, TSG_Grid_Resampling ZResampling) const
{
	double	Value;

	return( Get_Value(p.x, p.y, p.z, Value, Resampling, ZResampling) ? Value : Get_NoData_Value() );
}

double CSG_Grids::Get_Value(double x, double y, double z, TSG_Grid_Resampling Resampling, TSG_Grid_Resampling ZResampling) const
{
	double	Value;

	return( Get_Value(x, y, z, Value, Resampling, ZResampling) ? Value : Get_NoData_Value() );
}

bool CSG_Grids::Get_Value(const TSG_Point_Z &p, double &Value, TSG_Grid_Resampling Resampling, TSG_Grid_Resampling ZResampling) const
{
	return( Get_Value(p.x, p.y, p.z, Value, Resampling, ZResampling) );
}

//---------------------------------------------------------
bool CSG_Grids::Get_Value(double x, double y, double z, double &Value, TSG_Grid_Resampling Resampling, TSG_Grid_Resampling ZResampling) const
{
	if(	!Get_System().Get_Extent(true).Contains(x, y) )
	{
		return( false );
	}

	int	iz;	double	dz;

	if( !_Get_Z(z, iz, dz) )
	{
		return( false );
	}

	if( dz == 0.0 )
	{
		return( m_pGrids[iz]->Get_Value(x, y, Value, Resampling) );
	}

	if( ZResampling == GRID_RESAMPLING_Undefined )
	{
		ZResampling	= Resampling;
	}

	if( (ZResampling == GRID_RESAMPLING_BicubicSpline || ZResampling == GRID_RESAMPLING_BSpline)
	&&  (iz < 1 || iz >= m_Attributes.Get_Count() - 2) )
	{
		ZResampling	= GRID_RESAMPLING_Bilinear;
	}

	switch( ZResampling )
	{
	case GRID_RESAMPLING_NearestNeighbour: default:
		return( m_pGrids[dz < 0.5 ? iz : iz + 1]->Get_Value(x, y, Value, Resampling) );

	case GRID_RESAMPLING_Bilinear:
		{
			double	v[2];

			if( m_pGrids[iz    ]->Get_Value(x, y, v[0], Resampling)
			&&  m_pGrids[iz + 1]->Get_Value(x, y, v[1], Resampling) )
			{
				Value	= v[0] + dz * (v[1] - v[0]);

				return( true );
			}

			return( false );
		}

	case GRID_RESAMPLING_BicubicSpline:
	case GRID_RESAMPLING_BSpline:
		{
			CSG_Spline	s;

			#define ADD_TO_SPLINE(i)	if( i < 0 || i >= Get_NZ() || !m_pGrids[i]->Get_Value(x, y, Value, Resampling) ) return( false ); s.Add(Get_Z(i), Value);

			ADD_TO_SPLINE(iz - 1);
			ADD_TO_SPLINE(iz    );
			ADD_TO_SPLINE(iz + 1);
			ADD_TO_SPLINE(iz + 2);

			return( s.Get_Value(z, Value) );
		}
		break;
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_Grids::_Get_Z(double z, int &iz, double &dz) const
{
	if( z < m_Attributes[0                           ].asDouble(m_Z_Attribute)
	||  z > m_Attributes[m_Attributes.Get_Count() - 1].asDouble(m_Z_Attribute) )
	{
		return( false );
	}

	double	z0, z1	= m_Attributes[0].asDouble(m_Z_Attribute);

	for(iz=0; iz<m_Attributes.Get_Count()-1; iz++)
	{
		z0 = z1; z1	= m_Attributes[iz + 1].asDouble(m_Z_Attribute);

		if( z < z1 )
		{
			dz	= z0 < z1 ? (z - z0) / (z1 - z0) : 0.0;

			return( true );
		}
	}

	return( (dz = z - z1) == 0.0 );
}


///////////////////////////////////////////////////////////
//														 //
//						Index							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define SORT_SWAP(a,b)	{itemp=(a);(a)=(b);(b)=itemp;}

bool CSG_Grids::_Set_Index(void)
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
#undef SORT_SWAP


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
		double	Offset = Get_Offset(), Scaling = is_Scaled() ? Get_Scaling() : 0.0;

		m_Statistics.Invalidate();

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
			for(sLong i=0; i<Get_NCells(); i++)
			{
				double	Value	= asDouble(i, false);

				if( !is_NoData_Value(Value) )
				{
					m_Statistics	+= Scaling ? Offset + Scaling * Value : Value;
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

//---------------------------------------------------------
double CSG_Grids::Get_Quantile(double Quantile)
{
	Quantile	= Quantile <= 0.0 ? 0.0 : Quantile >= 100.0 ? 1.0 : Quantile / 100.0;

	sLong	n	= (sLong)(Quantile * (Get_Data_Count() - 1));

	if( Get_Sorted(n, n, false) )
	{
		return( asDouble(n) );
	}

	return( Get_NoData_Value() );
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
const CSG_Simple_Statistics & CSG_Grids::Get_Statistics(void)
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
bool CSG_Grids::Get_Statistics(const CSG_Rect &rWorld, CSG_Simple_Statistics &Statistics, bool bHoldValues) const
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

			for(int z=0; z<Get_NZ(); z++)
			{
				double	Value	= asDouble(x, y, z, false);

				if( !is_NoData_Value(Value) )
				{
					Statistics	+= Scaling ? Offset + Scaling * Value : Value;
				}
			}
		}
	}
	else
	{
		for(int x=xMin; x<=xMax; x++)
		{
			for(int y=yMin; y<=yMax; y++)
			{
				for(int z=0; z<Get_NZ(); z++)
				{
					double	Value	= asDouble(x, y, z, false);

					if( !is_NoData_Value(Value) )
					{
						Statistics	+= Scaling ? Offset + Scaling * Value : Value;
					}
				}
			}
		}
	}

	return( Statistics.Get_Count() > 0 );
}

//---------------------------------------------------------
bool CSG_Grids::Set_Max_Samples(sLong Max_Samples)
{
	if( CSG_Data_Object::Set_Max_Samples(Max_Samples) )
	{
		for(int i=0; i<Get_Grid_Count(); i++)
		{
			Get_Grid_Ptr(i)->Set_Max_Samples(Max_Samples);
		}

		return( true );
	}

	return( false );
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

	if( _Load_PGSQL     (FileName)
	||  _Load_Normal    (FileName)
	||  _Load_Compressed(FileName)
	||  _Load_External  (FileName) )
	{
		Set_Modified(false);

		Set_Name(SG_File_Get_Name(FileName, false));

		SG_UI_Process_Set_Ready();
		SG_UI_Msg_Add(_TL("okay"), false, SG_UI_MSG_STYLE_SUCCESS);

		return( true );
	}

	SG_UI_Process_Set_Ready();
	SG_UI_Msg_Add(_TL("failed"), false, SG_UI_MSG_STYLE_FAILURE);

	return( false );
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
	SG_UI_Process_Set_Ready();

	if( bResult )
	{
		Set_Modified(false);

		Set_File_Name(FileName, true);

		SG_UI_Msg_Add(_TL("okay"), false, SG_UI_MSG_STYLE_SUCCESS);

		return( true );
	}

	SG_UI_Msg_Add(_TL("failed"), false, SG_UI_MSG_STYLE_FAILURE);

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grids::_Load_External(const CSG_String &FileName)
{
	bool	bResult	= false;

	CSG_Data_Manager	Data;

	CSG_Tool	*pTool	= SG_Get_Tool_Library_Manager().Get_Tool("io_gdal", 0);	// import raster

	if(	pTool && pTool->On_Before_Execution() && pTool->Settings_Push(&Data) )
	{
		SG_UI_Msg_Lock(true);

		if( pTool->Set_Parameter("FILES"   , FileName)
		&&	pTool->Set_Parameter("MULTIPLE", 1       )	// output as grid collection
		&&	pTool->Execute()
		&&  Data.Grid_System_Count() > 0 && Data.Get_Grid_System(0)->Count() > 0 && Data.Get_Grid_System(0)->Get(0)->is_Valid() )
		{
			CSG_Grids	*pGrids	= (CSG_Grids *)Data.Get_Grid_System(0)->Get(0);

			for(int i=0; i<pGrids->Get_Grid_Count(); i++)
			{
				Add_Grid(pGrids->Get_Z(i), pGrids->Get_Grid_Ptr(i), true);
			}

			pGrids->Del_Grids(true);

			Set_File_Name(FileName, false);

			Set_Name       (pGrids->Get_Name       ());
			Set_Description(pGrids->Get_Description());
		
			bResult	= true;
		}

		SG_UI_Msg_Lock(false);

		pTool->Settings_Pop();
	}

	return( bResult );
}

//---------------------------------------------------------
bool CSG_Grids::_Load_PGSQL(const CSG_String &FileName)
{
	bool	bResult	= false;

	if( FileName.BeforeFirst(':').Cmp("PGSQL") == 0 )	// database source
	{
		CSG_String	s(FileName);

		s	= s.AfterFirst(':');	CSG_String	Host  (s.BeforeFirst(':'));
		s	= s.AfterFirst(':');	CSG_String	Port  (s.BeforeFirst(':'));
		s	= s.AfterFirst(':');	CSG_String	DBName(s.BeforeFirst(':'));
		s	= s.AfterFirst(':');	CSG_String	Table (s.BeforeFirst(':'));
		s	= s.AfterFirst(':');	CSG_String	rid   (s.BeforeFirst(':').AfterFirst('='));

		//-------------------------------------------------
		CSG_String_Tokenizer	rids(rid, ",");	rid.Clear();

		while( rids.Has_More_Tokens() )
		{
			if( !rid.is_Empty() )
			{
				rid	+= " OR ";
			}

			rid	+= "rid=\'" + rids.Get_Next_Token() + "\'";
		}

		//-------------------------------------------------
		CSG_Tool	*pTool	= SG_Get_Tool_Library_Manager().Get_Tool("db_pgsql", 0);	// CGet_Connections

		if(	pTool != NULL )
		{
			SG_UI_ProgressAndMsg_Lock(true);

			//---------------------------------------------
			CSG_Table	Connections;
			CSG_String	Connection	= DBName + " [" + Host + ":" + Port + "]";

			pTool->On_Before_Execution();
			pTool->Settings_Push();

			if( SG_TOOL_PARAMETER_SET("CONNECTIONS", &Connections) && pTool->Execute() )	// CGet_Connections
			{
				for(int i=0; !bResult && i<Connections.Get_Count(); i++)
				{
					if( !Connection.Cmp(Connections[i].asString(0)) )
					{
						bResult	= true;
					}
				}
			}

			pTool->Settings_Pop();

			//---------------------------------------------
			if( bResult && (bResult = (pTool = SG_Get_Tool_Library_Manager().Get_Tool("db_pgsql", 30)) != NULL) == true )	// CPGIS_Raster_Load
			{
				CSG_Data_Manager	Grids;

				pTool->On_Before_Execution();
				pTool->Settings_Push(&Grids);

				bResult	=  SG_TOOL_PARAMETER_SET("CONNECTION", Connection)
						&& SG_TOOL_PARAMETER_SET("TABLES"    , Table)
						&& SG_TOOL_PARAMETER_SET("MULTIPLE"  , 1)	// grid collection
						&& SG_TOOL_PARAMETER_SET("WHERE"     , rid)
						&& pTool->Execute();

				pTool->Settings_Pop();

				//-----------------------------------------
				if( Grids.Grid_System_Count() > 0 && Grids.Get_Grid_System(0)->Get(0) && Grids.Get_Grid_System(0)->Get(0)->is_Valid() )
				{
					CSG_Grids	*pGrids	= (CSG_Grids *)Grids.Get_Grid_System(0)->Get(0);

					Set_File_Name(FileName);

					Create(pGrids);

					for(int i=0; i<pGrids->Get_Grid_Count(); i++)
					{
						Add_Grid(pGrids->Get_Attributes(i), pGrids->Get_Grid_Ptr(i), true);
					}

					pGrids->Del_Grids(true);
				}
			}

			SG_UI_ProgressAndMsg_Lock(false);
		}
	}

	return( Get_NZ() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grids::_Load_Normal(const CSG_String &_FileName)
{
	if( !SG_File_Cmp_Extension(_FileName, "sg-gds") ) // GRIDS_FILETYPE_Normal
	{
		return( false );
	}

	CSG_String	FileName(_FileName);

	CSG_File	Stream;

	//-----------------------------------------------------
	if( !Stream.Open(FileName, SG_FILE_R, false) || !_Load_Header(Stream) )
	{
		return( false );
	}

	SG_File_Set_Extension(FileName, "sg-att");

	if( m_Attributes.Get_Count() <= 0 )	// <<< DEPRECATED
	if( !Stream.Open(FileName, SG_FILE_R, false) || !_Load_Attributes(Stream) )
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
	Set_File_Name(_FileName, true);

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

	SG_File_Set_Extension(FileName, "sg-att");

	if( !Stream.Open(FileName, SG_FILE_W, false) || !_Save_Attributes(Stream) )
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
	if( !SG_File_Cmp_Extension(_FileName, "sg-gds-z") ) // GRIDS_FILETYPE_Compressed
	{
		return( false );
	}

	CSG_File_Zip	Stream(_FileName, SG_FILE_R);

	CSG_String	FileName(SG_File_Get_Name(_FileName, false) + ".");

	//-----------------------------------------------------
	if( !Stream.Get_File(FileName + "sg-gds") || !_Load_Header(Stream) )
	{
		return( false );
	}

	if( m_Attributes.Get_Count() <= 0 )	// <<< DEPRECATED
	if( !Stream.Get_File(FileName + "sg-att") || !_Load_Attributes(Stream) )
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
	Set_File_Name(_FileName, true);

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

	if( !Stream.Add_File(FileName + "sg-att") || !_Save_Attributes(Stream) )
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

	if( !System.is_Valid() || Type == SG_DATATYPE_Undefined || !m_pGrids[0]->Create(System, Type) )
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
	m_Attributes.Destroy();

	if( Header("ATTRIBUTES") && Header["ATTRIBUTES"]("FIELDS") == NULL )
	{
		const CSG_MetaData	&Fields	= Header["ATTRIBUTES"];

		for(int iField=0; iField<Fields.Get_Children_Count(); iField++)
		{
			if( Fields[iField].Cmp_Name("FIELD") && Fields[iField].Get_Property("TYPE") )
			{
				m_Attributes.Add_Field(Fields[iField].Get_Content(), SG_Data_Type_Get_Type(Fields[iField].Get_Property("TYPE")));
			}
		}

		if( !Fields.Get_Property("Z_FIELD", m_Z_Attribute) || m_Z_Attribute >= m_Attributes.Get_Field_Count() )
		{
			m_Z_Attribute	= 0;
		}

		if( !Fields.Get_Property("Z_NAME", m_Z_Name      ) || m_Z_Name      >= m_Attributes.Get_Field_Count() )
		{
			m_Z_Name	= -1;	// same as m_Z_Attribute
		}
	}

	//-----------------------------------------------------
	// >>> DEPRECATED >>> //
	if( Header("ATTRIBUTES") && Header["ATTRIBUTES"]("FIELDS") != NULL )
	{
		if( !Header["ATTRIBUTES"].Get_Property("ZATTRIBUTE", m_Z_Attribute) )
		{
			m_Z_Attribute	= 0;
		}

		int		iField;

		const CSG_MetaData	&Fields	= Header["ATTRIBUTES"]["FIELDS"];

		for(iField=0; iField<Fields.Get_Children_Count(); iField++)
		{
			if( Fields[iField].Cmp_Name("FIELD") && Fields[iField].Get_Property("TYPE") )
			{
				m_Attributes.Add_Field(Fields[iField].Get_Content(), SG_Data_Type_Get_Type(Fields[iField].Get_Property("TYPE")));
			}
		}

		if( m_Attributes.Get_Field_Count() > 0 && Header["ATTRIBUTES"]("RECORDS") )
		{
			CSG_Table	Attributes(m_Attributes);

			const CSG_MetaData	&Records	= Header["ATTRIBUTES"]["RECORDS"];

			for(int iRecord=0; iRecord<Records.Get_Children_Count(); iRecord++)
			{
				if( Records[iRecord].Cmp_Name("RECORD") )
				{
					CSG_String_Tokenizer	Values(Records[iRecord].Get_Content(), ";");

					if( Values.Get_Tokens_Count() == (size_t)Attributes.Get_Field_Count() )
					{
						CSG_Table_Record	*pRecord	= Attributes.Add_Record();

						for(int iField=0; iField<m_Attributes.Get_Field_Count(); iField++)
						{
							pRecord->Set_Value(iField, Values.Get_Next_Token());
						}

						if( !Add_Grid(*pRecord) )
						{
							return( false );
						}
					}
				}
			}
		}
	}
	else if( Header("NZ") && Header["NZ"].Get_Content().asInt() > 0 )
	{
		m_Attributes.Add_Field("ID", SG_DATATYPE_Int);

		for(int i=0, n=Header["NZ"].Get_Content().asInt(); i<n; i++)
		{
			if( !Add_Grid(i + 1.) )
			{
				return( false );
			}
		}
	}
	// <<< DEPRECATED <<< //

	//-----------------------------------------------------
	return( m_Attributes.Get_Field_Count() > 0 );
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

	Attributes.Add_Property("Z_FIELD", m_Z_Attribute);
	Attributes.Add_Property("Z_NAME" , m_Z_Name     );

	for(int iField=0; iField<m_Attributes.Get_Field_Count(); iField++)
	{
		Attributes.Add_Child("FIELD", m_Attributes.Get_Field_Name(iField))->Add_Property(
			"TYPE", SG_Data_Type_Get_Identifier(m_Attributes.Get_Field_Type(iField))
		);
	}

	//-----------------------------------------------------
	return( Header.Save(Stream) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grids::_Load_Attributes(CSG_File &Stream)
{
	CSG_Table	Attributes(m_Attributes);

	CSG_String	sLine;

	while( Stream.Read_Line(sLine) && !sLine.is_Empty() )
	{
		CSG_String_Tokenizer	Values(sLine, "\t", SG_TOKEN_RET_EMPTY_ALL);

		if( Values.Get_Tokens_Count() == (size_t)Attributes.Get_Field_Count() )
		{
			CSG_Table_Record	*pRecord	= Attributes.Add_Record();

			for(int iField=0; iField<m_Attributes.Get_Field_Count(); iField++)
			{
				pRecord->Set_Value(iField, Values.Get_Next_Token());
			}

			if( !Add_Grid(*pRecord) )
			{
				return( false );
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Grids::_Save_Attributes(CSG_File &Stream)
{
	for(int iRecord=0; iRecord<m_Attributes.Get_Count(); iRecord++)
	{
		for(int iField=0; iField<m_Attributes.Get_Field_Count(); iField++)
		{
			Stream.Write(m_Attributes[iRecord].asString(iField));
			Stream.Write(iField < m_Attributes.Get_Field_Count() - 1 ? "\t" : "\n");
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_Grids::_Load_Data(CSG_File &Stream, CSG_Grid *pGrid)
{
	if( !pGrid )
	{
		return( false );
	}

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

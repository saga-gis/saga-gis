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
//                  parameter_data.cpp                   //
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
#include "parameters.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define SG_MD_PARM_DATAOBJECT_CREATE		SG_T("CREATE")


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String SG_Parameter_Type_Get_Name(TSG_Parameter_Type Type)
{
	switch( Type )
	{
	default:								return( _TL("[PRM] Parameter") );

	case PARAMETER_TYPE_Node:				return( _TL("[PRM] Node") );

	case PARAMETER_TYPE_Bool:				return( _TL("[PRM] Boolean") );
	case PARAMETER_TYPE_Int:				return( _TL("[PRM] Integer") );
	case PARAMETER_TYPE_Double:				return( _TL("[PRM] Floating point") );
	case PARAMETER_TYPE_Degree:				return( _TL("[PRM] Degree") );
	case PARAMETER_TYPE_Range:				return( _TL("[PRM] Value range") );
	case PARAMETER_TYPE_Choice:				return( _TL("[PRM] Choice") );

	case PARAMETER_TYPE_String:				return( _TL("[PRM] Text") );
	case PARAMETER_TYPE_Text:				return( _TL("[PRM] Long text") );
	case PARAMETER_TYPE_FilePath:			return( _TL("[PRM] File path") );

	case PARAMETER_TYPE_Font:				return( _TL("[PRM] Font") );
	case PARAMETER_TYPE_Color:				return( _TL("[PRM] Color") );
	case PARAMETER_TYPE_Colors:				return( _TL("[PRM] Colors") );
	case PARAMETER_TYPE_FixedTable:			return( _TL("[PRM] Static table") );
	case PARAMETER_TYPE_Grid_System:		return( _TL("[PRM] Grid system") );
	case PARAMETER_TYPE_Table_Field:		return( _TL("[PRM] Table field") );

	case PARAMETER_TYPE_DataObject_Output:	return( _TL("[PRM] Data Object") );
	case PARAMETER_TYPE_Grid:				return( _TL("[PRM] Grid") );
	case PARAMETER_TYPE_Table:				return( _TL("[PRM] Table") );
	case PARAMETER_TYPE_Shapes:				return( _TL("[PRM] Shapes") );
	case PARAMETER_TYPE_TIN:				return( _TL("[PRM] TIN") );
	case PARAMETER_TYPE_PointCloud:			return( _TL("[PRM] Point Cloud") );

	case PARAMETER_TYPE_Grid_List:			return( _TL("[PRM] Grid list") );
	case PARAMETER_TYPE_Table_List:			return( _TL("[PRM] Table list") );
	case PARAMETER_TYPE_Shapes_List:		return( _TL("[PRM] Shapes list") );
	case PARAMETER_TYPE_TIN_List:			return( _TL("[PRM] TIN list") );
	case PARAMETER_TYPE_PointCloud_List:	return( _TL("[PRM] Point Cloud list") );

	case PARAMETER_TYPE_Parameters:			return( _TL("[PRM] Parameters") );
	}
}

//---------------------------------------------------------
CSG_String SG_Parameter_Type_Get_Identifier(TSG_Parameter_Type Type)
{
	switch( Type )
	{
	default:								return( SG_T("parameter") );

	case PARAMETER_TYPE_Node:				return( SG_T("node") );

	case PARAMETER_TYPE_Bool:				return( SG_T("boolean") );
	case PARAMETER_TYPE_Int:				return( SG_T("integer") );
	case PARAMETER_TYPE_Double:				return( SG_T("double") );
	case PARAMETER_TYPE_Degree:				return( SG_T("degree") );
	case PARAMETER_TYPE_Range:				return( SG_T("range") );
	case PARAMETER_TYPE_Choice:				return( SG_T("choice") );

	case PARAMETER_TYPE_String:				return( SG_T("text") );
	case PARAMETER_TYPE_Text:				return( SG_T("long_text") );
	case PARAMETER_TYPE_FilePath:			return( SG_T("file") );

	case PARAMETER_TYPE_Font:				return( SG_T("font") );
	case PARAMETER_TYPE_Color:				return( SG_T("color") );
	case PARAMETER_TYPE_Colors:				return( SG_T("colors") );
	case PARAMETER_TYPE_FixedTable:			return( SG_T("static_table") );
	case PARAMETER_TYPE_Grid_System:		return( SG_T("grid_system") );
	case PARAMETER_TYPE_Table_Field:		return( SG_T("table_field") );

	case PARAMETER_TYPE_DataObject_Output:	return( SG_T("data_object") );
	case PARAMETER_TYPE_Grid:				return( SG_T("grid") );
	case PARAMETER_TYPE_Table:				return( SG_T("table") );
	case PARAMETER_TYPE_Shapes:				return( SG_T("shapes") );
	case PARAMETER_TYPE_TIN:				return( SG_T("tin") );
	case PARAMETER_TYPE_PointCloud:			return( SG_T("points") );

	case PARAMETER_TYPE_Grid_List:			return( SG_T("grid_list") );
	case PARAMETER_TYPE_Table_List:			return( SG_T("table_list") );
	case PARAMETER_TYPE_Shapes_List:		return( SG_T("shapes_list") );
	case PARAMETER_TYPE_TIN_List:			return( SG_T("tin_list") );
	case PARAMETER_TYPE_PointCloud_List:	return( SG_T("points_list") );

	case PARAMETER_TYPE_Parameters:			return( SG_T("parameters") );
	}
}

//---------------------------------------------------------
TSG_Parameter_Type SG_Parameter_Type_Get_Type(const CSG_String &Identifier)
{
	if( !Identifier.Cmp(SG_T("node"			)) )	{	return( PARAMETER_TYPE_Node					);	}
	if( !Identifier.Cmp(SG_T("boolean"		)) )	{	return( PARAMETER_TYPE_Bool					);	}
	if( !Identifier.Cmp(SG_T("integer"		)) )	{	return( PARAMETER_TYPE_Int					);	}
	if( !Identifier.Cmp(SG_T("double"		)) )	{	return( PARAMETER_TYPE_Double				);	}
	if( !Identifier.Cmp(SG_T("degree"		)) )	{	return( PARAMETER_TYPE_Degree				);	}
	if( !Identifier.Cmp(SG_T("range"		)) )	{	return( PARAMETER_TYPE_Range				);	}
	if( !Identifier.Cmp(SG_T("choice"		)) )	{	return( PARAMETER_TYPE_Choice				);	}

	if( !Identifier.Cmp(SG_T("text"			)) )	{	return( PARAMETER_TYPE_String				);	}
	if( !Identifier.Cmp(SG_T("long_text"	)) )	{	return( PARAMETER_TYPE_Text					);	}
	if( !Identifier.Cmp(SG_T("file"			)) )	{	return( PARAMETER_TYPE_FilePath				);	}

	if( !Identifier.Cmp(SG_T("font"			)) )	{	return( PARAMETER_TYPE_Font					);	}
	if( !Identifier.Cmp(SG_T("color"		)) )	{	return( PARAMETER_TYPE_Color				);	}
	if( !Identifier.Cmp(SG_T("colors"		)) )	{	return( PARAMETER_TYPE_Colors				);	}
	if( !Identifier.Cmp(SG_T("static_table"	)) )	{	return( PARAMETER_TYPE_FixedTable			);	}
	if( !Identifier.Cmp(SG_T("grid_system"	)) )	{	return( PARAMETER_TYPE_Grid_System			);	}
	if( !Identifier.Cmp(SG_T("table_field"	)) )	{	return( PARAMETER_TYPE_Table_Field			);	}

	if( !Identifier.Cmp(SG_T("data_object"	)) )	{	return( PARAMETER_TYPE_DataObject_Output	);	}
	if( !Identifier.Cmp(SG_T("grid"			)) )	{	return( PARAMETER_TYPE_Grid					);	}
	if( !Identifier.Cmp(SG_T("table"		)) )	{	return( PARAMETER_TYPE_Table				);	}
	if( !Identifier.Cmp(SG_T("shapes"		)) )	{	return( PARAMETER_TYPE_Shapes				);	}
	if( !Identifier.Cmp(SG_T("tin"			)) )	{	return( PARAMETER_TYPE_TIN					);	}
	if( !Identifier.Cmp(SG_T("points"		)) )	{	return( PARAMETER_TYPE_PointCloud			);	}

	if( !Identifier.Cmp(SG_T("grid_list"	)) )	{	return( PARAMETER_TYPE_Grid_List			);	}
	if( !Identifier.Cmp(SG_T("table_list"	)) )	{	return( PARAMETER_TYPE_Table_List			);	}
	if( !Identifier.Cmp(SG_T("shapes_list"	)) )	{	return( PARAMETER_TYPE_Shapes_List			);	}
	if( !Identifier.Cmp(SG_T("tin_list"		)) )	{	return( PARAMETER_TYPE_TIN_List				);	}
	if( !Identifier.Cmp(SG_T("points_list"	)) )	{	return( PARAMETER_TYPE_PointCloud_List		);	}

	if( !Identifier.Cmp(SG_T("parameters"	)) )	{	return( PARAMETER_TYPE_Parameters			);	}

	return( PARAMETER_TYPE_Undefined );
}


///////////////////////////////////////////////////////////
//														 //
//						Base Class						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Data::CSG_Parameter_Data(CSG_Parameter *pOwner, long Constraint)
{
	m_pOwner		= pOwner;
	m_Constraint	= Constraint;
}

//---------------------------------------------------------
CSG_String CSG_Parameter_Data::Get_Type_Identifier(void)
{
	return( SG_Parameter_Type_Get_Identifier(Get_Type()) );
}

//---------------------------------------------------------
CSG_String CSG_Parameter_Data::Get_Type_Name(void)
{
	return( SG_Parameter_Type_Get_Name(Get_Type()) );
}

//---------------------------------------------------------
bool CSG_Parameter_Data::Set_Value(int Value)
{
	return( false );
}

bool CSG_Parameter_Data::Set_Value(double Value)
{
	return( false );
}

bool CSG_Parameter_Data::Set_Value(const CSG_String &Value)
{
	return( false );
}

bool CSG_Parameter_Data::Set_Value(void *Value)
{
	return( false );
}

//---------------------------------------------------------
int CSG_Parameter_Data::asInt(void)
{
	return( 0 );
}

double CSG_Parameter_Data::asDouble(void)
{
	return( 0.0 );
}

void * CSG_Parameter_Data::asPointer(void)
{
	return( NULL );
}

const SG_Char * CSG_Parameter_Data::asString(void)
{
	return( NULL );
}

//---------------------------------------------------------
void CSG_Parameter_Data::Set_Default(int            Value)
{
	m_Default.Printf(SG_T("%d"), Value);
}

void CSG_Parameter_Data::Set_Default(double         Value)
{
	m_Default.Printf(SG_T("%f"), Value);
}

void CSG_Parameter_Data::Set_Default(const CSG_String &Value)
{
	m_Default	= Value;
}

//---------------------------------------------------------
bool CSG_Parameter_Data::Assign(CSG_Parameter_Data *pSource)
{
	if( pSource && Get_Type() == pSource->Get_Type() )
	{
		m_Default	= pSource->m_Default;

		On_Assign(pSource);

		return( true );
	}

	return( false );
}

void CSG_Parameter_Data::On_Assign(CSG_Parameter_Data *pSource)
{}

//---------------------------------------------------------
bool CSG_Parameter_Data::Serialize(CSG_MetaData &Entry, bool bSave)
{
	return( On_Serialize(Entry, bSave) );
}

bool CSG_Parameter_Data::On_Serialize(CSG_MetaData &Entry, bool bSave)
{
	Entry.Set_Content(SG_T("-"));

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Node							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Node::CSG_Parameter_Node(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data(pOwner, Constraint)
{}


///////////////////////////////////////////////////////////
//														 //
//						Bool							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Bool::CSG_Parameter_Bool(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data(pOwner, Constraint)
{
	m_Value		= false;
}

//---------------------------------------------------------
bool CSG_Parameter_Bool::Set_Value(int Value)
{
	bool	bValue = Value != 0;

	if( m_Value != bValue )
	{
		m_Value		= bValue;

		return( true );
	}

	return( false );
}

bool CSG_Parameter_Bool::Set_Value(double Value)
{
	return( Set_Value((int)Value) );
}

//---------------------------------------------------------
const SG_Char * CSG_Parameter_Bool::asString(void)
{
	m_String	= m_Value ? _TL("yes") : _TL("no");

	return( m_String );
}

//---------------------------------------------------------
void CSG_Parameter_Bool::On_Assign(CSG_Parameter_Data *pSource)
{
	m_Value		= ((CSG_Parameter_Bool *)pSource)->m_Value;
}

//---------------------------------------------------------
bool CSG_Parameter_Bool::On_Serialize(CSG_MetaData &Entry, bool bSave)
{
	if( bSave )
	{
		Entry.Set_Content(m_Value ? SG_T("TRUE") : SG_T("FALSE"));
	}
	else
	{
		m_Value	= Entry.Cmp_Content(SG_T("TRUE"), true);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Value							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Value::CSG_Parameter_Value(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data(pOwner, Constraint)
{
	m_Minimum	= 0.0;
	m_bMinimum	= false;

	m_Maximum	= 0.0;
	m_bMaximum	= false;
}

//---------------------------------------------------------
bool CSG_Parameter_Value::Set_Range(double Minimum, double Maximum)
{
	if( 1 )
	{
		m_Minimum		= Minimum;
		m_Maximum		= Maximum;

		switch( Get_Type() )
		{
		default:
			return( false );

		case PARAMETER_TYPE_Int:
			Set_Value(asInt());
			break;

		case PARAMETER_TYPE_Double:
		case PARAMETER_TYPE_Degree:
			Set_Value(asDouble());
			break;
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CSG_Parameter_Value::Set_Minimum(double Minimum, bool bOn)
{
	if( bOn == false || (m_bMaximum && Minimum >= m_Maximum) )
	{
		m_bMinimum	= false;
	}
	else
	{
		m_bMinimum	= true;

		Set_Range(Minimum, m_Maximum);
	}
}

void CSG_Parameter_Value::Set_Maximum(double Maximum, bool bOn)
{
	if( bOn == false || (m_bMaximum && Maximum <= m_Minimum) )
	{
		m_bMaximum	= false;
	}
	else
	{
		m_bMaximum	= true;

		Set_Range(m_Minimum, Maximum);
	}
}

//---------------------------------------------------------
void CSG_Parameter_Value::On_Assign(CSG_Parameter_Data *pSource)
{
	m_Minimum	= ((CSG_Parameter_Value *)pSource)->m_Minimum;
	m_bMinimum	= ((CSG_Parameter_Value *)pSource)->m_bMinimum;

	m_Maximum	= ((CSG_Parameter_Value *)pSource)->m_Maximum;
	m_bMaximum	= ((CSG_Parameter_Value *)pSource)->m_bMaximum;
}


///////////////////////////////////////////////////////////
//														 //
//						Int								 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Int::CSG_Parameter_Int(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Value(pOwner, Constraint)
{
	m_Value		= 0;
}

//---------------------------------------------------------
bool CSG_Parameter_Int::Set_Value(int Value)
{
	if( m_bMinimum && Value < m_Minimum )
	{
		return( Set_Value((int)m_Minimum) );
	}

	if( m_bMaximum && Value > m_Maximum )
	{
		return( Set_Value((int)m_Maximum) );
	}

	if( m_Value != Value )
	{
		m_Value	= Value;

		return( true );
	}

	return( false );
}

bool CSG_Parameter_Int::Set_Value(double Value)
{
	return( Set_Value((int)Value) );
}

bool CSG_Parameter_Int::Set_Value(const CSG_String &Value)
{
	int		i;

	if( Value.asInt(i) )
	{
		return( Set_Value(i) );
	}

	return( false );
}

//---------------------------------------------------------
const SG_Char * CSG_Parameter_Int::asString(void)
{
	m_String.Printf(SG_T("%d"), m_Value);

	return( m_String );
}

//---------------------------------------------------------
void CSG_Parameter_Int::On_Assign(CSG_Parameter_Data *pSource)
{
	CSG_Parameter_Value::On_Assign(pSource);

	Set_Value(((CSG_Parameter_Value *)pSource)->asInt());
}

//---------------------------------------------------------
bool CSG_Parameter_Int::On_Serialize(CSG_MetaData &Entry, bool bSave)
{
	if( bSave )
	{
		Entry.Fmt_Content(SG_T("%d"), m_Value);
	}
	else
	{
		return( Entry.Get_Content().asInt(m_Value) );
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Double							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Double::CSG_Parameter_Double(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Value(pOwner, Constraint)
{
	m_Value		= 0.0;
}

//---------------------------------------------------------
bool CSG_Parameter_Double::Set_Value(int Value)
{
	return( Set_Value((double)Value) );
}

bool CSG_Parameter_Double::Set_Value(double Value)
{
	if( m_bMinimum && Value < m_Minimum )
	{
		return( Set_Value(m_Minimum) );
	}

	if( m_bMaximum && Value > m_Maximum )
	{
		return( Set_Value(m_Maximum) );
	}

	if( m_Value != Value )
	{
		m_Value	= Value;

		return( true );
	}

	return( false );
}

bool CSG_Parameter_Double::Set_Value(const CSG_String &Value)
{
	double	d;

	if( Value.asDouble(d) )
	{
		return( Set_Value(d) );
	}

	return( false );
}

//---------------------------------------------------------
const SG_Char * CSG_Parameter_Double::asString(void)
{
	m_String.Printf(SG_T("%f"), m_Value);

	return( m_String );
}

//---------------------------------------------------------
void CSG_Parameter_Double::On_Assign(CSG_Parameter_Data *pSource)
{
	CSG_Parameter_Value::On_Assign(pSource);

	Set_Value(((CSG_Parameter_Value *)pSource)->asDouble());
}

//---------------------------------------------------------
bool CSG_Parameter_Double::On_Serialize(CSG_MetaData &Entry, bool bSave)
{
	if( bSave )
	{
		Entry.Fmt_Content(SG_T("%f"), m_Value);
	}
	else
	{
		return( Entry.Get_Content().asDouble(m_Value) );
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Degree							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Degree::CSG_Parameter_Degree(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Double(pOwner, Constraint)
{}

//---------------------------------------------------------
bool CSG_Parameter_Degree::Set_Value(const CSG_String &Value)
{
	return( CSG_Parameter_Double::Set_Value(SG_Degree_To_Double(Value)) );
}

//---------------------------------------------------------
const SG_Char * CSG_Parameter_Degree::asString(void)
{
	m_String	= SG_Double_To_Degree(asDouble());

	return( m_String );
}


///////////////////////////////////////////////////////////
//														 //
//						Range							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Range::CSG_Parameter_Range(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data(pOwner, Constraint)
{
	m_pRange	= new CSG_Parameters;

	if( (m_Constraint & PARAMETER_INFORMATION) != 0 )
	{
		m_pLo	= m_pRange->Add_Info_Value(m_pOwner, SG_T("MIN"), SG_T("Minimum"), m_pOwner->Get_Description(), PARAMETER_TYPE_Double);
		m_pHi	= m_pRange->Add_Info_Value(m_pOwner, SG_T("MAX"), SG_T("Maximum"), m_pOwner->Get_Description(), PARAMETER_TYPE_Double);
	}
	else
	{
		m_pLo	= m_pRange->Add_Value     (m_pOwner, SG_T("MIN"), SG_T("Minimum"), m_pOwner->Get_Description(), PARAMETER_TYPE_Double);
		m_pHi	= m_pRange->Add_Value     (m_pOwner, SG_T("MAX"), SG_T("Maximum"), m_pOwner->Get_Description(), PARAMETER_TYPE_Double);
	}
}

CSG_Parameter_Range::~CSG_Parameter_Range(void)
{
	delete(m_pRange);
}

//---------------------------------------------------------
const SG_Char * CSG_Parameter_Range::asString(void)
{
	m_String.Printf(SG_T("[%f] - [%f]"), Get_LoVal(), Get_HiVal());

	return( m_String );
}

//---------------------------------------------------------
bool CSG_Parameter_Range::Set_Range(double loVal, double hiVal)
{
	bool	bResult;

	if( loVal > hiVal )
	{
		bResult	 = m_pLo->Set_Value(hiVal);
		bResult	|= m_pHi->Set_Value(loVal);
	}
	else
	{
		bResult	 = m_pLo->Set_Value(loVal);
		bResult	|= m_pHi->Set_Value(hiVal);
	}

	return( bResult );
}

//---------------------------------------------------------
bool CSG_Parameter_Range::Set_LoVal(double newValue)
{
	return( m_pLo->Set_Value(newValue) );
}

double CSG_Parameter_Range::Get_LoVal(void)
{
	return( m_pLo->asDouble() );
}

//---------------------------------------------------------
bool CSG_Parameter_Range::Set_HiVal(double newValue)
{
	return( m_pHi->Set_Value(newValue) );
}

double CSG_Parameter_Range::Get_HiVal(void)
{
	return( m_pHi->asDouble() );
}

//---------------------------------------------------------
bool CSG_Parameter_Range::Restore_Default(void)
{
	return( m_pLo->Restore_Default() && m_pHi->Restore_Default() );
}

//---------------------------------------------------------
void CSG_Parameter_Range::On_Assign(CSG_Parameter_Data *pSource)
{
	m_pLo->Assign(((CSG_Parameter_Range *)pSource)->m_pLo);
	m_pHi->Assign(((CSG_Parameter_Range *)pSource)->m_pHi);
}

//---------------------------------------------------------
bool CSG_Parameter_Range::On_Serialize(CSG_MetaData &Entry, bool bSave)
{
	if( bSave )
	{
		Entry.Fmt_Content(SG_T("%f; %f"), Get_LoVal(), Get_HiVal());

		return( true );
	}
	else
	{
		double		loVal, hiVal;
		CSG_String	s(Entry.Get_Content());

		if( s.BeforeFirst(SG_T(';')).asDouble(loVal) && s.AfterFirst(SG_T(';')).asDouble(hiVal) )
		{
			return( Set_Range(loVal, hiVal) );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//						Choice							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Choice::CSG_Parameter_Choice(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Int(pOwner, Constraint)
{}

//---------------------------------------------------------
bool CSG_Parameter_Choice::Set_Value(const CSG_String &Value)
{
	for(int i=0; i<m_Items.Get_Count(); i++)
	{
		if( m_Items[i].Cmp(Value) == 0 )
		{
			m_Value	= i;

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
const SG_Char * CSG_Parameter_Choice::asString(void)
{
	m_String	= Get_Item(m_Value) ? Get_Item(m_Value) : _TL("[no choice available]");

	return( m_String );
}

//---------------------------------------------------------
void CSG_Parameter_Choice::Set_Items(const SG_Char *String)
{
	m_Items.Clear();

	if( String && *String != '\0' )
	{
		CSG_String	Items(String);

		while( Items.Length() > 0 )
		{
			CSG_String	Item(Items.BeforeFirst('|'));

			if( Item.Length() )
			{
				m_Items	+= Item;
			}

			Items	= Items.AfterFirst('|');
		}
	}

	if( m_Items.Get_Count() <= 0 )
	{
		m_Items	+= _TL("[VAL] [not set]");
	}

	Set_Minimum(              0, true);
	Set_Maximum(Get_Count() - 1, true);

	CSG_Parameter_Int::Set_Value(m_Value);
}

//---------------------------------------------------------
const SG_Char * CSG_Parameter_Choice::Get_Item(int Index)
{
	if( Index >= 0 && Index < m_Items.Get_Count() )
	{
		const SG_Char	*Item	= m_Items[Index].c_str();

		if( *Item == SG_T('{') )
		{
			do	{	Item++;	}	while( *Item != SG_T('\0') && *Item != SG_T('}') );

			if( *Item == SG_T('\0') )
			{
				return( m_Items[Index].c_str() );
			}

			Item++;
		}

		return( Item );
	}

	return( NULL );
}

//---------------------------------------------------------
bool CSG_Parameter_Choice::Get_Data(int        &Value)
{
	CSG_String	sValue;

	if( Get_Data(sValue) )
	{
		return( sValue.asInt(Value) );
	}

	return( false );
}

bool CSG_Parameter_Choice::Get_Data(double     &Value)
{
	CSG_String	sValue;

	if( Get_Data(sValue) )
	{
		return( sValue.asDouble(Value) );
	}

	return( false );
}

bool CSG_Parameter_Choice::Get_Data(CSG_String &Value)
{
	if( m_Value >= 0 && m_Value < m_Items.Get_Count() )
	{
		const SG_Char	*Item	= m_Items[m_Value].c_str();

		if( *Item == SG_T('{') )
		{
			Item++;

			Value.Clear();

			do
			{
				Value	+= *(Item++);
			}
			while( *Item && *Item != SG_T('}') );

			return( Value.Length() > 0 );
		}
	}

	return( false );
}

//---------------------------------------------------------
void CSG_Parameter_Choice::On_Assign(CSG_Parameter_Data *pSource)
{
	m_Items	= ((CSG_Parameter_Choice *)pSource)->m_Items;

	CSG_Parameter_Int::On_Assign(pSource);
}


///////////////////////////////////////////////////////////
//														 //
//						String							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_String::CSG_Parameter_String(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data(pOwner, Constraint)
{
	m_bPassword	= false;
}

//---------------------------------------------------------
bool CSG_Parameter_String::is_Valid(void)
{
	return( m_String.Length() > 0 );
}

//---------------------------------------------------------
bool CSG_Parameter_String::Set_Value(const CSG_String &Value)
{
	if( m_String.Cmp(Value) )
	{
		m_String	= Value;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CSG_Parameter_String::On_Assign(CSG_Parameter_Data *pSource)
{
	m_String	= ((CSG_Parameter_String *)pSource)->m_String.c_str();

	m_bPassword	= ((CSG_Parameter_String *)pSource)->m_bPassword;
}

//---------------------------------------------------------
bool CSG_Parameter_String::On_Serialize(CSG_MetaData &Entry, bool bSave)
{
	if( bSave )
	{
		Entry.Set_Content(m_String);
	}
	else
	{
		m_String	= Entry.Get_Content();
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Text							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Text::CSG_Parameter_Text(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_String(pOwner, Constraint)
{}


///////////////////////////////////////////////////////////
//														 //
//						FilePath						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_File_Name::CSG_Parameter_File_Name(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_String(pOwner, Constraint)
{
	m_Filter.Printf(SG_T("%s|*.*"), _TL("All Files"));

	m_bSave			= false;
	m_bMultiple		= false;
	m_bDirectory	= false;
}

//---------------------------------------------------------
void CSG_Parameter_File_Name::Set_Filter(const SG_Char *Filter)
{
	if( Filter )
	{
		m_Filter	= Filter;
	}
	else
	{
		m_Filter.Printf(SG_T("%s|*.*"), _TL("All Files"));
	}
}

const SG_Char *  CSG_Parameter_File_Name::Get_Filter(void)
{
	return( m_Filter.c_str() );
}

//---------------------------------------------------------
void CSG_Parameter_File_Name::Set_Flag_Save(bool bFlag)
{
	m_bSave			= bFlag;
}

void CSG_Parameter_File_Name::Set_Flag_Multiple(bool bFlag)
{
	m_bMultiple		= bFlag;
}

void CSG_Parameter_File_Name::Set_Flag_Directory(bool bFlag)
{
	m_bDirectory	= bFlag;
}

//---------------------------------------------------------
bool CSG_Parameter_File_Name::Get_FilePaths(CSG_Strings &FilePaths)
{
	FilePaths.Clear();

	if( m_String.Length() > 0 )
	{
		if( !m_bMultiple )
		{
			FilePaths.Add(m_String);
		}
		else if( m_String[0] != '\"' )
		{
			FilePaths.Add(m_String);
		}
		else
		{
			CSG_String	s(m_String), sTmp;

			while( s.Length() > 2 )
			{
				s	= s.AfterFirst('\"');
				FilePaths.Add(s.BeforeFirst('\"'));
				s	= s.AfterFirst('\"');
			}
		}
	}

	return( FilePaths.Get_Count() > 0 );
}

//---------------------------------------------------------
void CSG_Parameter_File_Name::On_Assign(CSG_Parameter_Data *pSource)
{
	CSG_Parameter_String::On_Assign(pSource);

	Set_Filter(((CSG_Parameter_File_Name *)pSource)->m_Filter.c_str());

	m_bSave			= ((CSG_Parameter_File_Name *)pSource)->m_bSave;
	m_bMultiple		= ((CSG_Parameter_File_Name *)pSource)->m_bMultiple;
	m_bDirectory	= ((CSG_Parameter_File_Name *)pSource)->m_bDirectory;
}


///////////////////////////////////////////////////////////
//														 //
//						Font							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Font::CSG_Parameter_Font(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data(pOwner, Constraint)
{
	Restore_Default();
}

//---------------------------------------------------------
bool CSG_Parameter_Font::Restore_Default(void)
{
	m_Color		= SG_GET_RGB(0, 0, 0);
	m_Font		= SG_T("0;-13;0;0;0;400;0;0;0;0;3;2;1;34;Arial");
	m_String	= SG_T("Arial");

	return( true );
}

//---------------------------------------------------------
bool CSG_Parameter_Font::Set_Value(int Value)
{
	m_Color	= Value;

	return( true );
}

//---------------------------------------------------------
bool CSG_Parameter_Font::Set_Value(const CSG_String &Value)
{
	if( Value.Length() > 0 )
	{
		m_Font		= Value;
		m_String	= m_Font.AfterLast(SG_T(';'));
	}
	else
	{
		Restore_Default();
	}

	return( true );
}

//---------------------------------------------------------
void CSG_Parameter_Font::On_Assign(CSG_Parameter_Data *pSource)
{
	if( pSource && pSource->Get_Type() == Get_Type() )
	{
		m_Color		= ((CSG_Parameter_Font *)pSource)->m_Color;
		m_Font		= ((CSG_Parameter_Font *)pSource)->m_Font;
		m_String	= ((CSG_Parameter_Font *)pSource)->m_String;
	}
}

//---------------------------------------------------------
bool CSG_Parameter_Font::On_Serialize(CSG_MetaData &Entry, bool bSave)
{
	if( bSave )
	{
		Entry.Add_Child(SG_T("COLOR")	, CSG_String::Format(SG_T("R%03d G%03d B%03d"),
			SG_GET_R(m_Color),
			SG_GET_G(m_Color),
			SG_GET_B(m_Color)
		));

		Entry.Add_Child(SG_T("FONT")	, m_Font);
	}
	else
	{
		CSG_MetaData	*pEntry;

		if( (pEntry = Entry.Get_Child(SG_T("COLOR"))) != NULL )
		{
			Set_Value((int)SG_GET_RGB(
				pEntry->Get_Content().AfterFirst(SG_T('R')).asInt(),
				pEntry->Get_Content().AfterFirst(SG_T('G')).asInt(),
				pEntry->Get_Content().AfterFirst(SG_T('B')).asInt()
			));
		}

		if( (pEntry = Entry.Get_Child(SG_T("FONT"))) != NULL )
		{
			Set_Value(pEntry->Get_Content());
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Color							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Color::CSG_Parameter_Color(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Int(pOwner, Constraint)
{}

//---------------------------------------------------------
bool CSG_Parameter_Color::On_Serialize(CSG_MetaData &Entry, bool bSave)
{
	if( bSave )
	{
		Entry.Fmt_Content(SG_T("R%03d G%03d B%03d"), SG_GET_R(m_Value), SG_GET_G(m_Value), SG_GET_B(m_Value));
	}
	else
	{
		m_Value	= SG_GET_RGB(
			Entry.Get_Content().AfterFirst(SG_T('R')).asInt(),
			Entry.Get_Content().AfterFirst(SG_T('G')).asInt(),
			Entry.Get_Content().AfterFirst(SG_T('B')).asInt()
		);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Colors							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Colors::CSG_Parameter_Colors(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data(pOwner, Constraint)
{}

//---------------------------------------------------------
const SG_Char * CSG_Parameter_Colors::asString(void)
{
	m_String.Printf(SG_T("%d %s"), m_Colors.Get_Count(), _TL("colors"));

	return( m_String );
}

//---------------------------------------------------------
void CSG_Parameter_Colors::On_Assign(CSG_Parameter_Data *pSource)
{
	m_Colors.Assign(&((CSG_Parameter_Colors *)pSource)->m_Colors);
}

//---------------------------------------------------------
bool CSG_Parameter_Colors::On_Serialize(CSG_MetaData &Entry, bool bSave)
{
	if( bSave )
	{
		for(int i=0; i<m_Colors.Get_Count(); i++)
		{
			Entry.Add_Child(SG_T("COLOR"), CSG_String::Format(SG_T("R%03d G%03d B%03d"), m_Colors.Get_Red(i), m_Colors.Get_Green(i), m_Colors.Get_Blue(i)));
		}
	}
	else
	{
		if( Entry.Get_Children_Count() <= 1 )
		{
			return( false );
		}

		m_Colors.Set_Count(Entry.Get_Children_Count());

		for(int i=0; i<m_Colors.Get_Count(); i++)
		{
			CSG_String	s(Entry.Get_Child(i)->Get_Content());

			m_Colors.Set_Red  (i, s.AfterFirst(SG_T('R')).asInt());
			m_Colors.Set_Green(i, s.AfterFirst(SG_T('G')).asInt());
			m_Colors.Set_Blue (i, s.AfterFirst(SG_T('B')).asInt());
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						FixedTable						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Fixed_Table::CSG_Parameter_Fixed_Table(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data(pOwner, Constraint)
{
	m_Table.Set_Name(_TL("Table"));
}

//---------------------------------------------------------
const SG_Char * CSG_Parameter_Fixed_Table::asString(void)
{
	m_String.Printf(SG_T("%s (%s: %d, %s: %d)"), m_Table.Get_Name(), _TL("columns"), m_Table.Get_Field_Count(), _TL("rows"), m_Table.Get_Record_Count());

	return( m_String );
}

//---------------------------------------------------------
void CSG_Parameter_Fixed_Table::On_Assign(CSG_Parameter_Data *pSource)
{
	m_Table.Assign(&((CSG_Parameter_Fixed_Table *)pSource)->m_Table);
}

//---------------------------------------------------------
bool CSG_Parameter_Fixed_Table::On_Serialize(CSG_MetaData &Entry, bool bSave)
{
	int				iField, iRecord;
	CSG_MetaData	*pNode, *pEntry;

	if( bSave )
	{
		pNode	= Entry.Add_Child(SG_T("FIELDS"));

		for(iField=0; iField<m_Table.Get_Field_Count(); iField++)
		{
			pEntry	= pNode->Add_Child	(SG_T("FIELD")	, m_Table.Get_Field_Name(iField));
			pEntry	->Set_Property		(SG_T("type")	, gSG_Data_Type_Identifier[m_Table.Get_Field_Type(iField)]);
		}

		pNode	= Entry.Add_Child(SG_T("RECORDS"));

		for(iRecord=0; iRecord<m_Table.Get_Count(); iRecord++)
		{
			pEntry	= pNode->Add_Child(SG_T("RECORD"));

			for(iField=0; iField<m_Table.Get_Field_Count(); iField++)
			{
				pEntry->Add_Child(SG_T("FIELD"), m_Table[iRecord].asString(iField));
			}
		}
	}
	else
	{
		CSG_Table	t;

		if( (pNode = Entry.Get_Child(SG_T("FIELDS"))) == NULL )
		{
			return( false );
		}

		for(iField=0; iField<pNode->Get_Children_Count(); iField++)
		{
			CSG_String		s;
			TSG_Data_Type	type	= SG_DATATYPE_String;

			if( pNode->Get_Child(iField)->Get_Property(SG_T("type"), s) )
			{
				     if( !s.Cmp(gSG_Data_Type_Identifier[SG_DATATYPE_Bit   ]) )	type	= SG_DATATYPE_Bit;
				else if( !s.Cmp(gSG_Data_Type_Identifier[SG_DATATYPE_Byte  ]) )	type	= SG_DATATYPE_Byte;
				else if( !s.Cmp(gSG_Data_Type_Identifier[SG_DATATYPE_Char  ]) )	type	= SG_DATATYPE_Char;
				else if( !s.Cmp(gSG_Data_Type_Identifier[SG_DATATYPE_Word  ]) )	type	= SG_DATATYPE_Word;
				else if( !s.Cmp(gSG_Data_Type_Identifier[SG_DATATYPE_Short ]) )	type	= SG_DATATYPE_Short;
				else if( !s.Cmp(gSG_Data_Type_Identifier[SG_DATATYPE_DWord ]) )	type	= SG_DATATYPE_DWord;
				else if( !s.Cmp(gSG_Data_Type_Identifier[SG_DATATYPE_Int   ]) )	type	= SG_DATATYPE_Int;
				else if( !s.Cmp(gSG_Data_Type_Identifier[SG_DATATYPE_ULong ]) )	type	= SG_DATATYPE_ULong;
				else if( !s.Cmp(gSG_Data_Type_Identifier[SG_DATATYPE_Long  ]) )	type	= SG_DATATYPE_Long;
				else if( !s.Cmp(gSG_Data_Type_Identifier[SG_DATATYPE_Float ]) )	type	= SG_DATATYPE_Float;
				else if( !s.Cmp(gSG_Data_Type_Identifier[SG_DATATYPE_Double]) )	type	= SG_DATATYPE_Double;
				else if( !s.Cmp(gSG_Data_Type_Identifier[SG_DATATYPE_String]) )	type	= SG_DATATYPE_String;
				else if( !s.Cmp(gSG_Data_Type_Identifier[SG_DATATYPE_Date  ]) )	type	= SG_DATATYPE_Date;
				else if( !s.Cmp(gSG_Data_Type_Identifier[SG_DATATYPE_Color ]) )	type	= SG_DATATYPE_Color;
			}

			t.Add_Field(pNode->Get_Child(iField)->Get_Content(), type);
		}

		if( (pNode = Entry.Get_Child(SG_T("RECORDS"))) == NULL )
		{
			return( false );
		}

		for(iRecord=0; iRecord<pNode->Get_Children_Count(); iRecord++)
		{
			pEntry	= pNode->Get_Child(iRecord);

			t.Add_Record();

			for(iField=0; iField<pEntry->Get_Children_Count(); iField++)
			{
				t[iRecord].Set_Value(iField, pEntry->Get_Child(iField)->Get_Content());
			}
		}

		return( m_Table.Assign_Values(&t) );
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Grid_System						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Grid_System::CSG_Parameter_Grid_System(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data(pOwner, Constraint)
{}

//---------------------------------------------------------
bool CSG_Parameter_Grid_System::Set_Value(void *Value)
{
	int						i, j;
	CSG_Grid				*pGrid;
	CSG_Grid_System			Invalid;
	CSG_Parameters			*pParameters;
	CSG_Parameter_Grid_List	*pGrids;

	//-----------------------------------------------------
	if( Value == NULL )
	{
		Value	= &Invalid;
	}

	//-----------------------------------------------------
	if( !m_System.is_Equal(*((CSG_Grid_System *)Value)) )
	{
		m_System.Assign(*((CSG_Grid_System *)Value));

		//-------------------------------------------------
		pParameters	= m_pOwner->Get_Owner();

		for(i=0; i<pParameters->Get_Count(); i++)
		{
			if(	pParameters->Get_Parameter(i)->Get_Parent() == m_pOwner )
			{
				switch( pParameters->Get_Parameter(i)->Get_Type() )
				{
				default:
					break;

				case PARAMETER_TYPE_Grid:
					pGrid	= pParameters->Get_Parameter(i)->asGrid();

					if(	!m_System.is_Valid() || !SG_UI_DataObject_Check(pGrid, DATAOBJECT_TYPE_Grid) || (pGrid != DATAOBJECT_NOTSET && pGrid != DATAOBJECT_CREATE && !m_System.is_Equal(pGrid->Get_System())) )
					{
						pParameters->Get_Parameter(i)->Set_Value(DATAOBJECT_NOTSET);
					}
					break;

				case PARAMETER_TYPE_Grid_List:
					pGrids	= (CSG_Parameter_Grid_List *)pParameters->Get_Parameter(i)->Get_Data();

					for(j=pGrids->Get_Count()-1; j>=0; j--)
					{
						if( !m_System.is_Valid() || !SG_UI_DataObject_Check(pGrids->asGrid(j), DATAOBJECT_TYPE_Grid) || m_System.is_Equal(pGrids->asGrid(j)->Get_System()) == false )
						{
							pGrids->Del_Item(j);
						}
					}
					break;
				}
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
const SG_Char * CSG_Parameter_Grid_System::asString(void)
{
	m_String	= m_System.Get_Name();

	return( m_String );
}

//---------------------------------------------------------
void CSG_Parameter_Grid_System::On_Assign(CSG_Parameter_Data *pSource)
{
	m_System	= ((CSG_Parameter_Grid_System *)pSource)->m_System;
}

//---------------------------------------------------------
bool CSG_Parameter_Grid_System::On_Serialize(CSG_MetaData &Entry, bool bSave)
{
	if( bSave )
	{
		Entry.Add_Child(SG_T("CELLSIZE"), m_System.Get_Cellsize());
		Entry.Add_Child(SG_T("XMIN")    , m_System.Get_Extent().Get_XMin());
		Entry.Add_Child(SG_T("XMAX")    , m_System.Get_Extent().Get_XMax());
		Entry.Add_Child(SG_T("YMIN")    , m_System.Get_Extent().Get_YMin());
		Entry.Add_Child(SG_T("YMAX")    , m_System.Get_Extent().Get_YMax());
	}
	else
	{
		double		Cellsize;
		TSG_Rect	Extent;

		Cellsize	= Entry.Get_Child(SG_T("CELLSIZE"))->Get_Content().asDouble();
		Extent.xMin	= Entry.Get_Child(SG_T("XMIN"))    ->Get_Content().asDouble();
		Extent.xMax	= Entry.Get_Child(SG_T("XMAX"))    ->Get_Content().asDouble();
		Extent.yMin	= Entry.Get_Child(SG_T("YMIN"))    ->Get_Content().asDouble();
		Extent.yMax	= Entry.Get_Child(SG_T("YMAX"))    ->Get_Content().asDouble();

		m_System.Assign(Cellsize, CSG_Rect(Extent));
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//					Table Field							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Table_Field::CSG_Parameter_Table_Field(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Int(pOwner, Constraint)
{}

//---------------------------------------------------------
const SG_Char * CSG_Parameter_Table_Field::asString(void)
{
	CSG_Table	*pTable;

	if( (pTable = Get_Table()) != NULL )
	{
		if( m_Value >= 0 && m_Value < pTable->Get_Field_Count() )
		{
			return( m_String = pTable->Get_Field_Name(m_Value) );
		}

		return( m_String = _TL("[not set]") );
	}

	return( m_String = _TL("[no attributes]") );
}

//---------------------------------------------------------
bool CSG_Parameter_Table_Field::Set_Value(int Value)
{
	CSG_Table	*pTable;

	if( (pTable = Get_Table()) != NULL )
	{
		if( !m_pOwner->is_Optional() )
		{
			if( Value < 0 )
			{
				Value	= 0;
			}
			else if( Value >= pTable->Get_Field_Count() )
			{
				Value	= pTable->Get_Field_Count() - 1;
			}
		}
		else if( Value < 0 || Value >= pTable->Get_Field_Count() )
		{
			Value	= -1;
		}
	}
	else
	{
		Value	= -1;
	}

	if( m_Value != Value )
	{
		m_Value		= Value;
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Parameter_Table_Field::Set_Value(const CSG_String &Value)
{
	CSG_Table	*pTable;

	if( Value.Length() > 0 && (pTable = Get_Table()) != NULL )
	{
		for(int i=0; i<pTable->Get_Field_Count(); i++)
		{
			if( Value.CmpNoCase(pTable->Get_Field_Name(i)) == 0 )
			{
				m_Value	= i;

				return( true );
			}
		}
	}

	return( false );
}

//---------------------------------------------------------
CSG_Table * CSG_Parameter_Table_Field::Get_Table(void)
{
	CSG_Table		*pTable;
	CSG_Parameter	*pParent;

	pTable		= NULL;

	if( (pParent = m_pOwner->Get_Parent()) != NULL )
	{
		switch( m_pOwner->Get_Parent()->Get_Type() )
		{
		default:
			break;

		case PARAMETER_TYPE_Table:
		case PARAMETER_TYPE_Shapes:
		case PARAMETER_TYPE_TIN:
		case PARAMETER_TYPE_PointCloud:
			pTable	= pParent->asTable();
			break;
		}
	}

	return( pTable && pTable != DATAOBJECT_CREATE && pTable->Get_Field_Count() > 0 ? pTable : NULL );
}

//---------------------------------------------------------
void CSG_Parameter_Table_Field::On_Assign(CSG_Parameter_Data *pSource)
{
	CSG_Parameter_Int::On_Assign(pSource);
}


///////////////////////////////////////////////////////////
//														 //
//						DataObject						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Data_Object::CSG_Parameter_Data_Object(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data(pOwner, Constraint)
{
	m_pDataObject	= NULL;
}

//---------------------------------------------------------
bool CSG_Parameter_Data_Object::is_Valid(void)
{
	return(	m_pOwner->is_Optional() || (m_pDataObject && m_pDataObject->is_Valid()) );
}

//---------------------------------------------------------
bool CSG_Parameter_Data_Object::Set_Value(void *Value)
{
	if( m_pDataObject != Value )
	{
		m_pDataObject	= (CSG_Data_Object *)Value;
	}

	return( true );
}

//---------------------------------------------------------
const SG_Char * CSG_Parameter_Data_Object::asString(void)
{
	if( m_pDataObject == DATAOBJECT_NOTSET )
	{
		m_String	= m_pOwner->is_Output() && !m_pOwner->is_Optional()
					? _TL("[VAL] [create]")
					: _TL("[VAL] [not set]");
	}
	else if( m_pDataObject == DATAOBJECT_CREATE )
	{
		m_String	= _TL("[VAL] [create]");
	}
	else
	{
		m_String	= m_pDataObject->Get_Name();
	}

	return( m_String );
}

//---------------------------------------------------------
void CSG_Parameter_Data_Object::On_Assign(CSG_Parameter_Data *pSource)
{
	m_pDataObject	= ((CSG_Parameter_Data_Object *)pSource)->m_pDataObject;
}

//---------------------------------------------------------
bool CSG_Parameter_Data_Object::On_Serialize(CSG_MetaData &Entry, bool bSave)
{
	if( bSave )
	{
		if(	m_pDataObject == DATAOBJECT_CREATE )
		{
			Entry.Set_Content(SG_T("CREATE"));
		}
		else if( m_pDataObject == DATAOBJECT_NOTSET || !SG_File_Exists(m_pDataObject->Get_File_Name()) )
		{
			Entry.Set_Content(SG_T("NOT SET"));
		}
		else
		{
			Entry.Set_Content(m_pDataObject->Get_File_Name());
		}
	}
	else
	{
		if( Entry.Cmp_Content(SG_T("CREATE")) )
		{
			Set_Value(DATAOBJECT_CREATE);
		}
		else if( Entry.Cmp_Content(SG_T("NOT SET")) )
		{
			Set_Value(DATAOBJECT_NOTSET);
		}
		else
		{
			Set_Value(SG_UI_DataObject_Find(Entry.Get_Content(), -1));
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//					DataObject_Output					 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Data_Object_Output::CSG_Parameter_Data_Object_Output(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data_Object(pOwner, Constraint)
{
	m_Type	= DATAOBJECT_TYPE_Undefined;
}

//---------------------------------------------------------
bool CSG_Parameter_Data_Object_Output::Set_Value(void *Value)
{
	CSG_Data_Object	*pDataObject	= (CSG_Data_Object *)Value;

	if( pDataObject == DATAOBJECT_CREATE )
	{
		pDataObject	= NULL;
	}

	if( m_pDataObject != pDataObject && (pDataObject == NULL || pDataObject->Get_ObjectType() == m_Type) )
	{
		m_pDataObject	= pDataObject;

		SG_UI_DataObject_Add(m_pDataObject, false);
	}

	return( true );
}

//---------------------------------------------------------
bool CSG_Parameter_Data_Object_Output::Set_DataObject_Type(TSG_Data_Object_Type Type)
{
	if( m_Type == DATAOBJECT_TYPE_Undefined )	// m_Type should not be changed after set once!!!...
	{
		switch( Type )
		{
		default:
			break;

		case DATAOBJECT_TYPE_Grid:
		case DATAOBJECT_TYPE_Table:
		case DATAOBJECT_TYPE_Shapes:
		case DATAOBJECT_TYPE_TIN:
		case DATAOBJECT_TYPE_PointCloud:
			m_Type	= Type;
			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//						Grid							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Grid::CSG_Parameter_Grid(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data_Object(pOwner, Constraint)
{
	m_Type	= SG_DATATYPE_Undefined;
}

//---------------------------------------------------------
bool CSG_Parameter_Grid::Set_Value(void *Value)
{
	if( Value == DATAOBJECT_CREATE && !m_pOwner->is_Optional() )
	{
//		Value	= DATAOBJECT_NOTSET;
	}

	if( m_pDataObject == Value )
	{
		return( true );
	}

	CSG_Grid_System	*pSystem	= Get_System();

	if(	Value == DATAOBJECT_NOTSET || Value == DATAOBJECT_CREATE
	||	pSystem == NULL || pSystem->is_Equal(((CSG_Grid *)Value)->Get_System()) )
	{
		m_pDataObject	= (CSG_Data_Object *)Value;

		return( true );
	}
	else if( !m_pOwner->Get_Owner()->is_Managed() && pSystem != NULL )
	{
		pSystem->Assign(((CSG_Grid *)Value)->Get_System());
		m_pDataObject	= (CSG_Data_Object *)Value;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
CSG_Grid_System * CSG_Parameter_Grid::Get_System(void)
{
	if( m_pOwner->Get_Parent() && m_pOwner->Get_Parent()->Get_Type() == PARAMETER_TYPE_Grid_System )
	{
		return( m_pOwner->Get_Parent()->asGrid_System() );
	}

	return( NULL );
}

//---------------------------------------------------------
void CSG_Parameter_Grid::Set_Preferred_Type(TSG_Data_Type Type)
{
	m_Type	= Type;
}

//---------------------------------------------------------
void CSG_Parameter_Grid::On_Assign(CSG_Parameter_Data *pSource)
{
	CSG_Parameter_Data_Object::On_Assign(pSource);

	m_Type	= ((CSG_Parameter_Grid *)pSource)->m_Type;
}


///////////////////////////////////////////////////////////
//														 //
//						Table							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Table::CSG_Parameter_Table(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data_Object(pOwner, Constraint)
{}

//---------------------------------------------------------
bool CSG_Parameter_Table::Set_Value(void *Value)
{
	if( m_pDataObject == Value )
	{
		return( true );
	}

	m_pDataObject	= (CSG_Data_Object *)Value;

	CSG_Parameters	*pParameters	= m_pOwner->Get_Owner();

	for(int i=0; i<pParameters->Get_Count(); i++)
	{
		if(	pParameters->Get_Parameter(i)->Get_Parent() == m_pOwner
		&&	pParameters->Get_Parameter(i)->Get_Type()   == PARAMETER_TYPE_Table_Field )
		{
			pParameters->Get_Parameter(i)->Set_Value(0);
		}				
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Shapes							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Shapes::CSG_Parameter_Shapes(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data_Object(pOwner, Constraint)
{
	m_Type	= SHAPE_TYPE_Undefined;
}

//---------------------------------------------------------
bool CSG_Parameter_Shapes::Set_Value(void *Value)
{
	if(	Value != DATAOBJECT_NOTSET && Value != DATAOBJECT_CREATE
	&&	m_Type != SHAPE_TYPE_Undefined && m_Type != ((CSG_Shapes *)Value)->Get_Type() )
	{
		return( false );
	}

	if( m_pDataObject == Value )
	{
		return( true );
	}

	m_pDataObject	= (CSG_Data_Object *)Value;

	CSG_Parameters	*pParameters	= m_pOwner->Get_Owner();

	for(int i=0; i<pParameters->Get_Count(); i++)
	{
		if(	pParameters->Get_Parameter(i)->Get_Parent() == m_pOwner
		&&	pParameters->Get_Parameter(i)->Get_Type()   == PARAMETER_TYPE_Table_Field )
		{
			pParameters->Get_Parameter(i)->Set_Value(0);
		}				
	}

	return( true );
}

//---------------------------------------------------------
void CSG_Parameter_Shapes::Set_Shape_Type(TSG_Shape_Type Type)
{
	m_Type	= Type;
}

//---------------------------------------------------------
void CSG_Parameter_Shapes::On_Assign(CSG_Parameter_Data *pSource)
{
	CSG_Parameter_Data_Object::On_Assign(pSource);

	m_Type	= ((CSG_Parameter_Shapes *)pSource)->m_Type;
}


///////////////////////////////////////////////////////////
//														 //
//						TIN								 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_TIN::CSG_Parameter_TIN(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data_Object(pOwner, Constraint)
{}

//---------------------------------------------------------
bool CSG_Parameter_TIN::Set_Value(void *Value)
{
	if( m_pDataObject == Value )
	{
		return( true );
	}

	m_pDataObject	= (CSG_Data_Object *)Value;

	CSG_Parameters	*pParameters	= m_pOwner->Get_Owner();

	for(int i=0; i<pParameters->Get_Count(); i++)
	{
		if(	pParameters->Get_Parameter(i)->Get_Parent() == m_pOwner
		&&	pParameters->Get_Parameter(i)->Get_Type()   == PARAMETER_TYPE_Table_Field )
		{
			pParameters->Get_Parameter(i)->Set_Value(0);
		}				
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						PointCloud						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_PointCloud::CSG_Parameter_PointCloud(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data_Object(pOwner, Constraint)
{}

//---------------------------------------------------------
void CSG_Parameter_PointCloud::On_Assign(CSG_Parameter_Data *pSource)
{
	CSG_Parameter_Data_Object::On_Assign(pSource);
}

//---------------------------------------------------------
bool CSG_Parameter_PointCloud::Set_Value(void *Value)
{
	if( m_pDataObject == Value )
	{
		return( true );
	}

	m_pDataObject	= (CSG_Data_Object *)Value;

	CSG_Parameters	*pParameters	= m_pOwner->Get_Owner();

	for(int i=0; i<pParameters->Get_Count(); i++)
	{
		if(	pParameters->Get_Parameter(i)->Get_Parent() == m_pOwner
		&&	pParameters->Get_Parameter(i)->Get_Type()   == PARAMETER_TYPE_Table_Field )
		{
			pParameters->Get_Parameter(i)->Set_Value(0);
		}				
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						List							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_List::CSG_Parameter_List(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data(pOwner, Constraint)
{
	m_Objects	= NULL;
	m_nObjects	= 0;
}

CSG_Parameter_List::~CSG_Parameter_List(void)
{
	Del_Items();
}

//---------------------------------------------------------
const SG_Char * CSG_Parameter_List::asString(void)
{
	if( Get_Count() > 0 )
	{
		m_String.Printf(SG_T("%d %s ("), Get_Count(), Get_Count() == 1 ? _TL("[VAL] object") : _TL("[VAL] objects"));

		for(int i=0; i<Get_Count(); i++)
		{
			if( i > 0 )
			{
				m_String.Append(SG_T(", "));
			}

			m_String.Append(asDataObject(i)->Get_Name());
		}

		m_String.Append(SG_T("))"));
	}
	else
	{
		m_String.Printf(_TL("[VAL] No objects"));
	}

	return( m_String );
}

//---------------------------------------------------------
void CSG_Parameter_List::Add_Item(CSG_Data_Object *pObject)
{
	if( pObject )
	{
		m_Objects	= (CSG_Data_Object **)SG_Realloc(m_Objects, (m_nObjects + 1) * sizeof(CSG_Data_Object *));
		m_Objects[m_nObjects++]	= pObject;
	}
}

//---------------------------------------------------------
int CSG_Parameter_List::Del_Item(int iObject)
{
	int		i;

	if( iObject >= 0 && iObject < m_nObjects )
	{
		m_nObjects--;

		for(i=iObject; i<m_nObjects; i++)
		{
			m_Objects[i]	= m_Objects[i + 1];
		}

		m_Objects	= (CSG_Data_Object **)SG_Realloc(m_Objects, m_nObjects * sizeof(CSG_Data_Object *));
	}

	return( m_nObjects );
}

int CSG_Parameter_List::Del_Item(CSG_Data_Object *pObject)
{
	int		i;

	for(i=0; i<m_nObjects; i++)
	{
		if( pObject == m_Objects[i] )
		{
			return( Del_Item(i) );
		}
	}

	return( 0 );
}

//---------------------------------------------------------
void CSG_Parameter_List::Del_Items(void)
{
	if( m_nObjects > 0 )
	{
		SG_Free(m_Objects);
		m_Objects	= NULL;
		m_nObjects	= 0;
	}
}

//---------------------------------------------------------
void CSG_Parameter_List::On_Assign(CSG_Parameter_Data *pSource)
{
	Del_Items();

	for(int i=0; i<((CSG_Parameter_List *)pSource)->Get_Count(); i++)
	{
		Add_Item(((CSG_Parameter_List *)pSource)->asDataObject(i));
	}
}

//---------------------------------------------------------
bool CSG_Parameter_List::On_Serialize(CSG_MetaData &Entry, bool bSave)
{
	if( bSave )
	{
		for(int i=0; i<Get_Count(); i++)
		{
			if( SG_File_Exists(asDataObject(i)->Get_File_Name()) )
			{
				Entry.Add_Child(SG_T("DATA"), asDataObject(i)->Get_File_Name());
			}
		}
	}
	else
	{
		for(int i=0; i<Entry.Get_Children_Count(); i++)
		{
			CSG_Data_Object	*pObject	= SG_UI_DataObject_Find(Entry.Get_Child(i)->Get_Content(), -1);

			if( pObject )
			{
				Add_Item(pObject);
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//						Grid_List						 //
//														 //
///////////////////////////////////////////////////////////


//---------------------------------------------------------
CSG_Parameter_Grid_List::CSG_Parameter_Grid_List(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_List(pOwner, Constraint)
{}

//---------------------------------------------------------
CSG_Grid_System * CSG_Parameter_Grid_List::Get_System(void)
{
	if( m_pOwner->Get_Parent() && m_pOwner->Get_Parent()->Get_Type() == PARAMETER_TYPE_Grid_System )
	{
		return( m_pOwner->Get_Parent()->asGrid_System() );
	}

	return( NULL );
}


///////////////////////////////////////////////////////////
//														 //
//						Table_List						 //
//														 //
///////////////////////////////////////////////////////////


//---------------------------------------------------------
CSG_Parameter_Table_List::CSG_Parameter_Table_List(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_List(pOwner, Constraint)
{}


///////////////////////////////////////////////////////////
//														 //
//						Shapes_List						 //
//														 //
///////////////////////////////////////////////////////////


//---------------------------------------------------------
CSG_Parameter_Shapes_List::CSG_Parameter_Shapes_List(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_List(pOwner, Constraint)
{
	m_Type	= SHAPE_TYPE_Undefined;
}

//---------------------------------------------------------
void CSG_Parameter_Shapes_List::Set_Shape_Type(TSG_Shape_Type Type)
{
	m_Type	= Type;
}

//---------------------------------------------------------
void CSG_Parameter_Shapes_List::On_Assign(CSG_Parameter_Data *pSource)
{
	CSG_Parameter_List::On_Assign(pSource);

	m_Type	= ((CSG_Parameter_Shapes_List *)pSource)->m_Type;
}


///////////////////////////////////////////////////////////
//														 //
//						TIN_List						 //
//														 //
///////////////////////////////////////////////////////////


//---------------------------------------------------------
CSG_Parameter_TIN_List::CSG_Parameter_TIN_List(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_List(pOwner, Constraint)
{}


///////////////////////////////////////////////////////////
//														 //
//					PointCloud_List						 //
//														 //
///////////////////////////////////////////////////////////


//---------------------------------------------------------
CSG_Parameter_PointCloud_List::CSG_Parameter_PointCloud_List(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_List(pOwner, Constraint)
{}


///////////////////////////////////////////////////////////
//														 //
//						Parameters						 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter_Parameters::CSG_Parameter_Parameters(CSG_Parameter *pOwner, long Constraint)
	: CSG_Parameter_Data(pOwner, Constraint)
{
	m_pParameters	= new CSG_Parameters(pOwner->Get_Owner()->Get_Owner(), pOwner->Get_Name(), pOwner->Get_Description(), pOwner->Get_Identifier());
	m_pParameters	->Set_Callback();
}

CSG_Parameter_Parameters::~CSG_Parameter_Parameters(void)
{
	delete(m_pParameters);
}

//---------------------------------------------------------
const SG_Char * CSG_Parameter_Parameters::asString(void)
{
	m_String.Printf(SG_T("%d %s"), m_pParameters->Get_Count(), _TL("parameters"));

	return( m_String );
}

//---------------------------------------------------------
bool CSG_Parameter_Parameters::Restore_Default(void)
{
	return( m_pParameters->Restore_Defaults() );
}

//---------------------------------------------------------
void CSG_Parameter_Parameters::On_Assign(CSG_Parameter_Data *pSource)
{
	m_pParameters->Assign(((CSG_Parameter_Parameters *)pSource)->m_pParameters);
}

//---------------------------------------------------------
bool CSG_Parameter_Parameters::On_Serialize(CSG_MetaData &Entry, bool bSave)
{
	if( m_pParameters->Serialize(Entry, bSave) )
	{
		if( bSave )
		{
			Entry.Set_Property(SG_T("id")  , m_pOwner->Get_Identifier());
			Entry.Set_Property(SG_T("type"), m_pOwner->Get_Type_Identifier());
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


///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    User Interface                     //
//                                                       //
//                    Program: SAGA                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               Parameters_Properties.cpp               //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation; version 2 of the License.   //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
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
#include <saga_api/saga_api.h>

#include "res_dialogs.h"

#include "helper.h"
#include "dc_helper.h"

#include "wksp_data_manager.h"

#include "wksp_grid_manager.h"
#include "wksp_grid_system.h"
#include "wksp_grid.h"

#include "wksp_table_manager.h"
#include "wksp_table.h"

#include "wksp_shapes_manager.h"
#include "wksp_shapes_type.h"
#include "wksp_shapes.h"

#include "wksp_tin_manager.h"
#include "wksp_tin.h"

#include "parameters_properties.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GET_DATAOBJECT_LABEL(p)	(p->is_Option() ? p->Get_Name() : wxString::Format("%s %s", p->is_Input() ? (p->is_Optional() ? ">" : ">>") : (p->is_Optional() ? "<" : "<<"), p->Get_Name()).c_str())


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CParameters_PG_Choice::CParameters_PG_Choice(CParameter *pParameter)
	: wxEnumPropertyClass(GET_DATAOBJECT_LABEL(pParameter), pParameter->Get_Identifier(), NULL)
{
	m_pParameter	= pParameter;

	_Create();
}

//---------------------------------------------------------
CParameters_PG_Choice::~CParameters_PG_Choice(void)
{
	_Destroy();
}

//---------------------------------------------------------
void CParameters_PG_Choice::_Create(void)
{
	_Destroy();

	if( m_pParameter )
	{
		switch( m_pParameter->Get_Type() )
		{
		default:							m_index	= 0;					break;
		case PARAMETER_TYPE_Choice:			m_index	= _Set_Choice();		break;
		case PARAMETER_TYPE_Grid_System:	m_index	= _Set_Grid_System();	break;
		case PARAMETER_TYPE_Table_Field:	m_index	= _Set_Table_Field();	break;
		case PARAMETER_TYPE_Grid:			m_index	= _Set_Grid();			break;
		case PARAMETER_TYPE_Table:			m_index	= _Set_Table();			break;
		case PARAMETER_TYPE_Shapes:			m_index	= _Set_Shapes();		break;
		case PARAMETER_TYPE_TIN:			m_index	= _Set_TIN();			break;
		}
	}
}

//---------------------------------------------------------
void CParameters_PG_Choice::_Destroy(void)
{
	wxPGChoices	choices;

	m_choices.Assign(choices);

	m_index		= 0;
}

//---------------------------------------------------------
const wxChar * CParameters_PG_Choice::GetClassName(void) const
{
	return( "CParameters_PG_Choice" );
}

//---------------------------------------------------------
void CParameters_PG_Choice::_Append(const char *Label, long Value)
{
	m_choices.Add(Label, Value);
}

void CParameters_PG_Choice::_Append(const char *Label, void *Value)
{
	_Append(Label, (long)Value);
}

void CParameters_PG_Choice::_Append(const char *Label)
{
	m_choices.Add(Label);
}

//---------------------------------------------------------
bool CParameters_PG_Choice::Update(void)
{
	_Create();

	return( true );
}

//---------------------------------------------------------
int CParameters_PG_Choice::_Set_Choice(void)
{
	for(int i=0; i<m_pParameter->asChoice()->Get_Count(); i++)
	{
		_Append(m_pParameter->asChoice()->Get_Item(i), i);
	}

	return( m_pParameter->asInt() );
}

//---------------------------------------------------------
int CParameters_PG_Choice::_Set_Table_Field(void)
{
	CParameter	*pParent;
	CTable		*pTable;

	if(	(pParent = m_pParameter->Get_Parent()) != NULL)
	{
		switch( pParent->Get_Type() )
		{
	    default:					pTable	= NULL;					break;
		case PARAMETER_TYPE_Table:	pTable	= pParent->asTable();	break;
		case PARAMETER_TYPE_Shapes:	pTable	= pParent->asShapes() ? &pParent->asShapes()->Get_Table() : NULL;	break;
		case PARAMETER_TYPE_TIN:	pTable	= pParent->asTIN()    ? &pParent->asTIN()   ->Get_Table() : NULL;	break;
		}

		if( pTable )
		{
			for(int i=0; i<pTable->Get_Field_Count(); i++)
			{
				_Append(pTable->Get_Field_Name(i));
			}

			if( m_pParameter->is_Optional() || pTable->Get_Field_Count() == 0 )
			{
				_Append(LNG("[VAL] [not set]"));
			}

			return( m_pParameter->asInt() >= 0 ? m_pParameter->asInt() : m_choices.GetCount() - 1);
		}
	}

	_Append(LNG("[VAL] [not set]"));

	return( m_choices.GetCount() - 1 );
}

//---------------------------------------------------------
int CParameters_PG_Choice::_Set_Table(void)
{
	CWKSP_Table_Manager	*pTables;

	if( (pTables = g_pData->Get_Tables()) != NULL )
	{
		for(int i=0; i<pTables->Get_Count(); i++)
		{
			_Append(pTables->Get_Table(i)->Get_Name(), pTables->Get_Table(i)->Get_Table());
		}
	}

	return( _DataObject_Init() );
}

//---------------------------------------------------------
int CParameters_PG_Choice::_Set_Shapes(void)
{
	int						i, j, Shape_Type;
	CWKSP_Shapes_Manager	*pManager;
	CWKSP_Shapes_Type		*pShapes;

	if( (pManager = g_pData->Get_Shapes()) != NULL )
	{
		Shape_Type	= ((CParameter_Shapes *)m_pParameter->Get_Data())->Get_Shape_Type();

		for(i=0; i<pManager->Get_Count(); i++)
		{
			pShapes	= (CWKSP_Shapes_Type *)pManager->Get_Item(i);

			if( Shape_Type == SHAPE_TYPE_Undefined || Shape_Type == pShapes->Get_Shapes_Type() )
			{
				for(j=0; j<pShapes->Get_Count(); j++)
				{
					_Append(pShapes->Get_Shapes(j)->Get_Name(), pShapes->Get_Shapes(j)->Get_Shapes());
				}
			}
		}
	}

	return( _DataObject_Init() );
}

//---------------------------------------------------------
int CParameters_PG_Choice::_Set_TIN(void)
{
	CWKSP_TIN_Manager	*pTINs;

	if( (pTINs = g_pData->Get_TINs()) != NULL )
	{
		for(int i=0; i<pTINs->Get_Count(); i++)
		{
			_Append(pTINs->Get_TIN(i)->Get_Name(), pTINs->Get_TIN(i)->Get_TIN());
		}
	}

	return( _DataObject_Init() );
}

//---------------------------------------------------------
int CParameters_PG_Choice::_Set_Grid_System(void)
{
	int					i;
	CWKSP_Grid_Manager	*pSystems;

	if( (pSystems = g_pData->Get_Grids()) != NULL )
	{
		if( pSystems->Get_Count() > 0 )
		{
			for(i=0; i<pSystems->Get_Count(); i++)
			{
				_Append(pSystems->Get_System(i)->Get_Name(), pSystems->Get_System(i)->Get_System());
			}
		}

		_Append(LNG("[VAL] [not set]"), (void *)NULL);

		g_pData->Check_Parameter(m_pParameter);

		for(i=0; i<(int)m_choices.GetCount()-1; i++)
		{
			if( m_pParameter->asGrid_System()->is_Equal(*((CGrid_System *)m_choices.GetValue(i))) )
			{
				return( i );
			}
		}

		return( m_choices.GetCount() - 1 );
	}

	_Append(LNG("[VAL] [no choice available]"), (void *)NULL);

	return( m_choices.GetCount() - 1 );
}

//---------------------------------------------------------
int CParameters_PG_Choice::_Set_Grid(void)
{
	int					i;
	CParameter			*pParent;
	CWKSP_Grid_Manager	*pManager;
	CWKSP_Grid_System	*pSystem;

	pSystem		= NULL;
	pManager	= g_pData->Get_Grids();
	pParent		= m_pParameter->Get_Parent();

	if( pManager && pParent && pParent->Get_Type() == PARAMETER_TYPE_Grid_System && pParent->asGrid_System()->is_Valid() )
	{
		for(i=0; i<pManager->Get_Count() && !pSystem; i++)
		{
			if( pParent->asGrid_System()->is_Equal(*pManager->Get_System(i)->Get_System()) )
			{
				pSystem	= pManager->Get_System(i);
			}
		}
	}

	if( pSystem )
	{
		for(i=0; i<pSystem->Get_Count(); i++)
		{
			_Append(pSystem->Get_Grid(i)->Get_Name(), pSystem->Get_Grid(i)->Get_Grid());
		}
	}

	return( _DataObject_Init() );
}

//---------------------------------------------------------
int CParameters_PG_Choice::_DataObject_Init(void)
{
	if( m_pParameter->is_Output() )
	{
		_Append(LNG("[VAL] [create]"), DATAOBJECT_CREATE);
	}

	if( !m_pParameter->is_Output() || (m_pParameter->is_Output() && m_pParameter->is_Optional()) )
	{
		_Append(LNG("[VAL] [not set]"), DATAOBJECT_NOTSET);
	}

	g_pData->Check_Parameter(m_pParameter);

	for(int i=0; i<(int)m_choices.GetCount(); i++)
	{
		if( m_pParameter->asDataObject() == (void *)m_choices.GetValue(i) )
		{
			return( i );
		}
	}

	return( m_choices.GetCount() - 1 );
}

//---------------------------------------------------------
bool CParameters_PG_Choice::OnEvent(wxPropertyGrid *pPG, wxWindow *pPGCtrl, wxEvent &event)
{
	if( event.m_eventType == wxEVT_COMMAND_COMBOBOX_SELECTED )
	{
		if( m_pParameter && m_choices.IsOk() && m_index >= 0 && m_index < (int)m_choices.GetCount() )
		{
			switch( m_pParameter->Get_Type() )
			{
			default:
				break;

			case PARAMETER_TYPE_Choice:
			case PARAMETER_TYPE_Table_Field:
				m_pParameter->Set_Value(m_index);
				break;

			case PARAMETER_TYPE_Grid:
				m_pParameter->Set_Value((void *)m_choices.GetValue(m_index));
				break;

			case PARAMETER_TYPE_Grid_System:
				m_pParameter->Set_Value((void *)m_choices.GetValue(m_index));
				_Update_Grids(pPG);
				break;

			case PARAMETER_TYPE_Table:
			case PARAMETER_TYPE_Shapes:
			case PARAMETER_TYPE_TIN:
				m_pParameter->Set_Value((void *)m_choices.GetValue(m_index));
				_Update_TableFields(pPG);
				break;
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
void CParameters_PG_Choice::_Update_Grids(wxPropertyGrid *pPG)
{
	int			i;
	CParameter	*pChild;
	wxPGId		Id;

	if( m_pParameter && m_pParameter->Get_Type() == PARAMETER_TYPE_Grid_System )
	{
		for(i=0; i<m_pParameter->Get_Children_Count(); i++)
		{
			pChild	= m_pParameter->Get_Child(i);

			switch( pChild->Get_Type() )
			{
			default:
				break;
				
			case PARAMETER_TYPE_Grid_List:
				break;

			case PARAMETER_TYPE_Grid:
				Id	= pPG->GetPropertyByName(pChild->Get_Identifier());

				if( Id.IsOk() )
				{
					((CParameters_PG_Choice *)Id.GetPropertyPtr())->Update();
				}
			}
		}
	}
}

//---------------------------------------------------------
void CParameters_PG_Choice::_Update_TableFields(wxPropertyGrid *pPG)
{
	int			i;
	CParameter	*pChild;
	wxPGId		Id;

	if( m_pParameter )
	{
		for(i=0; i<m_pParameter->Get_Children_Count(); i++)
		{
			pChild	= m_pParameter->Get_Child(i);

			if(	pChild->Get_Type() == PARAMETER_TYPE_Table_Field )
			{
				Id	= pPG->GetPropertyByName(pChild->Get_Identifier());

				if( Id.IsOk() )
				{
					((CParameters_PG_Choice *)Id.GetPropertyPtr())->Update();
				}
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GRIDSYSTEM_GET_COUNT		(int)(m_choices.IsOk() ? m_choices.GetCount() : 0)
#define GRIDSYSTEM_GET_SYSTEM(i)	(i >= 0 && i < GRIDSYSTEM_GET_COUNT ? (CGrid_System *)m_choices.GetValue(i) : NULL)

//---------------------------------------------------------
CParameters_PG_GridSystem::CParameters_PG_GridSystem(CParameter *pParameter)
	: wxCustomPropertyClass(pParameter->Get_Name(), pParameter->Get_Identifier())
{
	m_pParameter	= pParameter;

	SetEditor(wxPGEditor_Choice);

	_Create();
}

//---------------------------------------------------------
CParameters_PG_GridSystem::~CParameters_PG_GridSystem(void)
{
	_Destroy();
}

//---------------------------------------------------------
void CParameters_PG_GridSystem::_Create(void)
{
	CWKSP_Grid_Manager	*pSystems;

	_Destroy();

	m_index	= -1;

	if( m_pParameter && m_pParameter->Get_Type() == PARAMETER_TYPE_Grid_System )
	{
		g_pData->Check_Parameter(m_pParameter);

		if( (pSystems = g_pData->Get_Grids()) != NULL )
		{
			if( pSystems->Get_Count() > 0 )
			{
				for(int i=0; i<pSystems->Get_Count(); i++)
				{
					m_choices.Add(pSystems->Get_System(i)->Get_Name().c_str(), (long)pSystems->Get_System(i)->Get_System());

					if( pSystems->Get_System(i)->Get_System()->is_Equal(*m_pParameter->asGrid_System()) )
					{
						m_index	= i;
					}
				}
			}

			m_choices.Add(LNG("[VAL] [not set]"), 0);
		}
	}

	if( GRIDSYSTEM_GET_COUNT == 0 )
	{
		m_choices.Add(LNG("[VAL] [no choice available]"), 0);
	}

	if( m_index < 0 )
	{
		m_index	= GRIDSYSTEM_GET_COUNT - 1;
	}

	SetValueFromInt(m_index);
}

//---------------------------------------------------------
void CParameters_PG_GridSystem::_Destroy(void)
{
	wxPGChoices	choices;

	m_choices.Assign(choices);

	m_index		= 0;
}

//---------------------------------------------------------
bool CParameters_PG_GridSystem::Update(void)
{
	_Create();

	return( true );
}

//---------------------------------------------------------
wxString CParameters_PG_GridSystem::GetValueAsString(int arg_flags) const
{
	if( m_choices.IsOk() && m_index >= 0 && m_index < (int)m_choices.GetCount() )
	{
		return( m_choices.GetLabel(m_index) );
	}

	return( LNG("<none>") );
}

//---------------------------------------------------------
bool CParameters_PG_GridSystem::SetValueFromInt(long value, int arg_flags)
{
	//-----------------------------------------------------
	if( arg_flags & wxPG_FULL_VALUE )
	{
		return( SetValueFromInt(GRIDSYSTEM_GET_COUNT > 0 ? m_choices.GetValues().Index(value) : 0) );
	}

	//-----------------------------------------------------
	bool	bChanged;

	if( GRIDSYSTEM_GET_COUNT > 0 )
	{
		bChanged	= m_index != value;
		m_index		= value;
		value		= (long)GRIDSYSTEM_GET_SYSTEM(m_index);

		wxCustomPropertyClass::SetValueFromInt(value, arg_flags);

		//-------------------------------------------------
		m_pParameter->Set_Value((void *)value);

		if( GetParent() && GetGrid() )
		{
			wxPGId	Id	= GetGrid()->GetFirstChild(GetId());

			while( Id.IsOk() )
			{
				if( strcmp("CParameters_PG_Choice", Id.GetPropertyPtr()->GetClassName()) == 0 )
				{
					((CParameters_PG_Choice *)Id.GetPropertyPtr())->Update();
				}

				Id	= GetGrid()->GetNextProperty(Id);
			}
		}
	}
	else
	{
		bChanged	= false;
	}

	return( bChanged );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CParameters_PG_DoublesValue::CParameters_PG_DoublesValue(void)
{
	m_nValues		= 0;
	m_Values		= NULL;
	m_Labels		= NULL;
	m_pParameter	= NULL;
}

//---------------------------------------------------------
CParameters_PG_DoublesValue::CParameters_PG_DoublesValue(class CParameter *pParameter)
{
	m_nValues		= 0;
	m_Values		= NULL;
	m_Labels		= NULL;
	m_pParameter	= NULL;

	_Create( pParameter );
}

//---------------------------------------------------------
CParameters_PG_DoublesValue::~CParameters_PG_DoublesValue(void)
{
	_Destroy();
}

//---------------------------------------------------------
bool CParameters_PG_DoublesValue::_Create(CParameter *pParameter)
{
	if( pParameter )
	{
		_Destroy();

		switch( pParameter->Get_Type() )
		{
		default:
			return( false );

		case PARAMETER_TYPE_Range:
			m_nValues	= 2;
			m_Values	= (double *)SG_Malloc(m_nValues * sizeof(double));
			m_Labels	= new wxString[m_nValues];

			m_Labels[0]	= LNG("Minimum");
			m_Labels[1]	= LNG("Maximum");
			break;

		case PARAMETER_TYPE_Degree:
			m_nValues	= 3;
			m_Values	= (double *)SG_Malloc(m_nValues * sizeof(double));
			m_Labels	= new wxString[m_nValues];

			m_Labels[0]	= LNG("°");
			m_Labels[1]	= LNG("'");
			m_Labels[2]	= LNG("''");
			break;
		}

		m_pParameter	= pParameter;

		Update_Values();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CParameters_PG_DoublesValue::_Destroy(void)
{
	if( m_nValues > 0 )
	{
		SG_Free(m_Values);
		delete[](m_Labels);

		m_nValues		= 0;
		m_Values		= NULL;
		m_Labels		= NULL;
	}

	m_pParameter	= NULL;
}

//---------------------------------------------------------
bool CParameters_PG_DoublesValue::Assign(const CParameters_PG_DoublesValue &Value)
{
	return( _Create(Value.m_pParameter) );
}

//---------------------------------------------------------
bool CParameters_PG_DoublesValue::Update_Parameter(void)
{
	if( m_pParameter )
	{
		switch( m_pParameter->Get_Type() )
		{
		default:
			break;

		case PARAMETER_TYPE_Range:
			m_pParameter->asRange()->Set_LoVal(m_Values[0]);
			m_pParameter->asRange()->Set_HiVal(m_Values[1]);
			break;

		case PARAMETER_TYPE_Degree:
			m_pParameter->Set_Value(Degree_To_Decimal(m_Values[0], m_Values[1], m_Values[2]));
			break;
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CParameters_PG_DoublesValue::Update_Values(void)
{
	if( m_pParameter )
	{
		switch( m_pParameter->Get_Type() )
		{
		default:
			break;

		case PARAMETER_TYPE_Range:
			m_Values[0]	= m_pParameter->asRange()->Get_LoVal();
			m_Values[1]	= m_pParameter->asRange()->Get_HiVal();
			return( true );

		case PARAMETER_TYPE_Degree:
			Decimal_To_Degree(m_pParameter->asDouble(), m_Values[0], m_Values[1], m_Values[2]);
			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
WX_PG_IMPLEMENT_VALUE_TYPE_VOIDP(CParameters_PG_DoublesValue, CParameters_PG_Doubles, CParameters_PG_DoublesValue(NULL))

//---------------------------------------------------------
class CParameters_PG_DoublesClass : public wxPGPropertyWithChildren
{
    WX_PG_DECLARE_PROPERTY_CLASS()

public:
	CParameters_PG_DoublesClass(const wxString &label, const wxString &name, const CParameters_PG_DoublesValue &value);
	virtual ~CParameters_PG_DoublesClass(void)	{}

	WX_PG_DECLARE_PARENTAL_TYPE_METHODS()
	WX_PG_DECLARE_PARENTAL_METHODS()

	CParameters_PG_DoublesValue		m_value;

};

//---------------------------------------------------------
WX_PG_IMPLEMENT_PROPERTY_CLASS(CParameters_PG_Doubles, wxBaseParentProperty, CParameters_PG_DoublesValue, const CParameters_PG_DoublesValue &, TextCtrl)

//---------------------------------------------------------
CParameters_PG_DoublesClass::CParameters_PG_DoublesClass(const wxString &Name, const wxString &Identifier, const CParameters_PG_DoublesValue &value)
	: wxPGPropertyWithChildren(Name, Identifier)
{
	DoSetValue((void *)&value);

	for(int i=0; i<m_value.m_nValues; i++)
	{
		AddChild(wxFloatProperty(m_value.m_Labels[i], wxString::Format("%s_%d", Identifier.c_str(), i), m_value.m_Values[i]));
	}
}

//---------------------------------------------------------
void CParameters_PG_DoublesClass::DoSetValue(wxPGVariant value)
{
	m_value.Assign(*((CParameters_PG_DoublesValue *)wxPGVariantToVoidPtr(value)));

	RefreshChildren();
}

//---------------------------------------------------------
wxPGVariant CParameters_PG_DoublesClass::DoGetValue(void) const
{
	return( wxPGVariant((void *)&m_value) );
}

//---------------------------------------------------------
void CParameters_PG_DoublesClass::RefreshChildren(void)
{
	if( (int)GetCount() == m_value.m_nValues )
	{
		for(int i=0; i<m_value.m_nValues; i++)
		{
			Item(i)->DoSetValue(m_value.m_Values[i]);
		}
	}
}

//---------------------------------------------------------
void CParameters_PG_DoublesClass::ChildChanged(wxPGProperty *p)
{
	int		i	= p->GetIndexInParent();

	if( i >= 0 && i < m_value.m_nValues )
	{
		m_value.m_Values[i]	= p->DoGetValue().GetDouble();
		m_value.Update_Parameter();
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CParameters_PG_DialogedValue::fromString(wxString String)
{
	switch( m_pParameter->Get_Type() )
	{
	default:
		return( false );

	case PARAMETER_TYPE_Text:
	case PARAMETER_TYPE_FilePath:
		m_pParameter->Set_Value(String.c_str());
		return( true );
	}
}

//---------------------------------------------------------
wxString CParameters_PG_DialogedValue::asString(void) const
{
	wxString	s;

	if( m_pParameter )
	{
		s	= m_pParameter->asString();
	}

	return( s );
}

//---------------------------------------------------------
bool CParameters_PG_DialogedValue::Do_Dialog(void)
{
	bool		bModified	= false;
	long		Color;
	wxString	Text;

	if( m_pParameter )
	{
		switch( m_pParameter->Get_Type() )
		{
		default:
			break;

		case PARAMETER_TYPE_Font:
			bModified	= DLG_Font			(m_pParameter->asFont(), Color = m_pParameter->asColor());

			if( bModified )
			{
				m_pParameter->Set_Value((int)Color);
			}
			break;

		case PARAMETER_TYPE_Text:
			bModified	= DLG_Text			(m_pParameter->Get_Name(), Text = m_pParameter->asString());

			if( bModified )
			{
				m_pParameter->Set_Value(Text.c_str());
			}
			break;

		case PARAMETER_TYPE_FilePath:
			Text	= m_pParameter->asString();

			if( m_pParameter->asFilePath()->is_Directory() )
			{
				bModified	= DLG_Directory	(Text, LNG("[CAP] Choose Directory"));
			}
			else if( m_pParameter->asFilePath()->is_Save() )
			{
				bModified	= DLG_Save		(Text, LNG("[CAP] Save"), m_pParameter->asFilePath()->Get_Filter());
			}
			else if( m_pParameter->asFilePath()->is_Multiple() == false )
			{
				bModified	= DLG_Open		(Text, LNG("[CAP] Open"), m_pParameter->asFilePath()->Get_Filter());
			}
			else
			{
				wxArrayString	Files;

				bModified	= DLG_Open		(Files, LNG("[CAP] Open"), m_pParameter->asFilePath()->Get_Filter());

				if( bModified )
				{
					if( Files.GetCount() > 0 )
					{
						Text.Clear();

						for(size_t i=0; i<Files.GetCount(); i++)
						{
							Text.Append(i > 0 ? " \"" : "\"");
							Text.Append(Files.Item(i));
							Text.Append("\"");
						}
					}
					else
					{
						Text	= Files.Item(0);
					}
				}
			}

			if( bModified )
			{
				m_pParameter->Set_Value(Text.c_str());
			}
			break;

		case PARAMETER_TYPE_FixedTable:
			bModified	= DLG_Table			(m_pParameter->Get_Name(), m_pParameter->asTable());
			break;

		case PARAMETER_TYPE_Grid_List:
			bModified	= DLG_List_Grid		(m_pParameter->Get_Name(), (CParameter_Grid_List   *)m_pParameter->Get_Data());
			break;

		case PARAMETER_TYPE_Table_List:
			bModified	= DLG_List_Table	(m_pParameter->Get_Name(), (CParameter_Table_List  *)m_pParameter->Get_Data());
			break;

		case PARAMETER_TYPE_Shapes_List:
			bModified	= DLG_List_Shapes	(m_pParameter->Get_Name(), (CParameter_Shapes_List *)m_pParameter->Get_Data());
			break;

		case PARAMETER_TYPE_TIN_List:
			bModified	= DLG_List_TIN		(m_pParameter->Get_Name(), (CParameter_TIN_List    *)m_pParameter->Get_Data());
			break;

		case PARAMETER_TYPE_Colors:
			bModified	= DLG_Colors		(m_pParameter->asColors());
			break;

		case PARAMETER_TYPE_Parameters:
			bModified	= DLG_Parameters	(m_pParameter->asParameters());
			break;
		}
	}

	return( bModified );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
WX_PG_IMPLEMENT_VALUE_TYPE_VOIDP(CParameters_PG_DialogedValue, CParameters_PG_Dialoged, CParameters_PG_DialogedValue(NULL))

//---------------------------------------------------------
class CParameters_PG_DialogedClass : public wxPGProperty
{
    WX_PG_DECLARE_PROPERTY_CLASS()

public:
	CParameters_PG_DialogedClass(const wxString &label, const wxString &name, const CParameters_PG_DialogedValue &value);
	virtual ~CParameters_PG_DialogedClass(void)	{}

	WX_PG_DECLARE_BASIC_TYPE_METHODS()
	WX_PG_DECLARE_EVENT_METHODS()

	CParameters_PG_DialogedValue		m_value;

};

//---------------------------------------------------------
WX_PG_IMPLEMENT_PROPERTY_CLASS(CParameters_PG_Dialoged, wxBaseProperty, CParameters_PG_DialogedValue, const CParameters_PG_DialogedValue &, TextCtrlAndButton)

//---------------------------------------------------------
CParameters_PG_DialogedClass::CParameters_PG_DialogedClass(const wxString &Name, const wxString &Identifier, const CParameters_PG_DialogedValue &value)
	: wxPGProperty(value.m_pParameter ? GET_DATAOBJECT_LABEL(value.m_pParameter) : Name.c_str(), Identifier)
{
	m_value	= value;

	DoSetValue((void *)&m_value);
}

//---------------------------------------------------------
void CParameters_PG_DialogedClass::DoSetValue(wxPGVariant value)
{
	m_value	= *((CParameters_PG_DialogedValue *)wxPGVariantToVoidPtr(value));
}

//---------------------------------------------------------
wxPGVariant CParameters_PG_DialogedClass::DoGetValue(void) const
{
	return( wxPGVariant((void *)&m_value) );
}

//---------------------------------------------------------
bool CParameters_PG_DialogedClass::SetValueFromString(const wxString &text, int flags)
{
	return( m_value.fromString(text) );
}

//---------------------------------------------------------
wxString CParameters_PG_DialogedClass::GetValueAsString(int flags) const
{
	return( m_value.asString() );
}

//---------------------------------------------------------
bool CParameters_PG_DialogedClass::OnEvent(wxPropertyGrid *propgrid, wxWindow *primary, wxEvent &event)
{
	if( event.m_eventType == wxEVT_COMMAND_BUTTON_CLICKED )
	{
		if( m_value.Do_Dialog() )
		{
			UpdateControl(primary);

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CParameters_PG_ColorsClass : public CParameters_PG_DialogedClass
{
    WX_PG_DECLARE_PROPERTY_CLASS()

public:
	CParameters_PG_ColorsClass(const wxString &label, const wxString &name, const CParameters_PG_DialogedValue &value);

	WX_PG_DECLARE_CUSTOM_PAINT_METHODS()

};


//---------------------------------------------------------
WX_PG_IMPLEMENT_PROPERTY_CLASS(CParameters_PG_Colors, wxBaseProperty, CParameters_PG_DialogedValue, const CParameters_PG_DialogedValue &, TextCtrlAndButton)

//---------------------------------------------------------
CParameters_PG_ColorsClass::CParameters_PG_ColorsClass(const wxString &Name, const wxString &Identifier, const CParameters_PG_DialogedValue &value)
	: CParameters_PG_DialogedClass(Name, Identifier, value)
{
}

//---------------------------------------------------------
void CParameters_PG_ColorsClass::OnCustomPaint(wxDC &dc, const wxRect &r, wxPGPaintData &pd)
{
	int		i, ax, bx;
	double	dx;
	CSG_Colors	*pColors;

	if( m_value.m_pParameter && m_value.m_pParameter->Get_Type() == PARAMETER_TYPE_Colors )
	{
		pColors	= m_value.m_pParameter->asColors();
		dx		= r.GetWidth() / (double)pColors->Get_Count();

		for(i=0, bx=r.GetLeft(); i<pColors->Get_Count(); i++)
		{
			ax	= bx;
			bx	= r.GetLeft() + (int)(dx * (i + 1.0));

			Draw_FillRect(dc, Get_Color_asWX(pColors->Get_Color(i)), ax, r.GetTop(), bx, r.GetBottom());
		}
	}
}

//---------------------------------------------------------
wxSize CParameters_PG_ColorsClass::GetImageSize() const
{
	return( wxPG_FLEXIBLE_SIZE(164, 64) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


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
//                Parameters_Control.cpp                 //
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
#include "res_controls.h"

#include "helper.h"

#include "wksp_data_manager.h"

#include "parameters_control.h"
#include "parameters_properties.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CParameters_Control, wxPanel)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CParameters_Control, wxPanel)
	EVT_SIZE			(CParameters_Control::On_Size)

	EVT_PG_CHANGED		(ID_WND_PARM_PG_ACTIVE, CParameters_Control::On_PG_Changed)
	EVT_PG_CHANGED		(ID_WND_PARM_PG_DIALOG, CParameters_Control::On_PG_Changed)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CParameters_Control::CParameters_Control(wxWindow *pParent, bool bDialog)
	: wxPanel(pParent, -1, wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxCLIP_CHILDREN)
{
	m_pPG	= new wxPropertyGrid(this, bDialog ? ID_WND_PARM_PG_DIALOG : ID_WND_PARM_PG_ACTIVE, wxDefaultPosition, wxDefaultSize,
				wxPG_BOLD_MODIFIED|
				wxPG_SPLITTER_AUTO_CENTER|
				//wxPG_AUTO_SORT|
				//wxPG_HIDE_MARGIN|
				//wxPG_STATIC_SPLITTER|
				//wxPG_HIDE_CATEGORIES|
				//wxPG_LIMITED_EDITING|
				wxTAB_TRAVERSAL|
				//wxPG_TOOLBAR|
				wxPG_DESCRIPTION|
				//wxPG_COMPACTOR|
				wxPGMAN_DEFAULT_STYLE
			);

	m_pParameters	= new CSG_Parameters();
	m_pOriginal		= NULL;

	Set_Parameters(NULL);
}

//---------------------------------------------------------
CParameters_Control::~CParameters_Control(void)
{
	delete(m_pParameters);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CParameters_Control::On_Size(wxSizeEvent &event)
{
	m_pPG->SetSize(wxRect(wxPoint(0, 0), GetSize()));

	m_pPG->CenterSplitter(true);
}

//---------------------------------------------------------
void CParameters_Control::On_PG_Changed(wxPropertyGridEvent &event)
{
	_Set_Parameter(event.GetPropertyName());
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CParameters_Control::Save_Changes(bool bSilent)
{
	if( m_pOriginal && m_bModified && (bSilent || DLG_Message_Confirm(LNG("[DLG] Save changes?"), wxString::Format("%s: %s", LNG("[CAP] Parameters"), m_pParameters->Get_Name()))) )
	{
		m_pOriginal->Assign_Values(m_pParameters);

		m_bModified	= false;

		m_pPG->ClearModifiedStatus();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CParameters_Control::Restore(void)
{
	if( m_pOriginal && m_bModified )
	{
		Set_Parameters(m_pOriginal);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CParameters_Control::Load(void)
{
	bool		bResult	= false;
	FILE		*Stream;
	wxString	File_Path;

	if( DLG_Open(File_Path, ID_DLG_PARAMETERS_OPEN) && (Stream = fopen(File_Path, "rb")) != NULL )
	{
		if( m_pParameters->Serialize(Stream, false) )
		{
		//	m_pPG->Freeze();
			m_pPG->Clear();

			_Add_Properties(m_pParameters);

		//	m_pPG->Thaw();
			m_pPG->Refresh();

			m_bModified	= true;
			bResult		= true;
		}

		fclose(Stream);
	}

	return( bResult );
}

//---------------------------------------------------------
bool CParameters_Control::Save(void)
{
	bool		bResult	= false;
	FILE		*Stream;
	wxString	File_Path;

	if( DLG_Save(File_Path, ID_DLG_PARAMETERS_SAVE) && (Stream = fopen(File_Path, "wb")) != NULL )
	{
		if( m_pParameters->Serialize(Stream, true) )
		{
			bResult		= true;
		}

		fclose(Stream);
	}

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CParameters_Control::Set_Parameters(CSG_Parameters *pParameters)
{
//	m_pPG->Freeze();
	m_pPG->Clear();

	m_bModified	= false;

	//-----------------------------------------------------
	if( (m_pOriginal = pParameters) != NULL )
	{
		m_pParameters->Assign(m_pOriginal);
		m_pParameters->Set_Callback(true);

		_Add_Properties(m_pParameters);
	}
	else
	{
		m_pPG->Append(wxPropertyCategory(LNG("[TXT] No parameters available."), wxPG_LABEL));
	}

	//-----------------------------------------------------
//	m_pPG->Thaw();
	m_pPG->Refresh();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define CHECK_LIST_OUTPUT(p)	if( p->is_Output() )	{	pRoot	= NULL;	break;	}

//---------------------------------------------------------
#define CHECK_DATA_NODE(pNode, Name, ID)	if( !pNode )\
	{\
		pNode	= wxPropertyCategory(LNG(Name), ID);\
		if( !pData )\
			m_pPG->Append(pData = wxPropertyCategory(LNG("[PRM] Data Objects"), "_DATAOBJECT_DATAOBJECTS"));\
		_Add_Property(pNode, pData);\
	}\
	pRoot	= pNode;

//---------------------------------------------------------
void CParameters_Control::_Add_Properties(CSG_Parameters *pParameters)
{
	wxPGProperty	*pGrids, *pShapes, *pTables, *pTINs, *pOptions, *pData, *pRoot;

	pData		= NULL;
	pGrids		= NULL;
	pShapes		= NULL;
	pTables		= NULL;
	pTINs		= NULL;
	pOptions	= NULL;

	for(int i=0; i<pParameters->Get_Count(); i++)
	{
		if(	pParameters->Get_Parameter(i)->Get_Parent() == NULL )
		{
			switch( pParameters->Get_Parameter(i)->Get_Type() )
			{
			case PARAMETER_TYPE_DataObject_Output:
				pRoot	= NULL;
				break;

			case PARAMETER_TYPE_Grid_System:
				if(1|| pParameters->Get_Parameter(i)->Get_Children_Count() > 0 )
				{
					CHECK_DATA_NODE(pGrids	, "[PRM] Grids" , "_DATAOBJECT_GRIDS");
				}
				else
				{
					pRoot	= NULL;
				}
				break;

			case PARAMETER_TYPE_Grid_List:
				CHECK_LIST_OUTPUT(pParameters->Get_Parameter(i));
			case PARAMETER_TYPE_Grid:
				CHECK_DATA_NODE(pGrids	, "[PRM] Grids" , "_DATAOBJECT_GRIDS");
				break;

			case PARAMETER_TYPE_Table_List:
				CHECK_LIST_OUTPUT(pParameters->Get_Parameter(i));
			case PARAMETER_TYPE_Table:
				CHECK_DATA_NODE(pTables	, "[PRM] Tables", "_DATAOBJECT_TABLES");
				break;

			case PARAMETER_TYPE_Shapes_List:
				CHECK_LIST_OUTPUT(pParameters->Get_Parameter(i));
			case PARAMETER_TYPE_Shapes:
				CHECK_DATA_NODE(pShapes	, "[PRM] Shapes", "_DATAOBJECT_SHAPES");
				break;

			case PARAMETER_TYPE_TIN_List:
				CHECK_LIST_OUTPUT(pParameters->Get_Parameter(i));
			case PARAMETER_TYPE_TIN:
				CHECK_DATA_NODE(pTINs	, "[PRM] TIN", "_DATAOBJECT_TINS");
				break;

			default:
				if( !pOptions )
				{
					pOptions	= wxPropertyCategory(LNG("[PRM] Options"), "_DATAOBJECT_OPTIONS");
					m_pPG->Append(pOptions);
				}

				pRoot	= pOptions;
				break;
			}

			if( pRoot )
			{
				_Add_Property(pParameters->Get_Parameter(i), pRoot);
			}
		}
	}
}

//---------------------------------------------------------
wxPGProperty * CParameters_Control::_Add_Property(wxPGProperty *pProperty, wxPGProperty *pParent)
{
	wxPGId	Id;

	if( pParent )
	{
		Id	= m_pPG->Insert(pParent->GetId(), -1, pProperty);
	}
	else
	{
		Id	= m_pPG->Append(pProperty);
	}

	return( Id.GetPropertyPtr() );
}

//---------------------------------------------------------
void CParameters_Control::_Add_Property(CSG_Parameter *pParameter, wxPGProperty *pParent)
{
	int				i;
	wxPGProperty	*pProperty;

	if( pParameter->Get_Children_Count() > 0 )
	{
		switch( pParameter->Get_Type() )
		{
		case PARAMETER_TYPE_Node:
		case PARAMETER_TYPE_Grid_System:
			pProperty	= _Get_Property(pParameter, pParent);
			break;

		case PARAMETER_TYPE_Grid:
		case PARAMETER_TYPE_Grid_List:
			_Get_Property(pParameter, pParent);
			pProperty	= wxPropertyCategory(wxString::Format("%s [%s]", pParameter->Get_Name(), LNG("[CAP] Options")), wxString::Format("%s_PARENT", pParameter->Get_Identifier()));
			if( pParameter->Get_Parent() && pParameter->Get_Parent()->Get_Type() == PARAMETER_TYPE_Grid_System )
			{
				wxPGId	Id	= m_pPG->GetPropertyByName("_DATAOBJECT_GRIDS");
				_Add_Property(pProperty, Id.GetPropertyPtr());
			}
			else
			{
				_Add_Property(pProperty, pParent);
			}
			break;

		default:
			_Get_Property(pParameter, pParent);
			pProperty	= wxPropertyCategory(wxString::Format("%s [%s]", pParameter->Get_Name(), LNG("[CAP] Options")), wxString::Format("%s_PARENT", pParameter->Get_Identifier()));
			_Add_Property(pProperty, pParent);
			break;
		}

		for(i=0; i<pParameter->Get_Children_Count(); i++)
		{
			if( pParameter->Get_Child(i)->Get_Children_Count() == 0 )
			{
				_Add_Property(pParameter->Get_Child(i), pProperty);
			}
		}

		for(i=0; i<pParameter->Get_Children_Count(); i++)
		{
			if( pParameter->Get_Child(i)->Get_Children_Count() > 0 )
			{
				_Add_Property(pParameter->Get_Child(i), pProperty);
			}
		}

		m_pPG->Expand(pProperty->GetId());
	}
	else if( pParameter->Get_Type() != PARAMETER_TYPE_Node )
	{
		_Get_Property(pParameter, pParent);
	}
}

//---------------------------------------------------------
wxPGProperty * CParameters_Control::_Get_Property(CSG_Parameter *pParameter, wxPGProperty *pParent)
{
	wxPGProperty	*pProperty	= NULL;

	switch( pParameter->Get_Type() )
	{
	default:
		pProperty	= _Add_Property(wxStringProperty(
			pParameter->Get_Name(), pParameter->Get_Identifier(), pParameter->Get_Type_Name()
		), pParent);
		break;

	case PARAMETER_TYPE_Node:
		pProperty	= _Add_Property(wxPropertyCategory(
			pParameter->Get_Name(), pParameter->Get_Identifier()
		), pParent);
		break;

	case PARAMETER_TYPE_Bool:
		pProperty	= _Add_Property(wxBoolProperty(
			pParameter->Get_Name(), pParameter->Get_Identifier(), pParameter->asBool()
		), pParent);

	    m_pPG->SetPropertyAttribute(pProperty->GetId(), wxPG_BOOL_USE_CHECKBOX, (long)1, wxPG_RECURSE);
		break;

	case PARAMETER_TYPE_Int:
		pProperty	= _Add_Property(wxIntProperty(
			pParameter->Get_Name(), pParameter->Get_Identifier(), pParameter->asInt()
		), pParent);
		break;

	case PARAMETER_TYPE_Double:
		pProperty	= _Add_Property(wxFloatProperty(
			pParameter->Get_Name(), pParameter->Get_Identifier(), pParameter->asDouble()
		), pParent);

		m_pPG->SetPropertyAttribute(pProperty->GetId(), wxPG_FLOAT_PRECISION, (long)12);
		break;

	case PARAMETER_TYPE_Range:
	case PARAMETER_TYPE_Degree:
		pProperty	= _Add_Property(CParameters_PG_Doubles(
			pParameter->Get_Name(), pParameter->Get_Identifier(), CParameters_PG_DoublesValue(pParameter)
		), pParent);
		break;

	case PARAMETER_TYPE_String:
		pProperty	= _Add_Property(wxStringProperty(
			pParameter->Get_Name(), pParameter->Get_Identifier(), pParameter->asString()
		), pParent);
		break;

	case PARAMETER_TYPE_Color:
		pProperty	= _Add_Property(wxColourProperty(
			pParameter->Get_Name(), pParameter->Get_Identifier(), Get_Color_asWX(pParameter->asColor())
		), pParent);

		pProperty->SetEditor(wxPGEditor_Choice);
		break;

	case PARAMETER_TYPE_Colors:
		pProperty	= _Add_Property(CParameters_PG_Colors(
			pParameter->Get_Name(), pParameter->Get_Identifier(), CParameters_PG_DialogedValue(pParameter)
		), pParent);
		break;

	case PARAMETER_TYPE_Grid_System:
		pProperty	= _Add_Property(new CParameters_PG_GridSystem(
			pParameter
		), pParent);
		break;

	case PARAMETER_TYPE_Choice:
	case PARAMETER_TYPE_Table_Field:
	case PARAMETER_TYPE_Grid:
	case PARAMETER_TYPE_Table:
	case PARAMETER_TYPE_Shapes:
	case PARAMETER_TYPE_TIN:
		pProperty	= _Add_Property(new CParameters_PG_Choice(
			pParameter
		), pParent);
		break;

	case PARAMETER_TYPE_Text:	// pProperty	= _Add_Property(wxLongStringProperty(pParameter->Get_Name(), pParameter->Get_Identifier(), pParameter->asString()), pParent);	break;
	case PARAMETER_TYPE_FilePath:
	case PARAMETER_TYPE_Font:
	case PARAMETER_TYPE_FixedTable:
	case PARAMETER_TYPE_Grid_List:
	case PARAMETER_TYPE_Table_List:
	case PARAMETER_TYPE_Shapes_List:
	case PARAMETER_TYPE_TIN_List:
	case PARAMETER_TYPE_Parameters:
		pProperty	= _Add_Property(CParameters_PG_Dialoged(
			pParameter->Get_Name(), pParameter->Get_Identifier(), CParameters_PG_DialogedValue(pParameter)
		), pParent);
		break;
	}

	//-----------------------------------------------------
	if( pProperty )
	{
		m_pPG->SetPropertyHelpString(pProperty->GetId(), pParameter->Get_Description(PARAMETER_DESCRIPTION_TEXT|PARAMETER_DESCRIPTION_PROPERTIES|PARAMETER_DESCRIPTION_OPTIONAL|PARAMETER_DESCRIPTION_TYPE).c_str());
	}

	return( pProperty );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CParameters_Control::_Set_Parameter(const char *Identifier)
{
	CSG_Parameter	*pParameter;

	if( (pParameter = m_pParameters->Get_Parameter(Identifier)) != NULL )
	{
		m_bModified	= true;

		wxPGId	Id	= m_pPG->GetPropertyByName(Identifier);

		if( Id.IsOk() )
		{
			switch( pParameter->Get_Type() )
			{
			default:
				break;

			case PARAMETER_TYPE_String:
			case PARAMETER_TYPE_FilePath:
				pParameter->Set_Value(m_pPG->GetPropertyValueAsString	(Id));
				break;

			case PARAMETER_TYPE_Bool:
				pParameter->Set_Value(m_pPG->GetPropertyValueAsBool		(Id));
				break;

			case PARAMETER_TYPE_Int:
				pParameter->Set_Value(m_pPG->GetPropertyValueAsInt		(Id));
				break;

			case PARAMETER_TYPE_Double:
				pParameter->Set_Value(m_pPG->GetPropertyValueAsDouble	(Id));
				break;

			case PARAMETER_TYPE_Color:
				wxASSERT(m_pPG->IsPropertyValueType(Id, CLASSINFO(wxColour)));
				pParameter->Set_Value(Get_Color_asInt(*(wxDynamicCast(m_pPG->GetPropertyValueAsWxObjectPtr(Id), wxColour))));
			//	wxASSERT(m_pPG->IsPropertyValueType(Id, CLASSINFO(wxColourPropertyValue)));
			//	pParameter->Set_Value(Get_Color_asInt(wxDynamicCast(m_pPG->GetPropertyValueAsWxObjectPtr(Id), wxColourPropertyValue)->m_colour));
				break;
			}
		}

		_Update_Parameters();
	}
}

//---------------------------------------------------------
void CParameters_Control::_Update_Parameters(void)
{
	if( m_pParameters )
	{
		for(int i=0; i<m_pParameters->Get_Count(); i++)
		{
			_Update_Parameter(m_pParameters->Get_Parameter(i));
		}
	}
}

//---------------------------------------------------------
void CParameters_Control::_Update_Parameter(CSG_Parameter *pParameter)
{
	wxPGId	Id	= m_pPG->GetPropertyByName(pParameter->Get_Identifier());

	if( Id.IsOk()  )
	{
		switch( pParameter->Get_Type() )
		{
		default:
			break;

		case PARAMETER_TYPE_String:
		case PARAMETER_TYPE_FilePath:
			if( m_pPG->GetPropertyValueAsString	(Id).Cmp(pParameter->asString()) != 0 )
			{
				m_pPG->SetPropertyValue(Id, pParameter->asString());
			}
			break;

		case PARAMETER_TYPE_Bool:
			if( m_pPG->GetPropertyValueAsBool	(Id) != pParameter->asBool() )
			{
				m_pPG->SetPropertyValue(Id, pParameter->asBool());
			}
			break;

		case PARAMETER_TYPE_Int:
			if( m_pPG->GetPropertyValueAsInt	(Id) != pParameter->asInt() )
			{
				m_pPG->SetPropertyValue(Id, pParameter->asInt());
			}
			break;

		case PARAMETER_TYPE_Double:
			if( m_pPG->GetPropertyValueAsDouble	(Id) != pParameter->asDouble() )
			{
				m_pPG->SetPropertyValue(Id, pParameter->asDouble());
			}
			break;
		}
	}
}

//---------------------------------------------------------
bool CParameters_Control::Update_DataObjects(void)
{
	CSG_Parameter	*pParameter;
	wxPGId		Id;

	if( m_pParameters )
	{
		for(int i=0; i<m_pParameters->Get_Count(); i++)
		{
			pParameter	= m_pParameters->Get_Parameter(i);
			Id			= m_pPG->GetPropertyByName(pParameter->Get_Identifier());

			if( Id.IsOk()  )
			{
				switch( pParameter->Get_Type() )
				{
				default:
					break;

				case PARAMETER_TYPE_Grid_System:
					((CParameters_PG_GridSystem *)Id.GetPropertyPtr())->Update();
					break;

				case PARAMETER_TYPE_Grid:
				case PARAMETER_TYPE_Table:
				case PARAMETER_TYPE_Shapes:
				case PARAMETER_TYPE_TIN:
					((CParameters_PG_Choice *)Id.GetPropertyPtr())->Update();
					break;

				case PARAMETER_TYPE_Grid_List:
				case PARAMETER_TYPE_Table_List:
				case PARAMETER_TYPE_Shapes_List:
				case PARAMETER_TYPE_TIN_List:
					if( g_pData->Check_Parameter(pParameter) == false )
					{
					}
					break;
				}
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

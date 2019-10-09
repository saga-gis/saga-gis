
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
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
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
#include <wx/propgrid/manager.h>
#include <wx/datectrl.h>

#include <saga_api/saga_api.h>

#include "res_dialogs.h"
#include "res_controls.h"

#include "helper.h"

#include "wksp_tool_manager.h"
#include "wksp_data_manager.h"

#include "parameters_control.h"
#include "parameters_properties.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CParameters_Grid : public wxPropertyGrid
{
	DECLARE_CLASS(CParameters_Grid)

public:
	CParameters_Grid(void)
	{}

	CParameters_Grid(wxWindow *pParent, wxWindowID id = wxID_ANY, const wxPoint &pos = wxDefaultPosition, const wxSize &size = wxDefaultSize, long style = wxPG_DEFAULT_STYLE, const wxString &name = wxPropertyGridNameStr)
	:	wxPropertyGrid(pParent, id, pos, size, style, name)
	{}

	virtual wxStatusBar *		GetStatusBar		(void)	{	return( NULL );	}

	void						On_Key				(wxKeyEvent &event)
	{
		event.Skip();

		wxPostEvent(GetParent()->GetParent(), event);
	}

	//-----------------------------------------------------
	DECLARE_EVENT_TABLE()
};

//---------------------------------------------------------
IMPLEMENT_CLASS(CParameters_Grid, wxPropertyGrid)

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CParameters_Grid, wxPropertyGrid)
	EVT_KEY_DOWN		(CParameters_Grid::On_Key)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CParameters_Grid_Manager : public wxPropertyGridManager
{
public:
	CParameters_Grid_Manager(void)	{}

	virtual wxPropertyGrid *	CreatePropertyGrid	(void) const
	{
		return( new CParameters_Grid() );
	}

	wxPropertyGrid *			Initialize			(wxWindow *pParent)
	{
		Create(pParent, ID_WND_PARM, wxDefaultPosition, wxDefaultSize,
			 wxPG_BOLD_MODIFIED
			|wxPG_SPLITTER_AUTO_CENTER
		//	|wxPG_AUTO_SORT
		//	|wxPG_HIDE_MARGIN
		//	|wxPG_STATIC_SPLITTER
		//	|wxPG_HIDE_CATEGORIES
		//	|wxPG_LIMITED_EDITING
			|wxPG_DESCRIPTION
			|wxBORDER_NONE
			|wxTAB_TRAVERSAL
		);

		return( GetGrid() );
	}

};


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
	EVT_KEY_DOWN		(CParameters_Control::On_Key)

	EVT_PG_SELECTED		(ID_WND_PARM, CParameters_Control::On_PG_Selected)
	EVT_PG_CHANGED		(ID_WND_PARM, CParameters_Control::On_PG_Changed)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CParameters_Control::CParameters_Control(wxWindow *pParent, bool bDialog)
	: wxPanel(pParent, -1, wxDefaultPosition, wxDefaultSize, wxNO_BORDER|wxCLIP_CHILDREN)
{
	m_pParameters	= new CSG_Parameters;
	m_pOriginal		= NULL;

	m_bFocus		= 0;

	m_pPGM	= new CParameters_Grid_Manager;
	
	m_pPG	= m_pPGM->Initialize(this);

//	m_pPGM->SetDescBoxHeight(bDialog ? 100 : 50);

	m_pPG->AddActionTrigger(wxPG_ACTION_PRESS_BUTTON, WXK_SPACE);

//	m_pPG->SetExtraStyle(wxPG_EX_HELP_AS_TOOLTIPS);
//	m_pPG->SetCellDisabledTextColour(wxColour(200, 200, 200));

	Set_Parameters(NULL);
}

//---------------------------------------------------------
CParameters_Control::~CParameters_Control(void)
{
	delete(m_pParameters);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CParameters_Control::On_Size(wxSizeEvent &event)
{
	if( m_pPG && event.GetSize().GetWidth() > 0 && event.GetSize().GetHeight() > 0 )
	{
		m_pPG->GetParent()->SetSize(event.GetSize());

		m_pPG->CenterSplitter(true);
	}

	event.Skip();
}

//---------------------------------------------------------
void CParameters_Control::On_Key(wxKeyEvent &event)
{
	if( event.GetKeyCode() == WXK_RETURN )
	{
		wxPostEvent(GetParent(), event);
	}

	event.Skip();
}

//---------------------------------------------------------
void CParameters_Control::On_PG_Selected(wxPropertyGridEvent &event)
{
	if( m_bFocus == 0 && m_pParameters && m_pOriginal )
	{
		SetFocus();
	}

	event.Skip();
}

//---------------------------------------------------------
void CParameters_Control::On_PG_Changed(wxPropertyGridEvent &event)
{
	_Set_Parameter(event.GetProperty());

	event.Skip();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CParameters_Control::Save_Changes(bool bSilent)
{
	if( m_pOriginal && m_bModified && (bSilent || DLG_Message_Confirm(_TL("Save changes?"), wxString::Format(wxT("%s: %s"), _TL("Parameters"), m_pParameters->Get_Name().w_str()))) )
	{
		m_pOriginal->Assign_Values(m_pParameters);

		m_bModified	= false;

		m_pPG->ClearModifiedStatus();
		m_pPG->Refresh();

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
bool CParameters_Control::Restore_Defaults(void)
{
	if( m_pParameters->Restore_Defaults() )
	{
		_Init_Pararameters();

		_Update_Parameters();

		m_bModified	= true;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CParameters_Control::Load(void)
{
	wxString	File_Path;

	if( DLG_Open(File_Path, ID_DLG_PARAMETERS_OPEN) )
	{
		CSG_File	File(&File_Path);

		m_pParameters->Set_Callback(false);

		if(	m_pParameters->Serialize_Compatibility(File)
		||	m_pParameters->Serialize(&File_Path, false) )
		{
			m_pParameters->Set_Callback(true);

			_Init_Pararameters();

			_Update_Parameters();

			m_bModified	= true;

			return( true );
		}

		m_pParameters->Set_Callback(true);

		DLG_Message_Show(_TL("Parameters file could not be imported."), _TL("Load Parameters"));
	}

	return( false );
}

//---------------------------------------------------------
bool CParameters_Control::Save(void)
{
	wxString	File_Path;

	if( DLG_Save(File_Path, ID_DLG_PARAMETERS_SAVE) )
	{
		if( m_pParameters->Serialize(&File_Path, true) )
		{
			return( true );
		}

		DLG_Message_Show(_TL("Parameters file could not be exported."), _TL("Save Parameters"));
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CParameters_Control::Set_Parameters(CSG_Parameters *pParameters)
{
	if( pParameters != m_pParameters )
	{
		m_bFocus++;

		m_pPG->Freeze();

		m_bModified	= false;
		m_pPG->ClearModifiedStatus();

		m_pPG->ClearSelection();
		m_pPGM->SetDescription("", "");

		if( pParameters == NULL || pParameters->Get_Count() == 0 )
		{
			m_pParameters->Set_Callback(false);
			m_pParameters->Assign(m_pOriginal = pParameters);

			m_pPG->Clear();

			m_pPG->Append(new wxPropertyCategory(_TL("No parameters available."), wxPG_LABEL));
		}
		else if( m_pOriginal != pParameters )
		{
			pParameters->DataObjects_Check(true);

			m_pParameters->Set_Callback(false);
			m_pParameters->Assign(m_pOriginal = pParameters);

			m_pPG->Clear();

			_Add_Properties(m_pParameters);

			m_pParameters->Set_Callback(true);

			_Init_Pararameters();
		}
		else // if( m_pOriginal == pParameters )
		{
			m_pParameters->Assign_Values(m_pOriginal);

			_Init_Pararameters();
		}

		m_pPG->Thaw();

		m_bFocus--;
	}

	//-----------------------------------------------------
	_Update_Parameters();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define CHECK_LIST_OUTPUT(p)	if( p->is_Output() ) { pRoot = NULL; break; }

//---------------------------------------------------------
#define CHECK_DATA_NODE(pNode, Name, ID)	if( !pNode )\
	{\
		pNode	= new wxPropertyCategory(Name, ID);\
		\
		if( !pData )\
		{\
			m_pPG->Append(pData = new wxPropertyCategory(_TL("Data Objects"), "_DATAOBJECT_DATAOBJECTS"));\
		}\
		\
		m_pPG->Insert(pData, -1, pNode);\
	}\
	\
	pRoot	= pNode;

//---------------------------------------------------------
void CParameters_Control::_Add_Properties(CSG_Parameters *pParameters)
{
	wxPGProperty *pData        = NULL;
	wxPGProperty *pGrids       = NULL;
	wxPGProperty *pShapes      = NULL;
	wxPGProperty *pTables      = NULL;
	wxPGProperty *pTINs        = NULL;
	wxPGProperty *pPointClouds = NULL;
	wxPGProperty *pOptions     = NULL;

	m_Precision	= g_pTools->Get_Parameter("FLOAT_PRECISION")->asInt();

	for(int i=0; i<pParameters->Get_Count(); i++)
	{
		CSG_Parameter	*pParameter	= pParameters->Get_Parameter(i);

		if(	pParameter->do_UseInGUI() && pParameter->Get_Parent() == NULL )
		{
			wxPGProperty	*pRoot	= NULL;

			switch( pParameter->Get_Type() )
			{
			case PARAMETER_TYPE_DataObject_Output:
				break;

			case PARAMETER_TYPE_Grid_System:
				if( pParameter->Get_Children_Count() == 0 )
				{
					CHECK_DATA_NODE(pGrids, _TL("Grids"), "_DATAOBJECT_GRIDS");
				}
				else for(int j=0; j<pParameter->Get_Children_Count() && !pRoot; j++)
				{
					if(	(pParameter->Get_Child(j)->Get_Type() != PARAMETER_TYPE_Grid_List
					  && pParameter->Get_Child(j)->Get_Type() != PARAMETER_TYPE_Grids_List)
					||   pParameter->Get_Child(j)->is_Input() )
					{
						CHECK_DATA_NODE(pGrids, _TL("Grids"), "_DATAOBJECT_GRIDS");
					}
				}
				break;

			case PARAMETER_TYPE_Grid_List      : CHECK_LIST_OUTPUT(pParameter);
			case PARAMETER_TYPE_Grid           :
				CHECK_DATA_NODE(pGrids      , _TL("Grids"       ), "_DATAOBJECT_GRIDS"      );
				break;

			case PARAMETER_TYPE_Grids_List     : CHECK_LIST_OUTPUT(pParameter);
			case PARAMETER_TYPE_Grids          :
				CHECK_DATA_NODE(pGrids      , _TL("Grids"       ), "_DATAOBJECT_GRIDS"      );
				break;

			case PARAMETER_TYPE_Table_List     : CHECK_LIST_OUTPUT(pParameter);
			case PARAMETER_TYPE_Table          :
				CHECK_DATA_NODE(pTables     , _TL("Tables"      ), "_DATAOBJECT_TABLES"     );
				break;

			case PARAMETER_TYPE_Shapes_List    : CHECK_LIST_OUTPUT(pParameter);
			case PARAMETER_TYPE_Shapes         :
				CHECK_DATA_NODE(pShapes     , _TL("Shapes"      ), "_DATAOBJECT_SHAPES"     );
				break;

			case PARAMETER_TYPE_TIN_List       : CHECK_LIST_OUTPUT(pParameter);
			case PARAMETER_TYPE_TIN            :
				CHECK_DATA_NODE(pTINs       , _TL("TIN"         ), "_DATAOBJECT_TINS"       );
				break;

			case PARAMETER_TYPE_PointCloud_List: CHECK_LIST_OUTPUT(pParameter);
			case PARAMETER_TYPE_PointCloud     :
				CHECK_DATA_NODE(pPointClouds, _TL("Point Clouds"), "_DATAOBJECT_POINTCLOUDS");
				break;

			default:
				if( !pOptions )
				{
					m_pPG->Append(pOptions = new wxPropertyCategory(_TL("Options"), "_DATAOBJECT_OPTIONS"));
				}
				pRoot	= pOptions;
				break;
			}

			if( pRoot )
			{
				_Add_Property(pRoot, pParameter);
			}
		}
	}
}

//---------------------------------------------------------
void CParameters_Control::_Add_Property(wxPGProperty *pParent, CSG_Parameter *pParameter)
{
	if( pParameter->do_UseInGUI() )
	{
		wxPGProperty	*pProperty	= _Get_Property(pParent, pParameter);

		if( pParameter->Get_Children_Count() > 0 )
		{
			for(int i=0; i<pParameter->Get_Children_Count(); i++)
			{
				_Add_Property(pProperty, pParameter->Get_Child(i));
			}

			m_pPG->Expand(pProperty);
		}
	}
}

//---------------------------------------------------------
wxPGProperty * CParameters_Control::_Get_Property(wxPGProperty *pParent, CSG_Parameter *pParameter)
{
	#define ADD_PROPERTY(p, limit)	if( pParent ) { m_pPG->Insert(pParent, -1, pProperty = p); } else { m_pPG->Append(pProperty = p); }\
		if( pParameter->is_Information() ) { m_pPG->LimitPropertyEditing(pProperty); m_pPG->EnableProperty(pProperty, false); } else\
		if( limit ) { m_pPG->LimitPropertyEditing(pProperty); } { CSG_String	s, sDesc;\
		sDesc	= pParameter->Get_Description(PARAMETER_DESCRIPTION_TYPE);\
		s		= pParameter->Get_Description(PARAMETER_DESCRIPTION_TEXT      ); if( !s.is_Empty() ) { sDesc.Append("\n___\n"); sDesc.Append(s); }\
		s		= pParameter->Get_Description(PARAMETER_DESCRIPTION_PROPERTIES); if( !s.is_Empty() ) { sDesc.Append("\n___\n"); sDesc.Append(s); }\
		m_pPG->SetPropertyHelpString(pProperty, sDesc.c_str()); }

	wxString	Name(pParameter->Get_Name()), ID(pParameter->Get_Identifier());

	wxPGProperty	*pProperty	= NULL;

	switch( pParameter->Get_Type() )
	{
	default:
		if( pParameter->Get_Children_Count() > 0 )
		{
			if( !pParameter->Get_Parent() || pParameter->Get_Parent()->Get_Type() == PARAMETER_TYPE_Node )
			{
				ADD_PROPERTY(new wxPropertyCategory(Name, ID    ), true);
			//	m_pPG->SetPropertyCell(pProperty, 0, "", wxNullBitmap, SYS_Get_Color(wxSYS_COLOUR_BTNTEXT), SYS_Get_Color(wxSYS_COLOUR_BTNFACE));
			}
			else
			{
				ADD_PROPERTY(new wxStringProperty  (Name, ID, ""), false);
			}
		}
		break;

	case PARAMETER_TYPE_Bool            :
		ADD_PROPERTY(new wxBoolProperty       (Name, ID, pParameter->asBool()  ), false);
		pProperty->SetAttribute(wxPG_BOOL_USE_CHECKBOX, true);
		break;

	case PARAMETER_TYPE_Int             :
		ADD_PROPERTY(new wxIntProperty        (Name, ID, pParameter->asInt()   ), false);
		break;

	case PARAMETER_TYPE_Double          :
		ADD_PROPERTY(new wxFloatProperty      (Name, ID, pParameter->asDouble()), false);
		pProperty->SetAttribute(wxPG_FLOAT_PRECISION, m_Precision);
		break;

	case PARAMETER_TYPE_Range           :
		ADD_PROPERTY(new CParameters_PG_Range (Name, ID, pParameter            ), false);
		break;

	case PARAMETER_TYPE_Degree          :
		ADD_PROPERTY(new CParameters_PG_Degree(Name, ID, pParameter            ), false);
		break;

	case PARAMETER_TYPE_Date            :
		ADD_PROPERTY(new wxDateProperty       (Name, ID, pParameter->asDouble()), false);	// from JDN
		pProperty->SetAttribute(wxPG_DATE_PICKER_STYLE, wxDP_DROPDOWN|wxDP_SHOWCENTURY);
		break;

	case PARAMETER_TYPE_String          :
		if( ((CSG_Parameter_String *)pParameter)->is_Password() )
		{
			ADD_PROPERTY(new wxStringProperty    (Name, ID, pParameter->asString()), false);
			pProperty->SetAttribute(wxPG_STRING_PASSWORD, true);
		}
		else
		{
			ADD_PROPERTY(new wxLongStringProperty(Name, ID, pParameter->asString()), false);
		}
		break;

	case PARAMETER_TYPE_Color          :
		ADD_PROPERTY(new wxColourProperty     (Name, ID, Get_Color_asWX(pParameter->asColor())), false);
		pProperty->SetEditor(wxPGEditor_Choice);
		break;

	case PARAMETER_TYPE_Colors         :
		ADD_PROPERTY(new CParameters_PG_Colors(Name, ID, pParameter), true);
		break;

	case PARAMETER_TYPE_FilePath       :
	case PARAMETER_TYPE_Choices        :
	case PARAMETER_TYPE_Table_Fields   :
		ADD_PROPERTY(new CParameters_PG_Dialog(Name, ID, pParameter), false);
		break;

	case PARAMETER_TYPE_Text           :
	case PARAMETER_TYPE_Font           :
	case PARAMETER_TYPE_FixedTable     :
	case PARAMETER_TYPE_Parameters     :
		ADD_PROPERTY(new CParameters_PG_Dialog(Name, ID, pParameter), true);
		break;

	case PARAMETER_TYPE_Grid_List      :
	case PARAMETER_TYPE_Grids_List     :
	case PARAMETER_TYPE_Table_List     :
	case PARAMETER_TYPE_Shapes_List    :
	case PARAMETER_TYPE_TIN_List       :
	case PARAMETER_TYPE_PointCloud_List:
		if( !pParameter->is_Output() )
		{
			ADD_PROPERTY(new CParameters_PG_Dialog(Name, ID, pParameter), true);
		}
		break;

	case PARAMETER_TYPE_Choice         :
	case PARAMETER_TYPE_Table_Field    :
	case PARAMETER_TYPE_Grid_System    :
	case PARAMETER_TYPE_Grid           :
	case PARAMETER_TYPE_Grids          :
	case PARAMETER_TYPE_Table          :
	case PARAMETER_TYPE_Shapes         :
	case PARAMETER_TYPE_TIN            :
	case PARAMETER_TYPE_PointCloud     :
		ADD_PROPERTY(new CParameters_PG_Choice(pParameter), false);
		break;
	}

	//-----------------------------------------------------
	return( pProperty );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CParameters_Control::_Get_Identifier(CSG_Parameter *pParameter)
{
	wxString	id;

	if( pParameter->Get_Parent() )
	{
		id	= _Get_Identifier(pParameter->Get_Parent()) + wxT(".");
	}

	id	+= pParameter->Get_Identifier();

	return( id );
}

//---------------------------------------------------------
bool CParameters_Control::_Get_Enabled(CSG_Parameter *pParameter)
{
	return( !pParameter || (pParameter->is_Enabled() && _Get_Enabled(pParameter->Get_Parent())) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter * CParameters_Control::_Get_Parameter(wxPGProperty *pProperty)
{
	return( pProperty ? m_pParameters->Get_Parameter(pProperty->GetBaseName().wx_str()) : NULL );
}

//---------------------------------------------------------
void CParameters_Control::_Set_Parameter(wxPGProperty *pProperty)
{
	CSG_Parameter	*pParameter	=  _Get_Parameter(pProperty);

	if( pParameter )
	{
		m_bModified	= true;

		switch( pParameter->Get_Type() )
		{
		default:
			break;

		case PARAMETER_TYPE_Date:
			pParameter->Set_Value(((wxDateProperty *)pProperty)->GetDateValue().GetJDN());
			break;

		case PARAMETER_TYPE_String:
		case PARAMETER_TYPE_FilePath:
			pParameter->Set_Value(m_pPG->GetPropertyValueAsString(pProperty).wx_str());
			break;

		case PARAMETER_TYPE_Bool:
			pParameter->Set_Value(m_pPG->GetPropertyValueAsBool(pProperty));
			break;

		case PARAMETER_TYPE_Int:
			pParameter->Set_Value(m_pPG->GetPropertyValueAsInt(pProperty));
			break;

		case PARAMETER_TYPE_Double:
			pParameter->Set_Value(m_pPG->GetPropertyValueAsDouble(pProperty));
			break;

		case PARAMETER_TYPE_Color:
			pParameter->Set_Value(Get_Color_asInt(((wxColourProperty *)pProperty)->GetVal().m_colour));
			break;
		}

		if( pParameter->Get_Type() != PARAMETER_TYPE_Date )
		{
			_Update_Parameters();

			m_pPG->SelectProperty(pProperty);
		}
	}
}

//---------------------------------------------------------
void CParameters_Control::_Update_Parameter(CSG_Parameter *pParameter)
{
	wxPGProperty	*pProperty	= m_pPG->GetProperty(_Get_Identifier(pParameter));

	if( pProperty )
	{
		pProperty->Enable( _Get_Enabled(pParameter));
		pProperty->Hide  (!_Get_Enabled(pParameter));

		switch( pParameter->Get_Type() )
		{
		default:
			break;

		case PARAMETER_TYPE_String:
			if( m_pPG->GetPropertyValueAsString	(pProperty).Cmp(pParameter->asString()) != 0 )
			{
				m_pPG->SetPropertyValue(pProperty, pParameter->asString());
			}
			break;

		case PARAMETER_TYPE_Bool:
			if( m_pPG->GetPropertyValueAsBool	(pProperty) != pParameter->asBool() )
			{
				m_pPG->SetPropertyValue(pProperty, pParameter->asBool());
			}
			break;

		case PARAMETER_TYPE_Int:
			if( m_pPG->GetPropertyValueAsInt	(pProperty) != pParameter->asInt() )
			{
				m_pPG->SetPropertyValue(pProperty, pParameter->asInt());
			}
			break;

		case PARAMETER_TYPE_Double:
			if( m_pPG->GetPropertyValueAsDouble	(pProperty) != pParameter->asDouble() )
			{
				m_pPG->SetPropertyValue(pProperty, pParameter->asDouble());
			}
			break;

		case PARAMETER_TYPE_Range:
			((CParameters_PG_Range  *)pProperty)->Update();
			break;

		case PARAMETER_TYPE_Degree:
			((CParameters_PG_Degree *)pProperty)->Update();
			break;

		case PARAMETER_TYPE_Date:
			{
				wxDateTime	Date(pParameter->asDouble());

				if( ((wxDateProperty *)pProperty)->GetDateValue() != Date )
				{
					((wxDateProperty *)pProperty)->SetDateValue(Date);
				}
			}
			break;

		case PARAMETER_TYPE_Choice         :
		case PARAMETER_TYPE_Table_Field    :
		case PARAMETER_TYPE_Grid_System    :
		case PARAMETER_TYPE_Grid           :
		case PARAMETER_TYPE_Grids          :
		case PARAMETER_TYPE_Table          :
		case PARAMETER_TYPE_Shapes         :
		case PARAMETER_TYPE_TIN            :
		case PARAMETER_TYPE_PointCloud     :
			((CParameters_PG_Choice *)pProperty)->Update();
		break;

		case PARAMETER_TYPE_Text           :
		case PARAMETER_TYPE_FilePath       :
		case PARAMETER_TYPE_Font           :
		case PARAMETER_TYPE_Table_Fields   :
		case PARAMETER_TYPE_Choices        :
		case PARAMETER_TYPE_FixedTable     :
		case PARAMETER_TYPE_Grid_List      :
		case PARAMETER_TYPE_Grids_List     :
		case PARAMETER_TYPE_Table_List     :
		case PARAMETER_TYPE_Shapes_List    :
		case PARAMETER_TYPE_TIN_List       :
		case PARAMETER_TYPE_PointCloud_List:
		case PARAMETER_TYPE_Parameters     :
			((CParameters_PG_Dialog *)pProperty)->Update();
			break;
		}
	}
}

//---------------------------------------------------------
bool CParameters_Control::Update_DataObjects(void)
{
	if( m_pParameters )
	{
		for(int i=0; i<m_pParameters->Get_Count(); i++)
		{
			CSG_Parameter	*pParameter	= m_pParameters->Get_Parameter(i);
			wxPGProperty	*pProperty	= m_pPG->GetProperty(_Get_Identifier(pParameter));

			if( pProperty  )
			{
				switch( pParameter->Get_Type() )
				{
				default:
					break;

				case PARAMETER_TYPE_Grid_System:
				case PARAMETER_TYPE_Grid:
				case PARAMETER_TYPE_Grids:
				case PARAMETER_TYPE_Table:
				case PARAMETER_TYPE_Shapes:
				case PARAMETER_TYPE_TIN:
				case PARAMETER_TYPE_PointCloud:
					((CParameters_PG_Choice *)pProperty)->Update();
					break;

				case PARAMETER_TYPE_Grid_List:
				case PARAMETER_TYPE_Grids_List:
				case PARAMETER_TYPE_Table_List:
				case PARAMETER_TYPE_Shapes_List:
				case PARAMETER_TYPE_TIN_List:
				case PARAMETER_TYPE_PointCloud_List:
					if( pParameter->Check() == false )
					{
					}
					break;
				}
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
#define UPDATE_DATA_NODE(NODE)	{\
	wxPGProperty	*pProperty	= m_pPG->GetProperty(NODE);\
	\
	if( pProperty )\
	{\
		bool	bShow	= false;\
		\
		for(size_t i=0; i<pProperty->GetChildCount() && !bShow; i++)\
		{\
			if( pProperty->Item(i)->IsEnabled() )\
			{\
				bShow	= true;\
			}\
		}\
		\
		pProperty->Hide(!bShow, wxPG_DONT_RECURSE);\
	}\
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

		UPDATE_DATA_NODE("_DATAOBJECT_GRIDS");
		UPDATE_DATA_NODE("_DATAOBJECT_TABLES");
		UPDATE_DATA_NODE("_DATAOBJECT_SHAPES");
		UPDATE_DATA_NODE("_DATAOBJECT_TINS");
		UPDATE_DATA_NODE("_DATAOBJECT_POINTCLOUDS");
		UPDATE_DATA_NODE("_DATAOBJECT_OPTIONS");

		m_pPG->Refresh();
	}
}

//---------------------------------------------------------
void CParameters_Control::_Init_Pararameters(void)
{
	if( m_pParameters )
	{
		for(int i=0; i<m_pParameters->Get_Count(); i++)
		{
			m_pParameters->Get_Parameter(i)->has_Changed(PARAMETER_CHECK_ENABLE);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

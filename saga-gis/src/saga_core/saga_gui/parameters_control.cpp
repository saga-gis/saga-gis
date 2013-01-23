/**********************************************************
 * Version $Id$
 *********************************************************/

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
#include <wx/propgrid/manager.h>

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
#ifndef _SAGA_LINUX
	#define	PG_USE_MANAGER
#endif


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

	EVT_PG_SELECTED		(ID_WND_PARM_PG_ACTIVE, CParameters_Control::On_PG_Selected)
	EVT_PG_SELECTED		(ID_WND_PARM_PG_DIALOG, CParameters_Control::On_PG_Selected)
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
	m_pParameters	= new CSG_Parameters();
	m_pOriginal		= NULL;

#ifdef PG_USE_MANAGER
	m_pPGM	= new wxPropertyGridManager(this, bDialog ? ID_WND_PARM_PG_DIALOG : ID_WND_PARM_PG_ACTIVE, wxDefaultPosition, wxDefaultSize,
		 wxPG_BOLD_MODIFIED
		|wxPG_SPLITTER_AUTO_CENTER
	//	|wxPG_AUTO_SORT
	//	|wxPG_HIDE_MARGIN
	//	|wxPG_STATIC_SPLITTER
	//	|wxPG_HIDE_CATEGORIES
	//	|wxPG_LIMITED_EDITING
		|wxTAB_TRAVERSAL
	//	|wxPG_TOOLBAR
		|wxPG_DESCRIPTION
	//	|wxPG_COMPACTOR
	//	|wxBORDER_SUNKEN
		|wxBORDER_NONE
	);

	m_pPG	= m_pPGM->GetGrid();

	m_pPGM->SetDescBoxHeight(bDialog ? 100 : 50);
#else
	m_pPG	= new wxPropertyGrid(this, bDialog ? ID_WND_PARM_PG_DIALOG : ID_WND_PARM_PG_ACTIVE, wxDefaultPosition, wxDefaultSize,
		 wxPG_BOLD_MODIFIED
		|wxPG_SPLITTER_AUTO_CENTER
	//	|wxPG_AUTO_SORT
	//	|wxPG_HIDE_MARGIN
	//	|wxPG_STATIC_SPLITTER
	//	|wxPG_HIDE_CATEGORIES
	//	|wxPG_LIMITED_EDITING
		|wxTAB_TRAVERSAL
	//	|wxPG_TOOLBAR
		|wxPG_DESCRIPTION
	//	|wxPG_COMPACTOR
	//	|wxBORDER_SUNKEN
		|wxBORDER_NONE
	);

	m_pPGM	= NULL;

#endif

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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CParameters_Control::On_Size(wxSizeEvent &event)
{
	if( m_pPGM )
	{
		m_pPGM->SetSize(GetClientSize());
	}
	else
	{
		m_pPG ->SetSize(GetClientSize());
	}

	m_pPG->CenterSplitter(true);

	event.Skip();
}

//---------------------------------------------------------
void CParameters_Control::On_PG_Selected(wxPropertyGridEvent &WXUNUSED(event))
{}

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
	if( m_pOriginal && m_bModified && (bSilent || DLG_Message_Confirm(_TL("Save changes?"), wxString::Format(wxT("%s: %s"), _TL("Parameters"), m_pParameters->Get_Name().w_str()))) )
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

		if(	m_pParameters->Serialize_Compatibility(File)
		||	m_pParameters->Serialize(&File_Path, false) )
		{
			_Init_Pararameters();

			_Update_Parameters();

			m_bModified	= true;

			return( true );
		}

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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CParameters_Control::Set_Parameters(CSG_Parameters *pParameters)
{
	if( pParameters != m_pParameters )
	{
		m_pPG->Freeze();

		m_bModified	= false;
		m_pPG->ClearModifiedStatus();

		if( pParameters == NULL || pParameters->Get_Count() == 0 )
		{
			m_pParameters->Set_Callback(false);
			m_pParameters->Assign(m_pOriginal = pParameters);

			m_pPG->Clear();

			m_pPG->Append(new wxPropertyCategory(_TL("No parameters available."), wxPG_LABEL));
		}
		else if( m_pOriginal != pParameters )
		{
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
	}

	//-----------------------------------------------------
	_Update_Parameters();

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
		pNode	= new wxPropertyCategory(Name, ID);\
		if( !pData )\
			m_pPG->Append(pData = new wxPropertyCategory(_TL("Data Objects"), wxT("_DATAOBJECT_DATAOBJECTS")));\
		m_pPG->Insert(pData, -1, pNode);\
	}\
	pRoot	= pNode;

//---------------------------------------------------------
void CParameters_Control::_Add_Properties(CSG_Parameters *pParameters)
{
	wxPGProperty	*pGrids, *pShapes, *pTables, *pTINs, *pPointClouds, *pOptions, *pData, *pRoot;

	pData			= NULL;
	pGrids			= NULL;
	pShapes			= NULL;
	pTables			= NULL;
	pTINs			= NULL;
	pPointClouds	= NULL;
	pOptions		= NULL;

	for(int i=0; i<pParameters->Get_Count(); i++)
	{
		if(	pParameters->Get_Parameter(i)->Get_Parent() == NULL )
		{
			pRoot	= NULL;

			switch( pParameters->Get_Parameter(i)->Get_Type() )
			{
			case PARAMETER_TYPE_DataObject_Output:
				break;

			case PARAMETER_TYPE_Grid_System:
				if( pParameters->Get_Parameter(i)->Get_Children_Count() == 0 )
				{
					CHECK_DATA_NODE( pGrids	, _TL("Grids"), wxT("_DATAOBJECT_GRIDS") );
				}
				else
				{
					for(int j=0; j<pParameters->Get_Parameter(i)->Get_Children_Count() && !pRoot; j++)
					{
						if(	pParameters->Get_Parameter(i)->Get_Child(j)->Get_Type() != PARAMETER_TYPE_Grid_List
						||	pParameters->Get_Parameter(i)->Get_Child(j)->is_Input() )
						{
							CHECK_DATA_NODE( pGrids	, _TL("Grids"), wxT("_DATAOBJECT_GRIDS") );
						}
					}
				}
				break;

			case PARAMETER_TYPE_Grid_List:
				CHECK_LIST_OUTPUT(pParameters->Get_Parameter(i));
			case PARAMETER_TYPE_Grid:
				CHECK_DATA_NODE(pGrids		, _TL("Grids") , wxT("_DATAOBJECT_GRIDS"));
				break;

			case PARAMETER_TYPE_Table_List:
				CHECK_LIST_OUTPUT(pParameters->Get_Parameter(i));
			case PARAMETER_TYPE_Table:
				CHECK_DATA_NODE(pTables		, _TL("Tables"), wxT("_DATAOBJECT_TABLES"));
				break;

			case PARAMETER_TYPE_Shapes_List:
				CHECK_LIST_OUTPUT(pParameters->Get_Parameter(i));
			case PARAMETER_TYPE_Shapes:
				CHECK_DATA_NODE(pShapes		, _TL("Shapes"), wxT("_DATAOBJECT_SHAPES"));
				break;

			case PARAMETER_TYPE_TIN_List:
				CHECK_LIST_OUTPUT(pParameters->Get_Parameter(i));
			case PARAMETER_TYPE_TIN:
				CHECK_DATA_NODE(pTINs		, _TL("TIN"), wxT("_DATAOBJECT_TINS"));
				break;

			case PARAMETER_TYPE_PointCloud_List:
				CHECK_LIST_OUTPUT(pParameters->Get_Parameter(i));
			case PARAMETER_TYPE_PointCloud:
				CHECK_DATA_NODE(pPointClouds, _TL("Point Clouds"), wxT("_DATAOBJECT_POINTCLOUDS"));
				break;

			default:
				if( !pOptions )
				{
					m_pPG->Append(pOptions = new wxPropertyCategory(_TL("Options"), wxT("_DATAOBJECT_OPTIONS")));
				}

				pRoot	= pOptions;
				break;
			}

			if( pRoot )
			{
				_Add_Property(pRoot, pParameters->Get_Parameter(i));
			}
		}
	}
}

//---------------------------------------------------------
void CParameters_Control::_Add_Property(wxPGProperty *pParent, CSG_Parameter *pParameter)
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

//---------------------------------------------------------
wxPGProperty * CParameters_Control::_Get_Property(wxPGProperty *pParent, CSG_Parameter *pParameter)
{
	wxString		Name(pParameter->Get_Name()), ID(pParameter->Get_Identifier());
	wxPGProperty	*pProperty	= NULL;

	switch( pParameter->Get_Type() )
	{
	case PARAMETER_TYPE_Node:	default:
		if( pParameter->Get_Children_Count() > 0 )
		{
			if( pParameter->Get_Parent() == NULL || pParameter->Get_Parent()->Get_Type() == PARAMETER_TYPE_Node )
				pProperty	= new wxPropertyCategory	(Name, ID);
			else
				pProperty	= new wxStringProperty		(Name, ID, wxT(""));
		}
		break;

	case PARAMETER_TYPE_Bool:
		pProperty	= new wxBoolProperty		(Name, ID, pParameter->asBool());
		break;

	case PARAMETER_TYPE_Int:
		pProperty	= new wxIntProperty			(Name, ID, pParameter->asInt());
		break;

	case PARAMETER_TYPE_Double:
		pProperty	= new wxFloatProperty		(Name, ID, pParameter->asDouble());
		break;

	case PARAMETER_TYPE_Range:
		pProperty	= new CParameters_PG_Range	(Name, ID, pParameter);
		break;

	case PARAMETER_TYPE_Degree:
		pProperty	= new CParameters_PG_Degree	(Name, ID, pParameter);
		break;

	case PARAMETER_TYPE_String:
		pProperty	= new wxStringProperty		(Name, ID, pParameter->asString());
		break;

	case PARAMETER_TYPE_Color:
		pProperty	= new wxColourProperty		(Name, ID, Get_Color_asWX(pParameter->asColor()));
		break;

	case PARAMETER_TYPE_Colors:
		pProperty	= new CParameters_PG_Colors	(Name, ID, pParameter);
		break;

	case PARAMETER_TYPE_Text:
	case PARAMETER_TYPE_FilePath:
	case PARAMETER_TYPE_Font:
	case PARAMETER_TYPE_Table_Fields:
	case PARAMETER_TYPE_FixedTable:
	case PARAMETER_TYPE_Parameters:
		pProperty	= new CParameters_PG_Dialog	(Name, ID, pParameter);
		break;

	case PARAMETER_TYPE_Grid_List:
	case PARAMETER_TYPE_Table_List:
	case PARAMETER_TYPE_Shapes_List:
	case PARAMETER_TYPE_TIN_List:
	case PARAMETER_TYPE_PointCloud_List:
		if( !pParameter->is_Output() )
		{
			pProperty	= new CParameters_PG_Dialog	(Name, ID, pParameter);
		}
		break;

	case PARAMETER_TYPE_Choice:
	case PARAMETER_TYPE_Table_Field:
	case PARAMETER_TYPE_Grid_System:
	case PARAMETER_TYPE_Grid:
	case PARAMETER_TYPE_Table:
	case PARAMETER_TYPE_Shapes:
	case PARAMETER_TYPE_TIN:
	case PARAMETER_TYPE_PointCloud:
		pProperty	= new CParameters_PG_Choice	(pParameter);
		break;
	}

	//-----------------------------------------------------
	if( pProperty )
	{
		if( pParent )
		{
			m_pPG->Insert(pParent, -1, pProperty);
		}
		else
		{
			m_pPG->Append(pProperty);
		}

		//-------------------------------------------------
		CSG_String	s, sDesc;

		sDesc	= pParameter->Get_Description(PARAMETER_DESCRIPTION_TYPE);

		s		= pParameter->Get_Description(PARAMETER_DESCRIPTION_TEXT);
		if( s.Length() > 0 )	{	sDesc.Append( wxT("\n___\n") );	sDesc.Append( s );	}

		s		= pParameter->Get_Description(PARAMETER_DESCRIPTION_PROPERTIES);
		if( s.Length() > 0 )	{	sDesc.Append(wxT("\n___\n"));	sDesc.Append(s);	}

		m_pPG->SetPropertyHelpString(pProperty, sDesc.c_str());

		//-------------------------------------------------
		switch( pParameter->Get_Type() )
		{
		case PARAMETER_TYPE_Node:	default:
			if( pParameter->Get_Parent() && pParameter->Get_Parent()->Get_Type() != PARAMETER_TYPE_Node )
			{
				m_pPG->LimitPropertyEditing(pProperty);
			//	m_pPG->SetPropertyCell(pProperty, 0, Name   , wxNullBitmap, SYS_Get_Color(wxSYS_COLOUR_BTNTEXT), SYS_Get_Color(wxSYS_COLOUR_BTNFACE));
			//	m_pPG->SetPropertyCell(pProperty, 1, wxT(""), wxNullBitmap, SYS_Get_Color(wxSYS_COLOUR_BTNTEXT), SYS_Get_Color(wxSYS_COLOUR_BTNFACE));
			}
			break;

		case PARAMETER_TYPE_Bool:
			pProperty->SetAttribute(wxPG_BOOL_USE_CHECKBOX	, (long)true);
			break;

		case PARAMETER_TYPE_Int:
			break;

		case PARAMETER_TYPE_Double:
			pProperty->SetAttribute(wxPG_FLOAT_PRECISION	, (long)16);
			break;

		case PARAMETER_TYPE_String:
			if( ((CSG_Parameter_String *)pParameter->Get_Data())->is_Password() )
			{
				pProperty->SetAttribute(wxPG_STRING_PASSWORD, (long)pParameter->asString());
			}
			else if( pParameter->is_Information() )
			{
				m_pPG->LimitPropertyEditing(pProperty);
			}
			break;

		case PARAMETER_TYPE_Color:
			pProperty->SetEditor(wxPGEditor_Choice);
			break;

		case PARAMETER_TYPE_FilePath:
			break;

		case PARAMETER_TYPE_Colors:
		case PARAMETER_TYPE_Text:
		case PARAMETER_TYPE_Font:
		case PARAMETER_TYPE_FixedTable:
		case PARAMETER_TYPE_Grid_List:
		case PARAMETER_TYPE_Table_List:
		case PARAMETER_TYPE_Shapes_List:
		case PARAMETER_TYPE_TIN_List:
		case PARAMETER_TYPE_PointCloud_List:
		case PARAMETER_TYPE_Parameters:
			m_pPG->LimitPropertyEditing(pProperty);
			break;
		}

		if( pParameter->is_Information() )
		{
			m_pPG->EnableProperty(pProperty, false);
		}
	}

	return( pProperty );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CParameters_Control::_Set_Parameter(const wxString &Identifier)
{
	wxPGProperty	*pProperty	= m_pPG->GetProperty(Identifier);

	if( pProperty )
	{
		CSG_Parameter	*pParameter	= m_pParameters->Get_Parameter(
			!pProperty->IsSubProperty() ? Identifier.wx_str() : Identifier.AfterLast(wxT('.')).wx_str()
		);

		if( pParameter )
		{
			switch( pParameter->Get_Type() )
			{
			default:
				break;

			case PARAMETER_TYPE_String:
			case PARAMETER_TYPE_FilePath:
				pParameter->Set_Value(m_pPG->GetPropertyValueAsString(pProperty).wx_str());
				break;

			case PARAMETER_TYPE_Bool:
				pParameter->Set_Value(m_pPG->GetPropertyValueAsBool		(pProperty));
				break;

			case PARAMETER_TYPE_Int:
				pParameter->Set_Value(m_pPG->GetPropertyValueAsInt		(pProperty));
				break;

			case PARAMETER_TYPE_Double:
				pParameter->Set_Value(m_pPG->GetPropertyValueAsDouble	(pProperty));
				break;

			case PARAMETER_TYPE_Color:
				pParameter->Set_Value(Get_Color_asInt(((wxColourProperty *)pProperty)->GetVal().m_colour));
				break;
			}

			m_bModified	= true;

			_Update_Parameters();
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

		case PARAMETER_TYPE_Choice:
		case PARAMETER_TYPE_Table_Field:
		case PARAMETER_TYPE_Grid_System:
		case PARAMETER_TYPE_Grid:
		case PARAMETER_TYPE_Table:
		case PARAMETER_TYPE_Shapes:
		case PARAMETER_TYPE_TIN:
		case PARAMETER_TYPE_PointCloud:
			((CParameters_PG_Choice *)pProperty)->Update();
		break;

		case PARAMETER_TYPE_Text:
		case PARAMETER_TYPE_FilePath:
		case PARAMETER_TYPE_Font:
		case PARAMETER_TYPE_Table_Fields:
		case PARAMETER_TYPE_FixedTable:
		case PARAMETER_TYPE_Grid_List:
		case PARAMETER_TYPE_Table_List:
		case PARAMETER_TYPE_Shapes_List:
		case PARAMETER_TYPE_TIN_List:
		case PARAMETER_TYPE_PointCloud_List:
		case PARAMETER_TYPE_Parameters:
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
				case PARAMETER_TYPE_Table:
				case PARAMETER_TYPE_Shapes:
				case PARAMETER_TYPE_TIN:
				case PARAMETER_TYPE_PointCloud:
					((CParameters_PG_Choice *)pProperty)->Update();
					break;

				case PARAMETER_TYPE_Grid_List:
				case PARAMETER_TYPE_Table_List:
				case PARAMETER_TYPE_Shapes_List:
				case PARAMETER_TYPE_TIN_List:
				case PARAMETER_TYPE_PointCloud_List:
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

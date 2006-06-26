
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
//                    WKSP_Layer.cpp                     //
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
#include "res_commands.h"
#include "res_dialogs.h"

#include "helper.h"

#include "active.h"
#include "active_parameters.h"

#include "wksp_base_control.h"

#include "wksp_map_manager.h"
#include "wksp_map.h"
#include "wksp_map_layer.h"

#include "wksp_layer.h"
#include "wksp_layer_classify.h"
#include "wksp_layer_legend.h"

#include "view_histogram.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Layer *	Get_Active_Layer(void)
{
	return( g_pACTIVE ? g_pACTIVE->Get_Layer() : NULL );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Layer::CWKSP_Layer(CDataObject *pObject)
{
	m_pObject		= pObject;
	m_pClassify		= new CWKSP_Layer_Classify;
	m_pLegend		= new CWKSP_Layer_Legend(this);
	m_pHistogram	= NULL;
}

//---------------------------------------------------------
CWKSP_Layer::~CWKSP_Layer(void)
{
	Histogram_Show(false);

	if( g_pMaps )
	{
		g_pMaps->Del(this);
	}

	if( m_pClassify )
	{
		delete(m_pClassify);
	}

	if( m_pLegend )
	{
		delete(m_pLegend);
	}

	//-----------------------------------------------------
	if( m_pObject )
	{
		if( m_pObject->is_Valid() == true )
		{
			MSG_General_Add(
  				wxString::Format("%s %s: %s...",
					LNG("[MSG] Close"),
					API_Get_DataObject_Name(m_pObject->Get_ObjectType()),
					m_pObject->Get_Name()
     			),
				true, true
			);

			delete(m_pObject);

			MSG_General_Add(LNG("[MSG] okay"), false);
		}
		else
		{
			delete(m_pObject);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Layer::On_Command(int Cmd_ID)
{
	switch( Cmd_ID )
	{
	default:
		return( CWKSP_Base_Item::On_Command(Cmd_ID) );

	case ID_CMD_SHAPES_SAVE:
	case ID_CMD_GRIDS_SAVE:
	case ID_CMD_TIN_SAVE:
		Save(m_pObject->Get_File_Path());
		break;

	case ID_CMD_SHAPES_SAVEAS:
	case ID_CMD_GRIDS_SAVEAS:
	case ID_CMD_TIN_SAVEAS:
		Save();
		break;

	case ID_CMD_WKSP_ITEM_RETURN:
	case ID_CMD_SHAPES_SHOW:
	case ID_CMD_GRIDS_SHOW:
	case ID_CMD_TIN_SHOW:
		g_pMaps->Add(this);
		break;
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Layer::On_Command_UI(wxUpdateUIEvent &event)
{
	switch( event.GetId() )
	{
	default:
		return( CWKSP_Base_Item::On_Command_UI(event) );

	case ID_CMD_TIN_SAVE:
	case ID_CMD_GRIDS_SAVE:
	case ID_CMD_SHAPES_SAVE:
		event.Enable(m_pObject->is_Modified() && m_pObject->Get_File_Path() && strlen(m_pObject->Get_File_Path()) > 0);
		break;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Layer::Create_Parameters(void)
{
	m_Parameters.Create(this, "", "");
	m_Parameters.Set_Callback_On_Parameter_Changed(&_On_Parameter_Changed);

	//-----------------------------------------------------
	m_Parameters.Add_Node(
		NULL							, "NODE_GENERAL"		, LNG("[CAP] General"),
		""
	);

	m_Parameters.Add_String(
		m_Parameters("NODE_GENERAL")	, "OBJECT_NAME"			, LNG("[CAP] Name"),
		"",
		m_pObject->Get_Name()
	);

	m_Parameters.Add_Value(
		m_Parameters("NODE_GENERAL")	, "SHOW_LEGEND"			, LNG("[CAP] Show Legend"),
		"",
		PARAMETER_TYPE_Bool, true
	);

	//-----------------------------------------------------
	m_Parameters.Add_Node(
		NULL							, "NODE_DISPLAY"		, LNG("[CAP] Display"),
		""
	);


	//-----------------------------------------------------
	// Visibility...

	m_Parameters.Add_Node(
		NULL							, "NODE_VISIBILITY"		, LNG("[CAP] Display: Visibility"),
		""
	);

	m_Parameters.Add_Value(
		m_Parameters("NODE_VISIBILITY")	, "SHOW_ALWAYS"			, LNG("[CAP] Always Show"),
		"",
		PARAMETER_TYPE_Bool, true
	);

	m_Parameters.Add_Range(
		m_Parameters("NODE_VISIBILITY")	, "SHOW_RANGE"			, LNG("[CAP] Scale Dependent"),
		"",
		0.0, 1000.0, 0.0, true
	);


	//-----------------------------------------------------
	// Classification...

	m_Parameters.Add_Node(
		NULL							, "NODE_COLORS"			, LNG("[CAP] Display: Color Classification"),
		""
	);

	m_Parameters.Add_Choice(
		m_Parameters("NODE_COLORS")		, "COLORS_TYPE"			, LNG("[CAP] Type"),
		"",
		wxString::Format("%s|%s|%s|",
			LNG("[VAL] Unique Symbol"),
			LNG("[VAL] Lookup Table"),
			LNG("[VAL] Graduated Color")
		), 0
	);


	//-----------------------------------------------------
	// Classification: Unique Value...

	m_Parameters.Add_Node(
		m_Parameters("NODE_COLORS")		, "NODE_UNISYMBOL"		, LNG("[CAP] Unique Symbol"),
		""
	);

	m_Parameters.Add_Value(
		m_Parameters("NODE_UNISYMBOL")	, "UNISYMBOL_COLOR"		, LNG("[CAP] Color"),
		"",
		PARAMETER_TYPE_Color, COLOR_GET_RGB(0, 255, 0)
	);


	//-----------------------------------------------------
	// Classification: Lookup Table...

	m_Parameters.Add_Node(
		m_Parameters("NODE_COLORS")		, "NODE_LUT"			, LNG("[CAP] Lookup Table"),
		""
	);

	CTable	LUT;
	LUT.Add_Field(LNG("COLOR")		, TABLE_FIELDTYPE_Color);
	LUT.Add_Field(LNG("NAME")			, TABLE_FIELDTYPE_String);
	LUT.Add_Field(LNG("DESCRIPTION")	, TABLE_FIELDTYPE_String);
	LUT.Add_Field(LNG("MINIMUM")		, TABLE_FIELDTYPE_Double);
	LUT.Add_Field(LNG("MAXIMUM")		, TABLE_FIELDTYPE_Double);

	m_Parameters.Add_FixedTable(
		m_Parameters("NODE_LUT")		, "LUT"					, LNG("[CAP] Table"),
		"",
		&LUT
	);


	//-----------------------------------------------------
	// Classification: Metric...

	m_Parameters.Add_Node(
		m_Parameters("NODE_COLORS")		, "NODE_METRIC"			, LNG("[CAP] Graduated Color"),
		""
	);

	m_Parameters.Add_Colors(
		m_Parameters("NODE_METRIC")		, "METRIC_COLORS"		, LNG("[CAP] Colors"),
		""
	);

	m_Parameters.Add_Range(
		m_Parameters("NODE_METRIC")		, "METRIC_ZRANGE"		, LNG("[CAP] Value Range"),
		""
	);

	m_Parameters.Add_Choice(
		m_Parameters("NODE_METRIC")		, "METRIC_SCALE_MODE"	, LNG("[CAP] Mode"),
		"",
		wxString::Format("%s|%s|%s|",
			LNG("[VAL] Linear"),
			LNG("[VAL] Logarithmic (up)"),
			LNG("[VAL] Logarithmic (down)")
		), 0
	);

	m_Parameters.Add_Value(
		m_Parameters("NODE_METRIC")		, "METRIC_SCALE_LOG"	, LNG("[CAP] Logarithmic Stretch Factor"),
		"",
		PARAMETER_TYPE_Double, 1.0
	);


	//-----------------------------------------------------
	m_pClassify->Initialise(this, m_Parameters("LUT")->asTable(), m_Parameters("METRIC_COLORS")->asColors());

	On_Create_Parameters();

	DataObject_Changed();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Layer::Save(void)
{
	bool		bResult;
	wxString	File_Path;

	switch( Get_Type() )
	{
	default:
		bResult	= false;
		break;

	case WKSP_ITEM_TIN:
	case WKSP_ITEM_Shapes:
		bResult	= DLG_Save(File_Path, ID_DLG_SHAPES_SAVE);
		break;

	case WKSP_ITEM_Grid:
		bResult	= DLG_Save(File_Path, ID_DLG_GRIDS_SAVE);
		break;
	}

	if( bResult )
	{
		bResult	= m_pObject->Save(File_Path);

		PROCESS_Set_Okay();
	}

	return( bResult );
}

//---------------------------------------------------------
bool CWKSP_Layer::Save(const char *File_Path)
{
	bool	bResult;

	if( File_Path && strlen(File_Path) > 0 )
	{
		bResult	= m_pObject->Save(File_Path);

		PROCESS_Set_Okay();

		return( bResult );
	}

	return( Save() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Layer::DataObject_Changed(CParameters *pParameters)
{
	if( pParameters )
	{
		m_Parameters.Assign_Values(pParameters);
	}

	DataObject_Changed();
}

void CWKSP_Layer::DataObject_Changed(CColors *pColors)
{
	if( m_pClassify->Get_Metric_Colors() && pColors )
	{
		m_pClassify->Get_Metric_Colors()->Assign(pColors);
	}

	DataObject_Changed();
}

void CWKSP_Layer::DataObject_Changed(void)
{
	//-----------------------------------------------------
	m_Parameters.Set_Name(wxString::Format("%02d. %s", 1 + Get_ID(), m_pObject->Get_Name()));

	m_Parameters("OBJECT_NAME")->Set_Value(m_pObject->Get_Name());

	//-----------------------------------------------------
	On_DataObject_Changed();

	g_pACTIVE->Parameters_Update(&m_Parameters, false);

	Parameters_Changed();
}

//---------------------------------------------------------
void CWKSP_Layer::Parameters_Changed(void)
{
	//-----------------------------------------------------
	m_pObject->Set_Name(m_Parameters("OBJECT_NAME")->asString());

	//-----------------------------------------------------
	m_pClassify->Set_Mode(m_Parameters("COLORS_TYPE")->asInt());

	m_pClassify->Set_Unique_Color(m_Parameters("UNISYMBOL_COLOR")->asInt());

	m_pClassify->Set_Metric(
		m_Parameters("METRIC_SCALE_MODE")	->asInt(),
		m_Parameters("METRIC_SCALE_LOG")	->asDouble(),
		m_Parameters("METRIC_ZRANGE")->asRange()->Get_LoVal() / (Get_Type() == WKSP_ITEM_Grid ? ((CGrid *)m_pObject)->Get_ZFactor() : 1.0),
		m_Parameters("METRIC_ZRANGE")->asRange()->Get_HiVal() / (Get_Type() == WKSP_ITEM_Grid ? ((CGrid *)m_pObject)->Get_ZFactor() : 1.0)
	);

	//-----------------------------------------------------
	On_Parameters_Changed();

	if( GetId().IsOk() )
	{
		Get_Control()->SetItemText(GetId(), Get_Name());
	}

	Update_Views(false);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CWKSP_Layer::_On_Parameter_Changed(CParameter *pParameter)
{
	if( pParameter && pParameter->Get_Owner() && pParameter->Get_Owner()->Get_Owner() )
	{
		return( ((CWKSP_Layer *)pParameter->Get_Owner()->Get_Owner())->
			On_Parameter_Changed(pParameter->Get_Owner(), pParameter)
		);
	}

	return( 0 );
}

//---------------------------------------------------------
int CWKSP_Layer::On_Parameter_Changed(CParameters *pParameters, CParameter *pParameter)
{
	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGEO_Rect CWKSP_Layer::Get_Extent(void)
{
	if( m_pObject )
	{
		switch( m_pObject->Get_ObjectType() )
		{
		case DATAOBJECT_TYPE_Grid:
			return( ((CGrid   *)m_pObject)->Get_Extent() );

		case DATAOBJECT_TYPE_Shapes:
			return( ((CShapes *)m_pObject)->Get_Extent() );

		case DATAOBJECT_TYPE_TIN:
			return( ((CTIN    *)m_pObject)->Get_Extent() );

		default:
			break;
		}
	}

	return( CGEO_Rect(0.0, 0.0, 0.0, 0.0) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CColors * CWKSP_Layer::Get_Colors(void)
{
	return( m_pClassify->Get_Metric_Colors() );
}

//---------------------------------------------------------
bool CWKSP_Layer::Get_Colors(CColors *pColors)
{
	if( m_pClassify->Get_Metric_Colors() && pColors )
	{
		return( pColors->Assign(m_pClassify->Get_Metric_Colors()) );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Layer::Set_Color_Range(double zMin, double zMax)
{
	CParameters	Parameters;

	Parameters.Add_Range(NULL, "METRIC_ZRANGE"	, "", "", zMin, zMax);
	DataObject_Changed(&Parameters);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Layer::do_Legend(void)
{
	return( m_Parameters("SHOW_LEGEND")->asBool() );
}

//---------------------------------------------------------
bool CWKSP_Layer::do_Show(CGEO_Rect const &rMap)
{
	double				d;
	CParameter_Range	*pRange;

	if( !m_Parameters("SHOW_ALWAYS")->asBool() )
	{
		pRange	= m_Parameters("SHOW_RANGE")->asRange();
		d		= rMap.Get_XRange() > rMap.Get_YRange() ? rMap.Get_XRange() : rMap.Get_YRange();

		return( pRange->Get_LoVal() <= d && d <= pRange->Get_HiVal() );
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Layer::Show(CWKSP_Map *pMap)
{
	if( pMap != NULL )
	{
		if( g_pMaps->Add(this, pMap) )
		{
			pMap->View_Show(true);

			return( true );
		}
	}
	else
	{
		for(int i=0; i<g_pMaps->Get_Count(); i++)
		{
			if( g_pMaps->Get_Map(i)->Find_Layer(this) != NULL )
			{
				pMap	= g_pMaps->Get_Map(i);

				pMap->View_Show(true);

				return( true );
			}
		}

		if( g_pMaps->Add(this, NULL) )
		{
			return( Show(NULL) );
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
void CWKSP_Layer::Draw(CWKSP_Map_DC &dc_Map, bool bEdit)
{
	On_Draw(dc_Map, bEdit);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Layer::Update_Views(bool bMapsOnly)
{
	g_pMaps->Update(this, bMapsOnly);

	if( !bMapsOnly )
	{
		if( Histogram_Get() )
		{
			Histogram_Get()->Update_Histogram();
		}

		On_Update_Views();
	}
}

//---------------------------------------------------------
void CWKSP_Layer::View_Closes(wxMDIChildFrame *pView)
{
	if( wxDynamicCast(pView, CVIEW_Histogram) != NULL )
	{
		m_pHistogram	= NULL;
	}
}

//---------------------------------------------------------
void CWKSP_Layer::Histogram_Show(bool bShow)
{
	if( bShow && !m_pHistogram )
	{
		m_pHistogram	= new CVIEW_Histogram(this);
	}
	else if( !bShow && m_pHistogram )
	{
		m_pHistogram->Destroy();
		delete(m_pHistogram);
	}
}

//---------------------------------------------------------
void CWKSP_Layer::Histogram_Toggle(void)
{
	Histogram_Show( m_pHistogram == NULL );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxMenu * CWKSP_Layer::Edit_Get_Menu(void)
{
	return( On_Edit_Get_Menu() );
}

//---------------------------------------------------------
TGEO_Rect CWKSP_Layer::Edit_Get_Extent(void)
{
	return( On_Edit_Get_Extent() );
}

//---------------------------------------------------------
bool CWKSP_Layer::Edit_Set_Attributes(void)
{
	return( On_Edit_Set_Attributes() );
}

//---------------------------------------------------------
bool CWKSP_Layer::Edit_On_Key_Down(int KeyCode)
{
	return( On_Edit_On_Key_Down(KeyCode) );
}

//---------------------------------------------------------
bool CWKSP_Layer::Edit_On_Mouse_Down(CGEO_Point Point, double ClientToWorld, int Key)
{
	m_Edit_Mouse_Down	= Point;

	return( On_Edit_On_Mouse_Down(Point, ClientToWorld, Key) );
}

//---------------------------------------------------------
bool CWKSP_Layer::Edit_On_Mouse_Up(CGEO_Point Point, double ClientToWorld, int Key)
{
	return( On_Edit_On_Mouse_Up(Point, ClientToWorld, Key) );
}

//---------------------------------------------------------
bool CWKSP_Layer::Edit_On_Mouse_Move(wxWindow *pMap, CGEO_Rect rWorld, wxPoint pt, wxPoint ptLast, int Key)
{
	return( On_Edit_On_Mouse_Move(pMap, rWorld, pt, ptLast, Key) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

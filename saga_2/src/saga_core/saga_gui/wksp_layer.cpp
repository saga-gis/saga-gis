
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

#include "wksp_data_manager.h"
#include "wksp_data_menu_files.h"

#include "wksp_map_manager.h"
#include "wksp_map.h"
#include "wksp_map_layer.h"

#include "wksp_data_layers.h"

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
#define DEF_LAYER_COLOUR_COUNT	15

//---------------------------------------------------------
static int	s_Def_Layer_Colours[DEF_LAYER_COLOUR_COUNT]	=
{
	SG_COLOR_RED,
	SG_COLOR_GREEN,
	SG_COLOR_BLUE,
	SG_COLOR_YELLOW,

	SG_GET_RGB(255, 127,   0),
	SG_COLOR_GREEN_LIGHT,
	SG_COLOR_BLUE_LIGHT,
	SG_GET_RGB(255, 255, 127),

	SG_COLOR_RED_DARK,
	SG_COLOR_GREEN_DARK,
	SG_COLOR_BLUE_DARK,
	SG_COLOR_YELLOW_DARK,

	SG_COLOR_BLUE_GREEN,
	SG_COLOR_PURPLE,
	SG_COLOR_PINK
};


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
CWKSP_Layer::CWKSP_Layer(CSG_Data_Object *pObject)
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
	if( g_pData->Exists(m_pObject) )
	{
		if( m_pObject->is_Valid() == true )
		{
			MSG_General_Add(
  				wxString::Format(wxT("%s %s: %s..."),
					LNG("[MSG] Close"),
					SG_Get_DataObject_Name(m_pObject->Get_ObjectType()),
					m_pObject->Get_Name()
     			),
				true, true
			);

			delete(m_pObject);

			MSG_General_Add(LNG("[MSG] okay"), false, false, SG_UI_MSG_STYLE_SUCCESS);
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
	case ID_CMD_POINTCLOUD_SAVE:
		Save(m_pObject->Get_File_Name());
		break;

	case ID_CMD_SHAPES_SAVEAS:
	case ID_CMD_GRIDS_SAVEAS:
	case ID_CMD_TIN_SAVEAS:
	case ID_CMD_POINTCLOUD_SAVEAS:
		Save();
		break;

	case ID_CMD_WKSP_ITEM_RETURN:
	case ID_CMD_SHAPES_SHOW:
	case ID_CMD_GRIDS_SHOW:
	case ID_CMD_TIN_SHOW:
	case ID_CMD_POINTCLOUD_SHOW:
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
	case ID_CMD_POINTCLOUD_SAVE:
		event.Enable(m_pObject->is_Modified() && m_pObject->Get_File_Name() && *(m_pObject->Get_File_Name()));
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
wxString CWKSP_Layer::Get_Name(void)
{
	return( wxString::Format(wxT("%04d. %s"), 1 + Get_ID(), m_pObject->Get_Name()) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Layer::Create_Parameters(void)
{
	m_Parameters.Create(this, LNG(""), LNG(""));
	m_Parameters.Set_Callback_On_Parameter_Changed(&_On_Parameter_Changed);

	//-----------------------------------------------------
	m_Parameters.Add_Node(
		NULL							, "NODE_GENERAL"		, LNG("[CAP] General"),
		LNG("")
	);

	m_Parameters.Add_String(
		m_Parameters("NODE_GENERAL")	, "OBJECT_NAME"			, LNG("[CAP] Name"),
		LNG(""),
		m_pObject->Get_Name()
	);

	m_Parameters.Add_Value(
		m_Parameters("NODE_GENERAL")	, "SHOW_LEGEND"			, LNG("[CAP] Show Legend"),
		LNG(""),
		PARAMETER_TYPE_Bool, true
	);

	m_Parameters.Add_Range(
		m_Parameters("NODE_GENERAL")	, "GENERAL_NODATA"		, LNG("[CAP] No Data"),
		LNG("")
	);


	//-----------------------------------------------------
	m_Parameters.Add_Node(
		NULL							, "NODE_DISPLAY"		, LNG("[CAP] Display"),
		LNG("")
	);


	//-----------------------------------------------------
	// Visibility...

	m_Parameters.Add_Node(
		NULL							, "NODE_VISIBILITY"		, LNG("[CAP] Display: Visibility"),
		LNG("")
	);

	m_Parameters.Add_Value(
		m_Parameters("NODE_VISIBILITY")	, "SHOW_ALWAYS"			, LNG("[CAP] Show Always"),
		LNG(""),
		PARAMETER_TYPE_Bool, true
	);

	m_Parameters.Add_Range(
		m_Parameters("NODE_VISIBILITY")	, "SHOW_RANGE"			, LNG("[CAP] Scale Dependent"),
		LNG(""),
		0.0, 1000.0, 0.0, true
	);


	//-----------------------------------------------------
	// Classification...

	m_Parameters.Add_Node(
		NULL							, "NODE_COLORS"			, LNG("[CAP] Display: Color Classification"),
		LNG("")
	);

	m_Parameters.Add_Choice(
		m_Parameters("NODE_COLORS")		, "COLORS_TYPE"			, LNG("[CAP] Type"),
		LNG(""),
		wxString::Format(wxT("%s|%s|%s|"),
			LNG("[VAL] Unique Symbol"),
			LNG("[VAL] Lookup Table"),
			LNG("[VAL] Graduated Color")
		), 0
	);


	//-----------------------------------------------------
	// Classification: Unique Value...

	m_Parameters.Add_Node(
		m_Parameters("NODE_COLORS")		, "NODE_UNISYMBOL"		, LNG("[CAP] Unique Symbol"),
		LNG("")
	);

	static	BYTE	s_Def_Layer_Colour	= 0;

	m_Parameters.Add_Value(
		m_Parameters("NODE_UNISYMBOL")	, "UNISYMBOL_COLOR"		, LNG("[CAP] Color"),
		LNG(""),
		PARAMETER_TYPE_Color, s_Def_Layer_Colours[s_Def_Layer_Colour++ % DEF_LAYER_COLOUR_COUNT]
	//	PARAMETER_TYPE_Color, SG_GET_RGB(Get_Random(128, 250), Get_Random(128, 200), Get_Random(128, 200))
	);


	//-----------------------------------------------------
	// Classification: Lookup Table...

	m_Parameters.Add_Node(
		m_Parameters("NODE_COLORS")		, "NODE_LUT"			, LNG("[CAP] Lookup Table"),
		LNG("")
	);

	CSG_Table	LUT;
	LUT.Add_Field(LNG("COLOR")		, SG_DATATYPE_Color);
	LUT.Add_Field(LNG("NAME")			, SG_DATATYPE_String);
	LUT.Add_Field(LNG("DESCRIPTION")	, SG_DATATYPE_String);
	LUT.Add_Field(LNG("MINIMUM")		, SG_DATATYPE_Double);
	LUT.Add_Field(LNG("MAXIMUM")		, SG_DATATYPE_Double);

	m_Parameters.Add_FixedTable(
		m_Parameters("NODE_LUT")		, "LUT"					, LNG("[CAP] Table"),
		LNG(""),
		&LUT
	);


	//-----------------------------------------------------
	// Classification: Metric...

	m_Parameters.Add_Node(
		m_Parameters("NODE_COLORS")		, "NODE_METRIC"			, LNG("[CAP] Graduated Color"),
		LNG("")
	);

	m_Parameters.Add_Colors(
		m_Parameters("NODE_METRIC")		, "METRIC_COLORS"		, LNG("[CAP] Colors"),
		LNG("")
	);

	m_Parameters.Add_Range(
		m_Parameters("NODE_METRIC")		, "METRIC_ZRANGE"		, LNG("[CAP] Value Range"),
		LNG("")
	);

	m_Parameters.Add_Choice(
		m_Parameters("NODE_METRIC")		, "METRIC_SCALE_MODE"	, LNG("[CAP] Mode"),
		LNG(""),
		wxString::Format(wxT("%s|%s|%s|"),
			LNG("[VAL] Linear"),
			LNG("[VAL] Logarithmic (up)"),
			LNG("[VAL] Logarithmic (down)")
		), 0
	);

	m_Parameters.Add_Value(
		m_Parameters("NODE_METRIC")		, "METRIC_SCALE_LOG"	, LNG("[CAP] Logarithmic Stretch Factor"),
		LNG(""),
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

	File_Path	= m_pObject->Get_File_Name() ? m_pObject->Get_File_Name() : m_pObject->Get_Name();

	switch( Get_Type() )
	{
	default:
		bResult	= false;
		break;

	case WKSP_ITEM_TIN:
	case WKSP_ITEM_Shapes:
		bResult	= DLG_Save(File_Path, ID_DLG_SHAPES_SAVE);
		break;

	case WKSP_ITEM_PointCloud:
		bResult	= DLG_Save(File_Path, ID_DLG_POINTCLOUD_SAVE);
		break;

	case WKSP_ITEM_Grid:
		bResult	= DLG_Save(File_Path, ID_DLG_GRIDS_SAVE);
		break;
	}

	if( bResult )
	{
		bResult	= m_pObject->Save(File_Path.c_str());

		PROCESS_Set_Okay();
	}

	return( bResult );
}

//---------------------------------------------------------
bool CWKSP_Layer::Save(const wxChar *File_Path)
{
	if( File_Path && *File_Path )
	{
		bool	bResult	= m_pObject->Save(File_Path);

		if( bResult )
		{
			g_pData->Get_FileMenus()->Recent_Add(m_pObject->Get_ObjectType(), m_pObject->Get_File_Name());
		}

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
void CWKSP_Layer::DataObject_Changed(CSG_Parameters *pParameters)
{
	if( pParameters )
	{
		m_Parameters.Assign_Values(pParameters);
	}
	else
	{
		if( m_pObject->Get_ObjectType() == DATAOBJECT_TYPE_Grid )
		{
			double		m, s, min, max;
			CSG_Grid	*pGrid	= (CSG_Grid *)m_pObject;

			switch( g_pData->Get_Parameters()->Get_Parameter("GRID_DISPLAY_RANGEFIT")->asInt() )
			{
			case 0:
				min	= pGrid->Get_ZMin(true);
				max	= pGrid->Get_ZMax(true);
				break;

			case 1:
				m	= pGrid->Get_ArithMean(true);
				s	= pGrid->Get_StdDev   (true) * 1.5;
				min	= m - s;	if( min < pGrid->Get_ZMin(true) )	min	= pGrid->Get_ZMin(true);
				max	= m + s;	if( max > pGrid->Get_ZMax(true) )	max	= pGrid->Get_ZMax(true);
				break;

			case 2:
				m	= pGrid->Get_ArithMean(true);
				s	= pGrid->Get_StdDev   (true) * 2.0;
				min	= m - s;	if( min < pGrid->Get_ZMin(true) )	min	= pGrid->Get_ZMin(true);
				max	= m + s;	if( max > pGrid->Get_ZMax(true) )	max	= pGrid->Get_ZMax(true);
			}

			m_Parameters("METRIC_ZRANGE")->asRange()->Set_Range(min, max);
		}
	}

	DataObject_Changed();
}

void CWKSP_Layer::DataObject_Changed(CSG_Colors *pColors)
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
	m_Parameters.Set_Name(wxString::Format(wxT("%02d. %s"), 1 + Get_ID(), m_pObject->Get_Name()));

	m_Parameters("OBJECT_NAME")->Set_Value(m_pObject->Get_Name());

	m_Parameters("GENERAL_NODATA")->asRange()->Set_Range(
		m_pObject->Get_NoData_Value(),
		m_pObject->Get_NoData_hiValue()
	);

	//-----------------------------------------------------
	On_DataObject_Changed();

	g_pACTIVE->Update(this, false);

	Parameters_Changed();
}

//---------------------------------------------------------
void CWKSP_Layer::Parameters_Changed(void)
{
	static bool	bUpdates	= false;

	//-----------------------------------------------------
	if( !bUpdates )
	{
		bUpdates	= true;

		m_pObject->Set_Name(m_Parameters("OBJECT_NAME")->asString());

		m_pObject->Set_NoData_Value_Range(
			m_Parameters("GENERAL_NODATA")->asRange()->Get_LoVal(),
			m_Parameters("GENERAL_NODATA")->asRange()->Get_HiVal()
		);

		//-----------------------------------------------------
		m_pClassify->Set_Mode(m_Parameters("COLORS_TYPE")->asInt());

		m_pClassify->Set_Unique_Color(m_Parameters("UNISYMBOL_COLOR")->asInt());

		m_pClassify->Set_Metric(
			m_Parameters("METRIC_SCALE_MODE")	->asInt(),
			m_Parameters("METRIC_SCALE_LOG")	->asDouble(),
			m_Parameters("METRIC_ZRANGE")->asRange()->Get_LoVal() / (Get_Type() == WKSP_ITEM_Grid ? ((CSG_Grid *)m_pObject)->Get_ZFactor() : 1.0),
			m_Parameters("METRIC_ZRANGE")->asRange()->Get_HiVal() / (Get_Type() == WKSP_ITEM_Grid ? ((CSG_Grid *)m_pObject)->Get_ZFactor() : 1.0)
		);

		//-----------------------------------------------------
		On_Parameters_Changed();

		Update_Views(false);

		_Set_Thumbnail();

		g_pData_Buttons->Refresh(false);

		//-----------------------------------------------------
		CWKSP_Base_Item::Parameters_Changed();

		bUpdates	= false;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CWKSP_Layer::_On_Parameter_Changed(CSG_Parameter *pParameter)
{
	if( pParameter && pParameter->Get_Owner() && pParameter->Get_Owner()->Get_Owner() )
	{
	//	return( ((CWKSP_Layer *)pParameter->Get_Owner()->Get_Owner())->
	//		On_Parameter_Changed(pParameter->Get_Owner(), pParameter)
	//	);

		((CWKSP_Layer *)pParameter->Get_Owner()->Get_Owner())->
			On_Parameter_Changed(pParameter->Get_Owner(), pParameter);

		g_pACTIVE->Get_Parameters()->Update_Parameters(pParameter->Get_Owner(), false);
	}

	return( 0 );
}

//---------------------------------------------------------
int CWKSP_Layer::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const wxBitmap & CWKSP_Layer::Get_Thumbnail(int dx, int dy)
{
	if( dx > 0 && m_Thumbnail.GetWidth()  != dx
	&&	dy > 0 && m_Thumbnail.GetHeight() != dy )
	{
		m_Thumbnail.Create(dx, dy);

		_Set_Thumbnail();
	}

	return( m_Thumbnail );
}

//---------------------------------------------------------
bool CWKSP_Layer::_Set_Thumbnail(void)
{
	if( m_pObject && m_Thumbnail.GetWidth() > 0 && m_Thumbnail.GetHeight() > 0 )
	{
		wxMemoryDC		dc;
		wxRect			r(0, 0, m_Thumbnail.GetWidth(), m_Thumbnail.GetHeight());
		CWKSP_Map_DC	dc_Map(Get_Extent(), r, 1.0, SG_GET_RGB(255, 255, 255));

		Draw(dc_Map, false);

		dc.SelectObject(m_Thumbnail);
		dc.SetBackground(*wxWHITE_BRUSH);
		dc.Clear();

		dc_Map.Draw(dc);

		dc.SelectObject(wxNullBitmap);

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
CSG_Rect CWKSP_Layer::Get_Extent(void)
{
	if( m_pObject )
	{
		switch( m_pObject->Get_ObjectType() )
		{
		case DATAOBJECT_TYPE_Grid:
			return( ((CSG_Grid       *)m_pObject)->Get_Extent(true) );

		case DATAOBJECT_TYPE_Shapes:
			return( ((CSG_Shapes     *)m_pObject)->Get_Extent() );

		case DATAOBJECT_TYPE_TIN:
			return( ((CSG_TIN        *)m_pObject)->Get_Extent() );

		case DATAOBJECT_TYPE_PointCloud:
			return( ((CSG_PointCloud *)m_pObject)->Get_Extent() );

		default:
			break;
		}
	}

	return( CSG_Rect(0.0, 0.0, 0.0, 0.0) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Colors * CWKSP_Layer::Get_Colors(void)
{
	return( m_pClassify->Get_Metric_Colors() );
}

//---------------------------------------------------------
bool CWKSP_Layer::Get_Colors(CSG_Colors *pColors)
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
	CSG_Parameters	Parameters;

	Parameters.Add_Range(NULL, "METRIC_ZRANGE"	, LNG(""), LNG(""), zMin, zMax);
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
bool CWKSP_Layer::do_Show(CSG_Rect const &rMap)
{
	double				d;
	CSG_Parameter_Range	*pRange;

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
TSG_Rect CWKSP_Layer::Edit_Get_Extent(void)
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
bool CWKSP_Layer::Edit_On_Mouse_Down(CSG_Point Point, double ClientToWorld, int Key)
{
	m_Edit_Mouse_Down	= Point;

	return( On_Edit_On_Mouse_Down(Point, ClientToWorld, Key) );
}

//---------------------------------------------------------
bool CWKSP_Layer::Edit_On_Mouse_Up(CSG_Point Point, double ClientToWorld, int Key)
{
	return( On_Edit_On_Mouse_Up(Point, ClientToWorld, Key) );
}

//---------------------------------------------------------
bool CWKSP_Layer::Edit_On_Mouse_Move(wxWindow *pMap, CSG_Rect rWorld, wxPoint pt, wxPoint ptLast, int Key)
{
	return( On_Edit_On_Mouse_Move(pMap, rWorld, pt, ptLast, Key) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

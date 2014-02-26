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
	return( g_pACTIVE ? g_pACTIVE->Get_Active_Layer() : NULL );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Layer::CWKSP_Layer(CSG_Data_Object *pObject)
	: CWKSP_Data_Item(pObject)
{
	m_pClassify		= new CWKSP_Layer_Classify;
	m_pLegend		= new CWKSP_Layer_Legend(this);

	m_pHistogram	= NULL;

	m_Edit_Index	= 0;
}

//---------------------------------------------------------
CWKSP_Layer::~CWKSP_Layer(void)
{
	Histogram_Show(false);

	if( g_pMaps     )	{	g_pMaps->Del(this);	}

	if( m_pClassify )	{	delete(m_pClassify);	}
	if( m_pLegend   )	{	delete(m_pLegend  );	}
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
		return( CWKSP_Data_Item::On_Command(Cmd_ID) );

	case ID_CMD_WKSP_ITEM_RETURN:
	case ID_CMD_SHAPES_SHOW:
	case ID_CMD_GRIDS_SHOW:
	case ID_CMD_TIN_SHOW:
	case ID_CMD_POINTCLOUD_SHOW:
		g_pMaps->Add(this);
		break;

	case ID_CMD_DATA_PROJECTION:
		_Set_Projection();
		break;
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Layer::On_Command_UI(wxUpdateUIEvent &event)
{
	return( CWKSP_Data_Item::On_Command_UI(event) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Layer::On_Create_Parameters(void)
{
	CWKSP_Data_Item::On_Create_Parameters();

	//-----------------------------------------------------
	// Nodes...

	m_Parameters.Add_Node(NULL, "NODE_DISPLAY"  , _TL("Display"  ), _TL(""));
	m_Parameters.Add_Node(NULL, "NODE_COLORS"   , _TL("Colors"   ), _TL(""));
	m_Parameters.Add_Node(NULL, "NODE_SIZE"     , _TL("Size"     ), _TL(""));
	m_Parameters.Add_Node(NULL, "NODE_LABEL"    , _TL("Labels"   ), _TL(""));
	m_Parameters.Add_Node(NULL, "NODE_SELECTION", _TL("Selection"), _TL(""));
	m_Parameters.Add_Node(NULL, "NODE_EDIT"     , _TL("Edit"     ), _TL(""));


	//-----------------------------------------------------
	// General...

	m_Parameters.Add_Value(
		m_Parameters("NODE_GENERAL"), "LEGEND_SHOW"   , _TL("Show Legend"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);

	m_Parameters.Add_Choice(
		m_Parameters("LEGEND_SHOW" ), "LEGEND_STYLE"  , _TL("Style"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("vertical"),
			_TL("horizontal")
		), 0
	);


	//-----------------------------------------------------
	// Display...

	m_Parameters.Add_Value(
		m_Parameters("NODE_DISPLAY"), "DISPLAY_TRANSPARENCY", _TL("Transparency [%]"),
		_TL(""),
		PARAMETER_TYPE_Double, 0.0, 0.0, true, 100.0, true
	);

	m_Parameters.Add_Value(
		m_Parameters("NODE_DISPLAY"), "SHOW_ALWAYS"         , _TL("Show at all scales"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);

	m_Parameters.Add_Range(
		m_Parameters("SHOW_ALWAYS" ), "SHOW_RANGE"			, _TL("Scale Range"),
		_TL("only show within scale range; values are given as extent measured in map units"),
		100.0, 1000.0, 0.0, true
	);


	//-----------------------------------------------------
	// Classification...

	m_Parameters.Add_Choice(
		m_Parameters("NODE_COLORS" ), "COLORS_TYPE"			, _TL("Type"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("Single Symbol"   ),	// CLASSIFY_UNIQUE
			_TL("Lookup Table"    ),	// CLASSIFY_LUT
			_TL("Discrete Colors" ),	// CLASSIFY_METRIC
			_TL("Graduated Colors") 	// CLASSIFY_GRADUATED
		), 0
	);


	//-----------------------------------------------------
	// Classification: Unique Value...

	m_Parameters.Add_Node(
		m_Parameters("NODE_COLORS"   ), "NODE_UNISYMBOL"		, _TL("Single Symbol"),
		_TL("")
	);

	static	BYTE	s_Def_Layer_Colour	= 0;

	m_Parameters.Add_Value(
		m_Parameters("NODE_UNISYMBOL"), "UNISYMBOL_COLOR"		, _TL("Color"),
		_TL(""),
		PARAMETER_TYPE_Color, s_Def_Layer_Colours[s_Def_Layer_Colour++ % DEF_LAYER_COLOUR_COUNT]
	//	PARAMETER_TYPE_Color, SG_GET_RGB(Get_Random(128, 250), Get_Random(128, 200), Get_Random(128, 200))
	);


	//-----------------------------------------------------
	// Classification: Lookup Table...

	m_Parameters.Add_Node(
		m_Parameters("NODE_COLORS")		, "NODE_LUT"			, _TL("Lookup Table"),
		_TL("")
	);

	CSG_Table	LUT;
	LUT.Add_Field(_TL("COLOR")			, SG_DATATYPE_Color);
	LUT.Add_Field(_TL("NAME")			, SG_DATATYPE_String);
	LUT.Add_Field(_TL("DESCRIPTION")	, SG_DATATYPE_String);
	LUT.Add_Field(_TL("MINIMUM")		, SG_DATATYPE_Double);
	LUT.Add_Field(_TL("MAXIMUM")		, SG_DATATYPE_Double);

	m_Parameters.Add_FixedTable(
		m_Parameters("NODE_LUT")		, "LUT"					, _TL("Table"),
		_TL(""),
		&LUT
	);


	//-----------------------------------------------------
	// Classification: Metric...

	m_Parameters.Add_Node(
		m_Parameters("NODE_COLORS")		, "NODE_METRIC"			, _TL("Scaling"),
		_TL("")
	);

	m_Parameters.Add_Colors(
		m_Parameters("NODE_METRIC")		, "METRIC_COLORS"		, _TL("Colors"),
		_TL("")
	);

	m_Parameters.Add_Range(
		m_Parameters("NODE_METRIC")		, "METRIC_ZRANGE"		, _TL("Value Range"),
		_TL("")
	);

	m_Parameters.Add_Choice(
		m_Parameters("NODE_METRIC")		, "METRIC_SCALE_MODE"	, _TL("Mode"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("Linear"),
			_TL("Logarithmic (up)"),
			_TL("Logarithmic (down)")
		), 0
	);

	m_Parameters.Add_Value(
		m_Parameters("NODE_METRIC")		, "METRIC_SCALE_LOG"	, _TL("Logarithmic Stretch Factor"),
		_TL(""),
		PARAMETER_TYPE_Double, 1.0
	);

	//-----------------------------------------------------
	m_pClassify->Initialise(this, m_Parameters("LUT")->asTable(), m_Parameters("METRIC_COLORS")->asColors());
}

//---------------------------------------------------------
int CWKSP_Layer::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter, int Flags)
{
	if( Flags & PARAMETER_CHECK_ENABLE )
	{
		if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("LEGEND_SHOW"))
		||	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("COLORS_TYPE")) )
		{
			pParameters->Get_Parameter("LEGEND_STYLE")->Set_Enabled(
				pParameters->Get_Parameter("LEGEND_SHOW")->asBool()
			&&	pParameters->Get_Parameter("COLORS_TYPE")->asInt() == CLASSIFY_GRADUATED
			);
		}

		if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("SHOW_ALWAYS")) )
		{
			pParameters->Get_Parameter("SHOW_RANGE")->Set_Enabled(pParameter->asBool() == false);
		}

		if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("COLORS_TYPE")) )
		{
			int		Value	= pParameter->asInt();

			pParameters->Get_Parameter("NODE_UNISYMBOL")->Set_Enabled(Value == CLASSIFY_UNIQUE);
			pParameters->Get_Parameter("NODE_LUT"      )->Set_Enabled(Value == CLASSIFY_LUT);
			pParameters->Get_Parameter("NODE_METRIC"   )->Set_Enabled(Value != CLASSIFY_UNIQUE && Value != CLASSIFY_LUT && Value != CLASSIFY_RGB);

			pParameters->Get_Parameter("METRIC_COLORS" )->Set_Enabled(Value == CLASSIFY_METRIC || Value == CLASSIFY_GRADUATED);
		}

		if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("METRIC_SCALE_MODE")) )
		{
			pParameters->Get_Parameter("METRIC_SCALE_LOG")->Set_Enabled(pParameter->asInt() != 0);
		}
	}

	return( CWKSP_Data_Item::On_Parameter_Changed(pParameters, pParameter, Flags) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Layer::On_Parameters_Changed(void)
{
	m_pObject->Set_Name       (m_Parameters("OBJECT_NAME")->asString());
	m_pObject->Set_Description(m_Parameters("OBJECT_DESC")->asString());

	m_pObject->Set_NoData_Value_Range(
		m_Parameters("GENERAL_NODATA")->asRange()->Get_LoVal(),
		m_Parameters("GENERAL_NODATA")->asRange()->Get_HiVal()
	);

	//-----------------------------------------------------
	m_pClassify->Initialise(this, m_Parameters("LUT")->asTable(), m_Parameters("METRIC_COLORS")->asColors());

	m_pClassify->Set_Mode(m_Parameters("COLORS_TYPE")->asInt());

	m_pClassify->Set_Unique_Color(m_Parameters("UNISYMBOL_COLOR")->asInt());

	m_pClassify->Set_Metric(
		m_Parameters("METRIC_SCALE_MODE")->asInt(),
		m_Parameters("METRIC_SCALE_LOG")->asDouble(),
		m_Parameters("METRIC_ZRANGE")->asRange()->Get_LoVal() / (Get_Type() == WKSP_ITEM_Grid ? ((CSG_Grid *)m_pObject)->Get_ZFactor() : 1.0),
		m_Parameters("METRIC_ZRANGE")->asRange()->Get_HiVal() / (Get_Type() == WKSP_ITEM_Grid ? ((CSG_Grid *)m_pObject)->Get_ZFactor() : 1.0)
	);

	m_pLegend->Set_Orientation(m_Parameters("LEGEND_STYLE")->asInt() == LEGEND_VERTICAL ? LEGEND_VERTICAL : LEGEND_HORIZONTAL);

	//-----------------------------------------------------
	CWKSP_Data_Item::On_Parameters_Changed();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
const wxBitmap & CWKSP_Layer::Get_Thumbnail(int dx, int dy)
{
	if( dx > 0 && dy > 0 && (!m_Thumbnail.IsOk() || m_Thumbnail.GetWidth() != dx || m_Thumbnail.GetHeight() != dy) )
	{
		m_Thumbnail.Create(dx, dy);

		_Set_Thumbnail(false);
	}

	return( m_Thumbnail );
}

//---------------------------------------------------------
bool CWKSP_Layer::_Set_Thumbnail(bool bRefresh)
{
	if( m_pObject && m_Thumbnail.IsOk() && m_Thumbnail.GetWidth() > 0 && m_Thumbnail.GetHeight() > 0 )
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

		if( bRefresh )
		{
			g_pData_Buttons->Refresh(false);
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
void CWKSP_Layer::_Set_Projection(void)
{
	CSG_Module	*pModule	= SG_Get_Module_Library_Manager().Get_Module(SG_T("pj_proj4"), 15);	// CCRS_Picker

	if(	pModule && Get_Object() )
	{
		CSG_Parameters	P; P.Assign(pModule->Get_Parameters());

		if( pModule->Get_Parameters()->Set_Parameter("CRS_PROJ4" , Get_Object()->Get_Projection().Get_Proj4())
		&&	DLG_Parameters(pModule->Get_Parameters())
		&&  pModule->Execute() )
		{
			Get_Object()->Get_Projection().Assign(pModule->Get_Parameters()->Get_Parameter("CRS_PROJ4")->asString(), SG_PROJ_FMT_Proj4);
		}

		pModule->Get_Parameters()->Assign_Values(&P);
	}
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
bool CWKSP_Layer::Set_Colors(CSG_Colors *pColors)
{
	if( m_pClassify->Get_Metric_Colors() && pColors )
	{
		return( m_pClassify->Get_Metric_Colors()->Assign(pColors) && DataObject_Changed() );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Layer::Set_Color_Range(double zMin, double zMax)
{
	CSG_Parameters	Parameters;

	Parameters.Add_Range(NULL, "METRIC_ZRANGE"	, _TL(""), _TL(""), zMin, zMax);

	return( DataObject_Changed(&Parameters) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Layer::do_Legend(void)
{
	return( m_Parameters("LEGEND_SHOW")->asBool() );
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
			return( Show((CWKSP_Map *)NULL) );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Layer::Show(int Flags)
{
	switch( Flags )
	{
	case SG_UI_DATAOBJECT_SHOW:
		return( Show((CWKSP_Map *)NULL) );

	case SG_UI_DATAOBJECT_SHOW_NEW_MAP:
		g_pMaps->Add(this, NULL);

	case SG_UI_DATAOBJECT_SHOW_LAST_MAP:
		return( Show(g_pMaps->Get_Map(g_pMaps->Get_Count() - 1)) );
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Layer::Update(CWKSP_Layer *pChanged)
{
	return( pChanged == this );
}

//---------------------------------------------------------
void CWKSP_Layer::On_Update_Views(bool bAll)
{
	g_pMaps->Update(this, !bAll);

	if( bAll )
	{
		_Set_Thumbnail(true);

		if( Histogram_Get() )
		{
			Histogram_Get()->Update_Histogram();
		}

		On_Update_Views();
	}
}

//---------------------------------------------------------
bool CWKSP_Layer::View_Closes(wxMDIChildFrame *pView)
{
	if( wxDynamicCast(pView, CVIEW_Histogram) != NULL )
	{
		m_pHistogram	= NULL;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

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
	return( NULL );
}

//---------------------------------------------------------
bool CWKSP_Layer::Edit_On_Key_Down(int KeyCode)
{
	return( false );
}

//---------------------------------------------------------
bool CWKSP_Layer::Edit_On_Mouse_Down(CSG_Point Point, double ClientToWorld, int Key)
{
	m_Edit_Mouse_Down	= Point;

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Layer::Edit_On_Mouse_Up(CSG_Point Point, double ClientToWorld, int Key)
{
	return( false );
}

//---------------------------------------------------------
bool CWKSP_Layer::Edit_On_Mouse_Move(wxWindow *pMap, CSG_Rect rWorld, wxPoint pt, wxPoint ptLast, int Key)
{
	return( false );
}

//---------------------------------------------------------
bool CWKSP_Layer::Edit_Set_Index(int Index)
{
	return( true );
}

//---------------------------------------------------------
int CWKSP_Layer::Edit_Get_Index(void)
{
	return( m_Edit_Index );
}

//---------------------------------------------------------
CSG_Table * CWKSP_Layer::Edit_Get_Attributes(void)
{
	return( &m_Edit_Attributes );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

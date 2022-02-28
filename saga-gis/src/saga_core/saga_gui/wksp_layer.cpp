
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
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "res_commands.h"
#include "res_dialogs.h"

#include "saga_frame.h"

#include "helper.h"

#include "active.h"
#include "active_parameters.h"

#include "wksp_base_control.h"

#include "wksp_data_manager.h"
#include "wksp_data_menu_files.h"

#include "wksp_map_manager.h"
#include "wksp_map.h"
#include "wksp_map_layer.h"

#include "wksp_data_manager.h"
#include "wksp_data_layers.h"

#include "wksp_layer.h"
#include "wksp_layer_classify.h"
#include "wksp_layer_legend.h"

#include "wksp_shapes.h"

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
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Layer *	Get_Active_Layer(void)
{
	return( g_pActive ? g_pActive->Get_Active_Layer() : NULL );
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
	if( g_pSAGA_Frame ) { g_pSAGA_Frame->Freeze(); }

	if( g_pMaps       ) { g_pMaps->Del(this);      }

	if( m_pClassify   ) { delete(m_pClassify);     }
	if( m_pLegend     ) { delete(m_pLegend  );     }

	if( g_pSAGA_Frame ) { g_pSAGA_Frame->Thaw();   }
}


///////////////////////////////////////////////////////////
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
	case ID_CMD_GRID_SHOW:
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
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Layer::On_Create_Parameters(void)
{
	CWKSP_Data_Item::On_Create_Parameters();

	//-----------------------------------------------------
	// Nodes...

	m_Parameters.Add_Node("", "NODE_DISPLAY"  , _TL("Display"  ), _TL(""));
	m_Parameters.Add_Node("", "NODE_COLORS"   , _TL("Colors"   ), _TL(""));
	m_Parameters.Add_Node("", "NODE_SIZE"     , _TL("Size"     ), _TL(""));
	m_Parameters.Add_Node("", "NODE_LABEL"    , _TL("Labels"   ), _TL(""));
	m_Parameters.Add_Node("", "NODE_SELECTION", _TL("Selection"), _TL(""));
	m_Parameters.Add_Node("", "NODE_EDIT"     , _TL("Edit"     ), _TL(""));

	//-----------------------------------------------------
	// General...

	m_Parameters.Add_Bool("NODE_GENERAL",
		"LEGEND_SHOW"   , _TL("Show Legend"),
		_TL(""),
		true
	);

	m_Parameters.Add_Choice("LEGEND_SHOW",
		"LEGEND_STYLE"	, _TL("Style"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("vertical"),
			_TL("horizontal")
		), 0
	);

	//-----------------------------------------------------
	// Display...

	m_Parameters.Add_Double("NODE_DISPLAY",
		"DISPLAY_TRANSPARENCY", _TL("Transparency [%]"),
		_TL(""),
		0., 0., true, 100., true
	);

	m_Parameters.Add_Bool("NODE_DISPLAY",
		"SHOW_ALWAYS"	, _TL("Show at all scales"),
		_TL(""),
		true
	);

	m_Parameters.Add_Range("SHOW_ALWAYS",
		"SHOW_RANGE"	, _TL("Scale Range"),
		_TL("only show within scale range; values are given as extent measured in map units"),
		100., 1000., 0., true
	);

	//-----------------------------------------------------
	if( m_pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_Shapes
	||  m_pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_PointCloud
	||  m_pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_TIN )
	{
		m_Parameters.Add_Choice("NODE_DISPLAY",
			"TABLE_FLT_STYLE"		, _TL("Floating Point Numbers"),
			_TL("Specify floating point decimal precision in table and similar views."),
			CSG_String::Format("%s|%s|%s",
				_TL("system default"),
				_TL("compact"),
				_TL("fix number of decimals")
			), g_pData->Get_Parameter("TABLE_FLT_STYLE")->asInt()
		);

		m_Parameters.Add_Int("TABLE_FLT_STYLE",
			"TABLE_FLT_DECIMALS"	, _TL("Decimals"),
			_TL(""),
			g_pData->Get_Parameter("TABLE_FLT_DECIMALS")->asInt(), 0, true
		);
	}

	//-----------------------------------------------------
	// Classification...

	if( m_pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_Shapes
	||  m_pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_TIN )
	{
		m_Parameters.Add_Choice("NODE_COLORS", "COLORS_TYPE", _TL("Type"), _TL(""), CSG_String::Format("%s|%s|%s|%s",
			_TL("Single Symbol"   ),	// CLASSIFY_SINGLE
			_TL("Classified"      ),	// CLASSIFY_LUT
			_TL("Discrete Colors" ),	// CLASSIFY_DISCRETE
			_TL("Graduated Colors") 	// CLASSIFY_GRADUATED
		), 0);
	}
	else if( m_pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_PointCloud )
	{
		m_Parameters.Add_Choice("NODE_COLORS", "COLORS_TYPE", _TL("Type"), _TL(""), CSG_String::Format("%s|%s|%s|%s|%s",
			_TL("Single Symbol"   ),	// CLASSIFY_SINGLE
			_TL("Classified"      ),	// CLASSIFY_LUT
			_TL("Discrete Colors" ),	// CLASSIFY_DISCRETE
			_TL("Graduated Colors"),	// CLASSIFY_GRADUATED
			_TL("RGB Coded Values")		// CLASSIFY_OVERLAY !!!
		), 0);
	}
	else if( m_pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_Grids )
	{
		m_Parameters.Add_Choice("NODE_COLORS", "COLORS_TYPE", _TL("Type"), _TL(""), CSG_String::Format("%s|%s|%s|%s|%s",
			_TL("Single Symbol"   ),	// CLASSIFY_SINGLE
			_TL("Classified"      ),	// CLASSIFY_LUT
			_TL("Discrete Colors" ),	// CLASSIFY_DISCRETE
			_TL("Graduated Colors"),	// CLASSIFY_GRADUATED
			_TL("RGB Composite"   )		// CLASSIFY_OVERLAY
		), 4);

		m_Parameters.Add_Choice("NODE_COLORS", "BAND"       , _TL("Band"      ), _TL(""), _TL("<default>"));
		m_Parameters.Add_Choice("NODE_COLORS", "OVERLAY_FIT", _TL("Statistics"), _TL(""), CSG_String::Format("%s|%s", _TL("all bands"), _TL("each band")), 1);
		m_Parameters.Add_Choice("NODE_COLORS", "BAND_R"     , _TL("Red"       ), _TL(""), "");
		m_Parameters.Add_Choice("NODE_COLORS", "BAND_G"     , _TL("Green"     ), _TL(""), "");
		m_Parameters.Add_Choice("NODE_COLORS", "BAND_B"     , _TL("Blue"      ), _TL(""), "");
	}
	else if( m_pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_Grid )
	{
		m_Parameters.Add_Choice("NODE_COLORS", "COLORS_TYPE", _TL("Type"), _TL(""), CSG_String::Format("%s|%s|%s|%s|%s|%s|%s",
			_TL("Single Symbol"   ),	// CLASSIFY_SINGLE
			_TL("Classified"      ),	// CLASSIFY_LUT
			_TL("Discrete Colors" ),	// CLASSIFY_DISCRETE
			_TL("Graduated Colors"),	// CLASSIFY_GRADUATED
			_TL("RGB Composite"   ),	// CLASSIFY_OVERLAY
			_TL("RGB Coded Values"),	// CLASSIFY_RGB
			_TL("Shade"           )		// CLASSIFY_SHADE
		), 3);
	}

	//-----------------------------------------------------
	// Classification: Single Symbol...

	static	BYTE	s_Def_Layer_Colour	= 0;

	m_Parameters.Add_Node("NODE_COLORS",
		"NODE_SINGLE"	, _TL("Single Symbol"),
		_TL("")
	);

	m_Parameters.Add_Color("NODE_SINGLE",
		"SINGLE_COLOR"	, _TL("Color"),
		_TL(""),
		s_Def_Layer_Colours[s_Def_Layer_Colour++ % DEF_LAYER_COLOUR_COUNT]
	);

	//-----------------------------------------------------
	// Classification: Classified...

	m_Parameters.Add_Node("NODE_COLORS",
		"NODE_LUT"			, _TL("Classified"),
		_TL("")
	);

	if( m_pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_Shapes
	||  m_pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_PointCloud
	||  m_pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_TIN )
	{
		m_Parameters.Add_Choice("NODE_LUT", "LUT_ATTRIB", _TL("Attribute"), _TL(""), _TL("<default>"));
	}

	CSG_Table	*pLUT	= m_Parameters.Add_FixedTable("NODE_LUT",
		"LUT"				, _TL("Table"),
		_TL("")
	)->asTable();

	pLUT->Get_MetaData().Add_Child("SAGA_GUI_LUT_TYPE", m_pObject->Get_ObjectType());

	pLUT->Add_Field(_TL("Color"      ), SG_DATATYPE_Color );
	pLUT->Add_Field(_TL("Name"       ), SG_DATATYPE_String);
	pLUT->Add_Field(_TL("Description"), SG_DATATYPE_String);
	pLUT->Add_Field(_TL("Minimum"    ), SG_DATATYPE_Double);
	pLUT->Add_Field(_TL("Maximum"    ), SG_DATATYPE_Double);

	m_pClassify->Initialise(this, pLUT, g_pData->Get_Parameter("COLORS_DEFAULT")->asColors());

	//-----------------------------------------------------
	// Classification: Colors...

	ColorsParms_Add();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Layer::ColorsParms_Add(void)
{
	m_Parameters.Add_Node("NODE_COLORS",
		"NODE_METRIC"		, _TL("Histogram Stretch"),
		_TL("")
	);

	if( m_pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_Shapes )
	{
		m_Parameters.Add_Choice("NODE_METRIC", "METRIC_ATTRIB", _TL("Attribute"), _TL(""), _TL("<default>"));
		m_Parameters.Add_Choice("NODE_METRIC", "METRIC_NORMAL", _TL("Normalize"), _TL(""), _TL("<default>"));

		m_Parameters.Add_Choice("METRIC_NORMAL", "METRIC_NORFMT", _TL("Labeling"), _TL(""),
			CSG_String::Format("%s|%s", _TL("fraction"), _TL("percentage")), 1
		);
	}
	else if( m_pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_PointCloud
		||   m_pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_TIN )
	{
		m_Parameters.Add_Choice("NODE_METRIC", "METRIC_ATTRIB", _TL("Attribute"), _TL(""), _TL("<default>"));
	}

	//-----------------------------------------------------
	m_Parameters.Add_Colors("NODE_METRIC",
		"METRIC_COLORS"	, _TL("Colors"),
		_TL(""),
		g_pData->Get_Parameter("COLORS_DEFAULT")->asColors()
	);

	m_Parameters.Add_Range("NODE_METRIC",
		"METRIC_ZRANGE"		, _TL("Range"),
		_TL("")
	);

	if( m_pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_Grids )
	{
		m_Parameters.Add_Range("NODE_METRIC", "METRIC_ZRANGE_R", _TL("Red"  ), _TL(""));
		m_Parameters.Add_Range("NODE_METRIC", "METRIC_ZRANGE_G", _TL("Green"), _TL(""));
		m_Parameters.Add_Range("NODE_METRIC", "METRIC_ZRANGE_B", _TL("Blue" ), _TL(""));
	}

	//-----------------------------------------------------
	if( m_pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_Grid
	||  m_pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_Grids )
	{
		m_Parameters.Add_Bool("", "STRETCH_UPDATE", "update flag, for internal use only", "", true)->Set_Enabled(false);

		m_Parameters.Add_Choice("NODE_METRIC",
			"STRETCH_DEFAULT"	, _TL("Adjustment"),
			_TL("Specify how to adjust histogram stretch."),
			CSG_String::Format("%s|%s|%s|%s",
				_TL("Linear"),
				_TL("Standard Deviation"),
				_TL("Percent Clip"),
				_TL("Manual")
			), g_pData->Get_Parameter("GRID_STRETCH_DEFAULT")->asInt()
		);

		m_Parameters.Add_Range("STRETCH_DEFAULT",
			"STRETCH_LINEAR"	, _TL("Linear Percent Stretch"),
			_TL("Linear percent stretch allows you to trim extreme values from both ends of the histogram using the percentage specified here."),
			5., 95., 0., true, 100., true
		);

		m_Parameters.Add_Double("STRETCH_DEFAULT",
			"STRETCH_STDDEV"	, _TL("Standard Deviation"),
			_TL(""),
			2., 0., true
		);

		m_Parameters.Add_Bool("STRETCH_STDDEV",
			"STRETCH_INRANGE"	, _TL("Keep in Range"),
			_TL("Prevents that minimum or maximum stretch value fall outside the data value range."),
			true
		);

		m_Parameters.Add_Range("STRETCH_DEFAULT",
			"STRETCH_PCTL"		, _TL("Percent Clip"),
			_TL(""),
			2., 98., 0., true, 100., true
		);
	}

	//-----------------------------------------------------
	if( m_pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_PointCloud )
	{
		m_Parameters.Add_Choice("NODE_METRIC",
			"STRETCH_DEFAULT"	, _TL("Adjustment"),
			_TL("Specify how to adjust histogram stretch."),
			CSG_String::Format("%s|%s|%s",
				_TL("Linear"),
				_TL("Standard Deviation"),
				_TL("Manual")
			), 1
		);

		m_Parameters.Add_Range("STRETCH_DEFAULT",
			"STRETCH_LINEAR"	, _TL("Linear Percent Stretch"),
			_TL("Linear percent stretch allows you to trim extreme values from both ends of the histogram using the percentage specified here."),
			5., 95., 0., true, 100., true
		);

		m_Parameters.Add_Double("STRETCH_DEFAULT",
			"STRETCH_STDDEV"	, _TL("Standard Deviation"),
			_TL(""),
			2., 0., true
		);

		m_Parameters.Add_Bool("STRETCH_STDDEV",
			"STRETCH_INRANGE"	, _TL("Keep in Range"),
			_TL("Prevents that minimum or maximum stretch value fall outside the data value range."),
			true
		);
	}

	//-----------------------------------------------------
	m_Parameters.Add_Choice("NODE_METRIC",
		"METRIC_SCALE_MODE"	, _TL("Scaling"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("linear intervals"),
			_TL("increasing geometrical intervals"),
			_TL("decreasing geometrical intervals")
		), 0
	);

	m_Parameters.Add_Double("METRIC_SCALE_MODE",
		"METRIC_SCALE_LOG"	, _TL("Geometrical Interval Factor"),
		_TL(""),
		10.
	);
}

//---------------------------------------------------------
void CWKSP_Layer::ColorsParms_On_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter, int Flags)
{
	//-----------------------------------------------------
	if( Flags & PARAMETER_CHECK_VALUES )
	{
		if( pParameter->Cmp_Identifier("METRIC_ZRANGE"  )
		||  pParameter->Cmp_Identifier("METRIC_ZRANGE_R")
		||  pParameter->Cmp_Identifier("METRIC_ZRANGE_G")
		||  pParameter->Cmp_Identifier("METRIC_ZRANGE_B") )
		{
			pParameters->Set_Parameter("STRETCH_DEFAULT", 3);	// manual
		}
	}

	//-----------------------------------------------------
	if( Flags & PARAMETER_CHECK_ENABLE )
	{
		if(	pParameter->Cmp_Identifier("METRIC_SCALE_MODE") )
		{
			pParameters->Set_Enabled("METRIC_SCALE_LOG", pParameter->asInt() != 0);
		}

		CSG_Parameter	*pStretch	= (*pParameters)("STRETCH_DEFAULT");

		if(	pStretch )
		{
			pParameters->Set_Enabled("STRETCH_LINEAR", pStretch->asInt() == 0);
			pParameters->Set_Enabled("STRETCH_STDDEV", pStretch->asInt() == 1);
			pParameters->Set_Enabled("STRETCH_PCTL"  , pStretch->asInt() == 2);
		}
	}
}

//---------------------------------------------------------
bool CWKSP_Layer::ColorsParms_Adjust(CSG_Parameters &Parameters, CSG_Data_Object *pObject, const CSG_String &Suffix)
{
	if( !pObject )
	{
		pObject	= Get_Object();

		if( pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_Grids && Parameters("COLORS_TYPE")->asInt() != CLASSIFY_OVERLAY )
		{
			int	i	= Parameters("BAND") ? Parameters("BAND")->asInt() : -1;

			if( i < 0 || i >= ((CSG_Grids *)pObject)->Get_NZ() || !(pObject = ((CSG_Grids *)pObject)->Get_Grid_Ptr(i)) )
			{
				return( false );
			}
		}
	}

	int	Field	= -1;

	if( pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_Shapes
	||  pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_PointCloud
	||  pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_TIN )
	{
		if( !Parameters("METRIC_ATTRIB") || (Field = Parameters("METRIC_ATTRIB")->asInt()) < 0 || Field >= ((CSG_Shapes *)pObject)->Get_Field_Count() )
		{
			return( false );
		}

		if( pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_TIN )
		{	// currently no support for histogram stretch options with shapes and tin...
			Parameters.Set_Parameter("METRIC_ZRANGE" + Suffix + ".MIN", ((CSG_TIN *)pObject)->Get_Minimum(Field));
			Parameters.Set_Parameter("METRIC_ZRANGE" + Suffix + ".MAX", ((CSG_TIN *)pObject)->Get_Maximum(Field));

			return( true );
		}

		if( pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_Shapes )
		{	// currently no support for histogram stretch options with shapes and tin...
			CWKSP_Shapes *pShapes = (CWKSP_Shapes *)this;

			pShapes->Set_Metrics(
				Parameters("METRIC_ATTRIB")->asInt(),
				Parameters("METRIC_NORMAL")->asInt(),
				Parameters("METRIC_NORFMT")->asInt()
			);

			Parameters.Set_Parameter("METRIC_ZRANGE" + Suffix + ".MIN", pShapes->Get_Value_Minimum());
			Parameters.Set_Parameter("METRIC_ZRANGE" + Suffix + ".MAX", pShapes->Get_Value_Maximum());

			return( true );
		}
	}

	double	Minimum, Maximum;

	switch( Parameters("STRETCH_DEFAULT")->asInt() )
	{
	case 0: {
		//-------------------------------------------------
		switch( pObject->Get_ObjectType() )
		{
		case SG_DATAOBJECT_TYPE_Grid:
			Minimum	= pObject->asGrid ()->Get_Min();
			Maximum	= pObject->asGrid ()->Get_Max();
			break;

		case SG_DATAOBJECT_TYPE_Grids:
			Minimum	= pObject->asGrids()->Get_Min();
			Maximum	= pObject->asGrids()->Get_Max();
			break;

		case SG_DATAOBJECT_TYPE_Shapes:
		case SG_DATAOBJECT_TYPE_PointCloud:
		case SG_DATAOBJECT_TYPE_TIN:
			Minimum	= ((CSG_Shapes *)pObject)->Get_Minimum(Field);
			Maximum	= ((CSG_Shapes *)pObject)->Get_Maximum(Field);
			break;

		default:
			return( false );
		}

		if( Parameters("STRETCH_DEFAULT")->asInt() == 0 )
		{
			double Range = (Maximum - Minimum) / 100.;

			Maximum	= Minimum + Range * Parameters("STRETCH_LINEAR.MAX")->asDouble();
			Minimum	= Minimum + Range * Parameters("STRETCH_LINEAR.MIN")->asDouble();
		}
		break;	}

	//-----------------------------------------------------
	case  1: {
		double	d	= Parameters("STRETCH_STDDEV")->asDouble(), Mean;

		switch( pObject->Get_ObjectType() )
		{
		case SG_DATAOBJECT_TYPE_Grid:
			Minimum	= pObject->asGrid ()->Get_Min   ();
			Maximum	= pObject->asGrid ()->Get_Max   ();
			Mean	= pObject->asGrid ()->Get_Mean  ();
			d	   *= pObject->asGrid ()->Get_StdDev();
			break;

		case SG_DATAOBJECT_TYPE_Grids:
			Minimum	= pObject->asGrids()->Get_Min   ();
			Maximum	= pObject->asGrids()->Get_Max   ();
			Mean	= pObject->asGrids()->Get_Mean  ();
			d	   *= pObject->asGrids()->Get_StdDev();
			break;

		case SG_DATAOBJECT_TYPE_Shapes:
		case SG_DATAOBJECT_TYPE_PointCloud:
		case SG_DATAOBJECT_TYPE_TIN:
			Minimum	= ((CSG_Shapes *)pObject)->Get_Minimum(Field);
			Maximum	= ((CSG_Shapes *)pObject)->Get_Maximum(Field);
			Mean	= ((CSG_Shapes *)pObject)->Get_Mean   (Field);
			d	   *= ((CSG_Shapes *)pObject)->Get_StdDev (Field);
			break;

		default:
			return( false );
		}

		if( !Parameters("STRETCH_INRANGE")->asBool() || Minimum < Mean - d ) Minimum = Mean - d;
		if( !Parameters("STRETCH_INRANGE")->asBool() || Maximum > Mean + d ) Maximum = Mean + d;

		break;	}

	//-----------------------------------------------------
	case  2: {
		switch( pObject->Get_ObjectType() )
		{
		case SG_DATAOBJECT_TYPE_Grid:
			Minimum	= ((CSG_Grid  *)pObject)->Get_Percentile(Parameters("STRETCH_PCTL.MIN")->asDouble());
			Maximum	= ((CSG_Grid  *)pObject)->Get_Percentile(Parameters("STRETCH_PCTL.MAX")->asDouble());
			break;

		case SG_DATAOBJECT_TYPE_Grids:
			Minimum	= ((CSG_Grids *)pObject)->Get_Percentile(Parameters("STRETCH_PCTL.MIN")->asDouble());
			Maximum	= ((CSG_Grids *)pObject)->Get_Percentile(Parameters("STRETCH_PCTL.MAX")->asDouble());
			break;

		default:
			return( false );
		}
		break;	}

	//-----------------------------------------------------
	default:
		return( false );
	}

	Parameters.Set_Parameter("METRIC_ZRANGE" + Suffix + ".MIN", Minimum);
	Parameters.Set_Parameter("METRIC_ZRANGE" + Suffix + ".MAX", Maximum);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Layer::On_DataObject_Changed(void)
{
	ColorsParms_Adjust(m_Parameters);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CWKSP_Layer::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter, int Flags)
{
	//-----------------------------------------------------
	if( Flags & PARAMETER_CHECK_VALUES )
	{
		if( pParameter->Cmp_Identifier("STRETCH_DEFAULT")
		||  pParameter->Cmp_Identifier("STRETCH_LINEAR" )
		||  pParameter->Cmp_Identifier("STRETCH_STDDEV" )
		||  pParameter->Cmp_Identifier("STRETCH_INRANGE")
		||  pParameter->Cmp_Identifier("STRETCH_PCTL"   )
		||  pParameter->Cmp_Identifier("BAND"           ) )
		{
			ColorsParms_Adjust(*pParameters);
		}
	}

	//-----------------------------------------------------
	if( Flags & PARAMETER_CHECK_ENABLE )
	{
		if(	pParameter->Cmp_Identifier("LEGEND_SHOW")
		||	pParameter->Cmp_Identifier("COLORS_TYPE") )
		{
			pParameters->Set_Enabled("LEGEND_STYLE",
					(*pParameters)("LEGEND_SHOW")->asBool()
				&&	(*pParameters)("COLORS_TYPE")->asInt() == CLASSIFY_GRADUATED
			);
		}

		if(	pParameter->Cmp_Identifier("SHOW_ALWAYS") )
		{
			pParameters->Set_Enabled("SHOW_RANGE", pParameter->asBool() == false);
		}

		if( pParameter->Cmp_Identifier("TABLE_FLT_STYLE") )
		{
			pParameters->Set_Enabled("TABLE_FLT_DECIMALS", pParameter->asInt() == 2);
		}

		if(	pParameter->Cmp_Identifier("COLORS_TYPE") )
		{
			int		Value	= pParameter->asInt();

			pParameters->Set_Enabled("METRIC_COLORS" , Value == CLASSIFY_DISCRETE || Value == CLASSIFY_GRADUATED);
			pParameters->Set_Enabled("NODE_SINGLE", Value == CLASSIFY_SINGLE);
			pParameters->Set_Enabled("NODE_LUT"      , Value == CLASSIFY_LUT);
		
			if( m_pObject->Get_ObjectType() != SG_DATAOBJECT_TYPE_Grids )
			{
				pParameters->Set_Enabled("NODE_METRIC", Value != CLASSIFY_SINGLE && Value != CLASSIFY_LUT && Value != CLASSIFY_RGB);
			}
		}
	}

	//-----------------------------------------------------
	ColorsParms_On_Changed(pParameters, pParameter, Flags);

	return( CWKSP_Data_Item::On_Parameter_Changed(pParameters, pParameter, Flags) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Layer::On_Parameters_Changed(void)
{
	//-----------------------------------------------------
	m_pClassify->Initialise(this, m_Parameters("LUT")->asTable(), m_Parameters("METRIC_COLORS")->asColors());

	m_pClassify->Set_Mode(m_Parameters("COLORS_TYPE")->asInt());

	m_pClassify->Set_Unique_Color(m_Parameters("SINGLE_COLOR")->asInt());

	m_pClassify->Set_Metric(
		m_Parameters("METRIC_SCALE_MODE")->asInt   (),
		m_Parameters("METRIC_SCALE_LOG" )->asDouble(),
		m_Parameters("METRIC_ZRANGE.MIN")->asDouble(),
		m_Parameters("METRIC_ZRANGE.MAX")->asDouble()
	);

	m_pLegend->Set_Orientation(m_Parameters("LEGEND_STYLE")->asInt() == LEGEND_VERTICAL ? LEGEND_VERTICAL : LEGEND_HORIZONTAL);

	//-----------------------------------------------------
	CWKSP_Data_Item::On_Parameters_Changed();
}


///////////////////////////////////////////////////////////
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
		CWKSP_Map_DC	dc_Map(Get_Extent(), r, 1., SG_GET_RGB(255, 255, 255));

		Draw(dc_Map, LAYER_DRAW_FLAG_NOEDITS|LAYER_DRAW_FLAG_NOLABELS|LAYER_DRAW_FLAG_THUMBNAIL);

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
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Rect CWKSP_Layer::Get_Extent(void)
{
	if( m_pObject )
	{
		switch( m_pObject->Get_ObjectType() )
		{
		case SG_DATAOBJECT_TYPE_Grid:
			return( ((CSG_Grid       *)m_pObject)->Get_Extent(true) );

		case SG_DATAOBJECT_TYPE_Grids:
			return( ((CSG_Grids      *)m_pObject)->Get_Extent(true) );

		case SG_DATAOBJECT_TYPE_Shapes:
			return( ((CSG_Shapes     *)m_pObject)->Get_Extent() );

		case SG_DATAOBJECT_TYPE_TIN:
			return( ((CSG_TIN        *)m_pObject)->Get_Extent() );

		case SG_DATAOBJECT_TYPE_PointCloud:
			return( ((CSG_PointCloud *)m_pObject)->Get_Extent() );

		default:
			break;
		}
	}

	return( CSG_Rect(0., 0., 0., 0.) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Layer::_Set_Projection(void)
{
	CSG_Tool	*pTool	= SG_Get_Tool_Library_Manager().Create_Tool("pj_proj4", 15, true);	// CCRS_Picker

	CSG_Projection	Projection(Get_Object()->Get_Projection());

	if(	pTool
	&&  pTool->Set_Parameter("CRS_EPSG"     , Projection.Get_Authority_ID())
	&&  pTool->Set_Parameter("CRS_EPSG_AUTH", Projection.Get_Authority   ())
	&&  pTool->Set_Parameter("CRS_PROJ4"    , Projection.Get_Proj4       ())
	&&	pTool->On_Before_Execution() && DLG_Parameters(pTool->Get_Parameters()) )
	{
		Projection.Destroy();

		if( pTool->Get_Parameter("CRS_EPSG")->asInt() > 0 )
		{
			Projection.Create(
				pTool->Get_Parameter("CRS_EPSG"     )->asInt   (),
				pTool->Get_Parameter("CRS_EPSG_AUTH")->asString()
			);
		}
		
		if( !Projection.is_Okay() )
		{
			Projection.Create(
				pTool->Get_Parameter("CRS_PROJ4")->asString(), SG_PROJ_FMT_Proj4
			);
		}

		if( Projection.is_Okay() && !Projection.is_Equal(Get_Object()->Get_Projection()) )
		{
			Get_Object()->Get_Projection().Create(Projection);
			Get_Object()->Set_Modified();

			DataObject_Changed();
		}
	}

	SG_Get_Tool_Library_Manager().Delete_Tool(pTool);
}


///////////////////////////////////////////////////////////
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
	if( m_pClassify->Get_Metric_Colors() && pColors && m_pClassify->Get_Metric_Colors()->Assign(pColors) )
	{
		Parameters_Changed();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Layer::Set_Color_Range(double Minimum, double Maximum)
{
	m_Parameters.Set_Parameter("STRETCH_DEFAULT"  , 3);	// manual
	m_Parameters.Set_Parameter("METRIC_ZRANGE.MIN", Minimum < Maximum ? Minimum : Maximum);
	m_Parameters.Set_Parameter("METRIC_ZRANGE.MAX", Minimum < Maximum ? Maximum : Minimum);

	Parameters_Changed();

	return( true );

//	return( DataObject_Changed() );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Layer::do_Legend(void)
{
	return( m_Parameters("LEGEND_SHOW")->asBool() );
}

//---------------------------------------------------------
bool CWKSP_Layer::do_Show(CSG_Rect const &Map_Extent, bool bIntersects)
{
	if( bIntersects && !Map_Extent.Intersects(Get_Extent()) && !Map_Extent.Intersects(Edit_Get_Extent()) )
	{
		return( false );
	}

	if( !m_Parameters("SHOW_ALWAYS")->asBool() )
	{
		double	d	= Map_Extent.Get_XRange() > Map_Extent.Get_YRange()
			? Map_Extent.Get_XRange()
			: Map_Extent.Get_YRange();

		return( m_Parameters("SHOW_RANGE.MIN")->asDouble() <= d
			&&  m_Parameters("SHOW_RANGE.MAX")->asDouble() >= d 
		);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Layer::Draw(CWKSP_Map_DC &dc_Map, int Flags, CSG_Data_Object *pObject)
{
	if( pObject && pObject->is_Valid() && pObject->Get_ObjectType() == m_pObject->Get_ObjectType() )
	{
		CSG_Data_Object	*pOriginal	= m_pObject;

		m_pObject	= pObject;

		On_Draw(dc_Map, Flags);

		m_pObject	= pOriginal;
	}
	else
	{
		On_Draw(dc_Map, Flags);
	}

	dc_Map.dc.SetBrush(wxNullBrush);
	dc_Map.dc.SetPen  (wxNullPen  );

	return( true );
}


///////////////////////////////////////////////////////////
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
			if( g_pMaps->Get_Map(i)->Get_Map_Layer(this) != NULL )
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
	_Set_Thumbnail(true);

	g_pMaps->Update(this, !bAll);

	if( bAll )
	{
		On_Update_Views();
	}
}

//---------------------------------------------------------
bool CWKSP_Layer::View_Closes(MDI_ChildFrame *pView)
{
	if( pView == m_pHistogram )
	{
		m_pHistogram	= NULL;
	}

	return( CWKSP_Data_Item::View_Closes(pView) );
}


///////////////////////////////////////////////////////////
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

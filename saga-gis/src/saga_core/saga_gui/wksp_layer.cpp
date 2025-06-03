
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

#include "view_map.h"
#include "view_histogram.h"

//---------------------------------------------------------
#include <saga_gdi/sgdi_classify.h>

#define CLASSIFY_UNIQUE    0
#define CLASSIFY_EQUAL     1
#define CLASSIFY_DEFINED   2
#define CLASSIFY_QUANTILE  3
#define CLASSIFY_GEOMETRIC 4
#define CLASSIFY_NATURAL   5
#define CLASSIFY_STDDEV    6
#define CLASSIFY_MANUAL    7


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define DEF_LAYER_COLOUR_COUNT	10

//---------------------------------------------------------
static int	s_Def_Layer_Colours[DEF_LAYER_COLOUR_COUNT]	=
{
	SG_COLOR_BLUE,
	SG_COLOR_GREEN,
	SG_COLOR_RED,
	SG_COLOR_YELLOW_DARK,
	SG_COLOR_BLUE_DARK,
	SG_COLOR_GREEN_DARK,
	SG_COLOR_RED_DARK,
	SG_COLOR_BLUE_GREEN,
	SG_COLOR_PURPLE,
	SG_COLOR_PINK
};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Layer *	Get_Active_Layer(void)
{
	return( g_pActive ? g_pActive->Get_Active_Layer() : NULL );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Layer::CWKSP_Layer(CSG_Data_Object *pObject)
	: CWKSP_Data_Item(pObject)
{
	m_pClassify  = new CWKSP_Layer_Classify;
	m_pLegend    = new CWKSP_Layer_Legend(this);

	m_pHistogram = NULL;
}

//---------------------------------------------------------
CWKSP_Layer::~CWKSP_Layer(void)
{
	MDI_Freeze();
	if( g_pMaps     ) { g_pMaps->Del(this);  }
	if( m_pClassify ) { delete(m_pClassify); }
	if( m_pLegend   ) { delete(m_pLegend  ); }
	MDI_Thaw();
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Layer::Add_ToolBar_Defaults(class wxToolBarBase *pToolBar)
{
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_WKSP_ITEM_CLOSE);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_DATA_SAVEAS);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_DATA_RELOAD);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_DATA_FORCE_UPDATE);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_DATA_PROJECTION);
	pToolBar->AddSeparator();
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_DATA_SHOW_MAP);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_DATA_CLASSIFY);
	CMD_ToolBar_Add_Item(pToolBar, true , ID_CMD_DATA_HISTOGRAM);

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Layer::On_Command(int Cmd_ID)
{
	switch( Cmd_ID )
	{
	default: return( CWKSP_Data_Item::On_Command(Cmd_ID) );

	//-----------------------------------------------------
	case ID_CMD_WKSP_ITEM_RETURN:
	case ID_CMD_DATA_SHOW_MAP   : g_pMaps->Add(this); break;

	case ID_CMD_DATA_PROJECTION : _Set_Projection (); break;

	case ID_CMD_DATA_CLASSIFY   : _Classify       (); break;

	case ID_CMD_DATA_HISTOGRAM  : Histogram_Toggle(); break;
	case ID_CMD_DATA_SCATTERPLOT: Add_ScatterPlot (); break;
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Layer::On_Command_UI(wxUpdateUIEvent &event)
{
	switch( event.GetId() )
	{
	default: return( CWKSP_Data_Item::On_Command_UI(event) );

	//-----------------------------------------------------
	case ID_CMD_DATA_HISTOGRAM  : event.Check(m_pHistogram != NULL); event.Enable(m_Parameters["COLORS_TYPE"].asInt() > 0); break;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
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
	m_Parameters.Add_Node("", "NODE_SELECTION", _TL("Selection"), _TL(""), true);
	m_Parameters.Add_Node("", "NODE_EDIT"     , _TL("Edit"     ), _TL(""), true);

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
			_TL("Single Symbol"   ), // CLASSIFY_SINGLE
			_TL("Classification"  ), // CLASSIFY_LUT
			_TL("Discrete Colors" ), // CLASSIFY_DISCRETE
			_TL("Graduated Colors")  // CLASSIFY_GRADUATED
		), 0);
	}
	else if( m_pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_PointCloud )
	{
		m_Parameters.Add_Choice("NODE_COLORS", "COLORS_TYPE", _TL("Type"), _TL(""), CSG_String::Format("%s|%s|%s|%s|%s",
			_TL("Single Symbol"   ), // CLASSIFY_SINGLE
			_TL("Classification"  ), // CLASSIFY_LUT
			_TL("Discrete Colors" ), // CLASSIFY_DISCRETE
			_TL("Graduated Colors"), // CLASSIFY_GRADUATED
			_TL("RGB Coded Values")  // CLASSIFY_OVERLAY !!!
		), 0);
	}
	else if( m_pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_Grids )
	{
		m_Parameters.Add_Choice("NODE_COLORS", "COLORS_TYPE", _TL("Type"), _TL(""), CSG_String::Format("%s|%s|%s|%s|%s",
			_TL("Single Symbol"   ), // CLASSIFY_SINGLE
			_TL("Classification"  ), // CLASSIFY_LUT
			_TL("Discrete Colors" ), // CLASSIFY_DISCRETE
			_TL("Graduated Colors"), // CLASSIFY_GRADUATED
			_TL("RGB Composite"   )  // CLASSIFY_OVERLAY
		), 4);

		m_Parameters.Add_Choice("NODE_COLORS", "BAND"       , _TL("Band"      ), _TL(""), "<not set>");
		m_Parameters.Add_Choice("NODE_COLORS", "OVERLAY_FIT", _TL("Statistics"), _TL(""), CSG_String::Format("%s|%s", _TL("all bands"), _TL("each band")), 1);
		m_Parameters.Add_Choice("NODE_COLORS", "BAND_R"     , _TL("Red"       ), _TL(""), "");
		m_Parameters.Add_Choice("NODE_COLORS", "BAND_G"     , _TL("Green"     ), _TL(""), "");
		m_Parameters.Add_Choice("NODE_COLORS", "BAND_B"     , _TL("Blue"      ), _TL(""), "");
	}
	else if( m_pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_Grid )
	{
		m_Parameters.Add_Choice("NODE_COLORS", "COLORS_TYPE", _TL("Type"), _TL(""), CSG_String::Format("%s|%s|%s|%s|%s|%s|%s",
			_TL("Single Symbol"   ), // CLASSIFY_SINGLE
			_TL("Classification"  ), // CLASSIFY_LUT
			_TL("Discrete Colors" ), // CLASSIFY_DISCRETE
			_TL("Graduated Colors"), // CLASSIFY_GRADUATED
			_TL("RGB Composite"   ), // CLASSIFY_OVERLAY
			_TL("RGB Coded Values"), // CLASSIFY_RGB
			_TL("Shade"           )  // CLASSIFY_SHADE
		), 3);
	}

	//-----------------------------------------------------
	// Classification: Single Symbol...

	static BYTE s_Def_Layer_Colour = 0;

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

	m_Parameters.Add_Node("NODE_COLORS", "NODE_LUT", _TL("Classification"), _TL(""));

	if( m_pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_Shapes
	||  m_pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_PointCloud
	||  m_pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_TIN )
	{
		m_Parameters.Add_Choice("NODE_LUT", "LUT_FIELD" , _TL("Field"        ), _TL(""), "<not set>");
		m_Parameters.Add_Choice("NODE_LUT", "LUT_NORMAL", _TL("Normalization"), _TL(""), "<not set>");
	}

	CSG_Table *pLUT = m_Parameters.Add_FixedTable("NODE_LUT", "LUT", _TL("Table"), _TL(""))->asTable();

	pLUT->Get_MetaData().Add_Child("SAGA_GUI_LUT_TYPE", m_pObject->Get_ObjectType());

	pLUT->Add_Field(_TL("Color"      ), SG_DATATYPE_Color );
	pLUT->Add_Field(_TL("Name"       ), SG_DATATYPE_String);
	pLUT->Add_Field(_TL("Description"), SG_DATATYPE_String);
	pLUT->Add_Field(_TL("Minimum"    ), SG_DATATYPE_Double);
	pLUT->Add_Field(_TL("Maximum"    ), SG_DATATYPE_Double);

	m_pClassify->Initialise(this, pLUT, g_pData->Get_Parameter("COLORS_DEFAULT")->asColors());

	if( m_pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_Grid
	||  m_pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_Grids )
	{
		m_Parameters.Add_Choice("NODE_LUT", "LUT_RESAMPLING", _TL("Resampling"), _TL(""),
			CSG_String::Format("%s|%s|%s|%s",
				_TL("Nearest Neighbour"),
				_TL("Bilinear Interpolation"),
				_TL("Bicubic Spline Interpolation"),
				_TL("B-Spline Interpolation")
			), 0
		);
	}

	//-----------------------------------------------------
	// Classification: Stretched...

	m_Parameters.Add_Node("NODE_COLORS",
		"NODE_METRIC", _TL("Histogram Stretch"),
		_TL("")
	);

	if( m_pObject->asTable(true) )
	{
		m_Parameters.Add_Choice("NODE_METRIC", "METRIC_FIELD" , _TL("Field"        ), _TL(""), "<not set>");
		m_Parameters.Add_Choice("NODE_METRIC", "METRIC_NORMAL", _TL("Normalization"), _TL(""), "<not set>");

		m_Parameters.Add_Choice("METRIC_NORMAL", "METRIC_NORFMT", _TL("Labeling"), _TL(""),
			CSG_String::Format("%s|%s", _TL("fraction"), _TL("percentage")), 0
		);
	}

	if( m_pObject->asGrids() )
	{
		m_Parameters.Add_Range("NODE_METRIC", "METRIC_ZRANGE_R", _TL("Red"  ), _TL(""));
		m_Parameters.Add_Range("NODE_METRIC", "METRIC_ZRANGE_G", _TL("Green"), _TL(""));
		m_Parameters.Add_Range("NODE_METRIC", "METRIC_ZRANGE_B", _TL("Blue" ), _TL(""));
	}

	//-----------------------------------------------------
	m_Parameters.Add_Colors("NODE_METRIC",
		"METRIC_COLORS", _TL("Colors"),
		_TL(""),
		g_pData->Get_Parameter("COLORS_DEFAULT")->asColors()
	);

	//-----------------------------------------------------
	m_Parameters.Add_Bool("", "STRETCH_UPDATE", "update flag, for internal use only", "", true)->Set_Enabled(false);

	m_Parameters.Add_Choice("NODE_METRIC",
		"STRETCH_DEFAULT"	, _TL("Adjustment"),
		_TL("Specify how to adjust histogram stretch."),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("Linear"),
			_TL("Standard Deviation"),
			_TL("Percent Clip"),
			_TL("Manual")
		), g_pData->Get_Parameter("STRETCH_DEFAULT")->asInt()
	);

	m_Parameters.Add_Range("STRETCH_DEFAULT",
		"STRETCH_LINEAR"	, _TL("Linear Percent Stretch"),
		_TL("Linear percent stretch allows you to trim extreme values from both ends of the histogram using the percentage specified here."),
		g_pData->Get_Parameter("STRETCH_LINEAR.MIN")->asDouble(),
		g_pData->Get_Parameter("STRETCH_LINEAR.MAX")->asDouble(), 0., true, 100., true
	);

	m_Parameters.Add_Double("STRETCH_DEFAULT",
		"STRETCH_STDDEV"	, _TL("Standard Deviation"),
		_TL(""),
		g_pData->Get_Parameter("STRETCH_STDDEV")->asDouble(), 0., true
	);

	m_Parameters.Add_Bool("STRETCH_STDDEV",
		"STRETCH_INRANGE"	, _TL("Keep in Range"),
		_TL("Prevents that minimum or maximum stretch value fall outside the data value range."),
		false
	);

	m_Parameters.Add_Range("STRETCH_DEFAULT",
		"STRETCH_PCTL"		, _TL("Percent Clip"),
		_TL(""),
		g_pData->Get_Parameter("STRETCH_PCTL.MIN")->asDouble(),
		g_pData->Get_Parameter("STRETCH_PCTL.MAX")->asDouble(), 0., true, 100., true
	);

	//-----------------------------------------------------
	m_Parameters.Add_Range("NODE_METRIC",
		"METRIC_ZRANGE"     , _TL("Range"),
		_TL("")
	);

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


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Layer::Set_Stretch(CSG_Parameters &Parameters, CSG_Data_Object *pObject, const CSG_String &Suffix)
{
	if( Parameters["STRETCH_DEFAULT"].asInt() == 3 ) // keep current stretch if set to 'manual'!
	{
		if( Get_Stretch_Range() > 0. )
		{
			return( false );
		}

		Parameters["STRETCH_DEFAULT"].Set_Value(1); // standard deviation
	}

	if( !pObject )
	{
		pObject	= m_pObject;
	}

	//-----------------------------------------------------
	int Field = -1; CSG_Table *pTable = _Get_Field_Table(Field, Parameters);

	if( pTable && Field < 0 )
	{
		return( false );
	}

	if( pObject->asGrids() && Parameters["COLORS_TYPE"].asInt() != CLASSIFY_OVERLAY )
	{
		int i = Parameters("BAND") ? Parameters["BAND"].asInt() : -1;

		if( i < 0 || i >= pObject->asGrids()->Get_NZ() || !(pObject = pObject->asGrids()->Get_Grid_Ptr(i)) )
		{
			return( false );
		}
	}

	//-----------------------------------------------------
    SG_UI_Process_Set_Busy(true);

	double Minimum, Maximum;

	switch( Parameters["STRETCH_DEFAULT"].asInt() )
	{
	case  0: // linear
		{
			CSG_Simple_Statistics Statistics = pTable ? pTable->Get_Statistics(Field) : pObject->asGrid() ? pObject->asGrid()->Get_Statistics() : pObject->asGrids()->Get_Statistics();

			Minimum = Statistics.Get_Minimum() + Statistics.Get_Range() * Parameters["STRETCH_LINEAR.MIN"].asDouble() / 100.;
			Maximum = Statistics.Get_Minimum() + Statistics.Get_Range() * Parameters["STRETCH_LINEAR.MAX"].asDouble() / 100.;
		}
		break;

	case  1: // standard deviation
		{
			CSG_Simple_Statistics Statistics = pTable ? pTable->Get_Statistics(Field) : pObject->asGrid() ? pObject->asGrid()->Get_Statistics() : pObject->asGrids()->Get_Statistics();

			Minimum = Statistics.Get_Mean() - Statistics.Get_StdDev() * Parameters["STRETCH_STDDEV"].asDouble();
			Maximum = Statistics.Get_Mean() + Statistics.Get_StdDev() * Parameters["STRETCH_STDDEV"].asDouble();

			if( Parameters["STRETCH_INRANGE"].asBool() )
			{
				if( Minimum < Statistics.Get_Minimum() ) { Minimum = Statistics.Get_Minimum(); }
				if( Maximum > Statistics.Get_Maximum() ) { Maximum = Statistics.Get_Maximum(); }
			}
		}
		break;

	case  2: // percentile
		{
			const CSG_Histogram &Histogram = pTable ? pTable->Get_Histogram(Field) : pObject->asGrid() ? pObject->asGrid()->Get_Histogram() : pObject->asGrids()->Get_Histogram();

			Minimum = Histogram.Get_Percentile(Parameters["STRETCH_PCTL.MIN"].asDouble());
			Maximum	= Histogram.Get_Percentile(Parameters["STRETCH_PCTL.MAX"].asDouble());
		}
		break;

	default: SG_UI_Process_Set_Busy(false); return( false );
	}

    SG_UI_Process_Set_Busy(false);

	Parameters.Set_Parameter("METRIC_ZRANGE" + Suffix + ".MIN", Minimum);
	Parameters.Set_Parameter("METRIC_ZRANGE" + Suffix + ".MAX", Maximum);

	return( true );
}

//---------------------------------------------------------
double CWKSP_Layer::Get_Stretch_Minimum(void) const
{
	return( m_Parameters["METRIC_ZRANGE.MIN"].asDouble() );
}

//---------------------------------------------------------
double CWKSP_Layer::Get_Stretch_Maximum(void) const
{
	return( m_Parameters["METRIC_ZRANGE.MAX"].asDouble() );
}

//---------------------------------------------------------
double CWKSP_Layer::Get_Stretch_Range(void) const
{
	return( Get_Stretch_Maximum() - Get_Stretch_Minimum() );
}

//---------------------------------------------------------
bool CWKSP_Layer::Set_Stretch_Range(double Minimum, double Maximum)
{
	if( m_Parameters["COLORS_TYPE"].asInt() != CLASSIFY_DISCRETE || m_Parameters["COLORS_TYPE"].asInt() != CLASSIFY_GRADUATED )
	{
		m_Parameters.Set_Parameter("COLORS_TYPE", CLASSIFY_GRADUATED);
	}

	m_Parameters.Set_Parameter("STRETCH_DEFAULT", _TL("Manual"));

	m_Parameters.Set_Parameter("METRIC_ZRANGE.MIN", Minimum < Maximum ? Minimum : Maximum);
	m_Parameters.Set_Parameter("METRIC_ZRANGE.MAX", Minimum < Maximum ? Maximum : Minimum);

	Parameters_Changed();

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Layer::Set_Stretch_FullRange(void)
{
	int Field = -1; CSG_Table *pTable = _Get_Field_Table(Field, m_Parameters);

	if( pTable )
	{
		return( Field < 0 ? false : Set_Stretch_Range(pTable->Get_Minimum(Field), pTable->Get_Maximum(Field)) );
	}

	CSG_Data_Object *pObject = m_pObject;

	if( pObject->asGrids() && m_Parameters["COLORS_TYPE"].asInt() != CLASSIFY_OVERLAY )
	{
		int Band = m_Parameters("BAND") ? m_Parameters["BAND"].asInt() : -1;

		if( Band < 0 || Band >= pObject->asGrids()->Get_NZ() || !(pObject = pObject->asGrids()->Get_Grid_Ptr(Band)) )
		{
			return( false );
		}
	}

	if( pObject->asGrid () ) { return( Set_Stretch_Range(pObject->asGrid ()->Get_Min(), pObject->asGrid ()->Get_Max()) ); }
	if( pObject->asGrids() ) { return( Set_Stretch_Range(pObject->asGrids()->Get_Min(), pObject->asGrids()->Get_Max()) ); }

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CWKSP_Layer::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter, int Flags)
{
	if( Flags & PARAMETER_CHECK_VALUES )
	{
		if( m_pObject->asTable(true) )
		{
			if(	pParameter->Cmp_Identifier("COLORS_TYPE"  )
			||	pParameter->Cmp_Identifier("METRIC_FIELD" )
			||	pParameter->Cmp_Identifier("METRIC_NORMAL")
			||	pParameter->Cmp_Identifier("METRIC_NORFMT") )
			{
				if(	pParameter->Cmp_Identifier("METRIC_FIELD" )
				||	pParameter->Cmp_Identifier("METRIC_NORMAL")
				||	pParameter->Cmp_Identifier("METRIC_NORFMT") )
				{
					m_Normalization.Destroy(); // invalidate, might need updates

					if( (*pParameters)("STRETCH_DEFAULT")->asInt() == 3 ) // manual? no adjustment!
					{
						pParameters->Set_Parameter("STRETCH_DEFAULT", 1); // standard deviation!
					}
				}

				if( (*pParameters)("COLORS_TYPE")->asInt() == 2 || (*pParameters)("COLORS_TYPE")->asInt() == 3 ) // discrete/graduated
				{
					Set_Stretch(*pParameters);
				}
			}

			if(	pParameter->Cmp_Identifier("LUT_FIELD") || (pParameter->Cmp_Identifier("COLORS_TYPE") && pParameter->asInt() == 1) ) // CLASSIFY_LUT
			{
				int Field = (*pParameters)("LUT_FIELD")->asInt();

				if(	Field >= 0 && Field < m_pObject->asTable(true)->Get_Field_Count() )
				{
					TSG_Data_Type Type = m_pObject->asTable(true)->is_Field_Numeric(Field)
						? SG_DATATYPE_Double : SG_DATATYPE_String;
			
					(*pParameters)("LUT")->asTable()->Set_Field_Type(LUT_MIN, Type);
					(*pParameters)("LUT")->asTable()->Set_Field_Type(LUT_MAX, Type);
				}
			}
		}

		if( pParameter->Cmp_Identifier("STRETCH_DEFAULT")
		||  pParameter->Cmp_Identifier("STRETCH_LINEAR" )
		||  pParameter->Cmp_Identifier("STRETCH_STDDEV" )
		||  pParameter->Cmp_Identifier("STRETCH_INRANGE")
		||  pParameter->Cmp_Identifier("STRETCH_PCTL"   )
		||  pParameter->Cmp_Identifier("BAND"           ) )
		{
			Set_Stretch(*pParameters);
		}

		if( pParameter->Cmp_Identifier("METRIC_ZRANGE"  )
		||  pParameter->Cmp_Identifier("METRIC_ZRANGE_R")
		||  pParameter->Cmp_Identifier("METRIC_ZRANGE_G")
		||  pParameter->Cmp_Identifier("METRIC_ZRANGE_B") )
		{
			pParameters->Set_Parameter("STRETCH_DEFAULT", 3); // manual
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
				&& (*pParameters)("COLORS_TYPE")->asInt() == CLASSIFY_GRADUATED
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
			int Value = pParameter->asInt();

			pParameters->Set_Enabled("NODE_SINGLE"    , Value == CLASSIFY_SINGLE);
			pParameters->Set_Enabled("NODE_LUT"       , Value == CLASSIFY_LUT);

			pParameters->Set_Enabled("NODE_METRIC"    , Value != CLASSIFY_SINGLE && Value != CLASSIFY_LUT);
			pParameters->Set_Enabled("METRIC_COLORS"  , Value == CLASSIFY_DISCRETE || Value == CLASSIFY_GRADUATED);

			if( m_pObject->Get_ObjectType() == SG_DATAOBJECT_TYPE_PointCloud )
			{
				pParameters->Set_Enabled("NODE_RGB"	  , Value == 4); // RGB Coded Values
			}

			if( m_pObject->Get_ObjectType() != SG_DATAOBJECT_TYPE_Grids )
			{
				pParameters->Set_Enabled("NODE_METRIC", Value != CLASSIFY_SINGLE && Value != CLASSIFY_LUT && Value != CLASSIFY_RGB);
			}
		}

		if(	pParameter->Cmp_Identifier("METRIC_FIELD") )
		{
			pParameters->Set_Enabled("METRIC_NORMAL", Get_Fields_Choice(pParameter) >= 0);
		}

		if( pParameter->Cmp_Identifier("METRIC_NORMAL") )
		{
			pParameter->Set_Children_Enabled(Get_Fields_Choice(pParameter) >= 0);
		}

		if(	pParameter->Cmp_Identifier("METRIC_SCALE_MODE") )
		{
			pParameters->Set_Enabled("METRIC_SCALE_LOG", pParameter->asInt() != 0);
		}

		CSG_Parameter *pStretch = (*pParameters)("STRETCH_DEFAULT");

		if(	pStretch )
		{
			pParameters->Set_Enabled("STRETCH_LINEAR", pStretch->asInt() == 0);
			pParameters->Set_Enabled("STRETCH_STDDEV", pStretch->asInt() == 1);
			pParameters->Set_Enabled("STRETCH_PCTL"  , pStretch->asInt() == 2);
		}
	}

	//-----------------------------------------------------
	return( CWKSP_Data_Item::On_Parameter_Changed(pParameters, pParameter, Flags) );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Layer::On_DataObject_Changed(void)
{
	m_Normalization.Destroy(); // invalidate

	if( m_Parameters["COLORS_TYPE"].asInt() == 2 || m_Parameters["COLORS_TYPE"].asInt() == 3 ) // discrete/graduated
	{
		Set_Stretch(m_Parameters);
	}

	//-----------------------------------------------------
	CWKSP_Data_Item::On_DataObject_Changed();
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Layer::On_Parameters_Changed(void)
{
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
//                                                       //
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
		CSG_Map_DC dc_Map(Get_Extent(), m_Thumbnail.GetSize(), 1., Get_Color_asInt(SYS_Get_Color(wxSYS_COLOUR_WINDOW)));

		if( Draw(dc_Map, LAYER_DRAW_FLAG_NOEDITS|LAYER_DRAW_FLAG_NOLABELS|LAYER_DRAW_FLAG_THUMBNAIL) )
		{
			wxBitmap Bitmap;

			if( dc_Map.Get_Bitmap(Bitmap) )
			{
				wxMemoryDC dc;

				dc.SelectObject(m_Thumbnail);
				dc.DrawBitmap(Bitmap, 0, 0, false);
				dc.SelectObject(wxNullBitmap);

				if( bRefresh && g_pData_Buttons )
				{
					g_pData_Buttons->Refresh(false);
				}

				return( true );
			}
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Rect CWKSP_Layer::Get_Extent(void)
{
	if( m_pObject )
	{
		switch( m_pObject->Get_ObjectType() )
		{
		case SG_DATAOBJECT_TYPE_Grid      : return( ((CSG_Grid       *)m_pObject)->Get_Extent(true) );
		case SG_DATAOBJECT_TYPE_Grids     : return( ((CSG_Grids      *)m_pObject)->Get_Extent(true) );
		case SG_DATAOBJECT_TYPE_Shapes    : return( ((CSG_Shapes     *)m_pObject)->Get_Extent    () );
		case SG_DATAOBJECT_TYPE_TIN       : return( ((CSG_TIN        *)m_pObject)->Get_Extent    () );
		case SG_DATAOBJECT_TYPE_PointCloud: return( ((CSG_PointCloud *)m_pObject)->Get_Extent    () );
		default                           : break;
		}
	}

	return( CSG_Rect(0., 0., 0., 0.) );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Layer::_Set_Projection(void)
{
	CSG_Projection &CRS = Get_Object()->Get_Projection();

	CSG_Tool *pTool = SG_Get_Tool_Library_Manager().Create_Tool("pj_proj4", 15, true); // CCRS_Picker

	if(	pTool
	&&  pTool->Set_Parameter("CRS_WKT" , CRS.Get_WKT2())
	&&  pTool->Set_Parameter("CRS_PROJ", CRS.Get_PROJ())
	&&	pTool->On_Before_Execution() && DLG_Parameters(pTool->Get_Parameters()) )
	{
		CSG_Projection new_CRS(pTool->Get_Parameter("CRS_WKT")->asString(), pTool->Get_Parameter("CRS_PROJ")->asString());

		if( new_CRS.is_Okay() && CRS.Get_WKT2().CmpNoCase(new_CRS.Get_WKT2()) )
		{
			CRS.Create(new_CRS); Get_Object()->Set_Modified();

			DataObject_Changed();
		}
	}

	SG_Get_Tool_Library_Manager().Delete_Tool(pTool);
}


///////////////////////////////////////////////////////////
//                                                       //
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


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Layer::Set_Normalization(int Field_Value, int Field_Normalize, double Scale_Normalize, sLong maxSamples)
{
	m_Normalization.Destroy();

	CSG_Table *pTable = m_pObject->asTable(true);

	if( !pTable || Field_Value < 0 || Field_Normalize < 0 || Field_Value >= pTable->Get_Field_Count() || Field_Normalize >= pTable->Get_Field_Count() )
	{
		return( false );
	}

	m_Normalization.Add_Field("Normalized", SG_DATATYPE_Double);

	for(sLong i=0; i<pTable->Get_Count(); i++)
	{
		double Value, Normalize;

		if( pTable->Get_Value(i, Field_Normalize, Normalize) && Normalize != 0. && pTable->Get_Value(i, Field_Value, Value) )
		{
			m_Normalization.Add_Record()->Set_Value(0, Scale_Normalize * Value / Normalize);
		}
	}

	return( m_Normalization.Get_Count() );
}

//---------------------------------------------------------
CSG_Table * CWKSP_Layer::_Get_Field_Table(int &Field, const CSG_Parameters &Parameters)
{
	CSG_Table *pTable = m_pObject->asTable(true); Field = -1;

	if( pTable && Parameters("METRIC_FIELD") && Parameters("METRIC_NORMAL") && Parameters("METRIC_NORFMT") )
	{
		Field = Get_Fields_Choice(Parameters("METRIC_FIELD")); if( Field >= pTable->Get_Field_Count() ) { Field = -1; }
		
		if( Field >= 0 )
		{
			if( m_Normalization.Get_Count() || Set_Normalization(Field, Get_Fields_Choice(Parameters("METRIC_NORMAL")), Parameters("METRIC_NORFMT")->asInt() == 0 ? 1. : 100.) )
			{
				pTable = &m_Normalization; Field = 0;
			}
		}
	}

	return( pTable );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_Layer::Get_Field_Naming(void) const
{
	wxString s;

	CSG_Table *pTable = m_pObject->asTable(true);

	if( pTable )
	{
		int Value = -1, Normal = -1;

		switch( m_Parameters["COLORS_TYPE"].asInt() )
		{
		case  1: // CLASSIFY_LUT
			Value  = Get_Fields_Choice(m_Parameters("LUT_FIELD"    ));
			Normal = Get_Fields_Choice(m_Parameters("LUT_NORMAL"   ));
			break;

		case  2: // CLASSIFY_DISCRETE
		case  3: // CLASSIFY_GRADUATED
			Value  = Get_Fields_Choice(m_Parameters("METRIC_FIELD" ));
			Normal = Get_Fields_Choice(m_Parameters("METRIC_NORMAL"));
			break;

		case  4: // CLASSIFY_RGB
			Value  = Get_Fields_Choice(m_Parameters("RGB_FIELD"    )); // point clouds only!
			break;
		}

		if( Value >= 0 )
		{
			s = pTable->Get_Field_Name(Value);

			if( Normal >= 0 )
			{
				s += " / "; s += pTable->Get_Field_Name(Normal);
			}
		}
	}

	return( s );
}

//---------------------------------------------------------
bool CWKSP_Layer::Get_Field_Value(sLong Index, int Field, int Normalize, double Scale, wxString &Value) const
{
	CSG_Table *pTable = m_pObject->asTable(true);

	if( Index >= 0 && Index < pTable->Get_Count() )
	{
		if( Field < 0 || Field >= pTable->Get_Field_Count() )
		{
			Value.Printf("%s: %lld", _TL("Index"), Index + 1);

			return( true );
		}

		//-------------------------------------------------
		if( m_pClassify->Get_Mode() == CLASSIFY_LUT )
		{
			if( !SG_Data_Type_is_Numeric(pTable->Get_Field_Type(Field)) )
			{
				CSG_String s;

				if( pTable->Get_Value(Index, Field, s) )
				{
					Value = m_pClassify->Get_Class_Name_byValue(s.c_str());

					return( true );
				}
			}
			else
			{
				double d;

				if( Get_Field_Value(Index, Field, Normalize, Scale, d) )
				{
					Value = m_pClassify->Get_Class_Name_byValue(d);

					return( true );
				}
			}
		}

		//-------------------------------------------------
		else if( m_pClassify->Get_Mode() == CLASSIFY_RGB )
		{
			double d;

			if( pTable->Get_Value(Index, Field, d) )
			{
				Value.Printf("R%03d G%03d B%03d", SG_GET_R((int)d), SG_GET_G((int)d), SG_GET_B((int)d));

				return( true );
			}
		}

		//-------------------------------------------------
		else // CLASSIFY_DISCRETE || CLASSIFY_GRADUATED
		{
			double d;

			if( Get_Field_Value(Index, Field, Normalize, Scale, d) )
			{
				Value.Printf("%f", d);

				return( true );
			}
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Layer::Get_Field_Value(sLong Index, int Field, int Normalize, double Scale, double &Value) const
{
	CSG_Table *pTable = m_pObject->asTable(true);

	if( Index >= 0 && Index < pTable->Get_Count() && Field >= 0 && Field < pTable->Get_Field_Count() )
	{
		if( pTable->Get_Value(Index, Field, Value) )
		{
			if( Normalize < 0 )
			{
				return( true );
			}

			double Divisor;

			if( pTable->Get_Value(Index, Normalize, Divisor) && Divisor != 0. )
			{
				Value *= Scale / Divisor;

				return( true );
			}
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Layer::Set_Fields_Choice(CSG_Parameter *pChoice, bool bNumeric, bool bAddNone, bool bSelectNone)
{
	if( pChoice && pChoice->asChoice() )
	{
		CSG_Table *pTable = m_pObject->asTable(true);

		if( pTable )
		{
			CSG_String Fields;

			for(int i=0; i<pTable->Get_Field_Count(); i++)
			{
				if( !bNumeric || SG_Data_Type_is_Numeric(pTable->Get_Field_Type(i)) )
				{
					Fields += CSG_String::Format("{%d}%s|", i, pTable->Get_Field_Name(i));
				}
			}

			if( !Fields.is_Empty() )
			{
				if( bAddNone )
				{
					Fields += CSG_String::Format("{-1}<%s>", _TL("none"));

					if( pChoice->asChoice()->Get_Count() <= 1 || Get_Fields_Choice(pChoice) < 0 )
					{
						bSelectNone = true;
					}
				}

				pChoice->asChoice()->Set_Items(Fields);

				if( bAddNone && bSelectNone )
				{
					pChoice->Set_Value(pChoice->asChoice()->Get_Count() - 1);
				}

				return( true );
			}
		}

		pChoice->asChoice()->Set_Items(CSG_String::Format("{-1}<%s>", _TL("none")));
	}

	return( false );
}

//---------------------------------------------------------
int CWKSP_Layer::Get_Fields_Choice(CSG_Parameter *pChoice)
{
	int Field; return( pChoice && pChoice->asChoice() && pChoice->asChoice()->Get_Data(Field) ? Field : -1 );
}


///////////////////////////////////////////////////////////
//                                                       //
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
		double d = Map_Extent.Get_XRange() > Map_Extent.Get_YRange()
			? Map_Extent.Get_XRange()
			: Map_Extent.Get_YRange();

		return( m_Parameters("SHOW_RANGE.MIN")->asDouble() <= d
		    &&  m_Parameters("SHOW_RANGE.MAX")->asDouble() >= d
		);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Layer::Draw(CSG_Map_DC &dc_Map, int Flags, CSG_Data_Object *pObject)
{
	if( pObject && pObject->is_Valid() && pObject->Get_ObjectType() == m_pObject->Get_ObjectType() )
	{
		CSG_Data_Object	*pOriginal = m_pObject;

		m_pObject = pObject;

		On_Draw(dc_Map, Flags);

		m_pObject = pOriginal;
	}
	else
	{
		On_Draw(dc_Map, Flags);
	}

	dc_Map.SetBrush(wxNullBrush);
	dc_Map.SetPen  (wxNullPen  );

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
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

		return( false );
	}

	//-----------------------------------------------------
	for(int i=0; i<g_pMaps->Get_Count(); i++) // find first map that includes this layer
	{
		if( g_pMaps->Get_Map(i)->Get_Map_Layer(this) != NULL )
		{
			g_pMaps->Get_Map(i)->View_Show(true);

			return( true );
		}
	}

	return( g_pMaps->Add(this, NULL) && Show((CWKSP_Map *)NULL) ); // not found? add to new map and show
}

//---------------------------------------------------------
bool CWKSP_Layer::Show(int Flags)
{
	switch( Flags )
	{
	case SG_UI_DATAOBJECT_SHOW_MAP:
		return( Show((CWKSP_Map *)NULL) );

	case SG_UI_DATAOBJECT_SHOW_MAP_NEW:
		g_pMaps->Add(this, NULL);

	case SG_UI_DATAOBJECT_SHOW_MAP_LAST:
		return( Show(g_pMaps->Get_Map(g_pMaps->Get_Count() - 1)) );

	case SG_UI_DATAOBJECT_SHOW_MAP_ACTIVE:
		return( Show(MDI_Get_Active_Map() ? ((CVIEW_Map *)MDI_Get_Active_Map())->Get_Map() : NULL) );
	}

	return( false );
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
		m_pHistogram = NULL;
	}

	return( CWKSP_Data_Item::View_Closes(pView) );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Layer::Histogram_Show(bool bShow)
{
	if( bShow && !m_pHistogram )
	{
		m_pHistogram = new CVIEW_Histogram(this);
	}
	else if( !bShow && m_pHistogram )
	{
		m_pHistogram->Destroy();
	}
}

//---------------------------------------------------------
void CWKSP_Layer::Histogram_Toggle(void)
{
	Histogram_Show(m_pHistogram == NULL);
}


///////////////////////////////////////////////////////////
//                                                       //
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
bool CWKSP_Layer::Edit_On_Mouse_Down(const CSG_Point &Point, double ClientToWorld, int Key)
{
	m_Edit_Mouse_Down	= Point;

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Layer::Edit_On_Mouse_Up(const CSG_Point &Point, double ClientToWorld, int Key)
{
	return( false );
}

//---------------------------------------------------------
bool CWKSP_Layer::Edit_On_Mouse_Move(wxWindow *pMap, const CSG_Rect &rWorld, const wxPoint &Point, const wxPoint &Last, int Key)
{
	return( false );
}

//---------------------------------------------------------
bool CWKSP_Layer::Edit_On_Mouse_Move_Draw(wxDC &dc, const CSG_Rect &rWorld, const wxPoint &Point)
{
	return( true );
}

//---------------------------------------------------------
bool CWKSP_Layer::Edit_Do_Mouse_Move_Draw(bool bMouseDown)
{
	return( false );
}

//---------------------------------------------------------
bool CWKSP_Layer::Edit_Set_Index(int Index)
{
	return( true );
}

//---------------------------------------------------------
CSG_Table * CWKSP_Layer::Edit_Get_Attributes(void)
{
	return( &m_Edit_Attributes );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CWKSP_Layer::_Classify_Callback(CSG_Parameter *pParameter, int Flags)
{
	CSG_Parameters *pParameters = pParameter ? pParameter->Get_Parameters() : NULL;

	//-----------------------------------------------------
	if( Flags & PARAMETER_CHECK_VALUES )
	{
		if( pParameter->Cmp_Identifier("NUM_FIELDS") )
		{
			int Field = Get_Fields_Choice(pParameter); CSG_Table *pTable = (*pParameters)("TABLE")->asTable();

			if( pTable )
			{
				pParameters->Set_Parameter("INTERVAL"    , pTable->Get_Range  (Field) / (*pParameters)("CLASSES")->asInt());
				pParameters->Set_Parameter("OFFSET_VALUE", pTable->Get_Minimum(Field));
			}
		}
	}

	//-----------------------------------------------------
	if( Flags & PARAMETER_CHECK_ENABLE )
	{
		if( pParameter->Cmp_Identifier("METHOD") )
		{
			pParameters->Set_Enabled("ALL_FIELDS"  , pParameter->asInt() == 0);
			pParameters->Set_Enabled("NUM_FIELDS"  , pParameter->asInt() != 0);
			pParameters->Set_Enabled("NUM_NORMAL"  , pParameter->asInt() != 0);
			pParameters->Set_Enabled("CLASSES_MAX" , pParameter->asInt() == 0);
			pParameters->Set_Enabled("CLASSES"     , pParameter->asInt() != 0 && pParameter->asInt() != 2 && pParameter->asInt() != 6);
			pParameters->Set_Enabled("INTERVAL"    , pParameter->asInt() == 2);
			pParameters->Set_Enabled("OFFSET"      , pParameter->asInt() == 2);
			pParameters->Set_Enabled("HISTOGRAM"   , pParameter->asInt() == 3);
			pParameters->Set_Enabled("INCREASING"  , pParameter->asInt() == 4);
			pParameters->Set_Enabled("STDDEV"      , pParameter->asInt() == 6);
		}

		if( pParameter->Cmp_Identifier("OFFSET") )
		{
			pParameters->Set_Enabled("OFFSET_VALUE", pParameter->asInt() == 1);
		}
	}

	return( 1 );
}

//---------------------------------------------------------
bool CWKSP_Layer::_Classify(void)
{
	const CSG_String Methods = CSG_String::Format("%s|%s|%s|%s|%s|%s|%s",
		_TL("unique values"     ), // 0
		_TL("equal interval"    ), // 1
		_TL("defined interval"  ), // 2
		_TL("quantile"          ), // 3
		_TL("geometric interval"), // 4
		_TL("natural breaks"    ), // 5
		_TL("standard deviation")  // 6
	);

	CSG_Table *pTable = m_pObject->asTable(true);

	if( pTable )
	{
		if( pTable->Get_Field_Count() < 1 )
		{
			DLG_Message_Show(_TL("failed because there are no attributes"), _TL("Classify"));

			return( false );
		}
	}

	//-----------------------------------------------------
	if( m_Classify.Get_Count() == 0 )
	{
		m_Classify.Create(_TL("Classify"), _TL(""), SG_T("CLASSIFY"));

		if( pTable )
		{
			m_Classify.Add_Table ("", "TABLE", _TL("Table"), _TL(""), PARAMETER_INPUT)->Set_Enabled(false); m_Classify["TABLE"].Set_Value(pTable);

			m_Classify.Add_Choice("", "ALL_FIELDS", _TL("Field"        ), _TL(""), "");
			m_Classify.Add_Choice("", "NUM_FIELDS", _TL("Field"        ), _TL(""), "");
			m_Classify.Add_Choice("", "NUM_NORMAL", _TL("Normalization"), _TL(""), "");
		}

		m_Classify.Add_Colors(""      , "COLORS"      , _TL("Colors"                    ), _TL(""), m_Parameters("METRIC_COLORS")->asColors());
		m_Classify.Add_Int   (""      , "CLASSES_MAX" , _TL("Maximum Number of Classes" ), _TL(""), 1000, 10, true);
		m_Classify.Add_Int   (""      , "CLASSES"     , _TL("Number of Classes"         ), _TL(""),   10,  1, true);
		m_Classify.Add_Choice(""      , "METHOD"      , _TL("Classification"            ), _TL(""), Methods, 1);
		m_Classify.Add_Choice("METHOD", "LABELING"    , _TL("Labeling"                  ), _TL(""), CSG_String::Format("%s|%s", _TL("range"), _TL("less than...")), 1);
		m_Classify.Add_Double(""      , "INTERVAL"    , _TL("Defined Interval"          ), _TL(""), 1., 0., true);
		m_Classify.Add_Choice(""      , "OFFSET"      , _TL("Offset"                    ), _TL(""), CSG_String::Format("%s|%s", _TL("minimum value"), _TL("user defined")));
		m_Classify.Add_Double("OFFSET", "OFFSET_VALUE", _TL("Value"                     ), _TL(""), 1., 0., true);
		m_Classify.Add_Choice(""      , "HISTOGRAM"   , _TL("Percentile Estimation"     ), _TL(""), CSG_String::Format("%s|%s", _TL("index"), _TL("histogram")), 1);
		m_Classify.Add_Bool  (""      , "INCREASING"  , _TL("Increasing Intervals"      ), _TL(""), true);
		m_Classify.Add_Double(""      , "STDDEV"      , _TL("Standard Deviation"        ), _TL(""), 0.5, 0.01, true);
		m_Classify.Add_Double("STDDEV", "STDDEV_MAX"  , _TL("Maximum Standard Deviation"), _TL(""), 4.5, 0.5 , true);

		if( m_pObject->asGrid () ) { m_Classify["INTERVAL"].Set_Value(m_pObject->asGrid ()->Get_Range() / 10); m_Classify["OFFSET_VALUE"].Set_Value(m_pObject->asGrid ()->Get_Min()); }
		if( m_pObject->asGrids() ) { m_Classify["INTERVAL"].Set_Value(m_pObject->asGrids()->Get_Range() / 10); m_Classify["OFFSET_VALUE"].Set_Value(m_pObject->asGrids()->Get_Min()); }
	}

	//-----------------------------------------------------
	if( pTable )
	{
		Set_Fields_Choice(m_Classify("ALL_FIELDS"), false, false);

		if( Set_Fields_Choice(m_Classify("NUM_FIELDS"), true, false) )
		{
			Set_Fields_Choice(m_Classify("NUM_NORMAL"), true, true, false);

			m_Classify["METHOD"].asChoice()->Set_Items(Methods);
		}
		else
		{
			m_Classify["METHOD"].asChoice()->Set_Items(Methods.BeforeFirst('|')); // only strings => unique values
		}
	}

	//-----------------------------------------------------
	CSG_Parameters Parameters(m_Classify); Parameters.Set_Callback_On_Parameter_Changed(&_Classify_Callback);

	if( !DLG_Parameters(&Parameters) )
	{
		return( false );
	}

	m_Classify.Assign_Values(&Parameters);

	//-----------------------------------------------------
	CSGDI_Classify Classify; int Method = m_Classify["METHOD"].asInt();

	if( pTable == NULL )
	{
		Classify.Create(m_pObject);
	}
	else if( Method == 0 ) // unique values, categories!
	{
		Classify.Create(pTable, Get_Fields_Choice(m_Classify("ALL_FIELDS")));
	}
	else // if( Method > 0 ) // quantities!
	{
		Classify.Create(pTable, Get_Fields_Choice(m_Classify("NUM_FIELDS")), Get_Fields_Choice(m_Classify("NUM_NORMAL")));
	}

	switch( Method )
	{
	case  0: Classify.Classify_Unique   (m_Classify["CLASSES_MAX"].asInt   ()                                         );   break;
	case  1: Classify.Classify_Equal    (m_Classify["CLASSES"    ].asInt   ()                                         );   break;
	case  2: if( m_Classify["OFFSET"].asInt() )
	     {   Classify.Classify_Defined  (m_Classify["INTERVAL"   ].asDouble(), m_Classify["OFFSET_VALUE"].asDouble()  ); }
	else {   Classify.Classify_Defined  (m_Classify["INTERVAL"   ].asDouble()                                         ); } break;
	case  3: Classify.Classify_Quantile (m_Classify["CLASSES"    ].asInt   (), m_Classify["HISTOGRAM"   ].asInt() == 1);   break;
	case  4: Classify.Classify_Geometric(m_Classify["CLASSES"    ].asInt   (), m_Classify["INCREASING"  ].asBool()    );   break;
	case  5: Classify.Classify_Natural  (m_Classify["CLASSES"    ].asInt   ()                                         );   break;
	case  6: Classify.Classify_StdDev   (m_Classify["STDDEV"     ].asDouble(), m_Classify["STDDEV_MAX"  ].asDouble()  );   break;
	}

	//-----------------------------------------------------
	if( Classify.Set_LUT(*m_Parameters["LUT"].asTable(), *m_Classify["COLORS"].asColors(), m_Classify["LABELING"].asInt()) )
	{
		if( m_Parameters["LUT"].asTable()->Get_MetaData().Get_Child("SAGA_GUI_LUT_TYPE") == NULL )
		{
			m_Parameters["LUT"].asTable()->Get_MetaData().Add_Child("SAGA_GUI_LUT_TYPE", m_pObject->Get_ObjectType());
		}

		m_Parameters["COLORS_TYPE"].Set_Value(CLASSIFY_LUT); // Lookup Table

		if( pTable )
		{
			if( Method == 0 ) // unique values, categories!
			{
				m_Parameters["LUT_FIELD" ].Set_Value(m_Classify["ALL_FIELDS"].asInt());
			}
			else // numeric values, quantities!
			{
				m_Parameters["LUT_FIELD" ].Set_Value(Get_Fields_Choice(m_Classify("NUM_FIELDS")));

				m_Parameters["LUT_NORMAL"].Set_Value(m_Classify["NUM_NORMAL"].asInt());
			}
		}

		Parameters_Changed();

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Layer::Classify(const CSG_MetaData &Options)
{
	int Method = -1;

	if( !Options.Get_Content("METHOD", Method) )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSGDI_Classify Classify; CSG_Table *pTable = m_pObject->asTable(true); int Field = -1;
	
	if( pTable )
	{
		if( !Options.Get_Content("FIELD", Field) || Field < 0 || Field >= pTable->Get_Field_Count() )
		{
			return( false );
		}

		Classify.Create(pTable, Field);
	}
	else
	{
		Classify.Create(m_pObject);
	}

	//-----------------------------------------------------
	int    Classes_Max; Options.Get_Content("CLASSES_MAX", Classes_Max = 1024);
	int    Classes    ; Options.Get_Content("CLASSES"    , Classes     =   10);
	int    Increasing ; Options.Get_Content("INCREASING" , Increasing  =    1);
	int    Histogram  ; Options.Get_Content("HISTOGRAM"  , Histogram   =    1);
	double StdDev     ; Options.Get_Content("STDDEV"     , StdDev      =  0.5);
	double StdDev_Max ; Options.Get_Content("STDDEV_MAX" , StdDev_Max  =  4.0);
	double Interval   ; Options.Get_Content("INTERVAL"   , Interval    =  0.0);
	double Offset; bool bOffset = Options.Get_Content("OFFSET", Offset =  0.0);

	switch( Method )
	{
	case  0: Classify.Classify_Unique   (Classes_Max             );   break;
	case  1: Classify.Classify_Equal    (Classes                 );   break;
	case  2: if( bOffset )
	     {   Classify.Classify_Defined  (Interval, Offset        ); }
	else {   Classify.Classify_Defined  (Interval                ); } break;
	case  3: Classify.Classify_Quantile (Classes, Histogram  == 1);   break;
	case  4: Classify.Classify_Geometric(Classes, Increasing == 1);   break;
	case  5: Classify.Classify_Natural  (Classes                 );   break;
	case  6: Classify.Classify_StdDev   (StdDev, StdDev_Max      );   break;
	}

	//-----------------------------------------------------
	int ColorsID = -1; Options.Get_Content("COLORS", ColorsID);
	CSG_Colors Colors; if( !Colors.Create(0, ColorsID) ) { Colors.Create(*m_Parameters("METRIC_COLORS")->asColors()); }

	if( Classify.Set_LUT(*m_Parameters["LUT"].asTable(), Colors, 1) )
	{
		if( m_Parameters["LUT"].asTable()->Get_MetaData().Get_Child("SAGA_GUI_LUT_TYPE") == NULL )
		{
			m_Parameters["LUT"].asTable()->Get_MetaData().Add_Child("SAGA_GUI_LUT_TYPE", m_pObject->Get_ObjectType());
		}

		m_Parameters["COLORS_TYPE"].Set_Value(CLASSIFY_LUT); // Lookup Table

		if( pTable )
		{
			m_Parameters["LUT_FIELD" ].Set_Value(Field);
			m_Parameters["LUT_NORMAL"].Set_Value(pTable->Get_Field_Count());
		}

		Parameters_Changed();

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

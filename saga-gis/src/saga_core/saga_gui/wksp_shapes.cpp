
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
//                   WKSP_Shapes.cpp                     //
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
#include <saga_gdi/sgdi_helper.h>

#include "res_commands.h"
#include "res_controls.h"
#include "res_dialogs.h"

#include "helper.h"

#include "active.h"

#include "wksp_layer_classify.h"

#include "wksp_data_manager.h"

#include "wksp_shapes.h"
#include "wksp_table.h"

#include "data_source_pgsql.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Shapes::CWKSP_Shapes(CSG_Shapes *pShapes)
	: CWKSP_Layer(pShapes)
{
	m_pTable      = new CWKSP_Table(pShapes);

	m_fInfo       = -1;

	m_Edit.Shapes.Create(pShapes->Get_Type());
	m_Edit.pShape = NULL;

	m_Edit_Attributes.Destroy();
	m_Edit_Attributes.Add_Field(_TL("Name" ), SG_DATATYPE_String);
	m_Edit_Attributes.Add_Field(_TL("Value"), SG_DATATYPE_String);

	m_Edit.Color  = *wxBLACK;
	m_Edit.bGleam = true;
	m_Edit.Mode   = EDIT_SHAPE_MODE_Normal;

	m_Sel_Color   = *wxRED;

	m_bVertices   = 0;
}

//---------------------------------------------------------
CWKSP_Shapes::~CWKSP_Shapes(void)
{
	delete(m_pTable);
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_Shapes::Get_Description(void)
{
	wxString s;

	//-----------------------------------------------------
	s += "<h4>" + Get_Manager()->Get_Name() + "</h4>";

	s += g_pData->Set_Description_Image(this);

	s += "<table border=\"0\">";

	DESC_ADD_STR(_TL("Name"               ), m_pObject->Get_Name());
	DESC_ADD_STR(_TL("Description"        ), m_pObject->Get_Description());

	if( SG_File_Exists(m_pObject->Get_File_Name(false)) )
	{
		DESC_ADD_STR(_TL("Data Source"    ), SG_File_Get_Path(m_pObject->Get_File_Name(false)      ).c_str());
		DESC_ADD_STR(_TL("File"           ), SG_File_Get_Name(m_pObject->Get_File_Name(false), true).c_str());

		if( m_pObject->Get_MetaData()("GDAL_DRIVER") )
		{
			DESC_ADD_STR(_TL("Driver"     ), m_pObject->Get_MetaData()["GDAL_DRIVER"].Get_Content().c_str());
		}
	}
	else if( m_pObject->Get_MetaData_DB().Get_Children_Count() )
	{
		DESC_ADD_STR(_TL("Data Source"    ), m_pObject->Get_File_Name(false));
	}
	else
	{
		DESC_ADD_STR(_TL("Data Source"    ), _TL("memory"));
	}

	DESC_ADD_STR  (_TL("Modified"         ), m_pObject->is_Modified() ? _TL("yes") : _TL("no"));
	DESC_ADD_STR  (_TL("Spatial Reference"), m_pObject->Get_Projection().Get_Description().c_str());
	DESC_ADD_FLT  (_TL("West"             ), Get_Shapes()->Get_Extent().Get_XMin  ());
	DESC_ADD_FLT  (_TL("East"             ), Get_Shapes()->Get_Extent().Get_XMax  ());
	DESC_ADD_FLT  (_TL("West-East"        ), Get_Shapes()->Get_Extent().Get_XRange());
	DESC_ADD_FLT  (_TL("South"            ), Get_Shapes()->Get_Extent().Get_YMin  ());
	DESC_ADD_FLT  (_TL("North"            ), Get_Shapes()->Get_Extent().Get_YMax  ());
	DESC_ADD_FLT  (_TL("South-North"      ), Get_Shapes()->Get_Extent().Get_YRange());
	DESC_ADD_STR  (_TL("Type"             ), SG_Get_ShapeType_Name(Get_Shapes()->Get_Type()).c_str());

	switch( Get_Shapes()->Get_Vertex_Type() )
	{
	default:
		DESC_ADD_STR  (_TL("Vertex Type"  ), _TL("X, Y"      ));
		break;
	case  1:
		DESC_ADD_STR  (_TL("Vertex Type"  ), _TL("X, Y, Z"   ));
		DESC_ADD_FLT  (_TL("Z Minimum"    ), Get_Shapes()->Get_ZMin());
		DESC_ADD_FLT  (_TL("Z Maximum"    ), Get_Shapes()->Get_ZMax());
		DESC_ADD_FLT  (_TL("Z Range"      ), Get_Shapes()->Get_ZMax() - Get_Shapes()->Get_ZMin());
		break;
	case  2:
		DESC_ADD_STR  (_TL("Vertex Type"  ), _TL("X, Y, Z, M"));
		DESC_ADD_FLT  (_TL("Z Minimum"    ), Get_Shapes()->Get_ZMin());
		DESC_ADD_FLT  (_TL("Z Maximum"    ), Get_Shapes()->Get_ZMax());
		DESC_ADD_FLT  (_TL("Z Range"      ), Get_Shapes()->Get_ZMax() - Get_Shapes()->Get_ZMin());
		DESC_ADD_FLT  (_TL("M Minimum"    ), Get_Shapes()->Get_MMin());
		DESC_ADD_FLT  (_TL("M Maximum"    ), Get_Shapes()->Get_MMax());
		DESC_ADD_FLT  (_TL("M Range"      ), Get_Shapes()->Get_MMax() - Get_Shapes()->Get_MMin());
		break;
	}

	DESC_ADD_LONG(_TL("Number of Shapes"  ), Get_Shapes()->Get_Count());
	DESC_ADD_LONG(_TL("Selected"          ), Get_Shapes()->Get_Selection_Count());
	DESC_ADD_STR (_TL("File Encoding"     ), Get_Shapes()->Get_File_Encoding() ? SG_T("UTF-8") : SG_T("ANSI"));

	s += "</table>";

	s += wxString::Format("<hr><h4>%s</h4>", _TL("Coordinate System Details"));
	s += m_pObject->Get_Projection().Get_Description(true).c_str();

	s += Get_TableInfo_asHTML(Get_Shapes());

	//-----------------------------------------------------
//	s += wxString::Format(wxT("<hr><b>%s</b><font size=\"-1\">"), _TL("Data History"));
//	s += Get_Shapes()->Get_History().Get_HTML();
//	s += wxString::Format(wxT("</font"));

	return( s );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxToolBarBase * CWKSP_Shapes::Get_ToolBar(void)
{
	static wxToolBarBase *static_pToolBar = NULL;

	if( !static_pToolBar )
	{
		static_pToolBar = CMD_ToolBar_Create(ID_TB_DATA_SHAPES);

		Add_ToolBar_Defaults(static_pToolBar);
	//	CMD_ToolBar_Add_Item(static_pToolBar, false, ID_CMD_DATA_SCATTERPLOT);
		CMD_ToolBar_Add_Item(static_pToolBar,  true, ID_CMD_DATA_DIAGRAM);
	//	static_pToolBar->AddSeparator();
		CMD_ToolBar_Add_Item(static_pToolBar,  true, ID_CMD_TABLE_SHOW);
	//	CMD_ToolBar_Add_Item(static_pToolBar, false, ID_CMD_SHAPES_SAVE_ATTRIBUTES);
	//	CMD_ToolBar_Add_Item(static_pToolBar, false, ID_CMD_TABLE_JOIN_DATA);

		CMD_ToolBar_Add(static_pToolBar, _TL("Shapes"));
	}

	return( static_pToolBar );
}

//---------------------------------------------------------
wxMenu * CWKSP_Shapes::Get_Menu(void)
{
	wxMenu *pSubMenu, *pMenu = new wxMenu(m_pObject->Get_Name());

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_CLOSE);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_SHOW_MAP);
	if( MDI_Get_Active_Map() )
	{
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAP_ZOOM_LAYER_ACTIVE);
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAP_PAN_LAYER_ACTIVE);
	}

	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_SAVE);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_SAVEAS);

	if( PGSQL_has_Connections() )
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_SAVETODB);

	if( m_pObject->is_Modified() && SG_File_Exists(m_pObject->Get_File_Name(false)) )
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_RELOAD);

	if( m_pObject->is_File_Native() )
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_DEL_FILES);

	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_PROJECTION);

	if( m_pObject->Get_MetaData().Get_Children_Count() > 0 )
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_METADATA);

	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_CLASSIFY);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_CLASSIFY_IMPORT);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_SETTINGS_COPY);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_FORCE_UPDATE);

	pMenu->AppendSeparator();

	pMenu->Append(ID_CMD_WKSP_FIRST, _TL("Attributes"), pSubMenu = new wxMenu());
	CMD_Menu_Add_Item(pSubMenu,  true, ID_CMD_TABLE_SHOW);
	CMD_Menu_Add_Item(pSubMenu, false, ID_CMD_SHAPES_SAVE_ATTRIBUTES);
	CMD_Menu_Add_Item(pSubMenu, false, ID_CMD_TABLE_JOIN_DATA);

	pMenu->Append(ID_CMD_WKSP_FIRST, _TL("Charts"    ), pSubMenu = new wxMenu());
	CMD_Menu_Add_Item(pSubMenu,  true, ID_CMD_DATA_HISTOGRAM);
	CMD_Menu_Add_Item(pSubMenu, false, ID_CMD_DATA_SCATTERPLOT);
	CMD_Menu_Add_Item(pSubMenu,  true, ID_CMD_DATA_DIAGRAM);

	pMenu->Append(ID_CMD_WKSP_FIRST, _TL("Selection" ), pSubMenu = new wxMenu());
	if( MDI_Get_Active_Map() && Get_Shapes()->Get_Selection_Count() > 0 )
	{
		CMD_Menu_Add_Item(pSubMenu, false, ID_CMD_MAP_ZOOM_LAYER_SELECTION);
		CMD_Menu_Add_Item(pSubMenu, false, ID_CMD_MAP_PAN_LAYER_SELECTION);
	}
	CMD_Menu_Add_Item(pSubMenu,  true, ID_CMD_DATA_SELECTION_CLEAR);
	CMD_Menu_Add_Item(pSubMenu, false, ID_CMD_DATA_SELECTION_INVERT);
	CMD_Menu_Add_Item(pSubMenu,  true, ID_CMD_TABLE_SELECT_NUMERIC);
	CMD_Menu_Add_Item(pSubMenu,  true, ID_CMD_TABLE_SELECT_STRING);

	pMenu->Append(ID_CMD_WKSP_FIRST, _TL("Edit"), Edit_Get_Menu());

	return( pMenu );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Shapes::On_Command(int Cmd_ID)
{
	switch( Cmd_ID )
	{
	default:
		return( CWKSP_Layer::On_Command(Cmd_ID) );

	//-----------------------------------------------------
	case ID_CMD_DATA_CLASSIFY_IMPORT  : _LUT_Import             (); break;

	case ID_CMD_TABLE_SHOW            : m_pTable->Toggle_View   (); break;
	case ID_CMD_DATA_DIAGRAM          : m_pTable->Toggle_Diagram(); break;

	//-----------------------------------------------------
	case ID_CMD_SHAPES_SAVE_ATTRIBUTES:
		{
			wxString File(m_pObject->Get_File_Name());

			if( DLG_Save(File, ID_DLG_TABLE_SAVE) )
			{
				CSG_Table Table(*m_pObject->asTable(true));

				Table.Save(&File);
			}
		}
		break;

	//-----------------------------------------------------
	case ID_CMD_SHAPES_EDIT_SHAPE    : _Edit_Shape         (); break;
	case ID_CMD_SHAPES_EDIT_ADD_SHAPE: _Edit_Shape_Add     (); break;
	case ID_CMD_SHAPES_EDIT_DEL_SHAPE: _Edit_Shape_Del     (); break;
	case ID_CMD_SHAPES_EDIT_ADD_PART : _Edit_Part_Add      (); break;
	case ID_CMD_SHAPES_EDIT_DEL_PART : _Edit_Part_Del      (); break;
	case ID_CMD_SHAPES_EDIT_DEL_POINT: _Edit_Point_Del     (); break;
	case ID_CMD_SHAPES_EDIT_MERGE    : _Edit_Merge         (); break;
	case ID_CMD_SHAPES_EDIT_SPLIT    : _Edit_Split         (); break;
	case ID_CMD_SHAPES_EDIT_MOVE     : _Edit_Move          (); break;
    case ID_CMD_SHAPES_EDIT_SEL_COPY : _Edit_Selection_Copy(); break;

	//-----------------------------------------------------
	case ID_CMD_DATA_SELECTION_CLEAR : Get_Shapes()->Select       (); Update_Views(); break;
	case ID_CMD_DATA_SELECTION_INVERT: Get_Shapes()->Inv_Selection(); Update_Views(); break;

	case ID_CMD_TABLE_SELECT_NUMERIC : { CSG_Tool *pTool = SG_Get_Tool_Library_Manager().Get_Tool("shapes_tools", 3);
			if(	pTool && pTool->On_Before_Execution() && pTool->Set_Parameter("SHAPES", m_pObject) && DLG_Parameters(pTool->Get_Parameters()) )
			{
				pTool->Execute();
			}
 		break; }

	case ID_CMD_TABLE_SELECT_STRING  : { CSG_Tool *pTool = SG_Get_Tool_Library_Manager().Get_Tool("shapes_tools", 4);
			if(	pTool && pTool->On_Before_Execution() && pTool->Set_Parameter("SHAPES", m_pObject) && DLG_Parameters(pTool->Get_Parameters()) )
			{
				pTool->Execute();
			}
 		break; }

	//-----------------------------------------------------
	case ID_CMD_TABLE_JOIN_DATA: { CSG_Tool *pTool = SG_Get_Tool_Library_Manager().Get_Tool("table_tools", 3);
			if(	pTool && pTool->On_Before_Execution() && pTool->Set_Parameter("TABLE_A", m_pObject) && DLG_Parameters(pTool->Get_Parameters()) )
			{
				pTool->Execute();
			}
		break; }
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Shapes::On_Command_UI(wxUpdateUIEvent &event)
{
	switch( event.GetId() )
	{
	default: return( CWKSP_Layer::On_Command_UI(event) );

	case ID_CMD_SHAPES_EDIT_SHAPE    : event.Enable(m_Edit.pShape != NULL || Get_Shapes()->Get_Selection_Count() > 0); event.Check(m_Edit.pShape != NULL); break;
	case ID_CMD_SHAPES_EDIT_ADD_SHAPE: event.Enable(m_Edit.pShape == NULL); break;
	case ID_CMD_SHAPES_EDIT_DEL_SHAPE: event.Enable(m_Edit.pShape == NULL && Get_Shapes()->Get_Selection_Count() > 0); break;
	case ID_CMD_SHAPES_EDIT_ADD_PART : event.Enable(m_Edit.pShape != NULL); break;
	case ID_CMD_SHAPES_EDIT_DEL_PART : event.Enable(m_Edit.pShape != NULL && m_Edit.Part >= 0); break;
	case ID_CMD_SHAPES_EDIT_DEL_POINT: event.Enable(m_Edit.pShape != NULL && m_Edit.Part >= 0 && m_Edit.Point >= 0); break;
	case ID_CMD_SHAPES_EDIT_MOVE     : event.Enable(m_Edit.pShape != NULL); event.Check(m_Edit.Mode == EDIT_SHAPE_MODE_Move); break;
	case ID_CMD_SHAPES_EDIT_SEL_COPY : event.Enable(m_Edit.pShape == NULL && Get_Shapes()->Get_Selection_Count() > 0); break;

	case ID_CMD_DATA_SELECTION_CLEAR : event.Enable(m_Edit.pShape == NULL && Get_Shapes()->Get_Selection_Count() > 0); break;
	case ID_CMD_DATA_SELECTION_INVERT: event.Enable(m_Edit.pShape == NULL); break;

	case ID_CMD_TABLE_SHOW           : event.Check(m_pTable->Get_View   () != NULL); break;
	case ID_CMD_DATA_DIAGRAM         : event.Check(m_pTable->Get_Diagram() != NULL); break;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Shapes::On_Create_Parameters(void)
{
	CWKSP_Layer::On_Create_Parameters();

	//-----------------------------------------------------
	// General...

	m_Parameters.Add_Int("NODE_GENERAL",
		"MAX_SAMPLES"     , _TL("Maximum Samples"),
		_TL("Maximum number of samples used to build statistics and histograms."),
		(int)m_pObject->Get_Max_Samples(), 0, true
	);

	m_Parameters.Add_Choice("NODE_GENERAL",
		"INFO_FIELD"      , _TL("Additional Information"),
		_TL("Field that provides file paths to additional record information (HTML formatted), either absolute or relative to this data set."),
		_TL("<default>")
	);

	//-----------------------------------------------------
	// Display...

	m_Parameters.Add_Parameters("NODE_DISPLAY",
		"DISPLAY_CHART"   , _TL("Chart"), _TL("")
	);

	//-----------------------------------------------------
	// Classification...

	m_Parameters.Add_Bool("NODE_COLORS",
		"NODATA_SHOW"     , _TL("Show No-Data"), _TL(""), true
	);

	m_Parameters.Add_Color("NODATA_SHOW",
		"NODATA_COLOR"    , _TL("Color"), _TL(""), SG_COLOR_GREY_LIGHT
	);

	//-----------------------------------------------------
	// Label...

	m_Parameters.Add_Choice("NODE_LABEL",
		"LABEL_FIELD"     , _TL("Field"),
		_TL(""),
		_TL("<default>")
	);

	m_Parameters.Add_Font("LABEL_FIELD",
		"LABEL_FIELD_FONT", _TL("Font"),
		_TL("")
	);

	#ifndef _SAGA_MSW
	m_Parameters.Add_Color("LABEL_FIELD_FONT",
		"LABEL_COLOR"		, _TL("Color"),
		_TL(""),
		Get_Color_asInt(SYS_Get_Color_Foreground())
	);
	#endif

	m_Parameters.Add_Choice("LABEL_FIELD",
		"LABEL_FIELD_EFFECT"	, _TL("Boundary Effect"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s",
			_TL("none"),
			_TL("full frame"),
			_TL("top"),
			_TL("top left"),
			_TL("left"),
			_TL("bottom left"),
			_TL("bottom"),
			_TL("bottom right"),
			_TL("right"),
			_TL("top right")
		), 1
	);

	m_Parameters.Add_Color("LABEL_FIELD_EFFECT",
		"LABEL_FIELD_EFFECT_COLOR"	, _TL("Color"),
		_TL(""),
		Get_Color_asInt(SYS_Get_Color_Background())
	);

	m_Parameters.Add_Int("LABEL_FIELD_EFFECT",
		"LABEL_FIELD_EFFECT_SIZE"	, _TL("Size"),
		_TL(""),
		1, 1, true
	);

	m_Parameters.Add_Choice("LABEL_FIELD",
		"LABEL_FIELD_PREC"	, _TL("Numerical Precision"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("fit to value"),
			_TL("standard"),
			SG_T("0|0.1|0.12|0.123|0.1234|0.12345|0.1234567|0.12345678|0.123456789|0.1234567890|0.12345678901|0.123456789012|0.1234567890123|0.12345678901234|0.123456789012345|0.1234567890123456|0.12345678901234567|0.123456789012345678|0.1234567890123456789|0.12345678901234567890")
		), 0
	);

	m_Parameters.Add_Choice("LABEL_FIELD",
		"LABEL_FIELD_SIZE_TYPE", _TL("Size relates to..."),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("Screen"),
			_TL("Map Units")
		), 0
	);

	m_Parameters.Add_Choice("LABEL_FIELD",
		"LABEL_FIELD_SIZEBY"		, _TL("Size by Attribute"),
		_TL(""),
		_TL("<default>")
	);

	m_Parameters.Add_Double("LABEL_FIELD_SIZEBY",
		"LABEL_FIELD_SIZE"			, _TL("Default Size"),
		_TL(""),
		100., 0., true
	);

	//-----------------------------------------------------
	// Selection...

	m_Parameters.Add_Color("NODE_SELECTION",
		"SEL_COLOR"		, _TL("Color"),
		_TL(""),
		m_Sel_Color.GetRGB()
	);

	//-----------------------------------------------------
	// Edit...

	m_Parameters.Add_Color("NODE_EDIT",
		"EDIT_COLOR"	, _TL("Color"),
		_TL(""),
		m_Edit.Color.GetRGB()
	);

	m_Parameters.Add_Bool("NODE_EDIT",
		"EDIT_GLEAM"	, _TL("Gleam"),
		_TL(""),
		m_Edit.bGleam
	);

	m_Parameters.Add_Shapes_List("NODE_EDIT",
		"EDIT_SNAP_LIST", _TL("Snap to..."),
		_TL(""),
		PARAMETER_INPUT
	)->asShapesList()->Add_Item(m_pObject);

	m_Parameters.Add_Int("EDIT_SNAP_LIST",
		"EDIT_SNAP_DIST", _TL("Snap Distance"),
		_TL("snap distance in screen units (pixels)"),
		10, 0, true
	);
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Shapes::On_DataObject_Changed(void)
{
	Set_Fields_Choice(m_Parameters("LUT_FIELD"         ), false, false);
	Set_Fields_Choice(m_Parameters("LUT_NORMAL"        ),  true,  true);
	Set_Fields_Choice(m_Parameters("METRIC_FIELD"      ),  true, false);
	Set_Fields_Choice(m_Parameters("METRIC_NORMAL"     ),  true,  true);
	Set_Fields_Choice(m_Parameters("LABEL_FIELD"       ), false,  true);
	Set_Fields_Choice(m_Parameters("LABEL_FIELD_SIZEBY"),  true,  true);
	Set_Fields_Choice(m_Parameters("INFO_FIELD"        ), false,  true);

	_Chart_Set_Options();

	//-----------------------------------------------------
	m_Parameters.Set_Parameter("MAX_SAMPLES", (int)m_pObject->Get_Max_Samples());

	//-----------------------------------------------------
	CWKSP_Layer::On_DataObject_Changed();

	m_pTable->DataObject_Changed();
}

//---------------------------------------------------------
void CWKSP_Shapes::On_Parameters_Changed(void)
{
	CWKSP_Layer::On_Parameters_Changed();

	//-----------------------------------------------------
	m_pObject->Set_Max_Samples(m_Parameters("MAX_SAMPLES")->asInt());

	//-----------------------------------------------------
	switch( m_Parameters("COLORS_TYPE")->asInt() )
	{
	default: // CLASSIFY_SINGLE
		m_Stretch.Value  = -1;
		m_Stretch.Normal = -1;
		break;

	case  1: // CLASSIFY_LUT
		m_Stretch.Value  = Get_Fields_Choice(m_Parameters("LUT_FIELD"    ));
		m_Stretch.Normal = Get_Fields_Choice(m_Parameters("LUT_NORMAL"   ));
		m_Stretch.Scale  = 1.;
		break;

	case  2: // CLASSIFY_DISCRETE
	case  3: // CLASSIFY_GRADUATED
		m_Stretch.Value  = Get_Fields_Choice(m_Parameters("METRIC_FIELD" ));
		m_Stretch.Normal = Get_Fields_Choice(m_Parameters("METRIC_NORMAL"));
		m_Stretch.Scale  = m_Parameters("METRIC_NORFMT")->asInt() == 0 ? 1. : 100.;
		break;
	}

	if( m_Stretch.Value < 0 )
	{
		m_pClassify->Set_Mode(CLASSIFY_SINGLE);
	}

	m_pClassify->Set_Unique_Color(m_pClassify->Get_Mode() == CLASSIFY_SINGLE
		? m_Parameters("SINGLE_COLOR")->asColor()
		: m_Parameters("NODATA_COLOR")->asColor()
	);

	//-----------------------------------------------------
	m_bNoData = m_Parameters("NODATA_SHOW")->asBool();

	//-----------------------------------------------------
	int fInfo = Get_Fields_Choice(m_Parameters("INFO_FIELD"));

	if( m_fInfo != fInfo )
	{
		m_fInfo = fInfo;

		if( g_pActive->Get_Active() == this )
		{
			g_pActive->Update_Info();
		}
	}

	//-----------------------------------------------------
	m_Label.Field = Get_Fields_Choice(m_Parameters("LABEL_FIELD"));

	m_Label.Effect_Color = m_Parameters("LABEL_FIELD_EFFECT_COLOR")->asColor();
	m_Label.Effect_Size  = m_Parameters("LABEL_FIELD_EFFECT_SIZE" )->asInt  ();
	m_Label.Precision    = m_Parameters("LABEL_FIELD_PREC"        )->asInt  ();

	switch( m_Label.Precision )
	{
	case  0: m_Label.Precision  = -m_Parameters("TABLE_FLT_DECIMALS")->asInt(); break;
	case  1: m_Label.Precision  = -99; break;
	default: m_Label.Precision -=   2; break;
	}

	switch( m_Parameters("LABEL_FIELD_EFFECT")->asInt() )
	{
	default: m_Label.Effect = TEXTEFFECT_NONE       ; break;
	case  1: m_Label.Effect = TEXTEFFECT_FRAME      ; break;
	case  2: m_Label.Effect = TEXTEFFECT_TOP        ; break;
	case  3: m_Label.Effect = TEXTEFFECT_TOPLEFT    ; break;
	case  4: m_Label.Effect = TEXTEFFECT_LEFT       ; break;
	case  5: m_Label.Effect = TEXTEFFECT_BOTTOMLEFT ; break;
	case  6: m_Label.Effect = TEXTEFFECT_BOTTOM     ; break;
	case  7: m_Label.Effect = TEXTEFFECT_BOTTOMRIGHT; break;
	case  8: m_Label.Effect = TEXTEFFECT_RIGHT      ; break;
	case  9: m_Label.Effect = TEXTEFFECT_TOPRIGHT   ; break;
	}

	//-----------------------------------------------------
	_Chart_Get_Options();
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CWKSP_Shapes::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter, int Flags)
{
	if( Flags & PARAMETER_CHECK_VALUES )
	{
		// nop
	}

	//-----------------------------------------------------
	if( Flags & PARAMETER_CHECK_ENABLE )
	{
		if(	pParameter->Cmp_Identifier("COLORS_TYPE") )
		{
			pParameters->Set_Enabled("NODATA_SHOW"  , pParameter->asInt() > 0);	// not 'single symbol'
		}

		if(	pParameter->Cmp_Identifier("NODATA_SHOW") )
		{
			pParameters->Set_Enabled("NODATA_COLOR" , pParameter->asBool());
		}

		if(	pParameter->Cmp_Identifier("OUTLINE") )
		{
			pParameters->Set_Enabled("OUTLINE_COLOR", pParameter->asBool());
			pParameters->Set_Enabled("OUTLINE_SIZE" , pParameter->asBool());
		}

		if(	pParameter->Cmp_Identifier("LABEL_FIELD") )
		{
			pParameter->Set_Children_Enabled(Get_Fields_Choice(pParameter) >= 0);
		}

		if( pParameter->Cmp_Identifier("LABEL_FIELD_SIZE_TYPE")
		||  pParameter->Cmp_Identifier("LABEL_FIELD_SIZEBY"   ) )
		{
			bool Value = (*pParameters)("LABEL_FIELD_SIZE_TYPE")->asInt() != 0 || Get_Fields_Choice((*pParameters)("LABEL_FIELD_SIZEBY")) >= 0;

			pParameters->Set_Enabled("LABEL_FIELD_SIZE", Value);
		}

		if(	pParameter->Cmp_Identifier("LABEL_FIELD_EFFECT") )
		{
			pParameter->Set_Children_Enabled(pParameter->asInt() != 0);
		}

		if(	pParameter->Cmp_Identifier("EDIT_SNAP_LIST") )
		{
			pParameters->Set_Enabled("EDIT_SNAP_DIST", pParameter->asList()->Get_Item_Count() > 0);
		}

		if(	pParameters->Cmp_Identifier("DISPLAY_CHART") )
		{
			CSG_String s(pParameter->Get_Identifier());

			if( s.Find("FIELD_") == 0 )
			{
				s.Replace("FIELD_", "COLOR_");

				pParameters->Set_Enabled(s, pParameter->asBool());
			}
		}
	}

	//-----------------------------------------------------
	return( CWKSP_Layer::On_Parameter_Changed(pParameters, pParameter, Flags) );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Shapes::On_Update_Views(void)
{
	m_pTable->Update_Views();
}

//---------------------------------------------------------
bool CWKSP_Shapes::Set_Diagram(bool bShow, CSG_Parameters *pParameters)
{
	return( m_pTable->Set_Diagram(bShow, pParameters) );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Shapes::_LUT_Import(void)
{
	wxString File, Filter;

	Filter.Printf("%s (*.qml)|*.qml|%s|*.*", _TL("QGIS Layer Style File"), _TL("All Files"));

	if( DLG_Open(File, _TL("Import Classification"), SG_T("QGIS Layer Style File (*.qml)|*.qml|All Files|*.*|")) )
	{
		CSG_Table Classes; CSG_String Attribute;

		if( QGIS_Styles_Import(&File, Classes, Attribute) )
		{
			m_Parameters.Set_Parameter("LUT_FIELD", Attribute);

			m_Parameters("LUT")->asTable()->Assign(&Classes);
			m_Parameters("LUT")->asTable()->Get_MetaData().Add_Child("SAGA_GUI_LUT_TYPE", m_pObject->Get_ObjectType());

			m_Parameters("COLORS_TYPE")->Set_Value(CLASSIFY_LUT); // Lookup Table

			Parameters_Changed();
		}
	}
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_Shapes::Get_Value(CSG_Point ptWorld, double Epsilon)
{
	wxString s; CSG_Shape *pShape = Get_Shapes()->Get_Shape(ptWorld, Epsilon);

	if( pShape )
	{
		CWKSP_Layer::Get_Field_Value(pShape->Get_Index(), m_Stretch.Value, m_Stretch.Normal, m_Stretch.Scale, s);
	}

	return( s );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Shapes::On_Draw(CSG_Map_DC &dc_Map, int Flags)
{
	if( Get_Extent().Intersects(dc_Map.rWorld()) == INTERSECTION_None )
	{
		if( m_Edit.pShape )
		{
			Edit_Shape_Draw(dc_Map);
		}

		return;
	}

	//-----------------------------------------------------
	if( (Flags & LAYER_DRAW_FLAG_THUMBNAIL) != 0 )
	{
		Draw_Initialize(dc_Map, Flags);

		for(sLong i=0; i<Get_Shapes()->Get_Count(); i++)
		{
			_Draw_Shape(dc_Map, Get_Shapes()->Get_Shape(i), LAYER_DRAW_FLAG_THUMBNAIL);
		}

		return;
	}

	//-----------------------------------------------------
	if( m_Parameters("DISPLAY_TRANSPARENCY")->asDouble() > 0. )
	{
		dc_Map.Draw_Layer_Begin();
	}

	m_Sel_Color   = Get_Color_asWX(m_Parameters("SEL_COLOR" )->asInt());
	m_Edit.Color  = Get_Color_asWX(m_Parameters("EDIT_COLOR")->asInt());
	m_Edit.bGleam =                m_Parameters("EDIT_GLEAM")->asBool();

	Draw_Initialize(dc_Map, Flags);

	//-----------------------------------------------------
	if( (Flags & LAYER_DRAW_FLAG_NOEDITS) != 0 || (Get_Shapes()->Get_Selection_Count() < 1 && !m_Edit.pShape) )
	{
		for(sLong i=0; i<Get_Shapes()->Get_Count(); i++)
		{
			_Draw_Shape(dc_Map, Get_Shapes()->Get_Shape(i));
		}

		if( _Chart_is_Valid() )
		{
			for(sLong i=0; i<Get_Shapes()->Get_Count(); i++)
			{
				_Draw_Chart(dc_Map, Get_Shapes()->Get_Shape(i));
			}
		}
	}

	//-----------------------------------------------------
	else // selection and/or editing
	{
		for(sLong i=0; i<Get_Shapes()->Get_Count(); i++)
		{
			if( !Get_Shapes()->Get_Shape(i)->is_Selected() )
			{
				_Draw_Shape(dc_Map, Get_Shapes()->Get_Shape(i));
			}
		}

		for(sLong i=0; i<Get_Shapes()->Get_Selection_Count(); i++)
		{
			if( i != m_Edit.Index )
			{
				_Draw_Shape(dc_Map, Get_Shapes()->Get_Selection(i), LAYER_DRAW_FLAG_SELECTION);
			}
		}

		//-------------------------------------------------
		if( !m_Edit.pShape )
		{
			_Draw_Shape(dc_Map, Get_Shapes()->Get_Selection(m_Edit.Index), LAYER_DRAW_FLAG_SELECTION|LAYER_DRAW_FLAG_HIGHLIGHT);
		}
		else
		{
			Edit_Shape_Draw(dc_Map);
		}

		if( m_Edit.Mode == EDIT_SHAPE_MODE_Split )
		{
			CSG_Shape *pSplit = m_Edit.Shapes.Get_Shape(1);

			if( pSplit && pSplit->Get_Point_Count() > 1 )
			{
				for(int i=0; i<=1; i++)
				{
					dc_Map.SetPen(wxPen(i == 0 ? *wxWHITE : *wxBLACK, i == 0 ? 3 : 1));

					TSG_Point_Int B, A = dc_Map.World2DC(pSplit->Get_Point());

					for(int iPoint=1; iPoint<pSplit->Get_Point_Count(); iPoint++)
					{
						B = A; A = dc_Map.World2DC(pSplit->Get_Point(iPoint));

						dc_Map.DrawLine(A.x, A.y, B.x, B.y);
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	if( (Flags & LAYER_DRAW_FLAG_NOLABELS) == 0 && m_Label.Field >= 0 )	// Labels
	{
		int    iSize = Get_Fields_Choice(m_Parameters("LABEL_FIELD_SIZEBY"));
		double dSize = m_Parameters("LABEL_FIELD_SIZE_TYPE")->asInt() == 1
			? dc_Map.World2DC() * m_Parameters("LABEL_FIELD_SIZE")->asDouble() : 1.;

		dc_Map.SetFont(Get_Font(m_Parameters("LABEL_FIELD_FONT")));
		dc_Map.Get_DC().SetTextForeground(m_Parameters(m_Parameters("LABEL_COLOR") ? "LABEL_COLOR" : "LABEL_FIELD_FONT")->asColor());

		if( iSize >= 0 && iSize < Get_Shapes()->Get_Field_Count() )	// size by attribute
		{
			for(sLong i=0; i<Get_Shapes()->Get_Count(); i++)
			{
				int Size = (int)(0.5 + dSize * Get_Shapes()->Get_Shape(i)->asDouble(iSize));

				if( Size > 0 )
				{
					_Draw_Label(dc_Map, Get_Shapes()->Get_Shape(i), Size);
				}
			}
		}
		else	// one size fits all
		{
			int Size = m_Parameters("LABEL_FIELD_SIZE_TYPE")->asInt() == 1 ? (int)(0.5 + dSize) : dc_Map.Get_DC().GetFont().GetPointSize();

			if( Size > 0 )
			{
				for(sLong i=0; i<Get_Shapes()->Get_Count(); i++)
				{
					_Draw_Label(dc_Map, Get_Shapes()->Get_Shape(i), Size);
				}
			}
		}
	}

	//-----------------------------------------------------
	if( m_Parameters("DISPLAY_TRANSPARENCY")->asDouble() > 0. ) // Transparency ?
	{
		dc_Map.Draw_Layer_End(m_Parameters("DISPLAY_TRANSPARENCY")->asDouble() / 100.);
	}
}

//---------------------------------------------------------
void CWKSP_Shapes::_Draw_Shape(CSG_Map_DC &dc_Map, CSG_Shape *pShape, int Flags)
{
	if( pShape && dc_Map.rWorld().Intersects(pShape->Get_Extent()) != INTERSECTION_None )
	{
		Draw_Shape(dc_Map, pShape, Flags);

		if( m_bVertices > 0 )
		{
			bool bLabel = m_bVertices == 2 && ((Flags & LAYER_DRAW_FLAG_SELECTION) || m_pObject->asShapes()->Get_Selection_Count() == 0);

			wxPen   oldPen  (dc_Map.Get_DC().GetPen  ()); dc_Map.SetPen  (*wxBLACK_PEN  );
			wxBrush oldBrush(dc_Map.Get_DC().GetBrush()); dc_Map.SetBrush(*wxWHITE_BRUSH);

			for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					TSG_Point_Int A = dc_Map.World2DC(pShape->Get_Point(iPoint, iPart));

					dc_Map.DrawCircle(A.x, A.y, 2);

					if( bLabel )
					{
						dc_Map.DrawText(TEXTALIGN_TOPLEFT, A.x, A.y, wxString::Format("%d", iPoint + 1));
					}
				}
			}

			dc_Map.SetPen  (oldPen  );
			dc_Map.SetBrush(oldBrush);
		}
	}
}

//---------------------------------------------------------
void CWKSP_Shapes::_Draw_Label(CSG_Map_DC &dc_Map, CSG_Shape *pShape, int PointSize)
{
	if( pShape && dc_Map.rWorld().Intersects(pShape->Get_Extent()) != INTERSECTION_None )
	{
		wxString Label(pShape->asString(m_Label.Field, m_Label.Precision)); Label.Trim(true).Trim(false);

		if( !Label.IsEmpty() )
		{
			if( PointSize > 0 && PointSize != dc_Map.Get_DC().GetFont().GetPointSize() )
			{
				wxFont Font(dc_Map.Get_DC().GetFont());

				Font.SetPointSize(PointSize);

				dc_Map.SetFont(Font);
			}

			Draw_Label(dc_Map, pShape, Label);
		}
	}
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter * CWKSP_Shapes::BrushList_Add(const CSG_String &ParentID, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description)
{
	return( m_Parameters.Add_Choice(ParentID, Identifier, Name, Description,
		CSG_String::Format("%s|%s|%s|%s|%s|%s|%s|%s|",
			_TL("Opaque"           ),
			_TL("Transparent"      ),
			_TL("Backward Diagonal"),
			_TL("Cross Diagonal"   ),
			_TL("Forward Diagonal" ),
			_TL("Cross"            ),
			_TL("Horizontal"       ),
			_TL("Vertical"         )
		), 0)
	);
}

//---------------------------------------------------------
wxBrushStyle CWKSP_Shapes::BrushList_Get_Style(const CSG_String &Identifier)
{
	switch( m_Parameters(Identifier)->asInt() )
	{
	default: return( wxBRUSHSTYLE_SOLID           );
	case  1: return( wxBRUSHSTYLE_TRANSPARENT     );
	case  2: return( wxBRUSHSTYLE_BDIAGONAL_HATCH );
	case  3: return( wxBRUSHSTYLE_CROSSDIAG_HATCH );
	case  4: return( wxBRUSHSTYLE_FDIAGONAL_HATCH );
	case  5: return( wxBRUSHSTYLE_CROSS_HATCH     );
	case  6: return( wxBRUSHSTYLE_HORIZONTAL_HATCH);
	case  7: return( wxBRUSHSTYLE_VERTICAL_HATCH  );
	}
}

//---------------------------------------------------------
CSG_Parameter * CWKSP_Shapes::PenList_Add(const CSG_String &ParentID, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description)
{
	return( m_Parameters.Add_Choice(ParentID, Identifier, Name, Description,
		CSG_String::Format("%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|%s|",
			_TL("Solid style"),
			_TL("Dotted style"),
			_TL("Long dashed style"),
			_TL("Short dashed style"), 
			_TL("Dot and dash style"),
			_TL("Backward diagonal hatch"),
			_TL("Cross-diagonal hatch"),
			_TL("Forward diagonal hatch"),
			_TL("Cross hatch"),
			_TL("Horizontal hatch"),
			_TL("Vertical hatch")
		//	_TL("Use the stipple bitmap")
		//	_TL("Use the user dashes")
		//	_TL("No pen is used")
		), 0)
	);
}

//---------------------------------------------------------
int CWKSP_Shapes::PenList_Get_Style(const CSG_String &Identifier)
{
	switch( m_Parameters(Identifier)->asInt() )
	{
	default: return( wxPENSTYLE_SOLID            ); // Solid style.
	case  1: return( wxPENSTYLE_DOT              ); // Dotted style.
	case  2: return( wxPENSTYLE_LONG_DASH        ); // Long dashed style.
	case  3: return( wxPENSTYLE_SHORT_DASH       ); // Short dashed style.
	case  4: return( wxPENSTYLE_DOT_DASH         ); // Dot and dash style.
	case  5: return( wxPENSTYLE_BDIAGONAL_HATCH  ); // Backward diagonal hatch.
	case  6: return( wxPENSTYLE_CROSSDIAG_HATCH  ); // Cross-diagonal hatch.
	case  7: return( wxPENSTYLE_FDIAGONAL_HATCH  ); // Forward diagonal hatch.
	case  8: return( wxPENSTYLE_CROSS_HATCH      ); // Cross hatch.
	case  9: return( wxPENSTYLE_HORIZONTAL_HATCH ); // Horizontal hatch.
	case 10: return( wxPENSTYLE_VERTICAL_HATCH   ); // Vertical hatch.
//	case 11: return( wxPENSTYLE_STIPPLE          ); // Use the stipple bitmap. 
//	case 12: return( wxPENSTYLE_USER_DASH        ); // Use the user dashes: see wxPen::SetDashes.
//	case 13: return( wxPENSTYLE_TRANSPARENT      ); // No pen is used.
	}
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Shapes::Get_Class_Color(CSG_Shape *pShape, int &Color)
{
	if( m_Stretch.Value >= 0 )
	{
		if( m_pClassify->Get_Mode() == CLASSIFY_LUT && !SG_Data_Type_is_Numeric(Get_Shapes()->Get_Field_Type(m_Stretch.Value)) )
		{
			return( m_pClassify->Get_Class_Color_byValue(pShape->asString(m_Stretch.Value), Color) );
		}

		double Value;

		if( CWKSP_Layer::Get_Field_Value(pShape->Get_Index(), m_Stretch.Value, m_Stretch.Normal, m_Stretch.Scale, Value)
		    && m_pClassify->Get_Class_Color_byValue(Value, Color) )
		{
			return( true );
		}
	}

	//-----------------------------------------------------
	Color = m_pClassify->Get_Unique_Color();

	return( m_pClassify->Get_Mode() == CLASSIFY_SINGLE );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Shapes::_Chart_Set_Options(void)
{
	CSG_Parameters	*pChart	= m_Parameters("DISPLAY_CHART")->asParameters();

	pChart->Del_Parameters();
	m_Chart.Clear();

	if( 1 )
	{
		int	i, n;

		for(i=0, n=0; i<Get_Shapes()->Get_Field_Count(); i++)
		{
			if( Get_Shapes()->Get_Field_Type(i) != SG_DATATYPE_String )
			{
				n++;
			}
		}

		if( n > 0 )
		{
			pChart->Add_Choice("",
				"TYPE"			, _TL("Chart Type"),
				_TL(""),
				CSG_String::Format("%s|%s|%s|%s|",
					_TL("bar"),
					_TL("bar (not outlined)"),
					_TL("pie"),
					_TL("pie (not outlined)")
				), 0
			);

			pChart->Add_Choice("",
				"SIZE_FIELD"	, _TL("Attribute (Size)"),
				_TL(""),
				CSG_String::Format("%s|", _TL("<not set>")), 0
			);

			pChart->Add_Choice("",
				"SIZE_TYPE"		, _TL("Size relates to..."),
				_TL(""),
				CSG_String::Format("%s|%s|",
					_TL("Screen"),
					_TL("Map Units")
				), 0
			);

			pChart->Add_Double("",
				"SIZE_DEFAULT"	, _TL("Default Size"),
				_TL(""),
				15., 0., true
			);

			pChart->Add_Range("",
				"SIZE_RANGE"	, _TL("Size Range"),
				_TL(""),
				5, 25, 0, true
			);

			pChart->Add_Node("", "FIELDS", _TL("Fields"), _TL(""));

			CSG_String	Fields, Field;
			CSG_Colors	Colors(n);

			for(i=0, n=0; i<Get_Shapes()->Get_Field_Count(); i++)
			{
				if( Get_Shapes()->Get_Field_Type(i) != SG_DATATYPE_String )
				{
					Fields	+= CSG_String::Format("%s|", Get_Shapes()->Get_Field_Name(i));

					Field.Printf("FIELD_%d", i);

					pChart->Add_Bool("FIELDS", Field, Get_Shapes()->Get_Field_Name(i), _TL(""), false);
					pChart->Add_Color(Field, CSG_String::Format("COLOR_%d", i), SG_T(""), _TL(""), Colors.Get_Color(n++));
				}
			}

			Fields	+= CSG_String::Format("%s|", _TL("<none>"));

			(*pChart)("SIZE_FIELD")->asChoice()->Set_Items(Fields);
			(*pChart)("SIZE_FIELD")->Set_Value(n);

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Shapes::_Chart_Get_Options(void)
{
	m_Chart.Clear();
	m_Chart_sField	= -1;

	CSG_Parameters	*pChart	= m_Parameters("DISPLAY_CHART")->asParameters();

	if( (*pChart)("FIELDS") )
	{
		for(int i=0, n=0; i<Get_Shapes()->Get_Field_Count(); i++)
		{
			CSG_Parameter	*p	= (*pChart)(CSG_String::Format("FIELD_%d", i));

			if(	p )
			{
				if( (*pChart)("SIZE_FIELD")->asInt() == n++ )
					m_Chart_sField	= i;

				if( p->asBool() && (p = (*pChart)(CSG_String::Format("COLOR_%d", i))) != NULL )
					m_Chart.Add(i, p->asColor());
			}
		}

		m_Chart_Type	= (*pChart)("TYPE"        )->asInt();
		m_Chart_sType	= (*pChart)("SIZE_TYPE"   )->asInt();
		m_Chart_sSize	= m_Chart_sField < 0
						? (*pChart)("SIZE_DEFAULT")->asDouble()
						: (*pChart)("SIZE_RANGE"  )->asRange()->Get_Min();
		m_Chart_sRange	= (*pChart)("SIZE_RANGE"  )->asRange()->Get_Max() - m_Chart_sSize;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CWKSP_Shapes::_Draw_Chart(CSG_Map_DC &dc_Map, CSG_Shape *pShape)
{
	if( dc_Map.rWorld().Intersects(pShape->Get_Extent()) != INTERSECTION_None )
	{
		int			s;
		double		dSize	= m_Chart_sSize;
		TSG_Point_Int	p;

		if( m_Chart_sField >= 0 )
		{
			double	range	= Get_Shapes()->Get_Range(m_Chart_sField);

			if( range > 0. )
			{
				dSize	+= m_Chart_sRange * ((pShape->asDouble(m_Chart_sField) - Get_Shapes()->Get_Minimum(m_Chart_sField)) / range);
			}
		}

		s	= (int)(dSize * (m_Chart_sType == 1 ? dc_Map.World2DC() : dc_Map.Scale()));

		switch( pShape->Get_Type() )
		{
		default:					p	= dc_Map.World2DC(pShape->Get_Extent().Get_Center());			break;
		case SHAPE_TYPE_Polygon:	p	= dc_Map.World2DC(((CSG_Shape_Polygon *)pShape)->Get_Centroid());	break;
		}

		dc_Map.SetPen(*wxBLACK_PEN);

		switch( m_Chart_Type )
		{
		case 0:	_Draw_Chart_Bar(dc_Map, pShape,  true, p.x, p.y, (int)(0.8 * s), s);	break; // bar outlined
		case 1:	_Draw_Chart_Bar(dc_Map, pShape, false, p.x, p.y, (int)(0.8 * s), s);	break; // bar
		case 2:	_Draw_Chart_Pie(dc_Map, pShape,  true, p.x, p.y, (int)(1.0 * s));	break; // pie outlined
		case 3:	_Draw_Chart_Pie(dc_Map, pShape, false, p.x, p.y, (int)(1.0 * s));	break; // pie
		}
	}
}

//---------------------------------------------------------
void CWKSP_Shapes::_Draw_Chart_Pie(CSG_Map_DC &dc_Map, CSG_Table_Record *pRecord, bool bOutline, int x, int y, int size)
{
	int		i, ix, iy, jx, jy;
	double	d, sum;

	for(i=0, sum=0.; i<m_Chart.Get_Count(); i++)
	{
		sum	+= fabs(pRecord->asDouble(m_Chart[i].x));
	}

	if( sum > 0. )
	{
		sum = M_PI_360 / sum;

		for(i=0, jx=x, jy=y-size, d=0.; i<m_Chart.Get_Count(); i++)
		{
			if( !bOutline )
			dc_Map.SetPen  (wxPen  (Get_Color_asWX(m_Chart[i].y)));
			dc_Map.SetBrush(wxBrush(Get_Color_asWX(m_Chart[i].y)));

			d += sum * fabs(pRecord->asDouble(m_Chart[i].x));

			ix = jx; jx = x - (int)(size * sin(-d));
			iy = jy; jy = y - (int)(size * cos(-d));

			dc_Map.DrawArc(jx, jy, ix, iy, x, y);
		}
	}
}

//---------------------------------------------------------
void CWKSP_Shapes::_Draw_Chart_Bar(CSG_Map_DC &dc_Map, CSG_Table_Record *pRecord, bool bOutline, int x, int y, int sx, int sy)
{
	int		i;
	double	d, dx, dy, max, ix;

	y	+= sy / 2;

	for(i=1, max=fabs(pRecord->asDouble(m_Chart[0].x)); i<m_Chart.Get_Count(); i++)
	{
		if( (d = fabs(pRecord->asDouble(m_Chart[i].x))) > max )
			max	= d;
	}

	if( max != 0. )
	{
		max	= sy / max;
		dx	= sx / (double)m_Chart.Get_Count();

		for(i=0, ix=x-sx/2.; i<m_Chart.Get_Count(); i++, ix+=dx)
		{
			if( !bOutline )
			dc_Map.SetPen	(wxPen  (Get_Color_asWX(m_Chart[i].y)));
			dc_Map.SetBrush	(wxBrush(Get_Color_asWX(m_Chart[i].y)));

			dy	= -pRecord->asDouble(m_Chart[i].x) * max;

			dc_Map.DrawRectangle((int)ix, y, (int)dx, (int)dy);
		}

		if( !bOutline )
		{
			dc_Map.SetPen(*wxBLACK_PEN);
			dc_Map.DrawLine(x - sx / 2, y, x + sx / 2, y);
		}
	}
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

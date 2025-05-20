
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
//                  WKSP_PointCloud.cpp                  //
//                                                       //
//          Copyright (C) 2009 by Olaf Conrad            //
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
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "res_commands.h"
#include "res_controls.h"
#include "res_dialogs.h"

#include "helper.h"

#include "active.h"
#include "active_attributes.h"

#include "wksp_map_control.h"

#include "wksp_layer_classify.h"

#include "wksp_data_manager.h"

#include "wksp_pointcloud.h"
#include "wksp_table.h"
#include "view_table.h"
#include "view_scatterplot.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_PointCloud::CWKSP_PointCloud(CSG_PointCloud *pPointCloud)
	: CWKSP_Layer(pPointCloud)
{
	m_pTable = new CWKSP_Table(pPointCloud);

	m_Edit_Attributes.Destroy();
	m_Edit_Attributes.Add_Field(_TL("Name" ), SG_DATATYPE_String);
	m_Edit_Attributes.Add_Field(_TL("Value"), SG_DATATYPE_String);

	//-----------------------------------------------------
	On_Create_Parameters();

	DataObject_Changed();

	m_Parameters("COLORS_TYPE"  )->Set_Value(CLASSIFY_GRADUATED);
	m_Parameters("METRIC_ATTRIB")->Set_Value(2);

	CSG_Table &LUT = *m_Parameters("LUT")->asTable(); LUT.Del_Records();

	LUT.Set_Field_Type(LUT_MIN, SG_DATATYPE_String);
	LUT.Set_Field_Type(LUT_MAX, SG_DATATYPE_String);

	#define ADD_CLASS(color, name, value) { CSG_Table_Record &r = *LUT.Add_Record(); r.Set_Value(0, color); r.Set_Value(1, name); r.Set_Value(3, value); r.Set_Value(4, value); }

	ADD_CLASS(12632256, "Created, Never Classified"   ,  0);
	ADD_CLASS( 8421504, "Unclassified"                ,  1);
	ADD_CLASS( 4227327, "Ground"                      ,  2);
	ADD_CLASS(   65408, "Low Vegetation"              ,  3);
	ADD_CLASS(   54528, "Medium Vegetation"           ,  4);
	ADD_CLASS(   32768, "High Vegetation"             ,  5);
	ADD_CLASS(     255, "Building"                    ,  6);
	ADD_CLASS(   16512, "Low Point (Noise)"           ,  7);
	ADD_CLASS(16711808, "Model Key-Point (Mass Point)",  8);
	ADD_CLASS(16711680, "Water"                       ,  9);
	ADD_CLASS(16711935, "Overlap Points"              , 12);

	On_Parameter_Changed(&m_Parameters, m_Parameters("METRIC_ATTRIB"), PARAMETER_CHECK_ALL);

	Parameters_Changed();
}

//---------------------------------------------------------
CWKSP_PointCloud::~CWKSP_PointCloud(void)
{
	delete(m_pTable);
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_PointCloud::Get_Description(void)
{
	wxString s;

	//-----------------------------------------------------
	s += wxString::Format("<h4>%s</h4>", _TL("Point Cloud"));

	s += g_pData->Set_Description_Image(this);

	s += "<table border=\"0\">";

	DESC_ADD_STR  (_TL("Name"            ), m_pObject->Get_Name());
	DESC_ADD_STR  (_TL("Description"     ), m_pObject->Get_Description());

	if( SG_File_Exists(m_pObject->Get_File_Name(false)) )
	{
		DESC_ADD_STR(_TL("Data Source"   ), SG_File_Get_Path(m_pObject->Get_File_Name(false)      ).c_str());
		DESC_ADD_STR(_TL("File"          ), SG_File_Get_Name(m_pObject->Get_File_Name(false), true).c_str());
	}
	else
	{
		DESC_ADD_STR(_TL("Data Source"   ), _TL("memory"));
	}

	DESC_ADD_STR (_TL("Modified"         ), m_pObject->is_Modified() ? _TL("yes") : _TL("no"));
	DESC_ADD_STR (_TL("Spatial Reference"), m_pObject->Get_Projection().Get_Description().c_str());
	DESC_ADD_FLT (_TL("West"             ), Get_PointCloud()->Get_Extent().Get_XMin());
	DESC_ADD_FLT (_TL("East"             ), Get_PointCloud()->Get_Extent().Get_XMax());
	DESC_ADD_FLT (_TL("West-East"        ), Get_PointCloud()->Get_Extent().Get_XRange());
	DESC_ADD_FLT (_TL("South"            ), Get_PointCloud()->Get_Extent().Get_YMin());
	DESC_ADD_FLT (_TL("North"            ), Get_PointCloud()->Get_Extent().Get_YMax());
	DESC_ADD_FLT (_TL("South-North"      ), Get_PointCloud()->Get_Extent().Get_YRange());
	DESC_ADD_FLT (_TL("Z Minimum"        ), Get_PointCloud()->Get_ZMin());
	DESC_ADD_FLT (_TL("Z Maximum"        ), Get_PointCloud()->Get_ZMax());
	DESC_ADD_FLT (_TL("Z Range"          ), Get_PointCloud()->Get_ZMax() - Get_PointCloud()->Get_ZMin());
	DESC_ADD_LONG(_TL("Number of Points" ), Get_PointCloud()->Get_Count());
	DESC_ADD_LONG(_TL("Selected"         ), Get_PointCloud()->Get_Selection_Count());

	s += "</table>";

	s += wxString::Format("<hr><h4>%s</h4>", _TL("Coordinate System Details"));
	s += m_pObject->Get_Projection().Get_Description(true).c_str();

	s += Get_TableInfo_asHTML(Get_PointCloud());

	//-----------------------------------------------------
//	s += wxString::Format("<hr><b>%s</b><font size=\"-1\">", _TL("Data History"));
//	s += Get_PointCloud()->Get_History().Get_HTML();
//	s += wxString::Format("</font");

	//-----------------------------------------------------
	return( s );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxToolBarBase * CWKSP_PointCloud::Get_ToolBar(void)
{
	static wxToolBarBase *static_pToolBar = NULL;

	if( !static_pToolBar )
	{
		static_pToolBar = CMD_ToolBar_Create(ID_TB_DATA_POINTCLOUD);

		Add_ToolBar_Defaults(static_pToolBar);
		CMD_ToolBar_Add_Item(static_pToolBar,  true, ID_CMD_TABLE_SHOW);

		CMD_ToolBar_Add(static_pToolBar, _TL("Point Cloud"));
	}

	return( static_pToolBar );
}

//---------------------------------------------------------
wxMenu * CWKSP_PointCloud::Get_Menu(void)
{
	wxMenu *pSubMenu, *pMenu = new wxMenu(m_pObject->Get_Name());

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_CLOSE);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_SHOW_MAP);
	if( MDI_Get_Active_Map() )
	{
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAP_ZOOM_ACTIVE);
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAP_PAN_ACTIVE);
	}

	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_SAVE);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_SAVEAS);

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
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_SETTINGS_COPY);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_FORCE_UPDATE);

	pMenu->Append(ID_CMD_WKSP_FIRST, _TL("Color Stretch"), pSubMenu = new wxMenu());
	CMD_Menu_Add_Item(pSubMenu, false, ID_CMD_DATA_RANGE_MINMAX   );
	CMD_Menu_Add_Item(pSubMenu, false, ID_CMD_DATA_RANGE_STDDEV150);
	CMD_Menu_Add_Item(pSubMenu, false, ID_CMD_DATA_RANGE_STDDEV200);

	//-----------------------------------------------------
	pMenu->AppendSeparator();

	pMenu->Append(ID_CMD_WKSP_FIRST, _TL("Attributes"), pSubMenu = new wxMenu());
	CMD_Menu_Add_Item(pSubMenu,  true, ID_CMD_TABLE_SHOW);
//	CMD_Menu_Add_Item(pSubMenu, false, ID_CMD_SHAPES_SAVE_ATTRIBUTES);

	pMenu->Append(ID_CMD_WKSP_FIRST, _TL("Charts"    ), pSubMenu = new wxMenu());
	CMD_Menu_Add_Item(pSubMenu,  true, ID_CMD_DATA_HISTOGRAM);
//	CMD_Menu_Add_Item(pSubMenu, false, ID_CMD_DATA_SCATTERPLOT);
//	CMD_Menu_Add_Item(pSubMenu,  true, ID_CMD_DATA_DIAGRAM);

	pMenu->Append(ID_CMD_WKSP_FIRST, _TL("Selection" ), pSubMenu = new wxMenu());
	if( MDI_Get_Active_Map() && Get_PointCloud()->Get_Selection_Count() > 0 )
	{
		CMD_Menu_Add_Item(pSubMenu, false, ID_CMD_MAP_ZOOM_SELECTION);
		CMD_Menu_Add_Item(pSubMenu, false, ID_CMD_MAP_PAN_SELECTION);
	}
	CMD_Menu_Add_Item(pSubMenu,  true, ID_CMD_DATA_SELECTION_CLEAR);
	CMD_Menu_Add_Item(pSubMenu, false, ID_CMD_DATA_SELECTION_INVERT);
	CMD_Menu_Add_Item(pSubMenu,  true, ID_CMD_TABLE_SELECT_NUMERIC);
	CMD_Menu_Add_Item(pSubMenu,  true, ID_CMD_TABLE_SELECT_STRING);

	return( pMenu );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_PointCloud::On_Command(int Cmd_ID)
{
	switch( Cmd_ID )
	{
	default:
		return( CWKSP_Layer::On_Command(Cmd_ID) );

	case ID_CMD_POINTCLOUD_LAST:
		break;

	case ID_CMD_DATA_RANGE_MINMAX:
		Set_Color_Range(
			Get_PointCloud()->Get_Minimum(m_fValue),
			Get_PointCloud()->Get_Maximum(m_fValue)
		);
		break;

	case ID_CMD_DATA_RANGE_STDDEV150:
		Set_Color_Range(
			Get_PointCloud()->Get_Mean(m_fValue) - 1.5 * Get_PointCloud()->Get_StdDev(m_fValue),
			Get_PointCloud()->Get_Mean(m_fValue) + 1.5 * Get_PointCloud()->Get_StdDev(m_fValue)
		);
		break;

	case ID_CMD_DATA_RANGE_STDDEV200:
		Set_Color_Range(
			Get_PointCloud()->Get_Mean(m_fValue) - 2. * Get_PointCloud()->Get_StdDev(m_fValue),
			Get_PointCloud()->Get_Mean(m_fValue) + 2. * Get_PointCloud()->Get_StdDev(m_fValue)
		);
		break;

	//-----------------------------------------------------
	case ID_CMD_DATA_SELECTION_CLEAR : Get_PointCloud()->Select       (); Update_Views(); break;
	case ID_CMD_DATA_SELECTION_INVERT: Get_PointCloud()->Inv_Selection(); Update_Views(); break;

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
	case ID_CMD_SHAPES_EDIT_DEL_SHAPE:
		if( Get_PointCloud()->Get_Selection_Count() > 0 && DLG_Message_Confirm(_TL("Delete selected point(s)."), _TL("Edit Point Cloud")) )
		{
			Get_PointCloud()->Del_Selection();

			Update_Views();
		}
		break;

    case ID_CMD_SHAPES_EDIT_SEL_COPY:
        if( Get_PointCloud()->Get_Selection_Count() > 0 )
        {
            CSG_PointCloud *pCopy = new CSG_PointCloud(Get_PointCloud());

            pCopy->Set_Name(CSG_String::Format(SG_T("%s [%s]"), Get_PointCloud()->Get_Name(), _TL("Selection")));

            for(sLong i=0; i<Get_PointCloud()->Get_Selection_Count() && SG_UI_Process_Set_Progress(i, Get_PointCloud()->Get_Selection_Count()); i++)
            {
                pCopy->Add_Shape(Get_PointCloud()->Get_Selection(i), SHAPE_COPY);
            }

            SG_UI_Process_Set_Okay();

            SG_UI_DataObject_Add(pCopy, false);
        }
        break;

	//-----------------------------------------------------
	case ID_CMD_TABLE_SHOW    : m_pTable->Toggle_View   (); break;
	case ID_CMD_DATA_DIAGRAM  : m_pTable->Toggle_Diagram(); break;
	case ID_CMD_DATA_HISTOGRAM: Histogram_Toggle        (); break;
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_PointCloud::On_Command_UI(wxUpdateUIEvent &event)
{
	switch( event.GetId() )
	{
	default:
		return( CWKSP_Layer::On_Command_UI(event) );

    case ID_CMD_DATA_SELECTION_CLEAR:
        event.Enable(Get_PointCloud()->Get_Selection_Count() > 0);
        break;

    case ID_CMD_DATA_SELECTION_INVERT:
        event.Enable(true);
        break;

    case ID_CMD_SHAPES_EDIT_DEL_SHAPE:
        event.Enable(Get_PointCloud()->Get_Selection_Count() > 0);
        break;

    case ID_CMD_SHAPES_EDIT_SEL_COPY:
        event.Enable(Get_PointCloud()->Get_Selection_Count() > 0);
        break;

	case ID_CMD_POINTCLOUD_LAST:
		break;

	case ID_CMD_TABLE_SHOW    : event.Check(m_pTable->Get_View   () != NULL); break;
	case ID_CMD_DATA_DIAGRAM  : event.Check(m_pTable->Get_Diagram() != NULL); break;
	case ID_CMD_DATA_HISTOGRAM: event.Check(m_pHistogram            != NULL); break;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_PointCloud::On_Create_Parameters(void)
{
	CWKSP_Layer::On_Create_Parameters();

	//-----------------------------------------------------
	m_Parameters.Add_Int("NODE_DISPLAY",
		"DISPLAY_SIZE"	, _TL("Point Size"),
		_TL("Radius given as number of pixels."),
		1, 1, true, 10, true
	);

	m_Parameters.Add_Choice("NODE_DISPLAY",
		"DISPLAY_VALUE_AGGREGATE", _TL("Value Aggregation"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("first value"),
			_TL("last value" ),
			_TL("lowest z"   ),
			_TL("highest z"  )
		), 3
	);

	//-----------------------------------------------------
	m_Parameters.Add_Node("NODE_COLORS", "NODE_RGB", _TL("RGB"), _TL(""));

	m_Parameters.Add_Choice("NODE_RGB",
		"RGB_ATTRIB"	, _TL("Attribute"),
		_TL(""),
		_TL("<default>")
	);

	//-----------------------------------------------------
	// Memory...

	m_Parameters.Add_Double("NODE_GENERAL",
		"MAX_SAMPLES"	, _TL("Maximum Samples"),
		_TL("Maximum number of samples used to build statistics and histograms expressed as percent of the total number of cells."),
		100. * m_pObject->Get_Max_Samples() / (double)Get_PointCloud()->Get_Count(), 0., true, 100., true
	);
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_PointCloud::On_DataObject_Changed(void)
{
	if( m_fValue >= Get_PointCloud()->Get_Field_Count() )
	{
		m_fValue = Get_PointCloud()->Get_Field_Count() - 1;
	}

	double m = Get_PointCloud()->Get_Mean(m_fValue), s = 2. * Get_PointCloud()->Get_StdDev(m_fValue);

	m_Parameters("METRIC_ZRANGE")->asRange()->Set_Range(m - s, m + s);

	//-----------------------------------------------------
	_AttributeList_Set(m_Parameters("LUT_FIELD"    ), false);
	_AttributeList_Set(m_Parameters("METRIC_ATTRIB"), false);
	_AttributeList_Set(m_Parameters("RGB_ATTRIB"   ), false);

	//-----------------------------------------------------
	m_Parameters.Set_Parameter("MAX_SAMPLES", 100. * m_pObject->Get_Max_Samples() / (double)Get_PointCloud()->Get_Count());

	//-----------------------------------------------------
	CWKSP_Layer::On_DataObject_Changed();

	m_pTable->DataObject_Changed();
}

//---------------------------------------------------------
void CWKSP_PointCloud::On_Parameters_Changed(void)
{
	CWKSP_Layer::On_Parameters_Changed();

	//-----------------------------------------------------
	switch( m_Parameters("COLORS_TYPE")->asInt() )
	{
	default: m_fValue = -1                                    ; break; // CLASSIFY_SINGLE
	case  1: m_fValue = m_Parameters("LUT_FIELD"   )->asInt(); break; // CLASSIFY_LUT
	case  2: m_fValue = m_Parameters("METRIC_ATTRIB")->asInt(); break; // CLASSIFY_DISCRETE
	case  3: m_fValue = m_Parameters("METRIC_ATTRIB")->asInt(); break; // CLASSIFY_GRADUATED
	case  4: m_fValue = m_Parameters("RGB_ATTRIB"   )->asInt(); break; // CLASSIFY_RGB
	}

	if( m_fValue < 0 || m_fValue >= Get_PointCloud()->Get_Field_Count() )
	{
		m_fValue = -1;

		m_pClassify->Set_Mode(CLASSIFY_SINGLE);
	}
	else if( m_Parameters("COLORS_TYPE")->asInt() == 4 ) // CLASSIFY_RGB
	{
		m_pClassify->Set_Mode(CLASSIFY_RGB);
	}

	//-----------------------------------------------------
	m_pObject->Set_Max_Samples(Get_PointCloud()->Get_Count() * (m_Parameters("MAX_SAMPLES")->asDouble() / 100.) );

	long DefColor = m_Parameters("SINGLE_COLOR")->asColor();
	m_Color_Pen   = wxColour(SG_GET_R(DefColor), SG_GET_G(DefColor), SG_GET_B(DefColor));

	m_PointSize   = m_Parameters("DISPLAY_SIZE")->asInt() - 1;
}

//---------------------------------------------------------
void CWKSP_PointCloud::On_Update_Views(void)
{
	m_pTable->Update_Views();
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CWKSP_PointCloud::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter, int Flags)
{
	//-----------------------------------------------------
	if( Flags & PARAMETER_CHECK_VALUES )
	{
		if(	pParameter->Cmp_Identifier("METRIC_ATTRIB") )
		{
			int zField = pParameter->asInt(); double m = Get_PointCloud()->Get_Mean(zField), s = 2. * Get_PointCloud()->Get_StdDev(zField);

			double min = m - s;	if( min < Get_PointCloud()->Get_Minimum(zField) ) { min = Get_PointCloud()->Get_Minimum(zField); }
			double max = m + s;	if( max > Get_PointCloud()->Get_Maximum(zField) ) { max = Get_PointCloud()->Get_Maximum(zField); }

			pParameters->Get_Parameter("METRIC_ZRANGE")->asRange()->Set_Range(min, max);
		}
	}

	//-----------------------------------------------------
	if( Flags & PARAMETER_CHECK_ENABLE )
	{
	}

	//-----------------------------------------------------
	return( CWKSP_Layer::On_Parameter_Changed(pParameters, pParameter, Flags) );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_PointCloud::Get_Name_Attribute(void)
{
	return(	m_fValue < 0 || m_pClassify->Get_Mode() == CLASSIFY_SINGLE ? SG_T("") : Get_PointCloud()->Get_Field_Name(m_fValue) );
}

//---------------------------------------------------------
void CWKSP_PointCloud::_AttributeList_Set(CSG_Parameter *pFields, bool bAddNoField)
{
	if( pFields && pFields->Get_Type() == PARAMETER_TYPE_Choice )
	{
		CSG_String s;

		for(int i=0; i<Get_PointCloud()->Get_Field_Count(); i++)
		{
			s += Get_PointCloud()->Get_Field_Name(i); s += "|";
		}

		if( bAddNoField )
		{
			s += _TL("<none>");

			pFields->asChoice()->Set_Items(s);

			pFields->Set_Value(Get_PointCloud()->Get_Field_Count());
		}
		else
		{
			pFields->asChoice()->Set_Items(s);
		}
	}
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_PointCloud::Get_Value(CSG_Point ptWorld, double Epsilon)
{
	wxString Value; sLong Index = Get_PointCloud()->Get_Point(ptWorld, Epsilon);

	if( Index >= 0 )
	{
		switch( m_fValue < 0 ? -1 : m_pClassify->Get_Mode() )
		{
		case -1               :
			Value.Printf("%s: %lld", _TL("Index"), Index + 1);
			break;

		default               :
			Value.Printf("%f", Get_PointCloud()->Get_Value(Index, m_fValue));
			break;

		case CLASSIFY_RGB     : { int color = (int)Get_PointCloud()->Get_Value(Index, m_fValue);
			Value.Printf("R%03d G%03d B%03d", SG_GET_R(color), SG_GET_G(color), SG_GET_B(color));
			break; }

		case CLASSIFY_LUT     :
			if( SG_Data_Type_is_Numeric(Get_PointCloud()->Get_Field_Type(m_fValue)) )
			{
				Value = m_pClassify->Get_Class_Name_byValue(Get_PointCloud()->Get_Value(Index, m_fValue));
			}
			else
			{
				CSG_String s; Get_PointCloud()->Get_Value(Index, m_fValue, s);

				Value = m_pClassify->Get_Class_Name_byValue(s.c_str());
			}
			break;
		}
	}

	return( Value );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CWKSP_PointCloud::Get_Value_Minimum(void)	{	return( m_fValue < 0 ? 0. : Get_PointCloud()->Get_Minimum(m_fValue) );	}
double CWKSP_PointCloud::Get_Value_Maximum(void)	{	return( m_fValue < 0 ? 0. : Get_PointCloud()->Get_Maximum(m_fValue) );	}
double CWKSP_PointCloud::Get_Value_Range  (void)	{	return( m_fValue < 0 ? 0. : Get_PointCloud()->Get_Range  (m_fValue) );	}
double CWKSP_PointCloud::Get_Value_Mean   (void)	{	return( m_fValue < 0 ? 0. : Get_PointCloud()->Get_Mean   (m_fValue) );	}
double CWKSP_PointCloud::Get_Value_StdDev (void)	{	return( m_fValue < 0 ? 0. : Get_PointCloud()->Get_StdDev (m_fValue) );	}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_PointCloud::asImage(CSG_Grid *pImage)
{
	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxMenu * CWKSP_PointCloud::Edit_Get_Menu(void)
{
	wxMenu *pMenu = new wxMenu;

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_SHAPES_EDIT_DEL_SHAPE);
    CMD_Menu_Add_Item(pMenu, false, ID_CMD_SHAPES_EDIT_SEL_COPY );
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_SELECTION_CLEAR );
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_SELECTION_INVERT);

	return( pMenu );
}

//---------------------------------------------------------
TSG_Rect CWKSP_PointCloud::Edit_Get_Extent(void)
{
	if( Get_PointCloud()->Get_Selection_Count() > 0 )
	{
		return( Get_PointCloud()->Get_Selection_Extent() );
	}

	return( Get_PointCloud()->Get_Extent() );
}

//---------------------------------------------------------
bool CWKSP_PointCloud::Edit_On_Mouse_Up(const CSG_Point &Point, double ClientToWorld, int Key)
{
	if( Key & TOOL_INTERACTIVE_KEY_RIGHT )
	{
		return( false );
	}
	else
	{
		CSG_Rect rWorld(m_Edit_Mouse_Down, Point);

		if( rWorld.Get_XRange() == 0. && rWorld.Get_YRange() == 0. )
		{
			rWorld.Inflate(2. * ClientToWorld, false);
		}

		g_pActive->Update_Attributes();

		Get_PointCloud()->Select(rWorld, (Key & TOOL_INTERACTIVE_KEY_CTRL) != 0);

		//-----------------------------------------------------
		m_Edit_Attributes.Del_Records();
		m_Edit_Index = 0;

		CSG_Table_Record *pRecord = Get_PointCloud()->Get_Selection();

		if( pRecord != NULL )
		{
			for(int i=0; i<Get_PointCloud()->Get_Field_Count(); i++)
			{
				CSG_Table_Record *pAttribute = m_Edit_Attributes.Add_Record();

				pAttribute->Set_Value(0, pRecord->Get_Table()->Get_Field_Name(i));
				pAttribute->Set_Value(1, pRecord->asString(i));
			}
		}

		//-----------------------------------------------------
		g_pActive->Update_Attributes();

		if( m_pTable->Get_View() )
		{
			m_pTable->Get_View()->Update_Selection();
		}

		Update_Views(false);
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_PointCloud::Edit_Set_Index(int Index)
{
	m_Edit_Attributes.Del_Records();

	if( Index > (int)Get_PointCloud()->Get_Selection_Count() )
	{
		Index = Get_PointCloud()->Get_Selection_Count();
	}

	CSG_Table_Record *pSelection = Get_PointCloud()->Get_Selection(Index);

	if( pSelection )
	{
		m_Edit_Index = Index;

		for(int i=0; i<Get_PointCloud()->Get_Field_Count(); i++)
		{
			CSG_Table_Record *pRecord = m_Edit_Attributes.Add_Record();

			pRecord->Set_Value(0, pSelection->Get_Table()->Get_Field_Name(i));
			pRecord->Set_Value(1, pSelection->asString(i));
		}
	}
	else
	{
		m_Edit_Index = 0;
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_PointCloud::Edit_Set_Attributes(void)
{
	CSG_Table_Record *pSelection = Get_PointCloud()->Get_Selection(m_Edit_Index);

	if( pSelection )
	{
		for(sLong i=0; i<m_Edit_Attributes.Get_Count(); i++)
		{
			pSelection->Set_Value(i, m_Edit_Attributes.Get_Record(i)->asString(1));
		}

		Update_Views(false);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_PointCloud::On_Draw(CSG_Map_DC &dc_Map, int Flags)
{
	if( Get_Extent().Intersects(dc_Map.rWorld()) && dc_Map.Draw_Image_Begin(m_Parameters("DISPLAY_TRANSPARENCY")->asDouble() / 100.) )
	{
		if( (Flags & LAYER_DRAW_FLAG_THUMBNAIL) == 0 )
		{
			_Draw_Points   (dc_Map);
		}
		else
		{
			_Draw_Thumbnail(dc_Map);
		}

		dc_Map.Draw_Image_End();
	}
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline void CWKSP_PointCloud::_Draw_Point(CSG_Map_DC &dc_Map, int x, int y, double z, int Color)
{
	if( m_Aggregation == 1 )	// last value
	{
		dc_Map.Draw_Image_Pixel(x, y, Color);
	}
	else if( m_Z.is_InGrid(x, y) )
	{
		switch( m_Aggregation )
		{
		case 0:	// first value
			if( m_N.asInt(x, y) == 0 )
			{
				dc_Map.Draw_Image_Pixel(x, y, Color);
			}
			break;

		case 2:	// lowest z
			if( m_N.asInt(x, y) == 0 || z < m_Z.asDouble(x, y) )
			{
				dc_Map.Draw_Image_Pixel(x, y, Color);
				m_Z.Set_Value(x, y, z);
			}
			break;

		case 3:	// highest z
			if( m_N.asInt(x, y) == 0 || z > m_Z.asDouble(x, y) )
			{
				dc_Map.Draw_Image_Pixel(x, y, Color);
				m_Z.Set_Value(x, y, z);
			}
			break;
		}

		m_N.Add_Value(x, y, 1);
	}
}

//---------------------------------------------------------
void CWKSP_PointCloud::_Draw_Point(CSG_Map_DC &dc_Map, int x, int y, double z, int Color, int Radius)
{
	_Draw_Point(dc_Map, x, y, z, Color);

	for(int iy=1; iy<=Radius; iy++)
	{
		for(int ix=0; ix<=Radius; ix++)
		{
			if( ix*ix + iy*iy <= Radius*Radius )
			{
				_Draw_Point(dc_Map, x + ix, y + iy, z, Color);
				_Draw_Point(dc_Map, x + iy, y - ix, z, Color);
				_Draw_Point(dc_Map, x - ix, y - iy, z, Color);
				_Draw_Point(dc_Map, x - iy, y + ix, z, Color);
			}
		}
	}
}

//---------------------------------------------------------
void CWKSP_PointCloud::_Draw_Points(CSG_Map_DC &dc_Map)
{
	m_Aggregation = m_Parameters("DISPLAY_VALUE_AGGREGATE")->asInt();

	if( m_Aggregation != 1 )
	{
		m_Z.Create(SG_DATATYPE_Double, dc_Map.rDC().GetWidth(), dc_Map.rDC().GetHeight());
		m_N.Create(SG_DATATYPE_Int   , dc_Map.rDC().GetWidth(), dc_Map.rDC().GetHeight());
	}

	//-----------------------------------------------------
	CSG_PointCloud *pPoints = Get_PointCloud();

	sLong Selection = pPoints->Get_Selection_Count() > 0 ? pPoints->Get_Selection_Index(m_Edit_Index) : -1;

	for(sLong i=0; i<pPoints->Get_Count(); i++)
	{
		pPoints->Set_Cursor(i);

		if( !pPoints->is_NoData(m_fValue) )
		{
			TSG_Point_3D Point = pPoints->Get_Point();

			if( dc_Map.rWorld().Contains(Point.x, Point.y) )
			{
				int x = (int)dc_Map.xWorld2DC(Point.x);
				int y = (int)dc_Map.yWorld2DC(Point.y);

				if( Selection >= 0 && pPoints->is_Selected(i) )
				{
					int Size = Selection == i ? 2 + m_PointSize : m_PointSize;

					_Draw_Point(dc_Map, x, y, Point.z, SG_COLOR_YELLOW, Size    );
					_Draw_Point(dc_Map, x, y, Point.z, SG_COLOR_RED   , Size + 2);
				}
				else
				{
					int Color;
					
					if( m_pClassify->Get_Mode() == CLASSIFY_LUT && !SG_Data_Type_is_Numeric(pPoints->Get_Field_Type(m_fValue)) )
					{
						CSG_String Value;

						if( pPoints->Get_Value(m_fValue, Value) && m_pClassify->Get_Class_Color_byValue(Value, Color) )
						{
							_Draw_Point(dc_Map, x, y, Point.z, Color, m_PointSize);
						}
					}
					else if( m_pClassify->Get_Class_Color_byValue(pPoints->Get_Value(m_fValue), Color) )
					{
						_Draw_Point(dc_Map, x, y, Point.z, Color, m_PointSize);
					}
				}
			}
		}
	}
}

//---------------------------------------------------------
void CWKSP_PointCloud::_Draw_Thumbnail(CSG_Map_DC &dc_Map)
{
	CSG_PointCloud *pPoints = Get_PointCloud();

	sLong n = 1 + (sLong)(pPoints->Get_Count() / (2 * dc_Map.rDC().GetWidth() * dc_Map.rDC().GetHeight()));

	for(sLong i=0; i<pPoints->Get_Count(); i+=n)
	{
		pPoints->Set_Cursor(i);

		if( !pPoints->is_NoData(m_fValue) )
		{
			TSG_Point_3D Point = pPoints->Get_Point();

			int x = (int)dc_Map.xWorld2DC(Point.x);
			int y = (int)dc_Map.yWorld2DC(Point.y);

			int Color;

			if( m_pClassify->Get_Class_Color_byValue(pPoints->Get_Value(m_fValue), Color) )
			{
				dc_Map.Draw_Image_Pixel(x, y, Color);
			}
		}
	}
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

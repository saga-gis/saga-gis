
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
#include "res_commands.h"
#include "res_dialogs.h"

#include "helper.h"
#include "dc_helper.h"

#include "active.h"

#include "wksp_layer_classify.h"

#include "wksp_data_manager.h"

#include "wksp_shapes.h"
#include "wksp_table.h"

#include "data_source_pgsql.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Shapes::CWKSP_Shapes(CSG_Shapes *pShapes)
	: CWKSP_Layer(pShapes)
{
	m_pTable		= new CWKSP_Table(pShapes);

	m_fInfo			= -1;

	m_Edit_Shapes.Create(pShapes->Get_Type());
	m_Edit_pShape	= NULL;

	m_Edit_Attributes.Destroy();
	m_Edit_Attributes.Add_Field(_TL("Name" ), SG_DATATYPE_String);
	m_Edit_Attributes.Add_Field(_TL("Value"), SG_DATATYPE_String);

	m_Edit_Color    = *wxBLACK;
	m_Edit_bGleam	= true;
	m_Edit_Mode		= EDIT_SHAPE_MODE_Normal;

	m_Sel_Color		= *wxRED;

	m_bVertices		= 0;
}

//---------------------------------------------------------
CWKSP_Shapes::~CWKSP_Shapes(void)
{
	delete(m_pTable);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_Shapes::Get_Description(void)
{
	wxString	s;

	//-----------------------------------------------------
	s	+= wxString::Format("<h4>%s</h4>", _TL("Shapes"));

	s	+= "<table border=\"0\">";

	DESC_ADD_STR(_TL("Name"            ), m_pObject->Get_Name());
	DESC_ADD_STR(_TL("Description"     ), m_pObject->Get_Description());

	if( SG_File_Exists(m_pObject->Get_File_Name(false)) )
	{
		DESC_ADD_STR(_TL("File"        ), m_pObject->Get_File_Name(false));

		if( m_pObject->Get_MetaData()("GDAL_DRIVER") )
			DESC_ADD_STR(_TL("Driver"  ), m_pObject->Get_MetaData()["GDAL_DRIVER"].Get_Content().c_str());
	}
	else if( m_pObject->Get_MetaData_DB().Get_Children_Count() )
	{
		DESC_ADD_STR(_TL("File"        ), m_pObject->Get_File_Name(false));
	}
	else
	{
		DESC_ADD_STR(_TL("File"        ), _TL("memory"));
	}

	DESC_ADD_STR  (_TL("Modified"        ), m_pObject->is_Modified() ? _TL("yes") : _TL("no"));
	DESC_ADD_STR  (_TL("Projection"      ), m_pObject->Get_Projection().Get_Description().c_str());
	DESC_ADD_FLT  (_TL("West"            ), Get_Shapes()->Get_Extent().Get_XMin  ());
	DESC_ADD_FLT  (_TL("East"            ), Get_Shapes()->Get_Extent().Get_XMax  ());
	DESC_ADD_FLT  (_TL("West-East"       ), Get_Shapes()->Get_Extent().Get_XRange());
	DESC_ADD_FLT  (_TL("South"           ), Get_Shapes()->Get_Extent().Get_YMin  ());
	DESC_ADD_FLT  (_TL("North"           ), Get_Shapes()->Get_Extent().Get_YMax  ());
	DESC_ADD_FLT  (_TL("South-North"     ), Get_Shapes()->Get_Extent().Get_YRange());
	DESC_ADD_STR  (_TL("Type"            ), SG_Get_ShapeType_Name(Get_Shapes()->Get_Type()).c_str());
	DESC_ADD_STR  (_TL("Vertex Type"     ), Get_Shapes()->Get_Vertex_Type() == 0 ? _TL("X, Y") : Get_Shapes()->Get_Vertex_Type() == 1 ? _TL("X, Y, Z") : _TL("X, Y, Z, M"));
	DESC_ADD_INT  (_TL("Number of Shapes"), Get_Shapes()->Get_Count());
	DESC_ADD_SIZET(_TL("Selected"        ), Get_Shapes()->Get_Selection_Count());
	DESC_ADD_STR  (_TL("File Encoding"   ), Get_Shapes()->Get_File_Encoding() ? SG_T("UTF-8") : SG_T("ANSI"));

	s	+= "</table>";

	s	+= Get_TableInfo_asHTML(Get_Shapes());

	//-----------------------------------------------------
//	s	+= wxString::Format(wxT("<hr><b>%s</b><font size=\"-1\">"), _TL("Data History"));
//	s	+= Get_Shapes()->Get_History().Get_HTML();
//	s	+= wxString::Format(wxT("</font"));

	return( s );
}

//---------------------------------------------------------
wxMenu * CWKSP_Shapes::Get_Menu(void)
{
	wxMenu	*pMenu	= new wxMenu(m_pObject->Get_Name());

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_CLOSE);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_SHAPES_SHOW);

	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_SAVE);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_SAVEAS);

	if( PGSQL_has_Connections() )
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_SAVETODB);

	if( m_pObject->is_File_Native() && m_pObject->is_Modified() )
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_RELOAD);

	if( m_pObject->is_File_Native() )
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_DEL_FILES);

	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_PROJECTION);

	if( m_pObject->Get_MetaData().Get_Children_Count() > 0 )
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_METADATA);

	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu,  true, ID_CMD_SHAPES_HISTOGRAM);

	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_SHAPES_SET_LUT);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_CLASSIFY_IMPORT);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_SETTINGS_COPY);

	pMenu->AppendSeparator();
	wxMenu	*pTable	= new wxMenu(_TL("Attributes"));
	CMD_Menu_Add_Item(pTable,  true, ID_CMD_TABLE_SHOW);
	CMD_Menu_Add_Item(pTable,  true, ID_CMD_TABLE_DIAGRAM);
	CMD_Menu_Add_Item(pTable, false, ID_CMD_TABLE_SCATTERPLOT);
	CMD_Menu_Add_Item(pTable, false, ID_CMD_SHAPES_SAVE_ATTRIBUTES);
	pMenu->Append(ID_CMD_WKSP_FIRST, _TL("Attributes"), pTable);

	pMenu->Append(ID_CMD_WKSP_FIRST, _TL("Edit"), Edit_Get_Menu());

	return( pMenu );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Shapes::On_Command(int Cmd_ID)
{
	switch( Cmd_ID )
	{
	default:
		return( CWKSP_Layer::On_Command(Cmd_ID) );

	//-----------------------------------------------------
	case ID_CMD_SHAPES_SET_LUT       :	_LUT_Create     ();	break;
	case ID_CMD_DATA_CLASSIFY_IMPORT :  _LUT_Import     ();	break;
	case ID_CMD_SHAPES_HISTOGRAM     :	Histogram_Toggle();	break;

	//-----------------------------------------------------
	case ID_CMD_SHAPES_EDIT_SHAPE    :	_Edit_Shape    ();	break;
	case ID_CMD_SHAPES_EDIT_ADD_SHAPE:	_Edit_Shape_Add();	break;
	case ID_CMD_SHAPES_EDIT_DEL_SHAPE:	_Edit_Shape_Del();	break;
	case ID_CMD_SHAPES_EDIT_ADD_PART :	_Edit_Part_Add ();	break;
	case ID_CMD_SHAPES_EDIT_DEL_PART :	_Edit_Part_Del ();	break;
	case ID_CMD_SHAPES_EDIT_DEL_POINT:	_Edit_Point_Del();	break;
	case ID_CMD_SHAPES_EDIT_MERGE    :	_Edit_Merge    ();	break;
	case ID_CMD_SHAPES_EDIT_SPLIT    :	_Edit_Split    ();	break;
	case ID_CMD_SHAPES_EDIT_MOVE     :	_Edit_Move     ();	break;

	case ID_CMD_SHAPES_EDIT_SEL_CLEAR:
		Get_Shapes()->Select();
		Update_Views();
		break;

	case ID_CMD_SHAPES_EDIT_SEL_INVERT:
		Get_Shapes()->Inv_Selection();
		Update_Views();
		break;

	//-----------------------------------------------------
	case ID_CMD_TABLE_SHOW   :	m_pTable->Toggle_View   ();	break;
	case ID_CMD_TABLE_DIAGRAM:	m_pTable->Toggle_Diagram();	break;

	case ID_CMD_TABLE_SCATTERPLOT:
		Add_ScatterPlot();
		break;

	case ID_CMD_SHAPES_SAVE_ATTRIBUTES:
		{
			wxString	File(m_pObject->Get_File_Name());

			if( DLG_Save(File, ID_DLG_TABLE_SAVE) )
			{
				CSG_Table	Table(*((CSG_Table *)m_pObject));

				Table.Save(&File);
			}
		}
		break;
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Shapes::On_Command_UI(wxUpdateUIEvent &event)
{
	switch( event.GetId() )
	{
	default:
		return( CWKSP_Layer::On_Command_UI(event) );

	case ID_CMD_SHAPES_EDIT_SHAPE:
		event.Enable(Get_Shapes()->Get_Selection_Count() > 0 || m_Edit_pShape != NULL);
		event.Check(m_Edit_pShape != NULL);
		break;

	case ID_CMD_SHAPES_EDIT_ADD_SHAPE:
		event.Enable(m_Edit_pShape == NULL);
		break;

	case ID_CMD_SHAPES_EDIT_DEL_SHAPE:
		event.Enable(Get_Shapes()->Get_Selection_Count() > 0 && m_Edit_pShape == NULL);
		break;

	case ID_CMD_SHAPES_EDIT_ADD_PART:
		event.Enable(m_Edit_pShape != NULL);
		break;

	case ID_CMD_SHAPES_EDIT_DEL_PART:
		event.Enable(m_Edit_pShape != NULL && m_Edit_iPart >= 0);
		break;

	case ID_CMD_SHAPES_EDIT_DEL_POINT:
		event.Enable(m_Edit_pShape != NULL && m_Edit_iPart >= 0 && m_Edit_iPoint >= 0);
		break;

	case ID_CMD_SHAPES_EDIT_SEL_CLEAR:
		event.Enable(m_Edit_pShape == NULL && Get_Shapes()->Get_Selection_Count() > 0);
		break;

	case ID_CMD_SHAPES_EDIT_SEL_INVERT:
		event.Enable(m_Edit_pShape == NULL);
		break;

	case ID_CMD_TABLE_SHOW:
		event.Check(m_pTable->Get_View() != NULL);
		break;

	case ID_CMD_TABLE_DIAGRAM:
		event.Check(m_pTable->Get_Diagram() != NULL);
		break;

	case ID_CMD_SHAPES_HISTOGRAM:
		event.Check(m_pHistogram != NULL);
		break;

	case ID_CMD_SHAPES_EDIT_MOVE:
		event.Check(m_Edit_Mode == EDIT_SHAPE_MODE_Move);
		break;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Shapes::On_Create_Parameters(void)
{
	CWKSP_Layer::On_Create_Parameters();

	//-----------------------------------------------------
	// General...

	m_Parameters.Add_Choice("NODE_GENERAL",
		"INFO_ATTRIB"	, _TL("Additional Information"),
		_TL("Field that provides file paths to additional record information (HTML formatted), either absolute or relative to this data set."),
		_TL("<default>")
	);

	//-----------------------------------------------------
	// Display...

	m_Parameters.Add_Parameters("NODE_DISPLAY",
		"DISPLAY_CHART"	, _TL("Chart"), _TL("")
	);

	//-----------------------------------------------------
	// Classification...

	m_Parameters.Add_Bool("NODE_COLORS",
		"NODATA_SHOW"	, _TL("Show No-Data"), _TL(""), true
	);

	m_Parameters.Add_Color("NODATA_SHOW",
		"NODATA_COLOR"	, _TL("Color"), _TL(""), SG_COLOR_GREY_LIGHT
	);

	//-----------------------------------------------------
	// Label...

	m_Parameters.Add_Choice("NODE_LABEL",
		"LABEL_ATTRIB"	, _TL("Attribute"),
		_TL(""),
		_TL("<default>")
	);

	m_Parameters.Add_Font("LABEL_ATTRIB",
		"LABEL_ATTRIB_FONT"	, _TL("Font"),
		_TL("")
	);

	m_Parameters.Add_Choice("LABEL_ATTRIB",
		"LABEL_ATTRIB_PREC"	, _TL("Numerical Precision"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("fit to value"),
			_TL("standard"),
			SG_T("0|0.1|0.12|0.123|0.1234|0.12345|0.1234567|0.12345678|0.123456789|0.1234567890|0.12345678901|0.123456789012|0.1234567890123|0.12345678901234|0.123456789012345|0.1234567890123456|0.12345678901234567|0.123456789012345678|0.1234567890123456789|0.12345678901234567890")
		), 0
	);

	m_Parameters.Add_Choice("LABEL_ATTRIB",
		"LABEL_ATTRIB_SIZE_TYPE", _TL("Size relates to..."),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("Screen"),
			_TL("Map Units")
		), 0
	);

	m_Parameters.Add_Choice("LABEL_ATTRIB",
		"LABEL_ATTRIB_EFFECT"	, _TL("Boundary Effect"),
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

	m_Parameters.Add_Color("LABEL_ATTRIB_EFFECT",
		"LABEL_ATTRIB_EFFECT_COLOR"	, _TL("Color"),
		_TL(""),
		SG_GET_RGB(255, 255, 255)
	);

	m_Parameters.Add_Int("LABEL_ATTRIB_EFFECT",
		"LABEL_ATTRIB_EFFECT_SIZE"	, _TL("Size"),
		_TL(""),
		1, 1, true
	);

	m_Parameters.Add_Choice("LABEL_ATTRIB",
		"LABEL_ATTRIB_SIZE_BY"		, _TL("Size by Attribute"),
		_TL(""),
		_TL("<default>")
	);

	m_Parameters.Add_Double("LABEL_ATTRIB_SIZE_BY",
		"LABEL_ATTRIB_SIZE"			, _TL("Default Size"),
		_TL(""),
		100.0, 0.0, true
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
		m_Edit_Color.GetRGB()
	);

	m_Parameters.Add_Bool("NODE_EDIT",
		"EDIT_GLEAM"	, _TL("Gleam"),
		_TL(""),
		m_Edit_bGleam
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

	m_Parameters("LUT")->asTable()->Set_Field_Type(LUT_MIN, SG_DATATYPE_String);
	m_Parameters("LUT")->asTable()->Set_Field_Type(LUT_MAX, SG_DATATYPE_String);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Shapes::On_DataObject_Changed(void)
{
	AttributeList_Set(m_Parameters("LUT_ATTRIB"          ), false);
	AttributeList_Set(m_Parameters("METRIC_ATTRIB"       ), false);
	AttributeList_Set(m_Parameters("METRIC_NORMAL"       ), true );
	AttributeList_Set(m_Parameters("LABEL_ATTRIB"        ), true );
	AttributeList_Set(m_Parameters("LABEL_ATTRIB_SIZE_BY"), true );
	AttributeList_Set(m_Parameters("INFO_ATTRIB"         ), true );

	_Chart_Set_Options();

	//-----------------------------------------------------
	CWKSP_Layer::On_DataObject_Changed();

	m_pTable->DataObject_Changed();
}

//---------------------------------------------------------
void CWKSP_Shapes::On_Parameters_Changed(void)
{
	CWKSP_Layer::On_Parameters_Changed();

	//-----------------------------------------------------
	switch( m_Parameters("COLORS_TYPE")->asInt() )
	{
	default:	// CLASSIFY_UNIQUE
		m_fValue	= -1;
		m_fNormal	= -1;
		break;

	case  1:	// CLASSIFY_LUT
		m_fValue	= m_Parameters("LUT_ATTRIB"   )->asInt();	if( m_fValue >= Get_Shapes()->Get_Field_Count() )	{	m_fValue	= -1;	}
		m_fNormal	= -1;

		{
			TSG_Data_Type	Type	= SG_Data_Type_is_Numeric(Get_Shapes()->Get_Field_Type(m_Parameters("LUT_ATTRIB")->asInt())) ? SG_DATATYPE_Double : SG_DATATYPE_String;

			m_Parameters("LUT")->asTable()->Set_Field_Type(LUT_MIN, Type);
			m_Parameters("LUT")->asTable()->Set_Field_Type(LUT_MAX, Type);
		}
		break;

	case  2:	// CLASSIFY_DISCRETE
	case  3:	// CLASSIFY_GRADUATED
		m_fValue	= m_Parameters("METRIC_ATTRIB")->asInt();	if( m_fValue  >= Get_Shapes()->Get_Field_Count() )	{	m_fValue	= -1;	}
		m_fNormal	= m_Parameters("METRIC_NORMAL")->asInt();	if( m_fNormal >= Get_Shapes()->Get_Field_Count() )	{	m_fNormal	= -1;	}
		break;
	}

	if( m_fValue < 0 )
	{
		m_pClassify->Set_Mode(CLASSIFY_UNIQUE);
	}

	m_pClassify->Set_Unique_Color(m_pClassify->Get_Mode() == CLASSIFY_UNIQUE
		? m_Parameters("UNISYMBOL_COLOR")->asColor()
		: m_Parameters("NODATA_COLOR"   )->asColor()
	);

	m_bNoData	= m_Parameters("NODATA_SHOW")->asBool();

	//-----------------------------------------------------
	int	fInfo	= m_Parameters("INFO_ATTRIB")->asInt();

	if( fInfo >= Get_Shapes()->Get_Field_Count() )
	{
		fInfo	= -1;
	}

	if( m_fInfo != fInfo )
	{
		m_fInfo	= fInfo;

		if( g_pActive->Get_Active() == this )
		{
			g_pActive->Update_Info();
		}
	}

	//-----------------------------------------------------
	if( (m_fLabel = m_Parameters("LABEL_ATTRIB")->asInt()) >= Get_Shapes()->Get_Field_Count() )
	{
		m_fLabel	= -1;
	}

	m_Label_Eff_Color	= m_Parameters("LABEL_ATTRIB_EFFECT_COLOR")->asColor();
	m_Label_Eff_Size	= m_Parameters("LABEL_ATTRIB_EFFECT_SIZE" )->asInt  ();
	m_Label_Prec		= m_Parameters("LABEL_ATTRIB_PREC"        )->asInt  ();

	switch( m_Label_Prec )
	{
	case  0:	m_Label_Prec  = -m_Parameters("TABLE_FLT_DECIMALS")->asInt(); break;
	case  1:	m_Label_Prec  = -99; break;
	default:	m_Label_Prec -=   2; break;
	}

	switch( m_Parameters("LABEL_ATTRIB_EFFECT")->asInt() )
	{
	default:	m_Label_Eff = TEXTEFFECT_NONE       ;	break;
	case  1:	m_Label_Eff = TEXTEFFECT_FRAME      ;	break;
	case  2:	m_Label_Eff = TEXTEFFECT_TOP        ;	break;
	case  3:	m_Label_Eff = TEXTEFFECT_TOPLEFT    ;	break;
	case  4:	m_Label_Eff = TEXTEFFECT_LEFT       ;	break;
	case  5:	m_Label_Eff = TEXTEFFECT_BOTTOMLEFT ;	break;
	case  6:	m_Label_Eff = TEXTEFFECT_BOTTOM     ;	break;
	case  7:	m_Label_Eff = TEXTEFFECT_BOTTOMRIGHT;	break;
	case  8:	m_Label_Eff = TEXTEFFECT_RIGHT      ;	break;
	case  9:	m_Label_Eff = TEXTEFFECT_TOPRIGHT   ;	break;
	}

	//-----------------------------------------------------
	_Chart_Get_Options();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Shapes::Set_Metrics(int zField, int nField)
{
	if( zField >= 0 && zField < Get_Shapes()->Get_Field_Count() )
	{
		if( nField < Get_Shapes()->Get_Field_Count() )
		{
			m_Metrics.Create();

			for(int i=0; i<Get_Shapes()->Get_Count(); i++)
			{
				double	z	= Get_Shapes()->Get_Record(i)->asDouble(zField);
				double	n	= Get_Shapes()->Get_Record(i)->asDouble(nField);
					
				if( !Get_Shapes()->is_NoData_Value(z) && !Get_Shapes()->is_NoData_Value(n) && n != 0.0 )
				{
					m_Metrics.Add_Value(z / n);
				}
			}
		}
		else
		{
			m_Metrics.Create(Get_Shapes()->Get_Statistics(zField));
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CWKSP_Shapes::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter, int Flags)
{
	//-----------------------------------------------------
	if( pParameters->Cmp_Identifier("CLASSIFY") )
	{
		if( Flags & PARAMETER_CHECK_ENABLE )
		{
			if( pParameter->Cmp_Identifier("METHOD") )
			{
				pParameters->Set_Enabled("COUNT"   , pParameter->asInt() != 0);
				pParameters->Set_Enabled("COUNTMAX", pParameter->asInt() == 0);
			}
		}

		return( CWKSP_Layer::On_Parameter_Changed(pParameters, pParameter, Flags) );
	}

	//-----------------------------------------------------
	if( Flags & PARAMETER_CHECK_VALUES )
	{
		if(	pParameter->Cmp_Identifier("COLORS_TYPE"  )
		||	pParameter->Cmp_Identifier("METRIC_ATTRIB")
		||	pParameter->Cmp_Identifier("METRIC_NORMAL") )
		{
			Set_Metrics(
				pParameters->Get_Parameter("METRIC_ATTRIB")->asInt(),
				pParameters->Get_Parameter("METRIC_NORMAL")->asInt()
			);

			pParameters->Get_Parameter("METRIC_ZRANGE")->asRange()->Set_Range(
				m_Metrics.Get_Minimum(),
				m_Metrics.Get_Maximum()
			);
		}

		if(	pParameter->Cmp_Identifier("LUT_ATTRIB")
		&&  pParameter->asInt() >= 0 && pParameter->asInt() < Get_Shapes()->Get_Field_Count() )
		{
			TSG_Data_Type	Type	= SG_Data_Type_is_Numeric(Get_Shapes()->Get_Field_Type(pParameter->asInt()))
									? SG_DATATYPE_Double : SG_DATATYPE_String;

			pParameters->Get_Parameter("LUT")->asTable()->Set_Field_Type(LUT_MIN, Type);
			pParameters->Get_Parameter("LUT")->asTable()->Set_Field_Type(LUT_MAX, Type);
		}
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

		if(	pParameter->Cmp_Identifier("METRIC_ATTRIB") )
		{
			pParameters->Set_Enabled("METRIC_NORMAL", pParameter->asInt() >= 0);
		}

		if(	pParameter->Cmp_Identifier("OUTLINE") )
		{
			pParameters->Set_Enabled("OUTLINE_COLOR", pParameter->asBool());
			pParameters->Set_Enabled("OUTLINE_SIZE" , pParameter->asBool());
		}

		if(	pParameter->Cmp_Identifier("LABEL_ATTRIB") )
		{
			bool	Value	= pParameter->asInt() < Get_Shapes()->Get_Field_Count();

			pParameters->Set_Enabled("LABEL_ATTRIB_FONT"     , Value);
			pParameters->Set_Enabled("LABEL_ATTRIB_SIZE_TYPE", Value);
			pParameters->Set_Enabled("LABEL_ATTRIB_PREC"     , Value);
			pParameters->Set_Enabled("LABEL_ATTRIB_SIZE_BY"  , Value);
			pParameters->Set_Enabled("LABEL_ATTRIB_EFFECT"   , Value);
		}

		if( pParameter->Cmp_Identifier("LABEL_ATTRIB_SIZE_TYPE")
		||  pParameter->Cmp_Identifier("LABEL_ATTRIB_SIZE_BY"  ) )
		{
			bool	Value	= pParameters->Get_Parameter("LABEL_ATTRIB_SIZE_TYPE")->asInt() != 0
						||    pParameters->Get_Parameter("LABEL_ATTRIB_SIZE_BY"  )->asInt() < Get_Shapes()->Get_Field_Count();

			pParameters->Set_Enabled("LABEL_ATTRIB_SIZE", Value);
		}

		if(	pParameter->Cmp_Identifier("LABEL_ATTRIB_EFFECT") )
		{
			bool	Value	= pParameter->asInt() > 0;

			pParameters->Set_Enabled("LABEL_ATTRIB_EFFECT_COLOR", Value);
		}

		if(	pParameter->Cmp_Identifier("EDIT_SNAP_LIST") )
		{
			pParameters->Set_Enabled("EDIT_SNAP_DIST", pParameter->asList()->Get_Item_Count() > 0);
		}

		if(	pParameters->Cmp_Identifier("DISPLAY_CHART") )
		{
			CSG_String	s(pParameter->Get_Identifier());

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

//---------------------------------------------------------
void CWKSP_Shapes::On_Update_Views(void)
{
	m_pTable->Update_Views();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Shapes::_LUT_Create(void)
{
	//-----------------------------------------------------
	if( Get_Shapes()->Get_Field_Count() <= 0 || Get_Shapes()->Get_Count() < 1 )
	{
		DLG_Message_Show(_TL("Function failed because no attributes are available"), _TL("Classify"));

		return;
	}

	//-----------------------------------------------------
	static CSG_Parameters	PStatic;

	if( PStatic.Get_Count() == 0 )
	{
		PStatic.Create(NULL, _TL("Classify"), _TL(""), SG_T("CLASSIFY"));
		PStatic.Add_Choice("", "FIELD"   , _TL("Attribute"                ), _TL(""), "");
		PStatic.Add_Colors("", "COLORS"  , _TL("Colors"                   ), _TL(""));
		PStatic.Add_Int   ("", "COUNT"   , _TL("Number of Classes"        ), _TL(""),   10, 1, true);
		PStatic.Add_Int   ("", "COUNTMAX", _TL("Maximum Number of Classes"), _TL(""), 1000, 1, true);
		PStatic.Add_Choice("", "METHOD"  , _TL("Classification"           ), _TL(""),
			CSG_String::Format("%s|%s|%s|%s",
				_TL("unique values"),
				_TL("equal intervals"),
				_TL("quantiles"),
				_TL("natural breaks")
			), 0
		);
	}

	AttributeList_Set(PStatic("FIELD"), false);

	CSG_Parameters	Parameters(this, _TL("Classify"), _TL(""), SG_T("CLASSIFY"));

	Parameters.Assign_Parameters(&PStatic);

	Parameters.Set_Callback_On_Parameter_Changed(&Parameter_Callback);

	if( !DLG_Parameters(&Parameters) )
	{
		return;
	}

	PStatic.Assign_Values(&Parameters);

	//-----------------------------------------------------
	DataObject_Changed();

	int	Field	= Parameters("FIELD")->asInt();

	int	Method	= SG_Data_Type_is_Numeric(Get_Shapes()->Get_Field_Type(Field)) ? Parameters("METHOD")->asInt() : 0;

	CSG_Colors	Colors(*Parameters("COLORS")->asColors());

	if( Method != 0 )
	{
		Colors.Set_Count(Parameters("COUNT")->asInt());
	}

	CSG_Table	Classes(m_Parameters("LUT")->asTable());

	switch( Method )
	{
	//-----------------------------------------------------
	case 0:	// unique values
		{
			TSG_Data_Type	Type	= SG_Data_Type_is_Numeric(Get_Shapes()->Get_Field_Type(Field))
				? SG_DATATYPE_Double : SG_DATATYPE_String;

			Classes.Set_Field_Type(LUT_MIN, Type);
			Classes.Set_Field_Type(LUT_MAX, Type);

			CSG_Unique_String_Statistics	s;

			int	maxClasses	= Parameters("COUNTMAX")->asInt();

			for(int iShape=0; iShape<Get_Shapes()->Get_Count() && s.Get_Count()<maxClasses; iShape++)
			{
				s	+= Get_Shapes()->Get_Shape(iShape)->asString(Field);
			}

			Colors.Set_Count(s.Get_Count());

			for(int iClass=0; iClass<s.Get_Count(); iClass++)
			{
				CSG_String	Value	= s.Get_Value(iClass);

				CSG_Table_Record	*pClass	= Classes.Add_Record();

				pClass->Set_Value(0, Colors[iClass]);	// Color
				pClass->Set_Value(1, Value         );	// Name
				pClass->Set_Value(2, Value         );	// Description
				pClass->Set_Value(3, Value         );	// Minimum
				pClass->Set_Value(4, Value         );	// Maximum
			}
		}
		break;

	//-----------------------------------------------------
	case 1:	// equal intervals
		{
			double	Minimum, Maximum, Interval;

			Interval	= Get_Shapes()->Get_Range  (Field) / (double)Colors.Get_Count();
			Minimum		= Get_Shapes()->Get_Minimum(Field);

			Classes.Set_Field_Type(LUT_MIN, SG_DATATYPE_Double);
			Classes.Set_Field_Type(LUT_MAX, SG_DATATYPE_Double);

			for(int iClass=0; iClass<Colors.Get_Count(); iClass++, Minimum+=Interval)
			{
				Maximum	= iClass < Colors.Get_Count() - 1 ? Minimum + Interval : Get_Shapes()->Get_Maximum(Field) + 1.0;

				CSG_String	Name	= SG_Get_String(Minimum, -2)
							+ " - " + SG_Get_String(Maximum, -2);

				CSG_Table_Record	*pClass	= Classes.Add_Record();

				pClass->Set_Value(0, Colors[iClass]);	// Color
				pClass->Set_Value(1, Name          );	// Name
				pClass->Set_Value(2, Name          );	// Description
				pClass->Set_Value(3, Minimum       );	// Minimum
				pClass->Set_Value(4, Maximum       );	// Maximum
			}
		}
		break;

	//-----------------------------------------------------
	case 2:	// quantiles
		{
			TSG_Table_Index_Order	old_Order	= Get_Shapes()->Get_Index_Order(0);
			int						old_Field	= Get_Shapes()->Get_Index_Field(0);

			Get_Shapes()->Set_Index(Field, TABLE_INDEX_Ascending);

			Classes.Set_Field_Type(LUT_MIN, SG_DATATYPE_Double);
			Classes.Set_Field_Type(LUT_MAX, SG_DATATYPE_Double);

			if( Get_Shapes()->Get_Count() < Colors.Get_Count() )
			{
				Colors.Set_Count(Get_Shapes()->Get_Count());
			}

			double	Minimum, Maximum, Count, iRecord;

			Maximum	= Get_Shapes()->Get_Minimum(Field);
			iRecord	= Count	= Get_Shapes()->Get_Count() / (double)Colors.Get_Count();

			for(int iClass=0; iClass<Colors.Get_Count(); iClass++, iRecord+=Count)
			{
				Minimum	= Maximum;
				Maximum	= iRecord < Get_Shapes()->Get_Count() ? Get_Shapes()->Get_Record_byIndex((int)iRecord)->asDouble(Field) : Get_Shapes()->Get_Maximum(Field) + 1.0;

				CSG_String	Name	= SG_Get_String(Minimum, -2)
							+ " - " + SG_Get_String(Maximum, -2);

				CSG_Table_Record	*pClass	= Classes.Add_Record();

				pClass->Set_Value(0, Colors[iClass]);	// Color
				pClass->Set_Value(1, Name          );	// Name
				pClass->Set_Value(2, Name          );	// Description
				pClass->Set_Value(3, Minimum       );	// Minimum
				pClass->Set_Value(4, Maximum       );	// Maximum
			}

			Get_Shapes()->Set_Index(old_Field, old_Order);
		}
		break;

	//-----------------------------------------------------
	case 3:	// natural breaks
		{
			CSG_Natural_Breaks	Breaks(Get_Shapes(), Field, Colors.Get_Count(), Get_Shapes()->Get_Count() > 4096 ? 256 : 0);

			if( Breaks.Get_Count() <= Colors.Get_Count() ) return;

			Classes.Set_Field_Type(LUT_MIN, SG_DATATYPE_Double);
			Classes.Set_Field_Type(LUT_MAX, SG_DATATYPE_Double);

			for(int iClass=0; iClass<Colors.Get_Count(); iClass++)
			{
				CSG_Table_Record	*pClass	= Classes.Add_Record();

				double	Minimum	= Breaks[iClass    ];
				double	Maximum	= Breaks[iClass + 1];

				CSG_String	Name	= SG_Get_String(Minimum, -2)
							+ " - " + SG_Get_String(Maximum, -2);

				pClass->Set_Value(0, Colors[iClass]);	// Color
				pClass->Set_Value(1, Name          );	// Name
				pClass->Set_Value(2, Name          );	// Description
				pClass->Set_Value(3, Minimum       );	// Minimum
				pClass->Set_Value(4, Maximum       );	// Maximum
			}
		}
		break;
	}

	//-----------------------------------------------------
	if( Classes.Get_Count() > 0 )
	{
		m_Parameters("LUT")->asTable()->Assign(&Classes);

		m_Parameters("COLORS_TYPE")->Set_Value(CLASSIFY_LUT);	// Lookup Table
		m_Parameters("LUT_ATTRIB" )->Set_Value(Field);

		Parameters_Changed();
	}
}

//---------------------------------------------------------
void CWKSP_Shapes::_LUT_Import(void)
{
	wxString	File, Filter;

	Filter.Printf("%s (*.qml)|*.qml|%s|*.*", _TL("QGIS Layer Style File"), _TL("All Files"));

	if( DLG_Open(File, _TL("Import Classification"), SG_T("QGIS Layer Style File (*.qml)|*.qml|All Files|*.*|")) )
	{
		CSG_Table	Classes;
		CSG_String	Attribute;

		if( QGIS_Styles_Import(&File, Classes, Attribute) )
		{
			m_Parameters.Set_Parameter("LUT_ATTRIB", Attribute);

			m_Parameters("LUT")->asTable()->Assign(&Classes);

			m_Parameters("COLORS_TYPE")->Set_Value(CLASSIFY_LUT);	// Lookup Table

			Parameters_Changed();
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_Shapes::Get_Value(CSG_Point ptWorld, double Epsilon)
{
	CSG_Shape	*pShape	= Get_Shapes()->Get_Shape(ptWorld, Epsilon);

	if( pShape == NULL )
	{
		return( _TL("") );
	}

	if( m_fValue < 0 )
	{
		return( wxString::Format("%s: %d", _TL("Index"), pShape->Get_Index() + 1) );
	}

	if( m_pClassify->Get_Mode() == CLASSIFY_LUT )
	{
		return( m_pClassify->Get_Class_Name_byValue(pShape->asString(m_fValue)) );
	}

	if( !pShape->is_NoData(m_fValue) )
	{
		if( m_fNormal < 0 )
		{
			return( pShape->asString(m_fValue) );
		}

		if( !pShape->is_NoData(m_fNormal) && pShape->asDouble(m_fNormal) != 0.0 )
		{
			return( wxString::Format("%f", pShape->asDouble(m_fValue) / pShape->asDouble(m_fNormal)) );
		}
	}

	return( _TL("") );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CWKSP_Shapes::Get_Value_Minimum(void)	{	return( m_Metrics.Get_Minimum() );	}
double CWKSP_Shapes::Get_Value_Maximum(void)	{	return( m_Metrics.Get_Maximum() );	}
double CWKSP_Shapes::Get_Value_Range  (void)	{	return( m_Metrics.Get_Range  () );	}
double CWKSP_Shapes::Get_Value_Mean   (void)	{	return( m_Metrics.Get_Mean   () );	}
double CWKSP_Shapes::Get_Value_StdDev (void)	{	return( m_Metrics.Get_StdDev () );	}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_Shapes::Get_Name_Attribute(void)
{
	wxString	s;

	if(	m_fValue >= 0 && m_pClassify->Get_Mode() != CLASSIFY_UNIQUE )
	{
		s	= Get_Shapes()->Get_Field_Name(m_fValue);

		if( m_fNormal >= 0
		&& (m_pClassify->Get_Mode() == CLASSIFY_METRIC
		||  m_pClassify->Get_Mode() == CLASSIFY_GRADUATED) )
		{
			s	+= " / "; s += Get_Shapes()->Get_Field_Name(m_fNormal);
		}
	}

	return( s );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Shapes::On_Draw(CWKSP_Map_DC &dc_Map, int Flags)
{
	int		iShape;

	//-----------------------------------------------------
	if( Get_Extent().Intersects(dc_Map.m_rWorld) == INTERSECTION_None )
	{
		if( m_Edit_pShape )
		{
			Edit_Shape_Draw(dc_Map);
		}

		return;
	}

	//-----------------------------------------------------
	if( (Flags & LAYER_DRAW_FLAG_THUMBNAIL) != 0 )
	{
		Draw_Initialize(dc_Map, Flags);

		for(int iShape=0; iShape<Get_Shapes()->Get_Count(); iShape++)
		{
			_Draw_Shape(dc_Map, Get_Shapes()->Get_Shape(iShape));
		}

		return;
	}

	//-----------------------------------------------------
	double	Transparency	= m_Parameters("DISPLAY_TRANSPARENCY")->asDouble() / 100.0;

	CWKSP_Map_DC	*pDC	= Transparency > 0.0 ? new CWKSP_Map_DC(dc_Map.m_rWorld, dc_Map.m_rDC, dc_Map.m_Scale, SG_GET_RGB(254, 255, 255)) : NULL;
	CWKSP_Map_DC	&dc		= pDC ? *pDC : dc_Map;

	m_Sel_Color		= Get_Color_asWX(m_Parameters("SEL_COLOR" )->asInt());
	m_Edit_Color	= Get_Color_asWX(m_Parameters("EDIT_COLOR")->asInt());
	m_Edit_bGleam	=                m_Parameters("EDIT_GLEAM")->asBool();

	Draw_Initialize(dc, Flags);

	//-----------------------------------------------------
	if( (Flags & LAYER_DRAW_FLAG_NOEDITS) != 0 || !(m_Edit_pShape || Get_Shapes()->Get_Selection_Count()) )
	{
		for(iShape=0; iShape<Get_Shapes()->Get_Count(); iShape++)
		{
			_Draw_Shape(dc, Get_Shapes()->Get_Shape(iShape));
		}

		if( _Chart_is_Valid() )
		{
			for(iShape=0; iShape<Get_Shapes()->Get_Count(); iShape++)
			{
				_Draw_Chart(dc, Get_Shapes()->Get_Shape(iShape));
			}
		}
	}
	else	// selection and/or editing
	{
		for(iShape=0; iShape<Get_Shapes()->Get_Count(); iShape++)
		{
			if( !Get_Shapes()->Get_Shape(iShape)->is_Selected() )
			{
				_Draw_Shape(dc, Get_Shapes()->Get_Shape(iShape));
			}
		}

		for(iShape=0; iShape<(int)Get_Shapes()->Get_Selection_Count(); iShape++)
		{
			if( iShape != m_Edit_Index )
			{
				_Draw_Shape(dc, Get_Shapes()->Get_Selection(iShape), 2);
			}
		}

		//-------------------------------------------------
		if( !m_Edit_pShape )
		{
			_Draw_Shape(dc, Get_Shapes()->Get_Selection(m_Edit_Index), 1);
		}
		else
		{
			Edit_Shape_Draw(dc);
		}

		if( m_Edit_Mode == EDIT_SHAPE_MODE_Split )
		{
			CSG_Shape	*pSplit	= m_Edit_Shapes.Get_Shape(1);

			if( pSplit && pSplit->Get_Point_Count() > 1 )
			{
				for(int i=0; i<=1; i++)
				{
					dc_Map.dc.SetPen(wxPen(i == 0 ? *wxWHITE : *wxBLACK, i == 0 ? 3 : 1));

					TSG_Point_Int	B, A	= dc_Map.World2DC(pSplit->Get_Point(0));

					for(int iPoint=1; iPoint<pSplit->Get_Point_Count(); iPoint++)
					{
						B	= A;	A	= dc_Map.World2DC(pSplit->Get_Point(iPoint));

						dc_Map.dc.DrawLine(A.x, A.y, B.x, B.y);
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	if( (Flags & LAYER_DRAW_FLAG_NOLABELS) == 0 && m_fLabel >= 0 )	// Labels
	{
		int		iSize	= m_Parameters("LABEL_ATTRIB_SIZE_BY"  )->asInt();
		double	dSize	= m_Parameters("LABEL_ATTRIB_SIZE_TYPE")->asInt() == 1 ?
			dc.m_World2DC * m_Parameters("LABEL_ATTRIB_SIZE")->asDouble() : 1.0;

		dc.dc.SetFont(Get_Font (m_Parameters("LABEL_ATTRIB_FONT")));
		dc.dc.SetTextForeground(m_Parameters("LABEL_ATTRIB_FONT")->asColor());

		if( iSize >= 0 && iSize < Get_Shapes()->Get_Field_Count() )	// size by attribute
		{
			for(iShape=0; iShape<Get_Shapes()->Get_Count(); iShape++)
			{
				int	Size	= (int)(0.5 + dSize * Get_Shapes()->Get_Shape(iShape)->asDouble(iSize));

				if( Size > 0 )
				{
					_Draw_Label(dc, Get_Shapes()->Get_Shape(iShape), Size);
				}
			}
		}
		else	// one size fits all
		{
			int	Size	= m_Parameters("LABEL_ATTRIB_SIZE_TYPE")->asInt() == 1 ? (int)(0.5 + dSize) : dc.dc.GetFont().GetPointSize();

			if( Size > 0 )
			{
				for(iShape=0; iShape<Get_Shapes()->Get_Count(); iShape++)
				{
					_Draw_Label(dc, Get_Shapes()->Get_Shape(iShape), Size);
				}
			}
		}
	}

	//-----------------------------------------------------
	if( pDC )	// Transparency ?
	{
		dc_Map.Draw_DC(dc, Transparency);

		delete(pDC);
	}
}

//---------------------------------------------------------
void CWKSP_Shapes::_Draw_Shape(CWKSP_Map_DC &dc_Map, CSG_Shape *pShape, int Selection)
{
	if( pShape && dc_Map.m_rWorld.Intersects(pShape->Get_Extent()) != INTERSECTION_None )
	{
		Draw_Shape(dc_Map, pShape, Selection);

		if( m_bVertices > 0 )
		{
			wxPen	oldPen  (dc_Map.dc.GetPen  ());	dc_Map.dc.SetPen  (*wxBLACK_PEN  );
			wxBrush	oldBrush(dc_Map.dc.GetBrush());	dc_Map.dc.SetBrush(*wxWHITE_BRUSH);

			for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					TSG_Point_Int	A	= dc_Map.World2DC(pShape->Get_Point(iPoint, iPart));

					dc_Map.dc.DrawCircle(A.x, A.y, 2);

					if( m_bVertices == 2 )
					{
						Draw_Text(dc_Map.dc, TEXTALIGN_TOPLEFT, A.x, A.y, wxString::Format("%d", iPoint + 1));
					}
				}
			}

			dc_Map.dc.SetPen  (oldPen  );
			dc_Map.dc.SetBrush(oldBrush);
		}
	}
}

//---------------------------------------------------------
void CWKSP_Shapes::_Draw_Label(CWKSP_Map_DC &dc_Map, CSG_Shape *pShape, int PointSize)
{
	if( pShape && dc_Map.m_rWorld.Intersects(pShape->Get_Extent()) != INTERSECTION_None )
	{
		wxString	Label(pShape->asString(m_fLabel, m_Label_Prec));	Label.Trim(true).Trim(false);

		if( !Label.IsEmpty() )
		{
			if( PointSize > 0 && PointSize != dc_Map.dc.GetFont().GetPointSize() )
			{
				wxFont	Font(dc_Map.dc.GetFont());

				Font.SetPointSize(PointSize);

				dc_Map.dc.SetFont(Font);
			}

			Draw_Label(dc_Map, pShape, Label);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Shapes::AttributeList_Set(CSG_Parameter *pFields, bool bAddNoField)
{
	if( pFields && pFields->Get_Type() == PARAMETER_TYPE_Choice )
	{
		CSG_String	Items;

		for(int i=0; i<Get_Shapes()->Get_Field_Count(); i++)
		{
			Items	+= CSG_String(Get_Shapes()->Get_Field_Name(i)) + "|";
		}

		if( bAddNoField || Get_Shapes()->Get_Field_Count() == 0 )
		{
			Items	+= CSG_String(_TL("<none>")) + "|";
		}

		int			iChoice	= pFields->asInt   ();
		CSG_String	sChoice	= pFields->asString();

		pFields->asChoice()->Set_Items(Items);

		if( bAddNoField && (iChoice < 0 || iChoice >= pFields->asChoice()->Get_Count() || sChoice.Cmp(pFields->asChoice()->Get_Item(iChoice))) )
		{
			pFields->Set_Value(Get_Shapes()->Get_Field_Count());
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
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
	default:	return( wxBRUSHSTYLE_SOLID           );
	case  1:	return( wxBRUSHSTYLE_TRANSPARENT     );
	case  2:	return( wxBRUSHSTYLE_BDIAGONAL_HATCH );
	case  3:	return( wxBRUSHSTYLE_CROSSDIAG_HATCH );
	case  4:	return( wxBRUSHSTYLE_FDIAGONAL_HATCH );
	case  5:	return( wxBRUSHSTYLE_CROSS_HATCH     );
	case  6:	return( wxBRUSHSTYLE_HORIZONTAL_HATCH);
	case  7:	return( wxBRUSHSTYLE_VERTICAL_HATCH  );
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
	default:	return( wxPENSTYLE_SOLID            ); // Solid style.
	case  1:	return( wxPENSTYLE_DOT              ); // Dotted style.
	case  2:	return( wxPENSTYLE_LONG_DASH        ); // Long dashed style.
	case  3:	return( wxPENSTYLE_SHORT_DASH       ); // Short dashed style.
	case  4:	return( wxPENSTYLE_DOT_DASH         ); // Dot and dash style.
	case  5:	return( wxPENSTYLE_BDIAGONAL_HATCH  ); // Backward diagonal hatch.
	case  6:	return( wxPENSTYLE_CROSSDIAG_HATCH  ); // Cross-diagonal hatch.
	case  7:	return( wxPENSTYLE_FDIAGONAL_HATCH  ); // Forward diagonal hatch.
	case  8:	return( wxPENSTYLE_CROSS_HATCH      ); // Cross hatch.
	case  9:	return( wxPENSTYLE_HORIZONTAL_HATCH ); // Horizontal hatch.
	case 10:	return( wxPENSTYLE_VERTICAL_HATCH   ); // Vertical hatch.
//	case 11:	return( wxPENSTYLE_STIPPLE          ); // Use the stipple bitmap. 
//	case 12:	return( wxPENSTYLE_USER_DASH        ); // Use the user dashes: see wxPen::SetDashes.
//	case 13:	return( wxPENSTYLE_TRANSPARENT      ); // No pen is used.
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Shapes::Get_Class_Color(CSG_Shape *pShape, int &Color)
{
	//-----------------------------------------------------
	if( m_fValue >= 0 )
	{
		if( m_pClassify->Get_Mode() == CLASSIFY_LUT )
		{
			if( !SG_Data_Type_is_Numeric(Get_Shapes()->Get_Field_Type(m_fValue)) )
			{
				return( m_pClassify->Get_Class_Color_byValue(pShape->asString(m_fValue), Color) );
			}
			else
			{
				return( m_pClassify->Get_Class_Color_byValue(pShape->asDouble(m_fValue), Color) );
			}
		}

		//-------------------------------------------------
		if( !pShape->is_NoData(m_fValue) )
		{
			if( m_fNormal < 0 )	// don't normalize
			{
				return( m_pClassify->Get_Class_Color_byValue(pShape->asDouble(m_fValue), Color) );
			}

			if( !pShape->is_NoData(m_fNormal) && pShape->asDouble(m_fNormal) != 0.0 )
			{
				return( m_pClassify->Get_Class_Color_byValue(pShape->asDouble(m_fValue) / pShape->asDouble(m_fNormal), Color) );
			}
		}
	}

	//-----------------------------------------------------
	Color	= m_pClassify->Get_Unique_Color();

	return( m_pClassify->Get_Mode() == CLASSIFY_UNIQUE );
}


///////////////////////////////////////////////////////////
//														 //
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
void CWKSP_Shapes::_Draw_Chart(CWKSP_Map_DC &dc_Map, CSG_Shape *pShape)
{
	if( dc_Map.m_rWorld.Intersects(pShape->Get_Extent()) != INTERSECTION_None )
	{
		int			s;
		double		dSize	= m_Chart_sSize;
		TSG_Point_Int	p;

		if( m_Chart_sField >= 0 )
		{
			double	range	= Get_Shapes()->Get_Range(m_Chart_sField);

			if( range > 0.0 )
			{
				dSize	+= m_Chart_sRange * ((pShape->asDouble(m_Chart_sField) - Get_Shapes()->Get_Minimum(m_Chart_sField)) / range);
			}
		}

		s	= (int)(dSize * (m_Chart_sType == 1 ? dc_Map.m_World2DC : dc_Map.m_Scale));

		switch( pShape->Get_Type() )
		{
		default:					p	= dc_Map.World2DC(pShape->Get_Extent().Get_Center());			break;
		case SHAPE_TYPE_Polygon:	p	= dc_Map.World2DC(((CSG_Shape_Polygon *)pShape)->Get_Centroid());	break;
		}

		dc_Map.dc.SetPen(*wxBLACK_PEN);

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
void CWKSP_Shapes::_Draw_Chart_Pie(CWKSP_Map_DC &dc_Map, CSG_Table_Record *pRecord, bool bOutline, int x, int y, int size)
{
	int		i, ix, iy, jx, jy;
	double	d, sum;

	for(i=0, sum=0.0; i<m_Chart.Get_Count(); i++)
	{
		sum	+= fabs(pRecord->asDouble(m_Chart[i].x));
	}

	if( sum > 0.0 )
	{
		sum	= M_PI_360 / sum;

		for(i=0, jx=x, jy=y-size, d=0.0; i<m_Chart.Get_Count(); i++)
		{
			if( !bOutline )
			dc_Map.dc.SetPen	(wxPen  (Get_Color_asWX(m_Chart[i].y)));
			dc_Map.dc.SetBrush	(wxBrush(Get_Color_asWX(m_Chart[i].y)));

			d	+= sum * fabs(pRecord->asDouble(m_Chart[i].x));

			ix	= jx;
			iy	= jy;
			jx	= x - (int)(size * sin(-d));
			jy	= y - (int)(size * cos(-d));

			dc_Map.dc.DrawArc(jx, jy, ix, iy, x, y);
		}
	}
}

//---------------------------------------------------------
void CWKSP_Shapes::_Draw_Chart_Bar(CWKSP_Map_DC &dc_Map, CSG_Table_Record *pRecord, bool bOutline, int x, int y, int sx, int sy)
{
	int		i;
	double	d, dx, dy, max, ix;

	y	+= sy / 2;

	for(i=1, max=fabs(pRecord->asDouble(m_Chart[0].x)); i<m_Chart.Get_Count(); i++)
	{
		if( (d = fabs(pRecord->asDouble(m_Chart[i].x))) > max )
			max	= d;
	}

	if( max != 0.0 )
	{
		max	= sy / max;
		dx	= sx / (double)m_Chart.Get_Count();

		for(i=0, ix=x-sx/2.0; i<m_Chart.Get_Count(); i++, ix+=dx)
		{
			if( !bOutline )
			dc_Map.dc.SetPen	(wxPen  (Get_Color_asWX(m_Chart[i].y)));
			dc_Map.dc.SetBrush	(wxBrush(Get_Color_asWX(m_Chart[i].y)));

			dy	= -pRecord->asDouble(m_Chart[i].x) * max;

			dc_Map.dc.DrawRectangle((int)ix, y, (int)dx, (int)dy);
		}

		if( !bOutline )
		{
			dc_Map.dc.SetPen(*wxBLACK_PEN);
			dc_Map.dc.DrawLine(x - sx / 2, y, x + sx / 2, y);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

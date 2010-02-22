
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

#include "wksp_layer_classify.h"

#include "wksp_shapes.h"
#include "wksp_table.h"

#include "view_scatterplot.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Shapes::CWKSP_Shapes(CSG_Shapes *pShapes)
	: CWKSP_Layer(pShapes)
{
	m_pShapes		= pShapes;
	m_pTable		= new CWKSP_Table(m_pShapes, this);

	m_Edit_Shapes.Create(m_pShapes->Get_Type());
	m_Edit_pShape	= NULL;

	m_Edit_Attributes.Destroy();
	m_Edit_Attributes.Add_Field(LNG("[FLD] Name") , SG_DATATYPE_String);
	m_Edit_Attributes.Add_Field(LNG("[FLD] Value"), SG_DATATYPE_String);
}

//---------------------------------------------------------
CWKSP_Shapes::~CWKSP_Shapes(void)
{
	delete(m_pTable);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_Shapes::Get_Name(void)
{
	return( wxString::Format(wxT("%02d. %s"), 1 + Get_ID(), m_pShapes->Get_Name()) );
}

//---------------------------------------------------------
wxString CWKSP_Shapes::Get_Description(void)
{
	wxString	s;

	//-----------------------------------------------------
	s.Append(wxString::Format(wxT("<b>%s</b><table border=\"0\">"),
		LNG("[CAP] Shapes")
	));

	s.Append(wxString::Format(wxT("<tr><td>%s</td><td>%s</td></tr>"),
		LNG("[CAP] Name")					, m_pShapes->Get_Name()
	));

	s.Append(wxString::Format(wxT("<tr><td>%s</td><td>%s</td></tr>"),
		LNG("[CAP] File")					, m_pShapes->Get_File_Name()
	));

	s.Append(wxString::Format(wxT("<tr><td>%s</td><td>%s</td></tr>"),
		LNG("[CAP] Projection")				, m_pShapes->Get_Projection().Get_Name().c_str()
	));

	s.Append(wxString::Format(wxT("<tr><td>%s</td><td>%s</td></tr>"),
		LNG("[CAP] Modified")				, m_pShapes->is_Modified() ? LNG("[VAL] yes") : LNG("[VAL] no")
	));

	s.Append(wxString::Format(wxT("<tr><td>%s</td><td>%s</td></tr>"),
		LNG("[CAP] Type")					, SG_Get_ShapeType_Name(m_pShapes->Get_Type())
	));

	s.Append(wxString::Format(wxT("<tr><td>%s</td><td>%d</td></tr>"),
		LNG("[CAP] Number Of Shapes")		, m_pShapes->Get_Count()
	));

	s.Append(wxT("</table>"));

	//-----------------------------------------------------
	s.Append(wxString::Format(wxT("<hr><b>%s</b>"), LNG("[CAP] Table Description")));
	s.Append(Get_TableInfo_asHTML(m_pShapes));

	//-----------------------------------------------------
//	s.Append(wxString::Format(wxT("<hr><b>%s</b><font size=\"-1\">"), LNG("[CAP] Data History")));
//	s.Append(m_pShapes->Get_History().Get_HTML());
//	s.Append(wxString::Format(wxT("</font")));

	return( s );
}

//---------------------------------------------------------
wxMenu * CWKSP_Shapes::Get_Menu(void)
{
	wxMenu	*pMenu;

	pMenu	= new wxMenu(m_pShapes->Get_Name());

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_CLOSE);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_SHAPES_SAVE);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_SHAPES_SAVEAS);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_SHAPES_SHOW);
	CMD_Menu_Add_Item(pMenu,  true, ID_CMD_SHAPES_HISTOGRAM);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_SHAPES_SET_LUT);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_SETTINGS_COPY);

	pMenu->AppendSeparator();

	wxMenu	*pTable	= new wxMenu(LNG("[MNU] Table"));
	CMD_Menu_Add_Item(pTable,  true, ID_CMD_TABLES_SHOW);
	CMD_Menu_Add_Item(pTable,  true, ID_CMD_TABLES_DIAGRAM);
	CMD_Menu_Add_Item(pTable, false, ID_CMD_TABLES_SCATTERPLOT);
	pMenu->Append(ID_CMD_WKSP_FIRST, LNG("[MNU] Attributes"), pTable);

	pMenu->Append(ID_CMD_WKSP_FIRST, LNG("[MNU] Edit"), Edit_Get_Menu());

	return( pMenu );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Shapes::On_Command(int Cmd_ID)
{
	switch( Cmd_ID )
	{
	default:
		return( CWKSP_Layer::On_Command(Cmd_ID) );

	case ID_CMD_SHAPES_SET_LUT:
		_LUT_Create();
		break;

	case ID_CMD_SHAPES_EDIT_SHAPE:
		_Edit_Shape();
		break;

	case ID_CMD_SHAPES_EDIT_ADD_SHAPE:
		_Edit_Shape_Add();
		break;

	case ID_CMD_SHAPES_EDIT_DEL_SHAPE:
		_Edit_Shape_Del();
		break;

	case ID_CMD_SHAPES_EDIT_ADD_PART:
		_Edit_Part_Add();
		break;

	case ID_CMD_SHAPES_EDIT_DEL_PART:
		_Edit_Part_Del();
		break;

	case ID_CMD_SHAPES_EDIT_DEL_POINT:
		_Edit_Point_Del();
		break;

	case ID_CMD_SHAPES_EDIT_SEL_INVERT:
		m_pShapes->Inv_Selection();
		Update_Views(false);
		break;

	case ID_CMD_TABLES_SHOW:
		m_pTable->Toggle_View();
		break;

	case ID_CMD_TABLES_DIAGRAM:
		m_pTable->Toggle_Diagram();
		break;

	case ID_CMD_TABLES_SCATTERPLOT:
		Add_ScatterPlot(Get_Table()->Get_Table());
		break;

	case ID_CMD_SHAPES_HISTOGRAM:
		Histogram_Toggle();
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
		event.Enable(m_pShapes->Get_Selection_Count() > 0 || m_Edit_pShape != NULL);
		event.Check(m_Edit_pShape != NULL);
		break;

	case ID_CMD_SHAPES_EDIT_ADD_SHAPE:
		event.Enable(m_Edit_pShape == NULL);
		break;

	case ID_CMD_SHAPES_EDIT_DEL_SHAPE:
		event.Enable(m_pShapes->Get_Selection_Count() > 0 && m_Edit_pShape == NULL);
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

	case ID_CMD_SHAPES_EDIT_SEL_INVERT:
		event.Enable(m_Edit_pShape == NULL);
		break;

	case ID_CMD_TABLES_SHOW:
		event.Check(m_pTable->Get_View() != NULL);
		break;

	case ID_CMD_TABLES_DIAGRAM:
		event.Check(m_pTable->Get_Diagram() != NULL);
		break;

	case ID_CMD_SHAPES_HISTOGRAM:
		event.Check(m_pHistogram != NULL);
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
void CWKSP_Shapes::On_Create_Parameters(void)
{
	//-----------------------------------------------------
	// General...

	_AttributeList_Add(
		m_Parameters("NODE_COLORS")		, "COLORS_ATTRIB"			, LNG("[CAP] Attribute"),
		LNG("")
	);


	//-----------------------------------------------------
	// Display...

	m_Parameters.Add_Parameters(
		m_Parameters("NODE_DISPLAY")	, "DISPLAY_CHART"			, LNG("[CAP] Chart"),
		LNG("")
	);

	//-----------------------------------------------------
	m_Parameters.Add_Value(
		m_Parameters("NODE_DISPLAY")	, "DISPLAY_TRANSPARENCY"	, LNG("[CAP] Transparency [%]"),
		LNG(""),
		PARAMETER_TYPE_Double, 0.0, 0.0, true, 100.0, true
	);


#ifdef USE_HTMLINFO
	//-----------------------------------------------------
	// HTML Extra info

	m_Parameters.Add_Node(
		NULL							, "NODE_EXTRAINFO"			, LNG("[CAP] Html Extra Info"),
		LNG("")
	);

	_AttributeList_Add(
		m_Parameters("NODE_EXTRAINFO")	, "EXTRAINFO_ATTRIB"		, LNG("[CAP] Attribute"),
		LNG("")
	);
#endif


	//-----------------------------------------------------
	// Label...

	m_Parameters.Add_Node(
		NULL							, "NODE_LABEL"				, LNG("[CAP] Display: Label"),
		LNG("")
	);

	_AttributeList_Add(
		m_Parameters("NODE_LABEL")		, "LABEL_ATTRIB"			, LNG("[CAP] Attribute"),
		LNG("")
	);

	m_Parameters.Add_Font(
		m_Parameters("NODE_LABEL")		, "LABEL_ATTRIB_FONT"		, LNG("[CAP] Font"),
		LNG("")
	);

	m_Parameters.Add_Choice(
		m_Parameters("NODE_LABEL")		, "LABEL_ATTRIB_SIZE_TYPE"	, LNG("[CAP] Size relates to..."),
		LNG(""),
		wxString::Format(wxT("%s|%s|"),
			LNG("[VAL] Screen"),
			LNG("[VAL] Map Units")
		), 0
	);

	m_Parameters.Add_Value(
		m_Parameters("NODE_LABEL")		, "LABEL_ATTRIB_SIZE"		, LNG("[CAP] Default Size"),
		LNG(""),
		PARAMETER_TYPE_Double, 100.0, 0.0, true
	);

	_AttributeList_Add(
		m_Parameters("NODE_LABEL")		, "LABEL_ATTRIB_SIZE_BY"	, LNG("[CAP] Size by Attribute"),
		LNG("")
	);


	//-----------------------------------------------------
	// Edit...

	m_Parameters.Add_Node(
		NULL							, "NODE_EDIT"				, LNG("[CAP] Edit"),
		LNG("")
	);

	m_Parameters.Add_Value(
		m_Parameters("NODE_EDIT")		, "EDIT_SNAP_DIST"			, LNG("[CAP] Snap Distance"),
		LNG(""),
		PARAMETER_TYPE_Int, 10, 0, true
	);

	m_Parameters.Add_Shapes_List(
		m_Parameters("NODE_EDIT")		, "EDIT_SNAP_LIST"			, LNG("[CAP] Snap to..."),
		LNG(""),
		PARAMETER_INPUT
	);

	m_Parameters.Add_Value(
		m_Parameters("NODE_EDIT")		, "EDIT_COLOR"				, LNG("[CAP] Color"),
		LNG(""),
		PARAMETER_TYPE_Color, SG_GET_RGB(0, 0, 0)
	);

	m_Parameters.Add_Node(
		m_Parameters("NODE_EDIT")		, "NODE_SELECTION"			, LNG("[CAP] Selection"),
		LNG("")
	);

	m_Parameters.Add_Value(
		m_Parameters("NODE_SELECTION")	, "EDIT_SEL_COLOR"			, LNG("[CAP] Color"),
		LNG(""),
		PARAMETER_TYPE_Color, SG_GET_RGB(255, 0, 0)
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Shapes::On_DataObject_Changed(void)
{
	_AttributeList_Set(m_Parameters("COLORS_ATTRIB")		, false);
	_AttributeList_Set(m_Parameters("LABEL_ATTRIB")			, true);
	_AttributeList_Set(m_Parameters("LABEL_ATTRIB_SIZE_BY")	, true);
#ifdef USE_HTMLINFO
	_AttributeList_Set(m_Parameters("EXTRAINFO_ATTRIB") , true);
#endif

	_Chart_Set_Options();

	m_pTable->DataObject_Changed(NULL);
}

//---------------------------------------------------------
void CWKSP_Shapes::On_Parameters_Changed(void)
{
	m_Def_Color	= Get_Color_asWX(m_Parameters("UNISYMBOL_COLOR")->asColor());

	//-----------------------------------------------------
	if( (m_iColor = m_Parameters("COLORS_ATTRIB")->asInt()) >= m_pShapes->Get_Field_Count() )
	{
		m_iColor	= -1;
	}

	if( m_iColor < 0 && (m_pClassify->Get_Mode() == CLASSIFY_METRIC || m_pClassify->Get_Mode() == CLASSIFY_SHADE) )
	{
		m_pClassify->Set_Mode(CLASSIFY_UNIQUE);
	}

	//-----------------------------------------------------
	if( (m_iLabel = m_Parameters("LABEL_ATTRIB")->asInt()) >= m_pShapes->Get_Field_Count() )
	{
		m_iLabel	= -1;
	}

	if( (m_iLabel_Size = m_Parameters("LABEL_ATTRIB_SIZE_BY")->asInt()) >= m_pShapes->Get_Field_Count() )
	{
		m_iLabel_Size	= -1;
	}

	//-----------------------------------------------------
#ifdef USE_HTMLINFO
	if( (m_iExtraInfo = m_Parameters("EXTRAINFO_ATTRIB")->asInt()) >= m_pShapes->Get_Field_Count() )
	{
		m_iExtraInfo	= -1;
	}
#endif

	//-----------------------------------------------------
	_Chart_Get_Options();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CWKSP_Shapes::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	//-----------------------------------------------------
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), wxT("COLORS_TYPE"))
	||	!SG_STR_CMP(pParameter->Get_Identifier(), wxT("COLORS_ATTRIB")) )
	{
		int		zField	= pParameters->Get_Parameter("COLORS_ATTRIB")->asInt();

		pParameters->Get_Parameter("METRIC_ZRANGE")->asRange()->Set_Range(
			m_pShapes->Get_Minimum(zField),
			m_pShapes->Get_Maximum(zField)
		);
	}

	//-----------------------------------------------------
	return( 1 );
}

//---------------------------------------------------------
void CWKSP_Shapes::On_Update_Views(void)
{
	m_pTable->Update_Views();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Shapes::_LUT_Create(void)
{
	int						iField, iRecord, old_Field, iID;
	double					dValue;
	TSG_Table_Index_Order	old_Order;
	CSG_Colors				*pColors;
	CSG_String				sFields, sValue;
	CSG_Table_Record		*pRecord, *pRecord_LUT;
	CSG_Table				*pTable, *pLUT;
	CSG_Parameters			Parameters;

	pTable	= Get_Table()->Get_Table();

	if( pTable->Get_Field_Count() <= 0 || pTable->Get_Record_Count() < 1 )
	{
		DLG_Message_Show(LNG("Function failed because no attributes are available"), LNG("Create Lookup Table"));
	}
	else
	{
		for(iField=0; iField<pTable->Get_Field_Count(); iField++)
		{
			sFields.Append(pTable->Get_Field_Name(iField));
			sFields.Append(wxT("|"));
		}

		Parameters.Create(NULL, LNG("Choose Attribute"), LNG(""));
		Parameters.Add_Choice(NULL, "FIELD"	, LNG("Attribute")	, LNG(""), sFields);
		Parameters.Add_Colors(NULL, "COLOR"	, LNG("Colors")		, LNG(""));

		if( DLG_Parameters(&Parameters) )
		{
			iField		= Parameters("FIELD")	->asInt();

			if( pTable->Get_Field_Type(iField) == SG_DATATYPE_String )
			{
				pTable->Add_Field(CSG_String::Format(wxT("%s_LUT"), pTable->Get_Field_Name(iField)), SG_DATATYPE_Int);
				iID		= pTable->Get_Field_Count() - 1;
			}
			else
			{
				iID		= iField;
			}

			pLUT		= m_Parameters("LUT")	->asTable();
			pLUT		->Del_Records();

			old_Order	= pTable->Get_Index_Order(0);
			old_Field	= pTable->Get_Index_Field(0);

			pTable->Set_Index(iField, TABLE_INDEX_Ascending);
			sValue		= pTable->Get_Record_byIndex(0)->asString(iField);
			dValue		= iID != iField ? 1.0 : pTable->Get_Record_byIndex(0)->asDouble(iField);

			for(iRecord=0; iRecord<pTable->Get_Record_Count(); iRecord++)
			{
				pRecord	= pTable->Get_Record_byIndex(iRecord);

				if( iRecord == 0 || sValue.Cmp(pRecord->asString(iField)) )
				{
					if( iRecord > 0 )
					{
						sValue		= pRecord->asString(iField);
						dValue		= iID != iField ? dValue + 1.0 : pRecord->asDouble(iField);
					}

					pRecord_LUT	= pLUT->Add_Record();
					pRecord_LUT	->Set_Value(1, sValue.c_str());	// Name
					pRecord_LUT	->Set_Value(2, sValue.c_str());	// Description
					pRecord_LUT	->Set_Value(3, dValue);			// Minimum
					pRecord_LUT	->Set_Value(4, dValue);			// Maximum
				}

				if( iID != iField )
				{
					pRecord->Set_Value(iID, dValue);
				}
			}

			pColors	= Parameters("COLOR")->asColors();
			pColors->Set_Count(pLUT->Get_Record_Count());

			for(iRecord=0; iRecord<pLUT->Get_Record_Count(); iRecord++)
			{
				pLUT->Get_Record(iRecord)->Set_Value(0, pColors->Get_Color(iRecord));
			}

			pTable->Set_Index(old_Field, old_Order);

			DataObject_Changed();

			m_Parameters("COLORS_TYPE")		->Set_Value(1);		// Lookup Table
			m_Parameters("COLORS_ATTRIB")	->Set_Value(iID);

			Parameters_Changed();
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_Shapes::Get_Value(CSG_Point ptWorld, double Epsilon)
{
	CSG_Shape		*pShape;

	if( (pShape = m_pShapes->Get_Shape(ptWorld, Epsilon)) != NULL )
	{
		if( m_iColor >= 0 )
		{
			switch( m_pClassify->Get_Mode() )
			{
			case CLASSIFY_LUT:
				return( m_pClassify->Get_Class_Name_byValue(pShape->asDouble(m_iColor)) );

			case CLASSIFY_METRIC:	default:
				return( pShape->asString(m_iColor) );
			}
		}
		else
		{
			return( wxString::Format(wxT("%s: %d"), LNG("[CAP] Index"), pShape->Get_Index() + 1) );
		}
	}

	return( LNG("") );
}

//---------------------------------------------------------
double CWKSP_Shapes::Get_Value_Range(void)
{
	return( m_iColor >= 0 ? m_pShapes->Get_Range(m_iColor) : 0.0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Shapes::asImage(CSG_Grid *pImage)
{
	return( false );
}

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Shapes::On_Draw(CWKSP_Map_DC &dc_Map, bool bEdit)
{
	int			iShape;
	CSG_Shape	*pShape;

	//-----------------------------------------------------
	if( Get_Extent().Intersects(dc_Map.m_rWorld) != INTERSECTION_None )
	{
		double	Transparency	= m_Parameters("DISPLAY_TRANSPARENCY")->asDouble() / 100.0;

		CWKSP_Map_DC	*pDC	= Transparency > 0.0 ? new CWKSP_Map_DC(dc_Map.m_rWorld, dc_Map.m_rDC, dc_Map.m_Scale, SG_GET_RGB(255, 255, 255)) : NULL;
		CWKSP_Map_DC	&dc		= pDC ? *pDC : dc_Map;

		m_Edit_Color	= Get_Color_asWX(m_Parameters("EDIT_COLOR")		->asInt());
		m_Sel_Color		= Get_Color_asWX(m_Parameters("EDIT_SEL_COLOR")	->asInt());

		_Draw_Initialize(dc);

		//-------------------------------------------------
		if( bEdit && (m_Edit_pShape || m_pShapes->Get_Selection_Count() > 0) )
		{
			for(iShape=0; iShape<m_pShapes->Get_Count(); iShape++)
			{
				pShape	= m_pShapes->Get_Shape(iShape);

				if( !pShape->is_Selected() && dc.m_rWorld.Intersects(pShape->Get_Extent()) != INTERSECTION_None )
				{
					_Draw_Shape(dc, pShape, false);
				}
			}

			//---------------------------------------------
			for(iShape=1; iShape<m_pShapes->Get_Selection_Count(); iShape++)
			{
				pShape	= m_pShapes->Get_Selection(iShape);

				if( dc.m_rWorld.Intersects(pShape->Get_Extent()) != INTERSECTION_None )
				{
					_Draw_Shape(dc, pShape, true);
				}
			}

			if( m_Edit_pShape )
			{
				_Edit_Shape_Draw(dc);
			}
			else
			{
				pShape	= m_pShapes->Get_Selection(0);

				if( dc.m_rWorld.Intersects(pShape->Get_Extent()) != INTERSECTION_None )
				{
					_Draw_Shape(dc, pShape, true);
				}
			}
		}

		//-------------------------------------------------
		else
		{
			for(iShape=0; iShape<m_pShapes->Get_Count(); iShape++)
			{
				pShape	= m_pShapes->Get_Shape(iShape);

				if( dc.m_rWorld.Intersects(pShape->Get_Extent()) != INTERSECTION_None )
				{
					_Draw_Shape(dc, pShape, false);
				}
			}

			if( _Chart_is_Valid() )
			{
				for(iShape=0; iShape<m_pShapes->Get_Count(); iShape++)
				{
					pShape	= m_pShapes->Get_Shape(iShape);

					if( dc.m_rWorld.Intersects(pShape->Get_Extent()) != INTERSECTION_None )
					{
						_Draw_Chart(dc, pShape);
					}
				}
			}
		}

		//-------------------------------------------------
		if( m_iLabel >= 0 )
		{
			int		Size;
			double	dSize;
			wxFont	Font	= *m_Parameters("LABEL_ATTRIB_FONT")->asFont();

			switch( m_Parameters("LABEL_ATTRIB_SIZE_TYPE")->asInt() )
			{
			case 0:	default:
				dSize	= m_iLabel_Size < 0 ? m_Parameters("LABEL_ATTRIB_FONT")->asFont()->GetPointSize() : 1.0;
				break;

			case 1:
				dSize	= dc.m_World2DC * m_Parameters("LABEL_ATTRIB_SIZE")->asDouble();
				break;
			}

			dc.dc.SetTextForeground(m_Parameters("LABEL_ATTRIB_FONT")->asColor());

			if( m_iLabel_Size < 0 && (Size = (int)(0.5 + dSize)) > 0 )
			{
				Font.SetPointSize(Size);
				dc.dc.SetFont(Font);

				for(iShape=0; iShape<m_pShapes->Get_Count(); iShape++)
				{
					pShape	= m_pShapes->Get_Shape(iShape);

					if( dc.m_rWorld.Intersects(pShape->Get_Extent()) != INTERSECTION_None )
					{
						_Draw_Label(dc, pShape);
					}
				}
			}
			else
			{
				for(iShape=0; iShape<m_pShapes->Get_Count(); iShape++)
				{
					pShape	= m_pShapes->Get_Shape(iShape);

					if( dc.m_rWorld.Intersects(pShape->Get_Extent()) != INTERSECTION_None
					&&	(Size = (int)(0.5 + dSize * pShape->asDouble(m_iLabel_Size))) > 0 )
					{
						Font.SetPointSize(Size);
						dc.dc.SetFont(Font);

						_Draw_Label(dc, pShape);
					}
				}
			}
		}

		//-------------------------------------------------
		if( pDC )
		{
			dc_Map.Draw_DC(dc, Transparency);

			delete(pDC);
		}
	}

	//-----------------------------------------------------
	else if( m_Edit_pShape )
	{
		_Edit_Shape_Draw(dc_Map);
	}

	//-----------------------------------------------------
	dc_Map.dc.SetBrush(wxNullBrush);
	dc_Map.dc.SetPen  (wxNullPen);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter * CWKSP_Shapes::_AttributeList_Add(CSG_Parameter *pNode, const char *Identifier, const wxChar *Name, const wxChar *Description)
{
	CSG_Parameter *pParameter;

	pParameter	= m_Parameters.Add_Choice(
		pNode, Identifier, Name, Description,
		wxString::Format(wxT("%s|"), LNG("[VAL] [default]")), 0
	);

	return( pParameter );
}

//---------------------------------------------------------
void CWKSP_Shapes::_AttributeList_Set(CSG_Parameter *pFields, bool bAddNoField)
{
	if( pFields && pFields->Get_Type() == PARAMETER_TYPE_Choice )
	{
		wxString	s;

		for(int i=0; i<m_pShapes->Get_Field_Count(); i++)
		{
			s.Append(wxString::Format(wxT("%s|"), m_pShapes->Get_Field_Name(i)));
		}

		if( bAddNoField )
		{
			s.Append(wxString::Format(wxT("%s|"), LNG("[VAL] [none]")));
		}

		pFields->asChoice()->Set_Items(s);

		if( bAddNoField )
		{
			pFields->Set_Value(m_pShapes->Get_Field_Count());
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameter * CWKSP_Shapes::_BrushList_Add(CSG_Parameter *pNode, const char *Identifier, const wxChar *Name, const wxChar *Description)
{
	CSG_Parameter *pParameter;

	pParameter	= m_Parameters.Add_Choice(
		pNode, Identifier, Name, Description,
		wxString::Format(wxT("%s|%s|%s|%s|%s|%s|%s|%s|"),
			LNG("[VAL] Opaque"),
			LNG("[VAL] Transparent"),
			LNG("[VAL] Backward Diagonal"),
			LNG("[VAL] Cross Diagonal"),
			LNG("[VAL] Forward Diagonal"),
			LNG("[VAL] Cross"),
			LNG("[VAL] Horizontal"),
			LNG("[VAL] Vertical")
		), 0
	);

	return( pParameter );
}

//---------------------------------------------------------
int CWKSP_Shapes::_BrushList_Get_Style(int Index)
{
	switch( Index )
	{
	default:
	case 0:	return( wxSOLID				);
	case 1:	return( wxTRANSPARENT		);
	case 2:	return( wxBDIAGONAL_HATCH	);
	case 3:	return( wxCROSSDIAG_HATCH	);
	case 4:	return( wxFDIAGONAL_HATCH	);
	case 5:	return( wxCROSS_HATCH		);
	case 6:	return( wxHORIZONTAL_HATCH	);
	case 7:	return( wxVERTICAL_HATCH	);
	}
}

//---------------------------------------------------------
CSG_Parameter * CWKSP_Shapes::_PenList_Add(CSG_Parameter *pNode, const char *Identifier, const wxChar *Name, const wxChar *Description)
{
	CSG_Parameter *pParameter;

	pParameter	= m_Parameters.Add_Choice(
		pNode, Identifier, Name, Description,
		wxString::Format(wxT("%s|%s|%s|%s|%s|"),
			LNG("[VAL] Solid"),
			LNG("[VAL] Dotted"),
			LNG("[VAL] Long Dashed"),
			LNG("[VAL] Short Dashed"),
			LNG("[VAL] Dot And Dash")
		), 0
	);

	return( pParameter );
}

//---------------------------------------------------------
int CWKSP_Shapes::_PenList_Get_Style(int Index)
{
	switch( Index )
	{
	default:
	case 0:	return( wxSOLID );
	case 1:	return( wxDOT );
	case 2:	return( wxLONG_DASH );
	case 3:	return( wxSHORT_DASH );
	case 4:	return( wxDOT_DASH );
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Shapes::_Chart_Set_Options(void)
{
	CSG_Parameters	*pChart	= m_Parameters("DISPLAY_CHART")->asParameters();

	pChart->Destroy();
	m_Chart.Clear();

	if( 1 )
	{
		int				i, n;
		CSG_Colors		Colors;
		CSG_String		sFields;
		CSG_Parameter	*pFields, *pField;

		for(i=0, n=0; i<m_pShapes->Get_Field_Count(); i++)
		{
			if( m_pShapes->Get_Field_Type(i) != SG_DATATYPE_String )
				n++;
		}

		if( n > 0 )
		{
			pChart->Create(NULL, LNG("[CAP] Chart Properties"), LNG(""));

			pChart->Add_Choice(
				NULL, "TYPE"	, LNG("Chart Type"),
				LNG(""),
				CSG_String::Format(wxT("%s|%s|%s|%s|"),
					LNG("bar"),
					LNG("bar (not outlined)"),
					LNG("pie"),
					LNG("pie (not outlined)")
				), 0
			);

			pChart->Add_Choice(
				NULL, "SIZE_FIELD"		, LNG("[CAP] Attribute (Size)"),
				LNG(""),
				CSG_String::Format(wxT("%s|"), LNG("[VAL] [not set]")), 0
			);

			pChart->Add_Choice(
				NULL, "SIZE_TYPE"		, LNG("[CAP] Size relates to..."),
				LNG(""),
				wxString::Format(wxT("%s|%s|"),
					LNG("[VAL] Screen"),
					LNG("[VAL] Map Units")
				), 0
			);

			pChart->Add_Value(
				NULL, "SIZE_DEFAULT"	, LNG("[CAP] Default Size"),
				LNG(""),
				PARAMETER_TYPE_Double, 15, 0, true
			);

			pChart->Add_Range(
				NULL, "SIZE_RANGE"		, LNG("[CAP] Size Range"),
				LNG(""),
				5, 25, 0, true
			);

			pFields	= pChart->Add_Node(NULL, "NODE_FIELDS", LNG("Fields"), LNG(""));

			Colors.Set_Count(n);

			for(i=0, n=0; i<m_pShapes->Get_Field_Count(); i++)
			{
				if( m_pShapes->Get_Field_Type(i) != SG_DATATYPE_String )
				{
					sFields.Append(CSG_String::Format(wxT("%s|"), m_pShapes->Get_Field_Name(i)));

					pField	= pChart->Add_Value(
						pFields	, wxString::Format(wxT("FIELD_%d"), i), m_pShapes->Get_Field_Name(i),
						LNG(""),
						PARAMETER_TYPE_Bool , false
					);

					pChart->Add_Value(
						pField	, wxString::Format(wxT("COLOR_%d"), i), SG_T(""),
						LNG(""),
						PARAMETER_TYPE_Color, Colors.Get_Color(n++)
					);
				}
			}

			sFields.Append(CSG_String::Format(wxT("%s|"), LNG("[VAL] [none]")));
			pFields	= pChart->Get_Parameter("SIZE_FIELD");
			pFields->asChoice()->Set_Items(sFields);
			pFields->Set_Value(m_pShapes->Get_Field_Count());

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Shapes::_Chart_Get_Options(void)
{
	CSG_Parameters	*pChart	= m_Parameters("DISPLAY_CHART")->asParameters();
	CSG_Parameter	*p;

	m_Chart.Clear();
	m_Chart_sField=-1;

	if( pChart->Get_Parameter("NODE_FIELDS") )
	{
		for(int i=0, n=0; i<m_pShapes->Get_Field_Count(); i++)
		{
			if(	(p = pChart->Get_Parameter(wxString::Format(wxT("FIELD_%d"), i))) != NULL )
			{
				if( pChart->Get_Parameter("SIZE_FIELD")->asInt() == n++ )
					m_Chart_sField	= i;

				if( p->asBool() && (p = pChart->Get_Parameter(wxString::Format(wxT("COLOR_%d"), i))) != NULL )
					m_Chart.Add(i, p->asColor());
			}
		}

		m_Chart_Type	= pChart->Get_Parameter("TYPE")			->asInt();
		m_Chart_sType	= pChart->Get_Parameter("SIZE_TYPE")	->asInt();
		m_Chart_sSize	= m_Chart_sField < 0
						? pChart->Get_Parameter("SIZE_DEFAULT")	->asDouble()
						: pChart->Get_Parameter("SIZE_RANGE")	->asRange()->Get_LoVal();
		m_Chart_sRange	= pChart->Get_Parameter("SIZE_RANGE")	->asRange()->Get_HiVal() - m_Chart_sSize;

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CWKSP_Shapes::_Draw_Chart(CWKSP_Map_DC &dc_Map, CSG_Shape *pShape)
{
	if( _Chart_is_Valid() )
	{
		int			s;
		double		dSize	= m_Chart_sSize;
		TSG_Point_Int	p;

		if( m_Chart_sField >= 0 )
		{
			double	range	= m_pShapes->Get_Range(m_Chart_sField);

			if( range > 0.0 )
			{
				dSize	+= m_Chart_sRange * ((pShape->asDouble(m_Chart_sField) - m_pShapes->Get_Minimum(m_Chart_sField)) / range);
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

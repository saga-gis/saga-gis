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
//                University of Hamburg                  //
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
#include "active_attributes.h"

#include "wksp_map_control.h"

#include "wksp_layer_classify.h"

#include "wksp_pointcloud.h"
#include "wksp_table.h"

#include "view_scatterplot.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_PointCloud::CWKSP_PointCloud(CSG_PointCloud *pPointCloud)
	: CWKSP_Layer(pPointCloud)
{
	m_pTable		= new CWKSP_Table(pPointCloud);

	m_Edit_Attributes.Destroy();
	m_Edit_Attributes.Add_Field(_TL("Name") , SG_DATATYPE_String);
	m_Edit_Attributes.Add_Field(_TL("Value"), SG_DATATYPE_String);

	//-----------------------------------------------------
	On_Create_Parameters();

	DataObject_Changed();

	m_Parameters("COLORS_TYPE"  )->Set_Value(CLASSIFY_METRIC);
	m_Parameters("METRIC_ATTRIB")->Set_Value(2);

	On_Parameter_Changed(&m_Parameters, m_Parameters("METRIC_ATTRIB"), PARAMETER_CHECK_ALL);

	Parameters_Changed();
}

//---------------------------------------------------------
CWKSP_PointCloud::~CWKSP_PointCloud(void)
{
	delete(m_pTable);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_PointCloud::Get_Description(void)
{
	wxString	s;

	//-----------------------------------------------------
	s	+= wxString::Format(wxT("<b>%s</b>"), _TL("Point Cloud"));

	s	+= wxT("<table border=\"0\">");

	DESC_ADD_STR(_TL("Name")			, m_pObject->Get_Name());
	DESC_ADD_STR(_TL("Description")		, m_pObject->Get_Description());
	DESC_ADD_STR(_TL("File")			, SG_File_Exists(m_pObject->Get_File_Name()) ? m_pObject->Get_File_Name() : _TL("memory"));
	DESC_ADD_STR(_TL("Modified")		, m_pObject->is_Modified() ? _TL("yes") : _TL("no"));
	DESC_ADD_STR(_TL("Projection")		, m_pObject->Get_Projection().Get_Description().c_str());
	DESC_ADD_FLT(_TL("West")			, Get_PointCloud()->Get_Extent().Get_XMin());
	DESC_ADD_FLT(_TL("East")			, Get_PointCloud()->Get_Extent().Get_XMax());
	DESC_ADD_FLT(_TL("West-East")		, Get_PointCloud()->Get_Extent().Get_XRange());
	DESC_ADD_FLT(_TL("South")			, Get_PointCloud()->Get_Extent().Get_YMin());
	DESC_ADD_FLT(_TL("North")			, Get_PointCloud()->Get_Extent().Get_YMax());
	DESC_ADD_FLT(_TL("South-North")		, Get_PointCloud()->Get_Extent().Get_YRange());
	DESC_ADD_INT(_TL("Number of Points"), Get_PointCloud()->Get_Count());

	s	+= wxT("</table>");

	s	+= Get_TableInfo_asHTML(Get_PointCloud());

	//-----------------------------------------------------
//	s	+= wxString::Format(wxT("<hr><b>%s</b><font size=\"-1\">"), _TL("Data History"));
//	s	+= Get_PointCloud()->Get_History().Get_HTML();
//	s	+= wxString::Format(wxT("</font"));

	//-----------------------------------------------------
	return( s );
}

//---------------------------------------------------------
wxMenu * CWKSP_PointCloud::Get_Menu(void)
{
	wxMenu	*pMenu	= new wxMenu(m_pObject->Get_Name());

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_CLOSE);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_POINTCLOUD_SHOW);
	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_SAVE);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_SAVEAS);
	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_DATA_PROJECTION);
	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu,  true, ID_CMD_SHAPES_HISTOGRAM);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_SHAPES_SET_LUT);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_SETTINGS_COPY);

	pMenu->AppendSeparator();

	//-----------------------------------------------------
	wxMenu	*pSubMenu	= new wxMenu(_TL("Classification"));

	CMD_Menu_Add_Item(pSubMenu	, false, ID_CMD_POINTCLOUD_RANGE_MINMAX);
	CMD_Menu_Add_Item(pSubMenu	, false, ID_CMD_POINTCLOUD_RANGE_STDDEV150);
	CMD_Menu_Add_Item(pSubMenu	, false, ID_CMD_POINTCLOUD_RANGE_STDDEV200);

	pMenu->Append(ID_CMD_WKSP_FIRST, _TL("Classification"), pSubMenu);

	//-----------------------------------------------------
	wxMenu	*pTable	= new wxMenu(_TL("Table"));

	CMD_Menu_Add_Item(pTable,  true, ID_CMD_TABLES_SHOW);
//	CMD_Menu_Add_Item(pTable,  true, ID_CMD_TABLES_DIAGRAM);
//	CMD_Menu_Add_Item(pTable, false, ID_CMD_TABLES_SCATTERPLOT);

	pMenu->Append(ID_CMD_WKSP_FIRST, _TL("Attributes"), pTable);

	return( pMenu );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
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

	case ID_CMD_SHAPES_SET_LUT:
		_LUT_Create();
		break;

	case ID_CMD_POINTCLOUD_RANGE_MINMAX:
		Set_Color_Range(
			Get_PointCloud()->Get_Minimum(m_Color_Field),
			Get_PointCloud()->Get_Maximum(m_Color_Field)
		);
		break;

	case ID_CMD_POINTCLOUD_RANGE_STDDEV150:
		Set_Color_Range(
			Get_PointCloud()->Get_Mean(m_Color_Field) - 1.5 * Get_PointCloud()->Get_StdDev(m_Color_Field),
			Get_PointCloud()->Get_Mean(m_Color_Field) + 1.5 * Get_PointCloud()->Get_StdDev(m_Color_Field)
		);
		break;

	case ID_CMD_POINTCLOUD_RANGE_STDDEV200:
		Set_Color_Range(
			Get_PointCloud()->Get_Mean(m_Color_Field) - 2.0 * Get_PointCloud()->Get_StdDev(m_Color_Field),
			Get_PointCloud()->Get_Mean(m_Color_Field) + 2.0 * Get_PointCloud()->Get_StdDev(m_Color_Field)
		);
		break;

	case ID_CMD_SHAPES_EDIT_SEL_CLEAR:
		Get_PointCloud()->Select();
		Update_Views();
		break;

	case ID_CMD_SHAPES_EDIT_SEL_INVERT:
		Get_PointCloud()->Inv_Selection();
		Update_Views();
		break;

	case ID_CMD_SHAPES_EDIT_DEL_SHAPE:
		if( Get_PointCloud()->Get_Selection_Count() > 0 && DLG_Message_Confirm(_TL("Delete selected point(s)."), _TL("Edit Point Cloud")) )
		{
			Get_PointCloud()->Del_Selection();
			Update_Views();
		}
		break;

	case ID_CMD_TABLES_SHOW:
		m_pTable->Toggle_View();
		break;

	case ID_CMD_TABLES_DIAGRAM:
		m_pTable->Toggle_Diagram();
		break;

	case ID_CMD_SHAPES_HISTOGRAM:
		Histogram_Toggle();
		break;
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

	case ID_CMD_POINTCLOUD_LAST:
		break;

	case ID_CMD_TABLES_SHOW:
		event.Check(m_pTable->Get_View() != NULL);
		break;

	case ID_CMD_TABLES_DIAGRAM:
		event.Check(m_pTable->Get_Diagram() != NULL);
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
void CWKSP_PointCloud::On_Create_Parameters(void)
{
	CWKSP_Layer::On_Create_Parameters();

	//-----------------------------------------------------
	m_Parameters.Add_Value(
		m_Parameters("NODE_DISPLAY")	, "DISPLAY_SIZE"			, _TL("Point Size"),
		_TL(""),
		PARAMETER_TYPE_Int, 0, 0, true
	);

	m_Parameters.Add_Choice(
		m_Parameters("NODE_DISPLAY")	, "DISPLAY_VALUE_AGGREGATE"		, _TL("Value Aggregation"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("first value"),
			_TL("last value"),
			_TL("lowest z"),
			_TL("highest z")
		), 3
	);

	//-----------------------------------------------------
	// Classification...

	((CSG_Parameter_Choice *)m_Parameters("COLORS_TYPE")->Get_Data())->Set_Items(
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|"),
			_TL("Single Symbol"   ),	// CLASSIFY_UNIQUE
			_TL("Lookup Table"    ),	// CLASSIFY_LUT
			_TL("Discrete Colors" ),	// CLASSIFY_METRIC
			_TL("Graduated Colors"),	// CLASSIFY_GRADUATED
		//	_TL("Shade"           ),	// CLASSIFY_SHADE
		//	_TL("RGB Overlay"     ),	// CLASSIFY_OVERLAY
			_TL("RGB"             )		// CLASSIFY_RGB
		)
	);

	_AttributeList_Add(
		m_Parameters("NODE_LUT")		, "LUT_ATTRIB"				, _TL("Attribute"),
		_TL("")
	);

	_AttributeList_Add(
		m_Parameters("NODE_METRIC")		, "METRIC_ATTRIB"			, _TL("Attribute"),
		_TL("")
	);

	m_Parameters.Add_Node(
		m_Parameters("NODE_COLORS")		, "NODE_RGB"				, _TL("RGB"),
		_TL("")
	);

	_AttributeList_Add(
		m_Parameters("NODE_RGB")		, "RGB_ATTRIB"				, _TL("Attribute"),
		_TL("")
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_PointCloud::On_DataObject_Changed(void)
{
	CWKSP_Layer::On_DataObject_Changed();

	_AttributeList_Set(m_Parameters("LUT_ATTRIB"   ), false);
	_AttributeList_Set(m_Parameters("METRIC_ATTRIB"), false);
	_AttributeList_Set(m_Parameters("RGB_ATTRIB"   ), false);

	m_pTable->DataObject_Changed();
}

//---------------------------------------------------------
void CWKSP_PointCloud::On_Parameters_Changed(void)
{
	CWKSP_Layer::On_Parameters_Changed();

	//-----------------------------------------------------
	switch( m_Parameters("COLORS_TYPE")->asInt() )
	{
	default:
	case 0:	m_Color_Field	= -1;										break;	// CLASSIFY_UNIQUE
	case 1:	m_Color_Field	= m_Parameters("LUT_ATTRIB"   )->asInt();	break;	// CLASSIFY_LUT
	case 2:	m_Color_Field	= m_Parameters("METRIC_ATTRIB")->asInt();	break;	// CLASSIFY_METRIC
	case 3:	m_Color_Field	= m_Parameters("METRIC_ATTRIB")->asInt();	break;	// CLASSIFY_GRADUATED
	case 4:	m_Color_Field	= m_Parameters("RGB_ATTRIB"   )->asInt();	break;	// CLASSIFY_RGB
	}

	if( m_Color_Field < 0 || m_Color_Field >= Get_PointCloud()->Get_Field_Count() )
	{
		m_Color_Field	= -1;

		m_pClassify->Set_Mode(CLASSIFY_UNIQUE);
	}
	else if( m_Parameters("COLORS_TYPE")->asInt() == 4 )
	{
		m_pClassify->Set_Mode(CLASSIFY_RGB);
	}

	//-----------------------------------------------------
	long	DefColor	= m_Parameters("UNISYMBOL_COLOR")->asColor();
	m_Color_Pen			= wxColour(SG_GET_R(DefColor), SG_GET_G(DefColor), SG_GET_B(DefColor));

	m_PointSize			= m_Parameters("DISPLAY_SIZE")	->asInt();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CWKSP_PointCloud::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter, int Flags)
{
	//-----------------------------------------------------
	if( Flags & PARAMETER_CHECK_VALUES )
	{
		if(	!SG_STR_CMP(pParameter->Get_Identifier(), wxT("METRIC_ATTRIB")) )
		{
			CSG_Parameters	Parameters;
	
			int		zField	= pParameter->asInt();

			double	m	= Get_PointCloud()->Get_Mean  (zField);
			double	s	= Get_PointCloud()->Get_StdDev(zField) * 2.0;
			double	min	= m - s;	if( min < Get_PointCloud()->Get_Minimum(zField) )	min	= Get_PointCloud()->Get_Minimum(zField);
			double	max	= m + s;	if( max > Get_PointCloud()->Get_Maximum(zField) )	max	= Get_PointCloud()->Get_Maximum(zField);

			pParameters->Get_Parameter("METRIC_ZRANGE")->asRange()->Set_Range(min, max);
		}
	}

	//-----------------------------------------------------
	if( Flags & PARAMETER_CHECK_ENABLE )
	{
		if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("COLORS_TYPE")) )
		{
			int	Value	= pParameter->asInt();

			pParameters->Get_Parameter("NODE_UNISYMBOL")->Set_Enabled(Value == CLASSIFY_UNIQUE);
			pParameters->Get_Parameter("NODE_LUT"      )->Set_Enabled(Value == CLASSIFY_LUT);
			pParameters->Get_Parameter("NODE_METRIC"   )->Set_Enabled(Value == CLASSIFY_METRIC || Value == CLASSIFY_GRADUATED);
			pParameters->Get_Parameter("NODE_RGB"	   )->Set_Enabled(Value == 4);

			return( 1 );
		}
	}

	//-----------------------------------------------------
	return( CWKSP_Layer::On_Parameter_Changed(pParameters, pParameter, Flags) );
}

//---------------------------------------------------------
void CWKSP_PointCloud::On_Update_Views(void)
{
	m_pTable->Update_Views();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_PointCloud::Get_Name_Attribute(void)
{
	return(	m_Color_Field < 0 || m_pClassify->Get_Mode() == CLASSIFY_UNIQUE ? SG_T("") : Get_PointCloud()->Get_Field_Name(m_Color_Field) );
}

//---------------------------------------------------------
CSG_Parameter * CWKSP_PointCloud::_AttributeList_Add(CSG_Parameter *pNode, const CSG_String &Identifier, const CSG_String &Name, const CSG_String &Description)
{
	CSG_Parameter *pParameter;

	pParameter	= m_Parameters.Add_Choice(
		pNode, Identifier, Name, Description,
		CSG_String::Format(SG_T("%s|"), _TL("<default>")), 0
	);

	return( pParameter );
}

//---------------------------------------------------------
void CWKSP_PointCloud::_AttributeList_Set(CSG_Parameter *pFields, bool bAddNoField)
{
	if( pFields && pFields->Get_Type() == PARAMETER_TYPE_Choice )
	{
		wxString	s;

		for(int i=0; i<Get_PointCloud()->Get_Field_Count(); i++)
		{
			s.Append(wxString::Format(wxT("%s|"), Get_PointCloud()->Get_Field_Name(i)));
		}

		if( bAddNoField )
		{
			s.Append(wxString::Format(wxT("%s|"), _TL("<none>")));
		}

		pFields->asChoice()->Set_Items(s);

		if( bAddNoField )
		{
			pFields->Set_Value(Get_PointCloud()->Get_Field_Count());
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_PointCloud::_LUT_Create(void)
{
	int				iField, Method;
	CSG_Colors		*pColors;
	CSG_Table		*pLUT;

	//-----------------------------------------------------
	if( Get_PointCloud()->Get_Field_Count() <= 0 || Get_PointCloud()->Get_Record_Count() < 1 )
	{
		DLG_Message_Show(_TL("Function failed because no attributes are available"), _TL("Create Lookup Table"));

		return;
	}

	//-----------------------------------------------------
	CSG_String	sFields;

	for(iField=0; iField<Get_PointCloud()->Get_Field_Count(); iField++)
	{
		sFields	+= Get_PointCloud()->Get_Field_Name(iField);	sFields	+= SG_T("|");
	}

	//-----------------------------------------------------
	static CSG_Parameters	Parameters;

	if( Parameters.Get_Count() != 0 )
	{
		Parameters("FIELD")->asChoice()->Set_Items(sFields);
	}
	else
	{
		Parameters.Create(NULL, _TL("Create Lookup Table"), _TL(""));

		Parameters.Add_Choice(
			NULL, "FIELD"	, _TL("Attribute"),
			_TL(""),
			sFields
		);

		Parameters.Add_Colors(
			NULL, "COLOR"	, _TL("Colors"),
			_TL("")
		)->asColors()->Set_Count(10);

		Parameters.Add_Choice(
			NULL, "METHOD"	, _TL("Classification Method"),
			_TL(""),
			CSG_String::Format(SG_T("%s|%s|%s|"),
				_TL("unique values"),
				_TL("equal intervals"),
				_TL("quantiles")
			), 0
		);
	}

	if( !DLG_Parameters(&Parameters) )
	{
		return;
	}

	//-----------------------------------------------------
	pColors	= Parameters("COLOR" )->asColors();
	iField	= Parameters("FIELD" )->asInt();
	Method	= Parameters("METHOD")->asInt();

	pLUT	= m_Parameters("LUT" )->asTable();
	pLUT	->Del_Records();

	switch( Method )
	{
	//-----------------------------------------------------
	case 0:	// unique values
		{
			TSG_Table_Index_Order	old_Order	= Get_PointCloud()->Get_Index_Order(0);
			int						old_Field	= Get_PointCloud()->Get_Index_Field(0);

			TSG_Data_Type	Type	= SG_Data_Type_is_Numeric(Get_PointCloud()->Get_Field_Type(iField))
									? SG_DATATYPE_Double : SG_DATATYPE_String;

			pLUT->Set_Field_Type(LUT_MIN, Type);
			pLUT->Set_Field_Type(LUT_MAX, Type);

			Get_PointCloud()->Set_Index(iField, TABLE_INDEX_Ascending);

			CSG_String	sValue;

			for(int iRecord=0; iRecord<Get_PointCloud()->Get_Count(); iRecord++)
			{
				CSG_Table_Record	*pRecord	= Get_PointCloud()->Get_Record_byIndex(iRecord);

				if( iRecord == 0 || sValue.Cmp(pRecord->asString(iField)) )
				{
					sValue	= pRecord->asString(iField);

					CSG_Table_Record	*pClass	= pLUT->Add_Record();

					pClass->Set_Value(1, sValue);	// Name
					pClass->Set_Value(2, sValue);	// Description
					pClass->Set_Value(3, sValue);	// Minimum
					pClass->Set_Value(4, sValue);	// Maximum
				}
			}

			pColors->Set_Count(pLUT->Get_Count());

			for(int iClass=0; iClass<pLUT->Get_Count(); iClass++)
			{
				pLUT->Get_Record(iClass)->Set_Value(0, pColors->Get_Color(iClass));
			}

			Get_PointCloud()->Set_Index(old_Field, old_Order);
		}
		break;

	//-----------------------------------------------------
	case 1:	// equal intervals
		{
			double	Minimum, Maximum, Interval;

			Interval	= Get_PointCloud()->Get_Range  (iField) / (double)pColors->Get_Count();
			Minimum		= Get_PointCloud()->Get_Minimum(iField);

			pLUT->Set_Field_Type(LUT_MIN, SG_DATATYPE_Double);
			pLUT->Set_Field_Type(LUT_MAX, SG_DATATYPE_Double);

			for(int iClass=0; iClass<pColors->Get_Count(); iClass++, Minimum+=Interval)
			{
				Maximum	= iClass < pColors->Get_Count() - 1 ? Minimum + Interval : Get_PointCloud()->Get_Maximum(iField) + 1.0;

				CSG_String	sValue;	sValue.Printf(SG_T("%s - %s"),
					SG_Get_String(Minimum, -2).c_str(),
					SG_Get_String(Maximum, -2).c_str()
				);

				CSG_Table_Record	*pClass	= pLUT->Add_Record();

				pClass->Set_Value(0, pColors->Get_Color(iClass));
				pClass->Set_Value(1, sValue);	// Name
				pClass->Set_Value(2, sValue);	// Description
				pClass->Set_Value(3, Minimum);	// Minimum
				pClass->Set_Value(4, Maximum);	// Maximum
			}
		}
		break;

		//-----------------------------------------------------
	case 2:	// quantiles
		{
			TSG_Table_Index_Order	old_Order	= Get_PointCloud()->Get_Index_Order(0);
			int						old_Field	= Get_PointCloud()->Get_Index_Field(0);

			Get_PointCloud()->Set_Index(iField, TABLE_INDEX_Ascending);

			pLUT->Set_Field_Type(LUT_MIN, SG_DATATYPE_Double);
			pLUT->Set_Field_Type(LUT_MAX, SG_DATATYPE_Double);

			if( Get_PointCloud()->Get_Count() < pColors->Get_Count() )
			{
				pColors->Set_Count(Get_PointCloud()->Get_Count());
			}

			double	Minimum, Maximum, Count, iRecord;

			Maximum	= Get_PointCloud()->Get_Minimum(iField);
			iRecord	= Count	= Get_PointCloud()->Get_Count() / (double)pColors->Get_Count();

			for(int iClass=0; iClass<pColors->Get_Count(); iClass++, iRecord+=Count)
			{
				Minimum	= Maximum;
				Maximum	= iRecord < Get_PointCloud()->Get_Count() ? Get_PointCloud()->Get_Record_byIndex((int)iRecord)->asDouble(iField) : Get_PointCloud()->Get_Maximum(iField) + 1.0;

				CSG_String	sValue;	sValue.Printf(SG_T("%s - %s"),
					SG_Get_String(Minimum, -2).c_str(),
					SG_Get_String(Maximum, -2).c_str()
				);

				CSG_Table_Record	*pClass	= pLUT->Add_Record();

				pClass->Set_Value(0, pColors->Get_Color(iClass));
				pClass->Set_Value(1, sValue);	// Name
				pClass->Set_Value(2, sValue);	// Description
				pClass->Set_Value(3, Minimum);	// Minimum
				pClass->Set_Value(4, Maximum);	// Maximum
			}

			Get_PointCloud()->Set_Index(old_Field, old_Order);
		}
		break;
	}

	//-----------------------------------------------------
	DataObject_Changed();

	m_Parameters("COLORS_TYPE")->Set_Value(CLASSIFY_LUT);	// Lookup Table
	m_Parameters("LUT_ATTRIB" )->Set_Value(iField);

	Parameters_Changed();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_PointCloud::Get_Value(CSG_Point ptWorld, double Epsilon)
{
	CSG_Shape		*pShape;

	if( (pShape = Get_PointCloud()->Get_Shape(ptWorld, Epsilon)) != NULL )
	{
		if( m_Color_Field >= 0 )
		{
			switch( m_pClassify->Get_Mode() )
			{
			case CLASSIFY_LUT:
				return( m_pClassify->Get_Class_Name_byValue(pShape->asDouble(m_Color_Field)) );

			case CLASSIFY_METRIC:	default:
				return( pShape->asString(m_Color_Field) );

			case CLASSIFY_RGB:
				double	Value = pShape->asDouble(m_Color_Field);
				return( wxString::Format(wxT("R%03d G%03d B%03d"), SG_GET_R((int)Value), SG_GET_G((int)Value), SG_GET_B((int)Value)) );
			}
		}
		else
		{
			return( wxString::Format(wxT("%s: %d"), _TL("Index"), pShape->Get_Index() + 1) );
		}
	}

	return( _TL("") );
}

//---------------------------------------------------------
double CWKSP_PointCloud::Get_Value_Range(void)
{
	return( m_Color_Field >= 0 ? Get_PointCloud()->Get_Range(m_Color_Field) : 0.0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_PointCloud::asImage(CSG_Grid *pImage)
{
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxMenu * CWKSP_PointCloud::Edit_Get_Menu(void)
{
	wxMenu	*pMenu	= new wxMenu;

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_SHAPES_EDIT_DEL_SHAPE);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_SHAPES_EDIT_SEL_CLEAR);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_SHAPES_EDIT_SEL_INVERT);

	return( pMenu );
}

//---------------------------------------------------------
TSG_Rect CWKSP_PointCloud::Edit_Get_Extent(void)
{
	if( Get_PointCloud()->Get_Selection_Count() > 0 )
	{
		return( Get_PointCloud()->Get_Selection_Extent().m_rect );
	}

	return( Get_PointCloud()->Get_Extent() );
}

//---------------------------------------------------------
bool CWKSP_PointCloud::Edit_On_Mouse_Up(CSG_Point Point, double ClientToWorld, int Key)
{
	if( Key & MODULE_INTERACTIVE_KEY_RIGHT )
	{
		return( false );
	}
	else
	{
		CSG_Rect	rWorld(m_Edit_Mouse_Down, Point);

		if( rWorld.Get_XRange() == 0.0 && rWorld.Get_YRange() == 0.0 )
		{
			rWorld.Inflate(2.0 * ClientToWorld, false);
		}

		g_pACTIVE->Get_Attributes()->Set_Attributes();

		Get_PointCloud()->Select(rWorld, (Key & MODULE_INTERACTIVE_KEY_CTRL) != 0);

		//-----------------------------------------------------
		m_Edit_Attributes.Del_Records();

		CSG_Table_Record	*pRecord	= Get_PointCloud()->Get_Selection();

		if( pRecord != NULL )
		{
			for(int i=0; i<Get_PointCloud()->Get_Field_Count(); i++)
			{
				CSG_Table_Record	*pAttribute	= m_Edit_Attributes.Add_Record();
				pAttribute->Set_Value(0, pRecord->Get_Table()->Get_Field_Name(i));
				pAttribute->Set_Value(1, pRecord->asString(i));
			}
		}

		//-----------------------------------------------------
		g_pACTIVE->Get_Attributes()->Set_Attributes();

		Update_Views(false);
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_PointCloud::Edit_Set_Attributes(void)
{
	CSG_Table_Record	*pSelection	= Get_PointCloud()->Get_Selection(m_Edit_Index);

	if( pSelection )
	{
		for(int i=0; i<m_Edit_Attributes.Get_Record_Count(); i++)
		{
			pSelection->Set_Value(i, m_Edit_Attributes.Get_Record(i)->asString(1));
		}

		Update_Views(false);

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
void CWKSP_PointCloud::On_Draw(CWKSP_Map_DC &dc_Map, int Flags)
{
	if( Get_Extent().Intersects(dc_Map.m_rWorld) != INTERSECTION_None && dc_Map.IMG_Draw_Begin(m_Parameters("DISPLAY_TRANSPARENCY")->asDouble() / 100.0) )
	{
		_Draw_Points	(dc_Map);

		dc_Map.IMG_Draw_End();
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline void CWKSP_PointCloud::_Draw_Point(CWKSP_Map_DC &dc_Map, int x, int y, double z, int Color)
{
	if( m_Aggregation == 1 )	// last value
	{
		dc_Map.IMG_Set_Pixel(x, y, Color);
	}
	else if( m_Z.is_InGrid(x, y) )
	{
		switch( m_Aggregation )
		{
		case 0:	// first value
			if( m_N.asInt(x, y) == 0 )
			{
				dc_Map.IMG_Set_Pixel(x, y, Color);
			}
			break;

		case 2:	// lowest z
			if( m_N.asInt(x, y) == 0 || z < m_Z.asDouble(x, y) )
			{
				dc_Map.IMG_Set_Pixel(x, y, Color);
				m_Z.Set_Value(x, y, z);
			}
			break;

		case 3:	// highest z
			if( m_N.asInt(x, y) == 0 || z > m_Z.asDouble(x, y) )
			{
				dc_Map.IMG_Set_Pixel(x, y, Color);
				m_Z.Set_Value(x, y, z);
			}
			break;
		}

		m_N.Add_Value(x, y, 1);
	}
}

//---------------------------------------------------------
void CWKSP_PointCloud::_Draw_Point(CWKSP_Map_DC &dc_Map, int x, int y, double z, int Color, int Radius)
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
void CWKSP_PointCloud::_Draw_Points(CWKSP_Map_DC &dc_Map)
{
	m_Aggregation	= m_Parameters("DISPLAY_VALUE_AGGREGATE")->asInt();

	if( m_Aggregation != 1 )
	{
		m_Z.Create(SG_DATATYPE_Double, dc_Map.m_rDC.GetWidth(), dc_Map.m_rDC.GetHeight());
		m_N.Create(SG_DATATYPE_Int   , dc_Map.m_rDC.GetWidth(), dc_Map.m_rDC.GetHeight());
	}

	//-----------------------------------------------------
	CSG_PointCloud	*pPoints	= Get_PointCloud();

	bool	bSelection	= pPoints->Get_Selection_Count() > 0;

	for(int i=0; i<Get_PointCloud()->Get_Count(); i++)
	{
		pPoints->Set_Cursor(i);

		if( !pPoints->is_NoData(m_Color_Field) )
		{
			TSG_Point_Z	Point	= pPoints->Get_Point();

			if( dc_Map.m_rWorld.Contains(Point.x, Point.y) )
			{
				int		x	= (int)dc_Map.xWorld2DC(Point.x);
				int		y	= (int)dc_Map.yWorld2DC(Point.y);

				if( bSelection && pPoints->is_Selected(i) )
				{
					_Draw_Point(dc_Map, x, y, Point.z, SG_COLOR_RED   , m_PointSize + 2);
					_Draw_Point(dc_Map, x, y, Point.z, SG_COLOR_YELLOW, m_PointSize);
				}
				else
				{
					int		Color;

					m_pClassify->Get_Class_Color_byValue(pPoints->Get_Value(m_Color_Field), Color);

					_Draw_Point(dc_Map, x, y, Point.z, Color, m_PointSize);
				}
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

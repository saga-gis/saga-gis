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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_PointCloud::CWKSP_PointCloud(CSG_PointCloud *pPointCloud)
	: CWKSP_Layer(pPointCloud)
{
	m_pPointCloud	= pPointCloud;

	m_Edit_Attributes.Destroy();
	m_Edit_Attributes.Add_Field(LNG("[CAP] Name") , SG_DATATYPE_String);
	m_Edit_Attributes.Add_Field(LNG("[CAP] Value"), SG_DATATYPE_String);

	Create_Parameters();

	//-----------------------------------------------------
	m_Parameters("COLORS_TYPE")		->Set_Value(CLASSIFY_METRIC);
	m_Parameters("COLORS_ATTRIB")	->Set_Value(2);

	On_Parameter_Changed(&m_Parameters, m_Parameters("COLORS_ATTRIB"));

	Parameters_Changed();
}

//---------------------------------------------------------
CWKSP_PointCloud::~CWKSP_PointCloud(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define DESC_ADD_STR(label, value)	s.Append(wxString::Format(wxT("<tr><td valign=\"top\">%s</td><td valign=\"top\">%s</td></tr>"), label, value))
#define DESC_ADD_INT(label, value)	s.Append(wxString::Format(wxT("<tr><td valign=\"top\">%s</td><td valign=\"top\">%d</td></tr>"), label, value))

//---------------------------------------------------------
wxString CWKSP_PointCloud::Get_Description(void)
{
	wxString	s;

	//-----------------------------------------------------
	s	+= wxString::Format(wxT("<b>%s</b>"), LNG("[CAP] Point Cloud"));

	s	+= wxT("<table border=\"0\">");

	DESC_ADD_STR(LNG("[CAP] Name")				, m_pPointCloud->Get_Name());
	DESC_ADD_STR(LNG("[CAP] File")				, m_pPointCloud->Get_File_Name());
	DESC_ADD_STR(LNG("[CAP] Projection")		, m_pPointCloud->Get_Projection().Get_Description().c_str());
	DESC_ADD_INT(LNG("[CAP] Number of Points")	, m_pPointCloud->Get_Count());

	s	+= wxT("</table>");

	s	+= Get_TableInfo_asHTML(m_pPointCloud);

	//-----------------------------------------------------
//	s	+= wxString::Format(wxT("<hr><b>%s</b><font size=\"-1\">"), LNG("[CAP] Data History"));
//	s	+= m_pPointCloud->Get_History().Get_HTML();
//	s	+= wxString::Format(wxT("</font"));

	//-----------------------------------------------------
	return( s );
}

//---------------------------------------------------------
wxMenu * CWKSP_PointCloud::Get_Menu(void)
{
	wxMenu	*pMenu, *pSubMenu;

	pMenu	= new wxMenu(m_pPointCloud->Get_Name());

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_CLOSE);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_POINTCLOUD_SHOW);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_POINTCLOUD_SAVE);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_POINTCLOUD_SAVEAS);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_SETTINGS_COPY);

	pMenu->AppendSeparator();

	pSubMenu	= new wxMenu(LNG("[MNU] Classificaton"));
	CMD_Menu_Add_Item(pSubMenu	, false, ID_CMD_POINTCLOUD_RANGE_MINMAX);
	CMD_Menu_Add_Item(pSubMenu	, false, ID_CMD_POINTCLOUD_RANGE_STDDEV150);
	CMD_Menu_Add_Item(pSubMenu	, false, ID_CMD_POINTCLOUD_RANGE_STDDEV200);

	pMenu->Append(ID_CMD_WKSP_FIRST, LNG("[MNU] Classification"), pSubMenu);


//	wxMenu	*pTable	= new wxMenu(LNG("[MNU] Table"));
//	CMD_Menu_Add_Item(pTable,  true, ID_CMD_TABLES_SHOW);
//	CMD_Menu_Add_Item(pTable,  true, ID_CMD_TABLES_DIAGRAM);
//	CMD_Menu_Add_Item(pTable, false, ID_CMD_TABLES_SCATTERPLOT);
//	pMenu->Append(ID_CMD_WKSP_FIRST, LNG("[MNU] Attributes"), pTable);

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

	case ID_CMD_POINTCLOUD_RANGE_MINMAX:
		Set_Color_Range(
			m_pPointCloud->Get_Minimum(m_Color_Field),
			m_pPointCloud->Get_Maximum(m_Color_Field)
		);
		break;

	case ID_CMD_POINTCLOUD_RANGE_STDDEV150:
		Set_Color_Range(
			m_pPointCloud->Get_Mean(m_Color_Field) - 1.5 * m_pPointCloud->Get_StdDev(m_Color_Field),
			m_pPointCloud->Get_Mean(m_Color_Field) + 1.5 * m_pPointCloud->Get_StdDev(m_Color_Field)
		);
		break;

	case ID_CMD_POINTCLOUD_RANGE_STDDEV200:
		Set_Color_Range(
			m_pPointCloud->Get_Mean(m_Color_Field) - 2.0 * m_pPointCloud->Get_StdDev(m_Color_Field),
			m_pPointCloud->Get_Mean(m_Color_Field) + 2.0 * m_pPointCloud->Get_StdDev(m_Color_Field)
		);
		break;

	case ID_CMD_SHAPES_EDIT_SEL_INVERT:
		m_pPointCloud->Inv_Selection();
		Update_Views(false);
		break;

	case ID_CMD_SHAPES_EDIT_DEL_SHAPE:
		if( m_pPointCloud->Get_Selection_Count() > 0 && DLG_Message_Confirm(LNG("[DLG] Delete selected point(s)."), LNG("[CAP] Edit Point Cloud")) )
		{
			m_pPointCloud->Del_Selection();
			Update_Views(false);
		}
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
	//-----------------------------------------------------
	// General...

	m_Parameters.Add_Choice(
		m_Parameters("NODE_COLORS")		, "COLORS_ATTRIB"			, LNG("[CAP] Attribute"),
		LNG(""),
		LNG("")
	);

	m_Parameters.Add_Choice(
		m_Parameters("NODE_COLORS")		, "COLORS_AGGREGATE"		, LNG("[CAP] Value Aggregation"),
		LNG(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			LNG("first value"),
			LNG("last value"),
			LNG("lowest z"),
			LNG("highest z")
		), 1
	);

	//-----------------------------------------------------
	m_Parameters.Add_Value(
		m_Parameters("NODE_DISPLAY")	, "DISPLAY_SIZE"			, LNG("[CAP] Point Size"),
		LNG(""),
		PARAMETER_TYPE_Int, 0, 0, true
	);

	//-----------------------------------------------------
	m_Parameters.Add_Value(
		m_Parameters("NODE_DISPLAY")	, "DISPLAY_TRANSPARENCY"	, LNG("[CAP] Transparency [%]"),
		LNG(""),
		PARAMETER_TYPE_Double, 0.0, 0.0, true, 100.0, true
	);

	//-----------------------------------------------------
	// Classification...

	((CSG_Parameter_Choice *)m_Parameters("COLORS_TYPE")->Get_Data())->Set_Items(
		wxString::Format(wxT("%s|%s|%s|%s|"),
			LNG("[VAL] Unique Symbol"),
			LNG("[VAL] Lookup Table"),
			LNG("[VAL] Graduated Color"),
			LNG("[VAL] RGB")
		)
	);

	m_Parameters("COLORS_TYPE")->Set_Value(CLASSIFY_METRIC);


	//-----------------------------------------------------
	DataObject_Changed((CSG_Parameters *)NULL);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_PointCloud::On_DataObject_Changed(void)
{
	int			i;
	wxString	sChoices;

	for(i=0; i<m_pPointCloud->Get_Field_Count(); i++)
	{
		sChoices.Append(wxString::Format(wxT("%s|"), m_pPointCloud->Get_Field_Name(i)));
	}

	m_Parameters("COLORS_ATTRIB")->asChoice()->Set_Items(sChoices);
}

//---------------------------------------------------------
void CWKSP_PointCloud::On_Parameters_Changed(void)
{
	if( (m_Color_Field = m_Parameters("COLORS_ATTRIB")->asInt()) >= m_pPointCloud->Get_Field_Count() )
	{
		m_Color_Field	= -1;
	}

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
int CWKSP_PointCloud::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), wxT("COLORS_ATTRIB")) )
	{
		CSG_Parameters	Parameters;
	
		int		zField	= pParameter->asInt();

		double	m	= m_pPointCloud->Get_Mean  (zField);
		double	s	= m_pPointCloud->Get_StdDev(zField) * 2.0;
		double	min	= m - s;	if( min < m_pPointCloud->Get_Minimum(zField) )	min	= m_pPointCloud->Get_Minimum(zField);
		double	max	= m + s;	if( max > m_pPointCloud->Get_Maximum(zField) )	max	= m_pPointCloud->Get_Maximum(zField);

		pParameters->Get_Parameter("METRIC_ZRANGE")->asRange()->Set_Range(min, max);
	}

	return( 1 );
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

	if( (pShape = m_pPointCloud->Get_Shape(ptWorld, Epsilon)) != NULL )
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
			return( wxString::Format(wxT("%s: %d"), LNG("[CAP] Index"), pShape->Get_Index() + 1) );
		}
	}

	return( LNG("") );
}

//---------------------------------------------------------
double CWKSP_PointCloud::Get_Value_Range(void)
{
	return( m_Color_Field >= 0 ? m_pPointCloud->Get_Range(m_Color_Field) : 0.0 );
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
wxMenu * CWKSP_PointCloud::On_Edit_Get_Menu(void)
{
	wxMenu	*pMenu;

	pMenu	= new wxMenu;

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_SHAPES_EDIT_DEL_SHAPE);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_SHAPES_EDIT_SEL_INVERT);

	return( pMenu );
}

//---------------------------------------------------------
bool CWKSP_PointCloud::On_Edit_On_Mouse_Up(CSG_Point Point, double ClientToWorld, int Key)
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

		m_pPointCloud->Select(rWorld, (Key & MODULE_INTERACTIVE_KEY_CTRL) != 0);

		//-----------------------------------------------------
		m_Edit_Attributes.Del_Records();

		CSG_Table_Record	*pRecord	= m_pPointCloud->Get_Selection();

		if( pRecord != NULL )
		{
			for(int i=0; i<m_pPointCloud->Get_Field_Count(); i++)
			{
				CSG_Table_Record	*pAttribute	= m_Edit_Attributes.Add_Record();
				pAttribute->Set_Value(0, pRecord->Get_Table()->Get_Field_Name(i));
				pAttribute->Set_Value(1, pRecord->asString(i));
			}
		}

		//-----------------------------------------------------
		g_pACTIVE->Get_Attributes()->Set_Attributes();

		Update_Views(true);
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_PointCloud::On_Edit_Set_Attributes(void)
{
	CSG_Table_Record	*pRecord;

	if( (pRecord = m_pPointCloud->Get_Selection()) != NULL )
	{
		for(int i=0; i<m_Edit_Attributes.Get_Record_Count(); i++)
		{
			pRecord->Set_Value(i, m_Edit_Attributes.Get_Record(i)->asString(1));
		}

		Update_Views(true);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
TSG_Rect CWKSP_PointCloud::On_Edit_Get_Extent(void)
{
	return( m_pPointCloud->Get_Extent() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_PointCloud::On_Draw(CWKSP_Map_DC &dc_Map, bool bEdit)
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
	m_Aggregation	= m_Parameters("COLORS_AGGREGATE")->asInt();

	if( m_Aggregation != 1 )
	{
		m_Z.Create(SG_DATATYPE_Double, dc_Map.m_rDC.GetWidth(), dc_Map.m_rDC.GetHeight());
		m_N.Create(SG_DATATYPE_Int   , dc_Map.m_rDC.GetWidth(), dc_Map.m_rDC.GetHeight());
	}

	//-----------------------------------------------------
	for(int i=0; i<m_pPointCloud->Get_Count(); i++)
	{
		TSG_Point_Z	Point	= m_pPointCloud->Get_Point(i);

		if( dc_Map.m_rWorld.Contains(Point.x, Point.y) )
		{
			int		x	= (int)dc_Map.xWorld2DC(Point.x);
			int		y	= (int)dc_Map.yWorld2DC(Point.y);

			if( m_pPointCloud->is_Selected(i) )
			{
				_Draw_Point(dc_Map, x, y, Point.z, SG_COLOR_RED   , m_PointSize + 2);
				_Draw_Point(dc_Map, x, y, Point.z, SG_COLOR_YELLOW, m_PointSize);
			}
			else
			{
				int		Color;

				m_pClassify->Get_Class_Color_byValue(m_pPointCloud->Get_Value(i, m_Color_Field), Color);

				_Draw_Point(dc_Map, x, y, Point.z, Color, m_PointSize);
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


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

#include "helper.h"

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
	m_Edit_Attributes.Add_Field(LNG("[CAP] Name") , TABLE_FIELDTYPE_String);
	m_Edit_Attributes.Add_Field(LNG("[CAP] Value"), TABLE_FIELDTYPE_String);

	Create_Parameters();
}

//---------------------------------------------------------
CWKSP_PointCloud::~CWKSP_PointCloud(void)
{
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_PointCloud::Get_Name(void)
{
	return( wxString::Format(wxT("%02d. %s"), 1 + Get_ID(), m_pPointCloud->Get_Name()) );
}

//---------------------------------------------------------
wxString CWKSP_PointCloud::Get_Description(void)
{
	wxString	s;

	//-----------------------------------------------------
	s.Append(wxString::Format(wxT("<b>%s</b><table border=\"0\">"),
		LNG("[CAP] Point Cloud")
	));

	s.Append(wxString::Format(wxT("<tr><td>%s</td><td>%s</td></tr>"),
		LNG("[CAP] Name")					, m_pPointCloud->Get_Name()
	));

	s.Append(wxString::Format(wxT("<tr><td>%s</td><td>%s</td></tr>"),
		LNG("[CAP] File")					, m_pPointCloud->Get_File_Name()
	));

	s.Append(wxString::Format(wxT("<tr><td>%s</td><td>%d</td></tr>"),
		LNG("[CAP] Number of Points")		, m_pPointCloud->Get_Count()
	));

	s.Append(wxT("</table>"));

	//-----------------------------------------------------
//	s.Append(wxString::Format(wxT("<hr><b>%s</b><font size=\"-1\">"), LNG("[CAP] Data History")));
//	s.Append(m_pPointCloud->Get_History().Get_HTML());
//	s.Append(wxString::Format(wxT("</font")));

	//-----------------------------------------------------
	return( s );
}

//---------------------------------------------------------
wxMenu * CWKSP_PointCloud::Get_Menu(void)
{
	wxMenu	*pMenu;

	pMenu	= new wxMenu(m_pPointCloud->Get_Name());

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_CLOSE);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_POINTCLOUD_SHOW);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_POINTCLOUD_SAVE);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_POINTCLOUD_SAVEAS);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_SETTINGS_COPY);

//	pMenu->AppendSeparator();

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
bool CWKSP_PointCloud::On_Edit_On_Mouse_Up(CSG_Point Point, double ClientToWorld, int Key)
{
	CSG_Rect	rWorld(m_Edit_Mouse_Down, Point);

	if( rWorld.Get_XRange() == 0.0 && rWorld.Get_YRange() == 0.0 )
	{
		rWorld.Inflate(2.0 * ClientToWorld, false);
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_PointCloud::On_Edit_Set_Attributes(void)
{
	return( true );
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
	if( Get_Extent().Intersects(dc_Map.m_rWorld) != INTERSECTION_None )
	{
		_Draw_Points	(dc_Map);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_PointCloud::_Draw_Points(CWKSP_Map_DC &dc_Map)
{
	if(	dc_Map.IMG_Draw_Begin(m_Parameters("DISPLAY_TRANSPARENCY")->asDouble() / 100.0) )
	{
		for(int i=0; i<m_pPointCloud->Get_Count(); i++)
		{
			int				Color;
			TSG_Point_3D	p		= m_pPointCloud->Get_Point(i);
			double			z		= m_pPointCloud->Get_Value(i, m_Color_Field);
			TSG_Point_Int	Point	= dc_Map.World2DC(CSG_Point(p.x, p.y));

			m_pClassify->Get_Class_Color_byValue(z, Color);

			if( m_PointSize > 0 )
			{
				dc_Map.IMG_Set_Rect(Point.x - m_PointSize, Point.y - m_PointSize, Point.x + m_PointSize, Point.y + m_PointSize, Color);
			}
			else
			{
				dc_Map.IMG_Set_Pixel(Point.x, Point.y, Color);
			}
		}

		dc_Map.IMG_Draw_End();
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

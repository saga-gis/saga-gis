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
//                 VIEW_ScatterPlot.cpp                  //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/window.h>

#include <wx/clipbrd.h>

#include <saga_api/saga_api.h>

#include "res_commands.h"
#include "res_controls.h"
#include "res_images.h"
#include "res_dialogs.h"

#include "helper.h"
#include "dc_helper.h"

#include "wksp.h"
#include "wksp_data_manager.h"

#include "wksp_grid.h"
#include "wksp_shapes.h"

#include "view_scatterplot.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define CHECK_DATA(pData)	if( pData != NULL && !SG_Get_Data_Manager().Exists(pData) )	{	pData	= NULL;	}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int Scatter_Plot_On_Parameter_Changed(CSG_Parameter *pParameter, int Flags)
{
	if( !pParameter || !pParameter->Get_Owner() )
	{
		return( -1 );
	}

	CSG_Parameters	*pParameters	= pParameter->Get_Owner();

	if( Flags & PARAMETER_CHECK_ENABLE )
	{
		if( pParameter->Cmp_Identifier("CMP_WITH") )
		{
			pParameters->Get_Parameter("GRID")->Get_Parent()->Set_Enabled(pParameter->asInt() == 0);
			pParameters->Set_Enabled("GRID"       , pParameter->asInt() == 0);
			pParameters->Set_Enabled("POINTS"     , pParameter->asInt() == 1);
		}

		if( pParameter->Cmp_Identifier("REG_SHOW") )
		{
			pParameters->Set_Enabled("REG_FORMULA", pParameter->asBool());
			pParameters->Set_Enabled("REG_COLOR"  , pParameter->asBool());
			pParameters->Set_Enabled("REG_SIZE"   , pParameter->asBool());
			pParameters->Set_Enabled("REG_INFO"   , pParameter->asBool());
		}

		if( pParameter->Cmp_Identifier("DISPLAY") )
		{
			pParameters->Set_Enabled("DENSITY_RES", pParameter->asInt() == 1);
			pParameters->Set_Enabled("DENSITY_PAL", pParameter->asInt() == 1);
			pParameters->Set_Enabled("DENSITY_LEG", pParameter->asInt() == 1);
		}
	}

	if( Flags & PARAMETER_CHECK_VALUES )
	{
		if( pParameter->Cmp_Identifier("CMP_WITH") )
		{
		//	pParameters->Get_Parameter("OPTIONS")->asParameters()->
		//	             Get_Parameter("DISPLAY")->Set_Value(pParameter->asInt() == 0 ? 1 : 0);
			pParameters->Get_Parameter("DISPLAY")->Set_Value(pParameter->asInt() == 0 ? 1 : 0);

			pParameters->Set_Enabled("DENSITY_RES", pParameter->asInt() == 0);
			pParameters->Set_Enabled("DENSITY_PAL", pParameter->asInt() == 0);
			pParameters->Set_Enabled("DENSITY_LEG", pParameter->asInt() == 0);
		}
	}

	return( 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
IMPLEMENT_CLASS(CVIEW_ScatterPlot, CVIEW_Base);

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CVIEW_ScatterPlot, CVIEW_Base)
	EVT_PAINT			(CVIEW_ScatterPlot::On_Paint)
	EVT_SIZE			(CVIEW_ScatterPlot::On_Size)

	EVT_MENU			(ID_CMD_SCATTERPLOT_PARAMETERS	, CVIEW_ScatterPlot::On_Parameters)
	EVT_MENU			(ID_CMD_SCATTERPLOT_UPDATE		, CVIEW_ScatterPlot::On_Update)
	EVT_MENU			(ID_CMD_SCATTERPLOT_AS_TABLE	, CVIEW_ScatterPlot::On_AsTable)
	EVT_MENU			(ID_CMD_SCATTERPLOT_TO_CLIPBOARD, CVIEW_ScatterPlot::On_ToClipboard)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CVIEW_ScatterPlot::CVIEW_ScatterPlot(CWKSP_Data_Item *pItem)
	: CVIEW_Base(pItem, ID_VIEW_SCATTERPLOT, wxString::Format("%s: %s", _TL("Scatterplot"), pItem->Get_Object()->Get_Name()), ID_IMG_WND_SCATTERPLOT, false)
{
	m_pItem		= pItem;

	m_pGrid		= NULL;
	m_pGrids	= NULL;
	m_pTable	= NULL;

	switch( m_pItem->Get_Type() )
	{
	case WKSP_ITEM_Grid      : m_pGrid  = pItem->Get_Object()->asGrid      (); break;
	case WKSP_ITEM_Grids     : m_pGrids = pItem->Get_Object()->asGrids     (); break;
	case WKSP_ITEM_Shapes    : m_pTable = pItem->Get_Object()->asShapes    (); break;
	case WKSP_ITEM_PointCloud: m_pTable = pItem->Get_Object()->asPointCloud(); break;
	default                  : m_pTable = pItem->Get_Object()->asTable     (); break;
	}

	m_Parameters.Set_Name(CSG_String::Format("%s: %s", _TL("Scatterplot"), m_pItem->Get_Object()->Get_Name()));

	_On_Construction();

	if( DLG_Parameters(&m_Parameters) )
	{
		_Update_Data();

		Do_Show();
	}
	else
	{
		Destroy();
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_ScatterPlot::Do_Update(void)
{
	_Update_Data();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxMenu * CVIEW_ScatterPlot::_Create_Menu(void)
{
	wxMenu	*pMenu	= new wxMenu;

	CMD_Menu_Add_Item(pMenu, false, ID_CMD_SCATTERPLOT_PARAMETERS);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_SCATTERPLOT_UPDATE);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_SCATTERPLOT_AS_TABLE);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_SCATTERPLOT_TO_CLIPBOARD);

	return( pMenu );
}

//---------------------------------------------------------
wxToolBarBase * CVIEW_ScatterPlot::_Create_ToolBar(void)
{
	wxToolBarBase	*pToolBar	= CMD_ToolBar_Create(ID_TB_VIEW_SCATTERPLOT);

	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_SCATTERPLOT_PARAMETERS);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_SCATTERPLOT_UPDATE);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_SCATTERPLOT_AS_TABLE);
	CMD_ToolBar_Add_Item(pToolBar, false, ID_CMD_SCATTERPLOT_TO_CLIPBOARD);

	CMD_ToolBar_Add(pToolBar, _TL("Scatterplot"));

	return( pToolBar );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
//#define m_Options	(*m_Parameters("OPTIONS")->asParameters())
#define m_Options	m_Parameters

//---------------------------------------------------------
void CVIEW_ScatterPlot::_On_Construction(void)
{
	SYS_Set_Color_BG_Window(this);

	//-----------------------------------------------------
	if( m_pGrid )
	{
		m_Parameters.Add_Grid("",
			"GRID"		, _TL("Grid"),
			_TL(""),
			PARAMETER_INPUT, false
		);

		m_Parameters.Add_Shapes("",
			"POINTS"	, _TL("Points"),
			_TL(""),
			PARAMETER_INPUT, SHAPE_TYPE_Point
		);

		m_Parameters.Add_Table_Field("POINTS",
			"FIELD"		, _TL("Attribute"),
			_TL("")
		);

		m_Parameters.Add_Choice("",
			"CMP_WITH"	, _TL("Compare with..."),
			_TL(""),
			CSG_String::Format("%s|%s|",
				_TL("another grid"),
				_TL("points")
			), 0
		);

		m_Parameters.Add_Choice("POINTS",
			"RESAMPLING", _TL("Resampling"),
			_TL(""),
			CSG_String::Format("%s|%s|%s|%s",
				_TL("Nearest Neighbour"),
				_TL("Bilinear Interpolation"),
				_TL("Bicubic Spline Interpolation"),
				_TL("B-Spline Interpolation")
			), 3
		);
	}
	else if( m_pGrids )
	{
		CSG_String	sChoices;

		for(int i=0; i<m_pGrids->Get_Grid_Count(); i++)
		{
			sChoices.Append(m_pGrids->Get_Grid_Name(i, SG_GRIDS_NAME_GRID) + "|");
		}

		m_Parameters.Add_Choice("", "BAND_X", "X", _TL(""), sChoices, 0);
		m_Parameters.Add_Choice("", "BAND_Y", "Y", _TL(""), sChoices, 1);
	}
	else if( m_pTable )
	{
		CSG_String	sChoices;

		for(int i=0; i<m_pTable->Get_Field_Count(); i++)
		{
			sChoices.Append(CSG_String::Format("%s|", m_pTable->Get_Field_Name(i)));
		}

		m_Parameters.Add_Choice("", "FIELD_X", "X", _TL(""), sChoices, 0);
		m_Parameters.Add_Choice("", "FIELD_Y", "Y", _TL(""), sChoices, 1);
	}

	//-----------------------------------------------------
//	m_Parameters.Add_Parameters("", "OPTIONS", _TL("Options"), _TL(""));

	m_Options.Add_Int("",
		"SAMPLES_MAX", _TL("Maximimum Number of Samples"),
		_TL(""),
		100000, 0, true
	);

	m_Options.Add_Bool("",
		"REG_SHOW"	, _TL("Show Regression"),
		_TL(""),
		false
	);

	m_Options.Add_String("REG_SHOW",
		"REG_FORMULA", _TL("Regression Formula"),
		_TL(""),
		"a + b * x"
	);

	m_Options.Add_Color("REG_SHOW",
		"REG_COLOR"	, _TL("Line Colour"),
		_TL("")
	);

	m_Options.Add_Int("REG_SHOW",
		"REG_SIZE"	, _TL("Line Size"),
		_TL(""),
		0, 0, true
	);

	m_Options.Add_Info_String("REG_SHOW",
		"REG_INFO"	, _TL("Regression Details"),
		_TL(""),
		_TL(""), true
	);

	m_Options.Add_Choice("",
		"DISPLAY"	, _TL("Display Type"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("Points"),
			_TL("Density")
		), m_pGrid || m_pGrids ? 1 : 0
	);

	m_Options.Add_Int("DISPLAY",
		"DENSITY_RES"	, _TL("Display Resolution"),
		_TL(""),
		50, 10, true
	);

	CSG_Colors	Colors(7, SG_COLORS_RAINBOW);

	Colors.Set_Color(0, 255, 255, 255);
	Colors.Set_Count(100);

	m_Options.Add_Colors("DISPLAY",
		"DENSITY_PAL"	, _TL("Colors"),
		_TL(""),
		&Colors
	);

	m_Options.Add_Bool("DISPLAY",
		"DENSITY_LEG"	, _TL("Show Legend"),
		_TL(""),
		true
	);

	//-----------------------------------------------------
	m_Parameters.Set_Callback_On_Parameter_Changed(&Scatter_Plot_On_Parameter_Changed);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_ScatterPlot::On_Parameters(wxCommandEvent &event)
{
	if( DLG_Parameters(&m_Parameters) )
	{
		_Update_Data();
	}
}

//---------------------------------------------------------
void CVIEW_ScatterPlot::On_Update(wxCommandEvent &event)
{
	_Update_Data();
}

//---------------------------------------------------------
void CVIEW_ScatterPlot::On_AsTable(wxCommandEvent &event)
{
	if( m_Trend.Get_Data_Count() > 1 )
	{
		CSG_Table	*pTable	= new CSG_Table;

		pTable->Set_Name("%s: [%s]-[%s]", _TL("Scatterplot"), m_sX.c_str(), m_sY.c_str());

		pTable->Add_Field("ID", SG_DATATYPE_Int   );
		pTable->Add_Field(m_sX, SG_DATATYPE_Double);
		pTable->Add_Field(m_sY, SG_DATATYPE_Double);

		for(int i=0; i<m_Trend.Get_Data_Count() && PROGRESSBAR_Set_Position(i, m_Trend.Get_Data_Count()); i++)
		{
			CSG_Table_Record	*pRecord	= pTable->Add_Record();

			pRecord->Set_Value(0, i + 1);
			pRecord->Set_Value(1, m_Trend.Get_Data_X(i));
			pRecord->Set_Value(2, m_Trend.Get_Data_Y(i));
		}

		PROGRESSBAR_Set_Position(0);

		g_pData->Add(pTable);
	}
}

//---------------------------------------------------------
void CVIEW_ScatterPlot::On_ToClipboard(wxCommandEvent &event)
{
	wxBitmap	BMP(GetSize());
	wxMemoryDC	dc;
	
	dc.SelectObject(BMP);
	dc.SetBackground(*wxWHITE_BRUSH);
	dc.Clear();

	_Draw(dc, wxRect(BMP.GetSize()));

	dc.SelectObject(wxNullBitmap);

	if( wxTheClipboard->Open() )
	{
		wxBitmapDataObject	*pBMP	= new wxBitmapDataObject;
		pBMP->SetBitmap(BMP);
		wxTheClipboard->SetData(pBMP);
		wxTheClipboard->Close();
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_ScatterPlot::On_Size(wxSizeEvent &event)
{
	Refresh();

	event.Skip();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GET_DC_X(x)	(r.GetLeft  () + (int)(0.5 + dx * ((x) - m_Trend.Get_Data_XMin())))
#define GET_DC_Y(y)	(r.GetBottom() - (int)(0.5 + dy * ((y) - m_Trend.Get_Data_YMin())))

//---------------------------------------------------------
void CVIEW_ScatterPlot::On_Paint(wxPaintEvent &event)
{
	wxPaintDC	dc(this);
	wxRect		r(wxPoint(0, 0), GetClientSize());

	Draw_Edge(dc, EDGE_STYLE_SUNKEN, r);

	_Draw(dc, r);
}

//---------------------------------------------------------
void CVIEW_ScatterPlot::_Draw(wxDC &dc, wxRect r)
{
	r	= _Draw_Get_rDiagram(r);

	if( m_Trend.Get_Data_Count() > 1 )
	{
		//-------------------------------------------------
		if( m_Options("DISPLAY")->asInt() == 1 )
		{
			if( m_Options("DENSITY_LEG")->asBool() )
			{
				r.SetRight(r.GetRight() - 40);

				_Draw_Legend(dc, wxRect(r.GetRight() + 10, r.GetTop(), 40 - 20, r.GetHeight()));
			}

			_Draw_Image (dc, r);
		}
		else
		{
			_Draw_Points(dc, r);
		}

		//-------------------------------------------------
		_Draw_Frame(dc, r);
	}
	else
	{
		Draw_Text(dc, TEXTALIGN_CENTER, r.GetLeft() + r.GetWidth() / 2, r.GetTop() + r.GetHeight() / 2, _TL("Invalid data!"));
	}
}

//---------------------------------------------------------
wxRect CVIEW_ScatterPlot::_Draw_Get_rDiagram(wxRect r)
{
	return(	wxRect(
		wxPoint(r.GetLeft () + 45, r.GetTop   () + 20),
		wxPoint(r.GetRight() - 20, r.GetBottom() - 40)
	));
}

//---------------------------------------------------------
void CVIEW_ScatterPlot::_Draw_Regression(wxDC &dc, wxRect r)
{
	wxPen	oldPen	= dc.GetPen();

	dc.SetPen(wxPen(
		m_Options("REG_COLOR")->asColor(),
		m_Options("REG_SIZE" )->asInt()
	));

	//-----------------------------------------------------
	double	dx	= (r.GetWidth () - 1.) / m_Trend.Get_Data_XStats().Get_Range();
	double	dy	= (r.GetHeight() - 1.) / m_Trend.Get_Data_YStats().Get_Range();

	//-----------------------------------------------------
	dc.DrawCircle(
		GET_DC_X(m_Trend.Get_Data_XStats().Get_Mean()),
		GET_DC_Y(m_Trend.Get_Data_YStats().Get_Mean()), 2
	);

	double	ex	= m_Trend.Get_Data_XStats().Get_Range() / (double)r.GetWidth();
	double	x	= m_Trend.Get_Data_XMin();

	for(int ix=0, ay, by=0; ix<r.GetWidth(); ix++, x+=ex)
	{
		double	y	= m_Trend.Get_Value(x);

		ay	= by; by = r.GetBottom() - (int)(dy * (y - m_Trend.Get_Data_YMin()));

		if( ix > 0 && r.GetTop() < ay && ay < r.GetBottom() && r.GetTop() < by && by < r.GetBottom() )
		{
			dc.DrawLine(r.GetLeft() + ix - 1, ay, r.GetLeft() + ix, by);
		}
	}

	dc.SetPen(oldPen);

	//-----------------------------------------------------
	Draw_Text(dc, TEXTALIGN_BOTTOMCENTER, r.GetLeft() + r.GetWidth() / 2, r.GetTop(),
		m_Trend.Get_Formula(SG_TREND_STRING_Compact).c_str()
	);
}

//---------------------------------------------------------
void CVIEW_ScatterPlot::_Draw_Legend(wxDC &dc, wxRect r)
{
	CSG_Colors	Colors(*m_Options("DENSITY_PAL")->asColors());

	Colors.Set_Count(r.GetHeight());

	for(int i=0, y=r.GetBottom(); i<Colors.Get_Count(); i++, y--)
	{
		Draw_FillRect(dc, Get_Color_asWX(Colors.Get_Color(i)), r.GetLeft(), y, r.GetRight(), y + 1);
	}

//	r.Offset(0, -r.GetHeight());

	Draw_Edge(dc, EDGE_STYLE_SIMPLE, r);

	Draw_Text(dc, TEXTALIGN_BOTTOMLEFT, 2 + r.GetRight(), r.GetBottom(), "0");
	Draw_Text(dc, TEXTALIGN_TOPLEFT   , 2 + r.GetRight(), r.GetTop   (), wxString::Format("%d", (int)m_Count.Get_Max()));
}

//---------------------------------------------------------
void CVIEW_ScatterPlot::_Draw_Image(wxDC &dc, wxRect r)
{
	CSG_Colors	*pColors	= m_Options("DENSITY_PAL")->asColors();

	wxImage	Image(r.GetWidth(), r.GetHeight());

	double	dCount	= (pColors->Get_Count() - 2.0) / log(1.0 + m_Count.Get_Max());

	double	dx		= (m_Count.Get_NX() - 1.0) / (double)r.GetWidth ();
	double	dy		= (m_Count.Get_NY() - 1.0) / (double)r.GetHeight();

	//-----------------------------------------------------
	#pragma omp parallel for
	for(int y=Image.GetHeight()-1; y>=0; y--)
	{
		double	Count;
		double	ix	= 0.0;
		double	iy	= m_Count.Get_NY() - 1 - y * dy;

		for(int x=0; x<Image.GetWidth(); x++, ix+=dx)
		{
			int	i	= m_Count.Get_Value(ix, iy, Count) && Count > 0.0 ? (int)(log(1.0 + Count) * dCount) : 0;

			Image.SetRGB(x, y, pColors->Get_Red(i), pColors->Get_Green(i), pColors->Get_Blue(i));
		}
	}

	dc.DrawBitmap(wxBitmap(Image), r.GetTopLeft());
}

//---------------------------------------------------------
void CVIEW_ScatterPlot::_Draw_Points(wxDC &dc, wxRect r)
{
	double	dx	= (r.GetWidth () - 1.) / m_Trend.Get_Data_XStats().Get_Range();
	double	dy	= (r.GetHeight() - 1.) / m_Trend.Get_Data_YStats().Get_Range();

	for(int i=0; i<m_Trend.Get_Data_Count(); i++)
	{
		dc.DrawCircle(
			GET_DC_X(m_Trend.Get_Data_X(i)),
			GET_DC_Y(m_Trend.Get_Data_Y(i)), 2
		);
	}
}

//---------------------------------------------------------
void CVIEW_ScatterPlot::_Draw_Frame(wxDC &dc, wxRect r)
{
	//-----------------------------------------------------
	Draw_Edge(dc, EDGE_STYLE_SIMPLE, r);

	//-------------------------------------------------
	Draw_Scale(dc, wxRect(r.GetLeft(), r.GetBottom(), r.GetWidth(), 20),
		m_Trend.Get_Data_XMin(), m_Trend.Get_Data_XMax(),
		true , true , true
	);

	Draw_Scale(dc, wxRect(r.GetLeft() - 20, r.GetTop(), 20, r.GetHeight()),
		m_Trend.Get_Data_YMin(), m_Trend.Get_Data_YMax(),
		false, false, false
	);

	Draw_Text(dc, TEXTALIGN_BOTTOMCENTER,
		r.GetLeft() - 25, r.GetTop() + r.GetHeight() / 2, 90.0,
		m_sY.c_str()
	);

	Draw_Text(dc, TEXTALIGN_TOPCENTER,
		r.GetLeft() + r.GetWidth() / 2, r.GetBottom() + 20,
		m_sX.c_str()
	);

	//-------------------------------------------------
	if( m_Options("REG_SHOW")->asBool() )
	{
		_Draw_Regression(dc, r);
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVIEW_ScatterPlot::_Update_Data(void)
{
	bool	bResult;

	m_Trend.Clr_Data();

	m_Parameters.DataObjects_Check();

	if( m_pGrid )
	{
		bResult	= m_Parameters("CMP_WITH")->asInt() == 0
			? _Initialize_Grids(m_pGrid, m_Parameters("GRID")->asGrid())
			: _Initialize_Shapes();
	}
	else if( m_pGrids )
	{
		bResult	= _Initialize_Grids(
			m_pGrids->Get_Grid_Ptr(m_Parameters("BAND_X")->asInt()),
			m_pGrids->Get_Grid_Ptr(m_Parameters("BAND_Y")->asInt())
		);
	}
	else // if( m_pTable )
	{
		bResult	= _Initialize_Table();
	}

	PROCESS_Set_Okay(true);

	//-----------------------------------------------------
	if( bResult )
	{
		CSG_String	Info(_TL("Regression"));

		if( !m_Trend.Set_Formula(m_Options("REG_FORMULA")->asString()) || !m_Trend.Get_Trend() )
		{
			Info	+= CSG_String::Format(" %s!\n", _TL("failed"));
			Info	+= m_Trend.Get_Error();
		}
		else
		{
			Info	+= CSG_String::Format(" %s\n", _TL("Details"));
			Info	+= m_Trend.Get_Formula(SG_TREND_STRING_Complete);
		}

		m_Options("REG_INFO")->Set_Value(Info);

		_Initialize_Count();

		Refresh();

		return( true );
	}

	Refresh();

	return( false );
}

//---------------------------------------------------------
bool CVIEW_ScatterPlot::_Initialize_Count(void)
{
	int	Resolution	= m_Options("DENSITY_RES")->asInt();

	m_Count.Create(SG_DATATYPE_Int, Resolution, Resolution, 1.0);

	double	dx	= (m_Count.Get_NX() - 1.) / (m_Trend.Get_Data_XMax() - m_Trend.Get_Data_XMin());
	double	dy	= (m_Count.Get_NY() - 1.) / (m_Trend.Get_Data_YMax() - m_Trend.Get_Data_YMin());

	for(int i=0; i<m_Trend.Get_Data_Count(); i++)
	{
		int x	= (int)(0.5 + dx * (m_Trend.Get_Data_X(i) - m_Trend.Get_Data_XMin()));
		int y	= (int)(0.5 + dy * (m_Trend.Get_Data_Y(i) - m_Trend.Get_Data_YMin()));

		if( m_Count.is_InGrid(x, y, false) )
		{
			m_Count.Add_Value(x, y, 1);
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVIEW_ScatterPlot::_Initialize_Grids(CSG_Grid *pGrid_X, CSG_Grid *pGrid_Y)
{
	CHECK_DATA(pGrid_X);
	CHECK_DATA(pGrid_Y);

	if( !pGrid_X || !pGrid_Y )
	{
		return( false );
	}

	//-----------------------------------------------------
	m_sTitle.Printf("%s: [%s/%s]", _TL("Scatterplot"), pGrid_X->Get_Name(), pGrid_Y->Get_Name());

	m_sX.Printf("%s", pGrid_X->Get_Name());
	m_sY.Printf("%s", pGrid_Y->Get_Name());

	bool	bEqual		= pGrid_X->Get_System() == pGrid_Y->Get_System();
	int		maxSamples	= m_Options("SAMPLES_MAX")->asInt();
	double	Step		= maxSamples > 0 && pGrid_X->Get_NCells() > maxSamples ? pGrid_X->Get_NCells() / maxSamples : 1.0;

	for(double dCell=0; dCell<pGrid_X->Get_NCells() && PROGRESSBAR_Set_Position(dCell, pGrid_X->Get_NCells()); dCell+=Step)
	{
		sLong	iCell	= (sLong)dCell;

		if( !pGrid_X->is_NoData(iCell) )
		{
			if( bEqual )
			{
				if( !pGrid_Y->is_NoData(iCell) )
				{
					m_Trend.Add_Data(pGrid_X->asDouble(iCell), pGrid_Y->asDouble(iCell));
				}
			}
			else
			{
				TSG_Point	p	= pGrid_X->Get_System().Get_Grid_to_World(
					(int)(iCell % pGrid_X->Get_NX()),
					(int)(iCell / pGrid_X->Get_NX())
				);

				double	y;

				if(	pGrid_Y->Get_Value(p, y) )
				{
					m_Trend.Add_Data(pGrid_X->asDouble(iCell), y);
				}
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVIEW_ScatterPlot::_Initialize_Shapes(void)
{
	CSG_Shapes	*pPoints	= m_Parameters("POINTS")->asShapes();
	int			Field		= m_Parameters("FIELD" )->asInt();

	TSG_Grid_Resampling	Resampling;

	switch( m_Parameters("RESAMPLING")->asInt() )
	{
	default: Resampling = GRID_RESAMPLING_NearestNeighbour; break;
	case  1: Resampling = GRID_RESAMPLING_Bilinear        ; break;
	case  2: Resampling = GRID_RESAMPLING_BicubicSpline   ; break;
	case  3: Resampling = GRID_RESAMPLING_BSpline         ; break;
	}

	CHECK_DATA(m_pGrid);
	CHECK_DATA(pPoints);

	if( !m_pGrid || !pPoints || Field < 0 || Field >= pPoints->Get_Field_Count() )
	{
		return( false );
	}

	m_sTitle.Printf("%s: [%s/%s]", _TL("Scatterplot"), m_pGrid->Get_Name(), pPoints->Get_Name());

	m_sX.Printf("%s", m_pGrid->Get_Name());
	m_sY.Printf("%s", pPoints->Get_Field_Name(Field));

	int	maxSamples	= m_Options("SAMPLES_MAX")->asInt();
	double	x, Step	= maxSamples > 0 && pPoints->Get_Count() > maxSamples ? pPoints->Get_Count() / maxSamples : 1.0;

	for(double i=0; i<pPoints->Get_Count() && PROGRESSBAR_Set_Position(i, pPoints->Get_Count()); i+=Step)
	{
		CSG_Shape	*pShape	= pPoints->Get_Shape((int)i);

		if( !pShape->is_NoData(Field) && m_pGrid->Get_Value(pShape->Get_Point(0), x, Resampling) )
		{
			m_Trend.Add_Data(x, pShape->asDouble(Field));
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVIEW_ScatterPlot::_Initialize_Table(void)
{
	int	xField	= m_Parameters("FIELD_X")->asInt();
	int	yField	= m_Parameters("FIELD_Y")->asInt();

	CHECK_DATA(m_pTable);

	if( !m_pTable || xField < 0 || xField >= m_pTable->Get_Field_Count() || yField < 0 || yField >= m_pTable->Get_Field_Count() )
	{
		return( false );
	}

	int	maxSamples	= m_Options("SAMPLES_MAX")->asInt();
	double	Step	= maxSamples > 0 && m_pTable->Get_Count() > maxSamples ? m_pTable->Get_Count() / maxSamples : 1.0;

	m_sTitle.Printf("%s: [%s]", _TL("Scatterplot"), m_pTable->Get_Name());

	m_sX.Printf("%s", m_pTable->Get_Field_Name(xField));
	m_sY.Printf("%s", m_pTable->Get_Field_Name(yField));

	for(double i=0; i<m_pTable->Get_Record_Count() && PROGRESSBAR_Set_Position(i, m_pTable->Get_Record_Count()); i+=Step)
	{
		CSG_Table_Record	*pRecord	= m_pTable->Get_Record((int)i);

		if( !pRecord->is_NoData(xField) && !pRecord->is_NoData(yField) )
		{
			m_Trend.Add_Data(pRecord->asDouble(xField), pRecord->asDouble(yField));
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

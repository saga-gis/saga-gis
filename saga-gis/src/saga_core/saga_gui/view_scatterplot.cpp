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
		if( !SG_STR_CMP(pParameter->Get_Identifier(), SG_T("CMP_WITH")) )
		{
			pParameters->Get_Parameter("GRID"  )->Get_Parent()->Set_Enabled(pParameter->asInt() == 0);
			pParameters->Get_Parameter("GRID"  )->Set_Enabled(pParameter->asInt() == 0);
			pParameters->Get_Parameter("POINTS")->Set_Enabled(pParameter->asInt() == 1);
		}

		if( !SG_STR_CMP(pParameter->Get_Identifier(), SG_T("REG_SHOW")) )
		{
			pParameters->Get_Parameter("REG_TYPE" )->Set_Enabled(pParameter->asBool());
			pParameters->Get_Parameter("REG_COLOR")->Set_Enabled(pParameter->asBool());
			pParameters->Get_Parameter("REG_SIZE" )->Set_Enabled(pParameter->asBool());
			pParameters->Get_Parameter("REG_INFO" )->Set_Enabled(pParameter->asBool());
		}

		if( !SG_STR_CMP(pParameter->Get_Identifier(), SG_T("DISPLAY")) )
		{
			pParameters->Get_Parameter("DENSITY_RES")->Set_Enabled(pParameter->asInt() == 1);
			pParameters->Get_Parameter("DENSITY_PAL")->Set_Enabled(pParameter->asInt() == 1);
			pParameters->Get_Parameter("DENSITY_LEG")->Set_Enabled(pParameter->asInt() == 1);
		}
	}

	if( Flags & PARAMETER_CHECK_VALUES )
	{
		if( !SG_STR_CMP(pParameter->Get_Identifier(), SG_T("CMP_WITH")) )
		{
			pParameters->Get_Parameter("OPTIONS")->asParameters()->
				Get_Parameter("DISPLAY")->Set_Value(pParameter->asInt() == 0 ? 1 : 0);
		}
	}

	return( 0 );
}

//---------------------------------------------------------
void		Add_ScatterPlot(CSG_Grid *pGrid)
{
	new CVIEW_ScatterPlot(pGrid);
}

//---------------------------------------------------------
void		Add_ScatterPlot(CSG_Table *pTable)
{
	new CVIEW_ScatterPlot(pTable);
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
	EVT_MENU			(ID_CMD_SCATTERPLOT_OPTIONS		, CVIEW_ScatterPlot::On_Options)
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
CVIEW_ScatterPlot::CVIEW_ScatterPlot(CSG_Grid *pGrid)
	: CVIEW_Base(ID_VIEW_SCATTERPLOT, _TL("Scatterplot"), ID_IMG_WND_SCATTERPLOT)
{
	m_pGrid		= pGrid;
	m_pTable	= NULL;

	m_Parameters.Set_Name(CSG_String::Format(SG_T("%s: %s"), _TL("Scatterplot"), m_pGrid->Get_Name()));

	_On_Construction();
}

//---------------------------------------------------------
CVIEW_ScatterPlot::CVIEW_ScatterPlot(CSG_Table *pTable)
	: CVIEW_Base(ID_VIEW_SCATTERPLOT, _TL("Scatterplot"), ID_IMG_WND_SCATTERPLOT)
{
	m_pGrid		= NULL;
	m_pTable	= pTable;

	m_Parameters.Set_Name(CSG_String::Format(SG_T("%s: %s"), _TL("Scatterplot"), m_pTable->Get_Name()));

	_On_Construction();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define m_Options	(*m_Parameters("OPTIONS")->asParameters())

//---------------------------------------------------------
void CVIEW_ScatterPlot::_On_Construction(void)
{
	SYS_Set_Color_BG_Window(this);

	//-----------------------------------------------------
	CSG_Parameter	*pNode;

	m_Parameters.Add_Parameters(NULL, "OPTIONS", _TL("Options"), _TL(""));

	if( m_pGrid )
	{
		m_Parameters.Add_Choice(
			NULL	, "CMP_WITH"	, _TL("Compare with..."),
			_TL(""),
			CSG_String::Format(SG_T("%s|%s|"),
				_TL("another grid"),
				_TL("points")
			), 0
		);

		m_Parameters.Add_Grid(
			NULL	, "GRID"	, _TL("Grid"),
			_TL(""),
			PARAMETER_INPUT, false
		);

		pNode	= m_Parameters.Add_Shapes(
			NULL	, "POINTS"	, _TL("Points"),
			_TL(""),
			PARAMETER_INPUT, SHAPE_TYPE_Point
		);

		m_Parameters.Add_Table_Field(
			pNode	, "FIELD"	, _TL("Attribute"),
			_TL("")
		);
	}
	else if( m_pTable )
	{
		CSG_String	sChoices;

		for(int i=0; i<m_pTable->Get_Field_Count(); i++)
		{
			sChoices.Append(CSG_String::Format(SG_T("%s|"), m_pTable->Get_Field_Name(i)));
		}

		m_Parameters.Add_Choice(NULL, "FIELD_X", SG_T("X"), _TL(""), sChoices);
		m_Parameters.Add_Choice(NULL, "FIELD_Y", SG_T("Y"), _TL(""), sChoices);
	}

	//-----------------------------------------------------
	m_Options.Add_Value(
		NULL	, "SAMPLES_MAX"	, _TL("Maximimum Number of Samples"),
		_TL(""),
		PARAMETER_TYPE_Int, 100000, 0, true
	);

	pNode	= m_Options.Add_Value(
		NULL	, "REG_SHOW"	, _TL("Show Regression"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);

	m_Options.Add_Choice(
		pNode	, "REG_TYPE"	, _TL("Regression Formula"),
		_TL(""),
		SG_T("Y = a + b * X|")
		SG_T("Y = a + b / X|")
		SG_T("Y = a / (b - X)|")
		SG_T("Y = a * X^b|")
		SG_T("Y = a e^(b * X)|")
		SG_T("Y = a + b * ln(X)|")
	);

	m_Options.Add_Value(
		pNode	, "REG_COLOR"	, _TL("Line Colour"),
		_TL(""),
		PARAMETER_TYPE_Color
	);

	m_Options.Add_Value(
		pNode	, "REG_SIZE"	, _TL("Line Size"),
		_TL(""),
		PARAMETER_TYPE_Int, 0, 0, true
	);

	m_Options.Add_Info_String(
		pNode	, "REG_INFO"	, _TL("Regression Details"),
		_TL(""),
		_TL(""), true
	);

	pNode	= m_Options.Add_Choice(
		NULL	, "DISPLAY"		, _TL("Display Type"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("Points"),
			_TL("Density")
		), m_pGrid ? 1 : 0
	);

	m_Options.Add_Value(
		pNode	, "DENSITY_RES"	, _TL("Display Resolution"),
		_TL(""),
		PARAMETER_TYPE_Int, 50, 10, true
	);

	CSG_Colors	Colors(7, SG_COLORS_RAINBOW);

	Colors.Set_Color(0, 255, 255, 255);
	Colors.Set_Count(100);

	m_Options.Add_Colors(
		pNode	, "DENSITY_PAL"	, _TL("Colors"),
		_TL(""),
		&Colors
	);

	m_Options.Add_Value(
		pNode	, "DENSITY_LEG"	, _TL("Show Legend"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);

	//-----------------------------------------------------
	m_Parameters.Set_Callback_On_Parameter_Changed(&Scatter_Plot_On_Parameter_Changed);

	if( DLG_Parameters(&m_Parameters) )
	{
		_Initialize();
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CVIEW_ScatterPlot::On_Parameters(wxCommandEvent &event)
{
	if( DLG_Parameters(&m_Parameters) )
	{
		_Initialize();
	}
}

//---------------------------------------------------------
void CVIEW_ScatterPlot::On_Options(wxCommandEvent &event)
{
	if( DLG_Parameters(&m_Options) )
	{
		_Initialize();
	}
}

//---------------------------------------------------------
void CVIEW_ScatterPlot::On_Update(wxCommandEvent &event)
{
	_Initialize();
}

//---------------------------------------------------------
void CVIEW_ScatterPlot::On_AsTable(wxCommandEvent &event)
{
	if( m_Regression.Get_Count() > 1 )
	{
		CSG_Table	*pTable	= new CSG_Table;

		pTable->Set_Name(CSG_String::Format(SG_T("%s: [%s]-[%s]"), _TL("Scatterplot"), m_sX.c_str(), m_sY.c_str()));

		pTable->Add_Field(SG_T("ID"), SG_DATATYPE_Int);
		pTable->Add_Field(m_sX      , SG_DATATYPE_Double);
		pTable->Add_Field(m_sY      , SG_DATATYPE_Double);

		for(int i=0; i<m_Regression.Get_Count() && PROGRESSBAR_Set_Position(i, m_Regression.Get_Count()); i++)
		{
			CSG_Table_Record	*pRecord	= pTable->Add_Record();

			pRecord->Set_Value(0, i + 1);
			pRecord->Set_Value(1, m_Regression.Get_xValue(i));
			pRecord->Set_Value(2, m_Regression.Get_yValue(i));
		}

		PROGRESSBAR_Set_Position(0);

		g_pData->Add(pTable);
	}
}

//---------------------------------------------------------
#include <wx/clipbrd.h>

void CVIEW_ScatterPlot::On_ToClipboard(wxCommandEvent &event)
{
	wxBitmap	BMP(GetSize());
	wxMemoryDC	dc;
	
	dc.SelectObject(BMP);
	dc.SetBackground(*wxWHITE_BRUSH);
	dc.Clear();

	Draw(dc, wxRect(BMP.GetSize()));

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
//														 //
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GET_DC_X(x)	(r.GetLeft()   + (int)(dx * ((x) - m_Regression.Get_xMin())))
#define GET_DC_Y(y)	(r.GetBottom() - (int)(dy * ((y) - m_Regression.Get_yMin())))

//---------------------------------------------------------
void CVIEW_ScatterPlot::On_Paint(wxPaintEvent &event)
{
	wxPaintDC	dc(this);
	wxRect		r(wxPoint(0, 0), GetClientSize());

	Draw_Edge(dc, EDGE_STYLE_SUNKEN, r);

	Draw(dc, r);
}

//---------------------------------------------------------
void CVIEW_ScatterPlot::Draw(wxDC &dc, wxRect r)
{
	r	= _Draw_Get_rDiagram(r);

	if( m_Regression.Get_Count() > 1 )
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
		wxPoint(r.GetLeft()  + 45, r.GetTop()    + 20),
		wxPoint(r.GetRight() - 20, r.GetBottom() - 40)
	));
}

//---------------------------------------------------------
void CVIEW_ScatterPlot::_Draw_Regression(wxDC &dc, wxRect r)
{
	int		ix, ay, by;
	double	a, b, x, y, dx, dy, ex;
	wxPen	oldPen	= dc.GetPen();

	dc.SetPen(wxPen(
		m_Options("REG_COLOR")->asColor(),
		m_Options("REG_SIZE" )->asInt()
	));

	//-----------------------------------------------------
	a	= m_Regression.Get_Constant();
	b	= m_Regression.Get_Coefficient();

	dx	= (r.GetWidth()  - 1.0) / (m_Regression.Get_xMax() - m_Regression.Get_xMin());
	dy	= (r.GetHeight() - 1.0) / (m_Regression.Get_yMax() - m_Regression.Get_yMin());

	//-----------------------------------------------------
	dc.DrawCircle(
		GET_DC_X(m_Regression.Get_xMean()),
		GET_DC_Y(m_Regression.Get_yMean()), 2
	);

	ex	= (m_Regression.Get_xMax() - m_Regression.Get_xMin()) / (double)r.GetWidth();
	x	= m_Regression.Get_xMin();
	by	= 0;

	for(ix=0; ix<r.GetWidth(); ix++, x+=ex)
	{
		switch( m_Regression.Get_Type() )
		{
		default:				y	= 0.0;				break;
		case REGRESSION_Linear:	y	= a + b * x;		break;
		case REGRESSION_Rez_X:	y	= a + b / x;		break;
		case REGRESSION_Rez_Y:	y	= a / (b - x);		break;
		case REGRESSION_Pow:	y	= a * pow(x, b);	break;
		case REGRESSION_Exp:	y	= a * exp(b * x);	break;
		case REGRESSION_Log:	y	= a + b * log(x);	break;
		}

		ay	= by;
		by	= r.GetBottom() - (int)(dy * (y - m_Regression.Get_yMin()));

		if( ix > 0 && r.GetTop() < ay && ay < r.GetBottom() && r.GetTop() < by && by < r.GetBottom() )
		{
			dc.DrawLine(r.GetLeft() + ix - 1, ay, r.GetLeft() + ix, by);
		}
	}

	dc.SetPen(oldPen);

	//-----------------------------------------------------
	wxString	s;

	switch( m_Regression.Get_Type() )
	{
	case REGRESSION_Linear:	s.Printf(wxT("Y = %f%+f*X"    ), a, b);	break;
	case REGRESSION_Rez_X:	s.Printf(wxT("Y = %f%+f/X"    ), a, b);	break;
	case REGRESSION_Rez_Y:	s.Printf(wxT("Y = %f/(%f-X)"  ), a, b);	break;
	case REGRESSION_Pow:	s.Printf(wxT("Y = %f*X^%f"    ), a, b);	break;
	case REGRESSION_Exp:	s.Printf(wxT("Y = %f e^(%f*X)"), a, b);	break;
	case REGRESSION_Log:	s.Printf(wxT("Y = %f%+f*ln(X)"), a, b);	break;
	}

	Draw_Text(dc, TEXTALIGN_BOTTOMCENTER, r.GetLeft() + r.GetWidth() / 2, r.GetTop(),
		s + wxString::Format(wxT("; r2 = %.2f%%"), 100.0 * m_Regression.Get_R2())
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

	Draw_Text(dc, TEXTALIGN_BOTTOMLEFT, 2 + r.GetRight(), r.GetBottom(), wxT("0"));
	Draw_Text(dc, TEXTALIGN_TOPLEFT   , 2 + r.GetRight(), r.GetTop   (), wxString::Format(wxT("%d"), (int)m_Count.Get_ZMax()));
}

//---------------------------------------------------------
void CVIEW_ScatterPlot::_Draw_Image(wxDC &dc, wxRect r)
{
	CSG_Colors	*pColors	= m_Options("DENSITY_PAL")->asColors();

	wxImage	Image(r.GetWidth(), r.GetHeight());

	double	dCount	= (pColors->Get_Count() - 2.0) / log(1.0 + m_Count.Get_ZMax());

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
	double	dx	= (r.GetWidth()  - 1.0) / (m_Regression.Get_xMax() - m_Regression.Get_xMin());
	double	dy	= (r.GetHeight() - 1.0) / (m_Regression.Get_yMax() - m_Regression.Get_yMin());

	for(int i=0; i<m_Regression.Get_Count(); i++)
	{
		dc.DrawCircle(
			GET_DC_X(m_Regression.Get_xValue(i)),
			GET_DC_Y(m_Regression.Get_yValue(i)), 2
		);
	}
}

//---------------------------------------------------------
void CVIEW_ScatterPlot::_Draw_Frame(wxDC &dc, wxRect r)
{
	//-----------------------------------------------------
	Draw_Edge(dc, EDGE_STYLE_SIMPLE, r);

	//-------------------------------------------------
	Draw_Scale(dc, wxRect(r.GetLeft() - 20, r.GetTop(), 20, r.GetHeight()),
		m_Regression.Get_yMin(), m_Regression.Get_yMax(),
		false, false, false
	);

	Draw_Scale(dc, wxRect(r.GetLeft(), r.GetBottom(), r.GetWidth(), 20),
		m_Regression.Get_xMin(), m_Regression.Get_xMax(),
		true , true , true
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CVIEW_ScatterPlot::_Initialize(void)
{
	bool	bResult;

	m_Regression.Destroy();

	if( m_pGrid )
	{
		if( m_Parameters("CMP_WITH")->asInt() == 0 )
		{
			bResult	= _Initialize_Grids ();
		}
		else
		{
			bResult	= _Initialize_Shapes();
		}
	}
	else // if( m_pTable )
	{
		bResult	= _Initialize_Table ();
	}

	PROCESS_Set_Okay(true);

	//-----------------------------------------------------
	if( bResult && m_Regression.Calculate((TSG_Regression_Type)m_Options("REG_TYPE")->asInt()) )
	{
		m_Options("REG_INFO")->Set_Value(m_Regression.asString());

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

	double	dx	= (m_Count.Get_NX() - 1.0) / (m_Regression.Get_xMax() - m_Regression.Get_xMin());
	double	dy	= (m_Count.Get_NY() - 1.0) / (m_Regression.Get_yMax() - m_Regression.Get_yMin());

	for(int i=0; i<m_Regression.Get_Count(); i++)
	{
		int x	= (int)(0.5 + dx * (m_Regression.Get_xValue(i) - m_Regression.Get_xMin()));
		int y	= (int)(0.5 + dy * (m_Regression.Get_yValue(i) - m_Regression.Get_yMin()));

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
bool CVIEW_ScatterPlot::_Initialize_Grids()
{
	CSG_Grid	*pGrid_Y	= m_Parameters("GRID")->asGrid();

	CHECK_DATA(m_pGrid);
	CHECK_DATA(pGrid_Y);

	if( !m_pGrid || !pGrid_Y )
	{
		return( false );
	}

	//-----------------------------------------------------
	m_sTitle.Printf(SG_T("%s: [%s/%s]"), _TL("Scatterplot"), m_pGrid->Get_Name(), pGrid_Y->Get_Name());

	m_sX.Printf(SG_T("%s"), m_pGrid->Get_Name());
	m_sY.Printf(SG_T("%s"), pGrid_Y->Get_Name());

	bool	bEqual			= m_pGrid->Get_System() == pGrid_Y->Get_System();
	int		Interpolation	= GRID_INTERPOLATION_BSpline;
	int		maxSamples		= m_Options("SAMPLES_MAX")->asInt();
	double	Step			= maxSamples > 0 && m_pGrid->Get_NCells() > maxSamples ? m_pGrid->Get_NCells() / maxSamples : 1.0;

	for(double i=0; i<m_pGrid->Get_NCells() && PROGRESSBAR_Set_Position(i, m_pGrid->Get_NCells()); i+=Step)
	{
		if( !m_pGrid->is_NoData((long)i) )
		{
			if( bEqual )
			{
				if( !pGrid_Y->is_NoData((long)i) )
				{
					m_Regression.Add_Values(
						m_pGrid->asDouble((long)i, true),
						pGrid_Y->asDouble((long)i, true)
					);
				}
			}
			else
			{
				int		x	= ((long)i) % m_pGrid->Get_NX();
				int		y	= ((long)i) / m_pGrid->Get_NX();
				double	z;

				if(	pGrid_Y->Get_Value(m_pGrid->Get_System().Get_Grid_to_World(x, y), z, Interpolation, true) )
				{
					m_Regression.Add_Values(m_pGrid->asDouble((long)i, true), z);
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

	CHECK_DATA(m_pGrid);
	CHECK_DATA(pPoints);

	if( !m_pGrid || !pPoints || Field < 0 || Field >= pPoints->Get_Field_Count() )
	{
		return( false );
	}

	m_sTitle.Printf(SG_T("%s: [%s/%s]"), _TL("Scatterplot"), m_pGrid->Get_Name(), pPoints->Get_Name());

	m_sX.Printf(SG_T("%s"), m_pGrid->Get_Name());
	m_sY.Printf(SG_T("%s"), pPoints->Get_Field_Name(Field));

	int	maxSamples	= m_Options("SAMPLES_MAX")->asInt();
	double	z, Step	= maxSamples > 0 && pPoints->Get_Count() > maxSamples ? pPoints->Get_Count() / maxSamples : 1.0;

	for(double i=0; i<pPoints->Get_Count() && PROGRESSBAR_Set_Position(i, pPoints->Get_Count()); i+=Step)
	{
		CSG_Shape	*pShape	= pPoints->Get_Shape((int)i);

		if( !pShape->is_NoData(Field) && m_pGrid->Get_Value(pShape->Get_Point(0), z, GRID_INTERPOLATION_BSpline, true) )
		{
			m_Regression.Add_Values(z, pShape->asDouble(Field));
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

	m_sTitle.Printf(SG_T("%s: [%s]"), _TL("Scatterplot"), m_pTable->Get_Name());

	m_sX.Printf(SG_T("%s"), m_pTable->Get_Field_Name(xField));
	m_sY.Printf(SG_T("%s"), m_pTable->Get_Field_Name(yField));

	for(double i=0; i<m_pTable->Get_Record_Count() && PROGRESSBAR_Set_Position(i, m_pTable->Get_Record_Count()); i+=Step)
	{
		CSG_Table_Record	*pRecord	= m_pTable->Get_Record((int)i);

		if( !pRecord->is_NoData(xField) && !pRecord->is_NoData(yField) )
		{
			m_Regression.Add_Values(
				pRecord->asDouble(xField),
				pRecord->asDouble(yField)
			);
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

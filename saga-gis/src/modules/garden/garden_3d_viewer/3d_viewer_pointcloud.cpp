/**********************************************************
 * Version $Id: 3d_viewer_pointcloud.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                       3d_viewer                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                3d_viewer_pointcloud.cpp               //
//                                                       //
//                 Copyright (C) 2014 by                 //
//                      Olaf Conrad                      //
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
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/dcclient.h>

#include "3d_viewer_pointcloud.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class C3D_Viewer_PointCloud_Panel : public CSG_3DView_Panel
{
public:
	C3D_Viewer_PointCloud_Panel(wxWindow *pParent, CSG_PointCloud *pPoints, int Field_Color);

	void						Set_Extent				(CSG_Rect Extent);


protected:

	virtual int					On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual void				Update_Statistics		(void);
	virtual void				Update_Parent			(void);

	virtual void				On_Key_Down				(wxKeyEvent   &event);

	virtual bool				On_Before_Draw			(void);
	virtual bool				On_Draw					(void);

	virtual int					Get_Color				(double Value, double z);


private:

	bool						m_Color_bGrad;

	double						m_Color_Min, m_Color_Scale, m_Color_Dim_Min, m_Color_Dim_Max;

	CSG_Array					m_Selection;

	CSG_Colors					m_Colors;

	CSG_Rect					m_Extent;

	CSG_PointCloud				*m_pPoints;


	//-----------------------------------------------------
	DECLARE_EVENT_TABLE()

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
BEGIN_EVENT_TABLE(C3D_Viewer_PointCloud_Panel, CSG_3DView_Panel)
	EVT_KEY_DOWN	(C3D_Viewer_PointCloud_Panel::On_Key_Down)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
C3D_Viewer_PointCloud_Panel::C3D_Viewer_PointCloud_Panel(wxWindow *pParent, CSG_PointCloud *pPoints, int cField)
	: CSG_3DView_Panel(pParent)
{
	m_pPoints	= pPoints;

	//-----------------------------------------------------
	CSG_String	Attributes;

	for(int i=0; i<m_pPoints->Get_Field_Count(); i++)
	{
		Attributes	+= m_pPoints->Get_Field_Name(i);
		Attributes	+= "|";
	}

	//-----------------------------------------------------
	CSG_Parameter	*pNode, *pNode_1;

	//-----------------------------------------------------
	pNode	= m_Parameters("NODE_GENERAL");

	m_Parameters.Add_Value(
		pNode	, "Z_SCALE"			, _TL("Exaggeration"),
		_TL(""),
		PARAMETER_TYPE_Double, 1.0
	);

	//-----------------------------------------------------
	pNode	= m_Parameters.Add_Node(
		NULL	, "NODE_VIEW"		, _TL("Point View Settings"),
		_TL("")
	);

	pNode_1	= m_Parameters.Add_Value(
		pNode	, "DETAIL"			, _TL("Level of Detail"),
		_TL(""),
		PARAMETER_TYPE_Double, 100.0, 0.0, true, 100.0, true
	);

	pNode_1	= m_Parameters.Add_Choice(
		pNode	, "COLORS_ATTR"		, _TL("Colour Attribute"),
		_TL(""),
		Attributes, cField
	);

	m_Parameters.Add_Value(
		pNode_1	, "VAL_AS_RGB"		, _TL("RGB Values"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	m_Parameters.Add_Colors(
		pNode_1	, "COLORS"			, _TL("Colours"),
		_TL("")
	);

	m_Parameters.Add_Value(
		pNode_1	, "COLORS_GRAD"		, _TL("Graduated"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);

	m_Parameters.Add_Range(
		pNode_1	, "COLORS_RANGE"	, _TL("Value Range"),
		_TL("")
	);

	//-----------------------------------------------------
	pNode_1	= m_Parameters.Add_Value(
		pNode	, "DIM"				, _TL("Dim"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	m_Parameters.Add_Range(
		pNode_1	, "DIM_RANGE"		, _TL("Distance Range"),
		_TL(""),
		0.0, 1.0, 0.0, true
	);

	//-----------------------------------------------------
	m_Parameters.Add_Value(
		pNode	, "SIZE"			, _TL("Size"),
		_TL(""),
		PARAMETER_TYPE_Int, 2, 1, true
	);

	m_Parameters.Add_Value(
		pNode	, "SIZE_SCALE"		, _TL("Size Scaling"),
		_TL(""),
		PARAMETER_TYPE_Double, 250.0, 0.0, true
	);

	//-----------------------------------------------------
	m_Extent	= pPoints->Get_Extent();

	m_Selection.Create(sizeof(int), 0, SG_ARRAY_GROWTH_2);

	Update_Statistics();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int C3D_Viewer_PointCloud_Panel::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "VAL_AS_RGB") )
	{
		pParameters->Get_Parameter("COLORS"      )->Set_Enabled(pParameter->asBool() == false);
		pParameters->Get_Parameter("COLORS_RANGE")->Set_Enabled(pParameter->asBool() == false);
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "DIM") )
	{
		pParameters->Get_Parameter("DIM_RANGE")->Set_Enabled(pParameter->asBool());
	}

	return( CSG_3DView_Panel::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void C3D_Viewer_PointCloud_Panel::Set_Extent(CSG_Rect Extent)
{
	if( Extent.Get_XRange() == 0.0 || Extent.Get_YRange() == 0.0 )
	{
		Extent	= m_pPoints->Get_Extent();
	}

	if( Extent.is_Equal(m_Extent) == false )
	{
		m_Extent	= Extent;
			
		Update_View(true);
	}
}

//---------------------------------------------------------
void C3D_Viewer_PointCloud_Panel::Update_Statistics(void)
{
	m_Data_Min.x	= m_Extent.Get_XMin();
	m_Data_Max.x	= m_Extent.Get_XMax();

	m_Data_Min.y	= m_Extent.Get_YMin();
	m_Data_Max.y	= m_Extent.Get_YMax();

	m_Selection.Set_Array(0);

	if( m_Extent.is_Equal(m_pPoints->Get_Extent()) )
	{
		int	cField	= m_Parameters("COLORS_ATTR")->asInt();

		m_Parameters("COLORS_RANGE")->asRange()->Set_Range(
			m_pPoints->Get_Mean(cField) - 1.5 * m_pPoints->Get_StdDev(cField),
			m_pPoints->Get_Mean(cField) + 1.5 * m_pPoints->Get_StdDev(cField)
		);

		m_Data_Min.z	= m_pPoints->Get_Minimum(2);	// Get_ZMin();	ToDo in CSG_PointCloud class!!!
		m_Data_Max.z	= m_pPoints->Get_Maximum(2);	// Get_ZMax();	ToDo in CSG_PointCloud class!!!
	}
	else
	{
		CSG_Simple_Statistics	cStats, zStats;

		int	cField	= m_Parameters("COLORS_ATTR")->asInt();

		for(int i=0; i<m_pPoints->Get_Count(); i++)
		{
			m_pPoints->Set_Cursor(i);

			if( m_Extent.Contains(m_pPoints->Get_X(), m_pPoints->Get_Y()) && m_Selection.Inc_Array() )
			{
				*((int *)m_Selection.Get_Entry(m_Selection.Get_Size() - 1))	= i;

				cStats	+= m_pPoints->Get_Value(cField);
				zStats	+= m_pPoints->Get_Z();
			}
		}

		m_Parameters("COLORS_RANGE")->asRange()->Set_Range(
			cStats.Get_Mean() - 1.5 * cStats.Get_StdDev(),
			cStats.Get_Mean() + 1.5 * cStats.Get_StdDev()
		);

		m_Data_Min.z	= zStats.Get_Minimum();
		m_Data_Max.z	= zStats.Get_Maximum();
	}
}

//---------------------------------------------------------
void C3D_Viewer_PointCloud_Panel::Update_Parent(void)
{
	((CSG_3DView_Dialog *)GetParent())->Update_Controls();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void C3D_Viewer_PointCloud_Panel::On_Key_Down(wxKeyEvent &event)
{
	switch( event.GetKeyCode() )
	{
	default:	CSG_3DView_Panel::On_Key_Down(event);	return;

	case WXK_F1:	m_Parameters("Z_SCALE"   )->Set_Value(m_Parameters("Z_SCALE"   )->asDouble() -  0.5);	break;
	case WXK_F2:	m_Parameters("Z_SCALE"   )->Set_Value(m_Parameters("Z_SCALE"   )->asDouble() +  0.5);	break;

	case WXK_F5:	m_Parameters("SIZE"      )->Set_Value(m_Parameters("SIZE"      )->asDouble() -  1.0);	break;
	case WXK_F6:	m_Parameters("SIZE"      )->Set_Value(m_Parameters("SIZE"      )->asDouble() +  1.0);	break;

	case WXK_F7:	m_Parameters("SIZE_SCALE")->Set_Value(m_Parameters("SIZE_SCALE")->asDouble() - 10.0);	break;
	case WXK_F8:	m_Parameters("SIZE_SCALE")->Set_Value(m_Parameters("SIZE_SCALE")->asDouble() + 10.0);	break;
	}

	//-----------------------------------------------------
	Update_View();
	Update_Parent();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool C3D_Viewer_PointCloud_Panel::On_Before_Draw(void)
{
	if( m_Play_State == SG_3DVIEW_PLAY_STOP )
	{
		m_Projector.Set_zScaling(m_Projector.Get_xScaling() * m_Parameters("Z_SCALE")->asDouble());
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int C3D_Viewer_PointCloud_Panel::Get_Color(double Value, double z)
{
	int	Color;

	if( m_Color_Scale <= 0.0 )
	{
		Color	= (int)Value;
	}
	else
	{
		double	c	= m_Color_Scale * (Value - m_Color_Min);

		Color	= m_Color_bGrad ? m_Colors.Get_Interpolated(c) : m_Colors[(int)c];
	}

	//-----------------------------------------------------
	if( m_Color_Dim_Min < m_Color_Dim_Max )
	{
		double	dim	= 1.0 - (z - m_Color_Dim_Min) / (m_Color_Dim_Max - m_Color_Dim_Min);

		if( dim < 1.0 )
		{
			Color	= Dim_Color(Color, dim < 0.1 ? 0.1 : dim);
		}
	}

	return( Color );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool C3D_Viewer_PointCloud_Panel::On_Draw(void)
{
	//-----------------------------------------------------
	int		cField	= m_Parameters("COLORS_ATTR")->asInt();

	if( m_Parameters("COLORS_RANGE")->asRange()->Get_LoVal()
	>=  m_Parameters("COLORS_RANGE")->asRange()->Get_HiVal() )
	{
		m_Parameters("COLORS_RANGE")->asRange()->Set_Range(
			m_pPoints->Get_Mean(cField) - 1.5 * m_pPoints->Get_StdDev(cField),
			m_pPoints->Get_Mean(cField) + 1.5 * m_pPoints->Get_StdDev(cField)
		);
	}

	m_Colors		= *m_Parameters("COLORS")->asColors();
	m_Color_bGrad	= m_Parameters("COLORS_GRAD")->asBool();
	m_Color_Min		= m_Parameters("COLORS_RANGE")->asRange()->Get_LoVal();
	m_Color_Scale	= m_Parameters("VAL_AS_RGB")->asBool() ? 0.0 :
		m_Colors.Get_Count() / (m_Parameters("COLORS_RANGE")->asRange()->Get_HiVal() - m_Color_Min);

	if( m_Parameters("DIM")->asBool() )
	{
		m_Color_Dim_Min	= m_Parameters("DIM_RANGE")->asRange()->Get_LoVal() * (m_Data_Max.z - m_Data_Min.z);
		m_Color_Dim_Max	= m_Parameters("DIM_RANGE")->asRange()->Get_HiVal() * (m_Data_Max.z - m_Data_Min.z);
	}
	else
	{
		m_Color_Dim_Min	= m_Color_Dim_Max	= 0.0;
	}

	//-----------------------------------------------------
	int		Size	= m_Parameters("SIZE"      )->asInt   ();
	double	dSize	= m_Parameters("SIZE_SCALE")->asDouble();
	
	if( dSize > 0.0 )	dSize = 1.0 / dSize; else dSize = 0.0;

	//-----------------------------------------------------
	int		nSkip	= 1 + (int)(0.001 * m_pPoints->Get_Count() * SG_Get_Square(1.0 - 0.01 * m_Parameters("DETAIL")->asDouble()));

	//-----------------------------------------------------
	int	nPoints	= m_Selection.Get_Size() > 0 ? (int)m_Selection.Get_Size() : m_pPoints->Get_Count();

	#pragma omp parallel for
	for(int iPoint=0; iPoint<nPoints; iPoint+=nSkip)
	{
		int	jPoint	= m_Selection.Get_Size() > 0 ? *((int *)m_Selection.Get_Entry(iPoint)) : iPoint;

		TSG_Point_Z	p	= m_pPoints->Get_Point(jPoint);

		m_Projector.Get_Projection(p);

		Draw_Point(p.x, p.y, p.z,
			Get_Color(m_pPoints->Get_Value(jPoint, cField), p.z),
			Size + (dSize <= 0.0 ? 0 : (int)(20.0 * exp(-dSize * p.z)))
		);
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CPointCloud_Overview : public wxDialog
{
public:
	CPointCloud_Overview(void)	{}

	void						Create			(wxWindow *pParent, CSG_PointCloud *pPoints, C3D_Viewer_PointCloud_Panel *pPanel)
	{
		m_pPanel	= pPanel;

		wxDialog::Create(pParent, wxID_ANY, _TL("Overview"), wxDefaultPosition, wxDefaultSize,
			wxCAPTION|wxCLOSE_BOX|wxSTAY_ON_TOP
		);

		//-------------------------------------------------
		m_Ratio	= pPoints->Get_Extent().Get_XRange() / pPoints->Get_Extent().Get_YRange();

		m_Count.Create(CSG_Grid_System(m_Ratio > 1.0
			? pPoints->Get_Extent().Get_XRange() / 100.0
			: pPoints->Get_Extent().Get_YRange() / 100.0,
			pPoints->Get_Extent()), SG_DATATYPE_Int
		);

		for(int i=0, x, y; i<pPoints->Get_Count(); i++)
		{
			if( m_Count.Get_System().Get_World_to_Grid(x, y, pPoints->Get_X(i), pPoints->Get_Y(i)) ) // && m_Count.is_InGrid(x, y, false) )
			{
				m_Count.Add_Value(x, y, 1);
			}
		}

		int	Size	= GetClientSize().GetWidth();

		if( m_Ratio > 1.0 )
			Set_Size(Size, (int)(Size / m_Ratio), false);
		else
			Set_Size((int)(Size * m_Ratio), Size, false);
	}


private:

	double						m_Ratio;

	wxPoint						m_Mouse_Down, m_Mouse_Move;

	CSG_Rect					m_Selection;

	wxImage						m_Image;

	CSG_Grid					m_Count;

	C3D_Viewer_PointCloud_Panel	*m_pPanel;


	//---------------------------------------------------------
	void						On_Mouse_LDown	(wxMouseEvent &event)
	{
		m_Mouse_Down	= m_Mouse_Move	= event.GetPosition();

		Draw_Inverse(m_Mouse_Down, m_Mouse_Move);

		CaptureMouse();
	}

	//---------------------------------------------------------
	void						On_Mouse_Motion	(wxMouseEvent &event)
	{
		if( HasCapture() && event.Dragging() && event.LeftIsDown() )
		{
			Draw_Inverse(m_Mouse_Down, m_Mouse_Move);
			Draw_Inverse(m_Mouse_Down, event.GetPosition());
		}

		m_Mouse_Move	= event.GetPosition();
	}

	//---------------------------------------------------------
	void						On_Mouse_LUp	(wxMouseEvent &event)
	{
		if( HasCapture() )
		{
			ReleaseMouse();
		}

		if( m_Mouse_Down.x != event.GetX() && m_Mouse_Down.y != event.GetY() )
		{
			double	dx	=  m_Count.Get_XRange() / GetClientSize().GetWidth ();
			double	dy	= -m_Count.Get_YRange() / GetClientSize().GetHeight();

			m_Selection.Assign(
				m_Count.Get_XMin() + dx * m_Mouse_Down.x,
				m_Count.Get_YMax() + dy * m_Mouse_Down.y,
				m_Count.Get_XMin() + dx * event.GetX(),
				m_Count.Get_YMax() + dy * event.GetY()
			);

			m_pPanel->Set_Extent(m_Selection);
		}

		Refresh(false);
	}

	//---------------------------------------------------------
	void						On_Mouse_RDown	(wxMouseEvent &event)
	{
		m_Selection.Assign(0.0, 0.0, 0.0, 0.0);

		m_pPanel->Set_Extent(m_Selection);

		Refresh(false);
	}

	//---------------------------------------------------------
	void						On_Key_Down		(wxKeyEvent   &event)
	{
		switch( event.GetKeyCode() )
		{
		case WXK_PAGEUP:
			Set_Size(GetClientSize().GetWidth() * 1.25, GetClientSize().GetHeight() * 1.25, true);
			break;

		case WXK_PAGEDOWN:
			Set_Size(GetClientSize().GetWidth() / 1.25, GetClientSize().GetHeight() / 1.25, true);
			break;
		}
	}

	//---------------------------------------------------------
	void						On_Close		(wxCloseEvent &event)
	{
		Hide();	((CSG_3DView_Dialog *)GetParent())->Update_Controls();
	}

	//---------------------------------------------------------
	void						On_Paint		(wxPaintEvent &WXUNUSED(event))
	{
		wxPaintDC	dc(this);

		if( m_Image.IsOk() )
		{
			dc.DrawBitmap(wxBitmap(m_Image), GetClientRect().GetTopLeft());
		}

		if( m_Selection.Get_XRange() > 0.0 && m_Selection.Get_YRange() > 0.0 )
		{
			dc.SetPen(wxPen(*wxRED));

			double	dx	=  GetClientSize().GetWidth () / m_Count.Get_XRange();
			double	dy	= -GetClientSize().GetHeight() / m_Count.Get_YRange();

			int	ax	= dx * (m_Selection.Get_XMin() - m_Count.Get_XMin());
			int	ay	= dy * (m_Selection.Get_YMin() - m_Count.Get_YMin()) + GetClientSize().GetHeight();
			int	bx	= dx * (m_Selection.Get_XMax() - m_Count.Get_XMin());
			int	by	= dy * (m_Selection.Get_YMax() - m_Count.Get_YMin()) + GetClientSize().GetHeight();

			Draw_Inverse(wxPoint(ax, ay), wxPoint(bx, by));

			dc.DrawLine(ax, ay, ax, by);
			dc.DrawLine(ax, by, bx, by);
			dc.DrawLine(bx, by, bx, ay);
			dc.DrawLine(bx, ay, ax, ay);
		}
	}

	//---------------------------------------------------------
	void						Draw_Inverse	(wxPoint A, wxPoint B)
	{
		wxClientDC	dc(this);
		dc.SetLogicalFunction(wxINVERT);
		dc.DrawRectangle(A.x, A.y, B.x - A.x, B.y - A.y);
	}

	//---------------------------------------------------------
	void						Set_Size		(int Width, int Height, bool bRefresh)
	{
		if( Width < 100 || Height < 100 || Width > 1000 || Height > 1000 )
		{
			return;
		}

		SetClientSize(Width, Height);

		if( !m_Image.IsOk() || m_Image.GetWidth() != Width )
		{
			CSG_Colors	Colors(7, SG_COLORS_RAINBOW);
			Colors.Set_Color(0, m_pPanel->m_Parameters("BGCOLOR")->asColor());
			Colors.Set_Count(100);

			double	dCount	= (Colors.Get_Count() - 2.0) / log(1.0 + m_Count.Get_ZMax());
			double	dx		= m_Count.Get_XRange() / (double)Width ;
			double	dy		= m_Count.Get_YRange() / (double)Height;

			m_Image.Create(Width, Height, false);

			#pragma omp parallel for
			for(int y=0; y<m_Image.GetHeight(); y++)
			{
				double	Count, ix = m_Count.Get_XMin(), iy = m_Count.Get_YMax() - y * dy;

				for(int x=0; x<m_Image.GetWidth(); x++, ix+=dx)
				{
					int	i	= m_Count.Get_Value(ix, iy, Count) && Count > 0.0 ? (int)(log(1.0 + Count) * dCount) : 0;

					m_Image.SetRGB(x, y, Colors.Get_Red(i), Colors.Get_Green(i), Colors.Get_Blue(i));
				}
			}
		}

		if( bRefresh )
		{
			Refresh(false);
		}
	}


	DECLARE_EVENT_TABLE()

};

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CPointCloud_Overview, wxDialog)
	EVT_LEFT_DOWN	(CPointCloud_Overview::On_Mouse_LDown)
	EVT_LEFT_UP		(CPointCloud_Overview::On_Mouse_LUp)
	EVT_RIGHT_DOWN	(CPointCloud_Overview::On_Mouse_RDown)
	EVT_MOTION		(CPointCloud_Overview::On_Mouse_Motion)
	EVT_KEY_DOWN	(CPointCloud_Overview::On_Key_Down)
	EVT_CLOSE		(CPointCloud_Overview::On_Close)
	EVT_PAINT		(CPointCloud_Overview::On_Paint)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class C3D_Viewer_PointCloud_Dialog : public CSG_3DView_Dialog
{
public:
	C3D_Viewer_PointCloud_Dialog(CSG_PointCloud *pPoints, int Field_Color)
		: CSG_3DView_Dialog(_TL("Point Cloud Viewer"))
	{
		Create(new C3D_Viewer_PointCloud_Panel(this, pPoints, Field_Color));

		wxArrayString	Attributes;

		for(int i=0; i<pPoints->Get_Field_Count(); i++)
		{
			Attributes.Add(pPoints->Get_Field_Name(i));
		}

		Add_Spacer();
		m_pField_C	= Add_Choice  (_TL("Colour"  ), Attributes, Field_Color);

		Add_Spacer();
		m_pOverview	= Add_CheckBox(_TL("Overview"), false);

		m_Overview.Create(this, pPoints, (C3D_Viewer_PointCloud_Panel *)m_pPanel);
	}

	virtual void				Update_Controls			(void);


protected:

	wxChoice					*m_pField_C;

	wxCheckBox					*m_pOverview;

	CPointCloud_Overview		m_Overview;


	virtual void				On_Update_Choices		(wxCommandEvent &event);
	virtual void				On_Update_Control		(wxCommandEvent &event);

	virtual void				Set_Menu				(wxMenu &Menu);
	virtual void				On_Menu					(wxCommandEvent &event);
	virtual void				On_Menu_UI				(wxUpdateUIEvent &event);


private:

	DECLARE_EVENT_TABLE()

};

//---------------------------------------------------------
BEGIN_EVENT_TABLE(C3D_Viewer_PointCloud_Dialog, CSG_3DView_Dialog)
	EVT_CHECKBOX	(wxID_ANY, C3D_Viewer_PointCloud_Dialog::On_Update_Control)
	EVT_CHOICE		(wxID_ANY, C3D_Viewer_PointCloud_Dialog::On_Update_Choices)
END_EVENT_TABLE()

//---------------------------------------------------------
void C3D_Viewer_PointCloud_Dialog::On_Update_Choices(wxCommandEvent &event)
{
	if( event.GetEventObject() == m_pField_C )
	{
		m_pPanel->m_Parameters("COLORS_ATTR")->Set_Value(m_pField_C->GetSelection());
		m_pPanel->Update_View(true);
		return;
	}

	CSG_3DView_Dialog::On_Update_Choices(event);
}

//---------------------------------------------------------
void C3D_Viewer_PointCloud_Dialog::On_Update_Control(wxCommandEvent &event)
{
	if( event.GetEventObject() == m_pOverview )
	{
#ifdef _SAGA_MSW
		m_Overview.Show(m_pOverview->GetValue() == 1 ? true : false);	// unluckily this does not work with linux (broken event handler chain, non-modal dialog as subprocess of a modal one!!)
#else
		m_Overview.ShowModal();
#endif
	}

	CSG_3DView_Dialog::On_Update_Control(event);
}

//---------------------------------------------------------
void C3D_Viewer_PointCloud_Dialog::Update_Controls(void)
{
	m_pField_C->SetSelection(m_pPanel->m_Parameters("COLORS_ATTR")->asInt());

	m_pOverview->SetValue(m_Overview.IsShown());

	CSG_3DView_Dialog::Update_Controls();
}

//---------------------------------------------------------
enum
{
	MENU_SCALE_Z_DEC	= MENU_USER_FIRST,
	MENU_SCALE_Z_INC,
	MENU_VAL_AS_RGB,
	MENU_COLORS_GRAD,
	MENU_SIZE_DEC,
	MENU_SIZE_INC,
	MENU_SIZE_SCALE_DEC,
	MENU_SIZE_SCALE_INC
};

//---------------------------------------------------------
void C3D_Viewer_PointCloud_Dialog::Set_Menu(wxMenu &Menu)
{
	wxMenu	*pMenu	= Menu.FindChildItem(Menu.FindItem(_TL("Display")))->GetSubMenu();

	pMenu->AppendSeparator();
	pMenu->Append         (MENU_SCALE_Z_DEC   , _TL("Decrease Exaggeration [F1]"));
	pMenu->Append         (MENU_SCALE_Z_INC   , _TL("Increase Exaggeration [F2]"));

	pMenu->AppendSeparator();
	pMenu->Append         (MENU_SIZE_DEC      , _TL("Decrease Size [F5]"));
	pMenu->Append         (MENU_SIZE_INC      , _TL("Increase Size [F6]"));

	pMenu->AppendSeparator();
	pMenu->Append         (MENU_SIZE_SCALE_DEC, _TL("Decrease Size Scaling Factor [F7]"));
	pMenu->Append         (MENU_SIZE_SCALE_INC, _TL("Increase Size Scaling Factor [F8]"));

	pMenu->AppendSeparator();
	pMenu->AppendCheckItem(MENU_VAL_AS_RGB    , _TL("Value as RGB"));
	pMenu->AppendCheckItem(MENU_COLORS_GRAD   , _TL("Graduated Colours"));
}

//---------------------------------------------------------
void C3D_Viewer_PointCloud_Dialog::On_Menu(wxCommandEvent &event)
{
	switch( event.GetId() )
	{
	default:	CSG_3DView_Dialog::On_Menu(event);	return;

	case MENU_SCALE_Z_DEC   :	MENU_VALUE_ADD("Z_SCALE"   ,  -0.5);	return;
	case MENU_SCALE_Z_INC   :	MENU_VALUE_ADD("Z_SCALE"   ,   0.5);	return;

	case MENU_SIZE_DEC      :	MENU_VALUE_ADD("SIZE"      ,  -1.0);	return;
	case MENU_SIZE_INC      :	MENU_VALUE_ADD("SIZE"      ,   1.0);	return;

	case MENU_SIZE_SCALE_DEC:	MENU_VALUE_ADD("SIZE_SCALE", -10.0);	return;
	case MENU_SIZE_SCALE_INC:	MENU_VALUE_ADD("SIZE_SCALE",  10.0);	return;

	case MENU_VAL_AS_RGB    :	MENU_TOGGLE("VAL_AS_RGB" );	return;
	case MENU_COLORS_GRAD   :	MENU_TOGGLE("COLORS_GRAD");	return;
	}
}

//---------------------------------------------------------
void C3D_Viewer_PointCloud_Dialog::On_Menu_UI(wxUpdateUIEvent &event)
{
	switch( event.GetId() )
	{
	default:	CSG_3DView_Dialog::On_Menu_UI(event);	break;

	case MENU_VAL_AS_RGB :	event.Check(m_pPanel->m_Parameters("VAL_AS_RGB" )->asBool());	break;
	case MENU_COLORS_GRAD:	event.Check(m_pPanel->m_Parameters("COLORS_GRAD")->asBool());	break;
	}
}

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
C3D_Viewer_PointCloud::C3D_Viewer_PointCloud(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Point Cloud Viewer"));

	Set_Author		("O. Conrad (c) 2014");

	Set_Description	(_TW(
		"3D viewer for point clouds."
	));

	//-----------------------------------------------------
	CSG_Parameter	*pNode	= Parameters.Add_PointCloud(
		NULL	, "POINTS"	, _TL("Point Cloud"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "COLOR"	, _TL("Color"),
		_TL("")
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool C3D_Viewer_PointCloud::On_Execute(void)
{
	if( !SG_UI_Get_Window_Main() )
	{
		Message_Add(_TL("point cloud viewer can only be run from graphical user interface"));

		return( false );
	}

	CSG_PointCloud	*pPoints	= Parameters("POINTS")->asPointCloud();

	if( pPoints->Get_Count() <= 0 )
	{
		Message_Add(_TL("point cloud viewer will not be started, because point cloud has no points"));

		return( false );
	}

	C3D_Viewer_PointCloud_Dialog	dlg(pPoints, Parameters("COLOR")->asInt());

	dlg.ShowModal();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

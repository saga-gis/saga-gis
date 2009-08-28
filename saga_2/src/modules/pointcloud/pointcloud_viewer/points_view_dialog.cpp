
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//            geostatistics_kriging_variogram            //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 points_view_dialog.cpp                //
//                                                       //
//                 Copyright (C) 2009 by                 //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Bundesstr. 55                          //
//                20146 Hamburg                          //
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
#include "points_view_control.h"
#include "points_view_extent.h"
#include "points_view_dialog.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CPoints_View_Dialog, CSGDI_Dialog)
	EVT_KEY_DOWN	(CPoints_View_Dialog::On_Key_Down)

	EVT_BUTTON		(wxID_ANY	, CPoints_View_Dialog::On_Button)
	EVT_CHECKBOX	(wxID_ANY	, CPoints_View_Dialog::On_Update_Control)
	EVT_TEXT_ENTER	(wxID_ANY	, CPoints_View_Dialog::On_Update_Control)
	EVT_SLIDER		(wxID_ANY	, CPoints_View_Dialog::On_Update_Control)
	EVT_CHOICE		(wxID_ANY	, CPoints_View_Dialog::On_Update_Choices)
END_EVENT_TABLE()

//---------------------------------------------------------
CPoints_View_Dialog::CPoints_View_Dialog(CSG_PointCloud *pPoints)
	: CSGDI_Dialog(_TL("Point Cloud Viewer"), SGDI_DLG_STYLE_START_MAXIMISED)
{
	SetWindowStyle(wxDEFAULT_FRAME_STYLE|wxNO_FULL_REPAINT_ON_RESIZE);

	//-----------------------------------------------------

	//-----------------------------------------------------
	wxArrayString	Attributes;

	Attributes.Empty();

	for(int i=0; i<pPoints->Get_Field_Count(); i++)
	{
		Attributes.Add(pPoints->Get_Field_Name(i));
	}

	//-----------------------------------------------------
	Add_Button(_TL("Close"), wxID_OK);

	m_pBtn_Prop			= Add_Button(_TL("Properties"), wxID_ANY);

	Add_Spacer();
	m_pField_Color		= Add_Choice	(_TL("Color Attribute")	, Attributes, pPoints->Get_Field_Count() - 1);
	m_pField_Z			= Add_Choice	(_TL("Z Attribute")		, Attributes, pPoints->Get_Field_Count() - 1);

	Add_Spacer();
	m_pCheck_Dist		= Add_CheckBox	(_TL("Central Projection")	, true);
	m_pCheck_Stereo		= Add_CheckBox	(_TL("Anaglyph")			, false);
	m_pCheck_Bold		= Add_CheckBox	(_TL("Bold Points")			, false);

	Add_Spacer();
	m_pSlide_xRotate	= Add_Slider	(_TL("X-Rotation"), 0.0, -180.0, 180.0);
	m_pSlide_yRotate	= Add_Slider	(_TL("Y-Rotation"), 0.0, -180.0, 180.0);
	m_pSlide_zRotate	= Add_Slider	(_TL("Z-Rotation"), 0.0, -180.0, 180.0);

	m_pSlide_Dist		= Add_Slider	(_TL("Eye Distance"), 500.0, 1.0, 1000.0);

	m_pSlide_Detail		= Add_Slider	(_TL("Level of Detail"), 1.0, 0.0, 1.0);

	Add_Spacer();
	Add_CustomCtrl(_TL("Extent"), m_pExtent = new CPoints_View_Extent(this, pPoints, wxSize(200, 200)));

//	Add_Spacer();
//	m_pParameters		= Add_TextCtrl	(_TL("Function Parameters"), wxTE_MULTILINE|wxTE_READONLY);

	//-----------------------------------------------------
	Add_Output(m_pView = new CPoints_View_Control(this, pPoints));

	//-----------------------------------------------------
	m_pView->m_bDist		= m_pCheck_Dist		->GetValue() == 1 ? 1 : 0;
	m_pView->m_bStereo		= m_pCheck_Stereo	->GetValue() == 1 ? 1 : 0;
	m_pView->m_Bold			= m_pCheck_Bold		->GetValue() == 1 ? 1 : 0;

	m_pView->m_xRotate		= m_pSlide_xRotate	->Get_Value() * M_DEG_TO_RAD;
	m_pView->m_yRotate		= m_pSlide_yRotate	->Get_Value() * M_DEG_TO_RAD;
	m_pView->m_zRotate		= m_pSlide_zRotate	->Get_Value() * M_DEG_TO_RAD;

	m_pView->m_Dist			= m_pSlide_Dist		->Get_Value();
	m_pView->m_Detail		= m_pSlide_Detail	->Get_Value();

	m_pView->m_cField		= m_pField_Color	->GetSelection();
	m_pView->m_zField		= m_pField_Z		->GetSelection();
}

//---------------------------------------------------------
void CPoints_View_Dialog::On_Update_Control(wxCommandEvent &event)
{
	if( event.GetEventObject() == m_pCheck_Dist )
	{
		m_pView->m_bDist		= m_pCheck_Dist->GetValue() == 1 ? 1 : 0;
	}
	else if( event.GetEventObject() == m_pCheck_Stereo )
	{	
		m_pView->m_bStereo		= m_pCheck_Stereo->GetValue() == 1 ? 1 : 0;
	}
	else if( event.GetEventObject() == m_pCheck_Bold )
	{	
		m_pView->m_Bold			= m_pCheck_Bold->GetValue() == 1 ? 1 : 0;
	}
	else if( event.GetEventObject() == m_pSlide_xRotate )
	{	
		m_pView->m_xRotate		= m_pSlide_xRotate	->Get_Value() * M_DEG_TO_RAD;
	}
	else if( event.GetEventObject() == m_pSlide_yRotate )
	{	
		m_pView->m_yRotate		= m_pSlide_yRotate	->Get_Value() * M_DEG_TO_RAD;
	}
	else if( event.GetEventObject() == m_pSlide_zRotate )
	{	
		m_pView->m_zRotate		= m_pSlide_zRotate	->Get_Value() * M_DEG_TO_RAD;
	}
	else if( event.GetEventObject() == m_pSlide_Dist )
	{	
		m_pView->m_Dist			= m_pSlide_Dist		->Get_Value();
	}
	else if( event.GetEventObject() == m_pSlide_Detail )
	{	
		m_pView->m_Detail		= m_pSlide_Detail	->Get_Value();
	}
	else
	{
		return;
	}

	m_pView->Update_View();
}

//---------------------------------------------------------
void CPoints_View_Dialog::On_Update_Choices(wxCommandEvent &event)
{
	if( event.GetEventObject() == m_pField_Z )
	{
		m_pView->m_zField		= m_pField_Z	->GetSelection();
	}
	else if( event.GetEventObject() == m_pField_Color )
	{
		m_pView->m_Settings("C_RANGE")->asRange()->Set_Range(0.0, 0.0);

		m_pView->m_cField		= m_pField_Color->GetSelection();
	}
	else
	{
		return;
	}

	m_pView->Update_View();
}

//---------------------------------------------------------
void CPoints_View_Dialog::On_Button(wxCommandEvent &event)
{
	if( event.GetEventObject() == m_pBtn_Prop )
	{
		if( SG_UI_Dlg_Parameters(&m_pView->m_Settings, m_pView->m_Settings.Get_Name()) )
		{
			m_pView->Update_View();
		}
	}
	else
	{
		event.Skip();
	}
}

//---------------------------------------------------------
void CPoints_View_Dialog::On_Key_Down(wxKeyEvent &event)
{
	m_pView->On_Key_Down(event);
}

//---------------------------------------------------------
void CPoints_View_Dialog::Update_Extent(void)
{
	m_pView->Update_Extent(m_pExtent->Get_Extent());
}

//---------------------------------------------------------
void CPoints_View_Dialog::Update_Rotation(void)
{
	double	d;

	d	= fmod(M_RAD_TO_DEG * m_pView->m_xRotate, 360.0);	if( d < -180.0 )	d	+= 360.0;	else if( d > 180.0 )	d	-= 360.0;
	m_pSlide_xRotate->Set_Value(d);

	d	= fmod(M_RAD_TO_DEG * m_pView->m_yRotate, 360.0);	if( d < -180.0 )	d	+= 360.0;	else if( d > 180.0 )	d	-= 360.0;
	m_pSlide_yRotate->Set_Value(d);

	d	= fmod(M_RAD_TO_DEG * m_pView->m_zRotate, 360.0);	if( d < -180.0 )	d	+= 360.0;	else if( d > 180.0 )	d	-= 360.0;
	m_pSlide_zRotate->Set_Value(d);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

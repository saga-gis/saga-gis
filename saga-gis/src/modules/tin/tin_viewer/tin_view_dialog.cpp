/**********************************************************
 * Version $Id: tin_view_dialog.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      tin_viewer                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  tin_view_dialog.cpp                  //
//                                                       //
//                 Copyright (C) 2011 by                 //
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
#include "tin_view_control.h"
#include "tin_view_dialog.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CTIN_View_Dialog, CSGDI_Dialog)
	EVT_MOUSEWHEEL	(CTIN_View_Dialog::On_Mouse_Wheel)
	EVT_BUTTON		(wxID_ANY	, CTIN_View_Dialog::On_Button)
	EVT_CHECKBOX	(wxID_ANY	, CTIN_View_Dialog::On_Update_Control)
	EVT_TEXT_ENTER	(wxID_ANY	, CTIN_View_Dialog::On_Update_Control)
	EVT_SLIDER		(wxID_ANY	, CTIN_View_Dialog::On_Update_Control)
	EVT_CHOICE		(wxID_ANY	, CTIN_View_Dialog::On_Update_Choices)
END_EVENT_TABLE()

//---------------------------------------------------------
CTIN_View_Dialog::CTIN_View_Dialog(CSG_TIN *pTIN, int Field_Z, int Field_Color, CSG_Grid *pRGB)
	: CSGDI_Dialog(_TL("TIN Viewer"), SGDI_DLG_STYLE_START_MAXIMISED)
{
	SetWindowStyle(wxDEFAULT_FRAME_STYLE|wxNO_FULL_REPAINT_ON_RESIZE);

	//-----------------------------------------------------
	m_Settings.Create(NULL, _TL("TIN Viewer Settings"), _TL(""));

	m_pView		= new CTIN_View_Control	(this, pTIN, Field_Z, Field_Color, m_Settings, pRGB);

	//-----------------------------------------------------
	wxArrayString	Attributes, Styles, Shadings;

	for(int i=0; i<pTIN->Get_Field_Count(); i++)
	{
		Attributes.Add(pTIN->Get_Field_Name(i));
	}

	Styles  .Add(_TL("faces and wire"));
	Styles  .Add(_TL("faces"));
	Styles  .Add(_TL("wire"));

	Shadings.Add(_TL("none"));
	Shadings.Add(_TL("shading"));
	Shadings.Add(_TL("shading (fixed light source)"));

	//-----------------------------------------------------
	Add_Button(_TL("Close"), wxID_OK);

	m_pBtn_Prop			= Add_Button	(_TL("Advanced Settings")	, wxID_ANY);

	Add_Spacer();
	m_pField_Z			= Add_Choice	(_TL("Z Attribute")			, Attributes, m_pView->m_zField);
	m_pField_Color		= Add_Choice	(_TL("Color Attribute")		, Attributes, m_pView->m_cField);
	m_pStyle			= Add_Choice	(_TL("Draw Style")			, Styles    , m_pView->m_Style);
	m_pShading			= Add_Choice	(_TL("Shading")				, Shadings  , m_pView->m_Shading);

	Add_Spacer();
	m_pCheck_Central	= Add_CheckBox	(_TL("Central Projection")	, m_pView->m_bCentral);
	m_pCheck_Stereo		= Add_CheckBox	(_TL("Anaglyph")			, m_pView->m_bStereo);
	m_pCheck_Frame		= Add_CheckBox	(_TL("Bounding Box")		, m_pView->m_bFrame);

	m_pCheck_RGB		= pRGB == NULL ? NULL
						: Add_CheckBox	(_TL("Drape Map")			, m_pView->m_bRGB);

	Add_Spacer();
	m_pSlide_xRotate	= Add_Slider	(_TL("X-Rotation")			, m_pView->m_xRotate * M_RAD_TO_DEG, -180.0, 180.0);
	m_pSlide_yRotate	= Add_Slider	(_TL("Y-Rotation")			, m_pView->m_yRotate * M_RAD_TO_DEG, -180.0, 180.0);
	m_pSlide_zRotate	= Add_Slider	(_TL("Z-Rotation")			, m_pView->m_zRotate * M_RAD_TO_DEG, -180.0, 180.0);

	m_pSlide_Central	= Add_Slider	(_TL("Eye Distance")		, m_pView->m_dCentral, 1.0, 2000.0);

	m_pSlide_Light_Hgt	= Add_Slider	(_TL("Light Source A")		, m_pView->m_Light_Hgt, -M_PI_090, M_PI_090);
	m_pSlide_Light_Dir	= Add_Slider	(_TL("Light Source B")		, m_pView->m_Light_Dir, -M_PI_360, M_PI_360);

//	Add_Spacer();
//	m_pParameters		= Add_TextCtrl	(_TL("Function Parameters"), wxTE_MULTILINE|wxTE_READONLY);

	Add_Output(m_pView);
}

//---------------------------------------------------------
void CTIN_View_Dialog::On_Mouse_Wheel(wxMouseEvent &event)
{
	m_pView->On_Mouse_Wheel(event);
}

//---------------------------------------------------------
void CTIN_View_Dialog::On_Update_Control(wxCommandEvent &event)
{
	if( event.GetEventObject() == m_pCheck_Central )
	{
		m_pView->m_bCentral		= m_pCheck_Central	->GetValue() == 1 ? 1 : 0;
	}
	else if( event.GetEventObject() == m_pCheck_Stereo )
	{	
		m_pView->m_bStereo		= m_pCheck_Stereo	->GetValue() == 1 ? 1 : 0;
	}
	else if( event.GetEventObject() == m_pCheck_Frame )
	{	
		m_pView->m_bFrame		= m_pCheck_Frame	->GetValue() == 1 ? 1 : 0;
	}
	else if( event.GetEventObject() == m_pCheck_RGB )
	{	
		m_pView->m_bRGB			= m_pCheck_RGB		->GetValue() == 1 ? 1 : 0;
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
	else if( event.GetEventObject() == m_pSlide_Central )
	{	
		m_pView->m_dCentral		= m_pSlide_Central	->Get_Value();
	}
	else if( event.GetEventObject() == m_pSlide_Light_Dir )
	{	
		m_pView->m_Light_Dir	= m_pSlide_Light_Dir->Get_Value();
	}
	else if( event.GetEventObject() == m_pSlide_Light_Hgt )
	{	
		m_pView->m_Light_Hgt	= m_pSlide_Light_Hgt->Get_Value();
	}
	else
	{
		return;
	}

	m_pView->Update_View();
}

//---------------------------------------------------------
void CTIN_View_Dialog::On_Update_Choices(wxCommandEvent &event)
{
	if( event.GetEventObject() == m_pField_Z )
	{
		m_pView		->m_zField	= m_pField_Z	->GetSelection();

		m_pView		->Update_View();
	}

	else if( event.GetEventObject() == m_pField_Color )
	{
		m_Settings("C_RANGE")->asRange()->Set_Range(0.0, 0.0);

		m_pView		->m_cField	= m_pField_Color->GetSelection();

		m_pView		->Update_Extent();
	}

	else if( event.GetEventObject() == m_pStyle )
	{
		m_pView		->m_Style	= m_pStyle->GetSelection();

		m_pView		->Update_View();
	}

	else if( event.GetEventObject() == m_pShading )
	{
		m_pView		->m_Shading	= m_pShading->GetSelection();

		m_pView		->Update_View();
	}

	else
	{
		return;
	}
}

//---------------------------------------------------------
void CTIN_View_Dialog::On_Button(wxCommandEvent &event)
{
	if( event.GetEventObject() == m_pBtn_Prop )
	{
		if( SG_UI_Dlg_Parameters(&m_Settings, m_Settings.Get_Name()) )
		{
			m_pView		->Update_View();
		}
	}
	else
	{
		event.Skip();
	}
}

//---------------------------------------------------------
void CTIN_View_Dialog::Update_Rotation(void)
{
	double	d;

	d	= fmod(M_RAD_TO_DEG * m_pView->m_xRotate, 360.0);	if( d < -180.0 )	d	+= 360.0;	else if( d > 180.0 )	d	-= 360.0;
	m_pSlide_xRotate->Set_Value(d);

	d	= fmod(M_RAD_TO_DEG * m_pView->m_yRotate, 360.0);	if( d < -180.0 )	d	+= 360.0;	else if( d > 180.0 )	d	-= 360.0;
	m_pSlide_yRotate->Set_Value(d);

	d	= fmod(M_RAD_TO_DEG * m_pView->m_zRotate, 360.0);	if( d < -180.0 )	d	+= 360.0;	else if( d > 180.0 )	d	-= 360.0;
	m_pSlide_zRotate->Set_Value(d);

	m_pCheck_Stereo	->SetValue(m_pView->m_bStereo);
	m_pCheck_Frame	->SetValue(m_pView->m_bFrame);

	if( m_pCheck_RGB )
	{
		m_pCheck_RGB->SetValue(m_pView->m_bRGB);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

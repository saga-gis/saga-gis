
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//           Application Programming Interface           //
//                                                       //
//                  Library: SAGA_GDI                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   3d_view_panel.cpp                   //
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
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <wx/dcclient.h>
#include <wx/clipbrd.h>
#include <wx/display.h>
#include <wx/frame.h>
#include <wx/anidecod.h>
#include <wx/imaggif.h>
#include <wx/wfstream.h>
#include <wx/quantize.h>
#include <wx/filedlg.h>

#include "3d_view.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSG_3DView_Twin : public wxFrame
{
public:

	wxImage m_Image; CSG_3DView_Panel *m_pPanel { NULL };


	//-----------------------------------------------------
	CSG_3DView_Twin(wxWindow *pParent, CSG_3DView_Panel *pPanel)
		: wxFrame(pParent, wxID_ANY, wxString::Format("%s | %s", _TL("Stereo View"), _TL("Right Eye")), wxDefaultPosition, wxDefaultSize, wxCAPTION|wxSTAY_ON_TOP)
	{
		m_pPanel = pPanel;

		Sync_Position();

		Show();
	}


	//-----------------------------------------------------
	void	On_Paint		(wxPaintEvent &event)
	{
		if( m_Image.IsOk() && m_Image.GetWidth() > 0 && m_Image.GetHeight() > 0 )
		{
			wxPaintDC dc(this); dc.DrawBitmap(wxBitmap(m_Image), 0, 0, false);
		}
	}

	//-----------------------------------------------------
	bool	Sync_Size		(void)
	{
		wxSize Size(m_pPanel->GetClientSize());

		SetClientSize(Size);

		if( !m_Image.IsOk() || Size.x != m_Image.GetWidth() || Size.y != m_Image.GetHeight() )
		{
			if( !m_Image.Create(Size.x, Size.y) )
			{
				return( false );
			}
		}

		return( true );
	}

	//-----------------------------------------------------
	bool	Sync_Position	(void)
	{
		wxPoint Position = m_pPanel->GetScreenPosition();

		if( wxDisplay::GetCount() > 1 && wxDisplay::GetFromPoint(Position) != wxNOT_FOUND )
		{
			int i = wxDisplay::GetFromPoint(Position); int j = (i + 1) % wxDisplay::GetCount();

			wxRect ir(wxDisplay(i).GetGeometry()), jr(wxDisplay(j).GetGeometry());

			wxPoint p(Position.x - ir.x + jr.x, Position.y - ir.y + jr.y);

			SetPosition(p);

			return( true );
		}

		return( false );
	}

	//-----------------------------------------------------
	DECLARE_EVENT_TABLE()
};

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CSG_3DView_Twin, wxFrame)
	EVT_PAINT      (CSG_3DView_Twin::On_Paint)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	PLAY_REC_ROTATE_X	= 0,
	PLAY_REC_ROTATE_Y,
	PLAY_REC_ROTATE_Z,
	PLAY_REC_SHIFT_X,
	PLAY_REC_SHIFT_Y,
	PLAY_REC_SHIFT_Z,
	PLAY_REC_SCALE_Z,
	PLAY_REC_CENTRAL,
	PLAY_REC_STEPS
};


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
BEGIN_EVENT_TABLE(CSG_3DView_Panel, wxPanel)
	EVT_SIZE       (CSG_3DView_Panel::On_Size)
	EVT_PAINT      (CSG_3DView_Panel::On_Paint)
	EVT_KEY_DOWN   (CSG_3DView_Panel::On_Key_Down)
	EVT_LEFT_DOWN  (CSG_3DView_Panel::On_Mouse_LDown)
	EVT_LEFT_UP    (CSG_3DView_Panel::On_Mouse_LUp)
	EVT_RIGHT_DOWN (CSG_3DView_Panel::On_Mouse_RDown)
	EVT_RIGHT_UP   (CSG_3DView_Panel::On_Mouse_RUp)
	EVT_MIDDLE_DOWN(CSG_3DView_Panel::On_Mouse_MDown)
	EVT_MIDDLE_UP  (CSG_3DView_Panel::On_Mouse_MUp)
	EVT_MOTION     (CSG_3DView_Panel::On_Mouse_Motion)
	EVT_MOUSEWHEEL (CSG_3DView_Panel::On_Mouse_Wheel)
END_EVENT_TABLE()


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_3DView_Panel::CSG_3DView_Panel(wxWindow *pParent, CSG_Grid *pDrape)
	: wxPanel(pParent, wxID_ANY, wxDefaultPosition, wxDefaultSize, wxTAB_TRAVERSAL|wxSUNKEN_BORDER|wxNO_FULL_REPAINT_ON_RESIZE)
{
	m_Parameters.Create(this, _TL("3D View"));

	m_Parameters.Set_Callback_On_Parameter_Changed(_On_Parameter_Changed);

	//-----------------------------------------------------
	m_Parameters.Add_Node("", "GENERAL", _TL("General"), _TL(""));
	m_Parameters.Add_Node("", "3D_VIEW", _TL("3D View"), _TL(""));

	m_Parameters.Add_Double("3D_VIEW" , "Z_SCALE"     , _TL("Exaggeration"         ), _TL(""), m_Projector.Get_zScaling());

	m_Parameters.Add_Node  ("3D_VIEW" , "ROTATION"    , _TL("Rotation"             ), _TL(""));
	m_Parameters.Add_Double("ROTATION", "ROTATION_X"  , _TL("X"                    ), _TL(""), m_Projector.Get_xRotation(true), -360., true, 360., true);
	m_Parameters.Add_Double("ROTATION", "ROTATION_Y"  , _TL("Y"                    ), _TL(""), m_Projector.Get_yRotation(true), -360., true, 360., true);
	m_Parameters.Add_Double("ROTATION", "ROTATION_Z"  , _TL("Z"                    ), _TL(""), m_Projector.Get_zRotation(true), -360., true, 360., true);

	m_Parameters.Add_Node  ("3D_VIEW" , "SHIFT"       , _TL("Shift"                ), _TL(""));
	m_Parameters.Add_Double("SHIFT"   , "SHIFT_X"     , _TL("Left/Right"           ), _TL(""), m_Projector.Get_xShift());
	m_Parameters.Add_Double("SHIFT"   , "SHIFT_Y"     , _TL("Up/Down"              ), _TL(""), m_Projector.Get_yShift());
	m_Parameters.Add_Double("SHIFT"   , "SHIFT_Z"     , _TL("In/Out"               ), _TL(""), m_Projector.Get_zShift());

	m_Parameters.Add_Choice("3D_VIEW" , "CENTRAL"     , _TL("Projection"           ), _TL(""), CSG_String::Format("%s|%s", _TL("parallel"), _TL("central")), 1);
	m_Parameters.Add_Double("CENTRAL" , "CENTRAL_DIST", _TL("Perspectivic Distance"), _TL(""), m_Projector.Get_Central_Distance(), 0.1, true);

	m_Parameters.Add_Bool  ("3D_VIEW" , "BOX"         , _TL("Bounding Box"         ), _TL(""), true);

	m_Parameters.Add_Choice("3D_VIEW" , "NORTH"       , _TL("North Arrow"          ), _TL(""), CSG_String::Format("%s|%s|%s", _TL("no"), _TL("yes"), _TL("yes, without bounding box")), m_North);
	m_Parameters.Add_Double("NORTH"   , "NORTH_SIZE"  , _TL("Size"                 ), _TL(""), m_North_Size, 1., true);

	m_Parameters.Add_Choice("3D_VIEW" , "LABELS"      , _TL("Axis Labeling"        ), _TL(""), CSG_String::Format("%s|%s|%s", _TL("all"), _TL("horizontal"), _TL("none")), m_Labels);
	m_Parameters.Add_Int   ("LABELS"  , "LABEL_RES"   , _TL("Resolution"           ), _TL(""), m_Label_Res, 20, true, 1000, true);
	m_Parameters.Add_Double("LABELS"  , "LABEL_SCALE" , _TL("Size"                 ), _TL(""), m_Label_Scale, 0.1, true, 10., true);

	m_Parameters.Add_Color ("3D_VIEW" , "BGCOLOR"     , _TL("Background Color"     ), _TL(""), SG_COLOR_WHITE);

	m_Parameters.Add_Choice("3D_VIEW" , "STEREO"      , _TL("Stereo View"          ), _TL(""), CSG_String::Format("%s|%s|%s", _TL("off"), _TL("anaglyph"), _TL("twin window for right eye")));
	m_Parameters.Add_Double("STEREO"  , "STEREO_DIST" , _TL("Eye Distance [Degree]"), _TL(""), m_dStereo, 0., true, 180., true);

	//-----------------------------------------------------
	if( (m_pDrape = pDrape) != NULL )
	{
		m_Parameters.Add_Node("", "MAP", _TL("Map Draping"), _TL(""));
		m_Parameters.Add_Bool("MAP", "MAP_DRAPE", _TL("Map Draping"), _TL(""), true);
		m_Parameters.Add_Choice("MAP", "MAP_DRAPE_MODE", _TL("Map Drape Resampling"), _TL(""),
			CSG_String::Format("%s|%s|%s|%s",
				_TL("Nearest Neighbour"),
				_TL("Bilinear Interpolation"),
				_TL("Bicubic Spline Interpolation"),
				_TL("B-Spline Interpolation")
			), 0
		);
	}

	//-----------------------------------------------------
	m_Parameters.Add_Node("",
		"PLAYER"    , _TL("Sequencer"),
		_TL("")
	);

	m_pPlay	= m_Parameters.Add_FixedTable("PLAYER",
		"PLAY"      , _TL("View Positions"),
		_TL("")
	)->asTable();

	m_pPlay->Add_Field(_TL("Rotate X"        ), SG_DATATYPE_Double);
	m_pPlay->Add_Field(_TL("Rotate Y"        ), SG_DATATYPE_Double);
	m_pPlay->Add_Field(_TL("Rotate Z"        ), SG_DATATYPE_Double);
	m_pPlay->Add_Field(_TL("Shift X"         ), SG_DATATYPE_Double);
	m_pPlay->Add_Field(_TL("Shift Y"         ), SG_DATATYPE_Double);
	m_pPlay->Add_Field(_TL("Shift Z"         ), SG_DATATYPE_Double);
	m_pPlay->Add_Field(_TL("Exaggeration Z"  ), SG_DATATYPE_Double);
	m_pPlay->Add_Field(_TL("Central Distance"), SG_DATATYPE_Double);
	m_pPlay->Add_Field(_TL("Steps to Next"   ), SG_DATATYPE_Int   );

	m_Parameters.Add_Bool("PLAYER",
		"PLAY_FIRST", _TL("Proceed to first View Position"),
		_TL("If played once, the end of sequence will proceed to first view position. Also applies when frames will be stored to animated GIF file."),
		true
	);

	m_Parameters.Add_FilePath("PLAYER",
		"PLAY_FILE" , _TL("Image File"),
		_TL("file path, name and type used to save frames to image files"),
		CSG_String::Format(
			"%s (*.png)"                "|*.png|"
			"%s (*.jpg, *.jif, *.jpeg)" "|*.jpg;*.jif;*.jpeg|"
			"%s (*.tif, *.tiff)"        "|*.tif;*.tiff|"
			"%s (*.gif)"                "|*.gif|"
			"%s (*.bmp)"                "|*.bmp|"
			"%s (*.pcx)"                "|*.pcx",
			_TL("Portable Network Graphics"  ),
			_TL("JPEG - JFIF Compliant"      ),
			_TL("Tagged Image File Format"   ),
			_TL("Graphics Interchange Format"),
			_TL("Windows or OS/2 Bitmap"     ),
			_TL("Zsoft Paintbrush"           )
		), NULL, true
	);

	m_Parameters.Add_Int("PLAY_FILE",
		"PLAY_DELAY", _TL("Delay"),
		_TL("Delay, in milliseconds, to wait between each frame. Used when storing animated GIF."),
		100, 0, true
	);

	m_Play_State = SG_3DVIEW_PLAY_STOP;
}

//---------------------------------------------------------
CSG_3DView_Panel::~CSG_3DView_Panel(void)
{
	if( m_pTwin )
	{
		delete(m_pTwin);
	}
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_3DView_Panel::Update_Parameters(bool bSave)
{
	if( bSave )
	{
		m_Parameters["ROTATION_X"  ].Set_Value(m_Projector.Get_xRotation() * M_RAD_TO_DEG);
		m_Parameters["ROTATION_Y"  ].Set_Value(m_Projector.Get_yRotation() * M_RAD_TO_DEG);
		m_Parameters["ROTATION_Z"  ].Set_Value(m_Projector.Get_zRotation() * M_RAD_TO_DEG);

		m_Parameters["SHIFT_X"     ].Set_Value(m_Projector.Get_xShift());
		m_Parameters["SHIFT_Y"     ].Set_Value(m_Projector.Get_yShift());
		m_Parameters["SHIFT_Z"     ].Set_Value(m_Projector.Get_zShift());

		m_Parameters["Z_SCALE"     ].Set_Value(m_Projector.Get_zScaling());

		m_Parameters["CENTRAL"     ].Set_Value(m_Projector.is_Central() ? 1 : 0);
		m_Parameters["CENTRAL_DIST"].Set_Value(m_Projector.Get_Central_Distance());

		//-------------------------------------------------
		m_Parameters["STEREO"      ].Set_Value(m_Stereo     );
		m_Parameters["STEREO_DIST" ].Set_Value(m_dStereo    );

		m_Parameters["BGCOLOR"     ].Set_Value(m_bgColor    );
		m_Parameters["BOX"         ].Set_Value(m_bBox       );

		m_Parameters["NORTH"       ].Set_Value(m_North      );
		m_Parameters["NORTH_SIZE"  ].Set_Value(m_North_Size );

		m_Parameters["LABELS"      ].Set_Value(m_Labels     );
		m_Parameters["LABEL_RES"   ].Set_Value(m_Label_Res  );
		m_Parameters["LABEL_SCALE" ].Set_Value(m_Label_Scale);

		if( m_Parameters("MAP_DRAPE_MODE") )
		{
			m_Parameters["MAP_DRAPE_MODE"].Set_Value((int)m_Drape_Mode);
		}
	}

	//-----------------------------------------------------
	else
	{
		m_Projector.Set_Rotation(
			m_Parameters["ROTATION_X"].asDouble() * M_DEG_TO_RAD,
			m_Parameters["ROTATION_Y"].asDouble() * M_DEG_TO_RAD,
			m_Parameters["ROTATION_Z"].asDouble() * M_DEG_TO_RAD
		);

		m_Projector.Set_Shift(
			m_Parameters["SHIFT_X"].asDouble(),
			m_Parameters["SHIFT_Y"].asDouble(),
			m_Parameters["SHIFT_Z"].asDouble()
		);

		m_Projector.Set_zScaling        (m_Parameters["Z_SCALE"     ].asDouble());

		m_Projector.do_Central          (m_Parameters["CENTRAL"     ].asBool  ());
		m_Projector.Set_Central_Distance(m_Parameters["CENTRAL_DIST"].asDouble());

		//-------------------------------------------------
		m_Stereo      = m_Parameters["STEREO"     ].asInt   ();
		m_dStereo     = m_Parameters["STEREO_DIST"].asDouble();

		m_bgColor     = m_Parameters["BGCOLOR"    ].asColor ();
		m_bBox        = m_Parameters["BOX"        ].asBool  ();

		m_North       = m_Parameters["NORTH"      ].asInt   ();
		m_North_Size  = m_Parameters["NORTH_SIZE" ].asDouble();

		m_Labels      = m_Parameters["LABELS"     ].asInt   ();
		m_Label_Res   = m_Parameters["LABEL_RES"  ].asInt   ();
		m_Label_Scale = m_Parameters["LABEL_SCALE"].asDouble();

		switch( m_Parameters("MAP_DRAPE_MODE") ? m_Parameters["MAP_DRAPE_MODE"].asInt() : 0 )
		{
		default: m_Drape_Mode = GRID_RESAMPLING_NearestNeighbour; break;
		case  1: m_Drape_Mode = GRID_RESAMPLING_Bilinear        ; break;
		case  2: m_Drape_Mode = GRID_RESAMPLING_BicubicSpline   ; break;
		case  3: m_Drape_Mode = GRID_RESAMPLING_BSpline         ; break;
		}
	}

	return( true );
}

//---------------------------------------------------------
int CSG_3DView_Panel::_On_Parameter_Changed(CSG_Parameter *pParameter, int Flags)
{
	CSG_Parameters *pParameters = pParameter ? pParameter->Get_Parameters() : NULL;

	if( pParameters )
	{
		CSG_3DView_Panel *pPanel = (CSG_3DView_Panel *)pParameters->Get_Owner();

		if( Flags & PARAMETER_CHECK_VALUES )
		{
			pPanel->On_Parameter_Changed(pParameters, pParameter);
		}

		if( Flags & PARAMETER_CHECK_ENABLE )
		{
			pPanel->On_Parameters_Enable(pParameters, pParameter);
		}

		return( 1 );
	}

	return( 0 );
}

//---------------------------------------------------------
int CSG_3DView_Panel::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( 1 );
}

//---------------------------------------------------------
int CSG_3DView_Panel::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("CENTRAL") )
	{
		pParameters->Set_Enabled("CENTRAL_DIST", pParameter->asInt() == 1);
	}

	if( pParameter->Cmp_Identifier("MAP_DRAPE") )
	{
		pParameters->Set_Enabled("MAP_DRAPE_MODE", pParameter->asBool());
	}

	if( pParameter->Cmp_Identifier("NORTH") )
	{
		pParameter->Set_Children_Enabled(pParameter->asBool());
	}

	if( pParameter->Cmp_Identifier("LABELS") )
	{
		pParameter->Set_Children_Enabled(pParameter->asInt() != 2);
	}

	if( pParameter->Cmp_Identifier("STEREO") )
	{
		pParameters->Set_Enabled("STEREO_DIST", pParameter->asInt() != 0);
	}

	if( pParameter->Cmp_Identifier("PLAY_FILE") )
	{
		pParameters->Set_Enabled("PLAY_DELAY", SG_File_Cmp_Extension(pParameter->asString(), "gif"));
	}

	return( 1 );
}

//---------------------------------------------------------
bool CSG_3DView_Panel::Parameter_Value_Toggle(const CSG_String &ID, bool bUpdate)
{
	CSG_Parameter *pParameter = m_Parameters(ID);

	if( !pParameter )
	{
		return( false );
	}

	Update_Parameters(true);

	switch( pParameter->Get_Type() )
	{
	default:
		return( false );

	case PARAMETER_TYPE_Bool  :
		pParameter->Set_Value(pParameter->asBool() ? 0 : 1);
		break;

	case PARAMETER_TYPE_Choice:
		pParameter->Set_Value((pParameter->asInt() + 1) % pParameter->asChoice()->Get_Count());
		break;
	}

	Update_Parameters(false);

	if( bUpdate )
	{
		Update_View(); Update_Parent();
	}

	return( false );
}

//---------------------------------------------------------
bool CSG_3DView_Panel::Parameter_Value_Add(const CSG_String &ID, double Value, bool bUpdate)
{
	CSG_Parameter *pParameter = m_Parameters(ID);

	if( !pParameter )
	{
		return( false );
	}

	Update_Parameters(true);

	switch( pParameter->Get_Type() )
	{
	default:
		return( false );

	case PARAMETER_TYPE_Int: case PARAMETER_TYPE_Double:
		pParameter->Set_Value(pParameter->asDouble() + Value);
		break;
	}

	Update_Parameters(false);

	if( bUpdate )
	{
		Update_View(); Update_Parent();
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_3DView_Panel::Update_Statistics(void)
{}

//---------------------------------------------------------
void CSG_3DView_Panel::Update_Parent(void)
{
	((CSG_3DView_Dialog *)GetParent())->Update_Controls();
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Table CSG_3DView_Panel::Get_Shortcuts(void)
{
	CSG_Table Shortcuts;

	Shortcuts.Add_Field("KEY", SG_DATATYPE_String);
	Shortcuts.Add_Field("CMD", SG_DATATYPE_String);

	#define ADD_SHORTCUT(KEY, CMD) { CSG_Table_Record &r = *Shortcuts.Add_Record(); r.Set_Value(0, KEY); r.Set_Value(1, CMD); }

	ADD_SHORTCUT("F1"       , _TL("Decrease Exaggeration"));
	ADD_SHORTCUT("F2"       , _TL("Increase Exaggeration"));

	ADD_SHORTCUT("1"        , _TL("Rotate Left"   ));
	ADD_SHORTCUT("2"        , _TL("Rotate Right"  ));
	ADD_SHORTCUT("3"        , _TL("Rotate Up"     ));
	ADD_SHORTCUT("4"        , _TL("Rotate Down"   ));
	ADD_SHORTCUT("5"        , _TL("Roll Left"     ));
	ADD_SHORTCUT("6"        , _TL("Roll Right"    ));

	ADD_SHORTCUT("Insert"   , _TL("Shift Left"    ));
	ADD_SHORTCUT("Delete"   , _TL("Shift Right"   ));
	ADD_SHORTCUT("Home"     , _TL("Shift Up"      ));
	ADD_SHORTCUT("End"      , _TL("Shift Down"    ));
	ADD_SHORTCUT("Page Up"  , _TL("Shift Forward" ));
	ADD_SHORTCUT("Page Down", _TL("Shift Backward"));

	ADD_SHORTCUT("B"        , _TL("Bounding Box"  ));
	ADD_SHORTCUT("N"        , _TL("North Arrow"   ));
	ADD_SHORTCUT("L"        , _TL("Axis Labeling" ));

	ADD_SHORTCUT("C"        , _TL("Parallel/Central Projection"));
	ADD_SHORTCUT("7"        , _TL("Increase Perspective Distance for Central Projection"));
	ADD_SHORTCUT("8"        , _TL("Decrease Perspective Distance for Central Projection"));

	ADD_SHORTCUT("T"        , _TL("Toggle Stereo View Twin Window"));
	ADD_SHORTCUT("U"        , _TL("Synchronize Stereo View Twin Window Position"));
	ADD_SHORTCUT("A"        , _TL("Toggle Anaglyph View"));
	ADD_SHORTCUT("9"        , _TL("Decrease Eye Distance Angle for Stereo View"));
	ADD_SHORTCUT("0"        , _TL("Increase Eye Distance Angle for Stereo View"));

	ADD_SHORTCUT("Ctrl+C"   , _TL("Copy to Clipboard"));

	ADD_SHORTCUT("Ctrl+A"   , CSG_String::Format("%s, %s", _TL("Sequencer"), _TL("Add Position"          )));
	ADD_SHORTCUT("Ctrl+D"   , CSG_String::Format("%s, %s", _TL("Sequencer"), _TL("Delete Last Position"  )));
	ADD_SHORTCUT("Ctrl+X"   , CSG_String::Format("%s, %s", _TL("Sequencer"), _TL("Delete All Positions"  )));
	ADD_SHORTCUT("Ctrl+P"   , CSG_String::Format("%s, %s", _TL("Sequencer"), _TL("Play Once"             )));
	ADD_SHORTCUT("Ctrl+L"   , CSG_String::Format("%s, %s", _TL("Sequencer"), _TL("Play Loop"             )));
	ADD_SHORTCUT("Ctrl+S"   , CSG_String::Format("%s, %s", _TL("Sequencer"), _TL("Play and Save to Image")));
	ADD_SHORTCUT("Escape"   , CSG_String::Format("%s, %s", _TL("Sequencer"), _TL("Stop Playing"          )));

	return( Shortcuts );
}

//---------------------------------------------------------
CSG_String CSG_3DView_Panel::Get_Usage(void)
{
	return( Get_Usage(Get_Shortcuts()) );
}

//---------------------------------------------------------
CSG_String CSG_3DView_Panel::Get_Usage(const CSG_Table &Shortcuts)
{
	CSG_String s;

	s += CSG_String::Format("<hr><h4>%s</h4><table>", _TL("Mouse"));

	s += CSG_String::Format("<tr><td>%s</td><td>%s</td></tr>", _TL("Left Button"  ), _TL("Rotate Left/Right and Up/Down"));
	s += CSG_String::Format("<tr><td>%s</td><td>%s</td></tr>", _TL("Right Button" ), _TL("Shift Left/Right and Up/Down"));
	s += CSG_String::Format("<tr><td>%s</td><td>%s</td></tr>", _TL("Middle Button"), _TL("Shift Forward/Backward (Up/Down) and Perspective Distance for Central Projection (Left/Right)"));
	s += CSG_String::Format("<tr><td>%s</td><td>%s</td></tr>", _TL("Wheel"        ), _TL("Shift Forward/Backward"));

	s += "</table>";

	s += CSG_String::Format("<hr><h4>%s</h4><table>", _TL("Keyboard"));

	CSG_Index Index; Shortcuts.Set_Index(Index, 0);

	for(int i=0; i<Shortcuts.Get_Count(); i++)
	{
		s += CSG_String::Format("<tr><td>%s</td><td>%s</td></tr>", Shortcuts[Index[i]].asString(0), Shortcuts[Index[i]].asString(1));
	}

	s += "</table>";

	return( s );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_3DView_Panel::On_Key_Down(wxKeyEvent &event)
{
	//-----------------------------------------------------
	if( event.ControlDown() )
	{
		switch( event.GetKeyCode() )
		{
		default : event.Skip      (); return;

		case 'A': Play_Pos_Add    (); return;
		case 'D': Play_Pos_Del    (); return;
		case 'X': Play_Pos_Clr    (); return;

		case 'P': Play_Once       (); return;
		case 'L': Play_Loop       (); return;
		case 'S': Play_Save       (); return;

		case 'C': Save_toClipboard(); return;
		}

		event.Skip();
	}

	//-----------------------------------------------------
	else
	{
		switch( event.GetKeyCode() )
		{
		default: event.Skip(); return;

		case WXK_ESCAPE:
			if( m_Play_State )
			{
				Play_Stop();
			}
			return;

		case '1'         : m_Projector.Inc_zRotation( 4., true); break;
		case '2'         : m_Projector.Inc_zRotation(-4., true); break;
		case '3'         : m_Projector.Inc_xRotation( 4., true); break;
		case '4'         : m_Projector.Inc_xRotation(-4., true); break;
		case '5'         : m_Projector.Inc_yRotation( 4., true); break;
		case '6'         : m_Projector.Inc_yRotation(-4., true); break;

		case WXK_INSERT  : m_Projector.Inc_xShift(-0.1); break;
		case WXK_DELETE  : m_Projector.Inc_xShift( 0.1); break;
		case WXK_HOME    : m_Projector.Inc_yShift(-0.1); break;
		case WXK_END     : m_Projector.Inc_yShift( 0.1); break;
		case WXK_PAGEUP  : m_Projector.Inc_zShift(-0.1); break;
		case WXK_PAGEDOWN: m_Projector.Inc_zShift( 0.1); break;

		case WXK_F1      : m_Projector.Inc_zScaling(-0.5); break;
		case WXK_F2      : m_Projector.Inc_zScaling( 0.5); break;

		case 'C'         : m_Projector.do_Central(!m_Projector.is_Central()); break;
		case '7'         : m_Projector.Inc_Central_Distance( 0.1); break;
		case '8'         : m_Projector.Inc_Central_Distance(-0.1); break;

		case '9'         : m_dStereo -= 0.5; break;
		case '0'         : m_dStereo += 0.5; break;

		case 'A'         : Set_Stereo_Mode(m_Stereo != 1 ? 1 : 0); break;
		case 'T'         : Set_Stereo_Mode(m_Stereo != 2 ? 2 : 0); break;
		case 'U'         : if( m_pTwin ) { m_pTwin->Sync_Position(); } break;

		case 'B'         : m_bBox     = !m_bBox           ; break;

		case 'N'         : m_North    = (m_North  + 1) % 3; break;
		case 'L'         : m_Labels   = (m_Labels + 1) % 3; break;
		}

		Update_Parameters(true); Update_Parent(); Update_View();
	}
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GET_MOUSE_X_RELDIFF	((double)(m_Down_Screen.x - event.GetX()) / (double)GetClientSize().x)
#define GET_MOUSE_Y_RELDIFF	((double)(m_Down_Screen.y - event.GetY()) / (double)GetClientSize().y)

//---------------------------------------------------------
void CSG_3DView_Panel::On_Mouse_LDown(wxMouseEvent &event)
{
	SetFocus();

	m_Down_Screen  = event.GetPosition();
	m_Down_Value.x = m_Projector.Get_zRotation();
	m_Down_Value.y = m_Projector.Get_xRotation();

	CaptureMouse();
}

void CSG_3DView_Panel::On_Mouse_LUp(wxMouseEvent &event)
{
	if( HasCapture() )
	{
		ReleaseMouse();
	}

	if( m_Down_Screen.x != event.GetX() || m_Down_Screen.y != event.GetY() )
	{
		m_Projector.Set_zRotation(m_Down_Value.x + GET_MOUSE_X_RELDIFF * M_PI_180);
		m_Projector.Set_xRotation(m_Down_Value.y + GET_MOUSE_Y_RELDIFF * M_PI_180);

		Update_View(); Update_Parent();
	}
}

//---------------------------------------------------------
void CSG_3DView_Panel::On_Mouse_RDown(wxMouseEvent &event)
{
	SetFocus();

	m_Down_Screen  = event.GetPosition();
	m_Down_Value.x = m_Projector.Get_xShift();
	m_Down_Value.y = m_Projector.Get_yShift();

	CaptureMouse();
}

void CSG_3DView_Panel::On_Mouse_RUp(wxMouseEvent &event)
{
	if( HasCapture() )
	{
		ReleaseMouse();
	}

	if( m_Down_Screen.x != event.GetX() || m_Down_Screen.y != event.GetY() )
	{
		m_Projector.Set_xShift(m_Down_Value.x - GET_MOUSE_X_RELDIFF);
		m_Projector.Set_yShift(m_Down_Value.y - GET_MOUSE_Y_RELDIFF);

		Update_View(); Update_Parent();
	}
}

//---------------------------------------------------------
void CSG_3DView_Panel::On_Mouse_MDown(wxMouseEvent &event)
{
	SetFocus();

	m_Down_Screen  = event.GetPosition();
	m_Down_Value.x = m_Projector.Get_Central_Distance();
	m_Down_Value.y = m_Projector.Get_zShift();

	CaptureMouse();
}

void CSG_3DView_Panel::On_Mouse_MUp(wxMouseEvent &event)
{
	if( HasCapture() )
	{
		ReleaseMouse();
	}

	if( m_Down_Screen.x != event.GetX() || m_Down_Screen.y != event.GetY() )
	{
		m_Projector.Set_Central_Distance(m_Down_Value.x + GET_MOUSE_X_RELDIFF);
		m_Projector.Set_zShift          (m_Down_Value.y + GET_MOUSE_Y_RELDIFF);

		Update_View(); Update_Parent();
	}
}

//---------------------------------------------------------
void CSG_3DView_Panel::On_Mouse_Motion(wxMouseEvent &event)
{
	if( HasCapture() && event.Dragging() )
	{
		if( event.LeftIsDown() )
		{
			m_Projector.Set_zRotation       (m_Down_Value.x + GET_MOUSE_X_RELDIFF * M_PI_180);
			m_Projector.Set_xRotation       (m_Down_Value.y + GET_MOUSE_Y_RELDIFF * M_PI_180);
		}
		else if( event.RightIsDown() )
		{
			m_Projector.Set_xShift          (m_Down_Value.x - GET_MOUSE_X_RELDIFF);
			m_Projector.Set_yShift          (m_Down_Value.y - GET_MOUSE_Y_RELDIFF);
		}
		else if( event.MiddleIsDown() )
		{
			m_Projector.Set_Central_Distance(m_Down_Value.x + GET_MOUSE_X_RELDIFF);
			m_Projector.Set_zShift          (m_Down_Value.y + GET_MOUSE_Y_RELDIFF);
		}
		else
		{
			return;
		}

		Update_View(); Update_Parent();
	}
}

//---------------------------------------------------------
void CSG_3DView_Panel::On_Mouse_Wheel(wxMouseEvent &event)
{
	m_Mouse_Wheel_Accumulator += event.GetWheelRotation();

	if( m_Mouse_Wheel_Accumulator >= event.GetWheelDelta() )
	{
		m_Projector.Set_zShift(m_Projector.Get_zShift()	- event.GetWheelDelta() * 0.001);

		Update_View(); Update_Parent();

		m_Mouse_Wheel_Accumulator = 0;
	}

	if( m_Mouse_Wheel_Accumulator <= -event.GetWheelDelta() )
	{
		m_Projector.Set_zShift(m_Projector.Get_zShift()	+ event.GetWheelDelta() * 0.001);

		Update_View(); Update_Parent();

		m_Mouse_Wheel_Accumulator = 0;
	}
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_3DView_Panel::On_Before_Draw(void)
{
	if( m_Play_State == SG_3DVIEW_PLAY_STOP )
	{
		m_Projector.Set_zScaling(m_Projector.Get_xScaling() * m_Parameters("Z_SCALE")->asDouble());
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_3DView_Panel::On_Size(wxSizeEvent &event)
{
	Update_View();

	event.Skip();
}

//---------------------------------------------------------
void CSG_3DView_Panel::On_Paint(wxPaintEvent &WXUNUSED(event))
{
	if( m_Image.IsOk() && m_Image.GetWidth() > 0 && m_Image.GetHeight() > 0 )
	{
		wxPaintDC dc(this); dc.DrawBitmap(wxBitmap(m_Image), 0, 0, false);
	}
}

//---------------------------------------------------------
bool CSG_3DView_Panel::Update_View(bool bStatistics)
{
	if( bStatistics )
	{
		Update_Statistics();
	}

	//-----------------------------------------------------
	if( m_Play_State == SG_3DVIEW_PLAY_STOP )
	{
	}

	//-----------------------------------------------------
	wxSize Size = GetClientSize();

	if( Size.x < 1 || Size.y < 1 )
	{
		return( false );
	}

	if( !m_Image.IsOk() || Size.x != m_Image.GetWidth() || Size.y != m_Image.GetHeight() )
	{
		if( !m_Image.Create(Size.x, Size.y) )
		{
			return( false );
		}
	}

	Set_Image(m_Image);

	Set_Stereo_Mode(m_Stereo);

	if( m_pTwin && m_pTwin->Sync_Size() )
	{
		Set_Image_Twin(m_pTwin->m_Image);
	}

	//-----------------------------------------------------
	CSG_Grid *pDrape = m_pDrape; if( m_pDrape && !m_Parameters("MAP_DRAPE")->asBool() ) m_pDrape = NULL;

	if( Draw() )
	{
		Refresh(false); Update();

		if( m_pTwin && m_pTwin->m_Image.IsOk() )
		{
			m_pTwin->Refresh(false); m_pTwin->Update();
		}
	}

	m_pDrape = pDrape;

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_3DView_Panel::Set_Stereo_Mode(int Mode)
{
	if( Mode != 2 && m_pTwin != NULL )
	{
		delete(m_pTwin); m_pTwin = NULL;
	}

	if( Mode == 2 && m_pTwin == NULL )
	{
		m_pTwin = new CSG_3DView_Twin((wxWindow *)SG_UI_Get_Window_Main(), this);
	}

	m_Stereo = Mode;

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSG_3DView_Panel::Save_asImage(const CSG_String &FileName)
{
	return( m_Image.SaveFile(FileName.c_str()) );
}

//---------------------------------------------------------
bool CSG_3DView_Panel::Save_toClipboard(void)
{
	if( m_Image.IsOk() && m_Image.GetWidth() > 0 && m_Image.GetHeight() > 0 && wxTheClipboard->Open() )
	{
		wxBitmapDataObject *pBMP = new wxBitmapDataObject;
		pBMP->SetBitmap(m_Image);
		wxTheClipboard->SetData(pBMP);
		wxTheClipboard->Close();

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CSG_3DView_Panel::Play_Pos_Add(void)
{
	if( m_Play_State == SG_3DVIEW_PLAY_STOP )	
	{
		CSG_Table_Record *pRecord = m_pPlay->Add_Record();

		pRecord->Set_Value(PLAY_REC_ROTATE_X, m_Projector.Get_xRotation       ());
		pRecord->Set_Value(PLAY_REC_ROTATE_Y, m_Projector.Get_yRotation       ());
		pRecord->Set_Value(PLAY_REC_ROTATE_Z, m_Projector.Get_zRotation       ());
		pRecord->Set_Value(PLAY_REC_SHIFT_X , m_Projector.Get_xShift          ());
		pRecord->Set_Value(PLAY_REC_SHIFT_Y , m_Projector.Get_yShift          ());
		pRecord->Set_Value(PLAY_REC_SHIFT_Z , m_Projector.Get_zShift          ());
		pRecord->Set_Value(PLAY_REC_SCALE_Z , m_Projector.Get_zScaling        ());
		pRecord->Set_Value(PLAY_REC_CENTRAL , m_Projector.Get_Central_Distance());
		pRecord->Set_Value(PLAY_REC_STEPS   , 10);
	}
}

//---------------------------------------------------------
void CSG_3DView_Panel::Play_Pos_Del(void)
{
	if( m_Play_State == SG_3DVIEW_PLAY_STOP )
	{
		m_pPlay->Del_Record(m_pPlay->Get_Count() - 1);
	}
}

//---------------------------------------------------------
void CSG_3DView_Panel::Play_Pos_Clr(void)
{
	if( m_Play_State == SG_3DVIEW_PLAY_STOP )
	{
		m_pPlay->Del_Records();
	}
}

//---------------------------------------------------------
void CSG_3DView_Panel::Play_Once(void)
{
	if( m_Play_State == SG_3DVIEW_PLAY_STOP )
	{
		m_Play_State = SG_3DVIEW_PLAY_RUN_ONCE;

		_Play();

		m_Play_State = SG_3DVIEW_PLAY_STOP;
	}
	else if( m_Play_State == SG_3DVIEW_PLAY_RUN_ONCE )
	{
		Play_Stop();
	}
}

//---------------------------------------------------------
void CSG_3DView_Panel::Play_Loop(void)
{
	if( m_Play_State == SG_3DVIEW_PLAY_STOP )
	{
		m_Play_State = SG_3DVIEW_PLAY_RUN_LOOP;

		while( _Play() );
	}
	else if( m_Play_State == SG_3DVIEW_PLAY_RUN_LOOP )
	{
		Play_Stop();
	}
}

//---------------------------------------------------------
void CSG_3DView_Panel::Play_Save(void)
{
	if( m_Play_State == SG_3DVIEW_PLAY_STOP )
	{
		m_Play_State = SG_3DVIEW_PLAY_RUN_SAVE;

		_Play();

		m_Play_State = SG_3DVIEW_PLAY_STOP;
	}
	else if( m_Play_State == SG_3DVIEW_PLAY_RUN_SAVE )
	{
		Play_Stop();
	}
}

//---------------------------------------------------------
void CSG_3DView_Panel::Play_Stop(void)
{
	if( m_Play_State != SG_3DVIEW_PLAY_STOP )
	{
		m_Play_State = SG_3DVIEW_PLAY_STOP;
	}
}

//---------------------------------------------------------
#define PLAYER_READ(iRecord)	{ CSG_Table_Record *pRecord = m_pPlay->Get_Record(iRecord); if( pRecord ) {\
	for(int i=0; i<=PLAY_REC_STEPS; i++)\
	{\
		Position[i][0] = Position[i][1];\
		Position[i][1] = pRecord->asDouble(i);\
	}\
}}

//---------------------------------------------------------
double	SG_Get_Short_Angle	(double Angle)
{
	Angle = fmod(Angle, M_PI_360);

	if( fabs(Angle) > M_PI_180 )
	{
		Angle += Angle < 0. ? M_PI_360 : -M_PI_360;
	}

	return( Angle );
}


//---------------------------------------------------------
#define PLAYER_GET_VAL(Value)	(Value[0] + d * (Value[1] - Value[0]))
#define PLAYER_GET_ROT(Value)	(Value[0] + d * SG_Get_Short_Angle(Value[1] - Value[0]))

//---------------------------------------------------------
bool CSG_3DView_Panel::_Play(void)
{
	if( m_Play_State == SG_3DVIEW_PLAY_STOP )
	{
		return( false );
	}

	if( m_pPlay->Get_Count() < 2 )
	{
		m_Play_State = SG_3DVIEW_PLAY_STOP;

		return( false );
	}

	//-----------------------------------------------------
	CSG_String File;

	if( m_Play_State == SG_3DVIEW_PLAY_RUN_SAVE )
	{
		File = m_Parameters["PLAY_FILE"].asString();

		if( File.is_Empty() )
		{
			wxFileDialog dlg(GetParent(),
				m_Parameters["PLAY_FILE"].Get_Name(), "", "",
				m_Parameters["PLAY_FILE"].asFilePath()->Get_Filter(), wxFD_SAVE|wxFD_OVERWRITE_PROMPT
			);

			if( dlg.ShowModal() == wxID_OK )
			{
				File = dlg.GetPath().wx_str();
			}
		}

		if( File.is_Empty() )
		{
			SG_UI_Dlg_Error(_TL("invalid image file path"), _TL("3D View Sequencer"));

			m_Play_State = SG_3DVIEW_PLAY_STOP;

			return( false );
		}
	}

	bool bAnimation = m_Play_State == SG_3DVIEW_PLAY_RUN_SAVE && SG_File_Cmp_Extension(File, "gif");

	wxImageArray Images;

	//-----------------------------------------------------
	CSG_Matrix Position(2, 9);

	PLAYER_READ(0);

	int nPositions = (int)m_pPlay->Get_Count() + (m_Play_State == SG_3DVIEW_PLAY_RUN_LOOP || m_Parameters["PLAY_FIRST"].asBool() ? 1 : 0);

	for(int iRecord=1, iFrame=0; iRecord<nPositions && m_Play_State!=SG_3DVIEW_PLAY_STOP; iRecord++)
	{
		PLAYER_READ(iRecord % m_pPlay->Get_Count());

		for(int iStep=0; iStep<(int)Position[PLAY_REC_STEPS][0] && m_Play_State!=SG_3DVIEW_PLAY_STOP; iStep++, iFrame++)
		{
			double d = iStep / Position[PLAY_REC_STEPS][0];

			m_Projector.Set_xRotation       (PLAYER_GET_ROT(Position[PLAY_REC_ROTATE_X]));
			m_Projector.Set_yRotation       (PLAYER_GET_ROT(Position[PLAY_REC_ROTATE_Y]));
			m_Projector.Set_zRotation       (PLAYER_GET_ROT(Position[PLAY_REC_ROTATE_Z]));
			m_Projector.Set_xShift          (PLAYER_GET_VAL(Position[PLAY_REC_SHIFT_X ]));
			m_Projector.Set_yShift          (PLAYER_GET_VAL(Position[PLAY_REC_SHIFT_Y ]));
			m_Projector.Set_zShift          (PLAYER_GET_VAL(Position[PLAY_REC_SHIFT_Z ]));
			m_Projector.Set_zScaling        (PLAYER_GET_VAL(Position[PLAY_REC_SCALE_Z ]));
			m_Projector.Set_Central_Distance(PLAYER_GET_VAL(Position[PLAY_REC_CENTRAL ]));

			Update_View();

			if( m_Play_State == SG_3DVIEW_PLAY_RUN_SAVE )
			{
				if( bAnimation )
				{
					wxImage *pImage = new wxImage;

					if( m_Image.HasAlpha() )
					{
						m_Image.ConvertAlphaToMask();
					}

					wxQuantize::Quantize(m_Image, *pImage);

					Images.Add(pImage);
				}
				else
				{
					m_Image.SaveFile(SG_File_Make_Path(
						SG_File_Get_Path     (File),
						SG_File_Get_Name     (File, false) + CSG_String::Format("%03d", iFrame),
						SG_File_Get_Extension(File)
					).c_str());
				}
			}

			SG_UI_Process_Get_Okay();
		}
	}

	//-----------------------------------------------------
	if( bAnimation && Images.Count() > 0 )
	{
		wxFileOutputStream Stream(File.c_str()); wxGIFHandler Handler;

		SG_UI_Process_Set_Busy(true);

		bool bResult = Stream.IsOk() && Handler.SaveAnimation(Images, &Stream, true, m_Parameters["PLAY_DELAY"].asInt());

		SG_UI_Process_Set_Busy(false);

		if( !bResult )
		{
			SG_UI_Dlg_Error(CSG_String::Format("%s\n\"%s\"", _TL("failed to create animation file!"), File.c_str()), _TL("3D View"));
		}
	}

	//-----------------------------------------------------
	if( m_Play_State != SG_3DVIEW_PLAY_RUN_LOOP )
	{
		m_Play_State = SG_3DVIEW_PLAY_STOP;

		Update_Parent(); Update_View();
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

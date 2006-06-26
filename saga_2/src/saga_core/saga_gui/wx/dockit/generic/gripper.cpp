/////////////////////////////////////////////////////////////////////////////
// Name:        gripper.cpp
// Purpose:     wxGripperWindow implementation.
// Author:      Mark McCormack
// Modified by:
// Created:     23/02/04
// RCS-ID:      $Id: gripper.cpp,v 1.1.1.1 2006-06-26 15:43:34 oconrad Exp $
// Copyright:   (c) 2004 Mark McCormack
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include <wx/gripper.h>
#include <wx/dcclient.h>

// ----------------------------------------------------------------------------
// wxGripWindow implementation
// ----------------------------------------------------------------------------

IMPLEMENT_CLASS( wxGripWindow, wxWindow )

DEFINE_EVENT_TYPE( wxEVT_GRIP_DBLCLICK )
DEFINE_EVENT_TYPE( wxEVT_GRIP_LEFTDOWN )
DEFINE_EVENT_TYPE( wxEVT_GRIP_LEFTUP )
DEFINE_EVENT_TYPE( wxEVT_GRIP_MOTION )

BEGIN_EVENT_TABLE( wxGripWindow, wxWindow )
    EVT_ERASE_BACKGROUND( wxGripWindow::OnErase )
    EVT_PAINT( wxGripWindow::OnPaint )
    EVT_LEFT_DCLICK( wxGripWindow::OnDoubleClick )
    EVT_LEFT_DOWN( wxGripWindow::OnLeftDown )
    EVT_LEFT_UP( wxGripWindow::OnLeftUp )
    EVT_MOTION( wxGripWindow::OnMotion )
END_EVENT_TABLE()

void wxGripWindow::Init() {
    // init
    pOwner_ = NULL;
}

bool wxGripWindow::Create( wxWindow * parent, wxOrientation orientation, wxGdi::eGripperStyle gripStyle ) {
    bool r = wxWindow::Create( parent, -1, wxDefaultPosition, wxDefaultSize, wxCLIP_CHILDREN );
    orientation_ = orientation;
    gripStyle_ = gripStyle;
    pOwner_ = parent;

    return r;
}

void wxGripWindow::SetOrientation( wxOrientation orientation ) {
    orientation_ = orientation;
}

void wxGripWindow::SetLabel( const wxString &label ){
    label_ = label;
    if( gripStyle_ != wxGdi::wxGRIP_STYLE_HEADER ) {
        SetToolTip( label );
    }
}

void wxGripWindow::OnErase( wxEraseEvent& WXUNUSED(event) ) {
    // skip erase
}

void wxGripWindow::OnPaint( wxPaintEvent& WXUNUSED(event) ) {
    wxPaintDC dc(this);
    wxRect cr = GetClientRect();

    // NOTE: as a horizontal gripper is a vertical image, and a vertical gripper is a horizontal image
    wxOrientation imageOrentation = (orientation_ == wxHORIZONTAL) ? wxVERTICAL : wxHORIZONTAL;  
    g_gdi.DrawGripper( dc, cr, imageOrentation, gripStyle_, label_ );
}

void wxGripWindow::OnDoubleClick( wxMouseEvent& event ) {
    // create a double-click event
    int x = 0, y = 0;
    event.GetPosition( &x, &y );
    createMouseEvent( wxEVT_GRIP_DBLCLICK, x, y );
}

void wxGripWindow::OnLeftDown( wxMouseEvent& event ) {
    // create a left-down event
    int x = 0, y = 0;
    event.GetPosition( &x, &y );
    createMouseEvent( wxEVT_GRIP_LEFTDOWN, x, y );
}

void wxGripWindow::OnLeftUp( wxMouseEvent& event ) {
    // create a left-up event
    int x = 0, y = 0;
    event.GetPosition( &x, &y );
    createMouseEvent( wxEVT_GRIP_LEFTUP, x, y );
}

void wxGripWindow::OnMotion( wxMouseEvent& event ) {
    // create a motion event
    int x = 0, y = 0;
    event.GetPosition( &x, &y );
    createMouseEvent( wxEVT_GRIP_MOTION, x, y );
}

void wxGripWindow::createMouseEvent( int eventId, int x, int y ) {
    wxMouseEvent e( eventId );
    e.SetEventObject( this );
    e.m_x = x;
    e.m_y = y;
    GetParent()->GetEventHandler()->ProcessEvent( e );
}

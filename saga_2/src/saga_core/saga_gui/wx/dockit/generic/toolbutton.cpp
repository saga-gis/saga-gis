/////////////////////////////////////////////////////////////////////////////
// Name:        generic/wxToolButton.cpp
// Purpose:     wxToolButton implementation.
// Author:      Mark McCormack
// Modified by:
// Created:     19/05/04
// RCS-ID:      $Id: toolbutton.cpp,v 1.1.1.1 2006-06-26 15:43:36 oconrad Exp $
// Copyright:   (c) 2004 Mark McCormack
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include <wx/toolbutton.h>

#ifdef wxEX_USE_GENERIC_TOOLBUTTON

#include <wx/gdi.h>
#include <wx/dcclient.h>
#include <wx/settings.h>

// XXX: it doesn't seem to be possible to click this button quickly on
// wxMSW (is this the 'double-click' event handling of Windows getting in the way?)

// ----------------------------------------------------------------------------
// wxToolButton implementation
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS( wxToolButton, wxControl )

BEGIN_EVENT_TABLE( wxToolButton, wxControl )
    EVT_ERASE_BACKGROUND( wxToolButton::OnErase )
    EVT_PAINT( wxToolButton::OnPaint )
    EVT_LEFT_DOWN( wxToolButton::OnLeftDown )
    EVT_LEFT_UP( wxToolButton::OnLeftUp )
    EVT_ENTER_WINDOW( wxToolButton::OnEnter ) 
    EVT_LEAVE_WINDOW( wxToolButton::OnLeave ) 
END_EVENT_TABLE()

// ----------------------------------------------------------------------------

bool wxToolButton::Create( wxWindow *parent,
                           wxWindowID id,
                           const wxPoint& pos,
                           const wxSize& size,
                           long style,
                           const wxString& name ) {

    // create the controls
    if( !wxControl::Create( parent, id, pos, size, style, wxDefaultValidator, name ) ) {
        return FALSE;
    }
    SetSizeHints(size);
    
    return TRUE;
}

wxToolButton::~wxToolButton() {
}

void wxToolButton::Init() {
    // init.
    pressed_ = false;
    hovered_ = false;

    wxToolButtonBase::Init();       
}

void wxToolButton::OnErase( wxEraseEvent& event ) {
    // skip erase
}

void wxToolButton::OnPaint( wxPaintEvent& event ) {
    wxPaintDC dc(this);
    wxRect cr = GetClientRect();
    g_gdi.DrawBackground( dc, cr );

	if( hovered_ ) {
        wxRect cr = GetClientRect();
        wxColour raisedColour = wxSystemSettings::GetColour( pressed_ ? wxSYS_COLOUR_3DDKSHADOW : wxSYS_COLOUR_3DHIGHLIGHT );
		wxPen raisedPen( raisedColour, 1, 1 );
        wxColour shadowColour = wxSystemSettings::GetColour( pressed_ ? wxSYS_COLOUR_3DHIGHLIGHT : wxSYS_COLOUR_3DDKSHADOW );
		wxPen shadowPen( shadowColour, 1, 1 );
        cr.width--;
        cr.height--;
        cr.Deflate( drawMargin_, drawMargin_ ) ;

        // draw top-left
        dc.SetPen( raisedPen );
        dc.DrawLine( cr.x, cr.y, cr.width, cr.y );
        dc.DrawLine( cr.x, cr.y, cr.x, cr.height );

        // draw bottom-right
        dc.SetPen( shadowPen );
        dc.DrawLine( cr.width, cr.y, cr.width, cr.height+1 );
        dc.DrawLine( cr.x, cr.height, cr.width+1, cr.height );
	}
    
    // draw cross
    if( pressed_ & hovered_ ) cr.Offset( 1, 1 );
    drawButtonImage( dc, cr );
}

void wxToolButton::OnEnter( wxMouseEvent& event ) {
    hovered_ = true;
    Refresh();
}

void wxToolButton::OnLeave( wxMouseEvent& event ) {
    hovered_ = false;
    Refresh();
}

void wxToolButton::OnLeftDown( wxMouseEvent& event ) {
    pressed_ = true;
    CaptureMouse();
    Refresh();
}

void wxToolButton::OnLeftUp( wxMouseEvent& event ) {
    if( pressed_ ) {
        pressed_ = false;
        ReleaseMouse();
        Refresh();
        if( hovered_ ) {
            sendClickEvent();
        }
    }
}

#endif  // wxEX_USE_GENERIC_TOOLBUTTON

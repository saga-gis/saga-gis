/////////////////////////////////////////////////////////////////////////////
// Name:        gdi.cpp
// Purpose:     wxGdi implementation.
// Author:      Mark McCormack
// Modified by:
// Created:     03/06/04
// RCS-ID:      $Id: gdi.cpp,v 1.1.1.1 2006-06-26 15:43:34 oconrad Exp $
// Copyright:   (c) 2004 Mark McCormack
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include <wx/gdi.h>
#include <wx/settings.h>

const int HEADER_TEXT_INDENT = 4;

void wxGdiBase::DrawClose( wxDC &dc, wxRect r, wxPen &pen, int size ) {
    // draw close image
    dc.SetPen( pen );
    int dx = (r.width/2)-(size/2);
    int dy = (r.height/2)-(size/2);
    r.Offset( dx, dy );
    r.SetWidth( size-1 );
    r.SetHeight( size-1 );
    dc.DrawLine( r.GetLeft(), r.GetTop(), r.GetRight(), r.GetBottom() );
    dc.DrawLine( r.GetLeft() + 1, r.GetTop(), r.GetRight() + 1, r.GetBottom() );
    dc.DrawLine( r.GetLeft(), r.GetBottom() - 1, r.GetRight(), r.GetTop() - 1 );
    dc.DrawLine( r.GetLeft() + 1, r.GetBottom() - 1, r.GetRight() + 1, r.GetTop() - 1 );
}

void wxGdiBase::DrawBackground( wxDC &dc, wxRect &r ) {
    // draw background for a window
    wxColour back = wxSystemSettings::GetColour( wxSYS_COLOUR_BTNFACE );
    wxBrush brush( back, wxSOLID );
    dc.SetBrush( brush );
    wxPen pen( back, 1, wxSOLID );
    dc.SetPen( pen );
    dc.DrawRectangle( r.x, r.y, r.width, r.height );
}

void wxGdiBase::DrawLabel( wxDC &dc, wxRect r, wxOrientation orientation, const wxString &label, const wxFont &font ) {
    // draw text label
    dc.SetFont( font );
    wxCoord w = 0, h = 0;
    dc.GetTextExtent( label, &w, &h );
    if( orientation == wxHORIZONTAL ) { 
        // horizontal text
        r.y += (r.height - h)/2;
        r.x += HEADER_TEXT_INDENT;
        dc.DrawText( label, r.x, r.y );
    }
    else {
        // vertical text
        r.x += (r.width - h)/2;
        r.y += r.height - HEADER_TEXT_INDENT;
        dc.DrawRotatedText( label, r.x, r.y, 90 );
    }
}

void wxGdiBase::DrawEdge( wxDC &dc, wxRect r, bool invert ) {
    // draw a raised/sunken 3D edge
    wxColour light = wxSystemSettings::GetColour( wxSYS_COLOUR_3DHIGHLIGHT );
    wxColour shadow = wxSystemSettings::GetColour( wxSYS_COLOUR_3DDKSHADOW );
    wxColour back = wxSystemSettings::GetColour( wxSYS_COLOUR_BACKGROUND );
    if( invert ) {
        light = wxSystemSettings::GetColour( wxSYS_COLOUR_3DDKSHADOW );
        shadow = wxSystemSettings::GetColour( wxSYS_COLOUR_3DHIGHLIGHT );
    }
    
#ifdef wxUSE_WX24
    wxBrush backBrush( back, wxSOLID );  
#else
    wxBrush backBrush( back );  
#endif
    dc.SetBrush( backBrush );
    
#ifdef wxUSE_WX24
    wxPen lightPen( light, 1, wxSOLID );
#else
    wxPen lightPen( light );
#endif
    dc.SetPen( lightPen );
    dc.DrawLine( r.GetLeft(), r.GetTop(), r.GetRight(), r.GetTop() );
    dc.DrawLine( r.GetLeft(), r.GetTop(), r.GetLeft(), r.GetBottom() );

#ifdef wxUSE_WX24
    wxPen shadowPen( shadow, 1, wxSOLID );
#else
    wxPen shadowPen( shadow );
#endif
    dc.SetPen( shadowPen );
    dc.DrawLine( r.GetLeft(), r.GetBottom(), r.GetRight() + 1, r.GetBottom() );
    dc.DrawLine( r.GetRight(), r.GetTop(), r.GetRight(), r.GetBottom() + 1 );
}

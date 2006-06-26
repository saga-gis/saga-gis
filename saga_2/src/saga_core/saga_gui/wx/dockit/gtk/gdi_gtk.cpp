/////////////////////////////////////////////////////////////////////////////
// Name:        gtk/gdi_gtk.cpp
// Purpose:     wxGdi GTK implementation.
// Author:      Mark McCormack
// Modified by:
// Created:     23/10/04
// RCS-ID:      $Id: gdi_gtk.cpp,v 1.1.1.1 2006-06-26 15:43:36 oconrad Exp $
// Copyright:   (c) 2004 Mark McCormack
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include <wx/gtk/gdi.h>
#include <wx/settings.h>

wxGdi g_gdi;    // the global instance

const int FRAME_WIDTH = 4;
const int SPLITTER_BORDER = 1;

void wxGdi::DrawGripper( wxDC &dc, wxRect r, wxOrientation orientation, eGripperStyle style, const wxString &label ) {
    DrawBackground( dc, r );

    if( style == wxGRIP_STYLE_HEADER ) {
        wxFont font = wxSystemSettings::GetFont( wxSYS_DEFAULT_GUI_FONT );
        DrawLabel(dc, r, orientation, label, font);
        return;
    }

    // TODO: GTK implementation
    const int gripSize = 3;
    const int gripMargin = 2;

    int bars = (style == wxGRIP_STYLE_FLOAT) ? 2 : 1;

    wxRect bar[2];
    int offset = (orientation == wxHORIZONTAL) ? r.height : r.width;
    offset -= (bars * gripSize);
    offset /= 2;
    for( int i=0; i<bars; i++ ) {
        if( orientation == wxHORIZONTAL ) {
            bar[i] = wxRect( gripMargin, offset, r.width-(gripMargin*2), offset+gripSize );
        }
        else {
            bar[i] = wxRect( offset, gripMargin, offset+gripSize, r.height-(gripMargin*2) );
        }
        offset += gripSize;
    }

    for( int i=0; i<bars; i++ ) {
        DrawEdge( dc, bar[i] );
    }
}

void wxGdi::DrawFrame( wxDC &dc, wxRect r, bool WXUNUSED(fDocked) ) {
    // TODO: GTK implementation
	dc.SetLogicalFunction( wxINVERT );
    dc.SetBrush( *wxBLACK_BRUSH );
	wxPen pen( *wxBLACK, 1, wxTRANSPARENT );
	dc.SetPen( pen );
    dc.DrawRectangle( r.GetLeft(), r.GetTop(), r.GetWidth(), FRAME_WIDTH );
    dc.DrawRectangle( r.GetRight() - FRAME_WIDTH + 1, r.GetTop() + FRAME_WIDTH, FRAME_WIDTH, r.GetHeight() - FRAME_WIDTH );
    dc.DrawRectangle( r.GetLeft(), r.GetBottom() - FRAME_WIDTH + 1, r.GetWidth() - FRAME_WIDTH, FRAME_WIDTH );
    dc.DrawRectangle( r.GetLeft(), r.GetTop() + FRAME_WIDTH, FRAME_WIDTH, r.GetHeight() - (FRAME_WIDTH*2) );
}

void wxGdi::DrawPattern( wxDC &dc, wxRect rect ) {
    // TODO: GTK implementation
	dc.SetLogicalFunction( wxINVERT );
    dc.SetBrush( *wxBLACK_BRUSH );
	dc.DrawRectangle( rect );
}

void wxGdi::DrawSplitter( wxDC &dc, wxRect r, wxOrientation WXUNUSED(orientation) ) {
    // TODO: GTK implementation
    DrawBackground( dc, r );
	wxRect rect( r.x + SPLITTER_BORDER, r.y + SPLITTER_BORDER, r.x + r.width - SPLITTER_BORDER, r.y + r.height - SPLITTER_BORDER );
	DrawEdge( dc, rect, false );
}

void wxGdi::DrawEmptyWorkspace( wxDC &dc, wxRect r, bool WXUNUSED(edge) ) {
    // TODO: GTK implementation
	dc.DrawRectangle( r );
}

void wxGdi::DrawHeader( wxDC &dc, wxRect r, wxOrientation orientation, const wxString &label, const wxFont &font ) {
    // edged background
    // TODO: GTK implementation for background
	DrawEdge( dc, r );

    // label
    DrawLabel( dc, r, orientation, label, font );
}

wxCursor wxGdi::GetMoveCursor() {
    return wxCursor(wxCURSOR_SIZENWSE);
}

wxCursor wxGdi::GetHSplitCursor() {
    return wxCursor(wxCURSOR_SIZEWE);
}

wxCursor wxGdi::GetVSplitCursor() {
    return wxCursor(wxCURSOR_SIZENS);
}

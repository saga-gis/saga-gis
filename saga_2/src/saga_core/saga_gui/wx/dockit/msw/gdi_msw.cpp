/////////////////////////////////////////////////////////////////////////////
// Name:        msw/gdi_msw.cpp
// Purpose:     wxGdi MSW implementation.
// Author:      Mark McCormack
// Modified by:
// Created:     04/03/04
// RCS-ID:      $Id: gdi_msw.cpp,v 1.1.1.1 2006-06-26 15:43:36 oconrad Exp $
// Copyright:   (c) 2004 Mark McCormack
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include <wx/msw/gdi.h>
#include <wx/msw/private.h>
#include <wx/settings.h>

wxGdi g_gdi;    // the global instance

const int SPLITTER_BORDER = 1;

// Need to define the LongToPtr macro if it is not defined by the environment
#ifndef LongToPtr
//typedef  long LONG_PTR, *PLONG_PTR;
#define LongToPtr( l )   ((VOID*)(LONG_PTR)((long)l))
#endif

void wxGdi::DrawGripper( wxDC &dc, wxRect r, wxOrientation orientation, eGripperStyle style, const wxString &label ) {
    DrawBackground( dc, r );
    if( style == wxGRIP_STYLE_HEADER ) {
        wxFont font = wxSystemSettings::GetFont( wxSYS_DEFAULT_GUI_FONT );
        DrawLabel(dc, r, orientation, label, font);
        return;
    }

    const int gripSize = 3;
    const int gripMargin = 2;

    int bars = (style == wxGRIP_STYLE_FLOAT) ? 2 : 1;

    RECT bar[2];
    int offset = (orientation == wxHORIZONTAL) ? r.height : r.width;
    offset -= (bars * gripSize);
    offset /= 2;
	int i;

    for( i=0; i<bars; i++ ) {
        if( orientation == wxHORIZONTAL ) {
            ::SetRect( &bar[i], gripMargin, offset, r.width-(gripMargin*2), offset+gripSize );
        }
        else {
            ::SetRect( &bar[i], offset, gripMargin, offset+gripSize, r.height-(gripMargin*2) );
        }
        offset += gripSize;
    }

    HDC hdc = GetHdcOf( dc );
    for( i=0; i<bars; i++ ) {
        ::DrawEdge( hdc, &bar[i], BDR_RAISEDINNER, BF_RECT );
    }
}

void wxGdi::DrawFrame( wxDC &dc, wxRect r, bool fDocked ) {
	static WORD _dotPatternBmp1[] = {
		0x00aa, 0x0055, 0x00aa, 0x0055, 0x00aa, 0x0055, 0x00aa, 0x0055
	};

	static WORD _dotPatternBmp2[] = {
		0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff, 0xffff
	};

	HBITMAP hbm;
	HBRUSH  hbr;
	HANDLE  hbrushOld;
	WORD    *bitmap;

	int width, height, x, y;
	int border;

    HDC hdc = GetHdcOf( dc );

	if( fDocked ) {
		border = 1;
		bitmap = _dotPatternBmp2;
	}
	else {
		border = 3;
		bitmap = _dotPatternBmp1;
	}
	
	x = r.GetLeft();
	y = r.GetTop();
	width  = r.width;
	height = r.height;

	hbm = ::CreateBitmap( 8, 8, 1, 1, bitmap );
	hbr = ::CreatePatternBrush( hbm );

	::SetBrushOrgEx( hdc, x, y, 0 );
	hbrushOld = ::SelectObject( hdc, hbr );

	::PatBlt( hdc, x+border,       y,               width-border,  border,        PATINVERT );
	::PatBlt( hdc, x+width-border, y+border,        border,        height-border, PATINVERT );
	::PatBlt( hdc, x,              y+height-border, width-border,  border,        PATINVERT );
	::PatBlt( hdc, x,              y,               border,        height-border, PATINVERT );

	::SelectObject( hdc, hbrushOld );
	::DeleteObject( hbr );
	::DeleteObject( hbm );
	::ReleaseDC( 0, hdc );
}

void wxGdi::DrawPattern( wxDC &dc, wxRect r ) {
	static WORD _dotPatternBmp[] = {
		0x00aa, 0x0055, 0x00aa, 0x0055, 0x00aa, 0x0055, 0x00aa, 0x0055
	};

	HBITMAP hbm;
	HBRUSH  hbr;
	HANDLE  hbrushOld;
	WORD    *bitmap;

	int width, height, x, y;

    HDC hdc = GetHdcOf( dc );
	bitmap = _dotPatternBmp;
	
	x = r.GetLeft();
	y = r.GetTop();
	width  = r.width;
	height = r.height;

	hbm = ::CreateBitmap( 8, 8, 1, 1, bitmap );
	hbr = ::CreatePatternBrush( hbm );

	::SetBrushOrgEx( hdc, x, y, 0 );
	hbrushOld = ::SelectObject( hdc, hbr );

	::PatBlt( hdc, x, y, width, height, PATINVERT );

	::SelectObject( hdc, hbrushOld );
	::DeleteObject( hbr );
	::DeleteObject( hbm );
	::ReleaseDC( 0, hdc );
}

void wxGdi::DrawSplitter( wxDC &dc, wxRect r, wxOrientation WXUNUSED(orientation) ) {
    HDC hdc = GetHdcOf( dc );
	RECT rect;
    ::SetRect( &rect, r.x + SPLITTER_BORDER, r.y + SPLITTER_BORDER, r.x + r.width - SPLITTER_BORDER, r.y + r.height - SPLITTER_BORDER );

    DrawBackground( dc, r );
    ::DrawEdge( hdc, &rect,  BDR_RAISEDINNER, BF_RECT );
}

void wxGdi::DrawEmptyWorkspace( wxDC &dc, wxRect r, bool edge ) {
    HDC hdc = GetHdcOf( dc );
    RECT rect = { r.x, r.y, r.width, r.height };
    if( edge) {
        ::DrawEdge( hdc, &rect, EDGE_SUNKEN, BF_RECT | BF_ADJUST );
    }
    ::FillRect( hdc, &rect, (HBRUSH)LongToPtr(COLOR_APPWORKSPACE + 1) ); 
}

void wxGdi::DrawHeader( wxDC &dc, wxRect r, wxOrientation orientation, const wxString &label, const wxFont &font ) {
    HDC hdc = GetHdcOf( dc );
    RECT rect = { r.x, r.y, r.width, r.height };

    // edged background
    DrawBackground( dc, r );
    ::FillRect( hdc, &rect, (HBRUSH)LongToPtr(COLOR_3DFACE + 1) );
    ::DrawEdge( hdc, &rect, EDGE_ETCHED, BF_LEFT | BF_TOP | BF_BOTTOM | BF_RIGHT | BF_ADJUST );

    // label
    DrawLabel( dc, r, orientation, label, font );
}

void wxGdi::DrawEdge( wxDC &dc, wxRect r, bool invert ) {
    HDC hdc = GetHdcOf( dc );
    RECT rect = { r.x, r.y, r.width, r.height };

    ::DrawEdge( hdc, &rect, invert ? BDR_SUNKENINNER : BDR_RAISEDINNER, BF_LEFT | BF_TOP | BF_BOTTOM | BF_RIGHT | BF_ADJUST );
}

wxCursor wxGdi::GetMoveCursor() {
    wxCursor tmp( wxCURSOR_SIZING );
    tmp.SetHCURSOR( (WXHCURSOR)LoadCursor(wxGetInstance(), wxT("IDC_SMALLARROWS")) );
    return tmp;
}

wxCursor wxGdi::GetHSplitCursor() {
    wxCursor tmp( wxCURSOR_SIZING );
    tmp.SetHCURSOR( (WXHCURSOR)LoadCursor(wxGetInstance(), wxT("IDC_HSPLITBAR")) );
    return tmp;
}

wxCursor wxGdi::GetVSplitCursor() {
    wxCursor tmp( wxCURSOR_SIZING );
    tmp.SetHCURSOR( (WXHCURSOR)LoadCursor(wxGetInstance(), wxT("IDC_VSPLITBAR")) );
    return tmp;
}

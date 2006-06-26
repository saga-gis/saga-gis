///////////////////////////////////////////////////////////////////////////////
// Name:        wx/wxGdi.h
// Purpose:     wxGdi class
// Author:      Mark McCormack
// Modified by:
// Created:     04/03/04
// RCS-ID:      $Id: gdi.h,v 1.1.1.1 2006-06-26 15:43:06 oconrad Exp $
// Copyright:   (c) 2004 Mark McCormack
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_WXGDI_BASE_H_
#define _WX_WXGDI_BASE_H_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include <wx/window.h>

#ifndef wxFULL_REPAINT_ON_RESIZE
#define wxFULL_REPAINT_ON_RESIZE 0  // XXX: just for < wx2.5.1 compatibility
#endif

// ----------------------------------------------------------------------------
// wxGdiBase
// ----------------------------------------------------------------------------

class wxGdiBase {
public:
    enum eGripperStyle {
        wxGRIP_STYLE_MOVE,
        wxGRIP_STYLE_FLOAT,
        wxGRIP_STYLE_HEADER
    };

    // drawing
    virtual void DrawGripper( wxDC &dc, wxRect r, wxOrientation orientation, 
		eGripperStyle style, const wxString &label = wxT("") ) = 0;
    virtual void DrawFrame( wxDC &dc, wxRect r, bool fDocked ) = 0;
    virtual void DrawPattern( wxDC &dc, wxRect r ) = 0;
    virtual void DrawSplitter( wxDC &dc, wxRect r, wxOrientation orientation ) = 0;
    virtual void DrawClose( wxDC &dc, wxRect r, wxPen &pen, int size );
    virtual void DrawEmptyWorkspace( wxDC &dc, wxRect r, bool edge ) = 0;
    virtual void DrawHeader( wxDC &dc, wxRect r, wxOrientation orientation, 
		const wxString &label, const wxFont &font ) = 0;
    virtual void DrawBackground( wxDC &dc, wxRect &r );
    virtual void DrawEdge( wxDC &dc, wxRect r, bool invert = false );
    virtual void DrawLabel( wxDC &dc, wxRect r, wxOrientation orientation, 
		const wxString &label, const wxFont &font );
    
    // cursors
    virtual wxCursor GetMoveCursor() = 0;
    virtual wxCursor GetHSplitCursor() = 0;
    virtual wxCursor GetVSplitCursor() = 0;
};

// ----------------------------------------------------------------------------
// include the platform-specific class declaration
// ----------------------------------------------------------------------------

#if defined(__WXMSW__)
    #include "wx/msw/gdi.h"
#elif defined(__WXGTK__)
	#include "wx/gtk/gdi.h"
#else
    #error "Your platform does not currently support wxGdi"
#endif

// ----------------------------------------------------------------------------
// define wxUSE_WX24 when compiling with wxWidgets 2.4.x, otherwise default to wxWidgets 2.5.x
// ----------------------------------------------------------------------------

//#define wxUSE_WX24

#endif
    // _WX_WXGDI_BASE_H_

///////////////////////////////////////////////////////////////////////////////
// Name:        wx/exsplitter.h
// Purpose:     wxExSplitter class
// Author:      Mark McCormack
// Modified by:
// Created:     23/02/04
// RCS-ID:      $Id: exsplitter.h,v 1.1.1.1 2006-06-26 15:43:06 oconrad Exp $
// Copyright:   (c) 2004 Mark McCormack
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_EXSPLITTER_H_
#define _WX_EXSPLITTER_H_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include <wx/defs.h>
#include <wx/window.h>

#define wxSPLITTER_WINDOWS     2
#define wxSPLITTER_SIZE        6
#define wxMINIMUM_WINDOW_SIZE  32

#include "wx/export.h"

// ----------------------------------------------------------------------------
// wxExSplitter
// ----------------------------------------------------------------------------

#define wxESF_LIVE_UPDATE	0x01
#define	wxESF_DRAW_GRIPPER	0x02

class wxExSplitter : public wxWindow {
    DECLARE_CLASS( wxExSplitter )
    DECLARE_EVENT_TABLE()

public:
    wxExSplitter() {
        Init();
    }
    void Init();
    
    wxExSplitter( wxWindow * parent, wxOrientation orientation, 
		wxWindow * win1, wxWindow * win2, unsigned int flags ) 
	{
        Init();
        Create( parent, orientation, win1, win2, flags );
    }
    
    // basic interface
    bool Create( wxWindow * parent, wxOrientation orientation, 
		wxWindow * win1, wxWindow * win2, unsigned int flags );
    
    // basic interface
    void SetWindows( wxWindow * pWin1, wxWindow * pWin2 );
    void SetFlags( unsigned int flags );
    
    // event handlers
    void OnLeftDown( wxMouseEvent& event );
    void OnMouseMove( wxMouseEvent& event );
    void OnErase( wxEraseEvent& event );
    void OnPaint( wxPaintEvent& event );
    void OnLeftUp( wxMouseEvent& event );

    void SetBoundaries ();

private:
    void draw( wxRect r );

private:
    wxWindow * pOwner_;
    wxWindow * pWindow_[wxSPLITTER_WINDOWS];
    wxOrientation orientation_;
    wxPoint startPosition_;
    wxRect startRect_[wxSPLITTER_WINDOWS];
    wxRect startRectUs_;

    // movement boundary top-left / bottom-left points
    wxPoint boundary[2];

    bool dragging_;
    bool bounded_;
    int minSize_;
    wxRect endRect_[wxSPLITTER_WINDOWS];
    wxRect endRectUs_;
    unsigned int flags_;
    wxRect lastDrawnRect_;
};

// ----------------------------------------------------------------------------
// wxExSplitter events
// ----------------------------------------------------------------------------

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EXPORTED_EVENT_TYPE( WXDLLIMPEXP_DI, wxEVT_SPLITTER_MOVED, wxEVT_FIRST + 1211 )  // TODO: must change id
END_DECLARE_EVENT_TYPES()

// This should keep wxDockit compatible with wxWidgets 2.4.x
#ifndef WXWIN_COMPATIBILITY_2_4
#define EVT_SPLITTER_MOVED( fn ) \
    DECLARE_EVENT_TABLE_ENTRY( wxEVT_SPLITTER_MOVED, -1, -1, \
		(wxObjectEventFunction) (wxEventFunction) & fn, NULL ),
#else
// Patch for wxWidgets-2.5.4 compat submitted by Jesus Gonzalez (2005-03-02)
typedef void (wxEvtHandler::*wxCommandEventFunction)(wxCommandEvent&);
#define EVT_SPLITTER_MOVED( fn ) \
    DECLARE_EVENT_TABLE_ENTRY( wxEVT_SPLITTER_MOVED, -1, -1, \
		(wxObjectEventFunction) (wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, & fn ), NULL ),
#endif

#endif
    // _WX_EXSPLITTER_H_

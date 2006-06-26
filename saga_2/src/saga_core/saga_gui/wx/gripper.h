///////////////////////////////////////////////////////////////////////////////
// Name:        wx/gripper.h
// Purpose:     wxGripperWindow class
// Author:      Mark McCormack
// Modified by:
// Created:     23/02/04
// RCS-ID:      $Id: gripper.h,v 1.1.1.1 2006-06-26 15:43:06 oconrad Exp $
// Copyright:   (c) 2004 Mark McCormack
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_GRIPPER_H_
#define _WX_GRIPPER_H_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include <wx/gdi.h>

#include "wx/export.h"

// ----------------------------------------------------------------------------
// wxGripWindow
// ----------------------------------------------------------------------------

class wxGripWindow : public wxWindow {
public:
    wxGripWindow() {
        Init();
    }
    void Init();
    
    wxGripWindow( wxWindow * parent, wxOrientation orientation, 
		wxGdi::eGripperStyle gripStyle ) 
	{
        Init();
        Create( parent, orientation, gripStyle );
    }

    // basic interface
    bool Create( wxWindow * parent, wxOrientation orientation, 
		wxGdi::eGripperStyle gripStyle );

    void SetOrientation( wxOrientation orientation );
    void SetLabel( const wxString &label );
    
    // event handlers
    void OnErase( wxEraseEvent& event );
    void OnPaint( wxPaintEvent& event );
    void OnDoubleClick( wxMouseEvent& event );
    void OnLeftDown( wxMouseEvent& event );
    void OnLeftUp( wxMouseEvent& event );
    void OnMotion( wxMouseEvent& event );

private:
    void createMouseEvent( int eventId, int x, int y );

private:
    wxString label_;
    wxWindow * pOwner_;
    wxOrientation orientation_;
    wxGdi::eGripperStyle gripStyle_;

private:
    DECLARE_CLASS( wxGripWindow )
    DECLARE_EVENT_TABLE()
};

// ----------------------------------------------------------------------------
// wxGripWindow events
// ----------------------------------------------------------------------------

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EXPORTED_EVENT_TYPE( WXDLLIMPEXP_DI, wxEVT_GRIP_DBLCLICK, wxEVT_FIRST + 1230)   // TODO: must change id
    DECLARE_EXPORTED_EVENT_TYPE( WXDLLIMPEXP_DI, wxEVT_GRIP_LEFTDOWN, wxEVT_FIRST + 1231)   // TODO: must change id
    DECLARE_EXPORTED_EVENT_TYPE( WXDLLIMPEXP_DI, wxEVT_GRIP_LEFTUP,   wxEVT_FIRST + 1232)   // TODO: must change id
    DECLARE_EXPORTED_EVENT_TYPE( WXDLLIMPEXP_DI, wxEVT_GRIP_MOTION,   wxEVT_FIRST + 1233)   // TODO: must change id
END_DECLARE_EVENT_TYPES()

// This should keep wxDockit compatible with wxWidgets 2.4.x
#ifndef WXWIN_COMPATIBILITY_2_4
#define EVT_GRIP_DBLCLICK(fn) \
    DECLARE_EVENT_TABLE_ENTRY( wxEVT_GRIP_DBLCLICK, -1, -1, \
		(wxObjectEventFunction) (wxEventFunction) & fn, NULL ),
#define EVT_GRIP_LEFTDOWN(fn) \
    DECLARE_EVENT_TABLE_ENTRY( wxEVT_GRIP_LEFTDOWN, -1, -1, \
		(wxObjectEventFunction) (wxEventFunction) & fn, NULL ),
#define EVT_GRIP_LEFTUP(fn) \
    DECLARE_EVENT_TABLE_ENTRY( wxEVT_GRIP_LEFTUP, -1, -1, \
		(wxObjectEventFunction) (wxEventFunction) & fn, NULL ),
#define EVT_GRIP_MOTION(fn) \
    DECLARE_EVENT_TABLE_ENTRY( wxEVT_GRIP_MOTION, -1, -1, \
		(wxObjectEventFunction) (wxEventFunction) & fn, NULL ),
#else
// Patch for wxWidgets-2.5.4 compat submitted by Jesus Gonzalez (2005-03-02)
typedef void (wxEvtHandler::*wxMouseEventFunction)(wxMouseEvent&);
#define EVT_GRIP_DBLCLICK(fn) \
    DECLARE_EVENT_TABLE_ENTRY( wxEVT_GRIP_DBLCLICK, -1, -1, \
		(wxObjectEventFunction) (wxEventFunction) wxStaticCastEvent( wxMouseEventFunction, & fn ), NULL ),
#define EVT_GRIP_LEFTDOWN(fn) \
    DECLARE_EVENT_TABLE_ENTRY( wxEVT_GRIP_LEFTDOWN, -1, -1, \
		(wxObjectEventFunction) (wxEventFunction) wxStaticCastEvent( wxMouseEventFunction, & fn ), NULL ),
#define EVT_GRIP_LEFTUP(fn) \
    DECLARE_EVENT_TABLE_ENTRY( wxEVT_GRIP_LEFTUP, -1, -1, \
		(wxObjectEventFunction) (wxEventFunction) wxStaticCastEvent( wxMouseEventFunction, & fn ), NULL ),
#define EVT_GRIP_MOTION(fn) \
    DECLARE_EVENT_TABLE_ENTRY( wxEVT_GRIP_MOTION, -1, -1, \
		(wxObjectEventFunction) (wxEventFunction) wxStaticCastEvent( wxMouseEventFunction, & fn ), NULL ),
#endif

#endif
    // _WX_GRIPPER_H_

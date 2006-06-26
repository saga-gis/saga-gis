///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/dockwindow_msw.h
// Purpose:     wxDockWindow MSW class
// Author:      Mark McCormack
// Modified by:
// Created:     23/02/04
// RCS-ID:      $Id: dockwindow.h,v 1.1.1.1 2006-06-26 15:43:06 oconrad Exp $
// Copyright:   (c) 2004 Mark McCormack
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_DOCKWINDOW_H_
#define _WX_DOCKWINDOW_H_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include <wx/dockwindow.h>
#include "wx/export.h"

// ----------------------------------------------------------------------------
// wxDockWindow
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_DI wxDockWindow : public wxDockWindowBase
{
public:
    wxDockWindow() {
        Init();
    }
    
    wxDockWindow( wxWindow * parent, wxWindowID id, const wxString& title, const wxPoint& pos = wxDefaultPosition, const wxSize& size = wxDefaultSize, const wxString& name = wxT("frame"), unsigned int flags = wxDWC_DEFAULT ) {
        Init();
        Create( parent, id, title, pos, size, name, flags );
    }
    
    virtual long MSWWindowProc( WXUINT nMsg, WXWPARAM wParam, WXLPARAM lParam );

    // platform
    virtual void StartDragging( int x, int y );
    virtual void StopDragging();
    virtual bool BlockDocking();

private:
	
};
#endif
    // _WX_DOCKWINDOW_H_

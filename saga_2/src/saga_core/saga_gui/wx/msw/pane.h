/////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/pane.h
// Purpose:     wxPane class
// Author:      Mark McCormack
// Modified by:
// Created:     28/12/03
// RCS-ID:      $Id: pane.h,v 1.1.1.1 2006-06-26 15:43:06 oconrad Exp $
// Copyright:   (c) 2004 Mark McCormack
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PANE_H_
#define _WX_PANE_H_

#include <wx/msw/private.h> // wxMSW specifics 
#include "wx/export.h"

// ----------------------------------------------------------------------------
// Pane control
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_DI wxPane : public wxPaneBase
{
public:
    // Default constructor
    wxPane() {
        Init();
    }
    
    // Normal constructor
    wxPane( wxWindow *parent, wxWindowID id = -1, const wxString& name = wxT("pane"), const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL ) {
        Init();
        Create( parent, id, name, pos, size, style );
    }

    virtual wxWindow * SetClient( wxWindow * pClient, bool removeBorder = false );
    
private:
    DECLARE_DYNAMIC_CLASS(wxPane)
};


#endif
    // _WX_PANE_H_

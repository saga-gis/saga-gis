/////////////////////////////////////////////////////////////////////////////
// Name:        wx/gtk/pane.h
// Purpose:     wxPane class
// Author:      Mark McCormack
// Modified by:
// Created:     23/10/04
// RCS-ID:      $Id: pane.h,v 1.1.1.1 2006-06-26 15:43:07 oconrad Exp $
// Copyright:   (c) 2004 Mark McCormack
// Licence:     wxWindows licence
/////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PANE_H_
#define _WX_PANE_H_

// ----------------------------------------------------------------------------
// Pane control
// ----------------------------------------------------------------------------

class WXDLLEXPORT wxPane : public wxPaneBase
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

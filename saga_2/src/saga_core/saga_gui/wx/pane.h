///////////////////////////////////////////////////////////////////////////////
// Name:        wx/pane.h
// Purpose:     wxPane class
// Author:      Mark McCormack
// Modified by:
// Created:     28/12/03
// RCS-ID:      $Id: pane.h,v 1.1.1.1 2006-06-26 15:43:06 oconrad Exp $
// Copyright:   (c) 2004 Mark McCormack
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_PANE_H_BASE_
#define _WX_PANE_H_BASE_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include <wx/panel.h>
#include "wx/export.h"

class wxToolButton;

// ----------------------------------------------------------------------------
// wxPane interface is defined by the class wxPaneBase
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_DI wxPaneBase : public wxPanel
{
    DECLARE_ABSTRACT_CLASS(wxPaneBase)
    DECLARE_EVENT_TABLE()

public:
    virtual bool Create( wxWindow *parent, wxWindowID id = -1, 
		const wxString& name = wxT("pane"), 
		const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = wxTAB_TRAVERSAL );
    
    virtual void Init();

    // from wxWindow
    virtual bool Show( bool state = TRUE );

    // for wxPane
    void ShowHeader( bool state );
    void ShowCloseButton( bool state );
    void SetOrientation( wxOrientation orientation );
    void SetVisibilityOnParent( bool state );

    wxWindow * GetClient();
    virtual wxWindow * SetClient( wxWindow * pClient, 
		bool removeBorder = false );

protected:
    void UpdateSize();
    void UpdateLayout( int w, int h );

    int GetHeaderSize();
    void CalcHeaderSize();
    const wxFont & GetTitleFont();

    // event handlers
    void OnSize( wxSizeEvent &event );
    void OnCloseButton( wxCommandEvent &event );
    void OnPaint( wxPaintEvent &event );
    void OnErase( wxEraseEvent &event );

protected:
    bool m_showHeader;            // header visibility
    bool m_showClosebutton;       // close button visibility
    bool m_visibilityOnParent;    // parent responds to Show() rather than actual pane
    int m_headerSize;             // extent of header (for both horizontal and vertical)
    wxOrientation m_orientation;  // orientation of the pane header (horizontal is default)
    
    wxSize m_toolButtonSize;      // total button size

    wxWindow * m_pClient;          // the client window
    wxToolButton * m_pCloseButton; // the close button
};

// ----------------------------------------------------------------------------
// wxPane event class
// ----------------------------------------------------------------------------

BEGIN_DECLARE_EVENT_TYPES()
    DECLARE_EXPORTED_EVENT_TYPE( WXDLLIMPEXP_DI, wxEVT_PANE_CLOSED, wxEVT_FIRST + 1230)   // TODO: must change id
END_DECLARE_EVENT_TYPES()

// This should keep wxDockit compatible with wxWidgets 2.4.x
#ifndef WXWIN_COMPATIBILITY_2_4
#define EVT_PANE_CLOSED(fn) \
    DECLARE_EVENT_TABLE_ENTRY( wxEVT_PANE_CLOSED, -1, -1, \
		(wxObjectEventFunction) (wxEventFunction)  & fn, NULL ),
#else
// Patch for wxWidgets-2.5.4 compat submitted by Jesus Gonzalez (2005-03-02)
typedef void (wxEvtHandler::*wxCommandEventFunction)(wxCommandEvent&);
#define EVT_PANE_CLOSED(fn) \
    DECLARE_EVENT_TABLE_ENTRY( wxEVT_PANE_CLOSED, -1, -1, \
		(wxObjectEventFunction) (wxEventFunction) wxStaticCastEvent( wxCommandEventFunction, & fn ), NULL ),
#endif

// ----------------------------------------------------------------------------
// include the platform-specific class declaration
// ----------------------------------------------------------------------------

#if defined(__WXMSW__)
    #include "wx/msw/pane.h"
#elif defined(__WXGTK__)
	#include "wx/gtk/pane.h"
#else
    #error "Your platform does not currently support wxPane"
#endif

#endif
    // _WX_PANE_H_BASE_

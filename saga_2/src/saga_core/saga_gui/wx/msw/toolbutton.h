///////////////////////////////////////////////////////////////////////////////
// Name:        wx/msw/toolbutton.h
// Purpose:     wxToolButton class
// Author:      Mark McCormack
// Modified by: Francesco Montorsi
// Created:     10/04/04
// RCS-ID:      $Id: toolbutton.h,v 1.1.1.1 2006-06-26 15:43:06 oconrad Exp $
// Copyright:   (c) 2004 Mark McCormack
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_TOOLBUTTON_H_
#define _WX_TOOLBUTTON_H_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include <wx/toolbutton.h>
#include <wx/msw/private.h>
#include "wx/export.h"

// ----------------------------------------------------------------------------
// wxToolButton
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_DI wxToolButton : public wxToolButtonBase
{
public:
    // Default constructor
    wxToolButton() {
        Init();
    }

    void Init();
    
    // Normal constructor
    wxToolButton( wxWindow *parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxT("toolbutton") ) {
        Init();
        Create(parent, id, pos, size, style, name);
    }

    bool Create( wxWindow *parent, wxWindowID id = -1, const wxPoint& pos = wxDefaultPosition,
        const wxSize& size = wxDefaultSize, long style = 0, const wxString& name = wxT("toolbutton") );

    virtual ~wxToolButton();

private:
    void OnSize( wxSizeEvent &event );
    void OnErase( wxEraseEvent &event );

    // MSW Specific
    virtual bool MSWOnNotify( int idCtrl, WXLPARAM lParam, WXLPARAM *result );
    virtual bool MSWCommand( WXUINT param, WXWORD WXUNUSED(id) );
    
    void createToolbar( const wxPoint& pos, const wxSize& size, long style, wxWindowID id );

private:
    // attributes
    SIZE m_toolButtonSize;

private:
    DECLARE_DYNAMIC_CLASS( wxToolButton )
    DECLARE_EVENT_TABLE()
};

#endif
    // _WX_TOOLBUTTONBASE_H_

///////////////////////////////////////////////////////////////////////////////
// Name:        wx/toolbutton.h
// Purpose:     wxToolButtonBase class
// Author:      Mark McCormack
// Modified by: Francesco Montorsi
// Created:     10/04/04
// RCS-ID:      $Id: toolbutton.h,v 1.1.1.1 2006-06-26 15:43:06 oconrad Exp $
// Copyright:   (c) 2004 Mark McCormack
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

//#define wxEX_USE_GENERIC_TOOLBUTTON   // - intended for debugging on wxMSW only

#ifndef _WX_TOOLBUTTONBASE_H_
#define _WX_TOOLBUTTONBASE_H_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include <wx/defs.h>
#include <wx/control.h>
#include <wx/settings.h>
#include <wx/gdi.h>
#include "wx/export.h"

// not required since wxButtonNameStr is already defined in button.h
//extern WXDLLIMPEXP_DATA_DI(const wxChar*) wxButtonNameStr;

#define wxDEFAULT_DRAW_SIZE             8

// ----------------------------------------------------------------------------
// wxToolButtonBase
// ----------------------------------------------------------------------------

class WXDLLIMPEXP_DI wxToolButtonBase : public wxControl
{
public:
    virtual void Init() {
        drawMargin_ = 0;
        drawSize_ = wxDEFAULT_DRAW_SIZE;
    }

    void SetDrawMargin( int drawMargin ) { drawMargin_ = drawMargin; }
    void SetDrawSize( int drawSize ) { drawSize_ = drawSize; }
    void SetBitmap( wxBitmap &bitmap ) { bitmap_ = bitmap; }
    
protected:
    bool sendClickEvent() {
        // create the button-click event
        wxCommandEvent event( wxEVT_COMMAND_BUTTON_CLICKED, GetId() );
        event.SetEventObject( this );
        return GetEventHandler()->ProcessEvent( event );
    }

    void drawButtonImage( wxDC &dc, wxRect& rect ) {
        if( bitmap_.Ok() ) {
            // draw custom image (centered)
            int ox = (rect.width / 2) - (bitmap_.GetWidth() / 2) + rect.x;
            int oy = (rect.height / 2) - (bitmap_.GetHeight() / 2) + rect.y;
            dc.DrawBitmap( bitmap_, ox, oy, true );
        }
        else {
            // for now, do a cross if no bitmap
	        wxColour crossColour = wxSystemSettings::GetColour( wxSYS_COLOUR_BTNTEXT );
	        wxPen pen( crossColour, 1, 1 );
            g_gdi.DrawClose( dc, rect, pen, drawSize_ );
       }
    }

protected:
    int drawMargin_;
    int drawSize_;
    wxBitmap bitmap_;
};

// ----------------------------------------------------------------------------
// include the platform-specific class declaration
// ----------------------------------------------------------------------------

#ifdef wxEX_USE_GENERIC_TOOLBUTTON
    #include "wx/generic/toolbutton.h"    
#else
    #if defined(__WXMSW__)
        #include "wx/msw/toolbutton.h"                
    #else
        // other platform-specific implementation are not currently available...
        #define wxEX_USE_GENERIC_TOOLBUTTON
        #include "wx/generic/toolbutton.h"        
    #endif
#endif

#endif
    // _WX_TOOLBUTTONBASE_H_

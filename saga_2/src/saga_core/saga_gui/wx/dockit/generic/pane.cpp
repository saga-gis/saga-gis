/////////////////////////////////////////////////////////////////////////////
// Name:        pane.cpp
// Purpose:     wxPaneBase implementation.
// Author:      Mark McCormack
// Modified by:
// Created:     28/12/03
// RCS-ID:      $Id: pane.cpp,v 1.1.1.1 2006-06-26 15:43:35 oconrad Exp $       $Id: pane.cpp,v 1.1.1.1 2006-06-26 15:43:35 oconrad Exp $
// Copyright:   (c) 2004 Mark McCormack
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include <wx/pane.h>
#include <wx/toolbutton.h>
#include <wx/settings.h>
#include <wx/gdi.h>
#include <wx/dcclient.h>

//#include <algorithm>
//using std::max;

#ifndef min
#define min(a,b) ((a) < (b) ? (a) : (b))
#endif
#ifndef max
#define max(a,b) ((a) > (b) ? (a) : (b))
#endif

// ----------------------------------------------------------------------------
// wxPaneBase constants & wx-macros
// ----------------------------------------------------------------------------

BEGIN_EVENT_TABLE( wxPaneBase, wxPanel )
    EVT_SIZE( wxPaneBase::OnSize )
    EVT_BUTTON( 0, wxPaneBase::OnCloseButton )
    EVT_ERASE_BACKGROUND( wxPaneBase::OnErase )
    EVT_PAINT( wxPaneBase::OnPaint )
END_EVENT_TABLE()

DEFINE_EVENT_TYPE( wxEVT_PANE_CLOSED )

IMPLEMENT_ABSTRACT_CLASS( wxPaneBase, wxPanel )

const wxSize DEFAULT_BUTTON_SIZE( 17, 16 );
const int DEFAULT_BUTTON_IMAGE_SIZE = 9;

const int HEADER_BORDER = 2;    // distance around outside of header

// ----------------------------------------------------------------------------
// wxPaneBase implementation
// ----------------------------------------------------------------------------

bool wxPaneBase::Create( wxWindow *parent, wxWindowID id, const wxString& name, const wxPoint& pos, const wxSize& size, long style ) {
    bool result = wxPanel::Create( parent, id, pos, size, style, name );

    // create toolbutton (the close button)
    m_toolButtonSize = DEFAULT_BUTTON_SIZE;
    m_pCloseButton = new wxToolButton( this, 0, wxDefaultPosition, m_toolButtonSize );
    m_pCloseButton->SetDrawSize( DEFAULT_BUTTON_IMAGE_SIZE );
    m_pCloseButton->SetToolTip( wxT("Close") );
    wxASSERT(m_pCloseButton);

    CalcHeaderSize();

    return result;
}

void wxPaneBase::Init() {
    m_visibilityOnParent = false;
    m_orientation = wxHORIZONTAL;
    m_headerSize = 0;
    m_toolButtonSize.x = 0;
    m_toolButtonSize.y = 0;
    m_pCloseButton = NULL;
    m_pClient = NULL;
    m_showHeader = true;
    m_showClosebutton = true;
}

bool wxPaneBase::Show( bool state ) {
    if( m_visibilityOnParent ) {
        // visibility on parent
        return GetParent()->Show( state );
    }
    else {
        // visibility on self
        return wxPanel::Show( state );
    }
}

void wxPaneBase::ShowHeader( bool state ) {
    m_showHeader = state;
    UpdateSize();
}

wxWindow * wxPaneBase::SetClient( wxWindow * pClient, bool WXUNUSED(removeBorder) ) {
    // set the pane's client window
    wxWindow * pOldClient = m_pClient;
    m_pClient = pClient;
    if( m_pClient ) {
        // force reparent
        m_pClient->Reparent( this );
        UpdateSize();
    }
    return pOldClient;
}

void wxPaneBase::SetOrientation( wxOrientation orientation ) {
    // set pane's header orientation
    m_orientation = orientation;
    UpdateSize();
}

void wxPaneBase::SetVisibilityOnParent( bool state ) {
    // set whether visibility commands to the pane are re-directed to the parent window
    m_visibilityOnParent = state;
}

void wxPaneBase::ShowCloseButton( bool state ) {
    wxASSERT(m_pCloseButton);
    m_showClosebutton = state;
    m_pCloseButton->Show( state );
}

wxWindow * wxPaneBase::GetClient() {
    return m_pClient;
}

void wxPaneBase::UpdateSize() {
    // update the layout
    wxRect r = GetClientRect();
    UpdateLayout( r.width, r.height );
}

void wxPaneBase::OnSize( wxSizeEvent& WXUNUSED(event) ) {
    // size event - do internal update
    UpdateSize();
}

void wxPaneBase::OnCloseButton( wxCommandEvent& WXUNUSED(event) ) {
    // create a close event
    wxCommandEvent closeEvent( wxEVT_PANE_CLOSED );
    closeEvent.SetEventObject( this );
    GetEventHandler()->ProcessEvent( closeEvent );
}

void wxPaneBase::OnErase( wxEraseEvent& WXUNUSED(event) ) {
	// just skip
}

void wxPaneBase::OnPaint( wxPaintEvent& WXUNUSED(event) ) {
    wxPaintDC dc(this);

    int headerSize = GetHeaderSize();

    // draw header
    wxRect hr = GetClientRect();
    if( m_orientation == wxVERTICAL ) {
        hr.SetRight( hr.x + headerSize );
    } else {
        hr.SetBottom( hr.y + headerSize );
    }

	// create a clipping region to exclude the close button
	if( m_pCloseButton && m_showClosebutton ) {
		dc.DestroyClippingRegion();
		wxRegion region( hr );
		wxRect sr = m_pCloseButton->GetRect();
		region.Subtract( sr );
		dc.SetClippingRegion( region );
	}

	g_gdi.DrawHeader( dc, hr, m_orientation, GetName(), GetTitleFont() );

    // draw contents
    wxRect cr = GetClientRect();
    if( m_orientation == wxVERTICAL ) {
        cr.x += headerSize;
    }
    else {
        cr.y += headerSize;
    }

	// create a clipping region to exclude the child window
	dc.DestroyClippingRegion();
	wxRegion region( cr );
	if( m_pClient ) {
		wxRect sr = m_pClient->GetRect();
		region.Subtract( sr );
	}
	dc.SetClippingRegion( region );

    g_gdi.DrawEmptyWorkspace( dc, cr, true );
}

int wxPaneBase::GetHeaderSize() {
    return m_showHeader ? m_headerSize : 0;
}

void wxPaneBase::UpdateLayout( int cxWidth, int cyHeight ) {
    // update the components position & size
    wxRect rect;
    int headerSize = GetHeaderSize();

    if( m_orientation == wxVERTICAL )
    {
        // vertical positioning
        rect = wxRect( 0, 0, headerSize, cyHeight );
        if( m_pCloseButton ) {
            m_pCloseButton->SetSize( (headerSize/2)-(m_toolButtonSize.x/2), HEADER_BORDER, m_toolButtonSize.x, m_toolButtonSize.y );
        }

        if( m_pClient ) {
            m_pClient->SetSize( headerSize, 0, (cxWidth -  headerSize), cyHeight );
        } else {
            rect.SetRight( cxWidth );
        }
    }
    else {
        // horizontal positioning
        rect = wxRect( 0, 0, cxWidth, headerSize );
        if( m_pCloseButton ) {
            m_pCloseButton->SetSize( (rect.GetRight() - m_toolButtonSize.x - HEADER_BORDER), (headerSize/2)-(m_toolButtonSize.y/2), m_toolButtonSize.x, m_toolButtonSize.y );
        }

        if( m_pClient ) {
            m_pClient->SetSize( 0, headerSize, cxWidth, (cyHeight  - headerSize) );
        } else {
            rect.SetBottom( cyHeight );
        }
    }
    if( m_pCloseButton ) m_pCloseButton->Show( (m_showHeader & m_showClosebutton) ? true : false );

    Refresh();
}

void wxPaneBase::CalcHeaderSize() {
    // header size calculation
    wxFont font = GetTitleFont();
    int cyFont = font.GetPointSize() + (HEADER_BORDER*2);
    int cyBtn = m_toolButtonSize.y + (HEADER_BORDER*2);

    // which is largest, text height or button height?
    m_headerSize = max( cyFont, cyBtn );
}

const wxFont & wxPaneBase::GetTitleFont() {
    static wxFont font;
    font = wxSystemSettings::GetFont( wxSYS_DEFAULT_GUI_FONT );
    return font;
}

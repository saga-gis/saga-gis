/////////////////////////////////////////////////////////////////////////////
// Name:        msw/toolbutton_msw.cpp
// Purpose:     wxToolButton implementation.
// Author:      Mark McCormack
// Modified by:
// Created:     20/05/04
// RCS-ID:      $Id: toolbutton_msw.cpp,v 1.2 2006-08-16 18:27:30 oconrad Exp $
// Copyright:   (c) 2004 Mark McCormack
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include <wx/toolbutton.h>

#ifndef wxEX_USE_GENERIC_TOOLBUTTON

#include <wx/dcclient.h>
#include <wx/gdi.h>
#include <wx/settings.h>
#include <wx/tooltip.h>

#ifdef _WIN32_IE
#undef _WIN32_IE
#endif

// This definition is required otherwise CDDS_PREPAINT definition
// (which is placed into commctrl.h) could be skipped if _WIN32_IE
// is defined to a value lower than 0x0300 
// (like, for example, MinGW does...)
#define _WIN32_IE       0x500

#include <commctrl.h>       // windows common controls
#include <windowsx.h>       // macros

// Need to define the LongToPtr macro if it is not defined by the environment
#ifndef LongToPtr
//#undef LONG_PTR;
//typedef long  LONG_PTR;
typedef long* PLONG_PTR;
#define LongToPtr( l )   ((VOID*)(LONG_PTR)((long)l))
#endif

// ----------------------------------------------------------------------------
// wxPane constants & wx-macros
// ----------------------------------------------------------------------------

IMPLEMENT_DYNAMIC_CLASS( wxToolButton, wxWindow )

BEGIN_EVENT_TABLE( wxToolButton, wxWindow )
    EVT_SIZE( wxToolButton::OnSize )
    EVT_ERASE_BACKGROUND( wxToolButton::OnErase )
END_EVENT_TABLE()

// this combination of setting sizes appears to give us an actual button size of (width x height)
#define CALC_SIZE( w, h ) (LPARAM) MAKELONG( w-7, h-6 )

// ----------------------------------------------------------------------------
// wxPane implementation
// ----------------------------------------------------------------------------

bool wxToolButton::Create( wxWindow *parent, 
                           wxWindowID id,
                           const wxPoint& pos,
                           const wxSize& size,
                           long style, 
                           const wxString& name ) {
    // common initialisation
    if( !CreateControl( parent, id, pos, size, style, wxDefaultValidator, name ) ) {
        return FALSE;
    }

    createToolbar( pos, size, style, id );

    return TRUE;
}

void wxToolButton::Init() {
    m_toolButtonSize.cx = 0;
    m_toolButtonSize.cy = 0;

    wxToolButtonBase::Init();
}

wxToolButton::~wxToolButton() {
}

bool wxToolButton::MSWOnNotify( int WXUNUSED(idCtrl), WXLPARAM lParam, WXLPARAM *result ) {
    // First check if this applies to us
    NMHDR *hdr = (NMHDR *)lParam;
    switch( hdr->code ) {
        case NM_CUSTOMDRAW:
        {
            LPNMCUSTOMDRAW lpNMCustomDraw = (LPNMCUSTOMDRAW) lParam;
            if( lpNMCustomDraw->hdr.hwndFrom == GetHwnd() ) {  // ignore the ctrlId, we only have one button
                // control pre-paint
                if( lpNMCustomDraw->dwDrawStage == CDDS_PREPAINT ) {
                    *result = CDRF_NOTIFYITEMDRAW;  // ask for per-item draw notifications
                }

                // item pre-paint
                if( lpNMCustomDraw->dwDrawStage == CDDS_ITEMPREPAINT ) {
                    HDC hdc = lpNMCustomDraw->hdc;
                    RECT& rc = lpNMCustomDraw->rc;

                    // draw item background
                    ::FillRect( hdc, &rc, (HBRUSH)LongToPtr(COLOR_3DFACE + 1) );

                    *result = CDRF_NOTIFYPOSTPAINT; //  ask for post-paint item notification
                }

                // item post-paint
                if( lpNMCustomDraw->dwDrawStage == CDDS_ITEMPOSTPAINT ) {
                    RECT rc = lpNMCustomDraw->rc;
                    HDC hdc = lpNMCustomDraw->hdc;

                    // drawing rect manipulate
                    RECT rcDraw = rc;
                    if( (lpNMCustomDraw->uItemState & CDIS_SELECTED) != 0 ) {
                        // offset draw if item is selected
                        ::OffsetRect( &rcDraw, 1, 1 );
                    }

                    // draw the item 
                    wxClientDC dc;
                    dc.SetHDC( (WXHDC)hdc );
                    wxRect cr( wxPoint( rcDraw.left, rcDraw.top ), wxPoint( rcDraw.right, rcDraw.bottom ) );
                    drawButtonImage( dc, cr );

                    *result = CDRF_DODEFAULT;    // continue with the default item painting
                }
            }

            return true;
        }

        // don't need to break since we always return TRUE
        //break;
        
#if wxUSE_TOOLTIPS
        case TTN_NEEDTEXTA:
        case TTN_NEEDTEXTW:
        {
            // ask the common tool-tip handler to display
            wxToolTip * pToolTip = GetToolTip();
            if( pToolTip ) {
                return HandleTooltipNotify( hdr->code, lParam, pToolTip->GetTip() );
            }
        }
        break;
#endif
    }

    return false;
}

bool wxToolButton::MSWCommand( WXUINT WXUNUSED(param), WXWORD WXUNUSED(id) ) {
    // create button event
    sendClickEvent();

    return true;    
}

void wxToolButton::createToolbar( const wxPoint& pos, const wxSize& size, long style, wxWindowID id ) {
    // create
    style |= CCS_NODIVIDER | CCS_NORESIZE | CCS_NOPARENTALIGN | CCS_NOMOVEY | TBSTYLE_TOOLTIPS | TBSTYLE_FLAT;
    WXWORD exStyle = 0;
    if( !MSWCreateControl( TOOLBARCLASSNAME, style, pos, size, _T(""), exStyle ) ) {
        return;
    }

    // toolbar-specific post initialisation
    SendMessage( GetHwnd(), TB_BUTTONSTRUCTSIZE, sizeof(TBBUTTON), 0 );
    SendMessage( GetHwnd(), TB_SETBITMAPSIZE, 0, CALC_SIZE( size.GetWidth(), size.GetHeight() ) );

    // add button
    TBBUTTON button = { 0 };
    button.fsState = TBSTATE_ENABLED;
    button.fsStyle = TBSTYLE_BUTTON;
    button.idCommand = id;
    SendMessage( GetHwnd(), TB_ADDBUTTONS, 1, (LPARAM)&button );

    // save actual size of button used by the toolbar
    DWORD tbSize = SendMessage( GetHwnd(), TB_GETBUTTONSIZE, 0, 0 );
    m_toolButtonSize.cx = LOWORD( tbSize );
    m_toolButtonSize.cy = HIWORD( tbSize );
}

void wxToolButton::OnSize( wxSizeEvent &event ) {
    // update
    wxSize size = event.GetSize();
    int sx = size.GetWidth();
    int sy = size.GetHeight();
    SendMessage( GetHwnd(), TB_SETBITMAPSIZE, 0, CALC_SIZE(sx, sy) );
}

void wxToolButton::OnErase( wxEraseEvent& WXUNUSED(event) ) {
}

#endif  // wxEX_USE_GENERIC_TOOLBUTTON

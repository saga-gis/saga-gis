/////////////////////////////////////////////////////////////////////////////
// Name:        util.cpp
// Purpose:     wxUtil implementation.
// Author:      Mark McCormack
// Modified by:
// Created:     07/06/04
// RCS-ID:      $Id: util.cpp,v 1.1.1.1 2006-06-26 15:43:36 oconrad Exp $
// Copyright:   (c) 2004 Mark McCormack
// Licence:     wxWindows license
/////////////////////////////////////////////////////////////////////////////

#include <wx/util.h>

#if defined(__WXMSW__)
#include <wx/msw/private.h>
#endif

namespace wxUtil {

void WriteString( wxOutputStream& stream, const wxString& string ) {
    // write wxString
    wxCharBuffer buffer = string.mb_str(wxConvLibc);
    size_t size = string.Length();
    stream.Write(&size, sizeof(size));
    stream.Write(buffer, size);
}

wxString ReadString( wxInputStream& stream ) {
    // read wxString
    size_t size = 0;
    stream.Read( &size, sizeof( size ) );
    char* data = new char[size+1];
    stream.Read(data, size);
    data[size] = 0;
    wxString string(data, wxConvLibc);
    delete data;
    return string;
}

bool SaveCheckFilename( const wxString& file ) {
    if( wxFile::Exists( file ) && !wxFile::Access( file, wxFile::write ) ) {
        return false;
    }
    return true;
}

bool LoadCheckFilename( const wxString& file ) {
    if( !wxFile::Exists( file ) ) {
        return false;
    }
    return true;
}

void WriteWindowLayout( wxOutputStream& stream, wxWindow * pWindow ) {
    wxASSERT(pWindow);

    // frame/window
    WriteString( stream, wxT("<window>") );

    wxRect size = pWindow->GetRect();
    wxFrame * pFrame = wxDynamicCast( pWindow, wxFrame );
    if( pFrame ) {
        // is frame, handle maximized
        bool isMaximized = pFrame->IsMaximized();
        stream.Write( &isMaximized, sizeof( isMaximized ) );

#if defined(__WXMSW__)
        if( isMaximized ) {
            // need to use raw API to get un-maximized placement on Win32
            WINDOWPLACEMENT wndPl;
            ::GetWindowPlacement( (HWND)pFrame->GetHandle(), &wndPl );
            RECT r = wndPl.rcNormalPosition;
            size.SetPosition( wxPoint( r.left, r.top ) );
            size.SetSize( wxSize( r.right - r.left, r.bottom - r.top ) );
        }
#endif
    }
    else {
        // dummy data
        bool isMaximized = false;
        stream.Write( &isMaximized, sizeof( isMaximized ) );
    }

    // write window position & size
    stream.Write( &size.x, sizeof( size.x ) );
    stream.Write( &size.y, sizeof( size.y ) );
    stream.Write( &size.width, sizeof( size.width ) );
    stream.Write( &size.height, sizeof( size.height ) );
}

void ReadWindowLayout( wxInputStream& stream, wxWindow * pWindow ) {
    wxASSERT(pWindow);

    // frame/window
    wxString windowTag = ReadString( stream );
    if( windowTag == wxT("<window>") ) {
        // frame
        bool maximized = false;
        stream.Read( &maximized, sizeof( maximized ) );
        wxRect size;
        stream.Read( &size.x, sizeof( size.x ) );
        stream.Read( &size.y, sizeof( size.y ) );
        stream.Read( &size.width, sizeof( size.width ) );
        stream.Read( &size.height, sizeof( size.height ) );

        wxFrame * pFrame = wxDynamicCast( pWindow, wxFrame );
        if( pFrame ) {
#if defined(__WXMSW__)
            if( maximized ) {
                // need to use raw API to set maximized placement on Win32
                WINDOWPLACEMENT wndPl;
                ::GetWindowPlacement( (HWND)pFrame->GetHandle(), &wndPl );
                wndPl.showCmd = SW_MAXIMIZE;
                RECT r = { size.x, size.y, size.x + size.width, size.y + size.height };
                wndPl.rcNormalPosition = r;
                ::SetWindowPlacement( (HWND)pFrame->GetHandle(), &wndPl );
                return;
            }
#endif
        }
        pWindow->SetSize( size );
    }
}

}


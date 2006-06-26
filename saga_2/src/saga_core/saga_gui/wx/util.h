///////////////////////////////////////////////////////////////////////////////
// Name:        wx/util.h
// Purpose:     wxUtil namespace
// Author:      Mark McCormack
// Modified by:
// Created:     07/06/04
// RCS-ID:      $Id: util.h,v 1.1.1.1 2006-06-26 15:43:06 oconrad Exp $
// Copyright:   (c) 2004 Mark McCormack
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////

#ifndef _WX_UTIL_H_
#define _WX_UTIL_H_

// ----------------------------------------------------------------------------
// headers
// ----------------------------------------------------------------------------

#include <wx/string.h>
#include <wx/stream.h>
#include <wx/wfstream.h>

#include <wx/window.h>
#include <wx/frame.h>

#include "wx/export.h"

// ----------------------------------------------------------------------------
// wxUtil
// ----------------------------------------------------------------------------

namespace wxUtil {
    
    // wxString <-> wxStream
    void WriteString( wxOutputStream& stream, const wxString& string );
    wxString ReadString( wxInputStream& stream );

    // filename checking
    bool SaveCheckFilename( const wxString& file );
    bool LoadCheckFilename( const wxString& file );

    // wxWindow/wxFrame <-> wxStream
    void WXDLLIMPEXP_DI WriteWindowLayout( wxOutputStream& stream, wxWindow * pWindow );
    void WXDLLIMPEXP_DI ReadWindowLayout( wxInputStream& stream, wxWindow * pWindow );

}   // namespace wxUtil

#endif
    // _WX_UTIL_H_

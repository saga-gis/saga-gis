///////////////////////////////////////////////////////////////////////////////
// Name:        wx/export.h
// Purpose:     Defines shared build macros and some other stuff for 
//              conditional-compilation
// Author:      Mark McCormack
// Modified by: Francesco Montorsi
// Created:     23/02/04
// RCS-ID:      $Id: export.h,v 1.1.1.1 2006-06-26 15:43:06 oconrad Exp $
// Copyright:   (c) 2004 Mark McCormack
// Licence:     wxWindows licence
///////////////////////////////////////////////////////////////////////////////


#ifndef _WX_DI_EXPORT_H_
#define _WX_DI_EXPORT_H_


// ----------------
// MACROS
// ----------------

// the WXMAKINGDLL_DOCKIT and the WXUSINGDLL macros are (eventually) defined
// by the makefile/project file used to compile the library...
#if defined(WXMAKINGDLL_DOCKIT)
#	define WXDLLIMPEXP_DI WXEXPORT
#	define WXDLLIMPEXP_DATA_DI(type) WXEXPORT type
#elif defined(WXUSINGDLL)
#	define WXDLLIMPEXP_DI WXIMPORT
#	define WXDLLIMPEXP_DATA_DI(type) WXIMPORT type
#else // not making nor using DLL
#	define WXDLLIMPEXP_DI
#	define WXDLLIMPEXP_DATA_DI(type) type
#endif


//! If defined, then the
//!   - wxLayoutManager::SaveToXML
//!   - wxLayoutManager::LoadFromXML
//! functions will be enabled.
//! When enabled these functions will require that the program using
//! wxDockIt will be linked with the XML library of wxWidgets: wxbaseXX_xml.lib.
#define wxDI_USE_XMLLOADSAVE



// we redefine the WXUNUSED macro here so that doxygen knows
// how to expand WXUNUSED when it finds it....
#ifdef WXUNUSED
#undef WXUNUSED
#endif

#define WXUNUSED(identifier) /* identifier */



#endif		// _WX_DI_EXPORT_H_

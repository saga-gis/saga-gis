/* This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at http://mozilla.org/MPL/2.0/. */

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//			The Module Link Library Interface			 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// 1. Include the appropriate SAGA-API header...

#include "MLB_Interface.h"


//---------------------------------------------------------
// 2. Place general module library informations here...
CSG_String Get_Info(int i)
{
	switch( i )
	{
	case TLB_INFO_Name:	default:
		return( _TL("RDB2") );

	case TLB_INFO_Category:
		return( _TL("Import/Export") );

	case TLB_INFO_Author:
		return( SG_T("R.Gschweicher (c) 2016") );

	case TLB_INFO_Description:
		return( _TL("Tools to import RDB2 files.") );

	case TLB_INFO_Version:
		return( SG_T("0.1") );

	case TLB_INFO_Menu_Path:
		return( _TL("Riegl|RDB2") );
	}
}


//---------------------------------------------------------
// 3. Include the headers of your modules here...

#include "rdb2_import.h"
#include "rdb2_info.h"


//---------------------------------------------------------
// 4. Allow your modules to be created here...

CSG_Tool *		Create_Tool(int i)
{
	switch( i )
	{
	case  0:	return( new CRDB2_Import );
	case  1:	return( new CRDB2_Info );
	default:	return( NULL );
	}

	return( NULL );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
//{{AFX_SAGA

	TLB_INTERFACE

//}}AFX_SAGA

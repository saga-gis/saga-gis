
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

const char * Get_Info(int i)
{
	switch( i )
	{
	case MLB_INFO_Name:	default:
		return( _TL("Import/Export - ESRI E00") );

	case MLB_INFO_Author:
		return( _TL("(c) 2004 by Olaf Conrad, Goettingen") );

	case MLB_INFO_Description:
		return( _TL("Import and export filter for ESRI's E00 file exchange format.") );

	case MLB_INFO_Version:
		return( "1.0" );

	case MLB_INFO_Menu_Path:
		return( _TL("File|ESRI E00") );
	}
}


//---------------------------------------------------------
// 3. Include the headers of your modules here...

#include "ESRI_E00_Import.h"


//---------------------------------------------------------
// 4. Allow your modules to be created here...

CModule *		Create_Module(int i)
{
	CModule	*pModule;

	switch( i )
	{
	case 0:
		pModule	= new CESRI_E00_Import;
		break;

	default:
		pModule	= NULL;
		break;
	}

	return( pModule );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
//{{AFX_SAGA

	MLB_INTERFACE

//}}AFX_SAGA

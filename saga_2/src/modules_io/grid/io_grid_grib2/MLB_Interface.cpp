/*
 * Thomas Schorr 2007
 */
 
#include "MLB_Interface.h"

const char *	Get_Info(int i)
{
	switch( i )
	{
	case MLB_INFO_Name:	default:
		return( _TL("GRIB2 import") );

	case MLB_INFO_Author:
		return( _TL("Thomas Schorr (c) 2007") );

	case MLB_INFO_Description:
		return( _TL("Import GRIB records") );

	case MLB_INFO_Version:
		return( "0.1" );

	case MLB_INFO_Menu_Path:
		return( _TL("File|Grid") );
	}
}

#include "grib2_import.h"

CSG_Module *		Create_Module(int i)
{
	CSG_Module	*pModule;

	switch( i )
	{
	case 0:
		pModule	= new SGGrib2Import;
		break;

	default:
		pModule	= NULL;
		break;
	}

	return( pModule );
}

//{{AFX_SAGA

	MLB_INTERFACE

//}}AFX_SAGA

/**********************************************************
 * Version $Id$
 *********************************************************/
/*
 * Thomas Schorr 2007
 */
 
#include "MLB_Interface.h"

CSG_String Get_Info(int i)
{
	switch( i )
	{
	case MLB_INFO_Name:	default:
		return( _TL("Import/Export - GRIB Files") );

	case MLB_INFO_Author:
		return( _TL("Thomas Schorr (c) 2007") );

	case MLB_INFO_Description:
		return( _TW(
			"Import and export of GRIdded Binary (GRIB) files using the "
			"g2clib library of the National Center for Environmental Prediction (NCEP). "
			"Find more information at "
			"<a target=\"_blank\" href=\"http://www.nco.ncep.noaa.gov/\">"
			"http://www.nco.ncep.noaa.gov/</a>"
		));

	case MLB_INFO_Version:
		return( _TL("0.1") );

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

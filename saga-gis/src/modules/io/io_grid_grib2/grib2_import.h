/**********************************************************
 * Version $Id: grib2_import.h 1246 2011-11-25 13:42:38Z oconrad $
 *********************************************************/
/*
 * Thomas Schorr 2007
 */
 
#ifndef HEADER_INCLUDED__grib2_import_H
#define HEADER_INCLUDED__grib2_import_H


#include "MLB_Interface.h"
extern "C" {
#include "g2clib-1.0.4/grib2.h"
}


class SGGrib2Import : public CSG_Module
{
public:
	SGGrib2Import(void);
	virtual ~SGGrib2Import(void);

	virtual CSG_String		Get_MenuPath	(void)		{	return( _TL("R:Import") );	}


protected:

	virtual bool			On_Execute		(void);


private:
	CSG_Grid	*pGrid;
	gribfield *gf;
	bool handle_latlon( void );
	bool handle_polar_stereographic( void );

};

#endif // #ifndef HEADER_INCLUDED__grib2_import_H

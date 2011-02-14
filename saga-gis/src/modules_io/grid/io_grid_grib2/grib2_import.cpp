/**********************************************************
 * Version $Id$
 *********************************************************/
/*
 * Thomas Schorr 2007
 */
 
#define DEBUG 1
 
#include "grib2_import.h"
#include <sys/types.h>
#include <sys/stat.h>

#ifdef _SAGA_LINUX
#include <unistd.h>
#endif

#ifdef _SAGA_MSW
double rint(double x)
{
	return( (int)(0.5 + x) );
}
#endif


SGGrib2Import::SGGrib2Import(void)
{
	Set_Name		(_TL("Import GRIB2 record"));

	Set_Author		(_TL("Copyright (c) 2007 by Thomas Schorr"));

	Set_Description	(_TW(
		"Import a GRIB2 record."
		"Under development."
	));

	Parameters.Add_Grid_Output(
		NULL	, "OUT_GRID"	, _TL("Grid"),
		_TL("")
	);

	Parameters.Add_FilePath(
		NULL	, "FILE"	, _TL("File"),
		_TL(""),
		_TW(
			"GRIdded Binary (GRIB) files|*.grib*;*.grb*|"
			"GRIB2 files|*.grib2;*.grb2|"
			"All Files|*.*"
		)
	);
}

SGGrib2Import::~SGGrib2Import(void)
{}


bool SGGrib2Import::On_Execute(void)
{
	size_t num_bytes;
	FILE		*fp;
	CSG_String	FileName;
	unsigned char *raw, *help;
	struct stat st;

	FileName	= Parameters("FILE")	->asString();
	gf = NULL;

	if ( stat( FileName.b_str(), &st ) == -1 )
		return false;
	raw = ( unsigned char * ) malloc( st.st_size );
	if ( raw == NULL ) return false;
	if( ( fp = fopen(FileName.b_str(), "rb")) != NULL )	{
		num_bytes = 0;
		help = raw;
		while ( feof( fp ) == 0 && ferror( fp ) == 0 ) {
			num_bytes = fread( help, sizeof( unsigned char ), 1024, fp );
			help += num_bytes;
		}
		fclose( fp );
		g2_getfld( raw, 1, 1, 1, &gf );
		switch ( gf->igdtnum ) {
			case 0:
				if ( ! handle_latlon() ) return false;
				break;
			case 20:
				if ( ! handle_polar_stereographic() ) return false;
				break;
			default:
				fprintf( stderr, "unhandled grid definition template no.: %d\n", gf->igdtnum );
				if ( ! handle_latlon() ) return false; 
		}
		pGrid->Set_Name( SG_File_Get_Name(FileName, false) );	// no need to use wx!!!
//		pGrid->Set_Name( wxFileName( wxString( FileName ) ).GetName() );
		Parameters( "OUT_GRID" )->Set_Value( pGrid );
		g2_free( gf );
		free( raw );
		return( true );
	}
	return( false );
}

bool SGGrib2Import::handle_latlon( void ) {
	int x, y, nx, ny;
	double cellsize, xmin, ymin;

	if( gf->igdtmpl == NULL || gf->fld == NULL )	// possible if jpeg/png is not supported!?
		return( false );

	nx = ( int ) gf->igdtmpl[ 7 ];
	ny = ( int ) gf->igdtmpl[ 8 ];
	cellsize = ( double ) ( gf->igdtmpl[ 16 ] * 1e-6 );
	xmin = ( double ) ( gf->igdtmpl[ 12 ] * 1e-6 ) + cellsize / 2.0;
	ymin = ( double ) ( gf->igdtmpl[ 11 ] * 1e-6 ) - 90.0 + cellsize / 2.0;
#ifdef DEBUG
	fprintf( stderr, "%d %d %f %f %f\n", nx, ny, cellsize, xmin, ymin );
#endif
	
	if ( ( nx == -1 ) || ( ny == -1 ) || 
		( ( pGrid = SG_Create_Grid( SG_DATATYPE_Float, nx, ny, cellsize, xmin, ymin ) ) == NULL ) )
		return false;
	for ( x = 0; x < nx; x++ ) {
		for ( y = 0; y < ny; y ++ )
			pGrid->Set_Value( x, ny - y - 1, gf->fld[ y * nx + x ] );
	}
	return true;
}

bool SGGrib2Import::handle_polar_stereographic( void ) {
	/* XXX fix me XXX*/
	int x, y, nx, ny;
	double cellsize, xmin, ymin;

	nx = ( int ) gf->igdtmpl[ 7 ];
	ny = ( int ) gf->igdtmpl[ 8 ];
	cellsize = ( double ) ( gf->igdtmpl[ 14 ] * 1e-3 );
	xmin = ( double ) ( gf->igdtmpl[ 10 ] ); 
	ymin = ( double ) ( gf->igdtmpl[ 9 ] );
#ifdef DEBUG
	fprintf( stderr, "%d %d %f %f %f\n", nx, ny, cellsize, xmin, ymin );
#endif
	
	if ( ( nx == -1 ) || ( ny == -1 ) || 
		( ( pGrid = SG_Create_Grid( SG_DATATYPE_Float, nx, ny, cellsize, xmin, ymin ) ) == NULL ) )
		return false;
	for ( x = 0; x < nx; x++ ) {
		for ( y = 0; y < ny; y ++ )
			pGrid->Set_Value( x, y, gf->fld[ y * nx + x ] );
	}
	return true;
}

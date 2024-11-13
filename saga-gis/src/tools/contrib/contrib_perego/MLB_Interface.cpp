
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                   contrib_a_perego                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   TLB_Interface.cpp                   //
//                                                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                                                       //
//    by Alessandro Perego (Italy)                       //
//                                                       //
//    e-mail:     alper78@alice.it                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// 1. Include the appropriate SAGA-API header...

#include <saga_api/saga_api.h>


//---------------------------------------------------------
// 2. Place general tool library information here...

CSG_String Get_Info(int i)
{
	switch( i )
	{
	case TLB_INFO_Name:	default:
		return( _TL("Filter (Perego 2009)") );

	case TLB_INFO_Category:
		return( _TL("Grid") );

	case TLB_INFO_Author:
		return( _TL("Alessandro Perego") );

	case TLB_INFO_Description:
		return( _TW(
			"Contributions from Alessandro Perego. Go to "
			"<a href=\"http://www.alspergis.altervista.org/software/software.html#saga\">www.alspergis.altervista.org</a> "
			"for further information."
		));

	case TLB_INFO_Version:
		return( "1.0" );

	case TLB_INFO_Menu_Path:
		return( _TL("Grid|Filter") );
	}
}


//---------------------------------------------------------
// 3. Include the headers of your tools here...

#include "A1WiTh.h"
#include "A2WiTh.h"
#include "A3WiTh.h"
#include "AvWiMa1.h"
#include "AvWiMa2.h"
#include "destriping1.h"
#include "destriping2.h"
#include "directional1.h"


//---------------------------------------------------------
// 4. Allow your tools to be created here...

CSG_Tool *		Create_Tool(int i)
{
	switch( i )
	{
	case  0:	return( new CA1WiTh );
	case  1:	return( new CA2WiTh );
	case  2:	return( new CA3WiTh );
	case  3:	return( new CAvWiMa1 );
	case  4:	return( new CAvWiMa2 );
	case  5:	return( new Cdestriping1 );
	case  6:	return( new Cdestriping2 );
	case  7:	return( new Cdirectional1 );

	//-----------------------------------------------------
	case  8:	return( NULL );
	default:	return( TLB_INTERFACE_SKIP_TOOL );
	}
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

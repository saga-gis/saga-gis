
///////////////////////////////////////////////////////////
//														 //
//			The Module Link Library Interface			 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "MLB_Interface.h"

//---------------------------------------------------------
const SG_Char * Get_Info(int i)
{
	switch( i )
	{
	case MLB_INFO_Name:	default:
		return( _TL("Grid - Calculus BSL") );

	case MLB_INFO_Author:
		return( SG_T("SAGA User Group Associaton (c) 2009") );

	case MLB_INFO_Description:
		return( _TL("Boehner's Simple Language (BSL) is a macro script language for grid based calculations.") );

	case MLB_INFO_Version:
		return( SG_T("1.0") );

	case MLB_INFO_Menu_Path:
		return( _TL("Grid|Calculus") );
	}
}

//---------------------------------------------------------
#include "bsl_interpreter.h"

//---------------------------------------------------------
CSG_Module *		Create_Module(int i)
{
	switch( i )
	{
	case 0:	return( new CBSL_Interpreter(false) );
	case 1:	return( new CBSL_Interpreter(true) );
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

	MLB_INTERFACE

//}}AFX_SAGA

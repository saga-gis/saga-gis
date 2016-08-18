/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//														 //
//           The Tool Link Library Interface             //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "MLB_Interface.h"

//---------------------------------------------------------
CSG_String Get_Info(int i)
{
	switch( i )
	{
	case TLB_INFO_Name:	default:
		return( _TL("Calculus BSL") );

	case TLB_INFO_Category:
		return( _TL("Grid") );

	case TLB_INFO_Author:
		return( SG_T("SAGA User Group Associaton (c) 2009") );

	case TLB_INFO_Description:
		return( _TL("Boehner's Simple Language (BSL) is a macro script language for grid based calculations.") );

	case TLB_INFO_Version:
		return( SG_T("1.0") );

	case TLB_INFO_Menu_Path:
		return( _TL("Grid|Calculus") );
	}
}

//---------------------------------------------------------
#include "bsl_interpreter.h"

//---------------------------------------------------------
CSG_Tool *		Create_Tool(int i)
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

	TLB_INTERFACE

//}}AFX_SAGA

/**********************************************************
 * Version $Id: TLB_Interface.cpp 1246 2011-11-25 13:42:38Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//														 //
//           The Tool Link Library Interface             //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// 1. Include the appropriate SAGA-API header...

#include "MLB_Interface.h"


//---------------------------------------------------------
// 2. Place general tool library information here...

CSG_String Get_Info(int i)
{
	switch( i )
	{
	case TLB_INFO_Name:	default:
		return( _TL("ESRI E00") );

	case TLB_INFO_Category:
		return( _TL("Import/Export") );

	case TLB_INFO_Author:
		return( SG_T("O. Conrad (c) 2004") );

	case TLB_INFO_Description:
		return( _TL("Import and export filter for ESRI's E00 file exchange format.") );

	case TLB_INFO_Version:
		return( SG_T("1.0") );

	case TLB_INFO_Menu_Path:
		return( _TL("File|ESRI E00") );
	}
}


//---------------------------------------------------------
// 3. Include the headers of your tools here...

#include "ESRI_E00_Import.h"


//---------------------------------------------------------
// 4. Allow your tools to be created here...

CSG_Tool *		Create_Tool(int i)
{
	CSG_Tool	*pTool;

	switch( i )
	{
	case 0:
		pTool	= new CESRI_E00_Import;
		break;

	default:
		pTool	= NULL;
		break;
	}

	return( pTool );
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

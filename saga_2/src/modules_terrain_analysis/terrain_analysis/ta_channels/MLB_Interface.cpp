
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      ta_channels                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   MLB_Interface.cpp                   //
//                                                       //
//                 Copyright (C) 2003 by                 //
//                      Olaf Conrad                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation; version 2 of the License.   //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//			The Module Link Library Interface			 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "MLB_Interface.h"


//---------------------------------------------------------
const char * Get_Info(int i)
{
	switch( i )
	{
	case MLB_INFO_Name:	default:
		return( _TL("Terrain Analysis - Channels") );

	case MLB_INFO_Author:
		return( _TL("Olaf Conrad, Victor Olaya (c) 2001-4") );

	case MLB_INFO_Description:
		return( _TL("Tools for (grid based) digital terrain analysis.") );

	case MLB_INFO_Version:
		return( "1.0" );

	case MLB_INFO_Menu_Path:
		return( _TL("Terrain Analysis|Channels") );
	}
}


//---------------------------------------------------------
#include "ChannelNetwork.h"
#include "Watersheds.h"
#include "Watersheds_ext.h"
#include "ChannelNetwork_Altitude.h"
#include "ChannelNetwork_Distance.h"
#include "D8_Flow_Analysis.h"
#include "Strahler.h"


//---------------------------------------------------------
CSG_Module *		Create_Module(int i)
{
	CSG_Module	*pModule;

	switch( i )
	{
	case 0:
		pModule	= new CChannelNetwork;
		break;

	case 1:
		pModule	= new CWatersheds;
		break;
	
	case 2:
		pModule	= new CWatersheds_ext;
		break;

	case 3:
		pModule	= new CChannelNetwork_Altitude;
		break;

	case 4:
		pModule	= new CChannelNetwork_Distance;
		break;

	case 5:
		pModule	= new CD8_Flow_Analysis;
		break;

	case 6:
		pModule	= new CStrahler;
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

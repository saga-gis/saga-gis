/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                  points_view_module                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 points_view_module.cpp                //
//                                                       //
//                 Copyright (C) 2009 by                 //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute for Geography                //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "points_view_module.h"

#include "points_view_dialog.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPoints_View_Module::CPoints_View_Module(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("[deprecated] Point Cloud Viewer"));

	Set_Author		(SG_T("O. Conrad (c) 2009"));

	Set_Description	(_TW(
		"This module is a 3D viewer for SAGA Point Clouds. Amongst others, "
		"the viewer supports attribute based coloring, a map view to control "
		"the area-of-interest, point size scaling based on distance, level "
		"of detail control and an anaglyph modus (to use with red/blue or "
		"red/green glasses).\n\n"
		"Module development was supported by "
		"<a href=\"http://www.laserdata.at\">http://Laserdata GmbH, Austria</a>\n"
		"and "
		"<a href=\"http://www.alp-s.at\">http://alpS GmbH, Austria</a>\n"
	));

	//-----------------------------------------------------
	Parameters.Add_PointCloud(
		NULL	, "POINTS"		, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPoints_View_Module::On_Execute(void)
{
	if( !SG_UI_Get_Window_Main() )
	{
		Message_Add(_TL("point cloud viewer can only be run from graphical user interface"));

		return( false );
	}

	CSG_PointCloud	*pPoints	= Parameters("POINTS")->asPointCloud();

	if( pPoints->Get_Count() <= 0 )
	{
		Message_Add(_TL("point cloud viewer will not be started, because point cloud has no points"));

		return( false );
	}

	CPoints_View_Dialog	dlg(pPoints);

	dlg.ShowModal();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

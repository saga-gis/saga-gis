/**********************************************************
 * Version $Id: points_view_module.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                    tin_view_module                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  tin_view_module.cpp                  //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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
#include "tin_view_module.h"

#include "tin_view_dialog.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTIN_View_Module::CTIN_View_Module(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("TIN Viewer"));

	Set_Author		(SG_T("O. Conrad (c) 2011"));

	Set_Description	(_TW(
		"This module is a 3D viewer for SAGA TINs. Amongst others, "
		"the viewer supports attribute based coloring, a map view to control "
		"the area-of-interest, point size scaling based on distance, level "
		"of detail control and an anaglyph modus (to use with red/blue or "
		"red/green glasses).\n\n"
	));

	//-----------------------------------------------------
	CSG_Parameter	*pNode	= Parameters.Add_TIN(
		NULL	, "TIN"		, _TL("TIN"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "HEIGHT"	, _TL("Elevation"),
		_TL("")
	);

	Parameters.Add_Table_Field(
		pNode	, "COLOR"	, _TL("Color"),
		_TL("")
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTIN_View_Module::On_Execute(void)
{
	if( !SG_UI_Get_Window_Main() )
	{
		Message_Add(_TL("point cloud viewer can only be run from graphical user interface"));

		return( false );
	}

	CSG_TIN	*pTIN	= Parameters("TIN")->asTIN();

	if( pTIN->Get_Count() <= 0 )
	{
		Message_Add(_TL("point cloud viewer will not be started, because point cloud has no points"));

		return( false );
	}

	CTIN_View_Dialog	dlg(pTIN, Parameters("HEIGHT")->asInt(), Parameters("COLOR")->asInt());

	dlg.ShowModal();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

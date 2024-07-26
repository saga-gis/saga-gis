
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      dev_tools                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   pysaga_tools.cpp                    //
//                                                       //
//                 Copyright (C) 2023 by                 //
//                      Olaf Conrad                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation, either version 2 of the     //
// License, or (at your option) any later version.       //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "pysaga_tools.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPySAGA_Tools::CPySAGA_Tools(void)
{
	Set_Name	("PySAGA Tool Interface Generator");

	Set_Author	("O.Conrad (c) 2023");

	Parameters.Add_Choice  ("", "TARGET"  , "Target"                , "", "single file|one file per library", 1);
	Parameters.Add_FilePath("", "FILE"    , "File"                  , "", SG_T("Python Files|*.py|All Files|*.*"), NULL, true);
	Parameters.Add_FilePath("", "FOLDER"  , "Folder"                , "", NULL, NULL, true, true);
	Parameters.Add_Choice  ("", "NAMING"  , "Function Names"        , "", "tool name|library + tool id", 0);
	Parameters.Add_Bool    ("", "CLEAN"   , "Make Clean"            , "", true);
	Parameters.Add_Bool    ("", "LOADDEFS", "Load Default Libraries", "", true);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CPySAGA_Tools::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("TARGET") )
	{
		pParameters->Set_Enabled("FILE"  , pParameter->asInt() == 0);
		pParameters->Set_Enabled("FOLDER", pParameter->asInt() == 1);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPySAGA_Tools::On_Execute(void)
{
	bool bName  = Parameters["NAMING"].asInt() == 0;
	bool bClean = Parameters["CLEAN" ].asBool();

	if( Parameters["LOADDEFS"].asBool() )
	{
		SG_Get_Tool_Library_Manager().Add_Default_Libraries();
	}

	if( Parameters["TARGET"].asInt() == 0 ) // single file
	{
		return( SG_Get_Tool_Library_Manager().Create_Python_ToolBox(Parameters("FILE")->asString(), bClean, bName, true) );
	}

	return( SG_Get_Tool_Library_Manager().Create_Python_ToolBox(Parameters("FOLDER")->asString(), bClean, bName, false) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

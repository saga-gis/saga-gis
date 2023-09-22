
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

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_FilePath(
		"", "DIRECTORY", "Target Directory",
		"",
		NULL, NULL, true, true
	);

	Parameters.Add_Bool(
		"", "CLEAR"    , "Clear Directory",
		"",
		true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPySAGA_Tools::On_Execute(void)
{
	CSG_String Directory(Parameters("DIRECTORY")->asString());

	if( Parameters("CLEAR")->asBool() )
	{
		CSG_Strings Files;

		SG_Dir_List_Files(Files, Directory);

		for(int i=0; i<Files.Get_Count(); i++)
		{
			SG_File_Delete(Files[i]);
		}
	}

	//-----------------------------------------------------
	int nTools = 0;

	for(int iLibrary=0; iLibrary<SG_Get_Tool_Library_Manager().Get_Count() && Set_Progress(iLibrary, SG_Get_Tool_Library_Manager().Get_Count()); iLibrary++)
	{
		CSG_Tool_Library *pLibrary = SG_Get_Tool_Library_Manager().Get_Library(iLibrary);

		if( !pLibrary->Get_Category    ().Cmp("SAGA Development" )	// generally exclude certain categories/libraries
		||  !pLibrary->Get_Category    ().Cmp("Garden"           )
		||  !pLibrary->Get_Library_Name().Cmp("vis_3d_viewer"    )
		||  !pLibrary->Get_Library_Name().Cmp("grid_calculus_bsl")
		||   pLibrary->Get_Type() == TOOL_CHAINS )
		{
			continue;
		}

		Process_Set_Text(CSG_String::Format("%s: %s", SG_T("Library"), pLibrary->Get_Library_Name().c_str()));

		CSG_String Library(pLibrary->Get_Library_Name());

		CSG_File Stream;

		if( !Stream.Open(SG_File_Make_Path(Directory, Library, "py"), SG_FILE_W, false) )
		{
			continue;
		}

		Stream.Write("#! /usr/bin/env python\n");
		Stream.Write("from PySAGA import saga_api, helper as saga_helper\n\n");

		for(int iTool=0, nAdded=0; iTool<pLibrary->Get_Count(); iTool++)
		{
			CSG_Tool *pTool = pLibrary->Get_Tool(iTool);

			if( pTool == NULL || pTool == TLB_INTERFACE_SKIP_TOOL || pTool->needs_GUI() || pTool->is_Interactive() || pTool->Get_Parameters_Count() > 0 )
			{
				continue;
			}

			CSG_String Function, Code, Name(pTool->Get_Name()); Name.Replace(" ", "_"); Name.Replace("(", ""); Name.Replace(")", ""); Name.Replace("[", ""); Name.Replace("]", ""); Name.Replace(".", ""); Name.Replace(",", "");

			Function = "def Run_" + Name + "(";

			Code.Printf("    Tool = saga_helper.Tool_Wrapper('%s', '%s', '%s')\n", Library.c_str(), pTool->Get_ID().c_str(), pTool->Get_Name().c_str());
			Code +=     "    if Tool.is_Okay():\n";

			CSG_Parameters &Parameters = *pTool->Get_Parameters();

			for(int i=0; i<Parameters.Get_Count(); i++)
			{
				CSG_Parameter &Parameter = *Parameters(i);

				if( i > 0 )
				{
					Function += ", ";
				}

				Function += Parameter.Get_Identifier() + CSG_String("=None");

				Code += "        ";

				if( Parameters[i].is_Input () ) { Code += "Tool.Set_Input "; }
				if( Parameters[i].is_Output() ) { Code += "Tool.Set_Output"; }
				if( Parameters[i].is_Option() ) { Code += "Tool.Set_Option"; }

				Code += CSG_String::Format("('%s', %s)\n", Parameter.Get_Identifier(), Parameter.Get_Identifier());
			}

			Function += "):\n";

			Code += "        return Tool.Execute()\n";
			Code += "    return False\n\n";

			Stream.Write(Function);
			Stream.Write(Code);
		}

		Stream.Close();
	}

	//-----------------------------------------------------
	Message_Fmt("Number of exported tool interfaces: %d", nTools);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

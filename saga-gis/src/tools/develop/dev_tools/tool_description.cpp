
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
//                  tool_description.cpp                 //
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
#include "tool_description.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTool_Description::CTool_Description(void)
{
	Set_Name	("Create Tool Descriptions");

	Set_Author	("O.Conrad (c) 2023");

	Parameters.Add_FilePath("", "FOLDER", "Folder", "", NULL, NULL, true, true);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTool_Description::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTool_Description::On_Execute(void)
{
	CSG_String Folder(Parameters("FOLDER")->asString());

	if( SG_Dir_Exists(Folder) )
	{
		SG_Dir_Delete(Folder, true);
	}

	if( !SG_Dir_Create(Folder, true) )
	{
		Error_Fmt("failed to create folder: %s", Folder.c_str());

		return( false );
	}

	for(int iLibrary=0, nTotal=0; iLibrary<SG_Get_Tool_Library_Manager().Get_Count(); iLibrary++)
	{
		CSG_Tool_Library *pLibrary = SG_Get_Tool_Library_Manager().Get_Library(iLibrary);

		Process_Set_Text(pLibrary->Get_Library_Name());

		for(int iTool=0; iTool<pLibrary->Get_Count() && Set_Progress(nTotal, SG_Get_Tool_Library_Manager().Get_Tool_Count()); iTool++, nTotal++)
		{
			CSG_Tool *pTool = pLibrary->Get_Tool(iTool);

			CSG_String Description = Get_Tool_Description(pLibrary, pTool);

			if( !Description.is_Empty() )
			{
				CSG_File Stream(SG_File_Make_Path(Folder, pLibrary->Get_Library_Name() + "_" + pTool->Get_ID(), "html"), SG_FILE_W);

				if( Stream.is_Open() )
				{
					Stream.Write(Description);
				}
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CTool_Description::Get_Tool_Description(CSG_Tool_Library *pLibrary, CSG_Tool *pTool)
{
	const CSG_String HTML =
		"<!DOCTYPE html>\n"
		"<meta charset=\"UTF-8\">\n"
		"<meta name=\"viewport\" content=\"width=device-width\">\n"
		"<title>Tool %s / SAGA-GIS Tool Library Documentation (v%s)</title>\n" // Tool Name, SAGA Version
		"<link rel=\"stylesheet\" href=\"./lib/styles.css\">\n"
		"<header>\n"
		"  <a href=\"http://saga-gis.org/\"><img class=\"logo\" src=\"./icons/logo.png\" alt=\"Logo\" /></a>\n"
		"  <h1>SAGA-GIS Tool Library Documentation (v%s)</h1>\n" // SAGA Version
		"  <nav>\n"
		"    <span class=\" a2z\"><a href=\"a2z.html\">Tools A-Z</a></span>\n"
		"    <span><a href=\"index.html\">Contents</a></span>\n"
		"    <span><a href=\"./%s.html\">%s - %s</a></span>\n" // Library Name (climate_tools), Category, Library's Name (Climate and Weather Tools)
		"  </nav>\n"
		"</header>\n"
		"\n"
		"<main>\n"
		//"<h1>%s</h1>\n" // Tool Name
		//"<p>%s</p>\n" // Tool Description
		//"<ul>\n"
		//"<li>Author: %s</li>\n" // Author
		//"<li>Menu: %s</li>\n" // Menu
		//"</ul>\n"
		//"\n"
		//"<h3>Parameters</h3>\n%s\n" // Parameters
		"%s\n"
		"<h3>Command-line</h3>\n"
		"<pre class=\"usage\">\n%s" // command-line
		"</pre>\n"
		"</main>\n";

	CSG_String Summary(pTool->Get_Summary()), Usage(pTool->Get_Script(TOOL_SCRIPT_CMD_USAGE));

	CSG_String Description; Description.Printf(HTML.c_str(), pTool->Get_Name().c_str(), SAGA_VERSION, SAGA_VERSION,
		pLibrary->Get_Library_Name(), pLibrary->Get_Category().c_str(), pLibrary->Get_Name().c_str(),
		Summary.c_str(), Usage.c_str()
	);

	return( Description );

	//CSG_String Parameters(Get_Parameters(pTool));

	//CSG_String Usage(pTool->Get_Script(TOOL_SCRIPT_CMD_USAGE));

	//CSG_String Description; Description.Printf(HTML.c_str(), pTool->Get_Name().c_str(), SAGA_VERSION, SAGA_VERSION,
	//	pLibrary->Get_Library_Name(), pLibrary->Get_Category().c_str(), pLibrary->Get_Name().c_str(),
	//	pTool->Get_Name().c_str(), pTool->Get_Description().c_str(), pTool->Get_Author().c_str(), pTool->Get_MenuPath().c_str(),
	//	Parameters.c_str(), Usage.c_str()
	//);

	//return( Description );
}

//---------------------------------------------------------
CSG_String CTool_Description::Get_Parameters(CSG_Tool *pTool)
{
	//"<table>\n"
	//"<tr><th>&nbsp;</th><th>Name</th><th>Type</th><th>Identifier</th><th>Description</th><th>Constraints</th></tr>\n"
	//<tr><td rowspan=\"6\" class=\"labelSection\">Input</td><td>Temperature (*)</td><td>grid, input, optional</td><td><code>T</code></td><td>[Celsius]</td><td>-</td></tr>
	//<tr> <td>Relative Humidity (*)</td><td>grid, input, optional</td><td><code>IN_RH</code></td><td>[%]</td><td>-</td></tr>
	//<tr> <td>Dew Point (*)</td><td>grid, input, optional</td><td><code>IN_DP</code></td><td>[Celsius]</td><td>-</td></tr>
	//<tr><td rowspan=\"7\" class=\"labelSection\">Output</td><td>Saturation Pressure (*)</td><td>grid, output, optional</td><td><code>OUT_VPSAT</code></td><td>[hPa]</td><td>-</td></tr>
	//<tr> <td>Dew Point Difference (*)</td><td>grid, output, optional</td><td><code>OUT_DPDIF</code></td><td>[Celsius]</td><td>-</td></tr>
	//<tr><td rowspan=\"9\" class=\"labelSection\">Options</td><td>Grid System</td><td>grid system</td><td><code>PARAMETERS_GRID_SYSTEM</code></td><td>-</td><td>-</td></tr>
	//<tr> <td>Default</td><td>floating point number</td><td><code>IN_DP_DEFAULT</code></td><td>default value if no grid has been selected</td><td>Minimum: -273.150000<br>Default: 14.000000</td></tr>
	//<tr> <td>Conversion from...</td><td>choice</td><td><code>CONVERSION</code></td><td>-</td><td>Available Choices:<br>[0] Vapor Pressure<br>[1] Specific Humidity<br>[2] Relative Humidity<br>[3] Dew Point<br>Default: 0</td></tr>
	//<tr> <td>Saturation Pressure</td><td>choice</td><td><code>VPSAT_METHOD</code></td><td>Formula used to estimate vapor pressure at saturation.</td><td>Available Choices:<br>[0] Magnus<br>[1] Lowe & Ficke<br>Default: 0</td></tr>
	//"<tr><td colspan=\"6\">(*) optional</td></tr>\n"
	//"</table>\n"

	CSG_Parameters *pParameters = pTool->Get_Parameters();

	CSG_String s("<table>\n<tr><th>Name</th><th>Type</th><th>Identifier</th><th>Description</th></tr>\n");

	for(int i=0; i<pParameters->Get_Count(); i++)
	{
		CSG_Parameter &P = *pParameters->Get_Parameter(i);

		s += CSG_String::Format("<tr><td>%s%s</td><td>%s%s%s</td><td><code>%s</code></td><td>%s</td><td>-</td></tr>",
			P.Get_Name(), P.is_Optional() ? SG_T(" (*)") : SG_T(""),
			SG_Parameter_Type_Get_Name(P.Get_Type()).c_str(), P.is_Input() ? SG_T(", input") : P.is_Output() ? SG_T(", output") : SG_T(""), !P.is_Option() && P.is_Optional() ? SG_T(", optional") : SG_T(""),
			P.Get_Identifier(), P.Get_Description()
		);
	}

	s += "<tr><td colspan=\"6\">(*) optional</td></tr>\n</table>\n";

	return( s );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

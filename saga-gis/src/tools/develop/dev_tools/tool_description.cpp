
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

	if( !SG_Dir_Exists(Folder) && !SG_Dir_Create(Folder, true) )
	{
		Error_Fmt("failed to create folder: %s", Folder.c_str());

		return( false );
	}

	//---------------------------------------------------------
	CSG_Table Libraries;
	Libraries.Add_Field("Category"   , SG_DATATYPE_String);
	Libraries.Add_Field("Library"    , SG_DATATYPE_String);
	Libraries.Add_Field("Name"       , SG_DATATYPE_String);
	Libraries.Add_Field("Description", SG_DATATYPE_String);
	Libraries.Add_Field("Tools"      , SG_DATATYPE_Int   );

	CSG_Table All_Tools;
	All_Tools.Add_Field("Library"    , SG_DATATYPE_String);
	All_Tools.Add_Field("ID"         , SG_DATATYPE_String);
	All_Tools.Add_Field("Tool"       , SG_DATATYPE_String);
	All_Tools.Add_Field("Menu"       , SG_DATATYPE_String);

	//---------------------------------------------------------
	for(int iLibrary=0, nTotal=0; iLibrary<SG_Get_Tool_Library_Manager().Get_Count(); iLibrary++)
	{
		CSG_Tool_Library *pLibrary = SG_Get_Tool_Library_Manager().Get_Library(iLibrary);

		if( !pLibrary->Get_Category().Cmp("SAGA Development") )	// generally exclude certain categories/libraries
		{
			continue;
		}

		Process_Set_Text(pLibrary->Get_Library_Name());

		CSG_Table_Record &Library = *Libraries.Add_Record();
		Library.Set_Value("Category"   , pLibrary->Get_Category    ());
		Library.Set_Value("Library"    , pLibrary->Get_Library_Name());
		Library.Set_Value("Name"       , pLibrary->Get_Name        ());
		Library.Set_Value("Description", pLibrary->Get_Description ());
		Library.Set_Value("Tools"      , pLibrary->Get_Count       ());

		//-------------------------------------------------
		CSG_Table Tools(&All_Tools);

		for(int iTool=0; iTool<pLibrary->Get_Count() && Set_Progress(nTotal, SG_Get_Tool_Library_Manager().Get_Tool_Count()); iTool++, nTotal++)
		{
			CSG_Tool *pTool = pLibrary->Get_Tool(iTool);

			CSG_Table_Record &Tool = *Tools.Add_Record();
			Tool.Set_Value("Library", pLibrary->Get_Library_Name());
			Tool.Set_Value("ID"     , pTool   ->Get_ID          ());
			Tool.Set_Value("Tool"   , pTool   ->Get_Name        ());
			Tool.Set_Value("Menu"   , pTool   ->Get_MenuPath(true));
			All_Tools.Add_Record(&Tool);

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

		//-------------------------------------------------
		Write_Library(Folder, pLibrary, Tools);
	}

	//-----------------------------------------------------
	Write_Libraries(Folder, Libraries);
	Write_All_Tools(Folder, All_Tools);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTool_Description::Write_Libraries(const CSG_String &Folder, CSG_Table &Libraries)
{
	CSG_String s("<!DOCTYPE html>\n<meta charset=\"UTF-8\">\n<meta name=\"viewport\" content=\"width=device-width\">\n");

	s += "<title>SAGA-GIS Tool Library Documentation (v" + CSG_String(SAGA_VERSION) + ")</title>\n";
	s += "<link rel=\"stylesheet\" href=\"./lib/styles.css\">\n";
	s += "<header>\n";
	s += " <a href=\"https://saga-gis.org/\"><img class=\"logo\" src=\"./icons/logo.png\" alt=\"Logo\" /></a>\n";
	s += "  <h1>SAGA-GIS Tool Library Documentation (v" + CSG_String(SAGA_VERSION) + ")</h1>\n";
	s += "  <nav>\n";
	s += "    <span class=\" a2z\"><a href=\"a2z.html\">Tools A-Z</a></span>\n";
	s += "    <span><a href=\"https://saga-gis.org/\">Home</a></span>\n";
	s += "  </nav>\n";
	s += "</header>\n";
	s += "<main>\n";
	s += "<h1>Contents</h1>\n";
	s += "<table>\n";
	s += "<tr><th>Category</th><th>Library</th><th>Description</th><th>Tools</th></tr>\n";

	Libraries.Set_Index(
		Libraries.Get_Field("Category"), TABLE_INDEX_Ascending,
		Libraries.Get_Field("Name"    ), TABLE_INDEX_Ascending
	);

	for(sLong i=0; i<Libraries.Get_Count(); i++)
	{
		CSG_Table_Record &Library = Libraries[i];

		s += CSG_String::Format("<tr><td style=\"white-space: nowrap\">%s</td><td style=\"white-space: nowrap\"><a href=\"%s.html\">%s</a></td><td>%s</td><td class='center'>%s</td></tr>\n",
			Library.asString("Category"   ),
			Library.asString("Library"    ),
			Library.asString("Name"       ),
			Library.asString("Description"),
			Library.asString("Tools"      )
		);
	}

	s += "</table>\n</main>\n";

	CSG_File Stream(SG_File_Make_Path(Folder, "index", "html"), SG_FILE_W);

	return( Stream.is_Open() && Stream.Write(s) );
}

//---------------------------------------------------------
bool CTool_Description::Write_All_Tools(const CSG_String &Folder, CSG_Table &Tools)
{
	CSG_String s("<!DOCTYPE html>\n<meta charset=\"UTF-8\">\n<meta name=\"viewport\" content=\"width=device-width\">\n");

	s += "<title>Full Tool Index / SAGA-GIS Tool Library Documentation (v" + CSG_String(SAGA_VERSION) + ")</title>\n";
	s += "<script src=\"./lib/tablesort.js\"></script>\n";
	s += "<link rel=\"stylesheet\" href=\"./lib/styles.css\">\n";
	s += "<header>\n";
	s += " <a href=\"https://saga-gis.org/\"><img class=\"logo\" src=\"./icons/logo.png\" alt=\"Logo\" /></a>\n";
	s += "  <h1>SAGA-GIS Tool Library Documentation (v" + CSG_String(SAGA_VERSION) + ")</h1>\n";
	s += "  <nav>\n";
	s += "    <span><a href=\"index.html\">Contents</a></span>\n";
	s += "  </nav>\n";
	s += "</header>\n";
	s += "<main>\n";
	s += "<h1>Full Tool Index</h1>\n";
	s += "<table class=\"sortable\">\n";
	s += "<tr><th class=\"sorted-asc\">Tool</th><th>Menu</th></tr>\n";

	Tools.Set_Index(Tools.Get_Field("Tool"));

	for(sLong i=0; i<Tools.Get_Count(); i++)
	{
		CSG_Table_Record &Tool = Tools[i];

		s += CSG_String::Format("<tr><td><a href=\"%s_%s.html\">%s</a></td><td class=\"menuPath\">%s</td></tr>\n",
			Tool.asString("Library"),
			Tool.asString("ID"     ),
			Tool.asString("Tool"   ),
			Tool.asString("Menu"   )
		);
	}

	s += "</table>\n</main>\n";

	CSG_File Stream(SG_File_Make_Path(Folder, "a2z", "html"), SG_FILE_W);

	return( Stream.is_Open() && Stream.Write(s) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTool_Description::Write_Library(const CSG_String &Folder, CSG_Tool_Library *pLibrary, CSG_Table &Tools)
{
	CSG_String s("<!DOCTYPE html>\n<meta charset=\"UTF-8\">\n<meta name=\"viewport\" content=\"width=device-width\">\n");

	s += "<title>Library " + pLibrary->Get_Name() + " / SAGA-GIS Tool Library Documentation (v" + SAGA_VERSION + ")</title>\n";
	s += "<link rel=\"stylesheet\" href=\"./lib/styles.css\">\n";
	s += "<header>\n";
	s += " <a href=\"https://saga-gis.org/\"><img class=\"logo\" src=\"./icons/logo.png\" alt=\"Logo\" /></a>\n";
	s += "  <h1>SAGA-GIS Tool Library Documentation (v" + CSG_String(SAGA_VERSION) + ")</h1>\n";
	s += "  <nav>\n";
	s += "    <span class=\" a2z\"><a href=\"a2z.html\">Tools A-Z</a></span>\n";
	s += "    <span><a href=\"index.html\">Contents</a></span>\n";
	s += "  </nav>\n";
	s += "</header>\n";
	s += "<main>\n";
	s += "<h1>Library " + pLibrary->Get_Name() + "</h1>\n";
	s += "<p>" + pLibrary->Get_Description() + "</p>\n";
	s += "<ul>\n";
	s += "<li>Author: "  + pLibrary->Get_Author () + "</li>\n";
	s += "<li>Version: " + pLibrary->Get_Version() + "</li>\n";
	s += "<li>Menu: "    + pLibrary->Get_Menu   () + "</li>\n";
	s += "</ul>\n";
	s += "<h3>Tools</h3>\n";
	s += "<ul>\n";

	Tools.Set_Index(Tools.Get_Field("Tool"));

	for(sLong i=0; i<Tools.Get_Count(); i++)
	{
		s += CSG_String::Format("<li><a href=\"%s_%s.html\">%s</a></li>\n",
			pLibrary->Get_Library_Name().c_str(), Tools[i].asString("ID"), Tools[i].asString("Tool")
		);
	}

	s += "</ul>\n</main>\n";

	CSG_File Stream(SG_File_Make_Path(Folder, pLibrary->Get_Library_Name(), "html"), SG_FILE_W);

	return( Stream.is_Open() && Stream.Write(s) );
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
		"  <a href=\"https://saga-gis.org/\"><img class=\"logo\" src=\"./icons/logo.png\" alt=\"Logo\" /></a>\n"
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
		pLibrary->Get_Library_Name().c_str(), pLibrary->Get_Category().c_str(), pLibrary->Get_Name().c_str(),
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

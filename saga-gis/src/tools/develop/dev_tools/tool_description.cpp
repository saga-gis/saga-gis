
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
		Library.Set_Value("Library"    , Get_Library(pLibrary)       );
		Library.Set_Value("Name"       , pLibrary->Get_Name        ());
		Library.Set_Value("Description", pLibrary->Get_Description ());
		Library.Set_Value("Tools"      , pLibrary->Get_Count       ());

		//-------------------------------------------------
		CSG_Table Tools(&All_Tools);

		for(int iTool=0; iTool<pLibrary->Get_Count() && Set_Progress(nTotal, SG_Get_Tool_Library_Manager().Get_Tool_Count()); iTool++, nTotal++)
		{
			CSG_Tool *pTool = pLibrary->Get_Tool(iTool);

			CSG_Table_Record &Tool = *Tools.Add_Record();
			Tool.Set_Value("Library", Get_Library(pLibrary)       );
			Tool.Set_Value("ID"     , pTool   ->Get_ID          ());
			Tool.Set_Value("Tool"   , pTool   ->Get_Name        ());
			Tool.Set_Value("Menu"   , pTool   ->Get_MenuPath(true));
			All_Tools.Add_Record(&Tool);

			Write_Tool(Folder, pLibrary, pTool);
		}

		//-------------------------------------------------
		Write_Library(Folder, pLibrary, Tools);
	}

	//-----------------------------------------------------
	Write_Libraries(Folder, Libraries);
	Write_Tools    (Folder, All_Tools);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_String CTool_Description::Get_Library(CSG_Tool_Library *pLibrary)
{
	CSG_String s(pLibrary->Get_Library_Name());

	if( pLibrary->Get_Type() == TOOL_CHAINS )
	{
		s.Prepend("tc_");
	}

	return( s );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTool_Description::Write_Libraries(const CSG_String &Folder, CSG_Table &Libraries)
{
	CSG_String s("<!DOCTYPE html>\n<meta charset=\"UTF-8\">\n<meta name=\"viewport\" content=\"width=device-width\">\n");

	s += "<title>SAGA " + CSG_String(SAGA_VERSION) + " | Tool Library Documentation</title>\n";
	s += "<link rel=\"stylesheet\" href=\"../styles.css\">\n";
	s += "<header>\n";
	s += " <a href=\"https://saga-gis.org/\"><img class=\"logo\" src=\"../logo.png\" alt=\"Logo\" /></a>\n";
	s += "  <h1>SAGA " + CSG_String(SAGA_VERSION) + " | Tool Library Documentation</h1>\n";
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

	//-----------------------------------------------------
	CSG_File Stream(SG_File_Make_Path(Folder, "index", "html"), SG_FILE_W);

	return( Stream.is_Open() && Stream.Write(s) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTool_Description::Write_Tools(const CSG_String &Folder, CSG_Table &Tools)
{
	CSG_String s("<!DOCTYPE html>\n<meta charset=\"UTF-8\">\n<meta name=\"viewport\" content=\"width=device-width\">\n");

	s += "<title>SAGA " + CSG_String(SAGA_VERSION) + " | Tool Library Documentation | Full Tool Index</title>\n";
	s += "<script src=\"../tablesort.js\"></script>\n";
	s += "<link rel=\"stylesheet\" href=\"../styles.css\">\n";
	s += "<header>\n";
	s += " <a href=\"https://saga-gis.org/\"><img class=\"logo\" src=\"../logo.png\" alt=\"Logo\" /></a>\n";
	s += "  <h1>SAGA " + CSG_String(SAGA_VERSION) + " | Tool Library Documentation</h1>\n";
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

	//-----------------------------------------------------
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

	s += "<title>SAGA " + CSG_String(SAGA_VERSION) + " | Tool Library Documentation | Library</title>\n";
	s += "<link rel=\"stylesheet\" href=\"../styles.css\">\n";
	s += "<header>\n";
	s += " <a href=\"https://saga-gis.org/\"><img class=\"logo\" src=\"../logo.png\" alt=\"Logo\" /></a>\n";
	s += "  <h1>SAGA " + CSG_String(SAGA_VERSION) + " | Tool Library Documentation</h1>\n";
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
			Get_Library(pLibrary).c_str(), Tools[i].asString("ID"), Tools[i].asString("Tool")
		);
	}

	s += "</ul>\n</main>\n";

	//-----------------------------------------------------
	CSG_File Stream(SG_File_Make_Path(Folder, Get_Library(pLibrary), "html"), SG_FILE_W);

	return( Stream.is_Open() && Stream.Write(s) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTool_Description::Write_Tool(const CSG_String &Folder, CSG_Tool_Library *pLibrary, CSG_Tool *pTool)
{
	CSG_String s("<!DOCTYPE html>\n<meta charset=\"UTF-8\">\n<meta name=\"viewport\" content=\"width=device-width\">\n");

	s += "<title>SAGA " + CSG_String(SAGA_VERSION) + " | Tool Library Documentation | Tool " + pTool->Get_Name() + "</title>\n";
	s += "<link rel=\"stylesheet\" href=\"../styles.css\">\n";
	s += "<header>\n";
	s += " <a href=\"https://saga-gis.org/\"><img class=\"logo\" src=\"../logo.png\" alt=\"Logo\" /></a>\n";
	s += "  <h1>SAGA " + CSG_String(SAGA_VERSION) + " | Tool Library Documentation</h1>\n";
	s += "  <nav>\n";
	s += "    <span class=\" a2z\"><a href=\"a2z.html\">Tools A-Z</a></span>\n";
	s += "    <span><a href=\"index.html\">Contents</a></span>\n";
	s += "    <span><a href=\"./" + Get_Library(pLibrary) + ".html\">" + pLibrary->Get_Category() + " - " + pLibrary->Get_Name() + "</a></span>\n";
	s += "  </nav>\n";
	s += "</header>\n";
	s += "<main>\n";
	s += "<h1>" + pTool->Get_Name() + "</h1>\n";
	s += "<ul>\n";
	s += "<li><b>Author</b>: " + pTool->Get_Author      () + "</li>\n"; // Author
	CSG_String Menu(pTool->Get_MenuPath(true)); Menu.Replace("|", " | ");
	s += "<li><b>Menu:</b> " + Menu + "</li>\n"; // Menu
	s += "</ul>\n";
	s += "<hr><h3>Description</h3>\n<p>" + pTool->Get_Description() + "</p>\n";

	//-----------------------------------------------------
	if( pTool->Get_References().Get_Count() > 0 )
	{
		s += "<hr><h3>References</h3><ul>";

		for(int i=0; i<pTool->Get_References().Get_Count(); i++)
		{
			s += "<li>" + pTool->Get_References()[i] + "</li>";
		}

		s += "</ul>";
	}

	//-----------------------------------------------------
	s += "<hr><h3>Parameters</h3>\n";
	s += Get_Parameters(pTool);

	//-----------------------------------------------------
	s += "<hr><h3>Command Line</h3>\n";
	s += "<pre class=\"usage\">\n" + pTool->Get_Script(TOOL_SCRIPT_CMD_USAGE) + "</pre>\n";
	s += "</main>\n";

	//-----------------------------------------------------
	CSG_File Stream(SG_File_Make_Path(Folder, Get_Library(pLibrary) + "_" + pTool->Get_ID(), "html"), SG_FILE_W);

	return( Stream.is_Open() && Stream.Write(s) );
}

//---------------------------------------------------------
CSG_String CTool_Description::Get_Parameters(CSG_Tool *pTool)
{
	CSG_Table Table[3];
	Table[0].Add_Field("Name"       , SG_DATATYPE_String);
	Table[0].Add_Field("Type"       , SG_DATATYPE_String);
	Table[0].Add_Field("Identifier" , SG_DATATYPE_String);
	Table[0].Add_Field("Description", SG_DATATYPE_String);
	Table[0].Add_Field("Constraints", SG_DATATYPE_String);

	Table[1].Create(&Table[0]);
	Table[2].Create(&Table[0]);

	Table[0].Set_Name("Input"); Table[1].Set_Name("Output"); Table[2].Set_Name("Options");

	//-----------------------------------------------------
	Get_Parameters(pTool->Get_Parameters(), Table);

	for(int i=0; i<pTool->Get_Parameters_Count(); i++)
	{
		Get_Parameters(pTool->Get_Parameters(i), Table, pTool->Get_Parameters(i)->Get_Identifier() + ".");
	}

	//-----------------------------------------------------
	CSG_String s("<table>\n<tr><th>&nbsp;</th><th>Name</th><th>Type</th><th>Identifier</th><th>Description</th><th>Constraints</th></tr>\n");

	for(int j=0; j<3; j++)
	{
		for(int i=0; i<Table[j].Get_Count(); i++)
		{
			s += "<tr>";

			if( i == 0 )
			{
				s += CSG_String::Format("<td rowspan=\"%d\" class=\"labelSection\">%s</td>", Table[j].Get_Count(), Table[j].Get_Name());
			}

			s += CSG_String::Format("<td>%s</td><td>%s</td><td><code>%s</code></td><td>%s</td><td>%s</td></tr>",
				 Table[j][i].asString("Name"       ),
				 Table[j][i].asString("Type"       ),
				 Table[j][i].asString("Identifier" ),
				*Table[j][i].asString("Description") ? Table[j][i].asString("Description") : SG_T("-"),
				*Table[j][i].asString("Constraints") ? Table[j][i].asString("Constraints") : SG_T("-")
			);
		}
	}

	s += "</table>\n";

	return( s );
}

//---------------------------------------------------------
bool CTool_Description::Get_Parameters(CSG_Parameters *pParameters, CSG_Table Table[3], const CSG_String &IDprefix)
{
	for(int i=0; i<pParameters->Get_Count(); i++)
	{
		CSG_Parameter &P = *pParameters->Get_Parameter(i); CSG_Table_Record *pRecord = NULL;

		if( P.is_Information() || P.Get_Type() == PARAMETER_TYPE_Node )
		{
			continue;
		}

		if( P.is_Input() )
		{
			pRecord = Table[0].Add_Record();
		}
		else if( P.is_Output() )
		{
			pRecord = Table[1].Add_Record();
		}
		else if( P.is_Option() )
		{
			if( P.asParameters() )
			{
				Get_Parameters(P.asParameters(), Table, IDprefix + P.Get_Identifier() + ".");
			}
			else
			{
				pRecord = Table[2].Add_Record();
			}
		}

		if( pRecord )
		{
			pRecord->Set_Value("Name"       , P.Get_Name       ());
			pRecord->Set_Value("Type"       , P.Get_Description(PARAMETER_DESCRIPTION_TYPE));
			pRecord->Set_Value("Identifier" , P.Get_Identifier ());
			pRecord->Set_Value("Description", P.Get_Description());
			pRecord->Set_Value("Constraints", P.Get_Description(PARAMETER_DESCRIPTION_PROPERTIES));
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

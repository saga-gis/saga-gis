
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

#include <wx/cmdline.h>


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

	CSG_String Summary(pTool->Get_Summary()), Usage(Get_Usage(pTool));

	CSG_String Description; Description.Printf(HTML.c_str(), pTool->Get_Name().c_str(), SAGA_VERSION, SAGA_VERSION,
		pLibrary->Get_Library_Name(), pLibrary->Get_Category().c_str(), pLibrary->Get_Name().c_str(),
		Summary.c_str(), Usage.c_str()
	);

	return( Description );

	//CSG_String Parameters(Get_Parameters(pTool));

	//CSG_String Usage(Get_Usage(pTool));

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

//---------------------------------------------------------
CSG_String CTool_Description::Get_ID(CSG_Parameter *pParameter)
{
	CSG_String ID(pParameter->Get_Parameters()->Get_Identifier());

	if( ID.Length() > 0 )
	{
		ID += "_";
	}

	ID += pParameter->Get_Identifier();

	ID.Replace(".", "_");
	ID.Replace("|", "_");
	ID.Replace(" ", "_");

	return( ID );
}

//---------------------------------------------------------
CSG_String CTool_Description::Get_Usage(CSG_Tool *pTool)
{
	wxCmdLineParser Parser; Parser.SetSwitchChars("-");

	Get_Usage(pTool->Get_Parameters(), Parser);

	for(int i=0; i<pTool->Get_Parameters_Count(); i++)
	{
		Get_Usage(pTool->Get_Parameters(i), Parser);
	}

	wxString Usage = wxString::Format("\nUsage: saga_cmd %s %s %s",
		pTool->Get_Library().c_str(),
		pTool->Get_ID     ().c_str(),
		Parser.GetUsageString().AfterFirst(' ').AfterFirst(' ')
	);

	CSG_String s(&Usage);

	return( s );
}

//---------------------------------------------------------
bool CTool_Description::Get_Usage(CSG_Parameters *pParameters, wxCmdLineParser &Parser)
{
	if( !pParameters )
	{
		return( false );
	}

	//-----------------------------------------------------
	for(int i=0; i<pParameters->Get_Count(); i++)
	{
		CSG_Parameter *pParameter = pParameters->Get_Parameter(i);

		//-------------------------------------------------
		if( pParameter->is_DataObject() )	// reset data object parameters, avoids problems when tool is called more than once without un-/reloading
		{
			pParameter->Set_Value(DATAOBJECT_NOTSET);
		}
		else if( pParameter->is_DataObject_List() )
		{
			pParameter->asList()->Del_Items();
		}

		//-------------------------------------------------
		if( pParameter->do_UseInCMD() == false )
		{
			continue;
		}

		wxString Description = pParameter->Get_Description(
			PARAMETER_DESCRIPTION_NAME|PARAMETER_DESCRIPTION_TYPE|PARAMETER_DESCRIPTION_PROPERTIES, SG_T("\n\t")
		).c_str();

		Description.Replace("\xb", "");	// unicode problem: quick'n'dirty bug fix, to be replaced

		wxString ID(Get_ID(pParameter).c_str());

		if( pParameter->is_Input() || pParameter->is_Output() )
		{
			Parser.AddOption(ID, wxEmptyString, Description, wxCMD_LINE_VAL_STRING, wxCMD_LINE_NEEDS_SEPARATOR|wxCMD_LINE_PARAM_OPTIONAL);
		}

		//-------------------------------------------------
		else if( pParameter->is_Option() && !pParameter->is_Information() )
		{
			switch( pParameter->Get_Type() )
			{
			case PARAMETER_TYPE_Parameters  :
				Get_Usage(pParameter->asParameters(), Parser);
				break;

			case PARAMETER_TYPE_Bool        :
				Parser.AddOption(ID, wxEmptyString, Description, wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
				break;

			case PARAMETER_TYPE_Int         :
				Parser.AddOption(ID, wxEmptyString, Description, wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL);
				break;

			case PARAMETER_TYPE_Data_Type   :
			case PARAMETER_TYPE_Choice      :
			case PARAMETER_TYPE_Choices     :
			case PARAMETER_TYPE_Table_Field :
			case PARAMETER_TYPE_Table_Fields:
				Parser.AddOption(ID, wxEmptyString, Description, wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
				break;

			case PARAMETER_TYPE_Double      :
			case PARAMETER_TYPE_Degree      :
				Parser.AddOption(ID, wxEmptyString, Description, wxCMD_LINE_VAL_DOUBLE, wxCMD_LINE_PARAM_OPTIONAL);
				break;

			case PARAMETER_TYPE_Date        :
				Parser.AddOption(ID, wxEmptyString, Description, wxCMD_LINE_VAL_DATE  , wxCMD_LINE_PARAM_OPTIONAL);
				break;

			case PARAMETER_TYPE_Range       :
				Parser.AddOption(ID + "_MIN", wxEmptyString, Description, wxCMD_LINE_VAL_DOUBLE, wxCMD_LINE_PARAM_OPTIONAL);
				Parser.AddOption(ID + "_MAX", wxEmptyString, Description, wxCMD_LINE_VAL_DOUBLE, wxCMD_LINE_PARAM_OPTIONAL);
				break;

			case PARAMETER_TYPE_Color       :
				Parser.AddOption(ID, wxEmptyString, Description, wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
				break;

			case PARAMETER_TYPE_Colors      :
				Parser.AddOption(ID, wxEmptyString, Description, wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
				break;

			case PARAMETER_TYPE_String      :
			case PARAMETER_TYPE_Text        :
			case PARAMETER_TYPE_FilePath    :
				Parser.AddOption(ID, wxEmptyString, Description, wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
				break;

			case PARAMETER_TYPE_FixedTable  :
				Parser.AddOption(ID, wxEmptyString, Description, wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
				break;

			case PARAMETER_TYPE_Grid_System :
				if( pParameter->Get_Children_Count() == 0 )
				{
					Parser.AddOption(ID + "_D"   , wxEmptyString, _TL("Cell Size"                          ), wxCMD_LINE_VAL_DOUBLE, wxCMD_LINE_PARAM_OPTIONAL);
					Parser.AddOption(ID + "_X"   , wxEmptyString, _TL("Lower Left Center Cell X-Coordinate"), wxCMD_LINE_VAL_DOUBLE, wxCMD_LINE_PARAM_OPTIONAL);
					Parser.AddOption(ID + "_Y"   , wxEmptyString, _TL("Lower Left Center Cell Y-Coordinate"), wxCMD_LINE_VAL_DOUBLE, wxCMD_LINE_PARAM_OPTIONAL);
					Parser.AddOption(ID + "_NX"  , wxEmptyString, _TL("Number of Columns"                  ), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL);
					Parser.AddOption(ID + "_NY"  , wxEmptyString, _TL("Number of Rows"                     ), wxCMD_LINE_VAL_NUMBER, wxCMD_LINE_PARAM_OPTIONAL);
					Parser.AddOption(ID + "_FILE", wxEmptyString, _TL("Grid File"                          ), wxCMD_LINE_VAL_STRING, wxCMD_LINE_PARAM_OPTIONAL);
				}
				break;

			default:
				break;
			}
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

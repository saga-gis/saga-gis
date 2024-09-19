
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
//                    qgistoolbox.cpp                    //
//                                                       //
//                 Copyright (C) 2020 by                 //
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
#include "qgistoolbox.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CQGIS_ToolBox::CQGIS_ToolBox(void)
{
	Set_Name	("QGIS Tool Interface Generator");

	Set_Author	("O.Conrad (c) 2020");

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

	Parameters.Add_Choice(
		"", "GROUP"    , "Group by...",
		"",
		"Category|Library"
	);

	Parameters.Add_Bool(
		"", "LOAD_ALL" , "Load All Standard Tool Libraries",
		"",
		true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CQGIS_ToolBox::On_Execute(void)
{
	if( Parameters("LOAD_ALL")->asBool() )
	{
		SG_Get_Tool_Library_Manager().Add_Default_Libraries();
	}

	//-----------------------------------------------------
	CSG_String Directory(Parameters("DIRECTORY")->asString());

	if( Parameters("CLEAR")->asBool() )
	{
		CSG_Strings Files;

		SG_Dir_List_Files(Files, Directory);

		for(int i=0; i<Files.Get_Count(); i++)
		{
			SG_File_Delete(Files[i]);
		}

		SG_Dir_Delete(Directory + "/description", true);
	}

	if( !SG_Dir_Exists(Directory + "/description") )
	{
		SG_Dir_Create(Directory + "/description");
	}

	//-----------------------------------------------------
	int nTools = 0; CSG_String Groups, Algorithms; bool bGroupByCategory = Parameters("GROUP")->asInt() == 0;

	for(int iLibrary=0; iLibrary<SG_Get_Tool_Library_Manager().Get_Count() && Set_Progress(iLibrary, SG_Get_Tool_Library_Manager().Get_Count()); iLibrary++)
	{
		CSG_Tool_Library *pLibrary = SG_Get_Tool_Library_Manager().Get_Library(iLibrary);

		if( !pLibrary->Get_Category    ().Cmp("SAGA Development" )	// generally exclude certain categories/libraries
		||  !pLibrary->Get_Category    ().Cmp("Garden"           )
		||  !pLibrary->Get_Category    ().Cmp("Grid Collection"  )
		||  !pLibrary->Get_Category    ().Cmp("Reports"          )
		||  !pLibrary->Get_Category    ().Cmp("Simulation"       )
		||  !pLibrary->Get_Category    ().Cmp("Table"            )
		||  !pLibrary->Get_Category    ().Cmp("TIN"              )
		||  !pLibrary->Get_Category    ().Cmp("Visualization"    )
		||  !pLibrary->Get_Library_Name().Cmp("grid_calculus_bsl")
		||  !pLibrary->Get_Library_Name().Cmp("db_odbc"          )
		||  !pLibrary->Get_Library_Name().Cmp("db_pgsql"         ) )
		{
			continue;
		}

		Process_Set_Text(CSG_String::Format("%s: %s", SG_T("Library"), pLibrary->Get_Library_Name().c_str()));

		CSG_String Library(pLibrary->Get_Library_Name());

		Library.Make_Lower();
		Library.Replace(" ", "_");

		for(int iTool=0, nAdded=0; iTool<pLibrary->Get_Count(); iTool++)
		{
			CSG_Tool *pTool = pLibrary->Get_Tool(iTool);
			CSG_String Code = pTool->Get_Name() + "|" + pTool->Get_ID() + "\n" + Library + "\n";

			if( Get_Tool(pTool, Code) )
			{
				CSG_String	Name(pLibrary->Get_Library_Name() + "_" + pTool->Get_Name());

				Name.Make_Lower();
				Name.Replace(" ", "_");
				Name.Replace("/", "-");
				Name.Replace(":", "_");

				CSG_File Stream;

				if( Stream.Open(SG_File_Make_Path(Directory + "/description", Name, "txt"), SG_FILE_W, false) )
				{
					Stream.Write(Code);

					nTools++;

					//-------------------------------------------------
					CSG_String s[2];
					
					s[0] = pTool->Get_Name();
					s[0].Replace("'", "''");

					s[1] = pTool->Get_Name();
					s[1].Replace("'", "''");
					s[1].Replace("Grid"  , "Raster"  );
					s[1].Replace("Shapes", "Features");

					if( !Algorithms.is_Empty() )
					{
						Algorithms += ",\n";
					}

					Algorithms += "'" + s[0] + "': '" + s[1] + "'";

					//-------------------------------------------------
					if( ++nAdded == 1 )
					{
						s[0] = Library;

						s[1] = bGroupByCategory
							? pLibrary->Get_Category()
							: pLibrary->Get_Category() + " - " + pLibrary->Get_Name();

						s[1].Replace("'", "''");
						s[1].Replace("Gridding", "Rasterizing");
						s[1].Replace("Grid"    , "Raster"     );
						s[1].Replace("Shapes"  , "Features"   );

						if( !Groups.is_Empty() )
						{
							Groups += ",\n";
						}

						Groups += "'" + s[0] + "': '" + s[1] + "'";
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	if( nTools > 0 )
	{
		CSG_File Stream;

		if( Stream.Open(SG_File_Make_Path(Directory, "SagaNameDecorator", "py"), SG_FILE_W, false) )
		{
			Stream.Write("# -*- coding: utf-8 -*-\n\n");

			Stream.Write("groups = {\n"     + Groups     + "}\n\n");
			Stream.Write("def decoratedGroupName(name):\n\treturn groups.get(name, name)\n\n");

			Stream.Write("algorithms = {\n" + Algorithms + "}\n\n");
			Stream.Write("def decoratedAlgorithmName(name):\n\treturn algorithms.get(name, name)\n\n");
		}

		//-------------------------------------------------
		if( Stream.Open(SG_File_Make_Path(Directory, "readme", "txt"), SG_FILE_W, false) )
		{
			Stream.Write(
				"The generated tool interface makes it possible to use this SAGA version in QGIS.\n\n"
				"SUMMARY:\n"
				"- install the SAGA binaries to a folder of your choice\n"
				"- install and configure the \"SAGA Next Gen\" QGIS provider\n"
				"- update the tool interface of the provider with the generated one\n\n"
				"STEP BY STEP:\n"
				"- install or unzip the SAGA binaries to a folder of your choice\n"
				"- start QGIS\n"
				"- open the plugins dialog: menu \"Plugins > Manage and Install Plugins...\"\n"
				"- enable \"Show also Experimental Plugins\" in the settings\n"
				"- in \"All\", search for \"saga\" and enable the \"Processing SAGA NextGen Provider\" (this installs the provider)\n"
				"- close the plugins dialog\n"
				"- open the toolbox dialog: menu \"Processing > Toolbox\"\n"
				"- open the toolbox's \"Options\" dialog and open \"Providers > SAGANG\"\n"
				"- set the \"SAGA folder\" to the installation folder of the SAGA binaries\n"
				"- check the \"Enable SAGA Import/Export optimizations\" option\n"
				"- close the options dialog, now the \"SAGA Next Gen\" provider shows up in the toolbox dialog\n"
				"- close QGIS\n"
				"- update the SAGA interface description of the provider:\n"
				"- the plugin installation folder is\n"
				"  for MS Windows\n"
				"    C:\\Users\\my_username\\AppData\\Roaming\\QGIS\\QGIS3\\profiles\\default\\python\\plugins\\processing_saga_nextgen\n"
				"  for Unix-likes\n"
				"    ~/.local/share/QGIS/QGIS3/profiles/default/python/plugins/processing_saga_nextgen\n"
				"- replace the \"description\" folder with the generated one\n"
				"- replace the \"SagaNameDecorator.py\" file in the \"processing\" subfolder\n"
				"- edit the file \"processing/provider.py\" and set the\n"
				"  REQUIRED_VERSION = 'x.x.' to the version of your SAGA binaries, e.g.\n"
				"  REQUIRED_VERSION = '9.2.'\n"
				"- start QGIS and enjoy SAGA\n"
			);
		}
	}

	//-----------------------------------------------------
	Message_Fmt("Number of exported tool interfaces: %d", nTools);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CQGIS_ToolBox::Get_Tool(CSG_Tool *pTool, CSG_String &Code)
{
	if( pTool == NULL || pTool == TLB_INTERFACE_SKIP_TOOL || pTool->needs_GUI() || pTool->is_Interactive() || pTool->Get_Parameters_Count() > 0 )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Parameters	*pParameters	= pTool->Get_Parameters();

	for(int i=0; i<pParameters->Get_Count(); i++)
	{
		CSG_String	Parameter;

		if( !Get_Parameter(pParameters->Get_Parameter(i), Parameter) )
		{
			return( false );
		}

		if( !Parameter.is_Empty() )
		{
			Code	+= Parameter + "\n";
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CQGIS_ToolBox::Get_Shape_Type(TSG_Shape_Type Type)
{
	switch( Type )
	{
	case SHAPE_TYPE_Point  : return(  0 );
	case SHAPE_TYPE_Points : return(  0 );
	case SHAPE_TYPE_Line   : return(  1 );
	case SHAPE_TYPE_Polygon: return(  2 );
	default                : return( -1 );
	}
}

//---------------------------------------------------------
CSG_String CQGIS_ToolBox::Get_Choices(CSG_Parameter_Choice *pChoice)
{
	CSG_String	Choices;

	for(int i=0; i<pChoice->Get_Count(); i++)
	{
		if( i > 0 )
		{
			Choices	+= ";";
		}

		Choices	+= CSG_String::Format("[%d] %s", i, pChoice->Get_Item(i));
	}

	return( Choices );
}

//---------------------------------------------------------
bool CQGIS_ToolBox::Get_Parameter(CSG_Parameter *pParameter, CSG_String &Parameter)
{
	if( !pParameter->do_UseInCMD() )
	{
		return( true );
	}

	if( pParameter->Cmp_Identifier("TARGET_DEFINITION") )
	{
		Parameter	= "Hardcoded|-TARGET_DEFINITION 0";

		Parameter	+= "\nQgsProcessingParameterExtent"
			"|TARGET_USER_XMIN"
			" TARGET_USER_XMAX"
			" TARGET_USER_YMIN"
			" TARGET_USER_YMAX"
			"|Output extent|None|True";

		Parameter	+= "\nQgsProcessingParameterNumber"
			"|TARGET_USER_SIZE"
			"|Cellsize|QgsProcessingParameterNumber.Double|100.0|False|None|None";

		Parameter	+= "\nQgsProcessingParameterEnum"
			"|TARGET_USER_FITS"
			"|Fit|[0] nodes;[1] cells|False|0";

		return( true );
	}

	if( pParameter->Cmp_Identifier("TARGET_USER_XMIN")
	||  pParameter->Cmp_Identifier("TARGET_USER_XMAX")
	||  pParameter->Cmp_Identifier("TARGET_USER_YMIN")
	||  pParameter->Cmp_Identifier("TARGET_USER_YMAX")
	||  pParameter->Cmp_Identifier("TARGET_USER_COLS")
	||  pParameter->Cmp_Identifier("TARGET_USER_ROWS")
	||  pParameter->Cmp_Identifier("TARGET_USER_SIZE")
	||  pParameter->Cmp_Identifier("TARGET_USER_FITS")
	||  pParameter->Cmp_Identifier("TARGET_TEMPLATE" ) )
	{
		return( true );
	}

	//-----------------------------------------------------
	#define PARAMETER_SET(type)	{ Parameter.Printf("QgsProcessingParameter%s|%s|%s", SG_T(type), pParameter->Get_Identifier(), pParameter->Get_Name()); }
	#define PARAMETER_STR(val)	{ Parameter += "|"; Parameter += val; }
	#define PARAMETER_BOL(val)	{ Parameter += (val) ? "|True" : "|False"; }
	#define PARAMETER_INT(val)	{ Parameter += CSG_String::Format("|%d", (int)val); }
	#define PARAMETER_FLT(val)	{ Parameter += CSG_String::Format("|%f", val); }

	//-----------------------------------------------------
	switch( pParameter->Get_Type() )
	{
	case PARAMETER_TYPE_Grid           :
		if( pParameter->is_Input() )
		{
			PARAMETER_SET("RasterLayer");
			PARAMETER_STR("None");
			PARAMETER_BOL(pParameter->is_Optional());
		}
		else
		{
			PARAMETER_SET("RasterDestination");
			PARAMETER_STR("None");
			PARAMETER_BOL(pParameter->is_Optional());
		}
		break;

	case PARAMETER_TYPE_Grid_List      :
		if( pParameter->is_Input() )
		{
			PARAMETER_SET("MultipleLayers");
			PARAMETER_INT(3);
			PARAMETER_STR("None");
			PARAMETER_BOL(pParameter->is_Optional());
		}
		else
		{
			PARAMETER_SET("RasterDestination");
			PARAMETER_STR("None");
			PARAMETER_BOL(pParameter->is_Optional());
		}
		break;

	case PARAMETER_TYPE_Grids          :
	case PARAMETER_TYPE_Grids_List     :
		return( false );

	case PARAMETER_TYPE_Table          :
		if( pParameter->is_Input() )
		{
			PARAMETER_SET("FeatureSource");
			PARAMETER_INT(5);
			PARAMETER_STR("None");
			PARAMETER_BOL(pParameter->is_Optional());
		}
		else
		{
			PARAMETER_SET("VectorDestination");
			PARAMETER_INT(5);
			PARAMETER_STR("None");
			PARAMETER_BOL(pParameter->is_Optional());
		}
		break;

	case PARAMETER_TYPE_Table_List     :
		if( pParameter->is_Input() )
		{
			PARAMETER_SET("MultipleLayers");
			PARAMETER_INT(5);
			PARAMETER_STR("None");
			PARAMETER_BOL(pParameter->is_Optional());
		}
		else
		{
			PARAMETER_SET("VectorDestination");
			PARAMETER_INT(Get_Shape_Type(((CSG_Parameter_Shapes *)pParameter)->Get_Shape_Type()));
			PARAMETER_STR("None");
			PARAMETER_BOL(pParameter->is_Optional());
		}
		break;

	case PARAMETER_TYPE_Shapes         :
		if( pParameter->is_Input() )
		{
			PARAMETER_SET("FeatureSource");
			PARAMETER_INT(Get_Shape_Type(((CSG_Parameter_Shapes *)pParameter)->Get_Shape_Type()));
			PARAMETER_STR("None");
			PARAMETER_BOL(pParameter->is_Optional());
		}
		else
		{
			PARAMETER_SET("VectorDestination");
			PARAMETER_INT(Get_Shape_Type(((CSG_Parameter_Shapes *)pParameter)->Get_Shape_Type()));
			PARAMETER_STR("None");
			PARAMETER_BOL(pParameter->is_Optional());
		}
		break;

	case PARAMETER_TYPE_Shapes_List    :
		if( pParameter->is_Input() )
		{
			PARAMETER_SET("MultipleLayers");
			PARAMETER_INT(Get_Shape_Type(((CSG_Parameter_Shapes_List *)pParameter)->Get_Shape_Type()));
			PARAMETER_STR("None");
			PARAMETER_BOL(pParameter->is_Optional());
		}
		else
		{
			PARAMETER_SET("VectorDestination");
			PARAMETER_INT(Get_Shape_Type(((CSG_Parameter_Shapes *)pParameter)->Get_Shape_Type()));
			PARAMETER_STR("None");
			PARAMETER_BOL(pParameter->is_Optional());
		}
		break;

	case PARAMETER_TYPE_PointCloud     :
		if( pParameter->is_Input() )
		{
			PARAMETER_SET("PointCloudLayer");
			PARAMETER_STR("None");
			PARAMETER_BOL(pParameter->is_Optional());
		}
		else
		{
			PARAMETER_SET("PointCloudDestination");
			PARAMETER_STR("None");
			PARAMETER_BOL(pParameter->is_Optional());
		}
		break;

	case PARAMETER_TYPE_PointCloud_List:
		if( pParameter->is_Input() )
		{
			PARAMETER_SET("MultipleLayers");
			PARAMETER_INT(8);
			PARAMETER_STR("None");
			PARAMETER_BOL(pParameter->is_Optional());
		}
		else
		{
			PARAMETER_SET("PointCloudDestination");
			PARAMETER_STR("None");
			PARAMETER_BOL(pParameter->is_Optional());
		}
		break;

	case PARAMETER_TYPE_TIN            :
	case PARAMETER_TYPE_TIN_List       :
		return( false );

	case PARAMETER_TYPE_DataObject_Output:
		return( false );

	//-----------------------------------------------------
	case PARAMETER_TYPE_Bool           :
		PARAMETER_SET("Boolean");
		PARAMETER_BOL(pParameter->asBool());
		break;

	case PARAMETER_TYPE_Int            :
		PARAMETER_SET("Number");
		PARAMETER_STR("QgsProcessingParameterNumber.Integer");
		PARAMETER_STR(pParameter->asString());
		PARAMETER_BOL(false);
		if( pParameter->asValue()->has_Minimum() ) { PARAMETER_INT(pParameter->asValue()->Get_Minimum()); } else { PARAMETER_STR("None"); }
		if( pParameter->asValue()->has_Maximum() ) { PARAMETER_INT(pParameter->asValue()->Get_Maximum()); } else { PARAMETER_STR("None"); }
		break;

	case PARAMETER_TYPE_Degree         :
	case PARAMETER_TYPE_Double         :
		PARAMETER_SET("Number");
		PARAMETER_STR("QgsProcessingParameterNumber.Double");
		PARAMETER_FLT(pParameter->asDouble());
		PARAMETER_BOL(false);
		if( pParameter->asValue()->has_Minimum() ) { PARAMETER_FLT(pParameter->asValue()->Get_Minimum()); } else { PARAMETER_STR("None"); }
		if( pParameter->asValue()->has_Maximum() ) { PARAMETER_FLT(pParameter->asValue()->Get_Maximum()); } else { PARAMETER_STR("None"); }
		break;

	case PARAMETER_TYPE_Data_Type      :
	case PARAMETER_TYPE_Choice         :
		PARAMETER_SET("Enum");
		PARAMETER_STR(Get_Choices(pParameter->asChoice()));
		PARAMETER_BOL(false);
		PARAMETER_INT(pParameter->asInt());
		break;

	case PARAMETER_TYPE_Date           :
	case PARAMETER_TYPE_String         :
	case PARAMETER_TYPE_Text           :
		PARAMETER_SET("String");
		PARAMETER_STR(pParameter->asString());
		break;

	case PARAMETER_TYPE_FilePath       :
		PARAMETER_SET("File");
		PARAMETER_STR("QgsProcessingParameterFile.File");
		PARAMETER_STR("None");
		PARAMETER_BOL(false);
	//	pParameter->asFilePath()->is_Directory();
	//	pParameter->asFilePath()->is_Multiple ();
		break;

	case PARAMETER_TYPE_Table_Field    :
		PARAMETER_SET("Field");
		PARAMETER_STR("None");
		PARAMETER_STR(pParameter->Get_Parent()->Get_Identifier());
		PARAMETER_INT(-1);
		PARAMETER_BOL(false);
		PARAMETER_BOL(false);
		break;

	//-----------------------------------------------------
	case PARAMETER_TYPE_Table_Fields   :
		PARAMETER_SET("String");
		PARAMETER_STR(pParameter->asString());
		break;

	case PARAMETER_TYPE_FixedTable     :
		return( false );

	case PARAMETER_TYPE_Grid_System    :
		break;
		return( pParameter->Get_Children_Count() > 0 );

	//-----------------------------------------------------
	default:
		break;
	}

	//QgsProcessingParameterMultipleLayers,
    //QgsProcessingParameterMatrix,

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

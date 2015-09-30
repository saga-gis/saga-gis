/**********************************************************
 * Version $Id: arctoolbox.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      dev_tools                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    arctoolbox.cpp                     //
//                                                       //
//                 Copyright (C) 2015 by                 //
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
//                Institute of Geography                 //
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
#include "arctoolbox.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CArcToolBox::CArcToolBox(void)
{
	//-----------------------------------------------------
	Set_Name	(_TL("ArcSAGA Toolboxes"));

	Set_Author	("O.Conrad (c) 2015");

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_FilePath(
		NULL	, "DIRECTORY"	, _TL("Output Directory"),
		_TL(""),
		NULL, NULL, true, true
	);

	Parameters.Add_Choice(
		NULL	, "BOX_NAMING"	, _TL("Toolbox Naming"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("library file name"),
			_TL("category and library name")
		), 0
	);

	Parameters.Add_Choice(
		NULL	, "ARC_VERSION"	, _TL("ArcGIS Version"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			SG_T("10.1"),
			SG_T("10.2, 10.3")
		), 1
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CArcToolBox::On_Execute(void)
{
	int		nTools	= 0;

	m_ArcVersion	= Parameters("ARC_VERSION")->asInt();

	//-----------------------------------------------------
	for(int iLibrary=0; iLibrary<SG_Get_Module_Library_Manager().Get_Count() && Set_Progress(iLibrary, SG_Get_Module_Library_Manager().Get_Count()); iLibrary++)
	{
		CSG_Module_Library	*pLibrary	= SG_Get_Module_Library_Manager().Get_Library(iLibrary);

		if( !pLibrary->Get_Category().Cmp(_TL("Garden"    ))
		||  !pLibrary->Get_Category().Cmp(_TL("Reports"   ))
		||  !pLibrary->Get_Category().Cmp(_TL("Simulation"))
		||  !pLibrary->Get_Category().Cmp(_TL("Table"     ))
		||  !pLibrary->Get_Category().Cmp(_TL("TIN"       )) )
		{
			continue;
		}

		Process_Set_Text(CSG_String::Format("%s: %s", _TL("Library"), pLibrary->Get_Library_Name().c_str()));

		CSG_Strings	Names, Codes, Descs;

		for(int iTool=0; iTool<pLibrary->Get_Count(); iTool++)
		{
			CSG_String		Code;
			CSG_MetaData	Desc;

			if( Get_Tool(pLibrary, iTool, Code, Desc) )
			{
				Names	+= CSG_String::Format("tool_%d", iTool);
				Codes	+= Code;
				Descs	+= Desc.asText(1);	// '1' => as xml code
				nTools	++;
			}
		}

		Save(pLibrary, Parameters("DIRECTORY")->asString(), Names, Codes, Descs);
	}

	//-----------------------------------------------------
	Message_Add(CSG_String::Format("%s: %d", _TL("Number of added tools"), nTools));

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	FORMAT_ASCII,
	FORMAT_PLAIN,
	FORMAT_HTML,
	FORMAT_HTML_TXT,
	FORMAT_FILE
};

//---------------------------------------------------------
CSG_String CArcToolBox::Get_Formatted(CSG_String String, int Type)
{
	if( Type == FORMAT_FILE )
	{
		String.Replace("/", ".");

		return( String );
	}

	if( Type == FORMAT_ASCII )
	{
		String.Replace("ä", "ae");
		String.Replace("ö", "oe");
		String.Replace("ü", "ue");
		String.Replace("Ä", "Ae");
		String.Replace("Ö", "Oe");
		String.Replace("Ü", "Ue");
		String.Replace("ß", "sz");
	}

	String.Replace("\xb", "");
	String.Replace("\xB", "");

	String.Replace("²", "2");
	String.Replace("³", "3");

	String.Replace("\r",  "");
	String.Replace("\t", " ");

	String.Replace("\"", "\\\"");

	if( Type == FORMAT_HTML || Type == FORMAT_HTML_TXT )
	{
		if( Type == FORMAT_HTML )
		{
			String	= "<p>" + String + "</p>";
			String.Replace("\n", "</p><p>");
		}

		if( Type == FORMAT_HTML_TXT )
		{
			String	= "&lt;p&gt;" + String + "&lt;/p&gt;";
			String.Replace("\n", "&lt;/p&gt;&lt;p&gt;");
		}
	}
	else
	{
		String.Replace("\n", "\\n");
	}

	return( String );
}

//---------------------------------------------------------
CSG_String CArcToolBox::Get_Description(CSG_Module *pTool, int Type)
{
	CSG_String	s(pTool->Get_Description());

	s	+= "<p><hr>";
	s	+= "<span STYLE=\"font-style:italic;font-size:9pt;font-weight:bold\">";
	s	+= "SAGA - System for Automated Geoscientific Analyses<br></span>";
	s	+= "<span STYLE=\"font-style:italic;font-size:8pt\">";
	s	+= "www.saga-gis.org<br></span>";
	s	+= "<span STYLE=\"font-style:italic;font-size:6pt\">_____<br>Reference:<br>";
	s	+= "Conrad, O., Bechtel, B., Bock, M., Dietrich, H., Fischer, E., Gerlitz, L., Wehberg, J., Wichmann, V., and Böhner, J. (2015): ";
	s	+= "System for Automated Geoscientific Analyses (SAGA) v. 2.1.4, Geosci. Model Dev., 8, 1991-2007, doi:10.5194/gmd-8-1991-2015.";
	s	+= "</span></p>";

	return( Get_Formatted(s, Type) );
}

//---------------------------------------------------------
CSG_String CArcToolBox::Get_ID(CSG_Parameter *pParameter, const CSG_String &Modifier)
{
	CSG_String	ID(pParameter->Get_Owner()->Get_Identifier());

	if( ID.Length() > 0 )
	{
		ID	+= SG_T("_");
	}

	ID	+= pParameter->Get_Identifier();

	if( Modifier.Length() > 0 )
	{
		ID	+= SG_T("_") + Modifier;
	}

	return( ID );
}

//---------------------------------------------------------
CSG_Parameter * CArcToolBox::Get_GridTarget(CSG_Parameters *pParameters)
{
	CSG_Parameter	*pParameter	= pParameters->Get_Parameter("TARGET_DEFINITION");

	if( pParameter && pParameter->Get_Type() == PARAMETER_TYPE_Choice )
	{
		return( pParameter );
	}


	return( NULL );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	ARC_Raster	= 0,
	ARC_Feature,
	ARC_TableView,
	ARC_Boolean,
	ARC_Long,
	ARC_Double,
	ARC_String,
	ARC_Folder,
	ARC_File,
	ARC_Field,
	ARC_Table,
	ARC_nTypes
};

//---------------------------------------------------------
const CSG_String	ArcDataTypes[ARC_nTypes][2]	=
{
	{	"Raster Layer" , "GPRasterLayer"   },
	{	"Feature Layer", "GPFeatureLayer"  },
	{	"Table View"   , "GPTableView"     },
	{	"Boolean"      , "GPBoolean"       },
	{	"Long"         , "GPLong"          },
	{	"Double"       , "GPDouble"        },
	{	"String"       , "GPString"        },
	{	"Folder"       , "DEFolder"        },
	{	"File"         , "DEFile"          },
	{	"Field"        , "Field"           },
	{	"Table"        , "DETable"         }
};

//---------------------------------------------------------
#define	ArcDataType(type, multi)	", datatype=\"" + ArcDataTypes[type][m_ArcVersion] + (multi ? "\", multiValue=True)\n" : "\")\n")

//---------------------------------------------------------
bool CArcToolBox::Get_Parameter(CSG_Parameter *pParameter, CSG_Strings &Infos, CSG_Strings &Init, CSG_MetaData &Descs, const CSG_String &Name, const CSG_String &Identifier)
{
	int		i;

	//-----------------------------------------------------
	CSG_String	Info("\t\tparam = arcpy.Parameter(");

	Info	+=     "displayName=\"" + Name       + "\"";
	Info	+=          ", name=\"" + Identifier + "\"";
	Info	+=     ", direction=\"" + CSG_String(pParameter->is_Output() ? "Output" : "Input") + "\"";
	Info	+= ", parameterType=\"" + CSG_String(pParameter->is_Option() || pParameter->is_Optional() ? "Optional" : "Required") + "\"";

	switch( pParameter->Get_Type() )
	{
	//-----------------------------------------------------
	case PARAMETER_TYPE_Grid           :	Info	+= ArcDataType(ARC_Raster   , false);	break;
	case PARAMETER_TYPE_Grid_List      :	Info	+= ArcDataType(ARC_Raster   ,  true);	break;

	case PARAMETER_TYPE_Table          :	Info	+= ArcDataType(ARC_TableView, false);	break;
	case PARAMETER_TYPE_Table_List     :	Info	+= ArcDataType(ARC_TableView,  true);	break;

	case PARAMETER_TYPE_Shapes         :
		Info	+= ArcDataType(ARC_Feature  , false);
		switch( pParameter->is_Input() ? ((CSG_Parameter_Shapes *)pParameter->Get_Data())->Get_Shape_Type() : SHAPE_TYPE_Undefined )
		{
		case SHAPE_TYPE_Point  :	Info	+= "\t\tparam.filter.list = [\"Point\"]\n"     ;	break;
		case SHAPE_TYPE_Points :	Info	+= "\t\tparam.filter.list = [\"Multipoint\"]\n";	break;
		case SHAPE_TYPE_Line   :	Info	+= "\t\tparam.filter.list = [\"Polyline\"]\n"  ;	break;
		case SHAPE_TYPE_Polygon:	Info	+= "\t\tparam.filter.list = [\"Polygon\"]\n"   ;	break;
		}
		break;

	case PARAMETER_TYPE_Shapes_List    :
		Info	+= ArcDataType(ARC_Feature  ,  true);
		switch( pParameter->is_Input() ? ((CSG_Parameter_Shapes *)pParameter->Get_Data())->Get_Shape_Type() : SHAPE_TYPE_Undefined )
		{
		case SHAPE_TYPE_Point  :	Info	+= "\t\tparam.filter.list = [\"Point\"]\n"     ;	break;
		case SHAPE_TYPE_Points :	Info	+= "\t\tparam.filter.list = [\"Multipoint\"]\n";	break;
		case SHAPE_TYPE_Line   :	Info	+= "\t\tparam.filter.list = [\"Polyline\"]\n"  ;	break;
		case SHAPE_TYPE_Polygon:	Info	+= "\t\tparam.filter.list = [\"Polygon\"]\n"   ;	break;
		}
		break;

	case PARAMETER_TYPE_PointCloud     :
	case PARAMETER_TYPE_PointCloud_List:	return( false );

	case PARAMETER_TYPE_TIN            :
	case PARAMETER_TYPE_TIN_List       :	return( false );

	case PARAMETER_TYPE_DataObject_Output:
		return( false );

	//-----------------------------------------------------
	case PARAMETER_TYPE_Bool:
		Info	+= ArcDataType(ARC_Boolean  , false);
		Info	+= CSG_String::Format("\t\tparam.value = %s\n", pParameter->asBool() ? SG_T("True") : SG_T("False"));
		break;

	case PARAMETER_TYPE_Int:
		Info	+= ArcDataType(ARC_Long     , false);
		Info	+= CSG_String::Format("\t\tparam.value = %d\n", pParameter->asInt());
		break;

	case PARAMETER_TYPE_Degree:
	case PARAMETER_TYPE_Double:
		Info	+= ArcDataType(ARC_Double   , false);
		Info	+= CSG_String::Format("\t\tparam.value = %f\n", pParameter->asDouble());
		break;

	case PARAMETER_TYPE_Choice:
		Info	+= ArcDataType(ARC_String   , false);
		Info	+= "\t\tparam.filter.list = [\"";
		for(i=0; i<pParameter->asChoice()->Get_Count(); i++)
		{
			Info	+= pParameter->asChoice()->Get_Item(i) + CSG_String(i < pParameter->asChoice()->Get_Count() - 1 ? "\", \"" : "\"]\n");
		}
		Info	+= CSG_String::Format("\t\tparam.value = \"%s\"\n", pParameter->asString());
		break;

	case PARAMETER_TYPE_String:
	case PARAMETER_TYPE_Text:
		Info	+= ArcDataType(ARC_String   , false);
		Info	+= CSG_String::Format("\t\tparam.value = \"%s\"\n", Get_Formatted(pParameter->asString(), FORMAT_PLAIN).c_str());
		break;

	case PARAMETER_TYPE_FilePath:
		if( pParameter->asFilePath()->is_Directory() )
		{
			Info	+= ArcDataType(ARC_Folder, false);
		}
		else
		{
			Info	+= ArcDataType(ARC_File, pParameter->asFilePath()->is_Multiple());
		}
		break;

	case PARAMETER_TYPE_FixedTable:
		Info	+= ArcDataType(ARC_Table    , false);
		break;

	case PARAMETER_TYPE_Table_Field:
		Info	+= ArcDataType(ARC_Field    , false);
		Info	+= CSG_String::Format("\t\tparam.parameterDependencies = [\"%s\"]\n", pParameter->Get_Parent()->Get_Identifier());
		break;

	case PARAMETER_TYPE_Table_Fields:
		Info	+= ArcDataType(ARC_Field    ,  true);
		Info	+= CSG_String::Format("\t\tparam.parameterDependencies = [\"%s\"]\n", pParameter->Get_Parent()->Get_Identifier());
		break;

	//-----------------------------------------------------
	case PARAMETER_TYPE_Grid_System:
		return( pParameter->Get_Children_Count() > 0 );
		return( true );

	//-----------------------------------------------------
	case PARAMETER_TYPE_Font:
	case PARAMETER_TYPE_Color:
	case PARAMETER_TYPE_Colors:
	default:
		return( true );
	}

	if( Infos.Get_Count() == 0 )
	{
		Infos	+= Info + "\t\tparams  = [param]\n";
	}
	else
	{
		Infos	+= Info + "\t\tparams += [param]\n";
	}

//	#	param.filter.type = "ValueList"
//	#	param.schema.clone = True

	//-----------------------------------------------------
	if( pParameter->is_Option() )
	{
		Init	+= CSG_String::Format("\t\tTool.Set_Option('%s', parameters[%d].valueAsText)\n",
			Identifier.c_str(), Init.Get_Count()
		);
	}
	else if( pParameter->is_Input() )
	{
		Init	+= CSG_String::Format("\t\tTool.Set_Input ('%s', parameters[%d].valueAsText, '%s')\n",
			Identifier.c_str(), Init.Get_Count(), pParameter->Get_Type_Identifier().c_str()
		);
	}
	else if( pParameter->is_Output() )
	{
		Init	+= CSG_String::Format("\t\tTool.Set_Output('%s', parameters[%d].valueAsText, '%s')\n",
			Identifier.c_str(), Init.Get_Count(), pParameter->Get_Type_Identifier().c_str()
		);
	}

	//-----------------------------------------------------
	CSG_MetaData	&Desc	= *Descs.Add_Child("param");

	Desc.Add_Property(       "name", Identifier);
	Desc.Add_Property("displayname", pParameter->Get_Name      ());
	Desc.Add_Property(       "type", pParameter->is_Option() || pParameter->is_Optional() ? "Optional" : "Required");
	Desc.Add_Property(  "direction", pParameter->is_Input() ? "Input" : "Output");

	Desc.Add_Child("dialogReference", Get_Formatted(pParameter->Get_Description(PARAMETER_DESCRIPTION_ALL), FORMAT_HTML_TXT));

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CArcToolBox::Get_Parameter(CSG_Parameter *pParameter, CSG_Strings &Infos, CSG_Strings &Init, CSG_MetaData &Descs, CSG_Parameter *pGridTarget)
{
	if( !pParameter->do_UseInCMD() )
	{
		return( true );
	}

	if( pGridTarget )
	{
		if( pGridTarget == pParameter )
		{
			return( true );
		}

		if( pGridTarget == pParameter->Get_Parent() && SG_STR_CMP(pParameter->Get_Identifier(), "TARGET_USER_SIZE") )
		{
			return( true );
		}
		else if( pParameter->Get_Type() == PARAMETER_TYPE_Grid && !SG_STR_CMP(pParameter->Get_Identifier(), "TARGET_TEMPLATE") )
		{
			return( true );
		}
	}

	CSG_String	Name(pParameter->Get_Name());

	switch( pParameter->Get_Type() )
	{
	default:
		return( Get_Parameter(pParameter, Infos, Init, Descs, Name, Get_ID(pParameter, "")) );

	case PARAMETER_TYPE_Range:
		return( Get_Parameter(pParameter->asRange()->Get_LoParm(), Infos, Init, Descs, Name + " (Minimum)", Get_ID(pParameter, "MIN"))
			&&  Get_Parameter(pParameter->asRange()->Get_HiParm(), Infos, Init, Descs, Name + " (Maximum)", Get_ID(pParameter, "MAX")) );

	case PARAMETER_TYPE_Parameters:
		return( false );
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CArcToolBox::Get_Tool(CSG_Module_Library *pLibrary, int iTool, CSG_String &Code, CSG_MetaData &Description)
{
	CSG_Module	*pTool	= pLibrary->Get_Module(iTool);

	if( pTool == NULL || pTool == MLB_INTERFACE_SKIP_MODULE || pTool->needs_GUI() )
	{
		return( false );
	}

	if( pTool->Get_Parameters_Count() > 0 )
	{
		return( false );
	}

	int		i;

	Code.Clear();

	//-----------------------------------------------------
	{
		Description.Destroy();
		Description.Set_Name("metadata");
		Description.Add_Property("xml:lang", "en");

		CSG_MetaData	&Esri	= *Description.Add_Child("Esri");
		Esri.Add_Child   ("CreaDate"     , "20150910");
		Esri.Add_Child   ("CreaTime"     , "11550000");
		Esri.Add_Child   ("ArcGISFormat" , "1.0");
		Esri.Add_Child   ("SyncOnce"     , "TRUE");
		Esri.Add_Child   ("ArcGISProfile", "ItemDescription");

		CSG_MetaData	&Tool	= *Description.Add_Child("tool");
		Tool.Add_Property("name"         , CSG_String::Format("tool_%d", iTool));
		Tool.Add_Property("displayname"  , Get_Formatted(pTool->Get_Name(), FORMAT_ASCII));
		Tool.Add_Property("toolboxalias" , "");
		Tool.Add_Property("xmlns"        , "");
	//	Tool.Add_Child   ("arcToolboxHelpPath", "");
		Tool.Add_Child   ("parameters");
	//	Tool.Add_Child   ("summary"      , Get_Description(pTool, FORMAT_HTML_TXT));

		CSG_MetaData	&Info	= *Description.Add_Child("dataIdInfo");
		Info.Add_Child   ("idCitation")->Add_Child("resTitle", pTool->Get_Name());
	//	Info.Add_Child   ("idAbs"        , Get_Description(pTool, FORMAT_HTML_TXT));
		Info.Add_Child   ("idCredit"     , Get_Formatted(pTool->Get_Author(), FORMAT_ASCII));
		Info.Add_Child   ("searchKeys");
		Info("searchKeys")->Add_Child("SAGA");
	}

	//-----------------------------------------------------
	CSG_Strings	Info, Init;

	CSG_Parameter	*pGridTarget	= Get_GridTarget(pTool->Get_Parameters());

	for(i=0; i<pTool->Get_Parameters()->Get_Count(); i++)
	{
		if( !Get_Parameter(pTool->Get_Parameters()->Get_Parameter(i), Info, Init, *Description["tool"]("parameters"), pGridTarget) )
		{
			return( false );
		}
	}

	//-----------------------------------------------------
	Code	+= "\n";
	Code	+= "\n";
	Code	+= CSG_String::Format("class tool_%d(object):\n", iTool);
	Code	+= "\tdef __init__(self):\n";
	Code	+= "\t\tself.label = \""       + pTool->Get_Name() + "\"\n";
	Code	+= "\t\tself.description = \"" + Get_Description(pTool, FORMAT_HTML) + "\"\n";
	Code	+= "\t\tself.canRunInBackground = False\n";

	//-----------------------------------------------------
	Code	+= "\n";
	Code	+= "\tdef getParameterInfo(self):\n";

	for(i=0; i<Info.Get_Count(); i++)
	{
		Code	+= Info[i];
	}

	Code	+= "\t\treturn params\n";

	//-----------------------------------------------------
//	Code	+= "\n";
//	Code	+= "\tdef updateParameters(self, parameters):\n";
//	Code	+= "\t\treturn\n";

//	Code	+= "\n";
//	Code	+= "\tdef updateMessages(self, parameters):\n";
//	Code	+= "\t\treturn\n";

	//-----------------------------------------------------
	Code	+= "\n";
	Code	+= "\tdef execute(self, parameters, messages):\n";
	Code	+= "\t\tTool = ArcSAGA.SAGA_Tool('" + pLibrary->Get_Library_Name() + "', '" + pTool->Get_ID() + "')\n";

	for(i=0; i<Init.Get_Count(); i++)
	{
		Code	+= Init[i];
	}

	Code	+= "\t\tTool.Run()\n";
	Code	+= "\t\treturn\n";

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CArcToolBox::Save(CSG_Module_Library *pLibrary, const CSG_String &Directory, const CSG_Strings &Names, const CSG_Strings &Codes, const CSG_Strings &Descs)
{
	if( Codes.Get_Count() <= 0 )
	{
		return( false );
	}

	int			i;
	CSG_String	s;
	CSG_File	f;

	CSG_String	FileName	= Parameters("BOX_NAMING")->asInt() == 0
		? SG_File_Make_Path(Directory, Get_Formatted(pLibrary->Get_Library_Name(), FORMAT_FILE), SG_T(""))
		: SG_File_Make_Path(Directory, Get_Formatted(pLibrary->Get_Category    (), FORMAT_FILE), SG_T("")) + " - " + pLibrary->Get_Name();

	if( !f.Open(FileName + ".pyt", SG_FILE_W, true) )
	{
		return( false );
	}

	//-----------------------------------------------------
	s	+= "import arcpy, ArcSAGA\n";
	s	+= "\n";
	s	+= "class Toolbox(object):\n";
	s	+= "\tdef __init__(self):\n";
	s	+= "\t\tself.label = \"" + pLibrary->Get_Name() + "\"\n";
	s	+= "\t\tself.alias = \"\"\n";
	s	+= "\t\tself.tools = [";

	for(i=0; i<Names.Get_Count(); i++)
	{
		s	+= Names[i] + (i < Names.Get_Count() - 1 ? ", " : "]");
	}

	for(i=0; i<Codes.Get_Count(); i++)
	{
		s	+= Codes[i];
	}

	while( s.Length() > 0 )
	{
		f.Write(s.BeforeFirst('\n'));	f.Write("\n");	s	= s.AfterFirst('\n');
	}

	//-----------------------------------------------------
	for(i=0; i<Descs.Get_Count(); i++)
	{
		s	= FileName + "." + Names[i] + ".pyt.xml";

		if( f.Open(s, SG_FILE_W, true) )
		{
			s	= Descs[i];	s.Replace("&amp;", "&");

			while( s.Length() > 0 )
			{
				f.Write(s.BeforeFirst('\n'));	f.Write("\n");	s	= s.AfterFirst('\n');
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

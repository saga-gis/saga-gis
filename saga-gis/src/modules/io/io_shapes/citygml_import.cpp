/**********************************************************
 * Version $Id: citygml_import.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      io_shapes                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  citygml_import.cpp                   //
//                                                       //
//                 Copyright (C) 2014 by                 //
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
#include "citygml_import.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCityGML_Import::CCityGML_Import(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Import Building Sketches from CityGML"));

	Set_Author		("O.Conrad (c) 2014");

	Set_Description	(_TW(
		"This tool facilitates the import of building sketches using a CityGML based file format, "
		"that is commonly used by German land surveying offices and geoinformation distributors. "
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "BUILDINGS"	, _TL("Buildings"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_FilePath(
		NULL	, "FILES"		, _TL("Files"),
		_TL(""),
		CSG_String::Format(SG_T("%s|*.xml|%s|*.*"),
			_TL("XML Files (*.xml)"),
			_TL("All Files")
		), NULL, false, false, true
	);

	Parameters.Add_Value(
		NULL	, "PARTS"		, _TL("Check for Building Parts"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCityGML_Import::On_Execute(void)
{
	CSG_Strings	Files;

	if( !Parameters("FILES")->asFilePath()->Get_FilePaths(Files) )
	{
		return( false );
	}

	CSG_Shapes	Buildings(SHAPE_TYPE_Polygon), *pBuildings	= Parameters("BUILDINGS")->asShapes();

	for(int i=0; i<Files.Get_Count(); i++)
	{
		if( pBuildings->Get_Count() <= 0 )
		{
			Get_Buildings(Files[i], pBuildings, Parameters("PARTS")->asBool());
		}
		else if( Get_Buildings(Files[i], &Buildings, Parameters("PARTS")->asBool()) )
		{
			Add_Buildings(pBuildings, &Buildings);

			CSG_String	Description(pBuildings->Get_Description());

			Description	+= "\n";
			Description	+= Buildings.Get_Name();

			pBuildings->Set_Description(Description);
		}
	}

	return( pBuildings->Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCityGML_Import::Get_Buildings(const CSG_String &File, CSG_Shapes *pBuildings, bool bParts)
{
	//-----------------------------------------------------
	if( !Get_Buildings(File, pBuildings) )
	{
		Error_Set(_TL("CityGML file import failed"));

		return( false );
	}

	if( bParts == false )
	{
		return( true );
	}

	//-----------------------------------------------------
	// 1. check for building parts

	Process_Set_Text(CSG_String::Format(SG_T("%s: %s"), _TL("loading building parts"), SG_File_Get_Name(File, true).c_str()));

	CSG_MetaData	GML, GML_Parts;

	if( !GML.Create(File) )
	{
		Error_Set(_TL("loading failed"));

		return( false );
	}

	GML_Parts.Assign(GML, false);

	bParts	= false;

	for(int i=0; i<GML.Get_Children_Count(); i++)
	{
		if( GML[i].Get_Name().CmpNoCase("core:cityObjectMember") != 0 )
		{
			GML_Parts.Add_Child(GML[i]);
		}
		else if( Has_BuildingParts(GML[i]) && Add_BuildingParts(GML[i][0], GML_Parts) )
		{
			bParts	= true;
		}
	}

	if( bParts == false )
	{
		return( true );
	}

	//-----------------------------------------------------
	Process_Set_Text(CSG_String::Format(SG_T("%s: %s"), _TL("saving building parts"), SG_File_Get_Name(File, true).c_str()));

	CSG_String	tmpFile	= SG_File_Make_Path(SG_Dir_Get_Temp(), SG_File_Get_Name(File, true));

	if( !GML_Parts.Save(tmpFile) )
	{
		SG_File_Delete(tmpFile);

		Error_Set(_TL("check for building parts failed"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Shapes	Parts(SHAPE_TYPE_Polygon);

	if( Get_Buildings(tmpFile, &Parts) )
	{
		Add_Buildings(pBuildings, &Parts);
	}

	//-----------------------------------------------------
	SG_File_Delete(tmpFile);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCityGML_Import::Get_Buildings(const CSG_String &File, CSG_Shapes *pPolygons)
{
	//-----------------------------------------------------
	// import city gml line strings

	Process_Set_Text(_TL("importing line strings"));

	CSG_Data_Manager	tmpMgr;

	if( !tmpMgr.Add(File) || !tmpMgr.Get_Shapes() || !tmpMgr.Get_Shapes()->Get(0) )
	{
		Error_Set(CSG_String::Format(SG_T("%s: %s"), _TL("CityGML import failed"), File.c_str()));

		return( false );
	}


	//-----------------------------------------------------
	// convert line strings to polygons

	Process_Set_Text(_TL("polygon conversion"));

	CSG_Module	*pModule;

	if(	!(pModule = SG_Get_Module_Library_Manager().Get_Module(SG_T("shapes_polygons"), 3)) )	// Convert Lines to Polygons
	{
		Error_Set(_TL("could not locate line string to polygon conversion tool"));

		return( false );
	}

	CSG_Parameters	P;	P.Assign(pModule->Get_Parameters());	pModule->Set_Manager(NULL);

	bool	bResult	= pModule->Get_Parameters()->Set_Parameter("POLYGONS", pPolygons)
				&&    pModule->Get_Parameters()->Set_Parameter("LINES"   , (CSG_Shapes *)tmpMgr.Get_Shapes()->Get(0))
				&&    pModule->Get_Parameters()->Set_Parameter("MERGE"   , true)
				&&    pModule->Execute();

	pModule->Get_Parameters()->Assign_Values(&P);	pModule->Set_Manager(P.Get_Manager());

	pPolygons->Set_Name(SG_File_Get_Name(File, false));

	//-----------------------------------------------------
	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCityGML_Import::Add_Buildings(CSG_Shapes *pBuildings, CSG_Shapes *pAdd)
{
	int	i, j, *Index	= (int *)SG_Malloc(pBuildings->Get_Field_Count() * sizeof(int));

	for(i=0; i<pBuildings->Get_Field_Count(); i++)
	{
		CSG_String	Name(pBuildings->Get_Field_Name(i));

		Index[i]	= -1;

		for(j=0; Index[i]<0 && j<pAdd->Get_Field_Count(); j++)
		{
			if( !Name.CmpNoCase(pAdd->Get_Field_Name(j)) )
			{
				Index[i]	= j;
			}
		}
	}

	for(i=0; i<pAdd->Get_Count(); i++)
	{
		CSG_Shape	*pPart		= pAdd->Get_Shape(i);
		CSG_Shape	*pPolygon	= pBuildings->Add_Shape(pPart, SHAPE_COPY_GEOM);

		for(j=0; j<pBuildings->Get_Field_Count(); j++)
		{
			if( Index[j] >= 0 )
			{
				*pPolygon->Get_Value(j)	= *pPart->Get_Value(Index[j]);
			}
		}
	}

	SG_Free(Index);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCityGML_Import::Has_BuildingParts(const CSG_MetaData &GML)
{
	return( GML.Get_Name().CmpNoCase("core:cityObjectMember") == 0
		&&  GML.Get_Children_Count() == 1 && GML[0].Get_Name().CmpNoCase("bldg:Building") == 0
		&&  GML[0].Get_Child("bldg:consistsOfBuildingPart") != NULL
	);
}

//---------------------------------------------------------
bool CCityGML_Import::Add_BuildingParts(const CSG_MetaData &GML, CSG_MetaData &GML_Parts)
{
	if( GML.Get_Name().CmpNoCase("bldg:Building") )
	{
		return( false );
	}

	int				i;
	CSG_MetaData	head;

	for(i=0; i<GML.Get_Children_Count(); i++)
	{
		if( GML[i].Get_Name().CmpNoCase("core:creationDate")
		&&  GML[i].Get_Name().BeforeFirst(':').CmpNoCase("bldg") != 0 )
	//	&&  GML[i].Get_Name().BeforeFirst(':').CmpNoCase("core") != 0 )
	//	if( GML[i].Get_Name().CmpNoCase("bldg:consistsOfBuildingPart")
		{
			head.Add_Child(GML[i]);
		}
	}

	for(i=0; i<GML.Get_Children_Count(); i++)
	{
		if( GML[i]   .Get_Name().CmpNoCase("bldg:consistsOfBuildingPart") == 0 && GML[i].Get_Children_Count() == 1
		&&  GML[i][0].Get_Name().CmpNoCase("bldg:BuildingPart") == 0 )
		{
			CSG_MetaData	*pBuilding	= GML_Parts.Add_Child("core:cityObjectMember")->Add_Child(GML[i][0], false);

			pBuilding->Set_Name("bldg:Building");
			pBuilding->Add_Children(head);
			pBuilding->Add_Children(GML[i][0]);
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

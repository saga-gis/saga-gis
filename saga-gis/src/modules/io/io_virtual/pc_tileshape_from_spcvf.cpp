/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      io_virtual                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//              pc_tileshape_from_spcvf.cpp              //
//                                                       //
//                 Copyright (C) 2014 by                 //
//                    Volker Wichmann                    //
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
//    e-mail:     wichmann@laserdata.at                  //
//                                                       //
//    contact:    LASERDATA GmbH                         //
//                Management and Analysis of             //
//                Laserscanning Data                     //
//                Technikerstr. 21a                      //
//                6020 Innsbruck                         //
//                Austria                                //
//                www.laserdata.at                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "pc_tileshape_from_spcvf.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPointCloud_Create_Tileshape_From_SPCVF::CPointCloud_Create_Tileshape_From_SPCVF(void)
{
	Set_Name		(_TL("Create Tileshape from Virtual Point Cloud"));

	Set_Author		(SG_T("V. Wichmann, LASERDATA GmbH (c) 2014"));

	Set_Description	(_TW(
		"The module allows to create a polygon shapefile with the "
		"bounding boxes of a virtual point cloud dataset.\n"
		"A virtual point cloud dataset is a simple XML format "
		"with the file extension .spcvf, which can be created "
		"with the 'Create Virtual Point Cloud Dataset' module.\n\n"
	));


	//-----------------------------------------------------
	Parameters.Add_FilePath(
		NULL	, "FILENAME"		, _TL("Filename"),
		_TL("The full path and name of the .spcvf file"),
		CSG_String::Format(SG_T("%s|%s|%s|%s"),
			_TL("SAGA Point Cloud Virtual Format (*.spcvf)")	, SG_T("*.spcvf"),
			_TL("All Files")									, SG_T("*.*")
		)
 	);

	Parameters.Add_Shapes(
		NULL	, "TILE_SHP"		, _TL("Tileshape"),
		_TL("Polygon shapefile describing the bounding boxes of spcvf tiles."),
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPointCloud_Create_Tileshape_From_SPCVF::On_Execute(void)
{
	CSG_String		sFileName;
	CSG_Shapes		*pShapes;

	CSG_MetaData	SPCVF;
	CSG_String		sPathSPCVF, sFilePath;
	int				iPoints;
	double			dBBoxXMin, dBBoxYMin, dBBoxXMax, dBBoxYMax;


	//-----------------------------------------------------
	sFileName	= Parameters("FILENAME")->asString();
	pShapes		= Parameters("TILE_SHP")->asShapes();
	

	//-----------------------------------------------------
	if( !SPCVF.Create(sFileName) || SPCVF.Get_Name().CmpNoCase(SG_T("SPCVFDataset")) )
	{
		SG_UI_Msg_Add_Error(_TL("Please provide a valid *.scpvf file!"));
		return( false );
	}


	//-----------------------------------------------------
	CSG_String	sMethodPaths;
	SPCVF.Get_Property(SG_T("Paths"), sMethodPaths);

	if( !sMethodPaths.CmpNoCase(SG_T("absolute")) )
	{
		sPathSPCVF = SG_T("");
	}
	else if( !sMethodPaths.CmpNoCase(SG_T("relative")) )
	{
		sPathSPCVF = SG_File_Get_Path(sFileName);
		sPathSPCVF.Replace(SG_T("\\"), SG_T("/"));
	}
	else
	{
		SG_UI_Msg_Add_Error(_TL("Encountered invalid path description in *.spcvf file!"));
		return( false );
	}


	//-----------------------------------------------------
	pShapes->Destroy();

	pShapes->Add_Field(_TL("ID"),		SG_DATATYPE_Int);
	pShapes->Add_Field(_TL("Filepath"),	SG_DATATYPE_String);
	pShapes->Add_Field(_TL("File"),		SG_DATATYPE_String);
	pShapes->Add_Field(_TL("Points"),	SG_DATATYPE_Int);

	pShapes->Set_Name(CSG_String::Format(_TL("Tileshape_%s"), SG_File_Get_Name(sFileName, false).c_str()));


	//-----------------------------------------------------
	CSG_MetaData	*pDatasets = SPCVF.Get_Child(SG_T("Datasets"));

	for(int i=0; i<pDatasets->Get_Children_Count(); i++)
	{
		CSG_MetaData	*pDataset	= pDatasets->Get_Child(i);
		CSG_MetaData	*pBBox		= pDataset->Get_Child(SG_T("BBox"));

		pDataset->Get_Property(SG_T("File"), sFilePath);

		pDataset->Get_Property(SG_T("Points"), iPoints);

		pBBox->Get_Property(SG_T("XMin"), dBBoxXMin);
		pBBox->Get_Property(SG_T("YMin"), dBBoxYMin);
		pBBox->Get_Property(SG_T("XMax"), dBBoxXMax);
		pBBox->Get_Property(SG_T("YMax"), dBBoxYMax);

		//-----------------------------------------------------
		CSG_Shape	*pShape = pShapes->Add_Shape();

		pShape->Add_Point(dBBoxXMin, dBBoxYMin);
		pShape->Add_Point(dBBoxXMin, dBBoxYMax);
		pShape->Add_Point(dBBoxXMax, dBBoxYMax);
		pShape->Add_Point(dBBoxXMax, dBBoxYMin);

		pShape->Set_Value(0, i + 1);

		if( sPathSPCVF.Length() == 0 )	// absolute paths
		{
			pShape->Set_Value(1, sFilePath.BeforeLast('/'));
			pShape->Set_Value(2, sFilePath.AfterLast('/'));
		}
		else							// relative paths
		{
			pShape->Set_Value(1, sPathSPCVF);
			pShape->Set_Value(2, sFilePath);
		}

		pShape->Set_Value(3, iPoints);
	}


	//-----------------------------------------------------
	return( true );
}


//---------------------------------------------------------

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

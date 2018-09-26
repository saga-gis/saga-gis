/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      io_virtual                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//            pc_remove_overlap_from_spcvf.cpp           //
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
#include "pc_remove_overlap_from_spcvf.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPointCloud_Remove_Overlap_From_SPCVF::CPointCloud_Remove_Overlap_From_SPCVF(void)
{
	Set_Name		(_TL("Remove Overlap from Virtual Point Cloud Tiles"));

	Set_Author		(SG_T("V. Wichmann, LASERDATA GmbH (c) 2014"));

	Set_Description	(_TW(
		"The tool allows one to remove the overlap from point cloud "
		"tiles created from a virtual point cloud dataset. The "
		"tiles must have been created with an overlap and a spcvf "
		"tile info file must have been outputted too. The latter "
		"describes the original bounding boxes of the tiles (i.e. "
		"without overlap) and is used by this tool to remove "
		"the overlap.\n"
		"A virtual point cloud dataset is a simple XML format "
		"with the file extension .spcvf, which can be created "
		"with the 'Create Virtual Point Cloud Dataset' tool. "
		"Point cloud tiles with an overlap are usually created "
		"from such an virtual point cloud dataset with the "
		"'Get Subset from Virtual Point Cloud' tool.\n\n"
	));


	//-----------------------------------------------------
	Parameters.Add_FilePath(
		NULL	, "FILENAME"		, _TL("Tile Info File"),
		_TL("The full path and name of the spcvf tile info file describing the point cloud tiles without overlap"),
		CSG_String::Format(SG_T("%s|%s|%s|%s"),
			_TL("SAGA Point Cloud Virtual Format Tile Info (*.spcvf_tile_info)"), SG_T("*.spcvf_tile_info"),
			_TL("All Files")													, SG_T("*.*")
		)
 	);

	Parameters.Add_FilePath(
		NULL	, "FILEPATH"		, _TL("Output Filepath"),
		_TL("The full path to which the point cloud tiles without overlap should be written."),
		(const wchar_t *)0, (const wchar_t *)0,
		true, true, false
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPointCloud_Remove_Overlap_From_SPCVF::On_Execute(void)
{
	CSG_String		sFileName;
	CSG_Parameter_File_Name	*pFilePath = NULL;

	CSG_MetaData	SPCVF_Tile_Info;
	CSG_String		sPathSPCVF, sFilePath;
	double			dBBoxXMin, dBBoxYMin, dBBoxXMax, dBBoxYMax;


	//-----------------------------------------------------
	sFileName	= Parameters("FILENAME")->asString();
	pFilePath	= Parameters("FILEPATH")->asFilePath();
	

	//-----------------------------------------------------
	if( !pFilePath->is_Valid() || !pFilePath->is_Directory() )
	{
		SG_UI_Msg_Add_Error(_TL("Please provide a valid output file path!"));
		return( false );
	}

	if( !SPCVF_Tile_Info.Create(sFileName) || SPCVF_Tile_Info.Get_Name().CmpNoCase(SG_T("SPCVF_Tile_Info")) )
	{
		SG_UI_Msg_Add_Error(_TL("Please provide a valid *.scpvf_tile_info file!"));
		return( false );
	}


	//-----------------------------------------------------
	CSG_String	sMethodPaths;
	SPCVF_Tile_Info.Get_Property(SG_T("Paths"), sMethodPaths);

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
		SG_UI_Msg_Add_Error(_TL("Encountered invalid path description in *.spcvf_tile_info file!"));
		return( false );
	}


	//-----------------------------------------------------
	CSG_MetaData	*pDatasets = SPCVF_Tile_Info.Get_Child(SG_T("Tiles"));

	for(int i=0; i<pDatasets->Get_Children_Count(); i++)
	{
		CSG_MetaData	*pDataset	= pDatasets->Get_Child(i);
		CSG_MetaData	*pBBox		= pDataset->Get_Child(SG_T("BBox"));

		pBBox->Get_Property(SG_T("XMin"), dBBoxXMin);
		pBBox->Get_Property(SG_T("YMin"), dBBoxYMin);
		pBBox->Get_Property(SG_T("XMax"), dBBoxXMax);
		pBBox->Get_Property(SG_T("YMax"), dBBoxYMax);

		CSG_Rect BBox(dBBoxXMin, dBBoxYMin, dBBoxXMax, dBBoxYMax);

		CSG_String sFilePath;

		pDataset->Get_Property(SG_T("File"), sFilePath);
		sFilePath.Prepend(sPathSPCVF);

		//-----------------------------------------------------
		CSG_PointCloud	*pPC = SG_Create_PointCloud(sFilePath);

		CSG_PointCloud	*pPC_out = SG_Create_PointCloud(pPC);

		for(int iPoint=0; iPoint<pPC->Get_Count(); iPoint++)
		{
			if( dBBoxXMin <= pPC->Get_X(iPoint) && pPC->Get_X(iPoint) < dBBoxXMax &&
				dBBoxYMin <= pPC->Get_Y(iPoint) && pPC->Get_Y(iPoint) < dBBoxYMax )
			{
				pPC_out->Add_Point(pPC->Get_X(iPoint), pPC->Get_Y(iPoint), pPC->Get_Z(iPoint));

				for (int j=0; j<pPC->Get_Attribute_Count(); j++)
				{
					switch (pPC->Get_Attribute_Type(j))
					{
					default:					pPC_out->Set_Attribute(j, pPC->Get_Attribute(iPoint, j));		break;
					case SG_DATATYPE_Date:
					case SG_DATATYPE_String:	CSG_String sAttr; pPC->Get_Attribute(iPoint, j, sAttr); pPC_out->Set_Attribute(j, sAttr);		break;
					}
				}
			}
		}

		if( pPC_out->Get_Point_Count() > 0 )
		{
			CSG_String	sPath;

			sPath = pFilePath->asString();
			sPath += "/";

			pPC_out->Set_Name("%s%s", sPath.c_str(), SG_File_Get_Name(pPC->Get_Name(), false).c_str());
	
			pPC_out->Save(pPC_out->Get_Name());
		}
		else
		{
			SG_UI_Msg_Add(CSG_String::Format(_TL("Point Cloud %s is empty after removing overlap, skipping dataset!"), SG_File_Get_Name(pPC->Get_Name(), false).c_str()), true);
		}
		
		delete( pPC );
		delete( pPC_out );
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

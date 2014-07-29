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
//                pc_get_subset_spcvf.cpp                //
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
#include "pc_get_subset_spcvf.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPointCloud_Get_Subset_SPCVF::CPointCloud_Get_Subset_SPCVF(void)
{
	Set_Name		(_TL("Get Subset from Virtual Point Cloud"));

	Set_Author		(SG_T("V. Wichmann, LASERDATA GmbH (c) 2014"));

	Set_Description	(_TW(
		"The module allows to retrieve a point cloud from a virtual "
		"point cloud dataset by applying the provided area-of-interest "
		"(AOI). The extent of the AOI can be provided either as "
		"shapefile, grid or by coordinates. In case the AOI is "
		"provided as shapefile, additional functionality is available:\n"
		"* in case the output is written to a single point cloud and "
		"one or more shapes are selected in the AOI shapefile, "
		"only the extent of the selected shapes is used.\n"
		"* in case the shapefile contains several polygons and the "
		"'One Point Cloud per Polygon' parameter is checked, a point "
		"cloud dataset is outputted for each polygon. In case the "
		"'Tilename' attribute is provided, the output files are named "
		"by this attribute. Otherwise the output file names are build "
		"from the lower left coordinate of each tile.\n"
		"Optionally, an overlap can be added to the AOI.\n"
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

	if( SG_UI_Get_Window_Main() )
	{
		Parameters.Add_PointCloud_List(
			NULL	, "PC_OUT"			, _TL("Point Cloud"),
			_TL("The output point cloud(s)"),
			PARAMETER_OUTPUT
		);
	}
	else
	{
		Parameters.Add_FilePath(
			NULL	, "FILEPATH"		, _TL("Output Filepath"),
			_TL("The full path to which the output(s) should be written"),
			(const wchar_t *)0, (const wchar_t *)0,
			true, true, false
		);
	}

	CSG_Parameter *pNode = Parameters.Add_Node(NULL, "NODE_AOI", _TL("AOI"), _TL("AOI Input Settings"));

	CSG_Parameter *pNodeField = Parameters.Add_Shapes(
		pNode	, "AOI_SHP"			, _TL("Shape"),
		_TL("Shapefile describing the AOI."),
		PARAMETER_INPUT_OPTIONAL
	);
	Parameters.Add_Table_Field(
		pNodeField	, "FIELD_TILENAME", _TL("Tilename"),
		_TL("Attribute used for naming the output file(s)"), true
	);

	Parameters.Add_Grid(
		pNode	, "AOI_GRID"		, _TL("Grid"),
		_TL("Grid describing the AOI."),
		PARAMETER_INPUT_OPTIONAL, false
	);

	Parameters.Add_Range(
		pNode	, "AOI_XRANGE"		, _TL("X-Extent"),
		_TL("Minimum and maximum x-coordinate of AOI."),
		0.0, 0.0
	);
	Parameters.Add_Range(
		pNode	, "AOI_YRANGE"		, _TL("Y-Extent"),
		_TL("Minimum and maximum y-coordinate of AOI."),
		0.0, 0.0
	);

	Parameters.Add_Value(
		pNode	, "AOI_ADD_OVERLAP"	, _TL("Add Overlap"),
		_TL("Add overlap to AOI"),
		PARAMETER_TYPE_Bool, false
	);
	Parameters.Add_Value(
		Parameters("AOI_ADD_OVERLAP")	, "OVERLAP"	, _TL("Overlap"),
		_TL("Overlap [map units]"),
		PARAMETER_TYPE_Double,
		50.0, 0.0, true
	);

	Parameters.Add_Value(
		pNode	, "ONE_PC_PER_POLYGON"	, _TL("One Point Cloud per Polygon"),
		_TL("Write one point cloud dataset for each polygon"),
		PARAMETER_TYPE_Bool, false
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPointCloud_Get_Subset_SPCVF::On_Execute(void)
{
	CSG_String		sFileName;
	CSG_Parameter_PointCloud_List	*pPointCloudList;
	CSG_Parameter_File_Name	*pFilePath;
	CSG_Shapes		*pShapes;
	int				iFieldName;
	CSG_Grid		*pGrid;
	double			dAoiXMin, dAoiYMin, dAoiXMax, dAoiYMax;
	bool			bAddOverlap;
	double			dOverlap;
	bool			bMultiple;
	int				iOutputs = 1;

	CSG_Rect		AOI;
	CSG_MetaData	SPCVF;
	CSG_String		sPathSPCVF;
	double			dBBoxXMin, dBBoxYMin, dBBoxXMax, dBBoxYMax;
	CSG_Rect		BBoxSPCVF;


	//-----------------------------------------------------
	sFileName	= Parameters("FILENAME")->asString();

	if( SG_UI_Get_Window_Main() )
	{
		pPointCloudList	= Parameters("PC_OUT")->asPointCloudList();
	}
	else
	{
		pFilePath = Parameters("FILEPATH")->asFilePath();
	}

	pShapes		= Parameters("AOI_SHP")->asShapes();
	iFieldName	= Parameters("FIELD_TILENAME")->asInt();
	pGrid		= Parameters("AOI_GRID")->asGrid();
	dAoiXMin	= Parameters("AOI_XRANGE")->asRange()->Get_LoVal();
	dAoiXMax	= Parameters("AOI_XRANGE")->asRange()->Get_HiVal();
	dAoiYMin	= Parameters("AOI_YRANGE")->asRange()->Get_LoVal();
	dAoiYMax	= Parameters("AOI_YRANGE")->asRange()->Get_HiVal();

	bAddOverlap	= Parameters("AOI_ADD_OVERLAP")->asBool();
	dOverlap	= Parameters("OVERLAP")->asDouble();
	bMultiple	= Parameters("ONE_PC_PER_POLYGON")->asBool();

	if( pShapes == NULL )
	{
		bMultiple = false;
	}


	//-----------------------------------------------------
	if( pShapes == NULL && pGrid == NULL && (dAoiXMin == dAoiXMax || dAoiYMin == dAoiYMax) )
	{
		SG_UI_Msg_Add_Error(_TL("Please provide a valid AOI!"));
		return( false );
	}


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
	SPCVF.Get_Child(SG_T("BBox"))->Get_Property(SG_T("XMin"), dBBoxXMin);
	SPCVF.Get_Child(SG_T("BBox"))->Get_Property(SG_T("YMin"), dBBoxYMin);
	SPCVF.Get_Child(SG_T("BBox"))->Get_Property(SG_T("XMax"), dBBoxXMax);
	SPCVF.Get_Child(SG_T("BBox"))->Get_Property(SG_T("YMax"), dBBoxYMax);
	BBoxSPCVF.Assign(dBBoxXMin, dBBoxYMin, dBBoxXMax, dBBoxYMax);

	if( pShapes != NULL && !bMultiple )
	{
		if( pShapes->Get_Selection_Count() > 0 )
		{
			AOI = pShapes->Get_Selection(0)->Get_Extent();

			for(int i=1; i<pShapes->Get_Selection_Count(); i++)
			{
				AOI.Union(pShapes->Get_Selection(i)->Get_Extent());
			}
		}
		else
		{
			AOI = pShapes->Get_Extent();
		}
	}
	else if( pShapes != NULL && bMultiple )
	{
		iOutputs = pShapes->Get_Count();
	}
	else if( pGrid != NULL )
	{
		AOI = pGrid->Get_System().Get_Extent();
	}
	else
	{
		AOI.Assign(dAoiXMin, dAoiYMin, dAoiXMax, dAoiYMax);
	}


	//-----------------------------------------------------
	for(int iAOI=0; iAOI<iOutputs; iAOI++)
	{
		Process_Set_Text(CSG_String::Format(_TL("Processing AOI %d ..."), iAOI + 1));

		if( pShapes != NULL && bMultiple )
		{
			AOI = pShapes->Get_Shape(iAOI)->Get_Extent();
		}

		if( bAddOverlap )
		{
			AOI.Inflate(dOverlap, false);
		}
	
		if( !AOI.Intersect(BBoxSPCVF) )
		{
			SG_UI_Msg_Add(_TL("AOI does not intersect bounding box of SPCVF, nothing to do!"), true);
			continue;
		}


		//-----------------------------------------------------
		CSG_MetaData	*pDatasets = SPCVF.Get_Child(SG_T("Datasets"));

		CSG_Strings		sFilePaths;

		for(int i=0; i<pDatasets->Get_Children_Count(); i++)
		{
			CSG_MetaData	*pDataset	= pDatasets->Get_Child(i);
			CSG_MetaData	*pBBox		= pDataset->Get_Child(SG_T("BBox"));

			pBBox->Get_Property(SG_T("XMin"), dBBoxXMin);
			pBBox->Get_Property(SG_T("YMin"), dBBoxYMin);
			pBBox->Get_Property(SG_T("XMax"), dBBoxXMax);
			pBBox->Get_Property(SG_T("YMax"), dBBoxYMax);

			CSG_Rect BBox(dBBoxXMin, dBBoxYMin, dBBoxXMax, dBBoxYMax);

			if( AOI.Intersects(BBox) )
			{
				CSG_String sFilePath;

				pDataset->Get_Property(SG_T("File"), sFilePath);
				sFilePath.Prepend(sPathSPCVF);

				sFilePaths.Add(sFilePath);
			}
		}

		if( sFilePaths.Get_Count() == 0 )
		{
			SG_UI_Msg_Add(_TL("AOI does not intersect with any bounding box of the SPCVF datasets, nothing to do!"), true);
			continue;
		}


		//-----------------------------------------------------
		int				iDatasets = 0;
		CSG_PointCloud	*pPC_out;

		for(int i=0; i<sFilePaths.Get_Count() && Set_Progress(i, sFilePaths.Get_Count()); i++)
		{
			CSG_PointCloud	*pPC = SG_Create_PointCloud(sFilePaths.Get_String(i));

			if( i == 0 )
			{
				pPC_out = SG_Create_PointCloud(pPC);
			}

			bool bFound = false;

			for(int iPoint=0; iPoint<pPC->Get_Count(); iPoint++)
			{
				if( AOI.Contains(pPC->Get_X(iPoint), pPC->Get_Y(iPoint)) )
				{
					pPC_out->Add_Point(pPC->Get_X(iPoint), pPC->Get_Y(iPoint), pPC->Get_Z(iPoint));

					for(int iField=0; iField<pPC->Get_Attribute_Count(); iField++)
					{
						pPC_out->Set_Attribute(iField, pPC->Get_Attribute(iPoint, iField));
					}

					bFound = true;
				}
			}

			if( bFound )
			{
				iDatasets++;
			}

			delete( pPC );
		}

		//---------------------------------------------------------
		if( pPC_out->Get_Count() == 0 )
		{
			SG_UI_Msg_Add(_TL("AOI does not intersect with any point of the SPCVF datasets, nothing to do!"), true);
			delete( pPC_out );
			continue;
		}

		CSG_String	sPath = SG_T("");

		if( !SG_UI_Get_Window_Main() )
		{
			sPath = pFilePath->asString();
			sPath += SG_T("/");
		}

		if( bMultiple )
		{
			if( iFieldName > -1 )
			{
				pPC_out->Set_Name(CSG_String::Format(SG_T("%s%s"), sPath.c_str(), pShapes->Get_Record(iAOI)->asString(iFieldName)));
			}
			else
			{
				pPC_out->Set_Name(CSG_String::Format(SG_T("%s%d_%d"), sPath.c_str(), (int)AOI.Get_XMin(), (int)AOI.Get_YMin()));
			}
		}
		else
		{
			pPC_out->Set_Name(CSG_String::Format(SG_T("%spc_subset_%s"), sPath.c_str(), SG_File_Get_Name(sFileName, false).c_str()));
		}

		SG_UI_Msg_Add(CSG_String::Format(_TL("%d points from %d dataset(s) written to output point cloud %s."), pPC_out->Get_Count(), iDatasets, pPC_out->Get_Name()), true);


		if( SG_UI_Get_Window_Main() )
		{
			pPointCloudList->Add_Item(pPC_out);
		}
		else
		{
			pPC_out->Save(pPC_out->Get_Name());
			delete( pPC_out );
		}
	}


	//---------------------------------------------------------
	if( SG_UI_Get_Window_Main() )
	{
		for(int i=0; i<pPointCloudList->Get_Count(); i++)
		{
			CSG_Parameters	sParms;
			DataObject_Get_Parameters(pPointCloudList->asPointCloud(i), sParms);

			if( sParms("METRIC_ZRANGE") )
			{
				sParms("METRIC_ZRANGE")->asRange()->Set_Range(pPointCloudList->asPointCloud(i)->Get_Minimum(2), pPointCloudList->asPointCloud(i)->Get_Maximum(2));
			}

			DataObject_Set_Parameters(pPointCloudList->asPointCloud(i), sParms);
	
			DataObject_Update(pPointCloudList->asPointCloud(i));
		}
	}

	return( true );
}


//---------------------------------------------------------
int CPointCloud_Get_Subset_SPCVF::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("AOI_ADD_OVERLAP")) )
	{
		pParameters->Get_Parameter("OVERLAP")->Set_Enabled(pParameter->asBool());
	}

	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("AOI_SHP")) )
	{
		pParameters->Get_Parameter("FIELD_TILENAME"		)->Set_Enabled(pParameter->asShapes() != NULL);
		pParameters->Get_Parameter("ONE_PC_PER_POLYGON"	)->Set_Enabled(pParameter->asShapes() != NULL);
	}
	
	//-----------------------------------------------------
	return (1);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

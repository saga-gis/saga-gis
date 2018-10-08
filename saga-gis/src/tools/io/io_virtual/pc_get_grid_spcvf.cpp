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
//                 pc_get_grid_spcvf.cpp                 //
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
#include "pc_get_grid_spcvf.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPointCloud_Get_Grid_SPCVF_Base::CPointCloud_Get_Grid_SPCVF_Base(void)
{
	m_pShapes	= NULL;
	m_pFilePath	= NULL;
	m_pGridList	= NULL;
}


//---------------------------------------------------------
CPointCloud_Get_Grid_SPCVF_Base::~CPointCloud_Get_Grid_SPCVF_Base(void)
{
	Finalise();
}


//---------------------------------------------------------
void CPointCloud_Get_Grid_SPCVF_Base::Initialise(int iOutputs, CSG_Rect AOI, CSG_Shapes *pShapes, int iFieldName, bool bMultiple, bool bAddOverlap, double dOverlap,
												 CSG_String sFileName, CSG_Parameter_File_Name *pFilePath, CSG_Parameter_Grid_List *pGridList, double dCellsize,
												 bool bFitToCells, int iMethod, bool bConstrain, int iField, double dMinAttrRange, double dMaxAttrRange)
{
	m_iOutputs		= iOutputs;
	m_AOI			= AOI;
	m_pShapes		= pShapes;
	m_iFieldName	= iFieldName;
	m_bMultiple		= bMultiple;
	m_bAddOverlap	= bAddOverlap;
	m_dOverlap		= dOverlap;
	m_sFileName		= sFileName;
	m_pFilePath		= pFilePath;
	m_pGridList		= pGridList;
	m_dCellsize		= dCellsize;
	m_bFitToCells	= bFitToCells;
	m_iMethod		= iMethod;
	m_bConstrain	= bConstrain;
	m_iField		= iField;
	m_dMinAttrRange	= dMinAttrRange;
	m_dMaxAttrRange	= dMaxAttrRange;

	return;
}


//---------------------------------------------------------
void CPointCloud_Get_Grid_SPCVF_Base::Finalise(void)
{
	m_pShapes	= NULL;
	m_pFilePath	= NULL;
	m_pGridList	= NULL;

	return;
}


//---------------------------------------------------------
bool CPointCloud_Get_Grid_SPCVF_Base::Get_Subset(int iFieldToGrid)
{
	CSG_String		sVersion, sPathSPCVF, sMethodPaths;
	double			dBBoxXMin, dBBoxYMin, dBBoxXMax, dBBoxYMax;
	CSG_Rect		BBoxSPCVF;
	CSG_MetaData	SPCVF;


	//-----------------------------------------------------
	if( !SPCVF.Create(m_sFileName) || SPCVF.Get_Name().CmpNoCase(SG_T("SPCVFDataset")) )
	{
		SG_UI_Msg_Add_Error(_TL("Please provide a valid *.scpvf file!"));
		return( false );
	}


	CSG_MetaData	*pHeader = SPCVF.Get_Child(SG_T("Header"));

	if( pHeader != NULL )
	{
		int		iFieldCount;

		pHeader->Get_Child(SG_T("Attributes"))->Get_Property(SG_T("Count"), iFieldCount);

		if( iFieldToGrid < 0 || iFieldToGrid >= iFieldCount )
		{
			SG_UI_Msg_Add_Error(_TL("Attribute field number to grid is out of range!"));
			return( false );
		}

		if( m_bConstrain )
		{
			if( m_iField >= iFieldCount )
			{
				SG_UI_Msg_Add_Error(_TL("Constraining attribute field number is out of range!"));
				return( false );
			}
		}
	}



	//-----------------------------------------------------
	SPCVF.Get_Property(SG_T("Version"), sVersion);

	SPCVF.Get_Property(SG_T("Paths"), sMethodPaths);

	if( !sMethodPaths.CmpNoCase(SG_T("absolute")) )
	{
		sPathSPCVF = SG_T("");
	}
	else if( !sMethodPaths.CmpNoCase(SG_T("relative")) )
	{
		sPathSPCVF = SG_File_Get_Path(m_sFileName);
		sPathSPCVF.Replace(SG_T("\\"), SG_T("/"));
	}
	else
	{
		SG_UI_Msg_Add_Error(_TL("Encountered invalid path description in *.spcvf file!"));
		return( false );
	}

	//-----------------------------------------------------
	if( !sVersion.CmpNoCase(SG_T("1.0")) )
	{
		SPCVF.Get_Child(SG_T("BBox"))->Get_Property(SG_T("XMin"), dBBoxXMin);
		SPCVF.Get_Child(SG_T("BBox"))->Get_Property(SG_T("YMin"), dBBoxYMin);
		SPCVF.Get_Child(SG_T("BBox"))->Get_Property(SG_T("XMax"), dBBoxXMax);
		SPCVF.Get_Child(SG_T("BBox"))->Get_Property(SG_T("YMax"), dBBoxYMax);
	}
	else // 1.1
	{
		CSG_MetaData	*pHeader = SPCVF.Get_Child(SG_T("Header"));

		pHeader->Get_Child(SG_T("BBox"))->Get_Property(SG_T("XMin"), dBBoxXMin);
		pHeader->Get_Child(SG_T("BBox"))->Get_Property(SG_T("YMin"), dBBoxYMin);
		pHeader->Get_Child(SG_T("BBox"))->Get_Property(SG_T("XMax"), dBBoxXMax);
		pHeader->Get_Child(SG_T("BBox"))->Get_Property(SG_T("YMax"), dBBoxYMax);
	}

	BBoxSPCVF.Assign(dBBoxXMin, dBBoxYMin, dBBoxXMax, dBBoxYMax);


	//-----------------------------------------------------
	int				iDatasets = 0;
	CSG_Grid		*pGrid = NULL;
	double			dPoints = 0.0;

	for(int iAOI=0; iAOI<m_iOutputs; iAOI++)
	{
		if( m_iOutputs > 1 )
		{
			SG_UI_Process_Set_Text(CSG_String::Format("%s %d...", _TL("processing AOI"), iAOI + 1));
		}
		else
		{
			SG_UI_Process_Set_Text(_TL("Processing AOI ..."));
		}

		if( m_bMultiple )
		{
			iDatasets = 0;
		}

		if( m_pShapes != NULL )
		{
			if( m_pShapes->Get_Selection_Count() > 0 && !m_pShapes->is_Selected(iAOI) )
			{
				continue;
			}

			m_AOI = m_pShapes->Get_Shape(iAOI)->Get_Extent();
		}

		if( m_bAddOverlap )
		{
			m_AOI.Inflate(m_dOverlap, false);
		}
		else
		{
			m_dOverlap = 0.0;
		}

		if( m_AOI.Intersects(BBoxSPCVF) == INTERSECTION_None )
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

			if( m_AOI.Intersects(BBox) > INTERSECTION_None )
			{
				if( m_pShapes != NULL && !m_bAddOverlap && m_pShapes->Get_Shape(iAOI)->Intersects(BBox) == INTERSECTION_None )
				{
					continue;
				}

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
		CSG_Grid_System		System;
		int					x, y;

		if( m_bMultiple )
		{
			dPoints = 0.0;
		}

		for(int i=0; i<sFilePaths.Get_Count() && SG_UI_Process_Set_Progress(i, sFilePaths.Get_Count()); i++)
		{
			CSG_PointCloud	*pPC = SG_Create_PointCloud(sFilePaths.Get_String(i));

			if( pGrid == NULL && i == 0 )
			{
				CSG_Rect	r = m_AOI;

				if( m_bFitToCells )
					r.Inflate(0.5 * m_dCellsize, false);

				System.Assign(m_dCellsize, r);

				pGrid = SG_Create_Grid(System, SG_DATATYPE_Float);

				pGrid->Assign_NoData();
			}

			bool bFound = false;

			for(int iPoint=0; iPoint<pPC->Get_Count(); iPoint++)
			{
				if( m_AOI.Get_XMin() <= pPC->Get_X(iPoint) && pPC->Get_X(iPoint) < m_AOI.Get_XMax() &&
					m_AOI.Get_YMin() <= pPC->Get_Y(iPoint) && pPC->Get_Y(iPoint) < m_AOI.Get_YMax() )
				{
					if( m_pShapes != NULL && !m_bAddOverlap )
					{
						CSG_Shape_Polygon	*pPolygon	= (CSG_Shape_Polygon *)m_pShapes->Get_Shape(iAOI);

						if( !pPolygon->Contains(pPC->Get_X(iPoint), pPC->Get_Y(iPoint)) )
							continue;
					}

					if( m_bConstrain )
					{
						if( pPC->Get_Value(iPoint, m_iField) < m_dMinAttrRange || pPC->Get_Value(iPoint, m_iField) > m_dMaxAttrRange)
						{
							continue;
						}
					}

					if( System.Get_World_to_Grid(x, y, pPC->Get_X(iPoint), pPC->Get_Y(iPoint)) )
					{
						switch( m_iMethod )
						{
						default:
						case 0:		if( pGrid->is_NoData(x, y) || pGrid->asDouble(x, y) > pPC->Get_Value(iPoint, iFieldToGrid) )
									{
										pGrid->Set_Value(x, y, pPC->Get_Value(iPoint, iFieldToGrid));
									}
									break;

						case 1:		if( pGrid->is_NoData(x, y) || pGrid->asDouble(x, y) < pPC->Get_Value(iPoint, iFieldToGrid) )
									{
										pGrid->Set_Value(x, y, pPC->Get_Value(iPoint, iFieldToGrid));
									}
									break;
						}

						bFound = true;
						dPoints++;
					}
				}
			}

			if( bFound )
			{
				iDatasets++;
			}

			delete( pPC );
		}

		//---------------------------------------------------------
		if( m_bMultiple )
		{
			if( pGrid != NULL && dPoints == 0.0 )
			{
				SG_UI_Msg_Add(_TL("AOI does not intersect with any point of the SPCVF datasets, nothing to do!"), true);
				delete( pGrid );
				pGrid = NULL;
				continue;
			}

			Write_Subset(pGrid, iAOI, iDatasets, dPoints);

			pGrid = NULL;
		}
	}

	//---------------------------------------------------------
	if( !m_bMultiple && pGrid != NULL)
	{
		if( pGrid != NULL && dPoints == 0.0 )
		{
			SG_UI_Msg_Add(_TL("AOI does not intersect with any point of the SPCVF datasets, nothing to do!"), true);
			delete( pGrid );
			return( true );
		}

		Write_Subset(pGrid, 0, iDatasets, dPoints);
	}

	return( true );
}


//---------------------------------------------------------
void CPointCloud_Get_Grid_SPCVF_Base::Write_Subset(CSG_Grid *pGrid, int iAOI, int iDatasets, double dPoints)
{
	CSG_String	sPath = SG_T("");

	if( m_pFilePath != NULL )
	{
		sPath = m_pFilePath->asString();
		sPath += SG_T("/");
	}

	if( m_bMultiple )
	{
		if( m_iFieldName > -1 )
		{
			pGrid->Set_Name("%s%s", sPath.c_str(), m_pShapes->Get_Record(iAOI)->asString(m_iFieldName));
		}
		else
		{
			pGrid->Set_Name("%s%d_%d", sPath.c_str(), (int)(m_AOI.Get_XMin() + m_dOverlap), (int)(m_AOI.Get_YMin() + m_dOverlap));
		}
	}
	else
	{
		pGrid->Set_Name("%spc_subset_%s", sPath.c_str(), SG_File_Get_Name(m_sFileName, false).c_str());
	}

	SG_UI_Msg_Add(CSG_String::Format(_TL("%.0f points from %d dataset(s) written to output grid %s."), dPoints, iDatasets, pGrid->Get_Name()), true);

	if( m_pFilePath == NULL )
	{
		m_pGridList->Add_Item(pGrid);
	}
	else
	{
		pGrid->Save(pGrid->Get_Name());
		delete( pGrid );
	}

	return;
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPointCloud_Get_Grid_SPCVF::CPointCloud_Get_Grid_SPCVF(void)
{
	Set_Name		(_TL("Get Grid from Virtual Point Cloud"));

	Set_Author		(SG_T("V. Wichmann, LASERDATA GmbH (c) 2014"));

	Set_Description	(_TW(
		"The tool allows one to retrieve a grid from a virtual "
		"point cloud dataset by applying the provided area-of-interest "
		"(AOI). The extent of the AOI can be provided either as polygon "
		"shapefile, grid or by coordinates. Optionally, an overlap can "
		"be added to the AOI. In case an overlap is used and the AOI "
		"is provided as polygon shapfile, only the bounding boxes of the "
		"polygons are used.\n"
		"With polygon shapefiles additional functionality is available:\n"
		"* in case one or more polygons are selected, only the selected "
		"polygons are used.\n"
		"* in case the shapefile contains several polygons a grid "
		"dataset is outputted for each polygon. In case the "
		"'Tilename' attribute is provided, the output files are named "
		"by this attribute. Otherwise the output file names are build "
		"from the lower left coordinate of each tile.\n"
		"The derived datasets can be outputted either as grid "
		"list or written to an output directory. For the latter, "
		"you must provide a valid file path with the 'Optional Output "
		"Filepath' parameter.\n"
		"Optionally, the query can be constrained by providing an "
		"attribute field and a value range that must be met.\n"
		"A virtual point cloud dataset is a simple XML format "
		"with the file extension .spcvf, which can be created "
		"with the 'Create Virtual Point Cloud Dataset' tool.\n\n"
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

	Parameters.Add_Grid_List(
		NULL	, "GRID_OUT"			, _TL("Grid"),
		_TL("The output grid(s)"),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_FilePath(
		NULL	, "FILEPATH"		, _TL("Optional Output Filepath"),
		_TL("The full path to which the output(s) should be written. Leave empty to output the datasets as grid list."),
		(const wchar_t *)0, (const wchar_t *)0,
		true, true, false
	);

	Parameters.Add_Value(
		NULL	, "ATTR_FIELD_GRID"	, _TL("Attribute Field to Grid"),
		_TL("The attribute field to grid. Field numbers start with 1, so elevation is attribute field 3."),
		PARAMETER_TYPE_Int, 1, 3, true
	);

	Parameters.Add_Value(
		NULL	, "CELL_SIZE"	, _TL("Cellsize"),
		_TL("Cellsize of the output grid [map units]"),
		PARAMETER_TYPE_Double, 1.0, 0.001, true
	);

	Parameters.Add_Choice(
		NULL	, "GRID_SYSTEM_FIT"		, _TL("Grid System Fit"),
		_TL("Choose how to align the ouput grid system to the AOI"),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("nodes"),
			_TL("cells")
		), 1
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"	, _TL("Aggregation"),
		_TL("Choose how to aggregate the values"),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("lowest"),
			_TL("highest")
		), 1
	);

	Parameters.Add_Value(
		NULL	, "CONSTRAIN_QUERY"	, _TL("Constrain Query"),
		_TL("Check this parameter to constrain the query by an attribute range."),
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Value(
		Parameters("CONSTRAIN_QUERY")	, "ATTR_FIELD"	, _TL("Attribute Field"),
		_TL("The attribute field to use as constraint. Field numbers start with 1."),
		PARAMETER_TYPE_Int, 1, 1, true
	);

	Parameters.Add_Range(
		Parameters("CONSTRAIN_QUERY")	, "VALUE_RANGE"	, _TL("Value Range"),
		_TL("Minimum and maximum of attribute range []."),
		2.0, 2.0
	);


	CSG_Parameter *pNode = Parameters.Add_Node(NULL, "NODE_AOI", _TL("AOI"), _TL("AOI Input Settings"));

	CSG_Parameter *pNodeField = Parameters.Add_Shapes(
		pNode	, "AOI_SHP"			, _TL("Shape"),
		_TL("Shapefile describing the AOI."),
		PARAMETER_INPUT_OPTIONAL, SHAPE_TYPE_Polygon
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
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPointCloud_Get_Grid_SPCVF::On_Execute(void)
{
	CSG_String		sFileName;
	CSG_Parameter_Grid_List	*pGridList = NULL;
	CSG_Parameter_File_Name	*pFilePath = NULL;
	int				iFieldToGrid;
	double			dCellsize;
	bool			bFitToCells;
	int				iMethod;
	bool			bConstrain;
	int				iField;
	double			dMinAttrRange, dMaxAttrRange;
	CSG_Shapes		*pShapes;
	int				iFieldName;
	CSG_Grid		*pAOIGrid = NULL;
	double			dAoiXMin, dAoiYMin, dAoiXMax, dAoiYMax;
	bool			bAddOverlap;
	double			dOverlap;
	bool			bMultiple;
	int				iOutputs = 1;
	CSG_Rect		AOI;


	//-----------------------------------------------------
	sFileName		= Parameters("FILENAME")->asString();
	pGridList		= Parameters("GRID_OUT")->asGridList();
	pFilePath		= Parameters("FILEPATH")->asFilePath();
	iFieldToGrid	= Parameters("ATTR_FIELD_GRID")->asInt() - 1;
	dCellsize		= Parameters("CELL_SIZE")->asDouble();
	bFitToCells		= Parameters("GRID_SYSTEM_FIT")->asBool();
	iMethod			= Parameters("METHOD")->asInt();
	bConstrain		= Parameters("CONSTRAIN_QUERY")->asBool();
	iField			= Parameters("ATTR_FIELD")->asInt() - 1;
	dMinAttrRange	= Parameters("VALUE_RANGE")->asRange()->Get_Min();
	dMaxAttrRange	= Parameters("VALUE_RANGE")->asRange()->Get_Max();
	pShapes			= Parameters("AOI_SHP")->asShapes();
	iFieldName		= Parameters("FIELD_TILENAME")->asInt();
	pAOIGrid		= Parameters("AOI_GRID")->asGrid();
	dAoiXMin		= Parameters("AOI_XRANGE")->asRange()->Get_Min();
	dAoiXMax		= Parameters("AOI_XRANGE")->asRange()->Get_Max();
	dAoiYMin		= Parameters("AOI_YRANGE")->asRange()->Get_Min();
	dAoiYMax		= Parameters("AOI_YRANGE")->asRange()->Get_Max();

	bAddOverlap		= Parameters("AOI_ADD_OVERLAP")->asBool();
	dOverlap		= Parameters("OVERLAP")->asDouble();


	if( pShapes == NULL )
	{
		bMultiple = false;
	}
	else
	{
		bMultiple = true;
	}

	CSG_String	sPath = pFilePath->asString();

	if( sPath.Length() <= 1 )
	{
		pFilePath = NULL;
	}


	//-----------------------------------------------------
	if( pShapes == NULL && pAOIGrid == NULL && (dAoiXMin == dAoiXMax || dAoiYMin == dAoiYMax) )
	{
		SG_UI_Msg_Add_Error(_TL("Please provide a valid AOI!"));
		return( false );
	}


	//-----------------------------------------------------
	if( pShapes != NULL )
	{
		iOutputs = pShapes->Get_Count();
	}
	else if( pAOIGrid != NULL )
	{
		AOI = pAOIGrid->Get_System().Get_Extent();
	}
	else
	{
		AOI.Assign(dAoiXMin, dAoiYMin, dAoiXMax, dAoiYMax);
	}

	//---------------------------------------------------------

	m_Get_Grid_SPCVF.Initialise(iOutputs, AOI, pShapes, iFieldName, bMultiple, bAddOverlap, dOverlap, sFileName, pFilePath, pGridList, dCellsize, bFitToCells, iMethod,
								bConstrain, iField, dMinAttrRange, dMaxAttrRange);

	bool bResult = m_Get_Grid_SPCVF.Get_Subset(iFieldToGrid);

	m_Get_Grid_SPCVF.Finalise();

	return( bResult );
}


//---------------------------------------------------------
int CPointCloud_Get_Grid_SPCVF::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier(SG_T("CONSTRAIN_QUERY")) )
	{
		pParameters->Get_Parameter("ATTR_FIELD"			)->Set_Enabled(pParameter->asBool());
		pParameters->Get_Parameter("VALUE_RANGE"		)->Set_Enabled(pParameter->asBool());
	}

	if(	pParameter->Cmp_Identifier(SG_T("AOI_ADD_OVERLAP")) )
	{
		pParameters->Get_Parameter("OVERLAP"			)->Set_Enabled(pParameter->asBool());
	}

	if(	pParameter->Cmp_Identifier(SG_T("AOI_SHP")) )
	{
		pParameters->Get_Parameter("FIELD_TILENAME"		)->Set_Enabled(pParameter->asShapes() != NULL);
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
CPointCloud_Get_Grid_SPCVF_Interactive::CPointCloud_Get_Grid_SPCVF_Interactive(void)
{
	Set_Name		(_TL("Get Grid from Virtual Point Cloud"));

	Set_Author		(SG_T("V. Wichmann, LASERDATA GmbH (c) 2014"));

	Set_Description	(_TW(
		"The tool allows one to retrieve a grid from a virtual "
		"point cloud dataset by dragging a box (AOI) in a Map View.\n"
		"Optionally, the query can be constrained by providing an "
		"attribute field and a value range that must be met.\n"
		"A virtual point cloud dataset is a simple XML format "
		"with the file extension .spcvf, which can be created "
		"with the 'Create Virtual Point Cloud Dataset' tool.\n\n"
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

	Parameters.Add_Grid_List(
		NULL	, "GRID_OUT"			, _TL("Grid"),
		_TL("The output grid(s)"),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Value(
		NULL	, "ATTR_FIELD_GRID"	, _TL("Attribute Field to Grid"),
		_TL("The attribute field to grid. Field numbers start with 1, so elevation is attribute field 3."),
		PARAMETER_TYPE_Int, 1, 3, true
	);

	Parameters.Add_Value(
		NULL	, "CELL_SIZE"	, _TL("Cellsize"),
		_TL("Cellsize of the output grid [map units]"),
		PARAMETER_TYPE_Double, 1.0, 0.001, true
	);

	Parameters.Add_Choice(
		NULL	, "GRID_SYSTEM_FIT"		, _TL("Grid System Fit"),
		_TL("Choose how to align the ouput grid system to the AOI"),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("nodes"),
			_TL("cells")
		), 1
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"	, _TL("Aggregation"),
		_TL("Choose how to aggregate the values"),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("lowest"),
			_TL("highest")
		), 1
	);

	Parameters.Add_Value(
		NULL	, "CONSTRAIN_QUERY"	, _TL("Constrain Query"),
		_TL("Check this parameter to constrain the query by an attribute range."),
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Value(
		Parameters("CONSTRAIN_QUERY")	, "ATTR_FIELD"	, _TL("Attribute Field"),
		_TL("The attribute field to use as constraint. Field numbers start with 1."),
		PARAMETER_TYPE_Int, 1, 1, true
	);

	Parameters.Add_Range(
		Parameters("CONSTRAIN_QUERY")	, "VALUE_RANGE"	, _TL("Value Range"),
		_TL("Minimum and maximum of attribute range []."),
		2.0, 2.0
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPointCloud_Get_Grid_SPCVF_Interactive::On_Execute(void)
{
	return( true );
}


//---------------------------------------------------------
bool CPointCloud_Get_Grid_SPCVF_Interactive::On_Execute_Position(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode)
{
	switch( Mode )
	{
  default:
    break;

	//-----------------------------------------------------
	case TOOL_INTERACTIVE_LDOWN:

		m_ptDown	= ptWorld;

		return( true );

	//-----------------------------------------------------
	case TOOL_INTERACTIVE_LUP:

		CSG_Rect	AOI(m_ptDown.Get_X(), m_ptDown.Get_Y(), ptWorld.Get_X(), ptWorld.Get_Y());

		// as long as this tool only supports to drag a box, we initialize it with a fake overlap in order
		// to use CSG_Rect instead of CSG_Shape for point in polygon check in Get_Subset():
		m_Get_Grid_SPCVF.Initialise(1, AOI, NULL, -1, false, true, 0.0, Parameters("FILENAME")->asString(),
									NULL, Parameters("GRID_OUT")->asGridList(), Parameters("CELL_SIZE")->asDouble(),
									Parameters("GRID_SYSTEM_FIT")->asBool(), Parameters("METHOD")->asInt(),
									Parameters("CONSTRAIN_QUERY")->asBool(), Parameters("ATTR_FIELD")->asInt()-1,
									Parameters("VALUE_RANGE")->asRange()->Get_Min(), Parameters("VALUE_RANGE")->asRange()->Get_Max());

		bool bResult = m_Get_Grid_SPCVF.Get_Subset(Parameters("ATTR_FIELD_GRID")->asInt()-1);

		if( bResult )
		{
			CSG_Grid *pGrid = Parameters("GRID_OUT")->asGridList()->Get_Grid(Parameters("GRID_OUT")->asGridList()->Get_Grid_Count()-1);

			DataObject_Update(pGrid, SG_UI_DATAOBJECT_SHOW_LAST_MAP);
		}

		m_Get_Grid_SPCVF.Finalise();

		return( bResult );
	}

	return( false );
}


//---------------------------------------------------------
int CPointCloud_Get_Grid_SPCVF_Interactive::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier(SG_T("CONSTRAIN_QUERY")) )
	{
		pParameters->Get_Parameter("ATTR_FIELD"			)->Set_Enabled(pParameter->asBool());
		pParameters->Get_Parameter("VALUE_RANGE"		)->Set_Enabled(pParameter->asBool());
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

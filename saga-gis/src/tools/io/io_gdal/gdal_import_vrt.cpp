
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       io_gdal                         //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  gdal_import_vrt.cpp                  //
//                                                       //
//                 Copyright (C) 2019 by                 //
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
//    e-mail:     wichmann@laserdata                     //
//                                                       //
//    contact:    Volker Wichmann                        //
//                LASERDATA GmbH                         //
//                Innsbruck, Austria                     //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "gdal_import_vrt.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGDAL_Import_VRT::CGDAL_Import_VRT(void)
{
	//-----------------------------------------------------
	Set_Name	(_TL("Import from Virtual Raster (VRT)"));

	Set_Author	("V. Wichmann (c) 2019");

	Add_Reference("GDAL/OGR contributors", "2019",
		"GDAL/OGR Geospatial Data Abstraction software Library",
		"A translator library for raster and vector geospatial data formats. Open Source Geospatial Foundation.",
		SG_T("https://gdal.org"), SG_T("Link")
	);

	CSG_String	Description, Filter, Filter_All;

	Set_Description(_TW(
		"The tool allows one to clip / extract a raster subset from a virtual raster dataset (VRT). Such a VRT is "
		"actually an XML based description of a mosaic of raster datasets and can be created "
		"with the \"Create Virtual Raster (VRT)\" tool.\n\n"
	));

	
	//-----------------------------------------------------
	Parameters.Add_FilePath("",
		"VRT_NAME", _TL("VRT Filename"),
		_TL("The full path and name of the .vrt file."),
		CSG_String::Format("%s|*.vrt|%s|*.*",
			_TL("Virtual Dataset (*.vrt)"),
			_TL("All Files")
		), NULL, false, false, false
 	);

	Parameters.Add_Grid_List("",
		"GRIDS", _TL("Grids"),
		_TL(""),
		PARAMETER_OUTPUT, false
	);

	//-----------------------------------------------------
	Parameters.Add_Choice("",
		"EXTENT"	, _TL("Extent"),
		_TL("Choose how to define the clipping extent."),
		CSG_String::Format("%s|%s|%s",
			_TL("user defined"),
			_TL("grid system"),
			_TL("shapes extent")
		), 0
	);

	Parameters.Add_Grid_System("EXTENT",
		"GRIDSYSTEM", _TL("Grid System"),
		_TL("The target grid system.")
	);

	Parameters.Add_Shapes("EXTENT",
		"SHAPES"	, _TL("Shapes Extent"),
		_TL("The shapefile defining the clipping extent."),
		PARAMETER_INPUT
	);

	Parameters.Add_Double("EXTENT", "XMIN", _TL("Left"   ), _TL("The minimum x-coordinate of the clipping extent."));
	Parameters.Add_Double("EXTENT", "XMAX", _TL("Right"  ), _TL("The maximum x-coordinate of the clipping extent."));
	Parameters.Add_Double("EXTENT", "YMIN", _TL("Bottom" ), _TL("The minimum y-coordinate of the clipping extent."));
	Parameters.Add_Double("EXTENT", "YMAX", _TL("Top"    ), _TL("The maximum y-coordinate of the clipping extent."));

	Parameters.Add_Double("",
		"BUFFER"	, _TL("Buffer"),
		_TL("The optional buffer added to the clipping extent [map units]."),
		0.0, 0.0, true
	);

	Parameters.Add_Choice("",
		"MULTIPLE"		, _TL("Multiple Bands Output"),
		_TL("Choose how to handle output datasets with multiple bands."),
		CSG_String::Format("%s|%s|%s|",
			_TL("single grids"),
			_TL("grid collection"),
			_TL("automatic")
		), 2
	);

	//-----------------------------------------------------
	Parameters.Add_Bool("",
		"TRANSFORM"		, _TL("Transformation"),
		_TL("Align grid to coordinate system."),
		true
	);

	Parameters.Add_Choice("TRANSFORM",
		"RESAMPLING"	, _TL("Resampling"),
		_TL("Resampling type to be used, if grid needs to be aligned to coordinate system."),
		CSG_String::Format("%s|%s|%s|%s|",
			_TL("Nearest Neighbour"),
			_TL("Bilinear Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGDAL_Import_VRT::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("EXTENT") )
	{
		pParameters->Set_Enabled("XMIN"      , pParameter->asInt() == 0);
		pParameters->Set_Enabled("XMAX"      , pParameter->asInt() == 0);
		pParameters->Set_Enabled("YMIN"      , pParameter->asInt() == 0);
		pParameters->Set_Enabled("YMAX"      , pParameter->asInt() == 0);
		pParameters->Set_Enabled("GRIDSYSTEM", pParameter->asInt() == 1);
		pParameters->Set_Enabled("SHAPES"    , pParameter->asInt() == 2);
	}

	return( 1 );
}

///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGDAL_Import_VRT::On_Execute(void)
{
	
	//-----------------------------------------------------
	CSG_String	Filename = Parameters("VRT_NAME")->asString();
	
	Parameters("GRIDS")->asGridList()->Del_Items();


	//--------------------------------------------------------
	CSG_Rect	Extent;

	switch( Parameters("EXTENT")->asInt() )
	{
	case 0:	// user defined
		Extent.Assign(
			Parameters("XMIN")->asDouble(), Parameters("YMIN")->asDouble(),
			Parameters("XMAX")->asDouble(), Parameters("YMAX")->asDouble()
		);
		break;

	case 1: // grid system
		Extent.Assign(Parameters("GRIDSYSTEM")->asGrid_System()->Get_Extent());
		break;

	case 2:	// shapes extent
		Extent.Assign(Parameters("SHAPES")->asShapes()->Get_Extent());
		break;
	}

	if( Parameters("BUFFER")->asDouble() > 0.0 )
	{
		Extent.Inflate(Parameters("BUFFER")->asDouble(), false);
	}

	//--------------------------------------------------------
	// align query extent to grid system of VRT

	CSG_GDAL_DataSet	DataSet;

	if( DataSet.Open_Read(Filename) == false )
	{
		return( false );
	}

	CSG_Grid_System	SystemVRT	= DataSet.Get_System();
	
	double	d, minX, maxX, minY, maxY;

	d		= Extent.Get_XMin() - SystemVRT.Get_XMin(true);
	minX	= SystemVRT.Get_XMin(true) + floor(d / SystemVRT.Get_Cellsize()) * SystemVRT.Get_Cellsize() + 0.5 * SystemVRT.Get_Cellsize();
	d		= Extent.Get_XMax() - SystemVRT.Get_XMax(true);
	maxX	= SystemVRT.Get_XMax(true) + ceil(d / SystemVRT.Get_Cellsize()) * SystemVRT.Get_Cellsize() - 0.5 * SystemVRT.Get_Cellsize();
	d		= Extent.Get_YMin() - SystemVRT.Get_YMin(true);
	minY	= SystemVRT.Get_YMin(true) + floor(d / SystemVRT.Get_Cellsize()) * SystemVRT.Get_Cellsize() + 0.5 * SystemVRT.Get_Cellsize();
	d		= Extent.Get_YMax() - SystemVRT.Get_YMax(true);
	maxY	= SystemVRT.Get_YMax(true) + ceil(d / SystemVRT.Get_Cellsize()) * SystemVRT.Get_Cellsize() - 0.5 * SystemVRT.Get_Cellsize();
		
	Extent.Set_BottomLeft(minX, minY);
	Extent.Set_TopRight(maxX, maxY);

	//--------------------------------------------------------
	// check if we query outside the VRT extent, and if so, set these
	// areas to No Data in the result later; currently the used
	// VRTAddSimpleSource() in Open_Read does not support the masking
	// of these cells; this work around does not help with empty areas
	// within the VRT extent

	bool	bMaskOutsideVRT = false;

	if( Extent.Get_XMin() < SystemVRT.Get_XMin(true) || Extent.Get_XMax() > SystemVRT.Get_XMax(true) ||
		Extent.Get_YMin() < SystemVRT.Get_YMin(true) || Extent.Get_YMax() > SystemVRT.Get_YMax(true) )
	{
		bMaskOutsideVRT = true;
	}


	//--------------------------------------------------------
	// read subset

	CSG_Grid_System	System(SystemVRT.Get_Cellsize(), Extent);

	if( !System.is_Valid() )
	{
		Error_Set(_TL("Invalid grid system, verify extent of subset."));
		return( false );
	}
		
	if( DataSet.Open_Read(Filename, System) == false || DataSet.Get_Count() < 1 )
	{
		return( false );
	}

	//-----------------------------------------------------
	TSG_Grid_Resampling	Resampling;

	switch( Parameters("RESAMPLING")->asInt() )
	{
	default:	Resampling	= GRID_RESAMPLING_NearestNeighbour;	break;
	case  1:	Resampling	= GRID_RESAMPLING_Bilinear        ;	break;
	case  2:	Resampling	= GRID_RESAMPLING_BicubicSpline   ;	break;
	case  3:	Resampling	= GRID_RESAMPLING_BSpline         ;	break;
	}

	CSG_Vector	A;	CSG_Matrix	B;	DataSet.Get_Transformation(A, B);

	bool	bTransform	= Parameters("TRANSFORM")->asBool() && DataSet.Needs_Transformation();

	//-----------------------------------------------------
	Message_Add("\n", false);
	Message_Fmt("\n%s: %s", _TL("Driver" ), DataSet.Get_DriverID().c_str());
	Message_Fmt("\n%s: %d", _TL("Bands"  ), DataSet.Get_Count   ()        );
	Message_Fmt("\n%s: %d", _TL("Rows"   ), DataSet.Get_NX      ()        );
	Message_Fmt("\n%s: %d", _TL("Columns"), DataSet.Get_NY      ()        );
	Message_Add("\n", false);

	if( DataSet.Needs_Transformation() )
	{
		Message_Fmt("\n%s:", _TL("Transformation")                               );
		Message_Fmt("\n  x' = %.6f + x * %.6f + y * %.6f", A[0], B[0][0], B[0][1]);
		Message_Fmt("\n  y' = %.6f + x * %.6f + y * %.6f", A[1], B[1][0], B[1][1]);
		Message_Add("\n", false);
	}

	//-----------------------------------------------------
	CSG_Array_Pointer	pGrids;

	for(int i=0; i<DataSet.Get_Count() && Process_Get_Okay(); i++)
	{
		CSG_String	Message	= "%s: " + SG_File_Get_Name(Filename, false);	if( DataSet.Get_Count() > 1 )	Message	+= CSG_String::Format(" [%d/%d]", i + 1, DataSet.Get_Count());

		Process_Set_Text(Message.c_str(), _TL("loading"));

		CSG_Grid	*pGrid	= DataSet.Read(i);

		if( pGrid != NULL )
		{
			if( bMaskOutsideVRT )
			{
				for(int y=0; y<pGrid->Get_NY() && Set_Progress(y); y++)
				{
					double yWorld = pGrid->Get_System().Get_yGrid_to_World(y);

					#pragma omp parallel for
					for(int x=0; x<pGrid->Get_NX(); x++)
					{
						double xWorld = pGrid->Get_System().Get_xGrid_to_World(x);

						if( xWorld < SystemVRT.Get_XMin(true) || xWorld > SystemVRT.Get_XMax(true) ||
							yWorld < SystemVRT.Get_YMin(true) || yWorld > SystemVRT.Get_YMax(true) )
						{
							pGrid->Set_NoData(x, y);
						}
					}
				}
			}

			if( bTransform )
			{
				Process_Set_Text(Message.c_str(), _TL("translation"));

				DataSet.Get_Transformation(&pGrid, Resampling, true);
			}

			pGrid->Set_Name(CSG_String::Format("VRT_%s", SG_File_Get_Name(Filename, false).c_str()) + (DataSet.Get_Count() == 1 ? CSG_String("") : CSG_String::Format(" [%s]", DataSet.Get_Name(i).c_str())));

			pGrids.Add(pGrid);
		}
    }

	//-----------------------------------------------------
	CSG_Parameter_Grid_List	*pList	= Parameters("GRIDS")->asGridList();

	if( Parameters("MULTIPLE")->asInt() == 0 || (Parameters("MULTIPLE")->asInt() == 2 && pGrids.Get_Size() == 1) )
	{
		for(size_t i=0; i<pGrids.Get_Size(); i++)
		{
			pList->Add_Item((CSG_Grid *)pGrids[i]);
		}
	}
	else if( pGrids.Get_Size() > 0 )
	{
		CSG_Grids	*pCollection	= SG_Create_Grids();

		pCollection->Set_Name(CSG_String::Format("VRT_%s", SG_File_Get_Name(Filename, false).c_str()));
		pCollection->Set_Description(DataSet.Get_Description());

		pCollection->Get_MetaData().Add_Child("GDAL_DRIVER", DataSet.Get_DriverID());

		for(size_t i=0; i<pGrids.Get_Size(); i++)
		{
			pCollection->Add_Grid(i, (CSG_Grid *)pGrids.Get(i), true);
		}

		pList->Add_Item(pCollection);
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

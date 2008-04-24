
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library                     //
//                                                       //
//                     io_grid_gdal                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   gdal_import.cpp                     //
//                                                       //
//            Copyright (C) 2007 O. Conrad               //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     oconrad@saga-gis.de                    //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Bundesstr. 55                          //
//                D-20146 Hamburg                        //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "gdal_import.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGDAL_Import::CGDAL_Import(void)
{
	Set_Name	(_TL("Import Raster via GDAL"));

	Set_Author	(SG_T("(c) 2007 by O.Conrad (A.Ringeler)"));

	CSG_String	Description;

	Description	= _TW(
		"The \"GDAL Raster Import\" module imports grid data from various file formats using the "
		"\"Geospatial Data Abstraction Library\" (GDAL) by Frank Warmerdam. "
		"For more information have a look at the GDAL homepage:\n"
		"  <a target=\"_blank\" href=\"http://www.gdal.org/\">"
		"  http://www.gdal.org</a>\n"
		"\n"
		"Following raster formats are currently supported:\n"
		"<table border=\"1\"><tr><th>Name</th><th>Description</th></tr>\n"
	);

	for(int i=0; i<g_GDAL_Driver.Get_Count(); i++)
    {
		Description	+= CSG_String::Format(SG_T("<tr><td>%s</td><td>%s</td></tr>\n"),
			g_GDAL_Driver.Get_Identifier(i),
			g_GDAL_Driver.Get_Name(i)
		);
    }

	Description	+= SG_T("</table>");

	Set_Description(Description);

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL, "GRIDS"	, _TL("Grids"),
		_TL(""),
		PARAMETER_OUTPUT, false
	);

	Parameters.Add_FilePath(
		NULL, "FILE"	, _TL("File"),
		_TL("")
	);
}

//---------------------------------------------------------
CGDAL_Import::~CGDAL_Import(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGDAL_Import::On_Execute(void)
{
	double					zMin, *zLine;
	CSG_String				File_Name, Summary;
	CSG_Parameter_Grid_List	*pGrids;
	CSG_Grid				*pGrid;
	CGDAL_System			System;
	GDALDataset				*pDataset;
	GDALRasterBand			*pBand;

	//-----------------------------------------------------
	File_Name	= Parameters("FILE")	->asString();
	pGrids		= Parameters("GRIDS")	->asGridList();
	pGrids		->Del_Items();

	//-----------------------------------------------------
	if( System.Create(pDataset = (GDALDataset *)GDALOpen(File_Name.b_str(), GA_ReadOnly)) == false )
	{
		Message_Add(_TL("Unable to find appropriate import drivers."));
	}
	else
    {
		File_Name	= SG_File_Get_Name(File_Name, true);

		Summary	+= CSG_String::Format(
			SG_T("%s: %s/%s\n"),
			_TL("Driver"),
			pDataset->GetDriver()->GetDescription(), 
			pDataset->GetDriver()->GetMetadataItem(GDAL_DMD_LONGNAME)
		);

		Summary	+= CSG_String::Format(
			SG_T("%s: x %d, y %d\n%s: %d\n%s: x %.6f, y %.6f\n%s: x %.6f, y %.6f"),
			_TL("Cells")	, System.Get_NX(), System.Get_NY(),
			_TL("Bands")	, pDataset->GetRasterCount(),
			_TL("Offset")	, System.Get_xMin(), System.Get_yMin(),
			_TL("Cellsize")	, System.Get_DX(), System.Get_DY()
		);

		if( pDataset->GetProjectionRef() != NULL )
		{
			Summary	+= CSG_String::Format(
				SG_T("\n%s: %s"),
				_TL("Projection"),
				pDataset->GetProjectionRef()
			);
		}

		Message_Add(Summary);

		//-------------------------------------------------
		zLine	= (double *)SG_Malloc(System.Get_NX() * sizeof(double));

		//-------------------------------------------------
		for(int i=0; i<pDataset->GetRasterCount(); i++)
		{
			pBand	= pDataset->GetRasterBand(i + 1);
			zMin	= pBand->GetOffset();

			pGrids->Add_Item(
				pGrid	= SG_Create_Grid(g_GDAL_Driver.Get_Grid_Type(pBand->GetRasterDataType()),
					System.Get_NX(),
					System.Get_NY(),
					System.Get_DX(),
					System.Get_xMin(),
					System.Get_yMin()
				)
			);

			pGrid->Set_Name(pDataset->GetRasterCount() > 1
				? CSG_String::Format(SG_T("%s [%02d]"), File_Name.c_str(), i + 1).c_str()
				: File_Name.c_str()
			);

			pGrid->Set_Unit			(CSG_String(pBand->GetUnitType()));
			pGrid->Set_NoData_Value	(pBand->GetNoDataValue());
			pGrid->Set_ZFactor		(pBand->GetScale());

			DataObject_Add			(pGrid);
			DataObject_Set_Colors	(pGrid, CSG_Colors(100, SG_COLORS_BLACK_WHITE, false));

			for(int y=0; y<System.Get_NY() && Set_Progress(y, System.Get_NY()); y++)
			{
				if( pBand->RasterIO(GF_Read, 0, y, System.Get_NX(), 1, zLine, System.Get_NX(), 1, GDT_Float64, 0, 0) == CE_None )
				{
					for(int x=0; x<System.Get_NX(); x++)
					{
					//	double	NaN	= 0.0;	NaN	= -1.0 / NaN;	if( NaN == zLine[x] )	pGrid->Set_NoData(x, System.Get_NY() - 1 - y); else

						pGrid->Set_Value (x, System.Get_NY() - 1 - y, zMin + zLine[x]);
					}
				}
			}
        }

		//-------------------------------------------------
		SG_Free(zLine);

		GDALClose(pDataset);

		return( true );
	}

	//-----------------------------------------------------
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

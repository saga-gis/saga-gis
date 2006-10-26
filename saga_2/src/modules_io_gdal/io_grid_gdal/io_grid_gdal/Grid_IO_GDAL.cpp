
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library                     //
//                                                       //
//                      $$modul$$                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     Grid_IO_GDAL.cpp                  //
//                                                       //
//            Copyright (C) 2003 Your Name               //
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
//    e-mail:     your@e-mail.abc                        //
//                                                       //
//    contact:    Your Name                              //
//                And Address                            //
//                                                       //
///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Grid_IO_GDAL.h"

#include <gdal_priv.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_IO_GDAL::CGrid_IO_GDAL(void)
{
	GDALAllRegister();

	//-----------------------------------------------------
	Set_Name	(_TL("Import using GDAL (various raster formats)"));

	Set_Author	(_TL("Copyrights (c) 2004 by Andre Ringeler"));

	CSG_String	s(_TL(
		"Import GDAL. "
		"This Module imports various raster formats using:\n"
		"GDAL - Geospatial Data Abstraction Library\n"
		"by Frank Warmerdam\n"
		"for more look at:\n"
		"  <a target=\"_blank\" href=\"http://www.remotesensing.org/gdal/index.html\">"
		"  http://www.remotesensing.org/gdal/index.html</a>\n"
		"\n"
		"Imports the following Rasterformats:\n"
		"<table border=\"1\"><tr><th>Name</th><th>Description</th></tr>\n"
	));
	
	for(int i=0; i<GDALGetDriverCount(); i++)
    {
		GDALDriverH	hDriver	= GDALGetDriver(i);

		s.Append(CSG_String::Format("<tr><td>%s</td><td>%s</td></tr>\n",
			GDALGetDriverShortName(hDriver),
			GDALGetDriverLongName (hDriver)
		));
    }

	s.Append("</table>");

	Set_Description(s);

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL, "RESULT_LIST"	, _TL("Grid List"),
		"",
		PARAMETER_OUTPUT, false
	);

	Parameters.Add_FilePath(
		NULL, "FILE_DATA"	, _TL("File"),
		""
	);
}

//---------------------------------------------------------
CGrid_IO_GDAL::~CGrid_IO_GDAL(void)
{
	GDALDestroyDriverManager();	
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
// The only thing left to do is to realize your ideas whithin
// the On_Execute() function (which will be called automatically
// by the framework). But that's really your job :-)

bool CGrid_IO_GDAL::On_Execute(void)
{
	int		x, y;
	int		x_pos, y_pos;
	int		i;
	double GeoX, GeoY; 
	GDALDataset  *pDataset;
	CSG_String	msg;
   
	pDataset = (GDALDataset *) GDALOpen( Parameters("FILE_DATA")->asString(), GA_ReadOnly );
  
	if( pDataset != NULL )
    {
    
		double        adfGeoTransform[6];

		msg.Printf("Driver: %s/%s\n",
				pDataset->GetDriver()->GetDescription(), 
				pDataset->GetDriver()->GetMetadataItem( GDAL_DMD_LONGNAME ) );

		Message_Add(msg);

		msg.Printf("Size is %dx%dx%d\n", 
				pDataset->GetRasterXSize(), pDataset->GetRasterYSize(),
				pDataset->GetRasterCount() );
		
		Message_Add(msg);

		if( pDataset->GetProjectionRef()  != NULL )
		{
			Message_Add(CSG_String::Format("Projection is `%s'\n", pDataset->GetProjectionRef()));
		}

		if( pDataset->GetGeoTransform( adfGeoTransform ) == CE_None )
		{
			Message_Add(CSG_String::Format("Origin = (%.6f,%.6f)\n", adfGeoTransform[0], adfGeoTransform[3]));

			Message_Add(CSG_String::Format("Pixel Size = (%.6f,%.6f)\n", adfGeoTransform[1], adfGeoTransform[5]));
		}
		

   

		

		for(i=0 ; i< pDataset->GetRasterCount(); i++ )
		{
			
			if(adfGeoTransform[5]<0.0)
				Gdal_To_World(adfGeoTransform, 0 , pDataset->GetRasterYSize(), GeoX,  GeoY );
			else
			{
				adfGeoTransform[5]*=-1.0;
				Gdal_To_World(adfGeoTransform, 0 , pDataset->GetRasterYSize(), GeoX,  GeoY );
			}
			
			CSG_Grid *pGrid = SG_Create_Grid(
				GRID_TYPE_Float,  
				pDataset->GetRasterXSize(),
				pDataset->GetRasterYSize(),
				adfGeoTransform[1], 
				//fabs(adfGeoTransform[5]),
				GeoX/*+adfGeoTransform[1]/2.0*/,
				GeoY/*+fabs(adfGeoTransform[5]/2.0*/
			);
			
		
			CSG_Parameter_Grid_List *List = Parameters("RESULT_LIST")->asGridList();

			List->Add_Item(pGrid);

			GDALRasterBand  *pBand;

			pBand = pDataset->GetRasterBand( i+1 );

			pGrid->Set_Unit (pBand->GetUnitType());

			pGrid->Set_NoData_Value(pBand->GetNoDataValue());

			pGrid->Set_ZFactor(pBand->GetScale()); 

			double offfset = pBand->GetOffset();

			float * pScanline = (float *) CPLMalloc(sizeof(float)*pDataset->GetRasterXSize());

			for (y = 0; y < pGrid->Get_NY() && Set_Progress(y, pGrid->Get_NY()); y ++)
			{
				
				pBand->RasterIO( GF_Read, 0, y, pDataset->GetRasterXSize(), 1, 
							  pScanline, pDataset->GetRasterXSize(), 1, GDT_Float32, 
							  0, 0 );
				
				for (x = 0; x < pGrid->Get_NX(); x ++)
				{ 
						Gdal_To_World(adfGeoTransform, x , y, GeoX,  GeoY );
						World_To_Saga(pGrid ,  GeoX,  GeoY, x_pos, y_pos);
                        if(pGrid->is_InGrid(x_pos, y_pos))
						    pGrid->Set_Value(x_pos, y_pos ,pScanline[x]+offfset);
				}
			}
			CPLFree(pScanline);
        }
        GDALClose(pDataset);
	}
	else
	{
       Message_Add(_TL("No Import Driver found\n"));
    }
	//-----------------------------------------------------
	return( true );
}

void CGrid_IO_GDAL::Gdal_To_World (double GeoTransform[6], int x, int y, double &GeoX, double &GeoY )
{
	GeoX = GeoTransform[0] + GeoTransform[1] * x + GeoTransform[2] * y;
	GeoY = GeoTransform[3] + GeoTransform[4] * x + GeoTransform[5] * y;
}

void CGrid_IO_GDAL::World_To_Saga (CSG_Grid *grid , double GeoX, double GeoY, int &x, int &y )
{
	x	=(int)	((GeoX - grid->Get_XMin())/grid->Get_Cellsize() );
	y	=(int)	((GeoY - grid->Get_YMin())/grid->Get_Cellsize() );
}

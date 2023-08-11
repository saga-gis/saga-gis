
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
//                 gdal_catalogue_vrt.cpp                //
//                                                       //
//                 Copyright (C) 2023 by                 //
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
#include "gdal_catalogue_vrt.h"

#include <map>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGDAL_CatalogueVRT::CGDAL_CatalogueVRT(void)
{
	Set_Name	(_TL("Create Raster Catalogue from Virtual Raster (VRT)"));

	Set_Author	("V. Wichmann (c) 2023");

	Set_Description(
		"The tool allows one to create a polygon layer that shows the extent of "
		"each raster file referenced in the virtual raster. Each extent is attributed "
		"with the original file path, which can be used to load the dataset by "
		"'CTRL + left-click' in the table field.\n\n"
		"Note: the tool only supports basic variants of the VRT format."
	);

	Add_Reference("GDAL/OGR contributors", "2019",
		"GDAL/OGR Geospatial Data Abstraction software Library",
		"A translator library for raster and vector geospatial data formats. Open Source Geospatial Foundation.",
		SG_T("https://gdal.org"), SG_T("Link")
	);

	Parameters.Add_FilePath("",
		"VRT_FILE"	, _TL("VRT File"),
		_TL("The full path and name of the .vrt input file."),
		CSG_String::Format("%s (*.vrt)|*.vrt|%s|*.*",
			_TL("Virtual Dataset"),
			_TL("All Files")
		), NULL, false, false, false
 	);

	Parameters.Add_Shapes("",
		"CATALOGUE", _TL("Raster Catalogue"),
		_TL("The polygon layer with dataset boundaries."),
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGDAL_CatalogueVRT::On_Execute(void)
{
	CSG_String VRTFileName = Parameters("VRT_FILE")->asString();

	CSG_MetaData VRT;

	if( !VRT.Create(VRTFileName) || VRT.Get_Name().CmpNoCase(SG_T("VRTDataset")) )
	{
		SG_UI_Msg_Add_Error(_TL("Invalid input file!"));
	return (false);
	}


	//---------------------------------------------------------
	CSG_Shapes	*pCatalogue	= Parameters("CATALOGUE")->asShapes();

	pCatalogue->Create(SHAPE_TYPE_Polygon, _TL("Raster Catalogue"));

	pCatalogue->Add_Field("ID"      , SG_DATATYPE_Int   );
	pCatalogue->Add_Field("NAME"    , SG_DATATYPE_String);
	pCatalogue->Add_Field("FILE"    , SG_DATATYPE_String);
	pCatalogue->Add_Field("CRS"     , SG_DATATYPE_String);
	pCatalogue->Add_Field("BANDS"   , SG_DATATYPE_Int   );
	pCatalogue->Add_Field("CELLSIZE", SG_DATATYPE_Double);
	pCatalogue->Add_Field("ROWS"    , SG_DATATYPE_Int   );
	pCatalogue->Add_Field("COLUMNS" , SG_DATATYPE_Int   );


	//---------------------------------------------------------
	CSG_MetaData *pSRS =  VRT.Get_Child("SRS");
	CSG_String    WKT  = "";

	if (pSRS != NULL)
	{
		WKT = pSRS->Get_Content();

		if (WKT.Length() > 0)
		{
			pCatalogue->Get_Projection().Assign(WKT, SG_PROJ_FMT_WKT);
		}
	}


	//---------------------------------------------------------
	CSG_String GeoTransform = VRT.Get_Child("GeoTransform")->Get_Content();

	CSG_String_Tokenizer	tkz(GeoTransform, ",", SG_TOKEN_STRTOK);
	
	double	Transform[6];	// ULX, CellsizeX, RotX, ULY, RotY, CellsizeY
	int		i = 0;

	while( tkz.Has_More_Tokens() )
	{
		Transform[i] = tkz.Get_Next_Token().asDouble();
		i++;
	}

	if( Transform[2] != 0. || Transform[4] != 0. )
	{
		SG_UI_Msg_Add_Error(_TL("Rotated rasters are not supported!"));
		return( false );
	}


	//---------------------------------------------------------
	struct DATA {
			double	xOff		{ 0.0 };
			double	yOff		{ 0.0 };
			int		xSize		{ 0 };
			int		ySize		{ 0 };
			int		bands		{ 1 };
			int		relative	{ 0 };
	};

	std::map<std::wstring, DATA>	Datasets;
	

	for(int iBand=0; iBand<VRT.Get_Children_Count(); iBand++)
	{
		CSG_MetaData *pBand = VRT.Get_Child(iBand);
		
		if( pBand->Get_Name().Cmp(SG_T("VRTRasterBand")) != 0 )
		{
			continue;
		}

		for(int iSource=0; iSource<pBand->Get_Children_Count(); iSource++)
		{
			CSG_MetaData *pSource = pBand->Get_Child(iSource);

			if( pSource->Get_Name().Cmp(SG_T("SimpleSource")) != 0 && pSource->Get_Name().Cmp(SG_T("ComplexSource")) != 0 )
			{
				continue;
			}

			CSG_String FileName = pSource->Get_Child("SourceFilename")->Get_Content();

			std::map<std::wstring, DATA>::iterator it = Datasets.find(FileName.to_StdWstring());

			if( it != Datasets.end() )
			{
				it->second.bands++;
			}
			else
			{
				CSG_MetaData *pDstRect = pSource->Get_Child("DstRect");

				DATA d;

				pDstRect->Get_Property(SG_T("xOff") , d.xOff);
				pDstRect->Get_Property(SG_T("yOff") , d.yOff);
				pDstRect->Get_Property(SG_T("xSize"), d.xSize);
				pDstRect->Get_Property(SG_T("ySize"), d.ySize);

				pSource->Get_Child("SourceFilename")->Get_Property(SG_T("relativeToVRT"), d.relative);

				Datasets.insert(std::pair<std::wstring, DATA>(FileName.to_StdWstring(), d));
			}
		}
	}


	//---------------------------------------------------------
	int cnt = 0;

	for(std::map<std::wstring, DATA>::iterator it=Datasets.begin(); it!=Datasets.end(); ++it)
	{
		cnt++;

		DATA *d = &it->second;

		CSG_Shape * pShape = pCatalogue->Add_Shape();

		pShape->Add_Point(Transform[0] +  d->xOff * Transform[1]             , Transform[3] +  d->yOff * Transform[5]);
		pShape->Add_Point(Transform[0] + (d->xOff + d->xSize) * Transform[1] , Transform[3] +  d->yOff * Transform[5]);
		pShape->Add_Point(Transform[0] + (d->xOff + d->xSize) * Transform[1] , Transform[3] + (d->yOff + d->ySize) * Transform[5]);
		pShape->Add_Point(Transform[0] +  d->xOff * Transform[1]             , Transform[3] + (d->yOff + d->ySize) * Transform[5]);

		CSG_String dname = it->first.c_str();

		CSG_String name, fullpath;
		
		if( d->relative == 0 )
		{
			name     = SG_File_Get_Name(dname, true);
			fullpath = dname;
		}
		else
		{
			name     = dname;
			fullpath = SG_File_Make_Path(SG_File_Get_Path(VRTFileName), name);
		}

		pShape->Set_Value(0, cnt);
		pShape->Set_Value(1, name);
		pShape->Set_Value(2, fullpath);
		pShape->Set_Value(3, WKT);
		pShape->Set_Value(4, d->bands);
		pShape->Set_Value(5, Transform[1]);
		pShape->Set_Value(6, d->ySize);
		pShape->Set_Value(7, d->xSize);
	}


	//---------------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

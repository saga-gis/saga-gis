
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                                                       //
//                       io_gdal                         //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 gdal_import_aster.cpp                 //
//                                                       //
//            Copyright (C) 2018 O. Conrad               //
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
//    e-mail:     oconrad@saga-gis.de                    //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "gdal_import_aster.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGDAL_Import_ASTER::CGDAL_Import_ASTER(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Import ASTER Scene"));

	Set_Author		("O.Conrad (c) 2018");

	Set_Description	(_TW(
		"Import ASTER scene from Hierarchical Data Format version 4 (HDF4). "
	));

	Add_Reference("https://asterweb.jpl.nasa.gov/", _TL("ASTER Homepage Jet Propulsion Laboratory"));
	Add_Reference("https://lpdaac.usgs.gov/dataset_discovery/aster", _TL("ASTER Overview at NASA/USGS"));

	//-----------------------------------------------------
	Parameters.Add_FilePath("",
		"FILE"		, _TL("File"),
		_TL(""),
		CSG_String::Format("%s (*.hdf)|*.hdf|%s|*.*",
			_TL("HDF4 Files"),
			_TL("All Files")
		), NULL, false
	);

	Parameters.Add_Choice("",
		"FORMAT"	, _TL("Format"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("single grids"),
			_TL("grid collections")
		), 1
	);

	Parameters.Add_Grids_Output("", "VNIR", _TL("Visible and Near Infrared"), _TL(""));
	Parameters.Add_Grids_Output("", "SWIR", _TL("Short Wave Infrared"      ), _TL(""));
	Parameters.Add_Grids_Output("",  "TIR", _TL("Thermal Infrared"         ), _TL(""));

	Parameters.Add_Grid_List("", "BANDS", _TL("Bands"), _TL(""), PARAMETER_OUTPUT, false);

	Parameters.Add_Table("", "METADATA"	, _TL("Metadata"), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGDAL_Import_ASTER::On_Execute(void)
{
	//-----------------------------------------------------
	const double	Wave[14][2]	=
	{
		{	 0.520,  0.600	},
		{	 0.630,  0.690	},
		{	 0.760,  0.860	},
		{	 1.600,  1.700	},
		{	 2.145,  2.185	},
		{	 2.185,  2.225	},
		{	 2.235,  2.285	},
		{	 2.295,  2.365	},
		{	 2.360,  2.430	},
		{	 8.125,  8.475	},
		{	 8.475,  8.825	},
		{	 8.925,  9.275	},
		{	10.250, 10.950	},
		{	10.950, 11.650	}
	};

	//-----------------------------------------------------
	CSG_GDAL_DataSet	DataSet;

	if( !DataSet.Open_Read(Parameters("FILE")->asString()) )
	{
		Error_Fmt("%s [%s]", _TL("could not open file"), Parameters("FILE")->asString());

		return( false );
	}

	CSG_String	FileName	= SG_File_Get_Name(Parameters("FILE")->asString(), false);

	if( DataSet.Get_DriverID().Cmp("HDF4") )
	{
		Message_Fmt("\n%s: %s [%s]\n", _TL("Warning"), _TL("Driver"), DataSet.Get_DriverID().c_str());
	}

	//-----------------------------------------------------
	CSG_MetaData	MetaData;

	if( !DataSet.Get_MetaData(MetaData) )
	{
		Error_Fmt("%s [%s]", _TL("failed to read metadata"), FileName.c_str());

		return( false );
	}
	else if( Parameters("METADATA")->asTable() )
	{
		CSG_Table	*pTable	= Parameters("METADATA")->asTable();

		pTable->Destroy();
		pTable->Fmt_Name("%s [%s]", FileName.c_str(), _TL("Metadata"));
		pTable->Add_Field("KEY"  , SG_DATATYPE_String);
		pTable->Add_Field("VALUE", SG_DATATYPE_String);

		for(int i=0; i<MetaData.Get_Children_Count(); i++)
		{
			CSG_Table_Record	*pRecord	= pTable->Add_Record();

			pRecord->Set_Value(0, MetaData[i].Get_Name   ());
			pRecord->Set_Value(1, MetaData[i].Get_Content());
		}
	}

	//-----------------------------------------------------
	TSG_Rect	Extent;	CSG_Projection	Projection;

	if( !Get_System(MetaData, Extent, Projection) )
	{
		Error_Fmt("%s [%s]", _TL("failed to read spatial reference"), FileName.c_str());

		return( false );
	}

	//-----------------------------------------------------
	CSG_Strings	SubDataSets	= DataSet.Get_SubDataSets();

	if( SubDataSets.Get_Count() < 1 )
	{
		Error_Fmt("%s [%s]", _TL("failed to read subdatasets"), FileName.c_str());

		return( false );
	}

	//-----------------------------------------------------
	CSG_Parameter_Grid_List *pBands = NULL; CSG_Grids *pVNIR, *pSWIR, *pTIR; CSG_Table Attributes;

	if( Parameters("FORMAT")->asInt() == 0 )
	{
		pBands	= Parameters("BANDS")->asGridList();

		pBands->Del_Items();
	}
	else
	{
		Attributes.Add_Field("ID"    , SG_DATATYPE_Char  );
		Attributes.Add_Field("NAME"  , SG_DATATYPE_String);
		Attributes.Add_Field("WAVMIN", SG_DATATYPE_Double);
		Attributes.Add_Field("WAVMID", SG_DATATYPE_Double);
		Attributes.Add_Field("WAVMAX", SG_DATATYPE_Double);
		Attributes.Add_Record();

		Parameters("VNIR")->Set_Value(pVNIR = SG_Create_Grids());
		pVNIR->Get_Attributes_Ptr()->Create(&Attributes);
		pVNIR->Fmt_Name("%s %s", FileName.c_str(), SG_T("VNIR"));
		pVNIR->Get_MetaData().Add_Child(MetaData)->Set_Name("ASTER");
		pVNIR->Set_Z_Attribute (3);
		pVNIR->Set_Z_Name_Field(1);

		Parameters("SWIR")->Set_Value(pSWIR = SG_Create_Grids());
		pSWIR->Get_Attributes_Ptr()->Create(&Attributes);
		pSWIR->Fmt_Name("%s %s", FileName.c_str(), SG_T("SWIR"));
		pSWIR->Get_MetaData().Add_Child(MetaData)->Set_Name("ASTER");
		pSWIR->Set_Z_Attribute (3);
		pSWIR->Set_Z_Name_Field(1);

		Parameters( "TIR")->Set_Value(pTIR  = SG_Create_Grids());
		pTIR ->Get_Attributes_Ptr()->Create(&Attributes);
		pTIR ->Fmt_Name("%s %s", FileName.c_str(), SG_T( "TIR"));
		pTIR ->Get_MetaData().Add_Child(MetaData)->Set_Name("ASTER");
		pTIR ->Set_Z_Attribute (3);
		pTIR ->Set_Z_Name_Field(1);
	}

	//-----------------------------------------------------
	for(int i=0; i<SubDataSets.Get_Count() && Process_Get_Okay(); i++)
	{
		CSG_Grid	*pDataSet	= SubDataSets[i].Find("EOS_SWATH") > 0 && DataSet.Open_Read(SubDataSets[i]) ? DataSet.Read(0) : NULL;

		if( pDataSet )
		{
			CSG_Grid	*pBand	= SG_Create_Grid(
				pDataSet->Get_Type(), pDataSet->Get_NX(), pDataSet->Get_NY(),
				(Extent.xMax - Extent.xMin) / (pDataSet->Get_NX() - 1),
				Extent.xMin, Extent.yMin
			);

			if( pBand )
			{
				CSG_String	Band	= SubDataSets[i].AfterLast(':'); Band.Replace("ImageData", "");
				int			iBand	= Band.asInt();

				Band.Printf("Band %02d %s", iBand, iBand <= 3 ? SG_T("VNIR") : iBand <= 9 ? SG_T("SWIR") : SG_T("TIR"));

				pBand->Set_Name(Band);
				pBand->Get_Projection().Create(Projection);
				pBand->Set_NoData_Value(0.0);
				pBand->Set_Description(pDataSet->Get_MetaData().asText());

				#pragma omp parallel for
				for(int y=0; y<pDataSet->Get_NY(); y++)
				{
					for(int x=0; x<pDataSet->Get_NX(); x++)
					{
						pBand->Set_Value(x, y, pDataSet->asDouble(x, y));
					}
				}

				if( pBands )
				{
					Parameters("BANDS")->asGridList()->Add_Item(pBand);
				}
				else
				{
					Attributes[0].Set_Value(0, iBand);
					Attributes[0].Set_Value(1,  Band);
					Attributes[0].Set_Value(2,  Wave[iBand - 1][0]);
					Attributes[0].Set_Value(3, (Wave[iBand - 1][0] + Wave[iBand - 1][1]) / 2.0);
					Attributes[0].Set_Value(4,  Wave[iBand - 1][1]);

					if( iBand <= 3 )
					{
						pVNIR->Add_Grid(Attributes[0], pBand, true);
					}
					else if( iBand <= 9 )
					{
						pSWIR->Add_Grid(Attributes[0], pBand, true);
					}
					else // if( iBand <= 14 )
					{
						pTIR ->Add_Grid(Attributes[0], pBand, true);
					}
				}
			}

			delete(pDataSet);
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGDAL_Import_ASTER::Get_System(const CSG_MetaData &MetaData, TSG_Rect &Extent, CSG_Projection &Projection)
{
	if( !MetaData("UPPERLEFTM")
	||  !MetaData("LOWERRIGHTM")
	||  !MetaData("UTMZONENUMBER")
	||  !MetaData("NORTHBOUNDINGCOORDINATE") )
	{
		return( false );
	}

	Extent.xMin	= MetaData["UPPERLEFTM"   ].Get_Content().AfterFirst (',').asDouble();
	Extent.yMax	= MetaData["UPPERLEFTM"   ].Get_Content().BeforeFirst(',').asDouble();
	Extent.xMax	= MetaData["LOWERRIGHTM"  ].Get_Content().AfterFirst (',').asDouble();
	Extent.yMin	= MetaData["LOWERRIGHTM"  ].Get_Content().BeforeFirst(',').asDouble();

	int	Zone	= MetaData["UTMZONENUMBER"].Get_Content().asInt(), EPSG_ID;

	bool	bSouth	= MetaData["NORTHBOUNDINGCOORDINATE"].Get_Content().asDouble() < 0.0;

	if( bSouth )
	{
		Extent.yMin	+= 10000000;
		Extent.yMax	+= 10000000;

		EPSG_ID	= 32700 + Zone;
	}
	else
	{
		EPSG_ID	= 32600 + Zone;
	}

	if( !Projection.Create(EPSG_ID) )
	{
		CSG_String	Proj4;
		
		Proj4.Printf("+proj=utm +zone=%d +datum=WGS84 +units=m +no_defs ");

		if( bSouth )
		{
			Proj4	+= "+south ";
		}

		Projection.Create(Proj4, SG_PROJ_FMT_Proj4);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

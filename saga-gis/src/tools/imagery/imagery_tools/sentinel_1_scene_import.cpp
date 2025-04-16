
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     image_tools                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               sentinel_1_scene_import.cpp             //
//                                                       //
//                 Olaf Conrad (C) 2025                  //
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
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "sentinel_1_scene_import.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSentinel_1_Scene_Import::CSentinel_1_Scene_Import(void)
{
	Set_Name		(_TL("Import Sentinel-1 Scene"));

	Set_Author		("O.Conrad (c) 2025");

	Set_Description	(_TW(
		"A simple import tool for Sentinel-1 scenes. "
	));

	Add_Reference("https://sentinel.esa.int/web/sentinel/missions/sentinel-1",
		SG_T("Sentinel-1 at ESA's Sentinel Online")
	);

	Add_Reference("https://sentinel.esa.int/documents/247904/349449/S1_SP-1322_1.pdf",
		SG_T("Sentinel-1 Documentation")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid_List("", "BANDS"    , _TL("Bands"     ), _TL(""), PARAMETER_OUTPUT);

	Parameters.Add_Shapes   ("", "TIEPOINTS", _TL("Tie Points"), _TL(""), PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point);

	Parameters.Add_FilePath ("", "FILE"     , _TL("File"      ), _TL("The \'manifest.safe\' file for the scene."),
		CSG_String::Format("%s (*.safe)|manifest.safe|%s|*.*", _TL("Sentinel-1 Metadata"), _TL("All Files"))
	);

	m_CRS.Create(Parameters); Parameters.Set_Parameter("CRS_STRING", "epsg:4326"); m_CRS.On_Parameter_Changed(&Parameters, Parameters("CRS_STRING"));

	m_Grid_Target.Create(&Parameters, false, "TARGET_NODE", "TARGET_");
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSentinel_1_Scene_Import::On_Before_Execution(void)
{
	m_CRS.Activate_GUI();

	return( CSG_Tool::On_Before_Execution() );
}

//---------------------------------------------------------
bool CSentinel_1_Scene_Import::On_After_Execution(void)
{
	m_CRS.Deactivate_GUI();

	return( CSG_Tool::On_After_Execution() );
}

//---------------------------------------------------------
int CSentinel_1_Scene_Import::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	m_CRS        .On_Parameter_Changed(pParameters, pParameter);
	m_Grid_Target.On_Parameter_Changed(pParameters, pParameter);

	if( pParameter->Cmp_Identifier("FILE") || pParameter->Cmp_Identifier("CRS_PICKER") )
	{
		CSG_Grid_System System = Get_System((*pParameters)("FILE")->asString(), (*pParameters)("CRS_STRING")->asString());

		if( System.is_Valid() )
		{
			pParameters->Set_Parameter("TARGET_USER_XMIN", System.Get_XMin    ());
			pParameters->Set_Parameter("TARGET_USER_XMAX", System.Get_XMax    ());
			pParameters->Set_Parameter("TARGET_USER_YMIN", System.Get_YMin    ());
			pParameters->Set_Parameter("TARGET_USER_YMAX", System.Get_YMax    ());
			pParameters->Set_Parameter("TARGET_USER_SIZE", System.Get_Cellsize());

			m_Grid_Target.On_Parameter_Changed(pParameters, (*pParameters)("TARGET_USER_SIZE"));
		}
	}

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CSentinel_1_Scene_Import::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	m_Grid_Target.On_Parameters_Enable(pParameters, pParameter);

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSentinel_1_Scene_Import::On_Execute(void)
{
	CSG_Strings Files[2];

	if( !Get_Files(Parameters("FILE")->asString(), Files) )
	{
		return( false );
	}

	CSG_Projection CRS;

	if( !m_CRS.Get_CRS(CRS) || !CRS.is_Okay() )
	{
		CRS.Set_GCS_WGS84();
	}

	CSG_Shapes *pPoints = Parameters("TIEPOINTS")->asShapes(), Points; if( !pPoints ) { pPoints = &Points; } CSG_String Error;

	if( !Get_Geolocations(Files[1][0], *pPoints, CRS, Error) )
	{
		Error_Set(Error);

		return( false );
	}

	//-----------------------------------------------------
	CSG_Data_Manager Data; CSG_Parameters P; CSG_Parameter_Grid_List *pGrids = P.Add_Grid_List("", "GRIDS", "Grids", "", PARAMETER_INPUT)->asGridList();

	for(int i=0; i<Files[0].Get_Count() && Process_Get_Okay(); i++)
	{
		SG_UI_Msg_Lock(true);
		CSG_Grid *pGrid = Data.Add_Grid(Files[0][i]);
		SG_UI_Msg_Lock(false);

		if( pGrid )
		{
			pGrids->Add_Item(pGrid); pGrid->Set_NoData_Value(0.);
		}
	}

	//-----------------------------------------------------
	CSG_Tool *pTool = SG_Get_Tool_Library_Manager().Get_Tool("pj_georeference", 10); // Rectify Grid List

	if( !pTool )
	{
		Error_Fmt("%s: %s", _TL("failed to request tool"), _TL("Rectify Grid List"));

		return( false );
	}

	Process_Set_Text(_TL("projecting..."));

	CSG_Grid_System System(m_Grid_Target.Get_System());

	SG_UI_Msg_Lock(true);

	if( !pTool->Settings_Push()
	||  !pTool->Set_Parameter("GRIDS"            , pGrids)
	||  !pTool->Set_Parameter("REF_SOURCE"       , pPoints)
	||  !pTool->Set_Parameter("XFIELD"           , "x")
	||  !pTool->Set_Parameter("YFIELD"           , "y")
	||  !pTool->Set_Parameter("METHOD"           , 0) // "Automatic"
	||  !pTool->Set_Parameter("RESAMPLING"       , 3) // "B-Spline Interpolation"
	||  !pTool->Set_Parameter("DATA_TYPE"        , 10) // "Preserve"
	||  !pTool->Set_Parameter("CRS_STRING"       , CRS.Get_WKT())
	||  !pTool->Set_Parameter("TARGET_DEFINITION", 1) // "user defined"
	||  !pTool->Set_Parameter("TARGET_SYSTEM"    , &System)
	||  !pTool->Execute() )
	{
		SG_UI_Msg_Lock(false);

		Error_Fmt("%s: %s", _TL("failed to execute tool"), pTool->Get_Name().c_str());

		pTool->Settings_Pop(); Data.Delete();

		return( false );
	}

	SG_UI_Msg_Lock(false);

	//-----------------------------------------------------
	CSG_Parameter_Grid_List &Bands = *Parameters("BANDS")->asGridList(); Bands.Del_Items();
	CSG_Parameter_Grid_List &Grids = *pTool->Get_Parameter("TARGET_GRIDS")->asGridList();

	for(int i=0; i<Grids.Get_Grid_Count(); i++)
	{
		Bands.Add_Item(Grids.Get_Grid(i));

		Get_MetaData(Files[1][i], *Bands.Get_Grid(i));
	}

	pTool->Settings_Pop(); Data.Delete();

	//-----------------------------------------------------
	if( Parameters("TIEPOINTS")->asShapes() )
	{
		for(int i=0; i<pPoints->Get_Count(); i++)
		{
			CSG_Shape &Point = *pPoints->Get_Shape(i);

			Point.Set_Point(Point.asDouble("x"), Point.asDouble("y"));
		}

		pPoints->Get_Projection() = CRS;
	}

	//-----------------------------------------------------
	return( Bands.Get_Grid_Count() == Files[0].Get_Count() );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSentinel_1_Scene_Import::Get_Files(const CSG_String &File, CSG_Strings Files[2])
{
	CSG_MetaData Manifest;
	
	if( !Manifest.Load(File) )
	{
		Error_Fmt("%s [%s]", _TL("metadata file did not load"), File.c_str());

		return( false );
	}

	// <xfdu:XFDU ...>
	//   <informationPackageMap>
	//   <metadataSection>
	//   <dataObjectSection>

	if( !Manifest("dataObjectSection") )
	{
		Error_Fmt("%s [%s]", _TL("meta data object section not found"), File.c_str());

		return( false );
	}

	CSG_String Directory = SG_File_Get_Path(File);

	const CSG_MetaData &Objects = Manifest["dataObjectSection"];

	for(int i=0; i<Objects.Get_Count(); i++)
	{
		if( Objects[i].Cmp_Property("repID", "s1Level1ProductSchema"    ) && Objects[i]("byteStream") && Objects[i]["byteStream"]("fileLocation") && Objects[i]["byteStream"]["fileLocation"].Get_Property("href") )
		{
			Files[1] += Directory + Objects[i]["byteStream"]["fileLocation"].Get_Property("href");
		}

		if( Objects[i].Cmp_Property("repID", "s1Level1MeasurementSchema") && Objects[i]("byteStream") && Objects[i]["byteStream"]("fileLocation") && Objects[i]["byteStream"]["fileLocation"].Get_Property("href") )
		{
			Files[0] += Directory + Objects[i]["byteStream"]["fileLocation"].Get_Property("href");
		}
	}

	return( Files[0].Get_Size() && Files[0].Get_Size() == Files[1].Get_Size() );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSentinel_1_Scene_Import::Get_MetaData(const CSG_String &File, CSG_Grid &Band)
{
	CSG_MetaData Annotation;

	if( !Annotation.Load(File) || !Annotation("adsHeader") )
	{
		Error_Fmt("%s [%s]", _TL("metadata file not found"), File.c_str());

		return( false );
	}

	if( Annotation("adsHeader") )
	{
		CSG_MetaData &Header = Annotation["adsHeader"];

		Band.Get_MetaData().Add_Child(Header);

		if( Header("missionId") && Header("polarisation") && Header("stopTime") )
		{
			CSG_String Time(Header("stopTime")->Get_Content()); Time = Time.BeforeFirst('T');

			Band.Fmt_Name("%s-%s [%s]",
				Header.Get_Content("missionId"   ),
				Header.Get_Content("polarisation"),
				Time.c_str()
			);
		}
	}

	if( Annotation("imageAnnotation.imageInformation") )
	{
		Band.Get_MetaData().Add_Child(Annotation["imageAnnotation.imageInformation"]);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSentinel_1_Scene_Import::Get_Geolocations(const CSG_String &File, CSG_Shapes &Points, const CSG_Projection &CRS, CSG_String &Error)
{
	CSG_MetaData Annotation;

	if( !Annotation.Load(File) )
	{
		Error.Printf("%s [%s]", _TL("metadata file did not load"), File.c_str());

		return( false );
	}

	//-----------------------------------------------------
	int Count;

	if( !Annotation("geolocationGrid") || !Annotation["geolocationGrid"]("geolocationGridPointList") || !Annotation["geolocationGrid"]["geolocationGridPointList"].Get_Property("count", Count) || Count < 1 )
	{
		Error.Printf("%s [%s]", _TL("metadata file does not provide geolocations"), File.c_str());

		return( false );
	}

	CSG_MetaData &List = Annotation["geolocationGrid"]["geolocationGridPointList"];

	//-----------------------------------------------------
	int nCols = 0, nRows = 0;

	if( Annotation("imageAnnotation.imageInformation") )
	{
		Annotation["imageAnnotation"]["imageInformation"].Get_Content("numberOfSamples", nCols);
		Annotation["imageAnnotation"]["imageInformation"].Get_Content("numberOfLines"  , nRows);
	}

	//-----------------------------------------------------
	const int nTags = 7; const char *Tag[nTags] =
	{
		"pixel", "line", "longitude", "latitude", "height", "incidenceAngle", "elevationAngle"
	};

	Points.Create(SHAPE_TYPE_Point, _TL("Geolocations"));

	for(int j=0; j<nTags; j++)
	{
		Points.Add_Field(Tag[j], SG_DATATYPE_Double);
	}

	for(int i=0; i<List.Get_Count(); i++)
	{
		double Values[nTags]; bool bOkay = true;

		for(int j=0; bOkay && j<nTags; j++)
		{
			bOkay = List[i].Get_Content(Tag[j], Values[j]);
		}

		if( bOkay )
		{
			CSG_Shape &Point = *Points.Add_Shape();

			Point.Set_Point(Values[0], nRows < 1 ? Values[1] : (double)nRows - 1. - Values[1]);

			for(int j=0; j<7; j++)
			{
				Point.Set_Value(j, Values[j]);
			}
		}
	}

	if( Points.Get_Count() != Count )
	{
		Error.Printf("%s [%s]", _TL("failed to load geolocations"), File.c_str());

		return( false );
	}

	//-----------------------------------------------------
	if( nRows < 1 )
	{
		nRows = (int)Points.Get_Maximum(1) - 1;

		for(int i=0; i< Points.Get_Count(); i++)
		{
			CSG_Shape &Point = *Points.Get_Shape(i);

			Point.Set_Point(Point.Get_Point().x, nRows - Point.Get_Point().y);
		}
	}

	//-----------------------------------------------------
	Points.Add_Field("x", SG_DATATYPE_Double);
	Points.Add_Field("y", SG_DATATYPE_Double);

	if( CRS == CSG_Projection::Get_GCS_WGS84() || !CRS.is_Okay() )
	{
		for(int i=0; i<Points.Get_Count(); i++)
		{
			CSG_Shape &Point = *Points.Get_Shape(i);

			Point.Set_Value("x", Point.asDouble("longitude"));
			Point.Set_Value("y", Point.asDouble("latitude" ));
		}
	}
	else
	{
		CSG_Tool *pTool = SG_Get_Tool_Library_Manager().Get_Tool("pj_proj4", 31); // Coordinate Conversion (Table)

		SG_UI_Msg_Lock(true);

		if( !pTool || !pTool->Settings_Push(NULL)
		||  !pTool->Set_Parameter("TABLE"   , &Points)
		||  !pTool->Set_Parameter("SOURCE_X", "longitude")
		||  !pTool->Set_Parameter("SOURCE_Y", "latitude" )
		||  !pTool->Set_Parameter("TARGET_X", "x")
		||  !pTool->Set_Parameter("TARGET_Y", "y")
		||  !pTool->Set_Parameter("SOURCE_CRS.CRS_STRING", "epsg:4326")
		||  !pTool->Set_Parameter("TARGET_CRS.CRS_STRING", CRS.Get_WKT())
		||  !pTool->Execute() )
		{
			SG_UI_Msg_Lock(false); pTool->Settings_Pop();

			Error.Printf("failed to project geolocations");

			return( false );
		}

		SG_UI_Msg_Lock(false); pTool->Settings_Pop();
	}

	//-----------------------------------------------------
	return( true );
}

///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Grid_System CSentinel_1_Scene_Import::Get_System(const CSG_String &File, const CSG_Projection &CRS)
{
	CSG_Grid_System System; CSG_MetaData Manifest;

	if( Manifest.Load(File) && Manifest("dataObjectSection") )
	{
		const CSG_MetaData &Objects = Manifest["dataObjectSection"]; CSG_String Annotation;

		for(int i=0; Annotation.is_Empty() && i<Objects.Get_Count(); i++)
		{
			if( Objects[i].Cmp_Property("repID", "s1Level1ProductSchema") && Objects[i]("byteStream.fileLocation") && Objects[i]["byteStream.fileLocation"].Get_Property("href") )
			{
				Annotation = SG_File_Get_Path(File) + Objects[i]["byteStream.fileLocation"].Get_Property("href");
			}
		}

		CSG_Shapes Points; CSG_String Error;

		if( Get_Geolocations(Annotation, Points, CRS, Error) && Points.Get_Count() > 1 )
		{
			CSG_Shape &A = *Points.Get_Shape(0), &B = *Points.Get_Shape(Points.Get_Count() - 1);

			double dImg = SG_Get_Distance(A.Get_Point(), B.Get_Point()), dCRS = SG_Get_Distance(A.asDouble("x"), A.asDouble("y"), B.asDouble("x"), B.asDouble("y"));

			if( dImg > 0. && dCRS > 0. )
			{
				double Cellsize = dCRS / dImg;

				if( Cellsize > 0. && CRS.is_Projection() )
				{
					Cellsize = (int)(0.5 + Cellsize);
				}

				System.Create(Cellsize,
					Points.Get_Minimum(Points.Get_Field("x")), Points.Get_Minimum(Points.Get_Field("y")),
					Points.Get_Maximum(Points.Get_Field("x")), Points.Get_Maximum(Points.Get_Field("y"))
				);
			}
		}
	}

	return( System );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

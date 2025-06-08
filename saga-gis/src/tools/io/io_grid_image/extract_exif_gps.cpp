
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       image_io                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 extract_exif_gps.cpp                  //
//                                                       //
//                 Copyright (C) 2025 by                 //
//                  Justus Spitzmueller                  //
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
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "extract_exif_gps.h"

#if HAVE_EXIV2

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CExtract_EXIF_GPS::CExtract_EXIF_GPS(void)
{
	Set_Name		(_TL("Extract Image GPS Tags"));

	Set_Author		("J.Spitzm\u00FCller \u00A9 2025");

	Set_Description (_TW(
		"This tool extracts EXIF GPS tags from image files and creates corresponding point features using the Exiv2 library. "
		"One can either select individual files or let the tool recursively search entire directories for supported image formats. "
		"The output features can be assigned a target coordinate system, and the tool will reproject the GPS coordinates accordingly. The default projection is Web Mercator."
		"The original GPS values are also preserved for reference.\n\n"
		"The following GPS information is currently extracted:\n"
		"<ul>"
		"<li>Latitude</li>"
		"<li>Longitude</li>"
		"<li>Altitude</li>"
		"<li>Direction</li>"
		"<li>Compass method (Magnetic or True North)</li>"
		"<li>Dilution of Precision (DOP)</li>"
		"</ul>\n\n"
		"This tool can also add the point features to a new or the currently active map and display the corresponding images next to each point. "
		"To modify the display settings, go to <i>Settings > Display > Image Field</i>."
));

	Add_Reference( "https://exiv2.org/", SG_T("Exiv2 C++ metadata library"));

	//-----------------------------------------------------
	Parameters.Add_Shapes("", "SHAPES", _TL("Shape"), _TL("Shape"), PARAMETER_OUTPUT );

	m_CRS.Create(Parameters, "SHAPES");

	//-----------------------------------------------------
	Parameters.Add_Choice("", 
		"SOURCE", _TL("Select From"), 
		_TL(""), 
		CSG_String::Format("%s|%s|", 
			_TL("Files"), 
			_TL("Directory")
		),0
	);

	Parameters.Add_FilePath("SOURCE",
		"FILES"     , _TL("Set Image Files"),
		_TL(""),
		CSG_String::Format(
			"%s"                        "|*.jpg;*.jif;*.jpeg;*.raw;*.dng|"
			"%s (*.jpg, *.jif, *.jpeg)" "|*.jpg;*.jif;*.jpeg|"
			"%s (*.raw, *.dng)" 		"|*.raw;*.dng|"
			"%s"                        "|*.*",
			_TL("Recognized File Types"      ),
			_TL("JPEG - JFIF Compliant"      ),
			_TL("RAW Images"      			 ),
			_TL("All Files"                  )
		),
		NULL, false, false, true
	);

	Parameters.Add_FilePath("SOURCE",
		"DIRECTORY"     , _TL("Set Image Directory"),
		_TL(""),
		NULL, NULL, false, true, false
	);

	Parameters.Add_Bool("SOURCE",
		"RECURSIVE", _TL("Recursive"), 
		_TL(""), false
	);
	
	//-----------------------------------------------------
	Parameters.Add_Choice("", 
		"ADD", _TL("Add to Map"), 
		_TL(""), 
		CSG_String::Format("%s|%s|%s|", 
			_TL("Don't Add"), 
			_TL("Add to New Map"),
			_TL("Add to Active Map")
		),1
	)->Set_UseInCMD( false );

}
//---------------------------------------------------------
bool CExtract_EXIF_GPS::On_Before_Execution(void)
{
	m_CRS.Activate_GUI();

	if( has_GUI() )
	{
		Parameters.Set_Parameter("SHAPES", DATAOBJECT_CREATE);
	}

	return( CSG_Tool::On_Before_Execution() );
}

//---------------------------------------------------------
bool CExtract_EXIF_GPS::On_After_Execution(void)
{
	m_CRS.Deactivate_GUI();


	return( CSG_Tool::On_After_Execution() );
}

//---------------------------------------------------------
int CExtract_EXIF_GPS::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	m_CRS.On_Parameter_Changed(pParameters, pParameter);

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CExtract_EXIF_GPS::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("SOURCE") )
	{
		pParameters->Set_Enabled( "FILES", 		pParameter->asInt() == 0 );
		pParameters->Set_Enabled( "DIRECTORY", 	pParameter->asInt() == 1 );
		pParameters->Set_Enabled( "RECURSIVE", 	pParameter->asInt() == 1 );
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}

//---------------------------------------------------------
bool CExtract_EXIF_GPS::Get_Coordinate( double& Coordinate, const Exiv2::Value& Value, const std::string& Ref )
{
	if( Value.count() != 3)
	{
		return false;
	}

    double Deg = Convert_Rational( Value, 0);
    double Min = Convert_Rational( Value, 1);
    double Sec = Convert_Rational( Value, 2);

	Coordinate = SG_Degree_To_Decimal( Deg, Min, Sec );
    
	if( Ref == "S" || Ref == "W") 
	{
		Coordinate *= -1;
	}

    return( true );

}

//---------------------------------------------------------
double CExtract_EXIF_GPS::Convert_Rational( const Exiv2::Value& Value, const size_t Position )
{
    return( Value.toRational(Position).first / static_cast<double>(Value.toRational(Position).second) );
}

///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CExtract_EXIF_GPS::On_Execute(void)
{
	CSG_Strings Files;
	CSG_String 	Name = "Images";

	if( Parameters("SOURCE")->asInt() == 0 )
	{
		if( !Parameters("FILES")->asFilePath()->Get_FilePaths(Files) )
		{
			Error_Set(_TL("No files found"));
			return( false );
		}
	}
	else 
	{
		CSG_Strings Directory;
		if( !Parameters("DIRECTORY")->asFilePath()->Get_FilePaths(Directory) )
		{
			Error_Set(_TL("No directory found"));
			return( false );
		}
		Name = SG_File_Get_Name(Directory[0], false);


		bool 			Recursive = Parameters("RECURSIVE")->asBool();
		const SG_Char* 	Ext[5] = {L"jpg",L"jif",L"jpeg",L"raw",L"dng"};
		CSG_Strings 	Extentions = CSG_Strings(5, Ext);
		for(int i=0; i<Extentions.Get_Count(); i++)
		{
			CSG_Strings List;
			SG_Dir_List_Files(List, Directory[0], Extentions[i], Recursive);
			Files += List;
		}
	}
		
	CSG_Shapes *pShapes = Parameters("SHAPES")->asShapes();
	pShapes->Create(SHAPE_TYPE_Point, Name, NULL, SG_VERTEX_TYPE_XYZ);
	m_CRS.Get_CRS(pShapes->Get_Projection(), true);

	if( pShapes->Get_Projection().Get_Type() == ESG_CRS_Type::Undefined )
	{
		pShapes->Get_Projection().Set_GCS_WGS84();
	}

	CSG_Projection Target = pShapes->Get_Projection();
	CSG_Projection GPS; GPS.Set_GCS_WGS84();

	pShapes->Add_Field("File", 			SG_DATATYPE_String );
	pShapes->Add_Field("Path", 			SG_DATATYPE_String );
	pShapes->Add_Field("GPS X", 		SG_DATATYPE_Double );
	pShapes->Add_Field("GPS Y", 		SG_DATATYPE_Double );
	pShapes->Add_Field("Projected X", 	SG_DATATYPE_Double );
	pShapes->Add_Field("Projected Y", 	SG_DATATYPE_Double );
	pShapes->Add_Field("Altitude", 		SG_DATATYPE_Double );
	pShapes->Add_Field("Direction", 	SG_DATATYPE_Double );
	pShapes->Add_Field("Compass", 		SG_DATATYPE_String );
	pShapes->Add_Field("DOP", 			SG_DATATYPE_Double );


	for( int i=0; i<Files.Get_Count(); i++ )
	{
		CSG_String File = Files.Get_String(i);
		std::unique_ptr<Exiv2::Image> image = Exiv2::ImageFactory::open(File.to_StdString());
		image->readMetadata();
		Exiv2::ExifData &exifData = image->exifData();

        auto latIt 		= exifData.findKey(Exiv2::ExifKey("Exif.GPSInfo.GPSLatitude"));
        auto latRefIt 	= exifData.findKey(Exiv2::ExifKey("Exif.GPSInfo.GPSLatitudeRef"));
        auto lonIt 		= exifData.findKey(Exiv2::ExifKey("Exif.GPSInfo.GPSLongitude"));
        auto lonRefIt 	= exifData.findKey(Exiv2::ExifKey("Exif.GPSInfo.GPSLongitudeRef"));
		auto altIt 		= exifData.findKey(Exiv2::ExifKey("Exif.GPSInfo.GPSAltitude"));
		auto altRefIt 	= exifData.findKey(Exiv2::ExifKey("Exif.GPSInfo.GPSAltitudeRef"));
		//auto timeIt 	= exifData.findKey(Exiv2::ExifKey("Exif.GPSInfo.GPSTimeStamp"));
		auto dopIt 		= exifData.findKey(Exiv2::ExifKey("Exif.GPSInfo.GPSDOP"));
		auto dirIt 		= exifData.findKey(Exiv2::ExifKey("Exif.GPSInfo.GPSImgDirection"));
		auto dirRefIt 	= exifData.findKey(Exiv2::ExifKey("Exif.GPSInfo.GPSImgDirectionRef"));

		if( latIt != exifData.end() && latRefIt != exifData.end() && lonIt != exifData.end() && lonRefIt != exifData.end() )
		{
			CSG_Shape *pShape = pShapes->Add_Shape();
			pShape->Set_Value( 0, SG_File_Get_Name( File, true ) );
			pShape->Set_Value( 1, File );
			
			TSG_Point Point; 
			double X, Y;
			
			CSG_String Coord_Message = "No coordinates found in exif";
			CSG_String Alt_Message = "No altitude", Dir_Message = "No direction", Dop_Message = "No dop";
			if( Get_Coordinate(Y, latIt->value(), latRefIt->value().toString())
			&&	Get_Coordinate(X, lonIt->value(), lonRefIt->value().toString()) )
			{
				Point.x = X; 
				Point.y = Y;
				SG_Get_Projected(GPS, Target, Point);
				
				pShape->Add_Point( Point );
				pShape->Set_Value( 2, X ); pShape->Set_Value( 4, Point.x );
				pShape->Set_Value( 3, Y ); pShape->Set_Value( 5, Point.y );
				Coord_Message = "Found coordinates in exif";
				
				if( altIt != exifData.end() && altRefIt != exifData.end() && altIt->value().count() == 1 )
				{
    				double 	Altitide = Convert_Rational( altIt->value(), 0 );
					if( altRefIt->value().toUint32() == 1 ) Altitide *= -1;
					pShape->Set_Value( 6, Altitide );
					pShape->Set_Z( Altitide );
					Alt_Message = "Found altitude";
				}
				else
				{
					pShape->Set_NoData(6);
				}
				
				if( dirIt != exifData.end() && dirIt->value().count() == 1 && dirRefIt != exifData.end() )
				{
    				double Direction = Convert_Rational( dirIt->value(), 0 );
					pShape->Set_Value( 7, Direction );
					pShape->Set_Value( 8, dirRefIt->value().toString().c_str() );
					Dir_Message = "Found direction";
				}

				if( dopIt != exifData.end() && dopIt->value().count() == 1 )
				{
    				double DOP = Convert_Rational( dopIt->value(), 0 );
					pShape->Set_Value( 9, DOP );
					Dop_Message = "Found dop";
				}
			}
			Message_Fmt("File %s: %s, %s, %s, %s",
			   File.c_str(), Coord_Message.c_str(), Alt_Message.c_str(), 
			   Dir_Message.c_str(), Dop_Message.c_str()
			);
		}
	}

	int Add_Choice = Parameters("ADD")->asInt();
	if( has_GUI() && Add_Choice > 0 )
	{
		CSG_Parameters DataObjectUIParameter;
		DataObject_Get_Parameters(pShapes, DataObjectUIParameter);
		DataObjectUIParameter.Set_Parameter("IMAGE_FIELD", 1 	);
		DataObjectUIParameter.Set_Parameter("IMAGE_SCALE", 30.0 );
		DataObject_Set_Parameters(pShapes, DataObjectUIParameter);
		DataObject_Update(pShapes, 
			Add_Choice == 1 ? SG_UI_DATAOBJECT_SHOW_MAP_NEW : SG_UI_DATAOBJECT_SHOW_MAP_ACTIVE );
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
#endif // HAVE_EXIV2


///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       io_pdal                         //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 pdal_writer_las.cpp                   //
//                                                       //
//                 Copyrights (C) 2021                   //
//                   Volker Wichmann                     //
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
#include "pdal_writer_las.h"

#include <pdal/Options.hpp>
#include <pdal/PointTable.hpp>
#include <pdal/StageFactory.hpp>
#include <pdal/io/BufferReader.hpp>


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPDAL_Writer_Las::CPDAL_Writer_Las(void)
{
    Set_Name	(_TL("Export LAS/LAZ File"));

    Set_Author	("V.Wichmann (c) 2021");

    Add_Reference("https://pdal.io/", SG_T("PDAL Homepage"));
    Add_Reference("https://github.com/ASPRSorg/LAS/", SG_T("ASPRS LAS Specification"));

    CSG_String Description;

    Description = _TW(
        "The tool allows one to export a point cloud as ASPRS LAS (or compressed LAZ) file using the "
        "\"Point Data Abstraction Library\" (PDAL).\n"
        "The file extension of the output file determines whether the file is written compressed (*.laz) "
        "or uncompressed (*.las).\n"
        "The number and type of attributes that can be exported depends on the chosen LAS file version "
        "and point data record format. Please have a look at the ASPRS LAS specification on how these "
        "formats are defined.\n"
    );

	Description += CSG_String::Format("\nPDAL-%s\n", SG_Get_PDAL_Drivers().Get_Version().c_str());

    Set_Description(Description);

    //-----------------------------------------------------
    Parameters.Add_PointCloud("",
        "POINTS"     , _TL("Point Cloud"),
        _TL("The point cloud to export."),
        PARAMETER_INPUT
    );

	Parameters.Add_Table_Field("POINTS", "T"  , _TL("GPS-Time"                        ), _TL(""), true);
	Parameters.Add_Table_Field("POINTS", "r"  , _TL("Number of the Return"            ), _TL(""), true);
    Parameters.Add_Table_Field("POINTS", "n"  , _TL("Number of Returns of Given Pulse"), _TL(""), true);
    Parameters.Add_Table_Field("POINTS", "i"  , _TL("Intensity"                       ), _TL(""), true);
    Parameters.Add_Table_Field("POINTS", "c"  , _TL("Classification"                  ), _TL(""), true);
    Parameters.Add_Table_Field("POINTS", "sCH", _TL("Scanner Channel"                 ), _TL(""), true);
	Parameters.Add_Table_Field("POINTS", "C"  , _TL("RGB Color Value"                 ), _TL(""), true);
    Parameters.Add_Table_Field("POINTS", "B"  , _TL("Blue Channel Color"              ), _TL(""), true);
	Parameters.Add_Table_Field("POINTS", "G"  , _TL("Green Channel Color"             ), _TL(""), true);
	Parameters.Add_Table_Field("POINTS", "R"  , _TL("Red Channel Color"               ), _TL(""), true);
	Parameters.Add_Table_Field("POINTS", "NIR", _TL("Near Infrared"                   ), _TL(""), true);
    Parameters.Add_Table_Field("POINTS", "a"  , _TL("Scan Angle"                      ), _TL(""), true);
    Parameters.Add_Table_Field("POINTS", "d"  , _TL("Direction of Scan Flag"          ), _TL(""), true);
    Parameters.Add_Table_Field("POINTS", "e"  , _TL("Edge of Flight Line Flag"        ), _TL(""), true);
    Parameters.Add_Table_Field("POINTS", "u"  , _TL("User Data"                       ), _TL(""), true);
    Parameters.Add_Table_Field("POINTS", "p"  , _TL("Point Source ID"                 ), _TL(""), true);

    Parameters.Add_FilePath("",
        "FILE"       , _TL("Output File"),
        _TL("The LAS/LAZ output file."),
        CSG_String::Format("%s|%s|%s|%s|%s|%s|%s|%s",
            _TL("All Recognized File Types"), SG_T("*.las;*.LAS;*.laz;*.LAZ"),
            _TL("LAS Files")				, SG_T("*.las;*.LAS"),
            _TL("LAZ Files")				, SG_T("*.laz;*.LAZ"),
            _TL("All Files")				, SG_T("*.*")
        ),
        NULL, true
    );
    
    Parameters.Add_Choice("FILE",
		"FILE_FORMAT", _TL("File Format"),
        _TL("Choose the file format to write. The format determines which attributes can be written and in which data depth."),
        "LAS 1.2|LAS 1.4", 1
    );

    Parameters.Add_Choice("FILE",
        "FORMAT"     , _TL("Point Data Record Format"),
        _TL("Choose the point data record format to write. The format determines which attributes can be written."),
        "0|1|2|3|6|7|8", 3
    );

    Parameters.Add_Choice("",
        "RGB_RANGE"  , _TL("Color Depth"),
        _TL("Color depth of the Red, Green, Blue, NIR values in the input point cloud. 8 bit values will be scaled to 16 bit."),
        CSG_String::Format("%s|%s", _TL("16 bit"), _TL("8 bit")), 1
    );

    Parameters.Add_Double("", "OFF_X"  , _TL("Offset X"), _TL("") , 0.000);
    Parameters.Add_Double("", "OFF_Y"  , _TL("Offset Y"), _TL("") , 0.000);
    Parameters.Add_Double("", "OFF_Z"  , _TL("Offset Z"), _TL("") , 0.000);
    Parameters.Add_Double("", "SCALE_X", _TL("Scale X" ), _TL("") , 0.001);
    Parameters.Add_Double("", "SCALE_Y", _TL("Scale Y" ), _TL("") , 0.001);
    Parameters.Add_Double("", "SCALE_Z", _TL("Scale Z" ), _TL("") , 0.001);
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CPDAL_Writer_Las::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
    if(	pParameter->Cmp_Identifier("POINTS") && pParameter->asPointCloud() )
    {
        CSG_PointCloud *pPoints = pParameter->asPointCloud();

        long digits[3]; // calculate number of digits

		pPoints->Set_Max_Samples(pPoints->Get_Count());

		for(int i=0; i<3; i++)
        {
			digits[i] = abs((long)pPoints->Get_Maximum(i) - (long)pPoints->Get_Minimum(i));
			digits[i] = digits[i] < 10 ? 1 : 1 + (long)log10((double)digits[i]);
			digits[i] = (long)((long)((long)pPoints->Get_Maximum(i) / pow(10., digits[i] + 1)) * pow(10., digits[i] + 1));
		}

        pParameters->Set_Parameter("OFF_X", (int)digits[0]);
        pParameters->Set_Parameter("OFF_Y", (int)digits[1]);
        pParameters->Set_Parameter("OFF_Z", (int)digits[2]);

		if( has_GUI() ) // fetch the attributes
		{
			for(int i=0; i<pParameter->Get_Children_Count(); i++)
			{
				CSG_Parameter *pChild = pParameter->Get_Child(i);

				if( pChild->Get_Type() == PARAMETER_TYPE_Table_Field )
				{
					pChild->Set_Value(pPoints->Get_Field(pChild->Get_Name()));

					if( pChild->Cmp_Identifier("C") ) // rgb color value ?
					{
						On_Parameters_Enable(pParameters, pChild); // en-/disable red, green, blue attributes !
					}
				}
			}
		}
	}
   
    return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CPDAL_Writer_Las::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("FORMAT") )
	{
		int i = pParameter->asInt(); // this is the choices list index, not the point data record format!

		pParameters->Set_Enabled("T"        , i == 1 || i >= 3);
		pParameters->Set_Enabled("R"        , i == 2 || i == 3 || i >= 5);
		pParameters->Set_Enabled("G"        , i == 2 || i == 3 || i >= 5);
		pParameters->Set_Enabled("B"        , i == 2 || i == 3 || i >= 5);
		pParameters->Set_Enabled("C"        , i == 2 || i == 3 || i >= 5);
		pParameters->Set_Enabled("RGB_RANGE", i == 2 || i == 3 || i >= 5);
		pParameters->Set_Enabled("NIR"      , i == 6);
		pParameters->Set_Enabled("sCH"      , i >= 3);
	}

	if(	pParameter->Cmp_Identifier("C") )
	{
		pParameters->Set_Enabled("R", pParameter->asInt() < 0);
		pParameters->Set_Enabled("G", pParameter->asInt() < 0);
		pParameters->Set_Enabled("B", pParameter->asInt() < 0);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPDAL_Writer_Las::On_Execute(void)
{
	CSG_PointCloud *pPoints = Parameters("POINTS")->asPointCloud();

	if( pPoints->Get_Count() < 1 )
	{
		Error_Set(_TL("input point cloud is empty"));

		return( false );
	}

	//---------------------------------------------------------
	CSG_String File = Parameters("FILE")->asString();

	if( File.is_Empty() )
	{
		Error_Set(_TL("no output file provided"));

		return( false );
	}

	//---------------------------------------------------------
	int i_T   = Parameters("T"  )->asInt();
	int i_i   = Parameters("i"  )->asInt();
	int i_a   = Parameters("a"  )->asInt();
	int i_r   = Parameters("r"  )->asInt();
	int i_n   = Parameters("n"  )->asInt();
	int i_c   = Parameters("c"  )->asInt();
	int i_sCH = Parameters("sCH")->asInt();
	int i_u   = Parameters("u"  )->asInt();
	int i_C   = Parameters("C"  )->asInt();
	int i_R   = Parameters("R"  )->asInt();
	int i_G   = Parameters("G"  )->asInt();
	int i_B   = Parameters("B"  )->asInt();
	int i_NIR = Parameters("NIR")->asInt();
	int i_e   = Parameters("e"  )->asInt();
	int i_d   = Parameters("d"  )->asInt();
	int i_p   = Parameters("p"  )->asInt();
    
	int RGBrange = Parameters("RGB_RANGE")->asInt();

	if( i_C >= 0 )
	{
		i_R = i_G = i_B = -1;
	}

	if( (i_R >= 0 || i_G >= 0 || i_B >= 0) && (i_R < 0 || i_G < 0 || i_B < 0) )
	{
		Error_Set(_TL("incomplete setting of red, green, blue channel fields"));

		return( false );
	}

	//---------------------------------------------------------
	int Format, FileFormat = Parameters("FILE_FORMAT")->asInt() == 0 ? 2 : 4;

	switch( Parameters("FORMAT")->asInt() )
	{
	case  0: Format = 0; break;
	case  1: Format = 1; break;
	case  2: Format = 2; break;
	default: Format = 3; break;
	case  4: Format = 6; break;
	case  5: Format = 7; break;
	case  6: Format = 8; break;
	}

	if( FileFormat == 0 && Format > 3 )
	{
		Error_Set(CSG_String::Format(_TL("point data record format %s requires to write file format 1.4"), Parameters("FORMAT")->asString()));

		return( false );
	}

	//---------------------------------------------------------
	pdal::Options Options;
   
	Options.add(pdal::Option("filename"     , File.b_str()));
	Options.add(pdal::Option("minor_version", FileFormat));
	Options.add(pdal::Option("dataformat_id", Format));
	Options.add(pdal::Option("software_id"  , "SAGA-GIS"));
	Options.add(pdal::Option("system_id"    , "PDAL"));
	Options.add(pdal::Option("offset_x"     , Parameters("OFF_X")->asDouble()));
	Options.add(pdal::Option("offset_y"     , Parameters("OFF_Y")->asDouble()));
	Options.add(pdal::Option("offset_z"     , Parameters("OFF_Z")->asDouble()));
	Options.add(pdal::Option("scale_x"      , Parameters("SCALE_X")->asDouble()));
	Options.add(pdal::Option("scale_y"      , Parameters("SCALE_Y")->asDouble()));
	Options.add(pdal::Option("scale_z"      , Parameters("SCALE_Z")->asDouble()));

	if( pPoints->Get_Projection().is_Okay() )
	{
		Options.add(pdal::Option("a_srs", std::string(pPoints->Get_Projection().Get_WKT().b_str())));
	}

	if( SG_File_Cmp_Extension(File, "laz") )
	{
		Options.add(pdal::Option("compression", "laszip"));
	}

	//---------------------------------------------------------
	pdal::PointTable Table;

	Table.layout()->registerDim(pdal::Dimension::Id::X);
	Table.layout()->registerDim(pdal::Dimension::Id::Y);
	Table.layout()->registerDim(pdal::Dimension::Id::Z);

	if( i_T   >= 0 ) { Table.layout()->registerDim(pdal::Dimension::Id::GpsTime          ); }
	if( i_i   >= 0 ) { Table.layout()->registerDim(pdal::Dimension::Id::Intensity        ); }
	if( i_r   >= 0 ) { Table.layout()->registerDim(pdal::Dimension::Id::ReturnNumber     ); }
	if( i_n   >= 0 ) { Table.layout()->registerDim(pdal::Dimension::Id::NumberOfReturns  ); }
	if( i_c   >= 0 ) { Table.layout()->registerDim(pdal::Dimension::Id::Classification   ); }
	if( i_a   >= 0 ) { Table.layout()->registerDim(pdal::Dimension::Id::ScanAngleRank    ); }
	if( i_u   >= 0 ) { Table.layout()->registerDim(pdal::Dimension::Id::UserData         ); }
	if( i_e   >= 0 ) { Table.layout()->registerDim(pdal::Dimension::Id::EdgeOfFlightLine ); }
	if( i_d   >= 0 ) { Table.layout()->registerDim(pdal::Dimension::Id::ScanDirectionFlag); }
	if( i_p   >= 0 ) { Table.layout()->registerDim(pdal::Dimension::Id::PointSourceId    ); }
	if( i_sCH >= 0 ) { Table.layout()->registerDim(pdal::Dimension::Id::ScanChannel      ); }
	if( i_NIR >= 0 ) { Table.layout()->registerDim(pdal::Dimension::Id::Infrared         ); }
	if( i_C   >= 0 || i_R >= 0 )
	{   
		Table.layout()->registerDim(pdal::Dimension::Id::Red  );
		Table.layout()->registerDim(pdal::Dimension::Id::Green);
		Table.layout()->registerDim(pdal::Dimension::Id::Blue );
	}

	pdal::PointViewPtr View(new pdal::PointView(Table));

	//---------------------------------------------------------
	for(sLong i=0; i<pPoints->Get_Count(); i++ )
	{
		if( i % 100000 == 0 )
		{
			SG_UI_Process_Set_Progress(i, pPoints->Get_Count() * 2);
		}

		pPoints->Set_Cursor(i);

		View->setField(pdal::Dimension::Id::X, i, pPoints->Get_X());
		View->setField(pdal::Dimension::Id::Y, i, pPoints->Get_Y());
		View->setField(pdal::Dimension::Id::Z, i, pPoints->Get_Z());

		if( i_T   >= 0 ) { View->setField(pdal::Dimension::Id::GpsTime          , i, (double            )pPoints->Get_Value(i_T  )             ); }
		if( i_i   >= 0 ) { View->setField(pdal::Dimension::Id::Intensity        , i, (unsigned short int)pPoints->Get_Value(i_i  )             ); }
		if( i_r   >= 0 ) { View->setField(pdal::Dimension::Id::ReturnNumber     , i, (unsigned char     )pPoints->Get_Value(i_r  )             ); }
		if( i_n   >= 0 ) { View->setField(pdal::Dimension::Id::NumberOfReturns  , i, (unsigned char     )pPoints->Get_Value(i_n  )             ); }
		if( i_c   >= 0 ) { View->setField(pdal::Dimension::Id::Classification   , i, (unsigned char     )pPoints->Get_Value(i_c  )             ); }
		if( i_a   >= 0 ) { View->setField(pdal::Dimension::Id::ScanAngleRank    , i, (short int         )pPoints->Get_Value(i_a  )             ); }
		if( i_u   >= 0 ) { View->setField(pdal::Dimension::Id::UserData         , i, (unsigned char     )pPoints->Get_Value(i_u  )             ); }
		if( i_e   >= 0 ) { View->setField(pdal::Dimension::Id::EdgeOfFlightLine , i,                     pPoints->Get_Value(i_e  ) < 1. ? 0 : 1); }
		if( i_d   >= 0 ) { View->setField(pdal::Dimension::Id::ScanDirectionFlag, i,                     pPoints->Get_Value(i_d  ) < 1. ? 0 : 1); }
		if( i_p   >= 0 ) { View->setField(pdal::Dimension::Id::PointSourceId    , i, (unsigned short int)pPoints->Get_Value(i_p  )             ); }
		if( i_sCH >= 0 ) { View->setField(pdal::Dimension::Id::ScanChannel      , i, (unsigned char     )pPoints->Get_Value(i_sCH)             ); }
		if( i_NIR >= 0 )
		{   
			double nir = pPoints->Get_Value(i_NIR);

			if( RGBrange == 1 ) // 8bit
			{
				nir = nir * 65535 / 255.;
			}
            
			View->setField(pdal::Dimension::Id::Infrared, i, (unsigned short int)nir);
		}

		if( i_C >= 0 || i_R >= 0 )
		{
			double r = i_C < 0 ? pPoints->Get_Value(i_R) : SG_GET_R((int)pPoints->Get_Value(i_C));
			double g = i_C < 0 ? pPoints->Get_Value(i_G) : SG_GET_G((int)pPoints->Get_Value(i_C));
			double b = i_C < 0 ? pPoints->Get_Value(i_B) : SG_GET_B((int)pPoints->Get_Value(i_C));

			if( i_C >= 0 || RGBrange == 1 ) // 8bit
			{
				r *= 65535 / 255.; g *= 65535 / 255.; b *= 65535 / 255.;
			}

			View->setField(pdal::Dimension::Id::Red  , i, (unsigned short int)r);
			View->setField(pdal::Dimension::Id::Green, i, (unsigned short int)g);
			View->setField(pdal::Dimension::Id::Blue , i, (unsigned short int)b);
		}
	}

	//---------------------------------------------------------
	pdal::BufferReader Reader; Reader.addView(View);

	pdal::StageFactory Factory; pdal::Stage *Writer = Factory.createStage("writers.las");

	Writer->setInput  (Reader);
	Writer->setOptions(Options);
	Writer->prepare   (Table);
	Writer->execute   (Table);

	//---------------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

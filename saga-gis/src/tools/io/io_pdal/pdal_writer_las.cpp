
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


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPDAL_Writer_Las::CPDAL_Writer_Las(void)
{
    Set_Name    (_TL("Export LAS/LAZ File"));

    Set_Author    ("V.Wichmann (c) 2021");

    Add_Reference("https://pdal.io/", SG_T("PDAL Homepage"));

    CSG_String  Description;

    Description = _TW(
        "The tool allows one to export a point cloud as ASPRS LAS (or compressed LAZ) file using the "
        "\"Point Data Abstraction Library\" (PDAL).\n"
        "The file extension of the output file determines whether the file is written compressed (*.laz) "
        "or uncompressed (*.las).\n"
    );

    Description += CSG_String::Format("\n\nPDAL %s:%s\n\n", _TL("Version"), SG_Get_PDAL_Drivers().Get_Version().c_str());

    Set_Description(Description);


    //-----------------------------------------------------
    CSG_Parameter *pNode = Parameters.Add_PointCloud("",
        "POINTS"		, _TL("Point Cloud"),
        _TL("The point cloud to export."),
        PARAMETER_INPUT
    );
    Parameters.Add_Table_Field(pNode,
        "T"    , _TL("gps-time"),
        _TL(""), true
    );
    Parameters.Add_Table_Field(pNode,
        "r"    , _TL("number of the return"),
        _TL(""), true
    );
    Parameters.Add_Table_Field(pNode,
        "n"    , _TL("number of returns of given pulse"),
        _TL(""), true
    );
    Parameters.Add_Table_Field(pNode,
        "i"    , _TL("intensity"),
        _TL(""), true
    );
    Parameters.Add_Table_Field(pNode,
        "c"    , _TL("classification"),
        _TL(""), true
    );
    Parameters.Add_Table_Field(pNode,
        "sCH"  , _TL("scanner channel"),
        _TL(""), true
    );
    Parameters.Add_Table_Field(pNode,
        "R"    , _TL("red channel color"),
        _TL(""), true
    );
    Parameters.Add_Table_Field(pNode,
        "G"    , _TL("green channel color"),
        _TL(""), true
    );
    Parameters.Add_Table_Field(pNode,
        "B"    , _TL("blue channel color"),
        _TL(""), true
    );
    Parameters.Add_Table_Field(pNode,
        "C"    , _TL("SAGA RGB color"),
        _TL(""), true
    );
    Parameters.Add_Table_Field(pNode,
        "NIR"  , _TL("near infrared"),
        _TL(""), true
    );
    Parameters.Add_Table_Field(pNode,
        "a"    , _TL("scan angle"),
        _TL(""), true
    );
    Parameters.Add_Table_Field(pNode,
        "d"    , _TL("direction of scan flag"),
        _TL(""), true
    );
    Parameters.Add_Table_Field(pNode,
        "e"    , _TL("edge of flight line flag"),
        _TL(""), true
    );
    Parameters.Add_Table_Field(pNode,
        "u"    , _TL("user data"),
        _TL(""), true
    );
    Parameters.Add_Table_Field(pNode,
        "p"    , _TL("point source ID"),
        _TL(""), true
    );

    Parameters.Add_FilePath("",
        "FILE"		, _TL("Output File"),
        _TL("The LAS/LAZ output file."),
        CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|%s|%s"),
            _TL("All Recognized File Types"), SG_T("*.las;*.LAS;*.laz;*.LAZ"),
            _TL("LAS Files")				, SG_T("*.las;*.LAS"),
            _TL("LAZ Files")				, SG_T("*.laz;*.LAZ"),
            _TL("All Files")				, SG_T("*.*")
        ),
        NULL, true
    );
    
    Parameters.Add_Choice(
        Parameters("FILE"), "FILE_FORMAT", _TL("File Format"),
        _TL("Choose the file format to write. The format determines which attributes can be written and in which data depth."),
        CSG_String::Format(SG_T("%s|%s"),
            _TL("LAS 1.2"),
            _TL("LAS 1.4")
        ), 1
    );

    Parameters.Add_Choice(Parameters("FILE"),
        "FORMAT", _TL("Point Data Record Format"),
        _TL("Choose the point data record format to write. The format determines which attributes can be written."),
        CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|%s"),
            _TL("0"),
            _TL("1"),
            _TL("2"),
            _TL("3"),
            _TL("6"),
            _TL("7"),
            _TL("8")
        ), 4
    );

    Parameters.Add_Choice("",
        "RGB_RANGE", _TL("Input R,G,B (and NIR) Value Range"),
        _TL("Range of the R,G,B (and NIR) values in the input point cloud. 8 bit values will be scaled to 16 bit."),
        CSG_String::Format(SG_T("%s|%s|"),
            _TL("16 bit"),
            _TL("8 bit")
        ), 1
    );

    Parameters.Add_Double("",
        "OFF_X"	, _TL("Offset X"),
        _TL("") , 0.0
    );
    Parameters.Add_Double("",
        "OFF_Y"	, _TL("Offset Y"),
        _TL("") , 0.0
    );
    Parameters.Add_Double("",
        "OFF_Z"	, _TL("Offset Z"),
        _TL("") , 0.0
    );
    Parameters.Add_Double("",
        "SCALE_X"	, _TL("Scale X"),
        _TL("") , 0.001
    );
    Parameters.Add_Double("",
        "SCALE_Y"	, _TL("Scale Y"),
        _TL("") , 0.001
    );
    Parameters.Add_Double("",
        "SCALE_Z"	, _TL("Scale Z"),
        _TL("") , 0.001
    );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CPDAL_Writer_Las::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
    if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("FILE_FORMAT")) )
    {
        int i = pParameters->Get_Parameter("FILE_FORMAT")->asInt(); // 0 = 1.2, 1 = 1.4

        pParameters->Get_Parameter("sCH"        )->Set_Enabled(i == 1);
    }

    if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("FORMAT")) )
    {
        int	i = pParameters->Get_Parameter("FORMAT")->asInt();    // this is the choices list index, not the point data record format!

        pParameters->Get_Parameter("T"			)->Set_Enabled(i == 1 || i >= 3);
        pParameters->Get_Parameter("R"			)->Set_Enabled(i == 2 || i == 3 || i >= 5);
        pParameters->Get_Parameter("G"			)->Set_Enabled(i == 2 || i == 3 || i >= 5);
        pParameters->Get_Parameter("B"			)->Set_Enabled(i == 2 || i == 3 || i >= 5);
        pParameters->Get_Parameter("C"			)->Set_Enabled(i == 2 || i == 3 || i >= 5);
        pParameters->Get_Parameter("RGB_RANGE"	)->Set_Enabled(i == 2 || i == 3 || i >= 5);
        pParameters->Get_Parameter("NIR"		)->Set_Enabled(i == 6);
    }

    //-----------------------------------------------------
    return( 1 );
}


//---------------------------------------------------------
int CPDAL_Writer_Las::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
    if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("POINTS")) )
    {
        CSG_PointCloud	*pPoints = pParameters->Get_Parameter("POINTS")->asPointCloud();

        if( pPoints != NULL)
        {
            pPoints->Set_Max_Samples(pPoints->Get_Count());

            long iXYZ[3];	// calculate number of digits

            for (int i=0; i<3; i++)
            {
                iXYZ[i] = (long)pPoints->Get_Maximum(i) - (long)pPoints->Get_Minimum(i);
                iXYZ[i] = abs(iXYZ[i]);
                if (iXYZ[i] < 10)
                    iXYZ[i] = 1;
                else
                    iXYZ[i] = 1 + (long)log10((double)iXYZ[i]);
            }

            for (int i=0; i<3; i++)
            {
                iXYZ[i] = (long)((long)((long)pPoints->Get_Maximum(i) / pow(10.0, iXYZ[i] + 1)) * pow(10.0, iXYZ[i] + 1));
            }

            pParameters->Get_Parameter("OFF_X")->Set_Value((double)iXYZ[0]);
            pParameters->Get_Parameter("OFF_Y")->Set_Value((double)iXYZ[1]);
            pParameters->Get_Parameter("OFF_Z")->Set_Value((double)iXYZ[2]);
        }
    }

    if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("FILE_FORMAT")) )
    {
        if (pParameters->Get_Parameter("FILE_FORMAT")->asInt() == 0)
        {
            pParameters->Get_Parameter("FORMAT")->Set_Value(3);     // LAS 1.2 default: point data record format 3
        }
        else // == 1
        {
            pParameters->Get_Parameter("FORMAT")->Set_Value(4);     // LAS 1.4 default: point data record format 6
        }
    }
    
    //-----------------------------------------------------
    return (true);
}



///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPDAL_Writer_Las::On_Execute(void)
{
    CSG_PointCloud  *pPoints	= Parameters("POINTS")->asPointCloud();
    int             i_T			= Parameters("T")->asInt();
    int             i_i			= Parameters("i")->asInt();
    int             i_a			= Parameters("a")->asInt();
    int             i_r			= Parameters("r")->asInt();
    int             i_n			= Parameters("n")->asInt();
    int             i_c			= Parameters("c")->asInt();
    int             i_sCH       = Parameters("sCH")->asInt();
    int             i_u			= Parameters("u")->asInt();
    int             i_R			= Parameters("R")->asInt();
    int             i_G			= Parameters("G")->asInt();
    int             i_B			= Parameters("B")->asInt();
    int             i_NIR   	= Parameters("NIR")->asInt();
    int             i_e			= Parameters("e")->asInt();
    int             i_d			= Parameters("d")->asInt();
    int             i_p			= Parameters("p")->asInt();
    int             i_C			= Parameters("C")->asInt();
    
    CSG_String      File        = Parameters("FILE")->asString();
    CSG_String      FileExt     = File.AfterLast('.');
    int             RGBrange	= Parameters("RGB_RANGE")->asInt();


    //---------------------------------------------------------
    if( pPoints->Get_Count() < 1 )
    {
        Error_Set(_TL("input point cloud is empty"));
        return( false );
    }

    if( File.Length() < 1 )
    {
        Error_Set(_TL("no output file provided"));
        return( false );
    }

    if( (i_R > -1 && (i_G < 0 || i_B < 0)) || (i_G > -1 && (i_R < 0 || i_B < 0)) || (i_B > -1 &&  (i_R < 0 || i_G < 0)) )
    {
        Error_Set(_TL("incomplete setting of R,G,B fields"));
        return( false );
    }

    if (i_C > -1 && i_R > -1)
    {
        Error_Set(_TL("use either SAGA RGB or individual R,G,B components for export"));
        return( false );
    }


    //---------------------------------------------------------
    int FileFormat;
    
    if (Parameters("FILE_FORMAT")->asInt() == 0)
    {
        FileFormat = 2;
    }
    else
    {
        FileFormat = 4;
    }

    //---------------------------------------------------------
    int Format;

    switch (Parameters("FORMAT")->asInt())
    {
    case 0:		Format = 0;     break;
    case 1:		Format = 1;     break;
    case 2:		Format = 2;     break;
    case 3:	    
    default:    Format = 3;     break;
    case 4:     Format = 6;     break;
    case 5:	    Format = 7;     break;
    case 6:	    Format = 8;     break;
    }

    if( FileFormat == 0 && Format > 3 )
    {
        Error_Set(CSG_String::Format(_TL("point data record format %s requires to write file format 1.4"), Parameters("FORMAT")->asString()));
        return (false);
    }


    //---------------------------------------------------------
    pdal::Options   Options;
   
    Options.add(pdal::Option("filename"         , File.b_str()));
    Options.add(pdal::Option("minor_version"    , FileFormat));
    Options.add(pdal::Option("dataformat_id"    , Format));
    Options.add(pdal::Option("software_id"      , "SAGA-GIS"));
    Options.add(pdal::Option("system_id"        , "PDAL"));
    Options.add(pdal::Option("offset_x"         , Parameters("OFF_X")->asDouble()));
    Options.add(pdal::Option("offset_y"         , Parameters("OFF_Y")->asDouble()));
    Options.add(pdal::Option("offset_z"         , Parameters("OFF_Z")->asDouble()));
    Options.add(pdal::Option("scale_x"          , Parameters("SCALE_X")->asDouble()));
    Options.add(pdal::Option("scale_y"          , Parameters("SCALE_Y")->asDouble()));
    Options.add(pdal::Option("scale_z"          , Parameters("SCALE_Z")->asDouble()));

    if( pPoints->Get_Projection().is_Okay() )
    {
        Options.add(pdal::Option("a_srs"        , std::string(pPoints->Get_Projection().Get_WKT().b_str())));
    }
    
    if( FileExt.is_Same_As("laz", false) )
    {
        Options.add(pdal::Option("compression"  , "laszip"));
    }


    //---------------------------------------------------------
    pdal::PointTable    Table;
    Table.layout()->registerDim(pdal::Dimension::Id::X);
    Table.layout()->registerDim(pdal::Dimension::Id::Y);
    Table.layout()->registerDim(pdal::Dimension::Id::Z);

    if( i_T > -1 )      {   Table.layout()->registerDim(pdal::Dimension::Id::GpsTime);              }
    if( i_i > -1 )      {   Table.layout()->registerDim(pdal::Dimension::Id::Intensity);            }
    if( i_r > -1 )      {   Table.layout()->registerDim(pdal::Dimension::Id::ReturnNumber);         }
    if( i_n > -1 )      {   Table.layout()->registerDim(pdal::Dimension::Id::NumberOfReturns);      }
    if( i_c > -1 )      {   Table.layout()->registerDim(pdal::Dimension::Id::Classification);       }
    if( i_a > -1 )      {   Table.layout()->registerDim(pdal::Dimension::Id::ScanAngleRank);        }
    if( i_u > -1 )      {   Table.layout()->registerDim(pdal::Dimension::Id::UserData);             }
    if( i_e > -1 )      {   Table.layout()->registerDim(pdal::Dimension::Id::EdgeOfFlightLine);     }
    if( i_d > -1 )      {   Table.layout()->registerDim(pdal::Dimension::Id::ScanDirectionFlag);    }
    if( i_p > -1 )      {   Table.layout()->registerDim(pdal::Dimension::Id::PointSourceId);        }
    if( i_sCH > -1 )    {   Table.layout()->registerDim(pdal::Dimension::Id::ScanChannel);          }
    if( i_NIR > -1 )    {   Table.layout()->registerDim(pdal::Dimension::Id::Infrared);             }
    if( i_C > -1 || i_R > -1 )
    {   
        Table.layout()->registerDim(pdal::Dimension::Id::Red);
        Table.layout()->registerDim(pdal::Dimension::Id::Green);
        Table.layout()->registerDim(pdal::Dimension::Id::Blue);
    }

    pdal::PointViewPtr  View(new pdal::PointView(Table));


    //---------------------------------------------------------
    for( int i=0; i<pPoints->Get_Count(); i++ )
    {
        if (i % 100000 == 0)
            SG_UI_Process_Set_Progress(i, pPoints->Get_Count() * 2.0);

        View->setField(pdal::Dimension::Id::X, i, pPoints->Get_X(i));
        View->setField(pdal::Dimension::Id::Y, i, pPoints->Get_Y(i));
        View->setField(pdal::Dimension::Id::Z, i, pPoints->Get_Z(i));

        if( i_T > -1 )      {   View->setField(pdal::Dimension::Id::GpsTime             , i, (double)pPoints->Get_Value(i, i_T));               }
        if( i_i > -1 )      {   View->setField(pdal::Dimension::Id::Intensity           , i, (unsigned short int)pPoints->Get_Value(i, i_i));   }
        if( i_r > -1 )      {   View->setField(pdal::Dimension::Id::ReturnNumber        , i, (unsigned char)pPoints->Get_Value(i, i_r));        }
        if( i_n > -1 )      {   View->setField(pdal::Dimension::Id::NumberOfReturns     , i, (unsigned char)pPoints->Get_Value(i, i_n));        }
        if( i_c > -1 )      {   View->setField(pdal::Dimension::Id::Classification      , i, (unsigned char)pPoints->Get_Value(i, i_c));        }
        if( i_a > -1 )      {   View->setField(pdal::Dimension::Id::ScanAngleRank       , i, (short int)pPoints->Get_Value(i, i_a));            }
        if( i_u > -1 )      {   View->setField(pdal::Dimension::Id::UserData            , i, (unsigned char)pPoints->Get_Value(i, i_u));        }
        if( i_e > -1 )      {   View->setField(pdal::Dimension::Id::EdgeOfFlightLine    , i, pPoints->Get_Value(i, i_e) >= 1.0 ? 1 : 0);        }
        if( i_d > -1 )      {   View->setField(pdal::Dimension::Id::ScanDirectionFlag   , i, pPoints->Get_Value(i, i_d) >= 1.0 ? 1 : 0);        }
        if( i_p > -1 )      {   View->setField(pdal::Dimension::Id::PointSourceId       , i, (unsigned short int)pPoints->Get_Value(i, i_p));   }
        if( i_sCH > -1 )    {   View->setField(pdal::Dimension::Id::ScanChannel         , i, (unsigned char)pPoints->Get_Value(i, i_sCH));      }
        if( i_NIR > -1 )
        {   
            double nir	= pPoints->Get_Value(i, i_NIR);

            if (RGBrange == 1)	// 8bit
            {
                nir	= nir * 65535 / 255.0;
            }
            
            View->setField(pdal::Dimension::Id::Infrared, i, (unsigned short int)nir);
        }

        if( i_C > -1 || i_R > -1 )
        {
            double r, g, b;

            if( i_C > -1 )
            {
                r	= SG_GET_R((int)pPoints->Get_Value(i, i_C));
                g	= SG_GET_G((int)pPoints->Get_Value(i, i_C));
                b	= SG_GET_B((int)pPoints->Get_Value(i, i_C));
            }
            else
            {
                r	= pPoints->Get_Value(i, i_R);
                g	= pPoints->Get_Value(i, i_G);
                b	= pPoints->Get_Value(i, i_B);
            }

            if( i_C > -1 || RGBrange == 1)	// 8bit
            {
                r	= r * 65535 / 255.0;
                g	= g * 65535 / 255.0;
                b	= b * 65535 / 255.0;
            }

            View->setField(pdal::Dimension::Id::Red     , i, (unsigned short int)r);
            View->setField(pdal::Dimension::Id::Green   , i, (unsigned short int)g);
            View->setField(pdal::Dimension::Id::Blue    , i, (unsigned short int)b);
        }
    }


    //---------------------------------------------------------
    pdal::BufferReader  Reader;     Reader.addView(View);

    pdal::StageFactory  Factory;
    pdal::Stage         *Writer     = Factory.createStage("writers.las");

    Writer->setInput(Reader);
    Writer->setOptions(Options);
    Writer->prepare(Table);

    Writer->execute(Table);


    //---------------------------------------------------------
    return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

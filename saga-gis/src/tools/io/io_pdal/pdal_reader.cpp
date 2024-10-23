
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
//                   pdal_reader.cpp                     //
//                                                       //
//               Copyrights (C) 2020-2021                //
//                     Olaf Conrad                       //
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
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "pdal_reader.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
struct SLAS_Attributes
{
    CSG_String  ID, Name, Field;  TSG_Data_Type Type; pdal::Dimension::Id PDAL_ID;
};

//---------------------------------------------------------
const struct SLAS_Attributes  g_Attributes[]  =
{
    { "VAR_TIME"          , _TL("GPS-Time"                        ), _TL("Time"          ), SG_DATATYPE_Double, pdal::Dimension::Id::GpsTime           },
    { "VAR_INTENSITY"     , _TL("Intensity"                       ), _TL("Intensity"     ), SG_DATATYPE_Float , pdal::Dimension::Id::Intensity         },
    { "VAR_SCANANGLE"     , _TL("Scan Angle"                      ), _TL("Scan Angle"    ), SG_DATATYPE_Float , pdal::Dimension::Id::ScanAngleRank     },
    { "VAR_RETURN"        , _TL("Number of the Return"            ), _TL("Return"        ), SG_DATATYPE_Int   , pdal::Dimension::Id::ReturnNumber      },
    { "VAR_RETURNS"       , _TL("Number of Returns of Given Pulse"), _TL("Returns"       ), SG_DATATYPE_Int   , pdal::Dimension::Id::NumberOfReturns   },
    { "VAR_CLASSIFICATION", _TL("Classification"                  ), _TL("Classification"), SG_DATATYPE_Int   , pdal::Dimension::Id::Classification    },
    { "VAR_USERDATA"      , _TL("User Data"                       ), _TL("User Data"     ), SG_DATATYPE_Double, pdal::Dimension::Id::UserData          },
    { "VAR_EDGE"          , _TL("Edge of Flight Line Flag"        ), _TL("Edge Flag"     ), SG_DATATYPE_Char  , pdal::Dimension::Id::EdgeOfFlightLine  },
    { "VAR_DIRECTION"     , _TL("Direction of Scan Flag"          ), _TL("Direction Flag"), SG_DATATYPE_Char  , pdal::Dimension::Id::ScanDirectionFlag },
    { "VAR_SOURCEID"      , _TL("Point Source ID"                 ), _TL("Source ID"     ), SG_DATATYPE_Int   , pdal::Dimension::Id::PointSourceId     },
    { "VAR_COLOR_RED"     , _TL("Red Channel Color"               ), _TL("Red"           ), SG_DATATYPE_Int   , pdal::Dimension::Id::Red               },
    { "VAR_COLOR_GREEN"   , _TL("Green Channel Color"             ), _TL("Green"         ), SG_DATATYPE_Int   , pdal::Dimension::Id::Green             },
    { "VAR_COLOR_BLUE"    , _TL("Blue Channel Color"              ), _TL("Blue"          ), SG_DATATYPE_Int   , pdal::Dimension::Id::Blue              },
    { "" , "" , "", SG_DATATYPE_Undefined, pdal::Dimension::Id::Unknown }
};


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPDAL_Reader::CPDAL_Reader(void)
{
    Set_Name	(_TL("Import Point Cloud"));

    Set_Author	("O.Conrad, V.Wichmann (c) 2020-2021");

    Add_Reference("https://pdal.io/", SG_T("PDAL Homepage"));
    Add_Reference("https://github.com/ASPRSorg/LAS/", SG_T("ASPRS LAS Specification"));

    CSG_String  Description, Filter, Filter_All;

    Description = _TW(
        "The tool allows one to import point cloud data from various file formats using the "
        "\"Point Data Abstraction Library\" (PDAL).\n"
        "By default, all supported attributes will be imported. Note that the list of attributes "
        "supported by the tool is currently based on the attributes defined in the ASPRS LAS specification.\n\n"
    );

    Description += CSG_String::Format("\n\nPDAL %s:%s\n\n", _TL("Version"), SG_Get_PDAL_Drivers().Get_Version().c_str());

    Description += _TL("The following point cloud formats are currently supported:\n\n");

    Description += CSG_String::Format("\n<table border=\"1\"><tr><th>%s</th><th>%s</th><th>%s</th></tr>",
        _TL("Name"), _TL("Extension"), _TL("Description")
    );

    for(int i=0; i<SG_Get_PDAL_Drivers().Get_Count(); i++)
    {
        if( SG_Get_PDAL_Drivers().is_Reader(i) )
        {
            CSG_String Name(SG_Get_PDAL_Drivers().Get_Driver_Name(i));

            bool bSkipDriver = false;

            for(int j=0; !bSkipDriver && !g_Non_Working_Drivers[j].Name.is_Empty(); j++)
            {
                if( Name.Cmp(g_Non_Working_Drivers[j].Name) == 0 )
                {
                    bSkipDriver = true;
                }
            }

            if( bSkipDriver )
            {
                continue;
            }

            CSG_String  Desc = SG_Get_PDAL_Drivers().Get_Driver_Description(i);
            CSG_Strings Exts = SG_Get_PDAL_Drivers().Get_Driver_Extensions (i);

            CSG_String  Ext;

            for(int j=0; j<Exts.Get_Count(); j++)
            {
                Ext += Exts.Get_String(j) + ";";
            }

            Description += "<tr><td>" + Name + "</td><td>" + Ext.BeforeLast(';') + "</td><td>" + Desc + "</td></tr>";

            for(int j=0; j<Exts.Get_Count(); j++)
            {
                Ext = Exts.Get_String(j);

                if( !Ext.is_Empty() )
                {
                    Filter      += "*." + Ext + "|*." + Ext + "|";
                    Filter_All  += (Filter_All.is_Empty() ? "*." : ";*.") + Ext;
                }
            }
        }
    }

    Description += "</table>";

    Set_Description(Description);

    Filter.Prepend(CSG_String::Format("%s|%s|" , _TL("All Recognized Files"), Filter_All.c_str()));
    Filter.Append (CSG_String::Format("%s|*.*" , _TL("All Files")));


    //-----------------------------------------------------
    Parameters.Add_FilePath("",
        "FILES"        , _TL("Files"),
        _TL(""),
        Filter, NULL, false, false, true
    );

    Parameters.Add_Bool("",
        "VARS"         , _TL("Import All Attributes"),
        _TL("Check this to import all supported attributes, or select the attributes you want to become imported individually."),
        true
    );

    for(int i=0; !g_Attributes[i].ID.is_Empty(); i++)
    {
        Parameters.Add_Bool("VARS", g_Attributes[i].ID, g_Attributes[i].Name, _TL(""));
    }

    Parameters.Add_Bool("VARS",
        "VAR_COLOR"      , _TL("RGB-Coded Color"),
        _TL("Import R,G,B values as SAGA RGB-coded value."),
        false
    );

    Parameters.Add_Choice("",
        "RGB_RANGE"      , _TL("RGB Value Range"),
        _TL("The color depth of red, green, blue (and NIR) values in the LAS file."),
        CSG_String::Format("%s|%s",
            _TL( "8 bit"),
            _TL("16 bit")
        ), 1
    );

    Parameters.Add_PointCloud_List("",
        "POINTS"       , _TL("Points"),
        _TL(""),
        PARAMETER_OUTPUT
    );

	//-----------------------------------------------------
	Parameters.Add_String("",
		"CLASSES"      , _TL("Classes"),
		_TL("If classification is available only points are loaded whose identifiers are listed (comma separated, e.g. \"2, 3, 4, 5, 6\"). Ignored if empty."),
		""
	);

	//-----------------------------------------------------
	Parameters.Add_Choice("",
		"EXTENT"       , _TL("Extent"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("original"),
			_TL("user defined"),
			_TL("grid system"),
			_TL("shapes extent")
		), 0
	);

	Parameters.Add_Double("EXTENT", "EXTENT_XMIN", _TL("Left"  ), _TL(""));
	Parameters.Add_Double("EXTENT", "EXTENT_XMAX", _TL("Right" ), _TL(""));
	Parameters.Add_Double("EXTENT", "EXTENT_YMIN", _TL("Bottom"), _TL(""));
	Parameters.Add_Double("EXTENT", "EXTENT_YMAX", _TL("Top"   ), _TL(""));

	Parameters.Add_Grid_System("EXTENT",
		"EXTENT_GRID"  , _TL("Grid System"),
		_TL("")
	);

	Parameters.Add_Shapes("EXTENT",
		"EXTENT_SHAPES", _TL("Shapes Extent"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Double("EXTENT",
		"EXTENT_BUFFER", _TL("Buffer"),
		_TL(""),
		0., 0., true
	);
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CPDAL_Reader::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
    if( pParameter->Cmp_Identifier("VARS") )
    {
        pParameter->Set_Children_Enabled(pParameter->asBool() == false);
    }

	if(	pParameter->Cmp_Identifier("EXTENT") )
	{
		pParameters->Set_Enabled("EXTENT_XMIN"  , pParameter->asInt() == 1);
		pParameters->Set_Enabled("EXTENT_XMAX"  , pParameter->asInt() == 1);
		pParameters->Set_Enabled("EXTENT_YMIN"  , pParameter->asInt() == 1);
		pParameters->Set_Enabled("EXTENT_YMAX"  , pParameter->asInt() == 1);
		pParameters->Set_Enabled("EXTENT_GRID"  , pParameter->asInt() == 2);
		pParameters->Set_Enabled("EXTENT_SHAPES", pParameter->asInt() == 3);
		pParameters->Set_Enabled("EXTENT_BUFFER", pParameter->asInt() >= 2);
	}

    return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPDAL_Reader::On_Execute(void)
{
    CSG_Strings Files;

    if( !Parameters("FILES")->asFilePath()->Get_FilePaths(Files) )
    {
        Error_Set(_TL("empty file list"));

        return( false );
    }

    Parameters("POINTS")->asPointCloudList()->Del_Items();

	//-----------------------------------------------------
	CSG_Array_Int Classes;

	if( *Parameters("CLASSES")->asString() ) // not empty
	{
		CSG_Array_Int IDs; CSG_Strings List(SG_String_Tokenize(Parameters("CLASSES")->asString(), ",;"));

		for(int i=0, ID; i<List.Get_Count(); i++)
		{
			if( List[i].asInt(ID) )
			{
				IDs += ID;
			}
		}

		CSG_Index Index(IDs.Get_Size(), IDs.Get_Array()); // sort for better performance

		for(sLong i=0; i<IDs.Get_Size(); i++)
		{
			Classes += IDs[Index[i]];
		}
	}

	//-----------------------------------------------------
	CSG_Rect Extent; CSG_Projection Projection;

	switch( Parameters("EXTENT")->asInt() )
	{
	case  1:
		Extent.Assign(
			Parameters("EXTENT_XMIN")->asDouble(),
			Parameters("EXTENT_YMIN")->asDouble(),
			Parameters("EXTENT_XMAX")->asDouble(),
			Parameters("EXTENT_YMAX")->asDouble()
		);
		break;

	case  2:
		Extent       = Parameters("EXTENT_GRID"  )->asGrid_System()->Get_Extent();
		Extent.Inflate(Parameters("EXTENT_BUFFER")->asDouble(), false);
		break;

	case  3:
		Projection   = Parameters("EXTENT_SHAPES")->asShapes     ()->Get_Projection();
		Extent       = Parameters("EXTENT_SHAPES")->asShapes     ()->Get_Extent();
		Extent.Inflate(Parameters("EXTENT_BUFFER")->asDouble(), false);
		break;
	}

    //-----------------------------------------------------
    for(int i=0; i<Files.Get_Count() && Process_Get_Okay(); i++)
    {
        Process_Set_Text("[%d/%d] %s: %s", i + 1, Files.Get_Count(), _TL("File"), SG_File_Get_Name(Files[i], true).c_str());

        if( Files.Get_Count() == 1 )
        {
            Set_Progress(50., 100.);
        }
        else
        {
            Set_Progress(i + 1, Files.Get_Count());
        }

		CSG_PointCloud *pPoints = _Read_Points(Files[i], Extent, Classes,
			Parameters("VARS"     )->asBool(),
			Parameters("VAR_COLOR")->asBool(),
			Parameters("RGB_RANGE")->asInt ()
		);

		if( !pPoints->Get_Projection().is_Okay() && Projection.is_Okay() )
		{
			pPoints->Get_Projection() = Projection;
		}

		Parameters("POINTS")->asPointCloudList()->Add_Item(pPoints);
    }

    //-----------------------------------------------------
    return( Parameters("POINTS")->asInt() > 0 );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPDAL_Reader::_Find_Class(const CSG_Array_Int &Classes, int ID)
{
	for(sLong i=0; i<Classes.Get_Size(); i++)
	{
		if( ID  < Classes[i] ) // list is sorted ascending!
		{
			return( false );
		}

		if( ID == Classes[i] )
		{
			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
CSG_PointCloud * CPDAL_Reader::_Read_Points(const CSG_String &File, const CSG_Rect &Extent, const CSG_Array_Int &Classes, bool bVar_All, bool bVar_Color, int iRGB_Range)
{
    pdal::StageFactory Factory; std::string ReaderDriver = Factory.inferReaderDriver(File.b_str());

    if( ReaderDriver.empty() )
    {
        Message_Fmt("\n%s, %s: %s", _TL("Warning"), _TL("could not infer input file type"), File.c_str());

        return( NULL );
    }
    
    pdal::Stage *pReader = Factory.createStage(ReaderDriver);

    if( !pReader )
    {
        Message_Fmt("\n%s, %s: %s", _TL("Warning"), _TL("PDAL reader creation failed"), File.c_str());

        return( NULL );
    }
    
    pdal::Options Options; Options.add(pdal::Option("filename", File.b_str())); pReader->setOptions(Options);

    //-----------------------------------------------------
    CSG_PointCloud *pPoints = SG_Create_PointCloud();

    pPoints->Set_Name(SG_File_Get_Name(File, false));

	CSG_Array_Int Fields; int iRGB_Field = 0;

	//-----------------------------------------------------
    if( pReader->pipelineStreamable() )
    {
        pdal::StreamCallbackFilter StreamFilter;
        StreamFilter.setInput(*pReader);

        pdal::FixedPointTable Table(10000);
        
        StreamFilter.prepare(Table);

        pdal::PointLayoutPtr   PointLayout = Table.layout();
        pdal::SpatialReference SpatialRef  = Table.spatialReference();

		bool bClasses = Classes.Get_Size() && PointLayout->hasDim(pdal::Dimension::Id::Classification);

        _Init_PointCloud(pPoints, PointLayout, SpatialRef, File, bVar_All, bVar_Color, Fields, iRGB_Field);

        //-------------------------------------------------
		auto CallbackReadPoint = [=](pdal::PointRef &point)->bool
		{
			double x = point.getFieldAs<double>(pdal::Dimension::Id::X);
			double y = point.getFieldAs<double>(pdal::Dimension::Id::Y);

			if( (!Extent.Get_Area() || Extent.Contains(x, y)) && (!bClasses || _Find_Class(Classes, point.getFieldAs<int>(pdal::Dimension::Id::Classification))) )
			{
				pPoints->Add_Point(x, y, point.getFieldAs<double>(pdal::Dimension::Id::Z));

				for(int Field=0; Field<Fields.Get_Size(); Field++)
				{
					pPoints->Set_Value(3 + Field, point.getFieldAs<double>(g_Attributes[Fields[Field]].PDAL_ID));
				}

				if( iRGB_Field )
				{
					double r = point.getFieldAs<double>(pdal::Dimension::Id::Red  ); if( iRGB_Range ) { r *= 255. / 65535.; }
					double g = point.getFieldAs<double>(pdal::Dimension::Id::Green); if( iRGB_Range ) { g *= 255. / 65535.; }
					double b = point.getFieldAs<double>(pdal::Dimension::Id::Blue ); if( iRGB_Range ) { b *= 255. / 65535.; }

					pPoints->Set_Value(iRGB_Field, SG_GET_RGB(r, g, b));
				}
			}

			return( true );
		};
    
        StreamFilter.setCallback(CallbackReadPoint);
        StreamFilter.execute(Table);
    }
    else    // not streamable
    {
        pdal::PointTable   Table;    pReader->prepare(Table);
        pdal::PointViewSet ViewSet = pReader->execute(Table);
        pdal::PointViewPtr pView   = *ViewSet.begin();

        if( pView->size() < 1 )
        {
            Message_Fmt("\n%s, %s: %s", _TL("Warning"), _TL("invalid or empty file"), File.c_str());

			return( NULL );
        }

        pdal::PointLayoutPtr   PointLayout = Table.layout();
        pdal::SpatialReference SpatialRef  = Table.spatialReference();

		bool bClasses = Classes.Get_Size() && PointLayout->hasDim(pdal::Dimension::Id::Classification);

		_Init_PointCloud(pPoints, PointLayout, SpatialRef, File, bVar_All, bVar_Color, Fields, iRGB_Field);

        //-----------------------------------------------------
        for(pdal::PointId i=0; i<pView->size(); i++)
        {
			double x = pView->getFieldAs<double>(pdal::Dimension::Id::X, i);
			double y = pView->getFieldAs<double>(pdal::Dimension::Id::Y, i);

			if( (!Extent.Get_Area() || Extent.Contains(x, y)) && (!bClasses || _Find_Class(Classes, pView->getFieldAs<int>(pdal::Dimension::Id::Classification, i))) )
			{
				pPoints->Add_Point(x, y, pView->getFieldAs<double>(pdal::Dimension::Id::Z, i));

				for(int Field=0; Field<Fields.Get_Size(); Field++)
				{
					pPoints->Set_Value(3 + Field, pView->getFieldAs<double>(g_Attributes[Fields[Field]].PDAL_ID, i));
				}

				if( iRGB_Field )
				{
					double r = pView->getFieldAs<double>(pdal::Dimension::Id::Red  , i); if( iRGB_Range ) { r *= 255. / 65535.; }
					double g = pView->getFieldAs<double>(pdal::Dimension::Id::Green, i); if( iRGB_Range ) { g *= 255. / 65535.; }
					double b = pView->getFieldAs<double>(pdal::Dimension::Id::Blue , i); if( iRGB_Range ) { b *= 255. / 65535.; }

					pPoints->Set_Value(iRGB_Field, SG_GET_RGB(r, g, b));
				}
			}
        }
    }

    //-----------------------------------------------------
    if( pPoints->Get_Count() < 1 )
    {
        delete( pPoints );

		return( NULL );
    }

    return( pPoints );
}

//---------------------------------------------------------
void CPDAL_Reader::_Init_PointCloud(CSG_PointCloud *pPoints, pdal::PointLayoutPtr &PointLayout,
                                    pdal::SpatialReference &SpatialRef, const CSG_String &File,
                                    const bool &bVar_All, const bool &bVar_Color, CSG_Array_Int &Fields, int &iRGB_Field)
{
    if( !SpatialRef.empty() )
    {
        pPoints->Get_Projection().Create(SpatialRef.getWKT().c_str());
    }

    for(int Field=0; !g_Attributes[Field].ID.is_Empty(); Field++)
    {
        if( (bVar_All || Parameters(g_Attributes[Field].ID)->asBool()) )
        {
            if( PointLayout->hasDim(g_Attributes[Field].PDAL_ID) )
            {
                Fields += Field; pPoints->Add_Field(g_Attributes[Field].Name, g_Attributes[Field].Type);
            }
            else
            {
                SG_UI_Msg_Add_Execution(CSG_String::Format("\n%s, %s%s: %s", _TL("Warning"), _TL("file does not provide the dimension "), g_Attributes[Field].Name.c_str(), File.c_str()), true);
            }
        }
    }
    
    if( bVar_All || bVar_Color )
    {
        if( PointLayout->hasDim(pdal::Dimension::Id::Red  )
		&&  PointLayout->hasDim(pdal::Dimension::Id::Green)
		&&  PointLayout->hasDim(pdal::Dimension::Id::Blue ) )
        {
            iRGB_Field = pPoints->Get_Field_Count();

			pPoints->Add_Field("Color", SG_DATATYPE_DWord);
        }
    }

    return;
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


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
//                 Copyrights (C) 2020                   //
//                     Olaf Conrad                       //
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

//---------------------------------------------------------
#include <memory>

#include <pdal/Options.hpp>
#include <pdal/PointTable.hpp>
#include <pdal/PointView.hpp>
#include <pdal/io/LasReader.hpp>
#include <pdal/io/LasHeader.hpp>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
struct SLAS_Attributes
{
	CSG_String	ID, Name, Field;	TSG_Data_Type	Type;	pdal::Dimension::Id	PDAL_ID;
};

//---------------------------------------------------------
const struct SLAS_Attributes	g_Attributes[]	=
{
	{	"VAR_TIME"          , _TL("GPS-Time"                        ), _TL("Time"          ), SG_DATATYPE_Double, pdal::Dimension::Id::GpsTime           },
	{	"VAR_INTENSITY"     , _TL("Intensity"                       ), _TL("Intensity"     ), SG_DATATYPE_Float , pdal::Dimension::Id::Intensity         },
	{	"VAR_SCANANGLE"     , _TL("Scan Angle"                      ), _TL("Scan Angle"    ), SG_DATATYPE_Float , pdal::Dimension::Id::ScanAngleRank     },
	{	"VAR_RETURN"        , _TL("Number of the Return"            ), _TL("Return"        ), SG_DATATYPE_Int   , pdal::Dimension::Id::ReturnNumber      },
	{	"VAR_RETURNS"       , _TL("Number of Returns of Given Pulse"), _TL("Returns"       ), SG_DATATYPE_Int   , pdal::Dimension::Id::NumberOfReturns   },
	{	"VAR_CLASSIFICATION", _TL("Classification"                  ), _TL("Classification"), SG_DATATYPE_Int   , pdal::Dimension::Id::Classification    },
	{	"VAR_USERDATA"      , _TL("User Data"                       ), _TL("User Data"     ), SG_DATATYPE_Double, pdal::Dimension::Id::UserData          },
	{	"VAR_EDGE"          , _TL("Edge of Flight Line Flag"        ), _TL("Edge Flag"     ), SG_DATATYPE_Char  , pdal::Dimension::Id::EdgeOfFlightLine  },
	{	"VAR_DIRECTION"     , _TL("Direction of Scan Flag"          ), _TL("Direction Flag"), SG_DATATYPE_Char  , pdal::Dimension::Id::ScanDirectionFlag },
	{	"VAR_SOURCEID"      , _TL("Point Source ID"                 ), _TL("Source ID"     ), SG_DATATYPE_Int   , pdal::Dimension::Id::PointSourceId     },
	{	"VAR_COLOR_RED"     , _TL("Red Channel Color"               ), _TL("Red"           ), SG_DATATYPE_Int   , pdal::Dimension::Id::Red               },
	{	"VAR_COLOR_GREEN"   , _TL("Green Channel Color"             ), _TL("Green"         ), SG_DATATYPE_Int   , pdal::Dimension::Id::Green             },
	{	"VAR_COLOR_BLUE"    , _TL("Blue Channel Color"              ), _TL("Blue"          ), SG_DATATYPE_Int   , pdal::Dimension::Id::Blue              },
	{	"" , "" , "", SG_DATATYPE_Undefined, pdal::Dimension::Id::Unknown }
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPDAL_Reader::CPDAL_Reader(void)
{
	Set_Name		(_TL("Import Point Cloud"));

	Set_Author		("O.Conrad (c) 2020");

	Set_Description(_TW(
		"Import point cloud data sets from files using the Point Data Abstraction Library (PDAL)."
	));

	Add_Reference("https://pdal.io/", SG_T("PDAL Homepage"));

	//-----------------------------------------------------
	Parameters.Add_FilePath("",
		"FILES"		, _TL("Files"),
		_TL(""),
		CSG_String::Format("%s|*.las;*.laz|%s (*.las)|*.las|%s (*.laz)|*.laz|%s|*.*",
			_TL("Recognized Files"),
			_TL("LAS Files"),
			_TL("LAZ Files"),
			_TL("All Files")
		), NULL, false, false, true
	);

	Parameters.Add_Bool("",
		"VARS"		, _TL("Import Additional Attributes"),
		_TL("Check this to import all attributes provided by the data set, or select the attributes you want to become imported individually.")
	);

	for(int i=0; !g_Attributes[i].ID.is_Empty(); i++)
	{
		Parameters.Add_Bool("VARS", g_Attributes[i].ID, g_Attributes[i].Name, _TL(""));
	}

	Parameters.Add_Bool("VARS",
		"VAR_COLOR"	, _TL("RGB-Coded Color"),
		_TL("")
	);

	Parameters.Add_Choice("VAR_COLOR",
		"RGB_RANGE"	, _TL("RGB Value Range"),
		_TL("Data depth of red, green, blue values in LAS file."),
		CSG_String::Format("%s|%s",
			_TL ("8 bit"),
			_TL("16 bit")
		), 0
	);

	Parameters.Add_PointCloud_List("",
		"POINTS"	, _TL("Points"),
		_TL(""),
		PARAMETER_OUTPUT
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CPDAL_Reader::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("VARS") )
	{
		pParameter->Set_Children_Enabled(pParameter->asBool() == false);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPDAL_Reader::On_Execute(void)
{
	CSG_Strings	Files;

	if( !Parameters("FILES")->asFilePath()->Get_FilePaths(Files) )
	{
		Error_Set(_TL("empty file list"));

		return( false );
	}

	Parameters("POINTS")->asPointCloudList()->Del_Items();

	//-----------------------------------------------------
	for(int i=0; i<Files.Get_Count() && Process_Get_Okay(); i++)
	{
		Process_Set_Text("[%d/%d] %s: %s", i + 1, Files.Get_Count(), _TL("File"), SG_File_Get_Name(Files[i], true).c_str());

		Parameters("POINTS")->asPointCloudList()->Add_Item(Read_Points(Files[i]));
	}

	//-----------------------------------------------------
	return( Parameters("POINTS")->asInt() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_PointCloud * CPDAL_Reader::Read_Points(const CSG_String &File)
{
	pdal::LasReader	Reader;

	pdal::Options	Options;
	Options.add(pdal::Option("filename", File.b_str()));
	Reader.setOptions(Options);

	pdal::PointTable	Table;	  Reader.prepare(Table);
	pdal::PointViewSet	ViewSet	= Reader.execute(Table);
	pdal::PointViewPtr	pView	= *ViewSet.begin();

	if( Reader.header().pointCount() != pView->size() || Reader.header().pointCount() < 1 )
	{
		Message_Fmt("\n%s, %s: %s", _TL("Warning"), _TL("invalid or empty file"), File.c_str());

		return( false );
	}

	//-----------------------------------------------------
	CSG_PointCloud	*pPoints	= SG_Create_PointCloud();

	pPoints->Set_Name(SG_File_Get_Name(File, false));

	if( !Table.spatialReference().empty() )
	{
		pPoints->Get_Projection().Create(
			CSG_String(Table.spatialReference().getWKT  ().c_str()),
			CSG_String(Table.spatialReference().getProj4().c_str())
		);
	}

	//-----------------------------------------------------
	CSG_Array_Int	Fields;

	for(int Field=0; !g_Attributes[Field].ID.is_Empty(); Field++)
	{
		if( (Parameters("VARS")->asBool() || Parameters(g_Attributes[Field].ID)->asBool()) && pView->hasDim(g_Attributes[Field].PDAL_ID) )
		{
			Fields	+= Field; pPoints->Add_Field(g_Attributes[Field].Name, g_Attributes[Field].Type);
		}
	}

	int	RGB_Range	= Parameters("RGB_RANGE")->asInt();
	int	RGB_Field	= Parameters("VAR_COLOR")->asBool() && Reader.header().hasColor() ? pPoints->Get_Field_Count() : 0;
	if( RGB_Field )
	{
		pPoints->Add_Field("Color", SG_DATATYPE_Color);
	}

	//-----------------------------------------------------
	for(pdal::PointId i=0; i<pView->size() && Set_Progress(100. * i / (double)pView->size()); i++)
	{
		pPoints->Add_Point(
			pView->getFieldAs<double>(pdal::Dimension::Id::X, i),
			pView->getFieldAs<double>(pdal::Dimension::Id::Y, i),
			pView->getFieldAs<double>(pdal::Dimension::Id::Z, i)
		);

		for(int Field=0; Field<Fields.Get_Size(); Field++)
		{
			pPoints->Set_Value(3 + Field, pView->getFieldAs<double>(g_Attributes[Fields[Field]].PDAL_ID, i));
		}

		if( RGB_Field )
		{
			double	r	= pView->getFieldAs<int>(pdal::Dimension::Id::Red  , i); if( RGB_Range ) { r = r / 65535 * 255; }
			double	g	= pView->getFieldAs<int>(pdal::Dimension::Id::Green, i); if( RGB_Range ) { g = g / 65535 * 255; }
			double	b	= pView->getFieldAs<int>(pdal::Dimension::Id::Blue , i); if( RGB_Range ) { b = b / 65535 * 255; }

			pPoints->Set_Value(RGB_Field, SG_GET_RGB(r, g, b));
		}
	}

	//-----------------------------------------------------
	if( pPoints->Get_Count() < 1 )
	{
		delete(pPoints);

		return( NULL );
	}

	return( pPoints );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

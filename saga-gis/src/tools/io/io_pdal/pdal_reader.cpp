
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

#include <pdal/PointTable.hpp>
#include <pdal/PointView.hpp>
#include <pdal/Options.hpp>
#include <pdal/io/LasReader.hpp>
#include <pdal/io/LasHeader.hpp>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	VAR_T	= 0,	// gps-time
	VAR_i,			// intensity
	VAR_a,			// scan angle
	VAR_r,			// number of the return
	VAR_c,			// classification
	VAR_u,			// user data
	VAR_n,			// number of returns of given pulse
	VAR_R,			// red channel color
	VAR_G,			// green channel color
	VAR_B,			// blue channel color
	VAR_e,			// edge of flight line flag
	VAR_d,			// direction of scan flag
	VAR_p,			// point source ID
	VAR_C,			// color
	VAR_Count
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

	Parameters.Add_Node("",
		"VARS"		, _TL("Import Additional Attributes"),
		_TL("Select additional attributes to become imported, if these are available.")
	);

	Parameters.Add_Bool("VARS", "T", _TL("gps-time"                        ), _TL(""));
	Parameters.Add_Bool("VARS", "i", _TL("intensity"                       ), _TL(""));
	Parameters.Add_Bool("VARS", "a", _TL("scan angle"                      ), _TL(""));
	Parameters.Add_Bool("VARS", "r", _TL("number of the return"            ), _TL(""));
	Parameters.Add_Bool("VARS", "c", _TL("classification"                  ), _TL(""));
	Parameters.Add_Bool("VARS", "u", _TL("user data"                       ), _TL(""));
	Parameters.Add_Bool("VARS", "n", _TL("number of returns of given pulse"), _TL(""));
	Parameters.Add_Bool("VARS", "R", _TL("red channel color"               ), _TL(""));
	Parameters.Add_Bool("VARS", "G", _TL("green channel color"             ), _TL(""));
	Parameters.Add_Bool("VARS", "B", _TL("blue channel color"              ), _TL(""));
	Parameters.Add_Bool("VARS", "e", _TL("edge of flight line flag"        ), _TL(""));
	Parameters.Add_Bool("VARS", "d", _TL("direction of scan flag"          ), _TL(""));
	Parameters.Add_Bool("VARS", "p", _TL("point source ID"                 ), _TL(""));
	Parameters.Add_Bool("VARS", "C", _TL("rgb color"                       ), _TL(""));

	Parameters.Add_Choice("C",
		"RGB_RANGE"	, _TL("RGB value range"),
		_TL("Range of Red, Green, Blue values in LAS file."),
		CSG_String::Format("%s|%s",
			_TL("16 bit"),
			_TL("8 bit")
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

//	bool	bTime	= Reader.header().hasTime ();
//	bool	bColor	= Reader.header().hasColor();

//	pdal::Dimension::IdList dims = pView->dims();

	int	nFields	= 3, Field[VAR_Count];

	#define	ADD_FIELD(id, var, name, dim, type)	if( Parameters(id)->asBool() && pView->hasDim(dim) ) { Field[var] = nFields++; pPoints->Add_Field(name, type); } else { Field[var] = 0; }

	ADD_FIELD("T", VAR_T, _TL("GPS Time"                        ), pdal::Dimension::Id::GpsTime          , SG_DATATYPE_Double);	// SG_DATATYPE_Long
	ADD_FIELD("i", VAR_i, _TL("Intensity"                       ), pdal::Dimension::Id::Intensity        , SG_DATATYPE_Float );	// SG_DATATYPE_Word
	ADD_FIELD("a", VAR_a, _TL("Scan Angle"                      ), pdal::Dimension::Id::ScanAngleRank    , SG_DATATYPE_Float );	// SG_DATATYPE_Byte
	ADD_FIELD("r", VAR_r, _TL("Number of the Return"            ), pdal::Dimension::Id::ReturnNumber     , SG_DATATYPE_Int   );
	ADD_FIELD("c", VAR_c, _TL("Classification"                  ), pdal::Dimension::Id::Classification   , SG_DATATYPE_Int   );	// SG_DATATYPE_Byte
	ADD_FIELD("u", VAR_u, _TL("User Data"                       ), pdal::Dimension::Id::UserData         , SG_DATATYPE_Double);	// SG_DATATYPE_Byte
	ADD_FIELD("n", VAR_n, _TL("Number of Returns of Given Pulse"), pdal::Dimension::Id::UserData         , SG_DATATYPE_Int   );
	ADD_FIELD("e", VAR_e, _TL("Edge of Flight Line Flag"        ), pdal::Dimension::Id::NumberOfReturns  , SG_DATATYPE_Char  );
	ADD_FIELD("d", VAR_d, _TL("Direction of Scan Flag"          ), pdal::Dimension::Id::ScanDirectionFlag, SG_DATATYPE_Char  );
	ADD_FIELD("p", VAR_p, _TL("Point Source ID"                 ), pdal::Dimension::Id::PointSourceId    , SG_DATATYPE_Int   );	// SG_DATATYPE_Word
	ADD_FIELD("R", VAR_R, _TL("Red"                             ), pdal::Dimension::Id::Red              , SG_DATATYPE_Int   );	// SG_DATATYPE_Word
	ADD_FIELD("G", VAR_G, _TL("Green"                           ), pdal::Dimension::Id::Green            , SG_DATATYPE_Int   );
	ADD_FIELD("B", VAR_B, _TL("Blue"                            ), pdal::Dimension::Id::Blue             , SG_DATATYPE_Int   );
	ADD_FIELD("C", VAR_C, _TL("RGB Color"                       ), pdal::Dimension::Id::Red              , SG_DATATYPE_Int   );

	bool	bRGB_Range	= Parameters("RGB_RANGE")->asInt() == 0;

	//-----------------------------------------------------
	for(pdal::PointId i=0; i<pView->size() && Set_Progress(100. * i / (double)pView->size()); i++)
	{
		pPoints->Add_Point(
			pView->getFieldAs<double>(pdal::Dimension::Id::X, i),
			pView->getFieldAs<double>(pdal::Dimension::Id::Y, i),
			pView->getFieldAs<double>(pdal::Dimension::Id::Z, i)
		);

		if( Field[VAR_T] ) pPoints->Set_Value(Field[VAR_T], pView->getFieldAs<double>(pdal::Dimension::Id::GpsTime          , i));
		if( Field[VAR_i] ) pPoints->Set_Value(Field[VAR_i], pView->getFieldAs<int   >(pdal::Dimension::Id::Intensity        , i));
		if( Field[VAR_a] ) pPoints->Set_Value(Field[VAR_a], pView->getFieldAs<int   >(pdal::Dimension::Id::ScanAngleRank    , i));
		if( Field[VAR_r] ) pPoints->Set_Value(Field[VAR_r], pView->getFieldAs<int   >(pdal::Dimension::Id::ReturnNumber     , i));
		if( Field[VAR_c] ) pPoints->Set_Value(Field[VAR_c], pView->getFieldAs<int   >(pdal::Dimension::Id::Classification   , i));
		if( Field[VAR_u] ) pPoints->Set_Value(Field[VAR_u], pView->getFieldAs<int   >(pdal::Dimension::Id::UserData         , i));
		if( Field[VAR_n] ) pPoints->Set_Value(Field[VAR_n], pView->getFieldAs<int   >(pdal::Dimension::Id::NumberOfReturns  , i));
		if( Field[VAR_e] ) pPoints->Set_Value(Field[VAR_e], pView->getFieldAs<int   >(pdal::Dimension::Id::EdgeOfFlightLine , i));
		if( Field[VAR_d] ) pPoints->Set_Value(Field[VAR_d], pView->getFieldAs<int   >(pdal::Dimension::Id::ScanDirectionFlag, i));
		if( Field[VAR_p] ) pPoints->Set_Value(Field[VAR_p], pView->getFieldAs<int   >(pdal::Dimension::Id::PointSourceId    , i));
		if( Field[VAR_R] ) pPoints->Set_Value(Field[VAR_R], pView->getFieldAs<int   >(pdal::Dimension::Id::Red              , i));
		if( Field[VAR_G] ) pPoints->Set_Value(Field[VAR_G], pView->getFieldAs<int   >(pdal::Dimension::Id::Green            , i));
		if( Field[VAR_B] ) pPoints->Set_Value(Field[VAR_B], pView->getFieldAs<int   >(pdal::Dimension::Id::Blue             , i));
		if( Field[VAR_C] )
		{
			double	r	= pView->getFieldAs<int>(pdal::Dimension::Id::Red  , i);
			double	g	= pView->getFieldAs<int>(pdal::Dimension::Id::Green, i);
			double	b	= pView->getFieldAs<int>(pdal::Dimension::Id::Blue , i);

			if( bRGB_Range )	// 16 bit
			{
				r = r / 65535 * 255;
				g = g / 65535 * 255;
				b = b / 65535 * 255;
			}

			pPoints->Set_Value(Field[VAR_C], SG_GET_RGB(r, g, b));
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

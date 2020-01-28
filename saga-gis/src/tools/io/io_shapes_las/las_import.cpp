
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     Shapes_IO_LAS                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     las_import.cpp                    //
//                                                       //
//                 Copyright (C) 2009 by                 //
//              Olaf Conrad, Volker Wichmann             //
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
#include <liblas/laspoint.hpp>
#include <liblas/lasreader.hpp>
#include <liblas/capi/las_version.h>
#include <fstream>
#include <iostream>

#include "las_import.h"


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
CLAS_Import::CLAS_Import(void)
{
	Set_Name		(_TL("Import LAS Files"));

	Set_Author		("O. Conrad, V. Wichmann (c) 2009");

	CSG_String		Description(_TW(
		"This tool imports ASPRS LAS files (versions 1.0, 1.1 and 1.2) as Point Clouds "
		"using the \"libLAS\" library.\n"
		"\n"
		"Besides the x,y,z coordinates, the user can decide upon which attributes "
		"should be imported from the LAS file.\n"
		"\n"
		"Tool development was supported by "
		"<a href=\"http://www.laserdata.at\">http://Laserdata GmbH, Austria</a>\n"
		"and "
		"<a href=\"http://www.alp-s.at\">http://alpS GmbH, Austria</a>\n"
		"\nliblas version: "
	));

	Description	+= CSG_String(LIBLAS_RELEASE_NAME);

	Set_Description(Description);

	Add_Reference("http://liblas.org", SG_T("libLAS Homepage"));

	//-----------------------------------------------------
	Parameters.Add_FilePath("",
		"FILES"		, _TL("Input Files"),
		_TL(""),
		_TL("LAS Files (*.las)|*.las|LAS Files (*.LAS)|*.LAS|All Files|*.*"),
		NULL, false, false, true
	);

	Parameters.Add_PointCloud_List("",
		"POINTS"	, _TL("Point Clouds"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Node("",
		"VARS"		, _TL("Attributes to import besides x, y, z..."),
		_TL("")
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

	Parameters.Add_Bool("",
		"VALID"		, _TL("Check Point Validity"),
		_TL("")
	);

	Parameters.Add_Choice("",
		"RGB_RANGE"	, _TL("RGB value range"),
		_TL("Range of Red, Green, Blue values in LAS file."),
		CSG_String::Format("%s|%s",
			_TL("16 bit"),
			_TL("8 bit")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLAS_Import::On_Execute(void)
{
	CSG_Strings	Files;

	if( !Parameters("FILES")->asFilePath()->Get_FilePaths(Files) )
	{
		return( false );
	}

	//-----------------------------------------------------
	Parameters("POINTS")->asPointCloudList()->Del_Items();

	bool	bValidate	= Parameters("VALID")->asBool();
	int		RGBrange	= Parameters("RGB_RANGE")->asInt();
	int		nInvalid	= 0;

	//-----------------------------------------------------
	for(int iFile=0; iFile<Files.Get_Count() && Process_Get_Okay(); iFile++)
	{
		Message_Fmt("\n%s %s...", _TL("Loading"), SG_File_Get_Name(Files[iFile], true).c_str());

		if( !is_Okay(Files[iFile]) )
		{
			Message_Add(_TL("failed"));

			Error_Fmt(_TL("Unable to open LAS file!"), SG_File_Get_Name(Files[iFile], true).c_str());

			continue;
		}

		std::ifstream Stream(Files[iFile].b_str(), std::ios::in|std::ios::binary);

		liblas::LASReader Reader(Stream);

		//-------------------------------------------------
		CSG_PointCloud	*pPoints	= SG_Create_PointCloud();

		pPoints->Set_Name(SG_File_Get_Name(Files[iFile], false));

		CSG_MetaData	MetaData;

		if( MetaData.Create(Files[iFile], SG_T("xml")) )
		{
			pPoints->Get_MetaData() = MetaData;

			if( MetaData("MapProjectionDefinition") )
			{
				pPoints->Get_Projection().Create(MetaData["MapProjectionDefinition"].Get_Content());
			}
		}

		int	nFields	= 3, Field[VAR_Count];

		#define	ADD_FIELD(id, var, name, type)	if( Parameters(id)->asBool() ) { Field[var] = nFields++; pPoints->Add_Field(name, type); } else { Field[var] = -1; }

		ADD_FIELD("T", VAR_T, _TL("gps-time"                        ), SG_DATATYPE_Double);	// SG_DATATYPE_Long
		ADD_FIELD("i", VAR_i, _TL("intensity"                       ), SG_DATATYPE_Float );	// SG_DATATYPE_Word
		ADD_FIELD("a", VAR_a, _TL("scan angle"                      ), SG_DATATYPE_Float );	// SG_DATATYPE_Byte
		ADD_FIELD("r", VAR_r, _TL("number of the return"            ), SG_DATATYPE_Int   );
		ADD_FIELD("c", VAR_c, _TL("classification"                  ), SG_DATATYPE_Int   );	// SG_DATATYPE_Byte
		ADD_FIELD("u", VAR_u, _TL("user data"                       ), SG_DATATYPE_Double);	// SG_DATATYPE_Byte
		ADD_FIELD("n", VAR_n, _TL("number of returns of given pulse"), SG_DATATYPE_Int   );
		ADD_FIELD("R", VAR_R, _TL("red channel color"               ), SG_DATATYPE_Int   );	// SG_DATATYPE_Word
		ADD_FIELD("G", VAR_G, _TL("green channel color"             ), SG_DATATYPE_Int   );
		ADD_FIELD("B", VAR_B, _TL("blue channel color"              ), SG_DATATYPE_Int   );
		ADD_FIELD("e", VAR_e, _TL("edge of flight line flag"        ), SG_DATATYPE_Char  );
		ADD_FIELD("d", VAR_d, _TL("direction of scan flag"          ), SG_DATATYPE_Char  );
		ADD_FIELD("p", VAR_p, _TL("point source ID"                 ), SG_DATATYPE_Int   );	// SG_DATATYPE_Word
		ADD_FIELD("C", VAR_C, _TL("RGB color"                       ), SG_DATATYPE_Int   );

		//-----------------------------------------------------
		try
		{
			int	iPoint	= 0;

			while( Reader.ReadNextPoint() )
			{
				if( iPoint % 100000 == 0 )
				{
					Set_Progress(iPoint, Reader.GetHeader().GetPointRecordsCount()); 
				}

				liblas::LASPoint const& Point = Reader.GetPoint();

				if( bValidate && !Point.IsValid() )
				{
					nInvalid++;

					continue;
				}

				pPoints->Add_Point(Point.GetX(), Point.GetY(), Point.GetZ());

				if( Field[VAR_T] > 0 )	pPoints->Set_Value(iPoint, Field[VAR_T], Point.GetTime            ());
				if( Field[VAR_i] > 0 )	pPoints->Set_Value(iPoint, Field[VAR_i], Point.GetIntensity       ());
				if( Field[VAR_a] > 0 )	pPoints->Set_Value(iPoint, Field[VAR_a], Point.GetScanAngleRank   ());
				if( Field[VAR_r] > 0 )	pPoints->Set_Value(iPoint, Field[VAR_r], Point.GetReturnNumber    ());
				if( Field[VAR_c] > 0 )	pPoints->Set_Value(iPoint, Field[VAR_c], Point.GetClassification  ());
				if( Field[VAR_u] > 0 )	pPoints->Set_Value(iPoint, Field[VAR_u], Point.GetUserData        ());
				if( Field[VAR_n] > 0 )	pPoints->Set_Value(iPoint, Field[VAR_n], Point.GetNumberOfReturns ());
				if( Field[VAR_R] > 0 )	pPoints->Set_Value(iPoint, Field[VAR_R], Point.GetColor().GetRed  ());
				if( Field[VAR_G] > 0 )	pPoints->Set_Value(iPoint, Field[VAR_G], Point.GetColor().GetGreen());
				if( Field[VAR_B] > 0 )	pPoints->Set_Value(iPoint, Field[VAR_B], Point.GetColor().GetBlue ());
				if( Field[VAR_e] > 0 )	pPoints->Set_Value(iPoint, Field[VAR_e], Point.GetFlightLineEdge  ());
				if( Field[VAR_d] > 0 )	pPoints->Set_Value(iPoint, Field[VAR_d], Point.GetScanDirection   ());
				if( Field[VAR_p] > 0 )	pPoints->Set_Value(iPoint, Field[VAR_p], Point.GetPointSourceID   ());
				if( Field[VAR_C] > 0 )
				{
					double	r	= Point.GetColor().GetRed  ();
					double	g	= Point.GetColor().GetGreen();
					double	b	= Point.GetColor().GetBlue ();

					if( RGBrange == 0 )	// 16 bit
					{
						r = r / 65535 * 255;
						g = g / 65535 * 255;
						b = b / 65535 * 255;
					}
			
					pPoints->Set_Value(iPoint, Field[VAR_C], SG_GET_RGB(r, g, b));
				}

				iPoint++;
			}
		}

		//-------------------------------------------------
		catch( std::exception &e )
		{
			Error_Fmt("%s: %s", _TL("LAS reader exception"), e.what());

			continue;
		}

		catch( ... )
		{
			Error_Set(_TL("Unknown LAS reader exception!"));

			continue;
		}

		//-------------------------------------------------
		Parameters("POINTS")->asPointCloudList()->Add_Item(pPoints);

		DataObject_Add(pPoints);

		DataObject_Set_Parameter(pPoints, "DISPLAY_VALUE_AGGREGATE", 3);	// highest z
		DataObject_Set_Parameter(pPoints, "COLORS_TYPE"            , 2);	// graduated color
		DataObject_Set_Parameter(pPoints, "METRIC_ATTRIB"          , 2);	// z attrib
		DataObject_Set_Parameter(pPoints, "METRIC_ZRANGE.MIN"      , pPoints->Get_Minimum(2));
		DataObject_Set_Parameter(pPoints, "METRIC_ZRANGE.MAX"      , pPoints->Get_Maximum(2));

		Message_Add(_TL("okay"), false);
	}

	//-----------------------------------------------------
	if( nInvalid > 0 )
	{
		Message_Fmt("\n%s: %s [%d]", _TL("Warning"), _TL("invalid points skipped!"), nInvalid);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLAS_Import::is_Okay(const CSG_String &File)
{
	std::ifstream Stream(File.b_str(), std::ios::in|std::ios::binary);

	if( !Stream.good() )
	{
		Error_Fmt(_TL("Unable to open LAS file!"), SG_File_Get_Name(File, true).c_str());

		return( false );
	}

	liblas::LASReader *pReader;

	try	// Check if LAS version is supported
	{
		pReader	= new liblas::LASReader(Stream);
	}

	catch( std::exception &e )
	{
		SG_UI_Msg_Add_Error(CSG_String::Format("%s: %s", _TL("LAS header exception"), e.what()));

		return( false );
	}

	catch( ... )
	{
		SG_UI_Msg_Add_Error(_TL("Unknown LAS header exception!"));

		return( false );
	}

	delete(pReader);

	Stream.clear();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


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
//                     las_info.cpp                      //
//                                                       //
//                 Copyright (C) 2010 by                 //
//                    Volker Wichmann                    //
//                                                       //
//    Implementation builds upon the lasinfo tool of     //
//         Martin Isenburg (isenburg@cs.unc.edu)         //
//                  Copyright (C) 2007                   //
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
//                Management and analysis of             //
//                laserscanning data                     //
//                Innsbruck, Austria                     //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "las_info.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CLAS_Info::CLAS_Info(void)
{
	Set_Name		(_TL("LAS Info"));

	Set_Author		("Volker Wichmann (c) 2010, LASERDATA GmbH");

	CSG_String		Description(_TW(
		"Prints information on ASPRS LAS files (versions 1.0, 1.1 and 1.2) "
		"using the \"libLAS\" library. "
		"Get more information about this library at "
		"<a href=\"http://liblas.org\">http://liblas.org</a>\n"
		"Implementation of this tool builds upon the lasinfo tool of "
		"<a href=\"http://www.cs.unc.edu/~isenburg/lastools\">Martin Isenburg</a>.\n"
		"\n"
		"\nliblas version: "
	));

	Description	+= CSG_String(LIBLAS_RELEASE_NAME);

	Set_Description	(Description);

	//-----------------------------------------------------
	Parameters.Add_FilePath(
		"", "FILE"	, _TL("LAS File"),
		_TL(""),
		CSG_String::Format("%s (*.las)|*.las|%s|*.*",
			_TL("LAS Files"),
			_TL("All Files")
		)
	);

	Parameters.Add_Bool(
		"", "HEADER", _TL("Only Header Info"),
		_TL("Print only information available in LAS header."),
		false
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLAS_Info::On_Execute(void)
{
	CSG_String		fName;
	bool			bHeader;
	LASPointSummary summary;

	//-----------------------------------------------------
	fName		= Parameters("FILE")->asString();
	bHeader		= Parameters("HEADER")->asBool();	

	//-----------------------------------------------------
    std::ifstream   ifs;

    ifs.open(fName.b_str(), std::ios::in | std::ios::binary);

    if( !ifs )
    {
        Error_Fmt("%s: [%s]", _TL("Unable to open LAS file"), fName.c_str());

        return( false );
    }

	//-----------------------------------------------------
	// Check if LAS version is supported
	liblas::LASReader *pReader;
	try {
		pReader = new liblas::LASReader(ifs);
	}
	catch(std::exception &e) {
		Error_Fmt("%s: %s", _TL("LAS header exception"), e.what());
		ifs.close();
        return (false);
	}
	catch(...) {
		Error_Fmt(_TL("Unknown LAS header exception!"));
		ifs.close();
        return (false);
	}
	
	delete (pReader);
	ifs.clear();

	//-----------------------------------------------------
    liblas::LASReader reader(ifs);

    liblas::LASHeader const& header = reader.GetHeader();

	//-----------------------------------------------------
	Print_Header(fName, header);

	if( !bHeader )
	{
		LASPointSummary		*pSummary;

		pSummary = (LASPointSummary*) malloc(sizeof(LASPointSummary));

		if( !Summarize_Points(&reader, pSummary, header.GetPointRecordsCount()) )
			return (false);

		if( !Print_Point_Summary(header, pSummary) )
			return (false);

		free (pSummary);
	}

	//-----------------------------------------------------
	ifs.close();

	return( true );
}


//---------------------------------------------------------
bool CLAS_Info::Print_Header(CSG_String fName, liblas::LASHeader header)
{

	Message_Add("\n");
	Message_Add("\n---------------------------------------------------------");
	SG_UI_Msg_Add(_TL("Header Summary"), true, SG_UI_MSG_STYLE_BOLD);
	Message_Add("\n---------------------------------------------------------");

	Message_Fmt("\n  %s:\t\t\t%s", _TL("File Name"), fName.c_str());

	if( SG_STR_CMP(header.GetFileSignature().c_str(), SG_T("LASF")) )
	{
		SG_UI_Msg_Add_Error(_TL("File signature is not 'LASF'!"));

		return (false);
	}

	Message_Fmt("\n  %s:\t\t\t%d.%d", _TL("Version"),
		header.GetVersionMajor(),
		header.GetVersionMinor()
	);

	Message_Fmt("\n  %s:\t\t\t%d", _TL("Source ID"),
		header.GetFileSourceId()
	);

	Message_Fmt("\n  %s:\t\t\t%d", _TL("Reserved"),
		header.GetReserved()
	);

	Message_Fmt("\n  %s:\t\t'%s'", _TL("Project ID/GUID"),
		CSG_String(header.GetProjectId().to_string().c_str()).w_str()
	);

	Message_Fmt("\n  %s:\t\t'%s'", _TL("System Identifier"),
		CSG_String(header.GetSystemId().c_str()).w_str()
	);

	Message_Fmt("\n  %s:\t\t'%s'", _TL("Generating Software"),
		CSG_String(header.GetSoftwareId().c_str()).w_str()
	);

	Message_Fmt("\n  %s:\t\t%d/%d", _TL("File Creation Day/Year"),
		header.GetCreationDOY(),
		header.GetCreationYear()
	);

	Message_Fmt("\n  %s:\t\t\t%d", _TL("Header Size"),
		header.GetHeaderSize()
	);

	Message_Fmt("\n  %s:\t\t%d", _TL("Offset to Point Data"),
		header.GetDataOffset()
	);

	Message_Fmt("\n  %s:\t%d", _TL("Number Var. Length Records"),
		header.GetRecordsCount()
	);

	Message_Fmt("\n  %s:\t\t%d", _TL("Point Data Format"),
		header.GetDataFormatId()
	);

	Message_Fmt("\n  %s:\t%d", _TL("Point Data Record Length"),
		header.GetDataRecordLength()
	);

	Message_Fmt("\n  %s:\t%d", _TL("Number of Point Records"),
		header.GetPointRecordsCount()
	);

	Message_Fmt("\n  %s:\t%d  %d  %d  %d  %d", _TL("Number of Points by Return"),
		header.GetPointRecordsByReturnCount().at(0),
		header.GetPointRecordsByReturnCount().at(1),
		header.GetPointRecordsByReturnCount().at(2),
		header.GetPointRecordsByReturnCount().at(3),
		header.GetPointRecordsByReturnCount().at(4)
	);

	Message_Fmt("\n  %s X Y Z:\t\t%.6g  %.6g  %.6g", _TL("Scale Factor"),
		header.GetScaleX(),
		header.GetScaleY(),
		header.GetScaleZ()
	);

	Message_Fmt("\n  %s X Y Z:\t\t\t%.6f  %.6f  %.6f", _TL("Offset"),
		header.GetOffsetX(),
		header.GetOffsetY(),
		header.GetOffsetZ()
	);

	Message_Fmt("\n  %s X Y Z:\t\t\t%.6f  %.6f  %.6f", _TL("Min"),
		header.GetMinX(),
		header.GetMinY(),
		header.GetMinZ()
	);

	Message_Fmt("\n  %s X Y Z:\t\t\t%.6f  %.6f  %.6f", _TL("Max"),
		header.GetMaxX(),
		header.GetMaxY(),
		header.GetMaxZ()
	);

	Message_Fmt("\n  %s:\t\t%s", _TL("Spatial Reference"),
		CSG_String(header.GetSRS().GetProj4().c_str()).w_str()
	);

	return( true );
}

//---------------------------------------------------------
bool CLAS_Info::Print_Point_Summary(liblas::LASHeader header, LASPointSummary *pSummary)
{
	long rgpsum		= 0;
	long pbretsum	= 0;
	int	i			= 0;

	if( pSummary->number_of_point_records == 0 )
	{
		Error_Fmt(_TL("Point summary contains no points!"));

		return( false );
	}

	Message_Add("\n");
	Message_Add("\n---------------------------------------------------------");
	SG_UI_Msg_Add(_TL("Point Inspection Summary"), true, SG_UI_MSG_STYLE_BOLD);
	Message_Add("\n---------------------------------------------------------");

	Message_Fmt("\n %s:\t\t%d", _TL("Header Point Count"),
		header.GetPointRecordsCount()
	);

	Message_Fmt("\n %s:\t\t%d", _TL("Actual Point Count"),
		pSummary->number_of_point_records
	);

	Message_Add("\n");
	SG_UI_Msg_Add(_TL("Minimum and Maximum Attributes (min, max)"), true, SG_UI_MSG_STYLE_BOLD);
	Message_Add("\n---------------------------------------------------------");

	Message_Fmt("\n  %s X Y Z:\t\t\t%.6f  %.6f  %.6f", _TL("Min"),
		pSummary->pmin.GetX(),
		pSummary->pmin.GetY(),
		pSummary->pmin.GetZ()
	);

	Message_Fmt("\n  %s X Y Z:\t\t\t%.6f  %.6f  %.6f", _TL("Max"),
		pSummary->pmax.GetX(),
		pSummary->pmax.GetY(),
		pSummary->pmax.GetZ()
	);

	Message_Fmt("\n  %s:\t\t\t%.2f, %.2f, %.2f, %.2f", _TL("Bounding Box"),
		pSummary->pmin.GetX(),
		pSummary->pmin.GetY(),
		pSummary->pmax.GetX(),
		pSummary->pmax.GetY()
	);

	Message_Fmt("\n  %s:\t\t\t\t%.6f, %.6f", _TL("Time"),
		pSummary->pmin.GetTime(),
		pSummary->pmax.GetTime()
	);

	Message_Fmt("\n  %s:\t\t%d, %d", _TL("Return Number"),
		pSummary->pmin.GetReturnNumber(),
		pSummary->pmax.GetReturnNumber()
	);

	Message_Fmt("\n  %s:\t\t\t%d, %d", _TL("Return Count"),
		pSummary->pmin.GetNumberOfReturns(),
		pSummary->pmax.GetNumberOfReturns()
	);

	Message_Fmt("\n  %s:\t\t\t%d, %d", _TL("Flightline Edge"),
		pSummary->pmin.GetFlightLineEdge(),
		pSummary->pmax.GetFlightLineEdge()
	);

	Message_Fmt("\n  %s:\t\t\t%d, %d", _TL("Intensity"),
		pSummary->pmin.GetIntensity(),
		pSummary->pmax.GetIntensity()
	);

	Message_Fmt("\n  %s:\t\t%d, %d", _TL("Scan Direction Flag"),
		pSummary->pmin.GetScanDirection(),
		pSummary->pmax.GetScanDirection()
	);

	Message_Fmt("\n  %s:\t\t%d, %d", _TL("Scan Angle Rank"),
		pSummary->pmin.GetScanAngleRank(),
		pSummary->pmax.GetScanAngleRank()
	);

	Message_Fmt("\n  %s:\t\t\t%d, %d", _TL("Classification"),
		pSummary->pmin.GetClassification(),
		pSummary->pmax.GetClassification()
	);

	Message_Fmt("\n  %s:\t\t%d, %d", _TL("Point Source Id"),
		pSummary->pmin.GetPointSourceID(),
		pSummary->pmax.GetPointSourceID()
	);

	Message_Fmt("\n  %s:\t\t\t%d %d %d", _TL("Minimum Color"),
		pSummary->pmin.GetColor().GetRed(),
		pSummary->pmin.GetColor().GetGreen(),
		pSummary->pmin.GetColor().GetBlue()
	);

	Message_Fmt("\n  %s:\t\t%d %d %d", _TL("Maximum Color"),
		pSummary->pmax.GetColor().GetRed(),
		pSummary->pmax.GetColor().GetGreen(),
		pSummary->pmax.GetColor().GetBlue()
	);

	Message_Add("\n");
	SG_UI_Msg_Add(_TL("Number of Points by Return"), true, SG_UI_MSG_STYLE_BOLD);
	Message_Add("\n---------------------------------------------------------");

	for( i=0; i<5; i++ )
	{
		pbretsum = pbretsum + pSummary->number_of_points_by_return[i];

		Message_Fmt("\n  \t(%d) %d", i, pSummary->number_of_points_by_return[i]);
	}

	Message_Fmt("\n  %s:\t%ld", _TL("Total Points"), pbretsum);

	Message_Add("\n");
	SG_UI_Msg_Add(_TL("Number of Returns by Pulse"), true, SG_UI_MSG_STYLE_BOLD);
	Message_Add("\n---------------------------------------------------------");

	for(i=0; i<8; i++)
	{
		rgpsum = rgpsum + pSummary->number_of_returns_of_given_pulse[i];

		Message_Fmt("\n  \t(%d) %d", i, pSummary->number_of_returns_of_given_pulse[i]);
	}

	Message_Fmt("\n  %s:\t%ld", _TL("Total Pulses"), rgpsum);


	for( i=0; i<5; i++ )
	{
		if( header.GetPointRecordsByReturnCount().at(i) != pSummary->number_of_points_by_return[i] )
		{
			Message_Fmt("\n  %s:", _TL("Actual number of points by return is different from header (actual, header)"));

			for( int j=0; i<5; i++ )
			{
				Message_Fmt("\n  \t(%d, %d",
					pSummary->number_of_points_by_return[j],
					header.GetPointRecordsByReturnCount().at(j)
				);
			}
		}
	}


	Message_Add("\n");
	SG_UI_Msg_Add(_TL("Point Classifications"), true, SG_UI_MSG_STYLE_BOLD);
	Message_Add("\n---------------------------------------------------------");

	for( i=0; i<32; i++ )
	{
		if( pSummary->classification[i] )
		{
			Message_Fmt("\n  \t%d\t\t%s (%d)", 
				pSummary->classification[i],
				gLASPointClassification_Key_Name[i],
				i
			);
		}
	}

	if( pSummary->classification_synthetic || pSummary->classification_keypoint || pSummary->classification_withheld )
	{
		Message_Add("\n");
		SG_UI_Msg_Add(_TL("Point Classification Histogram"), true, SG_UI_MSG_STYLE_BOLD);
		Message_Add("\n---------------------------------------------------------");

		if( pSummary->classification_synthetic )
			Message_Fmt("\n  +-> %s:\t%d", _TL("flagged as synthetic"),
				pSummary->classification_synthetic
			);
		if( pSummary->classification_keypoint )
			Message_Fmt("\n  +-> %s:\t%d", _TL("flagged as keypoints"),
				pSummary->classification_keypoint
			);
		if( pSummary->classification_withheld )
			Message_Fmt("\n  +-> %s:\t%d", _TL("flagged as withheld"),
				pSummary->classification_withheld
			);
	}

	return (true);
}


//---------------------------------------------------------
bool CLAS_Info::Summarize_Points(liblas::LASReader *pReader, LASPointSummary *pSummary, int headerPts)
{

	liblas::LASColor	color;
	liblas::LASColor	min_color;
	liblas::LASColor	max_color;
	liblas::uint8_t		cls			= 0;
	liblas::uint16_t	red			= 0;
	liblas::uint16_t	green		= 0;
	liblas::uint16_t	blue		= 0;
	liblas::uint16_t	ptsrc		= 0;
    int					i			= 0;
	bool				bValid		= false;


    pSummary->number_of_point_records	= 0;
	for( i=0; i<8; i++ )
		pSummary->number_of_points_by_return[i]	= 0;
	for( i=0; i<8; i++ )
		pSummary->number_of_returns_of_given_pulse[i] = 0;
	for( i=0; i<32; i++ )
		pSummary->classification[i]		= 0;
	pSummary->classification_synthetic	= 0;
	pSummary->classification_keypoint	= 0;
	pSummary->classification_withheld	= 0;


	//-----------------------------------------------------
	if( !pReader->ReadNextPoint() )
	{
		SG_UI_Msg_Add_Error(_TL("Unable to read points from LAS file!"));
		return (false);
	}
	else
		bValid = true;

	liblas::LASPoint const& p = pReader->GetPoint();

	pSummary->pmin	= p;
    pSummary->pmax	= p;

	i = 1;

	//-----------------------------------------------------
	while( bValid )
	{
		if (i % 100000 == 0)
			SG_UI_Process_Set_Progress(i, headerPts);

		pSummary->x = p.GetX();
		if( pSummary->x < pSummary->pmin.GetX() )					pSummary->pmin.SetX(pSummary->x);
		if( pSummary->x > pSummary->pmax.GetX() )					pSummary->pmax.SetX(pSummary->x);

		pSummary->y = p.GetY();
		if( pSummary->y < pSummary->pmin.GetY() )					pSummary->pmin.SetY(pSummary->y);
		if( pSummary->y > pSummary->pmax.GetY() )					pSummary->pmax.SetY(pSummary->y);

		pSummary->z = p.GetZ();
		if( pSummary->z < pSummary->pmin.GetZ() )					pSummary->pmin.SetZ(pSummary->z);
		if( pSummary->z > pSummary->pmax.GetZ() )					pSummary->pmax.SetZ(pSummary->z);

		pSummary->intensity = p.GetIntensity();
		if( pSummary->intensity < pSummary->pmin.GetIntensity() )	pSummary->pmin.SetIntensity(pSummary->intensity);
		if( pSummary->intensity > pSummary->pmax.GetIntensity() )	pSummary->pmax.SetIntensity(pSummary->intensity);

		pSummary->t = p.GetTime();
		if( pSummary->t < pSummary->pmin.GetTime() )				pSummary->pmin.SetTime(pSummary->t);
		if( pSummary->t > pSummary->pmax.GetTime() )				pSummary->pmax.SetTime(pSummary->t);

		pSummary->retnum = p.GetReturnNumber();
		if( pSummary->retnum < pSummary->pmin.GetReturnNumber() )	pSummary->pmin.SetReturnNumber(pSummary->retnum);
		if( pSummary->retnum > pSummary->pmax.GetReturnNumber() )	pSummary->pmax.SetReturnNumber(pSummary->retnum);

		pSummary->numret = p.GetNumberOfReturns();
		if( pSummary->numret < pSummary->pmin.GetNumberOfReturns() )pSummary->pmin.SetNumberOfReturns(pSummary->numret);
		if( pSummary->numret > pSummary->pmax.GetNumberOfReturns() )pSummary->pmax.SetNumberOfReturns(pSummary->numret);

		pSummary->scandir = p.GetScanDirection();
		if( pSummary->scandir < pSummary->pmin.GetScanDirection() )	pSummary->pmin.SetScanDirection(pSummary->scandir);
		if( pSummary->scandir > pSummary->pmax.GetScanDirection() )	pSummary->pmax.SetScanDirection(pSummary->scandir);

		pSummary->fedge = p.GetFlightLineEdge();
		if( pSummary->fedge < pSummary->pmin.GetFlightLineEdge() )	pSummary->pmin.SetFlightLineEdge(pSummary->fedge);
		if( pSummary->fedge > pSummary->pmax.GetFlightLineEdge() )	pSummary->pmax.SetFlightLineEdge(pSummary->fedge);

		pSummary->scan_angle = p.GetScanAngleRank();
		if( pSummary->scan_angle < pSummary->pmin.GetScanAngleRank() )	pSummary->pmin.SetScanAngleRank(pSummary->scan_angle);
		if( pSummary->scan_angle > pSummary->pmax.GetScanAngleRank() )	pSummary->pmax.SetScanAngleRank(pSummary->scan_angle);

		pSummary->user_data = p.GetUserData();
		if( pSummary->user_data < pSummary->pmin.GetUserData() )	pSummary->pmin.SetUserData(pSummary->user_data);
		if( pSummary->user_data > pSummary->pmax.GetUserData() )	pSummary->pmax.SetUserData(pSummary->user_data);

		pSummary->number_of_point_records = i;

		if( p.GetReturnNumber() )
			pSummary->number_of_points_by_return[p.GetReturnNumber() - 1]++;
		else
			pSummary->number_of_points_by_return[p.GetReturnNumber()]++;

		pSummary->number_of_returns_of_given_pulse[p.GetNumberOfReturns()]++;

		cls = p.GetClassification();
		if( cls < pSummary->pmin.GetClassification() )				pSummary->pmin.SetClassification(cls);
		if( cls > pSummary->pmax.GetClassification() )				pSummary->pmax.SetClassification(cls);

		ptsrc = p.GetPointSourceID();
		if( ptsrc < pSummary->pmin.GetPointSourceID() )				pSummary->pmin.SetPointSourceID(ptsrc);
		if( ptsrc > pSummary->pmax.GetPointSourceID() )				pSummary->pmax.SetPointSourceID(ptsrc);

		color = p.GetColor();
		min_color = pSummary->pmin.GetColor();
		max_color = pSummary->pmax.GetColor();

		min_color.GetRed() < color.GetRed() ? red = min_color.GetRed() : red = color.GetRed();
		min_color.GetGreen() < color.GetGreen() ? green = min_color.GetGreen() : green = color.GetGreen();
		min_color.GetBlue() < color.GetBlue() ? blue = min_color.GetBlue() : blue = color.GetBlue();

		min_color.SetRed(red);
		min_color.SetGreen(green);
		min_color.SetBlue(blue);

		pSummary->pmin.SetColor(min_color);
		
		max_color.GetRed() > color.GetRed() ? red = max_color.GetRed() : red = color.GetRed();
		max_color.GetGreen() > color.GetGreen() ? green = max_color.GetGreen() : green = color.GetGreen();
		max_color.GetBlue() > color.GetBlue() ? blue = max_color.GetBlue() : blue = color.GetBlue();

		max_color.SetRed(red);
		max_color.SetGreen(green);
		max_color.SetBlue(blue);

		pSummary->pmax.SetColor(max_color);

		pSummary->classification[cls & 31]++;
		if( cls & 32 )
			pSummary->classification_synthetic++;
		if( cls & 64 )
			pSummary->classification_keypoint++;
		if( cls & 128 )
			pSummary->classification_withheld++;



		if( !pReader->ReadNextPoint() )
			bValid = false;		
		else
		{
			bValid = true;
			liblas::LASPoint const& p = pReader->GetPoint();
			i++;
		}
    }
    
    return (true);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

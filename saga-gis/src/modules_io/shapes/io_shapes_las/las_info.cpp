/**********************************************************
 * Version $Id$
 *********************************************************/


///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
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
// Free Software Foundation; version 2 of the License.   //
//                                                       //
// SAGA is distributed in the hope that it will be       //
// useful, but WITHOUT ANY WARRANTY; without even the    //
// implied warranty of MERCHANTABILITY or FITNESS FOR A  //
// PARTICULAR PURPOSE. See the GNU General Public        //
// License for more details.                             //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
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

	Set_Author		(SG_T("Volker Wichmann (c) 2010, LASERDATA GmbH"));

	CSG_String		Description(_TW(
		"Prints information on ASPRS LAS files (versions 1.0, 1.1 and 1.2) "
		"using the \"libLAS\" library. "
		"Get more information about this library at "
		"<a href=\"http://liblas.org\">http://liblas.org</a>\n"
		"Implementation of this module builds upon the lasinfo tool of "
		"<a href=\"http://www.cs.unc.edu/~isenburg/lastools\">Martin Isenburg</a>.\n"
		"\n"
		"\nliblas version: "
	));

	Description	+= CSG_String(LIBLAS_RELEASE_NAME);

	Set_Description	(Description);

	//-----------------------------------------------------
	Parameters.Add_FilePath(
		NULL	, "FILE"		, _TL("LAS File"),
		_TL(""),
		_TL("LAS Files (*.las)|*.las|All Files|*.*")
	);

	Parameters.Add_Value(
		NULL	, "HEADER"		, _TL("Only Header Info"),
		_TL("Print only information available in LAS header."),
		PARAMETER_TYPE_Bool,
		false
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLAS_Info::On_Execute(void)
{
	CSG_String		fName;
	bool			bHeader;
    std::ifstream   ifs;
	LASPointSummary summary;

	//-----------------------------------------------------
	fName		= Parameters("FILE")->asString();
	bHeader		= Parameters("HEADER")->asBool();
	

	//-----------------------------------------------------
    ifs.open(fName.b_str(), std::ios::in | std::ios::binary);
    if (!ifs)
    {
        SG_UI_Msg_Add_Error(CSG_String::Format(_TL("Unable to open LAS file!")));
        return (false);
    }

	//-----------------------------------------------------
	// Check if LAS version is supported
	liblas::LASReader *pReader;
	try {
		pReader = new liblas::LASReader(ifs);
	}
	catch(std::exception &e) {
		SG_UI_Msg_Add_Error(CSG_String::Format(_TL("LAS header exception: %s"), e.what()));
		ifs.close();
        return (false);
	}
	catch(...) {
		SG_UI_Msg_Add_Error(CSG_String::Format(_TL("Unknown LAS header exception!")));
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

	SG_UI_Msg_Add(SG_T(""), true);
	SG_UI_Msg_Add(SG_T("---------------------------------------------------------"), true);
	SG_UI_Msg_Add(_TL("  Header Summary"), true, SG_UI_MSG_STYLE_BOLD);
	SG_UI_Msg_Add(SG_T("---------------------------------------------------------"), true);

	SG_UI_Msg_Add(CSG_String::Format(_TL("  File Name:\t\t\t%s"), fName.c_str()), true);

	if (SG_STR_CMP(header.GetFileSignature().c_str(), SG_T("LASF")))
	{
		SG_UI_Msg_Add_Error(_TL("File signature is not 'LASF'!"));
		return (false);
	}

	SG_UI_Msg_Add(CSG_String::Format(_TL("  Version:\t\t\t%d.%d"),
									header.GetVersionMajor(),
									header.GetVersionMinor()), true);

	SG_UI_Msg_Add(CSG_String::Format(_TL("  Source ID:\t\t\t%d"),
									header.GetFileSourceId()), true);

	SG_UI_Msg_Add(CSG_String::Format(_TL("  Reserved:\t\t\t%d"),
									header.GetReserved()), true);

	SG_UI_Msg_Add(CSG_String::Format(_TL("  Project ID/GUID:\t\t'%s'"),
									CSG_String(header.GetProjectId().to_string().c_str()).w_str()), true);

	SG_UI_Msg_Add(CSG_String::Format(_TL("  System Identifier:\t\t'%s'"),
									CSG_String(header.GetSystemId().c_str()).w_str()), true);

	SG_UI_Msg_Add(CSG_String::Format(_TL("  Generating Software:\t\t'%s'"),
									CSG_String(header.GetSoftwareId().c_str()).w_str()), true);

	SG_UI_Msg_Add(CSG_String::Format(_TL("  File Creation Day/Year:\t\t%d/%d"),
									header.GetCreationDOY(),
									header.GetCreationYear()), true);

	SG_UI_Msg_Add(CSG_String::Format(_TL("  Header Size:\t\t\t%d"),
									header.GetHeaderSize()), true);

	SG_UI_Msg_Add(CSG_String::Format(_TL("  Offset to Point Data:\t\t%d"),
									header.GetDataOffset()), true);

	SG_UI_Msg_Add(CSG_String::Format(_TL("  Number Var. Length Records:\t%d"),
									header.GetRecordsCount()), true);

	SG_UI_Msg_Add(CSG_String::Format(_TL("  Point Data Format:\t\t%d"),
									header.GetDataFormatId()), true);

	SG_UI_Msg_Add(CSG_String::Format(_TL("  Point Data Record Length:\t%d"),
									header.GetDataRecordLength()), true);

	SG_UI_Msg_Add(CSG_String::Format(_TL("  Number of Point Records:\t%d"),
									header.GetPointRecordsCount()), true);

	SG_UI_Msg_Add(CSG_String::Format(_TL("  Number of Points by Return:\t%d  %d  %d  %d  %d"),
									header.GetPointRecordsByReturnCount().at(0),
									header.GetPointRecordsByReturnCount().at(1),
									header.GetPointRecordsByReturnCount().at(2),
									header.GetPointRecordsByReturnCount().at(3),
									header.GetPointRecordsByReturnCount().at(4)), true);

	SG_UI_Msg_Add(CSG_String::Format(_TL("  Scale Factor X Y Z:\t\t%.6g  %.6g  %.6g"),
									header.GetScaleX(),
									header.GetScaleY(),
									header.GetScaleZ()), true);

	SG_UI_Msg_Add(CSG_String::Format(_TL("  Offset X Y Z:\t\t\t%.6f  %.6f  %.6f"),
									header.GetOffsetX(),
									header.GetOffsetY(),
									header.GetOffsetZ()), true);

	SG_UI_Msg_Add(CSG_String::Format(_TL("  Min X Y Z:\t\t\t%.6f  %.6f  %.6f"),
									header.GetMinX(),
									header.GetMinY(),
									header.GetMinZ()), true);

	SG_UI_Msg_Add(CSG_String::Format(_TL("  Max X Y Z:\t\t\t%.6f  %.6f  %.6f"),
									header.GetMaxX(),
									header.GetMaxY(),
									header.GetMaxZ()), true);

	SG_UI_Msg_Add(CSG_String::Format(_TL("  Spatial Reference:\t\t%s"),
									CSG_String(header.GetSRS().GetProj4().c_str()).w_str()), true);

	return (true);
}


//---------------------------------------------------------
bool CLAS_Info::Print_Point_Summary(liblas::LASHeader header, LASPointSummary *pSummary)
{
	long rgpsum		= 0;
	long pbretsum	= 0;
	int	i			= 0;

	if( pSummary->number_of_point_records == 0 )
	{
		SG_UI_Msg_Add_Error(_TL("Point summary contains no points!"));
		return (false);
	}

	SG_UI_Msg_Add(SG_T(""), true);
	SG_UI_Msg_Add(SG_T("---------------------------------------------------------"), true);
	SG_UI_Msg_Add(_TL("  Point Inspection Summary"), true, SG_UI_MSG_STYLE_BOLD);
	SG_UI_Msg_Add(SG_T("---------------------------------------------------------"), true);

	SG_UI_Msg_Add(CSG_String::Format(_TL("   Header Point Count:\t\t%d"),
									header.GetPointRecordsCount()), true);

	SG_UI_Msg_Add(CSG_String::Format(_TL("   Actual Point Count:\t\t%d"),
									pSummary->number_of_point_records), true);


	SG_UI_Msg_Add(SG_T(""), true);
	SG_UI_Msg_Add(_TL("  Minimum and Maximum Attributes (min, max)"), true, SG_UI_MSG_STYLE_BOLD);
	SG_UI_Msg_Add(SG_T("---------------------------------------------------------"), true);

	SG_UI_Msg_Add(CSG_String::Format(_TL("  Min X Y Z:\t\t\t%.6f  %.6f  %.6f"),
									pSummary->pmin.GetX(),
									pSummary->pmin.GetY(),
									pSummary->pmin.GetZ()), true);

	SG_UI_Msg_Add(CSG_String::Format(_TL("  Max X Y Z:\t\t\t%.6f  %.6f  %.6f"),
									pSummary->pmax.GetX(),
									pSummary->pmax.GetY(),
									pSummary->pmax.GetZ()), true);

	SG_UI_Msg_Add(CSG_String::Format(_TL("  Bounding Box:\t\t\t%.2f, %.2f, %.2f, %.2f"),
									pSummary->pmin.GetX(),
									pSummary->pmin.GetY(),
									pSummary->pmax.GetX(),
									pSummary->pmax.GetY()), true);

	SG_UI_Msg_Add(CSG_String::Format(_TL("  Time:\t\t\t\t%.6f, %.6f"),
									pSummary->pmin.GetTime(),
									pSummary->pmax.GetTime()), true);

	SG_UI_Msg_Add(CSG_String::Format(_TL("  Return Number:\t\t%d, %d"),
									pSummary->pmin.GetReturnNumber(),
									pSummary->pmax.GetReturnNumber()), true);

	SG_UI_Msg_Add(CSG_String::Format(_TL("  Return Count:\t\t\t%d, %d"),
									pSummary->pmin.GetNumberOfReturns(),
									pSummary->pmax.GetNumberOfReturns()), true);

	SG_UI_Msg_Add(CSG_String::Format(_TL("  Flightline Edge:\t\t\t%d, %d"),
									pSummary->pmin.GetFlightLineEdge(),
									pSummary->pmax.GetFlightLineEdge()), true);

	SG_UI_Msg_Add(CSG_String::Format(_TL("  Intensity:\t\t\t%d, %d"),
									pSummary->pmin.GetIntensity(),
									pSummary->pmax.GetIntensity()), true);

	SG_UI_Msg_Add(CSG_String::Format(_TL("  Scan Direction Flag:\t\t%d, %d"),
									pSummary->pmin.GetScanDirection(),
									pSummary->pmax.GetScanDirection()), true);

	SG_UI_Msg_Add(CSG_String::Format(_TL("  Scan Angle Rank:\t\t%d, %d"),
									pSummary->pmin.GetScanAngleRank(),
									pSummary->pmax.GetScanAngleRank()), true);

	SG_UI_Msg_Add(CSG_String::Format(_TL("  Classification:\t\t\t%d, %d"),
									pSummary->pmin.GetClassification(),
									pSummary->pmax.GetClassification()), true);

	SG_UI_Msg_Add(CSG_String::Format(_TL("  Point Source Id:\t\t%d, %d"),
									pSummary->pmin.GetPointSourceID(),
									pSummary->pmax.GetPointSourceID()), true);

	SG_UI_Msg_Add(CSG_String::Format(_TL("  Minimum Color:\t\t\t%d %d %d"),
									pSummary->pmin.GetColor().GetRed(),
									pSummary->pmin.GetColor().GetGreen(),
									pSummary->pmin.GetColor().GetBlue()), true);

	SG_UI_Msg_Add(CSG_String::Format(_TL("  Maximum Color:\t\t%d %d %d"),
									pSummary->pmax.GetColor().GetRed(),
									pSummary->pmax.GetColor().GetGreen(),
									pSummary->pmax.GetColor().GetBlue()), true);

	
	SG_UI_Msg_Add(SG_T(""), true);
	SG_UI_Msg_Add(_TL("  Number of Points by Return"), true, SG_UI_MSG_STYLE_BOLD);
	SG_UI_Msg_Add(SG_T("---------------------------------------------------------"), true);

	for( i=0; i<5; i++ )
	{
		pbretsum = pbretsum + pSummary->number_of_points_by_return[i];
		SG_UI_Msg_Add(CSG_String::Format(SG_T("\t(%d) %d"),
										i,
										pSummary->number_of_points_by_return[i]), true);
	}
	SG_UI_Msg_Add(CSG_String::Format(_TL("  Total Points:\t%ld"),
									pbretsum), true);


	SG_UI_Msg_Add(SG_T(""), true);
	SG_UI_Msg_Add(_TL("  Number of Returns by Pulse"), true, SG_UI_MSG_STYLE_BOLD);
	SG_UI_Msg_Add(SG_T("---------------------------------------------------------"), true);

	for( i=0; i<8; i++ )
	{
		rgpsum = rgpsum + pSummary->number_of_returns_of_given_pulse[i];
		SG_UI_Msg_Add(CSG_String::Format(SG_T("\t(%d) %d"),
										i,
										pSummary->number_of_returns_of_given_pulse[i]), true);
	}
	SG_UI_Msg_Add(CSG_String::Format(_TL("  Total Pulses:\t%ld"),
									rgpsum), true);


	for( i=0; i<5; i++ )
	{
		if( header.GetPointRecordsByReturnCount().at(i) != pSummary->number_of_points_by_return[i] )
		{
			SG_UI_Msg_Add(CSG_String::Format(_TL("  Actual number of points by return is different from header (actual, header):")), true);
			for( int j=0; i<5; i++ )
			{
				SG_UI_Msg_Add(CSG_String::Format(SG_T("\t(%d, %d"),
												pSummary->number_of_points_by_return[j],
												header.GetPointRecordsByReturnCount().at(j)), true);
			}
		}
	}


	SG_UI_Msg_Add(SG_T(""), true);
	SG_UI_Msg_Add(_TL("  Point Classifications"), true, SG_UI_MSG_STYLE_BOLD);
	SG_UI_Msg_Add(SG_T("---------------------------------------------------------"), true);

	for( i=0; i<32; i++ )
	{
		if( pSummary->classification[i] )
		{
			SG_UI_Msg_Add(CSG_String::Format(SG_T("\t%d\t\t%s (%d)"),
											pSummary->classification[i],
											gLASPointClassification_Key_Name[i],
											i), true);
		}
	}

	if( pSummary->classification_synthetic || pSummary->classification_keypoint || pSummary->classification_withheld )
	{
		SG_UI_Msg_Add(SG_T(""), true);
		SG_UI_Msg_Add(_TL("  Point Classification Histogram"), true, SG_UI_MSG_STYLE_BOLD);
		SG_UI_Msg_Add(SG_T("---------------------------------------------------------"), true);

		if( pSummary->classification_synthetic )
			SG_UI_Msg_Add(CSG_String::Format(_TL("  +-> flagged as synthetic:\t%d"),
											pSummary->classification_synthetic), true);
		if( pSummary->classification_keypoint )
			SG_UI_Msg_Add(CSG_String::Format(_TL("  +-> flagged as keypoints:\t%d"),
											pSummary->classification_keypoint), true);
		if( pSummary->classification_withheld )
			SG_UI_Msg_Add(CSG_String::Format(_TL("  +-> flagged as withheld:\t%d"),
											pSummary->classification_withheld), true);
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

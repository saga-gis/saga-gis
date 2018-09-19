/**********************************************************
 * Version $Id: las_export.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

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
//                     las_export.cpp                    //
//                                                       //
//                 Copyright (C) 2010 by                 //
//                    Volker Wichmann                    //
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
#include "las_export.h"

#include <liblas/laspoint.hpp>
#include <liblas/laswriter.hpp>
#include <liblas/capi/las_version.h>
#include <fstream>
#include <iostream>
#include <cstddef>


//---------------------------------------------------------
#define	MAX_NUM_RETURN	5

//---------------------------------------------------------

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CLAS_Export::CLAS_Export(void)
{
	CSG_Parameter	*pNodeAttr;

	//-----------------------------------------------------
	// 1. Info...

	Set_Name		(_TL("Export LAS Files"));

	Set_Author		(SG_T("Volker Wichmann (c) 2010, LASERDATA GmbH"));

	CSG_String		Description(_TW(
		"This tool exports a SAGA Point Cloud as ASPRS LAS file (version 1.2) "
		"using the \"libLAS\" library. It allows writing Point Data Record Formats 1 to 4. "
		"Please have a look at the <a href=\"http://www.asprs.org/society/committees/standards/lidar_exchange_format.html\""
		">ASPRS LAS 1.2 Format Specification</a> for "
		"more information on the attributes supported with each format.\n"
		"Get more information about the \"libLAS\" library at "
		"<a href=\"http://liblas.org\">http://liblas.org</a>\n"
		"Please note, that the liblas writer does an \"isValid()\" check on each point. "
		"This might cause problems with MLS data because of the included scan angle check. "
		"MLS data ususally includes larger scan angles than ALS data and thus points may fail the "
		"test. Please always have a look at the summary printed in the message window.\n\n"
		"\nliblas version: "
	));

	Description	+= CSG_String(LIBLAS_RELEASE_NAME);

	Set_Description	(Description);

	//-----------------------------------------------------
	// 2. Parameters...

	pNodeAttr = Parameters.Add_PointCloud(
		NULL	, "POINTS"		, _TL("Point Cloud"),
		_TL("The point cloud to export."),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNodeAttr	, "T"    , _TL("gps-time"),
		_TL(""), true
	);
	Parameters.Add_Table_Field(
		pNodeAttr	, "i"    , _TL("intensity"),
		_TL(""), true
	);
	Parameters.Add_Table_Field(
		pNodeAttr	, "a"    , _TL("scan angle"),
		_TL(""), true
	);
	Parameters.Add_Table_Field(
		pNodeAttr	, "r"    , _TL("number of the return"),
		_TL(""), true
	);
	Parameters.Add_Table_Field(
		pNodeAttr	, "n"    , _TL("number of returns of given pulse"),
		_TL(""), true
	);
	Parameters.Add_Table_Field(
		pNodeAttr	, "c"    , _TL("classification"),
		_TL(""), true
	);
	Parameters.Add_Table_Field(
		pNodeAttr	, "u"    , _TL("user data"),
		_TL(""), true
	);
	Parameters.Add_Table_Field(
		pNodeAttr	, "R"    , _TL("red channel color"),
		_TL(""), true
	);
	Parameters.Add_Table_Field(
		pNodeAttr	, "G"    , _TL("green channel color"),
		_TL(""), true
	);
	Parameters.Add_Table_Field(
		pNodeAttr	, "B"    , _TL("blue channel color"),
		_TL(""), true
	);
	Parameters.Add_Table_Field(
		pNodeAttr	, "e"    , _TL("edge of flight line flag"),
		_TL(""), true
	);
	Parameters.Add_Table_Field(
		pNodeAttr	, "d"    , _TL("direction of scan flag"),
		_TL(""), true
	);
	Parameters.Add_Table_Field(
		pNodeAttr	, "p"    , _TL("point source ID"),
		_TL(""), true
	);
	Parameters.Add_Table_Field(
		pNodeAttr	, "RGB"    , _TL("SAGA RGB color"),
		_TL(""), true
	);

	Parameters.Add_Value(
		NULL	, "OFF_X"	, _TL("Offset X"),
		_TL(""),
		PARAMETER_TYPE_Double, 0.0
	);
	Parameters.Add_Value(
		NULL	, "OFF_Y"	, _TL("Offset Y"),
		_TL(""),
		PARAMETER_TYPE_Double, 0.0
	);
	Parameters.Add_Value(
		NULL	, "OFF_Z"	, _TL("Offset Z"),
		_TL(""),
		PARAMETER_TYPE_Double, 0.0
	);
	Parameters.Add_Value(
		NULL	, "SCALE_X"	, _TL("Scale X"),
		_TL(""),
		PARAMETER_TYPE_Double, 0.001
	);
	Parameters.Add_Value(
		NULL	, "SCALE_Y"	, _TL("Scale Y"),
		_TL(""),
		PARAMETER_TYPE_Double, 0.001
	);
	Parameters.Add_Value(
		NULL	, "SCALE_Z"	, _TL("Scale Z"),
		_TL(""),
		PARAMETER_TYPE_Double, 0.001
	);
	Parameters.Add_Choice(
		NULL, "FORMAT", _TL("Point Data Record Format"),
        _TL("Choose the Point Data Record Format you like to use"),
        CSG_String::Format(SG_T("%s|%s|%s|%s|"),
            _TL("0"),
            _TL("1"),
			_TL("2"),
			_TL("3")
        ), 3
    );

	Parameters.Add_FilePath(
		NULL	, "FILE"		, _TL("Output File"),
		_TL("The LAS output file."),
		_TL("LAS Files (*.las)|*.las|All Files|*.*"),
		NULL, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLAS_Export::On_Execute(void)
{
	CSG_PointCloud		*pPoints;
	CSG_String			fName;
	int					i_T, i_i, i_a, i_r, i_n, i_c, i_u, i_R, i_G, i_B, i_e, i_d, i_p, i_RGB;
	double				off_X, off_Y, off_Z;
	double				scale_X, scale_Y, scale_Z;
	int					format;
	int					cntRead = 0, cntWrite = 0;
	double				x, y, z, xmin, ymin, zmin, xmax, ymax, zmax;
	liblas::uint32_t	binPulse[MAX_NUM_RETURN];
	liblas::uint16_t	r;

    std::ofstream		ofs;


	//-----------------------------------------------------

	pPoints		= Parameters("POINTS")->asPointCloud();
	i_T			= Parameters("T")->asInt();
	i_i			= Parameters("i")->asInt();
	i_a			= Parameters("a")->asInt();
	i_r			= Parameters("r")->asInt();
	i_n			= Parameters("n")->asInt();
	i_c			= Parameters("c")->asInt();
	i_u			= Parameters("u")->asInt();
	i_R			= Parameters("R")->asInt();
	i_G			= Parameters("G")->asInt();
	i_B			= Parameters("B")->asInt();
	i_e			= Parameters("e")->asInt();
	i_d			= Parameters("d")->asInt();
	i_p			= Parameters("p")->asInt();
	i_RGB		= Parameters("RGB")->asInt();

	off_X		= Parameters("OFF_X")->asDouble();
	off_Y		= Parameters("OFF_Y")->asDouble();
	off_Z		= Parameters("OFF_Z")->asDouble();
	scale_X		= Parameters("SCALE_X")->asDouble();
	scale_Y		= Parameters("SCALE_Y")->asDouble();
	scale_Z		= Parameters("SCALE_Z")->asDouble();
	format		= Parameters("FORMAT")->asInt();

	fName		= Parameters("FILE")->asString();

    ofs.open(fName.b_str(), std::ios::out | std::ios::binary);
    if (!ofs)
    {
        SG_UI_Msg_Add_Error(CSG_String::Format(_TL("Unable to open LAS file!")));
        return (false);
    }

	//-----------------------------------------------------
	liblas::LASHeader	header;

	switch (format)
	{
	case 0:		header.SetDataFormatId(liblas::LASHeader::ePointFormat0);	break;
	case 1:		header.SetDataFormatId(liblas::LASHeader::ePointFormat1);	break;
	case 2:		header.SetDataFormatId(liblas::LASHeader::ePointFormat2);	break;
	case 3:
	default:	header.SetDataFormatId(liblas::LASHeader::ePointFormat3);	break;
	}

	header.SetOffset(off_X, off_Y, off_Z);
	header.SetScale(scale_X, scale_Y, scale_Z);

	// Get_ZMin() and Get_ZMax() do currently not work:
	/*header.SetMin(	(pPoints->Get_Extent().Get_XMin() - off_X) / scale_X,
					(pPoints->Get_Extent().Get_YMin() - off_Y) / scale_Y,
					(pPoints->Get_ZMin() - off_Z) / scale_Z);
	header.SetMax(	(pPoints->Get_Extent().Get_XMax() - off_X) / scale_X,
					(pPoints->Get_Extent().Get_YMax() - off_Y) / scale_Y,
					(pPoints->Get_ZMax() - off_Z) / scale_Z);*/

    liblas::LASWriter writer(ofs, header);

	if( pPoints->Get_Count() > 0 )
	{
		xmin = xmax = pPoints->Get_X(0);
		ymin = ymax = pPoints->Get_Y(0);
		zmin = zmax = pPoints->Get_Z(0);
	}

	for( int i=0; i<MAX_NUM_RETURN; i++ )
	{
		binPulse[i] = 0;
	}


	for( int i=0; i<pPoints->Get_Count(); i++ )
	{
		if (i % 100000 == 0)
			SG_UI_Process_Set_Progress(i, pPoints->Get_Count());

		cntRead++;

		liblas::LASPoint	point;

		x	= pPoints->Get_X(i);
		y	= pPoints->Get_Y(i);
		z	= pPoints->Get_Z(i);

		if( x < xmin ) xmin = x;
		if( x > xmax ) xmax = x;
		if( y < ymin ) ymin = y;
		if( y > ymax ) ymax = y;
		if( z < zmin ) zmin = z;
		if( z > zmax ) zmax = z;

		point.SetCoordinates(x, y, z);

		if( i_T > -1 )
			point.SetTime(pPoints->Get_Value(i, i_T));
		if( i_i > -1 )
			point.SetIntensity((liblas::uint16_t)pPoints->Get_Value(i, i_i));
		if( i_a > -1 )
			point.SetScanAngleRank((liblas::int8_t)pPoints->Get_Value(i, i_a));
		if( i_r > -1 )
		{
			r = (liblas::uint16_t)pPoints->Get_Value(i, i_r);
			point.SetReturnNumber(r);

			if( r > 0 && r <= MAX_NUM_RETURN )
				binPulse[r-1] += 1;
		}
		if( i_n > -1 )
			point.SetNumberOfReturns((liblas::uint16_t)pPoints->Get_Value(i, i_n));
		if( i_c > -1 )
			point.SetClassification((liblas::int8_t)pPoints->Get_Value(i, i_c));
		if( i_u > -1 )
			point.SetUserData((liblas::int8_t)pPoints->Get_Value(i, i_u));
		if( i_R > -1 && i_G > -1 && i_B > -1 )
		{
			liblas::LASColor	color;
			color.SetRed((liblas::uint16_t)pPoints->Get_Value(i, i_R));
			color.SetGreen((liblas::uint16_t)pPoints->Get_Value(i, i_G));
			color.SetBlue((liblas::uint16_t)pPoints->Get_Value(i, i_B));
			point.SetColor(color);
		}
		else if( i_RGB > -1 )
		{
			liblas::LASColor	color;
			color.SetRed((liblas::uint16_t)SG_GET_R((int)pPoints->Get_Value(i, i_RGB)));
			color.SetGreen((liblas::uint16_t)SG_GET_G((int)pPoints->Get_Value(i, i_RGB)));
			color.SetBlue((liblas::uint16_t)SG_GET_B((int)pPoints->Get_Value(i, i_RGB)));
			point.SetColor(color);
		}
		if( i_e > -1 )
			point.SetFlightLineEdge((liblas::uint16_t)pPoints->Get_Value(i, i_e));
		if( i_d > -1 )
			point.SetScanDirection((liblas::uint16_t)pPoints->Get_Value(i, i_d));
		if( i_p > -1 )
			point.SetPointSourceID((liblas::uint16_t)pPoints->Get_Value(i, i_p));

		if( writer.WritePoint(point) )
			cntWrite++;
	}


	header.SetMin(xmin, ymin, zmin);
	header.SetMax(xmax, ymax, zmax);

	SG_UI_Msg_Add(_TL("Summary:\n"), true);

	if( i_r > -1 )
	{
		SG_UI_Msg_Add(_TL("Number of points per return:"), true);
		for( int i=0; i<MAX_NUM_RETURN; i++ )
		{
			header.SetPointRecordsByReturnCount(i, binPulse[i]);
			SG_UI_Msg_Add(CSG_String::Format(SG_T("%s %d:\t\t%d %s"), _TL("return"), i + 1, binPulse[i], _TL("points")), true);
		}

		header.SetPointRecordsCount(cntWrite);
		SG_UI_Msg_Add(_TL(""), true);
	}

	writer.WriteHeader(header);

	//-----------------------------------------------------
	SG_UI_Msg_Add(CSG_String::Format(SG_T("%s:\t%d"), _TL("points read")   , cntRead ), true);
	SG_UI_Msg_Add(CSG_String::Format(SG_T("%s:\t%d"), _TL("points written"), cntWrite), true);

	return( true );
}


//---------------------------------------------------------
int CLAS_Export::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if (pParameter->Cmp_Identifier(SG_T("POINTS")))		// set attribute field choices to - NOT SET -
	{
		if (pParameters->Get_Parameter("POINTS")->asPointCloud() != NULL)
		{
			int	cntFields = pParameters->Get_Parameter("POINTS")->asPointCloud()->Get_Field_Count();

			pParameters->Get_Parameter("T")->Set_Value(cntFields);
			pParameters->Get_Parameter("i")->Set_Value(cntFields);
			pParameters->Get_Parameter("a")->Set_Value(cntFields);
			pParameters->Get_Parameter("r")->Set_Value(cntFields);
			pParameters->Get_Parameter("n")->Set_Value(cntFields);
			pParameters->Get_Parameter("c")->Set_Value(cntFields);
			pParameters->Get_Parameter("u")->Set_Value(cntFields);
			pParameters->Get_Parameter("R")->Set_Value(cntFields);
			pParameters->Get_Parameter("G")->Set_Value(cntFields);
			pParameters->Get_Parameter("B")->Set_Value(cntFields);
			pParameters->Get_Parameter("e")->Set_Value(cntFields);
			pParameters->Get_Parameter("d")->Set_Value(cntFields);
			pParameters->Get_Parameter("p")->Set_Value(cntFields);
			pParameters->Get_Parameter("RGB")->Set_Value(cntFields);
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

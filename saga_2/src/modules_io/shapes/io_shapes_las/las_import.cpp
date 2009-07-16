
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
//                     las_import.cpp                    //
//                                                       //
//                 Copyright (C) 2009 by                 //
//                      Olaf Conrad                      //
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
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "las_import.h"

#include <liblas/capi/liblas.h>


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
//	VAR_p,			// point source ID
//	VAR_C,			// colour
	VAR_Count
};

//---------------------------------------------------------
#define	ADD_FIELD(id, var, name, type)	if( Parameters(id)->asBool() ) { iField[var] = nFields++; pPoints->Add_Field(name, type); } else { iField[var] = -1; }


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CLAS_Import::CLAS_Import(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	// 1. Info...

	Set_Name		(_TL("Import LAS Files"));

	Set_Author		(SG_T("O. Conrad (c) 2009"));

	Set_Description	(_TW(
		"This module imports LAS files using the free \"libLAS\" library. "
		"Get more information about this library at "
		"<a href=\"http://liblas.org\">http://liblas.org</a>\n"
		"\n"
		"Featured by "
		"<a href=\"http://www.laserdata.at\">http://Laserdata GmbH, Austria</a>\n"
		"and in cooperation with "
		"<a href=\"http://www.alp-s.at\">http://alpS GmbH, Austria</a>\n"
	));


	//-----------------------------------------------------
	// 2. Parameters...

	Parameters.Add_PointCloud_Output(
		NULL	, "POINTS"		, _TL("Point Cloud"),
		_TL("")
	);

	Parameters.Add_FilePath(
		NULL	, "FILE"		, _TL("File"),
		_TL(""),
		_TL("LAS Files (*.las)|*.las|All Files|*.*")
	);

	pNode	= Parameters.Add_Node(
		NULL	, "NODE_VARS"	, _TL("Select Data"),
		_TL("")
	);

	Parameters.Add_Value(pNode, "T", _TL("gps-time")						, _TL(""), PARAMETER_TYPE_Bool, false);
	Parameters.Add_Value(pNode, "i", _TL("intensity")						, _TL(""), PARAMETER_TYPE_Bool, false);
	Parameters.Add_Value(pNode, "a", _TL("scan angle")						, _TL(""), PARAMETER_TYPE_Bool, false);
	Parameters.Add_Value(pNode, "r", _TL("number of the return")			, _TL(""), PARAMETER_TYPE_Bool, false);
	Parameters.Add_Value(pNode, "c", _TL("classification")					, _TL(""), PARAMETER_TYPE_Bool, false);
	Parameters.Add_Value(pNode, "u", _TL("user data")						, _TL(""), PARAMETER_TYPE_Bool, false);
	Parameters.Add_Value(pNode, "n", _TL("number of returns of given pulse"), _TL(""), PARAMETER_TYPE_Bool, false);
	Parameters.Add_Value(pNode, "R", _TL("red channel color")				, _TL(""), PARAMETER_TYPE_Bool, false);
	Parameters.Add_Value(pNode, "G", _TL("green channel color")				, _TL(""), PARAMETER_TYPE_Bool, false);
	Parameters.Add_Value(pNode, "B", _TL("blue channel color")				, _TL(""), PARAMETER_TYPE_Bool, false);
	Parameters.Add_Value(pNode, "e", _TL("edge of flight line flag")		, _TL(""), PARAMETER_TYPE_Bool, false);
	Parameters.Add_Value(pNode, "d", _TL("direction of scan flag")			, _TL(""), PARAMETER_TYPE_Bool, false);
//	Parameters.Add_Value(pNode, "p", _TL("point source ID")					, _TL(""), PARAMETER_TYPE_Bool, false);
//	Parameters.Add_Value(pNode, "C", _TL("colour")							, _TL(""), PARAMETER_TYPE_Bool, false);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CLAS_Import::On_Execute(void)
{
	CSG_String	fName	= Parameters("FILE")->asString();

	//-----------------------------------------------------
	if( !SG_File_Exists(fName) )
	{
		return( false );
	}

	//-----------------------------------------------------
	LASReaderH	reader	= NULL;
	LASHeaderH	header	= NULL;
	LASPointH	point	= NULL;

	if( (reader = LASReader_Create(fName.b_str())) == NULL )
	{
		Error_Set(_TL("Unable to read file"));

		return( false );
	}

	if( (header = LASReader_GetHeader(reader)) == NULL )
	{
		LASReader_Destroy(reader);

		Error_Set(_TL("Unable to fetch header for file"));

		return( false );
	}

	//-----------------------------------------------------
	int		nFields, iField[VAR_Count];

	CSG_PointCloud	*pPoints	= SG_Create_PointCloud();
	pPoints->Set_Name(SG_File_Get_Name(fName, false));
	Parameters("POINTS")->Set_Value(pPoints);

	nFields		= 3;

	ADD_FIELD("T", VAR_T, _TL("gps-time")							, POINTCLOUD_FIELDTYPE_Long);
	ADD_FIELD("i", VAR_i, _TL("intensity")							, POINTCLOUD_FIELDTYPE_Float);
	ADD_FIELD("a", VAR_a, _TL("scan angle")							, POINTCLOUD_FIELDTYPE_Float);
	ADD_FIELD("r", VAR_r, _TL("number of the return")				, POINTCLOUD_FIELDTYPE_Int);
	ADD_FIELD("c", VAR_c, _TL("classification")						, POINTCLOUD_FIELDTYPE_Int);
	ADD_FIELD("u", VAR_u, _TL("user data")							, POINTCLOUD_FIELDTYPE_Double);
	ADD_FIELD("n", VAR_n, _TL("number of returns of given pulse")	, POINTCLOUD_FIELDTYPE_Int);
	ADD_FIELD("R", VAR_R, _TL("red channel color")					, POINTCLOUD_FIELDTYPE_Char);
	ADD_FIELD("G", VAR_G, _TL("green channel color")				, POINTCLOUD_FIELDTYPE_Char);
	ADD_FIELD("B", VAR_B, _TL("blue channel color")					, POINTCLOUD_FIELDTYPE_Char);
	ADD_FIELD("e", VAR_e, _TL("edge of flight line flag")			, POINTCLOUD_FIELDTYPE_Char);
	ADD_FIELD("d", VAR_d, _TL("direction of scan flag")				, POINTCLOUD_FIELDTYPE_Char);
//	ADD_FIELD("p", VAR_p, _TL("point source ID")					, POINTCLOUD_FIELDTYPE_Int);
//	ADD_FIELD("C", VAR_C, _TL("color")								, POINTCLOUD_FIELDTYPE_Long);

	//-----------------------------------------------------
	int		iPoint	= 0;

	while( (point = LASReader_GetNextPoint(reader)) != NULL )
	{
		if( LASPoint_IsValid(point) )
		{
			LASColorH color	= LASPoint_GetColor(point);

			pPoints->Add_Point(LASPoint_GetX(point), LASPoint_GetY(point), LASPoint_GetZ(point));

			if( iField[VAR_T] > 0 )	pPoints->Set_Value(iPoint, iField[VAR_T], LASPoint_GetTime(point));
			if( iField[VAR_i] > 0 )	pPoints->Set_Value(iPoint, iField[VAR_i], LASPoint_GetIntensity(point));
			if( iField[VAR_a] > 0 )	pPoints->Set_Value(iPoint, iField[VAR_a], LASPoint_GetScanAngleRank(point));
			if( iField[VAR_r] > 0 )	pPoints->Set_Value(iPoint, iField[VAR_r], LASPoint_GetReturnNumber(point));
			if( iField[VAR_c] > 0 )	pPoints->Set_Value(iPoint, iField[VAR_c], LASPoint_GetClassification(point));
			if( iField[VAR_u] > 0 )	pPoints->Set_Value(iPoint, iField[VAR_u], LASPoint_GetUserData(point));
			if( iField[VAR_n] > 0 )	pPoints->Set_Value(iPoint, iField[VAR_n], LASPoint_GetNumberOfReturns(point));
			if( iField[VAR_R] > 0 )	pPoints->Set_Value(iPoint, iField[VAR_R], LASColor_GetRed(color));
			if( iField[VAR_G] > 0 )	pPoints->Set_Value(iPoint, iField[VAR_G], LASColor_GetGreen(color));
			if( iField[VAR_B] > 0 )	pPoints->Set_Value(iPoint, iField[VAR_B], LASColor_GetBlue(color));
			if( iField[VAR_e] > 0 )	pPoints->Set_Value(iPoint, iField[VAR_e], LASPoint_GetFlightLineEdge(point));
			if( iField[VAR_d] > 0 )	pPoints->Set_Value(iPoint, iField[VAR_d], LASPoint_GetScanDirection(point));
//			if( iField[VAR_p] > 0 )	pPoints->Set_Value(iPoint, iField[VAR_p], reader->point.point_source_ID);
//			if( iField[VAR_C] > 0 )	pPoints->Set_Value(iPoint, iField[VAR_C], color);

			LASColor_Destroy(color);

			iPoint++;
		}
	}

	LASReader_Destroy(reader);
	LASHeader_Destroy(header);

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

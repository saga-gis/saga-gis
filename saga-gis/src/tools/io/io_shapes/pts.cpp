
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       io_shapes                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                        pts.cpp                        //
//                                                       //
//                 Copyright (C) 2019 by                 //
//                      Olaf Conrad                      //
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
#include "pts.h"


///////////////////////////////////////////////////////////
//														 //
//						Import							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPTS_Import::CPTS_Import(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Import Point Cloud from PTS File"));

	Set_Author		("O.Conrad (c) 2019");

	Set_Description	(_TW(
		"Imports point cloud data from a PTS file."
	));

	//-----------------------------------------------------
	Parameters.Add_FilePath("",
		"FILENAME"	, _TL("File"),
		_TL(""),
		CSG_String::Format("%s (*.pts)|*.pts|%s|*.*",
			_TL("pts Files"),
			_TL("All Files")
		), NULL, false
	);

	Parameters.Add_PointCloud("",
		"POINTS"	, _TL("Points"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice("",
		"RGB"		, _TL("Import RGB Values as..."),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("separate values"),
			_TL("single rgb-coded integer value")
		), 1
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPTS_Import::On_Execute(void)
{
	CSG_File	Stream;

	if( !Stream.Open(Parameters("FILENAME")->asString(), SG_FILE_R) )
	{
		Error_Set(_TL("file could not be opened"));

		return( false );
	}

	//-----------------------------------------------------
	int	RGB	= Parameters("RGB")->asInt();

	CSG_PointCloud	*pPoints	= Parameters("POINTS")->asPointCloud();

	pPoints->Destroy();

	pPoints->Set_Name(Stream.Get_File_Name());

	pPoints->Add_Field("INTENSITY", SG_DATATYPE_Short);

	if( RGB == 0 )
	{
		pPoints->Add_Field("R", SG_DATATYPE_Byte);
		pPoints->Add_Field("G", SG_DATATYPE_Byte);
		pPoints->Add_Field("B", SG_DATATYPE_Byte);
	}
	else
	{
		pPoints->Add_Field("RGB", SG_DATATYPE_DWord);
	}

	//-----------------------------------------------------
	CSG_String	sLine;

	int	nPoints;

	if( !Stream.Read_Line(sLine) || !sLine.asInt(nPoints) )
	{
		Error_Set(_TL("could not read headline"));

		return( false );
	}

	//-----------------------------------------------------
	for(int iPoint=0; iPoint<nPoints && !Stream.is_EOF() && Set_Progress(iPoint, nPoints); iPoint++)
	{
		if( Stream.Read_Line(sLine) )
		{
			double x, y, z; int i, r, g, b;

			CSG_Strings	s	= SG_String_Tokenize(sLine);

		//	if( sscanf(sLine.b_str(), "%lf %lf %lf %d %d %d %d", &x, &y, &z, &i, &r, &g, &b) == 7 )
			if( s.Get_Count() == 7
			&&  s[0].asDouble(x) && s[1].asDouble(y) && s[2].asDouble(z)
			&&  s[3].asInt(i) && s[4].asInt(r) && s[5].asInt(g) && s[6].asInt(b) )
			{
				pPoints->Add_Point(x, y, z);

				pPoints->Set_Attribute(0, i);

				if( RGB == 0 )
				{
					pPoints->Set_Attribute(1, r);
					pPoints->Set_Attribute(2, g);
					pPoints->Set_Attribute(3, b);
				}
				else
				{
					pPoints->Set_Attribute(1, SG_GET_RGB(r, g, b));
				}
			}
		}
	}

	//-----------------------------------------------------
	return( pPoints->Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

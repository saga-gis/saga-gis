
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     shapes_tools                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     WASP_MAP.cpp                      //
//                                                       //
//                 Copyright (C) 2006 by                 //
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
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "wasp_map.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWASP_MAP_Export::CWASP_MAP_Export(void)
{
	Set_Name		(_TL("Export WASP terrain map file"));

	Set_Author		(SG_T("(c) 2006 by O.Conrad"));

	Set_Description	(_TW(
		"Reference:\n"
		"<a href=\"http://www.risoe.dk/vea/projects/nimo/WAsPHelp/Wasp8.htm#FileFormatofMAP.htm\" target=\"_blank\">"
		"http://www.risoe.dk/vea/projects/nimo/WAsPHelp/Wasp8.htm#FileFormatofMAP.htm</a>"
	));

	//-----------------------------------------------------
	CSG_Parameter	*pShapes	= Parameters.Add_Shapes(
		NULL	, "SHAPES"		, _TL("Contour Lines"),
		_TL(""),
		PARAMETER_INPUT			, SHAPE_TYPE_Line
	);

	Parameters.Add_Table_Field(
		pShapes	, "ELEVATION"	, _TL("Map File"),
		_TL("")
	);

	Parameters.Add_FilePath(
		NULL	, "FILE"		, _TL("File Name"),
		_TL(""),
		CSG_String::Format(SG_T("%s|*.map|%s|*.*"),
			_TL("WASP Map Files (*.map)"),
			_TL("All Files")
		), NULL, true
	);
}

//---------------------------------------------------------
CWASP_MAP_Export::~CWASP_MAP_Export(void)
{}

//---------------------------------------------------------
bool CWASP_MAP_Export::On_Execute(void)
{
	int			zField;
	FILE		*Stream;
	CSG_String	fName;
	CSG_Shape	*pLine;
	CSG_Shapes	*pLines;

	//-----------------------------------------------------
	pLines	= Parameters("SHAPES")		->asShapes();
	zField	= Parameters("ELEVATION")	->asInt();
	fName	= Parameters("FILE")		->asString();

	//-----------------------------------------------------
	if( pLines && pLines->is_Valid() && (Stream = fopen(fName.b_str(), "w")) != NULL )
	{
		// 1)	Text string identifying the terrain map: + ...

		fprintf(Stream, "+ %s\n", pLines->Get_Name());


		// 2)	Fixed point #1 in user and metric [m] coordinates:
		//			X1(user) Y1(user) X1(metric) Y1(metric)

		fprintf(Stream, "%f %f %f %f\n", 0.0, 0.0, 0.0, 0.0);


		// 3)	Fixed point #2 in user and metric [m] coordinates:
		//			X2(user) Y2(user) X2(metric) Y2(metric)

		fprintf(Stream, "%f %f %f %f\n", 1.0, 1.0, 1.0, 1.0);


		// 4)	Scaling factor and offset for height scale (Z):
		//			Zmetric = {scaling factor}(Zuser + {offset})

		fprintf(Stream, "%f %f\n", 1.0, 0.0);


		for(int iLine=0; iLine<pLines->Get_Count() && Set_Progress(iLine, pLines->Get_Count()); iLine++)
		{
			pLine	= pLines->Get_Shape(iLine);

			for(int iPart=0; iPart<pLine->Get_Part_Count(); iPart++)
			{
				if( pLine->Get_Point_Count(iPart) > 1 )
				{
					// 5a)	Height contour: elevation (Z) and number of points (n) in line:
					//			Z n

					fprintf(Stream, "%f %d\n", pLine->asDouble(zField), pLine->Get_Point_Count(iPart));


					// 5b)	Roughness change line:
					//			roughness lengths to the left (z0l) and right (z0r) side of the line,
					//			respectively, and number of points:
					//				z0l z0r n

					// 5c)	Roughness and contour line:
					//			roughness lengths to the left and right of the line,
					//			respectively, elevation and number of points:
					//				z0l z0r Z n


					// 6–)	Cartesian coordinates (X, Y) of line described in 5a, 5b or 5c:
					//			X1 Y1 [... Xn Yn]
					//			Xn+1 Yn+1
					//			... where [] embrace optional numbers and n is > 0

					for(int iPoint=0; iPoint<pLine->Get_Point_Count(iPart); iPoint++)
					{
						TSG_Point	p	= pLine->Get_Point(iPoint, iPart);

						fprintf(Stream, "%f\t%f\n", p.x, p.y);
					}
				}
			}
		}

		fclose(Stream);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWASP_MAP_Import::CWASP_MAP_Import(void)
{
	Set_Name		(_TL("Import WASP terrain map file"));

	Set_Author		(SG_T("(c) 2006 by O.Conrad"));

	Set_Description	(_TW(
		"Reference:\n"
		"<a href=\"http://www.risoe.dk/vea/projects/nimo/WAsPHelp/Wasp8.htm#FileFormatofMAP.htm\" target=\"_blank\">"
		"http://www.risoe.dk/vea/projects/nimo/WAsPHelp/Wasp8.htm#FileFormatofMAP.htm</a>"
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes(
		NULL	, "SHAPES"		, _TL("Contour Lines"),
		_TL(""),
		PARAMETER_OUTPUT		, SHAPE_TYPE_Line
	);

	Parameters.Add_FilePath(
		NULL	, "FILE"		, _TL("File Name"),
		_TL(""),
		CSG_String::Format(SG_T("%s|*.map|%s|*.*"),
			_TL("WASP Map Files (*.map)"),
			_TL("All Files")
		), NULL, false
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Input Specification"),
		_TL(""),

		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("elevation"),
			_TL("roughness"),
			_TL("elevation and roughness")
		), 0
	);
}

//---------------------------------------------------------
CWASP_MAP_Import::~CWASP_MAP_Import(void)
{}

//---------------------------------------------------------
bool CWASP_MAP_Import::On_Execute(void)
{
	int			n, Method, nLength;
	double		z, dz, zMin, rLeft, rRight;
	FILE		*Stream;
	TSG_Point	p, pu[2], pm[2];
	CSG_String	fName, sLine;
	CSG_Shape	*pLine;
	CSG_Shapes	*pLines;

	//-----------------------------------------------------
	pLines	= Parameters("SHAPES")		->asShapes();
	fName	= Parameters("FILE")		->asString();
	Method	= Parameters("METHOD")		->asInt();

	//-----------------------------------------------------
	if( (Stream = fopen(fName.b_str(), "r")) != NULL )
	{
		fseek(Stream, 0, SEEK_END);
		nLength	= ftell(Stream);
		fseek(Stream, 0, SEEK_SET);

		pLines->Create(SHAPE_TYPE_Line, SG_File_Get_Name(fName, false));

		switch( Method )
		{
		case 0:	// elevation
			pLines->Add_Field("Z"		, TABLE_FIELDTYPE_Double);
			break;

		case 1:	// roughness
			pLines->Add_Field("RLEFT"	, TABLE_FIELDTYPE_Double);
			pLines->Add_Field("RRIGHT"	, TABLE_FIELDTYPE_Double);
			break;

		case 2:	// elevation and roughness
			pLines->Add_Field("Z"		, TABLE_FIELDTYPE_Double);
			pLines->Add_Field("RLEFT"	, TABLE_FIELDTYPE_Double);
			pLines->Add_Field("RRIGHT"	, TABLE_FIELDTYPE_Double);
			break;
		}


		// 1)	Text string identifying the terrain map: + ...

		SG_Read_Line(Stream, sLine);


		// 2)	Fixed point #1 in user and metric [m] coordinates:
		//			X1(user) Y1(user) X1(metric) Y1(metric)

		fscanf(Stream, "%lf %lf %lf %lf", &pu[0].x, &pu[0].y, &pm[0].x, &pm[0].y);


		// 3)	Fixed point #2 in user and metric [m] coordinates:
		//			X2(user) Y2(user) X2(metric) Y2(metric)

		fscanf(Stream, "%lf %lf %lf %lf", &pu[1].x, &pu[1].y, &pm[1].x, &pm[1].y);


		// 4)	Scaling factor and offset for height scale (Z):
		//			Zmetric = {scaling factor}(Zuser + {offset})

		fscanf(Stream, "%lf %lf", &dz, &zMin);


		while( !feof(Stream) && Set_Progress(ftell(Stream), nLength) )
		{
			pLine	= NULL;

			switch( Method )
			{
			case 0:	// elevation
				// 5a)	Height contour: elevation (Z) and number of points (n) in line:
				//			Z n

				fscanf(Stream, "%lf %d", &z, &n);

				if( !feof(Stream) && n > 1 )
				{
					pLine	= pLines->Add_Shape();
					pLine->Set_Value(0, zMin + dz * z);
				}
				break;

			case 1:	// roughness
				// 5b)	Roughness change line:
				//			roughness lengths to the left (z0l) and right (z0r) side of the line,
				//			respectively, and number of points:
				//				z0l z0r n

				fscanf(Stream, "%lf %lf %d", &rLeft, &rRight, &n);

				if( !feof(Stream) && n > 1 )
				{
					pLine	= pLines->Add_Shape();
					pLine->Set_Value(0, rLeft);
					pLine->Set_Value(1, rRight);
				}
				break;

			case 2:	// elevation and roughness
				// 5c)	Roughness and contour line:
				//			roughness lengths to the left and right of the line,
				//			respectively, elevation and number of points:
				//				z0l z0r Z n

				fscanf(Stream, "%lf %lf %lf %d", &rLeft, &rRight, &z, &n);

				if( !feof(Stream) && n > 1 )
				{
					pLine	= pLines->Add_Shape();
					pLine->Set_Value(0, zMin + dz * z);
					pLine->Set_Value(1, rLeft);
					pLine->Set_Value(2, rRight);
				}
				break;
			}


			// 6–)	Cartesian coordinates (X, Y) of line described in 5a, 5b or 5c:
			//			X1 Y1 [... Xn Yn]
			//			Xn+1 Yn+1
			//			... where [] embrace optional numbers and n is > 0

			for(int i=0; i<n && !feof(Stream) && Process_Get_Okay(false); i++)
			{
				fscanf(Stream, "%lf %lf", &p.x, &p.y);

				pLine->Add_Point(p);
			}
		}

		fclose(Stream);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

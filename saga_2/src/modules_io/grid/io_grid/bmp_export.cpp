
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//    System for an Automated Geo-Scientific Analysis    //
//                                                       //
//                    Module Library:                    //
//                        Grid_IO                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    bmp_export.cpp                     //
//                                                       //
//                 Copyright (C) 2005 by                 //
//                      Olaf Conrad                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for an Automated  //
// Geo-Scientific Analysis'. SAGA is free software; you  //
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
//    e-mail:     oconrad@gwdg.de                        //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
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
#include "bmp_export.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CBMP_Export::CBMP_Export(void)
{
	//-----------------------------------------------------
	Set_Name	(_TL("Export True Color Bitmap"));

	Set_Author	("Copyrights (c) 2005 by Olaf Conrad");

	Set_Description(
		"Export red-green-blue coded image grids to MS-Windows true color bitmaps. "
		"This module writes the data directly to the file and is hence particularly "
		"suitable for very large data sets. "
	);

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "IMAGE"	, _TL("Image Grid"),
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_FilePath(
		NULL	, "FILE"	, _TL("File"),
		"",
		_TL("MS Windows Bitmap (*.bmp)|*.bmp|All Files|*.*"),
		NULL, true, false
	);
}

//---------------------------------------------------------
CBMP_Export::~CBMP_Export(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define	SIZE_HEADER_FILE	14
#define	SIZE_HEADER_BITMAP	40
#define	SIZE_HEADER_TOTAL	(SIZE_HEADER_FILE + SIZE_HEADER_BITMAP)

//---------------------------------------------------------
bool CBMP_Export::On_Execute(void)
{
	//-----------------------------------------------------
	char		*Line, *pLine;
	int			x, y, Value, nBytes_Image, nBytes_Line;
	FILE		*Stream;
	CSG_Grid		*pGrid;
	CSG_String	FileName;

	//-----------------------------------------------------
	pGrid		= Parameters("IMAGE")	->asGrid();
	FileName	= Parameters("FILE")	->asString();

	//-----------------------------------------------------
	if( (Stream = fopen(FileName, "wb")) != NULL )
	{
		nBytes_Line		= 3 * sizeof(BYTE) * Get_NX();

		if( (Value = nBytes_Line % 4) != 0 )					// must be a multiple of DWORD size...
		{
			nBytes_Line	+= 4 - Value;
		}

		nBytes_Image	= nBytes_Line * Get_NY();

		//-------------------------------------------------
		Write_WORD	(Stream, 0x4d42);							// bfType      - 0x42 = "B", 0x4d = "M"...
		Write_DWORD	(Stream, SIZE_HEADER_TOTAL + nBytes_Image);	// bfSize      - size of the entire file...
		Write_WORD	(Stream, 0);								// bfReserved1
		Write_WORD	(Stream, 0);								// bfReserved2
		Write_DWORD	(Stream, SIZE_HEADER_TOTAL);				// bfOffBits   - offset to the array of color indices...

		//-------------------------------------------------
		Write_DWORD	(Stream, SIZE_HEADER_BITMAP);				// biSize;
		Write_LONG	(Stream, Get_NX());							// biWidth;
		Write_LONG	(Stream, Get_NY());							// biHeight;
		Write_WORD	(Stream, 1);								// biPlanes;
		Write_WORD	(Stream, 24);								// biBitCount;
		Write_DWORD	(Stream, 0);								// biCompression;
		Write_DWORD	(Stream, nBytes_Image);						// biSizeImage;
		Write_LONG	(Stream, 0);								// biXPelsPerMeter;
		Write_LONG	(Stream, 0);								// biYPelsPerMeter;
		Write_DWORD	(Stream, 2^24);								// biClrUsed;
		Write_DWORD	(Stream, 0);								// biClrImportant;

		//-------------------------------------------------
		Line	= (char *)SG_Calloc(nBytes_Line, sizeof(char));

		for(y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			for(x=0, pLine=Line; x<Get_NX(); x++, pLine+=3)
			{
				Value		= pGrid->asInt(x, y);

				pLine[0]	= SG_GET_B(Value);
				pLine[1]	= SG_GET_G(Value);
				pLine[2]	= SG_GET_R(Value);
			}

			fwrite(Line, sizeof(char), nBytes_Line, Stream);
		}

		SG_Free(Line);

		//-------------------------------------------------
		fclose(Stream);

		FileName	= SG_File_Make_Path(SG_File_Get_Path(FileName), SG_File_Get_Name(FileName, false), "bpw");

		if( (Stream = fopen(FileName, "w")) != NULL )
		{
			fprintf(Stream, "%f\n%f\n%f\n%f\n%f\n%f\n",
				 pGrid->Get_Cellsize(),
				 0.0, 0.0,
				-pGrid->Get_Cellsize(),
				 pGrid->Get_XMin(),
				 pGrid->Get_YMax()
			);

			fclose(Stream);
		}

		return( true );
	}

	//-----------------------------------------------------
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CBMP_Export::Write_WORD	(FILE *Stream, WORD  Value)
{
	fwrite(&Value, sizeof(Value), 1, Stream);
}

//---------------------------------------------------------
void CBMP_Export::Write_DWORD	(FILE *Stream, DWORD Value)
{
	fwrite(&Value, sizeof(Value), 1, Stream);
}

//---------------------------------------------------------
void CBMP_Export::Write_LONG	(FILE *Stream, long  Value)
{
	fwrite(&Value, sizeof(Value), 1, Stream);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

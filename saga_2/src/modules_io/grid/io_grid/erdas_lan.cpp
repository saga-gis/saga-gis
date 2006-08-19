
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
//                    erdas_lan.cpp                      //
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
#include <string.h>

#include "erdas_lan.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CErdas_LAN_Import::CErdas_LAN_Import(void)
{
	//-----------------------------------------------------
	Set_Name	("Import Erdas LAN/GIS");

	Set_Author	("Copyrights (c) 2005 by Olaf Conrad");

	Set_Description(
		"Import Erdas LAN/GIS files. \n"
		"The format analysis is based on the GRASS module i.in.erdas. "
		"Go to the " 
		"<a target=\"_blank\" href=\"http://grass.itc.it/\">GRASS GIS Hompage</a> "
		"for more information.\n"
	);

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL	, "GRIDS"	, "Grids",
		"",
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_FilePath(
		NULL	, "FILE"	, "File",
		"",
		"Erdas LAN/GIS files (*.lan, *.gis)|*.lan;*.gis|All Files|*.*",
		NULL, false, false
	);
}

//---------------------------------------------------------
CErdas_LAN_Import::~CErdas_LAN_Import(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define SWAP(v)		if( bSwap ) SG_Swap_Bytes(&v, sizeof(v));

//---------------------------------------------------------
bool CErdas_LAN_Import::On_Execute(void)
{
	//-----------------------------------------------------
	bool					bResult	= false, bSwap	= false;
	BYTE					*Line, *pLine;
	char					head[6], empty[56];
	short					vType, nBands, maptyp, nclass, unit;
	int						x, y, i, nLine;
	long					nx, ny, px, py;
	float					area, dx, dy, xPos, yPos;
	double					Cellsize, xMin, yMin, Value;
	FILE					*Stream;
	TGrid_Type				gType;
	CSG_Colors					Colors;
	CParameter_Grid_List	*Grids;
	CSG_String				FileName;

	//-----------------------------------------------------
	Grids		= Parameters("GRIDS")	->asGridList();
	FileName	= Parameters("FILE")	->asString();

	Grids->Del_Items();

	//-----------------------------------------------------
	if( (Stream = fopen(FileName, "rb")) != NULL )
	{
		fread( head		,  6, sizeof(char)	, Stream);					// "HEAD74"
		fread(&vType	,  1, sizeof(short)	, Stream);	SWAP(vType);	// 0 = 8 bit, 1 = 4 bit, 2 = 16 bit
		fread(&nBands	,  1, sizeof(short)	, Stream);	SWAP(nBands);	// number of bands
		fread( empty	,  6, sizeof(char)	, Stream);					// empty
		fread(&nx		,  1, sizeof(long)	, Stream);	SWAP(nx);		// number of columns
		fread(&ny		,  1, sizeof(long)	, Stream);	SWAP(ny);		// number of rows
		fread(&px		,  1, sizeof(long)	, Stream);	SWAP(px);		// x position of top-left pixel "database location"
		fread(&py		,  1, sizeof(long)	, Stream);	SWAP(py);		// y position of top-left pixel "database location"
		fread( empty	, 56, sizeof(char)	, Stream);					// empty
		fread(&maptyp	,  1, sizeof(short)	, Stream);	SWAP(maptyp);	// map projection
		fread(&nclass	,  1, sizeof(short)	, Stream);	SWAP(nclass);	// number of classes
		fread( empty	, 14, sizeof(char)	, Stream);					// empty
		fread(&unit		,  1, sizeof(short)	, Stream);	SWAP(unit);		// units of area per pixel - 0 = none, 1 = acre, 2 = hectare, 3 = other
		fread(&area		,  1, sizeof(float)	, Stream);	SWAP(area);		// area per pixel
		fread(&xPos		,  1, sizeof(float)	, Stream);	SWAP(xPos);		// x coordinate of top-left pixel
		fread(&yPos		,  1, sizeof(float)	, Stream);	SWAP(yPos);		// y coordinate of top-left pixel
		fread(&dx		,  1, sizeof(float)	, Stream);	SWAP(dx);		// linear size of a pixel (feet/degrees/meters)
		fread(&dy		,  1, sizeof(float)	, Stream);	SWAP(dy);		// linear size of a pixel (feet/degrees/meters)

		//-------------------------------------------------
		if( feof(Stream) )
		{
			Message_Add(_TL("File error"));
		}
		else if( strncmp(head, "HEAD74", sizeof(head)) )
		{
			Message_Add(_TL("ERDAS files other than version 7.4 are not supported."));
		}

		//-------------------------------------------------
		else
		{
			bResult		= true;
			Cellsize	= dx;
			xMin		= xPos;
			yMin		= yPos - nx * Cellsize;

			switch( vType )
			{
			case 0:	default:	//  8 bit
				gType	= GRID_TYPE_Byte;
				nLine	= nx;
				break;

			case 1:				//  4 bit
				gType	= GRID_TYPE_Byte;
				nLine	= nx / 2 + (nx % 2 ? 1 : 0);
				break;

			case 2:				// 16 bit
				gType	= GRID_TYPE_Word;
				nLine	= nx * 2;
				break;
			}

			Line	= (BYTE *)SG_Malloc(nLine * sizeof(BYTE));

			for(i=0; i<nBands; i++)
			{
				Grids->Add_Item(SG_Create_Grid(gType, nx, ny, Cellsize, xMin, yMin));
				Grids->asGrid(i)->Set_Name(CSG_String::Format("%s [%d]", SG_File_Get_Name(FileName, false).c_str(), i + 1));
			}

			//---------------------------------------------
			for(y=0; y<ny && Set_Progress(y, ny); y++)
			{
				for(i=0; i<nBands; i++)
				{
					fread(Line, nLine, sizeof(BYTE), Stream);

					for(x=0, pLine=Line; x<nx; x++)
					{
						switch( vType )
						{
						case 0: default:
							Value	= *pLine;
							pLine	++;
							break;

						case 1:
							if( x % 2 )
							{
								Value	= (*pLine & ~0x11) >> 4;
								pLine	++;
							}
							else
							{
								Value	= (*pLine &  0x11);
							}
							break;

						case 2:
							if( bSwap )
							{
								SG_Swap_Bytes(pLine, 2);
							}

							Value	= *((short *)pLine);
							pLine	+= 2;
							break;
						}

						Grids->asGrid(i)->Set_Value(x, ny - y - 1, Value);
					}
				}
			}

			//---------------------------------------------
			SG_Free(Line);

			Colors.Set_Palette(SG_COLORS_BLACK_WHITE);

			for(i=0; i<nBands; i++)
			{
				DataObject_Add			(Grids->asGrid(i));
				DataObject_Set_Colors	(Grids->asGrid(i), Colors);
			}
		}

		//-------------------------------------------------
		fclose(Stream);
	}

	//-----------------------------------------------------
	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

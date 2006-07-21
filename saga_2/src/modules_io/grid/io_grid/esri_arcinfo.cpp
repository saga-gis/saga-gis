
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                        Grid_IO                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   ESRI_ArcInfo.cpp                    //
//                                                       //
//                 Copyright (C) 2003 by                 //
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

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <string.h>

#include "esri_arcinfo.h"

//---------------------------------------------------------
#define HDR_NROWS		"NROWS"
#define HDR_NCOLS		"NCOLS"
#define HDR_X_CORNER	"XLLCORNER"
#define HDR_Y_CORNER	"YLLCORNER"
#define HDR_X_CENTER	"XLLCENTER"
#define HDR_Y_CENTER	"YLLCENTER"
#define HDR_CELLSIZE	"CELLSIZE"
#define HDR_NODATA		"NODATA_VALUE"


///////////////////////////////////////////////////////////
//														 //
//						Import							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CESRI_ArcInfo_Import::CESRI_ArcInfo_Import(void)
{
	//-----------------------------------------------------
	// 1. Info...

	Set_Name(_TL("Import ESRI Arc/Info Grid"));

	Set_Author(_TL("Copyrights (c) 2001 by Olaf Conrad"));

	Set_Description(_TL(
		"Import grid from ESRI's Arc/Info grid format.\n")
	);


	//-----------------------------------------------------
	// 2. Parameters...

	Parameters.Add_Grid_Output(
		NULL	, "GRID"	, _TL("Grid"),
		""
	);

	Parameters.Add_FilePath(
		NULL	, "FILE"	, _TL("File"),
		"",
		_TL(
		"ESRI Arc/Info Grids|*.asc;*.flt|"
		"ESRI Arc/Info ASCII Grids (*.asc)|*.asc|"
		"ESRI Arc/Info Binary Grids (*.flt)|*.flt|"
		"All Files|*.*")
	);
}

//---------------------------------------------------------
CESRI_ArcInfo_Import::~CESRI_ArcInfo_Import(void)
{}

//---------------------------------------------------------
bool CESRI_ArcInfo_Import::On_Execute(void)
{
	bool		bResult;
	int			x, y, iy;
	float		Value, *Line;
	FILE		*Stream;
	CSG_String	fName;
	CGrid		*pGrid;

	//-----------------------------------------------------
	bResult	= false;
	pGrid	= NULL;

	if( Parameters("FILE")->asString() )
	{
		//-------------------------------------------------
		// Binary...

		if(	SG_File_Cmp_Extension(Parameters("FILE")->asString(), "flt")
		||	SG_File_Cmp_Extension(Parameters("FILE")->asString(), "hdr") )
		{
			fName	= SG_File_Make_Path("", Parameters("FILE")->asString(), "hdr");

			if( (Stream = fopen(fName, "r")) != NULL )
			{
				pGrid	= Read_Header(Stream);

				fName	= SG_File_Make_Path("", Parameters("FILE")->asString(), "flt");

				if( pGrid && (Stream = fopen(fName, "rb")) != NULL )
				{
					Line	= (float *)SG_Malloc(pGrid->Get_NX() * sizeof(float));

					for(iy=0, y=pGrid->Get_NY()-1; iy<pGrid->Get_NY() && !feof(Stream) && Set_Progress(iy, pGrid->Get_NY()); iy++, y--)
					{
						fread(Line, pGrid->Get_NX(), sizeof(float), Stream);

						for(x=0; x<pGrid->Get_NX(); x++)
						{
							pGrid->Set_Value(x, y, Line[x]);
						}
					}

					SG_Free(Line);

					fclose(Stream);
				}
			}
		}


		//-------------------------------------------------
		// ASCII...

		else
		{
			if( (Stream = fopen(Parameters("FILE")->asString(), "r")) != NULL )
			{
				if( (pGrid = Read_Header(Stream)) != NULL )
				{
					for(iy=0, y=pGrid->Get_NY()-1; iy<pGrid->Get_NY() && !feof(Stream) && Set_Progress(iy, pGrid->Get_NY()); iy++, y--)
					{
						for(x=0; x<pGrid->Get_NX(); x++)
						{
							fscanf(Stream, "%f", &Value);

							pGrid->Set_Value(x, y, Value);
						}
					}
				}

				fclose(Stream);
			}
		}

		//-------------------------------------------------
		if( pGrid )
		{
			pGrid->Set_Name(SG_File_Get_Name(Parameters("FILE")->asString(), false));

			Parameters("GRID")->Set_Value(pGrid);

			bResult	= true;
		}
	}

	return( bResult );
}

//---------------------------------------------------------
CGrid * CESRI_ArcInfo_Import::Read_Header(FILE *Stream)
{
	bool		bCorner_X, bCorner_Y;
	char		buffer[32];
	int			NX, NY;
	double		CellSize, xMin, yMin, NoData;
	CSG_String	s;
	CGrid		*pGrid;

	//-----------------------------------------------------
	if( Stream )
	{
		//-------------------------------------------------
		fread(buffer, 1, strlen(HDR_NCOLS)		, Stream);
		buffer[strlen(HDR_NCOLS)]		= '\0';
		s	= buffer;

		if( s.CmpNoCase(HDR_NCOLS) )
			return( NULL );

		fscanf(Stream, "%d", &NX);
		while( !feof(Stream) && fgetc(Stream) != '\n' );

		//-------------------------------------------------
		fread(buffer, 1, strlen(HDR_NROWS)		, Stream);
		buffer[strlen(HDR_NROWS)]		= '\0';
		s	= buffer;

		if( s.CmpNoCase(HDR_NROWS) )
			return( NULL );

		fscanf(Stream, "%d", &NY);
		while( !feof(Stream) && fgetc(Stream) != '\n' );

		//-------------------------------------------------
		fread(buffer, 1, strlen(HDR_X_CORNER)	, Stream);
		buffer[strlen(HDR_X_CORNER)]	= '\0';
		s	= buffer;

		if(      !s.CmpNoCase(HDR_X_CORNER) )
			bCorner_X	= true;
		else if( !s.CmpNoCase(HDR_X_CENTER) )
			bCorner_X	= false;
		else
			return( NULL );

		fscanf(Stream, "%lf", &xMin);
		while( !feof(Stream) && fgetc(Stream) != '\n' );

		//-------------------------------------------------
		fread(buffer, 1, strlen(HDR_Y_CORNER)	, Stream);
		buffer[strlen(HDR_Y_CORNER)]	= '\0';
		s	= buffer;

		if(      !s.CmpNoCase(HDR_Y_CORNER) )
			bCorner_Y	= true;
		else if( !s.CmpNoCase(HDR_Y_CENTER) )
			bCorner_Y	= false;
		else
			return( NULL );

		fscanf(Stream, "%lf", &yMin);
		while( !feof(Stream) && fgetc(Stream) != '\n' );

		//-------------------------------------------------
		fread(buffer, 1, strlen(HDR_CELLSIZE)	, Stream);
		buffer[strlen(HDR_CELLSIZE)]	= '\0';
		s	= buffer;

		if( s.CmpNoCase(HDR_CELLSIZE) )
			return( NULL );

		fscanf(Stream, "%lf", &CellSize);
		while( !feof(Stream) && fgetc(Stream) != '\n' );

		//-------------------------------------------------
		fread(buffer, 1, strlen(HDR_NODATA)		, Stream);
		buffer[strlen(HDR_NODATA)]		= '\0';
		s	= buffer;

		if( s.CmpNoCase(HDR_NODATA) )
			return( NULL );

		fscanf(Stream, "%lf", &NoData);
		while( !feof(Stream) && fgetc(Stream) != '\n' );

		//-------------------------------------------------
		if( bCorner_X )
			xMin	+= CellSize / 2.0;

		if( bCorner_Y )
			yMin	+= CellSize / 2.0;

		//-------------------------------------------------
		if( (pGrid = SG_Create_Grid(GRID_TYPE_Float, NX, NY, CellSize, xMin, yMin)) != NULL )
		{
			pGrid->Set_NoData_Value(NoData);

			return( pGrid );
		}
	}

	return( NULL );
}


///////////////////////////////////////////////////////////
//														 //
//						Export							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CESRI_ArcInfo_Export::CESRI_ArcInfo_Export(void)
{
	//-----------------------------------------------------
	// 1. Info...

	Set_Name(_TL("Export ESRI Arc/Info Grid"));

	Set_Author(_TL("Copyrights (c) 2001 by Olaf Conrad"));

	Set_Description(_TL(
		"Export grid to ESRI's Arc/Info grid format.\n")
	);


	//-----------------------------------------------------
	// 2. Parameters...

	Parameters.Add_Grid(
		NULL	, "GRID"	, _TL("Grid"),
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_FilePath(
		NULL	, "FILE"	, _TL("File"),
		"",
		_TL(
		"ESRI Arc/Info Grids|*.asc;*.flt|"
		"ESRI Arc/Info ASCII Grids (*.asc)|*.asc|"
		"ESRI Arc/Info Binary Grids (*.flt)|*.flt|"
		"All Files|*.*"), NULL, true
	);

	Parameters.Add_Choice(
		NULL	, "FORMAT"	, _TL("Format"),
		"",
		_TL(
		"Binary|"
		"ASCII|")	, 1
	);

	Parameters.Add_Choice(
		NULL	, "GEOREF"	, _TL("Geo-Reference"),
		_TL(
		"The grids geo-reference must be related either to the center or the corner of its lower left grid cell."),
		_TL(
		"Corner|"
		"Center|"), 0
	);

	Parameters.Add_Value(
		NULL	, "PREC"	, _TL("Precision"),
		_TL("Number of decimals when writing floating point values to ASCII format"),
		PARAMETER_TYPE_Int	, 2
	);
}

//---------------------------------------------------------
CESRI_ArcInfo_Export::~CESRI_ArcInfo_Export(void)
{}

//---------------------------------------------------------
bool CESRI_ArcInfo_Export::On_Execute(void)
{
	bool		bResult;
	int			x, y, iy, Precision;
	float		*Line;
	FILE		*Stream;
	CGrid		*pGrid;
	CSG_String	fName;

	//-----------------------------------------------------
	bResult	= false;
	pGrid	= Parameters("GRID")->asGrid();


	//-----------------------------------------------------
	// Binary...

	if( Parameters("FORMAT")->asInt() == 0 )
	{
		fName	= SG_File_Make_Path("", Parameters("FILE")->asString(), "hdr");

		if( (Stream = fopen(fName, "w")) != NULL )
		{
			if( Write_Header(Stream, pGrid) )
			{
				fclose(Stream);

				fName	= SG_File_Make_Path("", Parameters("FILE")->asString(), "flt");

				if( (Stream = fopen(fName, "wb")) != NULL )
				{
					Line	= (float *)SG_Malloc(pGrid->Get_NX() * sizeof(float));

					for(iy=0, y=pGrid->Get_NY()-1; iy<pGrid->Get_NY() && Set_Progress(iy, pGrid->Get_NY()); iy++, y--)
					{
						for(x=0; x<pGrid->Get_NX(); x++)
						{
							Line[x]	= pGrid->asFloat(x, y);
						}

						fwrite(Line, pGrid->Get_NX(), sizeof(float), Stream);
					}

					SG_Free(Line);

					fclose(Stream);

					bResult	= true;
				}
			}
			else
			{
				fclose(Stream);
			}
		}
	}


	//-----------------------------------------------------
	// ASCII...

	else
	{
		if( (Stream = fopen(Parameters("FILE")->asString(), "w")) != NULL )
		{
			if( Write_Header(Stream, pGrid) )
			{
				Precision	= Parameters("PREC")->asInt();

				for(iy=0, y=pGrid->Get_NY()-1; iy<pGrid->Get_NY() && Set_Progress(iy, pGrid->Get_NY()); iy++, y--)
				{
					for(x=0; x<pGrid->Get_NX(); x++)
					{
						if( Precision < 0 )
						{
							fprintf(Stream, "%f "	, Precision, pGrid->asFloat(x, y));
						}
						else
						{
							fprintf(Stream, "%.*f "	, Precision, pGrid->asFloat(x, y));
						}
					}

					fprintf(Stream, "\n");
				}

				bResult	= true;
			}

			fclose(Stream);
		}
	}

	return( bResult );
}

//---------------------------------------------------------
bool CESRI_ArcInfo_Export::Write_Header(FILE *Stream, CGrid *pGrid)
{
	if( Stream && pGrid && pGrid->is_Valid() )
	{
		fprintf(Stream, "%s %d\n"		, HDR_NCOLS		, pGrid->Get_NX());
		fprintf(Stream, "%s %d\n"		, HDR_NROWS		, pGrid->Get_NY());

		if( Parameters("GEOREF")->asInt() == 0 )
		{
			fprintf(Stream, "%s %f\n"	, HDR_X_CORNER	, pGrid->Get_XMin() - pGrid->Get_Cellsize() / 2.0);
			fprintf(Stream, "%s %f\n"	, HDR_Y_CORNER	, pGrid->Get_YMin() - pGrid->Get_Cellsize() / 2.0);
		}
		else
		{
			fprintf(Stream, "%s %f\n"	, HDR_X_CENTER	, pGrid->Get_XMin());
			fprintf(Stream, "%s %f\n"	, HDR_Y_CENTER	, pGrid->Get_YMin());
		}

		fprintf(Stream, "%s %f\n"		, HDR_CELLSIZE	, (float)pGrid->Get_Cellsize());
		fprintf(Stream, "%s %f\n"		, HDR_NODATA	, (float)pGrid->Get_NoData_Value());

		return( true );
	}

	return( false );
}

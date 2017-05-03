/**********************************************************
 * Version $Id: surfer.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                        Grid_IO                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                      Surfer.cpp                       //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
#include "surfer.h"


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define NODATAVALUE	1.70141e38f


///////////////////////////////////////////////////////////
//														 //
//						Import							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSurfer_Import::CSurfer_Import(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Import Surfer Grid"));

	Set_Author		("O.Conrad (c) 2001");

	Set_Description	(_TW(
		"Import grid from Golden Software's Surfer grid format.\n")
	);

	//-----------------------------------------------------
	CSG_Parameter	*pNode;

	Parameters.Add_Grid_Output(
		NULL	, "GRID"	, _TL("Grid"),
		_TL("")
	);

	Parameters.Add_FilePath(
		NULL	, "FILE"	, _TL("File"),
		_TL(""),
		CSG_String::Format("%s (*.grd)|*.grd|%s|*.*",
			_TL("Surfer Grid"),
			_TL("All Files")
		)
	);

	pNode	= Parameters.Add_Choice(
		NULL	, "NODATA"	, _TL("No Data Value"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("Surfer's No Data Value"),
			_TL("User Defined")
		), 0
	);

	Parameters.Add_Double(
		pNode	, "NODATA_VAL"	, _TL("User Defined No Data Value"),
		_TL(""),
		-99999.
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CSurfer_Import::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "NODATA") )
	{
		pParameters->Set_Enabled("NODATA_VAL", pParameter->asInt() == 1);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSurfer_Import::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_String	File	= Parameters("FILE")->asString();

	FILE	*Stream	 = fopen(File.b_str(), "rb");

	if( !Stream )
	{
		Error_Set(_TL("failed to open file"));

		return( false );
	}

	CSG_Grid	*pGrid	= NULL;

	char	Id[4];	fread(Id, 1, 4 * sizeof(char), Stream);

	//-----------------------------------------------------
	if( !strncmp(Id, "DSRB", 4) )	// Surfer 7: Binary...
	{
		long	lValue, nx, ny;
		double	dValue, dx, dy, xmin, ymin;

		fread(&lValue, 1, sizeof(long), Stream);		// SectionSize...
		fread(&lValue, 1, sizeof(long), Stream);		// Version
		fread(&lValue, 1, sizeof(long), Stream);

		if( lValue == 0x44495247 )						// Grid-Header...
		{
			fread(&lValue, 1, sizeof(long  ), Stream);	// SectionSize...
			fread(&ny    , 1, sizeof(long  ), Stream);
			fread(&nx    , 1, sizeof(long  ), Stream);
			fread(&xmin  , 1, sizeof(double), Stream);
			fread(&ymin  , 1, sizeof(double), Stream);
			fread(&dx    , 1, sizeof(double), Stream);
			fread(&dy    , 1, sizeof(double), Stream);
			fread(&dValue, 1, sizeof(double), Stream);
			fread(&dValue, 1, sizeof(double), Stream);
			fread(&dValue, 1, sizeof(double), Stream);	// Rotation (unused)...
			fread(&dValue, 1, sizeof(double), Stream);	// Blank Value...
			fread(&lValue, 1, sizeof(long  ), Stream);	// ???...

			if( lValue == 0x41544144 )	// Load Binary Double...
			{
				fread(&lValue, 1, sizeof(long), Stream);	// SectionSize...

				//-----------------------------------------
				if( !feof(Stream) && (pGrid = SG_Create_Grid(SG_DATATYPE_Double, nx, ny, dx, xmin, ymin)) != NULL )
				{
					double	*Line	= (double *)SG_Malloc(pGrid->Get_NX() * sizeof(double));

					for(int y=0; y<pGrid->Get_NY() && !feof(Stream) && Set_Progress(y, pGrid->Get_NY()); y++)
					{
						fread(Line, pGrid->Get_NX(), sizeof(double), Stream);

						for(int x=0; x<pGrid->Get_NX(); x++)
						{
							pGrid->Set_Value(x, y, Line[x]);
						}
					}

					SG_Free(Line);
				}
			}
		}
	}

	//-----------------------------------------------------
	else if( !strncmp(Id, "DSBB", 4) )	// Surfer 6: Binary...
	{
		short	sValue, nx, ny;
		double	dValue, dx, dy, xmin, ymin;

		fread(&nx    , 1, sizeof(short ), Stream);
		fread(&ny    , 1, sizeof(short ), Stream);
		fread(&xmin  , 1, sizeof(double), Stream);
		fread(&dx    , 1, sizeof(double), Stream);	dx	= (dx - xmin) / (nx - 1.0);	// XMax
		fread(&ymin  , 1, sizeof(double), Stream);
		fread(&dy    , 1, sizeof(double), Stream);	dy	= (dy - ymin) / (ny - 1.0);	// YMax...
		fread(&dValue, 1, sizeof(double), Stream);	// ZMin...
		fread(&dValue, 1, sizeof(double), Stream);	// ZMax...

		//-------------------------------------------------
		if( !feof(Stream) && (pGrid = SG_Create_Grid(SG_DATATYPE_Float, nx, ny, dx, xmin, ymin)) != NULL )
		{
			float	*Line	= (float *)SG_Malloc(pGrid->Get_NX() * sizeof(float));

			for(int y=0; y<pGrid->Get_NY() && !feof(Stream) && Set_Progress(y, pGrid->Get_NY()); y++)
			{
				fread(Line, pGrid->Get_NX(), sizeof(float), Stream);

				for(int x=0; x<pGrid->Get_NX(); x++)
				{
					pGrid->Set_Value(x, y, Line[x]);
				}
			}

			SG_Free(Line);
		}
	}

	//-----------------------------------------------------
	else if( !strncmp(Id, "DSAA", 4) )	// Surfer 6: ASCII...
	{
		int		nx, ny;
		double	dx, dy, xmin, ymin, dValue;

		fscanf(Stream, "%d  %d" , &nx    , &ny    );
		fscanf(Stream, "%lf %lf", &xmin	 , &dx    );	dx	= (dx - xmin) / (nx - 1.0);
		fscanf(Stream, "%lf %lf", &ymin	 , &dy    );	dy	= (dy - ymin) / (ny - 1.0);
		fscanf(Stream, "%lf %lf", &dValue, &dValue);

		//-------------------------------------------------
		if( !feof(Stream) && (pGrid = SG_Create_Grid(SG_DATATYPE_Float, nx, ny, dx, xmin, ymin)) != NULL )
		{
			for(int y=0; y<pGrid->Get_NY() && !feof(Stream) && Set_Progress(y, pGrid->Get_NY()); y++)
			{
				for(int x=0; x<pGrid->Get_NX(); x++)
				{
					fscanf(Stream, "%lf", &dValue);

					pGrid->Set_Value(x, y, dValue);
				}
			}
		}
	}

	//-----------------------------------------------------
	fclose(Stream);

	if( pGrid )
	{
		pGrid->Set_Name(SG_File_Get_Name(File, false));

		pGrid->Set_NoData_Value(Parameters("NODATA")->asInt() == 0 ? NODATAVALUE : Parameters("NODATA_VAL")->asDouble());

		Parameters("GRID")->Set_Value(pGrid);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//						Export							 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSurfer_Export::CSurfer_Export(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Export Surfer Grid"));

	Set_Author		("O.Conrad (c) 2001");

	Set_Description	(_TW(
		"Export grid to Golden Software's Surfer grid format.\n")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "GRID"	, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_FilePath(
		NULL	, "FILE"	, _TL("File"),
		_TL(""),
		CSG_String::Format("%s (*.grd)|*.grd|%s|*.*",
			_TL("Surfer Grid"),
			_TL("All Files")
		), NULL, true
	);

	Parameters.Add_Choice(
		NULL	, "FORMAT"	, _TL("Format"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("binary"),
			_TL("ASCII")
		), 0
	);

	Parameters.Add_Bool(
		NULL	, "NODATA"	, _TL("Use Surfer's No-Data Value"),
		_TL(""),
		false
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSurfer_Export::On_Execute(void)
{
	const char	ID_BINARY[]	= "DSBB";

	FILE		*Stream;

	CSG_Grid	*pGrid	= Parameters("GRID")->asGrid();

	CSG_String	File	= Parameters("FILE")->asString();

	bool		bNoData	= Parameters("NODATA")->asBool();

	switch( Parameters("FORMAT")->asInt() )
	{
	//-----------------------------------------------------
	case 0:	// Surfer 6 - Binary...

		if( (Stream = fopen(File.b_str(), "wb")) != NULL )
		{
			short	sValue;
			double	dValue;

			fwrite(ID_BINARY, 4, sizeof(char), Stream);

			sValue	= (short)pGrid->Get_NX  (); fwrite(&sValue, 1, sizeof(short ), Stream);
			sValue	= (short)pGrid->Get_NY  (); fwrite(&sValue, 1, sizeof(short ), Stream);
			dValue	=        pGrid->Get_XMin(); fwrite(&dValue, 1, sizeof(double), Stream);
			dValue	=        pGrid->Get_XMax(); fwrite(&dValue, 1, sizeof(double), Stream);
			dValue	=        pGrid->Get_YMin(); fwrite(&dValue, 1, sizeof(double), Stream);
			dValue	=        pGrid->Get_YMax(); fwrite(&dValue, 1, sizeof(double), Stream);
			dValue	=        pGrid->Get_Min (); fwrite(&dValue, 1, sizeof(double), Stream);
			dValue	=        pGrid->Get_Max (); fwrite(&dValue, 1, sizeof(double), Stream);

			//---------------------------------------------
			float	*fLine	= (float *)SG_Malloc(pGrid->Get_NX() * sizeof(float));

			for(int y=0; y<pGrid->Get_NY() && Set_Progress(y, pGrid->Get_NY()); y++)
			{
				for(int x=0; x<pGrid->Get_NX(); x++)
				{
					fLine[x]	= bNoData && pGrid->is_NoData(x, y) ? NODATAVALUE : pGrid->asFloat(x, y);
				}

				fwrite(fLine, pGrid->Get_NX(), sizeof(float), Stream);
			}

			SG_Free(fLine);

			fclose(Stream);

			return( true );
		}
		break;

	//-----------------------------------------------------
	case 1:	// Surfer - ASCII...

		if( (Stream = fopen(File.b_str(), "w")) != NULL )
		{
			fprintf(Stream, "DSAA\n");
			fprintf(Stream, "%d %d\n", pGrid->Get_NX  (), pGrid->Get_NY  ());
			fprintf(Stream, "%f %f\n", pGrid->Get_XMin(), pGrid->Get_XMax());
			fprintf(Stream, "%f %f\n", pGrid->Get_YMin(), pGrid->Get_YMax());
			fprintf(Stream, "%f %f\n", pGrid->Get_Min (), pGrid->Get_Max ());

			//---------------------------------------------
			for(int y=0; y<pGrid->Get_NY() && Set_Progress(y, pGrid->Get_NY()); y++)
			{
				for(int x=0; x<pGrid->Get_NX(); x++)
				{
					fprintf(Stream, "%f ", bNoData && pGrid->is_NoData(x, y) ? NODATAVALUE : pGrid->asFloat(x, y));
				}

				fprintf(Stream, "\n");
			}

			fclose(Stream);

			return( true );
		}
		break;
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

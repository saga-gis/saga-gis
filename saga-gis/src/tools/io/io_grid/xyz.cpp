/**********************************************************
 * Version $Id: xyz.cpp 1921 2014-01-09 10:24:11Z oconrad $
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
//                        XYZ.cpp                        //
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
#include "xyz.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CXYZ_Export::CXYZ_Export(void)
{
	Set_Name		(_TL("Export Grid to XYZ"));

	Set_Author		("O.Conrad (c) 2003");

	Set_Description	(_TW(
		"The tool allows one to export a grid or several grids to a table (text format), "
		"which stores the x/y-coordinates and the cell values of the input grid(s).\n"
		"By default, No-Data cells are not written to the output. This can be changed "
		"with the \"Write No-Data\" parameter. If No-Data cells are skipped, these "
		"are detected for the first input grid which operates like a mask.\n\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List("",
		"GRIDS"		, _TL("Grids"),
		_TL("The grid(s) to export."),
		PARAMETER_INPUT
	);

	Parameters.Add_FilePath("",
		"FILENAME"	, _TL("File"),
		_TL("The output file."),
		CSG_String::Format(
			"%s|*.xyz|%s|*.txt|%s|*.*",
			_TL("XYZ files (*.xyz)"),
			_TL("Text files (*.txt)"),
			_TL("All Files")
		), NULL, true
	);

	Parameters.Add_Bool("",
		"HEADER"	, _TL("Write Header"),
		_TL("Write a header to the .xyz file."),
		true
	);

	Parameters.Add_Bool("",
		"NODATA"	, _TL("Write No-Data"),
		_TL("Write No-Data cells to the .xyz file."),
		false
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CXYZ_Export::On_Execute(void)
{
	CSG_Parameter_Grid_List	*pGrids	= Parameters("GRIDS")->asGridList();

	if( pGrids->Get_Grid_Count() <= 0 )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_File	Stream;

	if( !Stream.Open(Parameters("FILENAME")->asString(), SG_FILE_W, false) )
	{
		Error_Fmt("%s\n[%s]", _TL("could not open file"), Parameters("FILENAME")->asString());

		return( false );
	}

	//-----------------------------------------------------
	if( Parameters("HEADER")->asBool() )
	{
		Stream.Printf("\"X\"\t\"Y\"");

		for(int i=0; i<pGrids->Get_Grid_Count(); i++)
		{
			Stream.Printf("\t\"%s\"", pGrids->Get_Grid(i)->Get_Name());
		}

		Stream.Printf("\n");
	}

	//-----------------------------------------------------
	bool	bNoData	= Parameters("NODATA")->asBool();

	TSG_Point	p;	p.y	= Get_YMin();

	for(int y=0; y<Get_NY() && Set_Progress(y); y++, p.y+=Get_Cellsize())
	{
		p.x	= Get_XMin();

		for(int x=0; x<Get_NX(); x++, p.x+=Get_Cellsize())
		{
			if( bNoData || !pGrids->Get_Grid(0)->is_NoData(x, y) )
			{
				Stream.Printf("%f\t%f", p.x, p.y);

				for(int i=0; i<pGrids->Get_Grid_Count(); i++)
				{
					Stream.Printf("\t%f", pGrids->Get_Grid(i)->asDouble(x, y));
				}

				Stream.Printf("\n");
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CXYZ_Import::CXYZ_Import(void)
{
	Set_Name		(_TL("Import Grid from XYZ"));

	Set_Author		("O.Conrad (c) 2009");

	Set_Description	(_TW(
		"The tool allows one to import gridded data from a table (text format), "
		"which contains for each grid cell the x/y-coordinates and the cell "
		"value. If several values end up in a single grid cell, the mean value "
		"of all input values is written to that cell.\n\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_Output("",
		"GRID"		, _TL("Grid"),
		_TL("The imported grid.")
	);

	Parameters.Add_Grid_Output("",
		"COUNT"		, _TL("Count"),
		_TL("The number of values detected in each grid cell.")
	);

	Parameters.Add_FilePath("",
		"FILENAME"	, _TL("File"),
		_TL("The input file."),
		CSG_String::Format(
			"%s|*.xyz;*.txt|%s|*.xyz|%s|*.txt|%s|*.*",
			_TL("Recognized Files"),
			_TL("XYZ files (*.xyz)"),
			_TL("Text files (*.txt)"),
			_TL("All Files")
		), NULL, false
	);

	Parameters.Add_Int("",
		"SKIP"		, _TL("Skip Leading Lines"),
		_TL("The number of leading lines to skip (usually header data)."),
		0, 0, true
	);

	Parameters.Add_Double("",
		"CELLSIZE"	, _TL("Cell Size"),
		_TL("The cell size of the output grid."),
		1.0, 0.0, true
	);

	Parameters.Add_Choice("",
		"SEPARATOR"	, _TL("Separator"),
		_TL(""),
		CSG_String::Format("%s|%s|,|;|%s|%s|",
			_TL("default delimiters"),
			_TL("space"),
			_TL("tabulator"),
			_TL("user defined")
		), 0
	);

	Parameters.Add_String("SEPARATOR",
		"USER"		, _TL("User Defined"),
		_TL("The user defined delimiter."),
		"*"
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CXYZ_Import::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("SEPARATOR") )
	{
		pParameters->Set_Enabled("USER", pParameter->asInt() == 5);
	}

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CXYZ_Import::On_Execute(void)
{
	double	Cellsize	= Parameters("CELLSIZE")->asDouble();

	if( Cellsize <= 0.0 )
	{
		Error_Set(_TL("cell size has to be greater than zero"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_File	Stream;

	if( !Stream.Open(Parameters("FILENAME")->asString(), SG_FILE_R, false) )
	{
		Error_Fmt("%s\n[%s]", _TL("could not open file"), Parameters("FILENAME")->asString());

		return( false );
	}

	//-----------------------------------------------------
	switch( Parameters("SEPARATOR")->asInt() )
	{
	default: m_Delimiters = " \t\r\n";	break;
	case  1: m_Delimiters =       " ";	break;
	case  2: m_Delimiters =      "\t";	break;
	case  3: m_Delimiters =       ",";	break;
	case  4: m_Delimiters =       ";";	break;
	case  5: m_Delimiters = Parameters("USER")->asString();	break;
	}

	//-----------------------------------------------------
	CSG_String	sLine;

	{
		for(int Skip=Parameters("SKIP")->asInt(); Skip>0; Skip--)
		{
			Stream.Read_Line(sLine);
		}
	}

	//-----------------------------------------------------
	sLong		fLength = Stream.Length(), nValues = 0;
	double		z;
	TSG_Point	p;
	CSG_Rect	Extent;

	while( !Stream.is_EOF() && Set_Progress((double)Stream.Tell(), (double)fLength) )
	{
		if( Read_Values(Stream, p.x, p.y, z) )
		{
			if( nValues++ == 0 )
			{
				Extent.Assign(p, p);
			}
			else
			{
				Extent.Union(p);
			}
		}
	}

	if( !Process_Get_Okay() || !(Extent.Get_XRange() > 0.0 || Extent.Get_YRange() > 0.0) )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid	*pGrid	= SG_Create_Grid(CSG_Grid_System(Cellsize, Extent), SG_DATATYPE_Float);

	if( !pGrid )
	{
		return( false );
	}

	Parameters("GRID")->Set_Value(pGrid);

	pGrid->Set_Name(SG_File_Get_Name(Parameters("FILENAME")->asString(), false));

	//-----------------------------------------------------
	CSG_Grid	*pCount	= SG_Create_Grid(pGrid->Get_System(), SG_DATATYPE_Byte);

	if( !pCount )
	{
		return( false );
	}

	Parameters("COUNT")->Set_Value(pCount);

	pCount->Fmt_Name("%s [%s]", pGrid->Get_Name(), _TL("Count"));

	//-----------------------------------------------------
	Stream.Seek_Start();

	{
		for(int Skip=Parameters("SKIP")->asInt(); Skip>0; Skip--)
		{
			Stream.Read_Line(sLine);
		}
	}

	//-----------------------------------------------------
	while( !Stream.is_EOF() && Set_Progress((double)Stream.Tell(), (double)fLength) )
	{
		if( Read_Values(Stream, p.x, p.y, z) )
		{
			int	x, y;

			if( pGrid->Get_System().Get_World_to_Grid(x, y, p) )
			{
				pGrid ->Add_Value(x, y, z);
				pCount->Add_Value(x, y, 1);
			}
		}
	}

	//-----------------------------------------------------
	for(int y=0; y<pGrid->Get_NY() && Set_Progress(y, pGrid->Get_NY()); y++)
	{
		for(int x=0; x<pGrid->Get_NX(); x++)
		{
			nValues	= pCount->asInt(x, y);

			if( nValues == 0 )
			{
				pGrid->Set_NoData(x, y);
			}
			else if( nValues > 1 )
			{
				pGrid->Mul_Value(x, y, 1.0 / nValues);
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline bool CXYZ_Import::Read_Values(CSG_File &Stream, double &x, double &y, double &z)
{
	CSG_String	sLine;

	if( !Stream.Read_Line(sLine) )
	{
		return( false );
	}

	CSG_String_Tokenizer	Tokenizer(sLine, m_Delimiters);

	return( Tokenizer.Has_More_Tokens() && Tokenizer.Get_Next_Token().asDouble(x)
		&&  Tokenizer.Has_More_Tokens() && Tokenizer.Get_Next_Token().asDouble(y)
		&&  Tokenizer.Has_More_Tokens() && Tokenizer.Get_Next_Token().asDouble(z) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

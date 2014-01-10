/**********************************************************
 * Version $Id: xyz.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

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
#include "xyz.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CXYZ_Export::CXYZ_Export(void)
{
	//-----------------------------------------------------
	// 1. Info...

	Set_Name		(_TL("Export Grid to XYZ"));

	Set_Author		(SG_T("O. Conrad (c) 2003"));

	Set_Description	(_TW(
		"Export grid to a table (text format), that contains for each grid cell "
		"the x/y-coordinates and additionally data from selected grids.\n"
		"Optionally, it is possible to skip NoData cells from the output. In this "
		"case, the first input grid will perform like a mask.\n\n")
	);


	//-----------------------------------------------------
	// 2. Parameters...

	Parameters.Add_Grid_List(
		NULL	, "GRIDS"		, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_FilePath(
		NULL	, "FILENAME"	, _TL("File Name"),
		_TL(""),
		CSG_String::Format(
			SG_T("%s|*.xyz|%s|*.txt|%s|*.*"),
			_TL("XYZ files (*.xyz)"),
			_TL("Text files (*.txt)"),
			_TL("All Files")
		), NULL, true
	);

	Parameters.Add_Value(
		NULL	, "CAPTION"		, _TL("Write Field Names"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);

	Parameters.Add_Value(
		NULL	, "EX_NODATA"	, _TL("Exclude NoData Cells"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);
}

//---------------------------------------------------------
bool CXYZ_Export::On_Execute(void)
{
	bool					bExNoData;
	int						x, y, i;
	TSG_Point				p;
	CSG_File				Stream;
	CSG_String				FileName;
	CSG_Parameter_Grid_List	*pGrids;

	pGrids		= Parameters("GRIDS")	->asGridList();
	FileName	= Parameters("FILENAME")->asString();
	bExNoData	= Parameters("EX_NODATA")->asBool();

	if( pGrids->Get_Count() > 0 && Stream.Open(FileName, SG_FILE_W, false) )
	{
		if( Parameters("CAPTION")->asBool() )
		{
			Stream.Printf(SG_T("\"X\"\t\"Y\""));

			for(i=0; i<pGrids->Get_Count(); i++)
			{
				Stream.Printf(SG_T("\t\"%s\""), pGrids->asGrid(i)->Get_Name());
			}

			Stream.Printf(SG_T("\n"));
		}

		for(y=0, p.y=Get_YMin(); y<Get_NY() && Set_Progress(y); y++, p.y+=Get_Cellsize())
		{
			for(x=0, p.x=Get_XMin(); x<Get_NX(); x++, p.x+=Get_Cellsize())
			{
				if( !bExNoData || (bExNoData && !pGrids->asGrid(0)->is_NoData(x, y)) )
				{
					Stream.Printf(SG_T("%f\t%f"), p.x,  p.y);

					for(i=0; i<pGrids->Get_Count(); i++)
					{
						Stream.Printf(SG_T("\t%f"), pGrids->asGrid(i)->asDouble(x, y));
					}

					Stream.Printf(SG_T("\n"));
				}
			}
		}

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
CXYZ_Import::CXYZ_Import(void)
{
	//-----------------------------------------------------
	// 1. Info...

	Set_Name		(_TL("Import Grid from XYZ"));

	Set_Author		(SG_T("O. Conrad (c) 2009"));

	Set_Description	(_TW(
		"Import grid from a table (text format), that contains for each grid cell "
		"the x/y/z-coordinates and additional data from selected grids.\n"
	));


	//-----------------------------------------------------
	// 2. Parameters...

	Parameters.Add_Grid_Output(
		NULL	, "GRID"		, _TL("Grid"),
		_TL("")
	);

	Parameters.Add_Grid_Output(
		NULL	, "COUNT"		, _TL("Count"),
		_TL("")
	);

	Parameters.Add_FilePath(
		NULL	, "FILENAME"	, _TL("File Name"),
		_TL(""),
		CSG_String::Format(
			SG_T("%s|*.xyz|%s|*.txt|%s|*.*"),
			_TL("XYZ files (*.xyz)"),
			_TL("Text files (*.txt)"),
			_TL("All Files")
		), NULL, false
	);

	Parameters.Add_Value(
		NULL	, "CAPTION"		, _TL("Has Field Names"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);

	Parameters.Add_Value(
		NULL	, "CELLSIZE"	, _TL("Target Cellsize"),
		_TL(""),
		PARAMETER_TYPE_Double, 1.0, 0.0, true
	);

	Parameters.Add_Choice(
		NULL	, "SEPARATOR"	, _TL("Separator"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|,|;|"),
			_TL("space"),
			_TL("tabulator")
		), 1
	);
}

//---------------------------------------------------------
bool CXYZ_Import::On_Execute(void)
{
	int			nx, ny, nValues, fLength;
	double		x, y, z, xMin, yMin, xMax, yMax, Cellsize;
	CSG_File	Stream;
	CSG_String	FileName, sLine;
	CSG_Grid	*pGrid, *pCount;

	FileName	= Parameters("FILENAME")->asString();
	Cellsize	= Parameters("CELLSIZE")->asDouble();

	switch( Parameters("SEPARATOR")->asInt() )
	{
	case 0:	m_Separator	= SG_T(' ');	break;
	case 1:	m_Separator	= SG_T('\t');	break;
	case 2:	m_Separator	= SG_T(',');	break;
	case 3:	m_Separator	= SG_T(';');	break;
	}

	if( Cellsize > 0.0 && Stream.Open(FileName, SG_FILE_R, false) )
	{
		if( Parameters("CAPTION")->asBool() )
		{
			Stream.Read_Line(sLine);
		}

		fLength	= Stream.Length();
		nValues	= 0;
		xMin	= xMax	= 0;
		yMin	= yMax	= 0;

		while( Read_Values(Stream, x, y, z) && Set_Progress(Stream.Tell(), fLength) )
		{
			if( nValues == 0 )
			{
				xMin	= xMax	= x;
				yMin	= yMax	= y;
			}
			else
			{
				if( xMin > x )	xMin	= x;	else if( xMax < x )	xMax	= x;
				if( yMin > y )	yMin	= y;	else if( yMax < y )	yMax	= y;
			}

			nValues++;
		}

		//-------------------------------------------------
		if( Process_Get_Okay() && xMin < xMax && yMin < yMax )
		{
			nx		= 1 + (int)((xMax - xMin) / Cellsize);
			ny		= 1 + (int)((yMax - yMin) / Cellsize);

			Parameters("GRID" )->Set_Value(pGrid  = SG_Create_Grid(SG_DATATYPE_Float, nx, ny, Cellsize, xMin, yMin));
			Parameters("COUNT")->Set_Value(pCount = SG_Create_Grid(SG_DATATYPE_Byte , nx, ny, Cellsize, xMin, yMin));

			if( pGrid && pCount )
			{
				pGrid	->Set_Name(FileName = SG_File_Get_Name(FileName, false));
				pCount	->Set_Name(CSG_String::Format(SG_T("%s [%s]"), FileName.c_str(), _TL("Count")));

				Stream.Seek_Start();

				if( Parameters("CAPTION")->asBool() )
				{
					Stream.Read_Line(sLine);
				}

				while( Read_Values(Stream, x, y, z) && Set_Progress(Stream.Tell(), fLength) )
				{
					if( pGrid->Get_System().Get_World_to_Grid(nx, ny, x, y) )
					{
						pGrid ->Add_Value(nx, ny, z);
						pCount->Add_Value(nx, ny, 1.0);
					}
				}

				for(ny=0; ny<pGrid->Get_NY() && Set_Progress(ny, pGrid->Get_NY()); ny++)
				{
					for(nx=0; nx<pGrid->Get_NX(); nx++)
					{
						nValues	= pCount->asInt(nx, ny);

						if( nValues == 0 )
						{
							pGrid->Set_NoData(nx, ny);
						}
						else if( nValues > 1 )
						{
							pGrid->Mul_Value(nx, ny, 1.0 / nValues);
						}
					}
				}

				return( true );
			}
		}
	}

	return( false );
}

//---------------------------------------------------------
inline bool CXYZ_Import::Read_Values(CSG_File &Stream, double &x, double &y, double &z)
{
	CSG_String	sLine;

	if( Stream.Read_Line(sLine) )
	{
		sLine.Trim();

		if( sLine.asDouble(x) )
		{
			sLine	= sLine.AfterFirst(m_Separator);

			sLine.Trim();

			if( sLine.asDouble(y) )
			{
				sLine	= sLine.AfterFirst(m_Separator);

				if( sLine.asDouble(z) )
				{
					return( true );
				}
			}
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

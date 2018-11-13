/**********************************************************
 * Version $Id: grid_table.cpp 1921 2014-01-09 10:24:11Z oconrad $
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
//                    Grid_Table.cpp                     //
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
#include "grid_table.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Table_Import::CGrid_Table_Import(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Import Grid from Table"));

	Set_Author		("O.Conrad (c) 2006");

	Set_Description	(_TW(
		"Imports a grid from a table."
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_Output(
		"", "GRID"		, _TL("Grid"),
		_TL("")
	);

	Parameters.Add_FilePath(
		"", "FILE"		, _TL("Table"),
		_TL(""),
		CSG_String::Format("%s|*.txt;*.dbf;*.csv|%s|*.*",
			_TL("Table Formats (*.txt, *.dbf, *.csv"),
			_TL("All Files")
		)
	);

	//-----------------------------------------------------
	Parameters.Add_Double("", "CELLSIZE" , _TL("Cell Size"    ), _TL(""), 1.0);
	Parameters.Add_Double("", "XMIN"     , _TL("Left Border"  ), _TL(""), 0.0);
	Parameters.Add_Double("", "YMIN"     , _TL("Lower Border" ), _TL(""), 0.0);
	Parameters.Add_String("", "UNIT"     , _TL("Unit Name"    ), _TL(""), "");
	Parameters.Add_Double("", "ZFACTOR"  , _TL("Z Multiplier" ), _TL(""), 1.0);
	Parameters.Add_Double("", "NODATA"   , _TL("No Data Value"), _TL(""), -99999.0);
	Parameters.Add_Int   ("", "HEADLINES", _TL("Header Lines" ), _TL(""), 0, 0, true);

	Parameters.Add_Choice(
		"", "DATA_TYPE"	, _TL("Data Type"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s|%s|%s|%s",
			_TL("1 Byte Integer (unsigned)"),
			_TL("1 Byte Integer (signed)"  ),
			_TL("2 Byte Integer (unsigned)"),
			_TL("2 Byte Integer (signed)"  ),
			_TL("4 Byte Integer (unsigned)"),
			_TL("4 Byte Integer (signed)"  ),
			_TL("4 Byte Floating Point"    ),
			_TL("8 Byte Floating Point"    )
		), 6
	);

	Parameters.Add_Choice(
		"", "TOPDOWN"	, _TL("Line Order"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("Bottom to Top"),
			_TL("Top to Bottom")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Table_Import::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Table	Table;

	if( !Table.Create(Parameters("FILE")->asString()) )
	{
		Error_Fmt("%s [%s]", _TL("could not open table file"), Parameters("FILE")->asString());

		return( false );
	}

	//-----------------------------------------------------
	int	nx, ny, nHeadLines	= Parameters("HEADLINES")->asInt();

	if( (nx = Table.Get_Field_Count()) < 1 || (ny = Table.Get_Record_Count()) < 1 )
	{
		Error_Fmt("%s [%s]", _TL("no data in table file"), Parameters("FILE")->asString());

		return( false );
	}

	//-----------------------------------------------------
	TSG_Data_Type	Type;

	switch( Parameters("DATA_TYPE")->asInt() )
	{
	case  0:	Type	= SG_DATATYPE_Byte  ;	break;	// 1 Byte Integer (unsigned)
	case  1:	Type	= SG_DATATYPE_Char  ;	break;	// 1 Byte Integer (signed)
	case  2:	Type	= SG_DATATYPE_Word  ;	break;	// 2 Byte Integer (unsigned)
	case  3:	Type	= SG_DATATYPE_Short ;	break;	// 2 Byte Integer (signed)
	case  4:	Type	= SG_DATATYPE_DWord ;	break;	// 4 Byte Integer (unsigned)
	case  5:	Type	= SG_DATATYPE_Int   ;	break;	// 4 Byte Integer (signed)
	default:	Type	= SG_DATATYPE_Float ;	break;	// 4 Byte Floating Point
	case  7:	Type	= SG_DATATYPE_Double;	break;	// 8 Byte Floating Point
	}

	//-----------------------------------------------------
	CSG_Grid	*pGrid	= SG_Create_Grid(Type, nx, ny,
		Parameters("CELLSIZE")->asDouble(),
		Parameters("XMIN"    )->asDouble(),
		Parameters("YMIN"    )->asDouble()
	);

	pGrid->Set_Name        (SG_File_Get_Name(Parameters("FILE")->asString(), false));
	pGrid->Set_Unit        (Parameters("UNIT"   )->asString());
	pGrid->Set_NoData_Value(Parameters("NODATA" )->asDouble());
	pGrid->Set_Scaling     (Parameters("ZFACTOR")->asDouble());

	Parameters("GRID")->Set_Value(pGrid);

	//-----------------------------------------------------
	bool	bDown	= Parameters("TOPDOWN")->asInt() == 1;

	for(int y=0; y<ny && Set_Progress(y, ny); y++)
	{
		CSG_Table_Record	*pRecord	= Table.Get_Record(y + nHeadLines);

		for(int x=0, yy=bDown?ny-1-y:y; x<nx; x++)
		{
			pGrid->Set_Value(x, yy, pRecord->asDouble(x));
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
CCRU_Table_Import::CCRU_Table_Import(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Import CRU Grids"));

	Set_Author		("O.Conrad (c) 2016");

	Set_Description	(_TW(
		"Import grids from <i>Climatic Research Unit Global Climate Dataset</i> files."
	));

	Add_Reference("http://www.cru.uea.ac.uk/data/",
		SG_T("Climatic Research Unit at University of East Anglia")
	);

	Add_Reference("http://www.ipcc-data.org/observ/clim/cru_climatologies.html",
		SG_T("CRU downloads at the IPCC Data Distribution Centre")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		"", "GRIDS"	, _TL("Grids"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_FilePath(
		"", "FILE"	, _TL("File"),
		_TL(""),
		CSG_String::Format("%s|*.dat|%s|*.*",
			_TL("CRU Data File (*.dat"),
			_TL("All Files")
		)
	);

	//-----------------------------------------------------
	Parameters.Add_Bool(
		"", "SHIFT"	, _TL("Shift"),
		_TL(""),
		true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CCRU_Table_Import::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_File	File;

	if( !File.Open(Parameters("FILE")->asString(), SG_FILE_R, false) )
	{
		Error_Fmt("%s [%s]", _TL("could not open file"), Parameters("FILE")->asString());

		return( false );
	}

	//-----------------------------------------------------
	CSG_String	sLine;

	if( !File.Read_Line(sLine) )
	{
		Error_Fmt("%s [%s]", _TL("failed to read header"), Parameters("FILE")->asString());

		return( false );
	}

	//-----------------------------------------------------
	int		nx, ny, nMonths;
	double	Cellsize, xMin, yMin, xMax, yMax;

	if( !File.Scan(Cellsize)
	||  !File.Scan(xMin    ) || !File.Scan(yMin    )
	||  !File.Scan(xMax    ) || !File.Scan(yMax    )
	||  !File.Scan(nx      ) || !File.Scan(ny      )
	||  !File.Scan(nMonths ) )
	{
		Error_Fmt("%s [%s]", _TL("failed to read header"), Parameters("FILE")->asString());

		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid_System	System(Cellsize, xMin, yMin, nx, ny);

	if( !System.is_Valid() || System.Get_XMax() != xMax || System.Get_YMax() != yMax )
	{
		Error_Fmt("%s [%s]", _TL("failed to read header"), Parameters("FILE")->asString());

		return( false );
	}

	//-----------------------------------------------------
	bool	bShift	= Parameters("SHIFT")->asBool();

	if( bShift )
	{
		System.Assign(Cellsize, xMin - 180.0, yMin, nx, ny);
	}

	//-----------------------------------------------------
	CSG_String	Name	= SG_File_Get_Name(Parameters("FILE")->asString(), false);

	Parameters("GRIDS")->asGridList()->Del_Items();

	for(int iMonth=0; iMonth<nMonths && !File.is_EOF() && Process_Get_Okay(); iMonth++)
	{
		Process_Set_Text("%s %d", _TL("Band"), 1 + iMonth);

		CSG_Grid	*pGrid	= SG_Create_Grid(System, SG_DATATYPE_Short);

		pGrid->Fmt_Name("%s_%02d", Name.c_str(), 1 + iMonth);
		pGrid->Set_NoData_Value(-9999);
		pGrid->Get_Projection().Set_GCS_WGS84();

		Parameters("GRIDS")->asGridList()->Add_Item(pGrid);

		//-------------------------------------------------
		for(int y=0; y<ny && !File.is_EOF() && Set_Progress(y, ny); y++)
		{
			if( File.Read_Line(sLine) && sLine.Length() >= 5. * nx )
			{
				for(int x=0, xx=bShift?nx/2:x, yy=ny-1-y; x<nx; x++, xx++)
				{
					double	z;

					CSG_String	s	= sLine.Mid(x * 5, 5);

					if( s.asDouble(z) )
					{
						pGrid->Set_Value(xx % nx, yy, z);
					}
					else
					{
						pGrid->Set_NoData(xx % nx, yy);
					}
				}
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

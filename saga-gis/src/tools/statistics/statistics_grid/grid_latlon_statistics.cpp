/**********************************************************
 * Version $Id: grid_latlon_statistics.cpp 1252 2011-12-15 15:43:13Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    statistics_grid                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               grid_latlon_statistics.cpp              //
//                                                       //
//                 Copyright (C) 2012 by                 //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "grid_latlon_statistics.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Statistics_Latitudinal::CGrid_Statistics_Latitudinal(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Longitudinal Grid Statistics"));

	Set_Author		("O.Conrad (c) 2012");

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		""	, "GRID"	, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table(
		""	, "STATS"	, _TL("Latitudinal Statistics"),
		_TL(""),
		PARAMETER_OUTPUT
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Statistics_Latitudinal::On_Execute(void)
{
	CSG_Grid	*pGrid	= Parameters("GRID" )->asGrid();
	CSG_Table	*pTable	= Parameters("STATS")->asTable();

	pTable->Destroy();
	pTable->Set_Name(CSG_String::Format("%s [%s]", _TL("Latitudinal Statistics"), pGrid->Get_Name()));
	pTable->Add_Field(SG_T("Y"     ), SG_DATATYPE_Double);
	pTable->Add_Field(SG_T("MEAN"  ), SG_DATATYPE_Double);
	pTable->Add_Field(SG_T("MIN"   ), SG_DATATYPE_Double);
	pTable->Add_Field(SG_T("MAX"   ), SG_DATATYPE_Double);
	pTable->Add_Field(SG_T("STDDEV"), SG_DATATYPE_Double);

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		CSG_Simple_Statistics	Statistics;

		for(int x=0; x<Get_NX(); x++)
		{
			Statistics	+= pGrid->asDouble(x, y);
		}

		CSG_Table_Record	*pRecord	= pTable->Add_Record();

		pRecord->Set_Value(0, pGrid->Get_System().Get_yGrid_to_World(y));
		pRecord->Set_Value(1, Statistics.Get_Mean   ());
		pRecord->Set_Value(2, Statistics.Get_Minimum());
		pRecord->Set_Value(3, Statistics.Get_Maximum());
		pRecord->Set_Value(4, Statistics.Get_StdDev ());
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Statistics_Meridional::CGrid_Statistics_Meridional(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Meridional Grid Statistics"));

	Set_Author		("O.Conrad (c) 2012");

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		""	, "GRID"	, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table(
		""	, "STATS"	, _TL("Meridional Statistics"),
		_TL(""),
		PARAMETER_OUTPUT
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Statistics_Meridional::On_Execute(void)
{
	CSG_Grid	*pGrid	= Parameters("GRID" )->asGrid();
	CSG_Table	*pTable	= Parameters("STATS")->asTable();

	pTable->Destroy();
	pTable->Set_Name(CSG_String::Format("%s [%s]", _TL("Meridional Statistics"), pGrid->Get_Name()));
	pTable->Add_Field(SG_T("X"     ), SG_DATATYPE_Double);
	pTable->Add_Field(SG_T("MEAN"  ), SG_DATATYPE_Double);
	pTable->Add_Field(SG_T("MIN"   ), SG_DATATYPE_Double);
	pTable->Add_Field(SG_T("MAX"   ), SG_DATATYPE_Double);
	pTable->Add_Field(SG_T("STDDEV"), SG_DATATYPE_Double);

	for(int x=0; x<Get_NX() && Set_Progress(x, Get_NX()); x++)
	{
		CSG_Simple_Statistics	Statistics;

		for(int y=0; y<Get_NY(); y++)
		{
			Statistics	+= pGrid->asDouble(x, y);
		}

		CSG_Table_Record	*pRecord	= pTable->Add_Record();

		pRecord->Set_Value(0, pGrid->Get_System().Get_xGrid_to_World(x));
		pRecord->Set_Value(1, Statistics.Get_Mean   ());
		pRecord->Set_Value(2, Statistics.Get_Minimum());
		pRecord->Set_Value(3, Statistics.Get_Maximum());
		pRecord->Set_Value(4, Statistics.Get_StdDev ());
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

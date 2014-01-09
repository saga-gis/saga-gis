/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                  Grid_Visualisation                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                Grid_Histogram_Surface.cpp             //
//                                                       //
//                 Copyright (C) 2010 by                 //
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
#include "Grid_Histogram_Surface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Histogram_Surface::CGrid_Histogram_Surface(void)
{
	Set_Name		(_TL("Histogram Surface"));

	Set_Author		(SG_T("O.Conrad (c) 2010"));

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "GRID"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_Output(
		NULL	, "HIST"		, _TL("Histogram"),
		_TL("")
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"		,	_TL("Method"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("rows"),
			_TL("columns"),
			_TL("circle")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Histogram_Surface::On_Execute(void)
{
	m_pGrid	= Parameters("GRID")->asGrid();

	switch( Parameters("METHOD")->asInt() )
	{
	case 0:	return( Get_Lines( true) );
	case 1:	return( Get_Lines(false) );
	case 2:	return( Get_Circle() );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Histogram_Surface::Get_Lines(bool bRows)
{
	int			i, j, n_i, n_j;
	CSG_Table	Values;
	CSG_Grid	*pHist;

	//-----------------------------------------------------
	Parameters("HIST")->Set_Value(pHist	= SG_Create_Grid(m_pGrid));

	pHist->Set_NoData_Value_Range(
		m_pGrid->Get_NoData_Value(),
		m_pGrid->Get_NoData_hiValue()
	);

	n_i	= bRows ? Get_NX() : Get_NY();
	n_j	= bRows ? Get_NY() : Get_NX();

	Values.Add_Field(SG_T("Z"), SG_DATATYPE_Double);

	for(i=0; i<n_i; i++)
	{
		Values.Add_Record();
	}

	//-----------------------------------------------------
	for(j=0; j<n_j && Set_Progress(j, n_j); j++)
	{
		for(i=0; i<n_i; i++)
		{
			Values.Get_Record(i)->Set_Value(0, bRows ? m_pGrid->asDouble(i, j) : m_pGrid->asDouble(j, i));
		}

		Values.Set_Index(0, TABLE_INDEX_Ascending);

		for(i=0; i<n_i; i++)
		{
			int		k	= i % 2 ? i / 2 : n_i - 1 - i / 2;

			if( bRows )
			{
				pHist->Set_Value(k, j, Values.Get_Record_byIndex(i)->asDouble(0));
			}
			else
			{
				pHist->Set_Value(j, k, Values.Get_Record_byIndex(i)->asDouble(0));
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
bool CGrid_Histogram_Surface::Get_Circle(void)
{
	long		i;
	int			n;
	double		r;
	CSG_Grid	*pHist;

	r	= sqrt(m_pGrid->Get_NCells() / M_PI);
	n	= 1 + (int)(2.0 * r);

	//-----------------------------------------------------
	Parameters("HIST")->Set_Value(pHist	= SG_Create_Grid(m_pGrid->Get_Type(), n, n, m_pGrid->Get_Cellsize(), -r * m_pGrid->Get_Cellsize(), -r * m_pGrid->Get_Cellsize()));

	pHist->Set_NoData_Value_Range(
		m_pGrid->Get_NoData_Value(),
		m_pGrid->Get_NoData_hiValue()
	);

	//-----------------------------------------------------
	for(int y=0; y<n && Set_Progress(y, n); y++)
	{
		for(int x=0; x<n; x++)
		{
			double	d	= SG_Get_Distance(x, y, r, r);

			if( d < r && m_pGrid->Get_Sorted((long)(d*d*M_PI), i) )
			{
				pHist->Set_Value(x, y, m_pGrid->asDouble(i));
			}
			else
			{
				pHist->Set_NoData(x, y);
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

/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      Grid_Filter                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    Filter_3x3.cpp                     //
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
#include "Filter_3x3.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFilter_3x3::CFilter_3x3(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("User Defined Filter"));

	Set_Author		("O.Conrad (c) 2001");

	Set_Description	(_TW(
		"User defined filter matrix. The filter can be chosen from loaded tables. "
		"If not specified a fixed table with 3 rows (and 3 columns) will be used. "
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		"", "INPUT"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		"", "RESULT"		, _TL("Filtered Grid"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Table(
		"", "FILTER"		, _TL("Filter Matrix"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Bool(
		"", "ABSOLUTE"	, _TL("Absolute Weighting"),
		_TL(""),
		true
	);

	//-----------------------------------------------------
	CSG_Table	Filter;

	Filter.Add_Field("1", SG_DATATYPE_Double);
	Filter.Add_Field("2", SG_DATATYPE_Double);
	Filter.Add_Field("3", SG_DATATYPE_Double);

	Filter.Add_Record();
	Filter.Add_Record();
	Filter.Add_Record();

	Filter[0][0]	= 0.5;	Filter[0][1]	= 1.0;	Filter[0][2]	= 0.5;
	Filter[1][0]	= 1.0;	Filter[1][1]	=-6.0;	Filter[1][2]	= 1.0;
	Filter[2][0]	= 0.5;	Filter[2][1]	= 1.0;	Filter[2][2]	= 0.5;

	Parameters.Add_FixedTable(
		"", "FILTER_3X3"	, _TL("Default Filter Matrix (3x3)"),
		_TL(""),
		&Filter
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFilter_3x3::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Table	*pFilter	= Parameters("FILTER")->asTable()
		? Parameters("FILTER"    )->asTable()
		: Parameters("FILTER_3X3")->asTable();

	if( pFilter->Get_Count() < 1 || pFilter->Get_Field_Count() < 1 )
	{
		Error_Set(_TL("invalid filter matrix"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Matrix	Filter(pFilter->Get_Field_Count(), pFilter->Get_Count());

	{
		for(int iy=0; iy<Filter.Get_NY(); iy++)
		{
			CSG_Table_Record	*pRecord	= pFilter->Get_Record(iy);

			for(int ix=0; ix<Filter.Get_NX(); ix++)
			{
				Filter[iy][ix]	= pRecord->asDouble(ix);
			}
		}
	}

	int	nx	= (Filter.Get_NX() - 1) / 2;
	int	ny	= (Filter.Get_NY() - 1) / 2;

	//-----------------------------------------------------
	CSG_Grid	*pInput 	= Parameters("INPUT" )->asGrid();
	CSG_Grid	*pResult	= Parameters("RESULT")->asGrid();

	if( !pResult || pResult == pInput )
	{
		pResult	= SG_Create_Grid(pInput);
	}
	else
	{
		pResult->Fmt_Name("%s [%s]", pInput->Get_Name(), _TL("Filter"));

		pResult->Set_NoData_Value(pInput->Get_NoData_Value());
	}

	//-----------------------------------------------------
	bool	bAbsolute	= Parameters("ABSOLUTE")->asBool();

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			double	s	= 0.0;
			double	n	= 0.0;

			if( pInput->is_InGrid(x, y) )
			{
				for(int iy=0, jy=y-ny; iy<Filter.Get_NY(); iy++, jy++)
				{
					for(int ix=0, jx=x-nx; ix<Filter.Get_NX(); ix++, jx++)
					{
						if( pInput->is_InGrid(jx, jy) )
						{
							s	+=      Filter[iy][ix] * pInput->asDouble(jx, jy);
							n	+= fabs(Filter[iy][ix]);
						}
					}
				}
			}

			if( n > 0.0 )
			{
				pResult->Set_Value(x, y, bAbsolute ? s : s / n);
			}
			else
			{
				pResult->Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	if( !Parameters("RESULT")->asGrid() || Parameters("RESULT")->asGrid() == pInput )
	{
		pInput->Assign(pResult);

		delete(pResult);

		DataObject_Update(pInput);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

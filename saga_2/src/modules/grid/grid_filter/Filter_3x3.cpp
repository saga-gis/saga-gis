
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
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
	Set_Name		(_TL("User Defined Filter (3x3)"));

	Set_Author		(SG_T("(c) 2001 by O.Conrad"));

	Set_Description	(_TW(
		"User defined 3x3 sub-window filter. The filter is entered as a table with 3 rows and 3 columns."
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL, "INPUT"	, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "RESULT"	, _TL("Filtered Grid"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	//-----------------------------------------------------
	CSG_Table			Filter, *pFilter;
	CSG_Table_Record	*pRecord;

	Filter.Add_Field("1", TABLE_FIELDTYPE_Double);
	Filter.Add_Field("2", TABLE_FIELDTYPE_Double);
	Filter.Add_Field("3", TABLE_FIELDTYPE_Double);

	Parameters.Add_FixedTable(NULL, "FILTER"	, _TL("Filter Matrix")	, _TL(""), &Filter);
	pFilter	= Parameters("FILTER")->asTable();

	pRecord	= pFilter->Add_Record();
	pRecord->Set_Value(0, 0.25);
	pRecord->Set_Value(1, 0.5);
	pRecord->Set_Value(2, 0.25);

	pRecord	= pFilter->Add_Record();
	pRecord->Set_Value(0, 0.5);
	pRecord->Set_Value(1,-1.0);
	pRecord->Set_Value(2, 0.5);

	pRecord	= pFilter->Add_Record();
	pRecord->Set_Value(0, 0.25);
	pRecord->Set_Value(1, 0.5);
	pRecord->Set_Value(2, 0.25);
}

//---------------------------------------------------------
CFilter_3x3::~CFilter_3x3(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFilter_3x3::On_Execute(void)
{
	int				x, y, ix, iy, dx, dy, fx, fy, fdx, fdy;
	double			Sum, nSum, **f;
	CSG_Grid			*pInput, *pResult;
	CSG_Table			*pFilter;
	CSG_Table_Record	*pRecord;

	//-----------------------------------------------------
	pInput	= Parameters("INPUT")->asGrid();
	pResult	= Parameters("RESULT")->asGrid();

	//-----------------------------------------------------
	pFilter	= Parameters("FILTER")->asTable();

	fdx		= pFilter->Get_Field_Count();
	fdy		= pFilter->Get_Record_Count();
	dx		= (fdx - 1) / 2;
	dy		= (fdy - 1) / 2;

	f		= (double **)SG_Malloc(fdy * sizeof(double *));
	f[0]	= (double  *)SG_Malloc(fdy * fdx * sizeof(double));

	for(fy=0; fy<fdy; fy++)
	{
		f[fy]	= f[0] + fy * fdx;

		pRecord	= pFilter->Get_Record(fy);

		for(fx=0; fx<fdx; fx++)
		{
			f[fy][fx]	= pRecord->asDouble(fx);
		}
	}

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			Sum		= nSum	= 0.0;

			for(fy=0, iy=y-dy; fy<fdy; fy++, iy++)
			{
				for(fx=0, ix=x-dx; fx<fdx; fx++, ix++)
				{
					if( pInput->is_InGrid(ix, iy) )
					{
						Sum		+= f[fy][fx] * pInput->asDouble(ix, iy);
						nSum	+= fabs(f[fy][fx]);
					}
				}
			}

			if( nSum > 0.0 )
			{
				pResult->Set_Value(x, y, Sum / nSum);
			}
			else
			{
				pResult->Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	SG_Free(f[0]);
	SG_Free(f);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

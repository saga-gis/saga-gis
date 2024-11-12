
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
#include "Filter_3x3.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFilter_3x3::CFilter_3x3(void)
{
	Set_Name		(_TL("User Defined Filter"));

	Set_Author		("O.Conrad (c) 2001");

	Set_Description	(_TW(
		"User defined filter matrix. The filter can be chosen from loaded tables. "
		"If not specified a fixed table with 3 rows (and 3 columns) will be used. "
	));

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"INPUT"     , _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"RESULT"    , _TL("Filtered Grid"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Table("",
		"FILTER"    , _TL("Kernel"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_FixedTable("FILTER",
		"FILTER_3X3", _TL("3x3 Kernel"),
		_TL("")
	);

	Parameters.Add_Bool("",
		"ABSOLUTE"  , _TL("Absolute Weighting"),
		_TL("If not checked to be absolute resulting sum will become divided by the sum of filter kernel's weights."),
		true
	);

	//-----------------------------------------------------
	CSG_Table &Kernel = *Parameters("FILTER_3X3")->asTable();

	Kernel.Add_Field("1", SG_DATATYPE_Double);
	Kernel.Add_Field("2", SG_DATATYPE_Double);
	Kernel.Add_Field("3", SG_DATATYPE_Double);

	Kernel.Set_Count(3);

	Kernel[0][0] = 0.5; Kernel[0][1] = 1.0;	Kernel[0][2] = 0.5;
	Kernel[1][0] = 1.0; Kernel[1][1] =-6.0;	Kernel[1][2] = 1.0;
	Kernel[2][0] = 0.5; Kernel[2][1] = 1.0;	Kernel[2][2] = 0.5;
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CFilter_3x3::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("FILTER") )
	{
		pParameters->Set_Enabled("FILTER_3X3", pParameter->asTable() == NULL);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFilter_3x3::On_Execute(void)
{
	CSG_Table *pKernel = Parameters("FILTER")->asTable()
		? Parameters("FILTER"    )->asTable()
		: Parameters("FILTER_3X3")->asTable();

	if( pKernel->Get_Count() < 1 || pKernel->Get_Field_Count() < 1 )
	{
		Error_Set(_TL("invalid filter matrix"));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Matrix Kernel(pKernel->Get_Field_Count(), pKernel->Get_Count());

	for(int iy=0; iy<Kernel.Get_NY(); iy++)
	{
		CSG_Table_Record *pRecord = pKernel->Get_Record(iy);

		for(int ix=0; ix<Kernel.Get_NX(); ix++)
		{
			Kernel[iy][ix] = pRecord->asDouble(ix);
		}
	}

	int nx = (Kernel.Get_NX() - 1) / 2;
	int ny = (Kernel.Get_NY() - 1) / 2;

	//-----------------------------------------------------
	CSG_Grid *pInput  = Parameters("INPUT" )->asGrid();
	CSG_Grid *pResult = Parameters("RESULT")->asGrid(), Result;

	if( !pResult || pResult == pInput )
	{
		pResult = &Result; Result.Create(*pInput);
	}
	else
	{
		pResult->Fmt_Name("%s [%s]", pInput->Get_Name(), _TL("User Defined Filter"));
	}

	//-----------------------------------------------------
	bool bAbsolute = Parameters("ABSOLUTE")->asBool();

	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			CSG_Simple_Statistics s;

			if( pInput->is_InGrid(x, y) )
			{
				for(int iy=0, jy=y-ny; iy<Kernel.Get_NY(); iy++, jy++)
				{
					for(int ix=0, jx=x-nx; ix<Kernel.Get_NX(); ix++, jx++)
					{
						if( pInput->is_InGrid(jx, jy) )
						{
							s.Add_Value(pInput->asDouble(jx, jy), Kernel[iy][ix]);
						}
					}
				}
			}

			if( s.Get_Count() > 0 )
			{
				pResult->Set_Value(x, y, bAbsolute ? s.Get_Sum() : s.Get_Mean());
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

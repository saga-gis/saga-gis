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
//                  Grid_Visualisation                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 Grid_Color_Blend.cpp                  //
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
#include "Grid_Color_Blend.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Color_Blend::CGrid_Color_Blend(void)
{
	Set_Name(_TL("Color Blending"));

	Set_Author		(SG_T("(c) 2001 by O.Conrad"));

	Set_Description(
		_TL("Creates a color animation based on the values of selected grids.\n")
	);

	Parameters.Add_Grid(
		NULL	, "GRID"	, _TL("Grid"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid_List(
		NULL	, "GRIDS"	, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Value(
		NULL	, "NSTEPS"	, _TL("Interpolation Steps"),
		_TL(""),
		PARAMETER_TYPE_Int	, 0, 0, true
	);

	Parameters.Add_Value(
		NULL	, "LOOP"	, _TL("Loop"),
		_TL(""),
		PARAMETER_TYPE_Bool	, true
	);

	Parameters.Add_Choice(
		NULL	, "RANGE"	, _TL("Color Stretch"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("fit to each grid"),
			_TL("fit to overall range"),
			_TL("fit to overall standard deviation")
		), 3
	);

	Parameters.Add_Value(
		NULL	, "STDDEV"	, _TL("Standard Deviation"),
		_TL(""),
		PARAMETER_TYPE_Double, 2.0, 0.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Color_Blend::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "RANGE") )
	{
		pParameters->Get_Parameter("STDDEV")->Set_Enabled(pParameter->asInt() == 2);
	}

	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Color_Blend::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Parameter_Grid_List	*pGrids	= Parameters("GRIDS")->asGridList();

	if( pGrids->Get_Count() < 2 )
	{
		return( false );
	}

	//-----------------------------------------------------
	switch( Parameters("RANGE")->asInt() )
	{
	case 0:	// fit to each grid
		{
			m_Range_Min	= 0.0;
			m_Range_Max	= 0.0;
		}
		break;

	case 1:	// fit to overall range
		{
			m_Range_Min	= pGrids->asGrid(0)->Get_Min();
			m_Range_Max	= pGrids->asGrid(0)->Get_Max();

			for(int i=1; i<pGrids->Get_Count(); i++)
			{
				if( m_Range_Min > pGrids->asGrid(i)->Get_Min() )	m_Range_Min	= pGrids->asGrid(i)->Get_Min();
				if( m_Range_Max < pGrids->asGrid(i)->Get_Max() )	m_Range_Max	= pGrids->asGrid(i)->Get_Max();
			}
		}
		break;

	case 2:	// fit to overall standard deviation
		{
			double	StdDev	= Parameters("STDDEV")->asDouble();

			m_Range_Min	= pGrids->asGrid(0)->Get_Mean() - StdDev * pGrids->asGrid(0)->Get_StdDev();
			m_Range_Max	= pGrids->asGrid(0)->Get_Mean() + StdDev * pGrids->asGrid(0)->Get_StdDev();

			for(int i=1; i<pGrids->Get_Count(); i++)
			{
				double	Min	= pGrids->asGrid(i)->Get_Mean() - StdDev * pGrids->asGrid(i)->Get_StdDev();
				double	Max	= pGrids->asGrid(i)->Get_Mean() + StdDev * pGrids->asGrid(i)->Get_StdDev();

				if( m_Range_Min > Min )	m_Range_Min	= Min;
				if( m_Range_Max < Max )	m_Range_Max	= Max;
			}
		}
		break;
	}

	//-----------------------------------------------------
	m_pGrid	= Parameters("GRID")->asGrid();
	m_pGrid	->Set_Name(_TL("Color Blending"));
	m_pGrid	->Assign(pGrids->asGrid(0));

	DataObject_Update(m_pGrid, SG_UI_DATAOBJECT_SHOW);

	do
	{
		for(int i=1; i<pGrids->Get_Count() && Set_Progress(i, pGrids->Get_Count() - 1); i++)
		{
			Blend(pGrids->asGrid(i - 1), pGrids->asGrid(i));
		}

		if( Parameters("LOOP")->asBool() && Process_Get_Okay() )
		{
			Blend(pGrids->asGrid(pGrids->Get_Count() - 1), pGrids->asGrid(0));
		}
	}
	while( Parameters("LOOP")->asBool() && Process_Get_Okay() );

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CGrid_Color_Blend::Blend(CSG_Grid *pA, CSG_Grid *pB)
{
	int	nSteps	= 1 + Parameters("NSTEPS")->asInt();

	for(int i=1; i<nSteps && Process_Get_Okay(); i++)
	{
		double	d	= i / (double)nSteps;

		#pragma omp parallel for
		for(int y=0; y<Get_NY(); y++)
		{
			for(int x=0; x<Get_NX(); x++)
			{
				double	a	= pA->asDouble(x, y);
				double	b	= pB->asDouble(x, y);

				m_pGrid->Set_Value(x, y, a + d * (b - a));
			}
		}

		if( m_Range_Min < m_Range_Max )
		{
			DataObject_Update(m_pGrid, m_Range_Min, m_Range_Max);
		}
		else
		{
			DataObject_Update(m_pGrid);
		}
	}

	//-----------------------------------------------------
	SG_UI_Progress_Lock(true);
	m_pGrid->Assign(pB);
	SG_UI_Progress_Lock(false);

	if( m_Range_Min < m_Range_Max )
	{
		DataObject_Update(m_pGrid, m_Range_Min, m_Range_Max);
	}
	else
	{
		DataObject_Update(m_pGrid);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

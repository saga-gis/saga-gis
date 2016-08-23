/**********************************************************
 * Version $Id: grids_trend_polynom.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                 statistics_regression                 //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                grids_trend_polynom.cpp                //
//                                                       //
//                 Copyright (C) 2011 by                 //
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
#include "grids_trend_polynom.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrids_Trend::CGrids_Trend(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Polynomial Trend from Grids"));

	Set_Author		(SG_T("O. Conrad (c) 2011"));

	Set_Description	(_TW(
		"Fits for each cell a polynomial trend function. "
		"Outputs are the polynomial coefficients for the "
		"polynomial trend function of chosen order. "
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL	, "Y_GRIDS"	, _TL("Dependent Variables"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List(
		NULL	, "COEFF"	, _TL("Polynomial Coefficients"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "R2"		, _TL("Coefficient of Determination"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Value(
		NULL	, "ORDER"	, _TL("Polynomial Order"),
		_TL(""),
		PARAMETER_TYPE_Int, 1, 1, true
	);

	Parameters.Add_Choice(
		NULL	, "XSOURCE"	, _TL("Get Independent Variable from ..."),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
			_TL("list order"),
			_TL("table"),
			_TL("grid list")
		), 0
	);

	Parameters.Add_FixedTable(
		NULL	, "X_TABLE"	, _TL("Independent Variable (per Grid)"),
		_TL("")
	)->asTable()->Add_Field(_TL("Value"), SG_DATATYPE_Double);

	Parameters.Add_Grid_List(
		NULL	, "X_GRIDS"	, _TL("Independent Variable (per Grid and Cell)"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	//-----------------------------------------------------
	CSG_Table	*pTable	= Parameters("X_TABLE")->asTable();

	pTable->Add_Record()->Set_Value(0, 1000.0);
	pTable->Add_Record()->Set_Value(0,  925.0);
	pTable->Add_Record()->Set_Value(0,  850.0);
	pTable->Add_Record()->Set_Value(0,  700.0);
	pTable->Add_Record()->Set_Value(0,  600.0);
	pTable->Add_Record()->Set_Value(0,  500.0);
	pTable->Add_Record()->Set_Value(0,  400.0);
	pTable->Add_Record()->Set_Value(0,  300.0);
	pTable->Add_Record()->Set_Value(0,  250.0);
	pTable->Add_Record()->Set_Value(0,  200.0);
	pTable->Add_Record()->Set_Value(0,  150.0);
	pTable->Add_Record()->Set_Value(0,  100.0);
	pTable->Add_Record()->Set_Value(0,   70.0);
	pTable->Add_Record()->Set_Value(0,   50.0);
	pTable->Add_Record()->Set_Value(0,   30.0);
	pTable->Add_Record()->Set_Value(0,   20.0);
	pTable->Add_Record()->Set_Value(0,   10.0);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrids_Trend::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("XSOURCE")) )
	{
		pParameters->Get_Parameter("X_TABLE")->Set_Enabled(pParameter->asInt() == 1);	// table
		pParameters->Get_Parameter("X_GRIDS")->Set_Enabled(pParameter->asInt() == 2);	// grid list
	}

	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrids_Trend::On_Execute(void)
{
	int						Order, xSource, nGrids;
	CSG_Table				*pXTable;
	CSG_Grid				*pR2;
	CSG_Parameter_Grid_List	*pYGrids, *pCoeff, *pXGrids;

	//-----------------------------------------------------
	pYGrids	= Parameters("Y_GRIDS")->asGridList();
	pCoeff	= Parameters("COEFF"  )->asGridList();
	pR2		= Parameters("R2"     )->asGrid();
	pXGrids	= Parameters("X_GRIDS")->asGridList();
	pXTable	= Parameters("X_TABLE")->asTable();
	Order	= Parameters("ORDER"  )->asInt();
	xSource	= Parameters("XSOURCE")->asInt();

	//-----------------------------------------------------
	nGrids	= pYGrids->Get_Count();

	if( nGrids <= Order )
	{
		Error_Set(_TL("fitting a polynom of ith order needs at least i + 1 samples"));

		return( false );
	}

	//-----------------------------------------------------
	switch( xSource )
	{
	case 0:	nGrids	= pYGrids->Get_Count();	break;	// list order
	case 1:	nGrids	= pXTable->Get_Count();	break;	// table
	case 2:	nGrids	= pXGrids->Get_Count();	break;	// grid list
	}

	if( nGrids < pXGrids->Get_Count() )
	{
		Error_Set(_TL("There are less predictor variables then dependent ones."));

		return( false );
	}

	if( nGrids > pXGrids->Get_Count() )
	{
		Message_Add(_TL("Warning: there are more predictor variables then dependent ones, surplus will be ignored."));

		nGrids	= pYGrids->Get_Count();
	}

	//-----------------------------------------------------
	pCoeff->Del_Items();

	for(int i=0; i<=Order; i++)
	{
		pCoeff->Add_Item(SG_Create_Grid(*Get_System()));
		pCoeff->asGrid(i)->Set_Name(CSG_String::Format(SG_T("%s [%d]"), _TL("Polynomial Coefficient"), i + 1));
	}

	if( pR2 )
	{
		pR2->Set_Name(CSG_String::Format(SG_T("%s"), _TL("Determination Coefficients")));
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			CSG_Trend_Polynom	Trend;

			Trend.Set_Order(Order);

			for(int i=0; i<nGrids; i++)
			{
				if( !pYGrids->asGrid(i)->is_NoData(x, y) )
				{
					switch( xSource )
					{
					case 0:	// list order
						Trend.Add_Data(i, pYGrids->asGrid(i)->asDouble(x, y));
						break;

					case 1:	// table
						Trend.Add_Data(pXTable->Get_Record(i)->asDouble(0), pYGrids->asGrid(i)->asDouble(x, y));
						break;

					case 2:	// grid list
						if( !pXGrids->asGrid(i)->is_NoData(x, y) )
						{
							Trend.Add_Data(pXGrids->asGrid(i)->asDouble(x, y), pYGrids->asGrid(i)->asDouble(x, y));
						}
						break;
					}
				}
			}

			if( Trend.Get_Trend() )
			{
				for(int iOrder=0; iOrder<Trend.Get_nCoefficients(); iOrder++)
				{
					pCoeff->asGrid(iOrder)->Set_Value(x, y, Trend.Get_Coefficient(iOrder));
				}

				if( pR2 )	pR2->Set_Value(x, y, Trend.Get_R2());
			}
			else
			{
				for(int iOrder=0; iOrder<Trend.Get_nCoefficients(); iOrder++)
				{
					pCoeff->asGrid(iOrder)->Set_NoData(x, y);
				}

				if( pR2 )	pR2->Set_NoData(x, y);
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

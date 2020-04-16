
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
#include "grids_trend_polynom.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrids_Trend::CGrids_Trend(void)
{
	Set_Name		(_TL("Cellwise Trend for Grids"));

	Set_Author		("O.Conrad (c) 2011");

	Set_Description	(_TW(
		"Fits for each cell a polynomial trend function. "
		"Outputs are the polynomial coefficients for the "
		"polynomial trend function of chosen order. "
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List("",
		"Y_GRIDS"	, _TL("Dependent Variables"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List("",
		"COEFF"		, _TL("Polynomial Coefficients"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid("", "R2"    , _TL("Determination Coefficient"         ), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid("", "R2ADJ" , _TL("Adjusted Determination Coefficient"), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid("", "STDERR", _TL("Standard Error"                    ), _TL(""), PARAMETER_OUTPUT_OPTIONAL);
	Parameters.Add_Grid("", "P"     , _TL("Significance Level"                ), _TL(""), PARAMETER_OUTPUT_OPTIONAL);

	Parameters.Add_Bool("",
		"LINEAR"	, _TL("Linear Trend"),
		_TL(""),
		true
	);

	Parameters.Add_Int("",
		"ORDER"		, _TL("Polynomial Order"),
		_TL("Order of the polynomial trend function."),
		2, 1, true
	);

	Parameters.Add_Choice("",
		"XSOURCE"	, _TL("Get Independent Variable from ..."),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("list order"),
			_TL("table"),
			_TL("grid list")
		), 0
	);

	Parameters.Add_FixedTable("",
		"X_TABLE"	, _TL("Independent Variable (per Grid)"),
		_TL("")
	)->asTable()->Add_Field(_TL("Value"), SG_DATATYPE_Double);

	Parameters.Add_Grid_List("",
		"X_GRIDS"	, _TL("Independent Variable (per Grid and Cell)"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrids_Trend::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("Y_GRIDS") )
	{
		int	nGrids	= (*pParameters)["Y_GRIDS"].asGridList()->Get_Grid_Count();

		CSG_Table	*pTable	= (*pParameters)["X_TABLE"].asTable();

		if( nGrids < pTable->Get_Count() )
		{
			pTable->Set_Count(nGrids);
		}
		else for(int i=pTable->Get_Count(); i<nGrids; i++)
		{
			pTable->Add_Record()->Set_Value(0, i + 1);
		}
	}

	return( CSG_Tool_Grid::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CGrids_Trend::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	pParameter->Cmp_Identifier("LINEAR") )
	{
		pParameters->Set_Enabled("ORDER"  , pParameter->asBool() == false);
		pParameters->Set_Enabled("R2ADJ"  , pParameter->asBool() ==  true);
		pParameters->Set_Enabled("STDERR" , pParameter->asBool() ==  true);
		pParameters->Set_Enabled("P"      , pParameter->asBool() ==  true);
	}

	if(	pParameter->Cmp_Identifier("XSOURCE") )
	{
		pParameters->Set_Enabled("X_TABLE", pParameter->asInt() == 1);	// table
		pParameters->Set_Enabled("X_GRIDS", pParameter->asInt() == 2);	// grid list
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrids_Trend::On_Execute(void)
{
	CSG_Parameter_Grid_List *pYGrids = Parameters("Y_GRIDS")->asGridList();

	CSG_Parameter_Grid_List *pXGrids = Parameters("X_GRIDS")->asGridList();
	CSG_Table               *pXTable = Parameters("X_TABLE")->asTable();

	//-----------------------------------------------------
	int	nGrids, xSource	= Parameters("XSOURCE")->asInt();

	switch( xSource )
	{
	default: nGrids = pYGrids->Get_Grid_Count(); break;	// list order
	case  1: nGrids = pXTable->Get_Count     (); break;	// table
	case  2: nGrids = pXGrids->Get_Grid_Count(); break;	// grid list
	}

	if( nGrids < pYGrids->Get_Grid_Count() )
	{
		Error_Set(_TL("There are less predictor variables then dependent ones."));

		return( false );
	}

	if( nGrids > pYGrids->Get_Grid_Count() )
	{
		Message_Add(_TL("Warning: there are more predictor variables then dependent ones, surplus will be ignored."));

		nGrids	= pYGrids->Get_Grid_Count();
	}

	//-----------------------------------------------------
	bool	bLinear	= Parameters("LINEAR")->asBool();

	int	Order	= bLinear ? 1 : Parameters("ORDER")->asInt();

	if( nGrids <= Order )
	{
		Error_Set(_TL("fitting a polynom of ith order needs at least i + 1 samples"));

		return( false );
	}

	Message_Fmt("\nTrend function: a0 + a1*x");

	for(int i=2; i<=Order; i++)
	{
		Message_Fmt(" + a%d*x^%d", i, i);
	}

	//-----------------------------------------------------
	CSG_Parameter_Grid_List	*pCoeffs	= Parameters("COEFF")->asGridList();

	pCoeffs->Del_Items();

	for(int i=0; i<=Order; i++)
	{
		CSG_Grid	*pCoeff	= SG_Create_Grid(Get_System());

		if( i == 0 )
		{
			pCoeff->Fmt_Name("%s [a0]", _TL("Intercept"));
		}
		else
		{
			pCoeff->Fmt_Name("%s %d [a%d]", _TL("Coefficient"), i, i);
		}

		pCoeffs->Add_Item(pCoeff);
	}

	CSG_Grid *pR2     = Parameters("R2"    )->asGrid();
	CSG_Grid *pR2adj  = Parameters("R2ADJ" )->asGrid();
	CSG_Grid *pStdErr = Parameters("STDERR")->asGrid();
	CSG_Grid *pP      = Parameters("P"     )->asGrid();

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			CSG_Matrix	Samples;

			for(int i=0; i<nGrids; i++)
			{
				if( !pYGrids->Get_Grid(i)->is_NoData(x, y) )
				{
					CSG_Vector	Sample; Sample.Add_Row(pYGrids->Get_Grid(i)->asDouble(x, y));

					switch( xSource )
					{
					default: {
						Sample.Add_Row(i);
						break; }	// list order

					case  1: {
						Sample.Add_Row(pXTable->Get_Record(i)->asDouble(0));
						break; }	// table

					case  2: if( !pXGrids->Get_Grid(i)->is_NoData(x, y) ) {
						Sample.Add_Row(pXGrids->Get_Grid(i)->asDouble(x, y));
						break; }	// grid list
					}

					if( Sample.Get_Size() == 2 )
					{
						Samples.Add_Row(Sample);
					}
				}
			}

			//---------------------------------------------
			bool	bOkay	= false;

			if( Samples.Get_NRows() > Order )
			{
				if( bLinear )
				{
					CSG_Regression_Multiple	Trend;

					if( (bOkay = Trend.Get_Model(Samples)) )
					{
						pCoeffs->Get_Grid(0) ->Set_Value(x, y, Trend.Get_RConst  ());
						pCoeffs->Get_Grid(1) ->Set_Value(x, y, Trend.Get_RCoeff (0));
						if( pR2     ) pR2    ->Set_Value(x, y, Trend.Get_R2      ());
						if( pR2adj  ) pR2adj ->Set_Value(x, y, Trend.Get_R2_Adj  ());
						if( pStdErr ) pStdErr->Set_Value(x, y, Trend.Get_StdError());
						if( pP      ) pP     ->Set_Value(x, y, Trend.Get_P       ());
					}
				}
				else // if( !bLinear )
				{
					CSG_Trend_Polynom	Trend; Trend.Set_Order(Order);

					for(int i=0; i<Samples.Get_NRows(); i++)
					{
						Trend.Add_Data(Samples[i][1], Samples[i][0]);
					}

					if( (bOkay = Trend.Get_Trend()) )
					{
						for(int i=0; i<=Order; i++)
						{
							pCoeffs->Get_Grid(i)->Set_Value(x, y, Trend.Get_Coefficient(i));
						}

						if( pR2 ) pR2->Set_Value(x, y, Trend.Get_R2());
					}
				}
			}

			//---------------------------------------------
			if( !bOkay )
			{
				for(int i=0; i<=Order; i++)
				{
					pCoeffs->Get_Grid(i)->Set_NoData(x, y);
				}

				if( pR2     ) pR2    ->Set_NoData(x, y);
				if( pR2adj  ) pR2adj ->Set_NoData(x, y);
				if( pStdErr ) pStdErr->Set_NoData(x, y);
				if( pP      ) pP     ->Set_NoData(x, y);
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

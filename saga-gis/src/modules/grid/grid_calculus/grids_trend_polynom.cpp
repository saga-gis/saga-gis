/**********************************************************
 * Version $Id: grids_trend_polynom.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     Grid_Calculus                     //
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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
		"Output are the polynomial coefficients for the chosen trend function. "
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL	, "GRIDS"	, _TL("Dependent Variables"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List(
		NULL	, "Y_GRIDS"	, _TL("Independent Variable (per Grid and Cell)"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_FixedTable(
		NULL	, "Y_TABLE"	, _TL("Independent Variable (per Grid)"),
		_TL("")
	)->asTable()->Add_Field(_TL("Value"), SG_DATATYPE_Double);

	Parameters.Add_Grid_List(
		NULL	, "PARMS"	, _TL("Polynomial Coefficients"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "QUALITY"	, _TL("Coefficient of Determination"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice(
		NULL	, "POLYNOM"	, _TL("Type of Approximated Function"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|"),
			_TL("first order polynom (linear regression)"),
			_TL("second order polynom"),
			_TL("third order polynom"),
			_TL("fourth order polynom"),
			_TL("fifth order polynom")
		), 2
	);

	//-----------------------------------------------------
	CSG_Table	*pTable	= Parameters("Y_TABLE")->asTable();

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
bool CGrids_Trend::On_Execute(void)
{
	int						i, nGrids;
	CSG_Trend				Trend;
	CSG_Table				*pYTable;
	CSG_Grid				*pQuality;
	CSG_Parameter_Grid_List	*pGrids, *pYGrids, *pParms;

	//-----------------------------------------------------
	pGrids		= Parameters("GRIDS")	->asGridList();
	pYGrids		= Parameters("Y_GRIDS")	->asGridList();
	pYTable		= Parameters("Y_TABLE")	->asTable();
	pParms		= Parameters("PARMS")	->asGridList();
	pQuality	= Parameters("QUALITY")	->asGrid();

	//-----------------------------------------------------
	nGrids		= pYGrids->Get_Count() > 0 ? pYGrids->Get_Count() : pYTable->Get_Count();

	if( nGrids > pGrids->Get_Count() )
	{
		nGrids	= pGrids->Get_Count();
	}

	//-----------------------------------------------------
	switch( Parameters("POLYNOM")->asInt() )
	{
	default:
	case 0:	Trend.Set_Formula(SG_T("a+b*x"));							break;
	case 1:	Trend.Set_Formula(SG_T("a+b*x+c*x*x"));						break;
	case 2:	Trend.Set_Formula(SG_T("a+b*x+c*x*x+d*x^3"));				break;
	case 3:	Trend.Set_Formula(SG_T("a+b*x+c*x*x+d*x^3+e*x^4"));			break;
	case 4:	Trend.Set_Formula(SG_T("a+b*x+c*x*x+d*x^3+e*x^4+f*x^5"));	break;
	}

	if( nGrids < Trend.Get_Parameter_Count() + 1 )
	{
		Error_Set(_TL("fitting a polynom of ith order needs at least i + 1 parameter sets given"));

		return( false );
	}

	//-----------------------------------------------------
	pParms->Del_Items();

	for(i=0; i<Trend.Get_Parameter_Count(); i++)
	{
		pParms->Add_Item(SG_Create_Grid(*Get_System()));
		pParms->asGrid(i)->Set_Name(CSG_String::Format(SG_T("%s [%d]"), _TL("Polynomial Coefficient"), i + 1));
	}

	//-----------------------------------------------------
	if( pQuality )
	{
		pQuality->Set_Name(CSG_String::Format(SG_T("%s"), _TL("Polynomial Trend Quality")));
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			Trend.Clr_Data();

			for(i=0; i<nGrids; i++)
			{
				if( !pGrids->asGrid(i)->is_NoData(x, y) )
				{
					if( pYGrids )
					{
						Trend.Add_Data(pYGrids->asGrid(i)->asDouble(x, y) , pGrids->asGrid(i)->asDouble(x, y));
					}
					else
					{
						Trend.Add_Data(pYTable->Get_Record(i)->asDouble(0), pGrids->asGrid(i)->asDouble(x, y));
					}
				}
			}

			if( Trend.Get_Trend() )
			{
				for(i=0; i<Trend.Get_Parameter_Count(); i++)
				{
					pParms->asGrid(i)->Set_Value(x, y, Trend.Get_Parameters()[i]);
				}

				if( pQuality )	pQuality->Set_Value(x, y, Trend.Get_R2());
			}
			else
			{
				for(i=0; i<Trend.Get_Parameter_Count(); i++)
				{
					pParms->asGrid(i)->Set_NoData(x, y);
				}

				if( pQuality )	pQuality->Set_NoData(x, y);
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

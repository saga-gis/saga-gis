
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
//                Grid_Color_Triangle.cpp                //
//                                                       //
//                 Copyright (C) 2008 by                 //
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
#include "Grid_Color_Triangle.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define METHOD_STRING	CSG_String::Format("%s|%s|%s|%s|%s",\
	_TL("0 - 1"),\
	_TL("Rescale to 0 - 1"),\
	_TL("User defined rescale"),\
	_TL("Percentiles"),\
	_TL("Percentage of standard deviation")\
), 4


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Color_Triangle::CGrid_Color_Triangle(void)
{
	Set_Name		(_TL("Color Triangle Composite"));

	Set_Author		("O.Conrad (c) 2008");

	Set_Description	(_TW(
		"Similar to 'RGB Composite', but the three colors representing intensity of each data set can be chosen by user. "
	));

	//-----------------------------------------------------
	Parameters.Add_Grid  (""      , "A_GRID"   , _TL("A"                               ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Color ("A_GRID", "A_COLOR"  , _TL("Color"                           ), _TL(""), SG_COLOR_RED);
	Parameters.Add_Choice("A_GRID", "A_METHOD" , _TL("Value Preparation"               ), _TL(""), METHOD_STRING);
	Parameters.Add_Range ("A_GRID", "A_RANGE"  , _TL("Rescale Range"                   ), _TL(""), 0., 1.);
	Parameters.Add_Range ("A_GRID", "A_PERCTL" , _TL("Percentiles"                     ), _TL(""), 1., 99., 0., true, 100., true);
	Parameters.Add_Double("A_GRID", "A_PERCENT", _TL("Percentage of Standard Deviation"), _TL(""), 150., 0., true);

	Parameters.Add_Grid  (""      , "B_GRID"   , _TL("B"                               ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Color ("B_GRID", "B_COLOR"  , _TL("Color"                           ), _TL(""), SG_COLOR_GREEN);
	Parameters.Add_Choice("B_GRID", "B_METHOD" , _TL("Value Preparation"               ), _TL(""), METHOD_STRING);
	Parameters.Add_Range ("B_GRID", "B_RANGE"  , _TL("Rescale Range"                   ), _TL(""), 0., 1.);
	Parameters.Add_Range ("B_GRID", "B_PERCTL" , _TL("Percentiles"                     ), _TL(""), 1., 99., 0., true, 100., true);
	Parameters.Add_Double("B_GRID", "B_PERCENT", _TL("Percentage of standard deviation"), _TL(""), 150., 0., true);

	Parameters.Add_Grid  (""      , "C_GRID"   , _TL("C"                               ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Color ("C_GRID", "C_COLOR"  , _TL("Color"                           ), _TL(""), SG_COLOR_BLUE);
	Parameters.Add_Choice("C_GRID", "C_METHOD" , _TL("Value Preparation"               ), _TL(""), METHOD_STRING);
	Parameters.Add_Range ("C_GRID", "C_RANGE"  , _TL("Rescale Range"                   ), _TL(""), 0., 1.);
	Parameters.Add_Range ("C_GRID", "C_PERCTL" , _TL("Percentiles"                     ), _TL(""), 1., 99., 0., true, 100., true);
	Parameters.Add_Double("C_GRID", "C_PERCENT", _TL("Percentage of Standard Deviation"), _TL(""), 150., 0., true);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"GRID"	, _TL("Composite"),
		_TL(""),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Int
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define CHK_COLOR(C)	if( C > 255 )	C	= 255;	else if( C < 0 )	C	= 0;

//---------------------------------------------------------
bool CGrid_Color_Triangle::On_Execute(void)
{
	int			x, y, Color[3], aC[3], bC[3], cC[3];
	double		aMin, bMin, cMin, aRange, bRange, cRange, a, b, c;
	CSG_Grid	*pA, *pB, *pC, *pRGB;

	//-----------------------------------------------------
	pA		= _Get_Grid(Parameters("A_GRID")->asGrid(), Parameters("A_METHOD")->asInt(), Parameters("A_RANGE")->asRange(), Parameters("A_PERCTL")->asRange(), Parameters("A_PERCENT")->asDouble(), aMin, aRange);
	pB		= _Get_Grid(Parameters("B_GRID")->asGrid(), Parameters("B_METHOD")->asInt(), Parameters("B_RANGE")->asRange(), Parameters("B_PERCTL")->asRange(), Parameters("B_PERCENT")->asDouble(), bMin, bRange);
	pC		= _Get_Grid(Parameters("C_GRID")->asGrid(), Parameters("C_METHOD")->asInt(), Parameters("C_RANGE")->asRange(), Parameters("C_PERCTL")->asRange(), Parameters("C_PERCENT")->asDouble(), cMin, cRange);

	x		= Parameters("A_COLOR")->asColor();
	aC[0]	= SG_GET_R(x);
	aC[1]	= SG_GET_G(x);
	aC[2]	= SG_GET_B(x);

	x		= Parameters("B_COLOR")->asColor();
	bC[0]	= SG_GET_R(x);
	bC[1]	= SG_GET_G(x);
	bC[2]	= SG_GET_B(x);

	x		= Parameters("C_COLOR")->asColor();
	cC[0]	= SG_GET_R(x);
	cC[1]	= SG_GET_G(x);
	cC[2]	= SG_GET_B(x);

	//-----------------------------------------------------
	pRGB	= Parameters("GRID")->asGrid();
	pRGB->Create(pRGB->Get_System(), SG_DATATYPE_Int);
	DataObject_Set_Colors(pRGB, 11, SG_COLORS_BLACK_WHITE);

	CSG_Parameters	Parms;

	if( DataObject_Get_Parameters(pRGB, Parms) && Parms("COLORS_TYPE") )
	{
		Parms("COLORS_TYPE")->Set_Value(3);	// Color Classification Type: RGB

		DataObject_Set_Parameters(pRGB, Parms);
	}

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( pA->is_NoData(x, y) || pB->is_NoData(x, y) || pC->is_NoData(x, y) )
			{
				pRGB->Set_NoData(x, y);
			}
			else
			{
				a	= aRange * (pA->asDouble(x, y) - aMin);	if( a > 1. )	a	= 1.;	else if( a < 0. )	a	= 0.;
				b	= bRange * (pB->asDouble(x, y) - bMin);	if( b > 1. )	b	= 1.;	else if( b < 0. )	b	= 0.;
				c	= cRange * (pC->asDouble(x, y) - cMin);	if( c > 1. )	c	= 1.;	else if( c < 0. )	c	= 0.;

				Color[0]	= (int)((a * aC[0] + b * bC[0] + c * cC[0]));	CHK_COLOR(Color[0]);
				Color[1]	= (int)((a * aC[1] + b * bC[1] + c * cC[1]));	CHK_COLOR(Color[1]);
				Color[2]	= (int)((a * aC[2] + b * bC[2] + c * cC[2]));	CHK_COLOR(Color[2]);

				pRGB->Set_Value(x, y, SG_GET_RGB(Color[0], Color[1], Color[2]));
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Grid * CGrid_Color_Triangle::_Get_Grid(CSG_Grid *pGrid, int Method, CSG_Parameter_Range *pRange, CSG_Parameter_Range *pPerctl, double Percent, double &Min, double &Range)
{
	if( pGrid )
	{
		int		x, y;
		sLong	n;

		switch( Method )
		{
		default:
		case 0:	// 0 - 1
			Min		= 0.;
			Range	= 1.;
			break;

		case 1:	// Rescale to 0 - 1
			Min		= pGrid->Get_Min();
			Range	= pGrid->Get_Range();
			Range	= Range > 0. ? 1. / Range : 0.;
			break;

		case 2:	// User defined rescale
			Min		= pRange->Get_Min();
			Range	= pRange->Get_Range();
			Range	= Range > 0. ? 1. / Range : 0.;
			break;

		case 3:	// Normalise
			n		= (sLong)(pGrid->Get_NCells() * pPerctl->Get_Min() / 100.);
			pGrid->Get_Sorted(n < 0 ? 0 : (n >= pGrid->Get_NCells() ? pGrid->Get_NCells() - 1 : n), x, y, false, false);
			Min		= pGrid->asDouble(x, y);
			n		= (sLong)(pGrid->Get_NCells() * pPerctl->Get_Max() / 100.);
			pGrid->Get_Sorted(n < 0 ? 0 : (n >= pGrid->Get_NCells() ? pGrid->Get_NCells() - 1 : n), x, y, false, false);
			Range	= pGrid->asDouble(x, y) - Min;
			Range	= Range > 0. ? 1. / Range : 0.;
			break;

		case 4:	// Standard deviation
			Range	= sqrt(pGrid->Get_Variance()) * Percent / 100.;
			Min		= pGrid->Get_Mean() - Range;
			Range	= Range * 2.;
			Range	= Range > 0. ? 1. / Range : 0.;
			break;
		}
	}

	return( pGrid );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

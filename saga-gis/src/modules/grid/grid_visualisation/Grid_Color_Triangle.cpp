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
#include "Grid_Color_Triangle.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Color_Triangle::CGrid_Color_Triangle(void)
{
	CSG_Parameter	*pNode;

	Set_Name		(_TL("Color Triangle Composite"));

	Set_Author		(SG_T("(c) 2008 by O.Conrad"));

	Set_Description	(_TW(
		"Similar to 'RGB Composite', but the three colors representing intensity of each data set can be chosen by user. "
	));

	//-----------------------------------------------------
	pNode	= NULL;

	pNode	= Parameters.Add_Grid(
		pNode	, "A_GRID"		, _TL("A"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Value(
		pNode	, "A_COLOR"		, _TL("Color"),
		_TL(""),
		PARAMETER_TYPE_Color	, SG_COLOR_RED
	);

	Parameters.Add_Choice(
		pNode	, "A_METHOD"	,	_TL("Value Preparation"),
		_TL(""),

		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|"),
			_TL("0 - 1"),
			_TL("Rescale to 0 - 1"),
			_TL("User defined rescale"),
			_TL("Percentiles"),
			_TL("Percentage of standard deviation")
		), 4
	);

	Parameters.Add_Range(
		pNode	, "A_RANGE"		, _TL("Rescale Range"),
		_TL(""),
		0, 1
	);

	Parameters.Add_Range(
		pNode	, "A_PERCTL"	, _TL("Percentiles"),
		_TL(""),
		1.0, 99.0, 0.0, true, 100.0, true
	);

	Parameters.Add_Value(
		pNode	, "A_PERCENT"	, _TL("Percentage of standard deviation"),
		_TL(""),
		PARAMETER_TYPE_Double	, 150.0, 0.0, true
	);

	//-----------------------------------------------------
	pNode	= NULL;

	pNode	= Parameters.Add_Grid(
		pNode	, "B_GRID"		, _TL("B"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Value(
		pNode	, "B_COLOR"		, _TL("Color"),
		_TL(""),
		PARAMETER_TYPE_Color	, SG_COLOR_GREEN
	);

	Parameters.Add_Choice(
		pNode	, "B_METHOD"	, _TL("Value Preparation"),
		_TL(""),

		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|"),
			_TL("0 - 1"),
			_TL("Rescale to 0 - 1"),
			_TL("User defined rescale"),
			_TL("Percentiles"),
			_TL("Percentage of standard deviation")
		), 4
	);

	Parameters.Add_Range(
		pNode	, "B_RANGE"		, _TL("Rescale Range"),
		_TL(""),
		0, 1
	);

	Parameters.Add_Range(
		pNode	, "B_PERCTL"	, _TL("Percentiles"),
		_TL(""),
		1.0, 99.0, 0.0, true, 100.0, true
	);

	Parameters.Add_Value(
		pNode	, "B_PERCENT"	, _TL("Percentage of standard deviation"),
		_TL(""),
		PARAMETER_TYPE_Double	, 150.0, 0.0, true
	);

	//-----------------------------------------------------
	pNode	= NULL;

	pNode	= Parameters.Add_Grid(
		pNode	, "C_GRID"		, _TL("C"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Value(
		pNode	, "C_COLOR"		, _TL("Color"),
		_TL(""),
		PARAMETER_TYPE_Color	, SG_COLOR_BLUE
	);

	Parameters.Add_Choice(
		pNode	, "C_METHOD"	, _TL("Value Preparation"),
		_TL(""),

		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|"),
			_TL("0 - 1.0"),
			_TL("Rescale to 0 - 1.0"),
			_TL("User defined rescale"),
			_TL("Percentiles"),
			_TL("Percentage of standard deviation")
		), 4
	);

	Parameters.Add_Range(
		pNode	, "C_RANGE"		, _TL("Rescale Range"),
		_TL(""),
		0, 1
	);

	Parameters.Add_Range(
		pNode	, "C_PERCTL"	, _TL("Percentiles"),
		_TL(""),
		1.0, 99.0, 0.0, true, 100.0, true
	);

	Parameters.Add_Value(
		pNode	, "C_PERCENT"	, _TL("Percentage of standard deviation"),
		_TL(""),
		PARAMETER_TYPE_Double	, 150.0, 0.0, true
	);

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "GRID"		, _TL("Composite"),
		_TL(""),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Int
	);
}

//---------------------------------------------------------
CGrid_Color_Triangle::~CGrid_Color_Triangle(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
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
	DataObject_Set_Colors(pRGB, 100, SG_COLORS_BLACK_WHITE);

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
				a	= aRange * (pA->asDouble(x, y) - aMin);	if( a > 1.0 )	a	= 1.0;	else if( a < 0.0 )	a	= 0.0;
				b	= bRange * (pB->asDouble(x, y) - bMin);	if( b > 1.0 )	b	= 1.0;	else if( b < 0.0 )	b	= 0.0;
				c	= cRange * (pC->asDouble(x, y) - cMin);	if( c > 1.0 )	c	= 1.0;	else if( c < 0.0 )	c	= 0.0;

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
			Min		= 0.0;
			Range	= 1.0;
			break;

		case 1:	// Rescale to 0 - 1
			Min		= pGrid->Get_ZMin();
			Range	= pGrid->Get_ZRange();
			Range	= Range > 0.0 ? 1.0 / Range : 0.0;
			break;

		case 2:	// User defined rescale
			Min		= pRange->Get_LoVal();
			Range	= pRange->Get_HiVal() - pRange->Get_LoVal();
			Range	= Range > 0.0 ? 1.0 / Range : 0.0;
			break;

		case 3:	// Normalise
			n		= (sLong)(pGrid->Get_NCells() * pPerctl->Get_LoVal() / 100.0);
			pGrid->Get_Sorted(n < 0 ? 0 : (n >= pGrid->Get_NCells() ? pGrid->Get_NCells() - 1 : n), x, y, false, false);
			Min		= pGrid->asDouble(x, y);
			n		= (sLong)(pGrid->Get_NCells() * pPerctl->Get_HiVal() / 100.0);
			pGrid->Get_Sorted(n < 0 ? 0 : (n >= pGrid->Get_NCells() ? pGrid->Get_NCells() - 1 : n), x, y, false, false);
			Range	= pGrid->asDouble(x, y) - Min;
			Range	= Range > 0.0 ? 1.0 / Range : 0.0;
			break;

		case 4:	// Standard deviation
			Range	= sqrt(pGrid->Get_Variance()) * Percent / 100.0;
			Min		= pGrid->Get_Mean() - Range;
			Range	= Range * 2.0;
			Range	= Range > 0.0 ? 1.0 / Range : 0.0;
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

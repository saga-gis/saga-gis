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
//                 Grid_RGB_Composite.cpp                //
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
#include "Grid_RGB_Composite.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define METHOD_STRING	CSG_String::Format(SG_T("%s|%s|%s|%s|%s|"),\
	_TL("take original value (0 - 255)"),\
	_TL("rescale to 0 - 255"),\
	_TL("user defined"),\
	_TL("percentiles"),\
	_TL("standard deviation")\
), 4

//---------------------------------------------------------
CGrid_RGB_Composite::CGrid_RGB_Composite(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("RGB Composite"));

	Set_Author		("O.Conrad (c) 2002");

	Set_Description	(_TW(
		"Create red-green-blue overlays of grids. "
	));

	//-----------------------------------------------------
	CSG_Parameter	*pNode;

	pNode	=
	Parameters.Add_Grid  (NULL , "R_GRID"  , _TL("Red"               ), _TL(""), PARAMETER_INPUT);
	pNode	=
	Parameters.Add_Choice(pNode, "R_METHOD", _TL("Value Preparation" ), _TL(""), METHOD_STRING);
	Parameters.Add_Range (pNode, "R_RANGE" , _TL("Rescale Range"     ), _TL(""), 0, 255);
	Parameters.Add_Range (pNode, "R_PERCTL", _TL("Percentiles"       ), _TL(""), 1.0, 99.0, 0.0, true, 100.0, true);
	Parameters.Add_Value (pNode, "R_STDDEV", _TL("Standard Deviation"), _TL(""), PARAMETER_TYPE_Double, 2.0, 0.0, true);

	pNode	=
	Parameters.Add_Grid  (NULL , "G_GRID"  , _TL("Green"             ), _TL(""), PARAMETER_INPUT);
	pNode	=
	Parameters.Add_Choice(pNode, "G_METHOD", _TL("Value Preparation" ), _TL(""), METHOD_STRING);
	Parameters.Add_Range (pNode, "G_RANGE" , _TL("Rescale Range"     ), _TL(""), 0, 255);
	Parameters.Add_Range (pNode, "G_PERCTL", _TL("Percentiles"       ), _TL(""), 1.0, 99.0, 0.0, true, 100.0, true);
	Parameters.Add_Value (pNode, "G_STDDEV", _TL("Standard Deviation"), _TL(""), PARAMETER_TYPE_Double, 2.0, 0.0, true);

	pNode	=
	Parameters.Add_Grid  (NULL , "B_GRID"  , _TL("Blue"              ), _TL(""), PARAMETER_INPUT);
	pNode	=
	Parameters.Add_Choice(pNode, "B_METHOD", _TL("Value Preparation" ), _TL(""), METHOD_STRING);
	Parameters.Add_Range (pNode, "B_RANGE" , _TL("Rescale Range"     ), _TL(""), 0, 255);
	Parameters.Add_Range (pNode, "B_PERCTL", _TL("Percentiles"       ), _TL(""), 1.0, 99.0, 0.0, true, 100.0, true);
	Parameters.Add_Value (pNode, "B_STDDEV", _TL("Standard Deviation"), _TL(""), PARAMETER_TYPE_Double, 2.0, 0.0, true);

	pNode	=
	Parameters.Add_Grid  (NULL , "A_GRID"  , _TL("Alpha"             ), _TL(""), PARAMETER_INPUT_OPTIONAL);
	pNode	=
	Parameters.Add_Choice(pNode, "A_METHOD", _TL("Value Preparation" ), _TL(""), METHOD_STRING);
	Parameters.Add_Range (pNode, "A_RANGE" , _TL("Rescale Range"     ), _TL(""), 0, 255);
	Parameters.Add_Range (pNode, "A_PERCTL", _TL("Percentiles"       ), _TL(""), 1.0, 99.0, 0.0, true, 100.0, true);
	Parameters.Add_Value (pNode, "A_STDDEV", _TL("Standard Deviation"), _TL(""), PARAMETER_TYPE_Double, 2.0, 0.0, true);

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "RGB"	, _TL("Composite"),
		_TL(""),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Int
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_RGB_Composite::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "R_GRID") )
	{
		pParameters->Set_Enabled("R_METHOD", pParameter->asGrid() != NULL);
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "R_METHOD") )
	{
		pParameters->Set_Enabled("R_RANGE" , pParameter->asInt() == 2);	// User defined rescale
		pParameters->Set_Enabled("R_PERCTL", pParameter->asInt() == 3);	// Percentiles
		pParameters->Set_Enabled("R_STDDEV", pParameter->asInt() == 4);	// Percentage of standard deviation
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "G_GRID") )
	{
		pParameters->Set_Enabled("G_METHOD", pParameter->asGrid() != NULL);
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "G_METHOD") )
	{
		pParameters->Set_Enabled("G_RANGE" , pParameter->asInt() == 2);	// User defined rescale
		pParameters->Set_Enabled("G_PERCTL", pParameter->asInt() == 3);	// Percentiles
		pParameters->Set_Enabled("G_STDDEV", pParameter->asInt() == 4);	// Percentage of standard deviation
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "B_GRID") )
	{
		pParameters->Set_Enabled("B_METHOD", pParameter->asGrid() != NULL);
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "B_METHOD") )
	{
		pParameters->Set_Enabled("B_RANGE" , pParameter->asInt() == 2);	// User defined rescale
		pParameters->Set_Enabled("B_PERCTL", pParameter->asInt() == 3);	// Percentiles
		pParameters->Set_Enabled("B_STDDEV", pParameter->asInt() == 4);	// Percentage of standard deviation
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "A_GRID") )
	{
		pParameters->Set_Enabled("A_METHOD", pParameter->asGrid() != NULL);
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "A_METHOD") )
	{
		pParameters->Set_Enabled("A_RANGE" , pParameter->asInt() == 2);	// User defined rescale
		pParameters->Set_Enabled("A_PERCTL", pParameter->asInt() == 3);	// Percentiles
		pParameters->Set_Enabled("A_STDDEV", pParameter->asInt() == 4);	// Percentage of standard deviation
	}

	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_RGB_Composite::On_Execute(void)
{
	double		rMin, rRange, gMin, gRange, bMin, bRange, aMin, aRange;

	//-----------------------------------------------------
	CSG_Grid	*pR	= _Get_Grid(Parameters("R_GRID")->asGrid(), Parameters("R_METHOD")->asInt(), Parameters("R_RANGE")->asRange(), Parameters("R_PERCTL")->asRange(), Parameters("R_STDDEV")->asDouble(), rMin, rRange);
	CSG_Grid	*pG	= _Get_Grid(Parameters("G_GRID")->asGrid(), Parameters("G_METHOD")->asInt(), Parameters("G_RANGE")->asRange(), Parameters("G_PERCTL")->asRange(), Parameters("G_STDDEV")->asDouble(), gMin, gRange);
	CSG_Grid	*pB	= _Get_Grid(Parameters("B_GRID")->asGrid(), Parameters("B_METHOD")->asInt(), Parameters("B_RANGE")->asRange(), Parameters("B_PERCTL")->asRange(), Parameters("B_STDDEV")->asDouble(), bMin, bRange);
	CSG_Grid	*pA	= _Get_Grid(Parameters("A_GRID")->asGrid(), Parameters("A_METHOD")->asInt(), Parameters("A_RANGE")->asRange(), Parameters("A_PERCTL")->asRange(), Parameters("A_STDDEV")->asDouble(), aMin, aRange);

	//-----------------------------------------------------
	CSG_Grid	*pRGB	= Parameters("RGB")->asGrid();

	pRGB->Create(pRGB->Get_System(), SG_DATATYPE_Int);
	pRGB->Set_Name(_TL("Composite"));

	CSG_String	s;

	s	+= CSG_String(_TL("Red"  )) + ": " + pR->Get_Name() + "\n";
	s	+= CSG_String(_TL("Green")) + ": " + pG->Get_Name() + "\n";
	s	+= CSG_String(_TL("Blue" )) + ": " + pB->Get_Name() + "\n";

	if( pA )
	{
		s	+= CSG_String(_TL("Alpha")) + ": " + pA->Get_Name() + "\n";
	}

	pRGB->Set_Description(s);

	DataObject_Set_Colors   (pRGB, 100, SG_COLORS_BLACK_WHITE);
	DataObject_Set_Parameter(pRGB, "COLORS_TYPE", 6);	// Color Classification Type: RGB

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( pR->is_NoData(x, y) || pG->is_NoData(x, y) || pB->is_NoData(x, y) || (pA && pA->is_NoData(x, y)) )
			{
				pRGB->Set_NoData(x, y);
			}
			else
			{
				int	r	= (int)(rRange * (pR->asDouble(x, y) - rMin)); if( r > 255 ) r = 255; else if( r < 0 ) r = 0;
				int	g	= (int)(gRange * (pG->asDouble(x, y) - gMin)); if( g > 255 ) g = 255; else if( g < 0 ) g = 0;
				int	b	= (int)(bRange * (pB->asDouble(x, y) - bMin)); if( b > 255 ) b = 255; else if( b < 0 ) b = 0;

				if( pA )
				{
					int	a	= (int)(aRange * (pA->asDouble(x, y) - aMin)); if( a > 255 ) a = 255; else if( a < 0 ) a = 0;

					pRGB->Set_Value(x, y, SG_GET_RGBA(r, g, b, a));
				}
				else
				{
					pRGB->Set_Value(x, y, SG_GET_RGB (r, g, b));
				}
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Grid * CGrid_RGB_Composite::_Get_Grid(CSG_Grid *pGrid, int Method, CSG_Parameter_Range *pRange, CSG_Parameter_Range *pPerctl, double StdDev, double &Min, double &Range)
{
	if( pGrid )
	{
		switch( Method )
		{
		default:
		case 0:	// 0 - 255
			Min		=   0.0;
			Range	= 255.0;
			break;

		case 1:	// Rescale to 0 - 255
			Min		= pGrid->Get_ZMin();
			Range	= pGrid->Get_ZRange();
			break;

		case 2:	// User defined rescale
			Min		= pRange->Get_LoVal();
			Range	= pRange->Get_HiVal() - Min;
			break;

		case 3:	// Percentile
			Min		= pGrid->Get_Percentile(pPerctl->Get_LoVal());
			Range	= pGrid->Get_Percentile(pPerctl->Get_HiVal()) - Min;
			break;

		case 4:	// Standard deviation
			Min		= pGrid->Get_Mean() - StdDev * pGrid->Get_StdDev();
			Range	= 2.0 * StdDev * pGrid->Get_StdDev();
			break;
		}

		Range	= Range > 0.0 ? 255.0 / Range : 0.0;
	}

	return( pGrid );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_RGB_Split::CGrid_RGB_Split(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Split RGB Composite"));

	Set_Author		("O.Conrad (c) 2014");

	Set_Description	(_TW(
		"Split red-green-blue channels of an rgb coded grid. "
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "RGB"		, _TL("RGB Composite"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "R"		, _TL("Red"),
		_TL(""),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Byte
	);

	Parameters.Add_Grid(
		NULL	, "G"		, _TL("Green"),
		_TL(""),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Byte
	);

	Parameters.Add_Grid(
		NULL	, "B"		, _TL("Blue"),
		_TL(""),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Byte
	);

	Parameters.Add_Grid(
		NULL	, "A"		, _TL("Alpha"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, true, SG_DATATYPE_Byte
	);

	Parameters.Add_Value(
		NULL	, "NODATA"	, _TL("Ignore No Data"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_RGB_Split::On_Execute(void)
{
	CSG_Grid	*pRGB	= Parameters("RGB")->asGrid();

	if( SG_Data_Type_Get_Size(pRGB->Get_Type()) < 4 )
	{
		Message_Add(_TL("warning, input uses less than 4 bytes per value"));
	}

	bool	bNoData	= Parameters("NODATA")->asBool();

	CSG_Grid	*pR	= Parameters("R")->asGrid();	if( bNoData && pR )	pR->Set_NoData_Value(-1);
	CSG_Grid	*pG	= Parameters("G")->asGrid();	if( bNoData && pG )	pG->Set_NoData_Value(-1);
	CSG_Grid	*pB	= Parameters("B")->asGrid();	if( bNoData && pB )	pB->Set_NoData_Value(-1);
	CSG_Grid	*pA	= Parameters("A")->asGrid();	if( bNoData && pA )	pA->Set_NoData_Value(-1);

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( bNoData || !pRGB->is_NoData(x, y) )
			{
				int	RGB	= pRGB->asInt(x, y);

				if( pR )	pR->Set_Value(x, y, SG_GET_R(RGB));
				if( pG )	pG->Set_Value(x, y, SG_GET_G(RGB));
				if( pB )	pB->Set_Value(x, y, SG_GET_B(RGB));
				if( pA )	pA->Set_Value(x, y, SG_GET_A(RGB));
			}
			else
			{
				if( pR )	pR->Set_NoData(x, y);
				if( pG )	pG->Set_NoData(x, y);
				if( pB )	pB->Set_NoData(x, y);
				if( pA )	pA->Set_NoData(x, y);
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

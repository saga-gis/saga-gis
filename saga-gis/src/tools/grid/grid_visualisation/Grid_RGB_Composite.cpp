
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
#include "Grid_RGB_Composite.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_RGB_Composite::CGrid_RGB_Composite(void)
{
	Set_Name		(_TL("RGB Composite"));

	Set_Author		("O.Conrad (c) 2002");

	Set_Description	(_TW(
		"Create red-green-blue overlays of grids. "
	));

	//-----------------------------------------------------
	Parameters.Add_Grid("", "R_GRID"  , _TL("Red"      ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid("", "G_GRID"  , _TL("Green"    ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid("", "B_GRID"  , _TL("Blue"     ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid("", "A_GRID"  , _TL("Alpha"    ), _TL(""), PARAMETER_INPUT_OPTIONAL);
	Parameters.Add_Grid("", "RGB"     , _TL("Composite"), _TL(""), PARAMETER_OUTPUT, true, SG_DATATYPE_Int);

	Parameters.Add_Choice("",
		"METHOD"	, _TL("Value Preparation"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|%s",
			_TL("take original value (0 - 255)"),
			_TL("rescale to 0 - 255"),
			_TL("user defined"),
			_TL("percentiles"),
			_TL("standard deviation")
		), 0
	);

	Parameters.Add_Range("METHOD",
		"RANGE"		, _TL("Rescale Range"),
		_TL(""),
		0., 255.
	);

	Parameters.Add_Range ("METHOD",
		"PERCTL"	, _TL("Percentiles"),
		_TL(""),
		1., 99., 0., true, 100., true
	);

	Parameters.Add_Double("METHOD",
		"STDDEV"	, _TL("Standard Deviation"),
		_TL(""),
		2., 0., true
	);

	Parameters.Add_Bool("",
		"NODATA"	, _TL("Ignore No Data Cells"),
		_TL(""),
		true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_RGB_Composite::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("METHOD") )
	{
		pParameters->Set_Enabled("RANGE" , pParameter->asInt() == 2);	// User defined rescale
		pParameters->Set_Enabled("PERCTL", pParameter->asInt() == 3);	// Percentiles
		pParameters->Set_Enabled("STDDEV", pParameter->asInt() == 4);	// Percentage of standard deviation
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_RGB_Composite::On_Execute(void)
{
	CSG_Grid	*pBand[4];	double	Offset[4], Scale[4];

	pBand[0]	= _Get_Grid(Parameters("R_GRID")->asGrid(), Offset[0], Scale[0]);
	pBand[1]	= _Get_Grid(Parameters("G_GRID")->asGrid(), Offset[1], Scale[1]);
	pBand[2]	= _Get_Grid(Parameters("B_GRID")->asGrid(), Offset[2], Scale[2]);
	pBand[3]	= _Get_Grid(Parameters("A_GRID")->asGrid(), Offset[3], Scale[3]);

	//-----------------------------------------------------
	CSG_Grid	*pRGB	= Parameters("RGB")->asGrid();

	pRGB->Create(pRGB->Get_System(), SG_DATATYPE_Int);
	pRGB->Set_Name(_TL("Composite"));

	bool	bNoData	= Parameters("NODATA")->asBool() == false;

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#ifndef _DEBUG
		#pragma omp parallel for
		#endif
		for(int x=0; x<Get_NX(); x++)
		{
			if( bNoData
			&& ((pBand[0] && pBand[0]->is_NoData(x, y))
			||  (pBand[1] && pBand[1]->is_NoData(x, y))
			||  (pBand[2] && pBand[2]->is_NoData(x, y))
			||  (pBand[3] && pBand[3]->is_NoData(x, y))) )
			{
				pRGB->Set_NoData(x, y);
			}
			else
			{
				BYTE c[4];

				for(int i=0; i<4; i++)
				{
					if( pBand[i] )
					{
						double	d	= Scale[i] * (pBand[i]->asDouble(x, y) - Offset[i]);
						
						c[i]	= d < 0. ? 0 : d > 255. ? 255 : (BYTE)d;
					}
					else
					{
						c[i]	= 255;
					}
				}

				pRGB->Set_Value(x, y, *((int *)c));
			}
		}
	}

	//-----------------------------------------------------
	DataObject_Set_Parameter(pRGB, "COLORS_TYPE", 5);	// Color Classification Type: RGB Coded Values

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Grid * CGrid_RGB_Composite::_Get_Grid(CSG_Grid *pGrid, double &Offset, double &Scale)
{
	if( pGrid )
	{
		switch( Parameters("METHOD")->asInt() )
		{
		default:	// 0 - 255
			Offset	=   0.;
			Scale	= 255.;
			break;

		case  1:	// Rescale to 0 - 255
			Offset	= pGrid->Get_Min();
			Scale	= pGrid->Get_Max() - Offset;
			break;

		case  2:	// User defined rescale
			Offset	= Parameters("RANGE.MIN")->asDouble();
			Scale	= Parameters("RANGE.MIN")->asDouble() - Offset;
			break;

		case  3:	// Percentile
			Offset	= pGrid->Get_Percentile(Parameters("PERCTL.MIN")->asDouble());
			Scale	= pGrid->Get_Percentile(Parameters("PERCTL.MAX")->asDouble()) - Offset;
			break;

		case  4:	// Standard deviation
			Scale	= Parameters("STDDEV")->asDouble();
			Offset	= pGrid->Get_Mean() - pGrid->Get_StdDev() * Scale;
			if( Offset < 0. ) Offset = 0.;
			Scale	*= 2. * pGrid->Get_StdDev();
			break;
		}

		Scale	= Scale > 0. ? 255. / Scale : 0.;
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
	Set_Name		(_TL("Split RGB Composite"));

	Set_Author		("O.Conrad (c) 2014");

	Set_Description	(_TW(
		"Split red-green-blue channels of an rgb coded grid. "
	));

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"RGB"	, _TL("RGB Composite"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"R"		, _TL("Red"),
		_TL(""),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Byte
	);

	Parameters.Add_Grid("",
		"G"		, _TL("Green"),
		_TL(""),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Byte
	);

	Parameters.Add_Grid("",
		"B"		, _TL("Blue"),
		_TL(""),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Byte
	);

	Parameters.Add_Grid("",
		"A"		, _TL("Alpha"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, true, SG_DATATYPE_Byte
	);

	Parameters.Add_Bool("",
		"NODATA", _TL("Ignore No Data"),
		_TL(""),
		true
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

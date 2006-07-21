
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
//                      Filter.cpp                       //
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
#include "Filter.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFilter::CFilter(void)
{
	//-----------------------------------------------------
	// 1. Info...

	Set_Name(_TL("Simple Filter"));

	Set_Author(_TL("Copyrights (c) 2003 by Olaf Conrad"));

	Set_Description(_TL("Filter for Grids")
	);


	//-----------------------------------------------------
	// 2. Parameters...

	Parameters.Add_Grid(
		NULL, "INPUT"		, _TL("Grid"),
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "RESULT"		, _TL("Filtered Grid"),
		"",
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice(
		NULL, "SEARCH_MODE"	, _TL("Search Mode"),
		"",
		CSG_String::Format("%s|%s|",
			_TL("Square"),
			_TL("Circle")
		), 1
	);

	Parameters.Add_Choice(
		NULL, "METHOD"		, _TL("Filter"),
		"",
		CSG_String::Format("%s|%s|%s|",
			_TL("Smooth"),
			_TL("Sharpen"),
			_TL("Edge")
		), 0
	);

	Parameters.Add_Value(
		NULL, "RADIUS"		, _TL("Radius"),
		"",
		PARAMETER_TYPE_Int, 1, 1, true
	);
}

//---------------------------------------------------------
CFilter::~CFilter(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFilter::On_Execute(void)
{
	int		x, y, Mode, Method, Radius;

	double	Mean;

	CGrid	*pResult;

	//-----------------------------------------------------
	pInput		= Parameters("INPUT")->asGrid();

	if( !Parameters("RESULT")->asGrid() )
	{
		Parameters("RESULT")->Set_Value(pInput);
	}

	pResult		= Parameters("RESULT")->asGrid();

	if( !pResult || pResult == pInput )
	{
		pResult	= SG_Create_Grid(pInput);
	}

	Mode		= Parameters("SEARCH_MODE")->asInt();
	Method		= Parameters("METHOD")->asInt();
	Radius		= Parameters("RADIUS")->asInt();

	switch( Mode )
	{
	case 0:
		break;

	case 1:
		pRadius		= new CSG_Grid_Radius(Radius + 1);
		break;
	}

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( pInput->is_InGrid(x, y) )
			{
				switch( Mode )
				{
				case 0:
					Mean	= Get_Mean_Square(x, y, Radius);
					break;

				case 1:
					Mean	= Get_Mean_Circle(x, y);
					break;
				}

				switch( Method )
				{
				case 0:	default:	// Smooth...
					pResult->Set_Value(x, y, Mean);
					break;

				case 1:				// Sharpen...
					pResult->Set_Value(x, y, pInput->asDouble(x, y) + (pInput->asDouble(x, y) - Mean));
					break;

				case 2:				// Edge...
					pResult->Set_Value(x, y, pInput->asDouble(x, y) - Mean);
					break;
				}
			}
		}
	}

	//-----------------------------------------------------
	if( !Parameters("RESULT")->asGrid() || Parameters("RESULT")->asGrid() == pInput )
	{
		pInput->Assign(pResult);

		delete(pResult);
	}

	switch( Mode )
	{
	case 0:
		break;

	case 1:
		delete(pRadius);
		break;
	}

	return( true );
}

//---------------------------------------------------------
double CFilter::Get_Mean_Square(int x, int y, int Radius)
{
	int		ax, ay, bx, by, ix, iy, n;
	double	Result;

	Result	= 0.0;
	n		= 0;

	//-----------------------------------------------------
	ax		= x - Radius;
	bx		= x + Radius;

	if( ax < 0 )
	{
		ax	= 0;
	}
	else if( bx >= Get_NX() )
	{
		bx	= Get_NX() - 1;
	}

	ay		= y - Radius;
	by		= y + Radius;

	if( ay < 0 )
	{
		ay	= 0;
	}
	else if( by >= Get_NY() )
	{
		by	= Get_NY() - 1;
	}

	//-----------------------------------------------------
	for(iy=ay; iy<=by; iy++)
	{
		for(ix=ax; ix<=bx; ix++)
		{
			if( pInput->is_InGrid(ix, iy) )
			{
				Result	+= pInput->asDouble(ix, iy);
				n++;
			}
		}
	}

	//-----------------------------------------------------
	if( n > 0 )
	{
		Result	/= (double)n;
	}

	return( Result );
}

//---------------------------------------------------------
double CFilter::Get_Mean_Circle(int x, int y)
{
	int		iRadius, iPoint, ix, iy, n;
	double	Result;

	Result	= 0.0;
	n		= 0;

	//-----------------------------------------------------
	for(iRadius=0; iRadius<pRadius->Get_Maximum(); iRadius++)
	{
		for(iPoint=0; iPoint<pRadius->Get_nPoints(iRadius); iPoint++)
		{
			pRadius->Get_Point(iRadius, iPoint, ix, iy);

			ix		+= x;
			iy		+= y;

			if( pInput->is_InGrid(ix, iy) )
			{
				Result	+= pInput->asDouble(ix, iy);
				n++;
			}
		}
	}

	//-----------------------------------------------------
	if( n > 0 )
	{
		Result	/= (double)n;
	}

	return( Result );
}

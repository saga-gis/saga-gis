
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     ta_hydrology                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                       Flow.cpp                        //
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
#include "Flow.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFlow::CFlow(void)
{
	pCatch_AspectY	= NULL;
	bPoint			= false;

	//-----------------------------------------------------
	Set_Description(_TL(
		"(c) 2001 by Olaf Conrad, Goettingen.\nemail: oconrad@gwdg.de")
	);


	//-----------------------------------------------------
	// Input...

	Parameters.Add_Grid(
		NULL	, "ELEVATION"	, _TL("Elevation"),
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "SINKROUTE"	, _TL("Sink Routes"),
		"",
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL	, "WEIGHT"		, _TL("Weight"),
		"",
		PARAMETER_INPUT_OPTIONAL
	);


	//-----------------------------------------------------
	// Output...

	Parameters.Add_Grid(
		NULL	, "CAREA"		, _TL("Catchment Area"),
		"",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "CHEIGHT"		, _TL("Catchment Height"),
		"",
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL	, "CSLOPE"		, _TL("Catchment Slope"),
		"",
		PARAMETER_OUTPUT_OPTIONAL
	);


	//-----------------------------------------------------
	// Options...

	Parameters.Add_Value(
		NULL	, "STEP"		, _TL("Step"),
		"",
		PARAMETER_TYPE_Int, 1, 1, true
	);
}

//---------------------------------------------------------
CFlow::~CFlow(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow::Set_Point(int x, int y)
{
	bPoint	= true;
	xPoint	= x;
	yPoint	= y;
}

//---------------------------------------------------------
bool CFlow::On_Execute(void)
{
	bool	bResult	= false;
	long	n;
	double	d;

	//-------------------------------------------------
	pDTM			= Parameters("ELEVATION")	->asGrid();
	pRoute			= Parameters("SINKROUTE")	->asGrid();
	pWeight			= Parameters("WEIGHT")		->asGrid();

	pCatch			= Parameters("CAREA")		->asGrid();
	DataObject_Set_Colors(pCatch, 100, SG_COLORS_WHITE_BLUE);

	pCatch_Height	= NULL;
	pCatch_Slope	= NULL;
	pCatch_Aspect	= NULL;
	pFlowPath		= NULL;

	Step			= Parameters("STEP")		->asInt();

	//-------------------------------------------------
	On_Initialize();

	//-------------------------------------------------
	if( pCatch )
	{
		pCatch			->Assign(0.0);
	}

	if( pCatch_Height )
	{
		pCatch_Height	->Assign(0.0);
	}

	if( pCatch_Slope )
	{
		pCatch_Slope	->Assign(0.0);
	}

	if( pCatch_Aspect )
	{
		pCatch_Aspect	->Assign(0.0);
		pCatch_AspectY	 = SG_Create_Grid(pCatch_Aspect);
	}

	if( pFlowPath )
	{
		pFlowPath		->Assign(0.0);
	}

	//-------------------------------------------------
	if( bPoint )
	{
		bPoint	= false;

		if( is_InGrid(xPoint, yPoint) )
		{
			Calculate(xPoint, yPoint);

			On_Finalize();

			for(n=0; n<Get_NCells(); n++)
			{
				d	= pCatch->asDouble(n);
				pCatch->Set_Value(n, 100.0 * d);
			}

			bResult		= true;
		}
	}
	else
	{
		pCatch_Height	= Parameters("CHEIGHT")	->asGrid();
		pCatch_Slope	= Parameters("CSLOPE")	->asGrid();

		Calculate();

		On_Finalize();
		Finalize();

		bResult			= true;
	}

	//-------------------------------------------------
	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow::Init_Cell(int x, int y)
{
	double	Weight, Slope, Aspect;

	Weight	= pWeight ? pWeight->asDouble(x, y) : 1.0;

	if( pCatch )
	{
		pCatch			->Add_Value(x, y, Weight);
	}

	if( pCatch_Height )
	{
		pCatch_Height	->Add_Value(x, y, Weight * pDTM->asDouble(x,y));
	}

	if( pCatch_Slope )
	{
		Get_Gradient(x, y, Slope, Aspect);

		pCatch_Slope	->Add_Value(x, y, Weight * Slope);
	}

	if( pCatch_Aspect && pCatch_AspectY )
	{
		Get_Gradient(x, y, Slope, Aspect);

		pCatch_Aspect	->Add_Value(x, y, Weight * sin(Aspect));
		pCatch_AspectY	->Add_Value(x, y, Weight * cos(Aspect));
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow::Finalize(void)
{
	long	n;
	double	z, CellSize, Catch, Contour, dContour, G, H;

	//-----------------------------------------------------
	CellSize	= Get_Cellsize() * Get_Cellsize();
	Contour		= 1.0;
	dContour	= 0.02 * M_PI * sqrt(CellSize / M_PI);

	//-----------------------------------------------------
	for(n=0; n<Get_NCells() && Set_Progress_NCells(n); n++)
	{
		if( pDTM->is_NoData(n) )
		{
			if( pCatch )
			{
				pCatch			->Set_NoData(n);
			}

			if( pCatch_Height )
			{
				pCatch_Height	->Set_NoData(n);
			}

			if( pCatch_Slope )
			{
				pCatch_Slope	->Set_NoData(n);
			}

			if( pCatch_Aspect )
			{
				pCatch_Aspect	->Set_NoData(n);
			}

			if( pFlowPath )
			{
				pFlowPath		->Set_NoData(n);
			}
		}
		else
		{
			z		= pDTM->asDouble(n);

			//---------------------------------------------
			Catch	= 1.0 / pCatch->asDouble(n);

			if( pCatch_Height )
			{
				pCatch_Height	->Set_Value(n, Catch * pCatch_Height->asDouble(n) - z);
			}

			if( pCatch_Slope )
			{
				pCatch_Slope	->Mul_Value(n, Catch);
			}

			if( pFlowPath )
			{
				pFlowPath		->Mul_Value(n, Catch);
			}

			//---------------------------------------------
			Catch	= CellSize / Catch;

			if( pCatch )
			{
				pCatch			->Set_Value(n, Catch);
			}

			//---------------------------------------------
			if( pCatch_Aspect && pCatch_AspectY )
			{
				G	= pCatch_Aspect	->asDouble(n);
				H	= pCatch_AspectY->asDouble(n);

				pCatch_Aspect	->Set_Value(n, G ? fmod(M_PI_270 + atan2(H, G), M_PI_360) : (H > 0 ? M_PI_270 : (H < 0 ? M_PI_090 : -1)));
			}
		}
	}

	//-----------------------------------------------------
	if( pCatch_AspectY )
	{
		delete(pCatch_AspectY);
		pCatch_AspectY	= NULL;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow::Get_Gradient(int x, int y, double &Slope, double &Aspect)
{
	pDTM->Get_Gradient(x, y, Slope, Aspect);
}

//---------------------------------------------------------
void CFlow::Add_Fraction(int x, int y, int Direction, double Fraction)
{
	int		ix, iy;

	if( is_InGrid(x, y) )
	{
		ix	= Get_xTo(Direction, x);
		iy	= Get_yTo(Direction, y);

		if( is_InGrid(ix, iy) )
		{
			if( pCatch )
			{
				pCatch			->Add_Value(ix, iy, Fraction * pCatch			->asDouble(x, y));
			}

			if( pCatch_Height )
			{
				pCatch_Height	->Add_Value(ix, iy, Fraction * pCatch_Height	->asDouble(x, y));
			}

			if( pCatch_Slope )
			{
				pCatch_Slope	->Add_Value(ix, iy, Fraction * pCatch_Slope		->asDouble(x, y));
			}

			if( pFlowPath )
			{
				pFlowPath		->Add_Value(ix, iy, Fraction * (pFlowPath		->asDouble(x, y) + Get_Length(Direction)));
			}

			if( pCatch_Aspect && pCatch_AspectY )
			{
				pCatch_Aspect	->Add_Value(ix, iy, Fraction * pCatch_Aspect	->asDouble(x, y));
				pCatch_AspectY	->Add_Value(ix, iy, Fraction * pCatch_AspectY	->asDouble(x, y));
			}
		}
	}
}

//---------------------------------------------------------
void CFlow::Add_Portion(int x, int y, int ix, int iy)
{
	if( is_InGrid(x, y) && is_InGrid(ix, iy) )
	{
		if( pCatch )
		{
			pCatch			->Add_Value(ix, iy, pCatch			->asDouble(x, y));
		}

		if( pCatch_Height )
		{
			pCatch_Height	->Add_Value(ix, iy, pCatch_Height	->asDouble(x, y));
		}

		if( pCatch_Slope )
		{
			pCatch_Slope	->Add_Value(ix, iy, pCatch_Slope	->asDouble(x, y));
		}

		if( pFlowPath )
		{
			pFlowPath		->Add_Value(ix, iy, pFlowPath		->asDouble(x, y));
		}

		if( pCatch_Aspect && pCatch_AspectY )
		{
			pCatch_Aspect	->Add_Value(ix, iy, pCatch_Aspect	->asDouble(x, y));
			pCatch_AspectY	->Add_Value(ix, iy, pCatch_AspectY	->asDouble(x, y));
		}
	}
}

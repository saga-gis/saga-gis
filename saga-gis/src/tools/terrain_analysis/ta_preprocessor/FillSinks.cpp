/**********************************************************
 * Version $Id: FillSinks.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    ta_preprocessor                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     FillSinks.cpp                     //
//                                                       //
//                 Copyright (C) 2003 by                 //
//                    Volker Wichmann                    //
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
//    e-mail:     reklovw@web.de                         //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "FillSinks.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFillSinks::CFillSinks(void)
{
	Set_Name(_TL("Fill Sinks (Planchon/Darboux, 2001)"));

	Set_Author(_TL("Copyrights (c) 2003 by Volker Wichmann"));

	Set_Description	(_TW(
		"Depression filling algorithm after Olivier Planchon & Frederic Darboux (2001)\n\n\n"
		"References:\n"
		"Planchon, O. & F. Darboux (2001): "
		"A fast, simple and versatile algorithm to fill the depressions of digital elevation models. "
		"Catena 46: 159-176\n\n")
	);


	Parameters.Add_Grid(
		NULL, "DEM"		, _TL("DEM"),
		_TL("digital elevation model [m]"),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "RESULT"	, _TL("Filled DEM"),
		_TL("processed DEM"),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL, "MINSLOPE", _TL("Minimum Slope [Degree]"),
		_TL("minimum slope angle preserved from one cell to the next, zero results in flat areas [Degree]"),
		PARAMETER_TYPE_Double, 0.01, 0.0, true
	);
}

//---------------------------------------------------------
CFillSinks::~CFillSinks(void)
{}


///////////////////////////////////////////////////////////
//														 //
// Sink filling algorithm after:						 //
// Planchon, O. & F. Darboux (2001): A fast, simple and  //
// versatile algorithm to fill the depressions of		 //
// digital elevation models. Catena 46: 159-176			 //
//														 //
///////////////////////////////////////////////////////////

bool CFillSinks::On_Execute(void)
{
	bool	something_done;
	int		x, y, scan, ix, iy, i, it;
	double	z, wz, wzn, minslope;

	pDEM		= Parameters("DEM")->asGrid();
	pResult		= Parameters("RESULT")->asGrid();

	pResult->Set_Name("%s [%s]", pDEM->Get_Name(), _TL("no sinks"));

	minslope	= tan(Parameters("MINSLOPE")->asDouble() * M_DEG_TO_RAD);

	pW			= new CSG_Grid(SG_DATATYPE_Double, pDEM->Get_NX(), pDEM->Get_NY(), pDEM->Get_Cellsize(), pDEM->Get_XMin(), pDEM->Get_YMin());
	pBorder		= new CSG_Grid(SG_DATATYPE_Int   , pDEM->Get_NX(), pDEM->Get_NY(), pDEM->Get_Cellsize(), pDEM->Get_XMin(), pDEM->Get_YMin());

	pW->Assign_NoData();
	pBorder->Assign_NoData();

	for(i=0; i<8; i++)
		epsilon[i] = minslope * Get_Length(i);

	R0[0] = 0; R0[1] = Get_NY()-1; R0[2] = 0; R0[3] = Get_NY()-1; R0[4] = 0; R0[5] = Get_NY()-1; R0[6] = 0; R0[7] = Get_NY()-1;
	C0[0] = 0; C0[1] = Get_NX()-1; C0[2] = Get_NX()-1; C0[3] = 0; C0[4] = Get_NX()-1; C0[5] = 0; C0[6] = 0; C0[7] = Get_NX()-1;
	dR[0] = 0; dR[1] = 0; dR[2] = 1; dR[3] = -1; dR[4] = 0; dR[5] = 0; dR[6] = 1; dR[7] = -1;
	dC[0] = 1; dC[1] = -1; dC[2] = 0; dC[3] = 0; dC[4] = -1; dC[5] = 1; dC[6] = 0; dC[7] = 0;
	fR[0] = 1; fR[1] = -1; fR[2] = -Get_NY()+1; fR[3] = Get_NY()-1; fR[4] = 1; fR[5] = -1; fR[6] = -Get_NY()+1; fR[7] = Get_NY()-1;
	fC[0] = -Get_NX()+1, fC[1] = Get_NX()-1; fC[2] = -1; fC[3] = 1; fC[4] = Get_NX()-1; fC[5] = -Get_NX()+1; fC[6] = 1; fC[7] = -1;

	Init_Altitude();															// Stage 1

	for(x=0; x<Get_NX(); x++)													// Stage 2, Section 1
	{
		for(y=0; y<Get_NY(); y++)
		{
			if( pBorder->asInt(x, y) == 1 )
				Dry_upward_cell(x, y);
		}
	}

	for(it=0; it<1000; it++)
	{
		for(scan=0; scan<8 && Set_Progress(scan,8); scan++)					// Stage 2, Section 2
		{
			R = R0[scan];
			C = C0[scan];
			something_done = false;

			do
			{
				if( !pDEM->is_NoData(C, R) && ((wz = pW->asDouble(C, R)) > (z = pDEM->asDouble(C, R))) )
				{
					for(i=0; i<8; i++)
					{
						ix	= Get_xTo(i, C);
						iy	= Get_yTo(i, R);

						if(	pDEM->is_InGrid(ix, iy) )
						{
							if( z >= (wzn = (pW->asDouble(ix, iy) + epsilon[i])) )	// operation 1
							{
								pW->Set_Value(C, R, z);
								something_done = true;
								Dry_upward_cell(C, R);
								break;
							}
							if( wz > wzn )											// operation 2
							{
								pW->Set_Value(C, R, wzn);
								something_done = true;
							}
						}
					}
				}
			}while( Next_Cell(scan) );

			if( something_done == false )
				break;
		}

	if( something_done == false )
		break;
	}

	pResult->Assign(pW);


	//-----------------------------------------------------
	delete pW;
	delete pBorder;

	return( true );
}

void CFillSinks::Dry_upward_cell(int x, int y)
{
	int const	MAX_DEPTH = 32000;													// recursion stack, maybe not needed??
	int			depth = 0;
	int			ix, iy, i;
	double		zn;

	depth += 1;

	if( depth <= MAX_DEPTH )
	{
		for(i=0; i<8; i++)
		{
			ix	= Get_xTo(i, x);
			iy	= Get_yTo(i, y);

			if(	is_InGrid(ix, iy) && pW->asDouble(ix, iy) == 50000 )
			{
				if( (zn = pDEM->asDouble(ix , iy)) >= (pW->asDouble(x, y) + epsilon[i]) )
				{
					pW->Set_Value(ix, iy, zn);
					Dry_upward_cell(ix, iy);
				}
			}
		}
	}
	depth -= 1;
}



void CFillSinks::Init_Altitude()
{
	bool	border;
	int		x, y, i, ix, iy;

	for(x=0; x<Get_NX(); x++)
	{
		for(y=0; y<Get_NY(); y++)
		{
			border = false;

			if( !pDEM->is_NoData(x, y) )
			{
				for(i=0; i<8; i++)
				{
					ix	= Get_xTo(i, x);
					iy	= Get_yTo(i, y);

					if(	!pDEM->is_InGrid(ix, iy) )
					{
						border = true;
						break;
					}
				}
				if( border == true )
				{
					pBorder->Set_Value(x, y, 1);
					pW->Set_Value(x, y, pDEM->asDouble(x, y));
				}
				else
					pW->Set_Value(x, y, 50000.0);
			}
		}
	}
}



bool CFillSinks::Next_Cell(int i)
{

	R	= R + dR[i];
	C	= C + dC[i];

	if( R < 0 || C < 0 || R >= Get_NY() || C >= Get_NX() )
	{
		R	= R + fR[i];
		C	= C + fC[i];

		if( R < 0 || C < 0 || R >= Get_NY() || C >= Get_NX() )
			return (false);
	}

	return (true);
}

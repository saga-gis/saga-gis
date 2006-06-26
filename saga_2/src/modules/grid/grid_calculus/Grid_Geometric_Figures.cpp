
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
//               Grid_Geometric_Figures.cpp              //
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
#include "Grid_Geometric_Figures.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Geometric_Figures::CGrid_Geometric_Figures(void)
{
	Set_Name	(_TL("Geometric Figures"));

	Set_Author	(_TL("Copyrights (c) 2001 by Olaf Conrad"));

	Set_Description(_TL(
		"Construct grids from geometric figures (planes, cones).\n"
		"(c) 2001 by Olaf Conrad, Goettingen\nemail: oconrad@gwdg.de")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL, "RESULT"		, _TL("Result"),
		"",
		PARAMETER_OUTPUT_OPTIONAL, false
	);

	Parameters.Add_Value(
		NULL, "CELL_COUNT"	, _TL("Cell Count"),
		"",
		PARAMETER_TYPE_Int, 100, 2.0, true
	);

	Parameters.Add_Value(
		NULL, "CELL_SIZE"	, _TL("Cell Size"),
		"",
		PARAMETER_TYPE_Double, 1.0, 0.0, true
	);

	Parameters.Add_Choice(
		NULL, "FIGURE"		, _TL("Figure"),
		"",
		CAPI_String::Format("%s|%s|%s|", _TL("Cone (up)"), _TL("Cone (down)"), _TL("Plane")), 
		0
	);

	Parameters.Add_Value(
		NULL, "PLANE"		, _TL("Direction of Plane [Degree]"),
		"",
		PARAMETER_TYPE_Double, 22.5
	);
}

//---------------------------------------------------------
CGrid_Geometric_Figures::~CGrid_Geometric_Figures(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Geometric_Figures::On_Execute(void)
{
	int		NXY;
	double	DXY;
	CGrid	*pGrid;

	//-----------------------------------------------------
	NXY		= Parameters("CELL_COUNT")	->asInt();
	DXY		= Parameters("CELL_SIZE")	->asDouble();

	Parameters("RESULT")->asGridList()->Add_Item(
		pGrid	= API_Create_Grid(GRID_TYPE_Float, NXY, NXY, DXY)
	);

	//-----------------------------------------------------
	switch( Parameters("FIGURE")->asInt() )
	{
	case 0:	default:
		Create_Cone		(pGrid, true);
		break;

	case 1:
		Create_Cone		(pGrid, false);
		break;

	case 2:
		Create_Plane	(pGrid, Parameters("PLANE")->asDouble());
		break;
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
void CGrid_Geometric_Figures::Create_Cone(CGrid *pGrid, bool bUp)
{
	int		x, y;
	double	nx_2, ny_2, dx, dy, d;

	//-----------------------------------------------------
	pGrid->Set_Name(bUp ? _TL("Cone (Up)") : _TL("Cone (Down)"));

	nx_2	= pGrid->Get_Cellsize() * (double)pGrid->Get_NX() / 2.0;
	ny_2	= pGrid->Get_Cellsize() * (double)pGrid->Get_NY() / 2.0;

	//-----------------------------------------------------
	for(y=0; y<pGrid->Get_NY() && Set_Progress(y, pGrid->Get_NY()); y++)
	{
		for(x=0; x<pGrid->Get_NX(); x++)
		{
			dx	= 0.5 + x * pGrid->Get_Cellsize() - nx_2;
			dy	= 0.5 + y * pGrid->Get_Cellsize() - ny_2;
			d	= sqrt(dx*dx + dy*dy);

			if( d < nx_2 )
			{
				pGrid->Set_Value(x, y, bUp ? d : -d);
			}
			else
			{
				pGrid->Set_NoData(x, y);
			}
		}
	}
}

//---------------------------------------------------------
void CGrid_Geometric_Figures::Create_Plane(CGrid *pGrid, double Direction)
{
	int		x, y;
	double	xPos, yPos, z, dSin, dCos, Max;

	//-----------------------------------------------------
	pGrid->Set_Name(CAPI_String::Format(_TL("Plane (%.2fDegree)"), Direction));

	Max		= sqrt(2.0) * pGrid->Get_Cellsize() * pGrid->Get_Cellsize() / 4.0;

	dSin	= sin(M_DEG_TO_RAD * Direction);
	dCos	= cos(M_DEG_TO_RAD * Direction);

	//-----------------------------------------------------
	for(y=0, yPos=0.5-pGrid->Get_Cellsize()*pGrid->Get_NY()/2.0; y<pGrid->Get_NY() && Set_Progress(y, pGrid->Get_NY()); y++, yPos+=pGrid->Get_Cellsize())
	{
		for(x=0, xPos=0.5-pGrid->Get_Cellsize()*pGrid->Get_NX()/2.0; x<pGrid->Get_NX(); x++, xPos+=pGrid->Get_Cellsize())
		{
		//	pGrid->Set_NoData(x, y);

		//	z	= dCos * xPos - dSin * yPos;

		//	if( z >= -Max && z <= Max )
			{
		//		z	= dSin * xPos + dCos * yPos;

		//		if( z >= -Max && z <= Max )
				{
					z	= xPos * dSin + yPos * dCos;

					pGrid->Set_Value(x, y, z);
				}
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

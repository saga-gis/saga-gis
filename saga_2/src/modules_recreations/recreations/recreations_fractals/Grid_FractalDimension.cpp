
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                       Fractals                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               Grid_FractalDimension.cpp               //
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
#include "Grid_FractalDimension.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_FractalDimension::CGrid_FractalDimension(void)
{
	Set_Name(_TL("Fractal Dimension of Grid Surface"));

	Set_Author(_TL("Copyrights (c) 2001 by Olaf Conrad"));

	Set_Description	(_TW(
		"Calculates surface areas for increasing mesh sizes.")
	);

	Parameters.Add_Grid(	NULL, "INPUT"	, _TL("Input")		, _TL(""), PARAMETER_INPUT);
	Parameters.Add_Table(	NULL, "RESULT"	, _TL("Result")		, _TL(""), PARAMETER_OUTPUT);
}

//---------------------------------------------------------
CGrid_FractalDimension::~CGrid_FractalDimension(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_FractalDimension::On_Execute(void)
{
	int				i;
	CSG_Table			*pTable;
	CSG_Table_Record	*pRecord;

	//-----------------------------------------------------
	pGrid		= Parameters("INPUT")	->asGrid();
	pTable		= Parameters("RESULT")	->asTable();

	dimCount	= pGrid->Get_NX() > pGrid->Get_NY() ? pGrid->Get_NX() - 1 : pGrid->Get_NY() - 1;

	//-----------------------------------------------------
	if( dimCount > 0 )
	{
		dimAreas	= (double *)SG_Calloc(dimCount, sizeof(double));

		for(i=0; i<dimCount && Set_Progress(i, dimCount); i++)
		{
			Get_Surface(i);
		}

		//-------------------------------------------------
		pTable->Destroy();
		pTable->Set_Name(_TL("Fractal Dimension"));

		pTable->Add_Field(_TL("Class")		, TABLE_FIELDTYPE_Int);
		pTable->Add_Field(_TL("Scale")		, TABLE_FIELDTYPE_Double);
		pTable->Add_Field(_TL("Area")		, TABLE_FIELDTYPE_Double);
		pTable->Add_Field(_TL("Ln(Area)")	, TABLE_FIELDTYPE_Double);
		pTable->Add_Field(_TL("Dim01")		, TABLE_FIELDTYPE_Double);
		pTable->Add_Field(_TL("Dim02")		, TABLE_FIELDTYPE_Double);

		for(i=0; i<dimCount-1; i++)
		{
			pRecord	= pTable->Add_Record();

			pRecord->Set_Value(0,  i + 1);
			pRecord->Set_Value(1, (i + 1) * Get_Cellsize());
			pRecord->Set_Value(2,     dimAreas[i]);
			pRecord->Set_Value(3, log(dimAreas[i]));
			pRecord->Set_Value(4,     dimAreas[i]  -     dimAreas[i + 1]);
			pRecord->Set_Value(5, log(dimAreas[i]) - log(dimAreas[i + 1]));
		}

		//-------------------------------------------------
		SG_Free(dimAreas);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CGrid_FractalDimension::Get_Surface(int Step)
{
	int		ya, yb, xStep, yStep;

	xStep	= Step + 1;
	yStep	= Step + 1;

	for(ya=0, yb=yStep; yb<Get_NY(); ya+=yStep, yb+=yStep)
	{
		Get_SurfaceRow(Step, xStep, yStep, ya, yb);
	}

	//---Rest----------------------------------------------
	if( Get_NY() % yStep )
	{
		yStep	= Get_NY() % yStep;
	}

	Get_SurfaceRow(Step, xStep, yStep, ya, Get_NY() - 1);
}

//---------------------------------------------------------
void CGrid_FractalDimension::Get_SurfaceRow(int Step, int xStep, int yStep, int ya, int yb)
{
	int		xa, xb;
	double	xdist, ydist;

	xdist	= xStep * Get_Cellsize();
	ydist	= yStep * Get_Cellsize();

	if( xStep == yStep )
	{
		for(xa=0, xb=xStep; xb<Get_NX(); xa+=xStep, xb+=xStep)
		{
			dimAreas[Step]	+= Get_Area(xdist,
								pGrid->asDouble(xa, ya), pGrid->asDouble(xb, ya),
								pGrid->asDouble(xb, yb), pGrid->asDouble(xa, yb)
							);
		}
	}
	else
	{
		for(xa=0, xb=xStep; xb<Get_NX(); xa+=xStep, xb+=xStep)
		{
			dimAreas[Step]	+= Get_Area(xdist, ydist,
								pGrid->asDouble(xa, ya), pGrid->asDouble(xb, ya),
								pGrid->asDouble(xb, yb), pGrid->asDouble(xa, yb)
							);
		}
	}

	//---Rest----------------------------------------------
	if( Get_NX() % xStep )
	{
		xStep	= Get_NX() % xStep;
	}

	dimAreas[Step]	+= Get_Area(xStep * Get_Cellsize(), ydist,
						pGrid->asDouble(xa          , ya), pGrid->asDouble(Get_NX() - 1, ya),
						pGrid->asDouble(Get_NX() - 1, yb), pGrid->asDouble(xa          , yb)
					);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CGrid_FractalDimension::Get_Distance(double za, double zb, double dist)
{
	za	= fabs(za - zb);

	return( sqrt(za*za + dist*dist) );
}

//---------------------------------------------------------
double CGrid_FractalDimension::Get_Area(double dist, double z1, double z2, double z3, double z4)
{
	int		i;
	double	z[4]	= { z1, z2, z3, z4 },
			Area	= 0,
			mz		= (z[0] + z[1] + z[2] + z[3]) / 4,
			mdist	= dist * sqrt(2.0) / 2.0,
			ab, am, bm, am2, k;

	bm		= Get_Distance(z[3],mz,mdist);

	for(i=0; i<4; i++)
	{
		am		= bm;
		bm		= Get_Distance(z[i], mz, mdist);
		ab		= Get_Distance(z[i], z[(i+3)%4], dist);
		am2		= am*am;
		k		= (am2 - bm*bm + ab*ab) / (2*ab);
		Area	+= ab * sqrt(am2 - k*k) / 2;
	}

	return( Area );
}

//---------------------------------------------------------
double CGrid_FractalDimension::Get_Area(double xdist, double ydist, double z1, double z2, double z3, double z4)
{
	int		i;
	double	z[4]	= { z1, z2, z3, z4 },
			Area	= 0,
			mz		= (z[0] + z[1] + z[2] + z[3]) / 4,
			mdist	= sqrt(xdist*xdist + ydist*ydist) / 2,
			ab, am, bm, am2, k;

	bm		= Get_Distance(z[3],mz,mdist);

	for(i=0; i<4; i++)
	{
		am		= bm;
		bm		= Get_Distance(z[i],mz,mdist);
		ab		= Get_Distance(z[i],z[(i+3)%4],i%2 ? xdist : ydist);
		am2		= am*am;
		k		= (am2 - bm*bm + ab*ab) / (2*ab);
		Area	+= ab * sqrt(am2 - k*k) / 2;
	}

	return(Area);
}

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
//                     ta_hydrology                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 Flow_RecursiveUp.cpp                  //
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
#include "Flow_RecursiveUp.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFlow_RecursiveUp::CFlow_RecursiveUp(void)
{
	Set_Name		(_TL("Catchment Area (Recursive)"));

	Set_Author		(SG_T("O.Conrad (c) 2001"));

	Set_Description	(_TW(
		"Recursive upward processing of cells for calculation of flow accumulation and related parameters. "
		"This set of algorithms processes recursively all upwards connected cells until each cell of the DEM has been processed.\n\n"

		"References:\n\n"

		"Deterministic 8\n"
		"- O'Callaghan, J.F. / Mark, D.M. (1984):\n"
		"    'The extraction of drainage networks from digital elevation data',\n"
		"    Computer Vision, Graphics and Image Processing, 28:323-344\n\n"

		"Rho 8:\n"
		"- Fairfield, J. / Leymarie, P. (1991):\n"
		"    'Drainage networks from grid digital elevation models',\n"
		"    Water Resources Research, 27:709-717\n\n"

		"Deterministic Infinity:\n"
		"- Tarboton, D.G. (1997):\n"
		"    'A new method for the determination of flow directions and upslope areas in grid digital elevation models',\n"
		"    Water Resources Research, Vol.33, No.2, p.309-319\n\n"

		"Multiple Flow Direction:\n"
		"- Freeman, G.T. (1991):\n"
		"    'Calculating catchment area with divergent flow based on a regular grid',\n"
		"    Computers and Geosciences, 17:413-22\n\n"

		"- Quinn, P.F. / Beven, K.J. / Chevallier, P. / Planchon, O. (1991):\n"
		"    'The prediction of hillslope flow paths for distributed hydrological modelling using digital terrain models',\n"
		"    Hydrological Processes, 5:59-79\n\n")
	);


	//-----------------------------------------------------
	// Input...

	Parameters.Add_Grid(
		NULL	, "TARGETS"		, _TL("Target Areas"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);


	//-----------------------------------------------------
	// Output...

	Parameters.Add_Grid(
		NULL	, "FLOWLEN"		, _TL("Flow Path Length"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);


	//-----------------------------------------------------
	// Method...

	Parameters.Add_Choice(
		NULL	, "Method"		, _TL("Method"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("Deterministic 8"),
			_TL("Rho 8"),
			_TL("Deterministic Infinity"),
			_TL("Multiple Flow Direction")
		), 3
	);


	//-----------------------------------------------------
	// Options...

	Parameters.Add_Value(
		NULL	, "CONVERGENCE"	, _TL("Convergence"),
		_TL("Convergence factor for Multiple Flow Direction Algorithm (Freeman 1991)"),
		PARAMETER_TYPE_Double	, 1.1
	);

	
	//-----------------------------------------------------
	// Initialisations...

	Flow	= NULL;
}

//---------------------------------------------------------
CFlow_RecursiveUp::~CFlow_RecursiveUp(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow_RecursiveUp::On_Create(void)
{
	int		x, y, Method;

	double	*p;

	//-----------------------------------------------------
	On_Destroy();

	Flow	= (double ***)SG_Malloc(    Get_NY()  * sizeof(double **));
	p		= (double   *)SG_Malloc((long) 8 * Get_NCells() * sizeof(double   ));

	for(y=0; y<Get_NY(); y++)
	{
		Flow[y]	= (double **)SG_Malloc( Get_NX()  * sizeof(double  *));

		for(x=0; x<Get_NX(); x++, p+=8)
		{
			Flow[y][x]	= p;
		}
	}

	//-----------------------------------------------------
	Lock_Create();

	Method	= Parameters("Method")->asInt();

	memset(Flow[0][0], 0, (long) 8 * Get_NCells() * sizeof(double) );

	for(y=0; y<Get_NY(); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( pRoute && pRoute->asChar(x,y) > 0 )
			{
				Flow[y][x][pRoute->asChar(x,y) % 8]	= 1.0;
			}
			else
			{
				switch( Method )
				{
					case 0:
						Set_D8(x,y);
						break;

					case 1:
						Set_Rho8(x,y);
						break;

					case 2:
						Set_DInf(x,y);
						break;

					case 3:
						Set_MFD(x,y);
						break;
				}
			}
		}
	}
}

//---------------------------------------------------------
void CFlow_RecursiveUp::On_Destroy(void)
{
	int		y;

	if( Flow )
	{
		SG_Free(Flow[0][0]);

		for(y=0; y<Get_NY(); y++)
		{
			SG_Free(Flow[y]);
		}

		SG_Free(Flow);

		Flow	= NULL;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow_RecursiveUp::On_Initialize(void)
{
	pFlowPath			= Parameters("FLOWLEN")->asGrid();

	MFD_Converge		= Parameters("CONVERGENCE")->asDouble();
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFlow_RecursiveUp::Calculate(void)
{
	CSG_Grid	*pTargets	= Parameters("TARGETS")->asGrid();

	On_Create();

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !pTargets || !pTargets->is_NoData(x, y) )
			{
				Get_Flow(x, y);
			}
		}
	}

	On_Destroy();

	return( true );
}

//---------------------------------------------------------
bool CFlow_RecursiveUp::Calculate(int x, int y)
{
	On_Create();

	Get_Flow(x,y);

	On_Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow_RecursiveUp::Get_Flow(int x, int y)
{
	int		i, ix, iy, j;

	double	jFlow;

	if( !is_Locked(x,y) )
	{
		Lock_Set(x,y);

		Init_Cell(x,y);

		for(i=0, j=4; i<8; i++, j=(j+1)%8)
		{
			ix	= Get_xTo(i,x);
			iy	= Get_yTo(i,y);

			if( is_InGrid(ix,iy) )
			{
				jFlow	= Flow[iy][ix][j];

				if( jFlow > 0 )
				{
					Get_Flow(ix,iy);

					Add_Fraction(ix,iy,j,jFlow);
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
void CFlow_RecursiveUp::Set_D8(int x, int y)
{
	int		Direction;

	Direction	= pDTM->Get_Gradient_NeighborDir(x,y);

	if( Direction >= 0 )
	{
		Flow[y][x][Direction % 8]	= 1.0;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow_RecursiveUp::Set_Rho8(int x, int y)
{
	int		Direction;

	double 	Slope, Aspect;

	Get_Gradient(x, y, Slope, Aspect);

	Aspect	*= M_RAD_TO_DEG;

	if( Aspect >= 0 )
	{
		Direction	= (int)(Aspect / 45.0);

		if( fmod(Aspect,45) / 45.0 > rand() / (double)RAND_MAX )
			Direction++;

		Direction	%= 8;

		Flow[y][x][Direction]	= 1.0;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow_RecursiveUp::Set_DInf(int x, int y)
{
	int		Direction;

	double 	Slope, Aspect;

	Get_Gradient(x, y, Slope, Aspect);

	Aspect	*= M_RAD_TO_DEG;

	if( Aspect >= 0 )
	{
		Direction	= (int)(Aspect / 45.0);
		Aspect		= fmod(Aspect,45) / 45.0;

		Flow[y][x][(Direction    ) % 8]	= 1 - Aspect;
		Flow[y][x][(Direction + 1) % 8]	=     Aspect;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow_RecursiveUp::Set_MFD(int x, int y)
{
	int		i, ix, iy;

	double	z, d, *dz, dzSum;

	z		= pDTM->asDouble(x,y);
	dz		= Flow[y][x];
	dzSum	= 0;

	for(i=0; i<8; i++)
	{
		ix	= Get_xTo(i,x);
		iy	= Get_yTo(i,y);

		if( is_InGrid(ix,iy) )
		{
			d	= z - pDTM->asDouble(ix,iy);

			if( d > 0 )
				dzSum	+= dz[i]	= pow(d / Get_Length(i), MFD_Converge);
		}
	}

	if( dzSum )
	{
		for(i=0; i<8; i++)
			if( dz[i] > 0 )
				dz[i]	/= dzSum;
	}
}

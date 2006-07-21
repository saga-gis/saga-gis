
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      Grid_Tools                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     Grid_Gaps.cpp                     //
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
#include "Grid_Gaps.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Gaps::CGrid_Gaps(void)
{
	//-----------------------------------------------------
	Set_Name(_TL("Close Gaps"));

	Set_Author(_TL("Copyrights (c) 2002 by Olaf Conrad"));

	Set_Description(_TL(
		"Close gaps of a grid data set (i.e. eliminate no data values). "
		"If the target is not set, the changes will be stored to the original grid. ")
	);


	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL, "INPUT"		, _TL("Grid"),
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "RESULT"		, _TL("Changed Grid"),
		"",
		PARAMETER_OUTPUT_OPTIONAL
	);


	//-----------------------------------------------------
	Parameters.Add_Value(
		NULL, "THRESHOLD"	, _TL("Tension Threshold"),
		"", PARAMETER_TYPE_Double, 0.1
	);
}

//---------------------------------------------------------
CGrid_Gaps::~CGrid_Gaps(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Gaps::On_Execute(void)
{
	bool	bKillInput;

	//-----------------------------------------------------
	pInput		= Parameters("INPUT")->asGrid();

	if( Parameters("RESULT")->asGrid() == NULL || Parameters("RESULT")->asGrid() == pInput )
	{
		pResult		= pInput;
		Parameters("RESULT")->Set_Value(pResult);

		pInput		= SG_Create_Grid(pInput);
		pInput->Assign(pResult);

		bKillInput	= true;
	}
	else
	{
		pResult		= Parameters("RESULT")->asGrid();
		pResult->Get_History().Assign(pInput->Get_History());

		bKillInput	= false;
	}

	//-----------------------------------------------------
	Tension_Main();

	//-----------------------------------------------------
	if( bKillInput )
	{
		delete(pInput);
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
void CGrid_Gaps::Tension_Main(void)
{
	int		iStep, iStart, n;

	double	max, Threshold;

	Threshold	= Parameters("THRESHOLD")->asDouble();

	n			= Get_NX() > Get_NY() ? Get_NX() : Get_NY();
	iStep		= 0;
	do	{	iStep++;	}	while( pow(2.0, iStep + 1) < n );
	iStart		= (int)pow(2.0, iStep);

	pTension_Keep		= new CGrid(pResult, GRID_TYPE_Byte);
	pTension_Temp		= new CGrid(pResult);

	pResult->Assign_NoData();

	for(iStep=iStart; iStep>=1; iStep/=2)
	{
		Tension_Init(iStep);

		do
		{
			max		= Tension_Step(iStep);
		}
		while( max > Threshold && Process_Get_Okay(true) );

		DataObject_Update(pResult, pInput->Get_ZMin(), pInput->Get_ZMax(), true);
	}

	delete(pTension_Keep);
	delete(pTension_Temp);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CGrid_Gaps::Tension_Init(int iStep)
{
	int		x, y, i, ix, iy, nx, ny, nz;

	double	z;

	//-----------------------------------------------------
	// 1. Channels...

	pTension_Temp->Assign_NoData();
	pTension_Keep->Assign();

	for(y=0; y<Get_NY(); y+=iStep)
	{
		ny	= y + iStep < Get_NY() ? y + iStep : Get_NY();

		for(x=0; x<Get_NX(); x+=iStep)
		{
			if( !pInput->is_NoData(x, y) )
			{
				pTension_Temp->Set_Value(x, y, pInput->asDouble(x, y) );
				pTension_Keep->Set_Value(x, y, 1.0);
			}
			else
			{
				nx	= x + iStep < Get_NX() ? x + iStep : Get_NX();
				nz	= 0;
				z	= 0.0;

				for(iy=y; iy<ny; iy++)
				{
					for(ix=x; ix<nx; ix++)
					{
						if( pInput->is_InGrid(ix, iy) )
						{
							z	+= pInput->asDouble(ix, iy);
							nz++;
						}
					}
				}

				if( nz > 0 )
				{
					pTension_Temp->Set_Value(x, y, z / (double)nz );
					pTension_Keep->Set_Value(x, y, 1.0);
				}
			}
		}
	}

	//-----------------------------------------------------
	// 2. Previous Iteration...

	for(y=0; y<Get_NY(); y+=iStep)
	{
		for(x=0; x<Get_NX(); x+=iStep)
		{
			if( pTension_Keep->asByte(x, y) == false )
			{
				if( !pResult->is_NoData(x, y) )
				{
					pTension_Temp->Set_Value(x, y, pResult->asDouble(x, y));
				}
				else
				{
					nz	= 0;
					z	= 0.0;

					for(i=0; i<8; i++)
					{
						ix	= x + iStep * Get_System()->Get_xTo(i);
						iy	= y + iStep * Get_System()->Get_yTo(i);

						if( pResult->is_InGrid(ix, iy) )
						{
							z	+= pResult->asDouble(ix, iy);
							nz++;
						}
					}

					if( nz > 0.0 )
					{
						pTension_Temp->Set_Value(x, y, z / (double)nz);
					}
					else
					{
						pTension_Temp->Set_Value(x, y, pInput->asDouble(x, y));
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	// 3. ...

	pResult->Assign(pTension_Temp);
}

//---------------------------------------------------------
double CGrid_Gaps::Tension_Step(int iStep)
{
	int		x, y;

	double	d, dMax;

	dMax	= 0.0;

	for(y=0; y<Get_NY(); y+=iStep)
	{
		for(x=0; x<Get_NX(); x+=iStep)
		{
			if( pTension_Keep->asByte(x, y) == false )
			{
				d	= Tension_Change(x, y, iStep);

				pTension_Temp->Set_Value(x, y, d);

				d	= fabs(d - pResult->asDouble(x, y));

				if( d > dMax )
				{
					dMax	= d;
				}
			}
		}
	}

	for(y=0; y<Get_NY(); y+=iStep)
	{
		for(x=0; x<Get_NX(); x+=iStep)
		{
			if( pTension_Keep->asByte(x, y) == false )
			{
				pResult->Set_Value(x, y, pTension_Temp->asDouble(x, y));
			}
		}
	}

	return( dMax );
}


//---------------------------------------------------------
double CGrid_Gaps::Tension_Change(int x, int y, int iStep)
{
	int		i, ix, iy;

	double	n, d, dz;

	for(i=0, d=0.0, n=0.0; i<8; i++)
	{
		ix	= x + iStep * Get_System()->Get_xTo(i);
		iy	= y + iStep * Get_System()->Get_yTo(i);

		if( pResult->is_InGrid(ix, iy) )
		{
			dz	= 1.0 / Get_System()->Get_UnitLength(i);
			d	+= dz * pResult->asDouble(ix, iy);
			n	+= dz;
		}
	}

	if( n > 0.0 )
	{
		d	/= n;

		return( d );
	}

	return( pResult->asDouble(x, y) );
}

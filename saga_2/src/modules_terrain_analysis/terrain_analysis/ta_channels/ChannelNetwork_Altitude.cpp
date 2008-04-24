
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      ta_channels                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//              ChannelNetwork_Altitude.cpp              //
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
#include "ChannelNetwork_Altitude.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CChannelNetwork_Altitude::CChannelNetwork_Altitude(void)
{
	//-----------------------------------------------------
	Set_Name(_TL("Vertical Distance to Channel Network"));

	Set_Author		(SG_T("(c) 2002 by O.Conrad"));

	Set_Description	(_TW(
		"This module calculates the vertical distance to a channel network base level. "
		"The algorithm consists of two major steps:\n"
		" 1. Interpolation of a channel network base level elevation grid\n"
		" 2. Subtraction of this grid from the original elevations\n")
	);


	//-----------------------------------------------------
	// Input...

	Parameters.Add_Grid(
		NULL, "ELEVATION"	, _TL("Elevation"),
		_TL("A grid that contains elevation data."),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "CHANNELS"	, _TL("Channel Network"),
		_TW("A grid providing information about the channel network. It is assumed that no-data cells are not part "
		"of the channel network. Vice versa all others cells are recognised as channel network members."),
		PARAMETER_INPUT
	);


	//-----------------------------------------------------
	// Output...

	Parameters.Add_Grid(
		NULL, "ALTITUDE"	, _TL("Vertical Distance to Channel Network"),
		_TL("The resulting grid gives the altitude above the channel network in the same units as the elevation data."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL, "BASELEVEL"	, _TL("Channel Network Base Level"),
		_TL("This optional grid output contains the interpolated channel network base level elevations."),
		PARAMETER_OUTPUT_OPTIONAL
	);


	//-----------------------------------------------------
	// Options...

	Parameters.Add_Value(
		NULL, "THRESHOLD"	, _TL("Tension Threshold [m]"),
		_TL(""),
		PARAMETER_TYPE_Double, 0.1
	);

	Parameters.Add_Value(
		NULL, "NOUNDERGROUND", _TL("Keep Base Level above Surface"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);
}

//---------------------------------------------------------
CChannelNetwork_Altitude::~CChannelNetwork_Altitude(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CChannelNetwork_Altitude::On_Execute(void)
{
	int			n, nCells, nCells_Start, iStep, nSteps;
	double		max_Change, Threshold;

	//-----------------------------------------------------
	pDTM				= Parameters("ELEVATION")		->asGrid();
	pChannels			= Parameters("CHANNELS")		->asGrid();

	pResult				= Parameters("ALTITUDE")		->asGrid();

	Threshold			= Parameters("THRESHOLD")		->asDouble();
	m_bNoUnderground	= Parameters("NOUNDERGROUND")	->asBool();

	//-----------------------------------------------------
	nCells			= Get_NX() > Get_NY() ? Get_NX() : Get_NY();
	for(nSteps=0; pow(2.0, nSteps + 1) < nCells; nSteps++);
	nCells_Start	= (int)pow(2.0, nSteps);

	//-----------------------------------------------------
	pResult->Assign_NoData();

	pT_Chnl			= SG_Create_Grid(pResult, GRID_TYPE_Byte);
	pT_Temp			= SG_Create_Grid(pResult);

	for(nCells=nCells_Start, iStep=1; nCells>0; nCells/=2, iStep++)
	{
		Process_Set_Text(CSG_String::Format(SG_T("%d [%d]"), iStep, nSteps + 1));

		Initialize_Surface(nCells);

		do
		{
			max_Change	= Set_Surface(nCells);
		}
		while( max_Change > Threshold && Process_Get_Okay(true) );
	}

	delete(pT_Chnl);
	delete(pT_Temp);

	//-----------------------------------------------------
	if( Parameters("BASELEVEL")->asGrid() )
	{
		Parameters("BASELEVEL")->asGrid()->Assign(pResult);
	}

	for(n=0; n<Get_NCells(); n++)
	{
		if( pResult->is_NoData(n) || pDTM->is_NoData(n) )
		{
			pResult->Set_NoData(n);
		}
		else
		{
			pResult->Set_Value(n, pDTM->asDouble(n) - pResult->asDouble(n));
		}
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
void CChannelNetwork_Altitude::Initialize_Surface(int nCells)
{
	int		x, y, i, ix, iy, nx, ny, nz;

	double	z;

	//-----------------------------------------------------
	// 1. Channels...

	pT_Temp->Assign_NoData();
	pT_Chnl->Assign();

	for(y=0; y<Get_NY(); y+=nCells)
	{
		ny	= y + nCells < Get_NY() ? y + nCells : Get_NY();

		for(x=0; x<Get_NX(); x+=nCells)
		{
			nx	= x + nCells < Get_NX() ? x + nCells : Get_NX();
			nz	= 0;
			z	= 0.0;

			for(iy=y; iy<ny; iy++)
			{
				for(ix=x; ix<nx; ix++)
				{
					if( pChannels->is_InGrid(ix, iy) )
					{
						z	+= pDTM->asDouble(ix, iy);
						nz++;
					}
				}
			}

			if( nz > 0 )
			{
				pT_Temp->Set_Value(x, y, z / (double)nz );
				pT_Chnl->Set_Value(x, y, 1.0);
			}
		}
	}

	//-----------------------------------------------------
	// 2. Previous Iteration...

	for(y=0; y<Get_NY(); y+=nCells)
	{
		for(x=0; x<Get_NX(); x+=nCells)
		{
			if( pT_Chnl->asByte(x, y) == false )
			{
				if( !pResult->is_NoData(x, y) )
				{
					pT_Temp->Set_Value(x, y, pResult->asDouble(x, y));
				}
				else
				{
					nz	= 0;
					z	= 0.0;

					for(i=0; i<8; i++)
					{
						ix	= x + nCells * Get_xTo(i);
						iy	= y + nCells * Get_yTo(i);

						if( pResult->is_InGrid(ix, iy) )
						{
							z	+= pResult->asDouble(ix, iy);
							nz++;
						}
					}

					if( nz > 0.0 )
					{
						pT_Temp->Set_Value(x, y, z / (double)nz);
					}
					else
					{
						pT_Temp->Set_Value(x, y, pDTM->asDouble(x, y));
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	// 3. ...

	pResult->Assign(pT_Temp);
}

//---------------------------------------------------------
double CChannelNetwork_Altitude::Set_Surface(int nCells)
{
	int		x, y;

	double	d, dMax;

	dMax	= 0.0;

	for(y=0; y<Get_NY(); y+=nCells)
	{
		for(x=0; x<Get_NX(); x+=nCells)
		{
			if( pT_Chnl->asByte(x, y) == false )
			{
				d	= Get_Changed(x, y, nCells);

				pT_Temp->Set_Value(x, y, d);

				d	= fabs(d - pResult->asDouble(x, y));

				if( d > dMax )
				{
					dMax	= d;
				}
			}
		}
	}

	for(y=0; y<Get_NY(); y+=nCells)
	{
		for(x=0; x<Get_NX(); x+=nCells)
		{
			if( pT_Chnl->asByte(x, y) == false )
			{
				pResult->Set_Value(x, y, pT_Temp->asDouble(x, y));
			}
		}
	}

	return( dMax );
}


//---------------------------------------------------------
double CChannelNetwork_Altitude::Get_Changed(int x, int y, int nCells)
{
	int		i, ix, iy;

	double	n, d, dz;

	for(i=0, d=0.0, n=0.0; i<8; i++)
	{
		ix	= x + nCells * Get_xTo(i);
		iy	= y + nCells * Get_yTo(i);

		if( pResult->is_InGrid(ix, iy) )
		{
			dz	= 1.0 / Get_UnitLength(i);
			d	+= dz * pResult->asDouble(ix, iy);
			n	+= dz;
		}
	}

	if( n > 0.0 )
	{
		d	/= n;

		return( m_bNoUnderground && d > pDTM->asDouble(x, y) ? pDTM->asDouble(x, y) : d );
	}

	return( pResult->asDouble(x, y) );
}

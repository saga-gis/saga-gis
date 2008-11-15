
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
//                     LakeFlood.cpp                     //
//                                                       //
//                 Copyright (C) 2005 by                 //
//                    Volker Wichmann                    //
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
//    e-mail:     reklovw@web.de                         //
//                                                       //
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "LakeFlood.h"

///////////////////////////////////////////////////////////
//														 //
//				Construction/Destruction				 //
//														 //
///////////////////////////////////////////////////////////

CLakeFlood::CLakeFlood(void)
{
	// Parameter Settings for input dialog of the module

	Parameters.Set_Name(_TL("Lake Flood"));
	Set_Author(_TL("Copyrights (c) 2005 by Volker Wichmann"));
	Parameters.Set_Description(_TW(
		"This module can be used to simulate the extent and volume of a lake for a specified water depth in a seed cell.\r\n"
	));


	Parameters.Add_Grid(	
		NULL, "ELEV", _TL("DEM"), 
		_TL("digital elevation model [m]"),
		PARAMETER_INPUT
	);
	Parameters.Add_Grid(	
		NULL, "INDEPTH", _TL("Water Depth"),
		_TL("seed cell coded with local water depth [m], all other cells NoData"),
		PARAMETER_INPUT
	);
	Parameters.Add_Grid(	
		NULL, "OUTDEPTH", _TL("Lake"), 
		_TL("extent of lake, coded with water depth [m]"),
		PARAMETER_OUTPUT
	);

}

//---------------------------------------------------------
CLakeFlood::~CLakeFlood(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////


bool CLakeFlood::On_Execute(void)		
{
	CSG_Grid	*pElev, *pIdepth, *pOdepth;
	CTraceOrder	*newCell, *firstCell, *iterCell, *lastCell;
	int			x, y, ix, iy, i;
	double		wzSeed, z, d;

	pElev		= Parameters("ELEV")->asGrid();
	pIdepth		= Parameters("INDEPTH")->asGrid();
	pOdepth		= Parameters("OUTDEPTH")->asGrid();


	// Initialize Grids
	pOdepth->Assign(0.0);

	
	// Get seed cell and water depth
	for(y=0; y<Get_NY(); y++)									
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( !pIdepth->is_NoData(x, y) )
			{
				if( !pElev->is_NoData(x, y) )
				{
					newCell = new CTraceOrder();
					newCell->x = x;
					newCell->y = y;
					firstCell = newCell;
					wzSeed = pIdepth->asDouble(x, y);
					pOdepth->Set_Value(x, y, wzSeed);
					wzSeed +=  pElev->asDouble(x, y);
					// terminate search
				}
				else
					firstCell = NULL;
			}
		}
	}

	// main
	//-----------------------------------------------------
	iterCell = firstCell;
	lastCell = firstCell;

	while( iterCell != NULL ) 
	{
		x	= iterCell->x;
		y	= iterCell->y;

		z		= pElev->asDouble(x, y);

		for(i=0; i<8; i++)												
		{
			ix	= Get_xTo(i, x);			
			iy	= Get_yTo(i, y);			
						
			if(	is_InGrid(ix, iy) && !pElev->is_NoData(ix, iy) && pOdepth->asDouble(ix, iy) == 0.0 )
			{ 
				d	= pElev->asDouble(ix, iy);
				if( d < wzSeed )
				{
					pOdepth->Set_Value(ix, iy, (wzSeed - d));
					newCell = new CTraceOrder();
					newCell->x = ix;
					newCell->y = iy;
					newCell->prev = lastCell;
					lastCell->next = newCell;
					lastCell = newCell;
				}
			}
		}
	
		newCell = firstCell;

		if( newCell->next == NULL )
		{
			firstCell = NULL;
			delete (newCell);
		}
		else
		{
			newCell->next->prev = NULL;
			firstCell = newCell->next;
			newCell->next = NULL;
			delete (newCell);
		}

		iterCell = firstCell;
	}

	//-----------------------------------------------------

	return( true );
}


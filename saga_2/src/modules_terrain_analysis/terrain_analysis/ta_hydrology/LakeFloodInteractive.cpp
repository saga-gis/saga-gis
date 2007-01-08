
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
//               LakeFloodInteractive.cpp                //
//                                                       //
//                Copyright (C) 2005-6 by                //
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
//    e-mail:     volker.wichmann@ku-eichstaett.de       //
//                                                       //
//    contact:    Volker Wichmann                        //
//                Research Associate                     //
//                Chair of Physical Geography		     //
//				  KU Eichstätt-Ingolstadt				 //
//                Ostenstr. 18                           //
//                85072 Eichstätt                        //
//                Germany                                //
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

CLakeFloodInteractive::CLakeFloodInteractive(void)
{
	// Parameter Settings for input dialog of the module

	Parameters.Set_Name(_TL("Lake Flood"));
	Set_Author(_TL("Copyrights (c) 2005-6 by Volker Wichmann"));
	Parameters.Set_Description(_TW(
		"This module works interactively and can be used to simulate the extent and volume of a lake for a given water depth. "
		"Execute the module and use the action tool on a cell of the digital elevation model to fill the lake starting from this "
		"location. Execute the module again to terminate module operation.\r\n")
	);


	Parameters.Add_Grid(	
		NULL, "ELEV", _TL("DEM"), 
		_TL("digital elevation model [m]"),
		PARAMETER_INPUT
	);
	Parameters.Add_Value(
		NULL, "INDEPTH", _TL("Water Depth"),
		_TL("water depth at the user specified cell [m]"),
		PARAMETER_TYPE_Double,
		2.5
	); 
	Parameters.Add_Grid(	
		NULL, "OUTDEPTH", _TL("Lake"), 
		_TL("extent of lake, coded with local water depth [m]"),
		PARAMETER_OUTPUT
	);

}

//---------------------------------------------------------
CLakeFloodInteractive::~CLakeFloodInteractive(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////


bool CLakeFloodInteractive::On_Execute(void)									
{
	pElev		= Parameters("ELEV")->asGrid();
	pOdepth		= Parameters("OUTDEPTH")->asGrid();
	wzSeed		= Parameters("INDEPTH")->asDouble();

	return (true);
}

//---------------------------------------------------------
bool CLakeFloodInteractive::On_Execute_Finish(void)
{
	return( true );
}


//---------------------------------------------------------
bool CLakeFloodInteractive::On_Execute_Position(CSG_Point ptWorld, TSG_Module_Interactive_Mode Mode)
{
	//-----------------------------------------------------
	if(  Mode == MODULE_INTERACTIVE_LDOWN )
	{
		int			x, y, ix, iy, i;
		double		z, d, wz;

		wz	= wzSeed;
	
		x	= Get_System()->Get_xWorld_to_Grid(ptWorld.Get_X());
		y	= Get_System()->Get_yWorld_to_Grid(ptWorld.Get_Y());
	
		if( pElev && pElev->is_InGrid(x, y) )
		{
			pOdepth->Assign(0.0);

			newCell = new CTraceOrder();
			newCell->x = x;
			newCell->y = y;
			firstCell = newCell;
			
			pOdepth->Set_Value(x, y, wzSeed);
			wz +=  pElev->asDouble(x, y);
		

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
						if( d < wz )
						{
							pOdepth->Set_Value(ix, iy, (wz - d));
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
			Message_Add(_TL("ready"), false);
			DataObject_Update(pOdepth, pOdepth->Get_ZMin(), pOdepth->Get_ZMax());

			return( true );
		}
	}

	return( false );
}

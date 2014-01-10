/**********************************************************
 * Version $Id: LakeFloodInteractive.cpp 1921 2014-01-09 10:24:11Z oconrad $
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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

CLakeFloodInteractive::CLakeFloodInteractive(void)
{
	Set_Name		(_TL("Lake Flood"));
	Set_Author		(SG_T("Volker Wichmann (c) 2005-2010"));
	Parameters.Set_Description(_TW(
		"This module works interactively and can be used to flood a digital elevation model for a given water depth or water level. "
		"Execute the module and use the action tool on a cell to flood the digital elevation model from this "
		"location. Execute the module again to terminate module operation.\r\n")
	);


	Parameters.Add_Grid(	
		NULL, "ELEV", _TL("DEM"), 
		_TL("digital elevation model"),
		PARAMETER_INPUT
	);
	Parameters.Add_Value(
		NULL, "WATER", _TL("Water"),
		_TL("local water depth or absolute water level at the user specified cell"),
		PARAMETER_TYPE_Double,
		2.5
	);
	Parameters.Add_Value(
		NULL, "LEVEL", _TL("Absolute Water Level"),
		_TL("check this in case parameter 'water' is absolute water level"),
		PARAMETER_TYPE_Bool, false
	);
	Parameters.Add_Grid(	
		NULL, "OUTDEPTH", _TL("Lake"), 
		_TL("extent of lake, coded with local water depth"),
		PARAMETER_OUTPUT
	);
	Parameters.Add_Grid(	
		NULL, "OUTLEVEL", _TL("Surface"), 
		_TL("Flooded digital elevation model"),
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
	m_water		= Parameters("WATER")->asDouble();
	m_bLevel	= Parameters("LEVEL")->asBool();
	pOdepth		= Parameters("OUTDEPTH")->asGrid();
	pOlevel		= Parameters("OUTLEVEL")->asGrid();

	pOdepth->Assign(0.0);
	pOlevel->Assign(pElev);

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
		double		level;
	
		x	= Get_System()->Get_xWorld_to_Grid(ptWorld.Get_X());
		y	= Get_System()->Get_yWorld_to_Grid(ptWorld.Get_Y());
	
		if( pElev->is_InGrid(x, y, true) )
		{
			if( !m_bLevel )
				level = m_water + pElev->asDouble(x, y);
			else
				level = m_water;


			if( level <= pOlevel->asDouble(x, y) )
				return (true);


			newCell		= new CTraceOrder();
			newCell->x	= x;
			newCell->y	= y;
			firstCell	= newCell;

			pOdepth->Set_Value(x, y, level - pElev->asDouble(x, y));
			pOlevel->Set_Value(x, y, level);


			iterCell = firstCell;
			lastCell = firstCell;

			while( iterCell != NULL ) 
			{
				x	= iterCell->x;
				y	= iterCell->y;

				for( i=0; i<8; i++ )												
				{
					ix	= Get_xTo(i, x);			
					iy	= Get_yTo(i, y);			
								
					if(	is_InGrid(ix, iy) && !pElev->is_NoData(ix, iy) && pOlevel->asDouble(ix, iy) < level )
					{ 
						pOdepth->Set_Value(ix, iy, level - pElev->asDouble(ix, iy));
						pOlevel->Set_Value(ix, iy, level);
						newCell = new CTraceOrder();
						newCell->x = ix;
						newCell->y = iy;
						newCell->prev = lastCell;
						lastCell->next = newCell;
						lastCell = newCell;
					}
				}
			
				newCell = firstCell;

				if( newCell->next == NULL )
				{
					firstCell = lastCell = NULL;
					delete (newCell);
					newCell = NULL;
				}
				else
				{
					newCell->next->prev = NULL;
					firstCell = newCell->next;
					newCell->next = NULL;
					delete (newCell);
					newCell = NULL;
				}

				iterCell = firstCell;
			}

			SG_UI_Msg_Add(_TL("ready ..."), true);
			DataObject_Update(pOdepth, pOdepth->Get_ZMin(), pOdepth->Get_ZMax());
			DataObject_Update(pOlevel, pOlevel->Get_ZMin(), pOlevel->Get_ZMax());
			return( true );
		}
	}

	return( false );
}

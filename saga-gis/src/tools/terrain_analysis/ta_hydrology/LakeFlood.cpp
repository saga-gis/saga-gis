/**********************************************************
 * Version $Id: LakeFlood.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
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

	Set_Name		(_TL("Lake Flood"));
	Set_Author		(SG_T("Volker Wichmann (c) 2005-2010"));
	Set_Description	(_TW("This tool can be used to flood a digital elevation model from seed points. "
						"Seed points have to be coded either with local water depth or absolute water level.\r\n"
	));


	Parameters.Add_Grid(	
		NULL, "ELEV", _TL("DEM"), 
		_TL("digital elevation model"),
		PARAMETER_INPUT
	);
	Parameters.Add_Grid(	
		NULL, "SEEDS", _TL("Seeds"),
		_TL("seed cells coded with local water depth or absolute water level, all other cells NoData"),
		PARAMETER_INPUT
	);
	Parameters.Add_Value(
		NULL, "LEVEL", _TL("Absolute Water Levels"),
		_TL("check this in case seed cells are coded with absolute water level"),
		PARAMETER_TYPE_Bool, false
	);
	Parameters.Add_Grid(	
		NULL, "OUTDEPTH", _TL("Lake"), 
		_TL("extent of lake, coded with water depth"),
		PARAMETER_OUTPUT
	);
	Parameters.Add_Grid(	
		NULL, "OUTLEVEL", _TL("Surface"), 
		_TL("Flooded digital elevation model"),
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

//---------------------------------------------------------
bool CLakeFlood::On_Execute(void)		
{
	CSG_Grid	*pElev, *pSeeds, *pOdepth, *pOlevel;
	CTraceOrder	*newCell, *firstCell, *iterCell, *lastCell;
	int			x, y, ix, iy, i;
	bool		bLevel;
	SEED		thisSeed;
	std::list<SEED>				seeds;
	std::list<SEED>::iterator	it;


	//---------------------------------------------------------
	pElev		= Parameters("ELEV")->asGrid();
	pSeeds		= Parameters("SEEDS")->asGrid();
	bLevel		= Parameters("LEVEL")->asBool();
	pOdepth		= Parameters("OUTDEPTH")->asGrid();
	pOlevel		= Parameters("OUTLEVEL")->asGrid();



	//---------------------------------------------------------
	// Initialize

	pOdepth->Assign(0.0);
	pOlevel->Assign(pElev);

	for( y=0; y<Get_NY(); y++ )									
	{
		for( x=0; x<Get_NX(); x++ )
		{
			if( !pSeeds->is_NoData(x, y) && !pElev->is_NoData(x, y) )
			{
				thisSeed.x	= x;
				thisSeed.y	= y;
				if( bLevel )
					thisSeed.z	= pSeeds->asDouble(x, y);
				else
					thisSeed.z	= pSeeds->asDouble(x, y) + pElev->asDouble(x, y);
				seeds.push_back(thisSeed);
			}
		}
	}


	//-----------------------------------------------------
	// Flood

	for( it=seeds.begin(); it!=seeds.end(); it++ )
	{
		if( it->z <= pOlevel->asDouble(it->x, it->y) )
			continue;

		newCell		= new CTraceOrder();
		newCell->x	= it->x;
		newCell->y	= it->y;
		firstCell	= newCell;

		pOdepth->Set_Value(it->x, it->y, it->z - pElev->asDouble(it->x, it->y));
		pOlevel->Set_Value(it->x, it->y, it->z);


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
							
				if(	is_InGrid(ix, iy) && !pElev->is_NoData(ix, iy) && pOlevel->asDouble(ix, iy) < it->z )
				{ 
					pOdepth->Set_Value(ix, iy, it->z - pElev->asDouble(ix, iy));
					pOlevel->Set_Value(ix, iy, it->z);
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
	}


	//-----------------------------------------------------
	return( true );
}


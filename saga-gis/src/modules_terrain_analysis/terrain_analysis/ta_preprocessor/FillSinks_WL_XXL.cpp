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
//                    ta_preprocessor                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    FillSinks_WL.cpp                   //
//                                                       //
//                 Copyright (C) 2007 by                 //
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
//    e-mail:     reklovw@web.de					     //
//                                                       //
///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "FillSinks_WL.h"


///////////////////////////////////////////////////////////
//														 //
//				Construction/Destruction				 //
//														 //
///////////////////////////////////////////////////////////

CFillSinks_WL_XXL::CFillSinks_WL_XXL(void)
{

	Set_Name		(_TL("Fill Sinks XXL (Wang & Liu)"));
	Set_Author		(_TL("Copyrights (c) 2007 by Volker Wichmann"));
	Set_Description	(_TW(
		"This module uses an algorithm proposed by Wang & Liu to identify and fill surface depressions in "
		"digital elevation models.\n"
		"The method was enhanced to allow the creation of hydrologic sound elevation models, i.e. not only to "
		"fill the depression(s) but also to preserve a downward slope along the flow path. If desired, this is accomplished "
		"by preserving a minimum slope gradient (and thus elevation difference) between cells.\n"
		"This version of the module is designed to work on large data sets (e.g. LIDAR data), with smaller "
		"datasets you might like to check out the fully featured standard version of the module.\n\n\n"
		"References:\n"
		"Wang, L. & H. Liu (2006): An efficient method for identifying and filling surface depressions in "
		"digital elevation models for hydrologic analysis and modelling. International Journal of Geographical "
		"Information Science, Vol. 20, No. 2: 193-213.\n"
	));


	Parameters.Add_Grid(	
		NULL, "ELEV", _TL("DEM"), 
		_TL("Digital elevation model"),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(	
		NULL, "FILLED", _TL("Filled DEM"), 
		_TL("Depression-free digital elevation model"),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL, "MINSLOPE", _TL("Minimum Slope [Degree]"),
		_TL("Minimum slope gradient to preserve from cell to cell; with a value of zero sinks are filled up to the spill elevation (which results in flat areas). Unit [Degree]"),
		PARAMETER_TYPE_Double, 0.1, 0.0, true
	);

}

//---------------------------------------------------------
CFillSinks_WL_XXL::~CFillSinks_WL_XXL(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////



bool CFillSinks_WL_XXL::On_Execute(void)		
{
	CSG_Grid	*pElev, *pFilled;
	PriorityQ	theQueue;
	CFillSinks_WL_Node		tempNode;

	int			x, y, ix, iy, i;
	double		z, iz, progress;
	double		minslope, mindiff[8];
	bool		preserve;


	pElev		= Parameters("ELEV")->asGrid();
	pFilled		= Parameters("FILLED")->asGrid();
	minslope	= Parameters("MINSLOPE")->asDouble();		

	pFilled->Set_Name(CSG_String::Format(SG_T("%s [%s]"), pElev->Get_Name(), _TL("no sinks")));


	if( minslope > 0.0 )
	{
		minslope = tan(minslope * M_DEG_TO_RAD);
		for(i=0; i<8; i++)
			mindiff[i] = minslope * Get_Length(i);
		preserve = true;
	}
	else
		preserve = false;

	
	pFilled->Assign_NoData();
	

	// fill priority queue with boundary cells, i.e. seed cells
	for(y=0; y<Get_NY(); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( !pElev->is_NoData(x, y) )
			{
				for(i=0; i<8; i++)
				{
					ix = Get_xTo(i, x);
					iy = Get_yTo(i, y);
					if( !is_InGrid(ix, iy) || pElev->is_NoData(ix, iy) )
					{
						z = pElev->asDouble(x, y);

						tempNode.x		= x;
						tempNode.y		= y;
						tempNode.spill	= z;
						theQueue.push( tempNode );

						pFilled->Set_Value(x, y, z);
						break;
					}
				}
			}
		}
	}


	// process queue
	progress = 0.0;

	while( !theQueue.empty() )
	{
		PriorityQ::value_type tempNode = theQueue.top();

		x = tempNode.x;
		y = tempNode.y;
		theQueue.pop();

		z = pFilled->asDouble(x, y);

		for(i=0; i<8; i++)
		{
			ix = Get_xTo(i, x);
			iy = Get_yTo(i, y);
			if( is_InGrid(ix, iy) && !pElev->is_NoData(ix, iy) && pFilled->is_NoData(ix, iy) )
			{
				iz = pElev->asDouble(ix, iy);

				if( preserve )
				{
					if( iz < (z + mindiff[i]) )
						iz = z + mindiff[i];
				}
				else if( iz < z )
					iz = z;

				tempNode.x		= ix;
				tempNode.y		= iy;
				tempNode.spill	= iz;
				theQueue.push( tempNode );

				pFilled->Set_Value(ix, iy, iz);
			}
		}

		progress += 1.0;
		if( ((int)progress) % 10000 == 0 )
			Set_Progress(progress, pElev->Get_NCells());
			//DataObject_Update(pFilled, pElev->Get_ZMin(), pElev->Get_ZMax(), true);
		
	}

	return (true);
}


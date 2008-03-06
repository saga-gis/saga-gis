
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
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
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

CFillSinks_WL::CFillSinks_WL(void)
{

	Set_Name		(_TL("Fill Sinks (Wang & Liu)"));
	Set_Author		(_TL("Copyrights (c) 2007 by Volker Wichmann"));
	Set_Description	(_TW(
		"This module uses an algorithm proposed by Wang & Liu to identify and fill surface depressions in "
		"digital elevation models.\n"
		"The method was enhanced to allow the creation of hydrologic sound elevation models, i.e. not only to "
		"fill the depression(s) but also to preserve a downward slope along the flow path. If desired, this is accomplished "
		"by preserving a minimum slope gradient (and thus elevation difference) between cells.\n"
		"This is the fully featured version of the module creating a depression less DEM, a flow path grid and "
		"a grid with watershed basins. If you encounter problems processing large data sets "
		"(e.g. LIDAR data) with this module try the basic version (Fill Sinks XXL).\n\n\n"
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

	Parameters.Add_Grid(
		NULL, "FDIR", _TL("Flow Directions"),
		_TL("Computed flow directions, 0=N, 1=NE, 2=E, ... 7=NW"),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL, "WSHED", _TL("Watershed Basins"),
		_TL("Delineated watershed basins"),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL, "MINSLOPE", _TL("Minimum Slope"),
		_TL("Minimum slope gradient to preserve from cell to cell; with a value of zero sinks are filled up to the spill elevation (which results in flat areas). Unit [Degree]"),
		PARAMETER_TYPE_Double, 0.0, 0.0, true
	);

}

//---------------------------------------------------------
CFillSinks_WL::~CFillSinks_WL(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////



bool CFillSinks_WL::On_Execute(void)		
{
	CSG_Grid	*pElev, *pSeed, *pFdir, *pWshed;
	PriorityQ	theQueue;
	CFillSinks_WL_Node		tempNode;

	int			x, y, ix, iy, i;
	int			inverse[8] = { 4, 5, 6, 7, 0, 1, 2, 3 };
	double		z, iz, progress;
	double		minslope, mindiff[8];
	bool		preserve;
	long		id;

	pElev		= Parameters("ELEV")->asGrid();
	pFilled		= Parameters("FILLED")->asGrid();
	pFdir		= Parameters("FDIR")->asGrid();
	pWshed		= Parameters("WSHED")->asGrid();
	minslope	= Parameters("MINSLOPE")->asDouble();		


	if( minslope > 0.0 )
	{
		minslope = tan(minslope * M_DEG_TO_RAD);
		for(i=0; i<8; i++)
			mindiff[i] = minslope * Get_Length(i);
		preserve = true;
	}
	else
		preserve = false;


	pSeed	= (CSG_Grid *) new CSG_Grid(GRID_TYPE_Bit, pElev->Get_NX(), pElev->Get_NY(), pElev->Get_Cellsize(), pElev->Get_XMin(), pElev->Get_YMin());
	
	pFilled->Assign_NoData();
	pWshed->Assign_NoData();
	pFdir->Assign_NoData();
	pSeed->Assign(0.0);


	
	// fill priority queue with boundary, i.e. seed cells
	id = 0;
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
						pFilled->Set_Value(x, y, z);
						pSeed->Set_Value(x, y, 1.0);
						pWshed->Set_Value(x, y, id);
						id += 1;

						tempNode.x		= x;
						tempNode.y		= y;
						tempNode.spill	= z;
						theQueue.push( tempNode );
						break;
					}
				}
			}
		}
	}

	// work through least cost path
	progress = 0.0;

	while( !theQueue.empty() )
	{
		PriorityQ::value_type tempNode = theQueue.top();

		x = tempNode.x;
		y = tempNode.y;
		theQueue.pop();

		id	= (long) pWshed->asDouble(x, y);
		z	= pFilled->asDouble(x, y);

		for(i=0; i<8; i++)
		{
			ix = Get_xTo(i, x);
			iy = Get_yTo(i, y);
			if( is_InGrid(ix, iy) && !pElev->is_NoData(ix, iy) )
			{
				if( pFilled->is_NoData(ix, iy) )
				{
					iz = pElev->asDouble(ix, iy);

					if( preserve )
					{
						if( iz < (z + mindiff[i]) )
							iz = z + mindiff[i];
					}
					else if( iz <= z )
					{
						iz = z;
						pFdir->Set_Value(ix, iy, inverse[i]);
					}

					tempNode.x		= ix;
					tempNode.y		= iy;
					tempNode.spill	= iz;
					theQueue.push( tempNode );

					pFilled->Set_Value(ix, iy, iz);
					pWshed->Set_Value(ix, iy, id);
				}
				else if( pSeed->asInt(ix, iy) == 1 )
					pWshed->Set_Value(ix, iy, id);
			}
		}

		if( pFdir->is_NoData(x, y) )
			pFdir->Set_Value(x, y, Get_Dir(x, y, z));


		progress += 1.0;
		if( ((int)progress) % 10000 == 0 )
			Set_Progress(progress, pElev->Get_NCells());
			//DataObject_Update(pFilled, pElev->Get_ZMin(), pElev->Get_ZMax(), true);
		
	}


	delete (pSeed);
	return (true);
}


int CFillSinks_WL::Get_Dir(int x, int y, double z)
{
	int		ix, iy, i, steepestn;
	double	iz, max, gradient;

	max = 0.0;
	steepestn = -1;

	for(i=0; i<8; i++)
	{
		ix = Get_xTo(i, x);
		iy = Get_yTo(i, y);

		if(	is_InGrid(ix, iy) && !pFilled->is_NoData(ix, iy) && (iz = pFilled->asDouble(ix, iy)) < z)
		{
			gradient = (z - iz) / Get_Length(i);
			if( max < gradient )
			{
				max = gradient;
				steepestn = i;
			}
		}
	}

	return (steepestn);
}
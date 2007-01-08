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
//                Grid_Buffer_Proximity.cpp              //
//                                                       //
//                 Copyright (C) 2006 by                 //
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

#include "Grid_Buffer_Proximity.h"

///////////////////////////////////////////////////////////
//														 //
//				Construction/Destruction				 //
//														 //
///////////////////////////////////////////////////////////

CGrid_Proximity_Buffer::CGrid_Proximity_Buffer(void){

	Set_Name(_TL("Grid Proximity Buffer"));
	Set_Author(_TL("Copyrights (c) 2006 by Volker Wichmann"));
	Set_Description(_TW(
		"This module calculates the euclidian distance within a buffer distance from all NoData cells to the nearest "
		"valid neighbour in a source grid. Additionally, the source cells define the zones that will be used in the "
		"euclidean allocation calculations. Cell values in the source grid are treated as IDs (integer) and "
		"used in the allocation grid to identify the grid value of the closest source cell. If a cell is at an equal "
		"distance to two or more sources, the cell is assigned to the source that is first encountered in the modules scanning "
		"process. The buffer grid is a "
		"reclassification of the distance grid using a user specified equidistance to create a set of discrete distance "
		"buffers from source features. The buffer zones are coded with the maximum distance value of the corresponding buffer interval. " 
		"The output value type for the distance grid is floating-point. The output values for the allocation and buffer "
		"grid are of type integer. The duration of module execution is dependent on the number of source cells and the buffer distance."));

	Parameters.Add_Grid(NULL, 
						"SOURCE",
						_TL("Source Grid"), 						
						_TL("Grid with features to be buffered [Category/NoData]"), 
						PARAMETER_INPUT);

	Parameters.Add_Grid(NULL, 
						"DISTANCE", 
						_TL("Distance Grid"), 
						_TL("Grid with euclidian distance to nearest source cell [grid units]"), 
						PARAMETER_OUTPUT, 
						true, 
						GRID_TYPE_Double);

	Parameters.Add_Grid(NULL, 
						"ALLOC", 
						_TL("Allocation Grid"), 
						_TL("Grid with category of nearest source cell [Category]"), 
						PARAMETER_OUTPUT, 
						true, 
						GRID_TYPE_Int);

	Parameters.Add_Grid(NULL, 
						"BUFFER", 
						_TL("Buffer Grid"), 
						_TL("Reclassification of distance grid to buffer zones with a width equal to the equidistance value."), 
						PARAMETER_OUTPUT, 
						true, 
						GRID_TYPE_Int);

	Parameters.Add_Value(NULL, 
						"DIST", 
						_TL("Buffer distance"), 
						_TL("Buffer distance (grid units)."), 
						PARAMETER_TYPE_Double, 
						500.0);

	Parameters.Add_Value(NULL, 
						"IVAL", 
						_TL("Equidistance"), 
						_TL("Reclassify buffer distance to intervals of euqidistance ..."), 
						PARAMETER_TYPE_Int, 
						100);
						

}


CGrid_Proximity_Buffer::~CGrid_Proximity_Buffer(void)
{}

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

bool CGrid_Proximity_Buffer::On_Execute(void){
	
	CSG_Grid	*pSource, *pDistance, *pAlloc, *pBuffer;
	double 		dBufDist, dDist, cellSize;
	int 		x, y, i, j, imin, imax, jmin, jmax, iBufDist, alloc, ival;

	pSource 	= Parameters("SOURCE")->asGrid();
	pDistance 	= Parameters("DISTANCE")->asGrid();
	pAlloc 		= Parameters("ALLOC")->asGrid();
	pBuffer 	= Parameters("BUFFER")->asGrid();
	ival		= Parameters("IVAL")->asInt();

	cellSize = pSource->Get_Cellsize();
	dBufDist = Parameters("DIST")->asDouble() / cellSize;
	iBufDist = (int) (dBufDist + 2.0);
	dBufDist = pow(dBufDist, 2);

	pDistance->Assign_NoData();
	pAlloc->Assign_NoData();
	pBuffer->Assign_NoData();

    for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{		
		for(x=0; x<Get_NX(); x++)
		{			
 			if( !pSource->is_NoData(x, y) )
			{
				alloc = pSource->asInt(x, y);

				pAlloc->Set_Value(x, y, alloc);
				pDistance->Set_Value(x, y, 0.0);

				imin = __max(0, x-iBufDist);
				imax = __min(x+iBufDist, Get_NX());
				jmin = __max(0, y-iBufDist);
				jmax = __min(y+iBufDist, Get_NY());

				for(i=imin; i<imax; i++)
				{
					for(j=jmin; j<jmax; j++)
					{
						if( pSource->is_NoData(i, j) )
						{
							dDist= (x-i)*(x-i)+(y-j)*(y-j);		
						
							if( dDist <= dBufDist && (pDistance->is_NoData(i, j) || pDistance->asDouble(i, j) > dDist) )
							{
								pDistance->Set_Value(i, j, dDist);
								pAlloc->Set_Value(i, j, alloc);
							}
						}
					}//for
				}//for
			}//if
		}//for
	}//for						


	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{		
		for(x=0; x<Get_NX(); x++)
		{
			if( !pDistance->is_NoData(x, y) )
			{
				dDist = sqrt(pDistance->asDouble(x, y)) * cellSize;
				pDistance->Set_Value(x, y, dDist);

				i = 0;
				while( i< dDist )
					i += ival;
				pBuffer->Set_Value(x, y, i);
			}
		}
	}

	return( true );

}

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

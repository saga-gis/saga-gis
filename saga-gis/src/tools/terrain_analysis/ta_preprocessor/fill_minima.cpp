
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    ta_preprocessor                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    fill_minima.cpp                    //
//                                                       //
//              Copyright (C) 2015-2023 by               //
//                   	Neil Flood                       //
//                  Justus Spitzmüller                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation, either version 3 of the     //
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
//    e-mail: j.spitzmueller@uni-goettingen.de           //
//                                                       //
//    contact:    Justus Spitzmüller                     //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////



/* This file is part of 'python-fmask' - a cloud masking module
* Copyright (C) 2015  Neil Flood
*
* This program is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 3
* of the License, or (at your option) any later version.
*
* This program is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
/*
Module to implement filling of local minima in a raster surface. 

The algorithm is from 
    Soille, P., and Gratin, C. (1994). An efficient algorithm for drainage network
        extraction on DEMs. J. Visual Communication and Image Representation. 
        5(2). 181-189. 
        
The algorithm is intended for hydrological processing of a DEM, but is used by the 
Fmask cloud shadow algorithm as part of its process for finding local minima which 
represent potential shadow objects. 
*/
#include "fill_minima.h"
#include <math.h>




CFillMinima::CFillMinima(void)
{
	Set_Name 		(_TL("Fill Minima"));

	Set_Author 		("Neil Flood (c) 2015, Justus Spitzmueller (c) 2023");

	Set_Version 	("1.1");

	Set_Description(_TW(
		"Minima filling for integer grids."
	));

	Add_Reference("https://www.pythonfmask.org/",
		SG_T("Python Fmask")
	);

    Add_Reference("Soille, P., & C. Gratin", "1994", 
		"An efficient algorithm for drainage network extraction on DEMs.",
		"J. Visual Communication and Image Representation. 5(2): 181-189."
	);
	
	//-----------------------------------------------------
	Parameters.Add_Grid(
		"", "DEM"		, _TL("DEM"),
		_TL("digital elevation model [m]"),
		PARAMETER_INPUT, true
	);

	Parameters.Add_Grid(
		"", "RESULT"	, _TL("Filled DEM"),
		_TL("processed DEM"),
		PARAMETER_OUTPUT, true, SG_DATATYPE_Byte 
	);

	Parameters.Add_Double(
		"", "BOUNDARY", _TL("Boundary Value"), _TL(""), 0.197
	);
}


//#define max(a,b) ((a) > (b) ? (a) : (b))

static int	xnb[] = { -1, 0, 1, 1, 1, 0,-1,-1};
static int	ynb[] = {  1, 1, 1, 0,-1,-1,-1, 0};

bool CFillMinima::On_Execute(void)
{
	CSG_Grid *pInput = Parameters("DEM")->asGrid();
	double NoDataValue = pInput->Get_NoData_Value();
	
	CSG_Grid* pOutput = Parameters("RESULT")->asGrid();
	pOutput->Create( Get_System(), pInput->Get_Type() );
	pOutput->Set_Name( CSG_String::Format("%s_Fill", pInput->Get_Name()) );
	pOutput->Set_NoData_Value( NoDataValue );
	pOutput->Set_Scaling( pInput->Get_Scaling(), pInput->Get_Offset() );

	CSG_Grid Copy( Get_System(), pInput->Get_Type() );
	Copy.Set_NoData_Value( NoDataValue );
	Copy.Set_Scaling( pInput->Get_Scaling(), pInput->Get_Offset() );
	
	int BoundaryVal = (int) ((Parameters("BOUNDARY")->asDouble() - pInput->Get_Offset()) / pInput->Get_Scaling() );

	int hMin = INT_MAX;
	int hMax = INT_MIN;

	Process_Set_Text(_TL("Creating statistics"));

	#pragma omp parallel for reduction(min:hMin) reduction(max:hMax) 
	for(sLong y=0; y<Get_NY(); y++)
	{
		for(sLong x=0; x<Get_NX(); x++)
		{
			if( !pInput->is_NoData(x,y) )
			{
				int value = pInput->asInt(x, y, false);
				hMin = std::min( hMin, value );
				hMax = std::max( hMax, value );
			}
		}
	}
	
	Process_Set_Text(_TL("Initializing output"));
	#pragma omp parallel for
	for(sLong y=0; y<Get_NY(); y++)
	{
		for(sLong x=0; x<Get_NX(); x++)
		{
			if( pInput->is_NoData(x,y) )
			{
				pOutput->Set_NoData(x,y);
				Copy.Set_NoData(x,y);
			}
			else 
			{
				pOutput->Set_Value( x, y, hMax, false);
				Copy.Set_Value( x, y, hMax, false);
			}
		}
	}

	int Num_Threads = SG_OMP_Get_Max_Num_Threads();
    int Chunk_Size = Get_NX() / Num_Threads;
	while( Chunk_Size % 4 != 0 ){ Chunk_Size++; }
	int Half = Chunk_Size / 2;
	int Overlap = Half;

	Process_Set_Text(_TL("Filling Minima"));
	if( Num_Threads <= 2 )
	{
		PixelQueue* pixQ = 	Create_Queue( pInput, pOutput, hMin, hMax, BoundaryVal, 0, Get_NX(), 0, Get_NY() );
							Fill_Queue(   pInput, pOutput, pixQ, hMin, hMax, 0, Get_NX(), 0, Get_NY() );
	}
	else 
	{
		#pragma omp parallel
		{
			int Thread_ID = SG_OMP_Get_Thread_Num();

			int xStart	=  Thread_ID * Chunk_Size - Overlap;
			int xEnd 	= (Thread_ID * Chunk_Size) + Chunk_Size + Overlap;
			if( xStart < 0 ) 		xStart = 0;
			if( xEnd > Get_NX() ) 	xEnd = Get_NX();

			int yStart = 0;
			int yEnd = Get_NY();
			
			PixelQueue* pixQ = 	Create_Queue( pInput, Thread_ID % 2 ? &Copy : pOutput, hMin, hMax, BoundaryVal, xStart, xEnd, yStart, yEnd );
								Fill_Queue(   pInput, Thread_ID % 2 ? &Copy : pOutput, pixQ, hMin, hMax, xStart, xEnd, yStart, yEnd);
			
		}

		#pragma omp parallel for
		for(sLong y=0; y<Get_NY(); y++)
		{
			for(sLong x=0; x<Get_NX(); x++)
			{
				if( !pOutput->is_NoData(x,y) )
				{
					int oValue = pOutput->asInt(x, y, false);
					int cValue = Copy.asInt(x, y, false);

					int writeValue = (oValue == hMax || cValue == hMax) ? std::min( oValue, cValue ) : std::max( oValue, cValue );
					pOutput->Set_Value(x,y, writeValue, false );
				}
			}
		}
	}

	return( true );
}

bool CFillMinima::Fill_Queue( CSG_Grid *pInput, CSG_Grid *pOutput, PixelQueue *pixQ, int hMin, int hMax, sLong xStart, sLong xEnd, sLong yStart, sLong yEnd )
{
	#ifdef _DEBUG 
	CSG_Simple_Statistics Stats;
	#endif

    PQel *p, *nbrs, *pNbr, *pNext;
    int hCrt = (int)hMin;
	
    do 
	{
        while (! PQ_empty(pixQ, hCrt)) 
		{
            p = PQ_first(pixQ, hCrt);
            //nbrs = neighbours(p, yStart, yEnd, xStart, xEnd );

			PQel *pl = NULL;
			for (int ii=-1; ii<=1; ii++)
			{
				for (int jj=-1; jj<=1; jj++)
				{
					if (!(ii == 0 && jj == 0))
					{
						int i = p->i + ii;
						int j = p->j + jj;
						if( i >= yStart && i < yEnd && j >= xStart && j < xEnd ) 
						{
							PQel *pNew = newPix(i, j);
							pNew->next = pl;
							pl = pNew;
						}
					}
				}
			}

            pNbr = pl;
            while (pNbr != NULL) 
			{
                int x = pNbr->j;
                int y = pNbr->i;
                // Exclude null area of original image 
                //if (! *((npy_bool*)PyArray_GETPTR2(pNullMask, r, c)))
                if ( !pInput->is_NoData(x, y) && y >= yStart && y < yEnd && x >= xStart && x < xEnd )
				{
                    int imgval = pInput->asInt(x,y, false);
                    int img2val = pOutput->asInt(x,y, false);
                    if (img2val == hMax) 
					{
                        img2val = std::max(hCrt, imgval);
                        // *((npy_int16*)PyArray_GETPTR2(pimg2, r, c)) = img2val;
						pOutput->Set_Value(x,y, img2val, false);
                        PQ_add(pixQ, pNbr, img2val);
						
						#ifdef _DEBUG 
						Stats += hCrt - img2val;
						#endif
                    }
                }
                pNext = pNbr->next;
                free(pNbr);
                pNbr = pNext;
            }
            free(p);
        }
        hCrt++;
    } while (hCrt < hMax);
    
    free(pixQ);

	#ifdef _DEBUG 
	#pragma omp critical
	{
		Message_Fmt("Thread No: %d\n", SG_OMP_Get_Thread_Num() );
		Message_Fmt("Operations: %d, Min: %.2f, Max: %.2f, Mean: %.2f\n", Stats.Get_Count(), Stats.Get_Minimum(), Stats.Get_Maximum(), Stats.Get_Mean());
	}
	#endif
	return true;
}


PixelQueue* CFillMinima::Create_Queue( CSG_Grid *pInput, CSG_Grid *pOutput, int hMin, int hMax, int BoundaryVal, sLong xStart, sLong xEnd, sLong yStart, sLong yEnd )
{
	#ifdef _DEBUG 
	CSG_Simple_Statistics Top_Boundary, Down_Boundary, Left_Boundary, Right_Boundary, Boundary, Values;
	#endif

	PixelQueue *pixQ = PQ_init(hMin,hMax);
	
	for( sLong y=yStart; y<yEnd; y++ )
	{
		for( sLong x=xStart; x<xEnd; x++ )
		{
			if( pInput->is_NoData(x,y) )
			{
				for( int i=0; i<8; i++ )
				{
					int xdiff = x + xnb[i];
					int ydiff = y + ynb[i];
					
					if( pInput->is_InGrid( xdiff, ydiff, true ) )
					{
						PQel *p = newPix(ydiff, xdiff);

                    	int imgval = pInput->asInt(xdiff, ydiff, false);
                    	pInput->Set_Value(xdiff, ydiff, BoundaryVal,false);
						PQ_add(pixQ, p, BoundaryVal);

						#ifdef _DEBUG 
						Boundary += imgval;
						#endif
					}
				}
			}
			else
			{
				if( x == xStart || x == xEnd-1 || y == yStart || y == yEnd-1 )
				{
					PQel *p = newPix(y, x);
                    int imgval = pInput->asInt(x, y, false);
					PQ_add(pixQ, p, imgval);

					#ifdef _DEBUG 
					if( 	 y == yEnd-1 && yEnd != Get_NY()) 	Top_Boundary 	+= imgval;
					else if( y == yStart && yStart != 0 )		Down_Boundary 	+= imgval;
					else if( x == xEnd-1 && xEnd != Get_NX()) 	Right_Boundary 	+= imgval;
					else if( x == xStart && xStart != 0 ) 		Left_Boundary 	+= imgval;
					else 										Boundary 		+= imgval;
					#endif
				}
				#ifdef _DEBUG 
				else
	  			{
					Values += pInput->asInt(x, y, false);
				}
				#endif
			}
		}
	}
	#ifdef _DEBUG 
	#pragma omp critical
	{
		Message_Fmt("Thread No: %d, Cells: %d, Min: %f.2, Max: %f.2\n", SG_OMP_Get_Thread_Num(), Values.Get_Count(), Values.Get_Minimum(), Values.Get_Maximum());
		if( Boundary.Get_Count() ) 		Message_Fmt("Boundary Cells: %d, Min: %f.2, Max: %f.2\n", Boundary.Get_Count(), Boundary.Get_Minimum(), Boundary.Get_Maximum());
		if( Top_Boundary.Get_Count() ) 	Message_Fmt("Top Tile Boundary Cells: %d, Min: %f.2, Max: %f.2\n", Top_Boundary.Get_Count(), Top_Boundary.Get_Minimum(), Top_Boundary.Get_Maximum());
		if( Down_Boundary.Get_Count() ) Message_Fmt("Down Tile Boundary Cells: %d, Min: %f.2, Max: %f.2\n", Down_Boundary.Get_Count(), Down_Boundary.Get_Minimum(), Down_Boundary.Get_Maximum());
		if( Left_Boundary.Get_Count() ) Message_Fmt("Left Tile Boundary Cells: %d, Min: %f.2, Max: %f.2\n", Left_Boundary.Get_Count(), Left_Boundary.Get_Minimum(), Left_Boundary.Get_Maximum());
		if( Right_Boundary.Get_Count() )Message_Fmt("Right Tile Boundary Cells: %d, Min: %f.2, Max: %f.2\n", Right_Boundary.Get_Count(), Right_Boundary.Get_Minimum(), Right_Boundary.Get_Maximum());
	}
	#endif

	return( pixQ );
}


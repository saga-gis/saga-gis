
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

struct Cell {
    sLong x, y;
	int height;
    bool operator>(const Cell &other) const {
        return height > other.height;
    }
};


static int	xnb[] = { 0, 1, 1, 1, 0,-1,-1,-1};
static int	ynb[] = { 1, 1, 0,-1,-1,-1, 0, 1};

#include <vector>
#include <queue>
#include <utility>

bool CFillMinima::On_Execute(void)
{
	m_pInput = Parameters("DEM")->asGrid();
	m_pImg2 = Parameters("RESULT")->asGrid();

	m_pImg2->Create( Get_System(), m_pInput->Get_Type() );
	m_pImg2->Set_Name( CSG_String::Format("%s_Fill", m_pInput->Get_Name()) );

	double NoDataValue = m_pInput->Get_NoData_Value();
	m_pImg2->Set_NoData_Value( NoDataValue );
	m_pImg2->Set_Scaling( m_pInput->Get_Scaling(), m_pInput->Get_Offset() );

	int hMin = INT_MAX;
	int hMax = INT_MIN;

	Process_Set_Text(_TL("Creating statistics"));
	for(sLong y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		int local_hMin = INT_MAX;
		int local_hMax = INT_MIN;
	
		#pragma omp parallel for reduction(min:local_hMin) reduction(max:local_hMax)
		for(sLong x=0; x<Get_NX(); x++)
		{
			if( !m_pInput->is_NoData(x,y) )
			{
				int value = m_pInput->asInt(x, y, false);
				local_hMin = std::min( local_hMin, value );
				local_hMax = std::max( local_hMax, value );
			}
		}

		hMin = std::min(hMin, local_hMin);
		hMax = std::max(hMax, local_hMax);
	}
	

	std::priority_queue<Cell, std::vector<Cell>, std::greater<Cell>> pq;
	
	

	int BoundaryVal = (int) ((Parameters("BOUNDARY")->asDouble() - m_pInput->Get_Offset()) / m_pInput->Get_Scaling() );
	
	Process_Set_Text(_TL("Initializing output"));
	for(sLong y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		#pragma omp parallel for
		for(sLong x=0; x<Get_NX(); x++)
		{
			m_pInput->is_NoData(x,y) ? m_pImg2->Set_NoData(x,y) : m_pImg2->Set_Value( x, y, hMax, false);
		}
	}

	
	int Num_Threads = SG_OMP_Get_Max_Num_Threads();

    int Chunk_Size 	= Get_NY() / Num_Threads;


	//PixelQueue *pixQ = Create_Queue( hMin, hMax, BoundaryVal, 0, Get_NX(), 924, 925 );

    #pragma omp parallel
    {
        int Thread_ID = SG_OMP_Get_Thread_Num();

		int yStart	= Thread_ID * Chunk_Size;
		int yEnd 	= Thread_ID == Num_Threads-1 ? Get_NY() : (Thread_ID + 1) * Chunk_Size;

		//int hMin = INT_MAX;
		//int hMax = INT_MIN;

		//for(sLong y=yStart; y<yEnd; y++)
		//{
		//	//#pragma omp parallel for reduction(min:local_hMin) reduction(max:local_hMax)
		//	for(sLong x=0; x<Get_NX(); x++)
		//	{
		//		if( !m_pInput->is_NoData(x,y) )
		//		{
		//			int value = m_pInput->asInt(x, y, false);
		//			hMin = std::min( hMin, value );
		//			hMax = std::max( hMax, value );
		//		}
		//	}
		//}

		PixelQueue* pixQ = Create_Queue( hMin, hMax, BoundaryVal, 0, Get_NX(), yStart, yEnd );
		bool b = Fill_Queue( pixQ, hMin, hMax, 0, Get_NX(), yStart, yEnd);
    }

	//int yStart	= 1 * Chunk_Size;
	//int yEnd 	= 1 == Num_Threads-2 ? Get_NY() : (1 + 1) * Chunk_Size;
	
	//int yStart	= 0;
	//int yEnd 	= Get_NY();
	/*
		int yStart	= 1 * Chunk_Size;
		int yEnd 	= 1 == Num_Threads-2 ? Get_NY() : (1 + 1) * Chunk_Size;

	int hMin = INT_MAX;
	int hMax = INT_MIN;

	for(sLong y=yStart; y<yEnd; y++)
	{
		//#pragma omp parallel for reduction(min:local_hMin) reduction(max:local_hMax)
		for(sLong x=0; x<Get_NX(); x++)
		{
			if( !m_pInput->is_NoData(x,y) )
			{
				int value = m_pInput->asInt(x, y, false);
				hMin = std::min( hMin, value );
				hMax = std::max( hMax, value );
			}
		}
	}
	for(sLong y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		#pragma omp parallel for
		for(sLong x=0; x<Get_NX(); x++)
		{
			m_pInput->is_NoData(x,y) ? m_pImg2->Set_NoData(x,y) : m_pImg2->Set_Value( x, y, hMax, false);
		}
	}
	*/
	/*
	CSG_Grid Tick_Off( Get_System(), SG_DATATYPE_Bit);
	Tick_Off.Set_NoData(0);
	Tick_Off.Assign_NoData();
	
	for( sLong y=yStart; y<yEnd; y++ )
	{
		for( sLong x=0; x<Get_NX(); x++ )
		{
			if( m_pInput->is_NoData(x,y) )
			{
				for( int i=0; i<8; i++ )
				{
					int xdiff = x + xnb[i];
					int ydiff = y + ynb[i];
					
					if( m_pInput->is_InGrid( xdiff, ydiff, true ) && Tick_Off.is_NoData(xdiff,ydiff) )
					{

                    	int imgval = m_pInput->asInt(xdiff, ydiff, false);
						pq.push ( {x, y, imgval} );
						
						
						Tick_Off.Set_Value(xdiff,ydiff,1.);
					}
				}
			}
	 	}
	}
	
	Tick_Off.Assign_NoData();


    while (!pq.empty()) {
        Cell cell = pq.top();
        pq.pop();

		std::vector<std::pair<sLong,sLong>> n({{0,1}, {1,0}, {0,-1}, {-1,0}});
		
		if( !Tick_Off.is_NoData(cell.x,cell.y) ) continue;
		
		Tick_Off.Set_Value(cell.x,cell.y, 1.0, true);

        for (auto d : n) 
		{
            sLong nx = cell.x + d.first;
			sLong ny = cell.y + d.second;
            if (nx >= 0 && ny >= 0 && nx < Get_NX() && ny < Get_NY() && Tick_Off.is_NoData(nx,ny) ) {
				int height = m_pImg2->asInt(nx,ny, false);
				m_pImg2->Set_Value( nx, ny, std::max( height, cell.height ) );

                pq.push({nx, ny, height});
            }
        }
    }




	return true;
	*/


	//PixelQueue* pixQ = Create_Queue( hMin, hMax, BoundaryVal, 0, Get_NX(), yStart, yEnd );
	//bool b = Fill_Queue( pixQ, hMin, hMax, 0, Get_NX(), yStart, yEnd);

	//for(sLong y=924; y<926 && Set_Progress_Rows(y); y++)
	//{
	//	#pragma omp parallel for
	//	for(sLong x=0; x<Get_NX(); x++)
	//	{
	//		m_pInput->is_NoData(x,y) ? m_pImg2->Set_NoData(x,y) : m_pImg2->Set_Value( x, y, hMax, false);
	//	}
	//}

	//bool b = Fill_Queue( pixQ, hMin, hMax, 0, Get_NX(), 0, 1850);
    
	//int Chunk_Size 	= Get_NY() / 8;
	//	
	//int yStart	= 0 * Chunk_Size;
    //int yEnd 	= Thread_ID == 0 ? Get_NY() : (Thread_ID + 1) * Chunk_Size - 1;
	//
	//PixelQueue* pixQ = Create_Queue( hMin, hMax, BoundaryVal, 0, Get_NX(), yStart, yEnd );
	//bool b = Fill_Queue( pixQ, hMin, hMax, 0, Get_NX(), yStart, yEnd);
	
	//PixelQueue* pixQ = Create_Queue( hMin, hMax, BoundaryVal, 0, Get_NX(), 0, Get_NY() );

	//bool b = Fill_Queue( pixQ, hMin, hMax);


	return( true );
}


bool CFillMinima::Fill_Queue( PixelQueue *pixQ, int hMin, int hMax, sLong xStart, sLong xEnd, sLong yStart, sLong yEnd )
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
            nbrs = neighbours(p, yStart, yEnd, xStart, xEnd );
            //nbrs = neighbours(p, Get_NY(), Get_NX());
            pNbr = nbrs;
            while (pNbr != NULL) 
			{
                int r = pNbr->i;
                int c = pNbr->j;
                // Exclude null area of original image 
                //if (! *((npy_bool*)PyArray_GETPTR2(pNullMask, r, c)))
                if ( !m_pInput->is_NoData(c, r) )
				{
                    int imgval = m_pInput->asInt(c,r, false);
                    int img2val = m_pImg2->asInt(c,r, false);
                    if (img2val == hMax) 
					{
                        img2val = std::max(hCrt, imgval);
                        // *((npy_int16*)PyArray_GETPTR2(pimg2, r, c)) = img2val;
						m_pImg2->Set_Value(c,r, img2val, false);
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

PixelQueue* CFillMinima::Create_Queue( int hMin, int hMax, int BoundaryVal, sLong xStart, sLong xEnd, sLong yStart, sLong yEnd )
{
	#ifdef _DEBUG 
	CSG_Simple_Statistics Top_Boundary, Down_Boundary, Left_Boundary, Right_Boundary, Boundary;
	#endif

	PixelQueue *pixQ = PQ_init(hMin,hMax);
	
	CSG_Grid Tick_Off( Get_System(), SG_DATATYPE_Bit);
	Tick_Off.Set_NoData(0);
	Tick_Off.Assign_NoData();
	
	for( sLong y=yStart; y<yEnd; y++ )
	{
		for( sLong x=xStart; x<xEnd; x++ )
		{
			if( m_pInput->is_NoData(x,y) )
			{
				for( int i=0; i<8; i++ )
				{
					int xdiff = x + xnb[i];
					int ydiff = y + ynb[i];
					
					if( m_pInput->is_InGrid( xdiff, ydiff, true ) && Tick_Off.is_NoData(xdiff,ydiff) )
					{
						PQel *p = newPix(ydiff, xdiff);

                    	int imgval = m_pInput->asInt(xdiff, ydiff, false);
                    	m_pInput->Set_Value(xdiff, ydiff, BoundaryVal,false);
						PQ_add(pixQ, p, BoundaryVal);
						
						//PQ_add(pixQ, p, BoundaryVal);
						
						Tick_Off.Set_Value(xdiff,ydiff,1.);

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
                    int imgval = m_pInput->asInt(x, y, false);
                    m_pInput->Set_Value(x, y, std::max( BoundaryVal, imgval),false);
					//PQ_add(pixQ, p, imgval);
					PQ_add(pixQ, p, BoundaryVal);
					Tick_Off.Set_Value( x, y, 1., true );

					#ifdef _DEBUG 
					if( 	 y == yEnd-1 && yEnd != Get_NY()) 	Top_Boundary 	+= imgval;
					else if( y == yStart && yStart != 0 )		Down_Boundary 	+= imgval;
					else if( x == xEnd-1 && xEnd != Get_NX()) 	Right_Boundary 	+= imgval;
					else if( x == xStart && xStart != 0 ) 		Left_Boundary 	+= imgval;
					else 										Boundary 		+= imgval;
					#endif
				}
			}
		}
	}
	#ifdef _DEBUG 
	#pragma omp critical
	{
		Message_Fmt("Thread No: %d\n", SG_OMP_Get_Thread_Num() );
		if( Boundary.Get_Count() ) 		Message_Fmt("Boundary Cells: %d, Min: %f.2, Max: %f.2\n", Boundary.Get_Count(), Boundary.Get_Minimum(), Boundary.Get_Maximum());
		if( Top_Boundary.Get_Count() ) 	Message_Fmt("Top Tile Boundary Cells: %d, Min: %f.2, Max: %f.2\n", Top_Boundary.Get_Count(), Top_Boundary.Get_Minimum(), Top_Boundary.Get_Maximum());
		if( Down_Boundary.Get_Count() ) Message_Fmt("Down Tile Boundary Cells: %d, Min: %f.2, Max: %f.2\n", Down_Boundary.Get_Count(), Down_Boundary.Get_Minimum(), Down_Boundary.Get_Maximum());
		if( Left_Boundary.Get_Count() ) Message_Fmt("Left Tile Boundary Cells: %d, Min: %f.2, Max: %f.2\n", Left_Boundary.Get_Count(), Left_Boundary.Get_Minimum(), Left_Boundary.Get_Maximum());
		if( Right_Boundary.Get_Count() )Message_Fmt("Right Tile Boundary Cells: %d, Min: %f.2, Max: %f.2\n", Right_Boundary.Get_Count(), Right_Boundary.Get_Minimum(), Right_Boundary.Get_Maximum());
	}
	#endif

	return( pixQ );
}

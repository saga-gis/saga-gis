
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
	
	Parameters.Add_Int(
		"", "OVERLAP", _TL("Overlap"), _TL(""), 1000
	);

	Parameters.Add_Int(
		"", "XS", _TL("X Start"), _TL(""), 0
	);

	Parameters.Add_Int(
		"", "XE", _TL("X End"), _TL(""), 0
	);

	Parameters.Add_Int(
		"", "YS", _TL("Y Start"), _TL(""), 0
	);

	Parameters.Add_Int(
		"", "YE", _TL("Y End"), _TL(""), 0
	);
}


//#define max(a,b) ((a) > (b) ? (a) : (b))



static int	xnb[] = { 0, 1, 1, 1, 0,-1,-1,-1};
static int	ynb[] = { 1, 1, 0,-1,-1,-1, 0, 1};

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
	for(sLong y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		int local_hMin = INT_MAX;
		int local_hMax = INT_MIN;
	
		#pragma omp parallel for reduction(min:local_hMin) reduction(max:local_hMax)
		for(sLong x=0; x<Get_NX(); x++)
		{
			if( !pInput->is_NoData(x,y) )
			{
				int value = pInput->asInt(x, y, false);
				local_hMin = std::min( local_hMin, value );
				local_hMax = std::max( local_hMax, value );
			}
		}
		hMin = std::min(hMin, local_hMin);
		hMax = std::max(hMax, local_hMax);
	}
	
	
	

	
	Process_Set_Text(_TL("Initializing output"));
	for(sLong y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		#pragma omp parallel for
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
    int Chunk_Size = Get_NY() / Num_Threads;
	while( Chunk_Size % 4 != 0 ){ Chunk_Size++; }
	int Half = Chunk_Size / 2;
	int Quarter = Half / 2;

	//int Overlap = Parameters("OVERLAP")->asInt();
	int Overlap = Half;


	int yS = Parameters("YS")->asInt();
	int yE = Parameters("YE")->asInt();
	int xS = Parameters("XS")->asInt();
	int xE = Parameters("XE")->asInt();

	if( Num_Threads <= 2 )
	{
		PixelQueue* pixQ = 	Create_Queue( pInput, pOutput, hMin, hMax, BoundaryVal, xS, xE, yS, yE );
		//PixelQueue* pixQ = 	Create_Queue( pInput, pOutput, hMin, hMax, BoundaryVal, 0, Get_NX(), 0, Get_NY() );
							Fill_Queue(   pInput, pOutput, pixQ, hMin, hMax,  xS, xE, yS, yE );
							//Fill_Queue(   pInput, pOutput, pixQ, hMin, hMax, 0, Get_NX(), 0, Get_NY() );
	}
	else 
	{
		CSG_Array _Thread_Data(sizeof(CSG_Grid*), Num_Threads ); CSG_Grid **Thread_Data = (CSG_Grid**) _Thread_Data.Get_Array();

		#pragma omp parallel
		{
			int Thread_ID = SG_OMP_Get_Thread_Num();

			//Thread_Data[Thread_ID] = new CSG_Grid( Get_System(), pOutput->Get_Type() );

			//CSG_Grid *pLocalCopy = Thread_Data[Thread_ID];

			int yStart	=  Thread_ID * Chunk_Size - Overlap;
			int yEnd 	= (Thread_ID * Chunk_Size) + Chunk_Size + Overlap;
			if( yStart < 0 ) 		yStart = 0;
			if( yEnd > Get_NY() ) 	yEnd = Get_NY();
			
			//for(sLong y=yStart; y<yEnd; y++)
			//{
			//	for(sLong x=0; x<Get_NX(); x++)
			//	{
			//		if( !pOutput->is_NoData(x,y) )
			//		{
			//			pLocalCopy->Set_Value(x,y, pOutput->asInt(x, y, false), false );
			//		}
			//	}
			//}
			
			PixelQueue* pixQ = 	Create_Queue( pInput, Thread_ID % 2 ? &Copy : pOutput, hMin, hMax, BoundaryVal, 0, Get_NX(), yStart, yEnd );
								Fill_Queue(   pInput, Thread_ID % 2 ? &Copy : pOutput, pixQ, hMin, hMax, 0, Get_NX(), yStart, yEnd);
			
			//yStart	= yEnd;
			//yEnd 	= (yStart + Half) < Get_NY() ? yStart + Half : Get_NY();
			//
			//for(sLong y=yStart; y<yEnd; y++)
			//{
			//	for(sLong x=0; x<Get_NX(); x++)
			//	{
			//		if( !pInput->is_NoData(x,y) )
			//		{
			//			pOutput->Set_Value(x,y, pInput->asInt(x, y, false), false );
			//		}
			//	}
			//}

		}
		#pragma omp parallel for
		for(sLong y=0; y<Get_NY(); y++)
		{
			for(sLong x=0; x<Get_NX(); x++)
			{
				if( !pOutput->is_NoData(x,y) )
				{
					pOutput->Set_Value(x,y, std::max(Copy.asInt(x, y, false), pOutput->asInt(x, y, false)), false );
				}
			}
			//int ID = SG_OMP_Get_Thread_Num();

			//if( ID < Num_Threads-1 )
			//{
			//	int yStart	= (ID + 1) * Chunk_Size - Overlap;
			//	int yEnd 	= yStart + 2 * Overlap;
			//	if( yStart < 0 		|| ID == 0 ) 		  	yStart = 0;
			//	if( yEnd > Get_NY() || ID == Num_Threads-2) yEnd = Get_NY();
			//	
			//}
			
			//int Thread_ID = SG_OMP_Get_Thread_Num();

			//int yStart	= Thread_ID * Chunk_Size - Quarter;
			//yStart = yStart < 0 || Thread_ID == 0 ? 0 : yStart;

			//int yEnd 	= (yStart + Chunk_Size < Get_NY()) ? yStart + Chunk_Size : Get_NY();
			//
			//PixelQueue* pixQ = 	Create_Queue( &Copy, pOutput, hMin, hMax, BoundaryVal, 0, Get_NX(), yStart, yEnd );
			//					Fill_Queue(   &Copy, pOutput, pixQ, hMin, hMax, 0, Get_NX(), yStart, yEnd);
			//
			//if( yStart < 0 )
			//	yStart = 0;

			//if( Thread_ID == 0 )
			//	yStart = 0;
			//
			//if( yEnd >= Get_NY() )
			//	yEnd = Get_NY();

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
			

			//PixelQueue* pixQ = Create_Queue( hMin, hMax, BoundaryVal, 0, Get_NX(), yStart, yEnd );
			//bool b = Fill_Queue( pixQ, hMin, hMax, 0, Get_NX(), yStart, yEnd);
			
			//if( Thread_ID % 2 == 0 )
			//{
			//	PixelQueue* pixQ = Create_Queue( hMin, hMax, BoundaryVal, 0, Get_NX(), yStart, yEnd );
			//	bool b = Fill_Queue( pixQ, hMin, hMax, 0, Get_NX(), yStart, yEnd);
			//}
			//else
			//{
			//	for(sLong y=yStart; y<yEnd; y++)
			//	{
			//		//#pragma omp parallel for reduction(min:local_hMin) reduction(max:local_hMax)
			//		for(sLong x=0; x<Get_NX(); x++)
			//		{
			//			if( !m_pInput->is_NoData(x,y) )
			//			{
			//				m_pImg2->Set_Value(x,y, m_pInput->asInt(x, y, false), false );
			//			}
			//		}
			//	}

			//}
			

			//PriorityQueuePtr pixQ = Create_Queue( NULL, hMin, hMax, BoundaryVal, 0, Get_NX(), yStart, yEnd );
			//bool b = Fill_Queue( pixQ, hMin, hMax, 0, Get_NX(), yStart, yEnd);
		}
		//#pragma omp parallel
		//{
		//	int ID = SG_OMP_Get_Thread_Num();
		//	delete Thread_Data[ID];
		//}
	}
	
	//int yStart	= 0;
	//int yEnd 	= 1;
	//	
	//PixelQueue* pixQ = Create_Queue( hMin, hMax, BoundaryVal, 0, Get_NX(), yStart, yEnd );

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

bool CFillMinima::Fill_Queue( CSG_Grid *pInput, CSG_Grid *pOutput, PixelQueue *pixQ, int hMin, int hMax, sLong xStart, sLong xEnd, sLong yStart, sLong yEnd )
{
	#ifdef _DEBUG 
	CSG_Simple_Statistics Stats;
	#endif

    PQel *p, *nbrs, *pNbr, *pNext;
    int hCrt = (int)hMin;
	
	CSG_Grid Tick_Off( Get_System(), SG_DATATYPE_Bit);
	Tick_Off.Set_NoData_Value(0);
	Tick_Off.Assign_NoData();

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
						if( i >= yStart && i < yEnd && j >= xStart && j < xEnd && Tick_Off.is_NoData(j,i) )
						{
							PQel *pNew = newPix(i, j);
							pNew->next = pl;
							pl = pNew;
							Tick_Off.Set_NoData(j,i);
						}
					}
				}
			}

            //nbrs = neighbours(p, Get_NY(), Get_NX());
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

/*
bool CFillMinima::Fill_Queue( PriorityQueuePtr PQ, int hMin, int hMax, sLong xStart, sLong xEnd, sLong yStart, sLong yEnd )
{
	#ifdef _DEBUG 
	CSG_Simple_Statistics Stats;
	#endif

	//CSG_Grid Tick_Off( Get_System(), SG_DATATYPE_Bit);
	//Tick_Off.Set_NoData(0);
	//Tick_Off.Assign_NoData();
    
	while (!PQ->empty() ) 
	{
		Cell P = PQ->top();
		PQ->pop();
		PriorityQueue Nbrs;
		
		for( int i=0; i<8; i++ )
		{
			int xdiff = P.x + xnb[i];
			int ydiff = P.y + ynb[i];
			
			if( m_pInput->is_InGrid( xdiff, ydiff, true ) ) //&& Tick_Off.is_NoData(xdiff,ydiff) )
			{
				int imgval = m_pInput->asInt(xdiff, ydiff, false);
				Cell Nb = { xdiff, ydiff, imgval };
				Nbrs.push(Nb);
				//Tick_Off.Set_Value(xdiff,ydiff,1.);
			}
		}

		while( !Nbrs.empty() )
		{
			Cell N = Nbrs.top();
			Nbrs.pop();
			int x = N.x;
			int y = N.y;

			if ( !m_pInput->is_NoData(x, y) )
			{
				int img2val = m_pImg2->asInt(x, y, false);
				if (img2val == hMax) 
				{
					img2val = std::max(P.height, N.height);
					N.height = img2val;
					m_pImg2->Set_Value(x,y, img2val, false);
					PQ->push(N);
				}
			}
		}
	}

	#ifdef _DEBUG 
	#pragma omp critical
	{
		Message_Fmt("Thread No: %d\n", SG_OMP_Get_Thread_Num() );
		Message_Fmt("Operations: %d, Min: %.2f, Max: %.2f, Mean: %.2f\n", Stats.Get_Count(), Stats.Get_Minimum(), Stats.Get_Maximum(), Stats.Get_Mean());
	}
	#endif
	return true;
}
*/

PixelQueue* CFillMinima::Create_Queue( CSG_Grid *pInput, CSG_Grid *pOutput, int hMin, int hMax, int BoundaryVal, sLong xStart, sLong xEnd, sLong yStart, sLong yEnd )
{
	#ifdef _DEBUG 
	CSG_Simple_Statistics Top_Boundary, Down_Boundary, Left_Boundary, Right_Boundary, Boundary, Values;
	#endif

	PixelQueue *pixQ = PQ_init(hMin,hMax);
	
	//CSG_Grid Tick_Off( Get_System(), SG_DATATYPE_Bit);
	//Tick_Off.Set_NoData_Value(0);
	//Tick_Off.Assign_NoData();
	
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
					
					if( pInput->is_InGrid( xdiff, ydiff, true ) ) //&& Tick_Off.is_NoData(xdiff,ydiff) )
					{
						PQel *p = newPix(ydiff, xdiff);

                    	int imgval = pInput->asInt(xdiff, ydiff, false);
                    	pInput->Set_Value(xdiff, ydiff, BoundaryVal,false);
						PQ_add(pixQ, p, BoundaryVal);
						
						//PQ_add(pixQ, p, BoundaryVal);
						
						//Tick_Off.Set_NoData(xdiff,ydiff);

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
                    //m_pInput->Set_Value(x, y, std::max( BoundaryVal, imgval),false);
					PQ_add(pixQ, p, imgval);
					//PQ_add(pixQ, p, BoundaryVal);
					//Tick_Off.Set_Value( x, y, 1., true );

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

/*
PriorityQueuePtr CFillMinima::Create_Queue( CSG_Grid *pGrid, int hMin, int hMax, int BoundaryVal, sLong xStart, sLong xEnd, sLong yStart, sLong yEnd )
{
	#ifdef _DEBUG 
	CSG_Simple_Statistics Top_Boundary, Down_Boundary, Left_Boundary, Right_Boundary, Boundary;
	#endif

	PriorityQueuePtr PG = std::make_shared<PriorityQueue>();
	
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
						Cell P = { xdiff, ydiff, BoundaryVal };

                    	int imgval = m_pInput->asInt(xdiff, ydiff, false);
                    	m_pInput->Set_Value(xdiff, ydiff, BoundaryVal,false);
						
						PG->push(P);
						
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
					Cell P = { x, y, BoundaryVal };
					PG->push(P);


                    int imgval = m_pInput->asInt(x, y, false);
                    m_pInput->Set_Value(x, y, std::max( BoundaryVal, imgval),false);
					//PQ_add(pixQ, p, imgval);
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

	return( PG );
}
*/


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

//---------------------------------------------------------
#include "fill_minima.h"
#include <math.h>

//#define max(a,b) ((a) > (b) ? (a) : (b))

static int	xnb[] = { -1, 0, 1, 1, 1, 0,-1,-1};
static int	ynb[] = {  1, 1, 1, 0,-1,-1,-1, 0};

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFillMinima::CFillMinima(void)
{
	Set_Name 		(_TL("Fill Minima"));

	Set_Author 		("Justus Spitzm\u00fcller \u00a9 2025, Neil Flood \u00a9 2015, Richard Barnes \u00a9 2014");

	Set_Version 	("2.0");

	Set_Description(_TW(
		"This tool performs basic sink filling on Digital Elevation Models (DEMs). It identifies and "
		"fills local depressions (sinks) to ensure continuous flow paths across the terrain surface. "
		"Note: this tool does not carry out a full hydrological correction \u2013 it only addresses "
		"depressions and does not modify flat areas, resolve flow directions, or enforce hydrological "
		"consistency beyond sink removal.\n\n"
		"The tool accepts a DEM as input and returns a version with filled depressions. Optionally, "
		"users can define a fixed boundary value to control how edge/boundary cells are initialized "
		"during filling.\n\n"
		"Recommended Usage:\n"
		"<ul>"
		"<li> Choose Soille & Gratin for (scaled) integer DEMs.</li>"
		"<li> Choose Barnes et al. for floating-point DEMs.</li>"
		"</ul>\n"
	));


    Add_Reference("Soille, P., & C. Gratin", "1994", 
		"An efficient algorithm for drainage network extraction on DEMs. ",
		"Journal of Visual Communication and Image Representation. 5(2): 181-189."
	);

    Add_Reference("Barnes, R., Lehman, C., & Mulla, D.", "2014",
		"Priority-flood: An optimal depression-filling and watershed-labeling "
		"algorithm for digital elevation models.", 
		"Computers & Geosciences, 62, 117-127."
	);

	Add_Reference("https://www.pythonfmask.org/",
		SG_T("Python Fmask")
	);

	Add_Reference("https://github.com/r-barnes/richdem",
		SG_T("RichDEM")
	);
	
	//-----------------------------------------------------
	Parameters.Add_Grid(
		"", "DEM"		, _TL("DEM"),
		_TL("Digital Elevation Model"),
		PARAMETER_INPUT, true
	);

	Parameters.Add_Grid(
		"", "RESULT"	, _TL("Filled DEM"),
		_TL("Processed DEM"),
		PARAMETER_OUTPUT, true
	);

	Parameters.Add_Choice(
		"", "METHOD", _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|",
			SG_T("Auto detection by input"),
			SG_T("Soille & Gratin 1994"),
			SG_T("Barnes et al. 2014")
		), 0 
	);
	
	Parameters.Add_Bool(
		"", "BOUNDARY", _TL("Set Fixed Boundary Value"), _TL(""), false 
	);

	Parameters.Add_Double(
		"BOUNDARY", "BOUNDARY_VALUE", _TL("Boundary Value"), _TL("The boundary value will be scaled by the scaling factor of the input. "), 1.0
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CFillMinima::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	pParameters->Set_Enabled("BOUNDARY_VALUE", pParameters->Get_Parameter("BOUNDARY")->asBool());
	
	if( pParameter->Cmp_Identifier("DEM") || pParameter->Cmp_Identifier("METHOD") )
	{
		CSG_Grid	*pDEM	= pParameters->Get_Parameter("DEM")->asGrid();
		int 		Method 	= pParameters->Get_Parameter("METHOD")->asInt();

		if( pDEM && Method == 0 )
		{
			TSG_Data_Type Type = pDEM->Get_Type();
			if( Type == SG_DATATYPE_Float || Type == SG_DATATYPE_Double )
			{
				pParameters->Set_Parameter("METHOD", 2);
			}
			else if( SG_Data_Type_is_Numeric(Type) )
			{
				pParameters->Set_Parameter("METHOD", 1);
			}
			else 
			{		
				pParameters->Set_Parameter("METHOD", 0);
			}
		}
	}

	return( CSG_Tool_Grid::On_Parameter_Changed(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFillMinima::On_Execute(void)
{
	CSG_Grid *pInput = Parameters("DEM")->asGrid();
	double NoDataValue = pInput->Get_NoData_Value();
	
	CSG_Grid* pOutput = Parameters("RESULT")->asGrid();
	pOutput->Create( Get_System(), pInput->Get_Type() );
	pOutput->Set_Name( CSG_String::Format("%s_Fill", pInput->Get_Name()) );
	pOutput->Set_NoData_Value( NoDataValue );
	pOutput->Set_Scaling( pInput->Get_Scaling(), pInput->Get_Offset() );
	
	CSG_Grid Closed(Get_System(), SG_DATATYPE_Bit);
	Closed.Assign_NoData();
	//CSG_Array _Closed(sizeof(bool), pInput->Get_NCells()); bool* Closed = (bool*)_Closed.Get_Array();
	//for( size_t i=0; i<Get_NCells(); i++ )
	//	Closed[i] = false;
	//
	//	Closed[y * nx + x]

	int 	Method = Parameters("METHOD")->asInt();
	bool 	Fix_Boundary = Parameters("BOUNDARY")->asBool();

	double 	Boundary_Value = (Parameters("BOUNDARY_VALUE")->asDouble() - pInput->Get_Offset()) / (pInput->is_Scaled() ? pInput->Get_Scaling() : 1. ); // Don't divide by zero

	int hMin = INT_MAX;
	int hMax = INT_MIN;
	
	if( Method == 0 )
	{
		On_Parameter_Changed( &Parameters, Parameters("METHOD") );
		Method = Parameters("METHOD")->asInt();
	}
	
	if( Method == 1 )
	{
		Process_Set_Text(_TL("Creating statistics"));
#if !defined(_SAGA_MSW)
		#pragma omp parallel for reduction(min:hMin) reduction(max:hMax)
#endif
		for(sLong y=0; y<Get_NY(); y++)
		{
			for(int x=0; x<Get_NX(); x++)
			{
				if( !pInput->is_NoData(x,y) )
				{
					int value = pInput->asInt(x, y, false);
					hMin = std::min( hMin, value );
					hMax = std::max( hMax, value );
				}
				else
				{
					pOutput->Set_NoData(x,y);
				}
			}
		}

		PixelQueue *pixQ = PQ_init(hMin,hMax);
		Process_Set_Text(_TL("Filling Minima"));
		Create_Queue( pInput, pOutput, &Closed, pixQ, NULL, Method, Fix_Boundary, Boundary_Value );
		Fill_Sinks_Soille( pInput, pOutput, &Closed, pixQ, Fix_Boundary ? (int) Boundary_Value : hMin, hMax );
	}
	

	if( Method == 2 )
	{
		Process_Set_Text(_TL("Filling Minima"));
		pOutput->Assign(pInput);
		grid_cellz_pq open;
		Create_Queue( pInput, pOutput, &Closed, NULL, &open, Method, Fix_Boundary, Boundary_Value );
		Fill_Sinks_Barnes( pInput, pOutput, &Closed, &open );
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFillMinima::Create_Queue( CSG_Grid *pInput, CSG_Grid *pOutput, CSG_Grid *pClosed, PixelQueue *pixQ, grid_cellz_pq *pOpen, int Method, bool Boundary, double Boundary_Value )
{
	if((Method == 1 && pixQ == NULL) 
	|| (Method == 2 && pOpen == NULL))
	{
		return( false );
	}

	bool Scale = (Method == 1) ? false : true;
	
	if( pInput->Get_NoData_Count() > 0 )
	{
		for( int y=0; y<Get_NY(); y++ )
		{
			for( int x=0; x<Get_NX(); x++ )
			{
				if( pInput->is_NoData(x,y) )
				{
					pClosed->Set_Value( x, y, true, true );
					
					for( int i=0; i<8; i++ )
					{
						int xdiff = x + xnb[i];
						int ydiff = y + ynb[i];

						if( pInput->is_InGrid( xdiff, ydiff, true ) && pClosed->is_NoData( xdiff, ydiff) )
						{
							pClosed->Set_Value( xdiff, ydiff, true, true );
							pOutput->Set_Value( xdiff, ydiff,  Boundary ? Boundary_Value : pInput->asDouble(xdiff,ydiff,Scale), Scale );
					
							if( x != 0 && x != Get_NX()-1 && y != 0 && y != Get_NY()-1 )
							{
								if( Method == 1 )
								{
									PQ_add(pixQ, newPix(ydiff, xdiff), Boundary ? Boundary_Value : pInput->asInt(xdiff,ydiff,false) );
								}
								if( Method == 2 )
								{	
									pOpen->push_cell(	xdiff, ydiff,  Boundary ? Boundary_Value : pInput->asDouble(xdiff,ydiff) );
								}
							}
						}
					}
				}
				else
				{
					if( x == 0 || x == Get_NX()-1 || y == 0 || y == Get_NY()-1 )
					{
						pClosed->Set_Value( x, y, true, true );
						pOutput->Set_Value( x, y,  Boundary ? Boundary_Value : pInput->asDouble(x,y,Scale), Scale );
					}

					if( x == 1 || x == Get_NX()-2 || y == 1 || y == Get_NY()-2 )
					{
						pClosed->Set_Value( x, y, true, true );
						pOutput->Set_Value( x, y,  Boundary ? Boundary_Value : pInput->asDouble(x,y,Scale), Scale );
						
						if( Method == 1 )
						{
							PQ_add(pixQ, newPix(y, x), Boundary ? Boundary_Value : pInput->asInt(x,y,false) );
						}
						if( Method == 2 )
						{	
							pOpen->push_cell( 	x, y,  Boundary ? Boundary_Value : pInput->asDouble(x,y) );
						}
					}
				}
			}
		}
	}
	else 
	{
		int cx[4] = { 0, Get_NX()-2, Get_NX()-2, 0 			};
		int cy[4] = { 0, 0, 		 Get_NY()-2, Get_NY()-2 };

		int dx[4] = { 1, 0, 0, 1 };
		int dy[4] = { 1, 1, 0, 0 };

		for( int c = 0; c < 4; c++ )
		{
			int sx = cx[c];
			int sy = cy[c];

			double Grid_Max = pInput->asDouble(sx + dx[0], sy + dy[0], Scale);
			for( int i = 1; i < 4; i++ )
			{
				Grid_Max = std::max(Grid_Max, pInput->asDouble(sx + dx[i], sy + dy[i], Scale));
			}

			if( Method == 1 )
			{
				PQ_add(pixQ, newPix(sy+dy[c],sx+dx[c]),   (int) (Boundary ? std::max(Boundary_Value, pInput->asDouble(sx+dx[c], sy+dy[c], Scale)) : Grid_Max) );
			}
			else if( Method == 2 )
			{
				pOpen->push_cell(sx+dx[c], sy+dy[c], 			(Boundary ? std::max(Boundary_Value, pInput->asDouble(sx+dx[c], sy+dy[c], Scale)) : Grid_Max) );
			}

			for( int i = 0; i < 4; i++ )
			{
				int x = sx + dx[i];
				int y = sy + dy[i];
				
				pClosed->Set_Value(x, y, true, true);
				pOutput->Set_Value(x, y, Boundary ? Boundary_Value : (i == c ? Grid_Max : pInput->asDouble(x,y,Scale)), Scale);
			}
		}

		for( int x=2; x<Get_NX()-2; x++ )
		{
			pClosed->Set_Value(	x, 0, 		   true, true );
			pClosed->Set_Value(	x, 1, 		   true, true );
			pClosed->Set_Value(	x, Get_NY()-1, true, true );
			pClosed->Set_Value(	x, Get_NY()-2, true, true );
			
			double Boundary_Top  = std::max(Boundary_Value							, pInput->asDouble( x, 1,			Scale)		 );
			double Boundary_Down = std::max(Boundary_Value							, pInput->asDouble( x, Get_NY()-2, 	Scale)		 ); 
			double Grid_Top	  	 = std::max(pInput->asDouble(x,0,Scale)				, pInput->asDouble( x, 1, 			Scale)		 );
			double Grid_Down 	 = std::max(pInput->asDouble(x,Get_NY()-1,Scale)	, pInput->asDouble( x, Get_NY()-2, 	Scale)		 ); 

			pOutput->Set_Value( x, 0, 				Boundary ? Boundary_Value 		: pInput->asDouble( x, 0,		   	Scale), Scale);
			pOutput->Set_Value( x, 1,				Boundary ? Boundary_Top   		: Grid_Top, 								Scale);
			pOutput->Set_Value( x, Get_NY()-1, 		Boundary ? Boundary_Value 		: pInput->asDouble( x, Get_NY()-1, 	Scale), Scale);
			pOutput->Set_Value( x, Get_NY()-2, 		Boundary ? Boundary_Down  		: Grid_Down,								Scale);
			
			if( Method == 1 )
			{
				PQ_add( pixQ, newPix(1, 		 x),Boundary ? Boundary_Top   		: Grid_Top 	);
				PQ_add( pixQ, newPix(Get_NY()-2, x),Boundary ? Boundary_Down  		: Grid_Down );
			}
			if( Method == 2 )
	  		{
				pOpen->push_cell(	x, 1, 		    Boundary ? Boundary_Top 		: Grid_Top 	);
				pOpen->push_cell(	x, Get_NY()-2,  Boundary ? Boundary_Down 		: Grid_Down );
			}
		}
		for( int y=2; y<Get_NY()-2; y++ )
		{
			pClosed->Set_Value( 0,          y, true, true );
			pClosed->Set_Value( 1,          y, true, true );
			pClosed->Set_Value( Get_NX()-1, y, true, true );
			pClosed->Set_Value( Get_NX()-2, y, true, true );

			double Boundary_Left   = std::max(Boundary_Value						, pInput->asDouble(1,          y, Scale)	   );
			double Boundary_Right  = std::max(Boundary_Value						, pInput->asDouble(Get_NX()-2, y, Scale)	   );
			double Grid_Left       = std::max(pInput->asDouble(0,         y,Scale)	, pInput->asDouble(1,          y, Scale)	   );
			double Grid_Right      = std::max(pInput->asDouble(Get_NX()-1,y,Scale)	, pInput->asDouble(Get_NX()-2, y, Scale)	   );

			pOutput->Set_Value( 0,          y, 		Boundary ? Boundary_Value  		: pInput->asDouble(0,          y, Scale), Scale);
			pOutput->Set_Value( 1,          y, 		Boundary ? Boundary_Left   		: Grid_Left,                      		  Scale);
			pOutput->Set_Value( Get_NX()-1, y, 		Boundary ? Boundary_Value  		: pInput->asDouble(Get_NX()-1, y, Scale), Scale);
			pOutput->Set_Value( Get_NX()-2, y, 		Boundary ? Boundary_Right  		: Grid_Right,                     		  Scale);

			if( Method == 1 )
			{
				PQ_add(pixQ, newPix(y, 1),          Boundary ? Boundary_Left  		: Grid_Left);
				PQ_add(pixQ, newPix(y, Get_NX()-2), Boundary ? Boundary_Right 		: Grid_Right);
			}
			if( Method == 2 )
			{
				pOpen->push_cell(1,           y, 	Boundary ? Boundary_Left  		: Grid_Left);
				pOpen->push_cell(Get_NX()-2,  y, 	Boundary ? Boundary_Right 		: Grid_Right);
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
bool CFillMinima::Fill_Sinks_Barnes( CSG_Grid *pInput, CSG_Grid *pOutput, CSG_Grid *pClosed, grid_cellz_pq *open )
{
	std::queue<grid_cellz> pit;
	while(open->size()>0 || pit.size()>0)
	{
		grid_cellz c;
		if(pit.size()>0)
		{
      		c=pit.front();
      		pit.pop();
		}
		else
		{
			c=open->top();
			open->pop();
    	}

		for(int n=1;n<=8;n++)
		{
			int nx=c.x+dx[n];
			int ny=c.y+dy[n];
      		
			if( pClosed->is_NoData( nx, ny) )
			{		
				pClosed->Set_Value( nx, ny, true, true );

				double elevation = pInput->asDouble( nx, ny);
      
				if( elevation <=c.z)
				{
					if( elevation <c.z )
					{
						pOutput->Set_Value( nx,ny, c.z);
					}
					pit.push(grid_cellz(nx,ny,c.z));
				} 
				else
				{
					open->push_cell(nx,ny,elevation);
				}
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
bool CFillMinima::Fill_Sinks_Soille( CSG_Grid *pInput, CSG_Grid *pOutput, CSG_Grid *pClosed, PixelQueue *pixQ, int hStart, int hEnd )
{
	PQel *p, *nbrs, *pNbr, *pNext;
    int hCrt = (int) hStart;

	int dxs[] = { 0, 1, 0,-1};
	int dys[] = { 1, 0,-1, 0};
    do 
	{
        while (! PQ_empty(pixQ, hCrt)) 
		{
            p = PQ_first(pixQ, hCrt);
			PQel *pl = NULL;
			
			for (int i=0; i<4; i++)
			{
				int y = p->i + dys[i];
				int x = p->j + dxs[i];

				if( pClosed->is_NoData(x, y) )
				{
					PQel *pNew = newPix(y, x);
					pNew->next = pl;
					pl = pNew;
				}
			}

            pNbr = pl;
            while (pNbr != NULL) 
			{
                int x = pNbr->j;
                int y = pNbr->i;
				
				if( pClosed->is_NoData(x, y) )
				{
                    int height = std::max(hCrt, pInput->asInt(x,y, false));
					
					pOutput->Set_Value(x,y, height, false);
					PQ_add(pixQ, pNbr, height);
				
					pClosed->Set_Value( x, y, true, true );
                }
                pNext = pNbr->next;
                free(pNbr);
                pNbr = pNext;
            }
            free(p);
        }
        hCrt++;
    } while (hCrt < hEnd);
    
    free(pixQ);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////




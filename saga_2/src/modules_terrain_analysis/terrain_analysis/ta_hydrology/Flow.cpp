
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
//                       Flow.cpp                        //
//                                                       //
//                 Copyright (C) 2003 by                 //
//                      Olaf Conrad                      //
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
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Flow.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFlow::CFlow(void)
{
	pCatch_AspectY	= NULL;
	bPoint			= false;

	//-----------------------------------------------------
	Set_Description (_TW(
		""
	));


	//-----------------------------------------------------
	// Input...

	Parameters.Add_Grid(
		NULL	, "ELEVATION"	, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "SINKROUTE"	, _TL("Sink Routes"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL	, "WEIGHT"		, _TL("Weight"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL	, "MATERIAL"	, _TL("Material"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL	, "TARGET"		, _TL("Target"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);	


	//-----------------------------------------------------
	// Output...

	Parameters.Add_Grid(
		NULL	, "CAREA"		, _TL("Catchment Area"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "CHEIGHT"		, _TL("Catchment Height"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL	, "CSLOPE"		, _TL("Catchment Slope"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL	, "ACCU_TOT"	, _TL("Total accumulated Material"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);	

	Parameters.Add_Grid(
		NULL	, "ACCU_LEFT"	, _TL("Accumulated Material from _left_ side"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);		

	Parameters.Add_Grid(
		NULL	, "ACCU_RIGHT"	, _TL("Accumulated Material from _right_ side"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);		
	
	//-----------------------------------------------------
	// Options...

	Parameters.Add_Value(
		NULL	, "STEP"		, _TL("Step"),
		_TL(""),
		PARAMETER_TYPE_Int, 1, 1, true
	);
}

//---------------------------------------------------------
CFlow::~CFlow(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow::Set_Point(int x, int y)
{
	bPoint	= true;
	xPoint	= x;
	yPoint	= y;
}

//---------------------------------------------------------
bool CFlow::On_Execute(void)
{
	bool	bResult	= false;
	long	n;
	double	d;

	//-------------------------------------------------
	pDTM			= Parameters("ELEVATION")	->asGrid();
	pRoute			= Parameters("SINKROUTE")	->asGrid();
	pWeight			= Parameters("WEIGHT")		->asGrid();
	pMaterial		= Parameters("MATERIAL")	->asGrid();
	pTarget			= Parameters("TARGET")		->asGrid();

	pCatch			= Parameters("CAREA")		->asGrid();
	DataObject_Set_Colors(pCatch, 100, SG_COLORS_WHITE_BLUE);
	pCatch_Height	= NULL;
	pCatch_Slope	= NULL;
	pCatch_Aspect	= NULL;
	pFlowPath		= NULL;
	pAccu_Tot		= NULL;
	pAccu_Left		= NULL;
	pAccu_Right		= NULL;

	Step			= Parameters("STEP")		->asInt();

	//-------------------------------------------------
	On_Initialize();

	//-------------------------------------------------
	if( pCatch )
	{
		pCatch			->Assign(0.0);
	}

	if( pCatch_Height )
	{
		pCatch_Height	->Assign(0.0);
	}

	if( pCatch_Slope )
	{
		pCatch_Slope	->Assign(0.0);
	}

	if( pCatch_Aspect )
	{
		pCatch_Aspect	->Assign(0.0);
		pCatch_AspectY	 = SG_Create_Grid(pCatch_Aspect);
	}

	if( pFlowPath )
	{
		pFlowPath		->Assign(0.0);
	}

	if( pAccu_Tot )
	{
		pAccu_Tot		->Assign(1.0);
	}	

	if( pAccu_Left )
	{
		pAccu_Left		->Assign(1.0);
	}	

	if( pAccu_Right )
	{
		pAccu_Right		->Assign(1.0);
	}	
	
	//-------------------------------------------------
	if( bPoint )
	{
		bPoint	= false;

		if( is_InGrid(xPoint, yPoint) )
		{
			Calculate(xPoint, yPoint);

			On_Finalize();

			for(n=0; n<Get_NCells(); n++)
			{
				d	= pCatch->asDouble(n);
				pCatch->Set_Value(n, 100.0 * d);
			}

			bResult		= true;
		}
	}
	else
	{
		pCatch_Height	= Parameters("CHEIGHT")	->asGrid();
		pCatch_Slope	= Parameters("CSLOPE")	->asGrid();
		
		pAccu_Tot		= Parameters("ACCU_TOT")	->asGrid();
		pAccu_Left		= Parameters("ACCU_LEFT")	->asGrid();
		pAccu_Right		= Parameters("ACCU_RIGHT")	->asGrid();
		
		Calculate();

		On_Finalize();
		Finalize();

		bResult			= true;
	}

	//-------------------------------------------------
	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow::Init_Cell(int x, int y)
{
	double	Weight, Material, Slope, Aspect;

	Weight		= pWeight   ? pWeight  ->asDouble(x, y) : 1.0;
	Material	= pMaterial ? pMaterial->asDouble(x, y) : 1.0;

	if( pCatch )
	{
		pCatch			->Add_Value(x, y, Weight);
	}

	if( pCatch_Height )
	{
		pCatch_Height	->Add_Value(x, y, Weight * pDTM->asDouble(x, y));
	}

	if( pCatch_Slope )
	{
		Get_Gradient(x, y, Slope, Aspect);

		pCatch_Slope	->Add_Value(x, y, Weight * Slope);
	}

	if( pCatch_Aspect && pCatch_AspectY )
	{
		Get_Gradient(x, y, Slope, Aspect);

		pCatch_Aspect	->Add_Value(x, y, Weight * sin(Aspect));
		pCatch_AspectY	->Add_Value(x, y, Weight * cos(Aspect));
	}
	
	if( pAccu_Tot )
	{
		pAccu_Tot		->Set_Value(x, y, Weight * Material);
	}	

	if( pAccu_Left )
	{
		pAccu_Left		->Set_Value(x, y, Weight * Material);
	}	

	if( pAccu_Right )
	{
		pAccu_Right		->Set_Value(x, y, Weight * Material);
	}	
	
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow::Finalize(void)
{
	for(int n=0; n<Get_NCells() && Set_Progress_NCells(n); n++)
	{
		if( pDTM->is_NoData(n) )
		{
			if( pCatch )		{	pCatch			->Set_NoData(n);	}
			if( pCatch_Height )	{	pCatch_Height	->Set_NoData(n);	}
			if( pCatch_Slope )	{	pCatch_Slope	->Set_NoData(n);	}
			if( pCatch_Aspect )	{	pCatch_Aspect	->Set_NoData(n);	}
			if( pFlowPath )		{	pFlowPath		->Set_NoData(n);	}
			if( pAccu_Tot )		{	pAccu_Tot		->Set_NoData(n);	}
			if( pAccu_Left )	{	pAccu_Left		->Set_NoData(n);	}
			if( pAccu_Right )	{	pAccu_Right		->Set_NoData(n);	}
		}
		else
		{
			//---------------------------------------------
			double	Catch	= pCatch->asDouble(n);

			if( pCatch )
			{
				pCatch	->Set_Value(n, Catch * Get_System()->Get_Cellarea());
			}

			if( Catch > 0.0 )
			{
				if( pCatch_Height )	{	pCatch_Height	->Set_Value(n, pCatch_Height->asDouble(n) / Catch - pDTM->asDouble(n));	}
				if( pCatch_Slope )	{	pCatch_Slope	->Mul_Value(n, 1.0 / Catch);	}
				if( pFlowPath )		{	pFlowPath		->Mul_Value(n, 1.0 / Catch);	}
			}
			else
			{
				if( pCatch_Height )	{	pCatch_Height	->Set_Value(n, pDTM->asDouble(n));	}
				if( pCatch_Slope )	{	pCatch_Slope	->Set_Value(n, 0.0);	}
				if( pFlowPath )		{	pFlowPath		->Set_Value(n, 0.0);	}
			}

			//---------------------------------------------
			if( pCatch_Aspect && pCatch_AspectY )
			{
				double	G	= pCatch_Aspect	->asDouble(n);
				double	H	= pCatch_AspectY->asDouble(n);

				pCatch_Aspect	->Set_Value(n, G ? fmod(M_PI_270 + atan2(H, G), M_PI_360) : (H > 0 ? M_PI_270 : (H < 0 ? M_PI_090 : -1)));
			}
			
			//---------------------------------------------
			double	Material	= pMaterial ? pMaterial->asDouble(n) : 1.0;
			double	Weight		= pWeight   ? pWeight  ->asDouble(n) : 1.0;

			if( pAccu_Left && pTarget )
			{
				if( pTarget->is_NoData(n) )
				{
					pAccu_Left	->Set_NoData(n);
				}
				else
				{
					pAccu_Left	->Add_Value(n, - 0.5 * Weight * Material);
				}
			}
			
			if( pAccu_Right && pTarget )
			{
				if( pTarget->is_NoData(n) )
				{	
					pAccu_Right	->Set_NoData(n);
				}
				else
				{
					pAccu_Right	->Add_Value(n, - 0.5 * Weight * Material);
				}
			}			
		}
	}

	//-----------------------------------------------------
	if( pCatch_AspectY )
	{
		delete(pCatch_AspectY);

		pCatch_AspectY	= NULL;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow::Get_Gradient(int x, int y, double &Slope, double &Aspect)
{
	pDTM->Get_Gradient(x, y, Slope, Aspect);
}

//---------------------------------------------------------

///////////////////////////////////////////////////////////
//		            SIDE-Algorithm                       //
//					                                     //
//              Copyright (c) 2010 by					 //
//														 //
//                  Thomas J. Grabs                      //
//					                                     //
//                                                       //
//  website:       www.thomasgrabs.com                   //
//					                                     //
//            Updates and compiled versions of           //
//           this algorithm can be found on the          //
//                    author's website                   //
//					                                     //
//  Purpose:				                             //
//	========				                             //
//  Determine the side of a flow line relative to the    //
//  (stream) flow direction in an adjacent grid cell     //
//					                                     //
//-------------------------------------------------------//
//           !!!    PLEASE CITE    !!!                   //
// the associated scientific article when redistributing //
// and/or using and/or modifying this source code or     //
// parts of it:                                          // 
//                                                       //
// "Calculating terrain indices along streams - a new    //
//  method for separating stream sides,                  //
//  Grabs, T. J. et al., published in Water Resources    //
//  Research (WRR), please search the complete reference //
// on the internet or on "http://www.agu.org/journals/wr"//
//-------------------------------------------------------//
//                                                       //
// This source code is part of 'SAGA - System for        //
// Automated Geoscientific Analyses'. You can            //
// redistribute this source code and/or modify  it under //
// the terms  of the GNU General Public License as       //
// published by the Free Software Foundation; version 2  //
// of the License.                                       //
//                                                       //
// The source code is distributed in the hope that it    //
// will be useful, but WITHOUT ANY WARRANTY; without     //
// even the implied warranty of MERCHANTABILITY or       //
// FITNESS FOR A PARTICULAR PURPOSE. See the GNU General //
// Public License for more details.                      //
//                                                       //
// You should have received a copy of the GNU General    //
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//														 //
///////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////
//														 //
// List of variables:                                    //
//														 //
// x,y         : Coordinates of the grid cell containing //
//			     the flow line.							 //
// Direction   : Direction of the flow line				 //
// left        : TRUE/FALSE depending on the location of //
//			     the flow line.							 //
// right       : TRUE/FALSE depending on the location of //
//			     the flow line.							 //
// FL_Dir      :  Direction of the flow line             //
// stream1_Dir :  Streamflow direction                   //
// stream2_Dir :  Streamflow direction (upstream)        //
// stream1_X   :  X-coord. of stream grid cell           //
// stream1_Y   :  Y-coord. of stream grid cell           //
// stream2_X   :  X-coord. of (upstr.) stream grid cell  //
// stream2_X   :  Y-coord. of (upstr.) stream grid cell  //
// FL_Vec      :  Direction of flow line (as vector)     //
// stream1_Vec :  Streamflow direction (as vector)       //
// stream2_Vec :  Streamflow direction (as vector, upstr)//
// SP          : Scalar product of 2 vectors             //
// CP_A        : Cross product "A" of 2 vectors          //
// CP_B        : Cross product "B" of 2 vectors          //
// CP_C        : Cross product "C" of 2 vectors          //
// Zcp_a       : Z component of the cross product "A"    //
// Zcp_b       : Z component of the cross product "B"    //
// Zcp_c       : Z component of the cross product "C"    //
// a-z         : auxiliary variables                     //
// NTributaries: Number of tributaries to a junction     //
// prev_right  : auxiliary variable (true/false)         //
// prev_left   : auxiliary variable (true/false)         //
// is_upstream : auxiliary variable (true/false)         //
//-------------------------------------------------------//
//														 //
// Conventions                                  		 //
//-------------------------------------------------------//
// left==true & right==false: Flow line is on the left   //
// left==false & right==true: Flow line is on the right  //
// left==true & right==true: The side of the flow line is//
//                          not determined (channel head)//
//                           or lies between             //
//                           tributaries to a junction   //
///////////////////////////////////////////////////////////

void CFlow::Find_Sides(int x, int y, int Direction, bool &left, bool &right)
{
	int			FL_Dir, stream1_Dir, stream2_Dir;
	int			stream1_X, stream1_Y;
	int			stream2_X, stream2_Y;
	CSG_Vector	FL_Vec(3), stream1_Vec(3), stream2_Vec(3);
	CSG_Vector  CP_A(3), CP_B(3), CP_C(3); 
	double		SP;
	double		Zcp_a, Zcp_b, Zcp_c;

	// Initialize the stream side variables to their default. 
	// Default: The side of the flow line is not determined.
	left = right = true;
				
    // Determine the coordinates and stream direction the adjacent
	// grid cell, to which the flow line points.
	FL_Dir		        = Direction;
	stream1_X			= Get_xTo( FL_Dir, x );
	stream1_Y			= Get_yTo( FL_Dir, y );
	stream1_Dir			= pTarget->asInt(stream1_X, stream1_Y);

	/*--Note: At this point it is already assumed that FL_Dir points to 
	    a grid cell that belongs to the stream network. Thus, stream1_X,
		stream1_Y and stream1_Dir have all non-missing values. If the code
		is to be used elsewhere, one should verify that FL_Dir points to
		a grid cell that belongs to the stream network.--*/
	
	// Write the direction of the flow line as vector:
	FL_Vec[0]	= Get_xTo( FL_Dir );
	FL_Vec[1]	= Get_yTo( FL_Dir );
	FL_Vec[2]	= 0.0; //z-component is normally 0
	
	// Write the streamflow directon as vector:
	stream1_Vec[0]		= Get_xTo( stream1_Dir );
	stream1_Vec[1]		= Get_yTo( stream1_Dir );
	stream1_Vec[2]		= 0.0;//z-component is normally 0

	// Initialize the upstream streamflow direction vector
	// and set all components to zero 
	stream2_Vec[0]		= 0.0;
	stream2_Vec[1]		= 0.0;
	stream2_Vec[2]		= 0.0;
	
	// Calculate the scalar product
	SP					= FL_Vec[0] * stream1_Vec[0] + FL_Vec[1] * stream1_Vec[1];
	
	// Adjust the scalar product by dividing it by the lengths of FL_Vec and stream1_Vec
	SP = SP /sqrt( FL_Vec[0]*FL_Vec[0] + FL_Vec[1]*FL_Vec[1])
			/sqrt(stream1_Vec[0]*stream1_Vec[0] + stream1_Vec[1]*stream1_Vec[1]);


	if( fabs(SP - (-1)) < 0.00001)
	{
		// SP is (approximately) equal to -1! 
		// The flow line is hence oriented opposite to the streamflow direction.
		// Further calculations are skipped: The side of the flow line remains the default.
		// This can occur if an endpoint of the streamflow direction map does *not*
		// point to a missing value. In other words, the stream outlet lies 'inside' the 
		// DEM and not right on the border of the DEM.
		// Since this can be intentional, a user notification is only optional.

		/*-- (optional) Notification of the user  --*/
	}
	else
	{
		/*--  Core of the SIDE-Algorithm  --*/

		int		NTributaries;
		bool	prev_right, prev_left;
		bool	is_upstream;
		
		// Initialize the number of tributaries and other auxiliary variables
		NTributaries = 0; //default: channel head
		prev_right = prev_left = true;
		is_upstream= false;
					
		// The full vector-cross-product of the streamflow direction and the flow line direction
		// CP_A				= FL_Vec * stream1_Vec;
		// is not calculated because it is more efficient to calculate only the z-component 
		// of the cross-product:
		
		Zcp_a	= FL_Vec[0]*stream1_Vec[1] - FL_Vec[1]*stream1_Vec[0];

		// Look for upstream tributaries / stream grid cells
		for(int i=0; i<8; i++)
		{
			// find adjacent grid cell coordinates
			stream2_X =  Get_xTo(i, stream1_X);
			stream2_Y =  Get_yTo(i, stream1_Y);
			
			// Make sure it is within the map domain
			if( is_InGrid(stream2_X, stream2_Y)	)
			{
				// Make sure it is not a missing-value
				if ( ! pTarget->is_NoData( stream2_X, stream2_Y ) )
				{
					// Is the stream cell an upstream tributary?
					stream2_Dir = pTarget->asInt(stream2_X, stream2_Y);
					is_upstream = stream1_X == Get_xTo(stream2_Dir, stream2_X) && 
									stream1_Y == Get_yTo(stream2_Dir, stream2_Y);
								
					if( is_upstream )
					{
						// The stream cell is an upstream tributary!
						NTributaries++;
						
						// Convert the upstream streamflow direction to a vector
						stream2_Vec[0]	= Get_xTo( stream2_Dir );
						stream2_Vec[1]	= Get_yTo( stream2_Dir );
						stream2_Vec[2]	= 0.0;
						
						//Calculate only z-component of the vector-cross-product
						Zcp_b			= FL_Vec[0]*stream2_Vec[1] - FL_Vec[1]*stream2_Vec[0];
						
						//store the previous position of the flow line
						prev_right		= right;
						prev_left		= left;

						// Test if Z components have the same sign
						// Note that posing the condition "( Zcp_a * Zcp_b >= 0 && Zcp_b != 0)"
						// and evaluating the sign of *only* Zcp_b
						// is *equivalent* but presumably a tiny bit more efficient
						// than  testing "( Zcp_a * Zcp_b > 0)"
						// and evaluating the sign of Zcp_a *or* Zcp_b
						// If you prefer to follow exactly the description in the WRR-article, simply
						// replace "( Zcp_a * Zcp_b >= 0 && Zcp_b != 0)" by "(Zcp_a * Zcp_b > 0)"
						// and it'll still work.
						if ( Zcp_a * Zcp_b >= 0 && Zcp_b != 0)
						{
							//Case1: Zcp_a and Zcp_b have the same sign, thus the position of the flow  
							//line is the same for both stream grid cells
							//Case2: Zcp_a is zero but Zcp_b is not zero, the position of the flow line
							//can simply be determined from the sign of Zcp_b
							right = (Zcp_b < 0);
							left  = !right;
						}
						else
						{
							//Since Zcp_a and Zcp_b have opposite signs (or Zcp_b is zero), the flow line
							//is located at a sharp bend. 

							//Calculate only z-component of the vector-cross-product
							Zcp_c	= stream1_Vec[0]*stream2_Vec[1] - stream1_Vec[1]*stream2_Vec[0];
							

							right = (Zcp_c < 0);
							left  = !right;
						}													
						if ( NTributaries > 1 && (right != prev_right) )
						{
							//It is a junction (NTributaries > 1) and the flow line lies between
							//two tributaries
							left = right = true;
							//Set i to 9 in order to exit the loop (there is no use in checking
							//for more potential tributaries since the flow line will always lie 
							//between two tributaries
							i = 9;
						}
					}
				}
			}		
		}
	}	
}


//---------------------------------------------------------
void CFlow::Add_Fraction(int x, int y, int Direction, double Fraction)
{
	int		ix, iy;

	if( is_InGrid(x, y) )
	{
		ix	= Get_xTo(Direction, x);
		iy	= Get_yTo(Direction, y);
		
		if( is_InGrid(ix, iy) )
		{
			if( pCatch )
			{
				pCatch			->Add_Value(ix, iy, Fraction * pCatch			->asDouble(x, y));
			}

			if( pCatch_Height )
			{
				pCatch_Height	->Add_Value(ix, iy, Fraction * pCatch_Height	->asDouble(x, y));
			}

			if( pCatch_Slope )
			{
				pCatch_Slope	->Add_Value(ix, iy, Fraction * pCatch_Slope		->asDouble(x, y));
			}

			if( pFlowPath )
			{
				pFlowPath		->Add_Value(ix, iy, Fraction * (pFlowPath		->asDouble(x, y) + Get_Length(Direction)));
			}

			if( pCatch_Aspect && pCatch_AspectY )
			{
				pCatch_Aspect	->Add_Value(ix, iy, Fraction * pCatch_Aspect	->asDouble(x, y));
				pCatch_AspectY	->Add_Value(ix, iy, Fraction * pCatch_AspectY	->asDouble(x, y));
			}
			
			if( !pTarget )
			{
				if( pAccu_Tot ) pAccu_Tot -> Add_Value(ix, iy, Fraction * pAccu_Tot ->asDouble(x, y));
				if( pAccu_Left ) pAccu_Left -> Add_Value(ix, iy, Fraction * pAccu_Left ->asDouble(x, y));
				if( pAccu_Right ) pAccu_Right -> Add_Value(ix, iy, Fraction * pAccu_Right ->asDouble(x, y));				
			}
			
			if( pTarget ) if( pTarget->is_NoData(ix, iy) && pTarget->is_NoData(x, y) )
			{
				if( pAccu_Tot ) pAccu_Tot -> Add_Value(ix, iy, Fraction * pAccu_Tot ->asDouble(x, y));
				if( pAccu_Left ) pAccu_Left -> Add_Value(ix, iy, Fraction * pAccu_Left ->asDouble(x, y));
				if( pAccu_Right ) pAccu_Right -> Add_Value(ix, iy, Fraction * pAccu_Right ->asDouble(x, y));				
			}
			

			if( pTarget ) if( !pTarget->is_NoData(ix, iy) && pTarget->is_NoData(x, y))
			{
				
				bool left, right;

				Find_Sides(x, y, Direction, left, right);
			
				if( pAccu_Tot ) pAccu_Tot -> Add_Value(ix, iy, Fraction * pAccu_Tot ->asDouble(x, y));
				
				if( right && !left && pAccu_Right ) pAccu_Right -> Add_Value(ix, iy, Fraction * pAccu_Right ->asDouble(x, y));
				
				if( left && !right && pAccu_Left ) pAccu_Left -> Add_Value(ix, iy, Fraction * pAccu_Left ->asDouble(x, y));
				
				if( left && right )
				{
					if( pAccu_Right ) pAccu_Right -> Add_Value(ix, iy, 0.5 * Fraction * pAccu_Right ->asDouble(x, y));

					if( pAccu_Left ) pAccu_Left -> Add_Value(ix, iy, 0.5 * Fraction * pAccu_Left ->asDouble(x, y));			
				}				
			}
		}
	}
}

//---------------------------------------------------------
void CFlow::Add_Portion(int x, int y, int ix, int iy, int Direction)
{
	if( is_InGrid(x, y) && is_InGrid(ix, iy) )
	{
		
		if( pCatch )
		{
			pCatch			->Add_Value(ix, iy, pCatch			->asDouble(x, y));
		}

		if( pCatch_Height )
		{
			pCatch_Height	->Add_Value(ix, iy, pCatch_Height	->asDouble(x, y));
		}

		if( pCatch_Slope )
		{
			pCatch_Slope	->Add_Value(ix, iy, pCatch_Slope	->asDouble(x, y));
		}

		if( pFlowPath )
		{
			pFlowPath		->Add_Value(ix, iy, pFlowPath		->asDouble(x, y));
		}

		if( pCatch_Aspect && pCatch_AspectY )
		{
			pCatch_Aspect	->Add_Value(ix, iy, pCatch_Aspect	->asDouble(x, y));
			pCatch_AspectY	->Add_Value(ix, iy, pCatch_AspectY	->asDouble(x, y));
		}
		
		if( pTarget ) if( pTarget->is_NoData(ix, iy) && pTarget->is_NoData(x, y) )
		{
			if( pAccu_Tot ) pAccu_Tot -> Add_Value(ix, iy, pAccu_Tot ->asDouble(x, y));
			if( pAccu_Left ) pAccu_Left -> Add_Value(ix, iy, pAccu_Left ->asDouble(x, y));
			if( pAccu_Right ) pAccu_Right -> Add_Value(ix, iy, pAccu_Right ->asDouble(x, y));				
		}
		if( pTarget ) if( !pTarget->is_NoData(ix, iy) && pTarget->is_NoData(x, y))
			{
				
				bool left, right;

				Find_Sides(x, y, Direction, left, right);
					
				if( pAccu_Tot )pAccu_Tot -> Add_Value(ix, iy, pAccu_Tot->asDouble(x, y));
				
				if( right && !left && pAccu_Right ) pAccu_Right -> Add_Value(ix, iy, pAccu_Right->asDouble(x, y));
				
				if( left && !right && pAccu_Left ) pAccu_Left -> Add_Value(ix, iy, pAccu_Left->asDouble(x, y));
				
				if( left && right )
				{
					if( pAccu_Right ) pAccu_Right -> Add_Value(ix, iy, 0.5 * pAccu_Right->asDouble(x, y));
					if( pAccu_Left ) pAccu_Left -> Add_Value(ix, iy, 0.5 * pAccu_Left->asDouble(x, y));			
				}
				
			}		
		
	}
}

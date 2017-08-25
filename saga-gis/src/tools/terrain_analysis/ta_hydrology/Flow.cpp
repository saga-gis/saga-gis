/**********************************************************
 * Version $Id: Flow.cpp 1921 2014-01-09 10:24:11Z oconrad $
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
#define SET_GRID_TO(grd, val)	if( grd )	{	grd->Assign(val);	}

#define SET_GRID_CELL_VAL(x, y, grid, val)	if( grid )	{	grid->Set_Value(x, y, val);	}
#define ADD_GRID_CELL_VAL(x, y, grid, val)	if( grid )	{	grid->Add_Value(x, y, val);	}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFlow::CFlow(void)
{
	m_bPoint	= false;

	//-----------------------------------------------------
	Parameters.Add_Grid("", "ELEVATION"    , _TL("Elevation"                        ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid("", "SINKROUTE"    , _TL("Sink Routes"                      ), _TL(""), PARAMETER_INPUT_OPTIONAL);
	Parameters.Add_Grid("", "WEIGHTS"      , _TL("Weights"                          ), _TL(""), PARAMETER_INPUT_OPTIONAL);

	Parameters.Add_Grid("", "FLOW"         , _TL("Flow Accumulation"                ), _TL(""), PARAMETER_OUTPUT);

	Parameters.Add_Grid("", "VAL_INPUT"    , _TL("Input for Mean over Catchment"    ), _TL(""), PARAMETER_INPUT_OPTIONAL);
	Parameters.Add_Grid("", "VAL_MEAN"     , _TL("Mean over Catchment"              ), _TL(""), PARAMETER_OUTPUT);

	Parameters.Add_Grid("", "ACCU_MATERIAL", _TL("Material for Accumulation"        ), _TL(""), PARAMETER_INPUT_OPTIONAL);
	Parameters.Add_Grid("", "ACCU_TARGET"  , _TL("Accumulation Target"              ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid("", "ACCU_TOTAL"   , _TL("Accumulated Material"             ), _TL(""), PARAMETER_OUTPUT_OPTIONAL);	
	Parameters.Add_Grid("", "ACCU_LEFT"    , _TL("Accumulated Material (Left Side)" ), _TL(""), PARAMETER_OUTPUT_OPTIONAL);		
	Parameters.Add_Grid("", "ACCU_RIGHT"   , _TL("Accumulated Material (Right Side)"), _TL(""), PARAMETER_OUTPUT_OPTIONAL);		
	
	//-----------------------------------------------------
	Parameters.Add_Int("",
		"STEP"		, _TL("Step"),
		_TL(""),
		1, 1, true
	);

	Parameters.Add_Choice("",
		"FLOW_UNIT"	, _TL("Flow Accumulation Unit"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("number of cells"),
			_TL("cell area")
		), 1
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow::Set_Point(int x, int y)
{
	m_bPoint	= true;

	m_xPoint	= x;
	m_yPoint	= y;
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CFlow::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "VAL_INPUT") )
	{
		pParameters->Set_Enabled("VAL_MEAN"   , pParameter->asGrid() != NULL);
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "ACCU_MATERIAL") )
	{
		pParameters->Set_Enabled("ACCU_TARGET", pParameter->asGrid() != NULL);
		pParameters->Set_Enabled("ACCU_TOTAL" , pParameter->asGrid() != NULL);
		pParameters->Set_Enabled("ACCU_LEFT"  , pParameter->asGrid() != NULL);
		pParameters->Set_Enabled("ACCU_RIGHT" , pParameter->asGrid() != NULL);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFlow::On_Execute(void)
{
	//-------------------------------------------------
	m_pDTM				= Parameters("ELEVATION"    )->asGrid();
	m_pRoute			= Parameters("SINKROUTE"    )->asGrid();
	m_pWeights			= Parameters("WEIGHTS"      )->asGrid();

	m_pAccu_Material	= Parameters("ACCU_MATERIAL")->asGrid();
	m_pAccu_Target		= Parameters("ACCU_TARGET"  )->asGrid();

	m_pFlow				= Parameters("FLOW"         )->asGrid();

	m_pFlow_Length		= NULL;

	if( (m_pVal_Input	= Parameters("VAL_INPUT"    )->asGrid()) != NULL
	&&  (m_pVal_Mean	= Parameters("VAL_MEAN"     )->asGrid()) != NULL )
	{
		m_pVal_Mean->Set_Name(CSG_String::Format("%s [%s]", m_pVal_Input->Get_Name(), _TL("Mean over Catchment")));
		m_pVal_Mean->Set_Unit(m_pVal_Input->Get_Unit());
	}
	else
	{
		m_pVal_Mean		= NULL;
	}

	m_pAccu_Total		= NULL;
	m_pAccu_Left		= NULL;
	m_pAccu_Right		= NULL;

	m_Step				= Parameters("STEP")->asInt();

	//-----------------------------------------------------
	On_Initialize();

	SET_GRID_TO(m_pFlow       , 0.0);
	SET_GRID_TO(m_pFlow_Length, 0.0);
	SET_GRID_TO(m_pVal_Mean   , 0.0);
	SET_GRID_TO(m_pAccu_Total , 1.0);
	SET_GRID_TO(m_pAccu_Left  , 1.0);
	SET_GRID_TO(m_pAccu_Right , 1.0);

	DataObject_Set_Colors(m_pFlow, 11, SG_COLORS_WHITE_BLUE);
	
	//-----------------------------------------------------
	if( m_bPoint )
	{
		m_bPoint	= false;

		if( is_InGrid(m_xPoint, m_yPoint) )
		{
			Calculate(m_xPoint, m_yPoint);

			On_Finalize();

			m_pFlow->Multiply(100.0);	// output as percentage

			return( true );
		}
	}

	//-----------------------------------------------------
	else
	{
		m_pAccu_Total	= Parameters("ACCU_TOTAL")->asGrid();
		m_pAccu_Left	= Parameters("ACCU_LEFT" )->asGrid();
		m_pAccu_Right	= Parameters("ACCU_RIGHT")->asGrid();
		
		DataObject_Set_Colors(m_pFlow_Length, 11, SG_COLORS_RED_GREY_BLUE);

		Calculate();

		On_Finalize();

		_Finalize();

		return( true );
	}

	//-----------------------------------------------------
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow::Init_Cell(int x, int y)
{
	double	Weight	= !m_pWeights ? 1.0 : m_pWeights->is_NoData(x, y) ? 0.0 : m_pWeights->asDouble(x, y);

	ADD_GRID_CELL_VAL(x, y, m_pFlow, Weight);

	if( m_pVal_Mean && !m_pVal_Input->is_NoData(x, y) )
	{
		ADD_GRID_CELL_VAL(x, y, m_pVal_Mean, Weight * m_pVal_Input->asDouble(x, y));
	}

	Weight	*= m_pAccu_Material ? m_pAccu_Material->asDouble(x, y) : 1.0;

	SET_GRID_CELL_VAL(x, y, m_pAccu_Total, Weight);
	SET_GRID_CELL_VAL(x, y, m_pAccu_Left , Weight);
	SET_GRID_CELL_VAL(x, y, m_pAccu_Right, Weight);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow::_Finalize(void)
{
	bool	bCellsToArea	= Parameters("FLOW_UNIT")->asInt() == 1;

	#pragma omp parallel for
	for(sLong n=0; n<Get_NCells(); n++)
	{
		if( m_pDTM->is_NoData(n) )
		{
			if( m_pFlow        )	{	m_pFlow       ->Set_NoData(n);	}
			if( m_pFlow_Length )	{	m_pFlow_Length->Set_NoData(n);	}
			if( m_pVal_Mean    )	{	m_pVal_Mean   ->Set_NoData(n);	}
			if( m_pAccu_Total  )	{	m_pAccu_Total ->Set_NoData(n);	}
			if( m_pAccu_Left   )	{	m_pAccu_Left  ->Set_NoData(n);	}
			if( m_pAccu_Right  )	{	m_pAccu_Right ->Set_NoData(n);	}
		}
		else
		{
			//---------------------------------------------
			double	Flow	= m_pFlow->asDouble(n);

			if( m_pFlow && bCellsToArea )
			{
				m_pFlow->Set_Value(n, Flow * Get_System()->Get_Cellarea());
			}

			if( Flow > 0.0 )
			{
				if( m_pFlow_Length )	{	m_pFlow_Length->Mul_Value(n, 1.0 / Flow);	}
				if( m_pVal_Mean    )	{	m_pVal_Mean   ->Mul_Value(n, 1.0 / Flow);	}
			}
			else
			{
				if( m_pFlow_Length )	{	m_pFlow_Length->Set_Value(n, 0.0);	}
				if( m_pVal_Mean    )	{	m_pVal_Mean   ->Set_Value(n, 0.0);	}
			}

			//---------------------------------------------
			if( m_pAccu_Target )
			{
				if( m_pAccu_Target->is_NoData(n) )
				{
					if( m_pAccu_Left  )	{	m_pAccu_Left ->Set_NoData(n);	}
					if( m_pAccu_Right )	{	m_pAccu_Right->Set_NoData(n);	}
				}
				else
				{
					double	Material	= m_pAccu_Material ? m_pAccu_Material->asDouble(n) : 1.0;
					double	Weight		= !m_pWeights ? 1.0 : m_pWeights->is_NoData(n) ? 0.0 : m_pWeights->asDouble(n);

					if( m_pAccu_Left  )	{	m_pAccu_Left ->Add_Value(n, - 0.5 * Weight * Material);	}
					if( m_pAccu_Right )	{	m_pAccu_Right->Add_Value(n, - 0.5 * Weight * Material);	}
				}
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow::Get_Gradient(int x, int y, double &Slope, double &Aspect)
{
	m_pDTM->Get_Gradient(x, y, Slope, Aspect);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CFlow::Add_Fraction(int x, int y, int Direction, double Fraction)
{
	if( !is_InGrid(x, y) || Direction < 0 )
	{
		return;
	}

	int	ix	= Get_xTo(Direction, x);
	int	iy	= Get_yTo(Direction, y);
		
	if( !is_InGrid(ix, iy) )
	{
		return;
	}

	ADD_GRID_CELL_VAL(ix, iy, m_pFlow       , Fraction *  m_pFlow       ->asDouble(x, y));
	ADD_GRID_CELL_VAL(ix, iy, m_pFlow_Length, Fraction * (m_pFlow_Length->asDouble(x, y) + Get_Length(Direction)));
	ADD_GRID_CELL_VAL(ix, iy, m_pVal_Mean   , Fraction *  m_pVal_Mean   ->asDouble(x, y));

	if( !m_pAccu_Target )
	{
		ADD_GRID_CELL_VAL(ix, iy, m_pAccu_Total, Fraction * m_pAccu_Total->asDouble(x, y));
		ADD_GRID_CELL_VAL(ix, iy, m_pAccu_Left , Fraction * m_pAccu_Left ->asDouble(x, y));
		ADD_GRID_CELL_VAL(ix, iy, m_pAccu_Right, Fraction * m_pAccu_Right->asDouble(x, y));
	}
	else if(  m_pAccu_Target->is_NoData(ix, iy) && m_pAccu_Target->is_NoData(x, y) )
	{
		ADD_GRID_CELL_VAL(ix, iy, m_pAccu_Total, Fraction * m_pAccu_Total->asDouble(x, y));
		ADD_GRID_CELL_VAL(ix, iy, m_pAccu_Left , Fraction * m_pAccu_Left ->asDouble(x, y));
		ADD_GRID_CELL_VAL(ix, iy, m_pAccu_Right, Fraction * m_pAccu_Right->asDouble(x, y));
	}
	else if( !m_pAccu_Target->is_NoData(ix, iy) && m_pAccu_Target->is_NoData(x, y) )
	{
		bool	left, right;

		Find_Sides(x, y, Direction, left, right);

		ADD_GRID_CELL_VAL(ix, iy, m_pAccu_Total, Fraction * m_pAccu_Total->asDouble(x, y));

		if( left && right )
		{
			ADD_GRID_CELL_VAL(ix, iy, m_pAccu_Right, Fraction * m_pAccu_Right->asDouble(x, y) * 0.5);
			ADD_GRID_CELL_VAL(ix, iy, m_pAccu_Left , Fraction * m_pAccu_Left ->asDouble(x, y) * 0.5);
		}
		else if( right && !left )
		{
			ADD_GRID_CELL_VAL(ix, iy, m_pAccu_Right, Fraction * m_pAccu_Right->asDouble(x, y));
		}				
		else if( left && !right )
		{
			ADD_GRID_CELL_VAL(ix, iy, m_pAccu_Left , Fraction * m_pAccu_Left ->asDouble(x, y));
		}
	}
}

//---------------------------------------------------------
void CFlow::Add_Portion(int x, int y, int ix, int iy, int Direction)
{
	if( !is_InGrid(x, y) || !is_InGrid(ix, iy) )
	{
		return;
	}

	ADD_GRID_CELL_VAL(ix, iy, m_pFlow       , m_pFlow       ->asDouble(x, y));
	ADD_GRID_CELL_VAL(ix, iy, m_pFlow_Length, m_pFlow_Length->asDouble(x, y));
	ADD_GRID_CELL_VAL(ix, iy, m_pVal_Mean   , m_pVal_Mean   ->asDouble(x, y));

	if( m_pAccu_Target )
	{
		if( m_pAccu_Target->is_NoData(ix, iy) && m_pAccu_Target->is_NoData(x, y) )
		{
			ADD_GRID_CELL_VAL(ix, iy, m_pAccu_Total, m_pAccu_Total->asDouble(x, y));
			ADD_GRID_CELL_VAL(ix, iy, m_pAccu_Left , m_pAccu_Left ->asDouble(x, y));
			ADD_GRID_CELL_VAL(ix, iy, m_pAccu_Right, m_pAccu_Right->asDouble(x, y));				
		}
		else if( !m_pAccu_Target->is_NoData(ix, iy) && m_pAccu_Target->is_NoData(x, y))
		{
			bool	left, right;

			Find_Sides(x, y, Direction, left, right);

			ADD_GRID_CELL_VAL(ix, iy, m_pAccu_Total, m_pAccu_Total->asDouble(x, y));
				
			if( left && right )
			{
				ADD_GRID_CELL_VAL(ix, iy, m_pAccu_Right, m_pAccu_Right->asDouble(x, y) * 0.5);
				ADD_GRID_CELL_VAL(ix, iy, m_pAccu_Left , m_pAccu_Left ->asDouble(x, y) * 0.5);
			}
			else if( right && !left )
			{
				ADD_GRID_CELL_VAL(ix, iy, m_pAccu_Right, m_pAccu_Right->asDouble(x, y));
			}
			else if( left && !right )
			{
				ADD_GRID_CELL_VAL(ix, iy, m_pAccu_Left , m_pAccu_Left ->asDouble(x, y));
			}				
		}
	}
}


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
// Public License along with this program; if not, see   //
// <http://www.gnu.org/licenses/>.                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

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

//---------------------------------------------------------
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
	stream1_Dir			= m_pAccu_Target->asInt(stream1_X, stream1_Y);

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
				if ( ! m_pAccu_Target->is_NoData( stream2_X, stream2_Y ) )
				{
					// Is the stream cell an upstream tributary?
					stream2_Dir = m_pAccu_Target->asInt(stream2_X, stream2_Y);
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

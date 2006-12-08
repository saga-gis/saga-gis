
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      Grid_Filter                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                Filter_Multi_Dir_Lee.cpp               //
//                                                       //
//                 Copyright (C) 2003 by                 //
//                     Andre Ringeler                    //
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
//    e-mail:     aringel@gwdg.de                        //
//                                                       //
//    contact:    Andre Ringeler                         //
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


#include "Filter_Multi_Dir_Lee.h"


unsigned char Filter_Directions[16][9][9]=
{
	{
		{0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0},
		{1, 1, 1, 1, 1, 1, 1, 1, 1},
		{2, 2, 2, 2, 2, 2, 2, 2, 2},
		{1, 1, 1, 1, 1, 1, 1, 1, 1},
		{0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0}
	},
		
	{
		{0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0},
		{1, 1, 0, 0, 0, 0, 0, 0, 0},
		{2, 2, 1, 1, 1, 1, 0, 0, 0},
		{1, 1, 2, 2, 2, 2, 2, 1, 1},
		{0, 0, 0, 1, 1, 1, 1, 2, 2},
		{0, 0, 0, 0, 0, 0, 0, 1, 1},
		{0, 0, 0, 0, 0, 0, 0, 0, 0},
		{0, 0, 0, 0, 0, 0, 0, 0, 0}
	},
		
	{
		{0, 0, 0, 0, 0, 0, 0, 0, 0},
		{1, 0, 0, 0, 0, 0, 0, 0, 0},
		{2, 2, 1, 0, 0, 0, 0, 0, 0},
		{1, 1, 2, 2, 1, 0, 0, 0, 0},
		{0, 0, 1, 1, 2, 1, 1, 0, 0},
		{0, 0, 0, 0, 1, 2, 2, 1, 1},
		{0, 0, 0, 0, 0, 0, 1, 2, 2},
		{0, 0, 0, 0, 0, 0, 0, 0, 1},
		{0, 0, 0, 0, 0, 0, 0, 0, 0}
	},
		
	{
		{1, 0, 0, 0, 0, 0, 0, 0, 0},
		{2, 1, 0, 0, 0, 0, 0, 0, 0},
		{1, 2, 2, 1, 0, 0, 0, 0, 0},
		{0, 1, 1, 2, 1, 0, 0, 0, 0},
		{0, 0, 0, 1, 2, 1, 0, 0, 0},
		{0, 0, 0, 0, 1, 2, 1, 1, 0},
		{0, 0, 0, 0, 0, 1, 2, 2, 1},
		{0, 0, 0, 0, 0, 0, 0, 1, 2},
		{0, 0, 0, 0, 0, 0, 0, 0, 1}
	},
		
	{
		{2, 1, 0, 0, 0, 0, 0, 0, 0},
		{1, 2, 1, 0, 0, 0, 0, 0, 0},
		{0, 1, 2, 1, 0, 0, 0, 0, 0},
		{0, 0, 1, 2, 1, 0, 0, 0, 0},
		{0, 0, 0, 1, 2, 1, 0, 0, 0},
		{0, 0, 0, 0, 1, 2, 1, 0, 0},
		{0, 0, 0, 0, 0, 1, 2, 1, 0},
		{0, 0, 0, 0, 0, 0, 1, 2, 1},
		{0, 0, 0, 0, 0, 0, 0, 1, 2}
	},
		
	{
		{1, 2, 1, 0, 0, 0, 0, 0, 0},
		{0, 1, 2, 1, 0, 0, 0, 0, 0},
		{0, 0, 1, 2, 0, 0, 0, 0, 0},
		{0, 0, 1, 2, 1, 0, 0, 0, 0},
		{0, 0, 0, 1, 2, 1, 0, 0, 0},
		{0, 0, 0, 0, 1, 2, 1, 0, 0},
		{0, 0, 0, 0, 0, 2, 1, 0, 0},
		{0, 0, 0, 0, 0, 1, 2, 1, 0},
		{0, 0, 0, 0, 0, 0, 1, 2, 1}
	},
		
	{
		{0, 1, 2, 1, 0, 0, 0, 0, 0},
		{0, 0, 2, 1, 0, 0, 0, 0, 0},
		{0, 0, 1, 2, 1, 0, 0, 0, 0},
		{0, 0, 0, 2, 1, 0, 0, 0, 0},
		{0, 0, 0, 1, 2, 1, 0, 0, 0},
		{0, 0, 0, 0, 1, 2, 0, 0, 0},
		{0, 0, 0, 0, 1, 2, 1, 0, 0},
		{0, 0, 0, 0, 0, 1, 2, 0, 0},
		{0, 0, 0, 0, 0, 1, 2, 1, 0}
	},
		
	{
		{0, 0, 1, 2, 1, 0, 0, 0, 0},
		{0, 0, 1, 2, 1, 0, 0, 0, 0},
		{0, 0, 0, 2, 1, 0, 0, 0, 0},
		{0, 0, 0, 1, 2, 1, 0, 0, 0},
		{0, 0, 0, 1, 2, 1, 0, 0, 0},
		{0, 0, 0, 1, 2, 1, 0, 0, 0},
		{0, 0, 0, 0, 1, 2, 0, 0, 0},
		{0, 0, 0, 0, 1, 2, 1, 0, 0},
		{0, 0, 0, 0, 1, 2, 1, 0, 0}
	},
		
	{
		{0, 0, 0, 1, 2, 1, 0, 0, 0},
		{0, 0, 0, 1, 2, 1, 0, 0, 0},
		{0, 0, 0, 1, 2, 1, 0, 0, 0},
		{0, 0, 0, 1, 2, 1, 0, 0, 0},
		{0, 0, 0, 1, 2, 1, 0, 0, 0},
		{0, 0, 0, 1, 2, 1, 0, 0, 0},
		{0, 0, 0, 1, 2, 1, 0, 0, 0},
		{0, 0, 0, 1, 2, 1, 0, 0, 0},
		{0, 0, 0, 1, 2, 1, 0, 0, 0},
	}
};

double corr[16] =
{
		97694238.970824,
		103389994.176977,
		109699939.129502,
		103392028.763373,
		103392435.589500,
		103392028.763371,
		109699939.129499,
		103389994.176979,
		97694238.970826,
		103389994.200091,
		109699939.176253,
		103392028.826671,
		103392435.659830,
		103392028.826669,
		109699939.176251,
		103389994.200092
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFilter_Multi_Dir_Lee::CFilter_Multi_Dir_Lee(void)
{
	//-----------------------------------------------------
	Set_Name(_TL("Multi Direction Lee Filter"));

	Set_Author(_TL("Copyrights (c) 2003 by Andre Ringeler"));

	Set_Description	(_TW(
		"This Multi Direction Lee Filter is a enhanced Lee Filter\n"
		"It looks into 16 directions for the direction with the minium variance\n"
		"and applied a Lee Filter on this direction.\n\n"
		"Uses this filter for remove speckle noise in SAR images or DTMs.\n"
		"On DTMs this filter will preserves the slope and  narrow valleys. \n\n"
		"For details on the Lee Filter, see the article by Jong-Sen Lee:\n"
		"\"Digital Image Enhancement and Noise Filtering by Use of Local Statistics\",\n"
		"IEEE Transactions on Pattern Analysis and Machine Intelligence,\n"
		"Volume PAMI-2, Number 2, pages 165-168, March 1980.\n\n")
	);


	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL, "INPUT"	, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "RESULT"	, _TL("Filtered Grid"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL, "DIR"		, _TL("Direction"),
		_TL("Direction of the filter with minimum variance."),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL, "VAR"		, _TL("Minimum Variance"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Value(
		NULL, "NOISE"	, _TL("Noise Variance"),
		_TL(""),
		PARAMETER_TYPE_Double, 1
	); 


	//-----------------------------------------------------
	// Mirror the last 8  filter directions

	int		i, x, y;

	for(i=1; i<8; i++)
	{
		for(y=0; y<9; y++)
		{
			for(x=0; x<9; x++)
			{
				Filter_Directions[i + 8][y][x] = Filter_Directions[i][y][8 - x];
			}
		}
	}
}

//---------------------------------------------------------
CFilter_Multi_Dir_Lee::~CFilter_Multi_Dir_Lee(void)
{}

//---------------------------------------------------------
#define SQR(x)	((x)*(x))

//---------------------------------------------------------
bool CFilter_Multi_Dir_Lee::On_Execute(void)
{
	int		Best_Direction, Count;
	int		i, k, x, y, to_x, to_y;		
	
	double	Min_Std_Dev, Mean, Std_Dev,	Best_Mean, Noise;
	
	double	Std_Dev_Sum[16], b, result;
	
	CSG_Grid	*pInput				=	 Parameters("INPUT")->asGrid();
	CSG_Grid	*pResult			=	 Parameters("RESULT")->asGrid();
	CSG_Grid	*pDirections		=	 Parameters("DIR")->asGrid();
	CSG_Grid	*pVariance			=	 Parameters("VAR")->asGrid();
	
			Noise				=	 Parameters("NOISE")->asDouble();
	
	for (i = 0; i < 16; i++)
		Std_Dev_Sum[i]	=	0;
	
	for (y = 0; y < pInput->Get_NY()  && Set_Progress(y, pInput->Get_NY()); y++)
	{
		for (x = 0; x < pInput->Get_NX(); x++)
		{
			Best_Mean		=	-99999.99;
			Min_Std_Dev		=	 99999.99;
			
			Best_Direction	=	-1;
			
			for (k = 0; k < 16; k++)
			{
				Mean	=	Std_Dev = 0.0;
				Count	=	0;
				
				for (to_y = -4; to_y <  5; to_y++)
				{
					for (to_x = -4; to_x <  5; to_x++)
					{
						if (Filter_Directions[k][to_y + 4][to_x + 4])
						{
							if (x + to_x >= 0 && y + to_y >= 0 && x + to_x < Get_NX() && y + to_y < Get_NY())
							{
								Mean += pInput->asDouble(x + to_x, y + to_y);
								Count++;
								
							}
						}
					}
				}	

				Mean		/=	Count;
				
				for (to_y = -4; to_y <  5; to_y++)
				{
					for (to_x = -4; to_x <  5; to_x++)
					{
						{
							if (Filter_Directions[k][to_y + 4][to_x + 4])
							{
								if (x + to_x >= 0 && y + to_y >= 0 && x + to_x < Get_NX() && y + to_y < Get_NY())
									Std_Dev+= SQR(Mean - pInput->asDouble(x + to_x, y + to_y));
							}
						}
					}
				}
				
				Std_Dev	=109699939.0 * sqrt(Std_Dev) /(corr[k] * Count);
				
				Std_Dev_Sum[k] +=	Std_Dev;
				
				if (Std_Dev < Min_Std_Dev)
				{
					Min_Std_Dev	=	Std_Dev;
					Best_Mean	=	Mean;
					Best_Direction	=	k;
				}
			}
			
			Min_Std_Dev	+=	0.000001;
			
			b	=(Min_Std_Dev*Min_Std_Dev - Noise*Noise)/(Min_Std_Dev*Min_Std_Dev);
			if (b < 0)
				b= 0;
			
			result	=	pInput->asDouble(x, y)*b +(1 - b)*Best_Mean;
			
			pResult->Set_Value(x, y, result); 
			
			if (pDirections)
				pDirections->Set_Value(x, y, Best_Direction);
			if (pVariance)
				pVariance->Set_Value(x, y, Min_Std_Dev);
		}
	}		
	return true;
}

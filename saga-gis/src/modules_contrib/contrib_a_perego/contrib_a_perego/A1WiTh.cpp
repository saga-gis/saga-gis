/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                        Module:                        //
//                        A1WiTh                         //
//                                                       //
//                       for SAGA                        //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                      A1WiTh.cpp                       //
//                                                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                                                       //
//    by Alessandro Perego (Italy)                       //
//                                                       //
//    e-mail:     alper78@alice.it                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "A1WiTh.h"

//---------------------------------------------------------
CA1WiTh::CA1WiTh(void)
{
	// 1. Info...
	Set_Name(_TL("Average With Thereshold 1"));
	Set_Author(_TL("Alessandro Perego"));
	Set_Description(_TL("Average With Thereshold for Grids calculates average in X and Y distances unsing only the values that differ form central pixel less than a specified threshold. It's useful to remove noise whit a known maximum reducing the loss of informations"));

	// 2. Parameters...
	Parameters.Add_Grid(NULL, "INPUT", _TL("Input"), _TL("This must be your input data of type grid."), PARAMETER_INPUT);
	Parameters.Add_Grid(NULL, "RESULT", _TL("AWT Grid"), _TL("New grid filtered with the A1WiTh module"), PARAMETER_OUTPUT);
	Parameters.Add_Value(NULL, "RX", _TL("Radius X"), _TL(""), PARAMETER_TYPE_Int, 1, 1, true);
	Parameters.Add_Value(NULL, "RY", _TL("Radius Y"), _TL(""), PARAMETER_TYPE_Int, 1, 1, true);
	Parameters.Add_Value(NULL, "THRESH", _TL("Threshold"), _TL("The values in the specified radius is used in the average calculation only if its difference with the central value is lesser or equal to this threshold."), PARAMETER_TYPE_Double, 2.0);
}

//---------------------------------------------------------
CA1WiTh::~CA1WiTh(void)
{}

//---------------------------------------------------------
bool CA1WiTh::On_Execute(void)
{
	int  x, y, Rx, Ry, ax, ay, bx, by, ix, iy, n;
	double  Thresh, Diff, Sum, c, d;
	CSG_Grid  *pInput, *pResult;

	pInput = Parameters("INPUT")->asGrid();
	pResult	= Parameters("RESULT")->asGrid();
	Rx = Parameters("RX")->asInt();
	Ry = Parameters("RY")->asInt();
	Thresh = Parameters("THRESH")->asDouble();

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			Sum = 0.0;
			n = 0;
			//----------------------------------------------------
			ax = x - Rx;
			bx = x + Ry;

			ay = y - Ry;
			by = y + Ry;
			
			if( ax <  0 )		{	ax	= 0;			}	
			if( bx >= Get_NX() ){	bx	= Get_NX() - 1;	}
			if( ay <  0 )		{	ay	= 0;			}
			if( by >= Get_NY() ){	by	= Get_NY() - 1;	}

			//-----------------------------------------------------
			for(iy=ay; iy<=by; iy++)
			{
				for(ix=ax; ix<=bx; ix++)
				{
					c = pInput->asDouble(ix, iy);
					d = pInput->asDouble(x, y);
					Diff = c - d; 
					if( Diff < 0)
					{
						Diff = 0 - Diff;
					}
					if( Diff <= Thresh )
					{
						Sum += pInput->asDouble(ix, iy);
						n++;
					}
				}
			}

			//------------------------------------------------------
			if( n > 0 )
			{
				pResult->Set_Value(x, y, Sum / n);
			}

		}
	}

	return( true );
}

	
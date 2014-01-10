/**********************************************************
 * Version $Id: A3WiTh.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                        Module:                        //
//                        A3WiTh                         //
//                                                       //
//                       for SAGA                        //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                      A3WiTh.cpp                       //
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
#include "A3WiTh.h"

//---------------------------------------------------------
CA3WiTh::CA3WiTh(void)
{
	// 1. Info...
	Set_Name(_TL("Average With Thereshold 3"));
	Set_Author(_TL("Alessandro Perego"));
	Set_Description(_TL("Average 3 With Thereshold for Grids calculates average in X and Y distances unsing only the values that differ form central pixel less than a specified threshold. Each value has a weight which is inversely proportional to the distance (method 2)."));

	// 2. Parameters...
	Parameters.Add_Grid(NULL, "INPUT", _TL("Input"), _TL("This must be your input data of type grid."), PARAMETER_INPUT);
	Parameters.Add_Grid(NULL, "RESULT", _TL("AWT Grid"), _TL("New grid filtered with the A3WiTh module"), PARAMETER_OUTPUT);
	Parameters.Add_Value(NULL, "RX", _TL("Radius X"), _TL(""), PARAMETER_TYPE_Int, 1, 1, true);
	Parameters.Add_Value(NULL, "RY", _TL("Radius Y"), _TL(""), PARAMETER_TYPE_Int, 1, 1, true);
	Parameters.Add_Value(NULL, "THRESH", _TL("Threshold"), _TL("The values in the specified radius is used in the average calculation only if its difference with the central value is lesser or equal to this threshold."), PARAMETER_TYPE_Double, 2.0);
}

//---------------------------------------------------------
CA3WiTh::~CA3WiTh(void)
{}

//---------------------------------------------------------
bool CA3WiTh::On_Execute(void)
{
	int  x, y, Rx, Ry, ax, ay, bx, by, ix, iy, iv, iw, irv, irw, n;
	double  Thresh, Diff, Sum, c, d, i;
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
			
			if( (ax = x - Rx) <  0 )		{	ax	= 0;			}	
			if( (bx = x + Rx) >= Get_NX() )	{	bx	= Get_NX() - 1;	}
			if( (ay = y - Ry) <  0 )		{	ay	= 0;			}
			if( (by = y + Ry) >= Get_NY() )	{	by	= Get_NY() - 1;	}

			//-----------------------------------------------------
			for(iy=ay; iy<=by; iy++)
			{
				for(ix=ax; ix<=bx; ix++)
				{
					iv = x - ix;
					if( iv >= 0) { irv = Rx - iv + 1; }
					else { irv = Rx + iv + 1; }

					iw = y - iy;
					if( iw >= 0) { irw = Ry - iw + 1; }
					else { irw = Ry + iw + 1; }

					c = pInput->asDouble(ix, iy);
					d = pInput->asDouble(x, y);
					Diff = c - d; 
					if( Diff < 0)
					{
						Diff = 0 - Diff;
					}
					if( Diff <= Thresh )
					{
						i = pInput->asDouble(ix, iy);
						Sum += ( i * irv * irw );
						n += ( irv * irw );
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

	
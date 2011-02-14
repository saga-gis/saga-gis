/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                        Module:                        //
//                        AvWiMa                         //
//                                                       //
//                       for SAGA                        //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                      AvWiMa2.cpp                      //
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
#include "AvWiMa2.h"

//---------------------------------------------------------
CAvWiMa2::CAvWiMa2(void)
{
	// 1. Info...
	Set_Name(_TL("Average With Mask 2"));
	Set_Author(_TL("Alessandro Perego"));
	Set_Description(_TL("Average With Mask 2 calculates average for cells specified by a mask grid. However cell excluded by the mask grid are used in the average calculation for right pixels."));

	// 2. Parameters...
	Parameters.Add_Grid(NULL, "INPUT", _TL("Input"), _TL("This must be your input data of type grid."), PARAMETER_INPUT);
	Parameters.Add_Grid(NULL, "MASK", _TL("Mask Grid"), _TL("This grid indicates the cells you want calculate the average."), PARAMETER_INPUT);
	Parameters.Add_Grid(NULL, "RESULT", _TL("AWM2 Grid"), _TL("New grid filtered with the AvWiMa2 module"), PARAMETER_OUTPUT);
	Parameters.Add_Value(NULL, "V", _TL("Mask value"), _TL("Value of right cells in the Mask Grid"), PARAMETER_TYPE_Double, 1.0);
	Parameters.Add_Value(NULL, "RX", _TL("Radius X"), _TL(""), PARAMETER_TYPE_Int, 1, 1, true);
	Parameters.Add_Value(NULL, "RY", _TL("Radius Y"), _TL(""), PARAMETER_TYPE_Int, 1, 1, true);
}

//---------------------------------------------------------
CAvWiMa2::~CAvWiMa2(void)
{}

//---------------------------------------------------------
bool CAvWiMa2::On_Execute(void)
{
	int  x, y, Rx, Ry, ax, ay, bx, by, ix, iy, n;
	double  Sum, m, v;
	CSG_Grid  *pInput, *pMask, *pResult;

	pInput = Parameters("INPUT")->asGrid();
	pMask = Parameters("MASK")->asGrid();
	pResult	= Parameters("RESULT")->asGrid();
	v = Parameters("V")->asDouble();
	Rx = Parameters("RX")->asInt();
	Ry = Parameters("RY")->asInt();

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			m = pMask->asDouble(x, y);
			if( m == v)
			{
				Sum = 0.0;
				n = 0;
				
				ax = x - Rx;
				bx = x + Ry;

				ay = y - Ry;
				by = y + Ry;
				
				if( ax <  0 )		{	ax	= 0;			}	
				if( bx >= Get_NX() ){	bx	= Get_NX() - 1;	}
				if( ay <  0 )		{	ay	= 0;			}
				if( by >= Get_NY() ){	by	= Get_NY() - 1;	}

				for(iy=ay; iy<=by; iy++)
				{
					for(ix=ax; ix<=bx; ix++)
					{
						Sum += pInput->asDouble(ix, iy);
						n++;
					}
				}

				if( n > 0 )
				{
					pResult->Set_Value(x, y, Sum/n);
				}
			}
			else
			{
				pResult->Set_Value(x, y, pInput->asDouble(x, y));
			}
		}
	}
	return( true );
}

	
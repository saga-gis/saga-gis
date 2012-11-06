/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                        Module:                        //
//                      directional1                     //
//                                                       //
//                       for SAGA                        //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     directional1.cpp                  //
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
#include "directional1.h"

//---------------------------------------------------------
Cdirectional1::Cdirectional1(void)
{
	// 1. Info...
	Set_Name(_TL("Directional Average"));
	Set_Author(_TL("Alessandro Perego"));
	Set_Description(_TL("directional1 average for Grids"));

	// 2. Parameters...
	Parameters.Add_Grid(NULL, "INPUT", _TL("Input"), _TL("This must be your input data of type grid."), PARAMETER_INPUT);
	Parameters.Add_Grid(NULL, "RESULT", _TL("Output Grid"), _TL("New grid filtered with the directional1 module"), PARAMETER_OUTPUT);
	Parameters.Add_Value(NULL, "ANG", _TL("Angle (in degrees)"), _TL("0 is horizontal, 90 is vertical."), PARAMETER_TYPE_Double, 0.0);
	Parameters.Add_Value(NULL, "R1", _TL("Main Radius"), _TL(""), PARAMETER_TYPE_Double, 1);
	Parameters.Add_Value(NULL, "R2", _TL("Transversal radius"), _TL(""), PARAMETER_TYPE_Double, 0.5);
	
}

//---------------------------------------------------------
Cdirectional1::~Cdirectional1(void)
{}

//---------------------------------------------------------
bool Cdirectional1::On_Execute(void)
{
	int  x, y, r1, dxmax, dymax, ix, ax, ay, bx, by, iy, iv, iw, n;
	double  r2, ang, a, si, si2, co, co2, cosi, sico, r2co, r2si, Sum;
	CSG_Grid  *pInput, *pResult;

	pInput = Parameters("INPUT")->asGrid();
	pResult = Parameters("RESULT")->asGrid();
	ang = Parameters("ANG")->asDouble();
	r1 = Parameters("R1")->asInt();
	r2 = Parameters("R2")->asDouble();

	//-----------------------------------------------------
	a = ang * 6.283185307179586 / 360;
	si = sin(a);
	co = cos(a);
	si2 = si * si;
	co2 = co * co;
	dxmax = (int)((r1*sqrt(co2))+(r2*sqrt(si2)));
	dymax = (int)((r1*sqrt(si2))+(r2*sqrt(co2)));

	if(si2 >= co2)
	{
		cosi = co / si;
		r2si = r2 / sqrt(si2);
		for(y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			for(x=0; x<Get_NX(); x++)
			{
				Sum = 0.0;
				n = 0;
				//----------------------------------------------------
				if( (ax = x - dxmax) <  0 )		{	ax = 0;			}	
				if( (bx = x + dxmax) >= Get_NX() )	{	bx = Get_NX() - 1;	}
				if( (ay = y - dymax) <  0 )		{	ay = 0;			}
				if( (by = y + dymax) >= Get_NY() )	{	by = Get_NY() - 1;	}
				//-----------------------------------------------------
				for(ix=ax; ix<=bx; ix++)
				{
					iv = x - ix;
					for(iy=ay; iy<=by; iy++)
					{
						iw = y - iy;
						if( iv >= (iw*cosi - r2si) && iv <= (iw*cosi + r2si))
						{
							Sum += pInput->asDouble(ix, iy);
							n++;
						}
					}
				}
				if( n > 0 )
				{
					pResult->Set_Value(x, y, Sum/n);
				}
			}
		}
	}

	if(si2 < co2)
	{
		sico = si / co;
		r2co = r2 / sqrt(co2);
		for(y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			for(x=0; x<Get_NX(); x++)
			{
				Sum = 0.0;
				n = 0;
				//----------------------------------------------------
				if( (ax = x - dxmax) <  0 )		{	ax = 0;			}	
				if( (bx = x + dxmax) >= Get_NX() )	{	bx = Get_NX() - 1;	}
				if( (ay = y - dymax) <  0 )		{	ay = 0;			}
				if( (by = y + dymax) >= Get_NY() )	{	by = Get_NY() - 1;	}
				//-----------------------------------------------------
				for(iy=ay; iy<=by; iy++)
				{
					iw = y - iy;
					for(ix=ax; ix<=bx; ix++)
					{
						iv = x - ix;
						if( iw >= (iv*sico - r2co) && iw <= (iv*sico + r2co))
						{
							Sum += pInput->asDouble(ix, iy);
							n++;
						}
					}
				}
				if( n > 0 )
				{
					pResult->Set_Value(x, y, Sum/n);
				}
			}
		}
	}
	return( true );
}


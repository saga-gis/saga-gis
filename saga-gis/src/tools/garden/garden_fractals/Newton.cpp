/**********************************************************
 * Version $Id: Newton.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                                                       //
//                      $$modul$$                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                      Newton.cpp                       //
//                                                       //
//            Copyright (C) 2003 Your Name               //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    e-mail:     your@e-mail.abc                        //
//                                                       //
//    contact:    Your Name                              //
//                And Address                            //
//                                                       //
///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#pragma warning(disable: 4275)

#include "Newton.h"
#include <complex> 

using namespace std;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CNewton::CNewton(void)
{
	//-----------------------------------------------------
	// Place information about your tool here...

	Set_Name(_TL("Newton-Raphson"));

	Set_Author(_TL("Copyrights (c) 2003 by Andre Ringeler"));

	Set_Description	(_TW(
		"Newton-Raphson Fractals")
	);


	//-----------------------------------------------------
	Parameters.Add_Grid_Output(
		""	, "RESULT"	, _TL("Result"),
		_TL("")
	);

	Parameters.Add_Grid_Output(
		""	, "SHADE"	, _TL("Shade"),
		_TL("")
	);

	Parameters.Add_Int   ("", "NX"    , _TL("Width (Cells)" ), _TL(""), 400, 1, true);
	Parameters.Add_Int   ("", "NY"    , _TL("Height (Cells)"), _TL(""), 400, 1, true);

	Parameters.Add_Range ("", "XRANGE", _TL("X-Range"       ), _TL(""), -2.0, 2.0);
	Parameters.Add_Range ("", "YRANGE", _TL("Y-Range"       ), _TL(""), -2.0, 2.0);

	Parameters.Add_Choice("", "METHOD", _TL("Equation"      ), _TL(""), "z^3-1|z^4-1|z^5-1|z^6-1|z^10 + 0.2 i * z^5 - 1|", 0);

	//-----------------------------------------------------
	Parameters.Add_Double("", "MAXITER", _TL("Max Iterations"), _TL("Maximum Iterations"), 50);
}

//---------------------------------------------------------
CNewton::~CNewton(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CNewton::On_Execute(void)
{
	int		x, y;
		
	x				= Parameters("NX")->asInt();
	xMin			= Parameters("XRANGE")->asRange()->Get_LoVal();
	dx	 =	xMax	= Parameters("XRANGE")->asRange()->Get_HiVal();
	dx				= (dx - xMin) / (x - 1.0);

	y				= Parameters("NY")->asInt();
	yMin			= Parameters("YRANGE")->asRange()->Get_LoVal();
	dy	=	yMax	= Parameters("YRANGE")->asRange()->Get_HiVal();
	dy				= (dy - yMin) / (y - 1.0);

	method			= Parameters("METHOD")->asInt();

	pResult			= SG_Create_Grid(SG_DATATYPE_Double, x, y, dx, xMin, yMin);
	pResult->Set_Name(_TL("Newton [A]"));
	Parameters("RESULT")->Set_Value(pResult);

	pShade			= SG_Create_Grid(SG_DATATYPE_Double, x, y, dx, xMin, yMin);
	pShade->Set_Name(_TL("Newton [B]"));
	Parameters("SHADE")	->Set_Value(pShade);

	Get_System()->Assign(pShade->Get_System());

	//-----------------------------------------------------
	iMax		= Parameters("MAXITER")->asInt();

	if( doNewton() )
	{
		DataObject_Update(pResult, true);
		DataObject_Update(pShade , true);

		return( true );
	}

	return( false );
}

bool CNewton::doNewton()
{
	int		x, y, n;
	double	i, r;
	complex<double> z , old;


	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			r	= (double) x / Get_NX()*(xMax-xMin) + xMin;
			i	= (double) y / Get_NY()*(yMax-yMin) + yMin;

// MinGW ERROR:
// Newton.cpp:181: error: no matching function for call to `std::complex<double>::real(double&)'
// error: candidates are: double std::complex<double>::real() const
#ifdef _SAGA_VC
			z.real(r);
			z.imag(i);
#endif
			
			for (n = 0 ;n< iMax; n++)
			{
				old = z;
				switch( method )
				{
					case 0:	default:

									
						z = z - ( z * z * z- 1.0 ) / (3.0  * z *z);
						break;


					case 1:	
									
						z = z - ( z * z * z * z- 1.0 ) / (4.0 * z * z *z);
						break;

					case 2:
						z = z - (z * z * z * z * z- 1.0 ) / (5.0 * z * z * z *z);
						break;

					case 3:
						//							"z^10 + 0.2 i * z^5 - 1", 0);
						{
							complex<double> z5;
							
							z5= z*z; z5= z5 * z5; 

							z5= z5*z;
							
							z =z - (z5*z - 1.0 ) / ( 6.0 * z5);
						
						break;
						}

					case 4:
						{

							complex<double> z9 , z4, wert(0,0.2);
							z4 = z*z, z4= z4*z4;
							z9 = z4*z4*z;

							z = z -(z9*z + wert * z4*z -1.0 ) / (10.0 * z9 + 5.0 * wert * z4);
						}

				}

				if ( abs(old - z)< 0.05 ) break;
			
			}
			
			if(n < iMax )
			pResult->Set_Value(x, y, z.real());
			else
			pResult->Set_NoData(x,y);

			pShade->Set_Value(x,y, exp (-(double) ((n - iMax/2)*(n - iMax/2))/(double)(iMax*iMax))  );
		}
	}

	//-----------------------------------------------------
	
	 return( true );
}



bool CNewton::On_Execute_Position(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode)
{
	//-----------------------------------------------------
	if(	Mode != TOOL_INTERACTIVE_LDOWN && Mode != TOOL_INTERACTIVE_RDOWN )
	{
		return( false );
	}

	//-----------------------------------------------------
	int ok = true;

	int x= Get_xGrid(); 	int y = Get_yGrid();
	
	double xpos, ypos;
	double deltax, deltay;

	xpos = (double ) x / Get_NX()*(xMax-xMin) + xMin;

	ypos = (double ) y / Get_NY()*(yMax-yMin) + yMin;

	deltax = (xMax-xMin);

	deltay = (yMax-yMin);

	if(Mode == TOOL_INTERACTIVE_LDOWN)
	{
		deltax/=4.0;
		deltay/=4.0;

		xMin = xpos - deltax;
		xMax = xpos + deltax;

		yMin = ypos - deltay;
		yMax = ypos + deltay;

	}

	if(Mode == TOOL_INTERACTIVE_RDOWN)
	{
		
		xMin = xpos - deltax;
		xMax = xpos + deltax;

		yMin = ypos - deltay;
		yMax = ypos + deltay;

	}

	doNewton();

	DataObject_Update(pResult,pResult->Get_Min(), pResult->Get_Max());
	DataObject_Update(pShade ,pShade ->Get_Min(), pShade ->Get_Max());

	return true;
}

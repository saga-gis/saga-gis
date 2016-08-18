/**********************************************************
 * Version $Id: destriping2.cpp 1514 2012-11-06 09:47:38Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                        Tool:                        //
//                      destriping                       //
//                                                       //
//                       for SAGA                        //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    destriping2.cpp                    //
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
#include "destriping2.h"

//---------------------------------------------------------
Cdestriping2::Cdestriping2(void)
{
	// 1. Info...
	Set_Name(_TL("Destriping with Mask"));
	Set_Author(_TL("Alessandro Perego"));
	Set_Description(_TW(
		"Destriping filter removes straight parallel stripes in raster data. "
		"It uses two low-pass filters elongated in the stripes direction; "
		"the first one is 1 pixel unit wide while the second one is wide as the striping wavelength. "
		"Their difference is the striping error which is removed from the original data to obtain the destriped DEM. "
		"This method is equivalent to that proposed by Oimoen (2000). "
		"With destriping 2 you can choose a range of value (min-max) from the input grid "
		"and a range of value (Mask min - Mask max) from a mask grid to select the target cells. "
		"\n"
		"\nReferences:\n"
		"- Oimoen, M.J. (2000): An Effective Filter For Removal Of Production Artifacts. "
		"In U.S. Geological Survey 7.5-Minute Digital Elevation Models. "
		"Proceedings of the Fourteenth International Conference on Applied Geologic Remote Sensing, "
		"6-8 November, Las Vegas, NV.\n"
		"\n"
		"- Peregro, A. (2009): SRTM DEM destriping with SAGA GIS: consequences on drainage network extraction. "
		"<a target=\"_blank\" href=\"http://www.webalice.it/alper78/saga_mod/destriping/destriping.html\">online</a>.\n"
	));

	// 2. Parameters...
	Parameters.Add_Grid(NULL, "INPUT", _TL("Input"), _TL("This must be your input data of type grid."), PARAMETER_INPUT);
	Parameters.Add_Grid(NULL, "MASK", _TL("Mask Grid"), _TL("This grid is used to select cells."), PARAMETER_INPUT);
	Parameters.Add_Grid(NULL, "RESULT3", _TL("Destriped Grid"), _TL("New grid filtered with the destriping2 tool"), PARAMETER_OUTPUT);
	Parameters.Add_Grid(NULL, "RESULT1", _TL("Low-pass 1"), _TL("Step 1: low-pass of stripe"), PARAMETER_OUTPUT);
	Parameters.Add_Grid(NULL, "RESULT2", _TL("Low-pass 2"), _TL("Step 2: low-pass between stripe and its surruondings"), PARAMETER_OUTPUT);
	Parameters.Add_Value(NULL, "ANG", _TL("Angle (in degrees)"), _TL("0 is horizontal, 90 is vertical."), PARAMETER_TYPE_Double, 0.0);
	Parameters.Add_Value(NULL, "R", _TL("Radius"), _TL(""), PARAMETER_TYPE_Double, 20);
	Parameters.Add_Value(NULL, "D", _TL("Stripes distance"), _TL(""), PARAMETER_TYPE_Double, 2);
	Parameters.Add_Value(NULL, "MIN", _TL("Min"), _TL(""), PARAMETER_TYPE_Double, -10);
	Parameters.Add_Value(NULL, "MAX", _TL("Max"), _TL(""), PARAMETER_TYPE_Double, 10);
	Parameters.Add_Value(NULL, "MMIN", _TL("Mask Min"), _TL(""), PARAMETER_TYPE_Double, -10000);
	Parameters.Add_Value(NULL, "MMAX", _TL("Mask Max"), _TL(""), PARAMETER_TYPE_Double, 10000);


	
}

//---------------------------------------------------------
Cdestriping2::~Cdestriping2(void)
{}

//---------------------------------------------------------
bool Cdestriping2::On_Execute(void)
{
	int  x, y, r, dxmax, dymax, ix, ax, ay, bx, by, iy, iv, iw, n1, n2;
	double  z, mz, iz, imz, d, ang, a, si, si2, co, co2, Sum1, Sum2, min, max, mmin, mmax;
	CSG_Grid  *pInput, *pMask, *pRes1, *pRes2, *pRes3;

	pInput = Parameters("INPUT")->asGrid();
	pMask = Parameters("MASK")->asGrid();
	pRes1 = Parameters("RESULT1")->asGrid();
	pRes2 = Parameters("RESULT2")->asGrid();
	pRes3 = Parameters("RESULT3")->asGrid();
	ang = Parameters("ANG")->asDouble();
	r = Parameters("R")->asInt();
	d = Parameters("D")->asDouble();
	min = Parameters("MIN")->asDouble();
	max = Parameters("MAX")->asDouble();
	mmin = Parameters("MMIN")->asDouble();
	mmax = Parameters("MMAX")->asDouble();

	//-----------------------------------------------------
	a = ang * 6.283185307179586 / 360;
	si = sin(a);
	co = cos(a);
	si2 = si * si;
	co2 = co * co;
	dxmax = (int)((r*sqrt(co2))+(d/2*sqrt(si2)));
	dymax = (int)((r*sqrt(si2))+(d/2*sqrt(co2)));

	if(si2 >= co2)
	{
		double  cosi, si05, dsi, iwcosi, ivmin1, ivmax1, ivmin2, ivmax2;
		cosi = co / si;
		si05 = sqrt( (0.5/si)*(0.5/si) );
		dsi = sqrt( (d/2/si)*(d/2/si) );
		for(y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			for(x=0; x<Get_NX(); x++)
			{
				z=pInput->asDouble(x, y);
				mz=pMask->asDouble(x, y);
				if( z>=min && z<=max && mz>=mmin && mz<=mmax)
				{
					Sum1 = 0.0;
					n1 = 0;
					Sum2 = 0.0;
					n2 = 0;
					//----------------------------------------------------
					if( (ax = x - dxmax) <  0 )		{	ax = 0;			}	
					if( (bx = x + dxmax) >= Get_NX() )	{	bx = Get_NX() - 1;	}
					if( (ay = y - dymax) <  0 )		{	ay = 0;			}
					if( (by = y + dymax) >= Get_NY() )	{	by = Get_NY() - 1;	}
					//-----------------------------------------------------
					for(iy=ay; iy<=by; iy++)
					{
						iw = y - iy;
						iwcosi = iw * cosi;
						ivmin1 = iwcosi - si05;
						ivmax1 = iwcosi + si05;
						ivmin2 = iwcosi - dsi;
						ivmax2 = iwcosi + dsi;
						for(ix=ax; ix<=bx; ix++)
						{
							iz = pInput->asDouble(ix, iy);
							imz=pMask->asDouble(ix, iy);
							if( iz>=min && iz<=max && imz>=mmin && imz<=mmax)
							{
								iv = x - ix;
								if( iv >= ivmin1 && iv <= ivmax1)
								{
									Sum1 += iz;
									n1++;
								}
								if( iv >= ivmin2 && iv <= ivmax2)
								{
									Sum2 += iz;
									n2++;
								}
							}
						}
					}
					if( n1 > 0 && n2 > 0 )
					{
						pRes1->Set_Value(x, y, Sum1/n1);
						pRes2->Set_Value(x, y, Sum2/n2);
						pRes3->Set_Value(x, y, (Sum1/n1)-(Sum2/n2)+z);
					}
				}
				else
				{
					pRes1->Set_Value(x, y, z);
					pRes2->Set_Value(x, y, z);
					pRes3->Set_Value(x, y, z);
				}
			}
		}
	}

	if(si2 < co2)
	{
		double  sico, co05, dco, ivsico, iwmin1, iwmax1, iwmin2, iwmax2;
		sico = si / co;
		co05 = sqrt( (0.5/co)*(0.5/co) );
		dco = sqrt( (d/2/co)*(d/2/co) );
		for(y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			for(x=0; x<Get_NX(); x++)
			{
				z=pInput->asDouble(x, y);
				mz=pMask->asDouble(x, y);
				if( z>=min && z<=max && mz>=mmin && mz<=mmax)
				{
					Sum1 = 0.0;
					n1 = 0;
					Sum2 = 0.0;
					n2 = 0;
					//----------------------------------------------------
					if( (ax = x - dxmax) <  0 )		{	ax = 0;			}	
					if( (bx = x + dxmax) >= Get_NX() )	{	bx = Get_NX() - 1;	}
					if( (ay = y - dymax) <  0 )		{	ay = 0;			}
					if( (by = y + dymax) >= Get_NY() )	{	by = Get_NY() - 1;	}
					//-----------------------------------------------------
					for(ix=ax; ix<=bx; ix++)
					{
						iv = x - ix;
						ivsico = iv * sico;
						iwmin1 = ivsico - co05;
						iwmax1 = ivsico + co05;
						iwmin2 = ivsico - dco;
						iwmax2 = ivsico + dco;
						for(iy=ay; iy<=by; iy++)
						{
							iz = pInput->asDouble(ix, iy);
							imz=pMask->asDouble(ix, iy);
							if( iz>=min && iz<=max && imz>=mmin && imz<=mmax)
							{
								iw = y - iy;
								if( iw >= iwmin1 && iw <= iwmax1)
								{
									Sum1 += iz;
									n1++;
								}
								if( iw >= iwmin2 && iw <= iwmax2)
								{
									Sum2 += iz;
									n2++;
								}
							}
						}
					}
					if( n1 > 0 && n2 > 0 )
					{
						pRes1->Set_Value(x, y, Sum1/n1);
						pRes2->Set_Value(x, y, Sum2/n2);
						pRes3->Set_Value(x, y, (Sum2/n2)-(Sum1/n1)+pInput->asDouble(x, y));
					}
				}
				else
				{
					pRes1->Set_Value(x, y, z);
					pRes2->Set_Value(x, y, z);
					pRes3->Set_Value(x, y, z);
				}
			}
		}
	}
	return( true );
}


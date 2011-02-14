
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                    ta_morphometry                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    wind_effect.cpp                    //
//                                                       //
//                 Copyright (C) 2008 by                 //
//                    Andre Ringeler                     //
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
//    e-mail:     aringeler@saga-gis.org                 //
//                                                       //
//    contact:    Andre Ringeler                         //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Bundesstr. 55                          //
//                20146 Hamburg                          //
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
#include "wind_effect.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWind_Effect::CWind_Effect(void)
{
	Set_Name		(_TL("Wind Effect (Windward/Leeward Index)"));

	Set_Author		(SG_T("J.Boehner, A.Ringeler (c) 2008"));

	Set_Description	(_TW(
		""
	));

	Parameters.Add_Grid(	NULL, "ELEVATION"	, _TL("Elevation")			, _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grid(	NULL, "RESULT"		, _TL("Wind Effect")		, _TL(""), PARAMETER_OUTPUT);

	Parameters.Add_Value(	NULL, "DIRECT"		, _TL("Wind Direction")		, _TL(""), PARAMETER_TYPE_Double, 135.0);
	Parameters.Add_Value(	NULL, "MAXDIST"		, _TL("Max. Distance [km]")	, _TL(""), PARAMETER_TYPE_Double, 300.0);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWind_Effect::On_Execute(void)
{
	int		x, y;

	double	Direction_RAD, Wr, dLuvB, dLee, dx, dy, LUV, LEE;

	//-----------------------------------------------------
	pDTM			= Parameters("ELEVATION")	->asGrid();
	pLeeLuv			= Parameters("RESULT")		->asGrid();

	Direction_RAD	= Parameters("DIRECT")		->asDouble() * M_DEG_TO_RAD;
	max_Distance	= Parameters("MAXDIST")		->asDouble() * 1000.0;


	//-----------------------------------------------------
	dx		= sin(Direction_RAD);
	dy		= cos(Direction_RAD);

	if( fabs(dx) > fabs(dy) )
	{
		dy	/= fabs(dx);
		dx	= dx < 0 ? -1 : 1;
	}
	else
	{
		dx	/= fabs(dy);
		dy	= dy < 0 ? -1 : 1;
	}

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( pDTM->is_NoData(x,y) )
			{
				pLeeLuv->Set_NoData(x,y);
			}
			else
			{
				Wr	=	Get_LUV(x,y, dx, dy);
				Get_LEE_LUV(x,y,-dx,-dy,dLuvB,dLee);

				Wr += dLuvB;

				if(Wr > 0.0)
					LUV = 1 + log(1+Wr);
				else
					LUV = 1.0 / (1 + log(1 - Wr));

				if (dLee>0.0)
					LEE = sqrt ( 1 + log (1 + dLee));
				else
					LEE = 1.0/ sqrt ( 1 + log (1 - dLee));

				pLeeLuv->Set_Value(x,y,sqrt(sqrt( LUV*LEE)));
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CWind_Effect::Get_LUV(int x, int y, double dx, double dy)
{
	double	ix, iy, Dist, SumDhi, Sum, dxy,  SumWeight ,z;

	dxy			= Get_Cellsize() * sqrt(dx*dx + dy*dy);
	Dist		= 0.0;
	SumWeight	= 0.0;
	Sum			= 0.0;
	SumDhi      = 0.0;

	z = pDTM->asDouble(x,y);

	for(ix=x+0.5, iy=y+0.5; ; )
	{
		Dist	+= dxy;

		ix		+= dx;
		iy		+= dy;

		x		= (int)ix;
		y		= (int)iy;

		if( !is_InGrid(x,y) || Dist > max_Distance )
		{
			break;
		}
		else if( !pDTM->is_NoData(x,y) )
		{
			SumDhi += 1/Dist;  
			
			Sum			+= 1/Dist * atan2 ( z-pDTM->asDouble(x,y) ,sqrt(Dist));
		}
	}

	if( SumDhi > 0 )
	{
		Sum		/= SumDhi;
	}

	return( Sum );
}

//---------------------------------------------------------
void CWind_Effect::Get_LEE_LUV(int x, int y, double dx, double dy, double &SumA, double &SumB)
{
	double	ix, iy, Dist, dxy, z,
			SumWeightA,
			SumWeightB;

	dxy			= Get_Cellsize() * sqrt(dx*dx + dy*dy);
	Dist		= 0.0;
	SumWeightA	= 0.0;
	SumA		= 0.0;
	SumWeightB	= 0.0;
	SumB		= 0.0;

					z = pDTM->asDouble(x,y);

	for(ix=x+0.5, iy=y+0.5; ; )
	{
		Dist	+= dxy;

		ix		+= dx;
		iy		+= dy;

		x		= (int)ix;
		y		= (int)iy;
		
		if( !is_InGrid(x,y) || Dist > max_Distance )
		{
			break;
		}
	
		else if( !pDTM->is_NoData(x,y) )
		{

			SumWeightA  += 1 / Dist;  
			
			SumA    	+= 1 / Dist * atan2 ( z - pDTM->asDouble(x,y) ,sqrt(Dist));

			
			SumWeightB	+= 1 / log(Dist);  
			
			SumB		+= 1 / log(Dist) * atan2 ( z - pDTM->asDouble(x,y) ,sqrt(Dist));
		}
	}

	if( SumWeightA > 0 )
	{
		SumA		/= SumWeightA;
	}

	if( SumWeightB > 0 )
	{
		SumB		/= SumWeightB;
	}
}
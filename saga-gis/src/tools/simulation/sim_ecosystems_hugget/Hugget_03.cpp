
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                 sim_ecosystems_hugget                 //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    Hugget_03.cpp                      //
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
#include "Hugget_03.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CHugget_03::CHugget_03(void)
{
	Set_Name		(_TL("Spatially Distributed Simulation of Soil Nitrogen Dynamics"));

	Set_Author		("O.Conrad (c) 2003");

	Set_Description	(_TW(
		"A simple spatially distributed simulation model of soil nitrogen dynamics. "
	));

	Add_Reference("Hugget, R.J.", "1993",
		"Modelling the Human Impact on Nature",
		"Oxford University Press."
	);

	//-----------------------------------------------------
	Parameters.Add_Grid  ("", "DEM"      , _TL("Elevation"               ), _TL(""          ), PARAMETER_INPUT );
	Parameters.Add_Grid  ("", "NSTORE"   , _TL("Soil Nitrogen"           ), _TL(""          ), PARAMETER_OUTPUT);

	Parameters.Add_Double("", "TIME_SPAN", _TL("Time Span"               ), _TL("[yrs]"     ),  100. , 0., true);
	Parameters.Add_Double("", "TIME_STEP", _TL("Time Interval"           ), _TL("[yrs]"     ),    1. , 0., true);

	Parameters.Add_Double("", "NINIT"    , _TL("Initial Nitrogen Content"), _TL("[kg/ha]"   ), 5000. , 0., true);
	Parameters.Add_Double("", "NRAIN"    , _TL("Nitrogen in Rainfall"    ), _TL("[kg/ha/yr]"),   16. , 0., true);

	Parameters.Add_Bool  ("", "UPDATE"	 , _TL("Update View"             ), _TL(""          ), true);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CHugget_03::On_Execute(void)
{
	CSG_Grid &N = *Parameters("NSTORE")->asGrid();

	N.Assign(Parameters("NINIT")->asDouble());

	DataObject_Set_Colors(&N, 11, SG_COLORS_YELLOW_GREEN);

	CSG_Grid *pDEM = Parameters("DEM")->asGrid(), Slopes[8]; Init_Slopes(pDEM, Slopes);

	//-----------------------------------------------------
	double Time_Span = Parameters("TIME_SPAN")->asDouble();
	double dTime     = Parameters("TIME_STEP")->asDouble();
	double Nrain     = Parameters("NRAIN"    )->asDouble();
	bool   bUpdate   = Parameters("UPDATE"   )->asBool  ();

	//-----------------------------------------------------
	for(double Time=0.; Time<=Time_Span && Set_Progress(Time, Time_Span); Time+=dTime)
	{
		Process_Set_Text(CSG_String::Format("%s [%s]: %f (%f)", _TL("Time"), _TL("years"), Time, Time_Span));

		SG_UI_ProgressAndMsg_Lock(true);

		if( bUpdate )
		{
			DataObject_Update(&N, true);
		}

		Set_Nitrogen(N, Nrain, Slopes, dTime);

		SG_UI_ProgressAndMsg_Lock(false);
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CHugget_03::Init_Slopes(CSG_Grid *pDEM, CSG_Grid Slopes[8])
{
	for(int i=0; i<8; i++)
	{
		Slopes[i].Create(pDEM, SG_DATATYPE_Float);
	}

	//-----------------------------------------------------
	#pragma omp parallel for
	for(int y=0; y<Get_NY(); y++) for(int x=0; x<Get_NX(); x++)
	{
		if( pDEM->is_NoData(x, y) )
		{
			for(int i=0; i<8; i++)
			{
				Slopes[i].Set_NoData(x, y);
			}
		}
		else
		{
			double z = pDEM->asDouble(x, y);

			for(int i=0; i<8; i++)
			{
				int ix = Get_xTo(i, x), iy = Get_yTo(i, y);

				if( pDEM->is_InGrid(ix, iy) )
				{
					Slopes[i].Set_Value(x, y, (pDEM->asDouble(ix, iy) - z) / Get_Length(i));
				}
				else
				{
					Slopes[i].Set_Value(x, y, 0.);
				}
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CHugget_03::Set_Nitrogen(CSG_Grid &N, double Nrain, CSG_Grid Slopes[8], double dTime)
{
	CSG_Grid N0(N);

	#pragma omp parallel for
	for(int y=0; y<Get_NY(); y++) for(int x=0; x<Get_NX(); x++)
	{
		if( !Slopes[0].is_NoData(x, y) )
		{
			double dN = 0.;

			for(int i=0; i<8; i++)
			{
				double Slope = Slopes[i].asDouble(x, y);

				if( Slope < 0. )
				{
					dN	+= Slope * N0.asDouble(x, y);
				}
				else if( Slope > 0. )
				{
					dN	+= Slope * N0.asDouble(Get_xTo(i, x), Get_yTo(i, y));
				}
			}

			dN = N0.asDouble(x, y) + (dN + Nrain) * dTime;

			if( dN < 0. )
			{
				dN = Nrain * dTime;
			}

			N.Set_Value(x, y, dN);
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

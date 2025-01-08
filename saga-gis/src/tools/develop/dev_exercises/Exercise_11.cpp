
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       Lectures                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   Exercise_11.cpp                     //
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
#include "Exercise_11.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CExercise_11::CExercise_11(void)
{
	Set_Name		(_TL("11: Dynamic Simulation - Soil Nitrogen Dynamics"));

	Set_Author		("O.Conrad (c) 2006");

	Set_Description	(_TW(
		"Spatially Distributed Simulation of Soil Nitrogen Dynamics."
	));

	Add_Reference("Conrad, O.", "2007",
		"SAGA - Entwurf, Funktionsumfang und Anwendung eines Systems für Automatisierte Geowissenschaftliche Analysen",
		"ediss.uni-goettingen.de.",
		SG_T("https://ediss.uni-goettingen.de/handle/11858/00-1735-0000-0006-B26C-6"), SG_T("online")
	);

	Add_Reference("O. Conrad, B. Bechtel, M. Bock, H. Dietrich, E. Fischer, L. Gerlitz, J. Wehberg, V. Wichmann, and J. Böhner", "2015",
		"System for Automated Geoscientific Analyses (SAGA) v. 2.1.4",
		"Geoscientific Model Development, 8, 1991-2007.",
		SG_T("https://doi.org/10.5194/gmd-8-1991-2015"), SG_T("doi:10.5194/gmd-8-1991-2015")
	);

	Add_Reference("Hugget, R.J.", "1993",
		"Modelling the Human Impact on Nature",
		"Oxford University Press."
	);

	//-----------------------------------------------------
	Parameters.Add_Grid(
		"", "DEM"      , _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		"", "NSTORE"   , _TL("Soil Nitrogen"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	//-----------------------------------------------------
	Parameters.Add_Double(
		"", "TIME_SPAN", _TL("Time Span [a]"),
		_TL(""),
		100., 0., true
	);

	Parameters.Add_Double(
		"", "TIME_STEP", _TL("Time Interval [a]"),
		_TL(""),
		0.1, 0., true
	);

	Parameters.Add_Bool(
		"", "UPDATE"   , _TL("Update View"),
		_TL(""),
		true
	);

	//-----------------------------------------------------
	Parameters.Add_Double(
		"", "NINIT"    , _TL("Initial Nitrogen Content [kg/ha]"),
		_TL(""),
		5000., 0., true
	);

	Parameters.Add_Double(
		"", "NRAIN"    , _TL("Nitrogen in Rainfall [kg/ha/a]"),
		_TL(""),
		16., 0., true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CExercise_11::On_Execute(void)
{
	Initialize();

	double Years = Parameters("TIME_SPAN")->asDouble();
	double dTime = Parameters("TIME_STEP")->asDouble();

	double NRain = Parameters("NRAIN")->asDouble();

	bool bUpdate = Parameters("UPDATE")->asBool();

	//-----------------------------------------------------
	for(int i=0, n=(int)(Years/dTime); i<=n && Set_Progress(i, n); i++)
	{
		Process_Set_Text("Time [a]: %f (%f)", dTime * i, Years);

		if( bUpdate )
		{
			DataObject_Update(m_pN, 1);
		}

		Next_Step(NRain, dTime);
	}

	//-----------------------------------------------------
	Finalize();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CExercise_11::Initialize(void)
{
	m_pN = Parameters("NSTORE")->asGrid();
	m_pN->Assign(Parameters("NINIT")->asDouble());
	DataObject_Set_Colors(m_pN, 11, SG_COLORS_YELLOW_GREEN);

	CSG_Grid *pDEM = Parameters("DEM")->asGrid();

	m_N.Create(pDEM, SG_DATATYPE_Float); // temporary

	for(int i=0; i<=8; i++)
	{
		m_dz[i].Create(pDEM, SG_DATATYPE_Float);
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY(); y++) for(int x=0; x<Get_NX(); x++)
	{
		if( pDEM->is_NoData(x, y) )
		{
			m_dz[8].Set_NoData(x, y);
		}
		else
		{
			double dz[8], dzSum = 0., z = pDEM->asDouble(x, y);

			for(int i=0; i<8; i++)
			{
				int ix = Get_xTo(i, x), iy = Get_yTo(i, y);

				if( pDEM->is_InGrid(ix, iy) && (dz[i] = (z - pDEM->asDouble(ix, iy)) / Get_Length(i)) > 0. )
				{
					dzSum += dz[i];
				}
				else
				{
					dz[i] = 0.;
				}
			}

			for(int i=0; i<8; i++)
			{
				m_dz[i].Set_Value(x, y, dzSum > 0. ? dz[i] / dzSum : 0.);
			}

			m_dz[8].Set_Value(x, y, dzSum);
		}
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CExercise_11::Finalize(void)
{
	m_N.Destroy();

	for(int i=0; i<=8; i++)
	{
		m_dz[i].Destroy();
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CExercise_11::Next_Step(double NRain, double dTime)
{
	m_N.Assign(NRain * dTime);

	for(int y=0; y<Get_NY(); y++) for(int x=0; x<Get_NX(); x++)
	{
		if( m_dz[8].is_NoData(x, y) )
		{
			m_N.Set_NoData(x, y);

			continue;
		}

		double dzSum = m_dz[8].asDouble(x, y);

		if( dzSum > 0. )
		{
			double N = m_pN->asDouble(x, y);

			double dN = N * dzSum * dTime;

			if( dN > N )
			{
				dN = N;
			}

			for(int i=0; i<8; i++)
			{
				double dz = m_dz[i].asDouble(x, y);

				if( dz > 0. )
				{
					m_N.Add_Value(Get_xTo(i, x), Get_yTo(i, y), dz * dN);
				}
			}

			m_N.Add_Value(x, y, N - dN);
		}
	}

	m_pN->Assign(&m_N);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

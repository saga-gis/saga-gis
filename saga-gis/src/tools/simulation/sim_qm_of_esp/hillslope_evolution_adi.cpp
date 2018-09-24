/**********************************************************
 * Version $Id: hillslope_evolution_adi.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      qm_of_esp                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               hillslope_evolution_adi.cpp             //
//                                                       //
//                 Copyright (C) 2013 by                 //
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
//                University of Hamburg                  //
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
#include "hillslope_evolution_adi.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool tridag(const CSG_Vector &a, const CSG_Vector &b, const CSG_Vector &c, const CSG_Vector &r, CSG_Vector &u)
{
	int			i, n = a.Get_N();
	double		beta;
	CSG_Vector	gamma(n);

	if( n < 2 || n != b.Get_N() || n != c.Get_N() || n != r.Get_N() || b[0] == 0.0 )
	{
		return( false );
	}

	u.Create(n);

	u[0]	= r[0] / (beta = b[0]);

	for(i=1; i<n; i++)
	{
		gamma[i]	= c[i - 1] / beta;
		beta		= b[i] - a[i] * gamma[i];

		if( beta == 0.0 )
		{
			return( false );
		}

		u[i]	= (r[i] - a[i] * u[i - 1]) / beta;
	}

	for(i=n-2; i>=0; i--)
	{
		u[i]	-= gamma[i + 1] * u[i + 1];
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CHillslope_Evolution_ADI::CHillslope_Evolution_ADI(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Diffusive Hillslope Evolution (ADI)"));

	Set_Author		("O.Conrad (c) 2013");

	Set_Description	(_TW(
		"Simulation of diffusive hillslope evolution using an Alternating-Direction-Implicit (ADI) method."
	));

	Add_Reference("Pelletier, J.D.",
		"2008", "Quantitative Modeling of Earth Surface Processes",
		"Cambridge, 295p."
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"DEM"		, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"CHANNELS"	, _TL("Channel Mask"),
		_TL("use a zero value for hillslopes, any other value for channel cells."),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid("",
		"MODEL"		, _TL("Modelled Elevation"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid("",
		"DIFF"		, _TL("Elevation Difference"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Bool("DIFF",
		"UPDATE"	, _TL("Update"),
		_TL(""),
		true
	);

	Parameters.Add_Double("",
		"KAPPA"		, _TL("Diffusivity [m2 / kyr]"),
		_TL(""),
		10.0, 0.0, true
	);

	Parameters.Add_Double("",
		"DURATION"	, _TL("Simulation Time [kyr]"),
		_TL(""),
		10000.0, 0.0, true
	);

	Parameters.Add_Choice("",
		"TIMESTEP"	, _TL("Time Step"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("user defined"),
			_TL("automatically")
		), 0
	);

	Parameters.Add_Double("TIMESTEP",
		"DTIME"		, _TL("Time Step [kyr]"),
		_TL(""),
		1000.0, 0.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CHillslope_Evolution_ADI::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("TIMESTEP") )
	{
		pParameters->Set_Enabled("DTIME", pParameter->asInt() == 0);
	}

	if( pParameter->Cmp_Identifier("DIFF") )
	{
		pParameters->Set_Enabled("UPDATE", pParameter->asPointer() != NULL);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CHillslope_Evolution_ADI::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Grid	DEM(*Get_System()), Channels(*Get_System(), SG_DATATYPE_Byte);

	m_pDEM_Old	= &DEM;

	m_pDEM		= Parameters("MODEL"   )->asGrid();
	m_pChannels	= Parameters("CHANNELS")->asGrid();

	m_pDEM->Assign(Parameters("DEM")->asGrid());

	DataObject_Set_Colors(Parameters("DIFF")->asGrid(), 10, SG_COLORS_RED_GREY_BLUE, true);

	//-----------------------------------------------------
	double	k, dTime, nTime;

	k		= Parameters("KAPPA"   )->asDouble();
	nTime	= Parameters("DURATION")->asDouble();

	if( Parameters("TIMESTEP")->asInt() == 0 )
	{
		dTime	= Parameters("DTIME")->asDouble();
	}
	else
	{
		dTime	= 0.5 * Get_Cellarea() / (2.0 * k);
	}

	if( dTime > nTime )
	{
		Message_Fmt("\n%s: %s [%f]", _TL("Warning"), _TL("Time step exceeds duration"), dTime);

		dTime	= nTime;
	}

	Message_Fmt("\n%s: %f", _TL("Time Step"), dTime);
	Message_Fmt("\n%s: %d", _TL("Steps"), (int)(nTime / dTime));

	//-----------------------------------------------------
	for(double iTime=dTime; iTime<=nTime && Set_Progress(iTime, nTime); iTime+=dTime)
	{
		Process_Set_Text(CSG_String::Format("%s: %.2f [%.2f]", _TL("Simulation Time"), iTime, nTime));

		SG_UI_Progress_Lock(true);

		Set_Diffusion(dTime * k / Get_Cellarea());

		Set_Difference();

		SG_UI_Progress_Lock(false);
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CHillslope_Evolution_ADI::Set_Difference(void)
{
	CSG_Grid	*pDiff	= Parameters("DIFF")->asGrid();

	if( pDiff )
	{
		CSG_Grid	*pDEM	= Parameters("DEM")->asGrid();

		#pragma omp parallel for
		for(int i=0; i<Get_NCells(); i++)
		{
			if( m_pDEM->is_NoData(i) )
			{
				pDiff->Set_NoData(i);
			}
			else
			{
				pDiff->Set_Value(i, m_pDEM->asDouble(i) - pDEM->asDouble(i));
			}
		}

		if( Parameters("UPDATE")->asBool() )
		{
			DataObject_Update(pDiff, SG_UI_DATAOBJECT_SHOW);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline bool CHillslope_Evolution_ADI::is_Channel(int x, int y)
{
	return( m_pChannels ? m_pChannels->asDouble(x, y) != 0.0 : false );
}

//---------------------------------------------------------
inline double CHillslope_Evolution_ADI::Get_Elevation(int x, int y)
{
	if( x < 0 ) x = 0; else if( x >= Get_NX() ) x = Get_NX() - 1;
	if( y < 0 ) y = 0; else if( y >= Get_NY() ) y = Get_NY() - 1;

	return( m_pDEM->asDouble(x, y) );
}

//---------------------------------------------------------
void CHillslope_Evolution_ADI::Set_Diffusion(double dFactor)
{
	int		x, y;

	for(int i=0; i<5 && Process_Get_Okay(); i++)
	{
		m_pDEM_Old->Assign(m_pDEM);

		#pragma omp parallel for private(x, y)
		for(x=0; x<Get_NX(); x++)
		{
			CSG_Vector	a(Get_NY()), b(Get_NY()), c(Get_NY()), u(Get_NY()), r(Get_NY());

			for(y=0; y<Get_NY(); y++)
			{
				if( y == 0 )
				{
					a[y]	= is_Channel(x, y) ? 0 : -dFactor;
					b[y]	= 1;
					c[y]	= 0;
					r[y]	= m_pDEM_Old->asDouble(x, y);
				}
				else if( y == Get_NY() - 1 )
				{
					a[y]	= 0;
					b[y]	= 1;
					c[y]	= is_Channel(x, y) ? 0 : -dFactor;
					r[y]	= m_pDEM_Old->asDouble(x, y);
				}
				else if( is_Channel(x, y) )
				{
					b[y]	= 1;
					a[y]	= 0;
					c[y]	= 0;
					r[y]	= m_pDEM_Old->asDouble(x, y);
				}
				else // if( !is_Channel(x, y) )
				{
					a[y]	= -dFactor;
					c[y]	= -dFactor;
					b[y]	= 4 * dFactor + 1;
					r[y]	= m_pDEM_Old->asDouble(x, y) + dFactor * (Get_Elevation(x - 1, y) + Get_Elevation(x + 1, y));
				}
			}

			tridag(a, b, c, r, u);

			for(y=0; y<Get_NY(); y++)
			{
				m_pDEM->Set_Value(x, y, u[y]);
			}
		}

		//-------------------------------------------------
		m_pDEM_Old->Assign(m_pDEM);

		#pragma omp parallel for private(x, y)
		for(y=0; y<Get_NY(); y++)
		{
			CSG_Vector	a(Get_NX()), b(Get_NX()), c(Get_NX()), u(Get_NX()), r(Get_NX());

			for(x=0; x<Get_NX(); x++)
			{
				if( x == 0 )
				{
					a[x]	= is_Channel(x, y) ? 0 : -dFactor;
					b[x]	= 1;
					c[x]	= 0;
					r[x]	= m_pDEM_Old->asDouble(x, y);
				}
				else if( x == Get_NX() - 1 )
				{
					a[x]	= 0;
					b[x]	= 1;
					c[x]	= is_Channel(x, y) ? 0 : -dFactor;
					r[x]	= m_pDEM_Old->asDouble(x, y);
				}
				else if( is_Channel(x, y) )
				{
					a[x]	= 0;
					b[x]	= 1;
					c[x]	= 0;
					r[x]	= m_pDEM_Old->asDouble(x, y);
				}
				else // if( !is_Channel(x, y) )
				{
					a[x]	= -dFactor;
					c[x]	= -dFactor;
					b[x]	= 4 * dFactor + 1;
					r[x]	= m_pDEM_Old->asDouble(x, y) + dFactor * (Get_Elevation(x, y - 1) + Get_Elevation(x, y + 1));
				}
			}

			tridag(a, b, c, r, u);

			for(x=0; x<Get_NX(); x++)
			{
				m_pDEM->Set_Value(x, y, u[x]);
			}
		}
	}
}

/*/---------------------------------------------------------
void CHillslope_Evolution_ADI::Set_Diffusion(double dFactor)
{
	int		x, y;

	for(int i=0; i<5 && Process_Get_Okay(); i++)
	{
		m_pDEM_Old->Assign(m_pDEM);

		#pragma omp parallel for private(x, y)
		for(x=0; x<Get_NX(); x++)
		{
			CSG_Vector	a(Get_NY()), b(Get_NY()), c(Get_NY()), u(Get_NY()), r(Get_NY());

			for(y=0; y<Get_NY(); y++)
			{
				if( !is_Channel(x, y) )
				{
					a[y]	= -dFactor;
					c[y]	= -dFactor;
					b[y]	= 4 * dFactor + 1;
					r[y]	= m_pDEM_Old->asDouble(x, y) + dFactor * (Get_Elevation(x - 1, y) + Get_Elevation(x + 1, y));
				}
				else
				{
					b[y]	= 1;
					a[y]	= 0;
					c[y]	= 0;
					r[y]	= m_pDEM_Old->asDouble(x, y);
				}

				if( y == 0 )
				{
					b[y]	= 1;
					c[y]	= 0;
					r[y]	= m_pDEM_Old->asDouble(x, y);
				}
				else if( y == Get_NY() - 1 )
				{
					b[y]	= 1;
					a[y]	= 0;
					r[y]	= m_pDEM_Old->asDouble(x, y);
				}
			}

			tridag(a, b, c, r, u);

			for(y=0; y<Get_NY(); y++)
			{
				m_pDEM->Set_Value(x, y, u[y]);
			}
		}

		//-------------------------------------------------
		m_pDEM_Old->Assign(m_pDEM);

		#pragma omp parallel for private(x, y)
		for(y=0; y<Get_NY(); y++)
		{
			CSG_Vector	a(Get_NX()), b(Get_NX()), c(Get_NX()), u(Get_NX()), r(Get_NX());

			for(x=0; x<Get_NX(); x++)
			{
				if( !is_Channel(x, y) )
				{
					a[x]	= -dFactor;
					c[x]	= -dFactor;
					b[x]	= 4 * dFactor + 1;
					r[x]	= m_pDEM_Old->asDouble(x, y) + dFactor * (Get_Elevation(x, y - 1) + Get_Elevation(x, y + 1));
				}
				else
				{
					b[x]	= 1;
					a[x]	= 0;
					c[x]	= 0;
					r[x]	= m_pDEM_Old->asDouble(x, y);
				}

				if( x == 0 )
				{
					b[x]	= 1;
					c[x]	= 0;
					r[x]	= m_pDEM_Old->asDouble(x, y);
				}
				else if( x == Get_NX() - 1 )
				{
					b[x]	= 1;
					a[x]	= 0;
					r[x]	= m_pDEM_Old->asDouble(x, y);
				}
			}

			tridag(a, b, c, r, u);

			for(x=0; x<Get_NX(); x++)
			{
				m_pDEM->Set_Value(x, y, u[x]);
			}
		}
	}
}/**/


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

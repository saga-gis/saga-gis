/**********************************************************
 * Version $Id: hillslope_evolution_ftcs.cpp 911 2011-02-14 16:38:15Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      qm_of_esp                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               hillslope_evolution_ftcs.cpp            //
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
#include "hillslope_evolution_ftcs.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CHillslope_Evolution_FTCS::CHillslope_Evolution_FTCS(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name	(_TL("Diffusive Hillslope Evolution (FTCS)"));

	Set_Author	("O.Conrad (c) 2013");

	Set_Description	(_TW(
		"Simulation of diffusive hillslope evolution using a Forward-Time-Centered-Space (FTCS) method.\n"
		"\nReferences:\n"
		"Pelletier, J.D. (2008): Quantitative Modeling of Earth Surface Processes. Cambridge, 295p.\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "DEM"			, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "MODEL"		, _TL("Modelled Elevation"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	pNode	= Parameters.Add_Grid(
		NULL	, "DIFF"		, _TL("Elevation Difference"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Value(
		pNode	, "UPDATE"		, _TL("Update"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);

	Parameters.Add_Value(
		NULL	, "KAPPA"		, _TL("Diffusivity [m2 / kyr]"),
		_TL(""),
		PARAMETER_TYPE_Double, 1.0, 0.0, true
	);

	Parameters.Add_Value(
		NULL	, "DURATION"	, _TL("Simulation Time [kyr]"),
		_TL(""),
		PARAMETER_TYPE_Double, 100.0, 0.0, true
	);

	pNode	= Parameters.Add_Choice(
		NULL	, "TIMESTEP"	, _TL("Time Step"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("user defined"),
			_TL("automatically")
		), 1
	);

	Parameters.Add_Value(
		pNode	, "DTIME"		, _TL("Time Step [kyr]"),
		_TL(""),
		PARAMETER_TYPE_Double, 10.0, 0.0, true
	);

	Parameters.Add_Choice(
		NULL	, "NEIGHBOURS"	, _TL("Neighbourhood"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("Neumann"),
			_TL("Moore")
		), 1
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CHillslope_Evolution_FTCS::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "TIMESTEP") )
	{
		pParameters->Get_Parameter("DTIME")->Set_Enabled(pParameter->asInt() == 0);
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "DIFF") )
	{
		pParameters->Get_Parameter("UPDATE")->Set_Enabled(pParameter->asGrid() != NULL);
	}

	return( 1 );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CHillslope_Evolution_FTCS::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Grid	DEM(*Get_System());

	m_pDEM_Old	= &DEM;

	m_pDEM		= Parameters("MODEL")->asGrid();

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

		if( Parameters("NEIGHBOURS")->asInt() == 1 )
		{
			dTime	/= sqrt(2.0);
		}
	}

	if( dTime > nTime )
	{
		Message_Add(CSG_String::Format(SG_T("\n%s: %s [%f]"), _TL("Warning"), _TL("Time step exceeds duration"), dTime), false);

		dTime	= nTime;
	}

	Message_Add(CSG_String::Format(SG_T("\n%s: %f"), _TL("Time Step"), dTime), false);
	Message_Add(CSG_String::Format(SG_T("\n%s: %d"), _TL("Steps"), (int)(nTime / dTime)), false);

	//-----------------------------------------------------
	for(double iTime=dTime; iTime<=nTime && Set_Progress(iTime, nTime); iTime+=dTime)
	{
		Process_Set_Text(CSG_String::Format(SG_T("%s: %.2f [%.2f]"), _TL("Simulation Time"), iTime, nTime));

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
void CHillslope_Evolution_FTCS::Set_Difference(void)
{
	CSG_Grid	*pDiff	= Parameters("DIFF")->asGrid();

	if( pDiff )
	{
		CSG_Grid	*pDEM	= Parameters("DEM")->asGrid();

		#pragma omp parallel for
		for(int i=0; i<Get_NCells(); i++)
		{
			if( pDEM->is_NoData(i) )
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
void CHillslope_Evolution_FTCS::Set_Diffusion(double dFactor)
{
	int	iStep	= Parameters("NEIGHBOURS")->asInt() == 1 ? 1 : 2;

	m_pDEM_Old->Assign(m_pDEM);

	#pragma omp parallel for
	for(int y=0; y<Get_NY(); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !m_pDEM_Old->is_NoData(x, y) )
			{
				double	z	= m_pDEM_Old->asDouble(x, y);
				double	dz	= 0.0;

				for(int i=0; i<8; i+=iStep)
				{
					int	ix	= Get_xTo(i, x);
					int	iy	= Get_yTo(i, y);

					if( m_pDEM_Old->is_InGrid(ix, iy) )
					{
						dz	+= (m_pDEM_Old->asDouble(ix, iy) - z) / Get_UnitLength(i);
					}
				}

				m_pDEM->Add_Value(x, y, dFactor * dz);
			}
		}
	}
}

/*/---------------------------------------------------------
void CHillslope_Evolution_FTCS::Set_Diffusion(double dFactor)
{
	m_pDEM_Old->Assign(m_pDEM);

	#pragma omp parallel for
	for(int y=0; y<Get_NY(); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !m_pDEM_Old->is_NoData(x, y) )
			{
				double	z, dz;

				z	= m_pDEM_Old->asDouble(x, y);
				dz	= -4.0 * z;

				for(int i=0; i<8; i+=2)
				{
					int	ix	= Get_xTo(i, x);
					int	iy	= Get_yTo(i, y);

					dz	+= m_pDEM_Old->is_InGrid(ix, iy) ? m_pDEM_Old->asDouble(ix, iy) : z;
				}

				m_pDEM->Add_Value(x, y, dFactor * dz);
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

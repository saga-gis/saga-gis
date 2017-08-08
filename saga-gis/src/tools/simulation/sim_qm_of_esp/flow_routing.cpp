/**********************************************************
 * Version $Id: flow_routing.cpp 911 2011-02-14 16:38:15Z reklov_w $
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
//                   flow_routing.cpp                    //
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
#include "flow_routing.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFlow_Routing::CFlow_Routing(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Flow Accumulation (QM of ESP)"));

	Set_Author		("O.Conrad (c) 2013");

	Set_Description	(_TW(
		"Calculation of flow accumulation, aka upslope contributing area, "
		"with the multiple flow direction method after Freeman (1991)."
	));

	Add_Reference("Freeman, G.T.",
		"1991", "Calculating catchment area with divergent flow based on a rectangular grid",
		"Computers & Geosciences, 17, pp.413-422."
	);

	Add_Reference("Pelletier, J.D.",
		"2008", "Quantitative Modeling of Earth Surface Processes",
		"Cambridge, 295p."
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"DEM"		, _TL("DEM"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"FLOW"		, _TL("Contributing Area"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice("",
		"PREPROC"	, _TL("Preprocessing"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|",
			_TL("none"),
			_TL("fill sinks temporarily"),
			_TL("fill sinks permanently")
		), 1
	);

	Parameters.Add_Double("",
		"DZFILL"	, _TL("Fill Increment"),
		_TL(""),
		0.01, 0.0000001, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CFlow_Routing::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "PREPROC") )
	{
		pParameters->Set_Enabled("DZFILL", pParameter->asInt() != 0);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFlow_Routing::On_Execute(void)
{
	m_pDEM	= Parameters("DEM" )->asGrid();
	m_pFlow	= Parameters("FLOW")->asGrid();

	return( Set_Flow(Parameters("PREPROC")->asInt(), Parameters("DZFILL")->asDouble()) );
}

//---------------------------------------------------------
bool CFlow_Routing::Set_Flow(CSG_Grid *pDEM, CSG_Grid *pFlow, int Preprocess, double dzFill)
{
	if( is_Executing() || !pDEM || !pFlow || !pDEM->is_Compatible(pFlow) || !Get_System()->Assign(pDEM->Get_System()) )
	{
		return( false );
	}

	m_pDEM	= pDEM;
	m_pFlow	= pFlow;

	return( Set_Flow(Preprocess, dzFill) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFlow_Routing::Set_Flow(int Preprocess, double dzFill)
{
	CSG_Grid	DEM;

	switch( Preprocess )
	{
	case 1:
		DEM.Create(*m_pDEM);	m_pDEM	= &DEM;

	case 2:
		{
			CFill_Sinks	Preprocessor;

			Preprocessor.Fill_Sinks(m_pDEM, NULL, dzFill);
		}
		break;
	}

	//-----------------------------------------------------
	int	i, x, y;

	m_pFlow->Assign(0.0);

	m_pDEM->Get_Sorted(0, x, y);

	Process_Set_Text(_TL("Calculating Contributing Area"));

	for(i=0; i<Get_NCells() && Set_Progress(i, Get_NCells()); i++)
	{
		if( m_pDEM->Get_Sorted(i, x, y) )
		{
			Set_Flow(x, y);
		}
	}

	//-----------------------------------------------------
	if( Preprocess == 2 )
	{
		DataObject_Update(m_pDEM);
	}

	return( true );
}

//---------------------------------------------------------
void CFlow_Routing::Set_Flow(int x, int y)
{
	if( !m_pDEM->is_NoData(x, y) )
	{
		m_pFlow->Add_Value(x, y, Get_Cellarea());

		int		i;
		double	z, dz[8], dzSum;

		for(i=0, dzSum=0.0, z=m_pDEM->asDouble(x, y); i<8; i++)
		{
			int	ix	= Get_xTo(i, x);
			int	iy	= Get_yTo(i, y);

			if( m_pDEM->is_InGrid(ix, iy) && (dz[i] = z - m_pDEM->asDouble(ix, iy)) > 0.0 )
			{
				dzSum	+= (dz[i] = pow(dz[i] / Get_Length(i), 1.1));
			}
			else
			{
				dz[i]	= 0.0;
			}
		}

		if( dzSum > 0.0 )
		{
			for(i=0, z=m_pFlow->asDouble(x, y)/dzSum; i<8; i++)
			{
				if( dz[i] > 0.0 )
				{
					m_pFlow->Add_Value(Get_xTo(i, x), Get_yTo(i, y), z * dz[i]);
				}
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

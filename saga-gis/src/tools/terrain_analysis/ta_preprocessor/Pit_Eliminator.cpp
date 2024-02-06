
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    ta_preprocessor                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  Pit_Eliminator.cpp                   //
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
#include "Pit_Eliminator.h"
#include "Pit_Router.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPit_Eliminator::CPit_Eliminator(void)
{
	Set_Name		(_TL("Sink Removal"));

	Set_Author		("O. Conrad (c) 2001");

	Set_Description	(_TW(
		"Sink removal. "
	));

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"DEM"        , _TL("DEM"),
		_TL("Digital Elevation Model that has to be processed"),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"SINKROUTE"  , _TL("Sink Route"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid("",
		"DEM_PREPROC", _TL("Preprocessed DEM"),
		_TL("Preprocessed DEM. If this is not set changes will be stored in the original DEM grid."),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Choice("",
		"METHOD"     , _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("Deepen Drainage Routes"),
			_TL("Fill Sinks")
		), 1
	);

	Parameters.Add_Bool("",
		"THRESHOLD"  , _TL("Threshold"),
		_TL(""),
		false
	);

	Parameters.Add_Double("",
		"THRSHEIGHT" , _TL("Threshold Height"),
		_TL("The maximum depth to which a sink is considered for removal."),
		100., 0., true
	);

	Parameters.Add_Double("",
		"EPSILON"    , _TL("Epsilon"),
		_TL("Difference in elevation used to create non-flat gradient conserving filled/carved surfaces."),
		1.e-3, M_FLT_EPSILON, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CPit_Eliminator::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("THRESHOLD") )
	{	
		pParameters->Set_Enabled("THRSHEIGHT", pParameter->asBool());
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPit_Eliminator::On_Execute(void)
{
	bool bResult = true;

	m_pDEM = Parameters("DEM_PREPROC")->asGrid();

	if( m_pDEM == NULL )
	{
		m_pDEM = Parameters("DEM")->asGrid();
	}
	else if( m_pDEM != Parameters("DEM")->asGrid() )
	{
		m_pDEM->Assign(Parameters("DEM")->asGrid());

		m_pDEM->Fmt_Name("%s [%s]", Parameters("DEM")->asGrid()->Get_Name(), _TL("no sinks"));
	}

	//-----------------------------------------------------
	int nPits = 1; CSG_Grid Route; m_pRoute = Parameters("SINKROUTE")->asGrid();

	if( !m_pRoute )
	{
		CPit_Router	Router; m_pRoute = &Route; Route.Create(m_pDEM);

		nPits = Router.Get_Routes(m_pDEM, m_pRoute, Parameters("THRESHOLD")->asBool() ? Parameters("THRSHEIGHT")->asDouble() : -1.0);
	}

	//-----------------------------------------------------
	if( nPits > 0 )
	{
		Process_Set_Text(_TL("Initializing direction matrix..."));

		Create_goRoute();

		m_Epsilon = Parameters("EPSILON")->asDouble();

		switch( Parameters("METHOD")->asInt() )
		{
		case  0:
			Process_Set_Text(_TL("I'm diggin'..."));
			bResult	= Dig_Channels();
			break;

		default:
			Process_Set_Text(_TL("I'm fillin'..."));
			bResult	= Fill_Sinks();
			break;
		}

		delete( m_goRoute );
	}

	//-----------------------------------------------------
	Lock_Destroy();

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CPit_Eliminator::Create_goRoute(void)
{
	m_goRoute = SG_Create_Grid(m_pRoute);

	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !is_InGrid(x,y) )
			{
				m_goRoute->Set_NoData(x, y);
			}
			else if( m_pRoute->asChar(x, y) > 0 )
			{
				m_goRoute->Set_Value(x, y, m_pRoute->asChar(x, y) % 8 );
			}
			else
			{
				m_goRoute->Set_Value(x, y, m_pDEM->Get_Gradient_NeighborDir(x, y));
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPit_Eliminator::Dig_Channels(void)
{
	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			bool bPit = true; double z = m_pDEM->asDouble(x, y);

			for(int i=0; bPit && i<8; i++)
			{
				int ix = Get_xTo(i, x), iy = Get_yTo(i, y);

				if( !is_InGrid(ix, iy) || z > m_pDEM->asDouble(ix, iy) )
				{
					bPit = false;
				}
			}

			if( bPit )
			{
				Dig_Channel(x, y);
			}
		}
	}

	return( is_Progress() );
}

//---------------------------------------------------------
void CPit_Eliminator::Dig_Channel(int x, int y)
{
	double z = m_pDEM->asDouble(x, y);

	for(;;)
	{
		z -= m_Epsilon; int goDir = m_goRoute->asChar(x, y);

		if( goDir < 0 )
		{
			return;
		}

		x = Get_xTo(goDir, x);
		y = Get_yTo(goDir, y);

		if( !is_InGrid(x, y) || z > m_pDEM->asDouble(x, y) )
		{
			return;
		}

		m_pDEM->Set_Value(x, y, z);
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPit_Eliminator::Fill_Sinks(void)
{
	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			Fill_Check(x, y);
		}
	}

	return( is_Progress() );
}

//---------------------------------------------------------
void CPit_Eliminator::Fill_Check(int x, int y)
{
	double z = m_pDEM->asDouble	(x, y);

	int i = m_goRoute->asChar(x, y); int ix = Get_xTo(i, x), iy = Get_yTo(i, y);

	if( !is_InGrid(ix, iy) || z > m_pDEM->asDouble(ix, iy) )
	{
		bool bOutlet = false; int j;

		for(i=0, j=4; !bOutlet && i<8; i++, j=(j+1)%8)
		{
			ix = Get_xTo(i, x); iy = Get_yTo(i, y);

			if( is_InGrid(ix, iy) && m_goRoute->asChar(ix, iy) == j && z > m_pDEM->asDouble(ix, iy) )
			{
				bOutlet	= true;
			}
		}

		if( bOutlet )
		{
			Lock_Create(); Lock_Set(x, y);

			for(i=0, j=4; i<8; i++, j=(j+1)%8)
			{
				ix = Get_xTo(i, x); iy = Get_yTo(i, y);

				Fill_Sink(ix, iy, j, z);
			}
		}
	}
}

//---------------------------------------------------------
void CPit_Eliminator::Fill_Sink(int x, int y, int j, double z)
{
	if( is_InGrid(x, y) && !is_Locked(x, y) && m_goRoute->asChar(x, y) == j )
	{
		Lock_Set(x, y);

		z += m_Epsilon * Get_UnitLength(j);

		if( m_pDEM->asDouble(x, y) < z )
		{
			m_pDEM->Set_Value(x, y, z);

			for(int i=0; i<8; i++)
			{
				Fill_Sink(Get_xTo(i, x), Get_yTo(i, y), (i + 4) % 8, z);
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

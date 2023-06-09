
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     ta_hydrology                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  TerrainFlooding.cpp                  //
//                                                       //
//                 Copyright (C) 2022 by                 //
//                    Volker Wichmann                    //
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
//    e-mail:     wichmann@laserdata                     //
//                                                       //
//    contact:    Volker Wichmann                        //
//                LASERDATA GmbH                         //
//                Management and analysis of             //
//                laserscanning data                     //
//                Innsbruck, Austria                     //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "TerrainFlooding.h"

#include <queue>
#include <set>


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTerrainFloodingBase::Create(CSG_Parameters &Parameters, bool bInteractive)
{
	Parameters.Add_Grid("",
		"DEM", _TL("DEM"), 
		_TL("The digital elevation model to flood."),
		PARAMETER_INPUT
	);

	if( bInteractive )
	{
		Parameters.Add_Double("",
			"WATER_LEVEL", _TL("Water Level"),
			_TL("The local water level, given either relative to the DEM or as absolute height [map units]."),
			0.5
		);
	}
	else
	{
		Parameters.Add_Shapes("",
			"SEEDS", _TL("Seed Points"),
			_TL("The point(s) from where to start the flooding."),
			PARAMETER_INPUT, SHAPE_TYPE_Point
		);

		Parameters.Add_Table_Field_or_Const("SEEDS",
			"WATER_LEVEL", _TL("Water Level"),
			_TL("The attribute field with the local water level, given either relative to the DEM or as absolute height [map units]."),
			0.5
		);
	}

	Parameters.Add_Choice("WATER_LEVEL",
		"LEVEL_REFERENCE", _TL("Water Level Reference"),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("level is given relative to DEM"),
			_TL("level is given as absolute water height")
		), 0
	);

	Parameters.Add_Bool("WATER_LEVEL",
		"CONSTANT_LEVEL", _TL("Constant Water Level"),
		_TL("Model the water level as constant. Otherwise the specified level is always taken as relative to the currently processed cell. This option is only available if the water level reference is relative to the DEM."),
		true
	);

	Parameters.Add_Grid("",
		"WATER_BODY", _TL("Water Body"), 
		_TL("The extent of the water body, labeled with local water depth [map units]."),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid("",
		"DEM_FLOODED", _TL("Flooded DEM"), 
		_TL("The flooded digital elevation model."),
		PARAMETER_OUTPUT_OPTIONAL
	);

	return( true );
}


//---------------------------------------------------------
bool CTerrainFloodingBase::Initialize(const CSG_Parameters &Parameters)
{
	m_pDEM				= Parameters("DEM")->asGrid();
	m_pWaterBody		= Parameters("WATER_BODY")->asGrid();
	m_pFlooded			= Parameters("DEM_FLOODED")->asGrid();
	m_dWaterLevel		= Parameters("WATER_LEVEL")->asDouble();
	m_iLevelReference	= Parameters("LEVEL_REFERENCE")->asInt();
	m_bConstantLevel	= Parameters("CONSTANT_LEVEL")->asBool();

	m_pWaterBody->Assign_NoData();
	m_pWaterBody->Set_Max_Samples(m_pWaterBody->Get_NCells());

	if (m_pFlooded == NULL)
	{
		m_pFlooded = SG_Create_Grid(m_pDEM);
	}

	m_pFlooded->Assign(m_pDEM);

	return( true );
}


//---------------------------------------------------------
bool CTerrainFloodingBase::Finalize(const CSG_Parameters &Parameters)
{
	if( Parameters("DEM_FLOODED")->asGrid() == NULL )
	{
		delete( m_pFlooded );
	}

	return( true );
}


//---------------------------------------------------------
bool CTerrainFloodingBase::Set_Flooding(double xWorld, double yWorld, double dWaterLevel, bool bShow, bool bReset)
{
	int x = m_pDEM->Get_System().Get_xWorld_to_Grid(xWorld);
	int y = m_pDEM->Get_System().Get_yWorld_to_Grid(yWorld);

	if( !m_pDEM->is_InGrid(x, y, true) )
	{
		return( false );
	}

	//-----------------------------------------------------
	if( bReset )
	{
		m_pWaterBody->Assign_NoData();
		m_pFlooded  ->Assign(m_pDEM);
	}

	double dWaterHeight = dWaterLevel;

	if( m_iLevelReference == 0 )
	{
		dWaterHeight += m_pDEM->asDouble(x, y);
	}

	//-----------------------------------------------------
	if( dWaterHeight > m_pFlooded->asDouble(x, y) )
	{
		m_pWaterBody->Set_Value(x, y, dWaterHeight - m_pDEM->asDouble(x, y));
		m_pFlooded  ->Set_Value(x, y, dWaterHeight);

		std::queue<sLong> qFIFO;

		sLong n = m_pDEM->Get_System().Get_IndexFromRowCol(x, y);

		qFIFO.push(n);

		while( qFIFO.size() > 0 && SG_UI_Process_Get_Okay() )
		{
			n = qFIFO.front();

			m_pDEM->Get_System().Get_RowColFromIndex(x, y, n);

			if( m_iLevelReference == 0 && !m_bConstantLevel )
			{
				dWaterHeight = m_pDEM->asDouble(x, y) + dWaterLevel;

				m_pWaterBody->Set_Value(x, y, dWaterLevel);
				m_pFlooded  ->Set_Value(x, y, dWaterHeight);
			}

			for(int i=0, ix, iy; i<8; i++)
			{
				if( m_pDEM->Get_System().Get_Neighbor_Pos(i, x, y, ix, iy) && !m_pFlooded->is_NoData(ix, iy) )
				{
					if( m_iLevelReference == 0 && !m_bConstantLevel && !m_pWaterBody->is_NoData(ix, iy) )
					{
						continue;
					}

					if( m_pFlooded->asDouble(ix, iy) < dWaterHeight )
					{
						m_pWaterBody->Set_Value(ix, iy, dWaterHeight - m_pDEM->asDouble(ix, iy));
						m_pFlooded->Set_Value(ix, iy, dWaterHeight);

						n = m_pDEM->Get_System().Get_IndexFromRowCol(ix, iy);

						qFIFO.push(n);
					}
				}
			}

			qFIFO.pop();
		}
	}

	//-----------------------------------------------------
	int iUpdate = bShow ? SG_UI_DATAOBJECT_SHOW_MAP_ACTIVE : SG_UI_DATAOBJECT_UPDATE;
		
	CSG_Parameters Parameters;
	Parameters.Add_Range("", "METRIC_ZRANGE", "", "", m_pWaterBody->Get_Min(), m_pWaterBody->Get_Max());
	SG_UI_DataObject_Update(m_pWaterBody, iUpdate, &Parameters);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//-----------------------------------------------------------
CTerrainFlooding::CTerrainFlooding(void)
{
	Set_Name		(_TL("Terrain Flooding"));

	Set_Author		("V. Wichmann (c) 2022");

	Parameters.Set_Description(_TW(
		"The tool allows one to flood a digital elevation model for a given water level. "
		"The water level can be provided either as absolute height or relative "
		"to the DEM.\n"
		"If the water level is given relative to the DEM, the tool can model "
		"a constant water level starting from the seed cell, or a water level that is "
		"always relative to the currently processed cell. This way also inclined water "
		"surfaces, e.g. along a river, can be modelled. Note that this usually requires "
		"rather small relative water levels in order to prevent the flooding of the "
		"complete DEM; the functionality is most suited to generate a segment (connected "
		"component) of a river bed.\n\n")
	);

	Create(Parameters, false);

}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTerrainFlooding::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("LEVEL_REFERENCE") )
	{
		pParameters->Set_Enabled("CONSTANT_LEVEL", pParameter->asInt() == 0);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTerrainFlooding::On_Execute(void)									
{
	Initialize(Parameters);

	CSG_Shapes *pPoints = Parameters("SEEDS")->asShapes();
	int			iField	= Parameters("WATER_LEVEL")->asInt();

	//-----------------------------------------------------
	for(sLong iPoint=0; iPoint<pPoints->Get_Count() && Process_Get_Okay(); iPoint++)
	{
		Process_Set_Text("%s %lld ...", _TL("Processing seed"), 1 + iPoint);

		CSG_Shape &Point = *pPoints->Get_Shape(iPoint);

		Set_Flooding(Point.Get_Point().x, Point.Get_Point().y, iField < 0 ? m_dWaterLevel : Point.asDouble(iField), false);
	}

	//-----------------------------------------------------
	Finalize(Parameters);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//-----------------------------------------------------------
CTerrainFloodingInteractive::CTerrainFloodingInteractive(void)
{
	Set_Name		(_TL("Terrain Flooding"));

	Set_Author		("V. Wichmann (c) 2022");
	
	Parameters.Set_Description(_TW(
		"The tool allows one to flood a digital elevation model for a given water level. "
		"The water level can be provided either as absolute height or relative "
		"to the DEM.\n"
		"If the water level is given relative to the DEM, the tool can model "
		"a constant water level starting from the seed cell, or a water level that is "
		"always relative to the currently processed cell. This way also inclined water "
		"surfaces, e.g. along a river, can be modelled. Note that this usually requires "
		"rather small relative water levels in order to prevent the flooding of the "
		"complete DEM; the functionality is most suited to generate a segment (connected "
		"component) of a river bed.\n\n")
	);

	Create(Parameters, true);	

	Parameters.Add_Bool("WATER_LEVEL", "CUMULATIVE", _TL("Cumulative"),
		_TL("Do not reset the water body grid before water level is calculated for the next point clicked."),
		false
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTerrainFloodingInteractive::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("LEVEL_REFERENCE") )
	{
		pParameters->Set_Enabled("CONSTANT_LEVEL", pParameter->asInt() == 0);
	}

	return( CSG_Tool_Grid_Interactive::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTerrainFloodingInteractive::On_Execute(void)									
{
	Initialize(Parameters);
	
	return( true );
}


//---------------------------------------------------------
bool CTerrainFloodingInteractive::On_Execute_Finish(void)
{
	Finalize(Parameters);
	
	return( true );
}


//---------------------------------------------------------
bool CTerrainFloodingInteractive::On_Execute_Position(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode)
{
	static bool bBuisy = false;

	if( Mode == TOOL_INTERACTIVE_LDOWN )
	{
		if( bBuisy == false )
		{
			bBuisy = true;

			Process_Set_Text("%s...", _TL("Processing"));

			SG_UI_Msg_Lock(true);
			bool bResult = Set_Flooding(ptWorld.x, ptWorld.y, m_dWaterLevel, true, !Parameters("CUMULATIVE")->asBool());
			SG_UI_Msg_Lock(false);

			SG_UI_Process_Set_Okay();

			bBuisy = false;

			return( bResult );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

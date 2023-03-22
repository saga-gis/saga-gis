
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                       TIN_Tools                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  TIN_Flow_Trace.cpp                   //
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
#include "TIN_Flow_Trace.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTIN_Flow_Trace::CTIN_Flow_Trace(void)
{
	Set_Name		(_TL("Flow Accumulation (Trace)"));

	Set_Author		("O.Conrad (c) 2004");

	Set_Description	(_TW(
		"Calculates the catchment area based on the selected elevation values.\n\n"
	));

	//-----------------------------------------------------
	Parameters.Add_TIN("",
		"DEM"	, _TL("TIN"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field("DEM",
		"ZFIELD", _TL("Z Values"),
		_TL("")
	);

	Parameters.Add_TIN("",
		"FLOW"	, _TL("Flow Accumulation"),
		_TL(""),
		PARAMETER_OUTPUT
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTIN_Flow_Trace::On_Execute(void)
{
	CSG_TIN *pDEM = Parameters("DEM")->asTIN();

	m_iHeight = Parameters("ZFIELD")->asInt();

	m_pFlow = Parameters("FLOW")->asTIN();

	m_pFlow->Create(*pDEM);

	m_iDir      = m_pFlow->Get_Field_Count(); m_pFlow->Add_Field("DIRECTION", SG_DATATYPE_Double);
	m_iArea     = m_pFlow->Get_Field_Count(); m_pFlow->Add_Field("AREA"     , SG_DATATYPE_Double);
	m_iFlow     = m_pFlow->Get_Field_Count(); m_pFlow->Add_Field("FLOW"     , SG_DATATYPE_Double);
	m_iSpecific	= m_pFlow->Get_Field_Count(); m_pFlow->Add_Field("Specific" , SG_DATATYPE_Double);

	//-----------------------------------------------------
	for(sLong iPoint=0; iPoint<m_pFlow->Get_Node_Count(); iPoint++)
	{
		CSG_TIN_Node *pPoint = m_pFlow->Get_Node(iPoint);

		pPoint->Set_Value(m_iDir , Get_Lowest_Neighbor(pPoint));
		pPoint->Set_Value(m_iArea, pPoint->Get_Polygon_Area());
	}

	//-----------------------------------------------------
	for(sLong iPoint=0; iPoint<m_pFlow->Get_Node_Count() && Set_Progress(iPoint, m_pFlow->Get_Node_Count()); iPoint++)
	{
		CSG_TIN_Node *pPoint = m_pFlow->Get_Node(iPoint);

		if( pPoint->asDouble(m_iArea) > 0. )
		{
			Trace(pPoint, pPoint->asDouble(m_iArea));
		}
	}

	//-----------------------------------------------------
	for(sLong iPoint=0; iPoint<m_pFlow->Get_Node_Count() && Set_Progress(iPoint, m_pFlow->Get_Node_Count()); iPoint++)
	{
		CSG_TIN_Node *pPoint = m_pFlow->Get_Node(iPoint);

		pPoint->Set_Value(m_iSpecific, pPoint->asDouble(m_iArea) > 0. ? 1. / pPoint->asDouble(m_iArea) : -1.);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTIN_Flow_Trace::Get_Lowest_Neighbor(CSG_TIN_Node *pPoint)
{
	int iMin = -1; double dzMin = 0.;

	for(int i=0; i<pPoint->Get_Neighbor_Count(); i++)
	{
		double dz = pPoint->Get_Gradient(i, m_iHeight);

		if( dz > dzMin )
		{
			iMin = i; dzMin = dz;
		}
	}

	return( iMin );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CTIN_Flow_Trace::Trace(CSG_TIN_Node *pPoint, double Area)
{
	CSG_TIN_Node *pNeighbor = pPoint->Get_Neighbor(pPoint->asInt(m_iDir));

	if( pNeighbor )
	{
		pNeighbor->Add_Value(m_iFlow, Area);

		Trace(pNeighbor, Area);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

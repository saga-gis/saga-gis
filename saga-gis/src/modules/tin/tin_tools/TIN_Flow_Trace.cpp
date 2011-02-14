
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
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
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Flow Accumulation (Trace)"));

	Set_Author		(SG_T("(c) 2004 by O.Conrad"));

	Set_Description	(_TW(
		"Calculates the catchment area based on the selected elevation values.\n\n"
	));


	//-----------------------------------------------------
	pNode	= Parameters.Add_TIN(
		NULL	, "DEM"			, _TL("TIN"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "ZFIELD"		, _TL("Z Values"),
		_TL("")
	);

	pNode	= Parameters.Add_TIN(
		NULL	, "FLOW"		, _TL("Flow Accumulation"),
		_TL(""),
		PARAMETER_OUTPUT
	);
}

//---------------------------------------------------------
CTIN_Flow_Trace::~CTIN_Flow_Trace(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTIN_Flow_Trace::On_Execute(void)
{
	int				iPoint;
	CSG_TIN_Node	*pPoint;
	CSG_TIN			*pDEM;

	//-----------------------------------------------------
	pDEM		= Parameters("DEM")		->asTIN();
	m_iHeight	= Parameters("ZFIELD")	->asInt();
	m_pFlow		= Parameters("FLOW")	->asTIN();

	m_pFlow->Create(*pDEM);

	m_iDir		= m_pFlow->Get_Field_Count();
	m_pFlow->Add_Field("DIRECTION"	, SG_DATATYPE_Double);

	m_iArea		= m_pFlow->Get_Field_Count();
	m_pFlow->Add_Field("AREA"		, SG_DATATYPE_Double);

	m_iFlow		= m_pFlow->Get_Field_Count();
	m_pFlow->Add_Field("FLOW"		, SG_DATATYPE_Double);

	m_iSpecific	= m_pFlow->Get_Field_Count();
	m_pFlow->Add_Field("Specific"	, SG_DATATYPE_Double);

	//-----------------------------------------------------
	for(iPoint=0; iPoint<m_pFlow->Get_Node_Count(); iPoint++)
	{
		pPoint	= m_pFlow->Get_Node(iPoint);

		pPoint->Set_Value(m_iDir	, Get_Lowest_Neighbor(pPoint));
		pPoint->Set_Value(m_iArea	, pPoint->Get_Polygon_Area());
	}

	//-----------------------------------------------------
	for(iPoint=0; iPoint<m_pFlow->Get_Node_Count() && Set_Progress(iPoint, m_pFlow->Get_Node_Count()); iPoint++)
	{
		pPoint	= m_pFlow->Get_Node(iPoint);

		if( pPoint->asDouble(m_iArea) > 0.0 )
		{
			Trace(pPoint, pPoint->asDouble(m_iArea));
		}
	}

	//-----------------------------------------------------
	for(iPoint=0; iPoint<m_pFlow->Get_Node_Count() && Set_Progress(iPoint, m_pFlow->Get_Node_Count()); iPoint++)
	{
		pPoint	= m_pFlow->Get_Node(iPoint);

		pPoint->Set_Value(m_iSpecific, pPoint->asDouble(m_iArea) > 0.0
			? 1.0 / pPoint->asDouble(m_iArea)
			: -1.0
		);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CTIN_Flow_Trace::Get_Lowest_Neighbor(CSG_TIN_Node *pPoint)
{
	int		i, iMin;
	double	dz, dzMin;

	for(i=0, iMin=-1, dzMin=0.0; i<pPoint->Get_Neighbor_Count(); i++)
	{
		if( (dz = pPoint->Get_Gradient(i, m_iHeight)) > dzMin )
		{
			dzMin	= dz;
			iMin	= i;
		}
	}

	return( iMin );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CTIN_Flow_Trace::Trace(CSG_TIN_Node *pPoint, double Area)
{
	CSG_TIN_Node	*pNeighbor;

	if( (pNeighbor = pPoint->Get_Neighbor(pPoint->asInt(m_iDir))) != NULL )
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

/**********************************************************
 * Version $Id$
 *********************************************************/

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
//                 TIN_Flow_Parallel.cpp                 //
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
#include "TIN_Flow_Parallel.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTIN_Flow_Parallel::CTIN_Flow_Parallel(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Flow Accumulation (Parallel)"));

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

	Parameters.Add_TIN(
		NULL	, "FLOW"		, _TL("Flow Accumulation"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Method"),
		_TL(""),

		CSG_String::Format(SG_T("%s|%s|"),
			_TL("Single Flow Direction"),
			_TL("Multiple Flow Direction")
		)
	);
}

//---------------------------------------------------------
CTIN_Flow_Parallel::~CTIN_Flow_Parallel(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTIN_Flow_Parallel::On_Execute(void)
{
	int			iPoint;
	CSG_TIN		*pDEM;

	//-----------------------------------------------------
	pDEM		= Parameters("DEM")		->asTIN();
	m_iHeight	= Parameters("ZFIELD")	->asInt();
	m_pFlow		= Parameters("FLOW")	->asTIN();

	m_pFlow->Create(*pDEM);

	m_iArea		= m_pFlow->Get_Field_Count();
	m_pFlow->Add_Field(_TL("AREA")		, SG_DATATYPE_Double);

	m_iFlow		= m_pFlow->Get_Field_Count();
	m_pFlow->Add_Field(_TL("FLOW")		, SG_DATATYPE_Double);

	m_iSpecific	= m_pFlow->Get_Field_Count();
	m_pFlow->Add_Field(_TL("SPECIFIC")	, SG_DATATYPE_Double);

	//-----------------------------------------------------
	m_pFlow->Set_Index(m_iHeight, TABLE_INDEX_Descending);

	for(iPoint=0; iPoint<m_pFlow->Get_Node_Count() && Set_Progress(iPoint, m_pFlow->Get_Node_Count()); iPoint++)
	{
		switch( Parameters("METHOD")->asInt() )
		{
		case 0: default:
			Let_it_flow_single		(m_pFlow->Get_Node(m_pFlow->Get_Record_byIndex(iPoint)->Get_Index()));
			break;

		case 1:
			Let_it_flow_multiple	(m_pFlow->Get_Node(m_pFlow->Get_Record_byIndex(iPoint)->Get_Index()));
			break;
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
void CTIN_Flow_Parallel::Let_it_flow_single(CSG_TIN_Node *pPoint)
{
	int		i, iMin;
	double	dz, dzMin, Area;

	Area	= pPoint->Get_Polygon_Area();

	pPoint->Set_Value(m_iArea, Area);
	pPoint->Add_Value(m_iFlow, Area);

	for(i=0, iMin=-1, dzMin=0.0; i<pPoint->Get_Neighbor_Count(); i++)
	{
		if( (dz = pPoint->Get_Gradient(i, m_iHeight)) > dzMin )
		{
			dzMin	= dz;
			iMin	= i;
		}
	}

	if( iMin >= 0 )
	{
		pPoint->Get_Neighbor(iMin)->Add_Value(m_iFlow, pPoint->asDouble(m_iFlow));
	}

	pPoint->Set_Value(m_iSpecific, Area > 0.0 ? 1.0 / Area : -1.0);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CTIN_Flow_Parallel::Let_it_flow_multiple(CSG_TIN_Node *pPoint)
{
	int		i;
	double	d, dzSum, *dz, Area;

	Area	= pPoint->Get_Polygon_Area();

	pPoint->Set_Value(m_iArea, Area);
	pPoint->Add_Value(m_iFlow, Area);

	if( pPoint->Get_Neighbor_Count() > 0 )
	{
		dz	= (double *)SG_Malloc(pPoint->Get_Neighbor_Count() * sizeof(double));

		for(i=0, dzSum=0.0; i<pPoint->Get_Neighbor_Count(); i++)
		{
			if( (d = pPoint->Get_Gradient(i, m_iHeight)) > 0.0 )
			{
				dzSum	+= (dz[i]	= d);
			}
			else
			{
				dz[i]	= 0.0;
			}
		}

		if( dzSum > 0.0 )
		{
			d	= pPoint->asDouble(m_iFlow);

			for(i=0; i<pPoint->Get_Neighbor_Count(); i++)
			{
				if( dz[i] > 0.0 )
				{
					pPoint->Get_Neighbor(i)->Add_Value(	m_iFlow, d * dz[i] / dzSum);
				}
			}
		}

		SG_Free(dz);
	}

	pPoint->Set_Value(m_iSpecific, Area > 0.0 ? 1.0 / Area : -1.0);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

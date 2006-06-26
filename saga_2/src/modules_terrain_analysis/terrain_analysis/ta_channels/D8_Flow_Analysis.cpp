
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      ta_channels                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  D8_Flow_Analysis.cpp                 //
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
#include "D8_Flow_Analysis.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CD8_Flow_Analysis::CD8_Flow_Analysis(void)
{
	//-----------------------------------------------------
	Set_Name(_TL("D8 Flow Analysis"));

	Set_Author(_TL("Copyrights (c) 2003 by Olaf Conrad"));

	Set_Description(_TL(
		"Deterministic 8 based flow network analysis\n")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "DEM"			, _TL("DEM"),
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "DIRECTION"	, _TL("Flow Direction"),
		"",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "CONNECTION"	, _TL("Flow Connectivity"),
		"",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Shapes(
		NULL	, "NETWORK"		, _TL("Flow Network"),
		"",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "MINCON"		, _TL("Minimum Connectivity"),
		"",
		PARAMETER_TYPE_Int, 0, 0, true, 8, true
	);
}

//---------------------------------------------------------
CD8_Flow_Analysis::~CD8_Flow_Analysis(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CD8_Flow_Analysis::On_Execute(void)
{
	//-----------------------------------------------------
	m_pDEM		= Parameters("DEM")			->asGrid();

	m_MinCon	= Parameters("MINCON")		->asInt();

	m_pDir		= Parameters("DIRECTION")	->asGrid();
	m_pCon		= Parameters("CONNECTION")	->asGrid();

	m_pNet		= Parameters("NETWORK")		->asShapes();
	m_pNet->Create(SHAPE_TYPE_Line, _TL("D8 Flow Network"));
	m_pNet->Get_Table().Add_Field("ID"		, TABLE_FIELDTYPE_Int);
	m_pNet->Get_Table().Add_Field(_TL("LENGTH")	, TABLE_FIELDTYPE_Double);

	//-----------------------------------------------------
	//Process_Set_Text("Channel Network: Pass 1");

	Set_Direction();
	Set_Connectivity();

	//-----------------------------------------------------
	Lock_Create();

	//-----------------------------------------------------
	Set_Network();

	//-----------------------------------------------------
	Lock_Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CD8_Flow_Analysis::Set_Direction(void)
{
	bool	bResult	= false;
	int		x, y, i;

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( m_pDEM->is_InGrid(x, y) && (i = m_pDEM->Get_Gradient_NeighborDir(x, y)) >= 0 )
			{
				bResult	= true;

				m_pDir->Set_Value(x, y, i);
			}
			else
			{
				m_pDir->Set_Value(x, y, -1);
			}
		}
	}

	return( bResult );
}

//---------------------------------------------------------
bool CD8_Flow_Analysis::Set_Connectivity(void)
{
	int		x, y, i, ix, iy;

	m_pCon->Assign(0.0);

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( (i = m_pDir->asInt(x, y)) >= 0 && Get_System()->Get_Neighbor_Pos(i, x, y, ix, iy) )
			{
				m_pCon->Add_Value(ix, iy, 1);
			}
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
bool CD8_Flow_Analysis::Set_Network(void)
{
	int		x, y, i, n;
	CShape	*pLine;

	for(y=0, n=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( !is_Locked(x, y) && (i = m_pCon->asInt(x, y)) != 1 && i >= m_MinCon )
			{
				pLine	= m_pNet->Add_Shape();
				pLine->Get_Record()->Set_Value(0, ++n);

				Set_Network(x, y, pLine);
			}
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CD8_Flow_Analysis::Set_Network(int x, int y, CShape *pLine)
{
	int		i, ix, iy;

	Lock_Set(x, y);

	pLine->Add_Point(Get_XMin() + x * Get_Cellsize(), Get_YMin() + y * Get_Cellsize());

	if( (i = m_pDir->asInt(x, y)) >= 0 )
	{
		pLine->Get_Record()->Add_Value(1, Get_Length(i));

		ix	= Get_xTo(i, x);
		iy	= Get_yTo(i, y);

		if( is_InGrid(ix, iy) && !is_Locked(ix, iy) && m_pCon->asInt(ix, iy) == 1 )
		{
			Set_Network(ix, iy, pLine);
		}
		else
		{
			pLine->Add_Point(Get_XMin() + ix * Get_Cellsize(), Get_YMin() + iy * Get_Cellsize());
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

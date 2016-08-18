/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
#define NODE_SPRING		1
#define NODE_JUNCTION	2
#define NODE_OUTLET		3


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CD8_Flow_Analysis::CD8_Flow_Analysis(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Channel Network and Drainage Basins"));

	Set_Author		(SG_T("O.Conrad (c) 2003"));

	Set_Description	(_TW(
		"Deterministic 8 based flow network analysis\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "DEM"			, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "DIRECTION"	, _TL("Flow Direction"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, true, SG_DATATYPE_Char
	);

	Parameters.Add_Grid(
		NULL	, "CONNECTION"	, _TL("Flow Connectivity"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, true, SG_DATATYPE_Char
	);

	Parameters.Add_Grid(
		NULL	, "ORDER"		, _TL("Strahler Order"), 
		_TL(""), 
		PARAMETER_OUTPUT_OPTIONAL, true, SG_DATATYPE_Short
	);

	Parameters.Add_Grid(
		NULL	, "BASIN"		, _TL("Drainage Basins"), 
		_TL(""), 
		PARAMETER_OUTPUT_OPTIONAL, true, SG_DATATYPE_Short
	);

	Parameters.Add_Shapes(
		NULL	, "SEGMENTS"	, _TL("Channels"), 
		_TL(""), 
		PARAMETER_OUTPUT, SHAPE_TYPE_Line
	);

	Parameters.Add_Shapes(
		NULL	, "BASINS"		, _TL("Drainage Basins"), 
		_TL(""), 
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Shapes(
		NULL	, "NODES"		, _TL("Junctions"), 
		_TL(""), 
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point
	);

	Parameters.Add_Value(
		NULL	, "THRESHOLD"	, _TL("Threshold"), 
		_TL("Strahler order to begin a channel."), 
		PARAMETER_TYPE_Int, 5, 1, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CD8_Flow_Analysis::On_Execute(void)
{
	CSG_Grid	Dir, Order, Basins;

	m_pDEM		= Parameters("DEM")			->asGrid();

	m_pDir		= Parameters("DIRECTION")	->asGrid();		if( !m_pDir    ) { m_pDir    = &Dir   ; Dir   .Create(*Get_System(), SG_DATATYPE_Char ); Dir   .Set_Name(_TL("Flow Direction" )); }
	m_pOrder	= Parameters("ORDER")		->asGrid();		if( !m_pOrder  ) { m_pOrder  = &Order ; Order .Create(*Get_System(), SG_DATATYPE_Short); Order .Set_Name(_TL("Strahler Order" )); }
	m_pBasins	= Parameters("BASIN")		->asGrid();		if( !m_pBasins ) { m_pBasins = &Basins; Basins.Create(*Get_System(), SG_DATATYPE_Short); Basins.Set_Name(_TL("Drainage Basins")); }

	m_Threshold	= Parameters("THRESHOLD")	->asInt();

	//-----------------------------------------------------
	Get_Direction();

	Get_Order();

	Get_Nodes();

	Get_Basins();

	Get_Segments();

	//-----------------------------------------------------
	m_pOrder->Add(1 - m_Threshold);

	m_Nodes.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CD8_Flow_Analysis::Get_Direction(void)
{
	Process_Set_Text(_TL("Flow Direction"));

	m_pDir->Set_NoData_Value(-1);

	CSG_Grid	*pCon	= Parameters("CONNECTION")	->asGrid();

	if( pCon )
	{
		pCon->Assign(0.0);
	}

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0, i, ix, iy; x<Get_NX(); x++)
		{
			if( (i = m_pDEM->Get_Gradient_NeighborDir(x, y)) >= 0 && m_pDEM->is_InGrid(ix = Get_xTo(i, x), iy = Get_yTo(i, y)) )
			{
				m_pDir->Set_Value(x, y, i);

				if( pCon )
				{
					pCon->Add_Value(ix, iy, 1);
				}
			}
			else
			{
				m_pDir->Set_NoData(x, y);
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
void CD8_Flow_Analysis::Get_Order(void)
{
	Process_Set_Text(_TL("Stream Order"));

	m_pOrder->Set_NoData_Value_Range(1 - m_Threshold, 0);
	m_pOrder->Assign(0.0);

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !m_pDEM->is_NoData(x, y) )
			{
				Get_Order(x, y);
			}
		}
	}
}

//---------------------------------------------------------
int CD8_Flow_Analysis::Get_Order(int x, int y)
{
	int		Order	= m_pOrder->asInt(x, y);

	if( Order == 0 )
	{
		int		i, ix, iy, n;

		for(i=0, n=0, Order=1; i<8; i++)
		{
			if( Get_System()->Get_Neighbor_Pos(i + 4, x, y, ix, iy) && m_pDir->asInt(ix, iy) == i )
			{
				int		iOrder	= Get_Order(ix, iy);

				if( Order < iOrder )
				{
					Order	= iOrder;
					n		= 1;
				}
				else if( Order == iOrder )
				{
					n++;
				}
			}
		}

		if( n > 1 )
		{
			Order++;
		}

		m_pOrder->Set_Value(x, y, Order);
	}

	return( Order );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CD8_Flow_Analysis::Get_Nodes(void)
{
	Process_Set_Text(_TL("Junctions"));

	CSG_Shapes	*pNodes	= Parameters("NODES")->asShapes();

	if( pNodes )
	{
		pNodes	->Create(SHAPE_TYPE_Point, _TL("Junctions"), NULL, SG_VERTEX_TYPE_XYZ);
		pNodes	->Add_Field(_TL("NODE_ID")		, SG_DATATYPE_Int);
		pNodes	->Add_Field(_TL("TYPE")			, SG_DATATYPE_String);
	}

	m_Nodes.Create(*Get_System(), SG_DATATYPE_Int);

	m_pBasins->Set_NoData_Value(0);
	m_pBasins->Assign_NoData();

	for(int y=0, nNodes=0, nBasins=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			int	i, Order	= m_pOrder->asInt(x, y);

			if( Order >= m_Threshold )
			{
				if( (i = m_pDir->asInt(x, y)) >= 0 )
				{
					int	ix	= Get_xTo(i, x);
					int	iy	= Get_yTo(i, y);

					if( !m_Nodes.asInt(ix, iy) && m_pOrder->asInt(ix, iy) > Order && m_pDir->asInt(ix, iy) >= 0 )
					{
						Set_Node(ix, iy, ++nNodes, NODE_JUNCTION, pNodes ? pNodes->Add_Shape() : NULL);
					}

					if( Order == m_Threshold )
					{
						bool	bSpring	= true;

						for(i=0; i<8 && bSpring; i++)
						{
							if( Get_System()->Get_Neighbor_Pos(i + 4, x, y, ix, iy) && m_pDir->asInt(ix, iy) == i )
							{
								bSpring	= m_pOrder->asInt(ix, iy) < m_Threshold;
							}
						}

						if( bSpring )
						{
							Set_Node(x, y, ++nNodes, NODE_SPRING, pNodes ? pNodes->Add_Shape() : NULL);
						}
					}
				}
				else
				{
					Set_Node(x, y, ++nNodes, NODE_OUTLET, pNodes ? pNodes->Add_Shape() : NULL);

					m_pBasins->Set_Value(x, y, ++nBasins);
				}
			}
		}
	}
}

//---------------------------------------------------------
void CD8_Flow_Analysis::Set_Node(int x, int y, int id, int type, CSG_Shape *pNode)
{
	m_Nodes.Set_Value(x, y, id);

	if( pNode )
	{
		pNode->Set_Value(0, id);
		pNode->Set_Value(1, type == NODE_SPRING ? _TL("Spring") : type == NODE_OUTLET ? _TL("Outlet") : _TL("Junction"));

		pNode->Add_Point(Get_System()->Get_Grid_to_World(x, y));
		pNode->Set_Z(m_pDEM->asDouble(x, y), 0);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CD8_Flow_Analysis::Get_Basins(void)
{
	Process_Set_Text(_TL("Drainage Basins"));

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			Get_Basin(x, y);
		}
	}

	//-----------------------------------------------------
	CSG_Shapes	*pBasins	= Parameters("BASINS")->asShapes();

	if( pBasins )
	{
		bool	bResult;

		SG_RUN_TOOL(bResult, "shapes_grid", 6,
				pTool->Get_Parameters()->Set_Parameter(SG_T("GRID")    , m_pBasins)
			&&	pTool->Get_Parameters()->Set_Parameter(SG_T("POLYGONS"),   pBasins)
		)

		pBasins->Set_Name(_TL("Drainage Basins"));
	}
}

//---------------------------------------------------------
int CD8_Flow_Analysis::Get_Basin(int x, int y)
{
	int		i, Basin	= m_pBasins->asInt(x, y);

	if( Basin <= 0 && (i = m_pDir->asInt(x, y)) >= 0 && (Basin = Get_Basin(Get_xTo(i, x), Get_yTo(i, y))) > 0 )
	{
		m_pBasins->Set_Value(x, y, Basin);
	}

	return( Basin );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CD8_Flow_Analysis::Get_Segments(void)
{
	Process_Set_Text(_TL("Channels"));

	m_pSegments	= Parameters("SEGMENTS")->asShapes();
	m_pSegments	->Create(SHAPE_TYPE_Line, _TL("Channels"), NULL, SG_VERTEX_TYPE_XYZ);

	m_pSegments	->Add_Field(SG_T("SEGMENT_ID")	, SG_DATATYPE_Int);
	m_pSegments	->Add_Field(SG_T("NODE_A")		, SG_DATATYPE_Int);
	m_pSegments	->Add_Field(SG_T("NODE_B")		, SG_DATATYPE_Int);
	m_pSegments	->Add_Field(SG_T("BASIN")		, SG_DATATYPE_Int);
	m_pSegments	->Add_Field(SG_T("ORDER")		, SG_DATATYPE_Int);
	m_pSegments	->Add_Field(SG_T("ORDER_CELL")	, SG_DATATYPE_Int);
	m_pSegments	->Add_Field(SG_T("LENGTH")		, SG_DATATYPE_Double);

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( m_Nodes.asInt(x, y) )
			{
				Get_Segment(x, y);
			}
		}
	}
}

//---------------------------------------------------------
void CD8_Flow_Analysis::Get_Segment(int x, int y)
{
	int		i	= m_pDir->asInt(x, y);

	if( i >= 0 )
	{
		CSG_Shape	*pSegment	= m_pSegments->Add_Shape();

		pSegment->Set_Value(0, m_pSegments->Get_Count());					// SEGMENT_ID
		pSegment->Set_Value(1, m_Nodes.asInt(x, y));						// NODE_A
		pSegment->Set_Value(3, m_pBasins->asInt(x, y));						// BASIN
		pSegment->Set_Value(4, m_pOrder->asInt(x, y) + 1 - m_Threshold);	// ORDER
		pSegment->Set_Value(5, m_pOrder->asInt(x, y));						// ORDER_CELL

		pSegment->Add_Point(Get_System()->Get_Grid_to_World(x, y));
		pSegment->Set_Z(m_pDEM->asDouble(x, y), pSegment->Get_Point_Count() - 1);

		do
		{
			x	+= Get_xTo(i);
			y	+= Get_yTo(i);

			pSegment->Add_Point(Get_System()->Get_Grid_to_World(x, y));
			pSegment->Set_Z(m_pDEM->asDouble(x, y), pSegment->Get_Point_Count() - 1);

			if( m_Nodes.asInt(x, y) )
			{
				pSegment->Set_Value(2, m_Nodes.asInt(x, y));						// NODE_B
				pSegment->Set_Value(6, ((CSG_Shape_Line *)pSegment)->Get_Length());	// LENGTH

				return;
			}
		}
		while( (i = m_pDir->asInt(x, y)) >= 0 );
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

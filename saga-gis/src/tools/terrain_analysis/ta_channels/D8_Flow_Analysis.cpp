
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
#include "D8_Flow_Analysis.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	NODE_SPRING   = 1,
	NODE_JUNCTION = 2,
	NODE_OUTLET   = 3,
	NODE_MOUTH    = 4
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CD8_Flow_Analysis::CD8_Flow_Analysis(void)
{
	Set_Name		(_TL("Channel Network and Drainage Basins"));

	Set_Author		("O.Conrad (c) 2003");

	Set_Description	(_TW(
		"Deterministic 8 based flow network analysis. "
	));

	//-----------------------------------------------------
	Parameters.Add_Grid  ("", "DEM"       , _TL("Elevation"        ), _TL(""), PARAMETER_INPUT);

	Parameters.Add_Grid  ("", "DIRECTION" , _TL("Flow Direction"   ), _TL(""), PARAMETER_OUTPUT_OPTIONAL, true, SG_DATATYPE_Char );
	Parameters.Add_Grid  ("", "CONNECTION", _TL("Flow Connectivity"), _TL(""), PARAMETER_OUTPUT_OPTIONAL, true, SG_DATATYPE_Char );
	Parameters.Add_Grid  ("", "ORDER"     , _TL("Strahler Order"   ), _TL(""), PARAMETER_OUTPUT_OPTIONAL, true, SG_DATATYPE_Short);
	Parameters.Add_Grid  ("", "BASIN"     , _TL("Drainage Basins"  ), _TL(""), PARAMETER_OUTPUT_OPTIONAL, true, SG_DATATYPE_Short);

	Parameters.Add_Shapes("", "SEGMENTS"  , _TL("Channels"         ), _TL(""), PARAMETER_OUTPUT         , SHAPE_TYPE_Line   );
	Parameters.Add_Shapes("", "BASINS"    , _TL("Drainage Basins"  ), _TL(""), PARAMETER_OUTPUT         , SHAPE_TYPE_Polygon);
	Parameters.Add_Shapes("", "NODES"     , _TL("Junctions"        ), _TL(""), PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point  );

	Parameters.Add_Int("",
		"THRESHOLD"	, _TL("Threshold"), 
		_TL("Strahler order to begin a channel."), 
		5, 1, true
	);

	Parameters.Add_Bool("",
		"SUBBASINS"	, _TL("Subbasins"), 
		_TL(""), 
		true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CD8_Flow_Analysis::On_Execute(void)
{
	m_pDEM      = Parameters("DEM"      )->asGrid();

	m_pDir      = Parameters("DIRECTION")->asGrid(); CSG_Grid Dir   ; if( !m_pDir    ) { m_pDir    = &Dir   ; Dir   .Create(Get_System(), SG_DATATYPE_Char ); }
	m_pOrder    = Parameters("ORDER"    )->asGrid(); CSG_Grid Order ; if( !m_pOrder  ) { m_pOrder  = &Order ; Order .Create(Get_System(), SG_DATATYPE_Short); }
	m_pBasins   = Parameters("BASIN"    )->asGrid(); CSG_Grid Basins; if( !m_pBasins ) { m_pBasins = &Basins; Basins.Create(Get_System(), SG_DATATYPE_Short); }

	m_Threshold	= Parameters("THRESHOLD")->asInt();

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
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CD8_Flow_Analysis::Get_Direction(void)
{
	Process_Set_Text(_TL("Flow Direction"));

	m_pDir->Set_NoData_Value(-1);

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			int i = m_pDEM->Get_Gradient_NeighborDir(x, y);

			if( i >= 0 ) // && m_pDEM->is_InGrid(Get_xTo(i, x), Get_yTo(i, y)) )
			{
				m_pDir->Set_Value(x, y, i);
			}
			else
			{
				m_pDir->Set_Value(x, y, -1);
			}
		}
	}

	//-----------------------------------------------------
	CSG_Grid *pConnection = Parameters("CONNECTION")->asGrid();

	if( pConnection )
	{
		Process_Set_Text(_TL("Connectivity"));

		pConnection->Set_NoData_Value(0.);

		for(int y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			#pragma omp parallel for
			for(int x=0; x<Get_NX(); x++)
			{
				int n = 0;

				for(int i=0; i<8; i++)
				{
					int ix = Get_xFrom(i, x), iy = Get_yFrom(i, y);

					if( m_pDEM->is_InGrid(ix, iy) && i == m_pDir->asInt(ix, iy) )
					{
						n++;
					}
				}

				pConnection->Set_Value(x, y, n);
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CD8_Flow_Analysis::Get_Order(void)
{
	Process_Set_Text(_TL("Stream Order"));

	m_pOrder->Set_NoData_Value_Range(1 - m_Threshold, 0);
	m_pOrder->Assign(0.);

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


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifdef USE_RECURSIVE_FUNCTION

//---------------------------------------------------------
int CD8_Flow_Analysis::Get_Order(int x, int y)
{
	int Order = m_pOrder->asInt(x, y);

	if( Order < 1 )
	{
		Order = 1; int n = 0;

		for(int i=0; i<8; i++)
		{
			int ix = Get_xFrom(i, x), iy = Get_yFrom(i, y);

			if( is_InGrid(ix, iy) && i == m_pDir->asInt(ix, iy) )
			{
				int iOrder = Get_Order(ix, iy);

				if( Order < iOrder )
				{
					Order = iOrder;
					n     = 1;
				}
				else if( Order == iOrder )
				{
					n      ++;
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

#else // #ifndef USE_RECURSIVE_FUNCTION

//---------------------------------------------------------
class CStack : public CSG_Stack
{
public:
	struct SData { int x, y; char i, n, o; };

	CStack(void) : CSG_Stack(sizeof(struct SData))	{}

	virtual bool Push (int  x, int  y, int  i, int  n, int  o)
	{
		struct SData *Data = (struct SData *)Get_Record_Push(); if( !Data ) { return( false ); }

		Data->x = x; Data->y = y; Data->i = (char)i; Data->n = (char)n; Data->o = (char)o;

		return( true );
	}

	virtual bool Pop (int &x, int &y, int &i, int &n, int &o)
	{
		struct SData *Data = (struct SData *)Get_Record_Pop (); if( !Data ) { return( false ); }

		x = Data->x; y = Data->y; i = Data->i; n = Data->n; o = Data->o;

		return( true );
	}
};

//---------------------------------------------------------
int CD8_Flow_Analysis::Get_Order(int x, int y)
{
	int Order = m_pOrder->asInt(x, y);

	if( Order < 1 )
	{
		Order = 1; int i = 0, n = 0; CStack Stack; Stack.Push(x, y, i, n, Order);

		while( Stack.Get_Size() > 0 && Process_Get_Okay() )
		{
			Stack.Pop(x, y, i, n, Order);

			for(; i<8; i++)
			{
				int ix = Get_xFrom(i, x), iy = Get_yFrom(i, y);

				if( is_InGrid(ix, iy) && i == m_pDir->asInt(ix, iy) )
				{
					int iOrder = m_pOrder->asInt(ix, iy);

					if( iOrder < 1 ) // neighbour has not been processed yet!
					{
						Stack.Push(x, y, i, n, Order); x = ix; y = iy; i = -1, n = 0, Order = 1;
					}
					else
					{
						if( Order < iOrder )
						{
							Order = iOrder;
							n     = 1;
						}
						else if( Order == iOrder )
						{
							n      ++;
						}
					}
				}
			}

			if( n > 1 )
			{
				Order++;
			}

			m_pOrder->Set_Value(x, y, Order);
		}
	}

	return( Order );
}
#endif


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CD8_Flow_Analysis::Get_Nodes(void)
{
	Process_Set_Text(_TL("Junctions"));

	CSG_Shapes *pNodes = Parameters("NODES")->asShapes();

	if( pNodes )
	{
		pNodes->Create(SHAPE_TYPE_Point, _TL("Junctions"), NULL, SG_VERTEX_TYPE_XYZ);
		pNodes->Add_Field("ID"   , SG_DATATYPE_Short );
		pNodes->Add_Field("TYPE" , SG_DATATYPE_String);
		pNodes->Add_Field("ORDER", SG_DATATYPE_Int   );
		pNodes->Add_Field("BASIN", SG_DATATYPE_Int   );
	}

	m_Nodes.Create(Get_System(), SG_DATATYPE_Short);

	//-----------------------------------------------------
	m_pBasins->Set_NoData_Value(0.);

	#pragma omp parallel for
	for(int y=0; y<Get_NY(); y++) for(int x=0; x<Get_NX(); x++)
	{
		m_pBasins->Set_Value(x, y, m_pDEM->is_NoData(x, y) ? 0 : -1);
	}

	//-----------------------------------------------------
	bool bSubbasins = Parameters("SUBBASINS")->asBool();

	for(int y=0, nNodes=0, nBasins=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			int Order = m_pOrder->asInt(x, y);

			if( Order >= m_Threshold )
			{
				int i = m_pDir->asInt(x, y);

				if( i >= 0 )
				{
					int ix = Get_xTo(i, x), iy = Get_yTo(i, y);

					if( !m_Nodes.asInt(ix, iy) && m_pOrder->asInt(ix, iy) > Order && m_pDir->asInt(ix, iy) >= 0 )
					{
						Set_Node(ix, iy, ++nNodes, NODE_JUNCTION, pNodes ? pNodes->Add_Shape() : NULL);

						if( bSubbasins )
						{
							for(int j=0; j<8; j++)
							{
								int jx = Get_xFrom(j, ix), jy = Get_yFrom(j, iy);

								if( is_InGrid(jx, jy) && m_pDir->asInt(jx, jy) == j && m_pOrder->asInt(jx, jy) >= m_Threshold )
								{
									m_pBasins->Set_Value(jx, jy, ++nBasins);

									Set_Node(jx, jy, 0, NODE_MOUTH, pNodes ? pNodes->Add_Shape() : NULL);
								}
							}
						}
					}

					if( Order == m_Threshold )
					{
						bool bSpring = true;

						for(int j=0; j<8 && bSpring; j++)
						{
							int jx = Get_xFrom(j, x), jy = Get_yFrom(j, y);

							if( is_InGrid(jx, jy) && m_pDir->asInt(jx, jy) == j )
							{
								bSpring	= m_pOrder->asInt(jx, jy) < m_Threshold;
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
					m_pBasins->Set_Value(x, y, ++nBasins);

					Set_Node(x, y, ++nNodes, NODE_OUTLET, pNodes ? pNodes->Add_Shape() : NULL);
				}
			}
		}
	}
}

//---------------------------------------------------------
void CD8_Flow_Analysis::Set_Node(int x, int y, int id, int type, CSG_Shape *pNode)
{
	if( type != NODE_MOUTH )
	{
		m_Nodes.Set_Value(x, y, id);
	}

	if( pNode )
	{
		pNode->Set_Value(0, id);

		pNode->Set_Value(1,
			type == NODE_SPRING   ? _TL("Spring"  ) :
			type == NODE_OUTLET   ? _TL("Outlet"  ) :
			type == NODE_JUNCTION ? _TL("Junction") :
			type == NODE_MOUTH    ? _TL("Mouth"   ) : _TL("")
		);

		pNode->Set_Value(2, m_pOrder->asInt(x, y) + 1 - m_Threshold);
		pNode->Set_Value(3, type == NODE_OUTLET || type == NODE_MOUTH ? m_pBasins->asInt(x, y) : 0);

		pNode->Add_Point(Get_System().Get_Grid_to_World(x, y));
		pNode->Set_Z(m_pDEM->asDouble(x, y), 0);
	}
}


///////////////////////////////////////////////////////////
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
	CSG_Shapes *pBasins = Parameters("BASINS")->asShapes();

	if( pBasins )
	{
		bool bResult;

		SG_RUN_TOOL(bResult, "shapes_grid", 6,
				pTool->Set_Parameter("GRID"    , m_pBasins)
			&&	pTool->Set_Parameter("POLYGONS",   pBasins)
		);

		if( bResult )
		{
			CSG_Shapes *pNodes = Parameters("NODES")->asShapes();

			pBasins->Set_Name(_TL("Drainage Basins"));

			pBasins->Del_Field(2); // NAME (redundancy!)
			pBasins->Add_Field("AREA"     , SG_DATATYPE_Double);
			pBasins->Add_Field("PERIMETER", SG_DATATYPE_Double);

			if( pNodes )
			{
				pBasins->Add_Field("ORDER", SG_DATATYPE_Int);
			}

			for(int i=0; i<pBasins->Get_Count(); i++)
			{
				CSG_Shape_Polygon *pBasin = pBasins->Get_Shape(i)->asPolygon();

				pBasin->Set_Value("AREA"     , pBasin->Get_Area     ());
				pBasin->Set_Value("PERIMETER", pBasin->Get_Perimeter());

				if( pNodes )
				{
					CSG_Table_Record *pNode = pNodes->Find_Record(3, pBasin->asInt("VALUE"));

					if( pNode )
					{
						pBasin->Set_Value("ORDER", pNode->asInt(2));
					}
				}
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifdef USE_RECURSIVE_FUNCTION

//---------------------------------------------------------
int CD8_Flow_Analysis::Get_Basin(int x, int y)
{
	int i, Basin = m_pBasins->asInt(x, y);

	if( Basin < 0 )
	{
		if( (i = m_pDir->asInt(x, y)) >= 0 && (Basin = Get_Basin(Get_xTo(i, x), Get_yTo(i, y))) >= 0 )
		{
			m_pBasins->Set_Value(x, y, Basin);
		}
	}

	return( Basin );
}

#else

//---------------------------------------------------------
int CD8_Flow_Analysis::Get_Basin(int x, int y)
{
	int i, Basin = m_pBasins->asInt(x, y);

	if( Basin < 0 )
	{
		CSG_Grid_Stack Stack;

		while( Basin < 0 && (i = m_pDir->asInt(x, y)) >= 0 )
		{
			Stack.Push(x, y);

			x += Get_xTo(i);
			y += Get_yTo(i);

			Basin = m_pBasins->asInt(x, y);
		}

		if( Basin < 0 )
		{
			Basin = 0; // not linked to any basin, mark as processed!
		}

		if( Stack.Get_Size() == 0 )
		{
			m_pBasins->Set_Value(x, y, Basin);
		}
		else while( Stack.Get_Size() > 0 )
		{
			Stack.Pop(x, y);

			m_pBasins->Set_Value(x, y, Basin);
		}
	}

	return( Basin );
}
#endif


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CD8_Flow_Analysis::Get_Segments(void)
{
	Process_Set_Text(_TL("Channels"));

	m_pSegments	= Parameters("SEGMENTS")->asShapes();
	m_pSegments->Create(SHAPE_TYPE_Line, _TL("Channels"), NULL, SG_VERTEX_TYPE_XYZ);

	m_pSegments->Add_Field("SEGMENT_ID", SG_DATATYPE_Int);
	m_pSegments->Add_Field("NODE_A"    , SG_DATATYPE_Int);
	m_pSegments->Add_Field("NODE_B"    , SG_DATATYPE_Int);
	m_pSegments->Add_Field("BASIN"     , SG_DATATYPE_Int);
	m_pSegments->Add_Field("ORDER"     , SG_DATATYPE_Int);
	m_pSegments->Add_Field("ORDER_CELL", SG_DATATYPE_Int);
	m_pSegments->Add_Field("LENGTH"    , SG_DATATYPE_Double);

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
	int i = m_pDir->asInt(x, y);

	if( i >= 0 )
	{
		CSG_Shape *pSegment = m_pSegments->Add_Shape();

		pSegment->Set_Value(0, m_pSegments->Get_Count()                ); // SEGMENT_ID
		pSegment->Set_Value(1, m_Nodes   .asInt(x, y)                  ); // NODE_A
		pSegment->Set_Value(3, m_pBasins->asInt(x, y)                  ); // BASIN
		pSegment->Set_Value(4, m_pOrder ->asInt(x, y) + 1 - m_Threshold); // ORDER
		pSegment->Set_Value(5, m_pOrder ->asInt(x, y)                  ); // ORDER_CELL

		pSegment->Add_Point(Get_System().Get_Grid_to_World(x, y));
		pSegment->Set_Z(m_pDEM->asDouble(x, y), pSegment->Get_Point_Count() - 1);

		do
		{
			x += Get_xTo(i);
			y += Get_yTo(i);

			pSegment->Add_Point(Get_System().Get_Grid_to_World(x, y));
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

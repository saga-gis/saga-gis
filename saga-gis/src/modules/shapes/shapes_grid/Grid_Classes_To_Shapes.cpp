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
//                      Grid_Shapes                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               Grid_Classes_To_Shapes.cpp              //
//                                                       //
//                 Copyright (C) 2008 by                 //
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
#include "Grid_Classes_To_Shapes.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Classes_To_Shapes::CGrid_Classes_To_Shapes(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Vectorising Grid Classes"));

	Set_Author		(SG_T("(c) 2008 by O.Conrad"));

	Set_Description	(_TW(
		"Vectorising grid classes."
	));


	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "GRID"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes(
		NULL	, "POLYGONS"	, _TL("Polygons"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Choice(
		NULL	, "CLASS_ALL"	, _TL("Class Selection"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("one single class specified by class identifier"),
			_TL("all classes")
		), 1
	);

	Parameters.Add_Value(
		NULL	, "CLASS_ID"	, _TL("Class Identifier"),
		_TL(""),
		PARAMETER_TYPE_Double, 1
	);

	Parameters.Add_Choice(
		NULL	, "SPLIT"		, _TL("Vectorised class as..."),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("one single (multi-)polygon object"),
			_TL("each island as separated polygon")
		), 0
	);

	Parameters.Add_Value(
		NULL	, "ALLVERTICES"	, _TL("Keep Vertices on Straight Lines"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Classes_To_Shapes::On_Execute(void)
{
	//-----------------------------------------------------
	if( !Get_Classes() || !Get_Edges() )
	{
		m_Classes.Destroy();

		return( false );
	}

	//-----------------------------------------------------
	if( Parameters("SPLIT")->asInt() == 1 )
	{
		Split_Polygons();
	}

	m_Classes.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Classes_To_Shapes::Get_Classes(void)
{
	sLong		i;
	int			id, x, y;
	double		Value;
	CSG_Grid	*pGrid;

	//-----------------------------------------------------
	pGrid		= Parameters("GRID")		->asGrid();

	m_pPolygons	= Parameters("POLYGONS")	->asShapes();

	m_pPolygons->Create(SHAPE_TYPE_Polygon);

	m_pPolygons->Add_Field(pGrid->Get_Name(), SG_DATATYPE_Double);
	m_pPolygons->Add_Field(SG_T("ID")		, SG_DATATYPE_Int);
	m_pPolygons->Add_Field(SG_T("NAME")		, SG_DATATYPE_String);

	DataObject_Set_Parameter(m_pPolygons, DataObject_Get_Parameter(pGrid, "LUT"));			// Lookup Table
	DataObject_Set_Parameter(m_pPolygons, DataObject_Get_Parameter(pGrid, "COLORS_TYPE"));	// Color Classification Type: Lookup Table
	DataObject_Set_Parameter(m_pPolygons, "LUT_ATTRIB", 0);								// Color Attribute

	m_pPolygons->Set_Name(pGrid->Get_Name());

	//-----------------------------------------------------
	Process_Set_Text(_TL("class identification"));

	m_Classes.Create(pGrid->Get_System(), SG_DATATYPE_Int);
	m_Classes.Set_NoData_Value(-1);
	m_Classes.Assign_NoData();

	//-----------------------------------------------------
	if( Parameters("CLASS_ALL")->asInt() == 1 )
	{
		for(i=0, id=-1; i<Get_NCells() && Set_Progress_NCells(i); i++)
		{
			if( pGrid->Get_Sorted(i, x, y, false) )
			{
				if( m_pPolygons->Get_Count() == 0 || Value != pGrid->asDouble(x, y) )
				{
					CSG_Shape	*pPolygon	= m_pPolygons->Add_Shape();

					pPolygon->Set_Value(0, Value = pGrid->asDouble(x, y));
					pPolygon->Set_Value(1, 1 + id++);
					pPolygon->Set_Value(2, CSG_String::Format(SG_T("%d"), m_pPolygons->Get_Count()));
				}

				m_Classes.Set_Value(x, y, id);
			}
		}
	}

	//-----------------------------------------------------
	else
	{
		CSG_Shape	*pPolygon	= m_pPolygons->Add_Shape();

		pPolygon->Set_Value(0, Value = Parameters("CLASS_ID")->asDouble());
		pPolygon->Set_Value(1, m_pPolygons->Get_Count());
		pPolygon->Set_Value(2, CSG_String::Format(SG_T("%d"), m_pPolygons->Get_Count()));

		for(y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			for(x=0; x<Get_NX(); x++)
			{
				if( pGrid->asDouble(x, y) == Value )
				{
					m_Classes.Set_Value(x, y, 0);
				}
			}
		}
	}

	//-----------------------------------------------------
	return( m_pPolygons->Get_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Classes_To_Shapes::Get_Edges(void)
{
	///////////////////////////////////////////////////////
	//-----------------------------------------------------
	Process_Set_Text(_TL("edge detection"));

	int		y, nEdges	= 0;

	m_Edges.Create(SG_DATATYPE_Int, 2 * Get_NX() + 1, 2 * Get_NY() + 1, 0.5 * Get_Cellsize(), Get_XMin() - 0.5 * Get_Cellsize(), Get_YMin() - 0.5 * Get_Cellsize());

	m_Edges.Set_NoData_Value(-2);
	m_Edges.Assign_NoData();

	m_bAllVertices	= Parameters("ALLVERTICES")->asBool();

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Process_Get_Okay(); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !m_Classes.is_NoData(x, y) )
			{
				int	ID	= m_Classes.asInt(x, y);

				for(int i=0; i<8; i+=2)
				{
					int	ix	= Get_xTo(i, x);
					int	iy	= Get_yTo(i, y);

					if( !m_Classes.is_InGrid(ix, iy) || m_Classes.asInt(ix, iy) != ID )
					{
						nEdges++;

						ix	= 1 + 2 * x;
						iy	= 1 + 2 * y;

						m_Edges.Set_Value(               ix,                 iy , ID);
						m_Edges.Set_Value(Get_xTo(i    , ix), Get_yTo(i    , iy), -1);
						m_Edges.Set_Value(Get_xTo(i - 1, ix), Get_yTo(i - 1, iy), -1);
					}
				}
			}
		}
	}

	if( nEdges == 0 )
	{
		Message_Add(_TL("no edges found"));

		return( false );
	}

	///////////////////////////////////////////////////////
	//-----------------------------------------------------
	Process_Set_Text(_TL("edge collection"));

	//-----------------------------------------------------
	for(y=0; y<m_Edges.Get_NY() && Set_Progress(y, m_Edges.Get_NY()); y++)
	{
		for(int x=0; x<m_Edges.Get_NX(); x++)
		{
			if( m_Edges.asInt(x, y) >= 0 )
			{
				for(int i=0; i<8; i+=2)
				{
					if( m_Edges.asInt(Get_xTo(i, x), Get_yTo(i, y)) == -1 )
					{
						Get_Edge(Get_xTo(i, x), Get_yTo(i, y), i + 2, m_Edges.asInt(x, y));

						break;
					}
				}
			}
		}
	}

	///////////////////////////////////////////////////////
	//-----------------------------------------------------
	m_Edges.Destroy();

	return( true );
}

//---------------------------------------------------------
bool CGrid_Classes_To_Shapes::Get_Edge(int x, int y, int i, int Class)
{
	CSG_Shape	*pPolygon	= m_pPolygons->Get_Shape(Class);

	if( !pPolygon )
	{
		return( false );
	}

	//-----------------------------------------------------
	int		xFirst, yFirst, ix, iy, iPart;

	xFirst	= x;
	yFirst	= y;

	iPart	= pPolygon->Get_Part_Count();

	pPolygon->Add_Point(m_Edges.Get_System().Get_Grid_to_World(x, y), iPart);

	//-----------------------------------------------------
	do
	{
		ix	= Get_xTo(i + 2, x);
		iy	= Get_yTo(i + 2, y);

		if( m_Edges.is_InGrid(ix, iy) && m_Edges.asInt(ix, iy) == -1 )		// go right ?
		{
			pPolygon->Add_Point(m_Edges.Get_System().Get_Grid_to_World(x, y), iPart);

			i	= (i + 2) % 8;
		}
		else
		{
			if( m_Edges.asInt(ix, iy) == Class )
			{
				m_Edges.Set_NoData(ix, iy);	// erase class id in right cells
			}

			ix	= Get_xTo(i, x);
			iy	= Get_yTo(i, y);

			if( m_Edges.is_InGrid(ix, iy) && m_Edges.asInt(ix, iy) == -1 )	// go ahead ?
			{
				if( m_bAllVertices )
				{
					pPolygon->Add_Point(m_Edges.Get_System().Get_Grid_to_World(x, y), iPart);
				}
			}
			else
			{
				ix	= Get_xTo(i + 6, x);
				iy	= Get_yTo(i + 6, y);

				if( m_Edges.is_InGrid(ix, iy) && m_Edges.asInt(ix, iy) == -1 )	// go left ?
				{
					pPolygon->Add_Point(m_Edges.Get_System().Get_Grid_to_World(x, y), iPart);

					i	= (i + 6) % 8;
				}
				else
				{
					return( false );
				}
			}
		}

		x	= ix;
		y	= iy;
	}
	while( x != xFirst || y != yFirst );

	pPolygon->Add_Point(m_Edges.Get_System().Get_Grid_to_World(x, y), iPart);

	if( pPolygon->Get_Point_Count(iPart) < 4 )
	{
		pPolygon->Del_Part(iPart);

		return( false );
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Classes_To_Shapes::Split_Polygons(void)
{
	Process_Set_Text(_TL("splitting polygon parts"));

	CSG_Shapes	Polygons(*m_pPolygons);

	m_pPolygons->Del_Records();

	for(int iPolygon=0; iPolygon<Polygons.Get_Count() && Set_Progress(iPolygon, Polygons.Get_Count()); iPolygon++)
	{
		CSG_Shape_Polygon	*pPolygon	= (CSG_Shape_Polygon *)Polygons.Get_Shape(iPolygon);

		for(int iPart=0, jPart, iPoint; iPart<pPolygon->Get_Part_Count() && Process_Get_Okay(); iPart++)
		{
			if( !pPolygon->is_Lake(iPart) )
			{
				CSG_Shape	*pShape	= m_pPolygons->Add_Shape(pPolygon, SHAPE_COPY_ATTR);

				for(iPoint=0, jPart=0; iPoint<pPolygon->Get_Point_Count(iPart); iPoint++)
				{
					pShape->Add_Point(pPolygon->Get_Point(iPoint, iPart), jPart);
				}

				for(int kPart=0; kPart<pPolygon->Get_Part_Count(); kPart++)
				{
					if( pPolygon->is_Lake(kPart) && pPolygon->Contains(pPolygon->Get_Point(0, kPart), iPart) )
					{
						jPart++;

						for(iPoint=0; iPoint<pPolygon->Get_Point_Count(kPart); iPoint++)
						{
							pShape->Add_Point(pPolygon->Get_Point(iPoint, kPart), jPart);
						}
					}
				}
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

/*/---------------------------------------------------------
class CSG_Vector_Topology
{
public:
	CSG_Vector_Topology(void)
	{
		m_Nodes.Create(SHAPE_TYPE_Point, SG_T("NODES"));
		m_Nodes.Add_Field(SG_T("ID")		, SG_DATATYPE_Int);

		m_Edges.Create(SHAPE_TYPE_Line , SG_T("EDGES"));
		m_Edges.Add_Field(SG_T("ID")		, SG_DATATYPE_Int);
		m_Edges.Add_Field(SG_T("NODE_FROM")	, SG_DATATYPE_Int);
		m_Edges.Add_Field(SG_T("NODE_TO")	, SG_DATATYPE_Int);
		m_Edges.Add_Field(SG_T("FACE_LEFT")	, SG_DATATYPE_Int);
		m_Edges.Add_Field(SG_T("FACE_RIGHT"), SG_DATATYPE_Int);
		m_Edges.Add_Field(SG_T("PROCESSED")	, SG_DATATYPE_Int);
	}

	virtual ~CSG_Vector_Topology(void)
	{
		Destroy();
	}

	bool					Destroy			(void)
	{
		m_Nodes.Del_Records();
		m_Edges.Del_Records();

		return( true );
	}

	bool					Add_Node		(int ID, double x, double y)
	{
		CSG_Shape	*pNode	= m_Nodes.Add_Shape();

		pNode->Set_Point(x, y, 0);
		pNode->Set_Value(0, ID);

		return( true );
	}

	bool					Add_Node		(int ID, const TSG_Point &Point)
	{
		return( Add_Node(ID, Point.x, Point.y) );
	}

	CSG_Shapes				m_Nodes, m_Edges;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Classes_To_Shapes::Get_Edges(void)
{
	///////////////////////////////////////////////////////
	//-----------------------------------------------------
	Process_Set_Text(_TL("edge detection"));

	int		y, nEdges	= 0;

	m_Topology.Destroy();

	m_Edges.Create(SG_DATATYPE_Int, 2 * Get_NX() + 1, 2 * Get_NY() + 1, 0.5 * Get_Cellsize(), Get_XMin() - 0.5 * Get_Cellsize(), Get_YMin() - 0.5 * Get_Cellsize());

	m_Edges.Set_NoData_Value(-2);
	m_Edges.Assign_NoData();

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Process_Get_Okay(); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !m_Classes.is_NoData(x, y) )
			{
				int	ID	= m_Classes.asInt(x, y);

				for(int i=0; i<8; i+=2)
				{
					int	ix	= Get_xTo(i, x);
					int	iy	= Get_yTo(i, y);

					if( !m_Classes.is_InGrid(ix, iy) || m_Classes.asInt(ix, iy) != ID )
					{
						nEdges++;

						m_Edges.Set_Value(Get_xTo(i    , 1 + 2 * x), Get_yTo(i    , 1 + 2 * y), -1);
						m_Edges.Set_Value(Get_xTo(i - 1, 1 + 2 * x), Get_yTo(i - 1, 1 + 2 * y), -1);
					}
				}
			}
		}
	}

	if( nEdges == 0 )
	{
		Message_Add(_TL("no edges found"));

		return( false );
	}

	///////////////////////////////////////////////////////
	//-----------------------------------------------------
	Process_Set_Text(_TL("node detection"));

	int		nNodes	= 0;

	//-----------------------------------------------------
	for(y=0; y<m_Edges.Get_NY() && Set_Progress(y, m_Edges.Get_NY()); y++)
	{
		for(int x=0; x<m_Edges.Get_NX(); x++)
		{
			if( m_Edges.asInt(x, y) == -1 )
			{
				int	n	= 0;

				for(int i=0; i<8; i+=2)
				{
					if( m_Edges.is_InGrid(Get_xTo(i, x), Get_yTo(i, y)) )
					{
						n++;
					}
				}

				if( n > 2 )
				{
					m_Topology.Add_Node(nNodes, m_Edges.Get_System().Get_Grid_to_World(x, y));

					m_Edges.Set_Value(x, y, nNodes++);
				}
			}
			else
			{
				m_Edges.Set_Value(x, y, -2);
			}
		}
	}

	///////////////////////////////////////////////////////
	//-----------------------------------------------------
	Process_Set_Text(_TL("edge collection"));

	//-----------------------------------------------------
	for(y=0; y<m_Edges.Get_NY() && Set_Progress(y, m_Edges.Get_NY()); y++)
	{
		for(int x=0; x<m_Edges.Get_NX(); x++)
		{
			if( m_Edges.asInt(x, y) >= 0 )	// node
			{
				for(int i=0; i<8; i+=2)
				{
					if( m_Edges.is_InGrid(Get_xTo(i, x), Get_yTo(i, y)) )
					{
						Get_Edge(x, y, i);
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	for(y=0; y<m_Edges.Get_NY() && Set_Progress(y, m_Edges.Get_NY()); y++)
	{
		for(int x=0; x<m_Edges.Get_NX(); x++)
		{
			if( m_Edges.asInt(x, y) == -1 )	// edge
			{
				m_Topology.Add_Node(nNodes, m_Edges.Get_System().Get_Grid_to_World(x, y));

				m_Edges.Set_Value(x, y, nNodes++);

				for(int i=0; i<8; i+=2)
				{
					if( m_Edges.is_InGrid(Get_xTo(i, x), Get_yTo(i, y)) )
					{
						Get_Edge(x, y, i);
					}
				}
			}
		}
	}

	///////////////////////////////////////////////////////
	//-----------------------------------------------------
	m_Edges.Destroy();

	return( true );
}

//---------------------------------------------------------
bool CGrid_Classes_To_Shapes::Get_Edge(int x, int y, int i)
{
	CSG_Shape	*pEdge	= m_Topology.m_Edges.Add_Shape();

	pEdge->Add_Point(m_Edges.Get_System().Get_Grid_to_World(x, y), 0);

	pEdge->Set_Value(0, m_Topology.m_Edges.Get_Count() - 1);	// edge id
	pEdge->Set_Value(1, m_Edges.asInt(x, y));					// from node

	int		ix, iy;

	ix	= Get_xTo(i - 1, x) / 2;
	iy	= Get_yTo(i - 1, y) / 2;
	pEdge->Set_Value(3, m_Classes.is_InGrid(ix, iy) ? m_Classes.asInt(ix, iy) : -1);	// left face

	ix	= Get_xTo(i + 1, x) / 2;
	iy	= Get_yTo(i + 1, y) / 2;
	pEdge->Set_Value(4, m_Classes.is_InGrid(ix, iy) ? m_Classes.asInt(ix, iy) : -1);	// right face

	//-----------------------------------------------------
	do
	{
		x	= Get_xTo(i, x);
		y	= Get_yTo(i, y);

		if( m_Edges.asInt(x, y) >= 0 )	// node
		{
			pEdge->Add_Point(m_Edges.Get_System().Get_Grid_to_World(x, y), 0);

			pEdge->Set_Value(2, m_Edges.asInt(x, y));	// to node

			return( true );
		}

		m_Edges.Set_NoData(x, y);

		//-------------------------------------------------
		if( !m_Edges.is_InGrid(Get_xTo(i, x), Get_yTo(i, y)) )	// do not go ahead ?
		{
			pEdge->Add_Point(m_Edges.Get_System().Get_Grid_to_World(x, y), 0);

			if( m_Edges.is_InGrid(Get_xTo(i + 2, x), Get_yTo(i + 2, y)) )		// go right ?
			{
				i	= (i + 2) % 8;
			}
			else if( m_Edges.is_InGrid(Get_xTo(i + 6, x), Get_yTo(i + 6, y)) )	// go left ?
			{
				i	= (i + 6) % 8;
			}
			else
			{
				i	= -1;
			}
		}
	}
	while( i >= 0 );

	//-----------------------------------------------------
	m_Topology.m_Edges.Del_Record(m_Topology.m_Edges.Get_Count() - 1);

	return( false );
}/**/


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

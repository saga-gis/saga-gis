/**********************************************************
 * Version $Id$
 *********************************************************/
/*******************************************************************************
    Watersheds.cpp
    Copyright (C) Victor Olaya
    
    This program is free software; you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation; either version 2 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program; if not, write to the Free Software
    Foundation, Inc., 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, USA
*******************************************************************************/ 

///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Watersheds_ext.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
enum
{
	FIELD_ID		= 0,
	FIELD_ID_MAIN,
	FIELD_MOUTH_X,
	FIELD_MOUTH_Y,
	FIELD_PERIMETER,
	FIELD_AREA,
	FIELD_CENTROID_X,
	FIELD_CENTROID_Y,
	FIELD_Z_MEAN,
	FIELD_Z_RANGE,
	FIELD_DIST_MEAN,
	FIELD_DIST_MAX,
	FIELD_CONCTIME,
	FIELD_BASINS_UP,
	FIELD_BASINS_DOWN,
	FIELD_BASIN_TYPE,
	FIELD_EQVRECT_A,
	FIELD_EQVRECT_B,
	FIELD_OROG_IDX,
	FIELD_MASS_IDX
};

//---------------------------------------------------------
#define BASIN_ADD_FIELDS(pBasins)	{\
	pBasins->Add_Field(_TL("ID")						, SG_DATATYPE_Int);\
	pBasins->Add_Field(_TL("Main Basin ID")				, SG_DATATYPE_Int);\
	pBasins->Add_Field(_TL("Outlet X")					, SG_DATATYPE_Double);\
	pBasins->Add_Field(_TL("Outlet Y")					, SG_DATATYPE_Double);\
	pBasins->Add_Field(_TL("Perimeter")					, SG_DATATYPE_Double);\
	pBasins->Add_Field(_TL("Area")						, SG_DATATYPE_Double);\
	pBasins->Add_Field(_TL("Centroid X")				, SG_DATATYPE_Double);\
	pBasins->Add_Field(_TL("Centroid Y")				, SG_DATATYPE_Double);\
	pBasins->Add_Field(_TL("Mean Elevation")			, SG_DATATYPE_Double);\
	pBasins->Add_Field(_TL("Elevation Range")			, SG_DATATYPE_Double);\
	pBasins->Add_Field(_TL("Mean Flow Distance")		, SG_DATATYPE_Double);\
	pBasins->Add_Field(_TL("Maximum Flow Distance")		, SG_DATATYPE_Double);\
	pBasins->Add_Field(_TL("Concentration time (h)")	, SG_DATATYPE_Double);\
	pBasins->Add_Field(_TL("Upslope Basins")			, SG_DATATYPE_String);\
	pBasins->Add_Field(_TL("Downslope Basins")			, SG_DATATYPE_Int);\
	pBasins->Add_Field(_TL("Basin Type (Gravelius)")	, SG_DATATYPE_String);\
	pBasins->Add_Field(_TL("Equivalent Rectangle (A)")	, SG_DATATYPE_Double);\
	pBasins->Add_Field(_TL("Equivalent Rectangle (B)")	, SG_DATATYPE_Double);\
	pBasins->Add_Field(_TL("Orographic Coefficient")	, SG_DATATYPE_Double);\
	pBasins->Add_Field(_TL("Massivity Coefficient")		, SG_DATATYPE_Double);\
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWatersheds_ext::CWatersheds_ext(void)
{
	Set_Name		(_TL("Watershed Basins (Extended)"));

	Set_Author		(SG_T("V.Olaya (c) 2004, O.Conrad (c) 2011"));

	Set_Description	(_TW(
		"Extended watershed basin analysis. "
	));

	Parameters.Add_Grid(
		NULL	, "DEM"			, _TL("DEM"),
		_TL(""), 
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "CHANNELS"	, _TL("Drainage Network"),
		_TL(""), 
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "BASINS"		, _TL("Basins"), 
		_TL(""), 
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "SUBBASINS"	, _TL("Subbasins"), 
		_TL(""), 
		PARAMETER_OUTPUT
	);

	Parameters.Add_Shapes(
		NULL	, "V_BASINS"	, _TL("Basins"),
		_TL(""), 
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Shapes(
		NULL	, "V_SUBBASINS"	, _TL("Subbasins"),
		_TL(""), 
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Shapes(
		NULL	, "HEADS"		, _TL("River Heads"),
		_TL(""), 
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes(
		NULL	, "MOUTHS"		, _TL("River Mouths"),
		_TL(""), 
		PARAMETER_OUTPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Value(
		NULL	, "DISTANCE"	, _TL("Flow Distances"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWatersheds_ext::On_Execute(void)
{
	int			x, y;
	CSG_Grid	*pBasins, *pSubBasins, Inflows;
	CSG_Shapes	*pHeads, *pMouths, *pVBasins, *pVSubBasins;

	m_pDEM		= Parameters("DEM")			->asGrid(); 
	m_pChannels	= Parameters("CHANNELS")	->asGrid();
	pBasins		= Parameters("BASINS")		->asGrid();
	pSubBasins	= Parameters("SUBBASINS")	->asGrid();
	pVBasins	= Parameters("V_BASINS")	->asShapes();
	pVSubBasins	= Parameters("V_SUBBASINS")	->asShapes();
	pHeads		= Parameters("HEADS")		->asShapes();
	pMouths		= Parameters("MOUTHS")		->asShapes();

	//-----------------------------------------------------
	Inflows		.Create(*Get_System(), SG_DATATYPE_Char);

	m_Direction	.Create(*Get_System(), SG_DATATYPE_Char);
	m_Direction	.Set_NoData_Value(-1);

	m_Distance	.Create(*Get_System(), SG_DATATYPE_Float);
	m_Distance	.Set_NoData_Value(-1);
	m_Distance	.Assign_NoData();

	pBasins		->Assign(0.0);
	pBasins		->Set_NoData_Value(0.0);

	pSubBasins	->Assign(0.0);
	pSubBasins	->Set_NoData_Value(0.0);

	pHeads		->Create(SHAPE_TYPE_Point	, _TL("Heads"));
	pHeads		->Add_Field("ID"			, SG_DATATYPE_Int);
	pHeads		->Add_Field("MAIN_ID"		, SG_DATATYPE_Int);
	pHeads		->Add_Field("ELEVATION"		, SG_DATATYPE_Double);
	pHeads		->Add_Field("DISTANCE"		, SG_DATATYPE_Double);

	pMouths		->Create(SHAPE_TYPE_Point	, _TL("Mouths"));
	pMouths		->Add_Field("ID"			, SG_DATATYPE_Int);
	pMouths		->Add_Field("MAIN_ID"		, SG_DATATYPE_Int);
	pMouths		->Add_Field("ELEVATION"		, SG_DATATYPE_Double);

	pVBasins	->Create(SHAPE_TYPE_Polygon	, _TL("Basins"));
	BASIN_ADD_FIELDS(pVBasins);

	pVSubBasins	->Create(SHAPE_TYPE_Polygon	, _TL("Subbasins"));
	BASIN_ADD_FIELDS(pVSubBasins);

	//-----------------------------------------------------
	Process_Set_Text(_TL("flow directions..."));

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			int	Direction	= -1;

			if( m_pDEM->is_InGrid(x, y) )
			{
				double	dMax		= 0.0;

				for(int i=0; i<8; i++)
				{
					int	ix	= Get_xTo(i, x);
					int	iy	= Get_yTo(i, y);

					if( m_pDEM->is_InGrid(ix, iy) && !m_pChannels->is_NoData(ix, iy) )
					{
						double	dz	= (m_pDEM->asDouble(x, y) - m_pDEM->asDouble(ix, iy)) / Get_Length(i);

						if( dMax < dz )
						{
							dMax		= dz;
							Direction	= i;
						}
					}
				}

				if( !m_pChannels->is_NoData(x, y) )
				{
					if( Direction >= 0 )
					{
						int	ix	= Get_xTo(Direction, x);
						int	iy	= Get_yTo(Direction, y);

						if( m_pDEM->is_InGrid(ix, iy) )
						{
							Inflows.Add_Value(ix, iy, 1);
						}
					}
				}
				else if( Direction < 0 )
				{
					Direction	= m_pDEM->Get_Gradient_NeighborDir(x, y);
				}
			}

			m_Direction.Set_Value(x, y, Direction);
		}
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("main basins..."));

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( m_pChannels->is_InGrid(x, y) && is_Outlet(x, y) )
			{
				Get_Basin(pBasins, pVBasins, x, y, -1);

				CSG_Shape	*pMouth	= pMouths->Add_Shape();

				pMouth->Add_Point(Get_System()->Get_Grid_to_World(x, y));

				pMouth->Set_Value(0, pVBasins->Get_Count());	// ID
				pMouth->Set_Value(1, pVBasins->Get_Count());	// MAIN_ID
				pMouth->Set_Value(2, m_pDEM->asDouble(x, y));	// ELEVATION
			}
		}
	}

	if( Parameters("DISTANCE")->asBool() )
	{
		m_Distance.Set_Name(_TL("Flow Distance"));

		DataObject_Add(SG_Create_Grid(m_Distance));
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("heads and mouths..."));

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( m_pChannels->is_InGrid(x, y) )
			{
				//-----------------------------------------
				if( Inflows.asInt(x, y) > 1 )		// mouth, linking channels of subcatchments
				{
					CSG_Shape	*pMouth	= pMouths->Add_Shape();

					pMouth->Add_Point(Get_System()->Get_Grid_to_World(x, y));

					pMouth->Set_Value(0, pMouths->Get_Count());		// ID
					pMouth->Set_Value(1, pBasins->asDouble(x, y));	// MAIN_ID
					pMouth->Set_Value(2, m_pDEM->asDouble(x, y));	// ELEVATION
				}

				//-----------------------------------------
				else if( Inflows.asInt(x, y) == 0 )	// head
				{
					CSG_Shape	*pHead	= pHeads->Add_Shape();

					pHead->Add_Point(Get_System()->Get_Grid_to_World(x, y));

					pHead->Set_Value(0, pHeads->Get_Count() + 1);
					pHead->Set_Value(1, m_Distance.asDouble(x, y));
				}
			}
		}
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("subbasins..."));

	pMouths->Set_Index(1, TABLE_INDEX_Ascending, 2, TABLE_INDEX_Descending);

	for(int iMouth=0; iMouth<pMouths->Get_Count() && Set_Progress(iMouth, pMouths->Get_Count()); iMouth++)
	{
		CSG_Shape	*pMouth	= pMouths->Get_Shape_byIndex(iMouth);

		if( Get_System()->Get_World_to_Grid(x, y, pMouth->Get_Point(0)) )
		{
			if( pMouth->asInt(0) == pMouth->asInt(1) )
			{
				Get_Basin(pSubBasins, pVSubBasins, x, y, pMouth->asInt(1));
			}
			else
			{
				for(int i=0; i<8; i++)
				{
					int	ix	= Get_xFrom(i, x);
					int	iy	= Get_yFrom(i, y);

					if( m_pChannels->is_InGrid(ix, iy) && m_Direction.asInt(ix, iy) == i )
					{
						Get_Basin(pSubBasins, pVSubBasins, ix, iy, pMouth->asInt(1));
					}
				}
			}
		}
	}

	if( Parameters("DISTANCE")->asBool() )
	{
		m_Distance.Set_Name(_TL("Subbasin Flow Distance"));

		DataObject_Add(SG_Create_Grid(m_Distance));
	}

	//-----------------------------------------------------
	m_Distance	.Destroy();
	m_Direction	.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline bool CWatersheds_ext::is_Outlet(int x, int y)
{
	int	Direction	= m_Direction.asInt(x, y);

	if( Direction >= 0 )
	{
		int	ix	= Get_xTo(Direction, x);
		int	iy	= Get_yTo(Direction, y);

		if( m_pDEM->is_InGrid(ix, iy) )
		{
			return( false );
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWatersheds_ext::Get_Basin(CSG_Grid *pBasins, CSG_Shapes *pPolygons, int xMouth, int yMouth, int Main_ID)
{
	int						x, y, Basin_ID	= 1 + pPolygons->Get_Count();
	CSG_Shape				*pPolygon;
	CSG_Grid_Stack			Stack;
	CSG_Simple_Statistics	s_Height, s_Distance;

	//-----------------------------------------------------
	Stack.Push(x = xMouth, y = yMouth);

	pBasins		->Set_Value(x, y, Basin_ID);
	m_Distance	 .Set_Value(x, y, 0.0);

	s_Height	+= m_pDEM->asDouble(x, y);
	s_Distance	+= 0.0;

	//-----------------------------------------------------
	while( Stack.Get_Size() > 0 && Process_Get_Okay() )
	{
		Stack.Pop(x, y);

		double	d	= m_Distance.asDouble(x, y);

		//-------------------------------------------------
		for(int i=0; i<8; i++)
		{
			int	ix	= Get_xFrom(i, x);
			int	iy	= Get_yFrom(i, y);

			if( is_InGrid(ix, iy) && pBasins->is_NoData(ix, iy) && i == m_Direction.asInt(ix, iy) )
			{
				Stack.Push(ix, iy);

				pBasins		->Set_Value(ix, iy, Basin_ID);
				m_Distance	 .Set_Value(ix, iy, d + Get_Length(i));

				s_Height	+= m_pDEM->asDouble(ix, iy);
				s_Distance	+= d + Get_Length(i);
			}
		}
	}

	//-----------------------------------------------------
	if( s_Height.Get_Count() > 1 && (pPolygon = Get_Basin(pBasins, pPolygons)) != NULL )
	{
		double		d, Area, Perimeter, Side_A, Side_B;
		CSG_String	Gravelius;

	//	Area		= s_Height.Get_Count() * Get_System()->Get_Cellarea();
		Area		= ((CSG_Shape_Polygon*)pPolygon)->Get_Area();
		Perimeter	= ((CSG_Shape_Polygon*)pPolygon)->Get_Perimeter();

		d			= 0.28 * Perimeter / sqrt(Area);
		Gravelius	= d > 1.75 ? _TL("rectangular")
					: d > 1.5  ? _TL("ovalooblonga-rectangularoblonga")
					: d > 1.25 ? _TL("ovaloredonda-ovalooblonga")
					:            _TL("redonda-ovaloredonda");

		d			= pow(Perimeter, 2.0) - 8.0 * Area;
		Side_A		= d > 0.0 ? (Perimeter + sqrt(d))      / 4.0 : -1.0;
		Side_B		= d > 0.0 ? (Perimeter - 2.0 * Side_A) / 2.0 : -1.0;

		pPolygon->Set_Value(FIELD_ID			, Basin_ID);
		pPolygon->Set_Value(FIELD_ID_MAIN		, Main_ID);

		pPolygon->Set_Value(FIELD_MOUTH_X		, Get_System()->Get_xGrid_to_World(xMouth));
		pPolygon->Set_Value(FIELD_MOUTH_Y		, Get_System()->Get_yGrid_to_World(yMouth));

		pPolygon->Set_Value(FIELD_PERIMETER		, Perimeter);
		pPolygon->Set_Value(FIELD_AREA			, Area);

		pPolygon->Set_Value(FIELD_CENTROID_X	, ((CSG_Shape_Polygon*)pPolygon)->Get_Centroid().x);
		pPolygon->Set_Value(FIELD_CENTROID_Y	, ((CSG_Shape_Polygon*)pPolygon)->Get_Centroid().y);

		pPolygon->Set_Value(FIELD_Z_MEAN		, s_Height  .Get_Mean());
		pPolygon->Set_Value(FIELD_Z_RANGE		, s_Height  .Get_Range());

		pPolygon->Set_Value(FIELD_DIST_MEAN		, s_Distance.Get_Mean());
		pPolygon->Set_Value(FIELD_DIST_MAX		, s_Distance.Get_Maximum());

		pPolygon->Set_Value(FIELD_CONCTIME		, s_Height.Get_Range() <= 0.0 ? -1.0 :
			pow(0.87 * pow(s_Distance.Get_Maximum() / 1000.0, 3.0) / s_Height.Get_Range(),  0.385)
		);

		pPolygon->Set_Value(FIELD_BASIN_TYPE	, Gravelius);

		pPolygon->Set_Value(FIELD_EQVRECT_A		, Side_A);
		pPolygon->Set_Value(FIELD_EQVRECT_B		, Side_B);

		pPolygon->Set_Value(FIELD_OROG_IDX		, SG_Get_Square(s_Height.Get_Mean()) / (0.0001 * Area));	// Orographic index, defined as the mean catchment altitude times the ratio of the mean catchment altitude to the orthogonal projection of drainage area (Alcázar, Palau (2010): Establishing environmental flow regimes in a Mediterranean watershed based on a regional classification. Journal of Hydrology, V. 388
		pPolygon->Set_Value(FIELD_MASS_IDX		, Perimeter / (0.0001 * Area));								// Perimeter / (0.0001 * Area) ??!!

		pPolygon->Set_Value(FIELD_BASINS_UP		, 0.0);	// Upslope Basins
		pPolygon->Set_Value(FIELD_BASINS_DOWN	, 0.0);	// Downslope Basins

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Shape * CWatersheds_ext::Get_Basin(CSG_Grid *pBasins, CSG_Shapes *pPolygons)
{
	int			x, y, nEdges, Basin_ID;
	CSG_Grid	Edge;
	CSG_Shape	*pPolygon	= NULL;

	Basin_ID	= 1 + pPolygons->Get_Count();

	//-----------------------------------------------------
	Edge.Create(SG_DATATYPE_Char, 2 * Get_NX() + 1, 2 * Get_NY() + 1, 0.5 * Get_Cellsize(), Get_XMin() - 0.5 * Get_Cellsize(), Get_YMin() - 0.5 * Get_Cellsize());
	Edge.Set_NoData_Value(0);

	for(y=0, nEdges=0; y<Get_NY() && Process_Get_Okay(); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( pBasins->asInt(x, y) == Basin_ID )
			{
				for(int i=0; i<8; i+=2)
				{
					int	ix	= Get_xTo(i, x);
					int	iy	= Get_yTo(i, y);

					if( !is_InGrid(ix, iy) || pBasins->asInt(ix, iy) != Basin_ID )
					{
						ix	= 1 + 2 * x;
						iy	= 1 + 2 * y;

						Edge.Set_Value(               ix,                 iy ,  1);
						Edge.Set_Value(Get_xTo(i    , ix), Get_yTo(i    , iy), -1);
						Edge.Set_Value(Get_xTo(i - 1, ix), Get_yTo(i - 1, iy), -1);

						nEdges++;
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	if( nEdges > 0 )
	{
		for(int yEdge=0; yEdge<Edge.Get_NY(); yEdge++)	for(int xEdge=0; xEdge<Edge.Get_NX(); xEdge++)
		{
			int	i	= 4;

			if( Edge.asInt(xEdge, yEdge) == 1 && Edge.asInt(Get_xTo(i, xEdge), Get_yTo(i, yEdge)) == -1 )
			{
				if( pPolygon == NULL )
				{
					pPolygon	= pPolygons->Add_Shape();
				}

				int	iPart	= pPolygon->Get_Part_Count();
				int	xFirst	= x	= Get_xTo(i, xEdge);
				int	yFirst	= y	= Get_yTo(i, yEdge);
				i	= i + 2;

				pPolygon	->Add_Point(Edge.Get_System().Get_Grid_to_World(x, y), iPart);

				do
				{
					int	ix	= Get_xTo(i + 2, x);
					int	iy	= Get_yTo(i + 2, y);

					if( Edge.is_InGrid(ix, iy) && Edge.asInt(ix, iy) == -1 )			// go right ?
					{
						pPolygon->Add_Point(Edge.Get_System().Get_Grid_to_World(x, y), iPart);

						i	= (i + 2) % 8;
					}
					else
					{
						if( Edge.asInt(ix, iy) == 1 )
						{
							Edge.Set_NoData(ix, iy);	// erase class id in right cells
						}

						ix	= Get_xTo(i, x);
						iy	= Get_yTo(i, y);

						if( Edge.is_InGrid(ix, iy) && Edge.asInt(ix, iy) == -1 )		// go ahead ?
						{
							// nop
						}
						else
						{
							ix	= Get_xTo(i + 6, x);
							iy	= Get_yTo(i + 6, y);

							if( Edge.is_InGrid(ix, iy) && Edge.asInt(ix, iy) == -1 )	// go left ?
							{
								pPolygon->Add_Point(Edge.Get_System().Get_Grid_to_World(x, y), iPart);

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

				pPolygon->Add_Point(Edge.Get_System().Get_Grid_to_World(x, y), iPart);
			}
		}
	}

	//-----------------------------------------------------
	return( pPolygon );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

/*/---------------------------------------------------------
void CWatersheds_ext::CreateShapesLayer(void)
{ //first shape (0) is the whole basin. 
	CSG_Table_Record *pRecord, *pRecord2;
	CSG_Shape *pSubbasin;
	TSG_Point	Point;	
	double fArea=0, fPerim=0;
	double fSide1, fSide2;
	double fConcTime;
	double fMinHeight;
	int i,j,k;	
	int x,y;
	int iNextX, iNextY;
	int iX, iY;
	int iXOrig, iYOrig;
	int iIndex;
	int iUpstreamBasin;
	int iDownstreamBasin;
	int iCode;
	CSG_String sSubbasins;
		
	CSG_Points_Int		m_Heads;

	
	//-----------------------------------------------------
	//upstream and downstream basins

	CSG_Shapes	*pBasins	= Parameters("V_BASINS")->asShapes();

	//-----------------------------------------------------
	TSG_Point_Int	m_Closing;

	for(i=0; i<m_Heads.Get_Count(); i++)
	{
		TSG_Point_Int	Point, Next	= m_Heads[i];

		do
		{
		//	Get_Next(m_pDEM, m_pChannels, Point = Next, Next);

			iDownstreamBasin	= m_pBasins->asInt(Next .x, Next .y);
			iUpstreamBasin		= m_pBasins->asInt(Point.x, Point.y);

			if( iUpstreamBasin != iDownstreamBasin && iDownstreamBasin != 0 && iUpstreamBasin != 0 )
			{
				for(j=0; j<pBasins->Get_Count(); j++)
				{
					pRecord	= pBasins->Get_Record(j);

					iCode	= pRecord->asInt(0);

					if( iCode == iUpstreamBasin )
					{
						pRecord->Set_Value(9, iDownstreamBasin);
						pRecord->Set_Value(1, Point.x * m_pDEM->Get_Cellsize() + m_pDEM->Get_XMin());						
						pRecord->Set_Value(2, Point.y * m_pDEM->Get_Cellsize() + m_pDEM->Get_YMin());
					}
				}
			}
		}
		while( !(Point.x == m_Closing.x && Point.y == m_Closing.y) && (Point.x != Next.x || Point.y != Next.y) );
	}// for

	//-----------------------------------------------------
	for (i = 1; i<pBasins->Get_Count(); i++)
	{
		pBasins->Get_Record(i)->Set_Value(8,SG_T(" --- "));
		iCode = pBasins->Get_Record(i)->asInt(0);

		for (j = 0; j<pBasins->Get_Count(); j++)
		{
			iDownstreamBasin = pBasins->Get_Record(j)->asInt(9);

			if (iDownstreamBasin == iCode)
			{
				sSubbasins = CSG_String(pBasins->Get_Record(i)->asString(8)) + SG_T(" ")
							+ SG_Get_String(pBasins->Get_Record(j)->asInt(0));

				pBasins->Get_Record(i)->Set_Value(8, sSubbasins.c_str());
			}//if
		}//for
	}//for

	//-----------------------------------------------------
}//*/


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

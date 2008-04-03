
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
		NULL, "GRID"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes(
		NULL, "SHAPES"		, _TL("Shapes"),
		_TL(""),
		PARAMETER_OUTPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Choice(
		NULL, "SPLIT"		, _TL("Vectorised class as..."),
		_TL(""),

		CSG_String::Format(SG_T("%s|%s|"),
			_TL("one single polygon object"),
			_TL("each island as separated polygon")
		), 0
	);
}

//---------------------------------------------------------
CGrid_Classes_To_Shapes::~CGrid_Classes_To_Shapes(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Classes_To_Shapes::On_Execute(void)
{
	bool			bSplit;
	CSG_Parameters	gParms, sParms;
	CSG_Shapes		*pShapes, Shapes(SHAPE_TYPE_Polygon);

	//-----------------------------------------------------
	m_pGrid		= Parameters("GRID")	->asGrid();
	pShapes		= Parameters("SHAPES")	->asShapes();
	bSplit		= Parameters("SPLIT")	->asInt() == 1;

	//-----------------------------------------------------
	pShapes->Create(SHAPE_TYPE_Polygon, m_pGrid->Get_Name());
	pShapes->Get_Table().Add_Field(_TL("ID")			, TABLE_FIELDTYPE_Int);
	pShapes->Get_Table().Add_Field(m_pGrid->Get_Name()	, TABLE_FIELDTYPE_Double);
	pShapes->Get_Table().Add_Field(_TL("Name")			, TABLE_FIELDTYPE_String);

	if(	DataObject_Get_Parameters(m_pGrid, gParms) && gParms("COLORS_TYPE") && gParms("LUT")
	&&	DataObject_Get_Parameters(pShapes, sParms) && sParms("COLORS_TYPE") && sParms("LUT") && sParms("COLORS_ATTRIB") )
	{
		sParms("LUT")->asTable()->Assign_Values(gParms("LUT")->asTable());	// Lookup Table
		sParms("COLORS_TYPE")->Set_Value(gParms("COLORS_TYPE")->asInt());	// Color Classification Type: Lookup Table
		sParms("COLORS_ATTRIB")->Set_Value(1);								// Color Attribute
		DataObject_Set_Parameters(pShapes, sParms);
	}

	//-----------------------------------------------------
	m_pShape	= NULL;

	Lock_Create();

	m_Edge.Create(GRID_TYPE_Byte, 2 * Get_NX() + 1, 2 * Get_NY() + 1, 0.5 * Get_Cellsize(), Get_XMin() - 0.5 * Get_Cellsize(), Get_YMin() - 0.5 * Get_Cellsize());

	for(int y=0, nClasses=0; y<Get_NY() && Process_Get_Okay(false); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( !m_pGrid->is_NoData(x, y) && !Lock_Get(x, y) )
			{
				//-----------------------------------------
				double		Value	= m_pGrid->asDouble(x, y);
				CSG_String	ID		= CSG_String::Format(SG_T("%d"), ++nClasses);

				if( bSplit )
				{
					if( m_pShape == NULL )
						m_pShape	= Shapes.Add_Shape();

					m_pShape->Del_Parts();
				}
				else
				{
					m_pShape	= pShapes->Add_Shape();
					m_pShape->Get_Record()->Set_Value(0, pShapes->Get_Count());
					m_pShape->Get_Record()->Set_Value(1, Value);
					m_pShape->Get_Record()->Set_Value(2, ID);
				}

				//-----------------------------------------
				Process_Set_Text(CSG_String::Format(SG_T("%s %d: %f"), _TL("vectorising class"), nClasses, Value));

				Get_Class(Value);

				//-----------------------------------------
				if( bSplit )
				{
					Split_Polygons(pShapes, Value, ID);
				}
			}
		}
	}

	Lock_Destroy();

	m_Edge.Destroy();

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Classes_To_Shapes::Split_Polygons(CSG_Shapes *pShapes, double Value, const CSG_String &ID)
{
	if( m_pShape && m_pShape->Get_Part_Count() > 0 )
	{
		bool		*bLake	= (bool *)SG_Malloc(m_pShape->Get_Part_Count() * sizeof(bool));
		int			iPart, iPoint, jPart, iShape;
		CSG_Shape	*pShape;

		for(iPart=0; iPart<m_pShape->Get_Part_Count() && Set_Progress(iPart, m_pShape->Get_Part_Count()); iPart++)
		{
			if( (bLake[iPart] = ((CSG_Shape_Polygon *)m_pShape)->is_Lake(iPart)) == false )
			{
				pShape	= pShapes->Add_Shape();
				pShape->Get_Record()->Set_Value(0, pShapes->Get_Count());
				pShape->Get_Record()->Set_Value(1, Value);
				pShape->Get_Record()->Set_Value(2, ID);

				for(iPoint=0; iPoint<m_pShape->Get_Point_Count(iPart); iPoint++)
				{
					pShape->Add_Point(m_pShape->Get_Point(iPoint, iPart), 0);
				}
			}
		}

		for(iPart=0; iPart<m_pShape->Get_Part_Count() && Set_Progress(iPart, m_pShape->Get_Part_Count()); iPart++)
		{
			if( bLake[iPart] == true )
			{
				TSG_Point	p	= m_pShape->Get_Point(0, iPart);

				for(iShape=0, jPart=0; jPart==0 && iShape<pShapes->Get_Count(); iShape++)
				{
					pShape	= pShapes->Get_Shape(iShape);

					if( pShape->Get_Record()->asDouble(1) == Value && ((CSG_Shape_Polygon *)pShape)->is_Containing(p, 0) )
					{
						jPart	= pShape->Get_Part_Count();

						for(iPoint=0; iPoint<m_pShape->Get_Point_Count(iPart); iPoint++)
						{
							pShape->Add_Point(m_pShape->Get_Point(iPoint, iPart), jPart);
						}
					}
				}
			}
		}

		SG_Free(bLake);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Classes_To_Shapes::Get_Class(double Value)
{
	int		x, y, i, ix, iy, n, nEdgeCells;

	//-----------------------------------------------------
	for(y=0, nEdgeCells=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( !Lock_Get(x, y) && m_pGrid->asDouble(x, y) == Value )
			{
				Lock_Set(x, y);

				for(i=0, n=0; i<8; i+=2)
				{
					ix	= Get_xTo(i, x);
					iy	= Get_yTo(i, y);

					if( !m_pGrid->is_InGrid(ix, iy) || m_pGrid->asDouble(ix, iy) != Value )
					{
						ix	= Get_xTo(i    , 1 + 2 * x);
						iy	= Get_yTo(i    , 1 + 2 * y);
						m_Edge.Add_Value(ix, iy, 1);

						ix	= Get_xTo(i + 1, 1 + 2 * x);
						iy	= Get_yTo(i + 1, 1 + 2 * y);
						m_Edge.Add_Value(ix, iy, 1);

						n++;
					}
				}

				//-----------------------------------------
				if( n == 4 )
				{
					Get_Square(1 + 2 * x, 1 + 2 * y);
				}
				else
				{
					nEdgeCells++;
				}
			}
		}
	}

	//-----------------------------------------------------
	if( nEdgeCells > 0 )
	{
		Get_Polygons();
	}

	return( m_pShape->Get_Part_Count() > 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CGrid_Classes_To_Shapes::Get_Square(int x, int y)
{
	int		i, ix, iy, iPart;

	iPart	= m_pShape->Get_Part_Count();

	for(i=0; i<8; i++)
	{
		ix	= Get_xTo(i, x);
		iy	= Get_yTo(i, y);

		if( m_Edge.is_InGrid(ix, iy) && m_Edge.asInt(ix, iy) )
		{
			m_Edge.Add_Value(ix, iy, -1);
		}

		if( i % 2 )
		{
			m_pShape->Add_Point(m_Edge.Get_System().Get_Grid_to_World(ix, iy), iPart);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Classes_To_Shapes::Get_Polygons(void)
{
	if( m_pShape )
	{
		int		x, y, i, ix, iy, iPart;

		for(y=0; y<m_Edge.Get_NY() && Set_Progress(y, m_Edge.Get_NY()); y++)
		{
			for(x=0; x<m_Edge.Get_NX(); x++)
			{
				if( m_Edge.asInt(x, y) )
				{
					iPart	= m_pShape->Get_Part_Count();

					for(i=0; i<8; i+=2)
					{
						ix	= Get_xTo(i, x);
						iy	= Get_yTo(i, y);

						if( m_Edge.is_InGrid(ix, iy) && m_Edge.asInt(ix, iy) )
						{
							m_pShape->Add_Point(m_Edge.Get_System().Get_Grid_to_World(x, y), iPart);

							Get_Polygon(ix, iy, iPart, i);
							break;
						}
					}
				}
			}
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
void CGrid_Classes_To_Shapes::Get_Polygon(int x, int y, int iPart, int iDirection)
{
	bool	bCorner;
	int		i, iDir, ix, iy;

	m_Edge.Add_Value(x, y, -1);

	bCorner	= m_Edge.asInt(x, y) > 0;

	for(iDir=iDirection; iDir<iDirection + 8; iDir+=2)
	{
		i	= iDir % 8;

		if( !bCorner || i != iDirection )
		{
			ix	= Get_xTo(i, x);
			iy	= Get_yTo(i, y);

			if( m_Edge.is_InGrid(ix, iy) && m_Edge.asInt(ix, iy) )
			{
				if( i != iDirection )
				{
					m_pShape->Add_Point(m_Edge.Get_System().Get_Grid_to_World(x, y), iPart);
				}

				Get_Polygon(ix, iy, iPart, i);

				return;
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

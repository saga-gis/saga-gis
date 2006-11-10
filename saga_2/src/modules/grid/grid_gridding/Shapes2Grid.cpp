
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     Grid_Gridding                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    Shapes2Grid.cpp                    //
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
#include <string.h>
#include "Shapes2Grid.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CShapes2Grid::CShapes2Grid(void)
{
	CSG_Parameter	*pNode_0, *pNode_1;
	CSG_Parameters	*pParameters;

	//-----------------------------------------------------
	Set_Name(_TL("Shapes to Grid"));

	Set_Author(_TL("Copyrights (c) 2003 by Olaf Conrad"));

	Set_Description(_TL(
		"Gridding of a shapes layer.")
	);


	//-----------------------------------------------------
	Parameters.Add_Grid_Output(
		NULL	, "GRID"		, _TL("Grid"),
		""
	);

	pNode_0	= Parameters.Add_Shapes(
		NULL	, "INPUT"		, _TL("Shapes"),
		"",
		PARAMETER_INPUT
	);

	pNode_1	= Parameters.Add_Table_Field(
		pNode_0	, "FIELD"		, _TL("Attribute"),
		""
	);

	pNode_0	= Parameters.Add_Choice(
		NULL	, "TARGET_TYPE"	, _TL("Target Dimensions"),
		"",

		CSG_String::Format("%s|%s|%s|",
			_TL("User defined"),
			_TL("Grid Project"),
			_TL("Grid")
		), 0
	);

	//-----------------------------------------------------
	pParameters	= Add_Parameters("USER", _TL("User defined grid"), "");

	pNode_0	= pParameters->Add_Value(
		NULL	, "CELL_SIZE"	, _TL("Grid Size"),
		"",
		PARAMETER_TYPE_Double, 100.0, 0.0, true
	);

	pNode_0	= pParameters->Add_Value(
		NULL	, "FIT_EXTENT"	, _TL("Fit Extent"),
		_TL("Automatically fits the grid to the shapes layers extent."),
		PARAMETER_TYPE_Bool		, true
	);

	pNode_1	= pParameters->Add_Range(
		pNode_0	, "X_EXTENT"	, _TL("X-Extent"),
		""
	);

	pNode_1	= pParameters->Add_Range(
		pNode_0	, "Y_EXTENT"	, _TL("Y-Extent"),
		""
	);

	pNode_0	= pParameters->Add_Choice(
		NULL	, "GRID_TYPE"	, _TL("Target Grid Type"),
		"",
		CSG_String::Format("%s|%s|%s|%s|%s|",
			_TL("Integer (1 byte)"),
			_TL("Integer (2 byte)"),
			_TL("Integer (4 byte)"),
			_TL("Floating Point (4 byte)"),
			_TL("Floating Point (8 byte)")
		), 3
	);

	//-----------------------------------------------------
	pParameters	= Add_Parameters("GET_SYSTEM"	, _TL("Choose Grid Project"), "");

	pNode_0	= pParameters->Add_Grid_System(
		NULL, "SYSTEM"		, _TL("System")		, ""
	);

	pNode_0	= pParameters->Add_Choice(
		NULL	, "GRID_TYPE"	, _TL("Target Grid Type"),
		"",
		CSG_String::Format("%s|%s|%s|%s|%s|",
			_TL("Integer (1 byte)"),
			_TL("Integer (2 byte)"),
			_TL("Integer (4 byte)"),
			_TL("Floating Point (4 byte)"),
			_TL("Floating Point (8 byte)")
		), 3
	);

	//-----------------------------------------------------
	pParameters	= Add_Parameters("GRID"			, _TL("Choose Grid")		, "");

	pNode_0	= pParameters->Add_Grid(
		NULL	, "GRID"		, _TL("Grid"),
		"",
		PARAMETER_INPUT	, false
	);
}

//---------------------------------------------------------
CShapes2Grid::~CShapes2Grid(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
TSG_Grid_Type CShapes2Grid::Get_Grid_Type(int iType)
{
	switch( iType )
	{
	case 0:
		return( GRID_TYPE_Byte );

	case 1:
		return( GRID_TYPE_Short );

	case 2:
		return( GRID_TYPE_Int );

	case 3: default:
		return( GRID_TYPE_Float );

	case 4:
		return( GRID_TYPE_Double );
	}
}

//---------------------------------------------------------
CSG_Grid * CShapes2Grid::Get_Target_Grid(CSG_Parameters *pParameters, CSG_Shapes *pShapes)
{
	int			nx, ny;
	double		Cell_Size, xMin, yMin, xMax, yMax;

	if( pParameters->Get_Parameter("FIT_EXTENT")->asBool() )
	{
		xMin	= pShapes->Get_Extent().m_rect.xMin;
		yMin	= pShapes->Get_Extent().m_rect.yMin;
		xMax	= pShapes->Get_Extent().m_rect.xMax;
		yMax	= pShapes->Get_Extent().m_rect.yMax;
	}
	else
	{
		xMin	= pParameters->Get_Parameter("X_EXTENT")->asRange()->Get_LoVal();
		yMin	= pParameters->Get_Parameter("Y_EXTENT")->asRange()->Get_LoVal();
		xMax	= pParameters->Get_Parameter("X_EXTENT")->asRange()->Get_HiVal();
		yMax	= pParameters->Get_Parameter("Y_EXTENT")->asRange()->Get_HiVal();
	}

	Cell_Size	= pParameters->Get_Parameter("CELL_SIZE")->asDouble();

	nx			= 1 + (int)((xMax - xMin) / Cell_Size);
	ny			= 1 + (int)((yMax - yMin) / Cell_Size);

	return( SG_Create_Grid(Get_Grid_Type(pParameters->Get_Parameter("GRID_TYPE")->asInt()), nx, ny, Cell_Size, xMin, yMin) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CShapes2Grid::On_Execute(void)
{
	int		i, iField;
	CSG_Shape	*pShape;

	//-----------------------------------------------------
	pShapes	= Parameters("INPUT")->asShapes();

	if( pShapes->Get_Table().Get_Field_Count() > 0 )
	{
		pGrid	= NULL;

		switch( Parameters("TARGET_TYPE")->asInt() )
		{
		case 0:	// User defined...
			if( Dlg_Parameters("USER") )
			{
				pGrid	= Get_Target_Grid(Get_Parameters("USER"), pShapes);
			}
			break;

		case 1:	// Grid Project...
			if( Dlg_Parameters("GET_SYSTEM") )
			{
				pGrid	= SG_Create_Grid(
							*Get_Parameters("GET_SYSTEM")->Get_Parameter("SYSTEM")->asGrid_System(),
							Get_Grid_Type(Get_Parameters("GET_SYSTEM")->Get_Parameter("GRID_TYPE")->asInt())
						);
			}
			break;

		case 2:	// Grid...
			if( Dlg_Parameters("GRID") )
			{
				pGrid	= Get_Parameters("GRID")->Get_Parameter("GRID")->asGrid();
			}
			break;
		}

		//-------------------------------------------------
		if( pGrid )
		{
			pGrid->Assign_NoData();
			Parameters("GRID")->Set_Value(pGrid);

			iField	= Parameters("FIELD")->asInt();

			//-------------------------------------------------
			if( iField >= 0 && iField < pShapes->Get_Table().Get_Field_Count()
			&&	pShapes->Get_Table().Get_Field_Type(iField) != TABLE_FIELDTYPE_String )
			{
				pGrid->Set_Name(CSG_String::Format("%s [%s]", pShapes->Get_Name(), pShapes->Get_Table().Get_Field_Name(iField)));

				for(i=0; i<pShapes->Get_Count() && Set_Progress(i, pShapes->Get_Count()); i++)
				{
					pShape	= pShapes->Get_Shape(i);

					switch( pShapes->Get_Type() )
					{
					case SHAPE_TYPE_Point:
						Gridding_Point	(pShape, pShape->Get_Record()->asDouble(iField));
						break;

					case SHAPE_TYPE_Line:
						Gridding_Line	(pShape, pShape->Get_Record()->asDouble(iField));
						break;

					case SHAPE_TYPE_Polygon:
						Gridding_Polygon(pShape, pShape->Get_Record()->asDouble(iField));
						break;
					}
				}
			}

			//-------------------------------------------------
			else
			{
				pGrid->Set_Name(pShapes->Get_Name());

				for(i=0; i<pShapes->Get_Count() && Set_Progress(i, pShapes->Get_Count()); i++)
				{
					pShape	= pShapes->Get_Shape(i);

					switch( pShapes->Get_Type() )
					{
					case SHAPE_TYPE_Point:
						Gridding_Point	(pShape, i + 1);
						break;

					case SHAPE_TYPE_Line:
						Gridding_Line	(pShape, i + 1);
						break;

					case SHAPE_TYPE_Polygon:
						Gridding_Polygon(pShape, i + 1);
						break;
					}
				}
			}
		}

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
void CShapes2Grid::Gridding_Point(CSG_Shape *pShape, double Value)
{
	int			iPart, iPoint, x, y;
	TSG_Point	p;

	for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
	{
		for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
		{
			p	= pShape->Get_Point(iPoint, iPart);

			x	= (int)((p.x - pGrid->Get_XMin()) / pGrid->Get_Cellsize());
			y	= (int)((p.y - pGrid->Get_YMin()) / pGrid->Get_Cellsize());

			if( pGrid->is_InGrid(x, y, false) )
			{
				pGrid->Set_Value(x, y, pGrid->is_NoData(x, y) ? Value : (Value + pGrid->asDouble(x, y)) / 2.0);
			}
		}
	}
}

//---------------------------------------------------------
#define SET_VALUE(P)	if( pGrid->is_InGrid(P.x, P.y, false) )\
						{	pGrid->Set_Value(P.x, P.y, pGrid->is_NoData(P.x, P.y) ? Value : (Value + pGrid->asDouble(P.x, P.y)) / 2.0);	}

//---------------------------------------------------------
void CShapes2Grid::Gridding_Line(CSG_Shape *pShape, double Value)
{
	int				iPart, iPoint;
	TSG_Point_Int	A, B;
	TSG_Point		a, b, p, c00, c01, c10, c11;

	for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
	{
		b	= pShape->Get_Point(0, iPart);
		b.x	= (b.x - pGrid->Get_XMin()) / pGrid->Get_Cellsize();
		b.y	= (b.y - pGrid->Get_YMin()) / pGrid->Get_Cellsize();
		B.x	= (int)(0.5 + b.x);
		B.y	= (int)(0.5 + b.y);
		SET_VALUE(B);

		for(iPoint=1; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
		{
			A	= B;
			a	= b;

			b	= pShape->Get_Point(iPoint, iPart);
			b.x	= (b.x - pGrid->Get_XMin()) / pGrid->Get_Cellsize();
			b.y	= (b.y - pGrid->Get_YMin()) / pGrid->Get_Cellsize();
			B.x	= (int)(0.5 + b.x);
			B.y	= (int)(0.5 + b.y);
			SET_VALUE(B);

			while( A.x != B.x && A.y != B.y )
			{
				c00.x	= c01.x	= A.x - 0.5;
				c00.y	= c10.y	= A.y - 0.5;
				c10.x	= c11.x	= A.x + 0.5;
				c01.y	= c11.y	= A.y + 0.5;

				if(      SG_Get_Crossing(p, a, b, c00, c01, true) )
				{
					A.x--;
				}
				else if( SG_Get_Crossing(p, a, b, c10, c11, true) )
				{
					A.x++;
				}
				else if( SG_Get_Crossing(p, a, b, c00, c10, true) )
				{
					A.y--;
				}
				else if( SG_Get_Crossing(p, a, b, c01, c11, true) )
				{
					A.y++;
				}
				else
				{
					break;
				}

				SET_VALUE(A);
			}
		}
	}
}

/*/---------------------------------------------------------
void CShapes2Grid::Gridding_Line(CSG_Shape *pShape, double Value)
{
	int			iPart, iPoint, x, y, sig;
	double		xa, ya, dx, dy, ix, iy;
	TSG_Point	pa, pb;

	for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
	{
		pb	= pShape->Get_Point(0, iPart);

		for(iPoint=1; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
		{
			pa	= pb;
			pb	= pShape->Get_Point(iPoint, iPart);


			xa	=  (pa.x - pGrid->Get_XMin()) / (double)pGrid->Get_Cellsize();
			ya	=  (pa.y - pGrid->Get_YMin()) / (double)pGrid->Get_Cellsize();

			dx	= ((pb.x - pGrid->Get_XMin()) / (double)pGrid->Get_Cellsize()) - xa;
			dy	= ((pb.y - pGrid->Get_YMin()) / (double)pGrid->Get_Cellsize()) - ya;

			if( fabs(dx) > fabs(dy) )
			{
				sig	= dx < 0 ? -1 : 1;
				dx	= fabs(dx);
				dy	/= dx;

				for(ix=0; ix<=dx; ix+=0.5)
				{
					x	= (int)(xa + ix * sig);
					y	= (int)(ya + ix * dy);

					if( pGrid->is_InGrid(x, y, false) )
					{
						pGrid->Set_Value(x, y, pGrid->is_NoData(x, y) ? Value : (Value + pGrid->asDouble(x, y)) / 2.0);
					}
				}
			}
			else if( fabs(dy) >= fabs(dx) && dy != 0 )
			{
				sig	= dy < 0 ? -1 : 1;
				dy	= fabs(dy);
				dx	/= dy;

				for(iy=0; iy<=dy; iy+=0.5)
				{
					x	= (int)(xa + iy * dx);
					y	= (int)(ya + iy * sig);

					if( pGrid->is_InGrid(x, y, false) )
					{
						pGrid->Set_Value(x, y, pGrid->is_NoData(x, y) ? Value : (Value + pGrid->asDouble(x, y)) / 2.0);
					}
				}
			}
			else
			{
				x	= (int)xa;
				y	= (int)ya;

				if( pGrid->is_InGrid(x, y, false) )
				{
					pGrid->Set_Value(x, y, pGrid->is_NoData(x, y) ? Value : (Value + pGrid->asDouble(x, y)) / 2.0);
				}
			}
		}
	}
}/**/

//---------------------------------------------------------
void CShapes2Grid::Gridding_Polygon(CSG_Shape *pShape, double Value)
{
	bool		bFill, *bCrossing;

	int			x, y, ix, xStart, xStop,
				iPart, iPoint;

	double		yPos;

	TSG_Point	pLeft, pRight, pa, pb, p;

	CSG_Rect	Extent;

	//-----------------------------------------------------
	bCrossing	= (bool *)SG_Malloc(pGrid->Get_NX() * sizeof(bool));

	Extent		= pShape->Get_Extent();

	xStart		= (int)((Extent.m_rect.xMin - pGrid->Get_XMin()) / pGrid->Get_Cellsize()) - 1;
	if( xStart < 0 )
		xStart	= 0;

	xStop		= (int)((Extent.m_rect.xMax - pGrid->Get_XMin()) / pGrid->Get_Cellsize()) + 1;
	if( xStop >= pGrid->Get_NX() )
		xStop	= pGrid->Get_NX() - 1;

	pLeft.x		= pGrid->Get_XMin() - 1.0;
	pRight.x	= pGrid->Get_XMax() + 1.0;

	//-----------------------------------------------------
	for(y=0, yPos=pGrid->Get_YMin(); y<pGrid->Get_NY(); y++, yPos+=pGrid->Get_Cellsize())
	{
		if( yPos >= Extent.m_rect.yMin && yPos <= Extent.m_rect.yMax )
		{
			memset(bCrossing, 0, pGrid->Get_NX() * sizeof(bool));

			pLeft.y	= pRight.y	= yPos;

			for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				pb		= pShape->Get_Point(pShape->Get_Point_Count(iPart) - 1, iPart);

				for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					pa	= pb;
					pb	= pShape->Get_Point(iPoint, iPart);

					if(	(	(pa.y <= yPos && yPos < pb.y)
						||	(pa.y > yPos && yPos >= pb.y)	)	)
					{
						SG_Get_Crossing(p, pa, pb, pLeft, pRight, false);

						ix	= (int)((p.x - pGrid->Get_XMin()) / pGrid->Get_Cellsize() + 1.0);

						if( ix < 0)
						{
							ix	= 0;
						}
						else if( ix >= pGrid->Get_NX() )
						{
							ix	= pGrid->Get_NX() - 1;
						}

						bCrossing[ix]	= !bCrossing[ix];
					}
				}
			}

			for(x=xStart, bFill=false; x<=xStop; x++)
			{
				if( bCrossing[x] )
				{
					bFill	= !bFill;
				}

				if( bFill )
				{
					pGrid->Set_Value(x, y, Value);
				}
			}
		}
	}

	//-------------------------------------------------
	SG_Free(bCrossing);
}

/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    User Interface                     //
//                                                       //
//                    Program: SAGA                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 WKSP_Shapes_Edit.cpp                  //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
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
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Goettingen               //
//                Goldschmidtstr. 5                      //
//                37077 Goettingen                       //
//                Germany                                //
//                                                       //
//    e-mail:     oconrad@saga-gis.org                   //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "res_commands.h"
#include "res_images.h"
#include "res_dialogs.h"

#include "dc_helper.h"

#include "active.h"
#include "active_attributes.h"
#include "active_HTMLExtraInfo.h"

#include "wksp_shapes.h"

#include "wksp_table.h"
#include "view_table.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define EDIT_TICKMARK_SIZE	4


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxMenu * CWKSP_Shapes::On_Edit_Get_Menu(void)
{
	wxMenu	*pMenu;

	pMenu	= new wxMenu;

	CMD_Menu_Add_Item(pMenu, true , ID_CMD_SHAPES_EDIT_SHAPE);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_SHAPES_EDIT_ADD_SHAPE);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_SHAPES_EDIT_DEL_SHAPE);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_SHAPES_EDIT_ADD_PART);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_SHAPES_EDIT_DEL_PART);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_SHAPES_EDIT_DEL_POINT);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_SHAPES_EDIT_SEL_INVERT);

	return( pMenu );
}

//---------------------------------------------------------
TSG_Rect CWKSP_Shapes::On_Edit_Get_Extent(void)
{
	if( m_Edit_pShape )
	{
		return( m_Edit_pShape->Get_Extent() );
	}

	if( m_pShapes->Get_Selection_Count() > 0 )
	{
		return( m_pShapes->Get_Selection_Extent().m_rect );
	}

	return( m_pShapes->Get_Extent() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Shapes::On_Edit_On_Key_Down(int KeyCode)
{
	switch( KeyCode )
	{
	default:
		return( false );

	case WXK_DELETE:
		if( m_Edit_pShape )
			return( _Edit_Point_Del() );
		else
			return( _Edit_Shape_Del() );

	case WXK_RETURN:
		if( !m_Edit_pShape )
			return( _Edit_Shape_Start() );
		else
			return( _Edit_Shape_Stop(true) );

	case WXK_ESCAPE:
		return( _Edit_Shape_Stop(false) );
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Shapes::On_Edit_On_Mouse_Down(CSG_Point Point, double ClientToWorld, int Key)
{
	int		iPart, iPoint;

	//-----------------------------------------------------
	if( m_Edit_pShape )
	{
		if( m_Edit_iPart >= 0 && m_Edit_iPoint < 0 )
		{
		}
		else switch( _Edit_Shape_HitTest(Point, EDIT_TICKMARK_SIZE * ClientToWorld, iPart, iPoint) )
		{
		//-------------------------------------------------
		case 0:	default:
		case 1:
			if( m_Edit_iPart != iPart || m_Edit_iPoint != iPoint )
			{
				m_Edit_iPart	= iPart;
				m_Edit_iPoint	= iPoint;

				Update_Views(true);
			}

			return( true );

		//-------------------------------------------------
		case 2:
			m_Edit_pShape->Ins_Point(Point, iPoint, iPart);

			m_Edit_iPart	= iPart;
			m_Edit_iPoint	= iPoint;

			Update_Views(true);

			return( true );
		}
	}

	//-----------------------------------------------------
	return( false );
}

//---------------------------------------------------------
bool CWKSP_Shapes::On_Edit_On_Mouse_Up(CSG_Point Point, double ClientToWorld, int Key)
{
	CSG_Rect		rWorld(m_Edit_Mouse_Down, Point);
	CSG_Table_Record	*pRecord;
	wxFileName		FileName;


	//-----------------------------------------------------
	if( Key & MODULE_INTERACTIVE_KEY_RIGHT )
	{
		if( m_Edit_pShape && m_Edit_iPart >= 0 && m_Edit_iPoint < 0 )
		{
			m_Edit_iPart	= -1;

			Update_Views(true);

			return( true );
		}
	}

	//-----------------------------------------------------
	else if( m_Edit_pShape )
	{
		if( m_Edit_iPart >= 0 )
		{
			if( m_Edit_iPoint >= 0 )
			{
				if( Point != m_Edit_Mouse_Down )
				{
					_Edit_Snap_Point(Point, ClientToWorld);

					m_Edit_pShape->Set_Point(Point, m_Edit_iPoint, m_Edit_iPart);

					Update_Views(true);

					return( true );
				}
			}
			else
			{
				_Edit_Snap_Point(Point, ClientToWorld);

				m_Edit_pShape->Add_Point(Point, m_Edit_iPart);

				Update_Views(true);

				return( true );
			}
		}
	}

	//-----------------------------------------------------
	else
	{
		rWorld.Assign(m_Edit_Mouse_Down, Point);

		if( rWorld.Get_XRange() == 0.0 && rWorld.Get_YRange() == 0.0 )
		{
			rWorld.Inflate(2.0 * ClientToWorld, false);
		}

		g_pACTIVE->Get_Attributes()->Set_Attributes();

		pRecord	= m_pShapes->Get_Selection();
		
		m_pShapes->Select(rWorld, (Key & MODULE_INTERACTIVE_KEY_CTRL) != 0);

		if( m_pTable->Get_View() )
		{
			m_pTable->Get_View()->Update_Selection();
		}

		if( pRecord != m_pShapes->Get_Selection() )
		{
			_Edit_Set_Attributes();
		}

		pRecord	= m_pShapes->Get_Selection();

		Update_Views(true);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Shapes::On_Edit_On_Mouse_Move(wxWindow *pMap, CSG_Rect rWorld, wxPoint pt, wxPoint ptLast, int Key)
{
	if( m_Edit_pShape )
	{
		double	ClientToWorld	= rWorld.Get_XRange() / (double)pMap->GetClientSize().x;

		//-------------------------------------------------
		if( m_Edit_iPart >= 0 && (m_Edit_iPoint < 0 || Key & MODULE_INTERACTIVE_KEY_LEFT) )
		{
			if( pt.x != ptLast.x || pt.y != ptLast.y )
			{
				wxClientDC	dc(pMap);
				dc.SetLogicalFunction(wxINVERT);

				_Edit_Shape_Draw_Move(dc, rWorld, ClientToWorld, ptLast);
				_Edit_Shape_Draw_Move(dc, rWorld, ClientToWorld, pt);
			}

			return( true );
		}

		//-------------------------------------------------
		else
		{
			int			iPart, iPoint;
			CSG_Point	Point(rWorld.Get_XMin() + pt.x * ClientToWorld, rWorld.Get_YMax() - pt.y * ClientToWorld);

			switch( _Edit_Shape_HitTest(Point, EDIT_TICKMARK_SIZE * ClientToWorld, iPart, iPoint) )
			{
			case 0:	default:
				pMap->SetCursor(IMG_Get_Cursor(ID_IMG_CRS_SELECT));
				break;

			case 1:
				pMap->SetCursor(IMG_Get_Cursor(ID_IMG_CRS_EDIT_POINT_MOVE));
				break;

			case 2:
				pMap->SetCursor(IMG_Get_Cursor(ID_IMG_CRS_EDIT_POINT_ADD));
				break;
			}

			return( true );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Shapes::_Edit_Set_Attributes(void)
{
	CSG_Table_Record	*pAttribute, *pRecord;

	m_Edit_Attributes.Del_Records();

	if( (pRecord = m_pShapes->Get_Selection()) != NULL )
	{
		for(int i=0; i<m_pShapes->Get_Field_Count(); i++)
		{
			pAttribute	= m_Edit_Attributes.Add_Record();
			pAttribute->Set_Value(0, pRecord->Get_Table()->Get_Field_Name(i));
			pAttribute->Set_Value(1, pRecord->asString(i));
		}
	}

	g_pACTIVE->Get_Attributes()->Set_Attributes();

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Shapes::On_Edit_Set_Attributes(void)
{
	CSG_Table_Record	*pRecord;

	if( (pRecord = m_pShapes->Get_Selection()) != NULL )
	{
		for(int i=0; i<m_Edit_Attributes.Get_Record_Count(); i++)
		{
			pRecord->Set_Value(i, m_Edit_Attributes.Get_Record(i)->asString(1));
		}

		Update_Views(true);

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
bool CWKSP_Shapes::_Edit_Shape(void)
{
	return( m_Edit_pShape ? _Edit_Shape_Stop() : _Edit_Shape_Start() );
}

//---------------------------------------------------------
bool CWKSP_Shapes::_Edit_Shape_Start(void)
{
	if( m_Edit_pShape == NULL && m_pShapes->Get_Selection(0) != NULL )
	{
		m_Edit_pShape	= m_Edit_Shapes.Add_Shape();
		m_Edit_pShape->Assign(m_pShapes->Get_Selection(0), false);

		m_Edit_iPart	= -1;
		m_Edit_iPoint	= -1;

		Update_Views(true);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Shapes::_Edit_Shape_Stop(void)
{
	return( _Edit_Shape_Stop(DLG_Message_Confirm(LNG("[DLG] Save changes?"), LNG("[CAP] Edit Shapes"))) );
}

bool CWKSP_Shapes::_Edit_Shape_Stop(bool bSave)
{
	if( m_Edit_pShape != NULL )
	{
		if( bSave )
		{
			CSG_Shape	*pShape;

			if( (pShape = m_pShapes->Get_Selection(0)) == NULL )
			{
				if( (pShape = m_pShapes->Add_Shape()) != NULL )
				{
					m_pShapes->Select(pShape);
				}
			}

			if( pShape != NULL )
			{
				pShape->Assign(m_Edit_pShape, false);
			}
		}

		m_Edit_Shapes.Del_Shapes();
		m_Edit_pShape	= NULL;

		Update_Views(false);
		_Edit_Set_Attributes();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Shapes::_Edit_Shape_Add(void)
{
	if( !m_Edit_pShape )
	{
		if( m_pShapes->Get_Selection_Count() > 0 )
		{
			m_pShapes->Select(NULL);
		}

		m_Edit_pShape	= m_Edit_Shapes.Add_Shape();

		return( _Edit_Part_Add() );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Shapes::_Edit_Part_Add(void)
{
	if( m_Edit_pShape )
	{
		m_Edit_iPart	= m_Edit_pShape->Get_Part_Count();
		m_Edit_iPoint	= -1;

		Update_Views(true);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Shapes::_Edit_Shape_Del(void)
{
	if( DLG_Message_Confirm(LNG("[DLG] Delete selected shape(s)."), LNG("[CAP] Edit Shapes")) )
	{
		if( m_pShapes->Get_Selection_Count() > 0 )
		{
			if( m_Edit_pShape )
			{
				_Edit_Shape_Stop(false);

				m_pShapes->Del_Shape(m_pShapes->Get_Selection(0));
			}
			else
			{
				m_pShapes->Del_Selection();
			}

			Update_Views(false);

			return( true );
		}

		return( _Edit_Shape_Stop(false) );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Shapes::_Edit_Part_Del(void)
{
	if( m_Edit_pShape && m_Edit_iPart >= 0 )
	{
		if( m_Edit_pShape->Get_Part_Count() > 1 )
		{
			m_Edit_pShape->Del_Part(m_Edit_iPart);

			m_Edit_iPart	= -1;
			m_Edit_iPoint	= -1;

			Update_Views(true);

			return( true );
		}
		else
		{
			return( _Edit_Shape_Del() );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Shapes::_Edit_Point_Del(void)
{
	if( m_Edit_pShape && m_Edit_iPart >= 0 && m_Edit_iPoint >= 0 )
	{
		if( m_Edit_pShape->Get_Point_Count(m_Edit_iPart) > 1 )
		{
			m_Edit_pShape->Del_Point(m_Edit_iPoint, m_Edit_iPart);

			if( m_Edit_iPoint >= m_Edit_pShape->Get_Point_Count(m_Edit_iPart) )
			{
				m_Edit_iPoint	= m_Edit_pShape->Get_Point_Count(m_Edit_iPart) - 1;
			}

			if( m_Edit_pShape->Get_Point_Count(m_Edit_iPart) <= 1 )
			{
				if( m_pShapes->Get_Type() == SHAPE_TYPE_Line || m_pShapes->Get_Type() == SHAPE_TYPE_Polygon )
				{
					m_Edit_iPoint	= -1;
				}
			}

			Update_Views(true);

			return( true );
		}
		else
		{
			return( _Edit_Part_Del() );
		}
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Shapes::_Edit_Shape_Draw_Point(wxDC &dc, TSG_Point_Int Point, bool bSelected)
{
	_Edit_Shape_Draw_Point(dc, Point.x, Point.y, bSelected);
}

void CWKSP_Shapes::_Edit_Shape_Draw_Point(wxDC &dc, int x, int y, bool bSelected)
{
	dc.SetPen  (wxPen(m_Edit_Color));
	dc.SetBrush(*wxTRANSPARENT_BRUSH);

	dc.DrawCircle(x, y, 2);

	Draw_Edge(dc, EDGE_STYLE_SIMPLE,
		x - EDIT_TICKMARK_SIZE - 1,
		y - EDIT_TICKMARK_SIZE - 1,
		x + EDIT_TICKMARK_SIZE,
		y + EDIT_TICKMARK_SIZE
	);

	if( bSelected )
	{
		dc.SetPen(*wxRED_PEN);

		Draw_Edge(dc, EDGE_STYLE_SIMPLE,
			x - EDIT_TICKMARK_SIZE - 2,
			y - EDIT_TICKMARK_SIZE - 2,
			x + EDIT_TICKMARK_SIZE + 1,
			y + EDIT_TICKMARK_SIZE + 1
		);
	}
}

//---------------------------------------------------------
void CWKSP_Shapes::_Edit_Shape_Draw_Move(wxDC &dc, CSG_Rect rWorld, double ClientToWorld, wxPoint Point)
{
	_Edit_Shape_Draw_Point(dc, Point.x, Point.y, false);
}

//---------------------------------------------------------
void CWKSP_Shapes::_Edit_Shape_Draw(CWKSP_Map_DC &dc_Map)
{
	int		iPart, iPoint;

	if( m_Edit_pShape )
	{
		for(iPart=0; iPart<m_Edit_pShape->Get_Part_Count(); iPart++)
		{
			for(iPoint=0; iPoint<m_Edit_pShape->Get_Point_Count(iPart); iPoint++)
			{
				_Edit_Shape_Draw_Point(dc_Map.dc, dc_Map.World2DC(m_Edit_pShape->Get_Point(iPoint, iPart)), false);
			}
		}

		if( m_Edit_iPart >= 0 && m_Edit_iPoint >= 0 )
		{
			_Edit_Shape_Draw_Point(dc_Map.dc, dc_Map.World2DC(m_Edit_pShape->Get_Point(m_Edit_iPoint, m_Edit_iPart)), true);
		}

		if( m_Parameters("EDIT_SNAP_LIST")->asShapesList()->Get_Count() > 0 )
		{
			iPoint	= m_Parameters("EDIT_SNAP_DIST")->asInt();

			dc_Map.dc.DrawCircle(1 + iPoint, 1 + iPoint, iPoint);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CWKSP_Shapes::_Edit_Snap_Point(CSG_Point &Point, double ClientToWorld)
{
	if( m_Edit_pShape )
	{
		CSG_Parameter_Shapes_List	*pList	= m_Parameters("EDIT_SNAP_LIST")->asShapesList();

		if( pList->Get_Count() > 0 )
		{
			int			i;
			double		snap_Dist, max_Dist;
			CSG_Point	snap_Point;

			max_Dist	= m_Parameters("EDIT_SNAP_DIST")->asDouble() * ClientToWorld;
			snap_Dist	= max_Dist + 1.0;

			for(i=0; i<pList->Get_Count(); i++)
			{
				_Edit_Snap_Point(Point, snap_Point, snap_Dist, pList->asShapes(i), false);
			}

			if( snap_Dist <= max_Dist )
			{
				Point	= snap_Point;
			}
			else if( m_pShapes->Get_Type() == SHAPE_TYPE_Line || m_pShapes->Get_Type() == SHAPE_TYPE_Polygon )
			{
				for(i=0; i<pList->Get_Count(); i++)
				{
					_Edit_Snap_Point(Point, snap_Point, snap_Dist, pList->asShapes(i), true);
				}

				if( snap_Dist <= max_Dist )
				{
					Point	= snap_Point;
				}
			}
		}
	}
}

//---------------------------------------------------------
void CWKSP_Shapes::_Edit_Snap_Point(CSG_Point Point, CSG_Point &snap_Point, double &snap_Dist, CSG_Shapes *pShapes, bool bLine)
{
	CSG_Shape	*pSelected	= pShapes->Get_Selection();

	if( pShapes->Select(CSG_Rect(Point.Get_X() - snap_Dist, Point.Get_Y() - snap_Dist, Point.Get_X() + snap_Dist, Point.Get_Y() + snap_Dist)) )
	{
		for(int i=0; i<pShapes->Get_Selection_Count(); i++)
		{
			if( pShapes != m_pShapes || pSelected != pShapes->Get_Selection(i) )
			{
				if( bLine )
				{
					_Edit_Snap_Point_ToLine(Point, snap_Point, snap_Dist, pShapes->Get_Selection(i));
				}
				else
				{
					_Edit_Snap_Point       (Point, snap_Point, snap_Dist, pShapes->Get_Selection(i));
				}
			}
		}
	}

	pShapes->Select(pSelected);
}

//---------------------------------------------------------
void CWKSP_Shapes::_Edit_Snap_Point(CSG_Point pos_Point, CSG_Point &snap_Point, double &snap_Dist, CSG_Shape *pShape)
{
	int			iPart, iPoint;
	double		d, dx, dy;
	TSG_Point	Point;

	for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
	{
		for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
		{
			Point	= pShape->Get_Point(iPoint, iPart);
			dx		= pos_Point.Get_X() - Point.x;
			dy		= pos_Point.Get_Y() - Point.y;
			d		= sqrt(dx*dx + dy*dy);

			if( d < snap_Dist )
			{
				snap_Dist	= d;
				snap_Point	= Point;
			}
		}
	}
}

//---------------------------------------------------------
void CWKSP_Shapes::_Edit_Snap_Point_ToLine(CSG_Point pos_Point, CSG_Point &snap_Point, double &snap_Dist, CSG_Shape *pShape)
{
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CWKSP_Shapes::_Edit_Shape_HitTest(CSG_Point pos_Point, double max_Dist, int &pos_iPart, int &pos_iPoint)
{
	int			Result, iPart, iPoint;
	double		d, dx, dy;
	TSG_Point	Point;

	Result		= 0;

	pos_iPoint	= -1;
	pos_iPart	= -1;

	if( m_Edit_pShape )
	{
		for(iPart=0; iPart<m_Edit_pShape->Get_Part_Count(); iPart++)
		{
			for(iPoint=0; iPoint<m_Edit_pShape->Get_Point_Count(iPart); iPoint++)
			{
				Point	= m_Edit_pShape->Get_Point(iPoint, iPart);
				dx		= pos_Point.Get_X() - Point.x;
				dy		= pos_Point.Get_Y() - Point.y;
				d		= sqrt(dx*dx + dy*dy);

				if( 0.0 > max_Dist || d < max_Dist )
				{
					Result		= 1;
					max_Dist	= d;
					pos_iPoint	= iPoint;
					pos_iPart	= iPart;
				}
			}
		}
	}

	return( Result );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

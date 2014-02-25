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
// 51 Franklin Street, 5th Floor, Boston, MA 02110-1301, //
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
#include <wx/dcclient.h>

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
wxMenu * CWKSP_Shapes::Edit_Get_Menu(void)
{
	wxMenu	*pMenu	= new wxMenu;

	CMD_Menu_Add_Item(pMenu, true , ID_CMD_SHAPES_EDIT_SHAPE);

	if( !is_Editing() )
	{
		pMenu->AppendSeparator();
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_SHAPES_EDIT_ADD_SHAPE);
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_SHAPES_EDIT_DEL_SHAPE);
		pMenu->AppendSeparator();
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_SHAPES_EDIT_SEL_INVERT);
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_SHAPES_EDIT_SEL_CLEAR);

		if( Get_Shapes()->Get_Selection_Count() > 1 && Get_Shapes()->Get_Type() != SHAPE_TYPE_Point )
		{
			pMenu->AppendSeparator();
			CMD_Menu_Add_Item(pMenu, false, ID_CMD_SHAPES_EDIT_MERGE);
		}

		if( Get_Shapes()->Get_Selection_Count() == 1 && Get_Shapes()->Get_Type() == SHAPE_TYPE_Polygon )
		{
			pMenu->AppendSeparator();
			CMD_Menu_Add_Item(pMenu, false, ID_CMD_SHAPES_EDIT_SPLIT);
		}
	}
	else if( Get_Shapes()->Get_Type() != SHAPE_TYPE_Point )
	{
		pMenu->AppendSeparator();
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_SHAPES_EDIT_ADD_PART);
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_SHAPES_EDIT_DEL_PART);
		CMD_Menu_Add_Item(pMenu, false, ID_CMD_SHAPES_EDIT_DEL_POINT);

		if( Get_Shapes()->Get_Type() == SHAPE_TYPE_Polygon )
		{
			pMenu->AppendSeparator();
			CMD_Menu_Add_Item(pMenu, false, ID_CMD_SHAPES_EDIT_SPLIT);
			CMD_Menu_Add_Item(pMenu,  true, ID_CMD_SHAPES_EDIT_MOVE);
		}
		else if( Get_Shapes()->Get_Type() != SHAPE_TYPE_Point )
		{
			pMenu->AppendSeparator();
			CMD_Menu_Add_Item(pMenu,  true, ID_CMD_SHAPES_EDIT_MOVE);
		}
	}

	return( pMenu );
}

//---------------------------------------------------------
TSG_Rect CWKSP_Shapes::Edit_Get_Extent(void)
{
	if( m_Edit_pShape )
	{
		return( m_Edit_pShape->Get_Extent() );
	}

	if( Get_Shapes()->Get_Selection_Count() > 0 )
	{
		return( Get_Shapes()->Get_Selection_Extent().m_rect );
	}

	return( Get_Shapes()->Get_Extent() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Shapes::Edit_On_Key_Down(int KeyCode)
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
	case WXK_SPACE:
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
bool CWKSP_Shapes::Edit_On_Mouse_Down(CSG_Point Point, double ClientToWorld, int Key)
{
	CWKSP_Layer::Edit_On_Mouse_Down(Point, ClientToWorld, Key);

	//-----------------------------------------------------
	switch( m_Edit_Mode )
	{
	case EDIT_SHAPE_MODE_Split:
		if( m_Edit_Shapes.Get_Count() > 1 )
		{
			m_Edit_Shapes.Get_Shape(1)->Add_Point(Point);

			Update_Views(false);

			return( true );
		}
		break;

	case EDIT_SHAPE_MODE_Move:
		if( m_Edit_Shapes.Get_Count() > 1 && m_Edit_Shapes.Get_Shape(1)->Get_Point_Count() == 0 )
		{
			m_Edit_Shapes.Get_Shape(1)->Add_Point(Point);

			return( true );
		}
		break;

	case EDIT_SHAPE_MODE_Normal: default:
		if( m_Edit_pShape )
		{
			if( m_Edit_iPart >= 0 && m_Edit_iPoint < 0 )
			{
				// NOP
			}
			else
			{
				int		iPart, iPoint;

				switch( Edit_Shape_HitTest(Point, EDIT_TICKMARK_SIZE * ClientToWorld, iPart, iPoint) )
				{
				//-----------------------------------------
				case 0:	default:
				case 1:
					if( m_Edit_iPart != iPart || m_Edit_iPoint != iPoint )
					{
						m_Edit_iPart	= iPart;
						m_Edit_iPoint	= iPoint;

						Update_Views(false);
					}

					return( true );

				//-----------------------------------------
				case 2:
					m_Edit_pShape->Ins_Point(Point, iPoint, iPart);

					m_Edit_iPart	= iPart;
					m_Edit_iPoint	= iPoint;

					Update_Views(false);

					return( true );
				}
			}
			break;
		}
	}

	//-----------------------------------------------------
	return( false );
}

//---------------------------------------------------------
bool CWKSP_Shapes::Edit_On_Mouse_Up(CSG_Point Point, double ClientToWorld, int Key)
{
	//-----------------------------------------------------
	if( Key & MODULE_INTERACTIVE_KEY_RIGHT )
	{
		switch( m_Edit_Mode )
		{
		case EDIT_SHAPE_MODE_Split:
			return( _Edit_Split() );

		case EDIT_SHAPE_MODE_Normal:
			if( m_Edit_pShape && m_Edit_iPart >= 0 && m_Edit_iPoint < 0 )
			{
				m_Edit_iPart	= -1;

				Update_Views(false);

				return( true );
			}
			break;
		}
	}

	//-----------------------------------------------------
	else if( m_Edit_pShape )
	{
		if( m_Edit_Mode == EDIT_SHAPE_MODE_Move )
		{
			if( m_Edit_Shapes.Get_Count() > 1 && m_Edit_Shapes.Get_Shape(1)->Get_Point_Count() > 0 )
			{
				m_Edit_Shapes.Get_Shape(1)->Add_Point(Point);

				return( _Edit_Move(false) );
			}
		}
		else if( m_Edit_iPart >= 0 )
		{
			if( m_Edit_iPoint >= 0 )
			{
				if( Point != m_Edit_Mouse_Down )
				{
					_Edit_Snap_Point(Point, ClientToWorld);

					m_Edit_pShape->Set_Point(Point, m_Edit_iPoint, m_Edit_iPart);

					Update_Views(false);

					return( true );
				}
			}
			else
			{
				_Edit_Snap_Point(Point, ClientToWorld);

				m_Edit_pShape->Add_Point(Point, m_Edit_iPart);

				Update_Views(false);

				return( true );
			}
		}
	}

	//-----------------------------------------------------
	else if( m_Edit_Mode == EDIT_SHAPE_MODE_Normal )
	{
		g_pACTIVE->Get_Attributes()->Save_Changes(true);

		CSG_Rect	rWorld(m_Edit_Mouse_Down, Point);

		if( rWorld.Get_XRange() == 0.0 && rWorld.Get_YRange() == 0.0 )
		{
			rWorld.Inflate(2.0 * ClientToWorld, false);
		}

		int		Count	= Get_Shapes()->Get_Selection_Count();

		Get_Shapes()->Select(rWorld, (Key & MODULE_INTERACTIVE_KEY_CTRL) != 0);

		Edit_Set_Index((Key & MODULE_INTERACTIVE_KEY_CTRL) == 0 ? 0 :
			Count < Get_Shapes()->Get_Selection_Count() || m_Edit_Index >= Get_Shapes()->Get_Selection_Count()
			? Get_Shapes()->Get_Selection_Count() - 1 : m_Edit_Index
		);

		if( m_pTable->Get_View() )
		{
			m_pTable->Get_View()->Update_Selection();
		}

		Update_Views(false);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Shapes::Edit_On_Mouse_Move(wxWindow *pMap, CSG_Rect rWorld, wxPoint pt, wxPoint ptLast, int Key)
{
	switch( m_Edit_Mode )
	{
	case EDIT_SHAPE_MODE_Split:
	case EDIT_SHAPE_MODE_Move:
		{
			CSG_Shape	*pShape	= m_Edit_Shapes.Get_Shape(1);

			if( pShape && pShape->Get_Point_Count() > 0 && (pt.x != ptLast.x || pt.y != ptLast.y) )
			{
				wxClientDC	dc(pMap);	dc.SetLogicalFunction(wxINVERT);

				Edit_Shape_Draw_Move(dc, rWorld, ptLast, pShape->Get_Point(0, 0, false));
				Edit_Shape_Draw_Move(dc, rWorld, pt    , pShape->Get_Point(0, 0, false));
			}

			return( true );
		}

	//-----------------------------------------------------
	case EDIT_SHAPE_MODE_Normal: default:
		if( m_Edit_pShape )
		{
			//---------------------------------------------
			if( m_Edit_iPart >= 0 && (m_Edit_iPoint < 0 || Key & MODULE_INTERACTIVE_KEY_LEFT) && (pt.x != ptLast.x || pt.y != ptLast.y) )
			{
				wxClientDC	dc(pMap);	dc.SetLogicalFunction(wxINVERT);

				Edit_Shape_Draw_Move(dc, rWorld, ptLast);
				Edit_Shape_Draw_Move(dc, rWorld, pt);

				return( true );
			}

			//---------------------------------------------
			else
			{
				int			iPart, iPoint;
				double		ClientToWorld	= rWorld.Get_XRange() / (double)pMap->GetClientSize().x;
				CSG_Point	Point(rWorld.Get_XMin() + pt.x * ClientToWorld, rWorld.Get_YMax() - pt.y * ClientToWorld);

				switch( Edit_Shape_HitTest(Point, EDIT_TICKMARK_SIZE * ClientToWorld, iPart, iPoint) )
				{
				default:	pMap->SetCursor(IMG_Get_Cursor(ID_IMG_CRS_SELECT         ));	break;
				case  1:	pMap->SetCursor(IMG_Get_Cursor(ID_IMG_CRS_EDIT_POINT_MOVE));	break;
				case  2:	pMap->SetCursor(IMG_Get_Cursor(ID_IMG_CRS_EDIT_POINT_ADD ));	break;
				}

				return( true );
			}
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
bool CWKSP_Shapes::Edit_Set_Index(int Index)
{
	m_Edit_Attributes.Del_Records();

	if( Index > Get_Shapes()->Get_Selection_Count() )
	{
		Index	= Get_Shapes()->Get_Selection_Count();
	}

	CSG_Table_Record	*pSelection	= Get_Shapes()->Get_Selection(Index);

	if( pSelection )
	{
		m_Edit_Index	= Index;

		for(int i=0; i<Get_Shapes()->Get_Field_Count(); i++)
		{
			CSG_Table_Record	*pRecord	= m_Edit_Attributes.Add_Record();

			pRecord->Set_Value(0, pSelection->Get_Table()->Get_Field_Name(i));
			pRecord->Set_Value(1, pSelection->asString(i));
		}
	}
	else
	{
		m_Edit_Index	= 0;
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Shapes::Edit_Set_Attributes(void)
{
	CSG_Table_Record	*pSelection	= Get_Shapes()->Get_Selection(m_Edit_Index);

	if( pSelection )
	{
		for(int i=0; i<m_Edit_Attributes.Get_Record_Count(); i++)
		{
			pSelection->Set_Value(i, m_Edit_Attributes.Get_Record(i)->asString(1));
		}

		Update_Views(false);

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
bool CWKSP_Shapes::_Edit_Merge(void)
{
	if( Get_Shapes()->Get_Selection_Count() < 2 || Get_Shapes()->Get_Type() == SHAPE_TYPE_Point )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Shape	*pMerged	= Get_Shapes()->Get_Selection(0);

	for(int i=1; i<Get_Shapes()->Get_Selection_Count(); i++)
	{
		CSG_Shape	*pShape	= Get_Shapes()->Get_Selection(i);

		for(int iPart=0, jPart=pMerged->Get_Part_Count(); iPart<pShape->Get_Part_Count(); iPart++)
		{
			for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
			{
				pMerged->Add_Point(pShape->Get_Point(iPoint, iPart), jPart);
			}
		}
	}

	if( Get_Shapes()->Get_Type() == SHAPE_TYPE_Polygon )
	{
		SG_Polygon_Dissolve(pMerged);
	}

	Get_Shapes()->Select(pMerged, true);
	Get_Shapes()->Del_Selection();
	Get_Shapes()->Select(pMerged, false);

	Update_Views(true);

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Shapes::_Edit_Split(void)
{
	if( Get_Shapes()->Get_Type() == SHAPE_TYPE_Polygon )
	{
		switch( m_Edit_Mode )
		{
		default:
			break;

		//-------------------------------------------------
		case EDIT_SHAPE_MODE_Normal:
			m_Edit_Mode	= EDIT_SHAPE_MODE_Split;

			if( m_Edit_Shapes.Get_Count() == 0 )
			{
				m_Edit_Shapes.Add_Shape(Get_Shapes()->Get_Selection());
			}

			if( m_Edit_Shapes.Get_Count() > 1 )
			{
				m_Edit_Shapes.Get_Shape(1)->Del_Parts();
			}
			else
			{
				m_Edit_Shapes.Add_Shape();
			}

			return( true );

		//-------------------------------------------------
		case EDIT_SHAPE_MODE_Split:
			m_Edit_Mode	= EDIT_SHAPE_MODE_Normal;

			CSG_Module	*pModule	= SG_Get_Module_Library_Manager().Get_Module(SG_T("shapes_polygons"), 8); // Polygon-Line Intersection

			if(	pModule )
			{
				CSG_Shapes	Line(SHAPE_TYPE_Line), Split(SHAPE_TYPE_Polygon);

				Line.Add_Shape();

				for(int i=0; i<m_Edit_Shapes.Get_Shape(1)->Get_Point_Count(); i++)
				{
					Line.Get_Shape(0)->Add_Point(m_Edit_Shapes.Get_Shape(1)->Get_Point(i));
				}

				m_Edit_Shapes.Del_Shape(1);

				CSG_Parameters	P; P.Assign(pModule->Get_Parameters());

				pModule->Set_Manager(NULL);

				if( pModule->Get_Parameters()->Set_Parameter("POLYGONS" , &m_Edit_Shapes)
				&&  pModule->Get_Parameters()->Set_Parameter("LINES"    , &Line)
				&&  pModule->Get_Parameters()->Set_Parameter("INTERSECT", &Split)
				&&  pModule->Execute() )
				{
					if( m_Edit_pShape )
					{
						m_Edit_pShape->Assign(Split.Get_Shape(0), false);

						for(int iSplit=1; iSplit<Split.Get_Count(); iSplit++)
						{
							CSG_Shape	*pSplit	= Split.Get_Shape(iSplit);

							for(int iPart=0; iPart<pSplit->Get_Part_Count(); iPart++)
							{
								for(int iPoint=0, jPart=m_Edit_pShape->Get_Part_Count(); iPoint<pSplit->Get_Point_Count(iPart); iPoint++)
								{
									m_Edit_pShape->Add_Point(pSplit->Get_Point(iPoint, iPart), jPart);
								}
							}
						}
					}
					else if( Get_Shapes()->Get_Selection_Count() == 1 ) // if( !m_Edit_pShape )
					{
						CSG_Shape	*pSelection	= Get_Shapes()->Get_Selection();
						
						pSelection->Assign(Split.Get_Shape(0), false);

						for(int iSplit=1; iSplit<Split.Get_Count(); iSplit++)
						{
							CSG_Shape	*pSplit	= Get_Shapes()->Add_Shape(Split.Get_Shape(iSplit));

							((CSG_Table_Record *)pSplit)->Assign(pSelection);

							Get_Shapes()->Select(pSplit, true);
						}

						m_Edit_Shapes.Del_Shapes();
					}
				}

				pModule->Get_Parameters()->Assign_Values(&P);
				pModule->Set_Manager(P.Get_Manager());
			}

			Update_Views(false);

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Shapes::_Edit_Move(bool bToggle)
{
	if( m_Edit_pShape )
	{
		if( bToggle )
		{
			switch( m_Edit_Mode )
			{
			default:
				break;

			//---------------------------------------------
			case EDIT_SHAPE_MODE_Normal:
				m_Edit_Mode	= EDIT_SHAPE_MODE_Move;

				if( m_Edit_Shapes.Get_Count() > 1 )
				{
					m_Edit_Shapes.Get_Shape(1)->Del_Parts();
				}
				else
				{
					m_Edit_Shapes.Add_Shape();
				}

				return( true );

			//---------------------------------------------
			case EDIT_SHAPE_MODE_Move:
				m_Edit_Mode	= EDIT_SHAPE_MODE_Normal;

				m_Edit_Shapes.Del_Shape(1);

				return( true );
			}
		}

		//-------------------------------------------------
		else // if( !bToggle )
		{
			if( m_Edit_Shapes.Get_Count() > 1 && m_Edit_Shapes.Get_Shape(1)->Get_Point_Count() > 1 )
			{
				CSG_Point	Move	= CSG_Point(m_Edit_Shapes.Get_Shape(1)->Get_Point(1))
									- CSG_Point(m_Edit_Shapes.Get_Shape(1)->Get_Point(0));

				m_Edit_Shapes.Get_Shape(1)->Del_Parts();

				if( SG_Get_Length(Move.Get_X(), Move.Get_Y()) > 0.0 )
				{
					for(int iPart=0; iPart<m_Edit_pShape->Get_Part_Count(); iPart++)
					{
						for(int iPoint=0; iPoint<m_Edit_pShape->Get_Point_Count(iPart); iPoint++)
						{
							m_Edit_pShape->Set_Point(Move + m_Edit_pShape->Get_Point(iPoint, iPart), iPoint, iPart);
						}
					}

					Update_Views(false);

					return( true );
				}
			}
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
bool CWKSP_Shapes::_Edit_Shape(void)
{
	return( m_Edit_pShape ? _Edit_Shape_Stop() : _Edit_Shape_Start() );
}

//---------------------------------------------------------
bool CWKSP_Shapes::_Edit_Shape_Start(void)
{
	if( m_Edit_pShape == NULL && Get_Shapes()->Get_Selection(m_Edit_Index) != NULL )
	{
		m_Edit_Mode		= EDIT_SHAPE_MODE_Normal;

		m_Edit_pShape	= m_Edit_Shapes.Add_Shape(Get_Shapes()->Get_Selection(m_Edit_Index), SHAPE_COPY_GEOM);

		m_Edit_iPart	= -1;
		m_Edit_iPoint	= -1;

		Update_Views(false);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Shapes::_Edit_Shape_Stop(void)
{
	return( _Edit_Shape_Stop(DLG_Message_Confirm(_TL("Apply changes?"), _TL("Edit Shapes"))) );
}

bool CWKSP_Shapes::_Edit_Shape_Stop(bool bSave)
{
	if( m_Edit_pShape != NULL )
	{
		if( bSave )
		{
			CSG_Shape	*pShape	= Get_Shapes()->Get_Selection(m_Edit_Index);

			if( pShape == NULL )
			{
				Get_Shapes()->Select(pShape = Get_Shapes()->Add_Shape());

				m_Edit_Index	= 0;
			}

			if( pShape != NULL )
			{
				pShape->Assign(m_Edit_pShape, false);
			}
		}

		m_Edit_Shapes.Del_Shapes();
		m_Edit_pShape	= NULL;
		m_Edit_Mode		= EDIT_SHAPE_MODE_Normal;

		Edit_Set_Index(m_Edit_Index);

		Update_Views();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Shapes::_Edit_Shape_Add(void)
{
	if( !m_Edit_pShape )
	{
		if( Get_Shapes()->Get_Selection_Count() > 0 )
		{
			Get_Shapes()->Select();	// deselect
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

		Update_Views(false);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Shapes::_Edit_Shape_Del(void)
{
	if( DLG_Message_Confirm(_TL("Delete selected shape(s)."), _TL("Edit Shapes")) )
	{
		if( Get_Shapes()->Get_Selection_Count() > 0 )
		{
			if( m_Edit_pShape )
			{
				_Edit_Shape_Stop(false);

				Get_Shapes()->Del_Shape(Get_Shapes()->Get_Selection(m_Edit_Index));
			}
			else
			{
				Get_Shapes()->Del_Selection();
			}

			Edit_Set_Index(0);

			Update_Views();

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

			Update_Views(false);

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
				if( Get_Shapes()->Get_Type() == SHAPE_TYPE_Line || Get_Shapes()->Get_Type() == SHAPE_TYPE_Polygon )
				{
					m_Edit_iPoint	= -1;
				}
			}

			Update_Views(false);

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
void CWKSP_Shapes::Edit_Shape_Draw_Move(wxDC &dc, const CSG_Rect &rWorld, const wxPoint &Point)
{
	_Edit_Shape_Draw_Point(dc, Point.x, Point.y, false);
}

//---------------------------------------------------------
void CWKSP_Shapes::Edit_Shape_Draw_Move(wxDC &dc, const CSG_Rect &rWorld, const wxPoint &Point, const TSG_Point &ptWorld)
{
	double	ClientToWorld	= rWorld.Get_XRange() / (double)dc.GetSize().x;

	dc.DrawLine(Point.x, Point.y,
		(int)((ptWorld.x - rWorld.Get_XMin()) / ClientToWorld),
		(int)((rWorld.Get_YMax() - ptWorld.y) / ClientToWorld)
	);
}

//---------------------------------------------------------
void CWKSP_Shapes::Edit_Shape_Draw(CWKSP_Map_DC &dc_Map)
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
			else if( Get_Shapes()->Get_Type() == SHAPE_TYPE_Line || Get_Shapes()->Get_Type() == SHAPE_TYPE_Polygon )
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
	CSG_Shape	*pSelected	= pShapes->Get_Selection(m_Edit_Index);

	if( pShapes->Select(CSG_Rect(Point.Get_X() - snap_Dist, Point.Get_Y() - snap_Dist, Point.Get_X() + snap_Dist, Point.Get_Y() + snap_Dist)) )
	{
		for(int i=0; i<pShapes->Get_Selection_Count(); i++)
		{
			if( pShapes != Get_Shapes() || pSelected != pShapes->Get_Selection(i) )
			{
				if( bLine )
				{
					Edit_Snap_Point_ToLine(Point, snap_Point, snap_Dist, pShapes->Get_Selection(i));
				}
				else
				{
					_Edit_Snap_Point      (Point, snap_Point, snap_Dist, pShapes->Get_Selection(i));
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
void CWKSP_Shapes::Edit_Snap_Point_ToLine(CSG_Point pos_Point, CSG_Point &snap_Point, double &snap_Dist, CSG_Shape *pShape)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CWKSP_Shapes::Edit_Shape_HitTest(CSG_Point pos_Point, double max_Dist, int &pos_iPart, int &pos_iPoint)
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

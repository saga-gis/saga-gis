
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
//                  WKSP_Map_Layer.cpp                   //
//                                                       //
//          Copyright (C) 2005 by Olaf Conrad            //
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
#include <wx/window.h>

#include <saga_api/saga_api.h>

#include "res_commands.h"

#include "wksp_data_manager.h"
#include "wksp_layer.h"
#include "wksp_layer_classify.h"
#include "wksp_map.h"
#include "wksp_map_layer.h"

#include "wksp_grid.h"
#include "wksp_grids.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CWKSP_Map_Layer::CWKSP_Map_Layer(CWKSP_Layer *pLayer)
{
	m_pLayer		= pLayer;

	m_bShow			= true;
	m_bProject		= false;
	m_bProject_Area	= false;
	m_bFitColors	= false;
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
wxString CWKSP_Map_Layer::Get_Name(void)
{
	wxString	Name(m_pLayer->Get_Name());

	return( !m_bShow ? "* " + Name : Name );
}

//---------------------------------------------------------
wxString CWKSP_Map_Layer::Get_Description(void)
{
	return( m_pLayer->Get_Description() );
}

//---------------------------------------------------------
wxMenu * CWKSP_Map_Layer::Get_Menu(void)
{
	wxMenu	*pMenu, *pMenu_Edit;

	pMenu	= new wxMenu(m_pLayer->Get_Name());

	//-----------------------------------------------------
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_WKSP_ITEM_CLOSE);
	CMD_Menu_Add_Item(pMenu,  true, ID_CMD_MAPS_LAYER_SHOW);
	pMenu->AppendSeparator();
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAPS_MOVE_TOP);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAPS_MOVE_UP);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAPS_MOVE_DOWN);
	CMD_Menu_Add_Item(pMenu, false, ID_CMD_MAPS_MOVE_BOTTOM);

	//-----------------------------------------------------
	switch( m_pLayer->Get_Type() )
	{
	default:
		break;

	case WKSP_ITEM_Grid:
	case WKSP_ITEM_Grids:
		pMenu->AppendSeparator();
		CMD_Menu_Add_Item(pMenu, true, ID_CMD_MAPS_GRID_FITCOLORS);
		break;
	}

	//-----------------------------------------------------
	CSG_Projection	prj_Layer, prj_Map;

	if( _Projected_Get_Projections(prj_Layer, prj_Map) )
	{
		CMD_Menu_Add_Item(pMenu, true, ID_CMD_MAPS_PROJECT);

		if( m_bProject && (m_pLayer->Get_Type() == WKSP_ITEM_Grid || m_pLayer->Get_Type() == WKSP_ITEM_Grids) )
		{
			CMD_Menu_Add_Item(pMenu, true, ID_CMD_MAPS_PROJECT_AREA);
		}
	}

	//-----------------------------------------------------
	if( (pMenu_Edit = m_pLayer->Edit_Get_Menu()) != NULL )
	{
		pMenu->AppendSeparator();
		pMenu->Append(ID_CMD_WKSP_FIRST, _TL("Edit"), pMenu_Edit);
	}

	//-----------------------------------------------------
	return( pMenu );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Map_Layer::On_Command(int Cmd_ID)
{
	switch( Cmd_ID )
	{
	default:
		return( CWKSP_Base_Item::On_Command(Cmd_ID) );

	case ID_CMD_SHAPES_EDIT_SHAPE:
	case ID_CMD_SHAPES_EDIT_ADD_SHAPE:
	case ID_CMD_SHAPES_EDIT_DEL_SHAPE:
	case ID_CMD_SHAPES_EDIT_ADD_PART:
	case ID_CMD_SHAPES_EDIT_DEL_PART:
	case ID_CMD_SHAPES_EDIT_DEL_POINT:
	case ID_CMD_SHAPES_EDIT_SEL_CLEAR:
	case ID_CMD_SHAPES_EDIT_SEL_INVERT:
		return( m_pLayer->On_Command(Cmd_ID) );

	case ID_CMD_WKSP_ITEM_RETURN:
	case ID_CMD_MAPS_LAYER_SHOW:
		m_bShow	= !m_bShow;
		((wxTreeCtrl *)Get_Control())->SetItemText(GetId(), Get_Name());
		((CWKSP_Map *)Get_Manager())->View_Refresh(true);
		break;

	case ID_CMD_MAPS_MOVE_TOP:
		if( Get_Manager()->Move_Top(this) )
			((CWKSP_Map *)Get_Manager())->View_Refresh(false);
		break;

	case ID_CMD_MAPS_MOVE_BOTTOM:
		if( Get_Manager()->Move_Bottom(this) )
			((CWKSP_Map *)Get_Manager())->View_Refresh(false);
		break;

	case ID_CMD_MAPS_MOVE_UP:
		if( Get_Manager()->Move_Up(this) )
			((CWKSP_Map *)Get_Manager())->View_Refresh(false);
		break;

	case ID_CMD_MAPS_MOVE_DOWN:
		if( Get_Manager()->Move_Down(this) )
			((CWKSP_Map *)Get_Manager())->View_Refresh(false);
		break;

	case ID_CMD_MAPS_GRID_FITCOLORS:
		if( (m_bFitColors = !m_bFitColors) == true )
		{
			Fit_Colors(((CWKSP_Map *)Get_Manager())->Get_Extent());
		}
		break;

	case ID_CMD_MAPS_PROJECT:
		m_bProject	= !m_bProject; ((CWKSP_Map *)Get_Manager())->View_Refresh(false);
		break;

	case ID_CMD_MAPS_PROJECT_AREA:
		if( m_bProject )
		{
			m_bProject_Area	= !m_bProject_Area; ((CWKSP_Map *)Get_Manager())->View_Refresh(false);
		}
		break;
	}

	return( true );
}

//---------------------------------------------------------
bool CWKSP_Map_Layer::On_Command_UI(wxUpdateUIEvent &event)
{
	switch( event.GetId() )
	{
	default:
		return( CWKSP_Base_Item::On_Command_UI(event) );

	case ID_CMD_SHAPES_EDIT_SHAPE:
	case ID_CMD_SHAPES_EDIT_ADD_SHAPE:
	case ID_CMD_SHAPES_EDIT_DEL_SHAPE:
	case ID_CMD_SHAPES_EDIT_ADD_PART:
	case ID_CMD_SHAPES_EDIT_DEL_PART:
	case ID_CMD_SHAPES_EDIT_DEL_POINT:
	case ID_CMD_SHAPES_EDIT_SEL_CLEAR:
	case ID_CMD_SHAPES_EDIT_SEL_INVERT:
		return( m_pLayer->On_Command_UI(event) );

	case ID_CMD_MAPS_LAYER_SHOW:
		event.Check(m_bShow);
		break;

	case ID_CMD_MAPS_MOVE_TOP:
	case ID_CMD_MAPS_MOVE_UP:
		event.Enable(Get_Index() > 0);
		break;

	case ID_CMD_MAPS_MOVE_DOWN:
	case ID_CMD_MAPS_MOVE_BOTTOM:
		event.Enable(Get_Index() < Get_Manager()->Get_Count() - 1);
		break;

	case ID_CMD_MAPS_GRID_FITCOLORS:
		event.Check(m_bFitColors);
		break;

	case ID_CMD_MAPS_PROJECT:
		event.Check(m_bProject);
		break;

	case ID_CMD_MAPS_PROJECT_AREA:
		event.Check(m_bProject_Area);
		break;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Map_Layer::Load_Settings(CSG_MetaData *pEntry)
{
	if( pEntry )
	{
		m_bShow         = !(*pEntry)("SHOW"        ) || (*pEntry)["SHOW"        ].Get_Content().CmpNoCase("true") == 0;
		m_bProject      =  (*pEntry)("PROJECT"     ) && (*pEntry)["PROJECT"     ].Get_Content().CmpNoCase("true") == 0;
		m_bProject_Area =  (*pEntry)("PROJECT_AREA") && (*pEntry)["PROJECT_AREA"].Get_Content().CmpNoCase("true") == 0;
		m_bFitColors    =  (*pEntry)("FITCOLORS"   ) && (*pEntry)["FITCOLORS"   ].Get_Content().CmpNoCase("true") == 0;

		if( !m_bShow )
		{
			((wxTreeCtrl *)Get_Control())->SetItemText(GetId(), Get_Name());
		}

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CWKSP_Map_Layer::Save_Settings(CSG_MetaData *pEntry)
{
	if( pEntry )
	{
		pEntry->Add_Child("SHOW"        , m_bShow         ? "true" : "false");
		pEntry->Add_Child("PROJECT"     , m_bProject      ? "true" : "false");
		pEntry->Add_Child("PROJECT_AREA", m_bProject_Area ? "true" : "false");
		pEntry->Add_Child("FITCOLORS"   , m_bFitColors    ? "true" : "false");

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Parameters * CWKSP_Map_Layer::Get_Parameters(void)
{
	return( m_pLayer->Get_Parameters() );
}

//---------------------------------------------------------
void CWKSP_Map_Layer::Parameters_Changed(void)
{
	m_pLayer->Parameters_Changed();

	CWKSP_Base_Item::Parameters_Changed();
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Map_Layer::Fit_Colors(const CSG_Rect &rWorld)
{
	if( m_bFitColors )
	{
		CSG_Rect	_rWorld(rWorld);	CSG_Projection	prj_Layer, prj_Map;

		if( m_bProject && _Projected_Get_Projections(prj_Layer, prj_Map) )
		{
			SG_Get_Projected(prj_Map, prj_Layer, _rWorld.m_rect);
		}

		switch( m_pLayer->Get_Type() )
		{
		default:
			return( false );

		case WKSP_ITEM_Grids:
			((CWKSP_Grids *)m_pLayer)->Fit_Colors(_rWorld);
			break;

		case WKSP_ITEM_Grid :
			((CWKSP_Grid  *)m_pLayer)->Fit_Colors(_rWorld);

			if( m_pLayer->Get_Parameter("COLORS_TYPE")->asInt() == CLASSIFY_OVERLAY )
			{
				#define	FIT_OVERLAY_GRID_COLORS(band, extent)	{\
					CWKSP_Grid	*pGrid	= (CWKSP_Grid *)g_pData->Get(m_pLayer->Get_Parameter(band)->asGrid());\
					if( pGrid && m_pLayer->Get_Parameter(band)->is_Enabled() )\
					{	pGrid->Fit_Colors(extent);	}\
				}

				FIT_OVERLAY_GRID_COLORS("OVERLAY_R", rWorld);
				FIT_OVERLAY_GRID_COLORS("OVERLAY_G", rWorld);
				FIT_OVERLAY_GRID_COLORS("OVERLAY_B", rWorld);
			}
			break;
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Map_Layer::is_Projecting(void)
{
	CSG_Projection	prj_Layer, prj_Map;

	return( m_bProject && _Projected_Get_Projections(prj_Layer, prj_Map) );
}

//---------------------------------------------------------
bool CWKSP_Map_Layer::_Projected_Get_Projections(CSG_Projection &prj_Layer, CSG_Projection &prj_Map)
{
	prj_Layer	= m_pLayer    ->Get_Object () ->Get_Projection();
	prj_Map		= ((CWKSP_Map *)Get_Manager())->Get_Projection();

	return( prj_Layer.is_Okay() && prj_Map.is_Okay() && prj_Layer.is_Equal(prj_Map) == false );
}

//---------------------------------------------------------
bool CWKSP_Map_Layer::_Set_Extent_Points(const CSG_Rect &Extent, CSG_Shapes &Points, int Resolution)
{
	double	d	= (Extent.Get_XRange() + Extent.Get_YRange()) / (double)Resolution;

	for(double y=Extent.Get_YMin(); y<Extent.Get_YMax(); y+=d)
	{
		for(double x=Extent.Get_XMin(); x<Extent.Get_XMax(); x+=d)
		{
			Points.Add_Shape()->Add_Point(x, y);
		}

		Points.Add_Shape()->Add_Point(Extent.Get_XMax(), y);
	}

	for(double x=Extent.Get_XMin(); x<Extent.Get_XMax(); x+=d)
	{
		Points.Add_Shape()->Add_Point(x, Extent.Get_YMax());
	}

	Points.Add_Shape()->Add_Point(Extent.Get_XMax(), Extent.Get_YMax());

	return( true );
}

//---------------------------------------------------------
CSG_Rect CWKSP_Map_Layer::Get_Extent(void)
{
	CSG_Projection	prj_Layer, prj_Map;

	if( !m_bProject || !_Projected_Get_Projections(prj_Layer, prj_Map) )
	{
		return( m_pLayer->Get_Extent() );
	}

	//-----------------------------------------------------
	CSG_Rect	rLayer	= m_pLayer->Get_Extent();

	if( rLayer.Get_XRange() == 0. || rLayer.Get_YRange() == 0. )
	{
		if( m_pLayer->Get_Object()->asShapes()
		&&  m_pLayer->Get_Object()->asShapes()->Get_Count() == 1
		&&  m_pLayer->Get_Object()->asShapes()->Get_Type() == SHAPE_TYPE_Point )
		{
			rLayer.Inflate(1., false);
		}

		if( m_pLayer->Get_Object()->asPointCloud()
		&&  m_pLayer->Get_Object()->asPointCloud()->Get_Count() == 1 )
		{
			rLayer.Inflate(1., false);
		}
	}

	//-----------------------------------------------------
	CSG_Shapes	Extent(SHAPE_TYPE_Point);

	Extent.Get_Projection().Create(prj_Layer);

	_Set_Extent_Points(rLayer, Extent);

	SG_Get_Projected(&Extent, NULL, prj_Map);

	return( Extent.Get_Extent() );

	//-----------------------------------------------------
	//CSG_Rect	rLayer(m_pLayer->Get_Extent());

	//SG_Get_Projected(prj_Layer, prj_Map, rLayer.m_rect);

	//return( rLayer );
}

//---------------------------------------------------------
CSG_Rect CWKSP_Map_Layer::_Projected_Get_Layer_Extent(const CSG_Rect &rMap)
{
	CSG_Projection	prj_Layer, prj_Map;

	if( !m_bProject || !_Projected_Get_Projections(prj_Layer, prj_Map) )
	{
		return( m_pLayer->Get_Extent() );
	}

	//-----------------------------------------------------
	CSG_Shapes	Extent(SHAPE_TYPE_Point);

	Extent.Get_Projection().Create(prj_Map);

	_Set_Extent_Points(rMap, Extent);

	SG_Get_Projected(&Extent, NULL, prj_Layer);

	TSG_Rect	rLayer	= Extent.Get_Extent();

	if( isinf(rLayer.xMin) ) rLayer.xMin = m_pLayer->Get_Extent().Get_XMin();
	if( isinf(rLayer.xMax) ) rLayer.xMax = m_pLayer->Get_Extent().Get_XMax();
	if( isinf(rLayer.yMin) ) rLayer.yMin = m_pLayer->Get_Extent().Get_YMin();
	if( isinf(rLayer.yMax) ) rLayer.yMax = m_pLayer->Get_Extent().Get_YMax();

	return( rLayer );
}

//---------------------------------------------------------
bool CWKSP_Map_Layer::_Projected_Shapes_Clipped(const CSG_Rect &rMap, CSG_Shapes *pShapes, CSG_Shapes &Clipped)
{
	CSG_Projection	prj_Layer, prj_Map;

	if( !m_bProject || !_Projected_Get_Projections(prj_Layer, prj_Map) )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Shapes	Shapes, Extent(SHAPE_TYPE_Point);	Extent.Get_Projection().Create(prj_Map);

	_Set_Extent_Points(rMap, Extent);

	if( !SG_Get_Projected(&Extent, NULL, prj_Layer) || Extent.Get_Extent().Get_Area() == 0 )
	{
		return( false );
	}

	if( std::isfinite(Extent.Get_Extent().Get_XMin()) && std::isfinite(Extent.Get_Extent().Get_XMax())
	&&  std::isfinite(Extent.Get_Extent().Get_YMin()) && std::isfinite(Extent.Get_Extent().Get_YMax()) )
	{
		CSG_Shapes	Clip(SHAPE_TYPE_Polygon);	CSG_Shape	*pClip	= Clip.Add_Shape();

		for(int i=0; i<Extent.Get_Count(); i++)
		{
			TSG_Point	p	= Extent.Get_Shape(i)->Get_Point(0);

			if( isinf(p.x) ) p.x = std::signbit(p.x) ? Extent.Get_Extent().Get_XMin() : Extent.Get_Extent().Get_XMax();
			if( isinf(p.y) ) p.y = std::signbit(p.y) ? Extent.Get_Extent().Get_YMin() : Extent.Get_Extent().Get_YMax();

			if( std::isfinite(p.x) && std::isfinite(p.y) )
			{
				pClip->Add_Point(p);
			}
		}

		//-------------------------------------------------
		if( ((CSG_Shape_Polygon *)pClip)->Get_Area() > 0.0 )
		{
			Shapes.Create(pShapes->Get_Type(), SG_T(""), pShapes);	Shapes.Get_Projection().Create(prj_Layer);

			for(int i=0; i<pShapes->Get_Count(); i++)
			{
				if( pClip->Intersects(pShapes->Get_Shape(i)) )
				{
					Shapes.Add_Shape(pShapes->Get_Shape(i));
				}
			}

			pShapes	= &Shapes;	//	SG_UI_DataObject_Add(SG_Create_Shapes(Clip), 0);
		}
	}

	//-----------------------------------------------------
	SG_Get_Projected(pShapes, pShapes == &Shapes ? NULL : &Shapes, prj_Map);

	//-----------------------------------------------------
	CSG_Shapes	Clip(SHAPE_TYPE_Polygon);	Clip.Get_Projection().Create(prj_Map);
	CSG_Shape	*pClip	= Clip.Add_Shape();
	pClip->Add_Point(rMap.Get_XMin(), rMap.Get_YMin());
	pClip->Add_Point(rMap.Get_XMin(), rMap.Get_YMax());
	pClip->Add_Point(rMap.Get_XMax(), rMap.Get_YMax());
	pClip->Add_Point(rMap.Get_XMax(), rMap.Get_YMin());

	SG_UI_ProgressAndMsg_Lock(true);

	CSG_Tool	*pTool	= SG_Get_Tool_Library_Manager().Create_Tool("shapes_polygons", 11);	// Polygon Clipping

	bool	bResult	= pTool && pTool->Set_Manager(NULL)
	&&  pTool->Set_Parameter("CLIP"    , &Clip)
	&&  pTool->Set_Parameter("S_INPUT" , &Shapes)
	&&  pTool->Set_Parameter("S_OUTPUT", &Clipped)
	&&  pTool->Set_Parameter("DISSOLVE", false)
	&&  pTool->Set_Parameter("MULTIPLE", false)
	&&  pTool->Execute();

	SG_Get_Tool_Library_Manager().Delete_Tool(pTool);

	SG_UI_ProgressAndMsg_Lock(false);

	return( bResult );
}

//---------------------------------------------------------
bool CWKSP_Map_Layer::_Projected_Shapes_Draw(CWKSP_Map_DC &dc_Map, int Flags, CSG_Shapes *pShapes, bool bSelection)
{
	CSG_Shapes	Shapes;

	if( pShapes->Get_Type() != SHAPE_TYPE_Polygon )
	{
		SG_Get_Projected(pShapes, &Shapes, ((CWKSP_Map *)Get_Manager())->Get_Projection());
	}
	else
	{
		CSG_Rect	rMap(dc_Map.m_rWorld);	rMap.Inflate(dc_Map.m_DC2World, false);	// inflate by one pixel

		_Projected_Shapes_Clipped(rMap, pShapes, Shapes);
	}

	if( bSelection )
	{
		Shapes.Inv_Selection();
	}

	return( m_pLayer->Draw(dc_Map, Flags, &Shapes) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CWKSP_Map_Layer::Draw(CWKSP_Map_DC &dc_Map, int Flags)
{
	CSG_Projection	prj_Layer, prj_Map;

	if( !m_bProject || !_Projected_Get_Projections(prj_Layer, prj_Map) )
	{
		if( m_pLayer->do_Show(dc_Map.m_rWorld) )
		{
			m_pLayer->Draw(dc_Map, Flags);
		}

		return( true );
	}

	//-----------------------------------------------------
	CSG_Rect	rLayer(_Projected_Get_Layer_Extent(dc_Map.m_rWorld));

	if( !m_pLayer->do_Show(rLayer, false) )
	{
		return( true );
	}

	switch( m_pLayer->Get_Object()->Get_ObjectType() )
	{
	//-----------------------------------------------------
	case SG_DATAOBJECT_TYPE_Shapes: {
		CSG_Shapes	*pShapes	= m_pLayer->Get_Object()->asShapes();

		_Projected_Shapes_Draw(dc_Map, Flags, pShapes);

		if( pShapes->Get_Selection_Count() > 0 )
		{
			CSG_Shapes	Selection(pShapes->Get_Type(), pShapes->Get_Name(), pShapes);

			for(int i=0; i<pShapes->Get_Selection_Count(); i++)
			{
				Selection.Add_Shape(pShapes->Get_Selection(i));
			}

			_Projected_Shapes_Draw(dc_Map, Flags, &Selection, true);
		}

	}	break;

	//-----------------------------------------------------
	case SG_DATAOBJECT_TYPE_PointCloud: {
		CSG_PointCloud	Points, *pPoints	= m_pLayer->Get_Object()->asPointCloud();

		Points.Create(pPoints);
		Points.Get_Projection().Create(pPoints->Get_Projection());

		for(int i=0; i<pPoints->Get_Count(); i++)
		{
			if( rLayer.Contains(pPoints->Get_X(i), pPoints->Get_Y(i)) )
			{
				Points.Add_Shape(pPoints->Get_Record(i));
			}
		}

		SG_Get_Projected(&Points, NULL, prj_Map);

		m_pLayer->Draw(dc_Map, Flags, &Points);

	}	break;

	//-----------------------------------------------------
	case SG_DATAOBJECT_TYPE_Grid: {
		CSG_Grid	Grid, *pGrid = m_pLayer->Get_Object()->asGrid();

		if( !pGrid->Get_Extent().Intersects(rLayer) )	// == INTERSECTION_None
		{
			return( true );
		}

		CSG_Grid_System	System(dc_Map.m_DC2World, dc_Map.m_rWorld.Get_XMin(), dc_Map.m_rWorld.Get_YMin(), dc_Map.dc.GetSize().GetWidth(), dc_Map.dc.GetSize().GetHeight());

		if( !System.is_Valid() || !Grid.Create(System, pGrid->Get_Type()) )
		{
			return( false );
		}

		//-------------------------------------------------
		SG_UI_ProgressAndMsg_Lock(true);

		CSG_Tool	*pTool	= SG_Get_Tool_Library_Manager().Create_Tool("pj_proj4", 4);	// Coordinate Transformation (Grid)

		if( pTool && pTool->Set_Manager(NULL)
			&&  pTool->Set_Parameter("CRS_PROJ4"        , prj_Map.Get_Proj4())
			&&  pTool->Set_Parameter("RESAMPLING"       , m_pLayer->Get_Parameter("DISPLAY_RESAMPLING")->asInt() ? 3 : 0)
			&&  pTool->Set_Parameter("KEEP_TYPE"        , true)
			&&  pTool->Set_Parameter("TARGET_AREA"      , m_bProject_Area)
			&&  pTool->Set_Parameter("TARGET_DEFINITION", 1)
			&&  pTool->Set_Parameter("SOURCE"           , pGrid)
			&&  pTool->Set_Parameter("GRID"             , &Grid)
			&&  pTool->Execute() )
		{
			m_pLayer->Draw(dc_Map, Flags, &Grid);
		}

		SG_Get_Tool_Library_Manager().Delete_Tool(pTool);

		SG_UI_ProgressAndMsg_Lock(false);

	}	break;

	//-----------------------------------------------------
	case SG_DATAOBJECT_TYPE_Grids: {
		CSG_Grids	*pGrids = m_pLayer->Get_Object()->asGrids();

		if( !pGrids->Get_Extent().Intersects(rLayer) )	// == INTERSECTION_None
		{
			return( true );
		}

		CSG_Grid_System	System(dc_Map.m_DC2World, dc_Map.m_rWorld.Get_XMin(), dc_Map.m_rWorld.Get_YMin(), dc_Map.dc.GetSize().GetWidth(), dc_Map.dc.GetSize().GetHeight());

		if( !System.is_Valid() )
		{
			return( false );
		}

		//-------------------------------------------------
		SG_UI_ProgressAndMsg_Lock(true);

		CSG_Tool	*pTool	= SG_Get_Tool_Library_Manager().Create_Tool("pj_proj4", 3);	// Coordinate Transformation (Grid List)

		if( pTool && pTool->Set_Manager(NULL)
			&&  pTool->Set_Parameter("CRS_PROJ4"        , prj_Map.Get_Proj4())
			&&  pTool->Set_Parameter("RESAMPLING"       , m_pLayer->Get_Parameter("DISPLAY_RESAMPLING")->asInt() ? 3 : 0)
			&&  pTool->Set_Parameter("KEEP_TYPE"        , true)
			&&  pTool->Set_Parameter("TARGET_AREA"      , true)
			&&  pTool->Set_Parameter("TARGET_DEFINITION", 0)
			&&  pTool->Set_Parameter("TARGET_USER_SIZE" , System.Get_Cellsize())
			&&  pTool->Set_Parameter("TARGET_USER_XMIN" , System.Get_XMin())
			&&  pTool->Set_Parameter("TARGET_USER_YMIN" , System.Get_YMin())
			&&  pTool->Set_Parameter("TARGET_USER_XMAX" , System.Get_XMax())
			&&  pTool->Set_Parameter("TARGET_USER_YMAX" , System.Get_YMax())
			&&  pTool->Get_Parameter("SOURCE")->asList()->Add_Item(pGrids)
			&&  pTool->Execute() && (pGrids = (CSG_Grids *)pTool->Get_Parameter("GRIDS")->asList()->Get_Item(0)) != NULL )
		{
			m_pLayer->Draw(dc_Map, Flags, pGrids);

			delete(pGrids);
		}

		SG_Get_Tool_Library_Manager().Delete_Tool(pTool);

		SG_UI_ProgressAndMsg_Lock(false);

	}	break;

	//-----------------------------------------------------
	default:
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

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
//                   pointcloud_tools                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                       pc_cut.cpp                      //
//                                                       //
//                Copyright (C) 2009-10 by               //
//                      Olaf Conrad                      //
//                    Volker Wichmann                    //
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
//                University of Hamburg                  //
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
#include "pc_cut.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPC_Cut::CPC_Cut(void)
{
	CSG_Parameters	*pParameters;

	//-----------------------------------------------------
	Set_Name		(_TL("Point Cloud Cutter"));

	Set_Author		(SG_T("O. Conrad, V. Wichmann (c) 2009-10"));

	Set_Description	(_TW(
		"This modules allows one to extract subsets from a Point Cloud. The area-of-interest "
		"is defined either by bounding box coordinates, the extent of a grid system or "
		"a shapes layer, or by polygons of a shapes layer. Note that the latter "
		"does not support the inverse selection.\n\n"
	));


	//-----------------------------------------------------
	Parameters.Add_PointCloud(
		NULL	, "POINTS"		, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_PointCloud(
		NULL	, "CUT"			, _TL("Cut"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL	, "AREA"		, _TL("Choose Cut from ..."),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("User Defined Extent"),
			_TL("Grid System Extent"),
			_TL("Shapes Extent"),
			_TL("Polygons")
		), 0
	);

	Parameters.Add_Value(
		NULL	, "INVERSE"		, _TL("Inverse"),
		_TL("Invert selection."),
		PARAMETER_TYPE_Bool, false
	);


	//-----------------------------------------------------
	// User Defined Extent

	pParameters	= Add_Parameters	(	   "USER"		, _TL("User Defined Extent"), _TL(""));
	pParameters	->Add_Value			(NULL, "XMIN"		, _TL("Left")				, _TL(""), PARAMETER_TYPE_Double);
	pParameters	->Add_Value			(NULL, "XMAX"		, _TL("Right")				, _TL(""), PARAMETER_TYPE_Double);
	pParameters	->Add_Value			(NULL, "YMIN"		, _TL("Bottom")				, _TL(""), PARAMETER_TYPE_Double);
	pParameters	->Add_Value			(NULL, "YMAX"		, _TL("Top")				, _TL(""), PARAMETER_TYPE_Double);

	//-----------------------------------------------------
	// Grid System Extent

	pParameters	= Add_Parameters	(	   "GRID"		, _TL("Grid System Extent")	, _TL(""));
	pParameters	->Add_Grid_System	(NULL, "GRID"		, _TL("Grid System")		, _TL(""));

	//-----------------------------------------------------
	// Shapes Extent

	pParameters	= Add_Parameters	(	   "EXTENT"		, _TL("Shapes Extent")		, _TL(""));
	pParameters	->Add_Shapes		(NULL, "EXTENT"		, _TL("Shapes Extent")		, _TL(""), PARAMETER_INPUT_OPTIONAL);

	//-----------------------------------------------------
	// Polygons

	pParameters	= Add_Parameters	(	   "POLYGONS"	, _TL("Polygons")			, _TL(""));
	pParameters	->Add_Shapes		(NULL, "POLYGONS"	, _TL("Polygons")			, _TL(""), PARAMETER_INPUT_OPTIONAL, SHAPE_TYPE_Polygon);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPC_Cut::On_Execute(void)
{
	CSG_PointCloud	*pPoints	= Parameters("POINTS")	->asPointCloud();
	CSG_PointCloud	*pCut		= Parameters("CUT")		->asPointCloud();

	switch( Parameters("AREA")->asInt() )
	{
	//-----------------------------------------------------
	case 0:	// User Defined Extent
		if( Dlg_Parameters("USER") )
		{
			CSG_Rect	r(
				Get_Parameters("USER")->Get_Parameter("XMIN")->asDouble(),
				Get_Parameters("USER")->Get_Parameter("YMIN")->asDouble(),
				Get_Parameters("USER")->Get_Parameter("XMAX")->asDouble(),
				Get_Parameters("USER")->Get_Parameter("YMAX")->asDouble()
			);

			return( Get_Cut(pPoints, pCut, r, Parameters("INVERSE")->asBool()) );
		}
		break;

	//-----------------------------------------------------
	case 1:	// Grid System Extent
		if( Dlg_Parameters("GRID") )
		{
			return( Get_Cut(pPoints, pCut, Get_Parameters("GRID")->Get_Parameter("GRID")->asGrid_System()->Get_Extent(), Parameters("INVERSE")->asBool()) );
		}
		break;

	//-----------------------------------------------------
	case 2:	// Shapes Extent
		if( Dlg_Parameters("EXTENT") )
		{
			if( Get_Parameters("EXTENT")->Get_Parameter("EXTENT")->asShapes() == NULL )
			{
				SG_UI_Msg_Add_Error(_TL("Please provide a shapefile with the 'Shapes Extent' parameter!"));

				return( false );
			}

			return( Get_Cut(pPoints, pCut, Get_Parameters("EXTENT")->Get_Parameter("EXTENT")->asShapes()->Get_Extent(), Parameters("INVERSE")->asBool()) );
		}
		break;

	//-----------------------------------------------------
	case 3:	// Polygons
		if( Dlg_Parameters("POLYGONS") )
		{
			if( Get_Parameters("POLYGONS")->Get_Parameter("POLYGONS")->asShapes() == NULL )
			{
				SG_UI_Msg_Add_Error(_TL("Please provide a shapefile with the 'Polygons' parameter!"));

				return( false );
			}

			if( Parameters("INVERSE")->asBool() && Get_Parameters("POLYGONS")->Get_Parameter("POLYGONS")->asShapes()->Get_Count() > 1 )
			{
				SG_UI_Msg_Add_Error(_TL("The inverse selection is not implemented for input shapefiles with more than one polygon!"));

				return (false);
			}

			return( Get_Cut(pPoints, pCut, Get_Parameters("POLYGONS")->Get_Parameter("POLYGONS")->asShapes(), Parameters("INVERSE")->asBool()) );
		}
		break;
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPC_Cut::Get_Cut(CSG_PointCloud *pPoints, CSG_PointCloud *pCut, const CSG_Rect &Extent, bool bInverse)
{
	if( pPoints && pPoints->is_Valid() && pCut )
	{
		pCut->Create(pPoints);
		pCut->Set_Name(CSG_String::Format(SG_T("%s [%s]"), pPoints->Get_Name(), _TL("Cut")));

		if( Extent.Intersects(pPoints->Get_Extent()) )
		{
			for(int i=0; i<pPoints->Get_Point_Count() && SG_UI_Process_Set_Progress(i, pPoints->Get_Point_Count()); i++)
			{
				pPoints->Set_Cursor(i);

				if( (Extent.Contains(pPoints->Get_X(), pPoints->Get_Y()) && !bInverse) || (!Extent.Contains(pPoints->Get_X(), pPoints->Get_Y()) && bInverse) )
				{
					pCut->Add_Point(pPoints->Get_X(), pPoints->Get_Y(), pPoints->Get_Z());

					for(int j=0; j<pPoints->Get_Field_Count() - 3; j++)
					{
						pCut->Set_Attribute(j, pPoints->Get_Attribute(j));
					}
				}
			}
		}

		return( pCut->Get_Count() > 0 );
	}

	return( false );
}

//---------------------------------------------------------
bool CPC_Cut::Get_Cut(CSG_PointCloud *pPoints, CSG_PointCloud *pCut, CSG_Shapes *pPolygons, bool bInverse)
{
	if( pPoints && pPoints->is_Valid() && pCut )
	{
		pCut->Create(pPoints);
		pCut->Set_Name(CSG_String::Format(SG_T("%s [%s]"), pPoints->Get_Name(), pPolygons->Get_Name()));

		if( pPolygons && pPolygons->Get_Type() == SHAPE_TYPE_Polygon && pPolygons->Get_Extent().Intersects(pPoints->Get_Extent()) )
		{
			for(int i=0; i<pPoints->Get_Point_Count() && SG_UI_Process_Set_Progress(i, pPoints->Get_Point_Count()); i++)
			{
				pPoints->Set_Cursor(i);

				if( (Contains(pPolygons, pPoints->Get_X(), pPoints->Get_Y()) && !bInverse) || (!Contains(pPolygons, pPoints->Get_X(), pPoints->Get_Y()) && bInverse) )
				{
					pCut->Add_Point(pPoints->Get_X(), pPoints->Get_Y(), pPoints->Get_Z());

					for(int j=0; j<pPoints->Get_Field_Count() - 3; j++)
					{
						pCut->Set_Attribute(j, pPoints->Get_Attribute(j));
					}
				}
			}
		}

		return( pCut->Get_Count() > 0 );
	}

	return( false );
}

//---------------------------------------------------------
bool CPC_Cut::Contains(CSG_Shapes *pPolygons, double x, double y)
{
	if( pPolygons->Get_Extent().Contains(x, y) )
	{
		for(int iPolygon=0; iPolygon<pPolygons->Get_Count(); iPolygon++)
		{
			CSG_Shape_Polygon	*pPolygon	= (CSG_Shape_Polygon *)pPolygons->Get_Shape(iPolygon);

			if( pPolygon->Contains(x, y) )
			{
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
CPC_Cut_Interactive::CPC_Cut_Interactive(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Point Cloud Cutter"));

	Set_Author		(SG_T("O. Conrad, V. Wichmann (c) 2009-10"));

	Set_Description	(_TW(
		"This modules allows one to extract subsets from a Point Cloud. The area-of-interest "
		"is interactively defined either by dragging a box or by digitizing a polygon.\n"
		"Best practice is to display the Point Cloud in a new Map View first and then "
		"execute the module. Use the Action tool to define the AOI.\n\n"
	));


	//-----------------------------------------------------
	Parameters.Add_PointCloud(
		NULL	, "POINTS"		, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_PointCloud(
		NULL	, "CUT"			, _TL("Cut"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL	, "AOI"			, _TL("Define AOI by ..."),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
                _TL("dragging a box"),
                _TL("digitzing a polygon")
            ), 0
	);

	Parameters.Add_Shapes(
		Parameters("AOI"), "AOISHAPE"	, _TL("AOI Shape"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Value(
		NULL	, "INVERSE"		, _TL("Inverse"),
		_TL("Invert selection."),
		PARAMETER_TYPE_Bool, false
	);


	//-----------------------------------------------------
	CSG_Parameters	*pParameters	= Add_Parameters("CUT", _TL("Cut"), _TL(""));

	pParameters->Add_Value(NULL, "XMIN", _TL("Left")	, _TL(""), PARAMETER_TYPE_Double);
	pParameters->Add_Value(NULL, "XMAX", _TL("Right")	, _TL(""), PARAMETER_TYPE_Double);
	pParameters->Add_Value(NULL, "YMIN", _TL("Bottom")	, _TL(""), PARAMETER_TYPE_Double);
	pParameters->Add_Value(NULL, "YMAX", _TL("Top")		, _TL(""), PARAMETER_TYPE_Double);
}

//---------------------------------------------------------
bool CPC_Cut_Interactive::On_Execute(void)
{
	m_pPoints	= Parameters("POINTS")	->asPointCloud();
	m_pCut		= Parameters("CUT")		->asPointCloud();
	m_bAOIBox	= Parameters("AOI")		->asInt() == 0 ? true : false;
	m_pAOI		= Parameters("AOISHAPE")->asShapes();
	m_bInverse	= Parameters("INVERSE")	->asBool();


	if( !m_bAOIBox )
	{
		m_bAdd      = false;
        Set_Drag_Mode(MODULE_INTERACTIVE_DRAG_LINE);

		if( m_pAOI == NULL )
		{
			m_pAOI = SG_Create_Shapes(SHAPE_TYPE_Polygon, CSG_String::Format(SG_T("AOI_%s"), m_pPoints->Get_Name()));
			m_pAOI->Add_Field("ID", SG_DATATYPE_Int);
			Parameters("AOISHAPE")->Set_Value(m_pAOI);
			DataObject_Add(m_pAOI, true);
		}
		else if( m_pAOI->Get_Field_Count() < 1)
			m_pAOI->Add_Field("ID", SG_DATATYPE_Int);

		
		CSG_Parameters	sParms;
		if( DataObject_Get_Parameters(m_pAOI, sParms) && sParms("DISPLAY_BRUSH") && sParms("OUTLINE_COLOR"))
		{
			sParms("OUTLINE_COLOR")	->Set_Value((int)SG_GET_RGB(180, 0, 0));	// outline color
			sParms("DISPLAY_BRUSH")	->Set_Value(1);								// fillstyle transparent
			DataObject_Set_Parameters(m_pAOI, sParms);
			DataObject_Update(m_pAOI, SG_UI_DATAOBJECT_SHOW_LAST_MAP);
		}
	}
	else
		Set_Drag_Mode(MODULE_INTERACTIVE_DRAG_BOX);


	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CPC_Cut_Interactive::On_Execute_Position(CSG_Point ptWorld, TSG_Module_Interactive_Mode Mode)
{
	switch( Mode )
	{
	//-----------------------------------------------------
	case MODULE_INTERACTIVE_LDOWN:

		if( m_bAOIBox )
			m_ptDown	= ptWorld;
		else
		{
			if( !m_bAdd )
			{
				m_bAdd	= true;
				m_pAOI	->Del_Records();
				m_pAOI	->Add_Shape();

				m_pAOI->Get_Shape(0)->Add_Point(ptWorld);	// workaround to have first line immediately displayed,
			}												// i.e. we add the first point clicked two times

			m_pAOI->Get_Shape(0)->Add_Point(ptWorld);
			DataObject_Update(m_pAOI, SG_UI_DATAOBJECT_SHOW_LAST_MAP);
		}

		return( true );

	//-----------------------------------------------------
	case MODULE_INTERACTIVE_LUP:

		if( m_bAOIBox )
		{
			CSG_Rect		r(m_ptDown.Get_X(), m_ptDown.Get_Y(), ptWorld.Get_X(), ptWorld.Get_Y());

			CSG_Parameters	*pParameters	= Get_Parameters("CUT");

			pParameters->Get_Parameter("XMIN")->Set_Value(r.Get_XMin());
			pParameters->Get_Parameter("XMAX")->Set_Value(r.Get_XMax());
			pParameters->Get_Parameter("YMIN")->Set_Value(r.Get_YMin());
			pParameters->Get_Parameter("YMAX")->Set_Value(r.Get_YMax());

			if( Dlg_Parameters("CUT") )
			{
				r.Assign(
					pParameters->Get_Parameter("XMIN")->asDouble(),
					pParameters->Get_Parameter("YMIN")->asDouble(),
					pParameters->Get_Parameter("XMAX")->asDouble(),
					pParameters->Get_Parameter("YMAX")->asDouble()
				);

				if( CPC_Cut::Get_Cut(m_pPoints, m_pCut, r, m_bInverse) )
				{
					DataObject_Update(m_pCut);
				}
			}

			return( true );
		}

		return( false );

	//-----------------------------------------------------
	case MODULE_INTERACTIVE_RDOWN:

		if( !m_bAOIBox )
		{
			m_bAdd    = false;

			if( CPC_Cut::Get_Cut(m_pPoints, m_pCut, m_pAOI, m_bInverse) )
			{
				DataObject_Update(m_pCut);
			}

			return( true );
		}

		return( false );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


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
//                 Copyright (C) 2009 by                 //
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
	//-----------------------------------------------------
	Set_Name		(_TL("Point Cloud Cutter"));

	Set_Author		(SG_T("O.Conrad (c) 2009"));

	Set_Description	(_TW(
		""
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


	//-----------------------------------------------------
	CSG_Parameters	*pParameters;

	pParameters	= Add_Parameters	(	   "USER"		, _TL("User Defined Extent"), _TL(""));
	pParameters->Add_Value			(NULL, "XMIN"		, _TL("Left")				, _TL(""), PARAMETER_TYPE_Double);
	pParameters->Add_Value			(NULL, "XMAX"		, _TL("Right")				, _TL(""), PARAMETER_TYPE_Double);
	pParameters->Add_Value			(NULL, "YMIN"		, _TL("Bottom")				, _TL(""), PARAMETER_TYPE_Double);
	pParameters->Add_Value			(NULL, "YMAX"		, _TL("Top")				, _TL(""), PARAMETER_TYPE_Double);

	pParameters	= Add_Parameters	(	   "GRID"		, _TL("Grid System Extent")	, _TL(""));
	pParameters->Add_Grid_System	(NULL, "GRID"		, _TL("Grid System")		, _TL(""));

	pParameters	= Add_Parameters	(	   "SHAPES"		, _TL("Shapes Extent")		, _TL(""));
	pParameters->Add_Shapes			(NULL, "SHAPES"		, _TL("Left")				, _TL(""), PARAMETER_INPUT);

	pParameters	= Add_Parameters	(	   "POLYGONS"	, _TL("Polygons")			, _TL(""));
	pParameters->Add_Shapes			(NULL, "POLYGONS"	, _TL("Polygons")			, _TL(""), PARAMETER_INPUT, SHAPE_TYPE_Polygon);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPC_Cut::On_Execute(void)
{
	CSG_PointCloud	*pPoints	= Parameters("POINTS")	->asPointCloud();
	CSG_PointCloud	*pCut		= Parameters("CUT")		->asPointCloud();

	//-----------------------------------------------------
	switch( Parameters("AREA")->asInt() )
	{
	case 0:	// User Defined Extent
		if( Dlg_Parameters("USER") )
		{
			CSG_Rect	r(
				Get_Parameters("USER")->Get_Parameter("XMIN")->asDouble(),
				Get_Parameters("USER")->Get_Parameter("YMIN")->asDouble(),
				Get_Parameters("USER")->Get_Parameter("XMAX")->asDouble(),
				Get_Parameters("USER")->Get_Parameter("YMAX")->asDouble()
			);

			return( Get_Cut(pPoints, pCut, r) );
		}
		break;

	case 1:	// Grid System Extent
		if( Dlg_Parameters("GRID") )
		{
			return( Get_Cut(pPoints, pCut, Get_Parameters("GRID")->Get_Parameter("GRID")->asGrid_System()->Get_Extent()) );
		}
		break;

	case 2:	// Shapes Extent
		if( Dlg_Parameters("SHAPES") )
		{
			return( Get_Cut(pPoints, pCut, Get_Parameters("SHAPES")->Get_Parameter("SHAPES")->asShapes()->Get_Extent()) );
		}
		break;

	case 3:	// Polygons
		if( Dlg_Parameters("POLYGONS") )
		{
			return( Get_Cut(pPoints, pCut, Get_Parameters("POLYGONS")->Get_Parameter("POLYGONS")->asShapes()) );
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
bool CPC_Cut::Get_Cut(CSG_PointCloud *pPoints, CSG_PointCloud *pCut, const CSG_Rect &Extent)
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

				if( Extent.Contains(pPoints->Get_X(), pPoints->Get_Y()) )
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
bool CPC_Cut::Get_Cut(CSG_PointCloud *pPoints, CSG_PointCloud *pCut, CSG_Shapes *pPolygons)
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

				if( Contains(pPolygons, pPoints->Get_X(), pPoints->Get_Y()) )
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

			if( pPolygon->is_Containing(x, y) )
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

	Set_Author		(SG_T("O.Conrad (c) 2009"));

	Set_Description	(_TW(
		""
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

		m_ptDown	= ptWorld;

		return( true );

	//-----------------------------------------------------
	case MODULE_INTERACTIVE_LUP:

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

			if( CPC_Cut::Get_Cut(m_pPoints, m_pCut, r) )
			{
				DataObject_Update(m_pCut);
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

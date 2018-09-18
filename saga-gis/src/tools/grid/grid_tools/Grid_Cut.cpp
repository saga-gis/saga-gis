/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      Grid_Tools                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     Grid_Cut.cpp                      //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Grid_Cut.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Grid_System Fit_Extent(const CSG_Grid_System &System, const CSG_Rect &Extent)
{
	CSG_Rect	r(
		System.Fit_xto_Grid_System(Extent.Get_XMin()),
		System.Fit_yto_Grid_System(Extent.Get_YMin()),
		System.Fit_xto_Grid_System(Extent.Get_XMax()),
		System.Fit_yto_Grid_System(Extent.Get_YMax())
	);

	r.Intersect(System.Get_Extent(true));

	return( CSG_Grid_System(System.Get_Cellsize(), r) );
}

//---------------------------------------------------------
void Fit_Extent(CSG_Parameters *pParameters, CSG_Parameter *pParameter, CSG_Grid_System *pSystem)
{
	if( pSystem && pSystem->is_Valid() )
	{
		CSG_Grid_System	System(Fit_Extent(*pSystem, CSG_Rect(
			pParameters->Get_Parameter("XMIN")->asDouble(),
			pParameters->Get_Parameter("YMIN")->asDouble(),
			pParameters->Get_Parameter("XMAX")->asDouble(),
			pParameters->Get_Parameter("YMAX")->asDouble()
		)));

		if( !SG_STR_CMP(pParameter->Get_Identifier(), "NX") )
		{
			System.Assign(System.Get_Cellsize(), System.Get_XMin(), System.Get_YMin(),
				pParameters->Get_Parameter("NX")->asInt(), System.Get_NY()
			);
		}

		if( !SG_STR_CMP(pParameter->Get_Identifier(), "NY") )
		{
			System.Assign(System.Get_Cellsize(), System.Get_XMin(), System.Get_YMin(),
				System.Get_NX(), pParameters->Get_Parameter("NY")->asInt()
			);
		}

		if( System.is_Valid() )
		{
			pParameters->Get_Parameter("XMIN")->Set_Value(System.Get_XMin());
			pParameters->Get_Parameter("XMAX")->Set_Value(System.Get_XMax());
			pParameters->Get_Parameter("YMIN")->Set_Value(System.Get_YMin());
			pParameters->Get_Parameter("YMAX")->Set_Value(System.Get_YMax());
			pParameters->Get_Parameter("NX"  )->Set_Value(System.Get_NX  ());
			pParameters->Get_Parameter("NY"  )->Set_Value(System.Get_NY  ());
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Clip_Interactive::CGrid_Clip_Interactive(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Clip Grids"));

	Set_Author		("O.Conrad (c) 2003");

	Set_Description	(_TW(
		"Clip selected grids to interactively defined extent."
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List("",
		"GRIDS"		, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List("",
		"CLIPPED"	, _TL("Clipped Grids"),
		_TL(""),
		PARAMETER_OUTPUT, false
	);

	Parameters.Add_Bool("",
		"RUN_ONCE"	, _TL("Run Once"),
		_TL(""),
		true
	);

	//-----------------------------------------------------
	CSG_Parameters	*pParameters	= Add_Parameters("EXTENT", _TL("Extent"), _TL(""));

	pParameters->Add_Double("", "XMIN", _TL("Left"   ), _TL(""));
	pParameters->Add_Double("", "XMAX", _TL("Right"  ), _TL(""));
	pParameters->Add_Double("", "YMIN", _TL("Bottom" ), _TL(""));
	pParameters->Add_Double("", "YMAX", _TL("Top"    ), _TL(""));
	pParameters->Add_Int   ("", "NX"  , _TL("Columns"), _TL(""), 1, 1, true);
	pParameters->Add_Int   ("", "NY"  , _TL("Rows"   ), _TL(""), 1, 1, true);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Clip_Interactive::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( is_Executing() && !SG_STR_CMP(pParameters->Get_Identifier(), "EXTENT") )
	{
		Fit_Extent(pParameters, pParameter, Get_System());
	}

	return( CSG_Tool_Grid::On_Parameter_Changed(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Clip_Interactive::On_Execute(void)
{
	m_bDown	= false;

	Parameters("CLIPPED")->asGridList()->Del_Items();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Clip_Interactive::On_Execute_Position(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode)
{
	switch( Mode )
	{
	//-----------------------------------------------------
	case TOOL_INTERACTIVE_LDOWN:
		if( m_bDown == false )
		{
			m_bDown		= true;
			m_ptDown	= ptWorld;
		}

		break;

	//-----------------------------------------------------
	case TOOL_INTERACTIVE_LUP:
		if( m_bDown == true )
		{
			m_bDown		= false;
			ptWorld		= ptWorld;

			CSG_Grid_System	System	= Fit_Extent(*Get_System(), CSG_Rect(
				m_ptDown.Get_X(), m_ptDown.Get_Y(), ptWorld.Get_X(), ptWorld.Get_Y()
			));

			CSG_Parameters	*pParameters	= Get_Parameters("EXTENT");

			pParameters->Set_Callback(false);
			(*pParameters)("XMIN")->Set_Value(System.Get_XMin());
			(*pParameters)("XMAX")->Set_Value(System.Get_XMax());
			(*pParameters)("YMIN")->Set_Value(System.Get_YMin());
			(*pParameters)("YMAX")->Set_Value(System.Get_YMax());
			(*pParameters)("NX"  )->Set_Value(System.Get_NX  ());
			(*pParameters)("NY"  )->Set_Value(System.Get_NY  ());
			pParameters->Set_Callback(true);

			if( !Dlg_Parameters(pParameters, _TL("Clip to Extent")) )
			{
				return( false );
			}

			System	= Fit_Extent(*Get_System(), CSG_Rect(
				(*pParameters)("XMIN")->asDouble(),
				(*pParameters)("YMIN")->asDouble(),
				(*pParameters)("XMAX")->asDouble(),
				(*pParameters)("YMAX")->asDouble()
			));

			if( !System.is_Valid() )
			{
				return( false );
			}

			//---------------------------------------------
			CSG_Parameter_Grid_List	*pInput	= Parameters("GRIDS")->asGridList();

			for(int i=0; i<pInput->Get_Item_Count(); i++)
			{
				CSG_Data_Object	*pClip, *pObject	= pInput->Get_Item(i);

				switch( pObject->Get_ObjectType() )
				{
				default:
					{
						CSG_Grid	*pGrid	= (CSG_Grid  *)pObject;

						pClip	= SG_Create_Grid(System, pGrid->Get_Type());

						((CSG_Grid  *)pClip)->Set_Unit   (pGrid ->Get_Unit());
						((CSG_Grid  *)pClip)->Set_Scaling(pGrid ->Get_Scaling(), pGrid ->Get_Offset());

						pClip->Set_NoData_Value_Range(pObject->Get_NoData_Value(), pObject->Get_NoData_hiValue());

						((CSG_Grid  *)pClip)->Assign(pGrid, GRID_RESAMPLING_NearestNeighbour);
					}
					break;

				case SG_DATAOBJECT_TYPE_Grids:
					{
						CSG_Grids	*pGrids	= (CSG_Grids *)pObject;

						pClip	= SG_Create_Grids(System, pGrids->Get_Attributes(), pGrids->Get_Z_Attribute(), pGrids->Get_Type(), true);

						((CSG_Grids *)pClip)->Set_Unit   (pGrids->Get_Unit());
						((CSG_Grids *)pClip)->Set_Scaling(pGrids->Get_Scaling(), pGrids->Get_Offset());

						pClip->Set_NoData_Value_Range(pObject->Get_NoData_Value(), pObject->Get_NoData_hiValue());

						((CSG_Grids *)pClip)->Assign(pGrids, GRID_RESAMPLING_NearestNeighbour);
					}
					break;
				}

				pClip->Set_Name       (pObject->Get_Name());
				pClip->Set_Description(pObject->Get_Description());

				pClip->Get_MetaData().Assign(pObject->Get_MetaData());

				Parameters("CLIPPED")->asGridList()->Add_Item(pClip);

				DataObject_Add(pClip);
				DataObject_Set_Parameters(pClip, pObject);
			}

			if( Parameters("RUN_ONCE")->asBool() )
			{
				Stop_Execution(false);
			}
		}

		break;

	//-----------------------------------------------------
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Clip::CGrid_Clip(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Clip Grids"));

	Set_Author		("O.Conrad (c) 2003");

	Set_Description	(_TW(
		"Clip selected grids to specified extent."
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List("",
		"GRIDS"		, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List("",
		"CLIPPED"	, _TL("Clipped Grids"),
		_TL(""),
		PARAMETER_OUTPUT, false
	);

	//-----------------------------------------------------
	Parameters.Add_Choice("",
		"EXTENT"	, _TL("Extent"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|",
			_TL("user defined"),
			_TL("grid system"),
			_TL("shapes extent"),
			_TL("polygon")
		), 0
	);

	Parameters.Add_Grid_System("EXTENT",
		"GRIDSYSTEM", _TL("Grid System"),
		_TL("")
	);

	Parameters.Add_Shapes("EXTENT",
		"SHAPES"	, _TL("Shapes Extent"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Shapes("EXTENT",
		"POLYGONS"	, _TL("Polygon"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Polygon
	);

	Parameters.Add_Double("EXTENT", "XMIN", _TL("Left"   ), _TL(""));
	Parameters.Add_Double("EXTENT", "XMAX", _TL("Right"  ), _TL(""));
	Parameters.Add_Double("EXTENT", "YMIN", _TL("Bottom" ), _TL(""));
	Parameters.Add_Double("EXTENT", "YMAX", _TL("Top"    ), _TL(""));
	Parameters.Add_Int   ("EXTENT", "NX"  , _TL("Columns"), _TL(""), 1, 1, true);
	Parameters.Add_Int   ("EXTENT", "NY"  , _TL("Rows"   ), _TL(""), 1, 1, true);

	Parameters.Add_Double("",
		"BUFFER"	, _TL("Buffer"),
		_TL("add buffer (map units) to extent"),
		0.0, 0.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Clip::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	CSG_Grid_System	*pSystem	= pParameters->Get_Parameter("PARAMETERS_GRID_SYSTEM")->asGrid_System();

	if( !SG_STR_CMP(pParameter->Get_Identifier(), "PARAMETERS_GRID_SYSTEM") && pSystem && pSystem->is_Valid() )
	{
		pParameters->Set_Parameter("XMIN", pSystem->Get_XMin());
		pParameters->Set_Parameter("XMAX", pSystem->Get_XMax());
		pParameters->Set_Parameter("YMIN", pSystem->Get_YMin());
		pParameters->Set_Parameter("YMAX", pSystem->Get_YMax());
	}

	Fit_Extent(pParameters, pParameter, pSystem);

	return( CSG_Tool_Grid::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CGrid_Clip::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "EXTENT") )
	{
		pParameters->Set_Enabled("XMIN"      , pParameter->asInt() == 0);
		pParameters->Set_Enabled("XMAX"      , pParameter->asInt() == 0);
		pParameters->Set_Enabled("YMIN"      , pParameter->asInt() == 0);
		pParameters->Set_Enabled("YMAX"      , pParameter->asInt() == 0);
		pParameters->Set_Enabled("NX"        , pParameter->asInt() == 0);
		pParameters->Set_Enabled("NY"        , pParameter->asInt() == 0);
		pParameters->Set_Enabled("GRIDSYSTEM", pParameter->asInt() == 1);
		pParameters->Set_Enabled("SHAPES"    , pParameter->asInt() == 2);
		pParameters->Set_Enabled("POLYGONS"  , pParameter->asInt() == 3);
		pParameters->Set_Enabled("BUFFER"    , pParameter->asInt() != 3);	// no buffering for polygon clip
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Clip::On_Execute(void)
{
	//--------------------------------------------------------
	CSG_Rect	Extent;

	switch( Parameters("EXTENT")->asInt() )
	{
	case 0:	// user defined
		Extent.Assign(
			Parameters("XMIN")->asDouble(), Parameters("YMIN")->asDouble(),
			Parameters("XMAX")->asDouble(), Parameters("YMAX")->asDouble()
		);
		break;

	case 1: // grid system
		Extent.Assign(Parameters("GRIDSYSTEM")->asGrid_System()->Get_Extent());
		break;

	case 2:	// shapes extent
		Extent.Assign(Parameters("SHAPES"  )->asShapes()->Get_Extent());
		break;

	case 3:	// polygon
		Extent.Assign(Parameters("POLYGONS")->asShapes()->Get_Extent());
		break;
	}

	if( Parameters("BUFFER")->asDouble() > 0.0 && Parameters("EXTENT")->asInt() != 3 )	// no buffering for polygon clip
	{
		Extent.Inflate(Parameters("BUFFER")->asDouble(), false);
	}

	//--------------------------------------------------------
	CSG_Grid_System	System	= Fit_Extent(*Get_System(), Extent);

	if( !System.is_Valid() )
	{
		Error_Set(_TL("clip extent does not match grid's extent"));

		return( false );
	}

	//--------------------------------------------------------
	CSG_Grid	Mask;
	
	if( Parameters("EXTENT")->asInt() == 3 && !Get_Mask(Mask, System, Parameters("POLYGONS")->asShapes()) )
	{
		Error_Set(_TL("failed to create polygon clipping mask"));

		return( false );
	}

	//--------------------------------------------------------
	CSG_Parameter_Grid_List	*pInput	= Parameters("GRIDS")->asGridList();

	Parameters("CLIPPED")->asGridList()->Del_Items();

	for(int i=0; i<pInput->Get_Item_Count(); i++)
	{
		CSG_Data_Object	*pClip, *pObject	= pInput->Get_Item(i);

		switch( pObject->Get_ObjectType() )
		{
		default:
			{
				CSG_Grid	*pGrid	= (CSG_Grid  *)pObject;

				pClip	= SG_Create_Grid(System, pGrid->Get_Type());

				((CSG_Grid  *)pClip)->Set_Unit   (pGrid ->Get_Unit());
				((CSG_Grid  *)pClip)->Set_Scaling(pGrid ->Get_Scaling(), pGrid ->Get_Offset());

				pClip->Set_NoData_Value_Range(pObject->Get_NoData_Value(), pObject->Get_NoData_hiValue());

				((CSG_Grid  *)pClip)->Assign(pGrid, GRID_RESAMPLING_NearestNeighbour);
			}
			break;

		case SG_DATAOBJECT_TYPE_Grids:
			{
				CSG_Grids	*pGrids	= (CSG_Grids *)pObject;

				pClip	= SG_Create_Grids(System, pGrids->Get_Attributes(), pGrids->Get_Z_Attribute(), pGrids->Get_Type(), true);

				((CSG_Grids *)pClip)->Set_Unit   (pGrids->Get_Unit());
				((CSG_Grids *)pClip)->Set_Scaling(pGrids->Get_Scaling(), pGrids->Get_Offset());

				pClip->Set_NoData_Value_Range(pObject->Get_NoData_Value(), pObject->Get_NoData_hiValue());

				((CSG_Grids *)pClip)->Assign(pGrids, GRID_RESAMPLING_NearestNeighbour);
			}
			break;
		}

		pClip->Set_Name       (pObject->Get_Name());
		pClip->Set_Description(pObject->Get_Description());

		pClip->Get_MetaData().Assign(pObject->Get_MetaData());

		if( Mask.is_Valid() ) // && Parameters("EXTENT")->asInt() == 3 )	// polygon clip
		{
			for(int y=0; y<System.Get_NY() && Set_Progress(y, System.Get_NY()); y++)
			{
				#pragma omp parallel for
				for(int x=0; x<System.Get_NX(); x++)
				{
					if( Mask.is_NoData(x, y) )
					{
						switch( pObject->Get_ObjectType() )
						{
						default:
							((CSG_Grid *)pClip)->Set_NoData(x, y);
							break;

						case SG_DATAOBJECT_TYPE_Grids:
							{
								for(int z=0; z<((CSG_Grids *)pClip)->Get_NZ(); z++)
								{
									((CSG_Grids *)pClip)->Set_NoData(x, y, z);
								}
							}
							break;
						}
					}
				}
			}
		}

		Parameters("CLIPPED")->asGridList()->Add_Item(pClip);

		DataObject_Add(pClip);
		DataObject_Set_Parameters(pClip, pObject);
	}

	//--------------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Clip::Get_Mask(CSG_Grid &Mask, CSG_Grid_System &System, CSG_Shapes *pPolygons)
{
	//-----------------------------------------------------
	if( !Mask.Create(System, SG_DATATYPE_Char) )
	{
		return( false );
	}

	Mask.Set_NoData_Value(0);

	//-----------------------------------------------------
	for(int i=0; i<pPolygons->Get_Count() && Set_Progress(i, pPolygons->Get_Count()); i++)
	{
		CSG_Shape	*pPolygon	= pPolygons->Get_Shape(i);

        if( pPolygons->Get_Selection_Count() > 0 && !pPolygon->is_Selected() )
		{
			continue;
		}

		int	xA	= System.Get_xWorld_to_Grid(pPolygon->Get_Extent().Get_XMin()) - 1; if( xA <  0               ) xA = 0;
		int	xB	= System.Get_xWorld_to_Grid(pPolygon->Get_Extent().Get_XMax()) + 1; if( xB >= System.Get_NX() ) xB = System.Get_NX() - 1;

		//-------------------------------------------------
		#pragma omp parallel for
		for(int y=0; y<System.Get_NY(); y++)
		{
			double	yRow	= System.Get_yGrid_to_World(y);

			if( yRow >= pPolygon->Get_Extent().Get_YMin()
			&&  yRow <= pPolygon->Get_Extent().Get_YMax() )
			{
				TSG_Point	Row[2];

				Row[0].x	= System.Get_XMin(true) - 1.0;
				Row[1].x	= System.Get_XMax(true) + 1.0;
				Row[0].y	= Row[1].y	= yRow;

				//-----------------------------------------
				int	*nCrossings	= (int *)SG_Calloc(System.Get_NX(), sizeof(int));

				for(int iPart=0; iPart<pPolygon->Get_Part_Count(); iPart++)
				{
					TSG_Point	C, A, B	= pPolygon->Get_Point(0, iPart, false);	// last point

					for(int iPoint=0; iPoint<pPolygon->Get_Point_Count(iPart); iPoint++)
					{
						A	= B;	B	= pPolygon->Get_Point(iPoint, iPart, true);

						if( (A.y <= yRow && yRow <  B.y)
						||  (A.y >  yRow && yRow >= B.y) )
						{
							SG_Get_Crossing(C, A, B, Row[0], Row[1], false);

							int x	= System.Get_xWorld_to_Grid(C.x);	if( x < 0 )	x	= 0;

							if( x >= 0 && x < System.Get_NX() )
							{
								nCrossings[x]	= nCrossings[x] ? 0 : 1;
							}
						}
					}
				}

				//-----------------------------------------
				for(int x=xA, Fill=0; x<=xB; x++)
				{
					if( nCrossings[x] )
					{
						Fill	= Fill ? 0 : 1;
					}

					if( Fill )
					{
						Mask.Set_Value(x, y, 1);
					}
				}

				SG_Free(nCrossings);
			}
		}
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

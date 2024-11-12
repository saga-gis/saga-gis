
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                    pj_georeference                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    Georef_Grid.cpp                    //
//                                                       //
//                 Copyright (C) 2006 by                 //
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
//    e-mail:     oconrad@saga-gis.de                    //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Georef_Grid.h"


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGeoref_Grid::CGeoref_Grid(bool bList)
{
	m_bList	= bList;

	//-----------------------------------------------------
	Set_Name		(CSG_String::Format("%s %s", _TL("Rectify"), bList ? _TL("Grid List") : _TL("Grid")));

	Set_Author		("O.Conrad (c) 2006");

	Set_Description	(_TW(
		"Georeferencing and rectification for grids. Either choose the attribute fields (x/y) "
		"with the projected coordinates for the reference points (origin) or supply an "
		"additional points layer with correspondent points in the target projection. "
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"REF_SOURCE", _TL("Reference Points (Origin)"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Shapes("",
		"REF_TARGET", _TL("Reference Points (Projection)"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL, SHAPE_TYPE_Point
	);

	Parameters.Add_Table_Field("REF_SOURCE",
		"XFIELD"    , _TL("x Position"),
		_TL("")
	);

	Parameters.Add_Table_Field("REF_SOURCE",
		"YFIELD"    , _TL("y Position"),
		_TL("")
	);

	Parameters.Add_Choice("",
		"METHOD"    , _TL("Method"),
		_TL(""),
		GEOREF_METHODS_CHOICE, 0
	);

	Parameters.Add_Int("",
		"ORDER"     , _TL("Polynomial Order"),
		_TL(""),
		3, 1, true
	);

	//-----------------------------------------------------
	Parameters.Add_Choice("",
		"RESAMPLING", _TL("Resampling"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("Nearest Neighbour"),
			_TL("Bilinear Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 3
	);

	Parameters.Add_Bool("",
		"BYTEWISE"  , _TL("Bytewise Interpolation"),
		_TL(""),
		false
	);

	Parameters.Add_Data_Type("",
		"DATA_TYPE"		, _TL("Data Type"),
		_TL(""),
		SG_DATATYPES_Numeric, SG_DATATYPE_Undefined, _TL("Preserve")
	);

	//-----------------------------------------------------
	m_CRS.Create(Parameters);

	if( m_bList )
	{
		Parameters.Add_Grid_List("",
			"GRIDS" , _TL("Grids"),
			_TL(""),
			PARAMETER_INPUT
		);

		Parameters.Add_Grid_List("",
			"TARGET_GRIDS", _TL("Target"),
			_TL(""),
			PARAMETER_OUTPUT_OPTIONAL
		);

		m_Grid_Target.Create(&Parameters, false, "TARGET_NODE", "TARGET_");
	}
	else
	{
		Parameters.Add_Grid("",
			"GRID"  , _TL("Grid"),
			_TL(""),
			PARAMETER_INPUT
		);

		m_Grid_Target.Create(&Parameters, false, "TARGET_NODE", "TARGET_");

		m_Grid_Target.Add_Grid("TARGET_GRID", _TL("Target"), false);
	}
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGeoref_Grid::On_Before_Execution(void)
{
	m_CRS.Activate_GUI();

	return( CSG_Tool::On_Before_Execution() );
}

//---------------------------------------------------------
bool CGeoref_Grid::On_After_Execution(void)
{
	m_CRS.Deactivate_GUI();

	return( CSG_Tool::On_After_Execution() );
}

//---------------------------------------------------------
int CGeoref_Grid::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	m_CRS        .On_Parameter_Changed(pParameters, pParameter);
	m_Grid_Target.On_Parameter_Changed(pParameters, pParameter);

	if( pParameter->Cmp_Identifier("REF_SOURCE") && pParameter->asShapes() )
	{
		if( pParameter->asShapes()->Get_Field("X_MAP") >= 0 )
		{
			pParameters->Get_Parameter("XFIELD")->Set_Value(pParameter->asShapes()->Get_Field("X_MAP"));
		}

		if( pParameter->asShapes()->Get_Field("Y_MAP") >= 0 )
		{
			pParameters->Get_Parameter("YFIELD")->Set_Value(pParameter->asShapes()->Get_Field("Y_MAP"));
		}
	}

	if( pParameter->Cmp_Identifier("REF_SOURCE")
	||  pParameter->Cmp_Identifier("X_MAP"     )
	||  pParameter->Cmp_Identifier("Y_MAP"     )
	||  pParameter->Cmp_Identifier("XFIELD"    )
	||  pParameter->Cmp_Identifier("YFIELD"    )
	||  pParameter->Cmp_Identifier("PARAMETERS_GRID_SYSTEM") )
	{
		Set_Target_System(pParameters);
	}

	if( pParameter->Cmp_Identifier("GRID") ) // single grid
	{
		CSG_Parameter *pColorsType = DataObject_Get_Parameter(pParameter->asGrid(), "COLORS_TYPE");

		if( pColorsType )
		{
			pParameters->Set_Parameter("BYTEWISE", pColorsType->asInt() == 5); // RGB Coded Values?
		}
	}

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CGeoref_Grid::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("REF_TARGET") )
	{
		pParameters->Set_Enabled("XFIELD", pParameter->asShapes() == NULL);
		pParameters->Set_Enabled("YFIELD", pParameter->asShapes() == NULL);
	}

	if( pParameter->Cmp_Identifier("METHOD") )
	{
		pParameters->Set_Enabled("ORDER", pParameter->asInt() == GEOREF_Polynomial);	// only show for polynomial, user defined order
	}

	if( pParameter->Cmp_Identifier("RESAMPLING") )
	{
		pParameters->Set_Enabled("BYTEWISE" , pParameter->asInt() > 0);
		pParameters->Set_Enabled("DATA_TYPE", pParameter->asInt() > 0 || (*pParameters)("BYTEWISE")->asBool() == false);
	}

	if( pParameter->Cmp_Identifier("BYTEWISE") )
	{
		pParameters->Set_Enabled("DATA_TYPE", pParameter->asBool() == false || (*pParameters)("RESAMPLING")->asInt() > 0);
	}

	m_Grid_Target.On_Parameters_Enable(pParameters, pParameter);

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGeoref_Grid::On_Execute(void)
{
	if( Init_Engine(&Parameters) )
	{
		if( Rectify() )
		{
			m_Engine.Destroy();

			return( true );
		}
	}

	if( !m_Engine.Get_Error().is_Empty() )
	{
		Error_Set(m_Engine.Get_Error());
	}

	m_Engine.Destroy();

	return( false );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGeoref_Grid::Init_Engine(CSG_Parameters *pParameters)
{
	if( (*pParameters)("REF_SOURCE")
	&&  (*pParameters)("REF_TARGET")
	&&  (*pParameters)("XFIELD"    )
	&&  (*pParameters)("YFIELD"    )
	&&  (*pParameters)("METHOD"    )
	&&  (*pParameters)("ORDER"     ) )
	{
		CSG_Shapes *pShapes_A = (*pParameters)("REF_SOURCE")->asShapes();
		CSG_Shapes *pShapes_B = (*pParameters)("REF_TARGET")->asShapes();

		int xField = (*pParameters)("XFIELD")->asInt();
		int yField = (*pParameters)("YFIELD")->asInt();

		//-------------------------------------------------
		if( ( pShapes_B && m_Engine.Set_Reference(pShapes_A, pShapes_B))
		||	(!pShapes_B && m_Engine.Set_Reference(pShapes_A, xField, yField))	)
		{
			int Method = (*pParameters)("METHOD")->asInt();
			int  Order = (*pParameters)("ORDER" )->asInt();

			return( m_Engine.Evaluate(Method, Order) );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CGeoref_Grid::Set_Target_System(CSG_Parameters *pParameters)
{
	if( !pParameters || !pParameters->Get_Grid_System() || !pParameters->Get_Grid_System()->is_Valid() || !Init_Engine(pParameters) )
	{
		return( false );
	}

	CSG_Rect Extent;

	if( (*pParameters)("METHOD")->asInt() == GEOREF_Triangulation )
	{
		if( !m_Engine.Get_Reference_Extent(Extent) )
		{
			return( false );
		}
	}
	else
	{
		CSG_Grid_System System(*pParameters->Get_Grid_System());

		Extent.xMin = Extent.yMin = 1.;
		Extent.xMax = Extent.yMax = 0.;

		for(int y=0; y<System.Get_NY(); y++)
		{
			Add_Target_Extent(Extent, System.Get_XMin(), System.Get_yGrid_to_World(y));
			Add_Target_Extent(Extent, System.Get_XMax(), System.Get_yGrid_to_World(y));
		}

		for(int x=0; x<System.Get_NX(); x++)
		{
			Add_Target_Extent(Extent, System.Get_xGrid_to_World(x), System.Get_YMin());
			Add_Target_Extent(Extent, System.Get_xGrid_to_World(x), System.Get_YMax());
		}

		if( Extent.Get_XRange() <= 0. || Extent.Get_YRange() <= 0. )
		{
			return( false );
		}
	}

	if( !m_Grid_Target.Set_User_Defined(pParameters, Extent, pParameters->Get_Grid_System()->Get_NY()) )
	{
		return( false );
	}

	return( true );
}

//---------------------------------------------------------
inline void CGeoref_Grid::Add_Target_Extent(CSG_Rect &Extent, double x, double y)
{
	if( m_Engine.Get_Converted(x, y) )
	{
		if( Extent.Get_XRange() >= 0. && Extent.Get_YRange() >= 0. )
		{
			Extent.Union(CSG_Point(x, y));
		}
		else
		{
			Extent.Assign(x, y, x, y);
		}
	}
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSG_Data_Object * CGeoref_Grid::Get_Target(CSG_Data_Object *pSource, TSG_Data_Type Type)
{
	CSG_Data_Object *pTarget = NULL;

	if( pSource->asGrid() )
	{
		if( Type == SG_DATATYPE_Undefined )
		{
			Type = pSource->asGrid()->Get_Type();
		}

		pTarget = m_Grid_Target.Get_Grid("TARGET_GRID", Type != SG_DATATYPE_Undefined ? Type : pSource->asGrid()->Get_Type());

		if( pTarget )
		{
			pTarget->asGrid()->Set_Unit   (pSource->asGrid()->Get_Unit());
			pTarget->asGrid()->Set_Scaling(pSource->asGrid()->Get_Scaling(), pSource->asGrid()->Get_Offset());
			pTarget->asGrid()->Assign_NoData();
		}
	}

	//-----------------------------------------------------
	if( pSource->asGrids() )
	{
		if( Type == SG_DATATYPE_Undefined )
		{
			Type = pSource->asGrids()->Get_Type();
		}

		pTarget = SG_Create_Grids(m_Grid_Target.Get_System(),
			pSource->asGrids()->Get_Attributes(),
			pSource->asGrids()->Get_Z_Attribute(), Type, true
		);

		if( pTarget )
		{
			pTarget->asGrids()->Set_Unit   (pSource->asGrids()->Get_Unit());
			pTarget->asGrids()->Set_Scaling(pSource->asGrids()->Get_Scaling(), pSource->asGrids()->Get_Offset());
			pTarget->asGrids()->Assign_NoData();
		}
	}

	//-----------------------------------------------------
	if( pTarget )
	{
		pTarget->Set_Name   (pSource->Get_Name());
		pTarget->Set_NoData_Value_Range(pSource->Get_NoData_Value(), pSource->Get_NoData_Value(true));

		m_CRS.Get_CRS(pTarget->Get_Projection(), true);

		CSG_Parameters P;
	
		if( DataObject_Get_Parameters(pSource, P) )
		{
			DataObject_Add(pTarget);
	
			DataObject_Set_Parameters(pTarget, P);
		}
	}

	return( pTarget );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGeoref_Grid::Rectify(void)
{
	if( !m_Engine.is_Okay() || !m_Grid_Target.Get_System().is_Valid() )
	{
		return( false );
	}

	//-----------------------------------------------------
	TSG_Grid_Resampling Resampling;

	switch( Parameters("RESAMPLING")->asInt() )
	{
	default: Resampling = GRID_RESAMPLING_NearestNeighbour; break;
	case  1: Resampling = GRID_RESAMPLING_Bilinear        ; break;
	case  2: Resampling = GRID_RESAMPLING_BicubicSpline   ; break;
	case  3: Resampling = GRID_RESAMPLING_BSpline         ; break;
	}

	bool bBytewise = Parameters("BYTEWISE")->asBool();

	TSG_Data_Type Type = (Resampling == GRID_RESAMPLING_NearestNeighbour || Parameters("BYTEWISE")->asBool())
		? SG_DATATYPE_Undefined : Parameters("DATA_TYPE")->asDataType()->Get_Data_Type();

	//-----------------------------------------------------
	CSG_Array_Pointer Sources, Targets;

	if( m_bList )
	{
		CSG_Parameter_Grid_List *pSources = Parameters(       "GRIDS")->asGridList();
		CSG_Parameter_Grid_List *pTargets = Parameters("TARGET_GRIDS")->asGridList(); pTargets->Del_Items();

		for(int i=0; i<pSources->Get_Item_Count(); i++)
		{
			CSG_Data_Object *pSource = pSources->Get_Item(i);
			CSG_Data_Object *pTarget = Get_Target(pSource, Type);

			if( pTarget )
			{
				pTargets->Add_Item(pTarget);

				if( pSource->asGrid() )
				{
					Sources += pSource;
					Targets += pTarget;
				}
				else // if( pSource->asGrids() )
				{
					for(int j=0; j<pTarget->asGrids()->Get_NZ(); j++)
					{
						Sources += pSource->asGrids()->Get_Grid_Ptr(j);
						Targets += pTarget->asGrids()->Get_Grid_Ptr(j);
					}
				}
			}
		}
	}
	else
	{
		Sources += Parameters("GRID")->asGrid();
		Targets += Get_Target(Parameters("GRID")->asGrid(), Type);
	}

	//-----------------------------------------------------
	if( !Rectify(Sources, Targets, Resampling, bBytewise) )
	{
		Error_Set(_TL("failed to grids"));

		return( false );
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGeoref_Grid::Rectify(CSG_Array_Pointer &Sources, CSG_Array_Pointer &Targets, TSG_Grid_Resampling Resampling, bool bBytewise)
{
	if( !Sources.Get_Size() || Sources.Get_Size() < Targets.Get_Size() )
	{
		return( false );
	}

	CSG_Grid_System Reference(m_Grid_Target.Get_System());

	for(int y=0; y<Reference.Get_NY() && Set_Progress(y, Reference.Get_NY()); y++)
	{
		double py = Reference.Get_YMin() + y * Reference.Get_Cellsize();

		#pragma omp parallel for
		for(int x=0; x<Reference.Get_NX(); x++)
		{
			CSG_Point p(Reference.Get_XMin() + x * Reference.Get_Cellsize(), py);

			if( m_Engine.Get_Converted(p, true) )
			{
				for(sLong i=0; i<Targets.Get_Size(); i++)
				{
					double z;

					if( ((CSG_Grid *)Sources[i])->Get_Value(p, z, Resampling, false, bBytewise) )
					{
						((CSG_Grid *)Targets[i])->Set_Value(x, y, z);
					}
					else
					{
						((CSG_Grid *)Targets[i])->Set_NoData(x, y);
					}
				}
			}
			else for(sLong i=0; i<Targets.Get_Size(); i++)
			{
				((CSG_Grid *)Targets[i])->Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//                                                       //
//                                                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

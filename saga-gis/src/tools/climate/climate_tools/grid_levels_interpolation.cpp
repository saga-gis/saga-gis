
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     climate_tools                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//              Grid_Level_Interpolation.cpp             //
//                                                       //
//                 Copyright (C) 2012 by                 //
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
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "grid_levels_interpolation.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Levels_Interpolation::CGrid_Levels_Interpolation(void)
{
	//-----------------------------------------------------
	Set_Author("O.Conrad (c) 2012");

	//-----------------------------------------------------
	Parameters.Add_Grid_List("",
		"VARIABLE"		, _TL("Variable"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Choice("",
		"X_SOURCE"		, _TL("Get Heights from ..."),
		_TL(""),
		CSG_String::Format("%s|%s",
			_TL("table"),
			_TL("grid list")
		), 1
	);

	Parameters.Add_Grid_List("",
		"X_GRIDS"		, _TL("Level Heights"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid("",
		"X_GRIDS_CHECK"	, _TL("Minimum Height"),
		_TL("if set, only values with level heights above DEM will be used"),
		PARAMETER_INPUT_OPTIONAL, true
	);

	Parameters.Add_FixedTable("",
		"X_TABLE"		, _TL("Level Heights"),
		_TL("")
	)->asTable()->Add_Field(_TL("Height"), SG_DATATYPE_Double);

	Parameters.Add_Choice("",
		"H_METHOD"		, _TL("Horizontal Interpolation Method"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("Nearest Neighbour"),
			_TL("Bilinear Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 3
	);

	Parameters.Add_Choice("",
		"V_METHOD"		, _TL("Vertical Interpolation Method"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("linear"),
			_TL("spline"),
			_TL("polynomial trend")
		), 0
	);

	Parameters.Add_Bool("V_METHOD",
		"COEFFICIENTS"	, _TL("Coefficient Interpolation"),
		_TL(""),
		false
	);

	Parameters.Add_Bool("V_METHOD",
		"LINEAR_SORTED"	, _TL("Sorted Levels"),
		_TL(""),
		false
	);

	Parameters.Add_Bool("V_METHOD",
		"SPLINE_ALL"	, _TL("Pre-analyze"),
		_TL(""),
		false
	);

	Parameters.Add_Int("V_METHOD",
		"TREND_ORDER"	, _TL("Polynomial Order"),
		_TL(""),
		3, 1, true
	);

	//-----------------------------------------------------
	for(int i=0; i<10; i++)
	{
		Parameters("X_TABLE")->asTable()->Add_Record()->Set_Value(0, i + 1);
	}

	Add_Parameters("INTERNAL", "", "");

	m_Coeff	= NULL;
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Levels_Interpolation::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("V_METHOD") )
	{
		pParameters->Set_Enabled("COEFFICIENTS" , pParameter->asInt() != 1 && Parameters("SURFACE"));	// not available for splines, needs reference surface
		pParameters->Set_Enabled("LINEAR_SORTED", pParameter->asInt() == 0);
		pParameters->Set_Enabled("SPLINE_ALL"   , pParameter->asInt() == 1);
		pParameters->Set_Enabled("TREND_ORDER"  , pParameter->asInt() >= 2);
	}

	if( pParameter->Cmp_Identifier("X_SOURCE") )
	{
		pParameters->Set_Enabled("X_TABLE"      , pParameter->asInt() == 0);
		pParameters->Set_Enabled("X_GRIDS"      , pParameter->asInt() == 1);
		pParameters->Set_Enabled("X_GRIDS_CHECK", pParameter->asInt() == 1);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Levels_Interpolation::Initialize(const CSG_Rect &Extent)
{
	//-----------------------------------------------------
	m_pVariables		= Parameters("VARIABLE"     )->asGridList();
	m_pXGrids			= Parameters("X_GRIDS"      )->asGridList();
	m_pXTable			= Parameters("X_TABLE"      )->asTable();

	m_xSource			= Parameters("X_SOURCE"     )->asInt();
	m_vMethod			= Parameters("V_METHOD"     )->asInt();

	m_Linear_bSorted	= Parameters("LINEAR_SORTED")->asBool();
	m_Spline_bAll		= Parameters("SPLINE_ALL"   )->asBool() == false;
	m_Trend_Order		= Parameters("TREND_ORDER"  )->asInt();

	switch( Parameters("H_METHOD")->asInt() )
	{
	default:	m_hMethod	= GRID_RESAMPLING_NearestNeighbour;	break;
	case  1:	m_hMethod	= GRID_RESAMPLING_Bilinear        ;	break;
	case  2:	m_hMethod	= GRID_RESAMPLING_BicubicSpline   ;	break;
	case  3:	m_hMethod	= GRID_RESAMPLING_BSpline         ;	break;
	}

	//-----------------------------------------------------
	if( m_pVariables->Get_Grid_Count() != (m_xSource == 0 ? m_pXTable->Get_Count() : m_pXGrids->Get_Grid_Count()) )
	{
		Error_Set(_TL("variable and height levels have to be of same number"));

		return( false );
	}

	if( m_vMethod == 2 && m_pVariables->Get_Grid_Count() <= m_Trend_Order )
	{
		Error_Set(_TL("fitting a polynom of ith order needs at least i + 1 samples"));

		return( false );
	}

	if( !Extent.Intersects(Get_System().Get_Extent(true)) )
	{
		Error_Set(_TL("target area is distinct from levels area "));

		return( false );
	}

	//-----------------------------------------------------
	CSG_Grid	*pHeight_Min	= m_xSource == 1 && Parameters("X_GRIDS_CHECK") ? Parameters("X_GRIDS_CHECK")->asGrid() : NULL;

	if( pHeight_Min )
	{
		if( !Get_Parameters("INTERNAL")->Get_Parameter("X_GRIDS") )
		{
			Get_Parameters("INTERNAL")->Add_Grid_List("", "X_GRIDS", "", "", PARAMETER_INPUT_OPTIONAL);
		}

		CSG_Parameter_Grid_List	*pXGrids	= Get_Parameters("INTERNAL")->Get_Parameter("X_GRIDS")->asGridList();

		for(int i=0; i<m_pXGrids->Get_Grid_Count(); i++)
		{
			CSG_Grid	*pHeight	= SG_Create_Grid(*m_pXGrids->Get_Grid(i));

			#pragma omp parallel for
			for(int y=0; y<Get_NY(); y++)
			{
				for(int x=0; x<Get_NX(); x++)
				{
					if( pHeight->asDouble(x, y) < pHeight_Min->asDouble(x, y) )
					{
						pHeight->Set_NoData(x, y);
					}
				}
			}

			pXGrids->Add_Item(pHeight);
		}

		m_pXGrids	= pXGrids;
	}

	//-----------------------------------------------------
	if( m_vMethod == 0 && Parameters("COEFFICIENTS")->asBool() && Parameters("SURFACE") )	// linear coefficients interpolation
	{
		CSG_Grid	Surface(Get_System());

		Surface.Assign(Parameters("SURFACE")->asGrid(), GRID_RESAMPLING_Mean_Cells);

		m_Trend_Order	= 1;	// linear

		m_Coeff	= new CSG_Grid[1 + m_Trend_Order];

		for(int i=0; i<=m_Trend_Order; i++)
		{
			if( !m_Coeff[i].Create(Get_System()) )
			{
				return( false );
			}
		}

		#pragma omp parallel
		for(int y=0; y<Get_NY(); y++)
		{
			double	p_y	= Get_YMin() + y * Get_Cellsize();

			for(int x=0; x<Get_NX(); x++)
			{
				double	p_x	= Get_XMin() + x * Get_Cellsize();

				double	zz[2], vv[2];

				CSG_Trend_Polynom	Trend;	Trend.Set_Order(m_Trend_Order);

				if( Get_Linear_Coeff(p_x, p_y, Surface.asDouble(x, y), vv, zz) )
				{
					Trend.Add_Data(zz[0], vv[0]);
					Trend.Add_Data(zz[1], vv[1]);
				}

				if( Trend.Get_Trend() )
				{
					m_Coeff[0].Set_Value(x, y, Trend.Get_Coefficient(0));
					m_Coeff[1].Set_Value(x, y, Trend.Get_Coefficient(1));
				}
				else
				{
					m_Coeff[0].Set_NoData(x, y);
					m_Coeff[1].Set_NoData(x, y);
				}
			}
		}
	}

	//-----------------------------------------------------
	if( m_vMethod == 2 && Parameters("COEFFICIENTS")->asBool() )	// polynomial trend coefficients interpolation
	{
		int		i;

		m_Coeff	= new CSG_Grid[1 + m_Trend_Order];

		for(i=0; i<=m_Trend_Order; i++)
		{
			if( !m_Coeff[i].Create(Get_System()) )
			{
				return( false );
			}
		}

		#pragma omp parallel for private(i)
		for(int y=0; y<Get_NY(); y++)
		{
			double	p_y	= Get_YMin() + y * Get_Cellsize();

			for(int x=0; x<Get_NX(); x++)
			{
				double	p_x	= Get_XMin() + x * Get_Cellsize();

				CSG_Trend_Polynom	Trend;	Trend.Set_Order(m_Trend_Order);

				for(i=0; i<m_pVariables->Get_Grid_Count(); i++)
				{
					double	Height, Variable;

					if( Get_Height(p_x, p_y, i, Height) && Get_Variable(p_x, p_y, i, Variable) )
					{
						Trend.Add_Data(Height, Variable);
					}
				}

				if( Trend.Get_Trend() )
				{
					for(i=0; i<=m_Trend_Order; i++)
					{
						m_Coeff[i].Set_Value(x, y, Trend.Get_Coefficient(i));
					}
				}
				else
				{
					for(i=0; i<=m_Trend_Order; i++)
					{
						m_Coeff[i].Set_NoData(x, y);
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CGrid_Levels_Interpolation::Finalize(void)
{
	if( Get_Parameters("INTERNAL")->Get_Parameter("X_GRIDS")
	&&  Get_Parameters("INTERNAL")->Get_Parameter("X_GRIDS")->asGridList() == m_pXGrids )
	{
		for(int i=0; i<m_pXGrids->Get_Grid_Count(); i++)
		{
			delete(m_pXGrids->Get_Grid(i));
		}

		m_pXGrids->Del_Items();
	}

	if( m_Coeff )
	{
		delete[](m_Coeff);

		m_Coeff	= NULL;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline double CGrid_Levels_Interpolation::Get_Variable(double x, double y, int iLevel)
{
	return( m_pVariables->Get_Grid(iLevel)->Get_Value(x, y, m_hMethod) );
}

//---------------------------------------------------------
inline bool CGrid_Levels_Interpolation::Get_Variable(double x, double y, int iLevel, double &Variable)
{
	return( m_pVariables->Get_Grid(iLevel)->Get_Value(x, y, Variable, m_hMethod) );
}

//---------------------------------------------------------
inline double CGrid_Levels_Interpolation::Get_Height(double x, double y, int iLevel)
{
	if( m_xSource == 0 )
	{
		return( m_pXTable->Get_Record(iLevel)->asDouble(0) );
	}

	return( m_pXGrids->Get_Grid(iLevel)->Get_Value(x, y, m_hMethod) );
}

//---------------------------------------------------------
inline bool CGrid_Levels_Interpolation::Get_Height(double x, double y, int iLevel, double &Height)
{
	if( m_xSource == 0 )
	{
		Height	= m_pXTable->Get_Record(iLevel)->asDouble(0);

		return( true );
	}

	return( m_pXGrids->Get_Grid(iLevel)->Get_Value(x, y, Height, m_hMethod) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Levels_Interpolation::Get_Values(double x, double y, double z, int &iLevel, CSG_Table &Values)
{
	Values.Destroy();

	Values.Add_Field("h", SG_DATATYPE_Double);
	Values.Add_Field("v", SG_DATATYPE_Double);

	for(int i=0; i<m_pVariables->Get_Grid_Count(); i++)
	{
		double	Height, Variable;

		if( Get_Height(x, y, i, Height) && Get_Variable(x, y, i, Variable) )
		{
			CSG_Table_Record	*pRecord	= Values.Add_Record();

			pRecord->Set_Value(0, Height);
			pRecord->Set_Value(1, Variable);
		}
	}

	if( Values.Get_Count() < 2 || !Values.Set_Index(0, TABLE_INDEX_Ascending) )
	{
		return( false );
	}

	for(iLevel=1; iLevel<Values.Get_Count()-1; iLevel++)
	{
		if( Values[iLevel][0].asDouble() > z )
		{
			return( true );
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline bool CGrid_Levels_Interpolation::Get_Value(double x, double y, double z, double &Value)
{
	switch( m_vMethod )
	{
	default:	// linear
		if( m_Coeff )
		{
			return( Get_Trend_Coeff  (x, y, z, Value) );
		}
		else
		{
			return( Get_Linear       (x, y, z, Value) );
		}

	case  1:	// spline
		if( m_Spline_bAll )
		{
			return( Get_Spline_All   (x, y, z, Value) );
		}
		else
		{
			return( Get_Spline       (x, y, z, Value) );
		}

	case  2:	// polynomial trend
		if( m_Coeff )
		{
			return( Get_Trend_Coeff  (x, y, z, Value) );
		}
		else
		{
			return( Get_Trend        (x, y, z, Value) );
		}
	}
}

//---------------------------------------------------------
inline bool CGrid_Levels_Interpolation::Get_Value(const TSG_Point &p, double z, double &Value)
{
	return( Get_Value(p.x, p.y, z, Value) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Levels_Interpolation::Get_Linear_Coeff(double x, double y, double z, double vv[2], double hh[2])
{
	int		iLevel;

	if( m_Linear_bSorted )
	{
		for(iLevel=1; iLevel<m_pVariables->Get_Grid_Count()-1; iLevel++)
		{
			if( Get_Height(x, y, iLevel) > z )
			{
				break;
			}
		}

		hh[0]	= Get_Height  (x, y, iLevel - 1);
		vv[0]	= Get_Variable(x, y, iLevel - 1);
		hh[1]	= Get_Height  (x, y, iLevel    );
		vv[1]	= Get_Variable(x, y, iLevel    );
	}
	else
	{
		CSG_Table	Values;

		if( !Get_Values(x, y, z, iLevel, Values) )
		{
			return( false );
		}

		hh[0]	= Values[iLevel - 1][0];
		vv[0]	= Values[iLevel - 1][1];
		hh[1]	= Values[iLevel    ][0];
		vv[1]	= Values[iLevel    ][1];
	}

	return( hh[0] < hh[1] );
}

//---------------------------------------------------------
bool CGrid_Levels_Interpolation::Get_Linear(double x, double y, double z, double &Value)
{
	double	zz[2], vv[2];

	if( Get_Linear_Coeff(x, y, z, vv, zz) )
	{
		Value	= vv[0] + (z - zz[0]) * (vv[1] - vv[0]) / (zz[1] - zz[0]);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Levels_Interpolation::Get_Spline_All(double x, double y, double z, double &Value)
{
	CSG_Spline	Spline;

	for(int i=0; i<m_pVariables->Get_Grid_Count(); i++)
	{
		double	Height, Variable;

		if( Get_Height(x, y, i, Height) && Get_Variable(x, y, i, Variable) )
		{
			Spline.Add(Height, Variable);
		}
	}

	if( Spline.Get_Value(z, Value) )
	{
		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CGrid_Levels_Interpolation::Get_Spline(double x, double y, double z, double &Value)
{
	int			iLevel;
	CSG_Table	Values;

	if( !Get_Values(x, y, z, iLevel, Values) )
	{
		return( false );
	}

	if( Values.Get_Count() < 3 )
	{
		return( Get_Linear(x, y, z, Value) );
	}

	if( iLevel >= Values.Get_Count() - 1 )
	{
		iLevel--;
	}

	CSG_Spline	Spline;

	if( iLevel > 1 )
	{
		Spline.Add(Values[iLevel - 2][0], Values[iLevel - 2][1]);
	}

	Spline.Add(Values[iLevel - 1][0], Values[iLevel - 1][1]);
	Spline.Add(Values[iLevel    ][0], Values[iLevel    ][1]);
	Spline.Add(Values[iLevel + 1][0], Values[iLevel + 1][1]);

	return( Spline.Get_Value(z, Value) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Levels_Interpolation::Get_Trend(double x, double y, double z, double &Value)
{
	CSG_Trend_Polynom	Trend;

	Trend.Set_Order(m_Trend_Order);

	for(int i=0; i<m_pVariables->Get_Grid_Count(); i++)
	{
		double	Height, Variable;

		if( Get_Height(x, y, i, Height) && Get_Variable(x, y, i, Variable) )
		{
			Trend.Add_Data(Height, Variable);
		}
	}

	if( Trend.Get_Trend() )
	{
		Value	= Trend.Get_Value(z);

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Levels_Interpolation::Get_Trend_Coeff(double x, double y, double z, double &Value)
{
	double	Coeff, zPower	= 1.0;

	Value	= 0.0;

	for(int i=0; i<=m_Trend_Order; i++)
	{
		if( !m_Coeff[i].Get_Value(x, y, Coeff, m_hMethod) )
		{
			return( false );
		}

		Value	+= Coeff * zPower;
		zPower	*= z;
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Levels_to_Surface::CGrid_Levels_to_Surface(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Multi Level to Surface Interpolation"));

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_System("",
		"SYSTEM"	, _TL("Grid system"),
		_TL("")
	);

	Parameters.Add_Grid("SYSTEM",
		"SURFACE"	, _TL("Surface"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("SYSTEM",
		"RESULT"	, _TL("Interpolation"),
		_TL(""),
		PARAMETER_OUTPUT
	);
}

//---------------------------------------------------------
bool CGrid_Levels_to_Surface::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Grid	*pSurface	= Parameters("SURFACE")->asGrid();
	CSG_Grid	*pResult	= Parameters("RESULT" )->asGrid();

	if( !(pSurface->Get_System() == pResult->Get_System()) )
	{
		Error_Set(_TL("surface and result grids have to share the same grid system"));

		return( false );
	}

	if( !Initialize(pSurface->Get_Extent()) )
	{
		Finalize();

		return( false );
	}

	//-----------------------------------------------------
	for(int y=0; y<pSurface->Get_NY() && Set_Progress(y, pSurface->Get_NY()); y++)
	{
		double	p_y	= pSurface->Get_YMin() + y * pSurface->Get_Cellsize();

		#pragma omp parallel for
		for(int x=0; x<pSurface->Get_NX(); x++)
		{
			double	Value, p_x	= pSurface->Get_XMin() + x * pSurface->Get_Cellsize();

			if( !pSurface->is_NoData(x, y) && Get_Value(p_x, p_y, pSurface->asDouble(x, y), Value) )
			{
				pResult->Set_Value(x, y, Value);
			}
			else
			{
				pResult->Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	Finalize();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Levels_to_Points::CGrid_Levels_to_Points(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Multi Level to Points Interpolation"));

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	Parameters.Add_Shapes("",
		"POINTS"	, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Table_Field("POINTS",
		"ZFIELD"	, _TL("Height"),
		_TL("")
	);

	Parameters.Add_Shapes("POINTS",
		"RESULT"	, _TL("Result"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point
	);

	Parameters.Add_String("",
		"NAME"		, _TL("Field Name"),
		_TL(""),
		_TL("Variable")
	);
}

//---------------------------------------------------------
bool CGrid_Levels_to_Points::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Shapes	*pPoints	= Parameters("POINTS")->asShapes();

	if( !Initialize(pPoints->Get_Extent()) )
	{
		Finalize();

		return( false );
	}

	//-----------------------------------------------------
	if( Parameters("RESULT")->asShapes() && Parameters("RESULT")->asShapes() != pPoints )
	{
		Parameters("RESULT")->asShapes()->Create(*pPoints);

		pPoints	= Parameters("RESULT")->asShapes();
	}

	//-----------------------------------------------------
	int	zField	= Parameters("ZFIELD")->asInt();
	int	vField	= pPoints->Get_Field_Count();

	CSG_String	Name = Parameters("NAME")->asString(); if( Name.is_Empty() ) Name = _TL("Variable");

	pPoints->Add_Field(Name, SG_DATATYPE_Double);

	//-----------------------------------------------------
//	#pragma omp parallel for
	for(int iPoint=0; iPoint<pPoints->Get_Count() && Set_Progress(iPoint, pPoints->Get_Count()); iPoint++)
	{
		CSG_Shape	*pPoint	= pPoints->Get_Shape(iPoint);

		double	Value;

		if( !pPoint->is_NoData(zField) && Get_Value(pPoint->Get_Point(0), pPoint->asDouble(zField), Value) )
		{
			pPoint->Set_Value(vField, Value);
		}
		else
		{
			pPoint->Set_NoData(vField);
		}
	}

	//-----------------------------------------------------
	if( Parameters("RESULT")->asShapes() == NULL )
	{
		DataObject_Update(pPoints);
	}

	Finalize();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

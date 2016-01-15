/**********************************************************
 * Version $Id: Grid_Level_Interpolation.cpp 1380 2012-04-26 12:02:19Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
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
	Set_Author		(SG_T("O.Conrad (c) 2012"));

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL	, "VARIABLE"		, _TL("Variable"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Choice(
		NULL	, "X_SOURCE"		, _TL("Get Heights from ..."),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("table"),
			_TL("grid list")
		), 1
	);

	Parameters.Add_Grid_List(
		NULL	, "X_GRIDS"			, _TL("Level Heights"),
		_TL(""),
		PARAMETER_INPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL	, "X_GRIDS_CHECK"	, _TL("Minimum Height"),
		_TL("if set, only values with level heights above DEM will be used"),
		PARAMETER_INPUT_OPTIONAL, true
	);

	Parameters.Add_FixedTable(
		NULL	, "X_TABLE"			, _TL("Level Heights"),
		_TL("")
	)->asTable()->Add_Field(_TL("Height"), SG_DATATYPE_Double);

	Parameters.Add_Choice(
		NULL	, "H_METHOD"		, _TL("Horizontal Interpolation Method"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s|",
			_TL("Nearest Neighbour"),
			_TL("Bilinear Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 3
	);

	Parameters.Add_Choice(
		NULL	, "V_METHOD"		, _TL("Vertical Interpolation Method"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("linear"),
			_TL("spline"),
			_TL("polynomial trend"),
			_TL("polynomial trend (coefficient interpolation)")
		), 0
	);

	Parameters.Add_Value(
		NULL	, "LINEAR_SORTED"	, _TL("Sorted Levels"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Value(
		NULL	, "SPLINE_ALL"		, _TL("Pre-analyze"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Value(
		NULL	, "TREND_ORDER"		, _TL("Polynomial Order"),
		_TL(""),
		PARAMETER_TYPE_Int, 3, 1, true
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
	if( !SG_STR_CMP(pParameter->Get_Identifier(), SG_T("V_METHOD")) )
	{
		pParameters->Get_Parameter("LINEAR_SORTED")->Set_Enabled(pParameter->asInt() == 0);
		pParameters->Get_Parameter("SPLINE_ALL"   )->Set_Enabled(pParameter->asInt() == 1);
		pParameters->Get_Parameter("TREND_ORDER"  )->Set_Enabled(pParameter->asInt() >= 2);
	}

	if( !SG_STR_CMP(pParameter->Get_Identifier(), SG_T("X_SOURCE")) )
	{
		pParameters->Get_Parameter("X_TABLE"      )->Set_Enabled(pParameter->asInt() == 0);
		pParameters->Get_Parameter("X_GRIDS"      )->Set_Enabled(pParameter->asInt() == 1);
		pParameters->Get_Parameter("X_GRIDS_CHECK")->Set_Enabled(pParameter->asInt() == 1);
	}

	return( 1 );
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
	case  1:	m_hMethod	= GRID_RESAMPLING_Bilinear;			break;
	case  2:	m_hMethod	= GRID_RESAMPLING_BicubicSpline;	break;
	case  3:	m_hMethod	= GRID_RESAMPLING_BSpline;			break;
	}

	//-----------------------------------------------------
	if( m_pVariables->Get_Count() != (m_xSource == 0 ? m_pXTable->Get_Count() : m_pXGrids->Get_Count()) )
	{
		Error_Set(_TL("variable and height levels have to be of same number"));

		return( false );
	}

	if( m_vMethod == 1 && m_pVariables->Get_Count() <= m_Trend_Order )
	{
		Error_Set(_TL("fitting a polynom of ith order needs at least i + 1 samples"));

		return( false );
	}

	if( !Extent.Intersects(Get_System()->Get_Extent(true)) )
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
			Get_Parameters("INTERNAL")->Add_Grid_List(NULL, "X_GRIDS", "", "", PARAMETER_INPUT_OPTIONAL);
		}

		CSG_Parameter_Grid_List	*pXGrids	= Get_Parameters("INTERNAL")->Get_Parameter("X_GRIDS")->asGridList();

		for(int i=0; i<m_pXGrids->Get_Count(); i++)
		{
			CSG_Grid	*pHeight	= SG_Create_Grid(*m_pXGrids->asGrid(i));

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
	if( m_vMethod == 3 )	// polynom coefficient interpolation
	{
		int		i;

		m_Coeff	= new CSG_Grid[1 + m_Trend_Order];

		for(i=0; i<=m_Trend_Order; i++)
		{
			m_Coeff[i].Create(*Get_System());
		}

		#pragma omp parallel for private(i)
		for(int y=0; y<Get_NY(); y++)
		{
			double	p_y	= Get_YMin() + y * Get_Cellsize();

			for(int x=0; x<Get_NX(); x++)
			{
				double	p_x	= Get_XMin() + x * Get_Cellsize();

				CSG_Trend_Polynom	Trend;	Trend.Set_Order(m_Trend_Order);

				for(i=0; i<m_pVariables->Get_Count(); i++)
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
		for(int i=0; i<m_pXGrids->Get_Count(); i++)
		{
			delete(m_pXGrids->asGrid(i));
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
	return( m_pVariables->asGrid(iLevel)->Get_Value(x, y, m_hMethod) );
}

//---------------------------------------------------------
inline bool CGrid_Levels_Interpolation::Get_Variable(double x, double y, int iLevel, double &Variable)
{
	return( m_pVariables->asGrid(iLevel)->Get_Value(x, y, Variable, m_hMethod) );
}

//---------------------------------------------------------
inline double CGrid_Levels_Interpolation::Get_Height(double x, double y, int iLevel)
{
	if( m_xSource == 0 )
	{
		return( m_pXTable->Get_Record(iLevel)->asDouble(0) );
	}

	return( m_pXGrids->asGrid(iLevel)->Get_Value(x, y, m_hMethod) );
}

//---------------------------------------------------------
inline bool CGrid_Levels_Interpolation::Get_Height(double x, double y, int iLevel, double &Height)
{
	if( m_xSource == 0 )
	{
		Height	= m_pXTable->Get_Record(iLevel)->asDouble(0);

		return( true );
	}

	return( m_pXGrids->asGrid(iLevel)->Get_Value(x, y, Height, m_hMethod) );
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

	for(int i=0; i<m_pVariables->Get_Count(); i++)
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
	default:
	case 0:	// linear
		if( m_Linear_bSorted )
		{
			return( Get_Linear_Sorted(x, y, z, Value) );
		}

		return( Get_Linear(x, y, z, Value) );

	case 1:	// spline
		if( m_Spline_bAll )
		{
			return( Get_Spline_All(x, y, z, Value) );
		}

		return( Get_Spline(x, y, z, Value) );

	case 2:	// polynomial trend
		return( Get_Trend(x, y, z, Value) );

	case 3:	// polynomial trend (coefficient interpolation)
		return( Get_Trend_Coeff(x, y, z, Value) );
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
bool CGrid_Levels_Interpolation::Get_Linear_Sorted(double x, double y, double z, double &Value)
{
	int		iLevel;

	for(iLevel=1; iLevel<m_pVariables->Get_Count()-1; iLevel++)
	{
		if( Get_Height(x, y, iLevel) > z )
		{
			break;
		}
	}

	//-----------------------------------------------------
	double	z0	= Get_Height(x, y, iLevel - 1);
	double	z1	= Get_Height(x, y, iLevel    );

	if( z0 < z1 )
	{
		double	v0	= Get_Variable(x, y, iLevel - 1);
		double	v1	= Get_Variable(x, y, iLevel    );

		Value	= v0 + (z - z0) * (v1 - v0) / (z1 - z0);

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CGrid_Levels_Interpolation::Get_Linear(double x, double y, double z, double &Value)
{
	int			iLevel;
	CSG_Table	Values;

	if( !Get_Values(x, y, z, iLevel, Values) )
	{
		return( false );
	}

	//-----------------------------------------------------
	double	z0	= Values[iLevel - 1][0];
	double	z1	= Values[iLevel    ][0];

	if( z0 < z1 )
	{
		double	v0	= Values[iLevel - 1][1];
		double	v1	= Values[iLevel    ][1];

		Value	= v0 + (z - z0) * (v1 - v0) / (z1 - z0);

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

	for(int i=0; i<m_pVariables->Get_Count(); i++)
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

	for(int i=0; i<m_pVariables->Get_Count(); i++)
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

	Set_Author		(SG_T("O.Conrad (c) 2012"));

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	CSG_Parameter	*pNode	= Parameters.Add_Grid_System(
		NULL	, "SYSTEM"	, _TL("Grid system"),
		_TL("")
	);

	Parameters.Add_Grid(
		pNode	, "SURFACE"	, _TL("Surface"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		pNode	, "RESULT"	, _TL("Interpolation"),
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

	Set_Author		(SG_T("O.Conrad (c) 2012"));

	Set_Description	(_TW(
		""
	));

	//-----------------------------------------------------
	CSG_Parameter	*pNode	= Parameters.Add_Shapes(
		NULL	, "POINTS"	, _TL("Points"),
		_TL(""),
		PARAMETER_INPUT, SHAPE_TYPE_Point
	);

	Parameters.Add_Table_Field(
		pNode	, "ZFIELD"	, _TL("Height"),
		_TL("")
	);

	Parameters.Add_Shapes(
		pNode	, "RESULT"	, _TL("Result"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point
	);

	Parameters.Add_String(
		NULL	, "NAME"	, _TL("Field Name"),
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

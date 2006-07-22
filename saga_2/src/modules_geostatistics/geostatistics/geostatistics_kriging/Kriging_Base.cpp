
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                 Geostatistics_Kriging                 //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    Kriging_Base.cpp                   //
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
#include "Kriging_Base.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CKriging_Base::CKriging_Base(void)
{
	CParameter	*pNode;
	CParameters	*pParameters;

	//-----------------------------------------------------
	Parameters.Add_Grid_Output(
		NULL	, "GRID"		, _TL("Grid"),
		""
	);

	Parameters.Add_Grid_Output(
		NULL	, "VARIANCE"	, _TL("Variance"),
		""
	);

	pNode	= Parameters.Add_Shapes(
		NULL	, "SHAPES"		, _TL("Points"),
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "FIELD"		, _TL("Attribute"),
		""
	);

	Parameters.Add_Value(
		NULL	, "BVARIANCE"	, _TL("Create Variance Grid"),
		"",
		PARAMETER_TYPE_Bool		, true
	);

	Parameters.Add_Choice(
		NULL	, "TARGET"		, _TL("Target Grid"),
		"",

		CSG_String::Format("%s|%s|%s|",
			_TL("user defined"),
			_TL("grid system"),
			_TL("grid")
		), 0
	);

	Parameters.Add_Choice(
		NULL	, "MODEL"		, _TL("Variogram Model"),
		"",
		CSG_String::Format("%s|%s|%s|%s|%s|%s|",
			_TL("Spherical Model"),
			_TL("Exponential Model"),
			_TL("Gaussian Model"),
			_TL("Linear Regression"),
			_TL("Exponential Regression"),
			_TL("Power Function Regression")
		), 3
	);

	Parameters.Add_Value(
		NULL	, "BLOG"		, _TL("Logarithmic Transformation"),
		"",
		PARAMETER_TYPE_Bool
	);

	Parameters.Add_Value(
		NULL	, "NUGGET"		, _TL("Nugget"),
		"",
		PARAMETER_TYPE_Double, 0.0, 0.0, true
	);

	Parameters.Add_Value(
		NULL	, "SILL"		, _TL("Sill"),
		"",
		PARAMETER_TYPE_Double, 10.0, 0.0, true
	);

	Parameters.Add_Value(
		NULL	, "RANGE"		, _TL("Range"),
		"",
		PARAMETER_TYPE_Double, 100.0, 0.0, true
	);

	pNode	= Parameters.Add_Node(
		NULL	, "PARMS"		, _TL("Additional Parameters"),
		""
	);

	Parameters.Add_Value(
		pNode	, "LIN_B"		, _TL("Linear Regression"),
		_TL("Parameter B for Linear Regression:\n   y = Nugget + B * x"),
		PARAMETER_TYPE_Double, 1.0
	);

	Parameters.Add_Value(
		pNode	, "EXP_B"		, _TL("Exponential Regression"),
		_TL("Parameter B for Exponential Regression:\n   y = Nugget * e ^ (B * x)"),
		PARAMETER_TYPE_Double, 0.1
	);

	Parameters.Add_Value(
		pNode	, "POW_A"		, _TL("Power Function - A"),
		_TL("Parameter A for Power Function Regression:\n   y = A * x ^ B"),
		PARAMETER_TYPE_Double, 1.0
	);

	Parameters.Add_Value(
		pNode	, "POW_B"		, _TL("Power Function - B"),
		_TL("Parameter B for Power Function Regression:\n   y = A * x ^ B"),
		PARAMETER_TYPE_Double, 0.5
	);

	//-----------------------------------------------------
	pParameters	= Add_Extra_Parameters("USER"	, _TL("User defined grid")	, "");

	pParameters->Add_Value(
		NULL	, "CELL_SIZE"	, _TL("Grid Size"),
		"",
		PARAMETER_TYPE_Double, 100.0, 0.0, true
	);

	pNode	= pParameters->Add_Value(
		NULL	, "FIT_EXTENT"	, _TL("Fit Extent"),
		_TL("Automatically fits the grid to the shapes layers extent."),
		PARAMETER_TYPE_Bool		, true
	);

	pParameters->Add_Range(
		pNode	, "X_EXTENT"	, _TL("X-Extent"),
		""
	);

	pParameters->Add_Range(
		pNode	, "Y_EXTENT"	, _TL("Y-Extent"),
		""
	);

	//-----------------------------------------------------
	pParameters	= Add_Extra_Parameters("SYSTEM"	, _TL("Choose Grid System")	, "");

	pParameters->Add_Grid_System(
		NULL	, "SYSTEM"		, _TL("Grid System"),
		""
	);

	//-----------------------------------------------------
	pParameters	= Add_Extra_Parameters("GRID"	, _TL("Choose Grid")		, "");

	pNode	= pParameters->Add_Grid_System(
		NULL	, "SYSTEM"		, _TL("Grid System"),
		""
	);

	pParameters->Add_Grid(
		pNode	, "GRID"		, _TL("Grid"),
		"",
		PARAMETER_INPUT	, false
	);

	pParameters->Add_Grid(
		pNode	, "VARIANCE"	, _TL("Variance"),
		"",
		PARAMETER_INPUT_OPTIONAL, false
	);
}

//---------------------------------------------------------
CKriging_Base::~CKriging_Base(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CKriging_Base::On_Execute(void)
{
	bool	bResult	= false;

	m_pShapes	= NULL;

	m_bLog		= Parameters("BLOG")	->asBool();

	m_Model		= Parameters("MODEL")	->asInt();

	m_Nugget	= Parameters("NUGGET")	->asDouble();
	m_Sill		= Parameters("SILL")	->asDouble() - m_Nugget;	
	m_Range		= Parameters("RANGE")	->asDouble();

	m_BLIN		= Parameters("LIN_B")	->asDouble();
	m_BEXP		= Parameters("EXP_B")	->asDouble();
	m_APOW		= Parameters("POW_A")	->asDouble();
	m_BPOW		= Parameters("POW_B")	->asDouble();

	//-----------------------------------------------------
	if( _Get_Grid() && _Get_Points() && On_Initialise() )
	{
		int		ix, iy;
		double	x, y, z, v;

		for(iy=0, y=m_pGrid->Get_YMin(); iy<m_pGrid->Get_NY() && Set_Progress(iy, m_pGrid->Get_NY()); iy++, y+=m_pGrid->Get_Cellsize())
		{
			for(ix=0, x=m_pGrid->Get_XMin(); ix<m_pGrid->Get_NX(); ix++, x+=m_pGrid->Get_Cellsize())
			{
				if( Get_Value(x, y, z, v) )
				{
					m_pGrid->Set_Value(ix, iy, m_bLog ? exp(z) : z);

					if( m_pVariance )
					{
						m_pVariance->Set_Value(ix, iy, v);
					}
				}
				else
				{
					m_pGrid->Set_NoData(ix, iy);

					if( m_pVariance )
					{
						m_pVariance->Set_NoData(ix, iy);
					}
				}
			}
		}

		bResult	= true;
	}

	//-----------------------------------------------------
	m_Points.Clear();
	m_Search.Destroy();
	m_G		.Destroy();
	m_W		.Destroy();

	if( m_pShapes && m_pShapes != Parameters("SHAPES")->asShapes() )
	{
		delete(m_pShapes);
	}

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CKriging_Base::Get_Weight(double d)
{
	if( d > 0.0 )
	{
		switch( m_Model )
		{
		case 0:
			return(	// Spherical Model
				d >= m_Range
			?	m_Nugget + m_Sill
			:	m_Nugget + m_Sill * (3.0 * d / (2.0 * m_Range) - d*d*d / (2.0 * m_Range*m_Range*m_Range))
			);

		case 1:
			return(	// Exponential Model
				m_Nugget + m_Sill * (1.0 - exp(-3.0 * d / m_Range))
			);

		case 2:
			return(	// Gaussian Model
				m_Nugget + m_Sill * SG_Get_Square(1.0 - exp(-3.0 * d / (m_Range*m_Range)))
			);

		case 3: default:
			return(	// Linear Regression
				m_Nugget + d * m_BLIN
			);

		case 4:
			return(	// Exponential Regression
				m_Nugget * exp(d * m_BEXP)
			);

		case 5:
			return(	// Power Function Regression
				m_Nugget + m_APOW * pow(d, m_BPOW)
			);
		}
	}

	return( m_Nugget );
}

//---------------------------------------------------------
double CKriging_Base::Get_Weight(double dx, double dy)
{
	return( Get_Weight(sqrt(dx*dx + dy*dy)) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CKriging_Base::_Get_Points(void)
{
	int		iShape, iPart, iPoint;
	CShape	*pShape , *pPoint;
	CShapes	*pPoints;

	m_pShapes	= Parameters("SHAPES")	->asShapes();
	m_zField	= Parameters("FIELD")	->asInt();

	if( m_pShapes->Get_Type() != SHAPE_TYPE_Point )
	{
		pPoints	= SG_Create_Shapes(SHAPE_TYPE_Point, "", &m_pShapes->Get_Table());

		for(iShape=0; iShape<m_pShapes->Get_Count() && Set_Progress(iShape, m_pShapes->Get_Count()); iShape++)
		{
			pShape	= m_pShapes->Get_Shape(iShape);

			for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					pPoint	= pPoints->Add_Shape(pShape->Get_Record());
					pPoint->Add_Point(pShape->Get_Point(iPoint, iPart));
				}
			}
		}

		m_pShapes	= pPoints;
	}

	return( m_pShapes->Get_Count() > 1 );
}

//---------------------------------------------------------
bool CKriging_Base::_Get_Grid(void)
{
	CShapes	*pShapes	= Parameters("SHAPES")->asShapes();

	m_pGrid		= NULL;
	m_pVariance	= NULL;

	//-------------------------------------------------
	switch( Parameters("TARGET")->asInt() )
	{
	case 0:	// user defined...
		if( Dlg_Extra_Parameters("USER") )
		{
			m_pGrid		= _Get_Grid(pShapes->Get_Extent());
		}
		break;

	case 1:	// grid system...
		if( Dlg_Extra_Parameters("SYSTEM") )
		{
			m_pGrid		= SG_Create_Grid(*Get_Extra_Parameters("SYSTEM")->Get_Parameter("SYSTEM")->asGrid_System(), GRID_TYPE_Float);
		}
		break;

	case 2:	// grid...
		if( Dlg_Extra_Parameters("GRID") )
		{
			m_pGrid		= Get_Extra_Parameters("GRID")->Get_Parameter("GRID")		->asGrid();
			m_pVariance	= Get_Extra_Parameters("GRID")->Get_Parameter("VARIANCE")	->asGrid();
		}
		break;
	}

	//-------------------------------------------------
	if( m_pGrid )
	{
		if( !m_pVariance && Parameters("BVARIANCE")->asBool() )
		{
			m_pVariance	= SG_Create_Grid(m_pGrid, GRID_TYPE_Float);
		}

		m_pGrid->Set_Name(CSG_String::Format("%s (%s)", pShapes->Get_Name(), Get_Name()));
		Parameters("GRID")->Set_Value(m_pGrid);

		if( m_pVariance )
		{
			m_pVariance->Set_Name(CSG_String::Format("%s (%s - %s)", pShapes->Get_Name(), Get_Name(), _TL("Variance")));
			Parameters("VARIANCE")->Set_Value(m_pVariance);
		}

		if( Parameters("TARGET")->asInt() == 2 )
		{
			Get_Extra_Parameters("GRID")->Get_Parameter("VARIANCE")->Set_Value(m_pVariance);
		}
	}

	//-----------------------------------------------------
	return( m_pGrid != NULL );
}

//---------------------------------------------------------
CGrid * CKriging_Base::_Get_Grid(TSG_Rect Extent)
{
	CParameters	*P	= Get_Extra_Parameters("USER");

	if( !P->Get_Parameter("FIT_EXTENT")->asBool() )
	{
		Extent.xMin	= P->Get_Parameter("X_EXTENT")->asRange()->Get_LoVal();
		Extent.yMin	= P->Get_Parameter("Y_EXTENT")->asRange()->Get_LoVal();
		Extent.xMax	= P->Get_Parameter("X_EXTENT")->asRange()->Get_HiVal();
		Extent.yMax	= P->Get_Parameter("Y_EXTENT")->asRange()->Get_HiVal();
	}

	double	d	= P->Get_Parameter("CELL_SIZE")->asDouble();

	int		nx	= 1 + (int)((Extent.xMax - Extent.xMin) / d);
	int		ny	= 1 + (int)((Extent.yMax - Extent.yMin) / d);

	return( nx > 1 && ny > 1 ? SG_Create_Grid(GRID_TYPE_Float, nx, ny, d, Extent.xMin, Extent.yMin) : NULL );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

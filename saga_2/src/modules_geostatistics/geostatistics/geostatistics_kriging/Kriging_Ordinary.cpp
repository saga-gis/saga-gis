
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
//                 Kriging_Ordinary.cpp                  //
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
#include "Kriging_Ordinary.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CKriging_Ordinary::CKriging_Ordinary(void)
{
	CParameter	*pNode_0, *pNode_1;
	CParameters	*pParameters;

	//-----------------------------------------------------
	Set_Name(_TL("Ordinary Kriging"));

	Set_Author(_TL("Copyrights (c) 2003 by Olaf Conrad"));

	Set_Description(
		_TL("Ordinary Kriging for grid interpolation from irregular sample points.")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid_Output(
		NULL	, "GRID"		, _TL("Grid"),
		""
	);

	Parameters.Add_Grid_Output(
		NULL	, "VARIANCE"	, _TL("Variance"),
		""
	);

	//-----------------------------------------------------
	pNode_0	= Parameters.Add_Shapes(
		NULL	, "SHAPES"		, _TL("Points"),
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode_0	, "FIELD"		, _TL("Attribute"),
		""
	);

	pNode_0	= Parameters.Add_Value(
		NULL	, "MAXRADIUS"	, _TL("Maximum Search Radius (map units)"),
		"",
		PARAMETER_TYPE_Double	, 1000.0, 0, true
	);

	pNode_0	= Parameters.Add_Range(
		NULL	, "NPOINTS"		, _TL("Min./Max. Number of Points"),
		"", 4, 20, 1, true
	);

	//-----------------------------------------------------
	pNode_0	= Parameters.Add_Choice(
		NULL	, "MODEL"		, _TL("Variogram Model"),
		"",
		CAPI_String::Format("%s|%s|%s|%s|%s|%s|",
			_TL("Spherical Model"),
			_TL("Exponential Model"),
			_TL("Gaussian Model"),
			_TL("Linear Regression"),
			_TL("Exponential Regression"),
			_TL("Power Function Regression")
		), 3
	);

	//-----------------------------------------------------
	pNode_0	= Parameters.Add_Value(
		NULL	, "LOGARITHMIC"	, _TL("Logarithmic Transformation"),
		"",
		PARAMETER_TYPE_Bool
	);

	pNode_0	= Parameters.Add_Value(
		NULL	, "NUGGET"		, _TL("Nugget"),
		"",
		PARAMETER_TYPE_Double, 0.0, 0.0, true
	);

	pNode_0	= Parameters.Add_Value(
		NULL	, "SILL"		, _TL("Sill"),
		"",
		PARAMETER_TYPE_Double, 10.0, 0.0, true
	);

	pNode_0	= Parameters.Add_Value(
		NULL	, "RANGE"		, _TL("Range"),
		"",
		PARAMETER_TYPE_Double, 100.0, 0.0, true
	);

	//-----------------------------------------------------
	pNode_0	= Parameters.Add_Node(
		NULL	, "PARMS"		, _TL("Additional Parameters"),
		""
	);

	pNode_1	= Parameters.Add_Value(
		pNode_0	, "PARM_LIN_B"	, _TL("Linear Regression"),
		_TL("Parameter B for Linear Regression:\n   y = Nugget + B * x"),
		PARAMETER_TYPE_Double, 1.0
	);

	pNode_1	= Parameters.Add_Value(
		pNode_0	, "PARM_EXP_B"	, _TL("Exponential Regression"),
		_TL("Parameter B for Exponential Regression:\n   y = Nugget * e ^ (B * x)"),
		PARAMETER_TYPE_Double, 0.1
	);

	pNode_1	= Parameters.Add_Value(
		pNode_0	, "PARM_POW_A"	, _TL("Power Function - A"),
		_TL("Parameter A for Power Function Regression:\n   y = A * x ^ B"),
		PARAMETER_TYPE_Double, 1.0
	);

	pNode_1	= Parameters.Add_Value(
		pNode_0	, "PARM_POW_B"	, _TL("Power Function - B"),
		_TL("Parameter B for Power Function Regression:\n   y = A * x ^ B"),
		PARAMETER_TYPE_Double, 0.5
	);

	pNode_0	= Parameters.Add_Choice(
		NULL	, "TARGET_TYPE"	, _TL("Target Dimensions"),
		"",
		CAPI_String::Format("%s|%s|%s|",
			_TL("User defined"),
			_TL("Grid Project"),
			_TL("Grid")
		), 0
	);

	//-----------------------------------------------------
	pParameters	= Add_Extra_Parameters("USER"	, _TL("User defined grid")	, "");

	pNode_0	= pParameters->Add_Value(
		NULL	, "CELL_SIZE"	, _TL("Grid Size"),
		"",
		PARAMETER_TYPE_Double, 100.0, 0.0, true
	);

	pNode_0	= pParameters->Add_Value(
		NULL	, "FIT_EXTENT"	, _TL("Fit Extent"),
		_TL("Automatically fits the grid to the shapes layers extent."),
		PARAMETER_TYPE_Bool		, true
	);

	pNode_1	= pParameters->Add_Range(
		pNode_0	, "X_EXTENT"	, _TL("X-Extent"),
		""
	);

	pNode_1	= pParameters->Add_Range(
		pNode_0	, "Y_EXTENT"	, _TL("Y-Extent"),
		""
	);

	//-----------------------------------------------------
	pParameters	= Add_Extra_Parameters("GRIDPRJ", _TL("Choose Project")		, "");

	pNode_0	= pParameters->Add_Grid(
		NULL	, "GRID"		, _TL("Grid"),
		"",
		PARAMETER_INPUT	, false
	);

	//-----------------------------------------------------
	pParameters	= Add_Extra_Parameters("GRID"	, _TL("Choose Grid")			, "");

	pNode_0	= pParameters->Add_Grid(
		NULL	, "GRID"		, _TL("Grid"),
		"",
		PARAMETER_INPUT	, false
	);

	pNode_0	= pParameters->Add_Grid(
		NULL	, "VARIANCE"	, _TL("Variance"),
		"",
		PARAMETER_INPUT	, false
	);
}

//---------------------------------------------------------
CKriging_Ordinary::~CKriging_Ordinary(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid * CKriging_Ordinary::Get_Target_Grid(CParameters *pParameters, CShapes *pShapes)
{
	int			nx, ny;
	double		Cell_Size, xMin, yMin, xMax, yMax;

	if( pParameters->Get_Parameter("FIT_EXTENT")->asBool() )
	{
		xMin	= pShapes->Get_Extent().m_rect.xMin;
		yMin	= pShapes->Get_Extent().m_rect.yMin;
		xMax	= pShapes->Get_Extent().m_rect.xMax;
		yMax	= pShapes->Get_Extent().m_rect.yMax;
	}
	else
	{
		xMin	= pParameters->Get_Parameter("X_EXTENT")->asRange()->Get_LoVal();
		yMin	= pParameters->Get_Parameter("Y_EXTENT")->asRange()->Get_LoVal();
		xMax	= pParameters->Get_Parameter("X_EXTENT")->asRange()->Get_HiVal();
		yMax	= pParameters->Get_Parameter("Y_EXTENT")->asRange()->Get_HiVal();
	}

	Cell_Size	= pParameters->Get_Parameter("CELL_SIZE")->asDouble();

	nx			= 1 + (int)((xMax - xMin) / Cell_Size);
	ny			= 1 + (int)((yMax - yMin) / Cell_Size);

	return( API_Create_Grid(GRID_TYPE_Float, nx, ny, Cell_Size, xMin, yMin) );
}

//---------------------------------------------------------
CShapes * CKriging_Ordinary::Get_Point_Shapes(CShapes *pShapes)
{
	int		iShape, iPart, iPoint;
	CShape	*pShape, *pPoint;
	CShapes	*pPoints;

	if( pShapes->Get_Type() != SHAPE_TYPE_Point )
	{
		pPoints	= API_Create_Shapes(SHAPE_TYPE_Point, NULL, &pShapes->Get_Table());

		for(iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
		{
			pShape	= pShapes->Get_Shape(iShape);

			for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					pPoint	= pPoints->Add_Shape(pShape->Get_Record());
					pPoint->Add_Point(pShape->Get_Point(iPoint, iPart));
				}
			}
		}

		return( pPoints );
	}

	return( pShapes );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CKriging_Ordinary::On_Execute(void)
{
	int		x, y, n;
	CShapes	*pShapes, *pShapes_Tmp;

	//-----------------------------------------------------
	pShapes			= Parameters("SHAPES")		->asShapes();
	zField			= Parameters("FIELD")		->asInt();

	Model			= Parameters("MODEL")		->asInt();
	bLogarithmic	= Parameters("LOGARITHMIC")	->asBool();

	Nugget			= Parameters("NUGGET")		->asDouble();
	Scale			= Parameters("SILL"  )		->asDouble() - Nugget;	
	Range			= Parameters("RANGE" )		->asDouble();

	BLIN			= Parameters("PARM_LIN_B")	->asDouble();
	BEXP			= Parameters("PARM_EXP_B")	->asDouble();
	APOW			= Parameters("PARM_POW_A")	->asDouble();
	BPOW			= Parameters("PARM_POW_B")	->asDouble();

	max_Radius		= Parameters("MAXRADIUS")	->asDouble();

	nPoints_Min		= (int)Parameters("NPOINTS")->asRange()->Get_LoVal();
	nPoints_Max		= (int)Parameters("NPOINTS")->asRange()->Get_HiVal();

	//-----------------------------------------------------
	if( pShapes->Get_Table().Get_Field_Count() > 0 )
	{
		pGrid		= NULL;

		switch( Parameters("TARGET_TYPE")->asInt() )
		{
		case 0:	// User defined...
			if( Dlg_Extra_Parameters("USER") )
			{
				pGrid		= Get_Target_Grid(Get_Extra_Parameters("USER"), pShapes);
				pVariance	= API_Create_Grid(pGrid);
			}
			break;

		case 1:	// Grid Project...
			if( Dlg_Extra_Parameters("GRIDPRJ") )
			{
				pGrid		= API_Create_Grid(Get_Extra_Parameters("GRIDPRJ")->Get_Parameter("GRID")->asGrid());
				pVariance	= API_Create_Grid(pGrid);
			}
			break;

		case 2:	// Grid...
			if( Dlg_Extra_Parameters("GRID") )
			{
				pGrid		= Get_Extra_Parameters("GRID")->Get_Parameter("GRID")		->asGrid();
				pVariance	= Get_Extra_Parameters("GRID")->Get_Parameter("VARIANCE")	->asGrid();
			}
			break;
		}

		//-------------------------------------------------
		if( pGrid )
		{
			pGrid					->Set_Name(CAPI_String::Format("%s (Kriging)", pShapes->Get_Name()));
			pGrid					->Assign_NoData();
			Parameters("GRID")		->Set_Value(pGrid);

			pVariance				->Set_Name(CAPI_String::Format("%s (Kriging - Variance)", pShapes->Get_Name()));
			pVariance				->Assign_NoData();
			Parameters("VARIANCE")	->Set_Value(pVariance);

			//---------------------------------------------
			if( SearchEngine.Create(pShapes_Tmp = Get_Point_Shapes(pShapes)) )
			{
				Points		= (TGEO_Position *)API_Malloc(nPoints_Max * sizeof(TGEO_Position));

				n			= nPoints_Max + 1;

				Gammas		= (double  *)API_Malloc(n * sizeof(double));

				Weights		= (double **)API_Malloc(n * sizeof(double *));
				Weights[0]	= (double  *)API_Malloc(n * n * sizeof(double));

				for(y=1; y<=nPoints_Max; y++)
				{
					Weights[y]	= Weights[0] + y * n;
				}

				//-----------------------------------------
				for(y=0; y<pGrid->Get_NY() && Set_Progress(y, pGrid->Get_NY()); y++)
				{
					for(x=0; x<pGrid->Get_NX(); x++)
					{
						Get_Grid_Value(x, y);
					}
				}

				//-----------------------------------------
				API_Free(Gammas);

				API_Free(Weights[0]);
				API_Free(Weights);

				API_Free(Points);

				SearchEngine.Destroy();

				if( pShapes_Tmp != pShapes )
				{
					delete(pShapes_Tmp);
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
bool CKriging_Ordinary::Get_Grid_Value(int x, int y)
{
	int			i, j, nPoints;
	double		xPos, yPos, dx, dy, Lambda, Value, Variance;

	//-----------------------------------------------------
	xPos	= pGrid->Get_XMin() + x * pGrid->Get_Cellsize();
	yPos	= pGrid->Get_YMin() + y * pGrid->Get_Cellsize();

	if(	(nPoints = Get_Weights(xPos, yPos)) >= nPoints_Min )
	{
		//-------------------------------------------------
		for(i=0; i<nPoints; i++)
		{
			dx			= Points[i].x - xPos;
			dy			= Points[i].y - yPos;

			Gammas[i]	= Get_Weight(sqrt(dx*dx + dy*dy));
		}

		Gammas[nPoints]	= 1.0;

		//-------------------------------------------------
		for(i=0, Value=0.0, Variance=0.0; i<nPoints; i++)
		{
			for(j=0, Lambda=0.0; j<=nPoints; j++)
			{
				Lambda	+= Weights[i][j] * Gammas[j];
			}

			Value	+= Lambda * Points[i].z;

			if( pVariance )
			{
				Variance	+= Lambda * Gammas[i];
			}
		}

		//-------------------------------------------------
		pGrid->Set_Value(x, y, bLogarithmic ? exp(Value) : Value);

		if( pVariance )
		{
			pVariance->Set_Value(x, y, Variance);
		}

		return( true );
	}

	//-----------------------------------------------------
	pGrid->Set_NoData(x, y);

	if( pVariance )
	{
		pVariance->Set_NoData(x, y);
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CKriging_Ordinary::Get_Weight(double d)
{
	if( d == 0.0 )
	{
		d	= 0.0001;
	}

	switch( Model )
	{
	case 0:				// Spherical Model
		if( d >= Range )
		{
			d	= Nugget + Scale;
		}
		else
		{
			d	= Nugget + Scale * (3 * d / ( 2 * Range) - d * d * d / (2 * Range * Range * Range));
		}
		break;

	case 1:				// Exponential Model
		d	= Nugget + Scale * (1 - exp(-3 * d / Range));
		break;

	case 2:				// Gaussian Model
		d	= 1 - exp(-3 * d / (Range * Range));
		d	= Nugget + Scale * d * d;
		break;

	case 3: default:	// Linear Regression
		d	= Nugget + d * BLIN;
		break;

	case 4:				// Exponential Regression
		d	= Nugget * exp(d * BEXP);
		break;

	case 5:				// Power Function Regression
		d	= APOW * pow(d, BPOW);
		break;
	}

	return( d );
}

//---------------------------------------------------------
int CKriging_Ordinary::Get_Weights(double x, double y)
{
	int			i, j, n;
	double		dx, dy;
	TGEO_Point	Point;
	CShape		*pPoint;

	//-----------------------------------------------------
	if( (n = SearchEngine.Select_Radius(x, y, max_Radius, true)) >= nPoints_Min )
	{
		if( n > nPoints_Max )
		{
			n	= nPoints_Max;
		}

		for(i=0; i<n; i++)
		{
			pPoint		= SearchEngine.Get_Selected_Point(i);
			Point		= pPoint->Get_Point(0);
			Points[i].x	= Point.x;
			Points[i].y	= Point.y;
			Points[i].z	= pPoint->Get_Record()->asDouble(zField);
		}

		//-------------------------------------------------
		for(i=0; i<n; i++)
		{
			Weights[i][i]	= 0.0;					// Diagonale...
			Weights[i][n]	= Weights[n][i]	= 1.0;	// Rand...

			for(j=i+1; j<n; j++)
			{
				dx				= Points[i].x - Points[j].x;
				dy				= Points[i].y - Points[j].y;

				Weights[i][j]	= Weights[j][i]	= Get_Weight(sqrt(dx*dx + dy*dy));
			}
		}

		Weights[n][n]	= 0.0;	// Rand-Diagonale...

		if( MATRIX_Invert(n + 1, Weights) )
		{
			return( n );
		}
	}

	return( 0 );
}

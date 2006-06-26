
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
//             Kriging_Universal_Global.cpp              //
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
#include "Kriging_Universal_Global.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CKriging_Universal_Global::CKriging_Universal_Global(void)
{
	CParameter	*pNode_0, *pNode_1;

	//-----------------------------------------------------
	Set_Name(_TL("Universal Kriging (Global)"));

	Set_Author(_TL("Copyrights (c) 2003 by Olaf Conrad"));

	Set_Description(_TL(
		"Universal Kriging for grid interpolation from irregular sample points.\n"
		"This implementation does not use a maximum search radius. The weighting "
		"matrix is generated globally for all points.")
	);

	//-----------------------------------------------------
	pNode_0	= Parameters.Add_Shapes(
		NULL	, "SHAPES"		, _TL("Points"),
		"",
		PARAMETER_INPUT
	);

	pNode_1	= Parameters.Add_Table_Field(
		pNode_0	, "FIELD"		, _TL("Attribute"),
		""
	);

	Parameters.Add_Grid_List(
		NULL	, "GRIDS"		, _TL("Grids"),
		"",
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "TARGET"		, _TL("Kriging"),
		"",
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "VARIANCE"	, _TL("Variance"),
		"",
		PARAMETER_OUTPUT
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
}

//---------------------------------------------------------
CKriging_Universal_Global::~CKriging_Universal_Global(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CKriging_Universal_Global::On_Execute(void)
{
	bool	bResult;
	int		x, y;
	CShapes	*pShapes;

	//-----------------------------------------------------
	bResult			= false;

	pShapes			= Parameters("SHAPES")		->asShapes();

	pGrids			= Parameters("GRIDS")		->asGridList();
	nGrids			= pGrids->Get_Count();

	pGrid			= Parameters("TARGET")		->asGrid();
	pVariance		= Parameters("VARIANCE")	->asGrid();

	Model			= Parameters("MODEL")		->asInt();
	bLogarithmic	= Parameters("LOGARITHMIC")	->asBool();

	Nugget			= Parameters("NUGGET")		->asDouble();
	Scale			= Parameters("SILL"  )		->asDouble() - Nugget;	
	Range			= Parameters("RANGE" )		->asDouble();

	BLIN			= Parameters("PARM_LIN_B")	->asDouble();
	BEXP			= Parameters("PARM_EXP_B")	->asDouble();
	APOW			= Parameters("PARM_POW_A")	->asDouble();
	BPOW			= Parameters("PARM_POW_B")	->asDouble();

	//-----------------------------------------------------
	if( pShapes->Get_Table().Get_Field_Count() > 0 )
	{
		pGrid->Set_Name(CAPI_String::Format("%s (Kriging)", pShapes->Get_Name()));
		pGrid->Assign_NoData();

		if( pVariance )
		{
			pVariance->Set_Name(CAPI_String::Format("%s (Kriging - Variance)", pShapes->Get_Name()));
			pVariance->Assign_NoData();
		}

		Points	= NULL;

		//-------------------------------------------------
		if( Get_Weights(pShapes, Parameters("FIELD")->asInt()) > 0 )
		{
			Process_Set_Text("Interpolation...");

			for(y=0; y<pGrid->Get_NY() && Set_Progress(y); y++)
			{
				for(x=0; x<pGrid->Get_NX(); x++)
				{
					Get_Grid_Value(x, y);
				}
			}

			API_Free(Gammas);

			API_Free(Weights[0]);
			API_Free(Weights);

			bResult	= true;
		}

		//-------------------------------------------------
		if( Points )
		{
			API_Free(Points);
		}
	}

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CKriging_Universal_Global::Get_Grid_Value(int x, int y)
{
	int		i, j;
	double	xPos, yPos, dx, dy, Lambda, Value, Variance;

	if(	nPoints > 1 )
	{
		//-------------------------------------------------
		xPos	= pGrid->Get_XMin() + x * pGrid->Get_Cellsize();
		yPos	= pGrid->Get_YMin() + y * pGrid->Get_Cellsize();

		for(i=0; i<nPoints; i++)
		{
			dx			= Points[i].x - xPos;
			dy			= Points[i].y - yPos;

			Gammas[i]	= Get_Weight(sqrt(dx*dx + dy*dy));
		}

		Gammas[nPoints]	= 1.0;

		for(i=0, j=nPoints+1; i<nGrids; i++, j++)
		{
			Gammas[j]	= pGrids->asGrid(i)->asDouble(x, y);
		}

		//-------------------------------------------------
		for(i=0, Value=0.0, Variance=0.0; i<nPoints; i++)
		{
			for(j=0, Lambda=0.0; j<=nPoints+nGrids; j++)
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
inline double CKriging_Universal_Global::Get_Weight(double d)
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
int CKriging_Universal_Global::Get_Weights(CShapes *pShapes, int zField)
{
	int				i, j, iGrid, n, iShape, iPart, iPoint, Interpolation = 1;
	double			dx, dy;
	CShape			*pShape;

	//-----------------------------------------------------
	if( pShapes )
	{
		Process_Set_Text(_TL("Reading points..."));

		nPoints	= 0;

		for(iShape=0; iShape<pShapes->Get_Count(); iShape++)
		{
			pShape	= pShapes->Get_Shape(iShape);

			for(iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				n		=  nPoints;
				nPoints	+= pShape->Get_Point_Count(iPart);
				Points	=  (TGEO_Position *)API_Realloc(Points, nPoints * sizeof(TGEO_Position));

				for(iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++, n++)
				{
					Points[n].x	= pShape->Get_Point(iPoint, iPart).x;
					Points[n].y	= pShape->Get_Point(iPoint, iPart).y;
					Points[n].z	= pShape->Get_Record()->asDouble(zField);
				}
			}
		}

		//-------------------------------------------------
		if( nPoints > 1 )
		{
			Process_Set_Text(_TL("Matrix Initiation..."));

			n			= nPoints + 1 + nGrids;

			Gammas		= (double  *)API_Malloc(n * sizeof(double));

			Weights		= (double **)API_Malloc(n * sizeof(double *));
			Weights[0]	= (double  *)API_Malloc(n * n * sizeof(double));

			for(i=1; i<n; i++)
			{
				Weights[i]	= Weights[0] + i * n;
			}

			//---------------------------------------------
			n			= nPoints;

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

				for(iGrid=0, j=n+1; iGrid<nGrids; iGrid++, j++)
				{
					Weights[i][j]	= Weights[j][i]	= pGrids->asGrid(iGrid)->Get_Value(Points[i].x, Points[i].y, Interpolation);
				}
			}

			for(i=n; i<=n+nGrids; i++)	// Rand-Diagonale bzw. Rest...
			{
				for(j=n; j<=n+nGrids; j++)
				{
					Weights[i][j]	= 0.0;
				}
			}

			//---------------------------------------------
			Process_Set_Text(_TL("Matrix Inversion..."));

			if( MATRIX_Invert(n + 1 + nGrids, Weights) )
			{
				return( n );
			}
		}
	}

	return( 0 );
}

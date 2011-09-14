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
//               geostatistics_regression                //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//             point_multi_grid_regression.cpp           //
//                                                       //
//                 Copyright (C) 2004 by                 //
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
#include "point_multi_grid_regression.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CPoint_Multi_Grid_Regression::CPoint_Multi_Grid_Regression(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Multiple Regression Analysis (Points/Grids)"));

	Set_Author		(SG_T("O.Conrad (c) 2004"));

	Set_Description	(_TW(
		"Linear regression analysis of point attributes with multiple grids. "
		"Details of the regression/correlation analysis will be saved to a table. "
		"The regression function is used to create a new grid with regression based values. "
		"The multiple regression analysis uses a forward selection procedure. \n"
		"\n"
		"Reference:\n"
		"- Bahrenberg, G., Giese, E., Nipper, J. (1992): "
		"'Statistische Methoden in der Geographie 2 - Multivariate Statistik', "
		"Stuttgart, 415p.\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL	, "GRIDS"		, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	pNode	= Parameters.Add_Shapes(
		NULL	, "SHAPES"		, _TL("Shapes"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Table_Field(
		pNode	, "ATTRIBUTE"	, _TL("Attribute"),
		_TL("")
	);

	Parameters.Add_Table(
		NULL	, "INFO_COEFF"	, _TL("Details: Coefficients"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Table(
		NULL	, "INFO_MODEL"	, _TL("Details: Model"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Table(
		NULL	, "INFO_STEPS"	, _TL("Details: Steps"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Shapes(
		NULL	, "RESIDUALS"	, _TL("Residuals"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL, SHAPE_TYPE_Point
	);

	Parameters.Add_Grid(
		NULL	, "REGRESSION"	, _TL("Regression"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice(
		NULL	,"INTERPOL"		, _TL("Grid Interpolation"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|"),
			_TL("Nearest Neighbor"),
			_TL("Bilinear Interpolation"),
			_TL("Inverse Distance Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 4
	);

	Parameters.Add_Value(
		NULL	, "COORD_X"		, _TL("Include X Coordinate"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Value(
		NULL	, "COORD_Y"		, _TL("Include Y Coordinate"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
	);

	Parameters.Add_Choice(
		NULL	,"METHOD"		, _TL("Method"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|"),
			_TL("include all"),
			_TL("forward"),
			_TL("backward"),
			_TL("stepwise")
		), 1
	);

	Parameters.Add_Value(
		NULL	, "P_IN"		, _TL("P in"),
		_TL("Level of significance for automated predictor selection, given as percentage"),
		PARAMETER_TYPE_Double, 5.0, 0.0, true, 100.0, true
	);

	Parameters.Add_Value(
		NULL	, "P_OUT"		, _TL("P out"),
		_TL("Level of significance for automated predictor selection, given as percentage"),
		PARAMETER_TYPE_Double, 5.0, 0.0, true, 100.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPoint_Multi_Grid_Regression::On_Execute(void)
{
	bool					bResult;
	int						iAttribute;
	double					P_in, P_out;
	CSG_Strings				Names;
	CSG_Matrix				Samples;
	CSG_Shapes				*pShapes;
	CSG_Grid				*pRegression;
	CSG_Parameter_Grid_List	*pGrids;

	//-----------------------------------------------------
	pGrids			= Parameters("GRIDS")		->asGridList();
	pRegression		= Parameters("REGRESSION")	->asGrid();
	pShapes			= Parameters("SHAPES")		->asShapes();
	iAttribute		= Parameters("ATTRIBUTE")	->asInt();
	P_in			= Parameters("P_IN")		->asDouble() / 100.0;
	P_out			= Parameters("P_OUT")		->asDouble() / 100.0;

	//-----------------------------------------------------
	if( !Get_Samples(pGrids, pShapes, iAttribute, Samples, Names) )
	{
		return( false );
	}

	//-----------------------------------------------------
	switch( Parameters("METHOD")->asInt() )
	{
	default:
	case 0:	bResult	= m_Regression.Calculate         (Samples             , &Names);	break;
	case 1:	bResult	= m_Regression.Calculate_Forward (Samples, P_in       , &Names);	break;
	case 2:	bResult	= m_Regression.Calculate_Backward(Samples,       P_out, &Names);	break;
	case 3:	bResult	= m_Regression.Calculate_Stepwise(Samples, P_in, P_out, &Names);	break;
	}

	if( bResult == false )
	{
		return( false );
	}

	Message_Add(m_Regression.Get_Info(), false);

	//-----------------------------------------------------
	Set_Regression(pGrids, pRegression, CSG_String::Format(SG_T("%s (%s)"), pShapes->Get_Name(), Get_Name()));

	Set_Residuals(pShapes, iAttribute, pRegression);

	//-----------------------------------------------------
	if( Parameters("INFO_COEFF")->asTable() )
	{
		Parameters("INFO_COEFF")->asTable()->Assign(m_Regression.Get_Regression());
		Parameters("INFO_COEFF")->asTable()->Set_Name(_TL("MLRA Coefficients"));
	}

	if( Parameters("INFO_MODEL")->asTable() )
	{
		Parameters("INFO_MODEL")->asTable()->Assign(m_Regression.Get_Model());
		Parameters("INFO_MODEL")->asTable()->Set_Name(_TL("MLRA Model"));
	}

	if( Parameters("INFO_STEPS")->asTable() )
	{
		Parameters("INFO_STEPS")->asTable()->Assign(m_Regression.Get_Steps());
		Parameters("INFO_STEPS")->asTable()->Set_Name(_TL("MLRA Steps"));
	}

	//-----------------------------------------------------
	m_Regression.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPoint_Multi_Grid_Regression::Get_Samples(CSG_Parameter_Grid_List *pGrids, CSG_Shapes *pShapes, int iAttribute, CSG_Matrix &Samples, CSG_Strings &Names)
{
	int			iGrid;
	double		zGrid;
	CSG_Vector	Sample;

	//-----------------------------------------------------
	int		Interpolation	= Parameters("INTERPOL")	->asInt();
	bool	bCoord_X		= Parameters("COORD_X")		->asBool();
	bool	bCoord_Y		= Parameters("COORD_Y")		->asBool();

	Names	+= pShapes->Get_Field_Name(iAttribute);		// Dependent Variable

	for(iGrid=0; iGrid<pGrids->Get_Count(); iGrid++)	// Independent Variables
	{
		Names	+= pGrids->asGrid(iGrid)->Get_Name();
	}

	if( bCoord_X )	{	Names	+= SG_T("X");	}
	if( bCoord_Y )	{	Names	+= SG_T("Y");	}

	Sample.Create(1 + pGrids->Get_Count() + (bCoord_X ? 1 : 0) + (bCoord_Y ? 1 : 0));

	//-----------------------------------------------------
	for(int iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
	{
		CSG_Shape	*pShape	= pShapes->Get_Shape(iShape);

		if( !pShape->is_NoData(iAttribute) )
		{
			Sample[0]	= pShape->asDouble(iAttribute);

			for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					bool		bAdd	= true;
					TSG_Point	Point	= pShape->Get_Point(iPoint, iPart);

					for(iGrid=0; iGrid<pGrids->Get_Count() && bAdd; iGrid++)
					{
						if( pGrids->asGrid(iGrid)->Get_Value(Point, zGrid, Interpolation) )
						{
							Sample[1 + iGrid]	= zGrid;
						}
						else
						{
							bAdd	= false;
						}
					}

					if( bAdd )
					{
						if( bCoord_X )	{	Sample[1 + iGrid++]	= Point.x;	}
						if( bCoord_Y )	{	Sample[1 + iGrid++]	= Point.y;	}

						Samples.Add_Row(Sample);
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	return( Samples.Get_NRows() >= pGrids->Get_Count() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPoint_Multi_Grid_Regression::Set_Regression(CSG_Parameter_Grid_List *pGrids, CSG_Grid *pRegression, const CSG_String &Name)
{
	//-----------------------------------------------------
	int		iGrid, nGrids;

	CSG_Grid	**ppGrids	= (CSG_Grid **)SG_Malloc(m_Regression.Get_nPredictors() * sizeof(CSG_Grid *));

	bool	bCoord_X		= false;
	bool	bCoord_Y		= false;

	for(iGrid=0, nGrids=0; iGrid<m_Regression.Get_nPredictors(); iGrid++)
	{
		if( m_Regression.Get_Predictor(iGrid) < pGrids->Get_Count() )
		{
			ppGrids[nGrids++]	= pGrids->asGrid(m_Regression.Get_Predictor(iGrid));
		}
		else if( m_Regression.Get_Predictor(iGrid) == pGrids->Get_Count() && Parameters("COORD_X")->asBool() )
		{
			bCoord_X = true;
		}
		else // if( m_Regression.Get_Predictor(iGrid) > pGrids->Get_Count() || Parameters("COORD_X")->asBool() == false )
		{
			bCoord_Y = true;
		}
	}

	pRegression->Set_Name(Name);

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			bool	bOkay;
			double	z	= m_Regression.Get_RConst();

			for(iGrid=0, bOkay=true; bOkay && iGrid<nGrids; iGrid++)
			{
				if( ppGrids[iGrid]->is_NoData(x, y) )
				{
					bOkay	= false;
				}
				else
				{
					z	+= m_Regression.Get_RCoeff(iGrid) * ppGrids[iGrid]->asDouble(x, y);
				}
			}

			//---------------------------------------------
			if( bOkay )
			{
				if( bCoord_X )
				{
					z	+= m_Regression.Get_RCoeff(iGrid++) * Get_System()->Get_xGrid_to_World(x);
				}

				if( bCoord_Y )
				{
					z	+= m_Regression.Get_RCoeff(iGrid++) * Get_System()->Get_yGrid_to_World(y);
				}

				pRegression->Set_Value (x, y, z);
			}
			else
			{
				pRegression->Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	SG_Free(ppGrids);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPoint_Multi_Grid_Regression::Set_Residuals(CSG_Shapes *pShapes, int iAttribute, CSG_Grid *pRegression)
{
	CSG_Shapes	*pResiduals		= Parameters("RESIDUALS")->asShapes();

	if( !pResiduals )
	{
		return( false );
	}

	//-----------------------------------------------------
	pResiduals->Create(SHAPE_TYPE_Point, CSG_String::Format(SG_T("%s [%s]"), pShapes->Get_Name(), _TL("Residuals")));
	pResiduals->Add_Field(pShapes->Get_Field_Name(iAttribute), SG_DATATYPE_Double);
	pResiduals->Add_Field("TREND"	, SG_DATATYPE_Double);
	pResiduals->Add_Field("RESIDUAL", SG_DATATYPE_Double);

	int	Interpolation	= Parameters("INTERPOL")->asInt();

	//-------------------------------------------------
	for(int iShape=0; iShape<pShapes->Get_Count() && Set_Progress(iShape, pShapes->Get_Count()); iShape++)
	{
		CSG_Shape	*pShape	= pShapes->Get_Shape(iShape);

		if( !pShape->is_NoData(iAttribute) )
		{
			double	zShape	= pShape->asDouble(iAttribute);

			for(int iPart=0; iPart<pShape->Get_Part_Count(); iPart++)
			{
				for(int iPoint=0; iPoint<pShape->Get_Point_Count(iPart); iPoint++)
				{
					double		zGrid;
					TSG_Point	Point	= pShape->Get_Point(iPoint, iPart);

					if( pRegression->Get_Value(Point, zGrid, Interpolation) )
					{
						CSG_Shape	*pResidual	= pResiduals->Add_Shape();

						pResidual->Add_Point(Point);
						pResidual->Set_Value(0, zShape);
						pResidual->Set_Value(1, zGrid);
						pResidual->Set_Value(2, zShape - zGrid);
					}
				}
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

//	//-----------------------------------------------------
//	Parameters.Add_Choice(
//		NULL	,"CORRECTION"	, _TL("Adjustment"),
//		_TL(""),
//		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|"),
//			_TL("Smith"),
//			_TL("Wherry 1"),
//			_TL("Wherry 2"),
//			_TL("Olkin & Pratt"),
//			_TL("Pratt"),
//			_TL("Claudy 3")
//		), 1
//	);
//
//	TSG_Regression_Correction	m_Correction;
//
//	switch( Parameters("CORRECTION")->asInt() )
//	{
//	case 0:	m_Correction	= REGRESSION_CORR_Smith;		break;
//	case 1:	m_Correction	= REGRESSION_CORR_Wherry_1;		break;
//	case 2:	m_Correction	= REGRESSION_CORR_Wherry_2;		break;
//	case 3:	m_Correction	= REGRESSION_CORR_Olkin_Pratt;	break;
//	case 4:	m_Correction	= REGRESSION_CORR_Pratt;		break;
//	case 5:	m_Correction	= REGRESSION_CORR_Claudy_3;		break;
//	}
//


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

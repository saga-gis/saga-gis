/**********************************************************
 * Version $Id: grid_multi_grid_regression.cpp 1160 2011-09-14 15:11:54Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                 statistics_regression                 //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//              grid_multi_grid_regression.cpp           //
//                                                       //
//                 Copyright (C) 2011 by                 //
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
#include "grid_multi_grid_regression.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Multi_Grid_Regression::CGrid_Multi_Grid_Regression(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Multiple Regression Analysis (Grid/Grids)"));

	Set_Author		(SG_T("O.Conrad (c) 2011"));

	Set_Description	(_TW(
		"Linear regression analysis of one grid as dependent and multiple grids as indepentent (predictor) variables. "
		"Details of the regression/correlation analysis will be saved to a table. "
		"Optionally the regression model is used to create a new grid with regression based values. "
		"The multiple regression analysis uses a forward selection procedure. \n"
		"\n"
		"Reference:\n"
		"- Bahrenberg, G., Giese, E., Nipper, J. (1992): "
		"'Statistische Methoden in der Geographie 2 - Multivariate Statistik', "
		"Stuttgart, 415p.\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "DEPENDENT"	, _TL("Dependent"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid_List(
		NULL	, "GRIDS"		, _TL("Grids"),
		_TL(""),
		PARAMETER_INPUT, false
	);

	Parameters.Add_Grid(
		NULL	, "REGRESSION"	, _TL("Regression"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL	, "RESIDUALS"	, _TL("Residuals"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
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
		), 3
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
bool CGrid_Multi_Grid_Regression::On_Execute(void)
{
	bool					bResult;
	double					P_in, P_out;
	CSG_Strings				Names;
	CSG_Matrix				Samples;
	CSG_Grid				*pDependent, *pRegression, *pResiduals;
	CSG_Parameter_Grid_List	*pGrids;

	//-----------------------------------------------------
	pDependent		= Parameters("DEPENDENT")	->asGrid();
	pGrids			= Parameters("GRIDS")		->asGridList();
	pRegression		= Parameters("REGRESSION")	->asGrid();
	pResiduals		= Parameters("RESIDUALS")	->asGrid();
	P_in			= Parameters("P_IN")		->asDouble() / 100.0;
	P_out			= Parameters("P_OUT")		->asDouble() / 100.0;

	//-----------------------------------------------------
	if( !Get_Samples(pGrids, pDependent, Samples, Names) )
	{
		return( false );
	}

	//-----------------------------------------------------
	switch( Parameters("METHOD")->asInt() )
	{
	default:
	case 0:	bResult	= m_Regression.Get_Model         (Samples             , &Names);	break;
	case 1:	bResult	= m_Regression.Get_Model_Forward (Samples, P_in       , &Names);	break;
	case 2:	bResult	= m_Regression.Get_Model_Backward(Samples,       P_out, &Names);	break;
	case 3:	bResult	= m_Regression.Get_Model_Stepwise(Samples, P_in, P_out, &Names);	break;
	}

	if( bResult == false )
	{
		return( false );
	}

	Message_Add(m_Regression.Get_Info(), false);

	//-----------------------------------------------------
	Set_Regression(pGrids, pDependent, pRegression, pResiduals, CSG_String::Format(SG_T("%s [%s]"), pDependent->Get_Name(), _TL("Regression Model")));

	//-----------------------------------------------------
	if( Parameters("INFO_COEFF")->asTable() )
	{
		Parameters("INFO_COEFF")->asTable()->Assign(m_Regression.Get_Info_Regression());
		Parameters("INFO_COEFF")->asTable()->Set_Name(_TL("MLRA Coefficients"));
	}

	if( Parameters("INFO_MODEL")->asTable() )
	{
		Parameters("INFO_MODEL")->asTable()->Assign(m_Regression.Get_Info_Model());
		Parameters("INFO_MODEL")->asTable()->Set_Name(_TL("MLRA Model"));
	}

	if( Parameters("INFO_STEPS")->asTable() )
	{
		Parameters("INFO_STEPS")->asTable()->Assign(m_Regression.Get_Info_Steps());
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
bool CGrid_Multi_Grid_Regression::Get_Samples(CSG_Parameter_Grid_List *pGrids, CSG_Grid *pDependent, CSG_Matrix &Samples, CSG_Strings &Names)
{
	int			iGrid, x, y;
	double		zGrid;
	TSG_Point	p;
	CSG_Vector	Sample;

	//-----------------------------------------------------
	int		Interpolation	= Parameters("INTERPOL")	->asInt();
	bool	bCoord_X		= Parameters("COORD_X")		->asBool();
	bool	bCoord_Y		= Parameters("COORD_Y")		->asBool();

	Names	+= pDependent->Get_Name();					// Dependent Variable

	for(iGrid=0; iGrid<pGrids->Get_Count(); iGrid++)	// Independent Variables
	{
		Names	+= pGrids->asGrid(iGrid)->Get_Name();
	}

	if( bCoord_X )	{	Names	+= SG_T("X");	}
	if( bCoord_Y )	{	Names	+= SG_T("Y");	}

	Sample.Create(1 + pGrids->Get_Count() + (bCoord_X ? 1 : 0) + (bCoord_Y ? 1 : 0));

	//-----------------------------------------------------
	for(y=0, p.y=Get_YMin(); y<Get_NY() && Set_Progress(y); y++, p.y+=Get_Cellsize())
	{
		for(x=0, p.x=Get_XMin(); x<Get_NX(); x++, p.x+=Get_Cellsize())
		{
			if( !pDependent->is_NoData(x, y) )
			{
				bool		bAdd	= true;

				for(iGrid=0; iGrid<pGrids->Get_Count() && bAdd; iGrid++)
				{
					if( pGrids->asGrid(iGrid)->Get_Value(p, zGrid, Interpolation) )
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
					Sample[0]	= pDependent->asDouble(x, y);

					if( bCoord_X )	{	Sample[1 + iGrid++]	= p.x;	}
					if( bCoord_Y )	{	Sample[1 + iGrid++]	= p.y;	}

					Samples.Add_Row(Sample);
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
bool CGrid_Multi_Grid_Regression::Set_Regression(CSG_Parameter_Grid_List *pGrids, CSG_Grid *pDependent, CSG_Grid *pRegression, CSG_Grid *pResiduals, const CSG_String &Name)
{
	//-----------------------------------------------------
	int			iGrid, nGrids, x, y;
	TSG_Point	p;

	int		Interpolation	= Parameters("INTERPOL")->asInt();

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

	if( pDependent && pResiduals )
	{
		pResiduals->Set_Name(CSG_String::Format(SG_T("%s [%s]"), Name.c_str(), _TL("Residuals")));
	}
	else
	{
		pResiduals	= NULL;
	}

	//-----------------------------------------------------
	for(y=0, p.y=Get_YMin(); y<Get_NY() && Set_Progress(y); y++, p.y+=Get_Cellsize())
	{
		for(x=0, p.x=Get_XMin(); x<Get_NX(); x++, p.x+=Get_Cellsize())
		{
			bool	bOkay;
			double	z	= m_Regression.Get_RConst();

			for(iGrid=0, bOkay=true; bOkay && iGrid<nGrids; iGrid++)
			{
				double	zGrid;

				if( ppGrids[iGrid]->Get_Value(p, zGrid, Interpolation) )
				{
					z	+= m_Regression.Get_RCoeff(iGrid) * zGrid;
				}
				else
				{
					bOkay	= false;
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

				if( pResiduals )
				{
					pResiduals->Set_Value(x, y, pDependent->asDouble(x, y) - z);
				}
			}
			else
			{
				pRegression->Set_NoData(x, y);

				if( pResiduals )
				{
					pResiduals->Set_NoData(x, y);
				}
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

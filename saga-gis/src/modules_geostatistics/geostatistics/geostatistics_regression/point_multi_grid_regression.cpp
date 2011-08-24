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
#define GRID_ZFACTOR	true


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
		NULL	, "TABLE"		, _TL("Details"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Shapes(
		NULL	, "RESIDUAL"	, _TL("Residuals"),
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
		NULL	,"CORRECTION"	, _TL("Adjustment"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|"),
			_TL("Smith"),
			_TL("Wherry 1"),
			_TL("Wherry 2"),
			_TL("Olkin & Pratt"),
			_TL("Pratt"),
			_TL("Claudy 3")
		), 1
	);

	pNode	= Parameters.Add_Value(
		NULL	, "SIGNIF_THRS"	, _TL("Minimum Level of Significance"),
		_TL("only take respect of parameters above specified significance level, given as percentage"),
		PARAMETER_TYPE_Double, 5.0, 0.0, true, 100.0, true
	);

	Parameters.Add_Value(
		pNode	, "SIGNIF_ADJ"	, _TL("Use Adjusted R2"),
		_TL(""),
		PARAMETER_TYPE_Bool, false
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
	int						iAttribute;
	CSG_Table				*pTable;
	CSG_Shapes				*pShapes, *pResiduals;
	CSG_Grid				*pRegression;
	CSG_Parameter_Grid_List	*pGrids;

	//-----------------------------------------------------
	pGrids			= Parameters("GRIDS")		->asGridList();
	pRegression		= Parameters("REGRESSION")	->asGrid();
	pTable			= Parameters("TABLE")		->asTable();
	pShapes			= Parameters("SHAPES")		->asShapes();
	pResiduals		= Parameters("RESIDUAL")	->asShapes();
	iAttribute		= Parameters("ATTRIBUTE")	->asInt();
	m_Interpolation	= Parameters("INTERPOL")	->asInt();
	m_bCoord_X		= Parameters("COORD_X")		->asBool();
	m_bCoord_Y		= Parameters("COORD_Y")		->asBool();
	m_Significance	= Parameters("SIGNIF_THRS")	->asDouble() / 100.0;
	m_bSignif_Adj	= Parameters("SIGNIF_ADJ")	->asBool();

	switch( Parameters("CORRECTION")->asInt() )
	{
	case 0:	m_Correction	= REGRESSION_CORR_Smith;		break;
	case 1:	m_Correction	= REGRESSION_CORR_Wherry_1;		break;
	case 2:	m_Correction	= REGRESSION_CORR_Wherry_2;		break;
	case 3:	m_Correction	= REGRESSION_CORR_Olkin_Pratt;	break;
	case 4:	m_Correction	= REGRESSION_CORR_Pratt;		break;
	case 5:	m_Correction	= REGRESSION_CORR_Claudy_3;		break;
	}

	//-----------------------------------------------------
	int			iGrid, nGrids	= pGrids->Get_Count();
	CSG_Array	Grids(sizeof(CSG_Grid *), nGrids);
	CSG_Grid	**ppGrids	= (CSG_Grid **)Grids.Get_Array();

	for(iGrid=0; iGrid<pGrids->Get_Count(); iGrid++)
	{
		ppGrids[iGrid]	= pGrids->asGrid(iGrid);
	}

	if( !Get_Regression(ppGrids, nGrids, pShapes, iAttribute) )
	{
		m_Regression.Destroy();

		return( false );
	}

	Msg_Correlation();

	//-----------------------------------------------------
	if( m_Significance > 0.0 )
	{
		for(iGrid=0, nGrids=0; iGrid<pGrids->Get_Count(); iGrid++)
		{
			if( m_Regression.Get_Signif_Partial(iGrid, false, m_bSignif_Adj ? m_Correction : REGRESSION_CORR_None) >= m_Significance )
			{
				ppGrids[nGrids++]	= pGrids->asGrid(iGrid);
			}
		}

		if( m_bCoord_X )
		{
			if( m_Regression.Get_Signif_Partial(iGrid++, false, m_bSignif_Adj ? m_Correction : REGRESSION_CORR_None) < m_Significance )
			{
				m_bCoord_X	= false;
			}
		}

		if( m_bCoord_Y )
		{
			if( m_Regression.Get_Signif_Partial(iGrid++, false, m_bSignif_Adj ? m_Correction : REGRESSION_CORR_None) < m_Significance )
			{
				m_bCoord_Y	= false;
			}
		}

		if( !Get_Regression(ppGrids, nGrids, pShapes, iAttribute) )
		{
			m_Regression.Destroy();

			return( false );
		}
	}

	Msg_Regression();

	//-----------------------------------------------------
	Set_Regression(ppGrids, nGrids, pRegression, CSG_String::Format(SG_T("%s (%s)"), pShapes->Get_Name(), Get_Name()));

	Set_Residuals(pShapes, iAttribute, pResiduals, pRegression);

	if( pTable )
	{
		pTable->Assign(m_Regression.Get_Result());

		pTable->Set_Name(_TL("Multiple Regression Analysis"));
	}

	m_Regression.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPoint_Multi_Grid_Regression::Get_Regression(CSG_Grid **ppGrids, int nGrids, CSG_Shapes *pShapes, int iAttribute)
{
	int			iGrid;
	double		zGrid;
	CSG_Table	Table;

	//-----------------------------------------------------
	Table.Add_Field(pShapes->Get_Name(), SG_DATATYPE_Double);

	for(iGrid=0; iGrid<nGrids; iGrid++)
	{
		Table.Add_Field(ppGrids[iGrid]->Get_Name(), SG_DATATYPE_Double);
	}

	if( m_bCoord_X )
	{
		Table.Add_Field(SG_T("X"), SG_DATATYPE_Double);
	}

	if( m_bCoord_Y )
	{
		Table.Add_Field(SG_T("Y"), SG_DATATYPE_Double);
	}

	//-----------------------------------------------------
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
					TSG_Point			Point		= pShape->Get_Point(iPoint, iPart);
					CSG_Table_Record	*pRecord	= Table.Add_Record();

					pRecord->Set_Value(0, zShape);

					for(iGrid=0; iGrid<nGrids && pRecord; iGrid++)
					{
						if( ppGrids[iGrid]->Get_Value(Point, zGrid, m_Interpolation, GRID_ZFACTOR) )
						{
							pRecord->Set_Value(1 + iGrid, zGrid);
						}
						else
						{
							Table.Del_Record(Table.Get_Record_Count() - 1);

							pRecord	= NULL;
						}
					}

					if( pRecord )
					{
						int		n	= nGrids + 1;

						if( m_bCoord_X )
						{
							pRecord->Set_Value(n++, Point.x);
						}

						if( m_bCoord_Y )
						{
							pRecord->Set_Value(n++, Point.y);
						}
					}
				}
			}
		}
	}

	//-----------------------------------------------------
	return( m_Regression.Calculate(Table) );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPoint_Multi_Grid_Regression::Set_Regression(CSG_Grid **ppGrids, int nGrids, CSG_Grid *pRegression, const CSG_String &Name)
{
	pRegression->Set_Name(Name);

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			bool	bOkay;
			int		iGrid;
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

			if( bOkay )
			{
				int		n	= nGrids;

				if( m_bCoord_X )
				{
					z	+= m_Regression.Get_RCoeff(n++) * Get_System()->Get_xGrid_to_World(x);
				}

				if( m_bCoord_Y )
				{
					z	+= m_Regression.Get_RCoeff(n++) * Get_System()->Get_yGrid_to_World(y);
				}

				pRegression->Set_Value (x, y, z);
			}
			else
			{
				pRegression->Set_NoData(x, y);
			}
		}
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CPoint_Multi_Grid_Regression::Set_Residuals(CSG_Shapes *pShapes, int iAttribute, CSG_Shapes *pResiduals, CSG_Grid *pRegression)
{
	if( !pResiduals )
	{
		return( false );
	}

	//-----------------------------------------------------
	pResiduals->Create(SHAPE_TYPE_Point, CSG_String::Format(SG_T("%s [%s]"), pShapes->Get_Name(), _TL("Residuals")));
	pResiduals->Add_Field(pShapes->Get_Field_Name(iAttribute), SG_DATATYPE_Double);
	pResiduals->Add_Field("TREND"	, SG_DATATYPE_Double);
	pResiduals->Add_Field("RESIDUAL", SG_DATATYPE_Double);

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

					if( pRegression->Get_Value(Point, zGrid, m_Interpolation, GRID_ZFACTOR) )
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
void CPoint_Multi_Grid_Regression::Msg_Correlation(void)
{
	CSG_String	s;

	s	+= CSG_String::Format(SG_T("\n%s:\n\n"), _TL("Correlation"));
	s	+= CSG_String::Format(SG_T("No.   \tR2 total\t    \tR2 partial\t  \tSignificance  \tParameter\n"));
	s	+= CSG_String::Format(SG_T("      \tunadj.\tadj.  \tunadj.\tadj.  \tunadj.\tadj.  \n"));
	s	+= CSG_String::Format(SG_T("------\t------\t------\t------\t------\t------\t------\t------\n"));

	for(int i=0, bSignificant=true; i<m_Regression.Get_Count(); i++)
	{
		if( !bSignificant || m_Significance > m_Regression.Get_Signif_Partial(i, true, m_bSignif_Adj ? m_Correction : REGRESSION_CORR_None) )
		{
			if( bSignificant )
			{
				bSignificant	= false;

				s	+= SG_T("------\t------\t------\t------\t------\t------\t------\t------\n");
			}

			s	+= SG_T("*");
		}

		s	+= CSG_String::Format(SG_T("%d.\t%04.2f\t%04.2f\t%4.2f\t%4.2f\t%4.2f\t%4.2f\t%s\n"),
			i + 1,
			100.0 * m_Regression.Get_R2				(i, true),
			100.0 * m_Regression.Get_R2				(i, true, m_Correction),
			100.0 * m_Regression.Get_R2_Partial		(i, true),
			100.0 * m_Regression.Get_R2_Partial		(i, true, m_Correction),
			100.0 * m_Regression.Get_Signif_Partial	(i, true),
			100.0 * m_Regression.Get_Signif_Partial	(i, true, m_Correction),
			        m_Regression.Get_Name			(i, true)
		);
	}

	s	+= SG_T("\n");

	Message_Add(s, false);
}

//---------------------------------------------------------
void CPoint_Multi_Grid_Regression::Msg_Regression(void)
{
	CSG_String	s;

	s	+= CSG_String::Format(SG_T("\n%s:\n\n  Y = \t+%f"), _TL("Regression"), m_Regression.Get_RConst());

	for(int i=0; i<m_Regression.Get_Count(); i++)
	{
		s	+= CSG_String::Format(SG_T("\n\t%+f\t* [%s]"), m_Regression.Get_RCoeff(i, true), m_Regression.Get_Name(i, true));
	}

	s	+= SG_T("\n");

	Message_Add(s, false);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

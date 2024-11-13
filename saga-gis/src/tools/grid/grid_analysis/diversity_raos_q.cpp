
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      Grid_Filter                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 diversity_raos_q.cpp                  //
//                                                       //
//                 Copyright (C) 2019 by                 //
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
#include "diversity_raos_q.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CDiversity_Raos_Q_Classic::CDiversity_Raos_Q_Classic(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Rao's Q Diversity Index (Classic)"));

	Set_Author		("O.Conrad (c) 2019");

	Set_Description	(_TW(
		"Grid based analysis of diversity with Rao's Q Index. "
		"Rao's Q diversity index is calculated locally for each grid cell using "
		"the specified kernel (aka 'moving window'). It is assumed "
		"that the grid cell values represent quantities. "
	));

	Add_Reference("Rocchini, D., Marcantonio, M., Ricotta, C.", "2017",
		"Measuring Rao's Q diversity index from remote sensing: An open source solution",
		"Ecological Indicators, Volume 72, p. 234-238.",
		SG_T("https://doi.org/10.1016/j.ecolind.2016.07.039"), SG_T("doi:10.1016/j.ecolind.2016.07.039.")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("",
		"VALUES"	, _TL("Values"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"COUNT"		, _TL("Number of Categories"),
		_TL("number of different categories (unique values) within search area"),
		PARAMETER_OUTPUT_OPTIONAL, true, SG_DATATYPE_Short
	);

	Parameters.Add_Grid("",
		"INDEX"		, _TL("Rao's Q"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	CSG_Grid_Cell_Addressor::Add_Parameters(Parameters);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CDiversity_Raos_Q_Classic::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CDiversity_Raos_Q_Classic::On_Execute(void)
{
	//-----------------------------------------------------
	m_pValues	= Parameters("VALUES")->asGrid();

	CSG_Grid	*pCount	= Parameters("COUNT")->asGrid();
	CSG_Grid	*pIndex	= Parameters("INDEX")->asGrid();

	if( pCount ) pCount->Fmt_Name("%s [%s]", m_pValues->Get_Name(), _TL("Count"  ));
	if( pIndex ) pIndex->Fmt_Name("%s [%s]", m_pValues->Get_Name(), _TL("Rao's Q"));

	//-----------------------------------------------------
	if( !m_Kernel.Set_Parameters(Parameters) )
	{
		Error_Set(_TL("could not initialize kernel"));

		return( false );
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			int	Count;	double	Index;

			if( Get_Index(x, y, Count, Index) )
			{
				if( pCount ) pCount->Set_Value(x, y, Count);
				if( pIndex ) pIndex->Set_Value(x, y, Index);
			}
			else
			{
				if( pCount ) pCount->Set_NoData(x, y);
				if( pIndex ) pIndex->Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	m_Kernel.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CDiversity_Raos_Q_Classic::Get_Index(int x, int y, int &Count, double &Index)
{
	if( m_pValues->is_NoData(x, y) )
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Unique_Number_Statistics	Values;

	int	nTotal	= 0;

	for(int iCell=0; iCell<m_Kernel.Get_Count(); iCell++)
	{
		int	ix	= m_Kernel.Get_X(iCell, x);
		int	iy	= m_Kernel.Get_Y(iCell, y);

		if( m_pValues->is_InGrid(ix, iy) )
		{
			Values	+= m_pValues->asDouble(ix, iy);

			nTotal	++;
		}
	}

	//-----------------------------------------------------
	Index	= 0.;
	Count	= Values.Get_Count();

	if( Count < 2 )
	{
		return( true );
	}

	//-----------------------------------------------------
	for(int i=0; i<Count-1; i++)
	{
		double	pi	= Values.Get_Count(i) / (double)nTotal;	// relative proportion of class members
		double	vi	= Values.Get_Value(i);

		for(int j=i+1; j<Count; j++)
		{
			double	pj	= Values.Get_Count(j) / (double)nTotal;	// relative proportion of class members
			double	vj	= Values.Get_Value(j);

			double	d	= fabs(vi - vj);

			Index	+= 2. * d * pi * pj;
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
CDiversity_Raos_Q::CDiversity_Raos_Q(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Rao's Q Diversity Index"));

	Set_Author		("O.Conrad (c) 2019");

	Set_Description	(_TW(
		"Grid based analysis of diversity with Rao's Q Index. "
		"Rao's Q diversity index is calculated locally for each grid cell using "
		"the specified kernel (aka 'moving window'). It is assumed "
		"that the grid cell values represent quantities. "
	));

	Add_Reference("Rocchini, D., Marcantonio, M., Ricotta, C.", "2017",
		"Measuring Rao's Q diversity index from remote sensing: An open source solution",
		"Ecological Indicators, Volume 72, p. 234-238.",
		SG_T("https://doi.org/10.1016/j.ecolind.2016.07.039"), SG_T("doi:10.1016/j.ecolind.2016.07.039.")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid_List("",
		"VALUES"	, _TL("Values"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Bool("VALUES",
		"NORMALIZE"	, _TL("Normalize"),
		_TL(""),
		false
	);

	Parameters.Add_Grid("",
		"INDEX"		, _TL("Rao's Q"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Choice("",
		"DISTANCE"	, _TL("Distance"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("Euclidean"),
			_TL("Manhatten"),
			_TL("Canberra"),
			_TL("Minkowski")
		), 0
	);

	Parameters.Add_Double("DISTANCE",
		"LAMBDA"	, _TL("Lambda"),
		_TL("Lambda for Minkowski distance calculation."),
		1., 0.0001, true
	);

	CSG_Grid_Cell_Addressor::Add_Parameters(Parameters);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CDiversity_Raos_Q::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("DISTANCE") )
	{
		pParameters->Set_Enabled("LAMBDA", pParameter->asInt() == 3);	// Minkowski
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CDiversity_Raos_Q::On_Execute(void)
{
	//-----------------------------------------------------
	m_pValues	= Parameters("VALUES")->asGridList();

	if( m_pValues->Get_Grid_Count() < 1 )
	{
		return( false );
	}

	CSG_Grid	*pIndex	= Parameters("INDEX")->asGrid();

	pIndex->Fmt_Name("%s", _TL("Rao's Q"));

	//-----------------------------------------------------
	if( !m_Kernel.Set_Parameters(Parameters) )
	{
		Error_Set(_TL("could not initialize kernel"));

		return( false );
	}

	m_bNormalize	= Parameters("NORMALIZE")->asBool();

	m_Distance		= Parameters("DISTANCE")->asInt();

	m_Lambda		= Parameters("LAMBDA")->asDouble();

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress_Rows(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			int	Count;	double	Index;

			if( Get_Index(x, y, Count, Index) )
			{
				pIndex->Set_Value(x, y, Index);
			}
			else
			{
				pIndex->Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	m_Kernel.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline bool CDiversity_Raos_Q::Get_Values(int x, int y, CSG_Vector &Values)
{
	if( !is_InGrid(x, y) || !Values.Create(m_pValues->Get_Grid_Count()) )
	{
		return( false );
	}

	for(int i=0; i<m_pValues->Get_Grid_Count(); i++)
	{
		CSG_Grid	*pValues	= m_pValues->Get_Grid(i);

		if( pValues->is_NoData(x, y) )
		{
			return( false );
		}

		if( m_bNormalize )
		{
			Values[i]	= (pValues->asDouble(x, y) - pValues->Get_Mean()) / pValues->Get_StdDev();
		}
		else
		{
			Values[i]	=  pValues->asDouble(x, y);
		}
	}

	return( true );
}

//---------------------------------------------------------
inline double CDiversity_Raos_Q::Get_Distance(double *A, double *B)
{
	double	Distance	= 0.;

	for(int i=0; i<m_pValues->Get_Grid_Count(); i++)
	{
		double	a	= A[i];
		double	b	= B[i];

		switch( m_Distance )
		{
		case  0:	// Euclidean
			Distance	+= SG_Get_Square(a - b);
			break;

		case  1:	// Manhatten
			Distance	+= fabs(a - b);
			break;

		case  2:	// Canberra
			Distance	+= !a && !b ? 0. : fabs(a - b) / (fabs(a) + fabs(b));
			break;

		case  3:	// Minkowski
			Distance	+= fabs(pow(a - b, m_Lambda));
			break;
		}
	}

	switch( m_Distance )
	{
	case  0:	// Euclidean
		Distance	= sqrt(Distance);
		break;

	case  3:	// Minkowski
		Distance	= pow(Distance, 1. / m_Lambda);
		break;
	}

	return( Distance );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CDiversity_Raos_Q::Get_Index(int x, int y, int &Count, double &Index)
{
	CSG_Vector	v;

	if( !Get_Values(x, y, v) )	// no-data at x, y ?!
	{
		return( false );
	}

	//-----------------------------------------------------
	CSG_Matrix	Values;

	for(int iCell=0; iCell<m_Kernel.Get_Count(); iCell++)
	{
		if( Get_Values(m_Kernel.Get_X(iCell, x), m_Kernel.Get_Y(iCell, y), v) )
		{
			Values.Add_Row(v);
		}
	}

	//-----------------------------------------------------
	Index	= 0.;

	if( Values.Get_NRows() < 2 )
	{
		return( true );
	}

	//-----------------------------------------------------
	double	d	= 2. / SG_Get_Square(Values.Get_NRows());

	for(int i=0; i<Values.Get_NRows()-1; i++)
	{
		for(int j=i+1; j<Values.Get_NRows(); j++)
		{
			Index	+= d * Get_Distance(Values[i], Values[j]);
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

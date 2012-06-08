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
//                       Tutorial                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    Grid_Merge.cpp                     //
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
#include "Grid_Merge.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Merge::CGrid_Merge(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Mosaicking"));

	Set_Author		(SG_T("O.Conrad (c) 2003-12"));

	Set_Description	(_TW(
		"Mosaicking several grids to a single new one. Formerly known as 'Merge Grids'."
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List(
		NULL	, "GRIDS"		, _TL("Input Grids"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Choice(
		NULL	, "TYPE"		, _TL("Preferred data storage type"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|%s|%s|%s|"),
			_TL("1 bit"),
			_TL("1 byte unsigned integer"),
			_TL("1 byte signed integer"),
			_TL("2 byte unsigned integer"),
			_TL("2 byte signed integer"),
			_TL("4 byte unsigned integer"),
			_TL("4 byte signed integer"),
			_TL("4 byte floating point"),
			_TL("8 byte floating point")
		), 7
	);

	Parameters.Add_Choice(
		NULL	, "INTERPOL"	, _TL("Interpolation"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|"),
			_TL("Nearest Neighbor"),
			_TL("Bilinear Interpolation"),
			_TL("Inverse Distance Interpolation"),
			_TL("Bicubic Spline Interpolation"),
			_TL("B-Spline Interpolation")
		), 0
	);

	Parameters.Add_Choice(
		NULL	, "OVERLAP"		, _TL("Overlapping Areas"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|%s|"),
			_TL("first"),
			_TL("last"),
			_TL("minimum"),
			_TL("maximum"),
			_TL("mean"),
			_TL("feathering"),
			_TL("blend boundary")
		), 1
	);

	Parameters.Add_Value(
		NULL	, "BLEND_DIST"	, _TL("Blending Distance"),
		_TL("blending distance given as number of cells"),
		PARAMETER_TYPE_Int, 10, 1, true
	);

	//-----------------------------------------------------
	Parameters.Add_Choice(
		NULL	, "TARGET"		, _TL("Target Grid"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("user defined"),
			_TL("existing grid or grid system")
		), 0
	);

	m_Grid_Target.Add_Parameters_User(Add_Parameters("USER", _TL("User Defined Grid")	, _TL("")));
	m_Grid_Target.Add_Parameters_Grid(Add_Parameters("GRID", _TL("Choose Grid")			, _TL("")));
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Merge::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if(	!SG_STR_CMP(pParameter->Get_Identifier(), SG_T("OVERLAP")) )
	{
		pParameters->Get_Parameter("BLEND_DIST")->Set_Enabled(pParameter->asInt() == 6);
	}

	return( m_Grid_Target.On_User_Changed(pParameters, pParameter) ? 1 : 0 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Merge::On_Execute(void)
{
	//-----------------------------------------------------
	if( !Initialize() )
	{
		return( false );
	}

	//-----------------------------------------------------
	for(int i=0; i<m_pGrids->Get_Count(); i++)
	{
		CSG_Grid	*pGrid	= m_pGrids->asGrid(i);

		Set_Weight(pGrid);

		int	ax	= (int)((pGrid->Get_XMin() - m_pMosaic->Get_XMin()) / m_pMosaic->Get_Cellsize());
		int	ay	= (int)((pGrid->Get_YMin() - m_pMosaic->Get_YMin()) / m_pMosaic->Get_Cellsize());

		//-------------------------------------------------
		if(	is_Aligned(pGrid) )
		{
			Process_Set_Text(CSG_String::Format(SG_T("[%d/%d] %s: %s"), i + 1, m_pGrids->Get_Count(), _TL("copying"), pGrid->Get_Name()));

			int	nx	= pGrid->Get_NX(); if( nx >= ax + m_pMosaic->Get_NX() )	nx	= ax + m_pMosaic->Get_NX() - 1;
			int	ny	= pGrid->Get_NY(); if( ny >= ay + m_pMosaic->Get_NY() )	ny	= ay + m_pMosaic->Get_NY() - 1;

			for(int y=0; y<ny && Set_Progress(y, ny); y++)
			{
				if( ay + y >= 0 )
				{
					#pragma omp parallel for
					for(int x=0; x<nx; x++)
					{
						if( ax + x >= 0 && !pGrid->is_NoData(x, y) )
						{
							Set_Value(ax + x, ay + y, pGrid->asDouble(x, y), Get_Weight(x, y));
						}
					}
				}
			}
		}

		//-------------------------------------------------
		else
		{
			Process_Set_Text(CSG_String::Format(SG_T("[%d/%d] %s: %s"), i + 1, m_pGrids->Get_Count(), _TL("resampling"), pGrid->Get_Name()));

			if( ax < 0 )	ax	= 0;
			if( ay < 0 )	ay	= 0;

			int	nx	= 1 + m_pMosaic->Get_System().Get_xWorld_to_Grid(pGrid->Get_XMax()); if( nx > m_pMosaic->Get_NX() )	nx	= m_pMosaic->Get_NX();
			int	ny	= 1 + m_pMosaic->Get_System().Get_yWorld_to_Grid(pGrid->Get_YMax()); if( ny > m_pMosaic->Get_NY() )	ny	= m_pMosaic->Get_NY();

			for(int y=ay; y<ny && Set_Progress(y-ay, ny-ay); y++)
			{
				double	py	= m_pMosaic->Get_YMin() + y * m_pMosaic->Get_Cellsize();

				#pragma omp parallel for
				for(int x=ax; x<nx; x++)
				{
					double	px	= m_pMosaic->Get_XMin() + x * m_pMosaic->Get_Cellsize();

					Set_Value(x, y, pGrid, px, py);
				}
			}
		}

		m_Weight.Destroy();
	}

	//-----------------------------------------------------
	if( m_Weights.is_Valid() )
	{
		for(int y=0; y<m_pMosaic->Get_NY() && Set_Progress(y, m_pMosaic->Get_NY()); y++)
		{
			#pragma omp parallel for
			for(int x=0; x<m_pMosaic->Get_NX(); x++)
			{
				double	w	= m_Weights.asDouble(x, y);

				if( w > 0.0 )
				{
					m_pMosaic->Mul_Value(x, y, 1.0 / w);
				}
			}
		}
	}

	m_Weights.Destroy();

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Merge::Initialize(void)
{
	//-----------------------------------------------------
	m_pMosaic		= NULL;
	m_Overlap		= Parameters("OVERLAP")->asInt();
	m_pGrids		= Parameters("GRIDS"  )->asGridList();

	if( m_pGrids->Get_Count() < 2 )
	{
		Error_Set(_TL("nothing to do, there are less than two grids in input list."));

		return( false );
	}

	//-----------------------------------------------------
	switch( Parameters("INTERPOL")->asInt() )
	{
	default:
	case 0:	m_Interpolation	= GRID_INTERPOLATION_NearestNeighbour;	break;	// Nearest Neighbor
	case 1:	m_Interpolation	= GRID_INTERPOLATION_Bilinear;			break;	// Bilinear Interpolation
	case 2:	m_Interpolation	= GRID_INTERPOLATION_InverseDistance;	break;	// Inverse Distance Interpolation
	case 3:	m_Interpolation	= GRID_INTERPOLATION_BicubicSpline;		break;	// Bicubic Spline Interpolation
	case 4:	m_Interpolation	= GRID_INTERPOLATION_BSpline;			break;	// B-Spline Interpolation
	}

	//-----------------------------------------------------
	TSG_Data_Type	Type;

	switch( Parameters("TYPE")->asInt() )
	{
	default:	Type	= SG_DATATYPE_Float;	break;
	case 0:		Type	= SG_DATATYPE_Bit;		break;
	case 1:		Type	= SG_DATATYPE_Byte;		break;
	case 2:		Type	= SG_DATATYPE_Char;		break;
	case 3:		Type	= SG_DATATYPE_Word;		break;
	case 4:		Type	= SG_DATATYPE_Short;	break;
	case 5:		Type	= SG_DATATYPE_DWord;	break;
	case 6:		Type	= SG_DATATYPE_Int;		break;
	case 7: 	Type	= SG_DATATYPE_Float;	break;
	case 8:		Type	= SG_DATATYPE_Double;	break;
	}

	//-----------------------------------------------------
	switch( Parameters("TARGET")->asInt() )
	{
	case 0:	// user defined...
		{
			double		d	= m_pGrids->asGrid(0)->Get_Cellsize();
			CSG_Rect	r	= m_pGrids->asGrid(0)->Get_Extent();

			for(int i=1; i<m_pGrids->Get_Count(); i++)
			{
				if( d > m_pGrids->asGrid(i)->Get_Cellsize() )
				{
					d	= m_pGrids->asGrid(i)->Get_Cellsize();
				}

				r.Union(m_pGrids->asGrid(i)->Get_Extent());
			}

			int	nx	= 1 + (int)(r.Get_XRange() / d);
			int	ny	= 1 + (int)(r.Get_YRange() / d);

			if( m_Grid_Target.Init_User(r.Get_XMin(), r.Get_YMin(), d, nx, ny) && Dlg_Parameters("USER") )
			{
				m_pMosaic	= m_Grid_Target.Get_User(Type);
			}
		}
		break;

	case 1:	// grid...
		{
			if( Dlg_Parameters("GRID") )
			{
				m_pMosaic	= m_Grid_Target.Get_Grid(Type);
			}
		}
		break;
	}

	//-----------------------------------------------------
	if( m_pMosaic )
	{
		m_pMosaic->Set_Name(_TL("Mosaic"));

		m_pMosaic->Assign_NoData();

		switch( m_Overlap )
		{
		case 0:	// first
		case 1:	// last
		case 2:	// minimum
		case 3:	// maximum
		case 6:	// blend
			break;

		case 4:	// mean
			if( !m_Weights.Create(m_pMosaic->Get_System(), m_pGrids->Get_Count() < 256 ? SG_DATATYPE_Byte : SG_DATATYPE_Word) )
			{
				Error_Set(_TL("could not create weights grid"));

				return( false );
			}
			break;

		case 5:	// feathering
			if( !m_Weights.Create(m_pMosaic->Get_System(), SG_DATATYPE_Word) )
			{
				Error_Set(_TL("could not create weights grid"));

				return( false );
			}
			break;
		}

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Merge::is_Aligned(CSG_Grid *pGrid)
{
	return(	pGrid->Get_Cellsize() == m_pMosaic->Get_Cellsize()
		&&	fabs(fmod(pGrid->Get_XMin() - m_pMosaic->Get_XMin(), m_pMosaic->Get_Cellsize())) <= 0.001 * m_pMosaic->Get_Cellsize()
		&&	fabs(fmod(pGrid->Get_YMin() - m_pMosaic->Get_YMin(), m_pMosaic->Get_Cellsize())) <= 0.001 * m_pMosaic->Get_Cellsize()
	);
}

//---------------------------------------------------------
inline void CGrid_Merge::Set_Value(int x, int y, double Value, double Weight)
{
	switch( m_Overlap )
	{
	case 0:	// first
		if( m_pMosaic->is_NoData(x, y) )
		{
			m_pMosaic->Set_Value(x, y, Value);
		}
		break;

	case 1:	// last
		{
			m_pMosaic->Set_Value(x, y, Value);
		}
		break;

	case 2:	// minimum
		if( m_pMosaic->is_NoData(x, y) || m_pMosaic->asDouble(x, y) > Value )
		{
			m_pMosaic->Set_Value(x, y, Value);
		}
		break;

	case 3:	// maximum
		if( m_pMosaic->is_NoData(x, y) || m_pMosaic->asDouble(x, y) < Value )
		{
			m_pMosaic->Set_Value(x, y, Value);
		}
		break;

	case 4:	// mean
		if( m_pMosaic->is_NoData(x, y) )
		{
			m_pMosaic->Set_Value(x, y, Value);
			m_Weights .Set_Value(x, y, 1);
		}
		else
		{
			m_pMosaic->Add_Value(x, y, Value);
			m_Weights .Add_Value(x, y, 1);
		}
		break;

	case 5:	// feathering
		if( m_pMosaic->is_NoData(x, y) )
		{
			m_pMosaic->Set_Value(x, y, Weight * Value);
			m_Weights .Set_Value(x, y, Weight);
		}
		else
		{
			m_pMosaic->Add_Value(x, y, Weight * Value);
			m_Weights .Add_Value(x, y, Weight);
		}
		break;

	case 6:	// blend
		if( m_pMosaic->is_NoData(x, y) )
		{
			m_pMosaic->Set_Value(x, y, Value);
		}
		else
		{
			m_pMosaic->Set_Value(x, y, (1.0 - Weight) * m_pMosaic->asDouble(x, y) + Weight * Value);
		}
		break;
	}
}

//---------------------------------------------------------
inline void CGrid_Merge::Set_Value(int x, int y, CSG_Grid *pGrid, double px, double py)
{
	double	z;

	if( pGrid->Get_Value(px, py, z, m_Interpolation) )
	{
		if( m_Weight.is_Valid() )
		{
			double	w;

			if( m_Weight.Get_Value(px, py, w, GRID_INTERPOLATION_BSpline, true) )
			{
				Set_Value(x, y, z, w);
			}
		}
		else
		{
			Set_Value(x, y, z, 1.0);
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline double CGrid_Merge::Get_Weight(int x, int y)
{
	return( m_Weight.is_Valid() ? m_Weight.asDouble(x, y, true) : 1.0 );
}

//---------------------------------------------------------
bool CGrid_Merge::Set_Weight(CSG_Grid *pGrid)
{
	if( m_Overlap < 5 )	// only feathering/blending
	{
		return( true );
	}

	//-----------------------------------------------------
	int	dBlend	= m_Overlap == 6 ? Parameters("BLEND_DIST")->asInt() : 0;

	if( !m_Weight.Create(pGrid->Get_System(), dBlend > 0 && dBlend < 255 ? SG_DATATYPE_Byte : SG_DATATYPE_Word) )
	{
		Error_Set(_TL("could not create distance grid"));

		return( false );
	}

	//-----------------------------------------------------
	int		x, y, d;

	for(y=0; y<pGrid->Get_NY() && Process_Get_Okay(); y++)
	{
		for(x=0, d=1; x<pGrid->Get_NX(); x++)
		{
			if( pGrid->is_NoData(x, y) )
				m_Weight.Set_Value(x, y, d = 0);
			else //if( m_Weight.asDouble(x, y) > d )
				m_Weight.Set_Value(x, y, d);

			if( dBlend <= 0 || d < dBlend )	d++;
		}

		for(x=pGrid->Get_NX()-1, d=1; x>=0; x--)
		{
			if( pGrid->is_NoData(x, y) )
				m_Weight.Set_Value(x, y, d = 0);
			else if( m_Weight.asDouble(x, y) > d )
				m_Weight.Set_Value(x, y, d);

			if( dBlend <= 0 || d < dBlend )	d++;
		}
	}

	for(x=0; x<pGrid->Get_NX() && Process_Get_Okay(); x++)
	{
		for(y=0, d=1; y<pGrid->Get_NY(); y++)
		{
			if( pGrid->is_NoData(x, y) )
				m_Weight.Set_Value(x, y, d = 0);
			else if( m_Weight.asDouble(x, y) > d )
				m_Weight.Set_Value(x, y, d);

			if( dBlend <= 0 || d < dBlend )	d++;
		}

		for(y=pGrid->Get_NY()-1, d=1; y>=0; y--)
		{
			if( pGrid->is_NoData(x, y) )
				m_Weight.Set_Value(x, y, d = 0);
			else if( m_Weight.asDouble(x, y) > d )
				m_Weight.Set_Value(x, y, d);

			if( dBlend <= 0 || d < dBlend )	d++;
		}
	}

	//-----------------------------------------------------
	if( m_Overlap == 5 )
	{
		m_Weight.Set_ZFactor(m_Weight.Get_Cellsize() / m_Weights.Get_Cellsize());
	}
	else
	{
		m_Weight.Set_ZFactor(1.0 / (dBlend > 0 ? dBlend : m_Weight.Get_ZMax()));
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

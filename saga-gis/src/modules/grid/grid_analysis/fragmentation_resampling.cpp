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
//                     grid_analysis                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//              Fragmentation_Resampling.cpp             //
//                                                       //
//                 Copyright (C) 2008 by                 //
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
#include "fragmentation_resampling.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFragmentation_Resampling::CFragmentation_Resampling(void)
	: CFragmentation_Base()
{
	//-----------------------------------------------------
	Set_Name		(_TL("Fragmentation (Alternative)"));

	Set_Author		(SG_T("(c) 2008 by O.Conrad"));

	Set_Description	(_TW(
		"\n"
		"(1) interior, if Density = 1.0\n"
		"(2) undetermined, if Density > 0.6 and Density = Connectivity\n"
		"(3) perforated, if Density > 0.6 and Density - Connectivity > 0\n"
		"(4) edge, if Density > 0.6 and Density - Connectivity < 0\n"
		"(5) transitional, if 0.4 < Density < 0.6\n"
		"(6) patch, if Density < 0.4\n"
		"\n"
		"\n"
		"References:\n"
		"Riitters, K., Wickham, J., O'Neill, R., Jones, B., Smith, E. (2000): \n"
		"Global-scale patterns of forest fragmentation. Conservation Ecology 4(2): 3\n"
		"<a href=\"http://www.ecologyandsociety.org/vol4/iss2/art3/\">http://www.ecologyandsociety.org/vol4/iss2/art3/</a>\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Value(
		NULL	, "LEVEL_GROW"		, _TL("Search Distance Increment"),
		_TL(""),
		PARAMETER_TYPE_Double		, 1.0, 0.0, true
	);

	Parameters.Add_Value(
		NULL	, "DENSITY_MEAN"	, _TL("Density from Neighbourhood"),
		_TL(""),
		PARAMETER_TYPE_Bool			, true
	);
}

//---------------------------------------------------------
CFragmentation_Resampling::~CFragmentation_Resampling(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFragmentation_Resampling::Initialise(CSG_Grid *pClasses, int Class)
{
	int			x, y, Level_Count;
	double		Level_Grow, Level_Start, Density, Connectivity;
	CSG_Grid	*pDensity, *pConnectivity;

	pDensity		= Parameters("DENSITY")			->asGrid();
	pConnectivity	= Parameters("CONNECTIVITY")	->asGrid();

	Level_Grow		= Parameters("LEVEL_GROW")		->asDouble();

	m_bDensityMean	= Parameters("DENSITY_MEAN")	->asBool();

	//-----------------------------------------------------
	if( Level_Grow > 0.0 )
	{
		for(y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			for(x=0; x<Get_NX(); x++)
			{
				if( Get_Connectivity(x, y, pClasses, Class, Density, Connectivity) )
				{
					pDensity		->Set_Value (x, y, Density);
					pConnectivity	->Set_Value (x, y, Connectivity);
				}
				else
				{
					pDensity		->Set_NoData(x, y);
					pConnectivity	->Set_NoData(x, y);
				}
			}
		}

		//-------------------------------------------------
	//	Level_Grow	*= Get_Cellsize();
	//	Level_Start	= 0.0;
	//	Level_Count	= m_Radius_iMax;
	//	m_Radius_iMin--;
	//	m_Radius_iMax--;

		Level_Count	= 1 + (int)((m_Radius_Max - m_Radius_Min) / Level_Grow);
		Level_Grow	*= Get_Cellsize();
		Level_Start	= Level_Grow * (1.0 + 2.0 * m_Radius_Min);

		if(	m_Density		.Create(pDensity		, Level_Grow, Level_Start, Level_Count, GRID_PYRAMID_Mean, GRID_PYRAMID_Arithmetic)
		&&	m_Connectivity	.Create(pConnectivity	, Level_Grow, Level_Start, Level_Count, GRID_PYRAMID_Mean, GRID_PYRAMID_Arithmetic) )
		{
			for(int iGrid=0; iGrid<m_Density.Get_Count(); iGrid++)
			{
				Message_Add(CSG_String::Format(SG_T("%s %d: %f (%f)"), _TL("Scale"), 1 + iGrid, m_Density.Get_Grid(iGrid)->Get_Cellsize(), m_Density.Get_Grid(iGrid)->Get_Cellsize() / Get_Cellsize()));
			//	DataObject_Add(SG_Create_Grid(*m_Density		.Get_Grid(iGrid)));
			//	DataObject_Add(SG_Create_Grid(*m_Connectivity	.Get_Grid(iGrid)));
			}

			return( true );
		}
	}

	return( false );
}

//---------------------------------------------------------
bool CFragmentation_Resampling::Finalise(void)
{
	m_Density		.Destroy();
	m_Connectivity	.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFragmentation_Resampling::Get_Fragmentation(int x, int y, double &Density, double &Connectivity)
{
	if( m_Density.Get_Grid(-1)->is_InGrid(x, y) )
	{
		TSG_Point	p	= Get_System()->Get_Grid_to_World(x, y);

	//	Density			= 0.0;	m_Density		.Get_Grid(m_Radius_iMin)->Get_Value(p, Density);
	//	Connectivity	= 0.0;	m_Connectivity	.Get_Grid(m_Radius_iMin)->Get_Value(p, Connectivity);

	//	for(int iGrid=m_Radius_iMin+1; iGrid<m_Density.Get_Count(); iGrid++)

		Density			= 0.0;	m_Density		.Get_Grid(0)->Get_Value(p, Density);
		Connectivity	= 0.0;	m_Connectivity	.Get_Grid(0)->Get_Value(p, Connectivity);

		for(int iGrid=1; iGrid<m_Density.Get_Count(); iGrid++)
		{
			double		d, c;

			if( m_Density.Get_Grid(iGrid)->Get_Value(p, d) && m_Connectivity.Get_Grid(iGrid)->Get_Value(p, c) )
			{
				switch( m_Aggregation )
				{
				case 0:	default:
					Density			= 0.5 * (d + Density);
					Connectivity	= 0.5 * (c + Connectivity);
					break;

				case 1:
					Density			*= d;
					Connectivity	*= c;
					break;
				}
			}
		}

		if( Density < 0.0 || Connectivity < 0.0 )
			return( false );

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
bool CFragmentation_Resampling::Get_Connectivity(int x, int y, CSG_Grid *pClasses, int Class, double &Density, double &Connectivity)
{
	if( pClasses && pClasses->is_InGrid(x, y) )
	{
		bool	bClass;
		int		i, j, nDensity, nConnectivity, ix, iy;

		//-------------------------------------------------
		bClass			= pClasses->asInt(x, y) == Class;
		Density			= bClass ? 1.0 : 0.0;
		Connectivity	= 0.0;

		for(i=0, j=7, nDensity=1, nConnectivity=0; i<8; j=i++)
		{
			if( Get_System()->Get_Neighbor_Pos(i, x, y, ix, iy) && !pClasses->is_NoData(ix, iy) )
			{
				nDensity++;

				if( pClasses->asInt(ix, iy) == Class )
				{
					if( m_bDensityMean )
						Density++;

					nConnectivity++;

					if( bClass )
						Connectivity++;

					if( Get_System()->Get_Neighbor_Pos(j, x, y, ix, iy) && !pClasses->is_NoData(ix, iy) )
					{
						nConnectivity++;

						if( pClasses->asInt(ix, iy) == Class )
							Connectivity++;
					}
				}
				else
				{
					if( bClass )
						nConnectivity++;

					if( Get_System()->Get_Neighbor_Pos(j, x, y, ix, iy) && !pClasses->is_NoData(ix, iy) && pClasses->asInt(ix, iy) == Class )
						nConnectivity++;
				}
			}
		}

		if( nDensity > 1 && m_bDensityMean )
		{
			Density			/= nDensity;
		}

		if( nConnectivity > 1 )
		{
			Connectivity	/= nConnectivity;
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

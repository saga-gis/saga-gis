
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library:                     //
//                     climate_tools                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    cloud_overlap.cpp                  //
//                                                       //
//                 Copyright (C) 2019 by                 //
//                  Dirk Nikolaus Karger                 //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for Automated     //
// Geoscientific Analyses'. SAGA is free software; you   //
// can redistribute it and/or modify it under the terms  //
// of the GNU General Public License as published by the //
// Free Software Foundation; version >=2 of the License. //
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
//    e-mail:     dirk.karger@wsl.ch                     //
//                                                       //
//    contact:    D.N. Karger                            //
//                Swiss Federal Research Institute WSL   //
//                Zürcherstrasse 111                     //
//                8930 Birmensdorf                       //
//                Switzerland                            //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "cloud_overlap.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CCloud_Overlap::CCloud_Overlap(void)
{
	Set_Name		(_TL("Cloud Overlap"));

	Set_Author		("D.N. Karger (c) 2019");

	Set_Description(_TW(
		"This tool calculates cloud overlay based on the maximum random overlap assumption "
		"for atmospheric cloud layers above ground. Alpha is a constant and a further parameter "
		"is the minimum cloud fraction, at which a cloud is identified as such. "
	));

	//-----------------------------------------------------
	Parameters.Add_Grid_List("",
		"COVERS"	, _TL("Cloud Fractions"),
		_TL("grid stack of cloud fractions"),
		PARAMETER_INPUT, false
	);

	Parameters.Add_Grid_List("",
		"HEIGHTS"	, _TL("Heights"),
		_TL("grid stack of geopotential level heights"),
		PARAMETER_INPUT, false
	);

	Parameters.Add_Grid("",
		"GROUND"	, _TL("Surface Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"WIND"		, _TL("Wind effect"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"CBASE"		, _TL("Cloud Base"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid("",
		"COVER"		, _TL("Total Cloud Cover"),
		_TL("statistics"),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid("",
		"BLOCKS"	, _TL("Number of Cloud Blocks"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Double("",
		"INTERVAL"	, _TL("Interval"),
		_TL("Vertical resolution for internal interpolation given in meters."),
		100., 1., true
	);

	Parameters.Add_Double("",
		"MINCOVER"	, _TL("Minimum Cloud Cover Fraction"),
		_TL("Minimum cloud cover fraction at which a cloud is identified as such"),
		0.1, -1., true, 1., true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CCloud_Overlap::Get_Value(const CSG_Table &Values, double z)
{
	double	z1 = Values[0].asDouble(0);
	double	c1 = Values[0].asDouble(1);

	if( z > z1 )
	{
		for(int i=1; i<Values.Get_Count(); i++)
		{
			double	z0 = z1; z1 = Values[i].asDouble(0);
			double	c0 = c1; c1 = Values[i].asDouble(1);

			if( z < z1 )
			{
				double	dz	= z1 - z0;
				double	dc	= c1 - c0;

				return( dz > 0. ? c0 + dc * (z - z0) / dz : c0 );
			}
		}
	}

	return( c1 );
}

//---------------------------------------------------------
bool CCloud_Overlap::Get_Values(double xWorld, double yWorld, CSG_Table &Values)
{
	Values.Destroy();

	Values.Add_Field("Z", SG_DATATYPE_Double);
	Values.Add_Field("C", SG_DATATYPE_Double);

	Values.Set_Count(m_pCovers->Get_Grid_Count());

	for(int i=0; i<m_pCovers->Get_Grid_Count(); i++)
	{
		Values[i].Set_Value(0, m_pHeights->Get_Grid(i)->Get_Value(xWorld, yWorld));
		Values[i].Set_Value(1, m_pCovers ->Get_Grid(i)->Get_Value(xWorld, yWorld));
	}

	return( Values.Set_Index(0, TABLE_INDEX_Ascending) );
}

//---------------------------------------------------------
bool CCloud_Overlap::Get_Values(double xWorld, double yWorld, double zGround, double zInterval, CSG_Vector &Covers)
{
	CSG_Table	Values;

	if( !Get_Values(xWorld, yWorld, Values) )
	{
		return( false );
	}

	int	i	= 0;

	for(double z=zGround; z<=Values.Get_Maximum(0); z+=zInterval)
	{
		Covers.Add_Row();	Covers[i++]	= Get_Value(Values, z);
	}

	return( Covers.Get_Size() > 0 );
}

//---------------------------------------------------------
bool CCloud_Overlap::On_Execute(void)
{
	m_pCovers  = Parameters("COVERS" )->asGridList();
	m_pHeights = Parameters("HEIGHTS")->asGridList();

	if( m_pCovers->Get_Grid_Count() != m_pHeights->Get_Grid_Count() )
	{
		Error_Fmt("%s (%d/%d)", _TL("mismatch in the number of cover and height grids"), m_pCovers->Get_Grid_Count(), m_pHeights->Get_Grid_Count());

		return( false );
	}

	if( m_pCovers->Get_Grid_Count() <= 1 )
	{
		Error_Set(_TL("a minimum of two input level grids is required"));

		return( false );
	}

	CSG_Grid	*pGround = Parameters("GROUND")->asGrid();
	CSG_Grid	*pWind   = Parameters("WIND"  )->asGrid();
	CSG_Grid	*pBase   = Parameters("CBASE" )->asGrid();
	CSG_Grid	*pCover  = Parameters("COVER" )->asGrid();
	CSG_Grid	*pBlocks = Parameters("BLOCKS")->asGrid();

	double	minCover = Parameters("MINCOVER")->asDouble();
	double	Interval = Parameters("INTERVAL")->asDouble();

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		double	yWorld	= Get_YMin() + y * Get_Cellsize();

		#ifndef _DEBUG
		#pragma omp parallel for
		#endif
		for(int x=0; x<Get_NX(); x++)
		{
			double	xWorld	= Get_XMin() + x * Get_Cellsize();
			double  Ground  = pGround->asDouble(x, y);

			CSG_Vector Covers;

			if( !Get_Values(xWorld, yWorld, Ground, Interval, Covers) )
			{
				if( pBlocks ) pBlocks->Set_NoData(x, y);
				if( pCover  ) pCover ->Set_NoData(x, y);

				continue;
			}

			//---------------------------------------------
			CSG_Vector Blocks;

			double  Wind	= pWind->asDouble(x, y);
			double  Base	= pBase->asDouble(x, y);

			for(size_t i=0, bCloud=false; i<Covers.Get_Size(); i++)
			{
				double	Wind_cor = Wind + (1. - Wind) * i / (Covers.Get_Size() - 1.);	// adjust the wind effect by distance to ground

				if( Ground < Base )
				{
					Wind_cor	-= (1. - Wind_cor) * (Ground - Base) / Base;
				}					

				double Cover	= Wind_cor * Covers[i];

				if( Cover > minCover ) // yes, it's a cloud!
				{
					if( !bCloud )	// it's a new cloud block starting
					{
						Blocks.Add_Row(Cover);
					}
					else if( Cover > Blocks[Blocks.Get_Size() - 1] )
					{
						Blocks[Blocks.Get_Size() - 1] = Cover;
					}

					bCloud = true;
				}
				else
				{
					bCloud = false;
				}
			}

			//---------------------------------------------
			double	Cover;

			if( Blocks.Get_Size() < 1 )
			{
				Cover	= 0.;
			}
			else if( Blocks.Get_Size() == 1 )
			{
				Cover	= Blocks[0];
			}
			else // if( Blocks.Get_Size() > 1 )
			{
				double	Cprod	= Blocks[0];

				for(size_t i=1; i<Blocks.Get_Size(); i++)
				{
					Cprod   *= 1. - Blocks[i];
				}

				Cover	= 1. - Cprod;
			}

			if( pBlocks ) pBlocks->Set_Value(x, y, (double)Blocks.Get_Size());	// number of blocks
			if( pCover  ) pCover ->Set_Value(x, y, Cover);
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

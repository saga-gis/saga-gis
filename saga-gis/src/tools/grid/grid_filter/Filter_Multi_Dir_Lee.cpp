/**********************************************************
 * Version $Id$
 *********************************************************/

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
//                Filter_Multi_Dir_Lee.cpp               //
//                                                       //
//                 Copyright (C) 2003 by                 //
//                     Andre Ringeler                    //
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
//    e-mail:     aringel@gwdg.de                        //
//                                                       //
//    contact:    Andre Ringeler                         //
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
#include "Filter_Multi_Dir_Lee.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double Filter_Directions[16][9][9]	=
{ {
	{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
	{0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5},
	{1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0},
	{0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5},
	{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}
}, {
	{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
	{0.5, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
	{1.0, 1.0, 0.5, 0.5, 0.5, 0.5, 0.0, 0.0, 0.0},
	{0.5, 0.5, 1.0, 1.0, 1.0, 1.0, 1.0, 0.5, 0.5},
	{0.0, 0.0, 0.0, 0.5, 0.5, 0.5, 0.5, 1.0, 1.0},
	{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.5, 0.5},
	{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}
}, {
	{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
	{0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
	{1.0, 1.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
	{0.5, 0.5, 1.0, 1.0, 0.5, 0.0, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.5, 0.5, 1.0, 0.5, 0.5, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.0, 0.5, 1.0, 1.0, 0.5, 0.5},
	{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.5, 1.0, 1.0},
	{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.5},
	{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}
}, {
	{0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
	{1.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
	{0.5, 1.0, 1.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0},
	{0.0, 0.5, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.5, 0.0},
	{0.0, 0.0, 0.0, 0.0, 0.0, 0.5, 1.0, 1.0, 0.5},
	{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.5, 1.0},
	{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.5}
}, {
	{1.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
	{0.5, 1.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
	{0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0},
	{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.5, 1.0, 0.5},
	{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.5, 1.0}
}, {
	{0.5, 1.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
	{0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.5, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.5, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0},
	{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.5, 1.0, 0.5}
}, {
	{0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0},
	{0.0, 0.0, 1.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 1.0, 0.5, 0.0, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.0, 0.5, 1.0, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.0, 0.0, 0.5, 1.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0}
}, {
	{0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 1.0, 0.5, 0.0, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.0, 0.5, 1.0, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0}
}, {
	{0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0},
	{0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0}
} };


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFilter_Multi_Dir_Lee::CFilter_Multi_Dir_Lee(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Multi Direction Lee Filter"));

	Set_Author		("A.Ringeler (c) 2003");

	Set_Description	(_TW(
		"The tool searches for the minimum variance within 16 directions "
		"and applies a Lee Filter in the direction of minimum variance. The "
		"filter is edge-preserving and can be used to remove speckle noise "
		"from SAR images or to smooth DTMs. Applied to DTMs, this filter "
		"will preserve slope breaks and narrow valleys.\n"
		"For more details, please refer to the references. "
	));

	Add_Reference("Lee, J.S.", "1980",
		"Digital image enhancement and noise filtering by use of local statistics",
        "IEEE Transactions on Pattern Analysis and Machine Intelligence, PAMI-2: 165-168."
	);

	Add_Reference("Lee, J.S., Papathanassiou, K.P., Ainsworth, T.L., Grunes, M.R., Reigber, A.", "1998",
		"A New Technique for Noise Filtering of SAR Interferometric Phase Images",
		"IEEE Transactions on Geosciences and Remote Sensing 36(5): 1456-1465."
	);

	Add_Reference("Selige, T., Böhner, J., Ringeler, A.", "2006",
		"Processing of SRTM X-SAR Data to correct interferometric elevation models for land surface process applications",
		"In: Böhner, J., McCloy, K.R., Strobl, J. [Eds.]: SAGA - Analysis and Modelling Applications. Göttinger Geographische Abhandlungen, Vol. 115: 97-104.",
        SG_T("http://downloads.sourceforge.net/saga-gis/gga115_09.pdf")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid(
		"", "INPUT"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		"", "RESULT"	, _TL("Filtered Grid"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		"", "STDDEV"	, _TL("Minimum Standard Deviation"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		"", "DIR"		, _TL("Direction of Minimum Standard Deviation"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Value(
		"", "NOISE_ABS"	, _TL("Estimated Noise (absolute)"),
		_TL("Estimated noise in units of input data"),
		PARAMETER_TYPE_Double, 1.0
	);

	Parameters.Add_Double(
		"", "NOISE_REL"	, _TL("Estimated Noise (relative)"),
		_TL("Estimated noise relative to mean standard deviation"),
		1.0
	);

	Parameters.Add_Bool(
		"", "WEIGHTED"	, _TL("Weighted"),
		_TL(""),
		true
	);

	Parameters.Add_Choice(
		"", "METHOD"	, _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("noise variance given as absolute value"),
			_TL("noise variance given relative to mean standard deviation"),
			_TL("original calculation (Ringeler)")
		), 1
	);

	//-----------------------------------------------------
	for(int i=1; i<8; i++)	// Mirror the last 8  filter directions
	{
		for(int y=0; y<9; y++)
		{
			for(int x=0; x<9; x++)
			{
				Filter_Directions[i + 8][y][x] = Filter_Directions[i][y][8 - x];
			}
		}
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFilter_Multi_Dir_Lee::On_Execute(void)
{
	m_pInput		= Parameters("INPUT" )->asGrid();
	m_pFiltered		= Parameters("RESULT")->asGrid();
	m_pStdDev		= Parameters("STDDEV")->asGrid();
	m_pDirection	= Parameters("DIR"   )->asGrid();

	if( m_pFiltered  ) m_pFiltered ->Set_Name("%s [%s]", m_pInput->Get_Name(), _TL("Lee Filter"));
	if( m_pStdDev    ) m_pStdDev   ->Set_Name("%s [%s]", m_pInput->Get_Name(), _TL("Lee Filter - Standard Deviation"));
	if( m_pDirection ) m_pDirection->Set_Name("%s [%s]", m_pInput->Get_Name(), _TL("Lee Filter - Direction"));

	switch( Parameters("METHOD")->asInt() )
	{
	default: return( Get_Filter( true) );
	case  1: return( Get_Filter(false) );

	case  2: return( Get_Filter_Ringeler() );	// obsolete?!
	}
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFilter_Multi_Dir_Lee::Get_Filter(bool bAbsolute)
{
	bool	bWeighted	= Parameters("WEIGHTED")->asBool();

	double	Noise	= Parameters("NOISE_ABS")->asDouble();

	CSG_Grid	Temp;

	if( !bAbsolute && m_pStdDev == NULL )
	{
		m_pStdDev	= &Temp; Temp.Create(Get_System());
	}

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<Get_NX(); x++)
		{
			if( m_pInput->is_NoData(x, y) )
			{
				if( m_pFiltered  ) m_pFiltered ->Set_NoData(x, y);
				if( m_pStdDev    ) m_pStdDev   ->Set_NoData(x, y);
				if( m_pDirection ) m_pDirection->Set_NoData(x, y);
			}
			else
			{
				int		Dir;
				double	Mean, StdDev;

				for(int k=0; k<16; k++)
				{
					CSG_Simple_Statistics	s;

					for(int ky=0, iy=y-4; ky<9; iy++, ky++)
					{
						for(int kx=0, ix=x-4; kx<9; ix++, kx++)
						{
							if( m_pInput->is_InGrid(ix, iy) && Filter_Directions[k][ky][kx] > 0.0 )
							{
								s.Add_Value(m_pInput->asDouble(ix, iy), bWeighted ? Filter_Directions[k][ky][kx] : 1.0);
							}
						}
					}

					if( k == 0 || StdDev > s.Get_StdDev() )
					{
						Dir		= k;
						Mean	= s.Get_Mean  ();
						StdDev	= s.Get_StdDev();
					}
				}

				//-----------------------------------------
				if( bAbsolute && StdDev > Noise )
				{
					double	b	= StdDev*StdDev;

					b	= (b - Noise*Noise) / b;

					m_pFiltered->Set_Value(x, y, m_pInput->asDouble(x, y) * b + (1.0 - b) * Mean);
				}
				else if( StdDev > 0.0 )
				{
					m_pFiltered->Set_Value(x, y, Mean);
				}
				else
				{
					m_pFiltered->Set_Value(x, y, m_pInput->asDouble(x, y));
				}

				if( m_pStdDev    ) m_pStdDev   ->Set_Value(x, y, StdDev);
				if( m_pDirection ) m_pDirection->Set_Value(x, y, Dir   );
			}
		}
	}

	//-----------------------------------------------------
	if( !bAbsolute )
	{
		Noise	= Parameters("NOISE_REL")->asDouble() * m_pStdDev->Get_Mean();

		#pragma omp parallel for
		for(sLong i=0; i<Get_NCells(); i++)
		{
			if( !m_pInput->is_NoData(i) )
			{
				double	b	= m_pStdDev->asDouble(i);
			
				if( b > Noise )
				{
					b	= (b*b - Noise*Noise) / b*b;

					m_pFiltered->Set_Value(i, m_pInput->asDouble(i) * b + (1. - b) * m_pFiltered->asDouble(i));
				}
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
const double corr_norm	= 109699939.0;

const double corr[16]	=
{
	 97694238.970824,
	103389994.176977,
	109699939.129502,
	103392028.763373,
	103392435.589500,
	103392028.763371,
	109699939.129499,
	103389994.176979,
	 97694238.970826,
	103389994.200091,
	109699939.176253,
	103392028.826671,
	103392435.659830,
	103392028.826669,
	109699939.176251,
	103389994.200092
};

//---------------------------------------------------------
bool CFilter_Multi_Dir_Lee::Get_Filter_Ringeler(void)
{
	int			x, y, ix, iy, k, kx, ky, Count, Best_Direction;
	double		Mean, StdDev, Variance, Best_Mean, Best_StdDev, Noise, Noise2;

	Noise	= Parameters("NOISE_ABS")->asDouble();
	Noise2	= Noise*Noise;

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( m_pInput->is_NoData(x, y) )
			{
				m_pFiltered->Set_NoData(x, y);

				if( m_pDirection )	m_pDirection	->Set_NoData(x, y);
				if( m_pStdDev )		m_pStdDev		->Set_NoData(x, y);
			}
			else
			{
				//-----------------------------------------
				for(k=0; k<16; k++)
				{
					Variance	= Mean	= Count	= 0;

					for(ky=0, iy=y-4; ky<9; iy++, ky++)
					{
						for(kx=0, ix=x-4; kx<9; ix++, kx++)
						{
							if( m_pInput->is_InGrid(ix, iy) && Filter_Directions[k][ky][kx] > 0.0 )
							{
								Mean		+= m_pInput->asDouble(ix, iy);
								Count		++;
							}
						}
					}

					Mean		= Mean     / Count;

					for(ky=0, iy=y-4; ky<9; iy++, ky++)
					{
						for(kx=0, ix=x-4; kx<9; ix++, kx++)
						{
							if( m_pInput->is_InGrid(ix, iy) && Filter_Directions[k][ky][kx] > 0.0 )
							{
								Variance	+= M_SQR(Mean - m_pInput->asDouble(ix, iy));
							}
						}
					}

					StdDev		= corr_norm * sqrt(Variance) / (corr[k] * Count);

					if( k == 0 || StdDev < Best_StdDev )
					{
						Best_StdDev		= StdDev;
						Best_Mean		= Mean;
						Best_Direction	= k;
					}
				}

				//-----------------------------------------
				if( Best_StdDev > Noise )
				{
					double	b	= Best_StdDev*Best_StdDev;

					b	= (b - Noise2) / b;

					m_pFiltered->Set_Value(x, y, m_pInput->asDouble(x, y) * b + (1.0 - b) * Best_Mean);
				}
				else if( Best_StdDev > 0.0 )
				{
					m_pFiltered->Set_Value(x, y, Best_Mean);
				}
				else
				{
					m_pFiltered->Set_Value(x, y, m_pInput->asDouble(x, y));
				}

				if( m_pDirection )	m_pDirection	->Set_Value(x, y, Best_Direction);
				if( m_pStdDev )		m_pStdDev		->Set_Value(x, y, Best_StdDev);
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

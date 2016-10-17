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


#include "Filter_Multi_Dir_Lee.h"


double Filter_Directions[16][9][9]	=
{
	{
		{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
		{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
		{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
		{0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5},
		{1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0, 1.0},
		{0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5, 0.5},
		{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
		{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
		{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}
	},

	{
		{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
		{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
		{0.5, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
		{1.0, 1.0, 0.5, 0.5, 0.5, 0.5, 0.0, 0.0, 0.0},
		{0.5, 0.5, 1.0, 1.0, 1.0, 1.0, 1.0, 0.5, 0.5},
		{0.0, 0.0, 0.0, 0.5, 0.5, 0.5, 0.5, 1.0, 1.0},
		{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.5, 0.5},
		{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
		{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}
	},

	{
		{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
		{0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
		{1.0, 1.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
		{0.5, 0.5, 1.0, 1.0, 0.5, 0.0, 0.0, 0.0, 0.0},
		{0.0, 0.0, 0.5, 0.5, 1.0, 0.5, 0.5, 0.0, 0.0},
		{0.0, 0.0, 0.0, 0.0, 0.5, 1.0, 1.0, 0.5, 0.5},
		{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.5, 1.0, 1.0},
		{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.5},
		{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0}
	},

	{
		{0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
		{1.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
		{0.5, 1.0, 1.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0},
		{0.0, 0.5, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0, 0.0},
		{0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0},
		{0.0, 0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.5, 0.0},
		{0.0, 0.0, 0.0, 0.0, 0.0, 0.5, 1.0, 1.0, 0.5},
		{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.5, 1.0},
		{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.5}
	},

	{
		{1.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
		{0.5, 1.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
		{0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0},
		{0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0, 0.0},
		{0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0},
		{0.0, 0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0},
		{0.0, 0.0, 0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0},
		{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.5, 1.0, 0.5},
		{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.5, 1.0}
	},

	{
		{0.5, 1.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0},
		{0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0},
		{0.0, 0.0, 0.5, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0},
		{0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0, 0.0},
		{0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0},
		{0.0, 0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0},
		{0.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.5, 0.0, 0.0},
		{0.0, 0.0, 0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0},
		{0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.5, 1.0, 0.5}
	},

	{
		{0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0},
		{0.0, 0.0, 1.0, 0.5, 0.0, 0.0, 0.0, 0.0, 0.0},
		{0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0, 0.0},
		{0.0, 0.0, 0.0, 1.0, 0.5, 0.0, 0.0, 0.0, 0.0},
		{0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0},
		{0.0, 0.0, 0.0, 0.0, 0.5, 1.0, 0.0, 0.0, 0.0},
		{0.0, 0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0},
		{0.0, 0.0, 0.0, 0.0, 0.0, 0.5, 1.0, 0.0, 0.0},
		{0.0, 0.0, 0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0}
	},

	{
		{0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0, 0.0},
		{0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0, 0.0},
		{0.0, 0.0, 0.0, 1.0, 0.5, 0.0, 0.0, 0.0, 0.0},
		{0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0},
		{0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0},
		{0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0},
		{0.0, 0.0, 0.0, 0.0, 0.5, 1.0, 0.0, 0.0, 0.0},
		{0.0, 0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0},
		{0.0, 0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0}
	},

	{
		{0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0},
		{0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0},
		{0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0},
		{0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0},
		{0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0},
		{0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0},
		{0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0},
		{0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0},
		{0.0, 0.0, 0.0, 0.5, 1.0, 0.5, 0.0, 0.0, 0.0}
	}
};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFilter_Multi_Dir_Lee::CFilter_Multi_Dir_Lee(void)
{
	//-----------------------------------------------------
	Set_Name(_TL("Multi Direction Lee Filter"));

	Set_Author(_TL("Copyrights (c) 2003 by Andre Ringeler"));

	Set_Description	(_TW(
		"The tool searches for the minimum variance within 16 directions "
		"and applies a Lee Filter in the direction of minimum variance. The "
		"filter is edge-preserving and can be used to remove speckle noise "
		"from SAR images or to smooth DTMs. Applied to DTMs, this filter "
		"will preserve slope breaks and narrow valleys.\n\n"
		"For more details, please refer to:\n"
		"Lee, J.S. (1980): Digital image enhancement and noise filtering by use of local statistics. "
        "IEEE Transactions on Pattern Analysis and Machine Intelligence, PAMI-2: 165-168\n\n"
        "Lee, J.S., Papathanassiou, K.P., Ainsworth, T.L., Grunes, M.R., Reigber, A. (1998): A "
        "New Technique for Noise Filtering of SAR Interferometric Phase Images. IEEE "
        "Transactions on Geosciences and Remote Sensing 36(5): 1456-1465.\n\n"
        "Selige, T., Böhner, J., Ringeler, A. (2006): Processing of SRTM X-SAR Data to correct interferometric "
        "elevation models for land surface process applications. In: Böhner, J., McCloy, K.R., Strobl, J. [Eds.]: "
        "SAGA - Analysis and Modelling Applications. Göttinger Geographische Abhandlungen, Vol. 115: 97-104 "
        "<a href=\"http://downloads.sourceforge.net/saga-gis/gga115_09.pdf\">&lt;PDF&gt;</a>\n"
        "\n\n"
		)
	);


	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL, "INPUT"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "RESULT"		, _TL("Filtered Grid"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		NULL, "STDDEV"		, _TL("Minimum Standard Deviation"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Grid(
		NULL, "DIR"			, _TL("Direction of Minimum Standard Deviation"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Value(
		NULL, "NOISE_ABS"	, _TL("Estimated Noise (absolute)"),
		_TL("Estimated noise in units of input data"),
		PARAMETER_TYPE_Double, 1.0
	);

	Parameters.Add_Value(
		NULL, "NOISE_REL"	, _TL("Estimated Noise (relative)"),
		_TL("Estimated noise relative to mean standard deviation"),
		PARAMETER_TYPE_Double, 1.0
	);

	Parameters.Add_Value(
		NULL, "WEIGHTED"	, _TL("Weighted"),
		_TL(""),
		PARAMETER_TYPE_Bool, true
	);

	Parameters.Add_Choice(
		NULL, "METHOD"		, _TL("Method"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|"),
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
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFilter_Multi_Dir_Lee::On_Execute(void)
{
	bool	bResult	= false, bWeighted;

	m_pInput		= Parameters("INPUT")		->asGrid();
	m_pFiltered		= Parameters("RESULT")		->asGrid();
	m_pStdDev		= Parameters("STDDEV")		->asGrid();
	m_pDirection	= Parameters("DIR")			->asGrid();
	bWeighted		= Parameters("WEIGHTED")	->asBool();

	switch( Parameters("METHOD")->asInt() )
	{
	case 0:	bResult	= Get_Filter(bWeighted, true);	break;
	case 1:	bResult	= Get_Filter(bWeighted, false);	break;
	case 2:	bResult	= Get_Filter_Ringeler();		break;
	}

	m_pFiltered->Set_Name(CSG_String::Format(SG_T("%s [%s]"), m_pInput->Get_Name(), _TL("Lee Filter")));

	if( m_pStdDev )
	{
		m_pStdDev		->Set_Name(CSG_String::Format(SG_T("%s [%s]"), m_pInput->Get_Name(), _TL("Lee Filter - Standard Deviation")));
	}

	if( m_pDirection )
	{
		m_pDirection	->Set_Name(CSG_String::Format(SG_T("%s [%s]"), m_pInput->Get_Name(), _TL("Lee Filter - Direction")));
	}

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFilter_Multi_Dir_Lee::Get_Filter(bool bWeighted, bool bAbsolute)
{
	int		x, y, ix, iy, k, kx, ky, Best_Direction;
	double	Count, Mean, StdDev, Variance, Best_Mean, Best_StdDev, Noise, Noise2;

	if( bAbsolute )
	{
		Noise	= Parameters("NOISE_ABS")->asDouble();
		Noise2	= Noise*Noise;
	}
	else if( m_pStdDev == NULL )
	{
		Parameters("STDDEV")->Set_Value(m_pStdDev = SG_Create_Grid(*Get_System(), SG_DATATYPE_Float));
	}

	//-----------------------------------------------------
	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( m_pInput->is_NoData(x, y) )
			{
				m_pFiltered	->Set_NoData(x, y);
				if( m_pStdDev )		m_pStdDev		->Set_NoData(x, y);
				if( m_pDirection )	m_pDirection	->Set_NoData(x, y);
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
								double	w	 = bWeighted ? Filter_Directions[k][ky][kx] : 1.0;
								double	z	 = m_pInput->asDouble(ix, iy);
								Mean		+= w * z;
								Variance	+= w * z*z;
								Count		+= w;
							}
						}
					}

					Mean		= Mean     / Count;
					Variance	= Variance / Count - Mean*Mean;
					StdDev		= sqrt(Variance);

					if( k == 0 || StdDev < Best_StdDev )
					{
						Best_StdDev		= StdDev;
						Best_Mean		= Mean;
						Best_Direction	= k;
					}
				}

				//-----------------------------------------
				if( bAbsolute && Best_StdDev > Noise )
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

				if( m_pStdDev )		m_pStdDev		->Set_Value(x, y, Best_StdDev);
				if( m_pDirection )	m_pDirection	->Set_Value(x, y, Best_Direction);
			}
		}
	}

	//-----------------------------------------------------
	if( !bAbsolute )
	{
		Noise	= Parameters("NOISE_REL")->asDouble() * m_pStdDev->Get_Mean();
		Noise2	= Noise*Noise;

		for(y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			for(x=0; x<Get_NX(); x++)
			{
				if( !m_pInput->is_NoData(x, y) && (Best_StdDev = m_pStdDev->asDouble(x, y)) > Noise )
				{
					double	b	= Best_StdDev*Best_StdDev;

					b	= (b - Noise2) / b;

					m_pFiltered->Set_Value(x, y, m_pInput->asDouble(x, y) * b + (1.0 - b) * m_pFiltered->asDouble(x, y));
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

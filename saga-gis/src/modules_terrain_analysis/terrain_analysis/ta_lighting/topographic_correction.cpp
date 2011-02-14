
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                      ta_lighting                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               topographic_correction.cpp              //
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

///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "topographic_correction.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CTopographic_Correction::CTopographic_Correction(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name		(_TL("Topographic Correction"));

	Set_Author		(_TL("Copyrights (c) 2008 by Olaf Conrad"));

	Set_Description	(_TW(
		"\n"
		"References:\n"
		"Civco, D. L. (1989): "
		"'Topographic Normalization of Landsat Thematic Mapper Digital Imagery', "
		"Photogrammetric Engineering and Remote Sensing, 55(9), pp.1303-1309.\n"
		"\n"
		"Law, K.H., Nichol, J. (2004): "
		"'Topographic Correction for Differential Illumination Effects on Ikonos Satellite Imagery', "
		"ISPRS 2004 International Society for Photogrammetry and Remote Sensing, "
		"<a href=\"http://www.cartesia.org/geodoc/isprs2004/comm3/papers/347.pdf\">pdf</a>.\n"
		"\n"
		"Phua, M.-H., Saito, H. (2003): "
		"'Estimation of biomass of a mountainous tropical forest using Landsat TM data', "
		"Canadian Journal of Remote Sensing, 29(4), pp.429-440.\n"
		"\n"
		"Riano, D., Chuvieco, E. Salas, J., Aguado, I. (2003): "
		"'Assessment of Different Topographic Corrections in Landsat-TM Data for Mapping Vegetation Types', "
		"IEEE Transactions on Geoscience and Remote Sensing, 41(5), pp.1056-1061, "
		"<a href=\"http://www.geogra.uah.es/~emilio/pdf/Riano2003b.pdf\">pdf</a>.\n"
		"\n"
		"Teillet, P.M., Guindon, B., Goodenough, D.G. (1982): "
		"'On the slope-aspect correction of multispectral scanner data', "
		"Canadian Journal of Remote Sensing, 8(2), pp.1537-1540.\n"
		"\n"
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "DEM"			, _TL("Elevation"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "ORIGINAL"	, _TL("Original Image"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "CORRECTED"	, _TL("Corrected Image"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	pNode	= Parameters.Add_Node(NULL, "NODE_SOLAR", _TL("Solar Position"), _TL(""));

	Parameters.Add_Value(
		pNode	, "AZI"			, _TL("Azimuth [Degree]"),
		_TL(""),
		PARAMETER_TYPE_Double	, 180.0, 0.0, true, 360.0, true
	);

	Parameters.Add_Value(
		pNode	, "DEC"			, _TL("Declination [Degree]"),
		_TL(""),
		PARAMETER_TYPE_Double	,  45.0, 0.0, true,  90.0, true
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Method"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|"),
			_TL("Cosine Correction (Teillet et al. 1982)"),
			_TL("Cosine Correction (Civco 1989)"),
			_TL("Minnaert Correction"),
			_TL("Minnaert Correction with Slope (Riano et al. 2003)"),
			_TL("Minnaert Correction with Slope (Law & Nichol 2004)"),
			_TL("C Correction")
		), 4
	);

	Parameters.Add_Value(
		NULL	, "MINNAERT"	, _TL("Minnaert Correction"),
		_TL(""),
		PARAMETER_TYPE_Double	, 0.5, 0.0, true, 1.0, true
	);

	Parameters.Add_Value(
		NULL	, "MAXCELLS"	, _TL("Maximum Cells (C Correction Analysis)"),
		_TL("Maximum number of grid cells used for trend analysis as required by C correction."),
		PARAMETER_TYPE_Int		, 1000.0, 10.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTopographic_Correction::On_Execute(void)
{
	//-----------------------------------------------------
	if( Initialise() )
	{
		Process_Set_Text(_TL("topographic correction"));

		for(int y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			for(int x=0; x<Get_NX(); x++)
			{
				if( !m_pOriginal->is_NoData(x, y) )
				{
					if( !m_Incidence.is_NoData(x, y) )
					{
						m_pCorrected->Set_Value(x, y, Get_Correction(
							m_Slope		.asDouble(x, y),
							m_Incidence	.asDouble(x, y),
							m_pOriginal->asDouble(x, y)
						));
					}
					else
					{
						m_pCorrected->Set_Value(x, y, m_pOriginal->asDouble(x, y));
					}
				}
				else
				{
					m_pCorrected->Set_NoData(x, y);
				}
			}
		}

		return( Finalise() );
	}

	//-----------------------------------------------------
	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTopographic_Correction::Initialise(void)
{
	int			x, y;
	double		Azimuth, Declination, Slope, Aspect;
	CSG_Grid	*pDEM;

	//-----------------------------------------------------
	Azimuth			= Parameters("AZI")			->asDouble() * M_DEG_TO_RAD;
	Declination		= Parameters("DEC")			->asDouble() * M_DEG_TO_RAD;
	m_cosDec		= cos(Declination);
	m_sinDec		= sin(Declination);

	m_Minnaert		= Parameters("MINNAERT")	->asDouble();
	m_Method		= Parameters("METHOD")		->asInt();

	pDEM			= Parameters("DEM")			->asGrid();
	m_pOriginal		= Parameters("ORIGINAL")	->asGrid();
	m_pCorrected	= Parameters("CORRECTED")	->asGrid();

	m_pCorrected->Set_Name(CSG_String::Format(SG_T("%s [%s]"), m_pOriginal->Get_Name(), _TL("Topographic Correction")));

	m_Slope		.Create(*Get_System());
	m_Incidence	.Create(*Get_System());

	//-----------------------------------------------------
	Process_Set_Text(_TL("incidence calculation"));

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( pDEM->Get_Gradient(x, y, Slope, Aspect) )
			{
				m_Slope		.Set_Value(x, y, Slope);
				m_Incidence	.Set_Value(x, y, cos(Slope) * m_sinDec + sin(Slope) * m_cosDec * cos(Aspect - Azimuth));
			}
			else
			{
				m_Slope		.Set_NoData(x, y);
				m_Incidence	.Set_NoData(x, y);
			}
		}
	}

	//-----------------------------------------------------
	if( m_Method == 5 )	// C Correction
	{
		Process_Set_Text(_TL("regression analysis"));

		CSG_Regression	R;

		int	n		= Parameters("MAXCELLS")->asInt();
		int	nStep	= Get_NCells() < n ? 1 : (int)(Get_NCells() / n);

		for(n=0; n<Get_NCells() && Set_Progress_NCells(n); n+=nStep)
		{
			R.Add_Values(m_pOriginal->asDouble(n), m_Incidence.asDouble(n));
		}

		if( !R.Calculate() || !R.Get_Constant() )
		{
			Finalise();

			return( false );
		}

		m_C	= R.Get_Coefficient() / R.Get_Constant();

		Message_Add(R.asString());
	}

	if( m_Method == 6 )	// Normalization (after Civco, modified by Law & Nichol)
	{
		m_C	= 1.0;
	}

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
bool CTopographic_Correction::Finalise(void)
{
	m_Slope		.Destroy();
	m_Incidence	.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CTopographic_Correction::Get_Correction(double Slope, double Incidence, double Value)
{
	switch( m_Method )
	{
	case 0:	// Cosine Correction (Teillet et al. 1982)
		return( Value * m_sinDec / Incidence );

	case 1:	// Cosine Correction (Civco 1989)
		return( Value + (Value * ((m_Incidence.Get_ArithMean() - Incidence) / m_Incidence.Get_ArithMean())) );

	case 2:	// Minnaert Correction
		return( Value * pow(m_sinDec / Incidence, m_Minnaert) );

	case 3:	// Minnaert Correction with Slope (Riano et al. 2003)
		return( Value * cos(Slope) * pow(m_sinDec / (Incidence * cos(Slope)), m_Minnaert) );

	case 4:	// Minnaert Correction with Slope (Law & Nichol 2004)
		return( Value * cos(Slope) / pow(Incidence * cos(Slope), m_Minnaert) );

	case 5:	// C Correction
		return( Value * (m_sinDec + m_C) / (Incidence + m_C) );

	case 6:	// Normalization
		return( Value + ((Value * ((m_Incidence.Get_ArithMean() - Incidence) / m_Incidence.Get_ArithMean())) * m_C) );
	}

	return( Value );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

/**********************************************************
 * Version $Id: topographic_correction.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

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
		PARAMETER_INPUT, false
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
		pNode	, "AZI"			, _TL("Azimuth"),
		_TL("direction of sun (degree, clockwise from North)"),
		PARAMETER_TYPE_Double	, 180.0, 0.0, true, 360.0, true
	);

	Parameters.Add_Value(
		pNode	, "HGT"			, _TL("Height"),
		_TL("height of sun above horizon (degree)"),
		PARAMETER_TYPE_Double	,  45.0, 0.0, true,  90.0, true
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Method"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|%s|"),
			_TL("Cosine Correction (Teillet et al. 1982)"),
			_TL("Cosine Correction (Civco 1989)"),
			_TL("Minnaert Correction"),
			_TL("Minnaert Correction with Slope (Riano et al. 2003)"),
			_TL("Minnaert Correction with Slope (Law & Nichol 2004)"),
			_TL("C Correction"),
			_TL("Normalization (after Civco, modified by Law & Nichol)")
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

	Parameters.Add_Choice(
		NULL	, "MAXVALUE"	, _TL("Value Range"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("1 byte (0-255)"),
			_TL("2 byte (0-65535)")
		), 0
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
	if( !Get_Illumination() )
	{
		m_Slope			.Destroy();
		m_Illumination	.Destroy();

		return( false );
	}

	if( !Get_Model() )
	{
		m_Slope			.Destroy();
		m_Illumination	.Destroy();

		return( false );
	}

	//-----------------------------------------------------
	Process_Set_Text(_TL("Topographic Correction"));

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( m_pOriginal->is_NoData(x, y) )
			{
				m_pCorrected->Set_NoData(x, y);
			}
			else
			{
				m_pCorrected->Set_Value(x, y, Get_Correction(
					m_Slope       .asDouble(x, y),
					m_Illumination.asDouble(x, y),
					m_pOriginal  ->asDouble(x, y)
				));
			}
		}
	}

	//-----------------------------------------------------
	m_Slope			.Destroy();
	m_Illumination	.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CTopographic_Correction::Get_Correction(double Slope, double Illumination, double Value)
{
	switch( m_Method )
	{
	case 0:	// Cosine Correction (Teillet et al. 1982)
		if( Illumination > 0.0 )
		{
			Value	= Value * m_cosTz / Illumination;
		}
		break;

	case 1:	// Cosine Correction (Civco 1989)
		Value	= Value + (Value * ((m_Illumination.Get_ArithMean() - Illumination) / m_Illumination.Get_ArithMean()));
		break;

	case 2:	// Minnaert Correction
		if( Illumination > 0.0 )
		{
			Value	= Value * pow(m_cosTz / Illumination, m_Minnaert);
		}
		break;

	case 3:	// Minnaert Correction with Slope (Riano et al. 2003)
		if( Illumination > 0.0 )
		{
			Value	= Value * cos(Slope) * pow(m_cosTz / (Illumination * cos(Slope)), m_Minnaert);
		}
		break;

	case 4:	// Minnaert Correction with Slope (Law & Nichol 2004)
		if( Illumination > 0.0 )
		{
			Value	= Value * cos(Slope) / pow(Illumination * cos(Slope), m_Minnaert);
		}
		break;

	case 5:	// C Correction
		Value	= Value * (m_cosTz + m_C) / (Illumination + m_C);
		break;

	case 6:	// Normalization
		Value	= Value + ((Value * ((m_Illumination.Get_ArithMean() - Illumination) / m_Illumination.Get_ArithMean())) * m_C);
		break;
	}

	return( Value < 0 ? 0 : Value > m_maxValue ? m_maxValue : Value );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CTopographic_Correction::Get_Model(void)
{
	//-----------------------------------------------------
	m_pOriginal		= Parameters("ORIGINAL")	->asGrid();
	m_pCorrected	= Parameters("CORRECTED")	->asGrid();

	m_pCorrected	->Set_Name(CSG_String::Format(SG_T("%s [%s]"), m_pOriginal->Get_Name(), _TL("Topographic Correction")));

	m_Method		= Parameters("METHOD")		->asInt();

	m_Minnaert		= Parameters("MINNAERT")	->asDouble();

	switch( Parameters("MAXVALUE")->asInt() )
	{
	default:	m_maxValue	=   255;	break;
	case  1:	m_maxValue	= 65535;	break;
	}

	switch( m_Method )
	{
	//-----------------------------------------------------
	case 5:	// C Correction
		{
			Process_Set_Text(_TL("Regression Analysis"));

			CSG_Regression	R;

			sLong n		= Parameters("MAXCELLS")->asInt();
			int	nStep	= Get_NCells() < n ? 1 : (int)(Get_NCells() / n);

			for(n=0; n<Get_NCells() && Set_Progress_NCells(n); n+=nStep)
			{
				R.Add_Values(m_pOriginal->asDouble(n), m_Illumination.asDouble(n));
			}

			if( !R.Calculate() || !R.Get_Constant() )
			{
				return( false );
			}

			m_C	= R.Get_Coefficient() / R.Get_Constant();

			Message_Add(R.asString());
		}
		break;

	//-----------------------------------------------------
	case 6:	// Normalization (after Civco, modified by Law & Nichol)
		{
			m_C	= 1.0;
		}
		break;
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
bool CTopographic_Correction::Get_Illumination(void)
{
	Process_Set_Text(_TL("Illumination calculation"));

	//-----------------------------------------------------
	CSG_Grid	DEM, *pDEM	= Parameters("DEM")->asGrid();

	if( !pDEM->Get_System().is_Equal(*Get_System()) )
	{
		DEM.Create(*Get_System());
		DEM.Assign(pDEM, pDEM->Get_Cellsize() > Get_Cellsize() ? GRID_INTERPOLATION_BSpline : GRID_INTERPOLATION_Mean_Cells);
		pDEM	= &DEM;
	}

	//-----------------------------------------------------
	double	Azi	= Parameters("AZI")->asDouble() * M_DEG_TO_RAD;
	double	Hgt	= Parameters("HGT")->asDouble() * M_DEG_TO_RAD;

	m_cosTz	= cos(M_PI_090 - Hgt);
	m_sinTz	= sin(M_PI_090 - Hgt);

	m_Slope			.Create(*Get_System());
	m_Illumination	.Create(*Get_System());

	//-----------------------------------------------------
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			double	Slope, Aspect;

			if( pDEM->Get_Gradient(x, y, Slope, Aspect) )
			{
				m_Slope			.Set_Value(x, y, Slope);
				m_Illumination	.Set_Value(x, y, cos(Slope) * m_cosTz + sin(Slope) * m_sinTz * cos(Azi - Aspect));
			}
			else
			{
				m_Slope			.Set_Value(x, y, 0.0);
				m_Illumination	.Set_Value(x, y, m_cosTz);
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

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
//                      Grid_Filter                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    Filter_Gauss.cpp                   //
//                                                       //
//                 Copyright (C) 2003 by                 //
//                    Andre Ringeler                     //
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

//---------------------------------------------------------
#include "Filter_Gauss.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CFilter_Gauss::CFilter_Gauss(void)
{
	//-----------------------------------------------------
	// 1. Info...

	Set_Name(_TL("Gaussian Filter"));

	Set_Author(_TL("Copyrights (c) 2003 by Andre Ringeler"));

	Set_Description	(_TW(
		"The Gauss Filter is a smoothing operator that is used to `blur' or 'soften' Grid Data\n"
		"and remove detail and noise.\n"
		"The degree of smoothing is determined by the standard deviation.\n"
		"For higher standard deviations you need a greater Radius\n"
	));


	//-----------------------------------------------------
	// 2. Parameters...

	Parameters.Add_Grid(
		NULL, "INPUT"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL, "RESULT"		, _TL("Filtered Grid"),
		_TL(""),
		PARAMETER_OUTPUT_OPTIONAL
	);

	Parameters.Add_Value(
		NULL, "SIGMA"		, _TL("Standard Deviation"),
		_TL(""),
		PARAMETER_TYPE_Double, 1, 0.0001, true
	);

	Parameters.Add_Choice(
		NULL, "MODE"		, _TL("Search Mode"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),
			_TL("Square"),
			_TL("Circle")
		), 1
	);

	Parameters.Add_Value(
		NULL, "RADIUS"		, _TL("Search Radius"),
		_TL(""),
		PARAMETER_TYPE_Int, 2, 1, true
	);
}

//---------------------------------------------------------
CFilter_Gauss::~CFilter_Gauss(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CFilter_Gauss::On_Execute(void)
{
	int			Mode, Radius;
	double		Sigma;
	CSG_Grid	*pResult;

	//-----------------------------------------------------
	m_pInput	= Parameters("INPUT")	->asGrid();
	pResult		= Parameters("RESULT")	->asGrid();
	Radius		= Parameters("RADIUS")	->asInt();
	Mode		= Parameters("MODE")	->asInt();
	Sigma		= Parameters("SIGMA")	->asDouble();

	//-----------------------------------------------------
	if( Initialise(Radius, Sigma, Mode) )
	{
		if( !pResult || pResult == m_pInput )
		{
			pResult	= SG_Create_Grid(m_pInput);
		}
		else
		{
			pResult->Set_Name(CSG_String::Format(SG_T("%s [%s]"), m_pInput->Get_Name(), _TL("Gaussian Filter")));

			pResult->Set_NoData_Value(m_pInput->Get_NoData_Value());
		}

		//-------------------------------------------------
		for(int y=0; y<Get_NY() && Set_Progress(y); y++)
		{
			for(int x=0; x<Get_NX(); x++)
			{
				if( m_pInput->is_InGrid(x, y) )
				{
					pResult->Set_Value(x, y, Get_Mean(x, y));
				}
				else
				{
					pResult->Set_NoData(x, y);
				}
			}
		}

		//-------------------------------------------------
		if( !Parameters("RESULT")->asGrid() || Parameters("RESULT")->asGrid() == m_pInput )
		{
			m_pInput->Assign(pResult);

			delete(pResult);

			DataObject_Update(m_pInput);
		}

		m_Weights.Destroy();

		return( true );
	}

	return( false );
}

//---------------------------------------------------------
bool CFilter_Gauss::Initialise(int Radius, double Sigma, int Mode)
{
	int		x, y;
	double	dx, dy, val, min, max;

	//-----------------------------------------------------
	m_Weights.Create(SG_DATATYPE_Double, 1 + 2 * Radius, 1 + 2 * Radius);

	//-----------------------------------------------------
	for(y=0, dy=-Radius, min=1.0, max=0.0; y<m_Weights.Get_NY(); y++, dy++)
	{
		for(x=0, dx=-Radius; x<m_Weights.Get_NX(); x++, dx++)
		{
			switch( Mode )
			{
			case 1:
				val	= sqrt(dx*dx + dy*dy) > Radius
					? 0.0
					: exp(-(dx*dx + dy*dy) / (2.0 * Sigma*Sigma)) / (M_PI * 2.0 * Sigma*Sigma);
				break;

			case 0:
				val	= exp(-(dx*dx + dy*dy) / (2.0 * Sigma*Sigma)) / (M_PI * 2.0 * Sigma*Sigma);
				break;
			}

			m_Weights.Set_Value(x, y, val);

			if( min > max )
			{
				min	= max	= val;
			}
			else if( val < min )
			{
				min	= val;
			}
			else if( val > max )
			{
				max	= val;
			}
		}
	}

	//-----------------------------------------------------
	if( max == 0.0 )
	{
		Message_Dlg(_TL("Radius is too small"));
	}
	else if( min / max > 0.367 / 2.0 )
	{
		Message_Dlg(_TL("Radius is too small for your Standard Deviation"), Get_Name());
	}
	else
	{
		return( true );
	}

	m_Weights.Destroy();

	return( false );
}

//---------------------------------------------------------
double CFilter_Gauss::Get_Mean(int x, int y)
{
	int		ix, iy, jx, jy;
	double	s, n, w;

	for(n=0.0, s=0.0, jy=0, iy=y-(m_Weights.Get_NY()-1)/2; jy<m_Weights.Get_NY(); jy++, iy++)
	{
		for(jx=0, ix=x-(m_Weights.Get_NX()-1)/2; jx<m_Weights.Get_NX(); jx++, ix++)
		{
			if( (w = m_Weights.asDouble(jx, jy)) > 0.0 && m_pInput->is_InGrid(ix, iy) )
			{
				s	+= w * m_pInput->asDouble(ix, iy);
				n	+= w;
			}
		}
	}

	return( n > 0.0 ? s / n : m_pInput->Get_NoData_Value() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

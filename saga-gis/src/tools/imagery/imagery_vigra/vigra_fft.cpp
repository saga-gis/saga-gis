
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                        VIGRA                          //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                     vigra_fft.cpp                     //
//                                                       //
//                 Copyright (C) 2009 by                 //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "vigra_fft.h"

//---------------------------------------------------------
#include <vigra/fftw3.hxx>

//---------------------------------------------------------
using namespace vigra;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool	Copy_ComplexGrid_SAGA_to_VIGRA	(CSG_Grid &Real, CSG_Grid &Imag, FFTWComplexImage &Image, bool bCreate)
{
	if( bCreate )
	{
		Image.resize(Real.Get_NX(), Real.Get_NY());
	}

	if(	Real.Get_NX() != Image.width() || Real.Get_NY() != Image.height()
	||	Imag.Get_NX() != Image.width() || Imag.Get_NY() != Image.height() )
	{
		return( false );
	}

	#pragma omp parallel for
	for(int y=0; y<Real.Get_NY(); y++)
	{
		for(int x=0; x<Real.Get_NX(); x++)
		{
			Image(x, y).re()	= Real.asDouble(x, y);
			Image(x, y).im()	= Imag.asDouble(x, y);
		}
	}

	return( true );
}

//---------------------------------------------------------
bool	Copy_ComplexGrid_VIGRA_to_SAGA	(CSG_Grid &Real, CSG_Grid &Imag, FFTWComplexImage &Image, bool bCreate)
{
	if( bCreate )
	{
		Real.Create(SG_DATATYPE_Float, Image.width(), Image.height());
		Imag.Create(SG_DATATYPE_Float, Image.width(), Image.height());
	}

	if(	Real.Get_NX() != Image.width() || Real.Get_NY() != Image.height()
	||	Imag.Get_NX() != Image.width() || Imag.Get_NY() != Image.height() )
	{
		return( false );
	}

	#pragma omp parallel for
	for(int y=0; y<Real.Get_NY(); y++)
	{
		for(int x=0; x<Real.Get_NX(); x++)
		{
			Real.Set_Value(x, y, Image(x, y).re());
			Imag.Set_Value(x, y, Image(x, y).im());
		}
	}

	return( true );
}

//---------------------------------------------------------
bool	Copy_ComplexGrid_VIGRA_to_SAGA	(CSG_Grid &Real, FFTWComplexImage &Image, bool bCreate)
{
	if( bCreate )
	{
		Real.Create(SG_DATATYPE_Float, Image.width(), Image.height());
	}

	if(	Real.Get_NX() != Image.width() || Real.Get_NY() != Image.height() )
	{
		return( false );
	}

	#pragma omp parallel for
	for(int y=0; y<Real.Get_NY(); y++)
	{
		for(int x=0; x<Real.Get_NX(); x++)
		{
			Real.Set_Value(x, y, Image(x, y).re());
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
CViGrA_FFT::CViGrA_FFT(void)
{
	Set_Name		(_TL("Fourier Transform (ViGrA)"));

	Set_Author		("O.Conrad (c) 2009");

	Set_Description	(_TW(
		"Fourier Transform."
	));

	Add_Reference("http://ukoethe.github.io/vigra/", SG_T("ViGrA - Vision with Generic Algorithms"));

	Parameters.Add_Grid(
		"", "INPUT"		, _TL("Input"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		"", "REAL"		, _TL("Real"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		"", "IMAG"		, _TL("Imaginary"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Bool(
		"", "CENTER"	, _TL("Centered"),
		_TL(""),
		true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CViGrA_FFT::On_Execute(void)
{
	CSG_Grid	*pInput, *pReal, *pImag;

	pInput	= Parameters("INPUT")->asGrid();
	pReal	= Parameters("REAL" )->asGrid();
	pImag	= Parameters("IMAG" )->asGrid();

	//-----------------------------------------------------
	vigra::FImage			Input;
	vigra::FFTWComplexImage	Output(Get_NX(), Get_NY());

	Copy_Grid_SAGA_to_VIGRA(*pInput, Input, true);

	if( !Parameters("CENTER")->asBool() )
	{
		fourierTransform(srcImageRange(Input) , destImage(Output));
	}
	else
	{
		vigra::FFTWComplexImage	tmp(Get_NX(), Get_NY());

		fourierTransform(srcImageRange(Input) , destImage(tmp));
		moveDCToCenter  (srcImageRange(tmp)   , destImage(Output));
	}

	//-----------------------------------------------------
	Copy_ComplexGrid_VIGRA_to_SAGA(*pReal, *pImag, Output, false);

	pReal->Fmt_Name("%s [FFT - %s]", pInput->Get_Name(), _TL("Real"     ));
	pImag->Fmt_Name("%s [FFT - %s]", pInput->Get_Name(), _TL("Imaginary"));

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CViGrA_FFT_Inverse::CViGrA_FFT_Inverse(void)
{
	Set_Name		(_TL("Fourier Transform Inverse (ViGrA)"));

	Set_Author		("O.Conrad (c) 2009");

	Set_Description	(_TW(
		"Inverse Fourier Transform."
	));

	Add_Reference("http://ukoethe.github.io/vigra/", SG_T("ViGrA - Vision with Generic Algorithms"));

	Parameters.Add_Grid(
		"", "REAL"		, _TL("Real"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		"", "IMAG"		, _TL("Imaginary"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		"", "OUTPUT"	, _TL("Output"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Bool(
		"", "CENTER"	, _TL("Centered"),
		_TL(""),
		true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CViGrA_FFT_Inverse::On_Execute(void)
{
	CSG_Grid	*pReal, *pImag;

	pReal	= Parameters("REAL")->asGrid();
	pImag	= Parameters("IMAG")->asGrid();

	//-----------------------------------------------------
	vigra::FFTWComplexImage	Input, Output(Get_NX(), Get_NY());

	Copy_ComplexGrid_SAGA_to_VIGRA(*pReal, *pImag, Input, true);

	if( !Parameters("CENTER")->asBool() )
	{
		fourierTransformInverse(srcImageRange(Input), destImage(Output));
	}
	else
	{
		vigra::FFTWComplexImage	tmp(Get_NX(), Get_NY());

		moveDCToUpperLeft      (srcImageRange(Input), destImage(tmp));
		fourierTransformInverse(srcImageRange(tmp)  , destImage(Output));
	}
 
	//-----------------------------------------------------
	pReal	= Parameters("OUTPUT")->asGrid();

	Copy_ComplexGrid_VIGRA_to_SAGA(*pReal, Output, false);

	pReal->Fmt_Name("%s [FFT - %s]", Get_Name().c_str(), _TL("Real"));

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CViGrA_FFT_Real::CViGrA_FFT_Real(void)
{
	Set_Name		(_TL("Fourier Transform (Real, ViGrA)"));

	Set_Author		("O.Conrad (c) 2009");

	Set_Description	(_TW(
		"Fourier Transform (Real)."
	));

	Add_Reference("http://ukoethe.github.io/vigra/", SG_T("ViGrA - Vision with Generic Algorithms"));

	Parameters.Add_Grid(
		"", "INPUT"		, _TL("Input"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		"", "OUTPUT"	, _TL("Output"),
		_TL(""),
		PARAMETER_OUTPUT
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CViGrA_FFT_Real::On_Execute(void)
{
	CSG_Grid	*pInput, *pOutput;

	pInput	= Parameters("INPUT" )->asGrid();
	pOutput	= Parameters("OUTPUT")->asGrid();

	//-----------------------------------------------------
	vigra::FImage	Input, Output(Get_NX(), Get_NY());

	Copy_Grid_SAGA_to_VIGRA(*pInput, Input, true);

	// forward cosine transform == reflective boundary conditions
	fourierTransformRealEE(srcImageRange(Input), destImage(Output), (fftw_real)1.0);

	//-----------------------------------------------------
	Copy_Grid_VIGRA_to_SAGA(*pOutput, Output, false);

	pOutput->Fmt_Name("%s [%s - %s]", Get_Name().c_str(), pInput->Get_Name());

	return( true );

	/*/ multiply with a first derivative of Gaussian in x-direction
	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=1; x<Get_NX(); x++)
		{
			double	dx	= x * M_PI / (Get_NX() - 1);
			double	dy	= y * M_PI / (Get_NY() - 1);

			fourier(x-1, y) = fourier(x, y) * dx * std::exp(-(dx*dx + dy*dy) * scale*scale / 2.0);
		}

		fourier(width-1, y) = 0.0;
	}

	// inverse transform -- odd symmetry in x-direction, even in y, due to symmetry of the filter
	fourierTransformRealOE(srcImageRange(fourier), destImage(spatial), (fftw_real)-4.0 * (Get_NX() + 1) * (Get_NY() - 1));
	/**/
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CViGrA_FFT_Filter::CViGrA_FFT_Filter(void)
{
	Set_Name		(_TL("Fourier Filter (ViGrA)"));

	Set_Author		("O.Conrad (c) 2009");

	Set_Description	(_TW(
		"Fourier Filter."
	));

	Add_Reference("http://ukoethe.github.io/vigra/", SG_T("ViGrA - Vision with Generic Algorithms"));

	Parameters.Add_Grid(
		"", "INPUT"		, _TL("Input"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		"", "OUTPUT"	, _TL("Output"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Double(
		"", "SCALE"		, _TL("Size of smoothing filter"),
		_TL(""),
		2.0, 0.0, true
	);

	Parameters.Add_Double(
		"", "POWER"		, _TL("Power"),
		_TL(""),
		0.5
	);

	Parameters.Add_Range(
		"", "RANGE"		, _TL("Range"),
		_TL(""),
		0.1, 0.9, 0.0, true
	);

	Parameters.Add_Choice(
		"", "FILTER"	, _TL("Filter"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("gaussian"),
			_TL("power of distance"),
			_TL("include range"),
			_TL("exclude range")
		)
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CViGrA_FFT_Filter::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("FILTER") )
	{
		pParameters->Set_Enabled("SCALE", pParameter->asInt() == 0);
		pParameters->Set_Enabled("POWER", pParameter->asInt() == 1);
		pParameters->Set_Enabled("RANGE", pParameter->asInt() >= 2);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CViGrA_FFT_Filter::On_Execute(void)
{
	int			Filter;
	double		Scale, Power, Range_Min, Range_Max;
	CSG_Grid	*pInput, *pOutput;

	pInput		= Parameters("INPUT" )->asGrid();
	pOutput		= Parameters("OUTPUT")->asGrid();
	Scale		= Parameters("SCALE" )->asDouble();
	Power		= Parameters("POWER" )->asDouble();
	Filter		= Parameters("FILTER")->asInt();
	Range_Min	= Parameters("RANGE" )->asRange()->Get_Min();
	Range_Max	= Parameters("RANGE" )->asRange()->Get_Max();

	//-----------------------------------------------------
	vigra::FImage			Input, Filter_Raw(Get_NX(), Get_NY()), Filter_(Get_NX(), Get_NY());
	vigra::FFTWComplexImage	Output(Get_NX(), Get_NY());

	//-----------------------------------------------------
	#pragma omp parallel for
	for(int y=0; y<Get_NY(); y++)
	{
		double	yy	= (y - Get_NY() / 2.0) / Get_NY();

		for(int x=0; x<Get_NX(); x++)
		{
			double	xx	= (x - Get_NX() / 2.0) / Get_NX();

			switch( Filter )
			{
			default:
				Filter_Raw(x, y)	= exp(-(xx*xx + yy*yy) / 2.0 * Scale);
				break;

			case  1:
				Filter_Raw(x, y)	= pow(sqrt(xx*xx + yy*yy), Power);
				break;

			case  2:
				Filter_Raw(x, y)	= (xx = sqrt(xx*xx + yy*yy)) < Range_Min || xx > Range_Max ? 0.0 : 1.0;
				break;

			case  3:
				Filter_Raw(x, y)	= (xx = sqrt(xx*xx + yy*yy)) < Range_Min || xx > Range_Max ? 1.0 : 0.0;
				break;
			}
		}
	}

	//-----------------------------------------------------
	moveDCToUpperLeft(srcImageRange(Filter_Raw), destImage(Filter_));    // applyFourierFilter() expects the filter's DC in the upper left

	Copy_Grid_SAGA_to_VIGRA(*pInput, Input, true);

	vigra::applyFourierFilter(srcImageRange(Input), srcImage(Filter_), destImage(Output));

	//-----------------------------------------------------
	Copy_ComplexGrid_VIGRA_to_SAGA(*pOutput, Output, false);

	pOutput->Fmt_Name("%s [FFT %s]", pInput->Get_Name(), _TL("Filter"));

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                        OpenCV                         //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   opencv_fourier.cpp                  //
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
#include "opencv_fourier.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define bDepth64 0

#if bDepth64
	#define CV_REAL CV_64F
	#define real    double
	#define asReal  asDouble
	#define SG_Real SG_DATATYPE_Double
#else
	#define CV_REAL CV_32F
	#define real    float
	#define asReal  asFloat
	#define SG_Real SG_DATATYPE_Float
#endif

//---------------------------------------------------------
// rearrange the quadrants of the Fourier image so that the
// origin is at the image center or vice versa
//---------------------------------------------------------
bool Swap_Quadrants(cv::Mat &Matrix)
{
	int x = Matrix.cols / 2, y = Matrix.rows / 2;

	{	cv::Mat a(Matrix, cv::Rect(0, 0, x, y)), b(Matrix, cv::Rect(x, y, x, y));

		cv::Mat c; a.copyTo(c); b.copyTo(a); c.copyTo(b);
	}

	{	cv::Mat a(Matrix, cv::Rect(x, 0, x, y)), b(Matrix, cv::Rect(0, y, x, y));

		cv::Mat c; a.copyTo(c); b.copyTo(a); c.copyTo(b);
	}

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
COpenCV_FFT::COpenCV_FFT(void)
{
	Set_Name		(_TL("Fourier Transformation"));

	Set_Author		("O.Conrad (c) 2009");

	Set_Description	(_TW(
		"Discrete Fourier transformation."
	));

	Add_Reference("https://docs.opencv.org/4.7.0/de/dbc/tutorial_py_fourier_transform.html",
		SG_T("OpenCV Tutorial | Fourier Transform")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid        ("", "GRID"   , _TL("Grid"                  ), _TL(""), PARAMETER_INPUT);
	Parameters.Add_Grids       ("", "DFT"    , _TL("Fourier Transformation"), _TL(""), PARAMETER_OUTPUT);
	Parameters.Add_Grids_Output("", "DFT_OPT", _TL("Fourier Transformation"), _TL(""));

	Parameters.Add_Bool("",
		"CENTERED", _TL("Centered"),
		_TL(""),
		true
	);

	Parameters.Add_Choice("",
		"SIZE"    , _TL("Output Size"),
		_TL("The optimal size for the discrete Fourier transformation might differ from that of the input grid."),
		CSG_String::Format("%s|%s",
			_TL("same as input grid"),
			_TL("optimal DFT size")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int COpenCV_FFT::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("SIZE") )
	{
		pParameters->Set_Enabled("DFT"    , pParameter->asInt() == 0);
		pParameters->Set_Enabled("DFT_OPT", pParameter->asInt() == 1);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool COpenCV_FFT::On_Execute(void)
{
	cv::Mat Grid[2] = { cv::Mat(Get_NY(), Get_NX(), CV_REAL), cv::Mat() };

	CSG_Grid *pGrid = Parameters("GRID")->asGrid();

	#pragma omp parallel for
	for(sLong i=0; i<Get_NCells(); i++)
	{
		((real *)Grid[0].data)[i] = pGrid->asReal(i);
	}

	//-----------------------------------------------------
	if( Parameters("SIZE")->asInt() == 1 ) // find and use optimal DFT size...
	{
		int nx = cv::getOptimalDFTSize(Get_NX());
		int ny = cv::getOptimalDFTSize(Get_NY());
		
		cv::Mat g; cv::copyMakeBorder(Grid[0], g, 0, ny - Grid[0].rows, 0, nx - Grid[0].cols, cv::BORDER_CONSTANT, cv::Scalar::all(0));
		
		Grid[0] = g(cv::Rect(0, 0, g.cols & -2, g.rows & -2 )); // crop if odd
	}

	Grid[1] = cv::Mat::zeros(Grid[0].size(), CV_REAL);

	//-----------------------------------------------------
	cv::Mat Complex; cv::merge(Grid, 2, Complex);

	cv::dft(Complex, Complex);

	cv::split(Complex, Grid); // Grid[0] => Re(DFT(I), Grid[1] => Im(DFT(I))

	//-----------------------------------------------------
	if( 0 ) // compute the magnitude and switch to logarithmic scale
	{
		cv::magnitude(Grid[0], Grid[1], Grid[0]); // Grid[0] => magnitude

		Grid[0] += cv::Scalar::all(1); log(Grid[0], Grid[0]); // => log(1 + sqrt(Re(DFT(I))^2 + Im(DFT(I))^2))
	}

	if( 0 ) // crop the spectrum, if it has an odd number of rows or columns
	{
		Grid[0] = Grid[0](cv::Rect(0, 0, Grid[0].cols & -2, Grid[0].rows & -2));
		Grid[1] = Grid[1](cv::Rect(0, 0, Grid[1].cols & -2, Grid[1].rows & -2));
	}

	//-----------------------------------------------------
	if( Parameters("CENTERED")->asBool() )
	{
		Swap_Quadrants(Grid[0]);
		Swap_Quadrants(Grid[1]);
	}

	//-----------------------------------------------------
	real *Re = (real *)Grid[0].data, *Im = (real *)Grid[1].data;

	CSG_Grids *pDFT = NULL;

	if( Parameters("SIZE")->asInt() == 0 )
	{
		CSG_Grids &DFT = *(pDFT = Parameters("DFT")->asGrids());

		DFT.Create(Get_System(), 2, 0., SG_Real);

		#pragma omp parallel for
		for(sLong i=0; i<Get_NCells(); i++)
		{
			DFT[0].Set_Value(i, Re[i]);
			DFT[1].Set_Value(i, Im[i]);
		}
	}
	else // find and use optimal DFT size...
	{
		int nx = Grid[0].cols, ny = Grid[0].rows;

		CSG_Grids &DFT = *(pDFT = SG_Create_Grids());

		Parameters("DFT_OPT")->Set_Value(pDFT);

		DFT.Create(nx, ny, 2, Get_Cellsize(), Get_XMin(), Get_YMin(), 0., SG_Real);

		#pragma omp parallel for
		for(int y=0; y<Get_NY(); y++)
		{
			for(int x=0, i=y*nx; x<Get_NX(); x++, i++)
			{
				DFT[0].Set_Value(x, y, Re[i]);
				DFT[1].Set_Value(x, y, Im[i]);
			}
		}
	}

	//-----------------------------------------------------
	pDFT->Fmt_Name("%s [DFT]", pGrid->Get_Name());
	pDFT->Get_Attributes_Ptr()->Set_Field_Name(0, _TL("ID"));
	pDFT->Add_Attribute(_TL("Name"), SG_DATATYPE_String);
	pDFT->Get_Attributes(0).Set_Value(0, 1); pDFT->Get_Attributes(0).Set_Value(1, _TL("real"     ));
	pDFT->Get_Attributes(1).Set_Value(0, 2); pDFT->Get_Attributes(1).Set_Value(1, _TL("imaginary"));
	pDFT->Set_Z_Name_Field(1);

	CSG_MetaData &Info = *pDFT->Get_MetaData().Add_Child("DFT_ORIGIN");

	Info.Add_Child("NAME"   , pGrid->Get_Name  ());
	Info.Add_Child("MIN"    , pGrid->Get_Min   ());
	Info.Add_Child("MAX"    , pGrid->Get_Max   ());
	Info.Add_Child("MEAN"   , pGrid->Get_Mean  ());
	Info.Add_Child("STDDEV" , pGrid->Get_StdDev());
	Info.Add_Child("TYPE_ID", pGrid->Get_Type  ());
	Info.Add_Child("TYPE"   , SG_Data_Type_Get_Name(pGrid->Get_Type()));

	Info.Add_Child("SYSTEM", "SYSTEM");
	Info("SYSTEM")->Add_Child("NX"      , Get_NX      ());
	Info("SYSTEM")->Add_Child("NY"      , Get_NY      ());
	Info("SYSTEM")->Add_Child("CELLSIZE", Get_Cellsize());
	Info("SYSTEM")->Add_Child("XMIN"    , Get_XMin    ());
	Info("SYSTEM")->Add_Child("YMIN"    , Get_YMin    ());

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
COpenCV_FFTinv::COpenCV_FFTinv(void)
{
	Set_Name		(_TL("Inverse Fourier Transformation"));

	Set_Author		("O.Conrad (c) 2023");

	Set_Description	(_TW(
		"Inverse discrete Fourier transformation."
	));

	Add_Reference("https://docs.opencv.org/4.7.0/de/dbc/tutorial_py_fourier_transform.html",
		SG_T("OpenCV Tutorial | Fourier Transform")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("", "REAL", _TL("Real"     ), _TL(""), PARAMETER_INPUT );
	Parameters.Add_Grid("", "IMAG", _TL("Imaginary"), _TL(""), PARAMETER_INPUT );
	Parameters.Add_Grid("", "GRID", _TL("Grid"     ), _TL(""), PARAMETER_OUTPUT);

	Parameters.Add_Bool("",
		"CENTERED", _TL("Centered"),
		_TL(""),
		true
	);

	Parameters.Add_Bool("",
		"RESTORE" , _TL("Restore"),
		_TL("Restore original grid information from metadata."),
		true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int COpenCV_FFTinv::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("REAL") )
	{
		CSG_Data_Object *pOrigin = pParameter->asGrid() ? pParameter->asGrid()->Get_Owner() : pParameter->asGrid();

		pParameters->Set_Enabled("RESTORE", pOrigin && pOrigin->Get_MetaData()("DFT_ORIGIN"));
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool COpenCV_FFTinv::On_Execute(void)
{
	cv::Mat Grid[2];

	Grid[0] = cv::Mat(Get_NY(), Get_NX(), CV_REAL);
	Grid[1] = cv::Mat(Get_NY(), Get_NX(), CV_REAL);

	CSG_Grid *pReal = Parameters("REAL")->asGrid();
	CSG_Grid *pImag = Parameters("IMAG")->asGrid();

	#pragma omp parallel for
	for(sLong i=0; i<Get_NCells(); i++)
	{
		((real *)Grid[0].data)[i] = pReal->asReal(i);
		((real *)Grid[1].data)[i] = pImag->asReal(i);
	}

	//-----------------------------------------------------
	if( Parameters("CENTERED")->asBool() )
	{
		Swap_Quadrants(Grid[0]);
		Swap_Quadrants(Grid[1]);
	}

	//-----------------------------------------------------
	cv::Mat Complex; cv::merge(Grid, 2, Complex);

	cv::idft(Complex, Complex);

	cv::split(Complex, Grid);

	real *data = (real *)Grid[0].data; CSG_Grid *pGrid = Parameters("GRID")->asGrid();

	#pragma omp parallel for
	for(sLong i=0; i<Get_NCells(); i++)
	{
		pGrid->Set_Value(i, data[i]);
	}

	pGrid->Set_Name(_TL("Inverse Fourier Transformation"));

	//-----------------------------------------------------
	if( Parameters("RESTORE")->asBool() )
	{
		CSG_MetaData &Data = pReal->Get_Owner() ? pReal->Get_Owner()->Get_MetaData() : pReal->Get_MetaData();

		if( Data("DFT_ORIGIN") )
		{
			CSG_String Name;

			if( Data["DFT_ORIGIN"].Get_Content("NAME", Name) )
			{
				pGrid->Fmt_Name("%s [DFT %s]", Name.c_str(), _TL("inverse"));
			}

			double a, b, c = pGrid->Get_Range();

			if( c > 0. && Data["DFT_ORIGIN"].Get_Content("MIN", a) && Data["DFT_ORIGIN"].Get_Content("MAX", b) )
			{
				b = (b - a) / c; c = pGrid->Get_Min();

				#pragma omp parallel for
				for(sLong i=0; i<Get_NCells(); i++)
				{
					pGrid->Set_Value(i, a + b * (pGrid->asDouble(i) - c));
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
COpenCV_FFT_Filter::COpenCV_FFT_Filter(void)
{
	Set_Name		(_TL("Frequency Domain Filter"));

	Set_Author		("O.Conrad (c) 2023");

	Set_Description	(_TW(
		"The frequency domain filter works on the discrete Fourier transformation."
	));

	Add_Reference("https://docs.opencv.org/4.7.0/de/dbc/tutorial_py_fourier_transform.html",
		SG_T("OpenCV Tutorial | Fourier Transform")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid("", "GRID"    , _TL("Grid"         ), _TL(""), PARAMETER_INPUT );
	Parameters.Add_Grid("", "FILTERED", _TL("Filtered Grid"), _TL(""), PARAMETER_OUTPUT);

	Parameters.Add_Choice("",
		"FILTER", _TL("Filter"),
		_TL(""),
		CSG_String::Format("%s|%s|%s|%s",
			_TL("range"),
			_TL("power of distance"),
			_TL("Hann window"),
			_TL("Gaussian")
		), 2
	);

	Parameters.Add_Bool  ("FILTER", "INVERSE", _TL("Inverse"   ), _TL(""), false);

	Parameters.Add_Range ("FILTER", "RANGE"  , _TL("Range"     ), _TL(""), 0.0, 0.5, 0.0, true, 1.0, true);
	Parameters.Add_Double("FILTER", "POWER"  , _TL("Power"     ), _TL(""), 0.5);
	Parameters.Add_Double("FILTER", "SCALE"  , _TL("Band Width"), _TL(""), 2.0, 0.0, true);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int COpenCV_FFT_Filter::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( pParameter->Cmp_Identifier("FILTER") )
	{
		pParameters->Set_Enabled("RANGE", pParameter->asInt() == 0);
		pParameters->Set_Enabled("POWER", pParameter->asInt() == 1);
		pParameters->Set_Enabled("SCALE", pParameter->asInt() >= 2);
	}

	return( CSG_Tool_Grid::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool COpenCV_FFT_Filter::On_Execute(void)
{
	CSG_Grid *pGrid = Parameters("GRID")->asGrid();

	COpenCV_FFT DFT;

	DFT.Set_Manager(NULL);
	DFT.Set_Parameter("GRID"    , pGrid);
	DFT.Set_Parameter("CENTERED", true);
	DFT.Set_Parameter("SIZE"    , 1);

	if( !DFT.Execute() )
	{
		return( false );
	}

	CSG_Grids &Complex = *DFT.Get_Parameter("DFT_OPT")->asGrids();

	//-----------------------------------------------------
	int    Filter = Parameters("FILTER"   )->asInt();

	bool bInverse = Parameters("INVERSE"  )->asBool();

	double Min    = Parameters("RANGE.MIN")->asDouble();
	double Max    = Parameters("RANGE.MAX")->asDouble();
	double Scale  = Parameters("SCALE"    )->asDouble();
	double Power  = Parameters("POWER"    )->asDouble();

	#pragma omp parallel for
	for(int y=0; y<Complex.Get_NY(); y++)
	{
		double dy = (y - 0.5 * Complex.Get_NY()) / Complex.Get_NY();

		for(int x=0; x<Complex.Get_NX(); x++)
		{
			double dx = (x - 0.5 * Complex.Get_NX()) / Complex.Get_NX(); double d = dx*dx + dy*dy;

			switch( Filter )
			{
			default:
				d = sqrt(d); d = d < Min || d > Max ? 0. : 1.;
				break;

			case  1:
				d = pow(sqrt(d), Power);
				break;

			case  2:
				d = sqrt(d); d = d > Scale ? 0. : 0.5 * (1. + cos(M_PI_360 * sqrt(d) / Scale));
				break;

			case  3:
				d = exp(-0.5 * d / (Scale*Scale));
				break;
			}

			if( bInverse )
			{
				d = 1. - d;
			}

			if( d < 0. ) d = 0.; else if( d > 1. ) d = 1.;

			Complex[0].Mul_Value(x, y, d);
			Complex[1].Mul_Value(x, y, d);
		}
	}

	//-----------------------------------------------------
	CSG_Grid Filtered(Complex.Get_System());

	COpenCV_FFTinv iDFT;

	iDFT.Set_Manager(NULL);
	iDFT.Set_Parameter("REAL"    , Complex.Get_Grid_Ptr(0));
	iDFT.Set_Parameter("IMAG"    , Complex.Get_Grid_Ptr(1));
	iDFT.Set_Parameter("GRID"    , &Filtered);
	iDFT.Set_Parameter("CENTERED", true);

	if( !iDFT.Execute() )
	{
		return( false );
	}

	//-----------------------------------------------------
	delete(&Complex);

	CSG_Grid *pFiltered = Parameters("FILTERED")->asGrid();

	pFiltered->Assign(&Filtered);

	pFiltered->Fmt_Name("%s [%s]", pGrid->Get_Name(), _TL("Frequency Domain Filter"));

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

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
//                     Grid_Calculus                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 Grid_Random_Field.cpp                 //
//                                                       //
//                 Copyright (C) 2005 by                 //
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
#include "Grid_Random_Field.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Random_Field::CGrid_Random_Field(void)
{
	Set_Name		(_TL("Random Field"));

	Set_Author		("O.Conrad (c) 2005");

	Set_Description	(_TW(
		"Create a grid with pseudo-random numbers as grid cell values. "
	));

	//-----------------------------------------------------
	m_Grid_Target.Create(&Parameters, false);

	m_Grid_Target.Add_Grid("OUT_GRID", _TL("Random Field"), false);

	//-----------------------------------------------------
	Parameters.Add_Choice("",
		"METHOD"	, _TL("Method"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("Uniform"),
			_TL("Gaussian")
		), 1
	);

	Parameters.Add_Node("",
		"UNIFORM"	, _TL("Uniform"),
		_TL("")
	);

	Parameters.Add_Range("UNIFORM",
		"RANGE"		, _TL("Range"),
		_TL(""),
		0.0, 1.0
	);

	Parameters.Add_Node("",
		"GAUSSIAN"	, _TL("Gaussian"),
		_TL("")
	);

	Parameters.Add_Double("GAUSSIAN",
		"MEAN"		, _TL("Arithmetic Mean"),
		_TL(""),
		0.0
	);

	Parameters.Add_Double("GAUSSIAN",
		"STDDEV"	, _TL("Standard Deviation"),
		_TL(""),
		1.0, 0.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Random_Field::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "METHOD") )
	{
		pParameters->Set_Enabled("UNIFORM" , pParameter->asInt() == 0);
		pParameters->Set_Enabled("GAUSSIAN", pParameter->asInt() == 1);
	}

	m_Grid_Target.On_Parameters_Enable(pParameters, pParameter);

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Random_Field::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Grid	*pGrid	= m_Grid_Target.Get_Grid("OUT_GRID");

	if( !pGrid )
	{
		return( false );
	}

	//-----------------------------------------------------
	int		Method	= Parameters("METHOD")->asInt();

	double	a	= Method == 0
		? Parameters("RANGE" )->asRange()->Get_LoVal()
		: Parameters("MEAN"  )->asDouble();

	double	b	= Method == 0
		? Parameters("RANGE" )->asRange()->Get_HiVal()
		: Parameters("STDDEV")->asDouble();

	//-----------------------------------------------------
	for(int y=0; y<pGrid->Get_NY() && Set_Progress(y, pGrid->Get_NY()); y++)
	{
		#pragma omp parallel for
		for(int x=0; x<pGrid->Get_NX(); x++)
		{
			switch( Method )
			{
			case 0:	// uniform...
				pGrid->Set_Value(x, y, CSG_Random::Get_Uniform (a, b));
				break;

			case 1:	// gaussian...
				pGrid->Set_Value(x, y, CSG_Random::Get_Gaussian(a, b));
				break;
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
CGrid_Fractal_Brownian_Noise::CGrid_Fractal_Brownian_Noise(void)
{
	Set_Name		(_TL("Fractal Brownian Noise"));

	Set_Author		("A.Jack, O.Conrad (c) 2017");

	Set_Description	(_TW(
		"This tool uses uniform random to create a grid that resembles fractal Brownian noise (FBN). "
		"The advantage of FBN noise is that it appears to have texture to the human eye, that resembles "
		"the types of textures that are observed in nature; terrains, algae growth, clouds, etc. "
		"The degree of texture observed in the FBN grid is dependent upon the sizes of the wavelengths chosen. "
		"The wavelengths should be chosen so they increase in size (a doubling of successive wavelengths "
		"is a good point to start). The greater the magnitude of the \"ramp\" of successive wavelengths the "
		"greater the texture in the FBN grid. "
	));

	//-----------------------------------------------------
	m_Grid_Target.Create(&Parameters, false);

	m_Grid_Target.Add_Grid("OUT_GRID", _TL("Fractal Brownian Noise"), false);

	//-----------------------------------------------------
	Parameters.Add_Choice("",
		"SCALING"	, _TL("Scaling"),
		_TL(""),
		CSG_String::Format("%s|%s|",
			_TL("linear"),
			_TL("geometric")
		), 1
	);

	Parameters.Add_Double("",
		"MAX_SCALE"	, _TL("Maximum Scale"),
		_TL(""),
		1., 0.0, true
	);

	Parameters.Add_Int("",
		"STEPS"		, _TL("Steps"),
		_TL(""),
		8, 1, true
	);

	Parameters.Add_Range("",
		"RANGE"		, _TL("Noise Range"),
		_TL(""),
		-1.0, 1.0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CGrid_Fractal_Brownian_Noise::On_Parameter_Changed(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	if( !SG_STR_CMP(pParameter->Get_Identifier(), "SYSTEM") )
	{
		CSG_Grid_System	System(*pParameter->asGrid_System());

		if( System.is_Valid() )
		{
			double	d	= 0.5 * SG_Get_Length(System.Get_XRange(), System.Get_YRange());

			pParameters->Set_Parameter("MAX_SCALE", System.Get_Cellsize() * (int)(d / System.Get_Cellsize()));
		}
	}

	return( CSG_Tool::On_Parameter_Changed(pParameters, pParameter) );
}

//---------------------------------------------------------
int CGrid_Fractal_Brownian_Noise::On_Parameters_Enable(CSG_Parameters *pParameters, CSG_Parameter *pParameter)
{
	m_Grid_Target.On_Parameters_Enable(pParameters, pParameter);

	return( CSG_Tool::On_Parameters_Enable(pParameters, pParameter) );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Fractal_Brownian_Noise::On_Execute(void)
{
	//-----------------------------------------------------
	CSG_Grid	*pGrid	= m_Grid_Target.Get_Grid("OUT_GRID");

	if( !pGrid )
	{
		return( false );
	}

	pGrid->Assign(0.0);
	pGrid->Set_Name(_TL("Fractal Brownian Noise"));

	int		Scaling		= Parameters("SCALING"  )->asInt   ();
	int		nSteps		= Parameters("STEPS"    )->asInt   ();
	double	maxScale	= Parameters("MAX_SCALE")->asDouble();

	if( maxScale < pGrid->Get_Cellsize() )
	{
		Error_Set(_TL("maximum scale must be greater than the cell size"));

		return( false );
	}

	double	Scale, dScale, Offset;

	Scale	= pGrid->Get_Cellsize();

	switch( Scaling )
	{
	default:	dScale	=        (maxScale - Scale) / nSteps ;	break;	// linear
	case  1:	dScale	= exp(log(maxScale / Scale) / nSteps);	break;	// geometric
	}

	//-----------------------------------------------------
	for(int i=0; i<=nSteps && Set_Progress(i, nSteps); i++)
	{
		Message_Add(CSG_String::Format("\n%s: [%d] %f", _TL("Scale"), i, Scale), false);

		Add_Noise(pGrid, Scale);

		switch( Scaling )
		{
		default:	Scale	+= dScale;	break;	// linear
		case  1:	Scale	*= dScale;	break;	// geometric
		}
	}

	//-----------------------------------------------------
	Offset	= Parameters("RANGE")->asRange()->Get_LoVal();	
	Scale	= Parameters("RANGE")->asRange()->Get_HiVal() - Offset;

	if( Scale <= 0.0 || pGrid->Get_Range() <= 0.0 )
	{
		Error_Set(_TL("grid value and noise range must be greater than zero"));

		return( false );
	}

	Scale	/= pGrid->Get_Range();
	dScale	 = pGrid->Get_Min();

	#pragma omp parallel for
	for(sLong i=0; i<pGrid->Get_NCells(); i++)
	{
		if( pGrid->is_NoData(i) == false )
		{
			pGrid->Set_Value(i, Offset + Scale * (pGrid->asDouble(i) - dScale));
		}
	}

	return( true );
}

//---------------------------------------------------------
bool CGrid_Fractal_Brownian_Noise::Add_Noise(CSG_Grid *pGrid, double Scale)
{
	CSG_Grid	Noise;
	CSG_Rect	Extent(pGrid->Get_Extent(true));

	Extent.Inflate(Scale, false);	// guarantee that the noise grid is always large enough, otherwise we get edge effects with larger scale values

	if( !Noise.Create(CSG_Grid_System(Scale, Extent)) )
	{
		return( false );
	}

	#pragma omp parallel for
	for(sLong i=0; i<Noise.Get_NCells(); i++)
	{
		Noise.Set_Value(i, CSG_Random::Get_Uniform(0, 1));
	}

	#pragma omp parallel for
	for(int y=0; y<pGrid->Get_NY(); y++)
	{
		TSG_Point	p;

		p.y	= pGrid->Get_YMin() + y * pGrid->Get_Cellsize();
		p.x	= pGrid->Get_XMin();

		for(int x=0; x<pGrid->Get_NX(); x++, p.x+=pGrid->Get_Cellsize())
		{
			if( pGrid->is_NoData(x, y) == false )
			{
				pGrid->Add_Value(x, y, Noise.Get_Value(p));
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

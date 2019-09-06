
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//          diffusion_gradient_concentration.cpp         //
//                                                       //
//                Copyright (C) 2007 by                  //
//                O.Conrad, R.Heinrich                   //
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
// Public License along with this program; if not,       //
// write to the Free Software Foundation, Inc.,          //
// 59 Temple Place - Suite 330, Boston, MA 02111-1307,   //
// USA.                                                  //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//    contact:    Ralph Heinrich                         //
//                                                       //
//    e-mail:     heinrich-ralph@web.de                  //
//                                                       //
//    phone:      +49-35603-152006                       //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "diffusion_gradient_concentration.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define MASK_LAKE	1
#define MASK_INLET	2
#define MASK_OUTLET	3

//---------------------------------------------------------
static const CSG_String	Description	= _TW(
	"Cellular automata are simple computational operators, but despite their simplicity, "
	"they allow the simulation of highly complex processes. This tool has been created to "
	"apply the concept of cellular automata to simulate diffusion and flow processes in "
	"shallow water bodies with in- and outflow, where monitoring data show concentration "
	"growth or decrease between the inflow and the outflow points. Parameters are for "
	"example nutrients like nitrate, which is reduced by denitrification process inside "
	"the water body.\n"
	"Values of mask grid are expected to be 1 for water area, 2 for inlet, 3 for outlet and "
	"0 for non water."
);

#define ADD_REFERENCE	Add_Reference("Heinrich, R. & Conrad, O.", "2008",\
	"Diffusion, Flow and Concentration Gradient Simulation with SAGA GIS using Cellular Automata Methods",\
	"In: Boehner, J., Blaschke, T., Montanarella, L. [Eds.]: SAGA - Seconds Out. Hamburger Beitraege zur Physischen Geographie und Landschaftsoekologie, Vol.19, p59-70.",\
	SG_T("http://downloads.sourceforge.net/saga-gis/hbpl19_07.pdf")\
);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CSim_Diffusion_Gradient::CSim_Diffusion_Gradient(void)
{
	//-----------------------------------------------------
	Set_Name		(_TL("Surface and Gradient"));

	Set_Author		("R.Heinrich, O.Conrad (c) 2007");

	Set_Description	(Description);

	ADD_REFERENCE

	//-----------------------------------------------------
	Parameters.Add_Grid(
		"", "MASK"		, _TL("Mask"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		"", "SURF"		, _TL("Surface"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		"", "GRAD"		, _TL("Gradient"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	//-----------------------------------------------------
	Parameters.Add_Double(
		"", "SURF_E"	, _TL("Surface Approximation Threshold"),
		_TL(""),
		0.001, 0.0, true
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSim_Diffusion_Gradient::On_Execute(void)
{
	m_pMask	= Parameters("MASK")->asGrid();

	CSG_Grid	*pSurface	= Parameters("SURF")->asGrid();
	CSG_Grid	*pGradient	= Parameters("GRAD")->asGrid();

	m_Tmp.Create(Get_System());

	//-----------------------------------------------------
	bool	bResult	= Surface_Initialise(pSurface);

	if( bResult )
	{
		Surface_Interpolate (pSurface);
		Surface_Get_Gradient(pSurface, pGradient);
	}

	//-----------------------------------------------------
	m_Tmp.Destroy();

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline bool CSim_Diffusion_Gradient::is_Lake(int x, int y)
{
	if( is_InGrid(x, y) )
	{
		int	Mask	= m_pMask->asInt(x, y);

		return( Mask == MASK_LAKE || Mask == MASK_INLET || Mask == MASK_OUTLET );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSim_Diffusion_Gradient::Surface_Initialise(CSG_Grid *pSurface)
{
	int		nIn = 0, nOut = 0;

	for(int y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			switch( m_pMask->asInt(x, y) )
			{
			case MASK_INLET : nIn++;  pSurface->Set_Value (x, y, 100.0); break;
			case MASK_LAKE  :         pSurface->Set_Value (x, y,  50.0); break;
			case MASK_OUTLET: nOut++; pSurface->Set_Value (x, y,   0.0); break;
			default         :         pSurface->Set_NoData(x, y       ); break;
			}
		}
	}

	return( nIn > 0 && nOut > 0 );
}

//---------------------------------------------------------
bool CSim_Diffusion_Gradient::Surface_Interpolate(CSG_Grid *pSurface)
{
	int		n, nMax;
	double	d, dEpsilon;

	nMax		= 100000;
	dEpsilon	= Parameters("SURF_E")->asDouble();

	DataObject_Update(pSurface, true);

	for(n=0, d=Surface_Set_Means(pSurface); n<nMax && d>dEpsilon && Process_Get_Okay(false); n++)
	{
		d	= Surface_Set_Means(pSurface);

		Process_Set_Text(SG_T("%d, %f"), n + 1, d);

		if( n % 25 == 0 )	DataObject_Update(pSurface, 0.0, 100.0);
	}

	Message_Fmt("\n%d iterations", n);

	return( true );
}

//---------------------------------------------------------
double CSim_Diffusion_Gradient::Surface_Set_Means(CSG_Grid *pSurface)
{
	int		y, in	= 1;

	//-----------------------------------------------------
	#pragma omp parallel for private(y)
	for(y=0; y<Get_NY(); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( is_Lake(x, y) )
			{
				CSG_Simple_Statistics	s;

				for(int iy=y-in; iy<=y+in; iy++)
				{
					for(int ix=x-in; ix<=x+in; ix++)
					{
						if( is_Lake(ix, iy) )
						{
							s	+= pSurface->asDouble(ix, iy);
						}
					}
				}

				m_Tmp.Set_Value(x, y, s.Get_Mean());
			}
		}
	}

	//-----------------------------------------------------
	double	dMax	= 0.0;

	for(y=0; y<Get_NY(); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			switch( m_pMask->asInt(x, y) )
			{
			case MASK_INLET :	pSurface->Set_Value(x, y, 100.0);	break;
			case MASK_OUTLET:	pSurface->Set_Value(x, y,   0.0);	break;
			case MASK_LAKE  :
				{
					double	s	= m_Tmp.asDouble(x, y);
					double	d	= fabs(pSurface->asDouble(x, y) - s);

					if( d > 0.0 )
					{
						if( dMax <= 0.0 || d > dMax )
						{
							dMax	= d;
						}

						pSurface->Set_Value(x, y, s);
					}
				}
				break;
			}
		}
	}

	return( dMax );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSim_Diffusion_Gradient::Surface_Get_Gradient(CSG_Grid *pSurface, CSG_Grid *pGradient)
{
	#pragma omp parallel for
	for(int y=0; y<Get_NY(); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( is_Lake(x, y) )
			{
				double	z, zMin, zMax	= pSurface->asDouble(x, y);	zMin = zMax;

				for(int i=0; i<8; i++)
				{
					int	ix	= Get_xTo(i, x);
					int	iy	= Get_yTo(i, y);

					if( is_Lake(ix, iy) )
					{
						if( zMin > (z = pSurface->asDouble(ix, iy)) )
						{
							zMin	= z;
						}
						else if( zMax < z )
						{
							zMax	= z;
						}
					}
				}

				switch( m_pMask->asInt(x, y) )
				{
				case MASK_INLET :
				case MASK_LAKE  :	pGradient->Set_Value(x, y,  zMax - zMin       );	break;
				case MASK_OUTLET:	pGradient->Set_Value(x, y, (zMax - zMin) * 2.0);	break;
				}
			}
			else
			{
				pGradient->Set_NoData(x, y);
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
CSim_Diffusion_Concentration::CSim_Diffusion_Concentration(void)
{
	Parameters.Create("", SG_T(""), SG_T(""), SG_T(""), true);

	//-----------------------------------------------------
	Set_Name		(_TL("Concentration"));

	Set_Author		("R.Heinrich, O.Conrad (c) 2007");

	Set_Description	(Description);

	ADD_REFERENCE

	//-----------------------------------------------------
	Parameters.Add_Grid(
		"", "MASK"		, _TL("Mask"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		"", "GRAD"		, _TL("Gradient"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		"", "CONC"		, _TL("Concentration"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	//-----------------------------------------------------
	Parameters.Add_Double(
		"", "CONC_IN"	, _TL("Inlet Concentration"),
		_TL(""),
		 5.0, 0.0, true
	);

	Parameters.Add_Double(
		"", "CONC_OUT"	, _TL("Outlet Concentration"),
		_TL(""),
		3.0, 0.0, true
	);

	Parameters.Add_Double(
		"", "CONC_E"	, _TL("Concentration Approximation Threshold"),
		_TL(""),
		0.001, 0.0, true
	);

	Parameters.Add_Double(
		"", "GRAD_MIN"	, _TL("Minimum Gradient"),
		_TL(""),
		0.0, 0.0, true
	);

	Parameters.Add_Choice(
		"", "NEIGHBOURS", _TL("Neighbourhood"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("Moore (8)"),
			_TL("Neumann (4)"),
			_TL("Optimised")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSim_Diffusion_Concentration::On_Execute(void)
{
	m_pMask	= Parameters("MASK")->asGrid();

	CSG_Grid	*pGradient		= Parameters("GRAD")->asGrid();
	CSG_Grid	*pConcentration	= Parameters("CONC")->asGrid();

	m_Conc_In		= Parameters("CONC_IN" )->asDouble();
	m_Conc_Out		= Parameters("CONC_OUT")->asDouble();
	m_MinGradient	= Parameters("GRAD_MIN")->asDouble();

	//-----------------------------------------------------
	m_Tmp.Create(Get_System());

	Concentration_Interpolate(pConcentration, pGradient);

	m_Tmp.Destroy();

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSim_Diffusion_Concentration::Concentration_Interpolate(CSG_Grid *pConcentration, CSG_Grid *pGradient)
{
	switch( Parameters("NEIGHBOURS") ? Parameters("NEIGHBOURS")->asInt() : 2 )
	{
	case 0:	// Moore
		{
			_Concentration_Interpolate	(pConcentration, pGradient, false);
		}
		break;

	case 1:	// Neumann
		{
			_Concentration_Interpolate	(pConcentration, pGradient, true);
		}
		break;

	case 2:	// Optimised
		{
			_Concentration_Interpolate	(pConcentration, pGradient, false);

			CSG_Grid	Concentration(*pConcentration);

			_Concentration_Interpolate	(pConcentration, pGradient, true);

			for(int y=0; y<Get_NY() && Set_Progress(y); y++)
			{
				for(int x=0; x<Get_NX(); x++)
				{
					if( pConcentration->is_NoData(x, y) || Concentration.is_NoData(x, y) )
					{
						pConcentration->Set_NoData(x, y);
					}
					else
					{
						pConcentration->Set_Value(x, y, (pConcentration->asDouble(x, y) + Concentration.asDouble(x, y)) / 2.0);
					}
				}
			}
		}
		break;
	}

	return( true );
}

//---------------------------------------------------------
bool CSim_Diffusion_Concentration::_Concentration_Interpolate(CSG_Grid *pConcentration, CSG_Grid *pGradient, bool bNeumann)
{
	double	d, d_lo, d_hi, f, f_lo, f_hi, d_Max;

	DataObject_Update(pConcentration, true);

	d_Max	= Parameters("CONC_E")->asDouble();

	d_lo	= _Concentration_Interpolate(pConcentration, pGradient, bNeumann, f_lo = 0.0);
	d_hi	= _Concentration_Interpolate(pConcentration, pGradient, bNeumann, f_hi = 0.01);

	while( d_hi > m_Conc_Out && Process_Get_Okay(false) )
	{
		f_hi	*= 10.0;
		d_hi	= _Concentration_Interpolate(pConcentration, pGradient, bNeumann, f_hi);
	}

	do
	{
		d	= _Concentration_Interpolate(pConcentration, pGradient, bNeumann, f = f_lo + 0.5 * (f_hi - f_lo));

		Process_Set_Text("f: %f, AK: %f, dif: %f", f, d, m_Conc_Out - d);
		Message_Fmt		("f: %f, AK: %f, dif: %f", f, d, m_Conc_Out - d);

		DataObject_Update(pConcentration, m_Conc_Out, m_Conc_In);

		if( fabs(d - m_Conc_Out) > d_Max )
		{
			if(      SG_IS_BETWEEN(d_lo, m_Conc_Out, d) )
			{
				f_hi	= f;
				d_hi	= d;
			}
			else if( SG_IS_BETWEEN(d_hi, m_Conc_Out, d) )
			{
				f_lo	= f;
				d_lo	= d;
			}
			else
			{
				return( false );
			}
		}
	}
	while( fabs(d - m_Conc_Out) > d_Max && f_hi > f_lo && Process_Get_Okay(false) );

	Message_Fmt("\nf: %f", f);

	//-----------------------------------------------------
	_Concentration_Initialise(pConcentration);

	for(int nChanges=1; nChanges>0 && Process_Get_Okay(false); )
	{
		nChanges	= _Concentration_Set_Means(pConcentration, pGradient, bNeumann, f, d);
	}

	return( true );
}

//---------------------------------------------------------
double CSim_Diffusion_Concentration::_Concentration_Interpolate(CSG_Grid *pConcentration, CSG_Grid *pGradient, bool bNeumann, double f)
{
	double	Conc_Out	= 0.0;

	_Concentration_Initialise(pConcentration);

	for(int nChanges=1; Conc_Out<=0.0 && nChanges>0 && Process_Get_Okay(false); )
	{
		nChanges	= _Concentration_Set_Means(pConcentration, pGradient, bNeumann, f, Conc_Out);
	}

	return( Conc_Out );
}

//---------------------------------------------------------
int CSim_Diffusion_Concentration::_Concentration_Set_Means(CSG_Grid *pConcentration, CSG_Grid *pGradient, bool bNeumann, double f, double &Conc_Out)
{
	int		y, n, iStep = bNeumann ? 2 : 1;

	//-----------------------------------------------------
	#pragma omp parallel for private(y)
	for(y=0; y<Get_NY(); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			if( is_Lake(x, y) && pConcentration->asDouble(x, y) == 0.0 )
			{
				double	d, dMax	= 0.0;

				for(int i=0; i<8; i+=iStep)
				{
					int	ix	= Get_xTo(i, x);
					int	iy	= Get_yTo(i, y);

					if( is_Lake(ix, iy) && dMax < (d = pConcentration->asDouble(ix, iy)) )
					{
						dMax	= d;
					}
				}

				if( dMax > 0.0 )
				{
					if( (d = pGradient->asDouble(x, y)) < m_MinGradient )
					{
						d	= m_MinGradient;
					}

					m_Tmp.Set_Value(x, y, dMax / (1.0 + (f / d)));
				}
			}
		}
	}

	//-----------------------------------------------------
	for(y=0, n=0, Conc_Out=0.0; y<Get_NY(); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			double	d;

			switch( m_pMask->asInt(x, y) )
			{
			case MASK_INLET:
				pConcentration->Set_Value(x, y, m_Conc_In);
				break;

			case MASK_OUTLET:
				if( pConcentration->asDouble(x, y) == 0.0 && (d = m_Tmp.asDouble(x, y)) > 0.0 )
				{
					pConcentration->Set_Value(x, y, Conc_Out = d);
					n++;
				}
				break;

			case MASK_LAKE:
				if( pConcentration->asDouble(x, y) == 0.0 && (d = m_Tmp.asDouble(x, y)) > 0.0 )
				{
					pConcentration->Set_Value(x, y, d);
					n++;
				}
				break;
			}
		}
	}

	return( n );
}

//---------------------------------------------------------
bool CSim_Diffusion_Concentration::_Concentration_Initialise(CSG_Grid *pConcentration)
{
	m_Tmp.Assign(0.0);

	#pragma omp parallel for
	for(int y=0; y<Get_NY(); y++)
	{
		for(int x=0; x<Get_NX(); x++)
		{
			switch( m_pMask->asInt(x, y) )
			{
			case MASK_LAKE  :	pConcentration->Set_Value (x, y, 0.0      );	break;
			case MASK_OUTLET:	pConcentration->Set_Value (x, y, 0.0      );	break;
			case MASK_INLET :	pConcentration->Set_Value (x, y, m_Conc_In);	break;
			default         :	pConcentration->Set_NoData(x, y           );	break;
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
CSim_Diffusion_Gradient_And_Concentration::CSim_Diffusion_Gradient_And_Concentration(void)
{
	Parameters.Create("", SG_T(""), SG_T(""), SG_T(""), true);

	//-----------------------------------------------------
	Set_Name		(_TL("Surface, Gradient and Concentration"));

	Set_Author		("R.Heinrich, O.Conrad (c) 2007");

	Set_Description	(Description);

	ADD_REFERENCE

	//-----------------------------------------------------
	Parameters.Add_Grid(
		"", "MASK"		, _TL("Mask"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		"", "SURF"		, _TL("Surface"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		"", "GRAD"		, _TL("Gradient"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Grid(
		"", "CONC"		, _TL("Concentration"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	//-----------------------------------------------------
	Parameters.Add_Double(
		"", "SURF_E"	, _TL("Surface Approximation Threshold"),
		_TL(""),
		0.001, 0.0, true
	);

	Parameters.Add_Double(
		"", "CONC_IN"	, _TL("Inlet Concentration"),
		_TL(""),
		5.0, 0.0, true
	);

	Parameters.Add_Double(
		"", "CONC_OUT"	, _TL("Outlet Concentration"),
		_TL(""),
		3.0, 0.0, true
	);

	Parameters.Add_Double(
		"", "CONC_E"	, _TL("Concentration Approximation Threshold"),
		_TL(""),
		0.001, 0.0, true
	);

	Parameters.Add_Double(
		"", "GRAD_MIN"	, _TL("Minimum Gradient"),
		_TL(""),
		0.0, 0.0, true
	);

	Parameters.Add_Choice(
		"", "NEIGHBOURS", _TL("Neighbourhood"),
		_TL(""),
		CSG_String::Format("%s|%s|%s",
			_TL("Moore (8)"),
			_TL("Neumann (4)"),
			_TL("Optimised")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CSim_Diffusion_Gradient_And_Concentration::On_Execute(void)
{
	m_pMask	= Parameters("MASK")->asGrid();

	CSG_Grid	*pSurface		= Parameters("SURF")->asGrid();
	CSG_Grid	*pGradient		= Parameters("GRAD")->asGrid();
	CSG_Grid	*pConcentration	= Parameters("CONC")->asGrid();

	m_Conc_In		= Parameters("CONC_IN" )->asDouble();
	m_Conc_Out		= Parameters("CONC_OUT")->asDouble();
	m_MinGradient	= Parameters("GRAD_MIN")->asDouble();

	m_Tmp.Create(Get_System());

	//-----------------------------------------------------
	bool	bResult	= Surface_Initialise(pSurface);

	if( bResult )
	{
		Surface_Interpolate (pSurface);
		Surface_Get_Gradient(pSurface, pGradient);

		Concentration_Interpolate(pConcentration, pGradient);
	}

	//-----------------------------------------------------
	m_Tmp.Destroy();

	return( bResult );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

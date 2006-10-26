
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                       Fractals                        //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    Mandelbrot.cpp                     //
//                                                       //
//                 Copyright (C) 2003 by                 //
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

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Mandelbrot.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CMandelbrot::CMandelbrot(void)
{
	//-----------------------------------------------------
	// 1. Info...

	Set_Name(_TL("Mandelbrot Set"));

	Set_Author(_TL("Copyrights (c) 2001 by Olaf Conrad"));

	Set_Description(_TL(
		"Calculates Mandelbrot and Julia sets.\n\n"
		"References:\n"
		"- Mandelbrot, B.B. (1983): 'The Fractal Geometry of Nature', New York, 490p.\n")
	);


	//-----------------------------------------------------
	// 2. Grids...

	Parameters.Add_Grid_Output(
		NULL	, "GRID"	, _TL("Grid"),
		""
	);

	Parameters.Add_Value(
		NULL	, "NX"		, _TL("Width (Cells)"),
		"",
		PARAMETER_TYPE_Int, 100, 1, true
	);

	Parameters.Add_Value(
		NULL	, "NY"		, _TL("Height (Cells)"),
		"", PARAMETER_TYPE_Int, 100, 1, true
	);

	Parameters.Add_Range(
		NULL	, "XRANGE"	, _TL("X-Range"),
		"", -2.0, 1.0
	);

	Parameters.Add_Range(
		NULL	, "YRANGE"	, _TL("Y-Range"),
		"",
		-1.5, 1.5
	);

	Parameters.Add_Value(
		NULL	, "JULIA_X"	, _TL("Julia - X"),
		"", PARAMETER_TYPE_Double, -0.7
	);

	Parameters.Add_Value(
		NULL	, "JULIA_Y"	, _TL("Julia - Y"),
		"", PARAMETER_TYPE_Double,  0.3
	);

	Parameters.Add_Value(
		NULL	, "MAXITER"	, _TL("Maximum Iterations"),
		"",
		PARAMETER_TYPE_Int, 300, 1, true
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"	, _TL("Fractal Type"),
		"",

		"Mandelbrot|"
		"Julia|"
	);
}

//---------------------------------------------------------
CMandelbrot::~CMandelbrot(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CMandelbrot::On_Execute(void)
{
	//-----------------------------------------------------
	m_Extent.Assign(
		Parameters("XRANGE")->asRange()->Get_LoVal(),
		Parameters("YRANGE")->asRange()->Get_LoVal(),
		Parameters("XRANGE")->asRange()->Get_HiVal(),
		Parameters("YRANGE")->asRange()->Get_HiVal()
	);

	m_maxIterations	= Parameters("MAXITER")	->asInt();
	m_maxDistance	= 4.0;

	m_Method		= Parameters("METHOD")	->asInt();

	m_xJulia		= Parameters("JULIA_X")	->asDouble();
	m_yJulia		= Parameters("JULIA_Y")	->asDouble();

	m_pGrid			= SG_Create_Grid(GRID_TYPE_Int, Parameters("NX")->asInt(), Parameters("NY")->asInt());
	m_pGrid->Set_Name(m_Method == 0 ? _TL("Mandelbrot Set") : _TL("Julia Set"));
	Parameters("GRID")->Set_Value(m_pGrid);

	//-----------------------------------------------------
	Calculate();

	//-----------------------------------------------------
	return( true );
}

//---------------------------------------------------------
#define GET_POS(p)		p.Assign(\
						m_Extent.Get_XMin() + m_Extent.Get_XRange() * (ptWorld.Get_X() - m_pGrid->Get_XMin()) / m_pGrid->Get_XRange(),\
						m_Extent.Get_YMin() + m_Extent.Get_YRange() * (ptWorld.Get_Y() - m_pGrid->Get_YMin()) / m_pGrid->Get_YRange());

#define SET_POS(a, b)	if( a.Get_X() > b.Get_X() )	{	d	= a.m_point.x; a.m_point.x	= b.m_point.x; b.m_point.x	= d;	}\
						if( a.Get_Y() > b.Get_Y() )	{	d	= a.m_point.y; a.m_point.y	= b.m_point.y; b.m_point.y	= d;	}

//---------------------------------------------------------
bool CMandelbrot::On_Execute_Position(CSG_Point ptWorld, TSG_Module_Interactive_Mode Mode)
{
	double	d;

	switch( Mode )
	{
	default:
		break;

	case MODULE_INTERACTIVE_LDOWN:
	case MODULE_INTERACTIVE_RDOWN:
		GET_POS(m_Down);

		return( true );

	case MODULE_INTERACTIVE_LUP:
		GET_POS(m_Up);
		SET_POS(m_Up, m_Down);

		if( m_Up.Get_X() >= m_Down.Get_X() || m_Up.Get_Y() >= m_Down.Get_Y() )
		{
			m_Extent.Inflate(50.0);
			m_Extent.Move(m_Up - m_Extent.Get_Center());
		}
		else
		{
			m_Extent.Assign(m_Up, m_Down);
		}

		Calculate();

		return( true );

	case MODULE_INTERACTIVE_RUP:
		GET_POS(m_Up);
		SET_POS(m_Up, m_Down);

		if( m_Up.Get_X() >= m_Down.Get_X() || m_Up.Get_Y() >= m_Down.Get_Y() )
		{
			m_Extent.Deflate(50.0);
			m_Extent.Move(m_Up - m_Extent.Get_Center());
		}
		else
		{
			m_Extent.Deflate(100.0 * (m_Down.Get_X() - m_Up.Get_X()) / m_Extent.Get_XRange());
			m_Extent.Move(m_Up - m_Extent.Get_Center());
		}

		Calculate();

		return( true );
	}

	return( false );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CMandelbrot::Calculate(void)
{
	int		x, y, i;
	double	xPos, yPos, dx, dy;

	dx	= m_Extent.Get_XRange() / (m_pGrid->Get_NX() - 1.0);
	dy	= m_Extent.Get_YRange() / (m_pGrid->Get_NY() - 1.0);

	for(y=0, yPos=m_Extent.Get_YMin(); y<m_pGrid->Get_NY() && Set_Progress(y, m_pGrid->Get_NY()); y++, yPos+=dy)
	{
		for(x=0, xPos=m_Extent.Get_XMin(); x<m_pGrid->Get_NX(); x++, xPos+=dx)
		{
			switch( m_Method )
			{
			default:
			case 0:	i	= Get_Mandelbrot	(xPos, yPos);	break;
			case 1:	i	= Get_Julia			(xPos, yPos);	break;
			}

			if( i >= m_maxIterations )
			{
				m_pGrid->Set_NoData(x, y);
			}
			else
			{
				m_pGrid->Set_Value(x, y, i);
			}
		}
	}

	DataObject_Update(m_pGrid, true);
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CMandelbrot::Get_Mandelbrot(double xPos, double yPos)
{
	int		i;
	double	x, y, k;

	for(i=0, x=0.0, y=0.0; i<m_maxIterations; i++)
	{
		k	= xPos + x*x - y*y;
		y	= yPos + 2.0 * x * y;
		x	= k;

		if( m_maxDistance < x*x + y*y )
		{
			return( i );
		}
	}

	return( i );
}

//---------------------------------------------------------
int CMandelbrot::Get_Julia(double xPos, double yPos)
{
	int		i;
	double	x, y, k;

	for(i=0, x=xPos, y=yPos; i<m_maxIterations; i++)
	{
		k	= m_xJulia + x*x - y*y;
		y	= m_yJulia + 2.0 * x * y;
		x	= k;

		if( m_maxDistance < x*x + y*y )
		{
			return( i );
		}
	}

	return( i );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

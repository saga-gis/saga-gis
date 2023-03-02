
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
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
#include "Mandelbrot.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CMandelbrot::CMandelbrot(void)
{
	Set_Name		(_TL("Mandelbrot Set"));

	Set_Author		("O.Conrad (c) 2001");

	Set_Description	(_TW(
		"Calculates Mandelbrot and Julia sets."
	));

	Add_Reference("Mandelbrot, B.B.", "1983",
		"The Fractal Geometry of Nature",
		"New York, 490p."
	);

	//-----------------------------------------------------
	Parameters.Add_Grid_Output("", "GRID"   , _TL("Mandelbrot Set"    ), _TL(""));

	Parameters.Add_Int        ("", "NX"     , _TL("Width"             ), _TL("Cells"), 100, 1, true);
	Parameters.Add_Int        ("", "NY"     , _TL("Height"            ), _TL("Cells"), 100, 1, true);

	Parameters.Add_Range      ("", "XRANGE" , _TL("X-Range"           ), _TL(""), -2.0, 1.0);
	Parameters.Add_Range      ("", "YRANGE" , _TL("Y-Range"           ), _TL(""), -1.5, 1.5);

	Parameters.Add_Double     ("", "JULIA_X", _TL("Julia - X"         ), _TL(""), -0.7);
	Parameters.Add_Double     ("", "JULIA_Y", _TL("Julia - Y"         ), _TL(""),  0.3);

	Parameters.Add_Int        ("", "MAXITER", _TL("Maximum Iterations"), _TL(""), 300, 1, true);

	Parameters.Add_Choice     ("", "METHOD" , _TL("Type"), _TL(""),
		CSG_String::Format("%s|%s",
			SG_T("Mandelbrot"),
			SG_T("Julia")
		), 0
	);
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CMandelbrot::On_Execute(void)
{
	m_Extent.Assign(
		Parameters("XRANGE")->asRange()->Get_Min(),
		Parameters("YRANGE")->asRange()->Get_Min(),
		Parameters("XRANGE")->asRange()->Get_Max(),
		Parameters("YRANGE")->asRange()->Get_Max()
	);

	m_maxIterations = Parameters("MAXITER")->asInt();
	m_maxDistance   = 4.;

	m_Method        = Parameters("METHOD" )->asInt();

	m_xJulia        = Parameters("JULIA_X")->asDouble();
	m_yJulia        = Parameters("JULIA_Y")->asDouble();

	m_pGrid         = SG_Create_Grid(SG_DATATYPE_Int, Parameters("NX")->asInt(), Parameters("NY")->asInt());
	m_pGrid->Set_Name(m_Method == 0 ? _TL("Mandelbrot Set") : _TL("Julia Set"));
	Parameters("GRID")->Set_Value(m_pGrid);

	//-----------------------------------------------------
	Calculate();

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define GET_POS(p)		p.Assign(\
						m_Extent.Get_XMin() + m_Extent.Get_XRange() * (ptWorld.Get_X() - m_pGrid->Get_XMin()) / m_pGrid->Get_XRange(),\
						m_Extent.Get_YMin() + m_Extent.Get_YRange() * (ptWorld.Get_Y() - m_pGrid->Get_YMin()) / m_pGrid->Get_YRange());

#define SET_POS(a, b)	if( a.Get_X() > b.Get_X() )	{	d	= a.Get_X(); a.Set_X(b.Get_X()); b.Set_X(d);	}\
						if( a.Get_Y() > b.Get_Y() )	{	d	= a.Get_Y(); a.Set_Y(b.Get_Y()); b.Set_Y(d);	}

//---------------------------------------------------------
bool CMandelbrot::On_Execute_Position(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode)
{
	double	d;

	switch( Mode )
	{
	default:
		break;

	case TOOL_INTERACTIVE_LDOWN:
	case TOOL_INTERACTIVE_RDOWN:
		GET_POS(m_Down);

		return( true );

	case TOOL_INTERACTIVE_LUP:
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

	case TOOL_INTERACTIVE_RUP:
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
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CMandelbrot::Calculate(void)
{
	double dx = m_Extent.Get_XRange() / (m_pGrid->Get_NX() - 1.);
	double dy = m_Extent.Get_YRange() / (m_pGrid->Get_NY() - 1.);

	for(int y=0; y<m_pGrid->Get_NY() && Set_Progress_Rows(y); y++)
	{
		double yPos = m_Extent.Get_YMin() + y * dy;

		#pragma omp parallel for
		for(int x=0; x<m_pGrid->Get_NX(); x++)
		{
			double xPos = m_Extent.Get_XMin() + x * dx; int i;

			switch( m_Method )
			{
			default: i = Get_Mandelbrot(xPos, yPos); break;
			case  1: i = Get_Julia     (xPos, yPos); break;
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
///////////////////////////////////////////////////////////

//---------------------------------------------------------
int CMandelbrot::Get_Mandelbrot(double xPos, double yPos)
{
	double xx, x = 0., y = 0.;

	for(int i=0; i<m_maxIterations; i++)
	{
		xx = xPos + x*x - y*y;
		y  = yPos + 2.0 * x*y; x = xx;

		if( m_maxDistance < x*x + y*y )
		{
			return( i );
		}
	}

	return( m_maxIterations );
}

//---------------------------------------------------------
int CMandelbrot::Get_Julia(double xPos, double yPos)
{
	double	xx, x = xPos, y = yPos;

	for(int i=0; i<m_maxIterations; i++)
	{
		xx = m_xJulia + x*x - y*y;
		y  = m_yJulia + 2.0 * x*y; x = xx;

		if( m_maxDistance < x*x + y*y )
		{
			return( i );
		}
	}

	return( m_maxIterations );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

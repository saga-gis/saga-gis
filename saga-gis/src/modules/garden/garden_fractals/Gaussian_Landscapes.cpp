/**********************************************************
 * Version $Id: Gaussian_Landscapes.cpp 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

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
//                Gaussian_Landscapes.cpp                //
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

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "Gaussian_Landscapes.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#define RANDOM(x)		(rand() % x)
#define RANDOMIZE()		(srand((unsigned int)time(NULL)))


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGaussian_Landscapes::CGaussian_Landscapes(void)
{
	//-----------------------------------------------------
	// 1. Info...

	Set_Name(_TL("Gaussian Landscapes"));

	Set_Author		(SG_T("(c) 2005 by O.Conrad"));

	Set_Description	(_TW(
		"Generates Gaussian landscapes.\n\n"
		"References:\n"
		"- Halling, H., Moeller, R. (1995): 'Mathematik fuers Auge', Heidelberg, 144p.\n"
		"- Mandelbrot, B.B. (1983): 'The Fractal Geometry of Nature', New York, 490p.\n")
	);


	//-----------------------------------------------------
	// 2. Grids...

	Parameters.Add_Grid_Output(
		NULL	, "GRID"	, _TL("Grid"),
		_TL("")
	);

	Parameters.Add_Value(
		NULL	, "NX"		, _TL("Width (Cells)"),
		_TL(""),
		PARAMETER_TYPE_Int, 100, 1, true
	);

	Parameters.Add_Value(
		NULL	, "NY"		, _TL("Height (Cells)"),
		_TL(""),
		PARAMETER_TYPE_Int, 100, 1, true
	);

	Parameters.Add_Value(
		NULL	, "H"		, _TL("Roughness/Smoothness"),
		_TL(""),
		PARAMETER_TYPE_Double, 0.75, 0.0, true, 1.0, true
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"	, _TL("Method"),
		_TL(""),

		CSG_String::Format(SG_T("%s|%s|"),
			_TL("Simple"),
			_TL("Flattening")
		), 0
	);

	Parameters.Add_Value(
		NULL	, "M"		, _TL("Flattening"),
		_TL(""),
		PARAMETER_TYPE_Double, 2.0
	);
}

//---------------------------------------------------------
CGaussian_Landscapes::~CGaussian_Landscapes(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGaussian_Landscapes::On_Execute(void)
{
	int		nx, ny, n;
	double	h, s, r;

	//-----------------------------------------------------
	nx			= Parameters("NX")->asInt();
	ny			= Parameters("NY")->asInt();

	m_pGrid		= SG_Create_Grid(SG_DATATYPE_Float, nx, ny, 1.0, 0.0, 0.0);
	m_pGrid->Set_Name(_TL("Gaussian Landscape"));
	m_pGrid->Assign_NoData();
	Parameters("GRID")->Set_Value(m_pGrid);

	m_Method	= Parameters("METHOD")	->asInt();
	m_M			= Parameters("M")		->asDouble();

	//-----------------------------------------------------
	nx			= nx > ny ? nx : ny;
	ny			= 1;
	do	{	n	= (int)pow(2.0, ny++);	}	while( n < nx );

	h			= Parameters("H")		->asDouble();
	r			= 1.0 / pow(2.0, h);
	s			= n * r;

	//-----------------------------------------------------
	Set_Values(0, 0, n, n, 0.0, 0.0, 0.0, 0.0, s, r);

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
inline void CGaussian_Landscapes::Set_Value(int x, int y, double Value)
{
	if( m_pGrid->is_InGrid(x, y, false) )
	{
		switch( m_Method )
		{
		case 0:	default:
			m_pGrid->Set_Value(x, y, Value);
			break;

		case 1:
			m_pGrid->Set_Value(x, y, pow(Value, m_M));
			break;
		}
	}
}

//---------------------------------------------------------
void CGaussian_Landscapes::Set_Values(int x_0, int y_0, int x_1, int y_1, double z_00, double z_10, double z_11, double z_01, double s, double r)
{
	int		x_n, y_n;
	double	z_n, z_n1, z_n2, z_n3, z_n4;

	//-----------------------------------------------------
	x_n		= (x_0 + x_1) / 2;
	y_n		= (y_0 + y_1) / 2;
	z_n		= (z_00 + z_10 + z_11 + z_01) / 4.0 + s * (double)(RANDOM(17) - 8) / 8.0;

	Set_Value(x_n, y_n, z_n);

	//-----------------------------------------------------
	if( x_0 != x_n && x_n != x_1 )
	{
		z_n1	= (z_00 + z_10) / 2.0;
		z_n2	= (z_10 + z_11) / 2.0;
		z_n3	= (z_11 + z_01) / 2.0;
		z_n4	= (z_01 + z_00) / 2.0;

		s		*= r;

		Set_Values(x_n, y_0, x_1, y_n, z_n1, z_10, z_n2, z_n , s, r);
		Set_Values(x_0, y_0, x_n, y_n, z_00, z_n1, z_n , z_n4, s, r);
		Set_Values(x_n, y_n, x_1, y_1, z_n , z_n2, z_11, z_n3, s, r);
		Set_Values(x_0, y_n, x_n, y_1, z_n4, z_n , z_n3, z_01, s, r);
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

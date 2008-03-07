
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
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
#include <time.h>

#include "Grid_Random_Field.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Random_Field::CGrid_Random_Field(void)
{
	CSG_Parameter	*pNode;

	//-----------------------------------------------------
	Set_Name(_TL("Random Field"));

	Set_Author	(SG_T("(c) 2005 by O.Conrad"));

	Set_Description(
		_TL("Create a grid with pseudo-random numbers as grid cell values. ")
	);

	//-----------------------------------------------------
	Parameters.Add_Grid_Output(
		NULL	, "OUTPUT"		, _TL("Random Field"),
		_TL("")
	);

	//-----------------------------------------------------
	pNode	= Parameters.Add_Node(
		NULL	, "NODE_GRID"	, _TL("Grid Properties"),
		_TL("")
	);

	Parameters.Add_Value(
		pNode	, "NX"			, _TL("Width (Cells)"),
		_TL(""),
		PARAMETER_TYPE_Int, 100, 1, true
	);

	Parameters.Add_Value(
		pNode	, "NY"			, _TL("Height (Cells)"),
		_TL(""),
		PARAMETER_TYPE_Int, 100, 1, true
	);

	Parameters.Add_Value(
		pNode	, "CELLSIZE"	, _TL("Cellsize"),
		_TL(""),
		PARAMETER_TYPE_Double, 1.0, 0.0, true
	);

	Parameters.Add_Value(
		pNode	, "XMIN"		, _TL("West"),
		_TL(""),
		PARAMETER_TYPE_Double, 0.0
	);

	Parameters.Add_Value(
		pNode	, "YMIN"		, _TL("South"),
		_TL(""),
		PARAMETER_TYPE_Double, 0.0
	);

	//-----------------------------------------------------
	Parameters.Add_Choice(
		NULL	, "METHOD"		, _TL("Method"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|"),_TL("Uniform"), _TL("Gaussian")), 
		1
	);

	pNode	= Parameters.Add_Node(
		NULL	, "NODE_UNIFORM", _TL("Uniform"),
		_TL("")
	);

	Parameters.Add_Range(
		pNode	, "RANGE"		, _TL("Range"),
		_TL(""),
		0.0, 1.0
	);

	pNode	= Parameters.Add_Node(
		NULL	, "NODE_GAUSS"	, _TL("Gaussian"),
		_TL("")
	);

	Parameters.Add_Value(
		pNode	, "MEAN"		, _TL("Arithmetic Mean"),
		_TL(""),
		PARAMETER_TYPE_Double, 0.0
	);

	Parameters.Add_Value(
		pNode	, "STDDEV"		, _TL("Standard Deviation"),
		_TL(""),
		PARAMETER_TYPE_Double, 1.0
	);
}

//---------------------------------------------------------
CGrid_Random_Field::~CGrid_Random_Field(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Random_Field::On_Execute(void)
{
	int		x, y, method;
	double	min, max, mean, stddev;
	CSG_Grid	*pGrid;

	//-----------------------------------------------------
	pGrid	= SG_Create_Grid(
		GRID_TYPE_Float,
		Parameters("NX")		->asInt(),
		Parameters("NY")		->asInt(),
		Parameters("CELLSIZE")	->asDouble(),
		Parameters("XMIN")		->asDouble(),
		Parameters("YMIN")		->asDouble()
	);

	pGrid->Set_Name(_TL("Random Field"));
	Parameters("OUTPUT")->Set_Value(pGrid);

	//-----------------------------------------------------
	method	= Parameters("METHOD")	->asInt();

	min		= Parameters("RANGE")	->asRange()->Get_LoVal();
	max		= Parameters("RANGE")	->asRange()->Get_HiVal();

	mean	= Parameters("MEAN")	->asDouble();
	stddev	= Parameters("STDDEV")	->asDouble();

	srand((unsigned)time(NULL));

	//-----------------------------------------------------
	for(y=0; y<pGrid->Get_NY() && Set_Progress(y, pGrid->Get_NY()); y++)
	{
		for(x=0; x<pGrid->Get_NX(); x++)
		{
			switch( method )
			{
			case 0:	// uniform...
				pGrid->Set_Value(x, y, Get_Random_Uniform	(min, max));
				break;

			case 1:	// uniform...
				pGrid->Set_Value(x, y, Get_Random_Gaussian	(mean, stddev));
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
// Uniform distributed pseudo-random numbers in the arbitrary range.
//
inline double CGrid_Random_Field::Get_Random_Uniform(double min, double max)
{
	return( min + (max - min) * rand() / (double)RAND_MAX );
}

//---------------------------------------------------------
// Uniform distributed pseudo-random numbers in the range from 0 to 1.
//
inline double CGrid_Random_Field::Get_Random_Uniform(void)
{
	return( 1.0 * rand() / (double)RAND_MAX );
}

//---------------------------------------------------------
// Generating Gaussian pseudo-random numbers using
// the polar form of the Box-Muller transformation.
//
// Box, G.E.P, Muller, M.E. (1958):
//   'A note on the generation of random normal deviates',
//    Annals Math. Stat, V. 29, pp. 610-611
//
// Link: http://www.taygeta.com/random/gaussian.html
//
double CGrid_Random_Field::Get_Random_Gaussian(double mean, double stddev)
{
	static bool		bCalculate	= true;
	static double	y2			= 0.0;
	double			x1, x2, w, y1;
 
	if( bCalculate )
	{
		do
		{
			x1	= 2.0 * Get_Random_Uniform() - 1.0;
			x2	= 2.0 * Get_Random_Uniform() - 1.0;

			w	= x1 * x1 + x2 * x2;
		}
		while( w >= 1.0 );

		w	= sqrt((-2.0 * log(w)) / w);

		y1	= x1 * w;
		y2	= x2 * w;

		return( mean + stddev * y1 );
	}

	return( mean + stddev * y2 );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

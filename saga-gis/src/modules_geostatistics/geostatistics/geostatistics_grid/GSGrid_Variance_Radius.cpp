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
//                  Geostatistics_Grid                   //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               GSGrid_Variance_Radius.cpp              //
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
#include "GSGrid_Variance_Radius.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGSGrid_Variance_Radius::CGSGrid_Variance_Radius(void)
{
	Set_Name		(_TL("Radius of Variance (Grid)"));

	Set_Author		(SG_T("(c) 2003 by O.Conrad"));

	Set_Description	(_TW(
		"Find the radius within which the cell values exceed the given variance criterium. "
		"This module is closely related to the representativeness calculation "
		"(variance within given search radius). "
		"For easier usage, the variance criterium is entered as standard deviation value. "
	));

	//-----------------------------------------------------
	Parameters.Add_Grid(
		NULL	, "INPUT"		, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Grid(
		NULL	, "RESULT"		, _TL("Variance Radius"),
		_TL(""),
		PARAMETER_OUTPUT
	);

	Parameters.Add_Value(
		NULL	, "VARIANCE"	, _TL("Standard Deviation"),
		_TL(""),
		PARAMETER_TYPE_Double	, 1.0
	);

	Parameters.Add_Value(
		NULL	, "RADIUS"		, _TL("Maximum Search Radius (cells)"),
		_TL(""),
		PARAMETER_TYPE_Int		, 20
	);

	Parameters.Add_Choice(
		NULL	, "OUTPUT"		, _TL("Type of Output"),
		_TL(""),

		CSG_String::Format(SG_T("%s|%s|"),
			_TL("Cells"),
			_TL("Map Units")
		), 0
	);

	//-----------------------------------------------------
	pInput		= NULL;
	pInputQ		= NULL;
	Check		= NULL;
	maxRadius	= 0;
}

//---------------------------------------------------------
CGSGrid_Variance_Radius::~CGSGrid_Variance_Radius(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGSGrid_Variance_Radius::On_Execute(void)
{
	int		x, y;

	//-----------------------------------------------------
	stopVariance	= M_SQR(Parameters("VARIANCE")->asDouble());
	maxRadius		= Parameters("RADIUS")	->asInt();
	bWriteGridsize	= Parameters("OUTPUT")	->asInt() == 0;

	pGrid			= Parameters("INPUT")	->asGrid();
	pResult			= Parameters("RESULT")	->asGrid();
	pResult->Set_Name(CSG_String::Format(SG_T("%s >= %f"), _TL("Radius with Variance"), stopVariance));

	//-----------------------------------------------------
	Initialize();

	for(y=0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			pResult->Set_Value(x, y, Get_Radius(x, y));
		}
	}

	Finalize();

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
void CGSGrid_Variance_Radius::Initialize(void)
{
	int		x, y;
	double	d;

	//-----------------------------------------------------
	pInput	= SG_Create_Grid(pGrid);
	pInputQ	= SG_Create_Grid(pGrid);

	for(y=0; y<Get_NY(); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			d	= pGrid->asDouble(x,y);
			pInput	->Set_Value(x, y, d  );
			pInputQ	->Set_Value(x, y, d*d);
		}
	}

	//-----------------------------------------------------
	// Radius Check-Matrix erstellen...
	Check	= (int **)malloc((maxRadius + 1) * sizeof(int *));

	for(y=0; y<=maxRadius; y++)
	{
		Check[y]	= (int *)malloc((maxRadius + 1) * sizeof(int));

		for(x=0; x<=maxRadius; x++)
		{
		//	Check[y][x]	= (int)sqrt(x*x + y*y);
			Check[y][x]	= (int)sqrt((x + 0.5) * (x + 0.5) + (y + 0.5) * (y + 0.5));
		}
	}
}

//---------------------------------------------------------
void CGSGrid_Variance_Radius::Finalize(void)
{
	if( pInput )
	{
		delete(pInput);
		pInput		= NULL;
	}

	if( pInputQ )
	{
		delete(pInputQ);
		pInputQ		= NULL;
	}

	if( Check )
	{
		for(int y=0; y<=maxRadius; y++)
		{
			free(Check[y]);
		}
		free(Check);
		Check		= NULL;
		maxRadius	= 0;
	}
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
double CGSGrid_Variance_Radius::Get_Radius(int xPoint, int yPoint)
{
	const double	sqrt2	= 1.0 / sqrt(2.0);

	int		x, y, dx, dy, sRadius,
			Radius		= 0,
			nValues		= 0;

	double	ArithMean, Variance,
			Sum			= 0.0,
			SumQ		= 0.0;

	do
	{
		sRadius	= (int)(sqrt2 * (double)Radius - 4.0);
		if( sRadius < 0 ) sRadius	= 0;

		//-------------------------------------------------
		for(dy=sRadius; dy<=Radius; dy++)
		{
			for(dx=sRadius; dx<=Radius; dx++)
			{
				if(Check[dy][dx] == Radius)
				{
					y	= yPoint - dy;

					if(y>=0)
					{
						x	= xPoint - dx;
						if(x>=0)
						{
							Sum			+= pInput->asDouble(x,y);
							SumQ		+= pInputQ->asDouble(x,y);
							nValues++;
						}

						x	= xPoint + dx;
						if(x<Get_NX())
						{
							Sum			+= pInput->asDouble(x,y);
							SumQ		+= pInputQ->asDouble(x,y);
							nValues++;
						}
					}

					y	= yPoint + dy;
					if(y<Get_NY())
					{
						x	= xPoint - dx;
						if(x>=0)
						{
							Sum			+= pInput->asDouble(x,y);
							SumQ		+= pInputQ->asDouble(x,y);
							nValues++;
						}

						x	= xPoint + dx;
						if(x<Get_NX())
						{
							Sum			+= pInput->asDouble(x,y);
							SumQ		+= pInputQ->asDouble(x,y);
							nValues++;
						}
					}
				}
			}
		}

		//-------------------------------------------------
		if(nValues)
		{
			ArithMean	= Sum  / nValues;
			Variance	= SumQ / nValues - ArithMean * ArithMean;

			// Andre, das ist die Formel aus deinem Buch...
			// Variance	= (SumQ - nValues * ArithMean * ArithMean) / nValues;
		}
		else
			Variance	= 0;

		Radius++;
	}
	while(Variance < stopVariance && Radius <= maxRadius);

	return( bWriteGridsize ? Radius : Radius * Get_Cellsize() );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

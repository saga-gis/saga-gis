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
//                    Grid_Volume.cpp                    //
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
#include "Grid_Volume.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
CGrid_Volume::CGrid_Volume(void)
{
	Set_Name(_TL("Grid Volume"));

	Set_Author	(SG_T("(c) 2005 by O.Conrad"));

	Set_Description(
		_TL("Calculate the volume under the grid's surface. This is mainly useful for Digital Elevation Models (DEM).")
	);

	Parameters.Add_Grid(
		NULL	, "GRID"	, _TL("Grid"),
		_TL(""),
		PARAMETER_INPUT
	);

	Parameters.Add_Choice(
		NULL	, "METHOD"	, _TL("Method"),
		_TL(""),
		CSG_String::Format(SG_T("%s|%s|%s|%s|"), 
			_TL("Count Only Above Base Level"), 
			_TL("Count Only Below Base Level"),
			_TL("Subtract Volumes Below Base Level"),
			_TL("Add Volumes Below Base Level")
		)
	);

	Parameters.Add_Value(
		NULL	, "LEVEL"	, _TL("Base Level"),
		_TL(""),
		PARAMETER_TYPE_Double, 0.0
	);
}

//---------------------------------------------------------
CGrid_Volume::~CGrid_Volume(void)
{}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool CGrid_Volume::On_Execute(void)
{
	int			x, y, Method;
	double		Level, Volume, z;
	CSG_Grid		*pGrid;
	CSG_String	s;

	//-----------------------------------------------------
	pGrid	= Parameters("GRID")	->asGrid();
	Level	= Parameters("LEVEL")	->asDouble();
	Method	= Parameters("METHOD")	->asInt();

	//-----------------------------------------------------
	for(y=0, Volume=0.0; y<Get_NY() && Set_Progress(y); y++)
	{
		for(x=0; x<Get_NX(); x++)
		{
			if( !pGrid->is_NoData(x, y) )
			{
				z	= pGrid->asDouble(x, y) - Level;

				switch( Method )
				{
				case 0:	// Count Only Above Base Level
					if( z > 0.0 )
					{
						Volume	+= z;
					}
					break;

				case 1:	// Count Only Below Base Level
					if( z < 0.0 )
					{
						Volume	-= z;
					}
					break;

				case 2:	// Subtract Volumes Below Base Level
					Volume	+= z;
					break;

				case 3:	// Add Volumes Below Base Level
					Volume	+= fabs(z);
					break;
				}
			}
		}
	}

	//-----------------------------------------------------
	Volume	*= pGrid->Get_Cellarea();

	s.Printf(_TL("Volume: %f"), Volume);

	Message_Add(s);
	Message_Dlg(s, _TL("Grid Volume"));

	//-----------------------------------------------------
	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

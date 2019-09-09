
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
//                      vigra.cpp                        //
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
#include "vigra.h"

//---------------------------------------------------------
using namespace vigra;


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
bool	Copy_RGBGrid_SAGA_to_VIGRA	(CSG_Grid &Grid, BRGBImage &Image, bool bCreate)
{
	if( bCreate )
	{
		Image.resize(Grid.Get_NX(), Grid.Get_NY());
	}

	if( Grid.Get_NX() != Image.width() || Grid.Get_NY() != Image.height() )
	{
		return( false );
	}

	for(int y=0; y<Grid.Get_NY() && SG_UI_Process_Set_Progress(y, Grid.Get_NY()); y++)
	{
		for(int x=0; x<Grid.Get_NX(); x++)
		{
			RGBValue<unsigned char>	rgb(SG_GET_R(Grid.asInt(x, y)), SG_GET_G(Grid.asInt(x, y)), SG_GET_B(Grid.asInt(x, y)));

			Image(x, y)	= rgb;
		}
	}

	SG_UI_Process_Set_Progress(0.0, 1.0);

	return( true );
}

//---------------------------------------------------------
bool	Copy_RGBGrid_VIGRA_to_SAGA	(CSG_Grid &Grid, BRGBImage &Image, bool bCreate)
{
	if( bCreate )
	{
		Grid.Create(Grid.Get_Type(), Image.width(), Image.height());
	}

	if( Grid.Get_NX() != Image.width() || Grid.Get_NY() != Image.height() )
	{
		return( false );
	}

	for(int y=0; y<Grid.Get_NY() && SG_UI_Process_Set_Progress(y, Grid.Get_NY()); y++)
	{
		for(int x=0; x<Grid.Get_NX(); x++)
		{
			RGBValue<unsigned char>	rgb	= Image(x, y);

			Grid.Set_Value(x, y, SG_GET_RGB(rgb.red(), rgb.green(), rgb.blue()));
		}
	}

	SG_UI_Process_Set_Progress(0.0, 1.0);

	return( true );
}


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------

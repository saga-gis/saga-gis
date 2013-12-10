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
//                        VIGRA                          //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                       vigra.h                         //
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
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__vigra_H
#define HEADER_INCLUDED__vigra_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
template <class VIGRA_Image>
bool	Copy_Grid_SAGA_to_VIGRA		(CSG_Grid &Grid, VIGRA_Image &Image, bool bCreate)
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
			Image(x, y)	= Grid.asDouble(x, y);
		}
	}

	SG_UI_Process_Set_Progress(0.0, 1.0);

	return( true );
}

//---------------------------------------------------------
template <class VIGRA_Image>
bool	Copy_Grid_VIGRA_to_SAGA		(CSG_Grid &Grid, VIGRA_Image &Image, bool bCreate)
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
			Grid.Set_Value(x, y, Image(x, y));
		}
	}

	SG_UI_Process_Set_Progress(0.0, 1.0);

	return( true );
}

//---------------------------------------------------------
bool	Copy_RGBGrid_SAGA_to_VIGRA	(CSG_Grid &Grid, vigra::BRGBImage &Image, bool bCreate);
bool	Copy_RGBGrid_VIGRA_to_SAGA	(CSG_Grid &Grid, vigra::BRGBImage &Image, bool bCreate);


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__vigra_H


///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library:                     //
//                     climate_tools                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               temperature_lapserates.h                //
//                                                       //
//                 Copyright (C) 2019 by                 //
//                  Dirk Nikolaus Karger                 //
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
//    e-mail:     dirk.karger@wsl.ch                     //
//                                                       //
//    contact:    D.N. Karger                            //
//                Swiss Federal Research Institute WSL   //
//                Zürcherstrasse 111                     //
//                8930 Birmensdorf                       //
//                Switzerland                            //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__temperature_lapse_rates_H
#define HEADER_INCLUDED__temperature_lapse_rates_H


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_api/saga_api.h>


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CTemperature_Lapse_Rates : public CSG_Tool_Grid
{
public:
	CTemperature_Lapse_Rates(void);

	virtual CSG_String			Get_MenuPath			(void)	{	return( _TL("Temperature") );	}


protected:

	virtual bool				On_Execute				(void);


private:

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__temperature_lapse_rates_H
#pragma once


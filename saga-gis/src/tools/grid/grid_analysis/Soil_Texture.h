/**********************************************************
 * Version $Id$
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//    System for an Automated Geo-Scientific Analysis    //
//                                                       //
//                     Tool Library                      //
//                    grid_analysis                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    Soil_Texture.h                     //
//                                                       //
//                 Copyright (C) 2007 by                 //
//                   Gianluca Massei                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
// This file is part of 'SAGA - System for an Automated  //
// Geo-Scientific Analysis'. SAGA is free software; you  //
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
//    e-mail:     g_massa@libero.it				     	 //
//                                                       //
//    contact:    Gianluca Massei                        //
//                Department of Economics and Appraisal  //
//                University of Perugia - Italy			 //
//                www.unipg.it                           //
//                                                       //
///////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__Soil_Texture_H
#define HEADER_INCLUDED__Soil_Texture_H

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSoil_Texture : public CSG_Tool_Grid
{
public:
	CSoil_Texture(void);

	virtual CSG_String		Get_MenuPath			(void)	{	return( _TL("Soil Analysis") );	}


protected:

	virtual int				On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);
	virtual int				On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool			On_Execute				(void);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CSoil_Texture_Table : public CSG_Tool
{
public:
	CSoil_Texture_Table(void);

	virtual CSG_String		Get_MenuPath			(void)	{	return( _TL("Soil Analysis") );	}


protected:

	virtual int				On_Parameter_Changed	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);
	virtual int				On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool			On_Execute				(void);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Soil_Texture_H

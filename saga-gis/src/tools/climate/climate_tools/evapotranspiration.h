
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     climate_tools                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                 evapotranspiration.h                  //
//                                                       //
//                 Copyright (C) 2011 by                 //
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
//    e-mail:     oconrad@saga-gis.de                    //
//                                                       //
//    contact:    Olaf Conrad                            //
//                Institute of Geography                 //
//                University of Hamburg                  //
//                Germany                                //
//                                                       //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__evapotranspiration_H
#define HEADER_INCLUDED__evapotranspiration_H


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#include <saga_api/saga_api.h>


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CETpot_Day_To_Hour : public CSG_Tool
{
public:
	CETpot_Day_To_Hour(void);

	virtual CSG_String	Get_MenuPath			(void)	{	return( _TL("Evapotranspiration") );	}


protected:

	virtual bool		On_Execute				(void);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CETpot_Table : public CSG_Tool
{
public:
	CETpot_Table(void);

	virtual CSG_String	Get_MenuPath			(void)	{	return( _TL("Evapotranspiration") );	}


protected:

	virtual int			On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool		On_Execute				(void);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CETpot_Grid : public CSG_Tool_Grid
{
public:
	CETpot_Grid(void);

	virtual CSG_String	Get_MenuPath			(void)	{	return( _TL("Evapotranspiration") );	}


protected:

	virtual int			On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool		On_Execute				(void);

};


///////////////////////////////////////////////////////////
//														 //
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__evapotranspiration_H

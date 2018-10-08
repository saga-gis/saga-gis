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
//                     grid_analysis                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    Fragmentation.h                    //
//                                                       //
//                 Copyright (C) 2008 by                 //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__Fragmentation_Base_H
#define HEADER_INCLUDED__Fragmentation_Base_H


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
// #define CLASS_ORIGINAL

#ifdef CLASS_ORIGINAL
	enum
	{
		CLASS_CORE			= 7,
		CLASS_INTERIOR		= 4,
		CLASS_UNDETERMINED	= 2,
		CLASS_PERFORATED	= 3,
		CLASS_EDGE			= 1,
		CLASS_TRANSITIONAL	= 6,
		CLASS_PATCH			= 5,
		CLASS_NONE			= 0
	};
#else
	enum
	{
		CLASS_CORE			= 1,
		CLASS_INTERIOR,
		CLASS_UNDETERMINED,
		CLASS_PERFORATED,
		CLASS_EDGE,
		CLASS_TRANSITIONAL,
		CLASS_PATCH,
		CLASS_NONE
	};
#endif


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CFragmentation_Base : public CSG_Tool_Grid
{
public:
	CFragmentation_Base(void);

	virtual CSG_String		Get_MenuPath		(void)	{	return( _TL("Fragmentation Analysis") );	}


protected:

	virtual bool			On_Execute			(void);

	virtual bool			Initialise			(CSG_Grid *pClasses, int Class)	{	return( true );	}
	virtual bool			Finalise			(void)							{	return( true );	}

	bool					Set_Classification	(CSG_Grid *pClasses);
	virtual bool			Get_Fragmentation	(int x, int y, double &Density, double &Connectivity)	= 0;


	int						m_Aggregation, m_Radius_iMin, m_Radius_iMax;

	double					m_Density_Min, m_Density_Interior, m_Weight, m_Radius_Min, m_Radius_Max;


	int						Get_Classification	(double Density, double Connectivity);

	void					Add_Border			(CSG_Grid *pFragmentation);

	void					Get_Statistics		(CSG_Grid *pFragmentation, CSG_Table &Statistics);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Fragmentation_Base_H

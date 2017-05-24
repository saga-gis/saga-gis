/**********************************************************
 * Version $Id: frost_change_frequency.h 1380 2012-04-26 12:02:19Z reklov_w $
 *********************************************************/

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
//                frost_change_frequency.h               //
//                                                       //
//                 Copyright (C) 2016 by                 //
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


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__frost_change_frequency_H
#define HEADER_INCLUDED__frost_change_frequency_H


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
class CFrost_Change_Frequency_Calculator
{
public:
	CFrost_Change_Frequency_Calculator(void);

	bool						Set_Temperatures		(CSG_Parameter_Grid_List *pTmin, CSG_Parameter_Grid_List *pTmax);

	bool						Get_Statistics			(int x, int y, CSG_Simple_Statistics &Dif, CSG_Simple_Statistics &Min);
	bool						Get_Statistics			(int x, int y, CSG_Simple_Statistics &Dif, CSG_Simple_Statistics &Min, CSG_Vector &Tmin, CSG_Vector &Tmax);


private:

	CSG_Parameter_Grid_List		*m_pTmin, *m_pTmax;


	bool						Get_Daily				(int x, int y, CSG_Parameter_Grid_List *pTemperatures, CSG_Vector &Daily);
	bool						Get_From_Daily			(int x, int y, CSG_Parameter_Grid_List *pTemperatures, CSG_Vector &Daily);
	bool						Get_From_Monthly		(int x, int y, CSG_Parameter_Grid_List *pTemperatures, CSG_Vector &Daily);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CFrost_Change_Frequency : public CSG_Tool_Grid
{
public:
	CFrost_Change_Frequency(void);

	virtual CSG_String			Get_MenuPath			(void)	{	return( _TL("Tools") );	}


protected:

	virtual bool				On_Execute				(void);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CFrost_Change_Frequency_Interactive : public CSG_Tool_Grid_Interactive
{
public:
	CFrost_Change_Frequency_Interactive(void);

	virtual CSG_String			Get_MenuPath			(void)	{	return( _TL("Tools") );	}


protected:

	virtual bool				On_Execute				(void);
	virtual bool				On_Execute_Position		(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode);


private:

	CFrost_Change_Frequency_Calculator	m_Calculator;

	CSG_Table					*m_pStatistics, *m_pDaily;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__frost_change_frequency_H

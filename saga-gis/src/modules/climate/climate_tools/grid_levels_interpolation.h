/**********************************************************
 * Version $Id: grid_Levels_interpolation.h 1380 2012-04-26 12:02:19Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     climate_tools                     //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//               grid_Levels_interpolation.h             //
//                                                       //
//                 Copyright (C) 2012 by                 //
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
#ifndef HEADER_INCLUDED__grid_Levels_interpolation_H
#define HEADER_INCLUDED__grid_Levels_interpolation_H

//---------------------------------------------------------
#include "MLB_Interface.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CGrid_Levels_Interpolation : public CSG_Module_Grid
{
public:
	CGrid_Levels_Interpolation(void);

//	virtual CSG_String			Get_MenuPath			(void)	{	return( _TL("Tools") );	}


protected:

	virtual int					On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	bool						Initialize				(const CSG_Rect &Extent);
	bool						Finalize				(void);

	bool						Get_Value				(double x, double y, double z, double &Value);
	bool						Get_Value				(const TSG_Point &p, double z, double &Value);


private:

	bool						m_Linear_bSorted, m_Spline_bAll;

	int							m_xSource, m_vMethod, m_Trend_Order;

	TSG_Grid_Resampling			m_hMethod;

	CSG_Table					*m_pXTable;

	CSG_Grid					*m_Coeff;

	CSG_Parameter_Grid_List		*m_pXGrids, *m_pVariables;


	double						Get_Variable			(double x, double y, int iLevel);
	bool						Get_Variable			(double x, double y, int iLevel, double &Variable);
	double						Get_Height				(double x, double y, int iLevel);
	bool						Get_Height				(double x, double y, int iLevel, double &Height);

	bool						Get_Values				(double x, double y, double z, int &iLevel, CSG_Table &Values);

	bool						Get_Linear_Sorted		(double x, double y, double z, double &Value);
	bool						Get_Linear				(double x, double y, double z, double &Value);
	bool						Get_Spline_All			(double x, double y, double z, double &Value);
	bool						Get_Spline				(double x, double y, double z, double &Value);
	bool						Get_Trend				(double x, double y, double z, double &Value);
	bool						Get_Trend_Coeff			(double x, double y, double z, double &Value);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CGrid_Levels_to_Surface : public CGrid_Levels_Interpolation
{
public:
	CGrid_Levels_to_Surface(void);


protected:

	virtual bool				On_Execute				(void);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CGrid_Levels_to_Points : public CGrid_Levels_Interpolation
{
public:
	CGrid_Levels_to_Points(void);


protected:

	virtual bool				On_Execute				(void);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__grid_Levels_interpolation_H

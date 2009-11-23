
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                     grid_spline                       //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                  Thin_Plate_Spline.h                  //
//                                                       //
//                 Copyright (C) 2006 by                 //
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
//                                                       //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#ifndef HEADER_INCLUDED__Thin_Plate_Spline_H
#define HEADER_INCLUDED__Thin_Plate_Spline_H


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
class grid_spline_EXPORT CThin_Plate_Spline
{
public:
	CThin_Plate_Spline(void);
	virtual ~CThin_Plate_Spline(void);

	bool					Destroy				(void);

	void					Add_Point			(double x, double y, double z);
	void					Add_Point			(const TSG_Point &p, double z);
	CSG_Points_Z &			Get_Points			(void)	{	return( m_Points );		}

	bool					Create				(double Regularization = 0.0, bool bSilent = true);

	bool					is_Okay				(void)	{	return( m_V.Get_N() > 0 );	}

	double					Get_Value			(double x, double y);


private:

	CSG_Points_Z			m_Points;

	CSG_Vector				m_V;


	double					_Get_hDistance		(TSG_Point_Z A, TSG_Point_Z B);
	double					_Get_Base_Funtion	(double x);
	double					_Get_Base_Funtion	(TSG_Point_Z A, double x, double y);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif	// #ifndef HEADER_INCLUDED__Thin_Plate_Spline_H

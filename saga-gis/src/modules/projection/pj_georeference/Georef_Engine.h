/**********************************************************
 * Version $Id: Georef_Engine.h 1921 2014-01-09 10:24:11Z oconrad $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                    pj_georeference                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    Georef_Engine.h                    //
//                                                       //
//                 Copyright (C) 2013 by                 //
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
//    e-mail:     oconrad@saga-gis.de                    //
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
#ifndef HEADER_INCLUDED__Georef_Engine_H
#define HEADER_INCLUDED__Georef_Engine_H


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
enum
{
	GEOREF_NotSet,
	GEOREF_Triangulation,
	GEOREF_Spline,
	GEOREF_Affine,
	GEOREF_Polynomial_1st_Order,
	GEOREF_Polynomial_2nd_Order,
	GEOREF_Polynomial_3rd_Order,
	GEOREF_Polynomial
};

//---------------------------------------------------------
#define GEOREF_METHODS_CHOICE	CSG_String::Format(SG_T("%s|%s|%s|%s|%s|%s|%s|%s|"),\
	_TL("Automatic"),\
	_TL("Triangulation"),\
	_TL("Spline"),\
	_TL("Affine"),\
	_TL("1st Order Polynomial"),\
	_TL("2nd Order Polynomial"),\
	_TL("3rd Order Polynomial"),\
	_TL("Polynomial, Order")\
)


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CGeoref_Engine
{
public:
	CGeoref_Engine(void);

	bool					Destroy					(void);

	bool					Add_Reference			(TSG_Point From, TSG_Point To);
	bool					Add_Reference			(double xFrom, double yFrom, double xTo, double yTo);

	bool					Set_Reference			(CSG_Shapes *pFrom, CSG_Shapes *pTo);
	bool					Set_Reference			(CSG_Shapes *pFrom, int xTo_Field, int yTo_Field);

	int						Get_Reference_Count		(void)	{	return( m_From.Get_Count() );	}

	bool					Get_Reference_Extent	(CSG_Rect &Extent, bool bInverse = false);

	double					Get_Reference_Residual	(int i);

	bool					Set_Scaling				(double Scaling);
	double					Get_Scaling				(void)	{	return( m_Scaling );	}

	bool					Evaluate				(int Method = GEOREF_NotSet, int Order = 1);

	bool					is_Okay					(void)	{	return( m_Method != GEOREF_NotSet );	}

	bool					Get_Converted			(TSG_Point &Point    , bool bInverse = false);
	bool					Get_Converted			(double &x, double &y, bool bInverse = false);

	CSG_String				Get_Error				(void)	{	return( m_Error );	}


private:

	int						m_Method, m_Order;

	double					m_Scaling;

	CSG_String				m_Error;

	CSG_Rect				m_rFrom, m_rTo;

	CSG_Points				m_From, m_To;

	CSG_Vector				m_Polynom_Fwd[2], m_Polynom_Inv[2];

	CSG_Thin_Plate_Spline	m_Spline_Fwd[2], m_Spline_Inv[2];

	CSG_TIN					m_TIN_Fwd, m_TIN_Inv;


	int						_Get_Reference_Minimum	(int Method, int Order);

	bool					_Set_Triangulation		(void);
	bool					_Get_Triangulation		(double &x, double &y, CSG_TIN *pTIN);

	bool					_Set_Spline				(void);
	bool					_Get_Spline				(double &x, double &y, CSG_Thin_Plate_Spline Spline[2]);

	bool					_Set_Polynomial			(CSG_Points &From, CSG_Points &To, CSG_Vector Polynom[2]);
	bool					_Get_Polynomial			(double &x, double &y, CSG_Vector Polynom[2]);
	void					_Get_Polynomial			(double x, double y, double *z);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Georef_Engine_H

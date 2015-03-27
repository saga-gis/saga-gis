/**********************************************************
 * Version $Id: crs_distance.h 2148 2014-06-10 12:59:52Z reklov_w $
 *********************************************************/

///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                    Module Library:                    //
//                   Projection_Proj4                    //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    crs_distance.h                     //
//                                                       //
//                 Copyright (C) 2015 by                 //
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
#ifndef HEADER_INCLUDED__crs_distance_H
#define HEADER_INCLUDED__crs_distance_H

//---------------------------------------------------------
#include "crs_base.h"


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CCRS_Distance_Calculator
{
public:
	CCRS_Distance_Calculator(void);
	CCRS_Distance_Calculator(const CSG_Projection &Projection, double Epsilon);
	virtual ~CCRS_Distance_Calculator(void);

	bool					Create					(const CSG_Projection &Projection, double Epsilon);

	double					Get_Orthodrome			(const TSG_Point &A, const TSG_Point &B, CSG_Shape *pLine);
	double					Get_Loxodrome			(const TSG_Point &A, const TSG_Point &B, CSG_Shape *pLine);


private:

	double					m_Epsilon;

	CSG_CRSProjector		m_Projector, m_ProjToGCS;


	double					Get_Distance			(TSG_Point A, TSG_Point B);

	void					Add_Segment				(const TSG_Point &A, const TSG_Point &B, CSG_Shape *pLine, double *Length = NULL);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CCRS_Distance_Lines : public CSG_Module
{
public:
	CCRS_Distance_Lines(void);

	virtual CSG_String		Get_MenuPath			(void)	{	return( _TL("R:Tools") );	}


protected:

	virtual bool			On_Execute				(void);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CCRS_Distance_Points : public CCRS_Base
{
public:
	CCRS_Distance_Points(void);

	virtual CSG_String		Get_MenuPath			(void)	{	return( _TL("R:Tools") );	}


protected:

	virtual bool			On_Execute				(void);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CCRS_Distance_Interactive : public CSG_Module_Interactive
{
public:
	CCRS_Distance_Interactive(void);

	virtual CSG_String		Get_MenuPath			(void)	{	return( _TL("R:Tools") );	}


protected:

	virtual bool			On_Execute				(void);

	virtual bool			On_Execute_Position		(CSG_Point ptWorld, TSG_Module_Interactive_Mode Mode);


private:

	CSG_Point				m_Down;

	CSG_Projection			m_Projection;

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__crs_distance_H

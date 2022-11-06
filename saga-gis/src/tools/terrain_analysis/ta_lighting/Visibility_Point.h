
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                      ta_lighting                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                   Visibility_Point.h                  //
//                                                       //
//                 Copyright (C) 2003 by                 //
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
#ifndef HEADER_INCLUDED__Visibility_Point_H
#define HEADER_INCLUDED__Visibility_Point_H


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
class CVisibility
{
protected:

	bool					Create					(CSG_Parameters &Parameters);

	bool					Initialize				(const CSG_Parameters &Parameters);
	bool					Finalize				(void);

	bool					Reset					(void);

	bool					Set_Visibility			(int x, int y, double Height = 0., bool bReset = true);


private:

	bool					m_bIgnoreNoData;

	int						m_Method;

	CSG_Grid				*m_pDEM, *m_pVisibility;


	bool					_Trace_Point			(int x, int y, double dx, double dy, double dz, int xOrigin, int yOrigin, double zMax);

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CVisibility_Point : public CSG_Tool_Grid_Interactive, CVisibility
{
public:
	CVisibility_Point(void);


protected:

	virtual bool			On_Execute				(void);

	virtual bool			On_Execute_Position		(CSG_Point ptWorld, TSG_Tool_Interactive_Mode Mode);


private:

	double					m_Height;

	bool					m_bMultiple, m_bNoDataOpaque;

};


///////////////////////////////////////////////////////////
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
class CVisibility_Points : public CSG_Tool_Grid, CVisibility
{
public:
	CVisibility_Points(void);


protected:

	virtual bool			On_Execute				(void);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__Visibility_Point_H


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
//                     geomorphons.h                     //
//                                                       //
//                 Copyright (C) 2019 by                 //
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
#ifndef HEADER_INCLUDED__geomorphons_H
#define HEADER_INCLUDED__geomorphons_H


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
class CGeomorphons : public CSG_Tool_Grid
{
public:
	CGeomorphons(void);

	virtual CSG_String		Get_MenuPath			(void)	{	return( _TL("A:Terrain Analysis|Terrain Classification" ));	}


protected:

	virtual int				On_Parameters_Enable	(CSG_Parameters *pParameters, CSG_Parameter *pParameter);

	virtual bool			On_Execute				(void);


private:

	const double			m_dx[8]	= { 0, 1, 1, 1, 0,-1,-1,-1 };
	const double			m_dy[8]	= { 1, 1, 0,-1,-1,-1, 0, 1 };

	int						m_Method, m_nLevels;

	double					m_Radius, m_Threshold;

	CSG_Grid_Pyramid		m_Pyramid;

	CSG_Grid				*m_pDEM;


	bool					Get_Geomorphon			(int x, int y, int &Geomorphon);

	bool					Get_Angles_Multi_Scale	(int x, int y, CSG_Vector &Max, CSG_Vector &Min);
	bool					Get_Angles_Sectoral		(int x, int y, CSG_Vector &Max, CSG_Vector &Min);
	bool					Get_Angle_Sectoral		(int x, int y, int i, double &Max, double &Min);

	bool					Set_Classification		(CSG_Grid *pGeomorphons);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__geomorphons_H

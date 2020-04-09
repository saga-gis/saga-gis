
///////////////////////////////////////////////////////////
//                                                       //
//                         SAGA                          //
//                                                       //
//      System for Automated Geoscientific Analyses      //
//                                                       //
//                     Tool Library                      //
//                     sim_air_flow                      //
//                                                       //
//-------------------------------------------------------//
//                                                       //
//                    cold_air_flow.h                    //
//                                                       //
//                 Copyright (C) 2020 by                 //
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
#ifndef HEADER_INCLUDED__cold_air_flow_H
#define HEADER_INCLUDED__cold_air_flow_H


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
class CCold_Air_Flow : public CSG_Tool_Grid
{
public:
	CCold_Air_Flow(void);


protected:

	virtual bool			On_Execute			(void);


private:

	bool					m_bEdge;

	double					m_dTime, m_Production, m_Friction, m_Delay, m_g_dT;

	CSG_Grid				*m_pDEM, *m_pProduction, *m_pFriction, *m_pAir, m_Air, *m_pVelocity, m_Velocity;

	CSG_Grids				m_dz;


	bool					Initialize			(void);
	bool					Finalize			(void);

	bool					Set_Time_Stamp		(double Time);

	double					Get_Production		(int x, int y);
	double					Get_Friction		(int x, int y);
	double					Get_Surface			(int x, int y);
	double					Get_Gradient		(int x, int y);

	bool					Get_Neighbour		(int x, int y, int i, int &ix, int &iy);

	double					Get_Velocity		(int x, int y);
	bool					Get_Velocity		(void);

	bool					Set_Air				(int x, int y);
	bool					Set_Air				(void);

};


///////////////////////////////////////////////////////////
//														 //
//														 //
//														 //
///////////////////////////////////////////////////////////

//---------------------------------------------------------
#endif // #ifndef HEADER_INCLUDED__cold_air_flow_H
